/*============================================================================
  @file AdcDiagDal.c

  Implementation of the DAL ADC diagnostic system.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/diag/src/AdcDiagDal.c#1 $

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "timetick.h"
#include "DALSys.h"
#include "DALQueue.h"
#include "DALDeviceId.h"
#include "AdcDiagDal.h"
#include "DDIAdc.h"

/*===========================================================================

  Macros

===========================================================================*/

#define ADCDIAGDAL_ERR_MSG(msg) DALSYS_LogEvent((DALDEVICEID)DALDEVICEID_ADC, DALSYS_LOGEVENT_ERROR, msg)

#define ADCDIAGDAL_CHECK_RESULT(DalResult, msg, retValue) do {\
  if(DalResult != DAL_SUCCESS)  \
  { \
    ADCDIAGDAL_ERR_MSG(msg); \
    return retValue; \
  }} while(0)

/*===========================================================================

  Structures

===========================================================================*/

typedef struct
{
  DALFW_Q_LinkType link;
  AdcDiagDAL_ReadResult result;
} AdcDiagDAL_ResultsBufferNodeType;

typedef struct
{
  DALSYSSyncHandle hBufferLock;
  DALSYSSyncObj bufferLockObject;

  DALSYSEventHandle hReadCallbackEvent;
  DALSYS_EVENT_OBJECT(readCallbackEventObject);

  //This 'queue' is actually just being used as a linked list
  DALFW_Q_Type qResults;
} AdcDiagDAL_ResultsBufferType;

/*===========================================================================

  Variables

===========================================================================*/

static DalDeviceHandle *phAdcDev;

static AdcDiagDAL_ResultsBufferType *pResultsBuffer;

/*===========================================================================

  General Functions

===========================================================================*/

int
AdcDiagDAL_BufferSearchComparer
(
  AdcDiagDAL_ResultsBufferNodeType *pItem,
  AdcDiagDAL_ChannelSpecification *pCompareVal
)
{
  int result = pItem->result.channel.nDeviceIdx == pCompareVal->nDeviceIdx;
  result &= pItem->result.channel.nChannelIdx == pCompareVal->nChannelIdx;
  return result;
}

void
AdcDiagDAL_ConversionCompleteCb
(
  AdcDiagDAL_ResultsBufferType *pBuffer,
  uint32 nToken,
  AdcResultType *pResult,
  uint32 nPayloadSize
)
{
  DALResult result;
  AdcDiagDAL_ChannelSpecification channel;
  AdcDiagDAL_ResultsBufferNodeType *pNode;

  if(pResult->eStatus ==  ADC_RESULT_INVALID || phAdcDev == NULL)
  {
    return;
  }

  channel.nDeviceIdx = pResult->nDeviceIdx;
  channel.nChannelIdx = pResult->nChannelIdx;

  DALSYS_SyncEnter(pBuffer->hBufferLock);
  pNode = DALFW_Q_LinearSearch(&pBuffer->qResults,
                               (DALFW_Q_CompareFuncType)AdcDiagDAL_BufferSearchComparer,
                               &channel);
  if(pNode == NULL)
  {
    //No buffer node for this channel - need to create one

    result = DALSYS_Malloc(sizeof(AdcDiagDAL_ResultsBufferNodeType), (void **)&pNode);
    if(result != DAL_SUCCESS || pNode == NULL)
    {
      DALSYS_SyncLeave(pBuffer->hBufferLock);
      ADCDIAGDAL_ERR_MSG("Malloc error - could not save result");
      return;
    }

    DALFW_Q_Link(pNode, &pNode->link);
    DALFW_Q_Put(&pBuffer->qResults, &pNode->link);
  }

  //Update the node
  pNode->result.eErrors = pResult->eStatus != ADC_RESULT_VALID;
  pNode->result.channel.nDeviceIdx = pResult->nDeviceIdx;
  pNode->result.channel.nChannelIdx = pResult->nChannelIdx;

  pNode->result.nPhysical = pResult->nPhysical;
  pNode->result.nPercent = pResult->nPercent;
  pNode->result.nMicrovolts = pResult->nMicrovolts;
  pNode->result.nCode = pResult->nCode;

  pNode->result.nReadTime = (uint32)timetick_get();

  DALSYS_SyncLeave(pBuffer->hBufferLock);
}

/*===========================================================================

  Dispatch Handlers

===========================================================================*/

PACKED void* AdcDiagDAL_VersionHandler( PACKED void *req_pkt, uint16 pkt_len )
{
  AdcDiagDAL_Version_RespPktType *pRespPkt;

  //Ensure the diag system is active
  if(phAdcDev == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_MODE_F, req_pkt, pkt_len));
  }

  pRespPkt =  (AdcDiagDAL_Version_RespPktType *) diagpkt_subsys_alloc(
    ADC_DIAG_SUBSYS_ID,
    ADC_DIAG_CMDDAL_VERSION,
    sizeof(AdcDiagDAL_Version_RespPktType));
  if(NULL == pRespPkt)
  {
	ADCDIAGDAL_ERR_MSG("AdcDiagDAL_VersionHandler - diagpkt_subsys_alloc failed ");
  	return pRespPkt;
  }

  pRespPkt->nMajorVersion = ADC_DIAG_DAL_VER_MAJOR;
  pRespPkt->nMinorVersion = ADC_DIAG_DAL_VER_MINOR;

  ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->reserved, 4);

  return pRespPkt;
}

PACKED void* AdcDiagDAL_ChannelInfoHandler( PACKED void *req_pkt, uint16 pkt_len )
{
  DALResult result;
  AdcInputPropertiesType adcInputProps;
  DALSYS_PROPERTY_HANDLE_DECLARE(hAdcProp);
  AdcDiagDAL_ChanInfo_ReqPktType *pReqPkt = (AdcDiagDAL_ChanInfo_ReqPktType *)req_pkt;
  AdcDiagDAL_ChanInfo_RespPktType *pRespPkt;

  adcInputProps.nDeviceIdx = 0xFFFFFFFF;
  adcInputProps.nChannelIdx = 0xFFFFFFFF;

  //Ensure the diag system is active
  if(phAdcDev == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_MODE_F, req_pkt, pkt_len));
  }

  //Packet checks
  if(pkt_len < sizeof(AdcDiagDAL_ChanInfo_ReqPktType))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_ChannelInfoHandler - Basic packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }
  else if(pReqPkt->nChannelIdLength <= 1)
  {
    // Using <= 1 since last char should be a zero and will not contribute to the length

    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_ChannelInfoHandler - No channel id");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_PARM_F, pReqPkt, pkt_len));
  }
  else if(pkt_len != sizeof(AdcDiagDAL_ChanInfo_ReqPktType) + (pReqPkt->nChannelIdLength - 1))
  {
    // -1 since 1 char is taken care of by sizeof(AdcDiagDAL_ChanInfo_ReqPktType)

    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_ChannelInfoHandler - Packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }

  //Ensure zero terminated string
  if(pReqPkt->zsChannelId[pReqPkt->nChannelIdLength - 1] != 0)
  {
    // -1 since the length field includes the zero
    // Note that the expected PC behaviour in determining string length is different
    // than that of the strlen function (it does not count the zero).

    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_ChannelInfoHandler - String was not zero terminated");
    pReqPkt->zsChannelId[pReqPkt->nChannelIdLength - 1] = 0;
  }

  result = DALSYS_GetDALPropertyHandle((DALDEVICEID)DALDEVICEID_ADC, hAdcProp);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not get ADC properties", NULL);

  pRespPkt =  (AdcDiagDAL_ChanInfo_RespPktType *) diagpkt_subsys_alloc(
    ADC_DIAG_SUBSYS_ID,
    ADC_DIAG_CMDDAL_CHAN_INFO,
    sizeof(AdcDiagDAL_ChanInfo_RespPktType));

  if(NULL == pRespPkt)
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_ChannelInfoHandler - diagpkt_subsys_alloc failed ");
    return pRespPkt;
  }

  result = DalAdc_GetAdcInputProperties(phAdcDev,
                                     pReqPkt->zsChannelId,
                                     pReqPkt->nChannelIdLength,
                                     &adcInputProps);
  pRespPkt->Error = result;
  if(result == DAL_SUCCESS)
  {
    pRespPkt->info.nDeviceIdx = adcInputProps.nDeviceIdx;
    pRespPkt->info.nChannelIdx = adcInputProps.nChannelIdx;
  }
  else
  {
    //Just to be sure the client knows there is an error
    pRespPkt->info.nDeviceIdx = 0xFFFFFFFF;
    pRespPkt->info.nChannelIdx = 0xFFFFFFFF;
  }

  ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->reserved, 2);

  return pRespPkt;
}

PACKED void* AdcDiagDAL_StartReadHandler( PACKED void *req_pkt, uint16 pkt_len )
{
  int i;
  uint16 respLength;
  AdcDiagDAL_StartRead_ReqPktType *pReqPkt =(AdcDiagDAL_StartRead_ReqPktType *) req_pkt;
  AdcDiagDAL_StartRead_RespPktType *pRespPkt;

  //Ensure the diag system is active
  if(phAdcDev == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_MODE_F, req_pkt, pkt_len));
  }

  //Packet checks
  if(pkt_len < sizeof(AdcDiagDAL_StartRead_ReqPktType))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_StartReadHandler - Basic packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }
  else if(pReqPkt->nChannelCount == 0)
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_StartReadHandler - Reading no channels");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_PARM_F, pReqPkt, pkt_len));
  }
  else if(pkt_len != sizeof(AdcDiagDAL_StartRead_ReqPktType) + sizeof(AdcDiagDAL_ChannelSpecification)*(pReqPkt->nChannelCount -1))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_StartReadHandler - Packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }

  respLength = sizeof(AdcDiagDAL_StartRead_RespPktType);
  respLength += sizeof(AdcDiagDAL_ErrorResult)*(pReqPkt->nChannelCount - 1);

  pRespPkt = (AdcDiagDAL_StartRead_RespPktType *)diagpkt_subsys_alloc(
    ADC_DIAG_SUBSYS_ID,
    ADC_DIAG_CMDDAL_STARTREAD,
    respLength);

  if(pRespPkt == NULL)
  {
    ADCDIAGDAL_ERR_MSG("Packet malloc");
    return NULL;
  }

  pRespPkt->nResultCount = 0;

  //Do this before requesting the conversion of anything so results can be filtered properly
  pRespPkt->nStartReadTime = (uint32)timetick_get();

  for(i = 0; i < pReqPkt->nChannelCount; i++ )
  {
    DALResult result;
    AdcRequestParametersType params;
    AdcRequestStatusType status;

    params.nDeviceIdx = pReqPkt->channels[i].nDeviceIdx;
    params.nChannelIdx= pReqPkt->channels[i].nChannelIdx;
    params.hEvent = pResultsBuffer->hReadCallbackEvent;

    result = DalAdc_RequestConversion(phAdcDev, &params, &status);

    //Setup response packet
    pRespPkt->results[i].channel = pReqPkt->channels[i];
    pRespPkt->results[i].result = result;

    if(result == DAL_SUCCESS && status.eStatus == ADC_REQUEST_STATUS_ERROR)
    {
      //TODO: Check if this is an expected case
      pRespPkt->results[i].result = 1;;
    }

    ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->results[i].reserved, 2);
    pRespPkt->nResultCount++;
  }

  ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->reserved, 2);
  return pRespPkt;
}

PACKED void* AdcDiagDAL_GetReadingsHandler( PACKED void *req_pkt, uint16 pkt_len )
{
  int i;
  uint16 respLength;
  AdcDiagDAL_GetReadings_ReqPktType *pReqPkt =(AdcDiagDAL_GetReadings_ReqPktType *) req_pkt;
  AdcDiagDAL_GetReadings_RespPktType *pRespPkt;

  //Ensure the diag system is active
  if(phAdcDev == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_MODE_F, req_pkt, pkt_len));
  }

  //Packet checks
  if(pkt_len < sizeof(AdcDiagDAL_GetReadings_ReqPktType))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_GetReadingsHandler - Basic packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }
  else if(pReqPkt->nChannelCount == 0)
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_GetReadingsHandler - Reading no channels");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_PARM_F, pReqPkt, pkt_len));
  }
  else if(pkt_len != sizeof(AdcDiagDAL_GetReadings_ReqPktType) + sizeof(AdcDiagDAL_ChannelSpecification)*(pReqPkt->nChannelCount - 1))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_GetReadingsHandler - Packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }

  respLength = sizeof(AdcDiagDAL_GetReadings_RespPktType);
  respLength += sizeof(AdcDiagDAL_ReadResult)*(pReqPkt->nChannelCount-1);

  pRespPkt = (AdcDiagDAL_GetReadings_RespPktType *) diagpkt_subsys_alloc(
    ADC_DIAG_SUBSYS_ID,
    ADC_DIAG_CMDDAL_GETREADINGS,
    respLength);

  if(pRespPkt == NULL)
  {
    ADCDIAGDAL_ERR_MSG("Packet malloc");
    return NULL;
  }

  pRespPkt->nResultCount = 0;

  DALSYS_SyncEnter(pResultsBuffer->hBufferLock);
  for(i=0; i<pReqPkt->nChannelCount; i++)
  {
    AdcDiagDAL_ResultsBufferNodeType *pBuffer;

    pBuffer = DALFW_Q_LinearSearch(&pResultsBuffer->qResults,
                                 (DALFW_Q_CompareFuncType)AdcDiagDAL_BufferSearchComparer,
                                 &pReqPkt->channels[i]);
    if(pBuffer == NULL)
    {
      //A read has not been performed for this channel
      //Alternatively, the channel does not exist
      pRespPkt->results[i].eErrors = ADC_ERROR_INVALID_DEVICE_IDX;
      pRespPkt->results[i].channel.nDeviceIdx = pReqPkt->channels[i].nDeviceIdx;
      pRespPkt->results[i].channel.nChannelIdx = pReqPkt->channels[i].nChannelIdx;

      pRespPkt->results[i].nMicrovolts = 0;
      pRespPkt->results[i].nPercent = 0;
      pRespPkt->results[i].nPhysical = 0;
      pRespPkt->results[i].nReadTime = timetick_get();
    }
    else
    {
      pRespPkt->results[i] = pBuffer->result;
    }

    ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->results[i].reserved, 2);
    ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->results[i].reserved2, 4);
    pRespPkt->nResultCount++;
  }
  DALSYS_SyncLeave(pResultsBuffer->hBufferLock);

  ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->reserved, 2);
  return pRespPkt;
}

PACKED void* AdcDiagDAL_RecalibrateHandler( PACKED void *req_pkt, uint16 pkt_len )
{
  int i;
  uint16 respLength;
  AdcDiagDAL_Recalibrate_ReqPktType *pReqPkt =(AdcDiagDAL_Recalibrate_ReqPktType *) req_pkt;
  AdcDiagDAL_Recalibrate_RespPktType *pRespPkt;

  //Ensure the diag system is active
  if(phAdcDev == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_MODE_F, req_pkt, pkt_len));
  }

  //Packet Checks
  if(pkt_len < sizeof(AdcDiagDAL_Recalibrate_ReqPktType))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_RecalibrateHandler - Basic packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }
  else if(pReqPkt->nChannelCount == 0)
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_RecalibrateHandler - Recalibrating no channels");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_PARM_F, pReqPkt, pkt_len));
  }
  else if(pkt_len != sizeof(AdcDiagDAL_Recalibrate_ReqPktType) + sizeof(AdcDiagDAL_ChannelSpecification)*(pReqPkt->nChannelCount - 1))
  {
    ADCDIAGDAL_ERR_MSG("AdcDiagDAL_RecalibrateHandler - Packet size error");
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, pReqPkt, pkt_len));
  }

  respLength = sizeof(AdcDiagDAL_Recalibrate_RespPktType);
  respLength += sizeof(AdcDiagDAL_ErrorResult)*(pReqPkt->nChannelCount-1);

  pRespPkt = (AdcDiagDAL_Recalibrate_RespPktType *) diagpkt_subsys_alloc(
    ADC_DIAG_SUBSYS_ID,
    ADC_DIAG_CMDDAL_RECALIBRATE,
    respLength);

  if(pRespPkt == NULL)
  {
    ADCDIAGDAL_ERR_MSG("Packet malloc");
    return NULL;
  }

  pRespPkt->nResultCount = 0;

  for(i = 0; i<pReqPkt->nChannelCount; i++ )
  {
    AdcRequestParametersType params;

    params.nDeviceIdx = pReqPkt->channels[i].nDeviceIdx;
    params.nChannelIdx = pReqPkt->channels[i].nChannelIdx;
    params.hEvent = NULL;

    pRespPkt->results[i].channel = pReqPkt->channels[i];
    pRespPkt->results[i].result = DalAdc_RequestRecalibration(phAdcDev, &params);

    ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->results[i].reserved, 2);
    pRespPkt->nResultCount++;
  }

  ADC_DIAG_SET_RESERVED_FIELDS(pRespPkt->reserved, 2);
  return pRespPkt;
}
/*===========================================================================

  Init, deint and other core functions

===========================================================================*/

static const diagpkt_user_table_entry_type AdcDiagDAL_DispatchTbl[] =
{
  {ADC_DIAG_CMDDAL_VERSION, ADC_DIAG_CMDDAL_VERSION, AdcDiagDAL_VersionHandler},
  {ADC_DIAG_CMDDAL_CHAN_INFO, ADC_DIAG_CMDDAL_CHAN_INFO, AdcDiagDAL_ChannelInfoHandler},
  {ADC_DIAG_CMDDAL_STARTREAD, ADC_DIAG_CMDDAL_STARTREAD, AdcDiagDAL_StartReadHandler},
  {ADC_DIAG_CMDDAL_GETREADINGS, ADC_DIAG_CMDDAL_GETREADINGS, AdcDiagDAL_GetReadingsHandler},
  {ADC_DIAG_CMDDAL_RECALIBRATE, ADC_DIAG_CMDDAL_RECALIBRATE, AdcDiagDAL_RecalibrateHandler},
};

DALResult AdcDiagDAL_Init(void)
{
  DALResult result = DAL_SUCCESS;
  static uint8 bHasRegisterdDispatchTable = 0;

  if(phAdcDev != NULL)
  {
    if(bHasRegisterdDispatchTable == 0)
    {
      ADCDIAGDAL_ERR_MSG("Inconsistent init state");
      return DAL_ERROR;
    }
    return DAL_SUCCESS;
  }

  result = DAL_AdcDeviceAttach((DALDEVICEID)DALDEVICEID_ADC, &phAdcDev);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not attach to ADC", result);

  result = DALSYS_Malloc(sizeof(AdcDiagDAL_ResultsBufferType), (void **)&pResultsBuffer);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not create buffer", result);

  result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                             &pResultsBuffer->hBufferLock,
                             &pResultsBuffer->bufferLockObject);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not create sync", result);

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT | DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                              &pResultsBuffer->hReadCallbackEvent,
                              &pResultsBuffer->readCallbackEventObject);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not create event", result);

  result = DALSYS_SetupCallbackEvent(pResultsBuffer->hReadCallbackEvent,
                                     (DALSYSCallbackFunc)AdcDiagDAL_ConversionCompleteCb,
                                     pResultsBuffer);
  ADCDIAGDAL_CHECK_RESULT(result, "Could not setup event", result);

  (void)DALFW_Q_Init(&pResultsBuffer->qResults);

  if(!bHasRegisterdDispatchTable)
  {
    bHasRegisterdDispatchTable = 1;
    DIAGPKT_DISPATCH_TABLE_REGISTER (ADC_DIAG_SUBSYS_ID, AdcDiagDAL_DispatchTbl);
  }

  return result;
}


DALResult AdcDiagDAL_DeInit(void)
{
  //This deinit implementation is not threadsafe.
  //Issues could arise if a dispatch hander is in use during deinit

  DALResult result = DAL_SUCCESS;
  phAdcDev = NULL;

  if (pResultsBuffer != NULL)
  {
    uint16 i;
    DALResult tmp = DAL_SUCCESS;

    //Dispose of the buffer nodes
    for(i=0; i< DALFW_Q_Cnt(&pResultsBuffer->qResults); i++)
    {
      AdcDiagDAL_ResultsBufferNodeType *pNode = DALFW_Q_Get(&pResultsBuffer->qResults);
      tmp = DALSYS_Free(pNode);
      if(tmp != DAL_SUCCESS)
      {
        result=tmp;
      }
    }

    tmp = DALSYS_DestroyObject(pResultsBuffer->hReadCallbackEvent);
    if(tmp != DAL_SUCCESS)
    {
      result=tmp;
    }

    tmp = DALSYS_DestroyObject(pResultsBuffer->hBufferLock);
    if(tmp != DAL_SUCCESS)
    {
      result=tmp;
    }

    tmp = DALSYS_Free(pResultsBuffer);
    if(tmp != DAL_SUCCESS)
    {
      result=tmp;
    }

    pResultsBuffer = NULL;
  }

  return result;
}

