#ifndef SNS_SAM_REG_H
#define SNS_SAM_REG_H

/*============================================================================
  @file sns_sam_reg.h

  @brief
  Implements all handling, processing, and generating messages to/from the
  Sensors Registry Service.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_sam_algo_api.h"

/*============================================================================
  External Objects
  ===========================================================================*/

/* Temporary objects used during SAM initialization. */
extern sns_sam_sensor_req sensorReqREG;

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Request the specified group data to be read from or written to the Sensors
 * Registry.
 *
 * @param[io] regData Registry data to read/write
 *
 * @return
 *  SAM_ENONE
 *  SAM_ETYPE Invalid Registry ID or wrong data length
 *  SAM_EFAILED Error occurred sending the request
 */
sns_sam_err sns_sam_reg_req( sns_sam_reg_data const *regData );

/**
 * Process a registry response message.  Note that the SAM Framework only ever
 * will send a registry group read or group write request, so those are the
 * only responses we need to handle.
 *
 * @param[i] respMsg Response message to process
 *
 * @note Message buffer will be freed by caller
 *
 * @return
 *  SAM_ENONE
 *  SAM_EFAILED Error in response message
 */
sns_sam_err sns_sam_reg_handle_resp( sns_sam_resp const *respMsg );

/**
 * Initialize all static objects contained within sns_sam_reg.c.
 *
 * @return SAM_ENONE
 *         SAM_ENOMEM Not enough memory to initialize Framework
 *         SAM_EFAILED Framework initialization failed
 */
sns_sam_err sns_sam_reg_init_fw( void );

#endif /* SNS_SAM_REG_H */
