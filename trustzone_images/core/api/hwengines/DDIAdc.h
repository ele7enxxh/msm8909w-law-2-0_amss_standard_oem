#ifndef __DDIADC_H__
#define __DDIADC_H__

/*============================================================================
  @file DDIAdc.h
 
  ADC Device Driver Interface header

  Clients may include this header and use these interfaces to read analog
  inputs.
  
               Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/DDIAdc.h#1 $ */

#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "AdcInputs.h"

#define DALADC_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)
#define ADC_BATCH_SAMPLE_INVALID (0x7FFFFFFF)

enum
{
   ADC_ERROR_INVALID_DEVICE_IDX = 1,
   ADC_ERROR_INVALID_CHANNEL_IDX,
   ADC_ERROR_NULL_POINTER,
   ADC_ERROR_DEVICE_QUEUE_FULL,
   ADC_ERROR_INVALID_PROPERTY_LENGTH,
   ADC_ERROR_REMOTE_EVENT_POOL_FULL,
   ADC_ERROR_BATCH_POOL_FULL,
   ADC_ERROR_OUT_OF_MEMORY,
   ADC_ERROR_API_UNSUPPORTED_IN_THIS_CONTEXT,
   ADC_ERROR_INVALID_TOKEN,
   ADC_ERROR_DEVICE_NOT_READY,
   ADC_ERROR_NOT_SUPPORTED = 0x7FFFFFFF
};

typedef enum
{
   ADC_RESULT_INVALID,
   ADC_RESULT_VALID,
   ADC_RESULT_NOT_SUPPORTED = 0x7FFFFFFF
} AdcResultStatusType;

typedef struct
{
   enum
   {
      ADC_REQUEST_STATUS_PENDING,   /* the request is being immediately performed */
      ADC_REQUEST_STATUS_QUEUED,    /* the request was queued */
      ADC_REQUEST_STATUS_ERROR,     /* the request was not started due to an error */
      ADC_REQUEST_STATUS_UNKNOWN = 0x7FFFFFFF
   } eStatus;
} AdcRequestStatusType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
   DALSYSEventHandle hEvent;
} AdcRequestParametersType;

typedef struct
{
   AdcResultStatusType eStatus;  /* status of the conversion */
   uint32 nToken;       /* token which identifies this conversion */
   uint32 nDeviceIdx;   /* the device index for this conversion */
   uint32 nChannelIdx;  /* the channel index for this conversion */
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 nPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 nMicrovolts;  /* result in microvolts */
   uint32 nReserved;    /* reserved for internal use */
} AdcResultType;

typedef struct
{
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
} AdcInputPropertiesType;

typedef enum
{
   ADC_BATCH_FORMAT_PHYSICAL = 0,   /* result in physical units. Units depend on BSP */
   ADC_BATCH_FORMAT_PERCENT,        /* result as percentage of reference voltage used
                                     * for conversion. 0 = 0%, 65535 = 100% */
   ADC_BATCH_FORMAT_MICROVOLTS,     /* result in microvolts */
   ADC_BATCH_FORMAT_NOT_SUPPORTED = 0x7FFFFFFF
} AdcBatchFormatType;

typedef struct
{
   uint32 nDeviceIdx;         /* the device index for the batch request */
   uint32 nChannelIdx;        /* the channel index for batch request */
   DALSYSEventHandle hEvent;  /* handle to callback event which will receive batch result data */
   AdcBatchFormatType eFormat;/* the format of the result type */
   uint32 nNumBatches;        /* the number of batches to make */
   uint32 nDurationUs;        /* the time to continuously sample in microseconds per batch
                                 or set = 0 to use nNumConversions */
   uint32 nNumConversions;    /* if nDurationUs = 0, the number of conversions to sample per patch
                                 or set = 0 to fill the buffer */
   uint32 nPeriodUs;          /* if nDurationUs = 0 and nNumConversions > 0, the period of time in
                                 microseconds to wait between each conversion result */
   uint32 isTimeStampRqd;     /* if isTimeStampRqd = TRUE, add time-stamp for every adc conversion. */
} AdcBatchParametersType;

typedef struct
{
   uint32 nToken;             /* token which identifies this batch */
} AdcBatchStatusType;

/*
 * Status of the batch request
 */
typedef enum {
      ADC_BATCH_STATUS_SUCCESS = 0,
      ADC_BATCH_STATUS_OVERFLOW,
      ADC_BATCH_STATUS_ERROR,
      ADC_BATCH_STATUS_UNKNOWN = 0x7FFFFFFF
} eStatus; 

typedef struct
{
   uint32 eStatus;           /* Status of the batch request          */
   uint32 nToken;            /* Token to identify this batch request */
   uint32 nSamples;          /* Number of samples in the result buffer aSamples */
   int32 aSamples[1];        /* Result buffer. Invalid samples are set to ADC_BATCH_SAMPLE_INVALID */
} AdcBatchResultType;

DALResult
GetAdcInputProperties(DalDeviceHandle *h,
   /*ibuf*/ const char *pInputName,
   /*ilen*/ uint32 nInputNameLen,
   /*ostruct*/ AdcInputPropertiesType *pAdcInputProps);

DALResult
RequestConversion(DalDeviceHandle *h,
   /*istruct*/ AdcRequestParametersType *pAdcParams,
   /*ostruct*/ AdcRequestStatusType *pAdcRequestStatus);

DALResult
RequestRecalibration(DalDeviceHandle *h,
   /*istruct*/ AdcRequestParametersType *pAdcParams);

DALResult
RequestBatch(DalDeviceHandle *h,
   /*istruct*/ AdcBatchParametersType *pParams,
   /*ostruct*/ AdcBatchStatusType *pStatus);

DALResult
StopBatch(DalDeviceHandle *h, uint32 nToken);

typedef struct DalAdc DalAdc;
struct DalAdc
{
   struct DalDevice DalDevice;
   DALResult (*RequestConversion)(DalDeviceHandle * _h, AdcRequestParametersType * pAdcParams, uint32 ilen1, AdcRequestStatusType * pAdcRequestStatus, uint32 olen2);
   DALResult (*RequestRecalibration)(DalDeviceHandle * _h, AdcRequestParametersType * pAdcParams, uint32 ilen1);
   DALResult (*GetAdcInputProperties)(DalDeviceHandle * _h, const char * pInputName,  uint32  nInputNameLen, AdcInputPropertiesType * pAdcInputProps, uint32 olen3);
   DALResult (*RequestBatch)(DalDeviceHandle * h,  AdcBatchParametersType * pParams, uint32 ilen1, AdcBatchStatusType * pStatus, uint32 olen2);
   DALResult (*StopBatch)(DalDeviceHandle * _h, uint32  nToken);
};

typedef struct DalAdcHandle DalAdcHandle; 
struct DalAdcHandle 
{
   uint32 dwDalHandleId;
   const DalAdc * pVtbl;
   void * pClientCtxt;
};


/**
   @brief Gets the properties for an analog input from the ADC DAL properties

   This function looks up the properties for an analog input.
   
   @param  hAdcProp: [in] The handle of the properties file
   @param  pInputName: [in] The name of the analog input to look up
   @param  pAdcInputProps: [out] The buffer to write the property values to

   @return DAL_SUCCESS if the input was found. DAL_SUCCESS if the input was
      found in the properties file.
 
      ADC_ERROR_INVALID_PROPERTY_LENGTH is returned if the
      property was incorrectly defined.
 
      All other values correspond to DAL error codes, and should
      generally be interpreted to mean that the analog input is
      not present on the target hardware.
           
*/
static __inline DALResult DalAdc_GetInputProperties(
                                DALSYSPropertyHandle hAdcProp,
                                const char *pInputName,
                                AdcInputPropertiesType *pAdcInputProps)
{
   DALSYSPropertyVar propInput;
   DALResult result;

   result = DALSYS_GetPropertyValue(hAdcProp, pInputName, 0, &propInput);

   if(result != DAL_SUCCESS)
   {
      return result;
   }

   if(propInput.dwLen != 2)
   {
      return ADC_ERROR_INVALID_PROPERTY_LENGTH;
   }

   if(pAdcInputProps != NULL)
   {
      pAdcInputProps->nDeviceIdx = propInput.Val.pdwVal[0];
      pAdcInputProps->nChannelIdx = propInput.Val.pdwVal[1];
   }

   return DAL_SUCCESS;
}

/**
   @brief Attaches to the ADC driver

   This function attaches the client to the ADC driver. This function is used
   to obtain the device handle which is required to use the ADC driver APIs.
   
   @param  DevId: [in] The device ID to attach to (e.g., DALDEVICEID_ADC)
   @param  hDalDevice: [out] Pointer to DAL device handle pointer which will receive
                             a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/
#define DAL_AdcDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALADC_INTERFACE_VERSION,hDalDevice)

/**
   @brief Gets the properties for an analog input from the ADC DAL properties

   This function looks up the properties for an analog input.
   
   @param  h: [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pInputName: [in] The name of the analog input to look up
   @param  nInputNameSize: [in] The size of the analog input name in bytes
   @param  pAdcInputProps: [out] The buffer to write the property values to

   @return DAL_SUCCESS if the input was found. DAL_SUCCESS if the input was
      found in the properties file.
 
      ADC_ERROR_INVALID_PROPERTY_LENGTH is returned if the
      property was incorrectly defined.
 
      All other values correspond to DAL error codes, and should
      generally be interpreted to mean that the analog input is
      not present on the target hardware.
           
*/
static __inline DALResult
DalAdc_GetAdcInputProperties(DalDeviceHandle * _h,  const char * pInputName,  uint32 nInputNameSize,  AdcInputPropertiesType * pAdcInputProps)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_8(DALVTBLIDX(((DalAdcHandle *)_h)->pVtbl, GetAdcInputProperties ), _h, (void *)pInputName, nInputNameSize, (void * )pAdcInputProps, sizeof( AdcInputPropertiesType ));
   }
   return ((DalAdcHandle *)_h)->pVtbl->GetAdcInputProperties( _h, pInputName, nInputNameSize, pAdcInputProps, sizeof( AdcInputPropertiesType ));
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
  
   @param  h: [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams: [in] Pointer to client's request parameters
   @param  pAdcRequestStatus: [out] Pointer to buffer which will indicate the status of request
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
DalAdc_RequestConversion(DalDeviceHandle * _h,  AdcRequestParametersType * pAdcParams,  AdcRequestStatusType * pAdcRequestStatus)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_8(DALVTBLIDX(((DalAdcHandle *)_h)->pVtbl, RequestConversion ), _h, pAdcParams, sizeof( AdcRequestParametersType ), pAdcRequestStatus, sizeof( AdcRequestStatusType ));
   }
   return ((DalAdcHandle *)_h)->pVtbl->RequestConversion( _h, pAdcParams, sizeof( AdcRequestParametersType ), pAdcRequestStatus, sizeof( AdcRequestStatusType ));
}


/**
   @brief Calibrates an analog input

   This function requests the ADC driver to recalibrate the specified
   channel. If other channels share the physical device configuration
   (as defined in the ADC device BSP), then those channnels may also be
   updated by the recalibration.
   
   @param  h: [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pAdcParams: [in] Pointer to client's request parameters

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams was NULL
           ADC_ERROR_DEVICE_QUEUE_FULL - the device request queue was full
           
*/
static __inline DALResult
DalAdc_RequestRecalibration(DalDeviceHandle * _h,  AdcRequestParametersType * pAdcParams)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalAdcHandle *)_h)->pVtbl, RequestRecalibration ), _h, pAdcParams, sizeof( AdcRequestParametersType ));
   }
   return ((DalAdcHandle *)_h)->pVtbl->RequestRecalibration( _h, pAdcParams, sizeof( AdcRequestParametersType ));
}

/**
   @brief Starts multiple conversions on a channel

   This function requests the ADC driver to continuosly or periodically sample
   a specified ADC channel. An internal buffer is filled with the samples until
   a batch is complete. After each batch, the client callback event is triggered
   with a AdcBatchResultType as the payload data. AdcBatchResultType contains
   a buffer with the sample results.

   This API is unsupported in the boot.
      
   @param  h: [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  pParams: [in] Pointer to client's request parameters
   @param  pStatus: [out] Pointer to the status of the batch request

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_DEVICE_IDX - the client specified an invalid device index
           ADC_ERROR_INVALID_CHANNEL_IDX - the client specified an invalid channel index
           ADC_ERROR_NULL_POINTER - pAdcParams was NULL
           ADC_ERROR_BATCH_POOL_FULL - too many batch requests are being processed
           ADC_ERROR_OUT_OF_MEMORY - memory could not be allocated for the sample buffer
           ADC_ERROR_API_UNSUPPORTED_IN_THIS_CONTEXT - this API was called in the boot
           
*/
static __inline DALResult
DalAdc_RequestBatch(DalDeviceHandle * _h,  AdcBatchParametersType * pParams,  AdcBatchStatusType * pStatus)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_8(DALVTBLIDX(((DalAdcHandle *)_h)->pVtbl, RequestBatch ), _h, pParams, sizeof( AdcBatchParametersType ), pStatus, sizeof( AdcBatchStatusType ));
   }
   return ((DalAdcHandle *)_h)->pVtbl->RequestBatch( _h, pParams, sizeof( AdcBatchParametersType ), pStatus, sizeof( AdcBatchStatusType ));
}

/**
   @brief Stops the batch request with the specified token

   This function stops the batch request with the specified token.
   Conversion requests due to the associated batch will no longer
   occur and the client event associated with this batch request will no longer be triggered.
   
   After this API is called on a batch request, outstanding conversions currently pending in
   the ADC Queue as a result of the batch request will continue to internally be processed,
   however the client event will not be triggered. The ADC driver will keep the batch
   request locked in the pool until outstanding ADC conversions have completed at which
   point the request and corresponding token will be released.

   This API is unsupported in the boot.
      
   @param  h: [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceAttach
   @param  nToken: [in] Token which identifies this batch request

   @see    DAL_AdcDeviceAttach

   @return DAL_SUCCESS if the request was successfully queued.

           Other return values indicate an error and include:
           ADC_ERROR_INVALID_TOKEN - the client specified an invalid token
*/
static __inline DALResult
DalAdc_StopBatch(DalDeviceHandle * _h, uint32  nToken)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalAdcHandle *)_h)->pVtbl, StopBatch ), _h, nToken);
   }
   return ((DalAdcHandle *)_h)->pVtbl->StopBatch( _h, nToken);
}
#endif
