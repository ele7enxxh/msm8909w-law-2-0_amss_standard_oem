/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/stm/dal/src/common/DalSTMCfg.c#1 $
=============================================================================*/

#include "halqdss_stm_config.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DDISTMCfg.h"

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct STMCfgDrvCtxt STMCfgDrvCtxt;
typedef struct STMCfgDevCtxt STMCfgDevCtxt;
typedef struct STMCfgClientCtxt STMCfgClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "STMCfg" Vtable
------------------------------------------------------------------------------*/
typedef struct STMCfgDALVtbl STMCfgDALVtbl;
struct STMCfgDALVtbl
{
   int (*STMCfg_DriverInit)(STMCfgDrvCtxt *);
   int (*STMCfg_DriverDeInit)(STMCfgDrvCtxt *);
};

struct STMCfgDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   STMCfgDrvCtxt  *pSTMCfgDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //STMCfg Dev state can be added by developers here
   uint32 claimTag;
   DALSYSMemHandle hMem;
};

struct STMCfgDrvCtxt
{
   //Base Members
   STMCfgDALVtbl STMCfgDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   STMCfgDevCtxt STMCfgDevCtxt[1];
   //STMCfg Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Client Context
------------------------------------------------------------------------------*/
struct STMCfgClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   STMCfgDevCtxt *pSTMCfgDevCtxt;
   DalSTMCfgHandle DalSTMCfgHandle;
   //STMCfg Client state can be added by developers here
};

static void _AccessGrant(uint32 claimTag)
{
   HAL_qdss_stm_AccessUnLock();

try_again:
   HAL_qdss_stm_SetClaimTag(claimTag);
   if(HAL_qdss_stm_GetClaimTag() != claimTag)
   {
      HAL_qdss_stm_ClrClaimTag(claimTag);
      goto try_again;
   }
}

static void _AccessRevoke(uint32 claimTag)
{
   HAL_qdss_stm_ClrClaimTag(claimTag);
   HAL_qdss_stm_AccessLock();
}

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
STMCfg_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
static DALDEVICEID DalSTMCfg_DeviceId[1] = {DALDEVICEID_STM_CONFIG};

const DALREG_DriverInfo
DALSTMCfg_DriverInfo = { STMCfg_DeviceAttach, 1, DalSTMCfg_DeviceId };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
STMCfg_AddRef(DalSTMCfgHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
STMCfg_DriverInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
STMCfg_DriverDeInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
STMCfg_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
STMCfg_DeviceInit(DalDeviceHandle *h)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   HAL_qdss_stm_PortSelRet portSel;
   DALSYSPropertyVar PropVar;
   DALSYSMemInfo memInfo;

   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle( pDevCtxt->DevId,
                                                   pDevCtxt->hProp ))
      return DAL_ERROR;


   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                "stm_claim_tag",
                                                 0,
                                                &PropVar ))
      return DAL_ERROR;

   pDevCtxt->claimTag = PropVar.Val.dwVal;


   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                "stm_phys_addr",
                                                 0,
                                                &PropVar ))
      return DAL_ERROR;

   if(DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                           PropVar.Val.dwVal,
                                           4096, &pDevCtxt->hMem, NULL ))
      return DAL_ERROR;

   DALSYS_MemInfo(pDevCtxt->hMem, &memInfo);

   HAL_qdss_stm_HalConfigInit(memInfo.VirtualAddr);

   _AccessGrant(pDevCtxt->claimTag);

   if(!HAL_qdss_stm_isEnabled())
   {
      HAL_qdss_stm_MasterSelectCfg(0, MASTER_SELECT_CTL_ALL);

      HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);

      HAL_qdss_stm_PortSelectCfg(0, 0, PORT_SELECT_CTL_SELECT_ALL, &portSel);

      HAL_qdss_stm_TimeStampEnable();

      /*STM ATID is 16 per ATID document*/
      HAL_qdss_stm_TraceIdCfg(16);

      /*Generate ATID 0x7D for writes to TRIG location*/      
      HAL_qdss_stm_TrigGenTrigLocation();

      /*Sync Period is 4K bytes*/
      HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,0xFFF);

      HAL_qdss_stm_Enable();
   }

   _AccessRevoke(pDevCtxt->claimTag);

   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceDeInit(DalDeviceHandle *h)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   
   _AccessGrant(pDevCtxt->claimTag);
   HAL_qdss_stm_Disable();
   _AccessRevoke(pDevCtxt->claimTag);

   if(pDevCtxt->hMem)
      DALSYS_DestroyObject(pDevCtxt->hMem);

   return DAL_SUCCESS;
}

static DALResult
STMCfg_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
STMCfg_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTMCFG_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSTMCfg Interface.
------------------------------------------------------------------------------*/

static DALResult
STMCfg_STMControl( DalDeviceHandle * h, STMControlType  uCtrl, uint32  uVal)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALResult nErr = DAL_SUCCESS;

   _AccessGrant(pDevCtxt->claimTag);

   switch(uCtrl)
   {
      case STM_SW_TRACE_EN:
         if(uVal)
            HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);
         else
            HAL_qdss_stm_PortEnableMask(0x0);
         break;

      case STM_TIMESTAMP_EN:
         if(uVal)
            HAL_qdss_stm_TimeStampEnable();
         else
            HAL_qdss_stm_TimeStampDisable();
         break;

      case STM_TIMESTAMP_FREQ:
         HAL_qdss_stm_SetTimeStampFreq(uVal);
         break;

      case STM_TIMESTAMP_FREQ_PKTGEN:
         if(uVal)
         {  HAL_qdss_stm_SetTimeStampFreq(HAL_qdss_stm_GetTimeStampFreq());
         }
         break;

      case STM_COMPRESSION_EN:
         if(uVal)
         {
            HAL_qdss_stm_CompEnable();
         }
         else
         {
            HAL_qdss_stm_CompDisable();
         }
      break;

      case STM_SYNC_EN:
         if(uVal)
         {
            HAL_qdss_stm_SyncEnable();
         }
         else
         {
            HAL_qdss_stm_SyncDisable();
         }
         break;

      case STM_SYNC_PERIOD_N:
       if(uVal)
       {
         HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,uVal);
       }
       break;

       case STM_SYNC_PERIOD_2POW_N:
        if(uVal)
        {
          HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES,uVal);
        }
         break;

      default:
         nErr = DAL_ERROR;
   }

   _AccessRevoke(pDevCtxt->claimTag);
   return nErr;
}

static void
STMCfg_InitInterface(STMCfgClientCtxt* pclientCtxt)
{
   static const DalSTMCfg vtbl =
   {
      {
         STMCfg_DeviceAttach,
         STMCfg_DeviceDetach,
         STMCfg_DeviceInit,
         STMCfg_DeviceDeInit,
         STMCfg_DeviceOpen,
         STMCfg_DeviceClose,
         STMCfg_DeviceInfo,
         STMCfg_DevicePowerEvent,
         STMCfg_DeviceSysRequest
      },

      STMCfg_STMControl,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalSTMCfgHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalSTMCfgHandle.pVtbl  = &vtbl;
   pclientCtxt->DalSTMCfgHandle.pClientCtxt = pclientCtxt;
}

DALResult
STMCfg_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static STMCfgDrvCtxt drvCtxt = {{STMCfg_DriverInit,
                                      STMCfg_DriverDeInit
                                     },1,
                                     sizeof(STMCfgDevCtxt)};
   static STMCfgClientCtxt clientCtxt;

   STMCfgClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      STMCfg_InitInterface(pclientCtxt);
      STMCfg_AddRef(&(pclientCtxt->DalSTMCfgHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSTMCfgHandle);
   }
   return nErr;
}
