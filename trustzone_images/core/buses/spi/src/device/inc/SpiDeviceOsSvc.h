#ifndef _SPIDEVICEOSSVC_H_
#define _SPIDEVICEOSSVC_H_
/*
===========================================================================

FILE:   SpiDeviceOs.h

DESCRIPTION:
    This file contains the API for the QUP OS Services 
    Interface: IQupSpiDevice 

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/inc/SpiDeviceOsSvc.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
05/15/13 ag     Created

===========================================================================
        Copyright c 2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"

/**
   Some of the most common errors may be defined here.
   Each OS defines its own values which will be interpreted
   differently based on OS.
 */
typedef enum {
   SPIDEVICEOSSVC_RESULT_OK = 0,
   SPIDEVICEOSSVC_RESULT_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_DEV_OS,
   SPIDEVICEOSSVC_RESULT_ERROR_TIMEOUT,
   SPIDEVICEOSSVC_RESULT_ERROR_EVT_CREATE_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_EVT_DESTROY_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_MEM_FREE_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_MEM_MALLOC_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_EVT_CLEAR_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_EVT_CTRL_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_EVT_WAIT_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_SYNC_CREATE_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_SYNC_DESTROY_FAILED,
   SPIDEVICEOSSVC_RESULT_ERROR_INVALID_HANDLE_TYPE,
   SPIDEVICEOSSVC_RESULT_ERROR_NULL_PTR,

   SPIDEVICEOSSVC_RESULT_ERROR_REGISTER_IST_HOOKS_START,

   SPIDEVICEOSSVC_RESULT_ERROR_REGISTER_IST_HOOKS_EXISTS,
   SPIDEVICEOSSVC_RESULT_ERROR_REGISTER_IST_HOOKS_END = SPIDEVICEOSSVC_RESULT_ERROR_REGISTER_IST_HOOKS_START+15,

} SpiDeviceOsSvc_Result;

typedef void* SPIDEVICEOSSVC_EVENT_HANDLE;

typedef void* SPIDEVICEOSSVC_CRITSECTION_HANDLE;

typedef uint32 (*IST_HOOK)(void *pdata);
typedef uint32 (*ISR_HOOK)(void *pdata);

/** @brief Creates an event object and sets handle pointer.

    @param[out] pEventHandle  Pointer to event handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_CreateEvent(SPIDEVICEOSSVC_EVENT_HANDLE *pEventHandle);

/** @brief Closes a handle destroying the object associated with it.

    @param[in] hEvent  Event handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_CloseHandle(SPIDEVICEOSSVC_EVENT_HANDLE hEvent);

/** @brief Signals the event object pointed to by the handle.

    @param[in] hEvent  Event handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_SetEvent(SPIDEVICEOSSVC_EVENT_HANDLE hEvent);

/** @brief Clears outstanding signals on the event object.

    @param[in] hEvent  Event handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_ClearEvent(SPIDEVICEOSSVC_EVENT_HANDLE hEvent);

/** @brief Wait with a timeout on the event object.

    @param[in] hEvent  Event handle.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Wait(SPIDEVICEOSSVC_EVENT_HANDLE hEvent, uint32 dwMilliseconds);

/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSecHandle  Pointer to critical section handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_CreateCriticalSection(SPIDEVICEOSSVC_CRITSECTION_HANDLE *pCritSecHandle);

/** @brief Enters the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_EnterCriticalSection(SPIDEVICEOSSVC_CRITSECTION_HANDLE hCriticalSection);

/** @brief Leaves the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_LeaveCriticalSection(SPIDEVICEOSSVC_CRITSECTION_HANDLE hCriticalSection);

/** @brief Allocates a memory buffer of the given size and sets the pointer to it.

    @param[out] ppBuffer pointer to pointer to allocated memory.
    @param[in]  size     size of the memory to be allocated.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Malloc(void **ppBuffer,uint32 size);

/** @brief Frees memory allocated by SpiDeviceOsSvc_Malloc.

    @param[in] pBuffer  pointer to allocated memory.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Free(void *pBuffer);

/** @brief Memset operation.

    @param[in] buffer input buffer 
    @param[in] val    memset value.
    @param[in] size   size of buffer or desired length of memset
    	  operation.
  @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_Memset(void *buffer, int32 val, uint32 size);

/** @brief Registers IST/ISR hooks for the indicated device.

    @param[in] uDevId   Device index.
    @param[in] IsrHook  ISR hook.
    @param[in] pIsrData Pointer to data handed back when ISR is called.
    @param[in] IstHook  IST hook, thread handling the request.
    @param[in] pIstData Pointer to data handed back when IST is called.
  @return          SpiDeviceOsSvc_Result .
   */ 
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_RegisterISTHooks(uint32 uDevId, ISR_HOOK IsrHook, void* pIsrData, IST_HOOK IstHook, void* pIstData);

/** @brief Unregisters IST/ISR hooks for the indicated device.

    @param[in] uDevId   Device index.
  @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result
SpiDeviceOsSvc_UnRegisterISTHooks(uint32 uDevId);

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @return          Nothing.
  */
void
SpiDeviceOsSvc_BusyWait(uint32 uTimeMicrosec);

/** @brief Allocates a memory buffer, which contiguous and
           suitable for DMA and returns the Physical and virtual
           memory of the allocated address.

    @param[in] uLen, size of the of memory address. 
    @param[out]phMem memory handle to the allocated memory.
          SpiDeviceOsSvc_PhysMemFree needs to be called with
          handle.
    @param [out] puVirtAddr pointer to the virtual address of
           the allocated memory.
    @param [out] puPhysAddr pointer to the physical address of
           the allocated memory.
    @return          SpiDeviceOsSvc_Result .
  */
SpiDeviceOsSvc_Result SpiDeviceOsSvc_PhysMemAlloc
(
   uint32 uLen, 
   void** phMem , 
   uint32 *puVirtAddr, 
   uint32 *puPhysAddr
);

/**
 * @brief Free up previously allocated physical memory.
 *
 * This function frees up previously allocated physical memory
 * 
 * @param[in] phMem  Pointer to the location of physical memory 
 *                   handle
 * 
 * @return SpiDeviceOsSvc_RESULT_OK on success, error code on error
 */
SpiDeviceOsSvc_Result SpiDeviceOsSvc_PhysMemFree(void* phMem);

#endif // _SPIDEVICEOSSVC_H_
