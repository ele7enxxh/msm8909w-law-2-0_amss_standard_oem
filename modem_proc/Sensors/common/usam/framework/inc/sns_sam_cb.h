#ifndef SNS_SAM_CB_H
#define SNS_SAM_CB_H

/*============================================================================
  @file sns_sam_cb.h

  @brief
  Generate the callback functions for an algorithm.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "fixed_point.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Initialize all data structures associated with algorithm callbacks.  This
 * will include generating the callback struct to be used by all algorithms.
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Unable to initialize all required static data
 */
sns_sam_err sns_sam_cb_init_fw( void );

/**
 * Process all algorithm timer events received.
 */
void sns_sam_handle_algo_timer( void );

/**
 * Clear all algorithm timer events associated with the specified algorithm
 * instance.
 *
 * @param[i] algoInst Algorithm instance to search for
 */
void sns_sam_clear_algo_timer( sns_sam_algo_inst const *algoInst );

#endif /* SNS_SAM_CB_H */
