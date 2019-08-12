/*! 
  @file
  rf_cdma_time_profile.c
 
  @brief
  This file defines the interfaces needed for RF Warmup (RF WU) time profiling
 
  @addtogroup RF_CDMA_COMMON_TIME_PROF
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_time_profile.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/27/13   vr      Added mode param to rf_cdma_init_wup_time
11/07/13   zhw     Split 1x/HDR wu_time logging structures
08/08/12   spa     Changed counter for histogram to uint32 since its value is 
                   compared later to curr wu time which is uint32 (warning fix)               
08/23/12   aro     Interface to initialize RF WUP profiling sequence
08/23/12   aro     Migrated to percise RF time profiler
08/23/12   aro     Doxygen Documentation update
06/11/12   spa     Added warmup time histogram as statistic to time profile 
12/14/11   sty     Changed message level to high
08/10/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rfm_internal.h"  /* for rfm_get_current_mode() call */
#include "rf_cdma_time_profile.h"
#include "rfcommon_msg.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the RF WU counters

  @details
  This API is called by the first RFM API in the RF warm up sequence. This will
  indicate that the RF Warmup time profilinf has started
 
  @param device
  RF device for which RF times need to be updated
 
  @param mode
  The current tech mode of the device.
 
  @param current_tick
  Snapshot of the time when the calling API invoked this function.
*/
void
rf_cdma_init_wup_time
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type mode,
  const rf_time_tick_type current_tick
)
{

  rf_cdma_data_status_type* dev_status_w = NULL; /* Vvalid device pointer */

  /* Get Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status(device);

  RF_MSG_2( RF_ERROR, "rf_cdma_init_wup_time: Dev %d  mode: %d",
              device, mode);

  /* check for valid pointer */
  if ( dev_status_w != NULL )
  {
    rf_cdma_warmup_data_type *rfwu_ptr = NULL;

    if ( mode == RFM_1X_MODE )
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_1x);
    }
    else if ( mode == RFM_1XEVDO_MODE ) 
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_hdr);
    }

    if ( rfwu_ptr != NULL )
    {
      if ( rfwu_ptr->init_done == FALSE ) 
      {
        /* tells that int has been done for this WU sequence */
        rfwu_ptr->init_done = TRUE; 
        rfwu_ptr->curr_wu_time = 0; /* initialized to a valid value */

        rfwu_ptr->total_sw_time = 0;

        /* Save the current tick snapshot as the starting tick */ 
        rfwu_ptr->sw_start_tick = current_tick;
        
        /* increment the counter that handles total num of RF WU sequences */
        rfwu_ptr->sample_size++; 

	RF_MSG_1( RF_ERROR, "rf_cdma_init_wup_time: tune_count:%d ",
			      rfwu_ptr->sample_size);
      } /* if ( rfwu_ptr->init_done == 0 ) */
    }
  } /* if ( dev_status_w != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_init_wup_time: Dev %d - Invalid Dev Status",
              device);
  } /* if ! ( dev_status_w != NULL ) */

} /* rf_cdma_update_wup_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the counters with the given time

  @details
  This API is called by all RFM APIs that are in the tune sequence for 1x or 
  HDR. If this API is called by the very first RFM API (rf_1x_mc_enter_mode for 
  instance), in the tune sequence, then the counters are initialized,
  else they are just updated with the time
 
  @param device
  RF device for which RF times need to be updated
 
  @param time_spent
  the time spent by the calling API. If this API is being called by the first 
  RFM API in the tune sequence, then the time_spent is just a snapshot of the
  time when the calling API invoked this function. The total_sw_time var is 
  updated with this time-snapshot
 
*/
void
rf_cdma_update_wup_time
(
  const rfm_device_enum_type device,
  const rf_time_type time_spent
)
{
  rf_cdma_data_status_type* dev_status_w = NULL; /* Vvalid device pointer */

  /* Get Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status(device);

  /* check for valid pointer */
  if ( dev_status_w != NULL )
  {
    rf_cdma_warmup_data_type *rfwu_ptr = NULL;

    if ( rf_cdma_data_get_rf_mode(device) == RFM_1X_MODE )
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_1x);
    }
    else if ( rf_cdma_data_get_rf_mode(device) == RFM_1XEVDO_MODE ) 
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_hdr);
    }

    if ( rfwu_ptr != NULL )
    {
      if ( rfwu_ptr->init_done == TRUE ) 
      {
        rfwu_ptr->curr_wu_time += time_spent;
      } /* if ( rfwu_ptr->init_done == TRUE ) */
    }

  } /* if ( dev_status_w != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_init_wup_time: Dev %d - Invalid Dev Status",
              device);
  } /* if ! ( dev_status_w != NULL ) */

} /* rf_cdma_update_wup_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Finalize the RF Warmup Time

  @details
  This function will finalize the RF warmup time. This will compute the
  current RF warm up time including the cumulative time taken by RF APIs only
  and the overall RF Warm up time in the entitr RF WU sequence. This function
  will also store the statistics of RF WU times over time.
 
  @param device
  RFM device for which the RFWU profiling is to be done/
*/
void
rf_cdma_finalize_wup_time
(
  const rfm_device_enum_type device
)
{
  /* Counter for histogram loop */
  uint32 ctr;

  /* get a valid device pointer */
  rf_cdma_data_status_type* dev_status_w = 
                                    rf_cdma_get_mutable_device_status(device);

  /* check for valid pointer */
  if (dev_status_w != NULL)
  {
    rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);
    rf_cdma_warmup_data_type *rfwu_ptr = NULL;

    if ( rf_cdma_data_get_rf_mode(device) == RFM_1X_MODE )
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_1x);
    }
    else if ( rf_cdma_data_get_rf_mode(device) == RFM_1XEVDO_MODE ) 
    {
      rfwu_ptr = &(dev_status_w->rf_wu_time_hdr);
    }
  
    if ( rfwu_ptr != NULL )
    {      
      /* check for a valid sample size -- more of a precaution, since it is
         used in the denominator later in the API */
      if (rfwu_ptr->sample_size <= 0)
      {
        RF_MSG_1 ( RF_ERROR, "rf_cdma_finalize_wup_time: sample size = %d, should" 
                   " be > 0 !!", rfwu_ptr->sample_size );
        return;
      }

      /* update the min and max RF WU times */
      if (rfwu_ptr->sample_size == 1)
      {
        rfwu_ptr->max_wu_time = rfwu_ptr->min_wu_time = rfwu_ptr->curr_wu_time;
      }
      else if (rfwu_ptr->curr_wu_time > rfwu_ptr->max_wu_time)
      {
        rfwu_ptr->max_wu_time = rfwu_ptr->curr_wu_time;
      }
      else if (rfwu_ptr->curr_wu_time < rfwu_ptr->min_wu_time)
      {
        rfwu_ptr->min_wu_time = rfwu_ptr->curr_wu_time;
      }
        
      /* update the running total - used to compute the avg RF WU time */    
      rfwu_ptr->cumulative_rf_time += rfwu_ptr->curr_wu_time;

      /* update the RF warmup time histogram based on current wu time */
      /* Populate warmup time histogram*/
      for( ctr = 0; ctr < RF_CDMA_TIME_PROFILE_NUM_HIST_BINS ; ctr++ )
      {
        /* for bins less than 20ms*/
        if( ctr < ( RF_CDMA_TIME_PROFILE_NUM_HIST_BINS - 1 ) )
        {
          if( rfwu_ptr->curr_wu_time >= ( ctr * 1000 ) &&
              rfwu_ptr->curr_wu_time < ( ( ctr + 1 )*1000 ) )
          {
            rfwu_ptr->histogram_wu_time[ctr]++;
            break; 
          }
        } /*if( ctr < 20 )*/
        /* for bin [20, inf)*/
        else 
        {
          if( rfwu_ptr->curr_wu_time >= ( ctr * 1000 ) )
          {
            rfwu_ptr->histogram_wu_time[ctr]++;
            break; 
          }
        }/*if!( ctr < 20 )*/
      }/*for( ctr = 0; ctr < 21 ; ctr++ )*/

      /* Get Total Software time */
      rfwu_ptr->total_sw_time = rf_time_get_elapsed( rfwu_ptr->sw_start_tick, 
                                                     RF_USEC );

      /* each tech needs RF WU time to be displayed in a specific format */
      if ( curr_mode == RFM_1XEVDO_MODE )
      {
        /* Report RF WU times */
        RF_MSG_4( RF_HIGH,"EVDO: RF WU[%d](us) RF_WU_Time=%d RF_WU_Avg=%d cnt=%d",
                  device, rfwu_ptr->curr_wu_time, 
                  ( rfwu_ptr->cumulative_rf_time / rfwu_ptr->sample_size ),
                    rfwu_ptr->sample_size );
        
        
        RF_MSG_3( RF_MED, "EVDO_RFWU Stats: Total_SW_Time=%d Min=%d Max=%d (us)",
                  rfwu_ptr->total_sw_time, rfwu_ptr->min_wu_time, 
                  rfwu_ptr->max_wu_time );
      }
      else if ( curr_mode == RFM_1X_MODE )
      {
        RF_MSG_4( RF_HIGH, "RF WU[%d](us) %d avg=%d cnt=%d", 
                  device, rfwu_ptr->curr_wu_time,
                  (rfwu_ptr->cumulative_rf_time / rfwu_ptr->sample_size),
                  rfwu_ptr->sample_size );
        
        RF_MSG_3( RF_MED, "1x_RFWU Stats: Total_SW_time=%d, Min=%d, Max=%d (us)", 
                  rfwu_ptr->total_sw_time, rfwu_ptr->min_wu_time, 
                  rfwu_ptr->max_wu_time ); 
      }
      else
      {
        RF_MSG_2( RF_ERROR, "rf_cdma_finalize_wup_time: Invalid Mode (%d) for "
                  "device (%d) !!!", curr_mode, device);
      }

      rfwu_ptr->total_sw_time = 0; /* update this for the next tune sequence */
      rfwu_ptr->init_done = FALSE; /* update this for the next tune sequence */

    }/* if ( rfwu_ptr != NULL ) */
  } /* if (dev_status_w != NULL) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_finalize_wup_time: Unable to get pointer to "
                      "device status" );
  }

} /* rf_cdma_finalize_wup_time */
#endif /* FEATURE_CDMA1X */
/*! @} */

