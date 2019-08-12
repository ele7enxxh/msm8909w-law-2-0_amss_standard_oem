/*===========================================================================
  FILE:         DalmpmFwk.c
  
  OVERVIEW:     This file implements a vMPM Device Driver
 
  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/dal/DalmpmFwk.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDImpm.h"
#include "Dalmpm.h"
#include "DALSys.h"

DALResult mpm_Dalmpm_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
  
static uint32 mpm_Dalmpm_AddRef(DalmpmHandle* h)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 mpm_Dalmpm_Detach(DalDeviceHandle* h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult mpm_Dalmpm_Init(DalDeviceHandle *h)
{  
  mpmClientCtxt *pClientCtxt = (mpmClientCtxt *)(h->pClientCtxt);
  DALSYS_GetDALPropertyHandle(pClientCtxt->pmpmDevCtxt->DevId,
                              pClientCtxt->pmpmDevCtxt->hProp);
  return mpm_DeviceInit(h->pClientCtxt);
}

static DALResult mpm_Dalmpm_DeInit(DalDeviceHandle *h)
{
  return mpm_DeviceDeInit(h->pClientCtxt);
}

static DALResult mpm_Dalmpm_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                       DalPowerDomain PowerDomain )
{
  return mpm_PowerEvent(h->pClientCtxt, PowerCmd, PowerDomain);
}

static DALResult mpm_Dalmpm_Open(DalDeviceHandle* h, uint32 mode)
{
  return mpm_Open(h->pClientCtxt, mode);
}

static DALResult mpm_Dalmpm_Close(DalDeviceHandle* h)
{
  return mpm_Close(h->pClientCtxt);
}

static DALResult mpm_Dalmpm_Info(DalDeviceHandle *h,
                                 DalDeviceInfo   *info,
                                 uint32           infoSize)
{
  info->Version = DALMPM_INTERFACE_VERSION;
  return mpm_Info(h->pClientCtxt, info, infoSize);
}

static DALResult mpm_Dalmpm_SysRequest(DalDeviceHandle *h,
                                       DalSysReq        ReqIdx,
                                       const void      *SrcBuf,
                                       uint32           SrcBufLen, 
                                       void            *DestBuf,
                                       uint32           DestBufLen,
                                       uint32          *DestBufLenReq)
{
  return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in Dalmpm Interface. 
------------------------------------------------------------------------------*/
static DALResult mpm_Dalmpm_ConfigPin( DalDeviceHandle      *h,
                                       uint32               mpmPinNum,
                                       mpm_pin_config_type  *mpmConfigInfo)
{
  return mpm_ConfigPin(((DalmpmHandle *)h)->pClientCtxt, mpmPinNum, mpmConfigInfo);
}

static DALResult mpm_Dalmpm_ConfigAndEnablePin( DalDeviceHandle      *h,
                                                uint32               mpmPinNum,
                                                mpm_pin_config_type  *mpmConfigInfo)
{
  return mpm_ConfigAndEnablePin(((DalmpmHandle *)h)->pClientCtxt, mpmPinNum, mpmConfigInfo);
}

static DALResult mpm_Dalmpm_EnablePin( DalDeviceHandle      *h,
                                       uint32               mpmPinNum)
{
  return mpm_EnablePin(((DalmpmHandle *)h)->pClientCtxt, mpmPinNum);
}

static DALResult mpm_Dalmpm_DisablePin( DalDeviceHandle *h, 
                                        uint32 mpmPinNum)
{
  return mpm_DisablePin(((DalmpmHandle *)h)->pClientCtxt, mpmPinNum);
}

static DALResult mpm_Dalmpm_GetNumMappedIrq( DalDeviceHandle *h, uint32 *pnum) 
{
  return mpm_GetNumMappedIrq(((DalmpmHandle *)h)->pClientCtxt, pnum);
}

static DALResult mpm_Dalmpm_GetNumMappedGpio( DalDeviceHandle *h, uint32 *pnum) 
{
  return mpm_GetNumMappedGpio(((DalmpmHandle *)h)->pClientCtxt, pnum);
}

static DALResult mpm_Dalmpm_MapInterrupts( DalDeviceHandle         *h,
                                           mpm_config_info_type    *irqArray,
                                           uint32                  irqCount,
                                           uint32                  *irqsMapped) 
{
  return mpm_MapInterrupts(((DalmpmHandle *)h)->pClientCtxt, irqArray, irqCount, irqsMapped);
}

static DALResult mpm_Dalmpm_SetupInterrupts( DalDeviceHandle         *h,
                                             mpm_config_info_type    *irqArray,
                                             uint32                  irqCount) 
{
  return mpm_SetupInterrupts(((DalmpmHandle *)h)->pClientCtxt, irqArray, irqCount);
}

static DALResult mpm_Dalmpm_QueryPinMapping( DalDeviceHandle *h,
                                             uint32          localPinNum, 
                                             mpm_pin_type    pinType, 
                                             uint32          *pMpmPinNum)
{
  return mpm_QueryPinMapping(((DalmpmHandle *)h)->pClientCtxt, 
                                                  localPinNum, 
                                                  pinType, 
                                                  pMpmPinNum);
}

static DALResult mpm_Dalmpm_QueryPinConfigInfo( DalDeviceHandle         *h, 
                                                uint32                  mpmPinNum, 
                                                mpm_pin_query_info_type *pMpmPinInfo )
{
  return mpm_QueryPinConfigInfo(((DalmpmHandle *)h)->pClientCtxt, 
                                                     mpmPinNum, 
                                                     pMpmPinInfo);
}

static DALResult mpm_Dalmpm_AddIrqPinToMappingTbl( 
                       DalDeviceHandle          *h,
                       uint32                   mpmPinNum, 
                       mpm_int_pin_mapping_type *pNewMapTblEntryInfo )
{
  return mpm_AddIrqPinToMappingTbl(((DalmpmHandle *)h)->pClientCtxt, 
                                                     mpmPinNum, 
                                                     pNewMapTblEntryInfo);
}

static void mpm_InitInterface(mpmClientCtxt* pclientCtxt)
{
  static const Dalmpm vtbl = {
    {
      mpm_Dalmpm_Attach,
      mpm_Dalmpm_Detach,
      mpm_Dalmpm_Init,
      mpm_Dalmpm_DeInit,
      mpm_Dalmpm_Open,
      mpm_Dalmpm_Close,
      mpm_Dalmpm_Info,
      mpm_Dalmpm_PowerEvent,
      mpm_Dalmpm_SysRequest
    } ,
    mpm_Dalmpm_ConfigPin,
    mpm_Dalmpm_ConfigAndEnablePin,
    mpm_Dalmpm_EnablePin,
    mpm_Dalmpm_DisablePin,
    mpm_Dalmpm_GetNumMappedIrq,
    mpm_Dalmpm_GetNumMappedGpio,
    mpm_Dalmpm_MapInterrupts,
    mpm_Dalmpm_SetupInterrupts,
    mpm_Dalmpm_QueryPinMapping,
    mpm_Dalmpm_QueryPinConfigInfo,
    mpm_Dalmpm_AddIrqPinToMappingTbl
  };  

  /*--------------------------------------------------------------------------
  Depending upon client type setup the vtables (entry points)
  --------------------------------------------------------------------------*/
  pclientCtxt->DalmpmHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalmpmHandle.pVtbl  = &vtbl;
  pclientCtxt->DalmpmHandle.pClientCtxt = pclientCtxt;

}

DALResult mpm_Dalmpm_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
  DALResult nErr;
  static mpmDrvCtxt drvCtxt = {{mpm_DriverInit,
                                mpm_DriverDeInit
                                },1,
                                sizeof(mpmDevCtxt)};
  static mpmClientCtxt clientCtxt;

  mpmClientCtxt *pclientCtxt = &clientCtxt;
  *phDalDevice = NULL;

  nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                    (DALClientCtxt *)pclientCtxt);

  if (DAL_SUCCESS == nErr)
  {
    mpm_InitInterface(pclientCtxt);
    mpm_Dalmpm_AddRef(&(pclientCtxt->DalmpmHandle));
    *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalmpmHandle);
  }
  return nErr;
}

