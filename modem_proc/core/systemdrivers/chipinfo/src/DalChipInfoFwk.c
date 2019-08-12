/*============================================================================

FILE:      DalChipInfoFwk.c

DESCRIPTION:
  This file implements a ChipInfo DeviceDriver.

PUBLIC CLASSES:
  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Not Applicable

============================================================================
        Copyright (c) 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/chipinfo/src/DalChipInfoFwk.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------
11/20/15   shm     Added API to query foundry.
07/11/11   pbi     Fixed warnings.
============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DalChipInfo.h"
#include "string.h" //for memset warning


DALResult 
ChipInfo_DalChipInfo_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
ChipInfo_DalChipInfo_AddRef(DalChipInfoHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
ChipInfo_DalChipInfo_Detach(DalDeviceHandle* h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult 
ChipInfo_DalChipInfo_Init(DalDeviceHandle *h)
{  
  ChipInfoClientCtxt *pClientCtxt = (ChipInfoClientCtxt *)(h->pClientCtxt);
  DALSYS_GetDALPropertyHandle(pClientCtxt->pChipInfoDevCtxt->DevId,
                              pClientCtxt->pChipInfoDevCtxt->hProp);
  return ChipInfo_DeviceInit(h->pClientCtxt);
}

static DALResult 
ChipInfo_DalChipInfo_DeInit(DalDeviceHandle *h)
{
  return ChipInfo_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
ChipInfo_DalChipInfo_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
  return ChipInfo_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
ChipInfo_DalChipInfo_Open(DalDeviceHandle* h, uint32 mode)
{
  return ChipInfo_Open(h->pClientCtxt,mode);
}

static DALResult 
ChipInfo_DalChipInfo_Close(DalDeviceHandle* h)
{
  return ChipInfo_Close(h->pClientCtxt);
}

static DALResult 
ChipInfo_DalChipInfo_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  info->Version = DALCHIPINFO_INTERFACE_VERSION;
  return ChipInfo_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
ChipInfo_DalChipInfo_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
  return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalChipInfo Interface. 
------------------------------------------------------------------------------*/

static DALResult 
ChipInfo_DalChipInfo_GetChipVersion( DalDeviceHandle * h,  uint32  nNotUsed,  DalChipInfoVersionType * pnVersion) 
{
  return ChipInfo_GetChipVersion(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, pnVersion);
}

static DALResult 
ChipInfo_DalChipInfo_GetRawChipVersion( DalDeviceHandle * h,  uint32  nNotUsed,  uint32 * pnVersion) 
{
  return ChipInfo_GetRawChipVersion(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, pnVersion);
}

static DALResult 
ChipInfo_DalChipInfo_GetChipId( DalDeviceHandle * h,  uint32  nNotUsed,  DalChipInfoIdType * peId) 
{
  return ChipInfo_GetChipId(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, peId);
}

static DALResult 
ChipInfo_DalChipInfo_GetRawChipId( DalDeviceHandle * h,  uint32  nNotUsed,  uint32 * pnId) 
{
  return ChipInfo_GetRawChipId(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, pnId);
}

static DALResult 
ChipInfo_DalChipInfo_GetChipIdString( DalDeviceHandle * h,  char * szIdString,  uint32  nMaxLength) 
{
  return ChipInfo_GetChipIdString(((DalChipInfoHandle *)h)->pClientCtxt, szIdString, nMaxLength);
}

static DALResult 
ChipInfo_DalChipInfo_GetChipFamily( DalDeviceHandle * h,  uint32  nNotUsed,  DalChipInfoFamilyType * peFamily) 
{
  return ChipInfo_GetChipFamily(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, peFamily);
}
static DALResult 
ChipInfo_DalChipInfo_GetModemSupport( DalDeviceHandle * h,  uint32  nNotUsed,  DalChipInfoModemType * pnModem) 
{
  return ChipInfo_GetModemSupport(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, pnModem);
}

static DALResult 
ChipInfo_DalChipInfo_GetFoundryId( DalDeviceHandle * h,  uint32  nNotUsed,  DalChipInfoFoundryIdType * peFoundryId) 
{
  return ChipInfo_GetFoundryId(((DalChipInfoHandle *)h)->pClientCtxt, nNotUsed, peFoundryId);
}

static void 
ChipInfo_InitInterface(ChipInfoClientCtxt* pclientCtxt)
{
  static const DalChipInfo vtbl =
  {
    {
      ChipInfo_DalChipInfo_Attach,
      ChipInfo_DalChipInfo_Detach,
      ChipInfo_DalChipInfo_Init,
      ChipInfo_DalChipInfo_DeInit,
      ChipInfo_DalChipInfo_Open,
      ChipInfo_DalChipInfo_Close,
      ChipInfo_DalChipInfo_Info,
      ChipInfo_DalChipInfo_PowerEvent,
      ChipInfo_DalChipInfo_SysRequest
    },
    ChipInfo_DalChipInfo_GetChipVersion,
    ChipInfo_DalChipInfo_GetRawChipVersion,
    ChipInfo_DalChipInfo_GetChipId,
    ChipInfo_DalChipInfo_GetRawChipId,
    ChipInfo_DalChipInfo_GetChipIdString,
    ChipInfo_DalChipInfo_GetChipFamily,
    ChipInfo_DalChipInfo_GetModemSupport,
    ChipInfo_DalChipInfo_GetFoundryId
  };  
  /*--------------------------------------------------------------------------
  Depending upon client type setup the vtables (entry points)
  --------------------------------------------------------------------------*/
  pclientCtxt->DalChipInfoHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalChipInfoHandle.pVtbl = &vtbl;
  pclientCtxt->DalChipInfoHandle.pClientCtxt = pclientCtxt;

}

static ChipInfoDrvCtxt ChipInfo_drvCtxt = 
{
  {
    ChipInfo_DriverInit,
    ChipInfo_DriverDeInit
  },
  1,
  sizeof(ChipInfoDevCtxt)
};

DALResult 
ChipInfo_DalChipInfo_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
  DALResult nErr;
  static ChipInfoClientCtxt clientCtxt;

  ChipInfoClientCtxt *pclientCtxt = &clientCtxt;


  *phDalDevice = NULL;

  nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&ChipInfo_drvCtxt,
                              (DALClientCtxt *)pclientCtxt);
  if (DAL_SUCCESS == nErr)
  {
    ChipInfo_InitInterface(pclientCtxt);
    ChipInfo_DalChipInfo_AddRef(&(pclientCtxt->DalChipInfoHandle));
    *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalChipInfoHandle);
  }
  return nErr;
}
