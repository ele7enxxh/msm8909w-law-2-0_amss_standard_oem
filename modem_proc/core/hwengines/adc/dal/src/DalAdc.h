#ifndef DALAdc_H
#define DALAdc_H
/*============================================================================
  @file DalAdc.h
 
  Function and data structure declarations for ADC DAL
 
 
                Copyright (c) 2008-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/dal/src/DalAdc.h#1 $ */

#include "DALFramework.h"
#include "DDIAdc.h"
#include "DALQueue.h"

/*------------------------------------------------------------------------------
ADC BSP structure
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
Declaring a "Adc" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct AdcDrvCtxt AdcDrvCtxt;
typedef struct AdcDevCtxt AdcDevCtxt;
typedef struct AdcClientCtxt AdcClientCtxt;
typedef struct AdcDeviceDataType AdcDeviceDataType;

/*------------------------------------------------------------------------------
Declaring a private "Adc" Vtable
------------------------------------------------------------------------------*/
typedef struct AdcDALVtbl AdcDALVtbl;
struct AdcDALVtbl
{
  int (*Adc_DriverInit)(AdcDrvCtxt *);
  int (*Adc_DriverDeInit)(AdcDrvCtxt *);
};

typedef enum
{
   ADC_REQUEST_DEVICE_CALIBRATION,
   ADC_REQUEST_CONVERSION,
   ADC_REQUEST_CHANNEL_RECALIBRATION
} AdcDeviceRequestType;

typedef struct
{
   DALFW_Q_LinkType link;
   AdcDeviceRequestType adcDeviceRequest;
   DALSYSEventHandle hClientEvent;
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
   uint32 nToken;
   AdcRequestStatusType adcRequestStatus;
} AdcDeviceQueueNodeType;

typedef enum
{
  ADC_DEVICE_NOT_AVAILABLE,
  ADC_DEVICE_AVAILABLE
} AdcDeviceStatusType;


/*
 * AdcDeviceDataType:
 *
 * nNumChannels        - number of channels supported by the ADC device.
 * phTimerDev          - handle to the timer device.
 * minAdcReqInterval   - indicates how often the adc conversion request can be
 *                       sent to the driver.
 * nDeviceQueueSize    - Queue size for the device.
 * 
 */
struct AdcDeviceDataType
{
   DalDeviceHandle *phDevice;
   uint32 nNumChannels;
   DALSYSEventHandle hCalibrationCompleteEvent;
   DALSYS_EVENT_OBJECT(calibrationCompleteEventObject);
   uint32            nDeviceQueueSize;
   AdcDeviceQueueNodeType  *paNodes;
   DALFW_Q_Type qFree;
   DALFW_Q_Type qQueued;
   DALFW_Q_Type qPending;
   AdcDeviceStatusType deviceStatus;
   DALBOOL bDeviceIsOpen;
};

struct AdcDevCtxt
{
   //Base Members
   uint32   dwRefs;                                   
   DALDEVICEID DevId;                                  
   uint32   dwDevCtxtRefIdx; 
   AdcDrvCtxt  *pAdcDrvCtxt;                             
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   
   //Adc Dev state can be added by developers here
   DALSYSSyncHandle hSync;          /* synchronization object */
   DALSYS_SYNC_OBJECT(syncObject);
   const AdcBspType *pBsp;          /* DAL ADC BSP */
   AdcDeviceDataType *paDeviceData; /* device data array */
   DALSYSEventHandle  hConversionCompleteEvent;
   DALSYS_EVENT_OBJECT(conversionCompleteEventObject);
};

struct AdcDrvCtxt
{
   // Base Members
   AdcDALVtbl AdcDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   AdcDevCtxt AdcDevCtxt[1];
   // Adc Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Adc" Client Context
------------------------------------------------------------------------------*/
struct AdcClientCtxt
{
   // Base Members
   uint32  dwRefs;                     
   uint32  dwAccessMode;  
   void *pPortCtxt;
   AdcDevCtxt *pAdcDevCtxt;            
   DalAdcHandle DalAdcHandle; 
   // Adc Client state can be added by developers here
};

DALResult Adc_DriverInit(AdcDrvCtxt *);
DALResult Adc_DriverDeInit(AdcDrvCtxt *);
DALResult Adc_DeviceInit(AdcClientCtxt *);
DALResult Adc_DeviceDeInit(AdcClientCtxt *);
DALResult Adc_Reset(AdcClientCtxt *);
DALResult Adc_PowerEvent(AdcClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Adc_Open(AdcClientCtxt *, uint32);
DALResult Adc_Close(AdcClientCtxt *);
DALResult Adc_Info(AdcClientCtxt *,DalDeviceInfo *, uint32);
DALResult Adc_InheritObjects(AdcClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalAdc interface
------------------------------------------------------------------------------*/
DALResult Adc_GetAdcInputProperties( AdcClientCtxt *,  const char *,  uint32 ,  AdcInputPropertiesType *);
DALResult Adc_RequestConversion( AdcClientCtxt *,  AdcRequestParametersType *, AdcRequestStatusType *);
DALResult Adc_RequestRecalibration( AdcClientCtxt *,  AdcRequestParametersType *);
DALResult Adc_AddDeviceQueueRequest( AdcClientCtxt *, AdcDeviceRequestType, uint32, uint32, const DALSYSEventHandle, AdcRequestStatusType *);

#endif /*DALAdc_H*/

