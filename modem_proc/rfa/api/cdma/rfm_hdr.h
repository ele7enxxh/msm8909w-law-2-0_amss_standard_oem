#ifndef RFM_HDR_H
#define RFM_HDR_H

/*!
   @file
   rfm_hdr.h

   @brief
   RF Driver's HDR external interface file.

   @details
   Based on the SVDO feature, will either include the old HDR RF interface,
   or the new HDR RF interface
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_hdr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/07/14   vr      ftm iq capture changes
05/02/14   spa     Moved API rfm_hdr_query_iq_sample_line_offset to rfm_hdr_ftm
01/15/14   JJ      Added api tp get mempool addr from FW shared memory for MCDO
11/11/13   JJ      Removed two funcs to set/get fw smem addr
10/21/13   JJ      Added two funcs to set/get fw smem addr
05/13/13   sty     Added prototype for rfm_hdr_update_rev_link_carrier_set
04/16/13   APU     Changed the name of rf_hdr_read_tx_filtered_power() API to 
                   rf_hdr_get_tx_filtered_power_v2() 
04/14/13   APU     Added another get_filtered_tx_power API that can query 
                   different kind of filters.
10/02/12   bmg     Added filtered Rx and Tx power query APIs
08/28/12   aro     Interface for antenna tuner action time
07/26/12   APU     Added Enhanced PA backoff support.
04/26/12   aro     Added generic RFM CDMA API status return type
04/21/12   av      Fix compile error
04/20/12   aro     Unified interface to return TxAGC State parameter
11/11/11   Saul    Thermal Backoff Support.
06/27/11   sar     Moved definition of rferrno_enum_type to this file from rf.h.
06/16/11   aro     Reverted back to previous revision
06/14/11   sty     Added rfm_cdma_agc_types.h - needed by RFM APIs that use 
                   rfm_hdr_agc_format_type
05/05/11   sty     Deprecated rfm_hdr_wakeup_rx()
04/11/11   aro     Added new Wakeup Rx Prepare/Execute Interfaces
04/04/11   aro     Added interface to query HDR Sleep state
03/14/11   sar     Moved definitions from rfm_hdr_compat.h.
03/14/11   sar     Moved contents from rfm_hdr_sv.h 03/24/09 - 2/13/11. rfm_hdr_sv.h
                   will be removed after the client SUs are updated.
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
01/23/11   aro     Renamed carrier type name
01/19/11   aro     Updated the HDR API with link manaager parameters
12/20/10   aro     Moved typedefs to smaller header files
12/14/10   aro     Removed HDR Get State function
12/06/10   aro     [1] Interface Update
                   [2] Doxygen Update
12/06/10   aro     [1] Added comdef.h
                   [2] Doxygen Update
12/06/10   bmg     Initial version of new/old vector include fie.

==============================================================================*/

#include "comdef.h"
#include "sys.h" /* Needed for the tuning APIs: band and channel types. */
#include "rfm_device_types.h"
#include "rfm_gain_state_types.h"
#include "rfm_hdr_agc_types.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_async_types.h"
#include "rfm_common.h" /* HDR clients also get the common RFM interfaces. */
#include "rfm_cdma_agc_types.h"
#include "rfm_gain_state_types.h"
#include "lm_types.h"
#include "rfm_cdma.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code when trying to tune to an invalid band for the current
  chipset.
*/
#define RFM_HDR_TUNE_ERROR__INVALID_BAND     -1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code when trying to tune to an invalid channel for the current
  chipset.
*/

/*----------------------------------------------------------------------------*/
#define RFM_HDR_TUNE_ERROR__INVALID_CHANNEL  -2
/*!
  @brief
  Error code when trying to tune the radio in an invalid mode.
*/
#define RFM_HDR_TUNE_ERROR__INVALID_MODE     -3

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code when the radio is in simultaneous mode and the 1x channel
  is not compatabile with the requested HDR channel in the supported
  bandwidth.
*/
#define RFM_HDR_TUNE_ERROR__SVDO_BANDWIDTH   -4

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code when trying to tune multiple channels that are not all
  in the same band.
*/
#define RFM_HDR_TUNE_ERROR__INVALID_BANDS    -5

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code when trying to tune multiple channels that are outside
  the supported bandwidth of the current chipset.
*/
#define RFM_HDR_TUNE_ERROR__INVALID_CHANNELS -6

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rate passed to rfm_hdr_notify_data_rate() when operating in RevA
  or higher modes.
*/
#define RFM_HDR_REVA_RATE_OFFSET  6

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The number of entries in the HDR reverse link rates power backoff
  structure.

  @details
  The reverse link rate power backoff is defined for the 12 reverse
  link packet sizes available in DOrA.
*/
#define MAX_HDR_REVA_RL_RATES 12

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The number of distinct transmission modes supported by HDR power backoff
  module.

  @details
  Two modes are supported
  * High Capacity Mode
  * Low Latency Mode
*/
#define MAX_HDR_REVA_RL_TRANSMISSION_MODES 2

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unit multiplier for power backoffs in the reverse link power
  backoff structure.

  @details
  Backoffs in the structure are stored in dB multiplied by this constant.
*/
#define RF_HDR_DSP_TX_AGC_UNITS 640

/*----------------------------------------------------------------------------*/

typedef enum {
  RF_SUCCESS = 0,
  RF_FAILURE,
  RF_OUT_OF_RANGE,
  RF_CAL_INVALID,
  RF_HW_ERROR
} rferrno_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A data type to control HDR Thermal Backoff State.
*/
typedef enum
{
  /*! To disable Thermal Backoff */
  RFM_HDR_THERMAL_STATE_DISABLE,
  /*! To enable Thermal Backoff */
  RFM_HDR_THERMAL_STATE_ENABLE
} rfm_hdr_thermal_state_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the current carrier based Tx Power parameter */
typedef struct
{
  int16 tx_adjust;
  /*!< The Tx closed loop adjust for a carrier in dB256 units on success */

  int16 open_loop;
  /*!<  The Tx open loop for a carrier in dB256 on success */

  int16 pilot;
  /*!< The Tx pilot power for a carrier in db256 on success */

  int16 total;
  /*!< The total Tx power for a carrier in db256 units on success */

} rfm_hdr_tx_carrier_power_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the current Tx AGC State parameter
 
  @details
  Structure to hold the current Tx AGC State parameter. This structure can be 
  expanded with new Tx AGC state parameter as per new requirement. This will 
  hold parameter for all carrier. The units of all Tx power will be dB256.
*/ 
typedef struct
{
  rfm_hdr_tx_carrier_power_type carrier_power[RFM_CDMA_CARRIER_NUM];
  /*!< Structure containing various Tx power parameter  */

  int16 total_power;
  /*!< The total cumulative Tx power in db256 units on success */

  rfm_pa_gain_state_type pa_state;
  /*!< Current PA STate */

} rfm_hdr_tx_agc_state_param_type ;
/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure to hold funnelling parameters for L1
 
  @details
  Each carrier index holds its indivual funnel bias as retrieved
  from the NV  
*/ 
#define RFM_HDR_INVALID_CARRIER_ID  0xF
 
typedef struct
{
  uint8   carrier_id ; 
  /*!< Carrier id.  Valid values: 0, 1, 2 and 15 */
 
  uint32 funnel_bias ;
  /*!< Funnel bias in Q21.10 dB units */
 
} rfm_hdr_per_carrier_funneling_bias_type ;


boolean 
rfm_hdr_get_funneling_metrics
(
  rfm_device_enum_type device ,
  /*!< RFM device whose funneling metrics are needed. */

  rfm_hdr_per_carrier_funneling_bias_type* funnel_bias_array_ptr
  /*!<  Pointer to an array of size 3 of typerfm_hdr_per_carrier_funneling_bias_type.
     The array will be populated with carrier_id and funnel_bias for each carrier.
     In the event of a hole in carrier assignment, the carrier_id for the entry corresponding
     to the hole would be set toRFM_HDR_INVALID_CARRIER_ID.  */
) ;

/*----------------------------------------------------------------------------*/
/*!

  @brief
  A data type to hold max power backoffs for all HDR reverse link rates
  in high-capacity and low latency modes.

  @details
  This is an internal data structure of the RF driver that was improperly
  exposed to HDR clients for the rf_hdr_reva_notify_tx_pwr_backoff() 
  function.  HDR currently does not use this data structure as the
  code that returns it is disabled via featurization, but internal
  HDR data structures now rely on the type to compile correctly.
*/
typedef struct 
{
  /*! Minimum battery voltage at which this set of backoffs applies, in
      millivolts. */
  uint16 vbatt_level_mv; 

  /*! Array of power backoff values in RF_HDR_DSP_TX_AGC_UNITS. */
  uint16 power_backoff_db[MAX_HDR_REVA_RL_TRANSMISSION_MODES][MAX_HDR_REVA_RL_RATES];
} rf_tx_hdr_reva_power_backoff_type;


/*----------------------------------------------------------------------------*/
/*!

  @brief
  A data type to hold paramters relevant while doing QTA.
*/
typedef struct 
{
  int dummy ;
}  rfm_hdr_qta_params ;

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type * const band_chan_array,
  const int32 * const carrier_index,
  const rfm_hdr_agc_acq_type* const agc_mode,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_retune
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type * const band_chan_array,
  const int32 * const carrier_index ,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_enable_diversity
(
  const rfm_device_enum_type device1,
  const lm_handle_type rxlm_handle1,
  const rfm_device_enum_type device2,
  const lm_handle_type rxlm_handle2,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_disable_diversity
(
  const rfm_device_enum_type device2,
  const lm_handle_type rxlm_handle2,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_hdr_update_rev_link_carrier_set
(
  const rfm_device_enum_type device,
  const sys_channel_type * const band_chan_array,
  const uint8 num_band_chan                  
);

/*----------------------------------------------------------------------------*/
rfm_lna_gain_state_type
rfm_hdr_get_lna_gain_state
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_rx_agc
(
  const rfm_device_enum_type device,
  const rfm_cdma_carrier_type mc_id,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_capture_agc
(
  const rfm_device_enum_type device,
  rfm_hdr_agc_info_struct_type* const agc_info
);

/*----------------------------------------------------------------------------*/
uint32
rfm_hdr_get_mem_pool_base_addr
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_filtered_rx_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_get_turnaround_info
(
  const rfm_device_enum_type device,
  int16* const turnaround_db,
  int16* const nominal_pwr_db
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_tx_adjust
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_tx_open_loop
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_tx_pilot_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_tx_total_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type mc_id,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
rfm_cdma_api_status_type
rfm_hdr_get_tx_agc_state_parameter
(
  const rfm_device_enum_type device,
  rfm_hdr_tx_agc_state_param_type *tx_state_param
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_filtered_tx_power
(
  const rfm_device_enum_type device,
  const rfm_hdr_carrier_id_type carrier,
  const rfm_hdr_agc_format_type format
);

/*----------------------------------------------------------------------------*/
int16
rfm_hdr_get_filtered_tx_power_v2
(
  const rfm_device_enum_type device ,
  const rfm_filter_type filter 
) ;

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_is_sleeping
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_notify_data_rate
(
  const rfm_device_enum_type device,
  const uint8 rate
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_set_pa_access_probe_config
(
  const rfm_device_enum_type device,
  const boolean is_traffic_state
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_set_nominal_pwr
(
  const rfm_device_enum_type device,
  const int16 nominal_pwr
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_set_tx_power_adjust
(
  const rfm_device_enum_type device,
  const int16 power_dbx2
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_set_thermal_state
(
  const rfm_device_enum_type device,
  rfm_hdr_thermal_state_type thermal_backoff_state
);


/*----------------------------------------------------------------------------*/
int32
rfm_hdr_antenna_tuner_action_time
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_hdr_prep_qta 
( 
  const rfm_device_enum_type   dev    , 
  rfm_hdr_qta_params*          params  
) ;


/*----------------------------------------------------------------------------*/
boolean 
rfm_hdr_end_qta 
( 
  const rfm_device_enum_type   dev
) ;


#ifdef __cplusplus
}
#endif

#endif /* RFM_HDR_H */

