#ifndef SNS_MEMMGR_H
#define SNS_MEMMGR_H

/*============================================================================
  @file sns_memmgr.h

  @brief
  Defines the sensors memory manager interface.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "sns_common.h"

/*============================================================================
  Preprocessor Definitions and Constans
  ===========================================================================*/

#define SNS_MAX_NUM_MEMMGR_CLIENTS 5

#ifdef SNS_USES_ISLAND
#define SNS_MEMMGR_UIMAGE_CODE __attribute__((section (".text.MEMMGR")))
#define SNS_MEMMGR_UIMAGE_DATA __attribute__((section (".data.MEMMGR")))
#define SNS_MEMMGR_UIMAGE_RODATA __attribute__((section (".rodata.MEMMGR")))
#else
#define SNS_MEMMGR_UIMAGE_CODE
#define SNS_MEMMGR_UIMAGE_DATA
#define SNS_MEMMGR_UIMAGE_RODATA
#endif /* USES_ISLAND */

/*============================================================================
  Type Declarations
  ===========================================================================*/

typedef enum {
    SNS_SAM,
    SNS_SMGR,
    SNS_SMR,
    SNS_PM,
    SNS_TEST,
    SNS_DDF,
    SNS_EM,
    SNS_USMR,
    SNS_OSA,
} sns_memmgr_module;

/* memmgr callback function type. used by other sensors' module to register
 * callback function to memmgr
 */
typedef void (*sns_memmgr_lowmem_cb_t)(void);

/* Low memory callback function structure */
typedef struct sns_memmgr_lowmem_cb_s
{
  uint8_t num_clients;
  sns_memmgr_lowmem_cb_t cb_func[SNS_MAX_NUM_MEMMGR_CLIENTS];
} sns_memmgr_lowmem_cb_s;

/*============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * @return Current size (in unit of bytes) of used heap in default heap
 */
uint32_t sns_memmgr_default_heap_usage(void);

/* See memcpy */
void* SNS_OS_MEMCOPY(void *dest, const void *source, size_t size);
/* See memset */
void* SNS_OS_MEMSET(void *ptr, int value, size_t size);
/* See memset */
void* SNS_OS_MEMZERO(void *ptr, size_t size);
/* See memcmp */
int SNS_OS_MEMCMP(const void *ptr1, const void *ptr2, size_t size);
/* See memscpy */
size_t SNS_OS_MEMSCPY(void *dest, size_t dest_size, const void *source, size_t source_size );

/**
 * Allocate a block of memory from the default, non-uimage heap.
 *
 * @peram[i] module Module according to sns_memmgr_module
 * @param[i] size Number of bytes to allocate
 *
 * @return Pointer to allocated memory, or NULL if none available
 */
void* SNS_OS_MALLOC(uint8_t module, uint32_t size);

/**
 * Free a block of memory allocated by SNS_OS_MALLOC, SNS_OS_U_MALLOC, or
 * SNS_OS_ANY_MALLOC.  See free.
 *
 * @param[i] ptr Block to be freed
 */
void SNS_OS_FREE(void *ptr);

/**
 * @return true if this address is located within the sensors uHeap; false otherwise
 */
bool SNS_OS_IS_UHEAP_PTR(void const *ptr);

/**
 * @return true if this function address is located in uImage; false otherwise
 */
bool SNS_OS_IS_ISLAND_FUNC(void const *func);

/**
 * Allocate a block of memory from any available heap.  uImage heap is tried first,
 * followed by the Default heap.  If from Default heap, uImage entry is blocked
 * until memory is freed.  This function should only be used for very transitory
 * allocations.
 *
 * @peram[i] module Module according to sns_memmgr_module
 * @param[i] size Number of bytes to allocate
 *
 * @return Allocated block; or NULL if all heaps are exhausted
 */
void* SNS_OS_ANY_MALLOC(uint8_t module, uint32_t size);

/**
 * Allocate a block of memory from the uimage heap.
 *
 * @peram[i] module Module according to sns_memmgr_module
 * @param[i] size Number of bytes to allocate
 *
 * @return Pointer to allocated memory, or NULL if none available
 */
void* SNS_OS_U_MALLOC(uint8_t module, uint32_t size);

/**
 * Same as SNS_OS_ANY_MALLOC.
 *
 * @peram[i] module Module according to sns_memmgr_module
 * @param[i] size Number of bytes to allocate
 * @param[i] calldepth Levels of callstack to ascend for logging
 *
 * @return Pointer to allocated memory, or NULL if none available
 */
void* sns_memmgr_int_any_malloc(uint8_t module, uint32_t size, uint32_t calldepth);

/**
 * Initializes separate heap for Sensors
 *
 * This function will create a separate Heap for Sensors.
 *
 * 'sns_heap_id' is heap ID for memory allocations from Sensors code.
 */
sns_err_code_e sns_heap_init( void );

/**
 * Destroys the Sensors default heap created by sns_heap_init
 */
sns_err_code_e sns_heap_deinit( void );

/**
 * Registers a callback to be called in out of memory conditions for default heap
 *
 * The implementation of this callback should free any unneeded or "low priority"
 * memory.
 *
 * @param[i] cb: The callback function
 *
 * @return
 *  SNS_SUCCESS The callback is successfully registered
 *  SNS_ERR_NOTALLOWED Exceeded maximum number of clients
*/
sns_err_code_e sns_memmgr_lowmem_cb_register( sns_memmgr_lowmem_cb_t cb );

#endif /* SNS_MEMMGR_H */
