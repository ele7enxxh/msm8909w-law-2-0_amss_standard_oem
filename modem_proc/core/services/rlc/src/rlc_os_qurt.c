#ifndef FEATURE_WINDOWS_SIM
/**
@file rlc_os_qurt.c
@brief
This module implements qurt version of operating system abstracted 
functions/wrappers used in RLC code. 
*/
/*=============================================================================
  Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
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
07/30/15   ps      CR882561 Use secapi_get_random instead of PRNG APIs. 
09/30/14   ps      CR728884 Use PrngML_getdata() instead of PrngCL_getdata
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc_os.h"
#include "qurt.h"
#include "err.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stringl/stringl.h>
#include "secapi.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define RLC_ANYSIG_MASK 0xFFFF

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

void rlc_memcpy(void* db, uint32 ds, void* sb, uint32 ss)
{
  memscpy(db,ds,sb,ss);
}

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

void rlc_strcpy(char* str1, const char* str2, uint32 max_length)
{
  strlcpy(str1,str2,max_length);
}

/** 
Wrapper for performing a safe version of string length.

@param[in]   str1         Location of string.
@param[in]   max_length   Max possible length of string to search.

@return
Length of string in number of bytes. 
 
@dependencies  
None.
*/

uint32 rlc_strnlen(char* str1, uint32 max_length)
{
  uint32 length;

  length = strlen(str1);
  if(length > max_length) 
  {
    /* Raise error here! */
    ERR_FATAL("String is not NULL terminated! Length:%d MaxLength:%d",length,max_length,0);
    return max_length;
  }
  return length;
}

/** 
Wrapper for initializing an operating system independent semaphore.

@param[out]  pSig    Pointer to a pointer to a semaphore.

@return
None. 
 
@dependencies  
None.
*/

void rlc_init_semaphore(rlc_sig_ptr *pSig)
{
  qurt_anysignal_t  *rlc_sig = NULL;
  
  rlc_sig = (qurt_anysignal_t*)rlc_malloc(sizeof(qurt_anysignal_t));

  if(rlc_sig != NULL)
  {
  qurt_anysignal_init (rlc_sig);
  *pSig = (rlc_sig_ptr) rlc_sig;
  }
}

/** 
Wrapper for initializing an operating system independent mutex.

@param[out]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
None.
*/

void rlc_init_mutex(rlc_mutex_ptr *ppMutex)
{
  qurt_mutex_t *rlc_mutex = NULL;
  
  rlc_mutex = (qurt_mutex_t*)rlc_malloc(sizeof(qurt_mutex_t));
  if(rlc_mutex != NULL)
  {
  qurt_pimutex_init ( rlc_mutex );
  *ppMutex = (rlc_mutex_ptr) rlc_mutex;
  }
}

/** 
Wrapper for locking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_lock_mutex(rlc_mutex_ptr *ppMutex)
{
  qurt_pimutex_lock((qurt_mutex_t*)*ppMutex);
}

/** 
Wrapper for locking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

boolean rlc_trylock_mutex(rlc_mutex_ptr *ppMutex)
{
  if( qurt_pimutex_try_lock((qurt_mutex_t*)*ppMutex) == 0 )
    return TRUE;
  return FALSE;
}

/** 
Wrapper for unlocking an operating system independent mutex.

@param[in]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_unlock_mutex(rlc_mutex_ptr *ppMutex)
{
  qurt_pimutex_unlock((qurt_mutex_t*)*ppMutex);
}

/** 
Wrapper for destroying an operating system independent mutex.

@param[out]  ppMutex    Pointer to a pointer to a mutex.

@return
None. 
 
@dependencies  
None.
*/

void rlc_destroy_mutex(rlc_mutex_ptr *ppMutex)
{
	qurt_pimutex_destroy((qurt_mutex_t*)*ppMutex);
}

/** 
Wrapper for waiting on an operating system independent semaphore.

@param[in]  pSig    Pointer to a semaphore.

@return
None. 
 
@dependencies  
Semaphore must be initialized.
*/

void rlc_sig_wait(rlc_sig_ptr pSig)
{
  uint32 sigs;
  sigs = qurt_anysignal_wait((qurt_anysignal_t*)pSig, RLC_ANYSIG_MASK);
  qurt_anysignal_clear((qurt_anysignal_t*)pSig, sigs);
}

/** 
Wrapper for signalling an operating system independent semaphore.

@param[in]  pSig    Pointer to a semaphore.

@return
None. 
 
@dependencies  
Semaphore must be initialized.
*/

void rlc_set_sig(rlc_sig_ptr pSig)
{
  (void) qurt_anysignal_set((qurt_anysignal_t*)pSig, 0x4);
}

/** 
Wrapper for locking an external operating system independent mutex.

@param[in]  pMutex    Pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_lock_var_mutex(rlc_mutex_ptr pMutex)
{

}

/** 
Wrapper for unlocking an external operating system independent mutex.

@param[in]  pMutex    Pointer to a mutex.

@return
None. 
 
@dependencies  
Mutex must be initialized.
*/

void rlc_unlock_var_mutex(rlc_mutex_ptr pMutex)
{

}

/** 
Wrapper for creating a thread.

@param[in]   task_function  Function pointer to the entry function.

@return
None. 
 
@dependencies  
None.
*/

void rlc_create_thread(void task_function(void))
{

}

/** 
Initializes the RLC heap.

@return
None. 
 
@dependencies  
None.
*/

void rlc_heap_init(void)
{
  /* TODO: Use Memheap2 */
  return;
}

/** 
Wrapper for allocating internal memory for RLC.

@param[in]   size  Size of the buffer to allocate in bytes.

@return
None. 
 
@dependencies  
Heap must be initialized.
*/

void* rlc_malloc(uint32 size)
{
  /* TODO: Use Memheap2 */
  void* buf;
  if(size != 0)
  {
    buf = malloc(size);
		if(buf == NULL)
		{
			return NULL;
		}
		memset(buf, 0, size); 
    return buf;
  }
  else
  {
    return NULL;
  }
}

/** 
Wrapper for freeing internal memory for RLC.

@param[in]   ptr  Pointer to memory that was malloced.

@return
None. 
 
@dependencies  
Heap must be initialized.
*/

void rlc_free(void* ptr)
{
  free(ptr);
}

/**
Initializes random number generator by seeding from time.

@return
None

@dependencies
None.
*/

void rlc_os_random_init()
{

}

/**
Initializes random number generator by seeding from time.

@return
None

@dependencies
rlc_os_random_init must be called previously.
*/

void rlc_os_random_deinit()
{
}

/**
Returns a random number from the random number generator.

@return
Random int.

@dependencies
1) Random number generator initializer needs to be run once.

@note
was int
*/

uint32 rlc_os_random()
{
	uint32 num=0;
	secapi_get_random(SECAPI_SECURE_RANDOM, (uint8 *)&num, 4);	
	return num;
}



/**
Bitwise square root function.

@return
The square root of the value given.

@dependencies
None.
*/

uint32 rlc_os_sqrt(uint32 num)
{
	/* Square root algorithm (3 cycle/bit, 51 cycle total) */

	uint32 root=0;
	uint32 delta=0x8000; //Delta

	while( delta != 0 )
	{
		if( (( root + delta ) * ( root + delta )) <= num )
		{
			root+=delta;
		}
		delta >>= 1;
	}

	return root;
}


#endif /*FEATURE_WINDOWS_SIM*/
