/*============================================================================
  FILE:         DALAdc.c

  OVERVIEW:     Implementation of the ADC DAL

  DEPENDENCIES: None
 
                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/dal/src/DalAdc.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-19  jjo  Platform subtype check for 8974.
  2012-06-13  jjo  Updated to use DAL device ID string names.
  2012-04-19  jdt  Updated for 8974: Removed remote support. Removed batch APIs.
  2009-03-16  jdt  Initial revision.

============================================================================*/
#include "DalAdc.h"
#include "DDIAdcDevice.h"
#include "timetick.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"

#define ADC_LOG_MAX 30

struct
{
   struct {
      enum {
         ADC_LOG_EVENT_REQUEST_CONVERSION = 1,
         ADC_LOG_EVENT_REQUEST_RECALIBRATION,
         ADC_LOG_EVENT_CONVERSION_COMPLETE,
         ADC_LOG_EVENT_CALIBRATION_COMPLETE
      } eEvent;
      uint32 nTimestamp;
      uint32 nDeviceIdx;
      uint32 nChannelIdx;
      DALSYSEventHandle hClientEvent;
   } aLog[ADC_LOG_MAX];
   uint32 nCurIdx;
} gAdcDebug;

DALSYSSyncHandle     ghAdcDebugSync;
DALSYS_SYNC_OBJECT(ghAdcDebugSyncObject);


/*------------------------------------------------------------------------------
Following functions are internal to the module. 
------------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        Adc_LogConversionRequest

  DESCRIPTION     This function logs a conversion request
           
  DEPENDENCIES  	None

  PARAMETERS      nDeviceIdx    the device index
                  nChannelIdx   the channel index

  RETURN VALUE   	None
                  

  SIDE EFFECTS   	None

======================================================================*/
static void Adc_LogConversionRequest(uint32 nDeviceIdx, uint32 nChannelIdx)
{
   DALSYS_SyncEnter(ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.nCurIdx].eEvent = ADC_LOG_EVENT_REQUEST_CONVERSION;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nTimestamp = timetick_get();
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nDeviceIdx = nDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nChannelIdx = nChannelIdx;

   if (gAdcDebug.nCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.nCurIdx++;
   }
   else
   {
      gAdcDebug.nCurIdx = 0;
   }
   DALSYS_SyncLeave(ghAdcDebugSync);
}

static void Adc_LogRecalibrationRequest(uint32 nDeviceIdx, uint32 nChannelIdx)
{
   DALSYS_SyncEnter(ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.nCurIdx].eEvent = ADC_LOG_EVENT_REQUEST_RECALIBRATION;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nTimestamp = timetick_get();
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nDeviceIdx = nDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nChannelIdx = nChannelIdx;

   if (gAdcDebug.nCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.nCurIdx++;
   }
   else
   {
      gAdcDebug.nCurIdx = 0;
   }
   DALSYS_SyncLeave(ghAdcDebugSync);
}

static void Adc_LogConversionComplete(uint32 nDeviceIdx, uint32 nChannelIdx, const DALSYSEventHandle hClientEvent)
{
   DALSYS_SyncEnter(ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.nCurIdx].eEvent = ADC_LOG_EVENT_CONVERSION_COMPLETE;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nTimestamp = timetick_get();
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nDeviceIdx = nDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nChannelIdx = nChannelIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].hClientEvent = hClientEvent;

   if (gAdcDebug.nCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.nCurIdx++;
   }
   else
   {
      gAdcDebug.nCurIdx = 0;
   }
   DALSYS_SyncLeave(ghAdcDebugSync);
}

static void Adc_LogCalibrationComplete(uint32 nDeviceIdx, uint32 nChannelIdx, const DALSYSEventHandle hClientEvent)
{
   DALSYS_SyncEnter(ghAdcDebugSync);
   gAdcDebug.aLog[gAdcDebug.nCurIdx].eEvent = ADC_LOG_EVENT_CALIBRATION_COMPLETE;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nTimestamp = timetick_get();
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nDeviceIdx = nDeviceIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].nChannelIdx = nChannelIdx;
   gAdcDebug.aLog[gAdcDebug.nCurIdx].hClientEvent = hClientEvent;

   if (gAdcDebug.nCurIdx < ADC_LOG_MAX - 1)
   {
      gAdcDebug.nCurIdx++;
   }
   else
   {
      gAdcDebug.nCurIdx = 0;
   }
   DALSYS_SyncLeave(ghAdcDebugSync);
}

/*======================================================================

  FUNCTION        GetNodeFromToken

  DESCRIPTION     This function returns the node pointer that
                  corresponds to the given result token
           
  DEPENDENCIES  	None

  PARAMETERS      pDevCtxt       pointer to the current device context
                  nResultToken   the token which identifies this node

  RETURN VALUE   	Pointer to the corresponding node if the token is valid.
                  Otherwise, NULL.
                  

  SIDE EFFECTS   	None

======================================================================*/
static AdcDeviceQueueNodeType * GetNodeFromToken(const AdcDevCtxt *pDevCtxt,
                                                 uint32 nResultToken)
{
   uint32 nDeviceIdx = nResultToken >> 16;
   uint32 nNodeIdx = (nResultToken & 0xFFFF);

   AdcDeviceQueueNodeType *pNode =
      &pDevCtxt->paDeviceData[nDeviceIdx].paNodes[nNodeIdx];

   if (pNode->nToken != nResultToken)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
         "Adc conversion complete callback got an invalid token ID");
      return NULL;
   }
   else
   {
      return pNode;
   }
}

/*======================================================================

  FUNCTION        CalcToken

  DESCRIPTION     This function calculates a device queue node token
                  (result token) given the device index and node index.
           
  DEPENDENCIES  	None

  PARAMETERS      nDeviceIdx  the index of the device queue in which the
                              node belongs to
                  nNodeIdx    the node index

  RETURN VALUE   	A token value which is a uniquely identifies
                  the node in the device queue.
                  
  SIDE EFFECTS   	None

======================================================================*/
static uint32 CalcToken(uint32 nDeviceIdx, uint32 nNodeIdx)
{
   return ((nDeviceIdx << 16) | nNodeIdx);
}

/*======================================================================

  FUNCTION        Adc_ProcessDeviceQueue

  DESCRIPTION     This function processes any requests that are pending
                  in the device queue.
           
  DEPENDENCIES  	Must be called within the device mutex lock
                     i.e. inside DALSYS_SyncEnter(pDevCtxt->hSync)

  PARAMETERS      pDevCtxt    pointer to the ADC device context
                  nDeviceIdx  the index of the device queue to process

  RETURN VALUE   	None
                  
  SIDE EFFECTS   	None

======================================================================*/
static void Adc_ProcessDeviceQueue(const AdcDevCtxt *pDevCtxt, uint32 nDeviceIdx)
{
   AdcDeviceChannelStatusType adcChannelStatus;
   AdcDeviceQueueNodeType *pNode, *pNextNode;
   AdcDeviceDataType *pDeviceData;

   /* Get the pointer to the device data corresponding to this device index */
   pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];

   /* Find the next item in the list of queued requests */
   pNextNode = (AdcDeviceQueueNodeType *)DALFW_Q_Check(&pDeviceData->qQueued);

   if (pNextNode == NULL)
   {
      if ((DALFW_Q_Check(&pDeviceData->qPending) == NULL) && (pDeviceData->bDeviceIsOpen == TRUE))
      {
         /* There are no more requests to issue to the devices. Close the device to save power. */
         DalDevice_Close(pDeviceData->phDevice);
         pDeviceData->bDeviceIsOpen = FALSE;
      }
   }
   else
   {
      do
      {
         /* If there is a request in the queue, then check to see if
          * the physical device is ready to service the request */
         pNode = pNextNode;
         pNextNode = (AdcDeviceQueueNodeType *)DALFW_Q_Next(&pDeviceData->qQueued,
                                                      (DALFW_Q_LinkType*)pNode);
         (void)DalAdcDevice_GetChannelStatus(pDeviceData->phDevice,
                                       pNode->nChannelIdx,
                                       &adcChannelStatus);
         if (adcChannelStatus == ADC_DEVICE_CHANNEL_READY)
         {
            if (pDeviceData->bDeviceIsOpen == FALSE)
            {
               /* A request is ready to be sent to the device now, so open the device. */
               DalDevice_Open(pDeviceData->phDevice, DAL_OPEN_EXCLUSIVE);
               pDeviceData->bDeviceIsOpen = TRUE;
            }
   
            /* The physical device can service the request,
             * so remove the request from the list of queued requests
             * and place it in the pending (in progress) list */
            DALFW_Q_Delete(&pDeviceData->qQueued, (DALFW_Q_LinkType*)pNode);
            DALFW_Q_Put(&pDeviceData->qPending, (DALFW_Q_LinkType *)pNode);
            pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_PENDING;
   
            switch(pNode->adcDeviceRequest)
            {
            case ADC_REQUEST_CONVERSION:
               /* The request is a conversion request, so request the
                * physical device to perform the conversion. */
   
               /* When the request is completed, the request node
                * will be placed in the list of completed requests by
                * the internal event hConversionCompleteEvent. The node
                * will not be placed back into the free queue until the
                * client calls GetResult. */
               (void)DalAdcDevice_StartConversion(pDeviceData->phDevice,
                  pNode->nChannelIdx,
                  pDevCtxt->hConversionCompleteEvent,
                  pNode->nToken);
               break;
   
            case ADC_REQUEST_CHANNEL_RECALIBRATION:
               /* The request is a calibration request, so request the
                * physical device to perform the calibration. */
   
               /* When the calibration is completed, the request node
                * will be released back into the list of free nodes by the
                * internal event hCalibrationCompleteEvent.
                */
               (void)DalAdcDevice_RecalibrateChannel(
                  pDeviceData->phDevice,
                  pNode->nChannelIdx,
                  pDeviceData->hCalibrationCompleteEvent,
                  pNode->nToken);
               break;
   
            case ADC_REQUEST_DEVICE_CALIBRATION:
               (void)DalAdcDevice_Calibrate(
                  pDeviceData->phDevice,
                  pDeviceData->hCalibrationCompleteEvent,
                  pNode->nToken);
               break;
   
            default:
               break;
            }
         }
      } while (pNextNode != NULL);
   }
}


/*======================================================================

  FUNCTION        Adc_AddDeviceQueueRequest

  DESCRIPTION     This function adds a conversion or recalibration
                  request to the device queue.
           
  DEPENDENCIES  	None

  PARAMETERS     
      pDevCtxt          [in] pointer to the ADC device context
      adcDeviceRequest  [in] which kind of request to add
      nDeviceIdx        [in] index to the ADC device
      nChannelIdx       [in] index to the ADC channel
      hEvent            [in] event to signal when the request is completed
                             set hEvent = NULL to block on the completion
      pAdcResult        [in] pointer to the result data
                             (note that pAdcResult will only be updated with
                              final result data if hEvent = NULL)


  RETURN VALUE   	DAL_SUCCESS if request was added to the queue
                        ADC_DEVICE_QUEUE_FULL if the queue was full
                  
  SIDE EFFECTS   	None

======================================================================*/
DALResult Adc_AddDeviceQueueRequest(
                     AdcClientCtxt *pCtxt,
                     AdcDeviceRequestType adcDeviceRequest,
                     uint32 nDeviceIdx,
                     uint32 nChannelIdx,
                     const DALSYSEventHandle hClientEvent,
                     AdcRequestStatusType *pAdcRequestStatus)
{
   AdcDeviceQueueNodeType *pNode;
   DALResult ret = DAL_SUCCESS;
   AdcDeviceDataType *pDeviceData;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   DALSYS_SyncEnter(pDevCtxt->hSync);
   /* Get the pointer to the device data corresponding to this device index */
   pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];

   /* Get a node from the pool of free nodes */
   pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];
   pNode = DALFW_Q_Get(&pDeviceData->qFree);
   if (pNode != NULL)
   {
      pNode->hClientEvent = hClientEvent;

      /* A free node is available, so copy the request data into the free node
       * and move the node from the list of free nodes to the request queue.
       */
      pNode->nDeviceIdx = nDeviceIdx;
      pNode->nChannelIdx = nChannelIdx;
      pNode->adcDeviceRequest = adcDeviceRequest;
      pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_QUEUED;

      DALFW_Q_Put(&pDeviceData->qQueued, (DALFW_Q_LinkType *)pNode);

      Adc_ProcessDeviceQueue(pDevCtxt, nDeviceIdx);

      if (pAdcRequestStatus != NULL)
      {
         pAdcRequestStatus->eStatus = pNode->adcRequestStatus.eStatus;
      }
   }
   else
   {
      if (pAdcRequestStatus != NULL)
      {
         pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_ERROR;
      }

      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                      "Adc device request queue is full");
      ret = ADC_ERROR_DEVICE_QUEUE_FULL;
   }
   DALSYS_SyncLeave(pDevCtxt->hSync);

   return ret;
}

/*======================================================================

  FUNCTION        Adc_ConversionCompleteCb

  DESCRIPTION     This function processes the end of conversion event.

  DEPENDENCIES  	None

  PARAMETERS     
      pDevCtxt          [in] pointer to the ADC device context
      dwParam           [in] the dwParam parameter (result token)
      pPayload          [in] payload data (device result data)
      dwPayloadSize     [in] size of the payload data

  RETURN VALUE   	None
                  
  SIDE EFFECTS   	None

======================================================================*/
static void Adc_ConversionCompleteCb(AdcDevCtxt *pDevCtxt,
  				     uint32 dwParam,
				     void * pPayload,
				     uint32 dwPayloadSize)
{
   AdcDeviceQueueNodeType *pNode;
   AdcDeviceDataType *pDeviceData;
   uint32 nDeviceIdx;
   uint32 nChannelIdx;
   uint32 nResultToken = dwParam;
   AdcDeviceResultType *pDeviceResult = pPayload;
   AdcResultType adcResult;

   pNode = GetNodeFromToken(pDevCtxt, nResultToken);
   if (pNode != NULL)
   {
      nDeviceIdx = pNode->nDeviceIdx;
      nChannelIdx = pNode->nChannelIdx;

      if (nChannelIdx != pDeviceResult->uChannelIdx)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
            "Adc driver unexpected internal error: channel index mismatch");
      }

      /* Set the result status and data */
      if (pDeviceResult->eStatus == ADC_DEVICE_RESULT_VALID)
      {
         adcResult.eStatus = ADC_RESULT_VALID;
      }
      else if (pDeviceResult->eStatus == ADC_DEVICE_RESULT_TIMEOUT)
      {
         adcResult.eStatus = ADC_RESULT_TIMEOUT;
      }
      else if (pDeviceResult->eStatus == ADC_DEVICE_RESULT_FIFO_NOT_EMPTY)
      {
         adcResult.eStatus = ADC_RESULT_FIFO_NOT_EMPTY;
      }
      else
      {
         adcResult.eStatus = ADC_RESULT_INVALID;
      }
      adcResult.nDeviceIdx = nDeviceIdx;
      adcResult.nChannelIdx = nChannelIdx;
      adcResult.nToken = nResultToken;
      adcResult.nPhysical = pDeviceResult->nPhysical;
      adcResult.nPercent = pDeviceResult->uPercent;
      adcResult.nMicrovolts = pDeviceResult->uMicrovolts;
      adcResult.nCode = pDeviceResult->uCode;

      /* Get the device and channel index and data */
      pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];

      Adc_LogConversionComplete(adcResult.nDeviceIdx, adcResult.nChannelIdx, pNode->hClientEvent);

      /* notify the client that the conversion is complete */
      (void)DALSYS_EventCtrlEx(
         /* hEvent = */pNode->hClientEvent,
         /* dwCtrl = */DALSYS_EVENT_CTRL_TRIGGER,
         /* dwParam = */nResultToken,
         /* pPayload = */&adcResult,
         /* dwPayloadSize = */sizeof(adcResult)
      );

      DALSYS_SyncEnter(pDevCtxt->hSync);
      pNode->adcRequestStatus.eStatus = ADC_REQUEST_STATUS_UNKNOWN;
      /* Remove node from pending queue and place it back into the free list */
      DALFW_Q_Delete(&pDeviceData->qPending, (DALFW_Q_LinkType *)pNode);
      DALFW_Q_Put(&pDeviceData->qFree, (DALFW_Q_LinkType *)pNode);

      Adc_ProcessDeviceQueue(pDevCtxt, nDeviceIdx);

      DALSYS_SyncLeave(pDevCtxt->hSync);
   }
   else
   {
      DALSYS_LogEvent(pDevCtxt->DevId, 
                        DALSYS_LOGEVENT_FATAL_ERROR, 
                        "Adc driver got a NULL node pointer");
   }
}

/*======================================================================

  FUNCTION        Adc_CalibrationCompleteCb

  DESCRIPTION     This function processes the end of calibration event.
                  
  DEPENDENCIES  	None

  PARAMETERS     
      pDevCtxt          [in] pointer to the ADC device context
      dwParam           [in] the dwParam parameter (result token)
      pPayload          [in] payload data (unused)
      dwPayloadSize     [in] size of the payload data

  RETURN VALUE   	None
                  
  SIDE EFFECTS   	None

======================================================================*/
static void Adc_CalibrationCompleteCb(AdcDevCtxt *pDevCtxt,
                                      uint32 dwParam,
                                      void *pPayload,
                                      uint32 dwPayloadSize)
{
   AdcDeviceQueueNodeType *pNode;
   AdcDeviceDataType *pDeviceData;
   uint32 nResultToken = dwParam;

   pNode = GetNodeFromToken(pDevCtxt, nResultToken);
   if (pNode != NULL)
   {
      pDeviceData = &pDevCtxt->paDeviceData[pNode->nDeviceIdx];

      Adc_LogCalibrationComplete(pNode->nDeviceIdx, pNode->nChannelIdx, pNode->hClientEvent);

      /* notify the client that the calibration is complete */
      (void)DALSYS_EventCtrlEx(
         /* hEvent = */pNode->hClientEvent,
         /* dwCtrl = */DALSYS_EVENT_CTRL_TRIGGER,
         /* dwParam = */0,
         /* pPayload = */NULL,
         /* dwPayloadSize = */0
      );

      DALSYS_SyncEnter(pDevCtxt->hSync);
      /* remove node from the pending list and place it back into the free list */
      DALFW_Q_Delete(&pDeviceData->qPending, (DALFW_Q_LinkType *)pNode);
      DALFW_Q_Put(&pDeviceData->qFree, (DALFW_Q_LinkType *)pNode);

      Adc_ProcessDeviceQueue(pDevCtxt, pNode->nDeviceIdx);

      DALSYS_SyncLeave(pDevCtxt->hSync);
   }
   else
   {
      DALSYS_LogEvent(pDevCtxt->DevId, 
                        DALSYS_LOGEVENT_FATAL_ERROR, 
                        "Adc driver got a NULL node pointer");
   }
}

/*======================================================================

  FUNCTION        Adc_CalibrateDevices

  DESCRIPTION     This function is called at device initialization to
                  calibrate all of the ADC Physical Devices.
                  
  DEPENDENCIES  	None

  PARAMETERS     
      pDevCtxt          [in] pointer to the ADC device context

  RETURN VALUE   	None
                  
  SIDE EFFECTS   	This function blocks on the calibration events,
                  so it can take several milliseconds to complete
                  depending on the number of ADC Phyiscal Devices and
                  the time it takes to perform the calibration for
                  each device.

======================================================================*/
static DALResult Adc_CalibrateDevices(AdcClientCtxt *pCtxt)
{
   uint32 nDeviceIdx;
   DALResult result = DAL_SUCCESS;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   for(nDeviceIdx = 0; nDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; nDeviceIdx++)
   {
      if (pDevCtxt->paDeviceData[nDeviceIdx].deviceStatus == ADC_DEVICE_NOT_AVAILABLE)
      {
        continue;
      }

      result = Adc_AddDeviceQueueRequest(pCtxt,
                     ADC_REQUEST_DEVICE_CALIBRATION,
                     nDeviceIdx,
                     0,
                     NULL,
                     NULL);

      if (result != DAL_SUCCESS)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                         "Adc could not start calibration");
         break;
      }
   }
   return result;
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult 
Adc_DriverInit(AdcDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
Adc_DriverDeInit(AdcDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/
DALResult 
Adc_DeviceInit(AdcClientCtxt *pCtxt)
{
   uint32 nDeviceIdx;
   uint32 nNodeIdx, nDeviceQueueSize;
   DALResult result = DAL_SUCCESS;
   DALSYSPropertyVar propVar;
   AdcDeviceDevicePropertiesType AdcDeviceProp;
   AdcDeviceDataType *pDeviceData;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;
   DalDeviceHandle *phPlatformInfoDevice;
   DalChipInfoFamilyType eChipInfoFamilyType;
   DalPlatformInfoPlatformInfoType platformInfo;
   const char *pszAdcBspPropName = "ADC_BSP";

   /* initialize variables to 0 */
   pDevCtxt->hSync = NULL;
   pDevCtxt->paDeviceData = NULL;
   pDevCtxt->hConversionCompleteEvent = NULL;
   gAdcDebug.nCurIdx = 0;

   /* Get the chip family */
   eChipInfoFamilyType = DalChipInfo_ChipFamily();

   if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MSM8974_PRO)
   {
      /* Get the platform info */
      result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
      if (result != DAL_SUCCESS)
      {
         return result;
      }

      result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
      if (result != DAL_SUCCESS)
      {
         return result;
      }

      if (platformInfo.subtype == 0x01)
      {
         /* 8974PRO_PMA8084_PM8941 */
         pszAdcBspPropName = "ADC_BSP_8974PRO_PMA8084_PM8941";
      }
   }

   /* Read the DAL properties */
   result = DALSYS_GetDALPropertyHandle(pDevCtxt->DevId, pDevCtxt->hProp);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                    "Could not open ADC DAL properties");
      return result;
   }

   result = DALSYS_GetPropertyValue(pDevCtxt->hProp, pszAdcBspPropName, 0, &propVar);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "BSP not found in ADC DAL properties");
      return result;
   }

   pDevCtxt->pBsp = (AdcBspType *)propVar.Val.pStruct;

   /* Initialize synchronization object */
   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                              &ghAdcDebugSync,
                              &ghAdcDebugSyncObject);

   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not create log synchronization object");
      return result;
   }
   
   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                              &pDevCtxt->hSync,
                              &pDevCtxt->syncObject);

   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not create device synchronization object");
      return result;
   }

   /* Allocate memory for the device data */
   result = DALSYS_Malloc(
      sizeof(AdcDeviceDataType) * pDevCtxt->pBsp->uNumPhysicalDevices,
      (void **)&pDevCtxt->paDeviceData);

   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not allocate memory for device data");
      return result;
   }

   /*
    * Initialize the device.
    */
   for (nDeviceIdx = 0; nDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; nDeviceIdx++)
   {
      /* Get the pointer to the device data corresponding to this device */
      pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];

      /* Initialize all of device data to 0 */
      DALSYS_memset(pDeviceData, 0, sizeof(AdcDeviceDataType));

      /* Flag to indicate device hardware resouraces are OFF. */
      pDeviceData->bDeviceIsOpen = FALSE;

      /*
       * Attach to the ADC device. The attach can fail when the device is
       * not physically present. In this case, we just try and attach to the
       * next available device.
       */
      result = DAL_AdcDeviceDeviceAttach(
                     pDevCtxt->pBsp->paAdcPhysicalDevices[nDeviceIdx].pszDevName,
                     &pDeviceData->phDevice);

      if ((result == DAL_SUCCESS) && (pDeviceData->phDevice != NULL))
      {
         pDeviceData->deviceStatus = ADC_DEVICE_AVAILABLE;
      }
      else
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                         "Adc failed attaching to a physical device");

         pDeviceData->deviceStatus = ADC_DEVICE_NOT_AVAILABLE;
      }

      if (pDeviceData->phDevice == NULL)
      {
         continue;
      }

      /* Get the number of channels for each device */
      result = DalAdcDevice_GetDeviceProperties(pDeviceData->phDevice,&AdcDeviceProp);
      if (result != DAL_SUCCESS)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                         "Error obtaining the number of ADC device channels");
         return result;
      }

      pDeviceData->nNumChannels = AdcDeviceProp.uNumChannels;

      /* initialize the device queue */
      (void)DALFW_Q_Init(&pDeviceData->qFree);
      (void)DALFW_Q_Init(&pDeviceData->qQueued);
      (void)DALFW_Q_Init(&pDeviceData->qPending);

      /* allocate the device queue nodes */
      nDeviceQueueSize = pDevCtxt->pBsp->paAdcPhysicalDevices[nDeviceIdx].uQueueSize;
      result = DALSYS_Malloc(
         sizeof(AdcDeviceQueueNodeType) * nDeviceQueueSize,
         (void **)&pDeviceData->paNodes);

      if (result != DAL_SUCCESS)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
            "Adc could not allocate memory for device queue nodes");
         return result;
      }

      pDeviceData->nDeviceQueueSize = nDeviceQueueSize;
      for(nNodeIdx = 0; nNodeIdx < nDeviceQueueSize; nNodeIdx++)
      {
         /* initialize all of node data to 0 */
         DALSYS_memset(&pDeviceData->paNodes[nNodeIdx], 0,
                        sizeof(AdcDeviceQueueNodeType));

         pDeviceData->paNodes[nNodeIdx].adcRequestStatus.eStatus = ADC_REQUEST_STATUS_UNKNOWN;

         /* set the token values for each node */
         pDeviceData->paNodes[nNodeIdx].nToken =
                                       CalcToken(nDeviceIdx, nNodeIdx);

         DALFW_Q_Put(&pDeviceData->qFree,
            (DALFW_Q_LinkType *)&pDeviceData->paNodes[nNodeIdx]);
      }

      /* create the calibration complete event */
      result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT,
         &pDeviceData->hCalibrationCompleteEvent,
         &pDeviceData->calibrationCompleteEventObject);

      if (result != DAL_SUCCESS)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                         "Adc could not create the calibration complete event");
         return result;
      }

      result = DALSYS_SetupCallbackEvent(
         pDeviceData->hCalibrationCompleteEvent,
         (DALSYSCallbackFunc)Adc_CalibrationCompleteCb,
         pDevCtxt);

      if (result != DAL_SUCCESS)
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                         "Adc could not setup the calibration complete event");
         return result;
      }
   } /* End of for. */

   /* setup the conversion-complete callback event */
   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                                 &pDevCtxt->hConversionCompleteEvent,
                                 &pDevCtxt->conversionCompleteEventObject);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not create the conversion complete event");
      return result;
   }

   result = DALSYS_SetupCallbackEvent(
      pDevCtxt->hConversionCompleteEvent,
      (DALSYSCallbackFunc)Adc_ConversionCompleteCb,
      pDevCtxt);

   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not setup the conversion complete event");
      return result;
   }

   /* calibrate the ADC devices */
   result = Adc_CalibrateDevices(pCtxt);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Adc could not perform initial device calibration");
      return result;
   }

   return result;
}

DALResult 
Adc_DeviceDeInit(AdcClientCtxt *pCtxt)
{
   uint32 nDeviceIdx;
   DALResult tmpResult;
   DALResult retResult = DAL_SUCCESS;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   /* destroy the conversion-complete event */
   if (pDevCtxt->hConversionCompleteEvent != NULL)
   {
      tmpResult = DALSYS_DestroyObject(&pDevCtxt->hConversionCompleteEvent);
      if (tmpResult != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
   }

   /* detach from the ADC devices */
   for(nDeviceIdx = 0; nDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; nDeviceIdx++)
   {
      if (pDevCtxt->paDeviceData[nDeviceIdx].phDevice != NULL)
      {
         tmpResult = DAL_DeviceDetach(pDevCtxt->paDeviceData[nDeviceIdx].phDevice);

         if (tmpResult != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }
   }

   for(nDeviceIdx = 0; nDeviceIdx < pDevCtxt->pBsp->uNumPhysicalDevices; nDeviceIdx++)
   {
      if (pDevCtxt->paDeviceData[nDeviceIdx].paNodes != NULL)
      {
         /* free device queue nodes */
         tmpResult = DALSYS_Free(pDevCtxt->paDeviceData[nDeviceIdx].paNodes);
         if (tmpResult != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }

      if (pDevCtxt->paDeviceData[nDeviceIdx].hCalibrationCompleteEvent != NULL)
      {
         /* destroy the calibration complete event */
         tmpResult = DALSYS_DestroyObject(
            &pDevCtxt->paDeviceData[nDeviceIdx].hCalibrationCompleteEvent);
         if (tmpResult != DAL_SUCCESS)
         {
            retResult = DAL_ERROR;
         }
      }
   }

   /* free device data array */
   if (pDevCtxt->paDeviceData != NULL)
   {
      tmpResult = DALSYS_Free(pDevCtxt->paDeviceData);
      if (tmpResult != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
   }

   /* Destroy synchronization object */
   if (pDevCtxt->hSync != NULL)
   {
      tmpResult = DALSYS_DestroyObject(pDevCtxt->hSync);
      if (tmpResult != DAL_SUCCESS)
      {
         retResult = DAL_ERROR;
      }
   }

   return retResult;
}

DALResult 
Adc_PowerEvent(AdcClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
Adc_Open(AdcClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
Adc_Close(AdcClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
Adc_Info(AdcClientCtxt *pCtxt, DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdc Interface. 

These functions are documented in DDIAdc.h
------------------------------------------------------------------------------*/

DALResult 
Adc_GetAdcInputProperties(AdcClientCtxt * pCtxt,
                          const char * pInputName,
                          uint32  nInputNameSize,
                          AdcInputPropertiesType * pAdcInputProps) 
{
   DALSYSPropertyVar propInput;
   DALResult result;
   uint32 nDeviceIdx, nChannelIdx;
   AdcDeviceDataType *pDeviceData;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   if (pAdcInputProps == NULL)
   {
      return DAL_ERROR;
   }

   result = DALSYS_GetPropertyValue(pDevCtxt->hProp, pInputName, 0, &propInput);

   if ((result == DAL_SUCCESS) && (pAdcInputProps != NULL))
   {
      pAdcInputProps->nDeviceIdx = propInput.Val.pdwVal[0];
      pAdcInputProps->nChannelIdx = propInput.Val.pdwVal[1];
      return DAL_SUCCESS;
   }

   for (nDeviceIdx = 0; nDeviceIdx< pDevCtxt->pBsp->uNumPhysicalDevices; nDeviceIdx++)
   {
      /* Get the pointer to the device data corresponding to this device index */
      pDeviceData = &pDevCtxt->paDeviceData[nDeviceIdx];

      if ((pDeviceData->deviceStatus == ADC_DEVICE_NOT_AVAILABLE) || (pDeviceData->phDevice == NULL))
      {
         continue;
      }
      
      if (DalAdcDevice_GetInputProperties(pDeviceData->phDevice,(char *)pInputName, &nChannelIdx) == DAL_SUCCESS)
      {
         pAdcInputProps->nDeviceIdx  = nDeviceIdx;
         pAdcInputProps->nChannelIdx = nChannelIdx;
         return DAL_SUCCESS;
      }
   }

   return DAL_ERROR;
}

DALResult 
Adc_RequestConversion(AdcClientCtxt *pCtxt,
                      AdcRequestParametersType *pAdcParams,
                      AdcRequestStatusType *pAdcRequestStatus) 
{
   DALResult ret;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   /* initialize request status to Error */
   if (pAdcRequestStatus != NULL)
   {
      pAdcRequestStatus->eStatus = ADC_REQUEST_STATUS_ERROR;
   }

   if ((pAdcParams == NULL) || (pAdcParams->hEvent == NULL))
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc conversion requested with a NULL pointer parameter");

      return ADC_ERROR_NULL_POINTER;
   }
   else if (pAdcParams->nDeviceIdx >= pDevCtxt->pBsp->uNumPhysicalDevices)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc conversion requested with invalid device index");

      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].deviceStatus ==
           ADC_DEVICE_NOT_AVAILABLE)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "ADC device not available.");

      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pAdcParams->nChannelIdx >=
            pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].nNumChannels)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc conversion requested with invalid channel index");

      return ADC_ERROR_INVALID_CHANNEL_IDX;
   }

   /* Log the ADC conversion request */
   Adc_LogConversionRequest(pAdcParams->nDeviceIdx, pAdcParams->nChannelIdx);

   ret = Adc_AddDeviceQueueRequest(pCtxt,
                     ADC_REQUEST_CONVERSION,
                     pAdcParams->nDeviceIdx,
                     pAdcParams->nChannelIdx,
                     pAdcParams->hEvent,
                     pAdcRequestStatus);

   return ret;
}

DALResult 
Adc_RequestRecalibration(AdcClientCtxt *pCtxt,
                         AdcRequestParametersType *pAdcParams) 
{
   DALResult ret;
   AdcDevCtxt * pDevCtxt = pCtxt->pAdcDevCtxt;

   if (pAdcParams == NULL)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc recalibration requested with a NULL pointer for pAdcParams");

      return ADC_ERROR_NULL_POINTER;
   }
   else if (pAdcParams->nDeviceIdx >= pDevCtxt->pBsp->uNumPhysicalDevices)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc calibration requested with invalid device index");

      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].deviceStatus ==
           ADC_DEVICE_NOT_AVAILABLE)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "ADC Device not available.");

      return ADC_ERROR_INVALID_DEVICE_IDX;
   }
   else if (pAdcParams->nChannelIdx >=
            pDevCtxt->paDeviceData[pAdcParams->nDeviceIdx].nNumChannels)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "Adc conversion requested with invalid channel index");

      return ADC_ERROR_INVALID_CHANNEL_IDX;
   }

   /* Log the ADC conversion request */
   Adc_LogRecalibrationRequest(pAdcParams->nDeviceIdx, pAdcParams->nChannelIdx);

   ret = Adc_AddDeviceQueueRequest(pCtxt,
                     ADC_REQUEST_CHANNEL_RECALIBRATION,
                     pAdcParams->nDeviceIdx,
                     pAdcParams->nChannelIdx,
                     pAdcParams->hEvent,
                     NULL);

   return ret;
}

