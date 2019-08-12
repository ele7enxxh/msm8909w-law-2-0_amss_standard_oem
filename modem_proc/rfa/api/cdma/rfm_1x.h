#ifndef RFM_1X_H
#define RFM_1X_H

/*!
   @file
   rfm_1x.h

   @brief
   RF Driver's 1x external interface file.

   @details
   Based on the SVDO feature, will either include the old 1x RF interface,
   or the new 1x RF interface
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_1x.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      1X 2 G QTA
10/07/14   vr      ftm iq capture changes
04/21/14   JJ      Remove rfm_1x_get_curr_rtc_cx1()
04/10/14   JJ      Remove api rfm_1x_get_fw_smem_addr()
04/09/14   JJ      Added wrap api to get current rtc time
11/11/13   JJ      Removed rfm_1x_update_fw_smem_addr() 
10/21/13   JJ      Added two funcs to set/get fw smem addr
04/16/13   APU     Changed the name of rf_1x_read_tx_filtered_power() API to 
                   rf_1x_get_tx_filtered_power_v2() 
04/14/13   APU     Added another get_filtered_tx_power API that can query 
                   different kind of filters.
10/02/12   bmg     Added filtered Rx and Tx power query APIs
08/28/12   aro     Interface for antenna tuner action time
04/26/12   aro     Converged the TxAGC State param units to dBm10 and dB10
04/26/12   aro     Added generic RFM CDMA API status return type
04/20/12   aro     Unified interface to return TxAGC State parameter
01/24/12   aro     Enumeration for invalid RF WU time
01/23/12   aro     Added new interface to query the 1x warmup time
10/20/11   bmg     Added rfm_1x_set_tx_open_loop_power() for 1x non-signaling
09/06/11   sty     Changed return type of rfm_1x_get_tx_pwr_limit()
09/02/11   aro     Added an interface to perform timed Rx sleep
07/25/11   sty     Updated return type for rfm_1x_get_rx_agc()
06/22/11   bmg     Added rfm_1x_get_btf()
06/16/11   aro     Reverted back to older revision
06/16/11   sar     Relocated private definitions.
05/21/11   aro     Deleted Rx Wakeup function
04/12/11   aro     Added Rx Wakeup prepare and execute interfaces
04/12/11   sty     Assigned value to RF_CFS_TUNE_WAIT
03/14/11   sar     Contents 03/24/09 - 01/20/11 were moved from rfm_1x_v.h which
                   was then removed in the ongoing effort to restructure rfa api.
01/20/11   aro     Added new Tx CL Mode and documentation update
01/20/11   aro     Updated TX Closed Loop Power mode Enum
01/19/11   aro     Updated Set Tx Closed Loop Power Interface
01/17/11   aro     Removed unnecessary Interface
01/15/11   aro     Changed the Tune function name
01/13/11   aro     Renamed function names
01/12/11   bmg     Updated the 1x API with link manaager parameters
01/06/11   aro     Added RxAGC Query function
12/16/10   aro     Renamed Get RxAGC function for consistency
12/10/10   aro     Added new interfaces for Tx Power Control
12/06/10   aro     [1] Interface Update
                   [2] Doxygen Update
10/16/09   bmg     Changed rfm_1x_get_cdma_tx_agc() interface to singed
                   1/10th dBm
09/10/09   sr      Defined RF_CFS_TUNE_WAIT
06/02/09   aro     Renamed Device State Enum
06/02/09   Saul    Removed rfm_trm.h.
05/20/09   sar     Updated rfm_1x_get_state() with new return type.
03/24/09   sl      Filled in functionality by calling legacy functions.
--------
12/06/10   aro     [1] Added comdef.h
                   [2] Doxygen Update
12/06/10   bmg     Initial version of new/old vector include fie.

==============================================================================*/

#include "comdef.h"
#include "sys.h" /* Needed for the tuning APIs: band and channel types. */
#include "cai.h" /* Needed for the 1x channel configuration data structure. */
#include "enc.h" /* Needed for the 1x channel configuration data structure. */
#include "rfm_device_types.h"
#include "rfm_async_types.h"
#include "rfm_device_state_types.h"
#include "lm_types.h"
#include "rfm_common.h" /* 1x clients also get the common RFM interfaces. */
#include "rfm_gain_state_types.h"
#include "rfm_cdma.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Error code when trying to tune to an invalid band for the current
chipset. */
#define RFM_1X_TUNE_ERROR__INVALID_BAND    -1

/*----------------------------------------------------------------------------*/
/*! Error code when trying to tune to an invalid channel for the current
chipset. */
#define RFM_1X_TUNE_ERROR__INVALID_CHANNEL -2

/*----------------------------------------------------------------------------*/
/*! Error code when trying to tune the radio in an invalid mode. */
#define RFM_1X_TUNE_ERROR__INVALID_MODE    -3

/*----------------------------------------------------------------------------*/
/*! Error code when the radio is in simultaneous mode and the HDR channel
is not compatabile with the requested 1x channel in the supported bandwidth */
#define RFM_1X_TUNE_ERROR__SVDO_BANDWIDTH  -4

/*----------------------------------------------------------------------------*/
/*! time to wait after Candidate Frequency Search Tunes */
#define RF_CFS_TUNE_WAIT   5

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate invalid RF warm up time */
#define RFM_1X_RF_WU_INVALID_TIME 0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*!
  @brief
  1x reverse link channels configuration composite data structure.

  @details
  This structure is used to pass the state of all the channels that can
  be active in the 1x reverse link.
*/
typedef struct
{
   boolean                       fch_on;
   /*!< Indicates if the R-FCH channel is enabled. */

   cai_radio_config_type         fch_rc;
   /*!< The R-FCH RC if the channel is enabed. */

   boolean                       dcch_on;
   /*!< Indicates if the R-DCCH channel is enabled. */

   cai_radio_config_type         dcch_rc;
   /*!< The R-DCCH RC if the channel is enabed. */

   boolean                       sch_on;
   /*!< Indicates if the R-SCH channel is enabled. */

   cai_radio_config_type         sch_rc;
   /*!< The R-SCH RC if the channel is enabed. */

   enc_sch_rate_type             sch_rate;
   /*!< The R-SCH rate if the channel is enabed. */

   boolean                       sch_to_pch_over_6dB;
   /*!< Indicates if the SCH to PCH ratio is over 6dB. */

} rfm_1x_txc_chan_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The current Tx power limit and the amount of adjustment due to
  PA backoff.

  @todo
  Verify the resolution of these data members.
*/
typedef struct
{

  uint16 tx_pwr_limit;
  /*!< Power limit in 1/12th dB resolution */

  int16  tx_pwr_limit_adj;
  /*!< Power limit adjustment in 1/12th dB resolution */

} rfm_1x_txc_pwr_limit_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration indicating the type of action to be taken on 1x Tx Closed Loop
  Power.
*/
typedef enum
{

  RFM_1X_TX_CL_NORMAL,
  /*!< Tx AGC closed loop power control will follow the reverse link power
  control commands */

  RFM_1X_TX_CL_INIT,
  /*!< Tx AGC closed loop power control input will be initialized to the
  specified value, and will immediately begin following the reverse link power 
  control commands.  */

  RFM_1X_TX_CL_FREEZE,
  /*!< Tx AGC closed loop power control input will be forced to the 
  last closed loop power, and will not track the reverse link power control 
  commands */

  RFM_1X_TX_CL_OVERRIDE
  /*!< Tx AGC closed loop power control input will be forced to the specified
  value, and will not track the reverse link power control commands  */

} rfm_1x_tx_cl_mode;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration indicating the type of action to be taken on 1x Tx Open Loop
  Power.
*/
typedef enum
{

  RFM_1X_TX_OL_NORMAL,
  /*!< Tx AGC open loop power control will follow the forward link AGC. */

  RFM_1X_TX_OL_OVERRIDE
  /*!< Tx AGC open loop power will be forced to the specified
  value, and will not track the forward link AGC. */

} rfm_1x_tx_ol_mode;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the breakdown of 1x Rx warmup time to be used by 1xL1 */
typedef struct
{

  uint32 prep_time_us;
  /*!< Estimated time in microseconds to be spent by RF to prepare to wakeup Rx 
  prior */

  uint32 exec_time_us;
  /*!< Estimated time in microseconds to be spent by RF to to execute the wakeup 
  Rx */

} rfm_1x_rx_warmup_time_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the current Tx AGC State parameter
 
  @details
  Structure to hold the current Tx AGC State parameter. This structure can be 
  expanded with new Tx AGC state parameter as per new requirement. This will 
  hold parameter for all carrier.
*/
typedef struct
{
  int16 gain_adjust;
  /*!< Tx AGC closed loop adjustment in dB10 units  */

  int16 open_loop;
  /*!< The Tx open loop for a carrier dBm10 units */

  int16 power_limit;
  /*!< Tx AGC max power limit in dBm10 units */

  int16 tx_total;
  /*!< Total 1x transmit power in dBm10 units  */

  rfm_pa_gain_state_type pa_state;
  /*!< Current PA STate */

} rfm_1x_tx_agc_state_param_type ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A data type to hold paramters relevant while doing QTA.
*/
typedef struct
{
  int dummy;
}  rfm_1x_qta_params;

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_enable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_disable
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_exec_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_timed_sleep_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 rtc_cx1_trigger,
  const boolean is_offline_qpch,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_wakeup_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_sleep_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_retune
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const sys_channel_type band_chan,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_wait_time_t
rfm_1x_enable_diversity
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
rfm_1x_disable_diversity
(
  const rfm_device_enum_type device2,
  const lm_handle_type rxlm_handle2,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
int8
rfm_1x_get_rx_agc
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint32
rfm_1x_get_mem_pool_base_addr
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_filtered_rx_power
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_tx_open_loop
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16 
rfm_1x_get_tx_closed_loop
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_tx_gain_adjust
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_tx_pwr_limit
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_tx_agc
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfm_cdma_api_status_type
rfm_1x_get_tx_agc_state_parameter
(
  const rfm_device_enum_type device,
  rfm_1x_tx_agc_state_param_type *tx_state_param
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_filtered_tx_power
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_filtered_tx_power_v2
(
  const rfm_device_enum_type device ,
  const rfm_filter_type filter 
) ;

/*----------------------------------------------------------------------------*/

boolean
rfm_1x_is_sleeping
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
sys_channel_type
rfm_1x_get_band_chan
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rfm_1x_set_pa_access_probe_config
(
  const rfm_device_enum_type device,
  const boolean is_traffic_state
);

/*----------------------------------------------------------------------------*/
void
rfm_1x_get_tx_pwr_limit_adj
(
  const rfm_device_enum_type device,
  rfm_1x_txc_chan_config_type* const chan_config,
  rfm_1x_txc_pwr_limit_type* const limit
);

/*----------------------------------------------------------------------------*/
void
rfm_1x_notify_tx_config
(
  const rfm_device_enum_type device,
  const boolean chan_config_valid,
  const rfm_1x_txc_chan_config_type* const chan_config
);

/*----------------------------------------------------------------------------*/
void 
rfm_1x_set_tx_closed_loop_power
(
  const rfm_device_enum_type device,
  const rfm_1x_tx_cl_mode tx_cl_mode,
  const int16 tx_cl_pwr
);

/*----------------------------------------------------------------------------*/
void 
rfm_1x_set_tx_open_loop_power
(
  const rfm_device_enum_type device,
  const rfm_1x_tx_ol_mode tx_ol_mode,
  const int16 tx_ol_pwr
);

/*----------------------------------------------------------------------------*/
uint16
rfm_1x_get_tx_warmup
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_get_btf
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_1x_get_rx_warmup_time 
( 
  const rfm_device_enum_type device,
  rfm_1x_rx_warmup_time_type * const warmup_time
);

/*----------------------------------------------------------------------------*/
int32
rfm_1x_antenna_tuner_action_time
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_prep_qta
(
   const rfm_device_enum_type   device  ,
   rfm_1x_qta_params*          params
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_end_qta
(
  const rfm_device_enum_type   device
);


#ifdef __cplusplus
}
#endif

#endif /* RFM_1X_H */

