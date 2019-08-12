/*============================================================================
  FILE:         DALVAdcDiag.c

  OVERVIEW:     This file provides the interface for the VADC with DIAG.
                
  DEPENDENCIES: None
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/vadc/src/VAdcDiag.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-02-07  jjo  Fix so correct status shows in QXDM.
  2012-04-20  jdt  Support for VADC peripheral.
  2011-12-02  gps  Initial revision

============================================================================*/


#include "VAdcDiag.h"

/* Version of VADC manager logging */
#define VADC_LOG_VERSION_1   1

/*------------------------------------------------------------------------------
Following functions are external to this module but internal to the VADC driver. 
------------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION VADC_DIAG_SEND_CLIENT_LOG

DESCRIPTION
  This function will dump the XO ADC information to an external
  logging interface i.e DIAG.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the log was sent to diag
  FALSE - If the logging failed
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean vadc_diag_send_client_log
(
  /* Current information of client */
  AdcDeviceResultType *current_data,
  uint32 decimationRatio,
  uint32 conversionRate,
  const VAdcCalibDataType *calibrationData
)
{
  //TODO UPDATE THIES:
  LOG_XOADC_C_type *log_ptr = NULL;  /* Diag Log packet */
  boolean         result   = FALSE; /* Result          */

  /* Allocate a log packet to send to diag */
  log_ptr = (LOG_XOADC_C_type*)log_alloc_ex
            (
              (log_code_type)LOG_XOADC_C,
              sizeof(LOG_XOADC_C_type)
            );

  if ( log_ptr != NULL ) {

    /* Update the version number of the log packet */
    log_ptr->version = VADC_LOG_VERSION_1;


    if(current_data != NULL)
    {
      /* Update the wakeup mgr client current information */
      
          if (current_data->eStatus == ADC_DEVICE_RESULT_VALID)
          {
             log_ptr->client_curr_info.eStatus = 1;
          }
          else
          {
             log_ptr->client_curr_info.eStatus = 0;
          }

          log_ptr->client_curr_info.nChannelIdx = current_data->uChannelIdx;
          log_ptr->client_curr_info.nPhysical   = current_data->nPhysical;
          log_ptr->client_curr_info.nPercent    = current_data->uPercent;

          log_ptr->client_curr_info.nMicrovolts = current_data->uMicrovolts;
          log_ptr->client_curr_info.nRaw        = current_data->uCode;
      
      
          log_ptr->client_curr_info.nVrefN      = calibrationData->uVrefN;
          log_ptr->client_curr_info.nVrefP      = calibrationData->uVrefP;
          log_ptr->client_curr_info.nVref0p625  = calibrationData->uVref1;
          log_ptr->client_curr_info.nVref0p125  = calibrationData->uVref2;      

          log_ptr->client_curr_info.nDecimationRatio = decimationRatio;
          log_ptr->client_curr_info.nConversionRate  = conversionRate;
          
    } 

    /* submit the log to DIAG */
    log_commit( log_ptr );

    result = TRUE;
  }

  /* Return whether the log packet was sent to Diag or not */
  return result;

} /* vadc_diag_send_client_log */

