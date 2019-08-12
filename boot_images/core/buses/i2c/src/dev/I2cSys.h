#ifndef I2CSYS_H
#define I2CSYS_H
/*===========================================================================
  @file   I2cSys.h

  This file contains the API for the QUP OS Services 
 
        Copyright (c) 2011 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

  ===========================================================================*/

/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/i2c/src/dev/I2cSys.h#1 $ */


/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cTypes.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/**
   Some of the most common errors may be defined here.
   Each OS defines its own values which will be interpreted
   differently based on OS.
 */
typedef enum
{
   I2CSYS_RESULT_OK = 0,
   I2CSYS_RESULT_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_OS,
   I2CSYS_RESULT_ERROR_EVT_WAIT_TIMEOUT,

   I2CSYS_RESULT_FIRST_SECTION = I2CSYS_RESULT_ERROR_EVT_WAIT_TIMEOUT,
   I2CSYS_RESULT_SECOND_SECTION = I2CSYS_RESULT_FIRST_SECTION+1000,


} I2CSYS_Result;

typedef void* I2CSYS_EVENT_HANDLE;

typedef void* I2CSYS_CRITSECTION_HANDLE;
typedef void* I2CSYS_INTLOCK_HANDLE;

typedef struct I2CSYS_PhysMemDesc
{
   void   *pObj;
   uint8  *pVirtAddress;
   uint8  *pPhysAddress;
   uint32  uSize;
} I2CSYS_PhysMemDesc;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/** @brief Creates an event object and sets handle pointer.

    @param[out] pEventHandle  Pointer to event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateEvent
(
   I2CSYS_EVENT_HANDLE *pEventHandle
);

/** @brief Closes a handle destroying the object associated with it.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CloseHandle
(
   I2CSYS_EVENT_HANDLE hEvent
);

/** @brief Signals the event object pointed to by the handle.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_SetEvent
(
   I2CSYS_EVENT_HANDLE hEvent
);

/** @brief Clears outstanding signals on the event object.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_ClearEvent
(
   I2CSYS_EVENT_HANDLE hEvent
);

/** @brief Wait with a timeout on the event object.

    @param[in] hEvent  Event handle.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Wait
(
   I2CSYS_EVENT_HANDLE hEvent,
   uint32              dwMilliseconds
);

/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSecHandle  Pointer to critical section handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE *pCritSecHandle
);

/** @brief Enters the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_EnterCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
);

/** @brief Leaves the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_LeaveCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
);









/** @brief Allocates a memory buffer of the given size and sets the pointer to it.

    @param[out] ppBuffer pointer to pointer to allocated memory.
    @param[in]  size     size of the memory to be allocated.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Malloc
(
   void **ppBuffer,
   uint32 size
);

/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to allocated memory.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Free
(
   void *pBuffer
);

/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to buffer.
    @param[in] c        character byte to init the memory.
    @param[in] size     the size of the buffer. 
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Memset
(
   void  *pBuffer,
   int32  c,
   uint32 size
);

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @param[in] uTimeMicrosec  wait time in microseconds.
  @return          Nothing.
  */
void
I2CSYS_BusyWait
(
   uint32 uTimeMicrosec
);

/**
   @brief Allocate physical memory
 
   This function allocates physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemAlloc
(
   I2CSYS_PhysMemDesc *pPhysMem
);

/**
   @brief Release physical memory
 
   This function releases physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemFree
(
   I2CSYS_PhysMemDesc *pPhysMem
);

/**
   @brief Translates from virtual address to physical
 
   @param[in] pVirtMem  Pointer to virtual memory.
   @param[in] uSize     Size of memory buffer.
 
   @return 0 if error, physical address otherwise.
 */
uint32
I2CSYS_VirtToPhys
(
   uint8       *pVirtMem,
   uint32       uSize
);

#endif /* #ifndef I2CSYS_H */

