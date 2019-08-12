#ifndef __OSAL_OS_H__
#define __OSAL_OS_H__
/***********************************************************************
 * osal_os.h
 *
 * Copyright (C) 2008-2013 QUALCOMM Technologies, Inc.
 *
 * This file header file includes all os-dependent header files.
 * 
 **********************************************************************/

/*===========================================================================


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when         who   what, where, why
----------   ---   --------------------------------------------------------- 
2013-10-31    ph   Updated osal_thread_join() API to fix posix tcb exhaustion.
2013-09-20   tbg   Added support for pthread thread local storage 
2012-09-04    ra   Removed dependency to task.h
2011-04-27    is   Resolve compiler warnings
2010-29-10    vs   Added atomic_compare_and_set
2010-06-30    vs   No-oped osal_enable/disable interrupts for qdsp6
2010-01-18    JV   Consolidate OSAL APIs and move defines from
                   osal_rex_types.h
2009-08-05    JV   Included task.h
2009-05-11    JV   Created
===========================================================================*/

#include "comdef.h"
#include "osal_rex_types.h"
#include "pthread.h"

int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val );

void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v);



#ifndef DIAG_REX_QDSP_EMULATION
#define osal_disable_interrupts() INTLOCK()
#define osal_enable_interrupts()  INTFREE()
#else
#define osal_disable_interrupts()
#define osal_enable_interrupts() 
#endif


#ifndef DIAG_REX_QDSP_EMULATION
	#define osal_lock_mutex(X) INTLOCK()
	#define osal_unlock_mutex(X) INTFREE()
#else
	#define osal_lock_mutex(X) REX_ISR_LOCK((rex_crit_sect_type *) X)
	#define osal_unlock_mutex(X) REX_ISR_UNLOCK((rex_crit_sect_type *)X)
#endif
  
/*===========================================================================
  FUNCTION   OSAL_PTHREAD_KEY_CREATE

  DESCRIPTION
       Creates a key for accessing a thread local storage data item.
    
  PARAMETERS
       key - Pointer to the newly-created thread local storage key value
       destructor - Pointer to the key specific destructor function. Passing NULL
	                specifies that no destructor function is defined for the key.
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
  ===========================================================================*/
int osal_pthread_key_create(osal_tls_key_t *key, void (*destructor)(void*));


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_KEY_DELETE

  DESCRIPTION
       Deletes the specified key from thread local storage
    
  PARAMETERS
       key - Thread local storage key to delete
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
  ===========================================================================*/
int osal_pthread_key_delete(osal_tls_key_t key);


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_GETSPECIFIC

  DESCRIPTION
       Loads the data item from thread local storage.
       Returns the data item that is stored in thread local storage with the specified key.
       The data item is always a pointer to user data.
    
  PARAMETERS
       key - Thread local storage key value.
  
  DEPENDENCIES
       Key must be initialized using osal_pthread_key_create.

   RETURN VALUE
       Pointer - Data item indexed by key in thread local storage
       If no thread-specific data value is associated with key, then the value NULL
	   shall be returned.
           
  SIDE EFFECTS
       None
  ===========================================================================*/
void *osal_pthread_getspecific(osal_tls_key_t key);


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_SETSPECIFIC

  DESCRIPTION
       Stores a data item to thread local storage along with the specified key.

  PARAMETERS
       key -  Thread local storage key value.
       value - Pointer to user data value to store.
  
  DEPENDENCIES
       Key must be initialized using osal_pthread_key_create.

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
===========================================================================*/
int osal_pthread_setspecific(osal_tls_key_t key, const void* value);

#endif /* not __OSAL_OS_H__ */

