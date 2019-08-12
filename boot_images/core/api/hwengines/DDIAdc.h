#ifndef __DDIADC_H__
#define __DDIADC_H__

/*============================================================================
  @file DDIAdc.h

  ADC Device Driver Interface header

  Clients may include this header and use these interfaces to read analog
  inputs.

               Copyright (c) 2008-2015 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/hwengines/DDIAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "AdcBoot.h"
#include "AdcInputs.h"
#include "DALFramework.h"
#include "DALDeviceId.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DALADC_INTERFACE_VERSION DALINTERFACE_VERSION(4,0)

enum
{
   ADC_ERROR_INVALID_DEVICE_IDX = 1,
   ADC_ERROR_INVALID_CHANNEL_IDX,
   ADC_ERROR_NULL_POINTER,
   ADC_ERROR_DEVICE_QUEUE_FULL,
   ADC_ERROR_INVALID_PROPERTY_LENGTH,
   ADC_ERROR_OUT_OF_MEMORY,
   ADC_ERROR_API_UNSUPPORTED_IN_THIS_CONTEXT,
   ADC_ERROR_DEVICE_NOT_READY,
   ADC_ERROR_INVALID_PARAMETER,
   ADC_ERROR_OUT_OF_TM_CLIENTS,
   ADC_ERROR_TM_NOT_SUPPORTED,
   ADC_ERROR_TM_THRESHOLD_OUT_OF_RANGE,
   _ADC_MAX_ERROR = 0x7FFFFFFF
};

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   ADC_RESULT_VALID,
   ADC_RESULT_INVALID,
   ADC_RESULT_TIMEOUT,
   ADC_RESULT_FIFO_NOT_EMPTY,
   _ADC_MAX_RESULT_STATUS = 0x7FFFFFFF
} AdcResultStatusType;

typedef struct
{
   enum
   {
      ADC_REQUEST_STATUS_PENDING,   /* the request is being immediately performed */
      ADC_REQUEST_STATUS_QUEUED,    /* the request was queued */
      ADC_REQUEST_STATUS_ERROR,     /* the request was not started due to an error */
      ADC_REQUEST_STATUS_UNKNOWN,   /* the request status is unknown */
      _ADC_MAX_REQUEST_STATUS = 0x7FFFFFFF
   } eStatus;
} AdcRequestStatusType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
} AdcInputPropertiesType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
   DALSYSEventHandle hEvent;
} AdcRequestParametersType;

typedef struct
{
   AdcResultStatusType eStatus;  /* status of the conversion */
   uint32 nDeviceIdx;   /* the device index for this conversion */
   uint32 nChannelIdx;  /* the channel index for this conversion */
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 nPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 nMicrovolts;  /* result in microvolts */
   uint32 nCode;        /* raw ADC code from hardware */
} AdcResultType;

typedef struct
{
   AdcResultStatusType eStatus;  /* status of the conversion */
   uint32 uToken;       /* token which identifies this conversion */
   uint32 uDeviceIdx;   /* the device index for this conversion */
   uint32 uChannelIdx;  /* the channel index for this conversion */
   int32 nPhysical1_uV; /* ref 1 in physical units of uV*/
   int32 nPhysical2_uV; /* ref 2 in physical units of uV */
   uint32 uCode1;       /* raw ADC code for ref 1 */
   uint32 uCode2;       /* raw ADC code for ref 2 */
} AdcRecalibrationResultType;

typedef enum
{
   ADC_TM_THRESHOLD_LOWER,    /* Lower threshold */
   ADC_TM_THRESHOLD_HIGHER,   /* Higher threshold */
   _ADC_TM_MAX_THRESHOLD = 0x7FFFFFFF
} AdcTMThresholdType;

typedef struct
{
   int32 nPhysicalMin;   /* Minimum threshold in physical units */
   int32 nPhysicalMax;   /* Maximum threshold in physical units */
} AdcTMRangeType;

typedef struct
{
   AdcTMThresholdType eThresholdTriggered;   /* Type of threshold that triggered */
   uint32 uTMChannelIdx;                     /* TM channel index */
   int32 nPhysicalTriggered;                 /* Physical value that triggered */
} AdcTMCallbackPayloadType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   @brief Attaches to the ADC driver

   This function attaches the client to the ADC driver. This function is used
   to obtain the device handle which is required to use the ADC driver APIs.

   @param  DevId [in] The device ID to attach to (e.g., DALDEVICEID_ADC)
   @param  hDalDevice [out] Pointer to DAL device handle pointer which will receive
                             a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.

*/
static __inline DALResult
DAL_AdcDeviceAttach(DALDEVICEID DevId, DalDeviceHandle **phDalDevice)
{
   if (DevId != DALDEVICEID_ADC)
   {
      return DAL_ERROR;
   }

   *phDalDevice = NULL;

   return AdcBoot_Init();
}

/**
   @brief Gets the properties for an analog input from the ADC DAL properties

   This function looks up the properties for an analog input.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pInputName [in] The name of the analog input to look up
   @param  nInputNameSize [in] The size of the analog input name in bytes
   @param  pAdcInputProps [out] The buffer to write the property values to

   @return DAL_SUCCESS if the input was found. DAL_SUCCESS if the input was
      found in the properties file.

      ADC_ERROR_INVALID_PROPERTY_LENGTH is returned if the
      property was incorrectly defined.

      All other values correspond to DAL error codes, and should
      generally be interpreted to mean that the analog input is
      not present on the target hardware.

*/
static __inline DALResult
DalAdc_GetAdcInputProperties(DalDeviceHandle *_h,  const char *pInputName,  uint32 nInputNameSize,  AdcInputPropertiesType *pAdcInputProps)
{
   AdcBootDeviceChannelType channel;
   DALResult result;

   result = AdcBoot_GetChannel(pInputName, &channel);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   if (channel.nDeviceIdx == ADC_BOOT_INVALID_IDX || channel.nChannelIdx == ADC_BOOT_INVALID_IDX)
   {
      return DAL_ERROR;
   }

   pAdcInputProps->nDeviceIdx = channel.nDeviceIdx;
   pAdcInputProps->nChannelIdx = channel.nChannelIdx;

   return DAL_SUCCESS;
}

/**
   @brief Reads an analog input

   This function performs a blocking ADC read for the device and channel specified by the
   client in pAdcInputProps.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcInputProps [in] Pointer to input properties for the channel
   @param  pAdcResult [out] ADC conversion result

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcInputProps or pAdcResult was NULL

*/
static __inline DALResult
DalAdc_Read(DalDeviceHandle *_h,  const AdcInputPropertiesType *pAdcInputProps,  AdcResultType *pAdcResult)
{
   AdcBootDeviceChannelType channel;
   AdcBootResultType adcBootResult;
   DALResult result;

   channel.nDeviceIdx = pAdcInputProps->nDeviceIdx;
   channel.nChannelIdx = pAdcInputProps->nChannelIdx;

   result = AdcBoot_Read(&channel, &adcBootResult);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   switch (adcBootResult.eStatus)
   {
      case ADC_BOOT_RESULT_VALID:
         pAdcResult->eStatus = ADC_RESULT_VALID;
         break;
      default:
         pAdcResult->eStatus = ADC_RESULT_INVALID;
   }

   pAdcResult->nDeviceIdx = pAdcInputProps->nDeviceIdx;
   pAdcResult->nChannelIdx = pAdcInputProps->nChannelIdx;
   pAdcResult->nPhysical = adcBootResult.nPhysical;
   pAdcResult->nPercent = adcBootResult.nPercent;
   pAdcResult->nMicrovolts = adcBootResult.nMicrovolts;
   pAdcResult->nCode = adcBootResult.nCode;

   return DAL_SUCCESS;
}

/**
   @brief Recalibrates an analog input

   This function requests the ADC driver to recalibrate the specified
   channel. If other channels share the physical device configuration
   (as defined in the ADC device BSP), then those channels may also be
   updated by the recalibration.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcInputProps [in] Pointer to input properties for the channel
   @param  pAdcRecalResult [out] ADC recalibration result

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full

*/
static __inline DALResult
DalAdc_Recalibrate(DalDeviceHandle *_h,  const AdcInputPropertiesType *pAdcInputProps,  AdcRecalibrationResultType *pAdcRecalResult)
{
   AdcBootDeviceChannelType channel;
   AdcBootRecalibrateChannelResultType adcBootRecalResult;
   DALResult result;

   channel.nDeviceIdx = pAdcInputProps->nDeviceIdx;
   channel.nChannelIdx = pAdcInputProps->nChannelIdx;

   result = AdcBoot_RecalibrateChannel(&channel, &adcBootRecalResult);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   switch (adcBootRecalResult.eStatus)
   {
      case ADC_BOOT_RESULT_VALID:
         pAdcRecalResult->eStatus = ADC_RESULT_VALID;
         break;
      default:
         pAdcRecalResult->eStatus = ADC_RESULT_INVALID;
   }

   pAdcRecalResult->uDeviceIdx = pAdcInputProps->nDeviceIdx;
   pAdcRecalResult->uChannelIdx = pAdcInputProps->nChannelIdx;
   pAdcRecalResult->nPhysical1_uV = adcBootRecalResult.nPhysical1 * 1000;
   pAdcRecalResult->nPhysical2_uV = adcBootRecalResult.nPhysical2 * 1000;
   pAdcRecalResult->uCode1 = adcBootRecalResult.uCode1;
   pAdcRecalResult->uCode2 = adcBootRecalResult.uCode2;

   return DAL_SUCCESS;
}

/**
   @brief Reads an analog input

   This function requests the ADC driver to start an ADC conversion on
   the device and channel specified by the client in pAdcParams.

   Conversion requests are processed on a FIFO basis.

   If the hEvent parameter in pAdcParams is a DAL callback event triggered by the
   ADC driver when the conversion is complete. The payload of the callback is a pointer
   to an AdcResultType struct which contains the result of the ADC conversion.

   If hEvent in pAdcParams is NULL, this function will return ADC_ERROR_NULL_POINTER.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams [in] Pointer to client's request parameters
   @param  pAdcRequestStatus [out] Pointer to buffer which will indicate the status of request
                        (Client can pass NULL for pAdcRequestStatus if status is unwanted)

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams, pAdcParams->hEvent, or pAdcResult was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full

*/
static __inline DALResult
DalAdc_RequestConversion(DalDeviceHandle *_h,  const AdcRequestParametersType *pAdcParams,  AdcRequestStatusType *pAdcRequestStatus)
{
   return DAL_ERROR;
}

/**
   @brief Calibrates an analog input

   This function requests the ADC driver to recalibrate the specified
   channel. If other channels share the physical device configuration
   (as defined in the ADC device BSP), then those channels may also be
   updated by the recalibration.

   The hEvent parameter in pAdcParams is a DAL callback event triggered by the
   ADC driver when the recalibration is complete. The payload of the callback is a pointer
   to an AdcRecalibrationResultType struct which contains the result of the ADC recalibration.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams [in] Pointer to client's request parameters

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full

*/
static __inline DALResult
DalAdc_RequestRecalibration(DalDeviceHandle *_h,  const AdcRequestParametersType *pAdcParams)
{
   return DAL_ERROR;
}

/**
   @brief Gets the input properties for a given ADC TM channel.

   This function looks up the properties for an analog input.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pInputName [in] The name of the analog input to look up
   @param  nInputNameLen [in] The size of the analog input name in bytes
   @param  pAdcTMInputProps [out] The buffer to write the property values to

   @return DAL_SUCCESS if the input was found. DAL_SUCCESS if the input was
      found in the properties file.

      ADC_ERROR_INVALID_PROPERTY_LENGTH is returned if the
      property was incorrectly defined.

      All other values correspond to DAL error codes, and should
      generally be interpreted to mean that the analog input is
      not present on the target hardware.

*/
static __inline DALResult
DalAdc_TMGetInputProperties(DalDeviceHandle * _h, const char * pInputName, uint32 nInputNameLen, AdcInputPropertiesType *pAdcTMInputProps)
{
   return ADC_ERROR_TM_NOT_SUPPORTED;
}

/**
   @brief Gets the range of physical values that can be set for a given TM channel.

   This function gets the minimum and maximum physical value that can be set as a
   threshold for a given VADC TM channel.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcTMInputProps [in] The VADC TM channel obtained by calling DalAdc_TMGetInputProperties
   @param  pAdcTMRange [out] Structure with minimum and maximum physical values

   @return DAL_SUCCESS if the ranges were successfully determined.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcTMInputProps or pAdcTMRange were NULL
*/
static __inline DALResult
DalAdc_TMGetRange(DalDeviceHandle * _h, const AdcInputPropertiesType *pAdcTMInputProps, AdcTMRangeType *pAdcTMRange)
{
   return ADC_ERROR_TM_NOT_SUPPORTED;
}

/**
   @brief Sets a threshold with the ADC TM

   The threshold event will be triggered once when the threshold is crossed:
    - ADC_TM_THRESHOLD_LOWER: current reading <= *pnThresholdDesired
    - ADC_TM_THRESHOLD_HIGHER: current reading >= *pnThresholdDesired
   After the event is triggered, the threshold will not trigger the event again
   and will be in a triggered state until the client calls DalAdc_TMSetThreshold
   to set a new threshold.

   The event type can be:
    - DALSYS_EVENT_ATTR_NORMAL: the event will be triggered.
    - DALSYS_EVENT_ATTR_CALLBACK_EVENT: the callback will have a payload pointing
      to a AdcTMCallbackPayloadType struct.

   Note that thresholds can be disabled/re-enabled on a per client basis by calling
   DalAdc_TMSetEnableThresholds. Thresholds are enabled by default, but calling
   DalAdc_TMSetThreshold does not automatically re-enable them if they were previously
   disabled by a call to DalAdc_TMSetEnableThresholds.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcTMParams [in] Pointer to client's request parameters
           ->nDeviceIdx [in] device index from DalAdc_TMGetInputProperties
           ->nChannelIdx [in] channel index from DalAdc_TMGetInputProperties
           ->hEvent [in] Threshold event that gets set
   @param  eThreshold [in] Type of threshold to monitor
   @param  pnThresholdDesired [in] Threshold value to set in physical units; if NULL the
                                    current threshold is cleared.
   @param  pnThresholdSet [out] The actual threshold value that was set in physical units.

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the threshold was successfully set.

           Other return values indicate an error and include:
           ADC_ERROR_OUT_OF_TM_CLIENTS - there are no free TM client handles
           ADC_ERROR_TM_THRESHOLD_OUT_OF_RANGE - the requested threshold is out of range
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams, pAdcParams->hEvent, or pAdcResult was NULL

*/
static __inline DALResult
DalAdc_TMSetThreshold(DalDeviceHandle *_h, const AdcRequestParametersType *pAdcTMParams, AdcTMThresholdType eThreshold, const int32 *pnThresholdDesired, int32 *pnThresholdSet)
{
   return ADC_ERROR_TM_NOT_SUPPORTED;
}

/**
   @brief Enables / disables thresholds for the ADC TM

   By default, thresholds are enabled.

   Thresholds are not monitored while the thresholds are disabled,
   and any threshold crossings which occurred while the thresholds were disabled
   are ignored.

   Threshold values and event handles set by DalAdc_TMSetThreshold
   are still retained while thresholds are disabled.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  bEnable [in] TRUE: enable, FALSE: disable

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS on success.

           Other return values indicate an error and include:
           ADC_ERROR_OUT_OF_TM_CLIENTS - there are no free TM client handles

*/
static __inline DALResult
DalAdc_TMSetEnableThresholds(DalDeviceHandle * _h, DALBOOL bEnable)
{
   return ADC_ERROR_TM_NOT_SUPPORTED;
}

#endif

