#ifndef RLC_OS_H
#define RLC_OS_H
/**
@file rlc_os.h
@brief
This module implements operating system independent functions/wrappers used in 
RLC code. 
*/
/*=============================================================================
  Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Operating system independent functions/wrappers used in RLC code.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc_v.h"

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Wrapper for copying memory.

@param[in]  db    Pointer to dest buffer.
@param[in]  ds    Size of dest buffer.
@param[in]  sb    Pointer to source buffer.
@param[in]  ss    Size of source buffer.

@return
None. 
 
@dependencies  
None.
*/

void rlc_memcpy(void* db, uint32 ds, void* sb, uint32 ss);

/** 
Wrapper for initializing an operating system independent mutex.

@param[out]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
None.
*/

void rlc_init_mutex(rlc_mutex_ptr *ppMutex);

/** 
Wrapper for destroying an operating system independent mutex.

@param[out]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
None.
*/

void rlc_destroy_mutex(rlc_mutex_ptr *ppMutex);

/** 
Wrapper for locking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_lock_mutex(rlc_mutex_ptr *ppMutex);

/** 
Wrapper for locking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
TRUE if mutex is acquired. 
FALSE if mutex is held by another thread.
 
@dependencies  
Mutex must be initialized.
*/

boolean rlc_trylock_mutex(rlc_mutex_ptr *ppMutex);
  
  
/** 
Wrapper for unlocking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_unlock_mutex(rlc_mutex_ptr *ppMutex);

/** 
Wrapper for initializing an operating system independent semaphore.

@param[out]  pSig    Pointer to a pointer to a semaphore.

@return
None. 
 
@dependencies  
None.
*/

void rlc_init_semaphore(rlc_sig_ptr *pSig);
  
/** 
Wrapper for initializing an operating system independent semaphore.

@param[out]  pSig    Pointer to a pointer to a semaphore.

@return
None. 
 
@dependencies  
None.
*/

void rlc_destroy_semaphore(rlc_sig_ptr *pSig);
  
/** 
Wrapper for waiting on an operating system independent semaphore.

@param[in]  pSig    Pointer to a semaphore.

@return
None. 
 
@dependencies  
Semaphore must be initialized.
*/

void rlc_sig_wait(rlc_sig_ptr pSig);

/** 
Wrapper for signalling an operating system independent semaphore.

@param[in]  pSig    Pointer to a semaphore.

@return
None. 
 
@dependencies  
Semaphore must be initialized.
*/

void rlc_set_sig(rlc_sig_ptr pSig);

/** 
Wrapper for locking an external operating system independent mutex.

@param[in]  pMutex    Pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_lock_var_mutex(rlc_mutex_ptr pMutex);

/** 
Wrapper for unlocking an external operating system independent mutex.

@param[in]  pMutex    Pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_unlock_var_mutex(rlc_mutex_ptr pMutex);

/** 
Wrapper for performing a safe version of string copy.

@param[in]   str1         Destination string.
@param[in]   str2         Source string.
@param[in]   max_length   Size of string.
@param[in]   count        Number of characters to be copied.

@return
None. 
 
@dependencies  
str2 must be a valid string.
*/

void rlc_strcpy(char* str1, const char* str2, uint32 max_length);

/** 
Wrapper for performing a safe version of string length.

@param[in]   str1         Location of string.
@param[in]   max_length   Max possible length of string to search.

@return
Length of string in number of bytes. 
 
@dependencies  
None.
*/

uint32 rlc_strnlen(char* str1, uint32 max_length);

/** 
Wrapper for creating a thread.

@param[in]   task_function  Function pointer to the entry function.

@return
None. 
 
@dependencies  
None.
*/


void rlc_create_thread(void task_function(void));

/** 
Initializes the RLC heap.

@return
None. 
 
@dependencies  
None.
*/

void rlc_heap_init(void);

/** 
Wrapper for allocating internal memory for RLC.

@param[in]   size  Size of the buffer to allocate in bytes.

@return
None. 
 
@dependencies  
Heap must be initialized.
*/

void* rlc_malloc(uint32 size);

/** 
Get the malloc and free stats.

@param[in]   nMallocs   Number of malloc calls.
@param[in]   nFrees     Number of free calls.

@return
RLC_SUCCESS -- Successfully returned stats. 
RLC_FAILED  -- No stats available. 
 
@dependencies  
Heap must be initialized.
*/

rlc_error_type rlc_get_malloc_stats(uint32* nMallocs, uint32* nFrees);

/** 
Wrapper for freeing internal memory for RLC.

@param[in]   ptr  Pointer to memory that was malloced.

@return
None. 
 
@dependencies  
Heap must be initialized.
*/

void rlc_free(void* ptr);

/**
Initializes random number generator by turning on clock.

@return
None

@dependencies
None.
*/

void rlc_os_random_init(void);

/**
Deinitializes random number generator to turn off clock.

@return
None

@dependencies
rlc_os_random_init must be called before this call.
*/

void rlc_os_random_deinit(void);

/**
Returns a random number from the random number generator.

@return
Random int.

@dependencies
1) Random number generator initializer needs to be run once.
*/

uint32 rlc_os_random(void);


/**
Bitwise square root function.

@return
The square root of the value given.

@dependencies
None.
*/

uint32 rlc_os_sqrt(uint32 num);

/**
OS Abstracted function for sleeping for a number of milliseconds.

@return
None.

@dependencies
None.
*/

void rlc_os_sleep(uint32 msecs);

#endif /*RLC_OS_H*/
