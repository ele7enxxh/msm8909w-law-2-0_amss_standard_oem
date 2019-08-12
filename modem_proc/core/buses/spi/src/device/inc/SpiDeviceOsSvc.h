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

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/inc/SpiDeviceOsSvc.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/29/13 sg     Added memscpy API.
07/14/12 ag     ISR and IST prototype changes.
04/07/12 ddk    Added name change to avoid conflicts with I2c library.
04/07/12 ddk    Added mem free API.
03/27/12 ddk    Added SPI BAM mode updates.
09/26/11 LK     Created

===========================================================================
        Copyright c 2011 - 2015 Qualcomm Technologies Incorporated.
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
typedef enum
{
   IBUSOS_RESULT_OK = 0,
   IBUSOS_RESULT_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_DEV_OS,
   IBUSOS_RESULT_ERROR_TIMEOUT,
   IBUSOS_RESULT_ERROR_EVT_CREATE_FAILED,
   IBUSOS_RESULT_ERROR_EVT_DESTROY_FAILED,
   IBUSOS_RESULT_ERROR_MEM_FREE_FAILED,
   IBUSOS_RESULT_ERROR_MEM_MALLOC_FAILED,
   IBUSOS_RESULT_ERROR_MEM_COPYS_FAILED,
   IBUSOS_RESULT_ERROR_EVT_CLEAR_FAILED,
   IBUSOS_RESULT_ERROR_EVT_CTRL_FAILED,
   IBUSOS_RESULT_ERROR_EVT_WAIT_FAILED,
   IBUSOS_RESULT_ERROR_SYNC_CREATE_FAILED,
   IBUSOS_RESULT_ERROR_SYNC_DESTROY_FAILED,
   IBUSOS_RESULT_ERROR_INVALID_HANDLE_TYPE,
   IBUSOS_RESULT_ERROR_NULL_PTR,

   IBUSOS_RESULT_ERROR_REGISTER_IST_HOOKS_START,

   IBUSOS_RESULT_ERROR_REGISTER_IST_HOOKS_EXISTS,
   IBUSOS_RESULT_ERROR_REGISTER_IST_HOOKS_END = IBUSOS_RESULT_ERROR_REGISTER_IST_HOOKS_START+15,

} IBUSOS_Result;

typedef void* IBUSOS_EVENT_HANDLE;

typedef void* IBUSOS_CRITSECTION_HANDLE;

typedef uint32 (*IST_HOOK)(void *pdata);
typedef uint32 (*ISR_HOOK)(void *pdata);

/** @brief Creates an event object and sets handle pointer.

    @param[out] pEventHandle  Pointer to event handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_CreateEvent(IBUSOS_EVENT_HANDLE *pEventHandle);

/** @brief Closes a handle destroying the object associated with it.

    @param[in] hEvent  Event handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_CloseHandle(IBUSOS_EVENT_HANDLE hEvent);

/** @brief Signals the event object pointed to by the handle.

    @param[in] hEvent  Event handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_SetEvent(IBUSOS_EVENT_HANDLE hEvent);

/** @brief Clears outstanding signals on the event object.

    @param[in] hEvent  Event handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_ClearEvent(IBUSOS_EVENT_HANDLE hEvent);

/** @brief Wait with a timeout on the event object.

    @param[in] hEvent  Event handle.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_Wait(IBUSOS_EVENT_HANDLE hEvent, uint32 dwMilliseconds);

/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSecHandle  Pointer to critical section handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_CreateCriticalSection(IBUSOS_CRITSECTION_HANDLE *pCritSecHandle);

/** @brief Enters the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_EnterCriticalSection(IBUSOS_CRITSECTION_HANDLE hCriticalSection);

/** @brief Leaves the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_LeaveCriticalSection(IBUSOS_CRITSECTION_HANDLE hCriticalSection);

/** @brief Allocates a memory buffer of the given size and sets the pointer to it.

    @param[out] ppBuffer pointer to pointer to allocated memory.
    @param[in]  size     size of the memory to be allocated.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_Malloc(void **ppBuffer,uint32 size);

/** @brief Frees memory allocated by IBUSOS_Malloc.

    @param[in] pBuffer  pointer to allocated memory.
    @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_Free(void *pBuffer);

/** @brief Memset operation.

    @param[in] buffer input buffer 
    @param[in] val    memset value.
    @param[in] size   size of buffer or desired length of memset
    	  operation.
  @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_Memset(void *buffer, int32 val, uint32 size);

/** @brief Registers IST/ISR hooks for the indicated device.

    @param[in] uDevId   Device index.
    @param[in] IsrHook  ISR hook.
    @param[in] pIsrData Pointer to data handed back when ISR is called.
    @param[in] IstHook  IST hook, thread handling the request.
    @param[in] pIstData Pointer to data handed back when IST is called.
  @return          IBUSOS_Result .
   */ 
IBUSOS_Result
IBUSOS_RegisterISTHooks(uint32 uDevId, ISR_HOOK IsrHook, void* pIsrData, IST_HOOK IstHook, void* pIstData);

/** @brief Unregisters IST/ISR hooks for the indicated device.

    @param[in] uDevId   Device index.
  @return          IBUSOS_Result .
  */
IBUSOS_Result
IBUSOS_UnRegisterISTHooks(uint32 uDevId);

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @return          Nothing.
  */
void
IBUSOS_BusyWait(uint32 uTimeMicrosec);

IBUSOS_Result IBUSOSSPI_PhysMemAlloc
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
 * @return IBUSOS_RESULT_OK on success, error code on error
 */
IBUSOS_Result IBUSOSSPI_PhysMemFree(void* phMem);

/**
 * @brief Size bounded memory copy. 
 *
 * Copies bytes from the source buffer to the destination buffer.
 * 
 *  @param[out]	dst      	Destination buffer.
 *  @param[in]  dstSize		Size of the destination buffer in
 *  	  					bytes.
 *  @param[in]	src       	Source buffer.
 *  @param[in]  srcSize		Number of bytes to copy from
 *   						source buffer.
 *  
 * @return IBUSOS_RESULT_OK  when dst buffer has 
 * number of bytes is greater or equal than source 
 * buffer,else error code.
 *  
 */

IBUSOS_Result IBUSOSSPI_MemCopyS(void *dst, size_t dstSize, 
						const void *src, size_t srcSize);

#endif // _QUPBUSOSSVC_H_
