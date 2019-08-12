/**
  @file  DDIAdcBoot.h
  @brief ADC Protocol for Boot.
*/
/*=============================================================================
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/hwengines/AdcBoot.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  when       who what, where, why
  ---------- --- -----------------------------------------------------------
  2013-07-12 jjo Added API comments.
  2012-11-05 gps Initial revision.
=============================================================================*/
#ifndef _ADCBOOT_H_
#define _ADCBOOT_H_

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "AdcInputs.h"
#include "DalDevice.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
#define ADC_BOOT_INVALID_IDX (uint32) (-1)

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

/** Status of an ADC result. */
typedef enum
{
   ADC_BOOT_RESULT_INVALID = 0,  /**< Result is invalid (result is out of range
                                     or an error occurred during conversion). */
   ADC_BOOT_RESULT_VALID        /**< Result is valid. */
} AdcBootResultStatusType;

/** Data structure for the output of ADC_BOOT_READ(). */
typedef struct
{
   AdcBootResultStatusType eStatus; /**< Status of the conversion; see
                                        #AdcBootResultStatusType for details. */
   int32 nPhysical;      /**< Result in physical units. */
   int32 nPercent;       /**< Result as a percentage ratio of the ADC
                             reference voltage used for the conversion
                             (if supported). 0 = 0%, 65535 = 100%. */
   int32 nMicrovolts;    /**< Result in microvolts (if supported). */
   int32 nCode;          /**< Raw ADC code. */
} AdcBootResultType;

/** Data structure for the input to ADC_BOOT_READ(). This structure is obtained
  by calling ADC_BOOT_GET_CHANNEL(). */
typedef struct
{
   uint32 nDeviceIdx;   /**< Device index. */
   uint32 nChannelIdx;  /**< Channel index. */
} AdcBootDeviceChannelType;

/** Data structure for the output of ADC_BOOT_RECALIBRATE_CHANNEL(). */
typedef struct
{
   AdcBootResultStatusType eStatus; /**< Status of the recalibration; see
                                        #AdcBootResultStatusType for details. */
   int32 nPhysical1;     /**< Physical value for ref 1. */
   int32 nPhysical2;     /**< Physical value for ref 2. */
   uint32 uCode1;        /**< Raw ADC code for ref 1. */
   uint32 uCode2;        /**< Raw ADC code for ref 2. */
} AdcBootRecalibrateChannelResultType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
   @brief ADC init

   Initializes ADC. Note: this function can be called more than once - only
   one initialization is actually performed.

   @dependencies Must be called after PMIC is initialized
                 (i.e. after boot_pm_driver_init in sbl1_hw_init_secondary)

   @return DAL_SUCCESS if the call completed successfully.
           Other values indicate an error.

*/
DALResult AdcBoot_Init(void);

/**
   @brief ADC deinit

   Deinitializes ADC.

   @see    AdcBoot_Init

   @return DAL_SUCCESS if the call completed successfully.
           Other values indicate an error.

*/
DALResult AdcBoot_DeInit(void);

/**
   @brief Gets a channel

   This function gets an ADC channel by looking at a string name.

   @param  pszInputName: [in] ADC channel string name
   @param  pChannel: [out] ADC channel

   @see    AdcBoot_Init

   @return DAL_SUCCESS if the call completed successfully.
           Other values indicate an error.

*/
DALResult AdcBoot_GetChannel(const char *pszInputName, AdcBootDeviceChannelType *pChannel);

/**
   @brief Reads an analog input

   This function performs an ADC converion.

   @param  pBootChannel: [in] Channel to perform a conversion on
   @param  pAdcBootResult: [out] ADC conversion result structure

   @see    AdcBoot_Init

   @return DAL_SUCCESS if the call completed successfully.
           Other values indicate an error.

           Note: make sure to check pAdcBootResult.eStatus == ADC_BOOT_RESULT_VALID

*/
DALResult AdcBoot_Read(AdcBootDeviceChannelType *pBootChannel, AdcBootResultType *pAdcBootResult);

/**
   @brief Recalibrates an analog input

   This function recalibrates an ADC channel.

   @param  pBootChannel: [in] Channel to recalibrate
   @param  pAdcBootRecalChanResult: [out] ADC recalibration result structure

   @see    AdcBoot_Init

   @return DAL_SUCCESS if the call completed successfully.
           Other values indicate an error.

           Note: make sure to check pAdcBootRecalChanResult.eStatus == ADC_BOOT_RESULT_VALID

*/
DALResult AdcBoot_RecalibrateChannel(AdcBootDeviceChannelType *pBootChannel, AdcBootRecalibrateChannelResultType *pAdcBootRecalChanResult);


#endif /* _ADCBOOT_H_ */

