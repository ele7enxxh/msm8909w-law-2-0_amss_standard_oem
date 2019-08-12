 /***********************************************************************
 * osal_rex.c
 * Description: This file contains the implementation of the OS abstraction
 * layer for REX. 
 *
 * Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                         EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/services/osal/rex/src/osal_rex.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   --------------------------------------------------------- 
2014-11-05    sa   Updated osal_delete_timer API.
2013-10-31    ph   Updated osal_thread_join() API to fix posix tcb exhaustion.
2013-09-20   tbg   Added support for pthread thread local storage 
2013-04-09    sr   Fixed the compiler warnings 
2013-02-15    sr   Moved to new Dog heartbeat implementation  
2012-09-04    ra   Added sanity check in osal_create_thread() to accomodate 
				   Priority changes in Dime 
2011-09-28    is   Support non-deferrable Diag timers
2011-06-14    hm   Migrating to REX opaque TCB APIs 
2011-04-27    is   Resolve compiler warnings
2010-11-05    sg   Featurized the call to rex_def_task_ext5() on q6 
                   until a common api would be available for all targets
                   to create the thread
2010-10-10    vs   updated API to create rex thread
2010-09-20    vs   Added atomic ops
2009-12-03    JV   New APIs osal_thread_exit and osal_thread_join
2009-12-03    JV   New API, osal_set_task_name() to set the current task name.
2009-09-23    vs   mutex implementation based on feature DIAG_REX_QDSP_EMULATION
2009-09-09    JV   osal_lock_mutex() and osal_inlock_mutex() are no more function calls. 
                   They map to INTLOCK() and INTFREE()
2009-08-11    JV   Using rex_def_task_ext2() instead of rex_def_task() to create a
                   task. This displays the task name in the task list in trace32.
2009-08-11    JV   Mutexes now map to INTLOCK_SAVE/INTLOCK_FREE (supports nesting)
2008-12-10    JV   Added APIs pertaining to watchdog and interrupts
2008-10-21    JV   Created

===========================================================================*/


#include "osal.h"
#include "msg.h"  /* For Diag messages */
#if defined(DIAG_DOG_SUPPORT)
#include "dog_hb_rex.h"
#endif

/* 
 *===================================
 * Definitions and declarations      
 *==================================*/
#ifdef FEATURE_USE_TIME_VU
timer_group_type  diag_timer_group;  /* non-deferrable to wakeup from sleep */
#endif

/* 
 *=========================
 * Synchronization  
 * Critical sections       
 *=========================*/
int osal_init_crit_sect(rex_crit_sect_type *cs)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (cs)
	{
		rex_init_crit_sect (cs);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_delete_crit_sect(osal_crit_sect_t *crit_sect)
{
	return OSAL_SUCCESS;
}

int osal_enter_crit_sect(rex_crit_sect_type *cs)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (cs)
	{
		rex_enter_crit_sect(cs);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_exit_crit_sect(rex_crit_sect_type *cs)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (cs)
	{
		rex_leave_crit_sect (cs);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}
/*=======================================
 *              Mutexes
Mutexes map to disabling/enabling interrupts
 ========================================*/


#ifndef DIAG_REX_QDSP_EMULATION
int osal_init_mutex(osal_mutex_arg_t *mutex)
{
 	return OSAL_SUCCESS;
}

#else
int osal_init_mutex(osal_mutex_arg_t *mutex)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if(mutex)
	{
		rex_init_crit_sect((rex_crit_sect_type *)&(mutex->handle));
        ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}
#endif

int osal_delete_mutex(osal_mutex_arg_t *mutex)
{
	return OSAL_SUCCESS;
}


#ifdef DIAG_REX_QDSP_EMULATION
int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val )
{
   return atomic_compare_and_set(target,old_val,new_val);
 
}	

void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit)
{
   atomic_set_bit(target,bit);
}

void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit)
{

   atomic_clear_bit(target,bit);
}

void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v)
{
   atomic_add(target, v);
}


#else
int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val )
{
    int ret_value = 1;

	INTLOCK();

	if (*target == old_val) {
		 *target = new_val;  
	}
	else {
        ret_value = 0;
	} 
	INTFREE();

	return ret_value;
}

void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit)
{
   INTLOCK();
   *target = *target | (1 << (bit % (sizeof(int) * 8)));
   INTFREE();
}

void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit)
{
   unsigned int mask = 0;
   INTLOCK();
   mask =  (1 << (bit % (sizeof(int) * 8)));
   mask = ~mask;
   *target = *target & mask;
   INTFREE();
}

void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v)
{
   INTLOCK();
   *target = *target + v;
   INTFREE();
}


#endif



/*==========================================
             Signaling 
============================================*/

int osal_set_sigs(rex_tcb_type *tcb_ptr, rex_sigs_type sig, rex_sigs_type *prev_sig)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (tcb_ptr && prev_sig)
	{
		*prev_sig = rex_set_sigs(tcb_ptr, sig);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_reset_sigs(rex_tcb_type *tcb_ptr, rex_sigs_type sig, rex_sigs_type *prev_sig)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (tcb_ptr && prev_sig)
	{
		*prev_sig = rex_clr_sigs(tcb_ptr, sig);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_get_sigs(rex_tcb_type *tcb_ptr, rex_sigs_type *current_sig)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (tcb_ptr && current_sig)
	{
		*current_sig = rex_get_sigs(tcb_ptr);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}
/*===========================================
*                 Timers 
=============================================*/

/*===========================================================================

FUNCTION OSAL_CREATE_TIMER
  
DESCRIPTION
  Creates a timer.
  
PARAMETERS
  timer   - pointer to a valid timer data structure.
  tcb_ptr - A valid REX TCB pointer to associate with the timer.
  sigs    - REX signals which will be set in the TCB pointer upon timer
                        expiration.
  
RETURN VALUE
  OSAL_SUCCESS if success
  OSAL_FAILURE if not created
  
SIDE EFFECTS:
   Memory is allocated while creating the timer, so the timer must be deleted
   when it is not needed using osal_delete_timer(..) API to free the memory allocated.
  
=======================================================================================*/

int osal_create_timer(rex_timer_type *timer,  rex_tcb_type *tcb_ptr, rex_sigs_type sigs)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (timer && tcb_ptr)
	{
		rex_def_timer(timer, tcb_ptr, sigs);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_create_timer_ex(rex_timer_type *timer_ptr, osal_tcb_t *tcb_ptr, 
                         rex_timer_cb_type timer_cb_ptr, unsigned long cb_param)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (timer_ptr && tcb_ptr)
	{
		rex_def_timer_ex(timer_ptr, timer_cb_ptr, cb_param);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

/*===========================================================================

FUNCTION OSAL_CREATE_DIAG_TIMER
  
DESCRIPTION
  Defines a timer.
  
PARAMETERS
  timer - timer to be created.
  tcb_ptr - Task to signal.
  sigs - task signals to be set to the tcb when the timer expires
  
RETURN VALUE
  OSAL_SUCCESS if success
  OSAL_FAILURE if not created
  
NOTE: Memory is allocated while creating timer and so this API has to be paired up 
      with the delete timer API to free up the memory allocated when the timer is not in use.
  
 ===========================================================================*/

int osal_create_diag_timer(rex_timer_type *timer, rex_tcb_type *tcb_ptr, rex_sigs_type sigs)
{
  int ret_value = OSAL_NULL_PTR_RECVD;
  if (timer && tcb_ptr)
  {
    #ifdef FEATURE_USE_TIME_VU
      timer_def(timer, &diag_timer_group, tcb_ptr, sigs, NULL, 0);
      ret_value = OSAL_SUCCESS;
    #else
      ret_value = OSAL_FAILURE;
    #endif
  }
  return(ret_value);
}

/*===========================================================================

FUNCTION OSAL_CREATE_DIAG_TIMER_EX
  
DESCRIPTION
  Defines a timer.
  
PARAMETERS
  timer - timer to be created.
  tcb_ptr - Task to signal.
  timer_cb_ptr - callback to be called at timer expiry.
  cb_param = data to sent in the call back
  
RETURN VALUE
  OSAL_SUCCESS if success
  OSAL_FAILURE if not created
  
NOTE: Memory is allocated while creating timer and so this API has to be paired up 
      with the delete timer API to free up the memory allocated when the timer is not in use.
  
===========================================================================*/

int osal_create_diag_timer_ex(rex_timer_type *timer_ptr, osal_tcb_t *tcb_ptr, 
                              rex_timer_cb_type timer_cb_ptr, unsigned long cb_param)
{
  int ret_value = OSAL_NULL_PTR_RECVD;
  if (timer_ptr && tcb_ptr)
  {
    #ifdef FEATURE_USE_TIME_VU
      timer_def(timer_ptr, &diag_timer_group, tcb_ptr, 0, (timer_t1_cb_type)timer_cb_ptr, (timer_cb_data_type)cb_param);
      ret_value = OSAL_SUCCESS;
    #else
      ret_value = OSAL_FAILURE;
    #endif
  }
  return(ret_value);
}

int osal_set_timer(rex_timer_type *timer_ptr, rex_timer_cnt_type timer_cnt)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (timer_ptr)
	{
		rex_set_timer(timer_ptr, timer_cnt);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

int osal_reset_timer(rex_timer_type *timer_ptr)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if (timer_ptr)
	{
		rex_clr_timer(timer_ptr);
		ret_value = OSAL_SUCCESS;
	}
	return(ret_value);
}

rex_sigs_type osal_timed_wait(rex_tcb_type *tcb_ptr, rex_sigs_type sigs, rex_timer_type *timer_ptr, rex_timer_cnt_type timer_cnt)
{
	if (tcb_ptr && timer_ptr)
	{
		return (rex_timed_wait(sigs, timer_ptr, timer_cnt));
	}
	return 0;
}

/*===========================================================================

FUNCTION OSAL_DELETE_TIMER
  
DESCRIPTION
  Deletes the timer and frees the memory allocated while creating the timer.
  
PARAMETERS
  timer - timer to be deleted
  
RETURN VALUE
  OSAL_SUCCESS if success
  OSAL_FAILURE if not deleted
  
NOTE: This API has to be paired up with the create timer API (which creates timer using timer_def())
      to free up the memory allocated while creating the timer.
  
===========================================================================*/
int osal_delete_timer(osal_timer_t *timer)
{
   int ret_value = OSAL_NULL_PTR_RECVD;
   if(timer)
   {
      /* This will delete the timer and also free the memory allocated while
         creating the timer using timer_def(..) */
      if(TE_SUCCESS == timer_undef(timer))
      {
         ret_value = OSAL_SUCCESS;
      }
      else
      {
         ret_value = OSAL_FAILURE;
      }
   }
   return ret_value;
}

rex_timer_cnt_type osal_get_remaining_time(rex_timer_type *timer_ptr)
{
	return(rex_get_timer(timer_ptr));       /* pointer to the timer to get */
}

/*================================================
*             Thread Manipulation 
==================================================*/
extern unsigned int rex_os_get_task_min_priority(void);
extern unsigned int rex_os_get_task_max_priority(void);

int osal_create_thread(rex_tcb_type *tcb_ptr, osal_thread_attr_t *attr, int *qube_msg_q_pri)
{
	int ret_value = OSAL_NULL_PTR_RECVD;
	if((NULL != tcb_ptr) & (NULL != attr))
    {
		/* The priority must be greater than the minimum task priority and 
		   less than the maximum Allowed user space task priority */
        if ( (rex_priority_type)(attr->priority) < rex_os_get_task_min_priority() || 
		     (rex_priority_type)(attr->priority) > rex_os_get_task_max_priority() )
		{
  		    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Error: Given priority %d not in the User Space Priority range %d - %d\n",
				  (rex_priority_type)(attr->priority), rex_os_get_task_min_priority(), rex_os_get_task_max_priority());
			ret_value = OSAL_FAILURE;
		}
		else
		{
			#ifdef DIAG_REX_QDSP_EMULATION
			  rex_def_task_ext5(REX_ANY_OTHER_SMT_MASK, tcb_ptr, (unsigned char *)(attr->stack_address), 
							  (rex_stack_size_type)  (attr->stack_size), (rex_priority_type) (attr->priority), 
							  (attr->start_func), (dword) (attr->arg), attr->p_tskname,
							   attr->suspended, attr->dog_report_val);
			#else
			  rex_def_task_ext2(tcb_ptr, (unsigned char *)(attr->stack_address), (rex_stack_size_type)  (attr->stack_size), 
							   (rex_priority_type) (attr->priority), (attr->start_func), (dword) (attr->arg), attr->p_tskname,
								attr->suspended, attr->dog_report_val);
			#endif 
			ret_value = OSAL_SUCCESS;
		}
	}
	return(ret_value);
}

/* rexl4 kills the task if we return (example - stress test) */
int osal_delete_thread(osal_tcb_t *tcb_ptr)
{
	return(OSAL_SUCCESS);
}

void osal_set_task_name(void *name)
{
	return;
}

void osal_thread_exit(int status)
{
	 return;
}

void osal_thread_join(osal_tcb_t *tcb_ptr, int *status)
{
    int ret_val;
    if(tcb_ptr)
    {
        ret_val= pthread_join(tcb_ptr->thread_id, NULL);
        if( ret_val!=0 )
        {
            MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "pthread_join failed %d", 
                                                                    ret_val);
        }
        /*Populate status with the return value*/
        if(status)
        {
            *status=ret_val;
        }
    }
    else
    {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "tcb_ptr is NULL");
    }
}

rex_tcb_type *osal_thread_self()
{
	 return( rex_self());
}

rex_priority_type osal_thread_get_pri()
{
	return (rex_get_pri());
}

rex_priority_type osal_thread_set_pri(rex_priority_type prio)
{
	return (rex_set_pri(prio));
}


rex_sigs_type osal_thread_wait(rex_tcb_type *tcb_ptr, rex_sigs_type sigs)
{
	if(NULL != tcb_ptr)
	{
		return (rex_wait(sigs));
	}
	return 0;
}

/*================================================================
* Certain rex specific APIs related to interrupts and the watchdog 
==================================================================*/

int osal_autodog_enable(int dog_report_val)
{
	#if defined(FEATURE_REX_OPAQUE_TCB_APIS)
	return(rex_autodog_enable_self());
	#else
	return(rex_autodog_enable(dog_report_val));
	#endif
}

int osal_autodog_disable( void )
{
	return(rex_autodog_disable());
}

int osal_ints_are_locked(void)
{
	return(rex_ints_are_locked());
}

boolean os_is_in_irq_mode(void)
{
	return (rex_is_in_irq_mode());
}

#if defined(DIAG_DOG_SUPPORT)
osal_dog_report_type osal_dog_hb_register (osal_sigs_t sigs)
{
	return(dog_hb_register_rex(sigs));
}


void osal_dog_hb_report(osal_dog_report_type handle)
{
	return(dog_hb_report(handle));
}
#endif /* DIAG_DOG_SUPPORT */


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
int osal_pthread_key_create(osal_tls_key_t *key, void (*destructor)(void*))
{
  if(pthread_key_create(key, destructor) != 0)
    return OSAL_FAILURE;
  else
    return OSAL_SUCCESS;
}

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
int osal_pthread_key_delete(osal_tls_key_t key)
{
  if(pthread_key_delete(key) != 0)
    return OSAL_FAILURE;
  else
    return OSAL_SUCCESS;
}


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
void *osal_pthread_getspecific(osal_tls_key_t key)
{
  return pthread_getspecific(key);
}

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
int osal_pthread_setspecific(osal_tls_key_t key, const void* value)
{
  if(pthread_setspecific(key, value) != 0)
    return OSAL_FAILURE;
  else
    return OSAL_SUCCESS;
}

