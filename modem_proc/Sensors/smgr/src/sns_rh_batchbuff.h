#ifndef SNS_RH_BATCHBUFF_H
#define SNS_RH_BATCHBUFF_H
/*=============================================================================
  @file sns_rh_batchbuff.h

  This header file contains the data definitions used within the
  Batch buffer module of Request Handler task.

*******************************************************************************
* Copyright (c) 2015 Qualcomm Technologies, Inc. 
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_batchbuff.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-11-23  pn   Added sns_rh_batchbuff_handle_conn_busy()
  2015-10-28  bd   Use number of samples instead of end_timestamp to limit sample cnt 
  2015-10-20  bd   Removed sns_rh_batchbuff_release
  2015-09-11  pn   Changed signature of sns_rh_batchbuff_save_samples()
  2015-09-03  bd   Separate batchbuff for uimg and bimg
  2015-08-24  bd   File cleanup
  2015-08-18  bd   Remove sample ready sig for each batching sample
  2015-08-18  bd   Change batchbuff structure to improve memory utilization
  2015-07-30  bd   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_osa.h"
#include "sns_queue.h"
#include "sns_rh.h"
#include "sns_smgr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_RH_BATCHBUFF_MAX_SAMPLE 20
#define SNS_RH_BATCHBUFF_FLUSH_THRESHOLD SNS_RH_BATCHBUFF_MAX_SAMPLE/2

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct {
  sns_ddf_time_t  timestamp;
  int32_t data[SNS_SMGR_SENSOR_DIMENSION_V01];
  /**<   Each sample can have up to SNS_SMGR_SENSOR_DIMENSION words, each word
    is in Q16 format and in the units specific to the sensor/data type pair.
    For 3-axis samples, Data[0], Data[1], and Data[2] are X, Y, and Z axis,
    respectively.  For others, only Data[0] has valid measurement.
    */
   
  uint8_t flags;
  /**<   Status flags of this sample.
      - raw data
      - SNS_SMGR_ITEM_FLAG_INVALID
      - SNS_SMGR_ITEM_FLAG_FAC_CAL : Factory calibration data was applied
      - SNS_SMGR_ITEM_FLAG_AUTO_CAL: Auto calibration data was applied
      - All other values defined as SNS_SMGR_ITEM_FLAG_XXXX style are reserved
        for future use
    */
  
  uint8_t quality;
    /**<   Quality of this sample.
      - SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE
      - SNS_SMGR_ITEM_QUALITY_FILTERED
      - SNS_SMGR_ITEM_QUALITY_INTERPOLATED
      - SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED
      - SNS_SMGR_ITEM_QUALITY_INVALID_FAILED_SENSOR
      - SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY
      - All other values defined as SNS_SMGR_ITEM_QUALITY_XXXX style are reserved
        for future use
    */
}sns_rh_batchbuff_sample_s;  /* Type */
  
  
/* size of sns_rh_batchbuff_s should not exceed SNS_BATCHBUFF_MAX_BLK_SIZE */

typedef struct
{
  sns_q_link_s q_link;
  uint8_t      num_valid_samples; /*total sample cnt stored*/
  uint8_t      num_samples;       /*number of samples that have not been sent*/
  uint16_t     first_sample_idx;  /*earlies sample which has not been sent*/
    
  sns_rh_batchbuff_sample_s samples[SNS_RH_BATCHBUFF_MAX_SAMPLE];
}sns_rh_batchbuff_s;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
extern void sns_rh_batchbuff_init();
extern bool sns_rh_batchbuff_save_samples(sns_rh_rpt_item_s* item_spec_ptr);

extern void sns_rh_process_batchbuff_low_request(void);

extern uint32_t sns_rh_batchbuff_generate_buffering_report(
  sns_rh_rpt_spec_s*  rpt_spec_ptr,
  uint32_t            num_samples,
  uint8_t             ind_type);

extern uint32_t sns_rh_batchbuff_compute_num_avail_samples(
  const sns_rh_rpt_spec_s*  rpt_spec_ptr,
  sns_ddf_time_t*           oldest_last_sent_ts_ptr,
  sns_ddf_time_t*           latest_sample_ts_ptr);

extern void sns_rh_batchbuff_handle_conn_busy(sns_rh_rpt_spec_s* rpt_ptr);

#endif /* #ifndef SNS_RH_MR_H */
