#ifndef RF_CDMA_TIME_PROFILE_H
#define RF_CDMA_TIME_PROFILE_H

/*!
  @file
  rf_cdma_time_profile.h

  @details
  This file exports the definitions and declartions needed for profiling RF
  warmup times
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_time_profile.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/27/13   vr      Added mode param to rf_cdma_init_wup_time
03/05/13   aro     Fixed rf_cdma_update_wup_time interface
08/23/12   aro     Interface to initialize RF WUP profiling sequence
08/23/12   aro     Migrated to percise RF time profiler
08/23/12   aro     Doxygen Documentation update
06/11/12   spa     Added warmup time histogram as statistic to time profile 
08/10/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Number of bins in warmup time histogram */
#define RF_CDMA_TIME_PROFILE_NUM_HIST_BINS 21

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A collection of warmup time statistics.
*/
typedef struct
{
  /*! flag that tells if the counters were initialized for the current WU
      sequence */
  boolean init_done;

  /*! holds the current RF WU time */
  rf_time_type curr_wu_time;  

  /*! holds the running total of WU times since power-up */
  rf_time_type cumulative_rf_time; 

  /*! holds the min. of RF WU time */
  rf_time_type min_wu_time;   

  /*! holds the max. of RF WU time */
  rf_time_type max_wu_time;   

  /*! Histogram for warm-up times, each bin is in format [a,b) with range from 
  min = 0ms to max = inf, each bin is for 1 milisecond*/
  uint32 histogram_wu_time[RF_CDMA_TIME_PROFILE_NUM_HIST_BINS];

  /*! number of time RF was instructed to go thru the tune routines */
  uint32 sample_size;  
  
  /*! Variable to store RF time tick at the start of RF warmup. */
  rf_time_tick_type sw_start_tick; 

  /*! holds the time that the SOFTWARE took to execute the RFM APIs - this 
    would include time taken by RF to execute the RFM APIs, and also the
    time taken by OS during the tune sequence. Typically, the total_sw_time and 
    curr_wu_time for any given tune sequence should be very close (the  
    difference should be in the order of 10s of microsecs). A large difference 
    in the two variables would indicate that RF is being starved of cycles.
  */ 
  rf_time_type total_sw_time; 

} rf_cdma_warmup_data_type;

/*----------------------------------------------------------------------------*/
void
rf_cdma_init_wup_time
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type mode,
  const rf_time_tick_type current_tick
);

/*----------------------------------------------------------------------------*/

void
rf_cdma_update_wup_time
(
  const rfm_device_enum_type device,
  const rf_time_type time_spent
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_finalize_wup_time
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_TIME_PROFILE_H */

