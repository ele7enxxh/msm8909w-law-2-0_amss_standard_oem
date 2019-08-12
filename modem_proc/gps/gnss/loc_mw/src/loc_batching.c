/*============================================================================
 @file loc_batching.c

 loc MW Batching module implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware Batching module implementation.

 Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
 
 Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_batching.c#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 04/13/16   yh      loc_batching_array  NULL pointer check
 09/22/15   sjk     Fixed a bug in fixId transalation
 06/11/15   sjk     LB 2.0 Integration
 07/25/13   bnk     Initial version

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "time_svc.h"
#include "qw.h"

#include "loc_api_2.h"
#include "aon_api.h"
#include "mgp_api.h"
#include "loc_batching.h"
#include "location_service_v02.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "gnss_calendar_util.h"
#include "loc_client.h"
#include "loc_aon.h"
#include "location_common.h"

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to push the position report to LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report 


@retval    TRUE           adding position report to LIFO success
@retval    FALSE          adding position report to LIFO fail
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_push_to_lifo(
   loc_client_info_s_type           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
);


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to retrieve the position report from LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   number_of_reads       Number of reports to retrieve
@param[out]   pReadFromBatchInd    Indication to store the reports


@retval    int32           number of reports actually retrieved from buffer
*//* ------------------------------------------------------------------------*/
static int32 loc_batching_pull_from_lifo
(
  loc_client_info_s_type         *const client_ptr,
  int32                          number_of_reads,
  qmiLocReadFromBatchIndMsgT_v02 *pReadFromBatchInd
);

static boolean loc_batching_sufficient_time_elapsed_to_batch_fix
(
   loc_client_info_s_type           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
)
{
   uint64 curr_time_utc       = 0;
   uint32 modified_min_interval = 0;

   if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL))
   {
      LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
      return TRUE;
   }
  
   /* round off timestamp to nearest second */
   curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;

   
   /* If the time gap between the current fix and the last fix is > minInterval, 
    * keep both fixes */
   if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc) 
        >= batchCfg->minInterval)
   {
      LOC_MSG_MED("Time bet new & last fix: (%d) > min interval == %d. Keep both", 
          (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batchCfg->minInterval, 0);
      return TRUE;
   }
   /* Time Gap between current fix and the last fix is < minInterval. */

   /* If the time gap between the current fix and the last_last fix is 
    * >= MIN_INTERVAL_GRACE_FACTOR * minInterval, keep both fixes. If we 
    * overwrite the last fix, our time gap will increase to > minInterval.
    *
    * We will have an opportunity to overwrite the current fix if we get a 
    * new fix soon in < minInterval time */
#define MIN_INTERVAL_GRACE_FACTOR            (1.10f)
#define MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD  (6000)
#define LARGE_BATCHING_INTERVAL              (5*60*1000)
#define SMALL_BATCHING_INTERVAL              (30*1000)
   if(batchCfg->minInterval >= LARGE_BATCHING_INTERVAL)
   {
     modified_min_interval = batchCfg->minInterval + 30*1000;
   }
   else if(batchCfg->minInterval >= SMALL_BATCHING_INTERVAL)
   {
     modified_min_interval = MIN_INTERVAL_GRACE_FACTOR*batchCfg->minInterval;
   }
   else
   {
     modified_min_interval = batchCfg->minInterval + 3*1000;
   }

   if((curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc) 
        >= modified_min_interval)
   {
     LOC_MSG_MED("Time bet new & last_last fix: (%d) > min interval == %d. Keep both",
         (curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc), 
         batchCfg->minInterval, 0);	    
      return TRUE;
   }

   /* Time Gap between current fix and the last last fix is 
    *      < modified_min_interval, but 
    *      > minInterval
    * AND Time Gap between current fix and the last fix is 
    *      < MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD seconds
    * We can afford to keep just one of the current and the last fixes.  */
   
   if(((curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc) 
         >= batchCfg->minInterval))
   {
     if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc) 
         < MIN_INTERVAL_FIX_TIME_GAP_THRESHOLD)
     {
       LOC_MSG_MED("Time bet new & last fix: (%d) > min interval == %d. Keep One",
           (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
           batchCfg->minInterval, 0);	     
       return FALSE;
     }
     else
     {
       LOC_MSG_MED("Time bet new & last_last fix: (%d) > min interval == %d. Keep both",
           (curr_time_utc - client_ptr->batching_info.last_last_batched_fix_utc), 
           batchCfg->minInterval, 0);
       return TRUE;       
     }
   }

   LOC_MSG_MED("UTC NewTime %d LastFix %d LastLastFix %d",
         curr_time_utc, client_ptr->batching_info.last_batched_fix_utc, 
         client_ptr->batching_info.last_last_batched_fix_utc);
   return FALSE;
}

static void loc_batching_select_best_fixes
(
   loc_client_info_s_type           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg,
   boolean  *ignoreNewFix,
   boolean  *overWriteOldFix
)
{
  int32  last_fix_index  = 0;
  qmiLocBatchedReportStructT_v02 *last_position = NULL;
  uint32  distance_bet_positions = 0;
  uint32  hepe_sum = 0;
  uint64 curr_time_utc   = 0;

  if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL) ||
     (ignoreNewFix == NULL) || (overWriteOldFix == NULL))
  {
     LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
     return;
  }

  if( NULL == client_ptr->batching_info.loc_batching_array )
  {
     LOC_MSG_ERROR("Null loc_batching_array!", 0, 0, 0);
     return;
  }
  /* Default option is to keep both fixes and neither throw away a new fix, 
   * nor overwrite an old fix */
  *ignoreNewFix    = FALSE;
  *overWriteOldFix = FALSE;
  
  /* Always batch the first two fixes generated for a batching req. When we get
   * the third fix, we will decide whether to keep the second or not */
  if(client_ptr->batching_info.num_fixes_batched_lifetime < 2)
  {
    LOC_MSG_MED("Batched only %d fixes over lifetime! Keep new fix", 
                 client_ptr->batching_info.num_fixes_batched_lifetime, 0, 0);
    return;
  }
  /* Always batch the first fix received for an empty batch. If we get any
   * subsequent fixes, we will compare the first batched fix with the later
   * fixes to store the 'better' fix, is possible */
  if(client_ptr->batching_info.total_occupied == 0)
  {
    LOC_MSG_MED("Keep first fix for this batch!", 0, 0, 0);
    return;
  }
  

  if(loc_batching_sufficient_time_elapsed_to_batch_fix(
                 client_ptr, batching_element, batchCfg) == TRUE)
  {
     LOC_MSG_MED("Sufficient Time Elapsed to keep new fix!", 0, 0, 0);
     return;
  }
  
  /**************************************************************************** 
   * There is at least one fix in our batching buffer and at least two fixes  *
   * generated over the lifetime of batching req! Further, this new fix is    *
   * closer to the last fix than the minInterval from user. We can do some    *
   * comparisons to decide which is the better fix to keep.                   *
   * We will use the time of the latest fix, the two fixes prior to it, and   *
   * the 'quality' of the fixes                                               *
   ****************************************************************************/
  
  LOC_MSG_MED("Batch all fixes == %d", 
               (batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES), 0, 0);

  if((batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES) == 0x0)
  {
     /* Time Gap between current fix and the last last fix is < minInterval.
        Assume Fix 1 --------  Fix2 -------- Fix 3.
        We will keep only one of Fix2 and Fix3 as the Time difference between 
        Fix 1 and Fix 3 is < minInterval for batching cfg. We will keep the 
        better fix between Fix2 and Fix3. */ 
      
     last_fix_index  = client_ptr->batching_info.write_index - 1;      
     if(last_fix_index < 0)
     {
       last_fix_index = client_ptr->batching_info.max_array_size - 1;
     }

     last_position = &client_ptr->batching_info.loc_batching_array[last_fix_index];
      
#define GOOD_HEPE_THRESHOLD_FOR_OLD_FIXES (100L)
#define GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES (125L)
#define HEPE_SCALE_FOR_NEW_FIXES         (1/2)

     /* We are biased towards keeping the latest fix. We keep the old fix only 
      *   If the old fix is reasonably accurate 
      *   AND the new fix HEPE is poor               
      *   AND the new fix has a punc that is twice that of the old fix */
     if((last_position->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_OLD_FIXES)    &&
        (batching_element->horUncCircular >= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES) &&
        (last_position->horUncCircular <= 
         (HEPE_SCALE_FOR_NEW_FIXES*batching_element->horUncCircular)))
     {
       LOC_MSG_MED("Last fix Hepe %d New fix HEPE %d. Drop New",
         last_position->horUncCircular, batching_element->horUncCircular, 0);
       *ignoreNewFix    = TRUE;
       *overWriteOldFix = FALSE;
     }
     else 
     {
       LOC_MSG_MED("Last fix Hepe %d New fix HEPE %d. Keep New",
         last_position->horUncCircular, batching_element->horUncCircular, 0);
       *ignoreNewFix    = FALSE;
       *overWriteOldFix = TRUE;
     }
     return;
  }  /* if((batchCfg->batchConfigMask & AON_BATCHING_CONFIG_MASK_BATCH_ALL_BREACHES) == 0x0) */
  else
  {
    /* round off timestamp to nearest second */
    curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;
	  
    /* Batch ALL is set !! */
#define MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES  (60*1000ULL) /* 60 seconds */
    /* Batch All is set, and the latest fix comes MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES 
     * milli seconds after the last fix. Keep this one and the previously batched fix */
    if((curr_time_utc - client_ptr->batching_info.last_batched_fix_utc)
        >= MIN_TBF_FOR_BATCH_ALL_TO_KEEP_ALL_FIXES)
    {
      LOC_MSG_MED("Time bet new & last fix: (%d) > 60 secs. Keep both",
          (curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batchCfg->minInterval, 0);	    	    
      return;
    }
    
    /* Time difference does not tell us which fix to keep. Select the "better
     * fix" */
    last_fix_index  = client_ptr->batching_info.write_index - 1;      
    if(last_fix_index < 0)
    {
      last_fix_index = client_ptr->batching_info.max_array_size - 1;
    }

    last_position = &client_ptr->batching_info.loc_batching_array[last_fix_index];
    
    /* We are biased towards keeping both fixes!! Figure out if two successive 
     * positions have a large intersection */
    distance_bet_positions =  loc_CalcDistBetLatLongHaversine(
                                batching_element->latitude  * LAT_LONG_SCALE,
                                batching_element->longitude * LAT_LONG_SCALE,
                                last_position->latitude  * LAT_LONG_SCALE, 
                                last_position->longitude * LAT_LONG_SCALE);
    hepe_sum = batching_element->horUncCircular + last_position->horUncCircular;


    MSG_5(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED,
          "Dist bet posn %d NewHepe %d newTech 0x%x OldHepe %d oldTech 0x%x",
          (distance_bet_positions), 
          batching_element->horUncCircular, last_position->horUncCircular,
          batching_element->technologyMask, last_position->technologyMask);
    
    /* Comparing the Distance between the positions with the horiz unc is a 
     * crude measure of how much the two positions intersect. 
     *
     * If we have a large intersection, we can afford to keep the better fix!! */
    if(distance_bet_positions <= (0.68 * hepe_sum))
    {
      /* Keep the fix with the better HEPE */
      if(last_position->horUncCircular < batching_element->horUncCircular)
      {
        LOC_MSG_MED("Drop New Fix!!", 0, 0, 0);
        *ignoreNewFix    = TRUE;
        *overWriteOldFix = FALSE;
      }
      else
      {
        LOC_MSG_MED("Overwrite Old Fix!!", 0, 0, 0);
        *ignoreNewFix    = FALSE;
        *overWriteOldFix = TRUE;
      }
      return;	    
    }
    else if(distance_bet_positions <= (0.8 * hepe_sum))
    {
      if((last_position->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES)    &&
         (batching_element->horUncCircular <= GOOD_HEPE_THRESHOLD_FOR_NEW_FIXES))
      {
        LOC_MSG_MED("Both fixes have good hepe. Keep both!", 0, 0, 0);
        /* Keep both fixes if both fixes have good hepe even if they intersect */
        return;
      }
      
      /* One of the two fixes does not have a good HEPE. Keep the fix with the 
       * better HEPE */
      if(last_position->horUncCircular < batching_element->horUncCircular)
      {
        LOC_MSG_MED("Drop New Fix!!", 0, 0, 0);
        *ignoreNewFix    = TRUE;
        *overWriteOldFix = FALSE;
      }
      else
      {
        LOC_MSG_MED("Overwrite Old Fix!!", 0, 0, 0);
        *ignoreNewFix    = FALSE;
        *overWriteOldFix = TRUE;
      }
      return;
    }
    else
    {
      LOC_MSG_MED("Keep both fixes as intersection is small!", 0, 0, 0);
      return;
    }
  }


  LOC_MSG_ERROR("Could not pick better of two fixes. Keep both!", 0, 0, 0);
  return;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to push the position report to LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report 


@retval    TRUE           adding position report to LIFO success
@retval    FALSE          adding position report to LIFO fail
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_push_to_lifo
(
   loc_client_info_s_type           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
)
{
  uint64 curr_time_utc   = 0;
  int32 curr_write_index = 0;
  boolean ignoreNewFix    = FALSE;
  boolean overWriteOldFix = FALSE;
  
  if((client_ptr == NULL) || (batching_element == NULL) || (batchCfg == NULL))
  {
     LOC_MSG_ERROR("Null pointer!", 0, 0, 0);
     return FALSE;
  }

  if( NULL == client_ptr->batching_info.loc_batching_array )
  {
     LOC_MSG_ERROR("Null loc_batching_array!", 0, 0, 0);
     return FALSE;
  }

  /* round off timestamp to nearest second */
  curr_time_utc = ((batching_element->timestampUtc + 500ULL) / 1000ULL) * 1000ULL;

  if(client_ptr->batching_info.total_occupied <= 0)
  {
    /* buffer is anyways empty, reset the write/read pointers */
    client_ptr->batching_info.write_index = 0;
    client_ptr->batching_info.read_index = -1;
    LOC_MSG_MED("PUSH: total_occupied <= 0 hence resetting write (0) & read (-1) indexes", 0, 0, 0);
  }
  curr_write_index = client_ptr->batching_info.write_index;
  
  /*****************************************************************************  
   * Figure out if we need to overwrite a fix in our buffer or throw away the  *
   * new fix, or keep the last fix as well as the latest fix                   *
   ****************************************************************************/
  loc_batching_select_best_fixes(client_ptr,
                                 batching_element,
                                 batchCfg,
                                 &ignoreNewFix,
                                 &overWriteOldFix);

  if(ignoreNewFix == TRUE)
  {
    LOC_MSG_HIGH("Ignore Latest Fix!", 0, 0, 0);
    return FALSE;
  }
  else if(overWriteOldFix == TRUE)
  {
    /* position should be written at index one behind the write index */
    curr_write_index = curr_write_index - 1;
    if(curr_write_index < 0)
    {
      curr_write_index = client_ptr->batching_info.max_array_size - 1;
    }
      
    LOC_MSG_HIGH("PUSH: Fix rcvd in %u ms interval, fix id %d overwrite fix@ idx %d",
          (uint32)(curr_time_utc - client_ptr->batching_info.last_batched_fix_utc), 
          batching_element->fixId,
          curr_write_index);
    
  }
  else
  {
    LOC_MSG_HIGH("Keep both the last and the latest fix!", 0, 0, 0);
  }

  /******************* Update the fix at curr_write_index ******************/
  /* update the fix id in the stored report */
  batching_element->fixId = client_ptr->batching_info.fix_id;  // update fixId for live report & batched pos
  
  /* copy the position into the buffer */
  client_ptr->batching_info.loc_batching_array[curr_write_index] = (*batching_element);
    
  /************************* increment the write index **********************/
  client_ptr->batching_info.write_index = curr_write_index+1;

  /* Check if the write index reached the top of buffer and reset. Mark the overwrite flag */
  if ( client_ptr->batching_info.write_index == client_ptr->batching_info.max_array_size )
  {
    client_ptr->batching_info.write_index = 0;
    client_ptr->batching_info.overwrite_flag = TRUE;
    LOC_MSG_LOW("PUSH: write_index reached max (%d) resetting to 0, and overwrite_flag is TRUE",
      client_ptr->batching_info.max_array_size, 0, 0);
  }

  if(ignoreNewFix == FALSE)
  {
    if(overWriteOldFix == FALSE)
    {
      /* If an old fix is not being overwritten, increment the total number of 
       * occupied fixes */
      (client_ptr->batching_info.total_occupied)++;
      (client_ptr->batching_info.num_fixes_batched_lifetime++);
      /* update the last last batched fix time. Subsequent positions will be checked against this */ 
      client_ptr->batching_info.last_last_batched_fix_utc = client_ptr->batching_info.last_batched_fix_utc;
    }
    client_ptr->batching_info.last_batched_fix_utc = curr_time_utc;      
  }

  /* array overwrites, the value still remains MAX-ARRAY */
  if ( client_ptr->batching_info.total_occupied > client_ptr->batching_info.max_array_size )
  {
    /* counter always remains same */
    client_ptr->batching_info.total_occupied = client_ptr->batching_info.max_array_size;
    LOC_MSG_LOW("PUSH: total_occupied peaked (%d), write index (%d)", client_ptr->batching_info.total_occupied, client_ptr->batching_info.write_index, 0);
  }

  return TRUE;
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle position report from LocAPI. The function pushes the 
  batched fix into the LIFO buffer and notifies the control point depending
  on the event registration masks.
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Pointer to the Batched position report

@retval    TRUE           push to LIFO buffer is success
@retval    FALSE          push to LIFO buffer is fail
                                  
*//* ------------------------------------------------------------------------*/
static boolean loc_batching_handle_position_report
(
   loc_client_info_s_type           *const client_ptr,
   qmiLocBatchedReportStructT_v02   *const batching_element,
   locAonBatchingConfig             *const batchCfg
)
{ 
  if(client_ptr->batching_info.last_fix_timestamp_utc == batching_element->timestampUtc)
  {
    /* Ignore fix with time stamp same as the last fix received */
    LOC_MSG_HIGH("Ignore fix with same timestamp as last fix reported", 0, 0, 0);
    return FALSE;
  }

  client_ptr->batching_info.last_fix_timestamp_utc = batching_element->timestampUtc;

		  
   /* push the entry into batching buffer */
  if (FALSE == loc_batching_push_to_lifo(
                   client_ptr, batching_element, batchCfg))
  {
    return FALSE;
  }

  /* update the fix id in the position report report */
  batching_element->fixId = client_ptr->batching_info.fix_id;  
   
  /* increment the fix id counter */
  client_ptr->batching_info.fix_id++; 

  LOC_MSG_MED("fix Id %d batched at index %d for client %d", batching_element->fixId,
	client_ptr->batching_info.write_index-1, client_ptr->client_handle);

  /* handle:: first report live fix, and then report batch-full */

  /* check if client registered for live fixes and report */
  loc_batching_report_live_position_event(client_ptr, batching_element);

  /* check for buffer full */
  if((client_ptr->batching_info.total_occupied == client_ptr->batching_info.max_array_size) &&
     (client_ptr->batching_info.buffer_full_reported != TRUE))
  {
    /* check if client registered for batch full event and notify */
    loc_batching_report_batch_full_event(client_ptr);
    client_ptr->batching_info.buffer_full_reported = TRUE;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle position report from loc AON. The function pushes the 
  batched fix into the LIFO buffer and notifies the control point depending
  on the event registration masks.
  
@param[in]   loc_client_handle     Client Handle 
@param[in]   batching_element      Pointer to the Batched position report

@retval    TRUE           push to LIFO buffer is success
@retval    FALSE          push to LIFO buffer is fail
                                  
*//* ------------------------------------------------------------------------*/
boolean loc_batching_batch_position
(
  loc_client_handle_type  loc_client_handle,
  qmiLocBatchedReportStructT_v02   *const batching_element,
  locAonBatchingConfig             *const batchCfg
)
{ 

  loc_client_info_s_type *client_info_ptr = loc_client_find_handle(loc_client_handle);

  if(NULL == client_info_ptr)
  {
	LOC_MSG_ERROR(" loc_batching_batch_position: Invalid params",0,0,0);
	return FALSE;
  }

  return (loc_batching_handle_position_report(
              client_info_ptr, batching_element, batchCfg));
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to free the allocated buffer memory and reset the client's batching
   related fields. 
  
@param[in]   client_ptr            Client Info Handle 

@retval    TRUE           Buffer memory release success
@retval    FALSE          Buffer memory release failed
                                  
*//* ------------------------------------------------------------------------*/
boolean loc_batching_free_and_reset(loc_client_info_s_type     *const client_ptr)
{
   if(NULL == client_ptr)
   {
      LOC_MSG_MED("loc_batching_free_and_reset NULL client_ptr", 0, 0, 0);
      return FALSE;
   }

   /* free the buffer */
   if ( NULL != client_ptr->batching_info.loc_batching_array )
   {
      os_MemFree((void **)&client_ptr->batching_info.loc_batching_array);
      LOC_MSG_MED("loc_batching_free_and_reset free loc batching array", 0, 0, 0);
   }

   /* reset all fields */
   memset(&(client_ptr->batching_info),0,sizeof(client_ptr->batching_info));
   client_ptr->batching_info.read_index = -1;
   client_ptr->batching_info.write_index = -1;

   LOC_MSG_MED("loc_batching_free_and_reset for batching client SUCCESS", 0, 0, 0);
   return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to allocate the buffer memory based on availability and control
   points request.
  
@param[in]   client_ptr            Client Info Handle 
@param[out]  p_GetBatchSizeInd     Get Batch Indication message to control point
@param[in]   client_batch_size_req Batch size requested by control point 
@param[in]   reqTransactionId      Transaction Id in request message by control point 

@retval    TRUE           Buffer memory alloc success
@retval    FALSE          Buffer memory alloc failed
                                  
*//* ------------------------------------------------------------------------*/
boolean locGetBatchSizeHandleAllocate(loc_client_info_s_type         *const client_ptr,
                                      qmiLocGetBatchSizeIndMsgT_v02  *const p_GetBatchSizeInd,
                                      uint32                         client_batch_size_req,
                                      uint32                         reqTransactionId)
{
  qmiLocStatusEnumT_v02 request_status = eQMI_LOC_SUCCESS_V02;
  uint32 actual_batch_size = 0;
  uint32 malloc_retry_reduce = 0;
  void* buffer_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  

  /* check if batch size requested is valid */
  if ( client_batch_size_req <= 0 )
  {
    client_batch_size_req = 0; /* return 0 to client in indication */
    request_status = eQMI_LOC_INVALID_PARAMETER_V02; /* return failure to client in indication */
  }
  else
  {
    actual_batch_size = client_batch_size_req + LOC_BATCHING_ADDITIONAL_SLOTS; /* add extra slots for buffer manipulation */

    LOC_MSG_MED ("locGetBatchSizeHandleAllocate: client handle = %d, client_batch_size_req = %d, actual_batch_size = %d with reserved",
                 client_ptr->client_handle, client_batch_size_req, actual_batch_size);

    /* malloc retry in 4 parts */
    malloc_retry_reduce = actual_batch_size / 4;

    /* avoid the possible (32bit) integer overflow when the total bytes size is computed. */
    if ( actual_batch_size < (UINT32_MAX / sizeof(qmiLocBatchedReportStructT_v02)) )   
    {
      /* actual_batch_size is in limits to avoid buffer overflow */
      LOC_MSG_MED("locGetBatchSizeHandleAllocate: client handle = %d, actual_batch_size is in limits to avoid buffer overflow",
                  client_ptr->client_handle, 0, 0);

      do
      {
        buffer_ptr = (void *)os_MemAlloc(actual_batch_size * sizeof(qmiLocBatchedReportStructT_v02), OS_MEM_SCOPE_TASK);

        if ( NULL == buffer_ptr )
        {
          /* if malloc fails, reduce the size of the batch and retry */
          actual_batch_size = actual_batch_size - malloc_retry_reduce;
        }
        else
        {
          /* malloc success */
          LOC_MSG_MED("locGetBatchSizeHandleAllocate: Sucess Batching malloc - client handle = %d, total batch size = %d, size of each fix = %d",
                      client_ptr->client_handle, actual_batch_size, sizeof(qmiLocBatchedReportStructT_v02));

          break;
        }
      }
      while ( actual_batch_size > LOC_BATCHING_ADDITIONAL_SLOTS );

    }
    else
    {
      LOC_MSG_MED("locGetBatchSizeHandleAllocate: client handle = %d, actual_batch_size is not in limits to avoid buffer overflow ",
                  client_ptr->client_handle, 0, 0);
    }

    if ( buffer_ptr == NULL )
    {
      /* insufficient memory - return failure IND */
      client_ptr->batching_info.is_batching_client = FALSE;
      client_ptr->batching_info.client_batch_size = 0;
      client_ptr->batching_info.total_batch_size = 0;
      client_ptr->batching_info.loc_batching_array = NULL;

      client_batch_size_req = 0; /* return 0 to client in indication */
      request_status = eQMI_LOC_INSUFFICIENT_MEMORY_V02;

      LOC_MSG_MED ("locGetBatchSizeHandleAllocate: Batching client handle = %d, allocation failed",
                   client_ptr->client_handle, 0, 0);
    }
    else
    {
      client_ptr->batching_info.is_batching_client = TRUE;
      client_ptr->batching_info.client_batch_size = actual_batch_size - LOC_BATCHING_ADDITIONAL_SLOTS;
      client_ptr->batching_info.total_batch_size = actual_batch_size;
      client_ptr->batching_info.loc_batching_array = (qmiLocBatchedReportStructT_v02 *)buffer_ptr;
	  client_batch_size_req = client_ptr->batching_info.client_batch_size; /* return size to client in indication */
      request_status = eQMI_LOC_SUCCESS_V02;

      LOC_MSG_MED ("locGetBatchSizeHandleAllocate: Batching client handle = %d, batch size = %d",
                   client_ptr->client_handle, client_ptr->batching_info.client_batch_size, 0);

      /* prepare the batching buffer and other variables */
      //loc_batching_variables_set.loc_batching_array = (loc_batched_position_s_type *)buffer_ptr;
      client_ptr->batching_info.write_index = 0;
      client_ptr->batching_info.read_index = -1;
      client_ptr->batching_info.total_occupied = 0;
      client_ptr->batching_info.buffer_full_reported = FALSE;
      client_ptr->batching_info.overwrite_flag = FALSE;
      client_ptr->batching_info.max_array_size = client_ptr->batching_info.client_batch_size;
      client_ptr->batching_info.last_batched_fix_utc = 0;
      client_ptr->batching_info.last_last_batched_fix_utc = 0;
      client_ptr->batching_info.num_fixes_batched_lifetime = 0;
      client_ptr->batching_info.fix_id = 0;
      client_ptr->batching_info.unable_to_track_reported = FALSE;
    }
  }

  /* update the status field in indication */
  p_GetBatchSizeInd->status = request_status;

  /* update batch size in indication - mandatory field */
  p_GetBatchSizeInd->batchSize = client_batch_size_req;

  /* update transaction id in indication - mandatory field */
  p_GetBatchSizeInd->transactionId = reqTransactionId; 
  
  return TRUE;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to retrieve the position report from LIFO buffer
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   number_of_reads       Number of reports to retrieve
@param[out]   pReadFromBatchInd    Indication to store the reports


@retval    int32           number of reports actually retrieved from buffer
*//* ------------------------------------------------------------------------*/
static int32 loc_batching_pull_from_lifo(loc_client_info_s_type         *const client_ptr,
                                         int32                          number_of_reads,
                                         qmiLocReadFromBatchIndMsgT_v02 *pReadFromBatchInd)
{
  int i = -1;

  /* pReadFromBatchInd is already memset'ed at caller, so no need to initialize each field */
  if((client_ptr == NULL) || (pReadFromBatchInd == NULL))
  {
     LOC_MSG_ERROR("loc_batching_pull_from_lifo Null pointer!", 0, 0, 0);
     return 0;
  }

  if( NULL == client_ptr->batching_info.loc_batching_array )
  {
     LOC_MSG_ERROR("loc_batching_pull_from_lifo Null loc_batching_array!", 0, 0, 0);
    return 0;
  }

  /* always read = (write - 1) */
  client_ptr->batching_info.read_index = client_ptr->batching_info.write_index - 1;
  client_ptr->batching_info.buffer_full_reported = FALSE;

  for(i = 0; i < number_of_reads; i++)  
  {
    if(client_ptr->batching_info.read_index < 0) 
    {
      if(client_ptr->batching_info.overwrite_flag == TRUE)
      {
        client_ptr->batching_info.read_index = client_ptr->batching_info.max_array_size - 1;
        client_ptr->batching_info.overwrite_flag = FALSE; /* reset the flag */
        LOC_MSG_LOW("PULL: overwrite_flag resetted to FALSE", 0, 0, 0);
      }
      else
      {
        break;
      }
    }

    if(client_ptr->batching_info.total_occupied <= 0)
    {
      break;
    }
    
    /* update the counter fields in the indication */
    pReadFromBatchInd->numberOfEntries++;
    pReadFromBatchInd->batchedReportList_len++;

    /* already memset'ed the buffer */
    /* fill the position report into the indication */
    pReadFromBatchInd->batchedReportList[i] = client_ptr->batching_info.loc_batching_array[client_ptr->batching_info.read_index];
    
    LOC_MSG_LOW("PULL: Peek fixId %d from Ind at index i %d", pReadFromBatchInd->batchedReportList[i].fixId, i, 0);

    /* adjust the Read/Write indexes */
    client_ptr->batching_info.write_index = client_ptr->batching_info.read_index;
    (client_ptr->batching_info.read_index)--;
    
    /* update the total entries remaining in the buffer */
    (client_ptr->batching_info.total_occupied)--;
    LOC_MSG_MED("PULL: updated read (%d) write (%d) indexes, total_occupied = %d", client_ptr->batching_info.read_index,
                client_ptr->batching_info.write_index, client_ptr->batching_info.total_occupied);
  }

  if ( i > 0 )
  {
    pReadFromBatchInd->numberOfEntries_valid = TRUE;
    pReadFromBatchInd->batchedReportList_valid = TRUE;
  }

  LOC_MSG_MED("loc_batching_pull_from_lifo: number of entries returning %d total occupied %d", i, client_ptr->batching_info.total_occupied, 0);

  return i;
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle read operation from the lifo buffer. 
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   number_of_reads       Number of fixes to be read from batching buffer
@param[out]  pReadFromBatchInd     Indication with batched position reports


@retval    TRUE           number of reads field is valid
@retval    FALSE          number of reads field is not valid
                                  
*//* ------------------------------------------------------------------------*/
boolean loc_handle_batching_read(loc_client_info_s_type            *const client_ptr,
                                 int32                             number_of_reads,
                                 qmiLocReadFromBatchIndMsgT_v02    *const pReadFromBatchInd)
{ 
  LOC_MSG_MED("loc_handle_batching_read: number of entries to read %d", number_of_reads, 0, 0);

  if (( number_of_reads <= 0 ) || 
      ( number_of_reads > QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02 ))
  {
    return FALSE;
  }

  /* pull the entries from batching buffer */
  loc_batching_pull_from_lifo(client_ptr, number_of_reads, pReadFromBatchInd);

  return TRUE;
}




