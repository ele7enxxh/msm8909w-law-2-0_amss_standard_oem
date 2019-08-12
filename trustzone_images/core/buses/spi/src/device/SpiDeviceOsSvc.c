/*
===========================================================================

FILE:   SpiDeviceOsSvc.c

DESCRIPTION:
    This file contains the implementation for the QUP OS Services 
    Interface: IQupSpiDevice 

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/SpiDeviceOsSvc.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
05/15/13 ag	Created

===========================================================================
        Copyright c 2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDeviceOsSvc.h"
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"


typedef enum SpiDeviceOs_HandleType
{
   SpiDeviceOs_EventHandleType,
   SpiDeviceOs_CritSecHandleType,
} SpiDeviceOs_HandleType;

typedef struct SpiDeviceOs_EventHandle
{
   SpiDeviceOs_HandleType    hType;
   DALSYSEventHandle      hQupEvents[2];
   DALSYSEventObj         aQupEventObj[2];

} SpiDeviceOs_EventHandle;

typedef struct SpiDeviceOs_SyncHandle
{
   SpiDeviceOs_HandleType    hType;
   DALSYSSyncHandle  hDeviceSynchronization;
   DALSYS_SYNC_OBJECT(DeviceSyncObject);

} SpiDeviceOs_SyncHandle;

typedef struct SpiDeviceOs_Handle
{
   SpiDeviceOs_HandleType    hType;
} SpiDeviceOs_Handle;

typedef struct SpiDeviceOs_IrqHandle
{
   ISR_HOOK IsrHook;
   IST_HOOK IstHook;
   void     *pIsrData;
   void     *pIstData;
   uint32   uInterruptId;
   uint32    uIrqTriggerCfg;
} SpiDeviceOs_IrqHandle;


SpiDeviceOsSvc_Result
SpiDeviceOsSvc_CreateEvent(SPIDEVICEOSSVC_EVENT_HANDLE *phEvt)
{
   return SPIDEVICEOSSVC_RESULT_OK;
}

SpiDeviceOsSvc_Result
SpiDeviceOsSvc_DestroyEvent(SpiDeviceOs_EventHandle *pEvt)
{
   return SPIDEVICEOSSVC_RESULT_OK;
}



SpiDeviceOsSvc_Result
SpiDeviceOsSvc_SetEvent(SPIDEVICEOSSVC_EVENT_HANDLE hEvent)
{
   return SPIDEVICEOSSVC_RESULT_OK;
}

SpiDeviceOsSvc_Result
SpiDeviceOsSvc_ClearEvent(SPIDEVICEOSSVC_EVENT_HANDLE hEvt)
{
   return SPIDEVICEOSSVC_RESULT_OK;
}


SpiDeviceOsSvc_Result
SpiDeviceOsSvc_CloseHandle(SPIDEVICEOSSVC_EVENT_HANDLE hEvt)
{
   return SPIDEVICEOSSVC_RESULT_OK;
}


SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Malloc(void **pBuffer,uint32 size)
{
   DALResult dalRes;
   dalRes = DALSYS_Malloc(size , pBuffer);
   if (DAL_SUCCESS != dalRes) {
      return SPIDEVICEOSSVC_RESULT_ERROR_MEM_MALLOC_FAILED;
   }
   //FIXME: I'm adding this a additional protection
   //temporarily. But users should use SpiDeviceOsSvc_Malloc
   //and SpiDeviceOsSvc_Memset seperately.
   DALSYS_memset(*pBuffer, 0, size);
   return SPIDEVICEOSSVC_RESULT_OK;
}

SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Memset(void *buffer, int32 val, uint32 size)
{
   DALSYS_memset( buffer, val, size);
   return SPIDEVICEOSSVC_RESULT_OK;
}

SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Free(void *pBuffer)
{
   DALResult dalRes;
  
   dalRes = DALSYS_Free(pBuffer);
   if (DAL_SUCCESS != dalRes) {
      return SPIDEVICEOSSVC_RESULT_ERROR_MEM_FREE_FAILED;
   }
 
   return SPIDEVICEOSSVC_RESULT_OK;
}


void SpiDeviceOsSvc_BusyWait(uint32 uTimeMicrosec)
{
   DALSYS_BusyWait( uTimeMicrosec );
}


/**
 * @brief Allocate physical memory
 *
 * This function allocates physical memory
 * 
   @param[in] uLen  Length of the physical memory to allocate in
                bytes
 * @param[out] phMem  Location to store the physical memory 
 *       handle
 * 
 * @return SPIDEVICEOSSVC_RESULT_OK on success, error code on error
 */
SpiDeviceOsSvc_Result SpiDeviceOsSvc_PhysMemAlloc
(
   uint32 uLen, 
   void** phMem , 
   uint32 *puVirtAddr, 
   uint32 *puPhysAddr
)
{
   SpiDeviceOsSvc_Result result = SPIDEVICEOSSVC_RESULT_OK;
   int32 res;   
   DALSYSMemInfo MemInfo;

   do
   {
      if ((NULL == puPhysAddr) || (NULL == puVirtAddr))
      {
         result = SPIDEVICEOSSVC_RESULT_ERROR_NULL_PTR;
         break;
      }

      res  = DALSYS_MemRegionAlloc(
                    DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                    DALSYS_MEM_ADDR_NOT_SPECIFIED,
                    DALSYS_MEM_ADDR_NOT_SPECIFIED,
                    uLen, phMem, NULL);
      
      if (res != DAL_SUCCESS)
      {
         result = SPIDEVICEOSSVC_RESULT_ERROR_MEM_MALLOC_FAILED;
         break;
      }
   
      res = DALSYS_MemInfo(*phMem, &MemInfo);

      if (res != DAL_SUCCESS)
      {
         result = SPIDEVICEOSSVC_RESULT_ERROR_MEM_MALLOC_FAILED;
         break;
      }

      *puVirtAddr = MemInfo.VirtualAddr;
      *puPhysAddr = MemInfo.PhysicalAddr;
   }while(0);
   return   result;
}



/**
 * @brief Free up previously allocated physical memory.
 *
 * This function frees up previously allocated physical memory
 * 
 * @param[in] phMem  Pointer to the location of physical memory 
 *                   handle
 * 
 * @return SPIDEVICEOSSVC_RESULT_OK on success, error code on error
 */
SpiDeviceOsSvc_Result SpiDeviceOsSvc_PhysMemFree(void* phMem)
{
   SpiDeviceOsSvc_Result result = SPIDEVICEOSSVC_RESULT_OK;
   int32 res;   

   do
   {
      if (NULL == phMem)
      {
         result = SPIDEVICEOSSVC_RESULT_ERROR_NULL_PTR;
         break;
      }

      res  = DALSYS_DestroyObject(phMem);
      
      if (res != DAL_SUCCESS)
      {
         result = SPIDEVICEOSSVC_RESULT_ERROR_MEM_FREE_FAILED;
         break;
      }
   
   }while(0);
   return   result;
}

