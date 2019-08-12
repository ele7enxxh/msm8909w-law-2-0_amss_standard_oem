/*=============================================================================
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
           All Rights Reserved.

           $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tmc/dal/src/DalTMC.c#1 $
=============================================================================*/

#include "halqdss_etfetb.h"
#include "halqdss_etr.h"
#include "halqdss_csr.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITMC.h"
#include "qdss_tmc_config.h"

#define TRY(nErr, func) \
   if (DAL_SUCCESS != (nErr = (func))) {\
      goto nErr##bail; \
   }

#define THROW(nErr, errno) \
   nErr = errno; \
   goto nErr##bail;

#define CATCH(nErr) nErr##bail: if (nErr != DAL_SUCCESS)

/*------------------------------------------------------------------------------
Declaring a "TMC" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TMCDrvCtxt TMCDrvCtxt;
typedef struct TMCDevCtxt TMCDevCtxt;
typedef struct TMCClientCtxt TMCClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "TMC" Vtable
------------------------------------------------------------------------------*/
typedef struct TMCDALVtbl TMCDALVtbl;
struct TMCDALVtbl
{
   int (*TMC_DriverInit)(TMCDrvCtxt *);
   int (*TMC_DriverDeInit)(TMCDrvCtxt *);
};

struct TMCDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TMCDrvCtxt  *pTMCDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TMC Dev state can be added by developers here
   DALSYSMemHandle hMemETFETB;
   DALSYSMemHandle hMemETR;
};

struct TMCDrvCtxt
{
   //Base Members
   TMCDALVtbl TMCDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TMCDevCtxt TMCDevCtxt[1];
   //TMC Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TMC" Client Context
------------------------------------------------------------------------------*/
struct TMCClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TMCDevCtxt *pTMCDevCtxt;
   DalTMCHandle DalTMCHandle;
   //TMC Client state can be added by developers here
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TMC_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
//static DALDEVICEID DalTMC_DeviceId[1] = {DALDEVICEID_TMC};

const DALREG_DriverInfo
DALTMC_DriverInfo = { TMC_DeviceAttach, 0, NULL};


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
TMC_AddRef(DalTMCHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

#define IGNORE(x)  ((void) x)


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TMC_DriverInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TMC_DriverDeInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TMC_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult TMC_MemMap(uint32 physAddr,
                            DALSYSMemHandle *phMem,
                            uint32 *virtAddr)
{
   int nErr;
   DALSYSMemInfo memInfo;

   #define TMC_PAGE_SIZE 4096

   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  TMC_PAGE_SIZE, phMem, NULL ));
   DALSYS_MemInfo(*phMem, &memInfo);
   *virtAddr = memInfo.VirtualAddr;
   CATCH(nErr) {}

   return nErr;
}


static
void ETRInit(void)
{
   HAL_qdss_etr_AccessUnLock();
   // Program the MODE Register for Circular Buffer mode.
   HAL_qdss_etr_SetMode(ETR_MODE_CIRCULAR_BUFFER);

   //Program the WrBurstLen field of the AXI Control Register
   HAL_qdss_etr_SetWriteBurstLength(0xF);//recommended value in HPG


   // Program the ScatterGatherMode field of the AXI Control Register (AXICTL) to 0
   HAL_qdss_etr_DisableScatterGather();

   //Set cache and prot ctrl to HPG recommendations
   HAL_qdss_etr_SetCacheProtCtrl();
   HAL_qdss_etr_EnableFormatting();
   HAL_qdss_etr_EnableTraceCapture();

   HAL_qdss_etr_AccessLock();




}


static DALResult
TMC_DeviceInit(DalDeviceHandle *h)
{
   int nErr;
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   uint32 VirtualAddr=0;

   TRY(nErr,TMC_MemMap(QDSS_ETFETB_BASE_PHYS,
                       &pDevCtxt->hMemETFETB,
                       &VirtualAddr));

   HAL_qdss_etfetb_HalConfigInit(VirtualAddr);

   TRY(nErr,TMC_MemMap(QDSS_ETR_BASE_PHYS,
                       &pDevCtxt->hMemETR,
                       &VirtualAddr));

   HAL_qdss_etr_HalConfigInit(VirtualAddr);


   CATCH(nErr) {}
   return nErr;
}


static DALResult
TMC_DeviceDeInit(DalDeviceHandle *h)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   if(pDevCtxt->hMemETFETB) {
      DALSYS_DestroyObject(pDevCtxt->hMemETFETB);
   }

   if(pDevCtxt->hMemETR) {
      DALSYS_DestroyObject(pDevCtxt->hMemETR);
   }
   return DAL_SUCCESS;
}

static DALResult
TMC_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TMC_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTMC_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTMC Interface.
------------------------------------------------------------------------------*/
static  DALResult
TMC_GetMode(DalDeviceHandle * _h,TMCTraceSinkMode *pMode)
{
   uint32 mode = HAL_qdss_etfetb_GetMode();

   switch(mode) {
   case QDSS_TMC_MODE_CBUF:
      *pMode = TMC_TRACESINK_ETB;
      break;
   case QDSS_TMC_MODE_HWFIFO:
      if (HAL_qdss_csr_IsUSBEnabled())  {
         *pMode = TMC_TRACESINK_USB;
      }
      else  {
         *pMode = TMC_TRACESINK_RAM;
      }
      break;
   }

   return DAL_SUCCESS;
}

static  DALResult
TMC_SetMode(DalDeviceHandle * _h,TMCTraceSinkMode mode)
{
   int nErr = DAL_SUCCESS;

   HAL_qdss_etfetb_AccessUnLock();
   switch(mode) {
   case  TMC_TRACESINK_ETB:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
      break;
   case  TMC_TRACESINK_RAM:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      break;
   default:
      nErr = DAL_ERROR;
   }
   HAL_qdss_etfetb_AccessLock();

   return nErr;
}

static  DALResult
TMC_GetTraceBuffer(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   HAL_qdss_etfetb_AccessUnLock();

   *pdwAddr = HAL_qdss_etr_GetDataBufferAddress();
   *pdwBufLen = HAL_qdss_etr_GetRAMSize();

   HAL_qdss_etfetb_AccessLock();

   return DAL_SUCCESS;
}

static  DALResult
TMC_SetTraceBuffer(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{

   HAL_qdss_etr_AccessUnLock();
   HAL_qdss_etr_SetDataBufferAddress(dwAddr);
   HAL_qdss_etr_SetRAMSize(dwBufLen);
   HAL_qdss_etr_AccessLock();

   return DAL_SUCCESS;
}



static DALResult
TMC_EnableTrace( DalDeviceHandle * h)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;

   ETRInit();


   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_etfetb_SetFFCRETBDefault();
   HAL_qdss_etfetb_EnableTraceCapture();
   HAL_qdss_etfetb_AccessLock();


   IGNORE(pClientCtxt);
   return DAL_SUCCESS;
}


static DALResult
TMC_DisableTrace( DalDeviceHandle * h)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;
   HAL_qdss_etfetb_DisableTraceCapture();
   IGNORE(pClientCtxt);
   return DAL_SUCCESS;
}


static DALResult
TMC_ReadTrace( DalDeviceHandle * h, uint32  addr, byte * buf, int  nBufLen, int *pnLenRead)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;
   uint32 dwVal = 0;
   int i = 0;
   uint32 *pdwBuf = (uint32 *)buf;

   HAL_qdss_etfetb_SetReadPtr(addr);

   while (i < nBufLen-4 )  {
      dwVal = HAL_qdss_etfetb_ReadData();
      *pdwBuf = dwVal;
      pdwBuf++;
      i+=4;
   }

   if (pnLenRead) {
      *pnLenRead = i;
   }

  IGNORE(pClientCtxt);
  return DAL_SUCCESS;
}

static DALResult
TMC_Flush( DalDeviceHandle * h, int flags)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;


   HAL_qdss_etfetb_Flush();

   if (flags & TMC_FLUSH_BLOCK) {
      HAL_qdss_etfetb_FlushWait();

   }
   IGNORE(pClientCtxt);
   return DAL_SUCCESS;
}


static DALResult
TMC_GetParam( DalDeviceHandle * h, TMCParamType  param, uint32  uVal)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;

   //Implementation

   IGNORE(pClientCtxt);

   return DAL_ERROR;
}


static DALResult
TMC_SetParam( DalDeviceHandle * h, TMCParamType  param, uint32  uVal)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;

   //Implementation
   IGNORE(pClientCtxt);

   return DAL_SUCCESS;
}


static void
TMC_InitInterface(TMCClientCtxt* pclientCtxt)
{
 static const DalTMC vtbl =
   {
      {
         TMC_DeviceAttach,
         TMC_DeviceDetach,
         TMC_DeviceInit,
         TMC_DeviceDeInit,
         TMC_DeviceOpen,
         TMC_DeviceClose,
         TMC_DeviceInfo,
         TMC_DevicePowerEvent,
         TMC_DeviceSysRequest
      },

      TMC_GetMode,
      TMC_SetMode,
      TMC_GetTraceBuffer,
      TMC_SetTraceBuffer,
      TMC_EnableTrace,
      TMC_DisableTrace,
      TMC_ReadTrace,
      TMC_Flush,
      TMC_GetParam,
      TMC_SetParam
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTMCHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTMCHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTMCHandle.pClientCtxt = pclientCtxt;
}

DALResult
TMC_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static TMCDrvCtxt drvCtxt = {{TMC_DriverInit,
                                      TMC_DriverDeInit
                                     },1,
                                     sizeof(TMCDevCtxt)};
   static TMCClientCtxt clientCtxt;

   TMCClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      TMC_InitInterface(pclientCtxt);
      TMC_AddRef(&(pclientCtxt->DalTMCHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTMCHandle);
   }
   return nErr;
}



