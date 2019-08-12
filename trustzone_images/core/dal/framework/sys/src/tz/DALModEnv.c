/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/dal/framework/sys/src/tz/DALModEnv.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $


when       who     description
--------   ---     ----------------------------------------------------------
02/04/15   sg      Ported DALSYS_DestroyObject
05/24/12   sc      Ported DevCfg
02/03/12   aa      DAL heap allocation done based on SCL configuration.
07/15/10   sc      Added busy wait support
07/12/10   SHO     Allow the first call to DALSYS_InitMod to initialize and 
                   ignore all subsequent calls
04/20/10   SHO     Create this file based on the boot version 
 
==============================================================================*/
#include "DALSys.h"
#include "DALSysCmn.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#include "DALReg.h"
#include "DALFramework.h"
#include "memheap_lite.h"
#include "stringl/stringl.h"

#if defined (DAL_SYSDRV_BUSYWAIT)
#include "busywait.h"
#else
#include "clk.h"
#endif

//
// must statically link against:
//    dalprops.c
//

//List of DALs in this boot-image.
extern DALREG_DriverInfoList gDALModDriverInfoList;
extern DALProps DALPROP_PropsInfo;
extern DALProps DAL_Mod_Info;

/*------------------------------------------------------------------------------
Pointer to DALSYS function pointer table
------------------------------------------------------------------------------*/
//static DALSYSConfig gDALSysCfg={NULL,0x00};


/*------------------------------------------------------------------------------
Per process global that holds the list of drivers modules
------------------------------------------------------------------------------*/
DALREG_DriverInfoList * gDALProcessDriverModList[1];  // only one module for bootloader

// Dummy placeholder for system device

static DALDEVICEID DalSystem_DeviceId[1] = {DALDEVICEID_SYSTEM};

static int modHasInit;

static int heapHasInit;

/*=============================================================================
      Structures
=============================================================================*/

typedef struct _DALSYSEventObj _DALSYSEventObj;
struct _DALSYSEventObj
{
   INHERIT_DALSYS_BASE_OBJ();
   DALSYSCallbackFunc    cbFunc;
   DALSYSCallbackFuncCtx cbFuncCtx;
};

DALResult
System_DalSystem_Attach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;
   return DAL_ERROR;
}

const DALREG_DriverInfo
DALSystem_DriverInfo = { System_DalSystem_Attach,
                         0,
                         DalSystem_DeviceId
                       };

// Use DAL Heap Manager 
//static DALHeap tzHeapDesc;
static mem_heap_type tzHeapDesc;
extern uint32  Image$$TZBSP_DAL_HEAP$$Base;
extern uint32  Image$$TZBSP_DAL_HEAP$$Length;
/*Memheap magic number arrays which needs to be passed in to all the memheap functions*/
static uint32 magic_num[MAX_HEAP_INIT] = {(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1};
static uint16 magic_num_index_array[MAX_HEAP_INIT] = {0,1,2,3,4};
static mem_magic_number_struct mem_magic_number;

// ensure memory section is 32-byte aligned for DMA and other applications
// mask to round up to 32
#define MASK_32  0x01f
// round up s to the nearest 32-byte aligned address
#define ALIGN_32( s ) \
   ( ( (DALSYSMemAddr)(s) + MASK_32) & ~MASK_32)
// minimum extra bytes required to shift a 4-byte aligned address to a 
// 32-byte aligned address 
#define ALIGN_32_PADDING_LENGTH ( MASK_32 - 3 )

// reuse this for all dynamic dalsys objects (except mem)
static DALEventObject commonObject;


/*--------------------------------------------------------------*/
/*------------------------ MMU/Cache Support -------------------*/
/*--------------------------------------------------------------*/

// Since MMU is off these macros do nothing
#define OALVAtoUA( va ) (va)
#define OALVAtoPA( va ) (va)

/*--------------------------------------------------------------*/
//---- cache commands in QCOM bootloaders
//Since MMU is off these macros do nothing
static void DAL_Dcache_Inval_Region(DALSYSMemAddr addr, uint32 length)
{
   return;
}

static void DAL_Dcache_Clean_Region(DALSYSMemAddr addr, uint32 length)
{
   return;
}

static void DAL_Dcache_Flush_Region(DALSYSMemAddr addr, uint32 length)
{
   return;
}

static uint32 updateUncachedMemProps( uint32 memProps )
{
   return memProps;
}
/*--------------------------------------------------------------*/
/*------------------- End-Of MMU/Cache Support -----------------*/
/*--------------------------------------------------------------*/

uint32 GetPID()
{
   return 0xDEFADEFA;   // default PID
}

void
DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir)
{
    *pDALPropsDir = (DALPropsDir *)DALPROP_PropsInfo.pDALPROP_PropBin;
}

const void **
DALSYS_GetPropStructPtrArray()
{
   return DALPROP_PropsInfo.pDALPROP_StructPtrs;
}

void
DALSYS_InitMod(DALSYSConfig * pCfg)
{
   if (!modHasInit)
   {
      // DALSYS module is only initialized on the first call, 
      // subsequent calls are ignored
      //if(DAL_SUCCESS != mem_init_heap( &tzHeapDesc, (void *) &Image$$TZBSP_DAL_HEAP$$Base, (uint32) &Image$$TZBSP_DAL_HEAP$$Length))
	    // return;      
      gDALProcessDriverModList[0] = &gDALModDriverInfoList;
      modHasInit = 1;
   }
}

void
DALSYS_DeInitMod(void)
{
}

DALResult
DALSYS_CopyObject(DALSYSObjHandle hObjOrig, DALSYSObjHandle *phObjCopy )
{
   return DAL_ERROR;
}

DALResult
DALSYS_RegisterWorkLoop(uint32 dwPriority,
                        uint32 dwMaxNumEvents,
                        DALSYSWorkLoopHandle *phWorkLoop,
                        DALSYSWorkLoopObj *pWorkLoopObj)
{
   return DAL_ERROR;
}

DALResult
DALSYS_AddEventToWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                          DALSYSWorkLoopExecute pfnWorkLoopExecute,
                          void * pArg,
                          DALSYSEventHandle hEvent,
                          DALSYSSyncHandle hSync)
{
   return DAL_ERROR;
}

DALResult
DALSYS_DeleteEventFromWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                               DALSYSEventHandle hEvent)
{
   return DAL_ERROR;
}


DALResult
DALSYS_EventCreate(uint32 dwEvtAttrib, DALSYSEventHandle *phEvent,
                   DALSYSEventObj *pEventObj)
{
   _DALSYSEventObj *_pEventObj = (_DALSYSEventObj *)pEventObj;
   // only support statically pre-allocated object
   if((_pEventObj == NULL) || (phEvent == NULL ))
      return DAL_ERROR;

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_CALLBACK_EVENT)
   {
      _pEventObj->dwObjInfo = 0;
      DALSYSCMN_SET_OBJECT_ID(_pEventObj->dwObjInfo,DALSYSCMN_EVENT_OBJECT_ID);
      DALSYSCMN_SET_CALLBACK_EVENT_BIT(_pEventObj->dwObjInfo);
   }
   else
      return DAL_ERROR;

   *phEvent = (DALSYSEventHandle)_pEventObj;

   return DAL_SUCCESS;
}

DALResult
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam,
                   void *pPayload, uint32 dwPayloadSize)
{
   DALResult nErr = DAL_ERROR;
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);
   if(pEventObj)
   {
      switch(dwCtrl)
      {
         case DALSYS_EVENT_CTRL_TRIGGER:
            if(DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
            {
                if(pEventObj->cbFunc)
                {
                    pEventObj->cbFunc(pEventObj->cbFuncCtx,
                                dwParam,pPayload,dwPayloadSize);
                    nErr = DAL_SUCCESS;
                }
            }
            break;

         default:
            nErr = DAL_SUCCESS;
            break;
      }
   }
   return nErr;
}

DALResult
DALSYS_EventWait(DALSYSEventHandle hEvent)
{
   return DAL_ERROR;
}

DALResult
DALSYS_EventMultipleWait(DALSYSEventHandle* phEvent, int nEvents,
                         uint32 dwTimeoutUs, uint32 *pdwEventIdx)
{
   return DAL_ERROR;
}

DALResult
DALSYS_SetupCallbackEvent(DALSYSEventHandle hEvent, DALSYSCallbackFunc cbFunc,
                          DALSYSCallbackFuncCtx cbFuncCtx)
{
  _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);

   if(pEventObj && DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
   {
      pEventObj->cbFunc = cbFunc;
      pEventObj->cbFuncCtx = cbFuncCtx;
      return DAL_SUCCESS;
   }
   return DAL_ERROR;
}

DALResult
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj)
{
   if(pSyncObj == NULL)
      *phSync = (DALSYSSyncHandle)&commonObject;
   else
      *phSync = (DALSYSSyncHandle)pSyncObj;
   return DAL_SUCCESS;
}

void
DALSYS_SyncEnter(DALSYSSyncHandle hSync)
{
}

void
DALSYS_SyncLeave(DALSYSSyncHandle hSync)
{
}

#define DALSYSMEM_PHYS_VIRT_ADDR_SPECIFIED   0x0
#define DALSYSMEM_VIRT_ADDR_SPECIFIED        0x2
#define DALSYSMEM_PHYS_ADDR_SPECIFIED        0x1
#define DALSYSMEM_NO_ADDR_SPECIFIED          0x3

typedef struct _DALSYSMemObj _DALSYSMemObj;

struct _DALSYSMemObj
{
    DALSYSMemObjBase BaseObj;
    void *pMemRegion;
};

DALResult DALSYS_DestroyMemObject(DALSYSMemHandle hMem)
{
   _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);

   if( !pMemObj || (DALSYSCMN_MEM_OBJECT_ID != 
            DALSYSCMN_GET_OBJECT_ID(pMemObj->BaseObj.dwObjInfo))) return DAL_ERROR;

   /*----------------------------------------------------------------------
    If DALSYS Mem object dynamically alloced free structure memory,
    otherwise reset the memory area to ZERO
    ----------------------------------------------------------------------*/
   if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pMemObj->BaseObj.dwObjInfo))
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
	  if(DAL_SUCCESS == DALSYS_Free(pMemObj->pMemRegion))
		return DALSYS_Free(pMemObj);
	  else
		return DAL_ERROR ;
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
      return DAL_SUCCESS;
   }
}

DALResult DALSYS_DestroyObject(DALSYSObjHandle hObj)
{
	DALSYSBaseObj * pSysObj = (DALSYSBaseObj *) DALSYSCMN_HANDLE_TO_OBJECT(hObj);
	if(pSysObj && DALSYSCMN_MEM_OBJECT_ID == DALSYSCMN_GET_OBJECT_ID(pSysObj->dwObjInfo))
			return DALSYS_DestroyMemObject(hObj);
   return DAL_SUCCESS;
}

static void *
_CreateMemSection(uint32 dwLen, uint32 dwAttribs,
                  DALSYSMemAddr *VirtualAddr,DALSYSMemAddr *PhysicalAddr)
{
    uint32 dwCmd = ((DALSYS_MEM_ADDR_NOT_SPECIFIED  == *PhysicalAddr)<<1)
                    |(DALSYS_MEM_ADDR_NOT_SPECIFIED == *VirtualAddr);
    void *pMemRegion = NULL;

    //Only support no addr specified in this environemt
    switch(dwCmd)
    {
    case DALSYSMEM_NO_ADDR_SPECIFIED:
         if(DAL_SUCCESS != DALSYS_Malloc(dwLen + ALIGN_32_PADDING_LENGTH,
                  &pMemRegion))
         {
             pMemRegion = NULL;
         }
         *VirtualAddr = (DALSYSMemAddr)ALIGN_32(pMemRegion);
         *PhysicalAddr = *VirtualAddr;
         break;
    default:
        pMemRegion = NULL;
    }
    return pMemRegion;
}


static
_DALSYSMemObj *_AllocAndInitMemObj(_DALSYSMemObj *pMemObj,
                    uint32 dwLen,
                    uint32 dwAttribs,
                    void * pMemRegion,
                    DALSYSMemAddr VirtualAddr,
                    DALSYSMemAddr PhysicalAddr)
{
    if(NULL == pMemObj)
    {
        if(DAL_SUCCESS == DALSYS_Malloc(DALSYS_MEM_OBJ_SIZE_BYTES,
                                         (void **)&pMemObj))
        {
            DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
            DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pMemObj->BaseObj.dwObjInfo);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
    }

    DALSYSCMN_SET_OBJECT_ID(pMemObj->BaseObj.dwObjInfo,DALSYSCMN_MEM_OBJECT_ID);

    // Update mem properties if we are asking for uncached
    dwAttribs = updateUncachedMemProps(dwAttribs);

    DALSYSCMN_SET_ATTRIB(pMemObj->BaseObj.dwObjInfo,dwAttribs);

    DALSYSCMN_RESET_MEM_COPY_BIT(pMemObj->BaseObj.dwObjInfo);

    pMemObj->BaseObj.dwLen        = dwLen;
    pMemObj->BaseObj.hOwnerProc   = GetPID();
    pMemObj->BaseObj.thisVirtualAddr = (DALSYSMemAddr)pMemObj;
    pMemObj->BaseObj.OwnerVirtualAddr = VirtualAddr;
    pMemObj->BaseObj.VirtualAddr = OALVAtoUA(VirtualAddr);
    pMemObj->BaseObj.PhysicalAddr = OALVAtoPA(VirtualAddr);

    // at his point if VirtualAddr or PhysicalAddr == NULL this is fatal
    if((pMemObj->BaseObj.VirtualAddr == NULL) || (pMemObj->BaseObj.PhysicalAddr == NULL))
    {
       DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, "MMU SW Config Error");
       return NULL;
    }

    pMemObj->pMemRegion = pMemRegion;
    return pMemObj;
}

 
DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr VirtualAddr,
                      DALSYSMemAddr PhysicalAddr, uint32 dwLen, DALSYSMemHandle *phMem,
                      DALSYSMemObj *pObj)
{
   void * pMemRegion = NULL;
   _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)pObj;
   *phMem = NULL;

   if((dwAttribs & DALSYS_MEM_PROPS_PHYS_PREALLOC))
   {
		//User has passed preallocated memory, only Alloc Memory for the DALSYS Mem Obj
		
		pMemObj = _AllocAndInitMemObj(pMemObj,dwLen,dwAttribs,pMemRegion,
                                        VirtualAddr,PhysicalAddr);	
		 if(NULL == pMemObj)
			return DAL_ERROR;
	
		// Return handle
		*phMem = (DALSYSMemHandle)pMemObj;
		return DAL_SUCCESS;
   
   }
  else
  {
	// only support uncached and phy cont memory in TZ	
	/*----------------------------------------------------------------------
	Create the underlying memsection
	----------------------------------------------------------------------*/
	pMemRegion = _CreateMemSection(dwLen,dwAttribs,&VirtualAddr,&PhysicalAddr);
	if(NULL == pMemRegion)
		return DAL_ERROR;

	/*----------------------------------------------------------------------
	Alloc Memory for the DALSYS Mem Obj
	----------------------------------------------------------------------*/
	pMemObj = _AllocAndInitMemObj(pMemObj,dwLen,dwAttribs,pMemRegion,
                                VirtualAddr,PhysicalAddr);
	if(NULL == pMemObj)
	{
		DALSYS_Free(pMemRegion);
		return DAL_ERROR;
	}

	// Return handle
	*phMem = (DALSYSMemHandle)pMemObj;
	return DAL_SUCCESS;
  }
}



DALResult
DALSYS_MemRegionMapPhys(DALSYSMemHandle hMem, uint32 dwVirtualBaseOffset,
                        DALSYSMemAddr PhysicalAddr, uint32 dwLen)
{
   return DAL_SUCCESS;
}

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
        pMemInfo->VirtualAddr  = pMemObj->BaseObj.VirtualAddr;
        pMemInfo->PhysicalAddr = pMemObj->BaseObj.PhysicalAddr;
        pMemInfo->dwLen        = pMemObj->BaseObj.dwLen;
        pMemInfo->dwMappedLen  = pMemObj->BaseObj.dwLen;
        DALSYSCMN_GET_ATTRIB(pMemObj->BaseObj.dwObjInfo,pMemInfo->dwProps);
        return DAL_SUCCESS;
    }
    return DAL_ERROR;
}

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
   switch(CacheCmd)
   {
      case DALSYS_CACHE_CMD_INVALIDATE:
         DAL_Dcache_Inval_Region(VirtualAddr, dwLen);
         break;
      case DALSYS_CACHE_CMD_CLEAN:
         DAL_Dcache_Clean_Region(VirtualAddr, dwLen);
         break;
      case DALSYS_CACHE_CMD_FLUSH:
         DAL_Dcache_Flush_Region(VirtualAddr, dwLen);
         break;
      default:
         return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
   if (NULL == ppMem)
   {
      return DAL_ERROR;
   }
   if(!heapHasInit)
   {
      mem_magic_number.magic_num=&magic_num[0];
      mem_magic_number.magic_num_index_array=&magic_num_index_array[0];
      mem_magic_number.magic_num_index=0;
		if(MEMHEAP_SUCCESS !=mem_init_heap( &tzHeapDesc, &mem_magic_number,(void *) &Image$$TZBSP_DAL_HEAP$$Base, (uint32) &Image$$TZBSP_DAL_HEAP$$Length))
			return DAL_ERROR;
		heapHasInit=1;
	}
   *ppMem = mem_malloc( &tzHeapDesc, &mem_magic_number,dwSize );
   if(*ppMem)
   {
      return DAL_SUCCESS;
   }
   DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, "OUT OF MEMORY");
   return DAL_ERROR;
}

DALResult
DALSYS_Free(void *pmem)
{
   mem_free( &tzHeapDesc,&mem_magic_number, pmem );
   return DAL_SUCCESS;
}

void
DALSYS_BusyWait(uint32 pause_time_us)
{
#if defined (DAL_SYSDRV_BUSYWAIT)
   busywait(pause_time_us);
#else // invoke legacy clk_busy_wait which maps to clk_pause in clk.h
   clk_pause(pause_time_us);
#endif
}

DALResult
DALSYS_MemDescAddBuf(DALSysMemDescList * pMemDesc, uint32 dwMemDescBufIdx,
                          uint32 dwOffset, uint32 dwSize)
{
   return DAL_ERROR;
}


DALResult
DALSYS_MemDescPrepare(DALSysMemDescList * pMemDesc, uint32 dwBufIdx)
{
   return DAL_ERROR;
}

DALResult
DALSYS_MemDescValidate(DALSysMemDescList * pMemDesc,uint32 dwBufIdx)
{
   return DAL_ERROR;
}

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   return _DALSYS_GetDALPropertyHandle(DeviceId,hDALProps);
}

DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   return _DALSYS_GetDALPropertyHandleStr(pszDevName, hDALProps);
}

DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                        uint32 dwId,
                        DALSYSPropertyVar *pDALPropVar)
{
   return _DALSYS_GetPropertyValue( hDALProps, pszName, dwId, pDALPropVar);
}

void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, const char * pszFmt, ...)
{
}

void
DALSYS_InitSystemHandle(DalDeviceHandle *hDalDevice)
{
}


static DALResult
_DALSYSCMN_GetDeviceObject(DALDEVICEID DevId, const char *pszArg, DalDeviceHandle **phDevice)
{
   DALREG_DriverInfoList *pModDriverInfoList = gDALProcessDriverModList[0];

   uint32 dwDriverIdx = 0;
   *phDevice = NULL;

   /*------------------------------------------------------------------
     For all drivers of this mod search for requested device id
    -----------------------------------------------------------------*/
   for(dwDriverIdx =0;dwDriverIdx<pModDriverInfoList->dwLen;dwDriverIdx++)
   {
      /*--------------------------------------------------------------
      For each devices supported by this driver
      --------------------------------------------------------------*/
      uint32 dwDeviceIdx = 0;
      DALREG_DriverInfo *pDriverInfo = pModDriverInfoList->pDriverInfo[dwDriverIdx];
      for(dwDeviceIdx=0;dwDeviceIdx<pDriverInfo->dwNumDevices;dwDeviceIdx++)
      {
         if(DevId == pDriverInfo->pDeviceId[dwDeviceIdx])
         {
            /*------------------------------------------------------
            search complete
            ------------------------------------------------------*/
            return
            pDriverInfo->pfnDALNewFunc(pszArg,DevId,phDevice);
         }
      }
   }
   /*--------------------------------------------------------------------------
   Did not find the requested device Id in this process
   --------------------------------------------------------------------------*/
   return DAL_ERROR;
}

static DALProps *
DALSYS_GetDAL_Mod(void)
{
   return (DALProps *)&DAL_Mod_Info;
}

static DALResult
_DALSYSCMN_GetStringDeviceObject(const char *pszDevName, const char *pszArg, DalDeviceHandle **phDevice)
{ 
   const StringDevice *pStringDeviceObj = NULL;
   DALREG_DriverInfo *pDriverInfo = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, DALSYS_GetDAL_Mod())){
      //Get the Property Dir Info Ptr
      pDriverInfo = pStringDeviceObj->pFunctionName;
      return pDriverInfo->pfnDALNewFunc(pszDevName,0,phDevice);
   } 
   return DAL_ERROR;
}

DALResult
DAL_DeviceAttachEx(const char *pszArg,
      DALDEVICEID DevId,
      DALInterfaceVersion ClientVersion,
      DalDeviceHandle **phDALDevice)
{
   DalDeviceInfo DeviceInfo;
   DALResult ret = DAL_DeviceAttachLocal(pszArg,DevId,phDALDevice);

   if(DAL_SUCCESS != ret)
   {
        // no support for remote
        *phDALDevice = NULL;
        return ret;
   }

   if(DAL_SUCCESS != DalDevice_Info(*phDALDevice,&DeviceInfo,
                                        sizeof(DeviceInfo)))
                return DAL_ERROR;

   //do version check
   if(DALINTERFACE_GET_MAJOR_VERSION(ClientVersion)
          != DALINTERFACE_GET_MAJOR_VERSION(DeviceInfo.Version))
   {
           //failed Major version check, interface change
           (void)DAL_DeviceDetach(*phDALDevice);
           *phDALDevice = NULL;
           return DAL_ERROR;
   }
   //has to be success...
   return DAL_SUCCESS;
}

DALResult
DAL_DeviceAttach( DALDEVICEID DevId,
                  DalDeviceHandle **phDalDevice)
{

   DALResult ret = _DALSYSCMN_GetDeviceObject(DevId, NULL, phDalDevice);

   if(DAL_SUCCESS == ret)
   {
       //if a remote handle the remote side does the Init
       if(!DALISREMOTEHANDLE(*phDalDevice))
       {
          if(1 == DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice))
          {
             ret = DalDevice_Init(*phDalDevice);
             if(DAL_SUCCESS != ret)
             {
                DAL_DeviceDetach(*phDalDevice);
                *phDalDevice = NULL;
             }
          }
       }
   }
   return ret;
}


DALResult
DAL_DeviceDetach(DalDeviceHandle * hDalDevice)
{
    if(!DALISREMOTEHANDLE(hDalDevice))
    {
       if(1 == DAL_GET_DEVICE_ATTACH_COUNT(hDalDevice))
       {
           (void)DalDevice_DeInit(hDalDevice);
       }
    }
    return DalDevice_Detach(hDalDevice);
}

DALResult
DAL_DeviceAttachRemote(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDALDevice)
{
   return DAL_ERROR;
}


DALResult
DAL_DeviceAttachLocal(const char *pszArg,DALDEVICEID DevId,
                      DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;

   if(DAL_SUCCESS == _DALSYSCMN_GetDeviceObject(DevId,pszArg,phDalDevice))
   {
       //if a remote handle the remote side does the Init
       if(!DALISREMOTEHANDLE(*phDalDevice))
       {
          if(1 == DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice))
          {
              DALResult ret = DalDevice_Init(*phDalDevice);
              if(DAL_SUCCESS != ret)
              {
                 DAL_DeviceDetach(*phDalDevice);
                 *phDalDevice = NULL;
                 return ret;
              }
          }
       }
       return DAL_SUCCESS;
   }
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

DALResult
DAL_StringDeviceAttach(const char *pszDevName,
                  DalDeviceHandle **phDalDevice)
{
  return DAL_StringDeviceAttachLocal(NULL, pszDevName, phDalDevice); 
}

DALResult
DAL_StringDeviceAttachLocal(const char *pszArg, const char *pszDevName,
                      DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;

   if(DAL_SUCCESS == _DALSYSCMN_GetStringDeviceObject(pszDevName,pszArg,phDalDevice))
   {
       //if a remote handle the remote side does the Init
       if(!DALISREMOTEHANDLE(*phDalDevice))
       {
          if(1 == DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice))
          {
              DALResult ret = DalDevice_Init(*phDalDevice);
              if(DAL_SUCCESS != ret)
              {
                 DAL_DeviceDetach(*phDalDevice);
                 *phDalDevice = NULL;
                 return ret;
              }
          }
       }
       return DAL_SUCCESS;
   }
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

DALResult
DAL_StringDeviceAttachEx(const char *pszArg,
                   const char *pszDevName,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDalDevice)
{
   DalDeviceInfo DeviceInfo;
   DALResult ret = DAL_StringDeviceAttachLocal(pszArg,pszDevName,phDalDevice);
   if(DAL_SUCCESS != ret)
   {
      // no support for remote
      *phDalDevice = NULL;
      return ret;
   }

   if (DAL_SUCCESS != DalDevice_Info(*phDalDevice,&DeviceInfo,
                                     sizeof(DeviceInfo)))
      return DAL_ERROR;

   //do version check
   if (DALINTERFACE_GET_MAJOR_VERSION(ClientVersion) !=
       DALINTERFACE_GET_MAJOR_VERSION(DeviceInfo.Version))
   {
      //failed Major version check, interface change
      (void)DAL_DeviceDetach(*phDalDevice);
      *phDalDevice = NULL;
      return DAL_ERROR;
   }
   //has to be success...
   return DAL_SUCCESS;
}

uint32 _DALSYS_memscpy(void * pDest, uint32 iDestSz, 
      const void * pSrc, uint32 iSrcSize)
{
   return memscpy(pDest, iDestSz, pSrc, iSrcSize);
}
