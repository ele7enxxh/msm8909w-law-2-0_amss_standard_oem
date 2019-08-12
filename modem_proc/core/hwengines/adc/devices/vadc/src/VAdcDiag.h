#ifndef VAdcQxdm_H
#define VAdcQxdm_H

/*============================================================================
  @file VAdcDiag.h
 
  Function and data structure declarations for VADC DIAG packet logging
 
 
                Copyright (c) 2011-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/vadc/src/VAdcDiag.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "log_codes.h"
#include "log.h"
#include "diagpkt.h"

#include "DalVAdc.h"
#include "DDIAdc.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  VADC information for logging
    - This structure holds the VADC info for logging
    - This structure is PACKED so there is no padding between members
    - Maintain the members in decending order of size for efficiency
----------------------------------------------------------------------------*/

typedef PACK( struct )
{
   uint32 eStatus;      /* status of the conversion, 1 = valid */
   uint32 nChannelIdx;  /* the channel index for this conversion */
   int32  nPhysical;    /* result in physical units. Units depend on BSP */
   uint32 nPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 nMicrovolts;  /* result in microvolts */
   uint32 nRaw;         /* Raw values */
     
   uint32 nVrefN;       /* Negative voltage reference */
   uint32 nVrefP;       /* Positive voltage reference */
   uint32 nVref0p625;   /* 0.625 V reference voltage */
   uint32 nVref0p125;   /* 0.125 V reference voltage */

   uint32 nDecimationRatio; /* Decimation ratio */
   uint32 nConversionRate;  /* Conversion rate */

} xoadc_diag_log_type;


/*----------------------------------------------------------------------------
  Diag Log packet type
    - This structure holds the XO ADC result info that will be sent to diag
    - This structure is PACKED so there is no padding between members
----------------------------------------------------------------------------*/
typedef PACK( struct )
{

  /* Diag Log header                         */
  log_hdr_type                hdr;

  /* Log version                             */
  uint8                       version;
  
  /* Current log data of client              */
  xoadc_diag_log_type   client_curr_info;

} LOG_XOADC_C_type;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
   @brief Dumps VADC information to DIAG

   This function will dump the VADC information to an external
   logging interface i.e DIAG.

   @param  current_data: Pointer to the VADC result data struct
   @param  decimationRatio: The VADC decimationRatio configuration index
   @param  conversationRate: The VADC conversion rate configuration index
   @param  calibrationData: Pointer to the calibration data struct used for the conversion

   @return TRUE if the log was sent to DIAG. FALSE if the logging failed.
*/
extern boolean vadc_diag_send_client_log
(
  /* Current information of client */
  AdcDeviceResultType *current_data,
  uint32 decimationRatio,
  uint32 conversionRate,
  const VAdcCalibDataType *calibrationData
);

#endif /* VAdcDiag_H */

