/*=============================================================================
  @file sns_smgr_depot.c

  This file implements the sample depot management functions for SMGR

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_depot.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-08-24  bd   Remove sns_smgr_depot_copy_samples() and sns_smgr_depot_dump()
  2015-08-23  bd   Check sns_smgr_depot_init_iter return before copying samples
  2015-04-21  pn   Fixed deficient depot copy method
  2015-04-09  pn   Added support for 56-bit timestamp
  2014-10-26  pn   Updated sns_smgr_depot_dump()
  2014-09-25  pn   Added sns_smgr_depot_dump()
  2014-06-05  vy   Further refactored to support uImage  
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_smgr_depot.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_smgr_depot_clear

===========================================================================*/
/*!
  @brief Clears sample depot for given sensors

  @param[i] sensor_ptr - the sensor of interest

  @return none
*/
/*=========================================================================*/
void sns_smgr_depot_clear(sns_smgr_sample_depot_s* depot_ptr, uint16_t num_save)
{
  if ( (depot_ptr != NULL) && (depot_ptr->used_bufs > num_save) )
  {
    SNS_SMGR_PRINTF3(
      LOW, "depot_clear - used=%u first_ts=%u last_ts=%u", depot_ptr->used_bufs,
      depot_ptr->used_bufs ? 
      SMGR_SAMPLE_TIMESTAMP(&depot_ptr->samples[depot_ptr->first_idx]) : 0,
      depot_ptr->used_bufs ? 
      SMGR_SAMPLE_TIMESTAMP(&depot_ptr->samples[depot_ptr->last_idx]) : 0 );
    depot_ptr->first_idx = (depot_ptr->last_idx + depot_ptr->max_bufs -
                            num_save + 1) % depot_ptr->max_bufs;
    depot_ptr->used_bufs = num_save;
    SNS_SMGR_PRINTF3(
      LOW, "depot_clear - used=%d first_idx=%d last_idx=%d",
      depot_ptr->used_bufs, depot_ptr->first_idx, depot_ptr->last_idx);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_depot_get_sample_count

===========================================================================*/
/*!
  @brief Returns number of samples in the given depot.

  @param[i] depot_ptr - the sample depot

  @return Count of used buffers
*/
/*=========================================================================*/
uint16_t sns_smgr_depot_get_sample_count(const sns_smgr_sample_depot_s* depot_ptr)
{
  uint16_t count = 0;
  if ( depot_ptr != NULL )
  {
    count = depot_ptr->used_bufs;
  }
  return count;
}


/*===========================================================================

  FUNCTION:   sns_smgr_depot_get_oldest_sample

===========================================================================*/
/*!
  @brief Returns pointer to oldest sample in then given depot.

  @param[i] depot_ptr - the sample depot

  @return Read-only poinnter to oldest sample, or NULL if no sample available
*/
/*=========================================================================*/
const sns_smgr_sample_s* sns_smgr_depot_get_oldest_sample(
  const sns_smgr_sample_depot_s* depot_ptr)
{
  const sns_smgr_sample_s* sample_ptr = NULL;
  if ( (depot_ptr != NULL) && (depot_ptr->used_bufs > 0) )
  {
    sample_ptr = &depot_ptr->samples[depot_ptr->first_idx];
  }
  return sample_ptr;
}


