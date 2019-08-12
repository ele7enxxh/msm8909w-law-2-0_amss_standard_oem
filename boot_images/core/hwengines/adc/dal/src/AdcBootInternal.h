#ifndef ADC_BOOT_INTERNAL_H
#define ADC_BOOT_INTERNAL_H
/*============================================================================
  @file AdcBootInternal.h

  Function and data structure declarations for ADC boot


                Copyright (c) 2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/dal/src/AdcBootInternal.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DalDevice.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   const char *pszDevName;
   uint32 uQueueSize;
} AdcPhysicalDeviceType;

typedef struct
{
   const AdcPhysicalDeviceType *paAdcPhysicalDevices;
   uint32 uNumPhysicalDevices;
} AdcBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef ADC_BOOT_INTERNAL_H */

