#ifndef SNS_SAM_DL_H
#define SNS_SAM_DL_H

/*============================================================================
  @file sns_sam_dl.h

  @brief
  Functions to support dynamic loading of SAM algorithms.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_sam.h"

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Initialize DL USAM capabilities.  Creates a handle with the DL service and
 * initializes SAM DL global state.
 *
 * @return SAM_ENONE
 *         SAM_EFAILED DL Framework initialization failed
 *         SAM_ENOMEM Not enough memory to initialize
 */
sns_sam_err sns_sam_init_dl( void );

/**
 * Lookup and record all dynamically loaded algorithms.
 */
void
sns_sam_dl_record_algos( void );

#endif /* SNS_SAM_DL_H */
