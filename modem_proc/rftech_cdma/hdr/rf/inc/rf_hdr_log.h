#ifndef RF_HDR_LOG_H
#define RF_HDR_LOG_H

/*!
   @file
   rf_hdr_log.h

   @brief
   This file contains structures for RF log sub-packets specific to CDMA.
 
   @addtogroup RF_LOG_DEFN_CDMA
   @{
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/05/12   spa     Removed log code definition, now defined in log_codes.h 
08/08/12   spa     Updated log packet code to 0x15BF  
08/30/12   bmg     Added sample logging
08/29/12   cri     Warmup log packet moved. Removing RF defintion. 
08/28/12   nrk     Updated log packet code to 0x15BF  
07/17/12   spa     Updated sub packet id to 3 and sub packet version to 2
07/17/12   spa     Changed device type to uint8 
07/17/12   spa     Added #define for expected warmup time (15ms) 
06/11/12   spa     Added warmup time histogram to log packet 
04/19/12   spa     Added  RF_HDR_WU_LOG_NUM_MAX_CARRIERS as #define
04/18/12   spa     Added lna gain word and enhanced wu time to warmup subpacket 
                   and added rf_hdr_log_wakeup_rx prototype 
04/09/12   spa     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_log.h"
#include "rf_cdma_log.h"
#include "log.h"
#include "rf_hdr_mdsp.h"
#include "rfm_hdr.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h"

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
  packet version, the number of subpackets, the sub packet ID ,the sub packet 
  version and number of bins in histogram*/ 
#define RF_HDR_WU_LOG_PKT_VER      1 
#define RF_HDR_WU_LOG_NUM_SUB_PKTS 1
#define RF_HDR_WU_LOG_SUB_PKT_ID   3
#define RF_HDR_WU_LOG_SUB_PKT_VER  2
#define RF_HDR_WU_LOG_NUM_HIST_BINS 21
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of carriers.

  @details
  This is the maximum number of carriers, used to define the number of 
  elements in the 'rx_agc' array field*/

#define RF_HDR_WU_LOG_NUM_MAX_CARRIERS 4

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Expected warm-up time.

  @details
  This is the expected warmup time for HDR, this is being hard-coded to 15ms 
  right now since the value cannot be determined the same way as its being 
  obtained in 1x i.e. from the RFC, but in the case of HDR this value is 
  dynamically calculated, therefore this define will be used to populate the
  expected warmup time for HDR for the time being.*/

#define RF_HDR_WU_LOG_EXPECTED_WU_TIME 15000

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constants defined for packet and subpacket headers.

  @details
  These are constant values which are required for the RF Warmup Log packet 
  header and the RF Warmup Log sub-packet header these are (in order) the 
  packet version, the number of subpackets, the sub packet ID and the sub packet 
  version*/ 
/*!
  @brief
  Structure for HDR RF warmup time sub-packet.

  @details
  This structure is for the HDR RF warmup time sub-packet, contains 
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

  /*! Expected warmup time (in microseconds)*/ 
  uint32 expected_wu_time;

  /*! Histogram for warm-up times, each bin is in format [a,b) with range from 
  min = 0ms to max = inf, each bin is for 1 milisecond*/
  uint32 histogram_wu_time[RF_HDR_WU_LOG_NUM_HIST_BINS];

  /*! Number of wake ups */ 
  uint32 wu_count;

  /*! LNA state */ 
  uint8 lna_state;

  /*! LNA gain word */ 
  uint8 lna_gain;

  /*! Rx AGC */ 
  int16 rx_agc[RF_HDR_WU_LOG_NUM_MAX_CARRIERS];

} rf_hdr_log_wu_time_sub_pkt_type;

/*! @} */

/*============================================================================*/
/*!
  @name HDR Log Packets

  @brief
  Definitions for HDR log packet
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
    
  /*! HDR RF log packet header, this contains information about the version of 
RF log packet and the number of sub packets */ 
  rfcommon_log_rf_header_type rf_header;

  /*! HDR RF warmup sub-packet, this contains all the stats related to RF warmup 
  time */ 
  rf_hdr_log_wu_time_sub_pkt_type wu_time_sub_pkt;

} rf_hdr_log_rf_warmup_log_pkt;

/*! @} */

/*----------------------------------------------------------------------------*/
void
rf_hdr_log_wakeup_rx
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_log_samples
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RF_HDR_LOG_H */
