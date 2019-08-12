#ifndef RF_1x_LOG_H
#define RF_1x_LOG_H

/*!
   @file
   rf_1x_log.h

   @brief
   This file contains structures for RF log sub-packets specific to CDMA.
 
   @addtogroup RF_LOG_DEFN_CDMA
   @{
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/30/12   bmg     Added sample logging
07/17/12   spa     Updated sub packet id to 3 and sub packet version to 1
07/17/12   spa     Changed device type to uint8 
06/11/12   spa     Added warmup time histogram to log packet 
04/18/12   spa     Added rf_1x_log_wakeup_rx prototype and required headers
04/09/12   spa     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_log.h"
#include "rf_cdma_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constants defined for packet and subpacket headers.

  @details
  These are constant values which are required for the RF Warmup Log packet 
  header and the RF Warmup Log sub-packet header these are (in order) the 
  packet version, the number of subpackets, the sub packet ID and the sub packet 
  version and number of bins in histogram */ 
#define RF_1X_WU_LOG_PKT_VER      1 
#define RF_1X_WU_LOG_NUM_SUB_PKTS 1
#define RF_1X_WU_LOG_SUB_PKT_ID   3
#define RF_1X_WU_LOG_SUB_PKT_VER  1
#define RF_1X_WU_LOG_NUM_HIST_BINS 21

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for 1x RF warmup time sub-packet.

  @details
  This structure is for the 1x RF warmup time sub-packet, contains 
  all the statistics associated with RF warmup time like (minimum WU time, 
  maximum WU time, avergae WU time, number of warmups, last warmup time and 
  expected warm up time ), RxAGC state, LNA state and gain */
typedef PACK(struct)
{
  /*! Sub-packet header*/ 
  rf_cdma_log_sub_pkt_header_type header;

  /*! The RF device */ 
  uint8 device;

  /*! current warmup time (in microseconds) */ 
  uint32 curr_wu_time;

  /*! minimum warmup time (in microseconds) */ 
  uint32 min_wu_time;

  /*! maximum warmup time (in microseconds)*/ 
  uint32 max_wu_time;

  /*! Running average of the warmup time (in microseconds)*/ 
  uint32 avg_wu_time;

  /*! Expected warmup time (in microseconds) */ 
  uint32 expected_wu_time;

  /*! Histogram for warm-up times, each bin is in format [a,b) with range from 
  min = 0ms to max = inf, each bin is for 1 milisecond*/
  uint32 histogram_wu_time[RF_1X_WU_LOG_NUM_HIST_BINS];

  /*! Number of wake ups */ 
  uint32 wu_count;

  /*! LNA state */ 
  uint8 lna_state;

  /*! LNA gain word */ 
  uint8 lna_gain;

  /*! Rx AGC */ 
  int16 rx_agc;

} rf_1x_log_wu_time_sub_pkt_type;

/*! @} */

/*============================================================================*/
/*!
  @name 1x Log Packets

  @brief
  Definitions for 1x log packet
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF warmup time log packet structure.

  @details
  This structure is the RF warmup time log packet, contains all the packet 
  headers and the sub-packets
*/
typedef PACK(struct)
{
    
  /*! 1x RF log packet header, this contains information about the version of RF 
  log packet and the number of sub packets */ 
  rfcommon_log_rf_header_type rf_header;

  /*! 1x RF warmup sub-packet, this contains all the stats related to RF warmup 
  time */ 
  rf_1x_log_wu_time_sub_pkt_type wu_time_sub_pkt;

} rf_1x_log_rf_warmup_log_pkt;

/*! @} */

/*----------------------------------------------------------------------------*/
void
rf_1x_log_wakeup_rx
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_1x_log_samples
(
  rfm_device_enum_type device
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RF_1X_LOG_H */
