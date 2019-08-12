#ifndef __DDIADCDEVICE_H__
#define __DDIADCDEVICE_H__
/*============================================================================
  @file DDIAdcDevice.h
 
  ADC Physical Device Driver Interface header

  This header is to be included solely by the ADC DAL and implementations of
  this DDI. It provides a common interface for the ADC DAL to interface with
  different ADC hardware.
  
  External clients may not access these interfaces.
  
                Copyright (c) 2008-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/common/protected/DDIAdcDevice.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DalDevice.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DALADCDEVICE_INTERFACE_VERSION DALINTERFACE_VERSION(2,0)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   ADC_DEVICE_CHANNEL_READY,
   ADC_DEVICE_CHANNEL_BUSY,
   ADC_DEVICE_CHANNEL_CALIBRATING,
   ADC_DEVICE_CHANNEL_ERROR,
   _ADC_MAX_DEVICE_CHANNEL_STATUS = 0x7FFFFFFF
} AdcDeviceChannelStatusType;

typedef enum 
{
   ADC_DEVICE_RESULT_VALID,
   ADC_DEVICE_RESULT_INVALID,
   ADC_DEVICE_RESULT_INVALID_CHANNEL,
   ADC_DEVICE_RESULT_TIMEOUT,
   ADC_DEVICE_RESULT_FIFO_NOT_EMPTY,
   _ADC_MAX_DEVICE_RESULT_STATUS = 0x7FFFFFFF
} AdcDeviceResultStatusType;

typedef struct
{
   uint32 uNumChannels;      /* number of ADC channels */
} AdcDeviceDevicePropertiesType;

typedef struct
{
   AdcDeviceResultStatusType eStatus;
   uint32 uChannelIdx;  /* the index of the channel which was recalibrated */
   int32 nPhysical1;    /* ref 1 in physical units. Units depend on BSP */
   int32 nPhysical2;    /* ref 2 in physical units. Units depend on BSP */
   uint32 uCode1;       /* raw ADC code for ref 1 */
   uint32 uCode2;       /* raw ADC code for ref 2 */
} AdcDeviceRecalibrateResultType;

typedef struct
{
   AdcDeviceResultStatusType eStatus;
   uint32 uChannelIdx;  /* the index of the channel which was sampled */
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 uPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 uMicrovolts;  /* result in microvolts */
   uint32 uCode;        /* raw ADC code */
} AdcDeviceResultType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
DALResult
GetInputProperties(DalDeviceHandle *h,
   /*in*/ const char *pChannelName,
   /*out*/ uint32 *puChannelIdx);

DALResult
GetDeviceProperties(DalDeviceHandle *h,
   /*out*/ AdcDeviceDevicePropertiesType *pAdcDeviceProp);

DALResult  
GetChannelStatus(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*out*/ AdcDeviceChannelStatusType *pAdcChannelStatus);

DALResult 
Calibrate(DalDeviceHandle *h,
   /*in*/ const DALSYSEventHandle hEvent,
   /*in*/ uint32 uEventParam);

DALResult 
RecalibrateChannel(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*in*/ const DALSYSEventHandle hEvent,
   /*in*/ uint32 uEventParam);

DALResult  
StartConversion(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*in*/ const DALSYSEventHandle hEvent,
   /*in*/ uint32 uEventParam);

DALResult 
RecalibrateChannelSync(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*out*/ AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult);

DALResult  
ReadChannelSync(DalDeviceHandle *h,
   /*in*/ uint32 uChannelIdx,
   /*out*/ AdcDeviceResultType *pAdcDeviceReadResult);

typedef struct DalAdcDevice DalAdcDevice;
struct DalAdcDevice
{
   DalDevice DalDevice;
   DALResult (*GetInputProperties)(DalDeviceHandle * _h, const char * pChannelName, uint32 *);
   DALResult (*GetDeviceProperties)(DalDeviceHandle * _h,  AdcDeviceDevicePropertiesType *pAdcDeviceProp);
   DALResult (*GetChannelStatus)(DalDeviceHandle * _h,  uint32  uChannelIdx,  AdcDeviceChannelStatusType * pAdcChannelStatus);
   DALResult (*Calibrate)(DalDeviceHandle * _h,  const DALSYSEventHandle  hEvent,  uint32  uEventParam);
   DALResult (*RecalibrateChannel)(DalDeviceHandle * _h,  uint32  uChannelIdx,  const DALSYSEventHandle  hEvent,  uint32  uEventParam);
   DALResult (*StartConversion)(DalDeviceHandle * _h,  uint32  uChannelIdx,  const DALSYSEventHandle  hEvent,  uint32  uEventParam);
   DALResult (*RecalibrateChannelSync)(DalDeviceHandle * _h,  uint32  uChannelIdx, AdcDeviceRecalibrateResultType *);
   DALResult (*ReadChannelSync)(DalDeviceHandle * _h,  uint32  uChannelIdx, AdcDeviceResultType *);
};

typedef struct DalAdcDeviceHandle DalAdcDeviceHandle; 
struct DalAdcDeviceHandle 
{
   uint32 dwDalHandleId;
   const DalAdcDevice * pVtbl;
   void * pClientCtxt;
};

/**
   @brief Attaches to the ADC physical device driver

   This function attaches the client to the physical ADC device driver.
   This function is used to obtain the device handle which is required to
   use the driver APIs.
   
   @param  pszDevName: [in] The device ID string to attach to
   @param  hDalDevice: [out] Pointer to DAL device handle pointer which will receive a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/
#define DAL_AdcDeviceDeviceAttach(pszDevName,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL, pszDevName, DALADCDEVICE_INTERFACE_VERSION, hDalDevice)

/**
   @brief Gets the number of channels defined for this device

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  pnNumChannels: [out] Pointer to result data
   @param  pAdcDeviceProp: [out] Pointer to result data

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_GetDeviceProperties(DalDeviceHandle * _h,  AdcDeviceDevicePropertiesType *pAdcDeviceProp)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetDeviceProperties( _h, pAdcDeviceProp);
}

/**
   Determine whether the channel is supported by the device.

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  puNumChannels: [out] Pointer to result data

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_GetInputProperties(DalDeviceHandle * _h, const char *pChannelName, uint32 *puChannelIdx)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetInputProperties( _h, pChannelName, puChannelIdx);
}

/**
   @brief Gets the status of the channel

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx: [in] The channel index
   @param  pAdcChannelStatus: [out] Pointer to result data

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_GetChannelStatus(DalDeviceHandle * _h,  uint32  uChannelIdx,  AdcDeviceChannelStatusType *pAdcChannelStatus)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetChannelStatus( _h, uChannelIdx, pAdcChannelStatus);
}

/**
   @brief Calibrates the device

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  hEvent: [in] An event to trigger when the calibration is complete

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_Calibrate(DalDeviceHandle * _h,  const DALSYSEventHandle  hEvent,  uint32  uEventParam)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->Calibrate( _h, hEvent, uEventParam);
}

/**
   @brief Recalibrates a specific channel

   This function is used to calibrate a specific channel. If multiple channels
   use the same configuration, then those channels will also be recalibrated.

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx: [in] The index of the channel to recalibrate
   @param  hEvent: [in] An event to trigger when the calibration is complete
   @param  uEventParam: [in] The value to use for dwParam when triggering hEvent

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_RecalibrateChannel(DalDeviceHandle * _h,  uint32  uChannelIdx,  const DALSYSEventHandle  hEvent,  uint32  uEventParam)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->RecalibrateChannel( _h, uChannelIdx, hEvent, uEventParam);
}

/**
   @brief Starts a conversion on the specified channel

   This function is used to start a conversion. The function will return
   immediately and will trigger hEvent when the conversion is complete.
   
   The conversion data is passed to the hEvent in the event payload as a
   AdcDeviceResultType struct.

   nEventParam is passed by the client to the driver. This value is passed
   as the dwParam to the hEvent. This is used by the client (the ADC driver)
   to link the conversion event to the initial request.

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx: [in] The index of the channel to recalibrate
   @param  hEvent: [in] An event to trigger when the calibration is complete
   @param  uEventParam: [in] The value to use for dwParam when triggering hEvent

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_StartConversion(DalDeviceHandle * _h,  uint32  uChannelIdx,  const DALSYSEventHandle  hEvent,  uint32  uEventParam)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->StartConversion( _h, uChannelIdx, hEvent, uEventParam);
}

/**
   @brief Recalibrates a specific channel synchronously

   This function is used to calibrate a specific channel. If multiple channels
   use the same configuration, then those channels will also be recalibrated.

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx: [in] The index of the channel to recalibrate
   @param  pAdcDeviceRecalibrateResult: [out] Recalibration result

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_RecalibrateChannelSync(DalDeviceHandle *_h, uint32 uChannelIdx, AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->RecalibrateChannelSync( _h, uChannelIdx, pAdcDeviceRecalibrateResult);
}

/**
   @brief Reads an ADC channel synchronously

   This function is used to read an ADC channel.

   @param  h: [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx: [in] The index of the channel to recalibrate
   @param  pAdcDeviceReadResult: [out] Read result

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.
           
*/
static __inline DALResult
DalAdcDevice_ReadChannelSync(DalDeviceHandle *_h, uint32 uChannelIdx, AdcDeviceResultType *pAdcDeviceReadResult)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->ReadChannelSync( _h, uChannelIdx, pAdcDeviceReadResult);
}

#endif
