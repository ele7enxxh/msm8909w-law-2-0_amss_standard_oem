/***********************************************************************
 * OSAL_qurt.c
 * Description: This file contains the implementation of the OS abstraction
 * layer for QURT. 
 * Copyright (C) 2011-2013 Qualcomm Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                         EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/services/osal/qurt/src/osal_qurt.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-20   tbg   Added support for pthread thread local storage 
2013-06-10    ph   Destroy the signal initialized in create thread at the time 
                   delete of thread happens.
2012-02-12    sr   Fixed compiler and Klockwork warnings 
2012-08-20    sg   Create a thread only when we have a free entry in the qurt_tcb_map_table 
2012-08-10    sg   Updated thread id in tcb ptr with proper value of thread id 
                   while creating thread 
2011-09-28    is   Support osal_create_diag_timer() and osal_create_diag_timer_ex() APIs
2011-09-15    sg   Created

===========================================================================*/


#include "osal.h"
#include <stdlib.h>
#include <stdio.h>

static tcb_map_table    qurt_tcb_map_table[QURT_MAX_THREADS];

/* mutex to protect the tcb_map_table */
static qurt_mutex_t         tcb_map_table_lock;

/* Called just once, the first time this OSAL is used.
Initializes the mutexes. Can be used in future for other
initializations */
static int osal_qurt_init (void);
static int thread_count = 0;
static osal_mutex_arg_t diag_timer_mutex;

/* Qube functions implementation
 *=========================
 * Synchronization  
 * Critical sections       
 *=========================*/
int osal_init_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_init(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
    
}

int osal_delete_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_destroy(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

int osal_enter_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_lock(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

int osal_exit_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_unlock(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

/*=======================================
 *              Mutexes
 ========================================*/
int osal_init_mutex(osal_mutex_arg_t *mutex)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if(mutex)
    {
        qurt_rmutex_init(&(mutex->handle));
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

int osal_delete_mutex(osal_mutex_arg_t *mutex)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if(mutex)
    {
        qurt_rmutex_destroy(&(mutex->handle));
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

void osal_lock_mutex(osal_mutex_arg_t *mutex)
{
    if(mutex)
    {
        qurt_rmutex_lock(&(mutex->handle));
    }
}

void osal_unlock_mutex(osal_mutex_arg_t *mutex)
{
    if (mutex)
    {
        qurt_rmutex_unlock(&(mutex->handle));
    }
}
/*==========================================
             Signaling 
============================================*/

int osal_set_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *prev_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && prev_sig)
    {

        *prev_sig = qurt_anysignal_set(&(tcb_ptr->current_sigs), sig);

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

int osal_reset_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *prev_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && prev_sig)
    {
        *prev_sig = qurt_anysignal_clear(&(tcb_ptr->current_sigs), sig);

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

int osal_get_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t *current_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && current_sig)
    {

        *current_sig = qurt_anysignal_get(&(tcb_ptr->current_sigs));

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================
*                 Timers 
=============================================*/
/* Qube does not support re-use of timer handles. So if we need to set or re-start
a timer that is already running, we need to delete the handle and re-start again. */
int osal_create_timer(osal_timer_t *timer,  osal_tcb_t *tcb_ptr, osal_sigs_t sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    static boolean initialized = FALSE;

    if (!initialized)
    {
    osal_init_mutex(&diag_timer_mutex);
    initialized = TRUE;
    }

    if ((NULL != timer) & (NULL != tcb_ptr))
    {
        /* memsetting to 0, so that we will be able to check later if the handle 
        is being re-used or not */
        memset(timer,0x0,sizeof(osal_timer_t));
        /* set up the tcb pointer that the timer is associated with */
        timer->qube_tcb = tcb_ptr;
        /* the message that needs to be sent, once the timer expires */
        timer->sig = sig;
        ret_value = OSAL_SUCCESS;
    }
    
    return(ret_value);
}

int osal_create_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
                           osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param)
{
    return OSAL_SUCCESS;
}

int osal_create_diag_timer(osal_timer_t *timer,  osal_tcb_t *tcb_ptr, osal_sigs_t sig)
{
  return osal_create_timer( timer, tcb_ptr, sig );
}

int osal_create_diag_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
                              osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param)
{
  return osal_create_timer_ex( timer, tcb_ptr, timer_cb_ptr, cb_param );
}

int osal_set_timer(osal_timer_t *timer_ptr, osal_timer_cnt_t timer_cnt)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (timer_ptr)
    {
        qurt_timer_attr_t       qube_timer_attr;
        osal_sigs_t previous_sig;

        /* initializing QUBE timer attributes */
        qurt_timer_attr_init (&qube_timer_attr);
        qurt_timer_attr_set_type (&qube_timer_attr, QURT_TIMER_ONESHOT);
        qurt_timer_attr_set_duration (&qube_timer_attr, (qurt_timer_duration_t)(timer_cnt * 1000));

        osal_reset_sigs(timer_ptr->qube_tcb, timer_ptr->sig, &previous_sig);

        osal_lock_mutex(&diag_timer_mutex);

        /* delete handle if already exists */
        if (timer_ptr->timer_handle != NULL)
        {           
            if (QURT_EOK != qurt_timer_delete(timer_ptr->timer_handle))
            {
                return(OSAL_FAILURE);
            }
            else
            {
                ret_value = OSAL_SUCCESS;
            }
        }
    
        if (QURT_EOK != qurt_timer_create(&(timer_ptr->timer_handle), 
                          &qube_timer_attr, &(timer_ptr->qube_tcb->current_sigs),timer_ptr->sig))
        {
            ret_value = OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
        osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}

osal_sigs_t osal_timed_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_timer_t *timer_ptr, osal_timer_cnt_t timer_cnt)
{
    if (tcb_ptr && timer_ptr)
    {
        osal_sigs_t return_sigs;

        osal_set_timer(timer_ptr, timer_cnt);

        return_sigs =  osal_thread_wait(tcb_ptr, sig);

        return return_sigs;
    }
    return NULL;
}

int osal_reset_timer(osal_timer_t *timer_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (timer_ptr)
    {
        osal_lock_mutex(&diag_timer_mutex);
        if (timer_ptr->timer_handle != NULL)
        {
          if (QURT_EOK != qurt_timer_delete(timer_ptr->timer_handle))
          {
            ret_value = OSAL_FAILURE;
          }
          else
          {
            ret_value = OSAL_SUCCESS;
          }
        
        timer_ptr->timer_handle = NULL;
        }
        osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}

int osal_delete_timer(osal_timer_t *timer_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (timer_ptr)
    {
        osal_lock_mutex(&diag_timer_mutex);
        if (timer_ptr->timer_handle != NULL)
        {
        
          if (QURT_EOK != qurt_timer_delete(timer_ptr->timer_handle))
          {
            ret_value = OSAL_FAILURE;
          }
          else
          {
            ret_value = OSAL_SUCCESS;
          }
          timer_ptr->timer_handle = NULL;
        }
        osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}
osal_timer_cnt_t osal_get_remaining_time(osal_timer_t *timer_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (timer_ptr)
    {
        qurt_timer_attr_t temp_attr;
        qurt_timer_duration_t time_remaining;
        if (QURT_EOK != qurt_timer_get_attr(timer_ptr->timer_handle, &temp_attr))
        {
            ret_value = OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
        qurt_timer_attr_get_remaining(&temp_attr, &time_remaining);
        ret_value = time_remaining;
    }
    return OSAL_SUCCESS;
}

/*================================================
*             Thread Manipulation 
==================================================*/

osal_tcb_t *osal_thread_self()
{
     int ctr = 0;
     int thread_id;
     thread_id = qurt_thread_get_id();

     for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
     {
         /* look up in the table for the thread ID corresponding to the
         tcb pointer */
         if( qurt_tcb_map_table[ctr].thd_id == thread_id )
         {
        
             return (qurt_tcb_map_table[ctr].tcb_id);
             
         }
     }
     return NULL;
}

osal_sigs_t osal_thread_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sigs)
{
    osal_sigs_t return_value = 0;
    if(tcb_ptr)
    {
     return_value = qurt_anysignal_wait(&(tcb_ptr->current_sigs), sigs);
    }
     return (return_value);
}

int osal_create_thread(osal_tcb_t *tcb_ptr, osal_thread_attr_t *attr, int *pri)
{
  typedef void (* thread_handler_func_t) (void *);
  int ret_value = OSAL_NULL_PTR_RECVD;
  int ctr = 0;
  osal_thread_t  thread_id;
  qurt_thread_attr_t tattr;
  unsigned int stackbase;
  
    if(tcb_ptr && attr)
    {
         static boolean initialized = FALSE;
        
        if ( thread_count >= QURT_MAX_THREADS )
        {
         fprintf(stderr, " osal_create_thread() failed since we reached maximum thread count  \n");
         return(OSAL_FAILURE);
        }
        else
        {
          qurt_anysignal_init(&(tcb_ptr->current_sigs));
    
          /* This function initializes mutexes */
          if (!initialized)
          {
            initialized = TRUE;
            if (OSAL_SUCCESS != osal_qurt_init())
            {
              fprintf(stderr, " osal_qurt_init failed \n");
              return(OSAL_FAILURE);
             
            }
          }
          /* The QURT OS expects the initial stack address. Stack grows upwards */
          stackbase = (unsigned int)attr->stack_address;
        
          qurt_thread_attr_init (&tattr);
          qurt_thread_attr_set_stack_size (&tattr, (attr->stack_size -8));
          qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase +7) &(~7)) );
          qurt_thread_attr_set_priority (&tattr, (unsigned short)attr->priority);
          qurt_thread_attr_set_name(&tattr,attr->name);
    
       
          /* update the global table to map the thread ID to the tcb */
          qurt_rmutex_lock(&tcb_map_table_lock);
          for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
          {
            /* look up in the table for the thread ID corresponding to the
            tcb pointer */
            
            if( qurt_tcb_map_table[ctr].tcb_id == 0)
            {
              ret_value =  qurt_thread_create(&thread_id, &tattr, (thread_handler_func_t)attr->start_func, (void *)attr->arg);
        
              if (QURT_EFATAL == ret_value)
              {
                ret_value = OSAL_FAILURE;
                qurt_rmutex_unlock(&tcb_map_table_lock);
                return(ret_value);
              }
              else
              {   
                tcb_ptr->thd_id = thread_id;
                ret_value = OSAL_SUCCESS;
              }
             
              qurt_tcb_map_table[ctr].thd_id = thread_id;
              qurt_tcb_map_table[ctr].tcb_id = tcb_ptr;
              ++thread_count;
              break;
            }

          }

          qurt_rmutex_unlock(&tcb_map_table_lock);
    
          qurt_rmutex_init(&tcb_ptr->tcb_msg_lock);
        }
    }
    return(ret_value);
}


int osal_delete_thread(osal_tcb_t *tcb_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    int ctr = 0;
    int thread_id;

    if(tcb_ptr)
    {
     thread_id = tcb_ptr->thd_id;
     qurt_rmutex_lock(&tcb_map_table_lock);
      for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
     {
         /* look up in the table for the thread ID corresponding to the
         tcb pointer */
         if( qurt_tcb_map_table[ctr].thd_id == thread_id )
         {
            /*Destroy the signal created in osal_create_thread*/
            qurt_anysignal_destroy(&(tcb_ptr->current_sigs));
            qurt_rmutex_destroy(&tcb_ptr->tcb_msg_lock);
             qurt_tcb_map_table[ctr].thd_id = 0;
             qurt_tcb_map_table[ctr].tcb_id = 0;
            --thread_count;
            ret_value = OSAL_SUCCESS;
            break;
         }
      }
     qurt_rmutex_unlock(&tcb_map_table_lock);
    }
    return(ret_value);
}

#if 0
void osal_set_task_name(void *name)
{
  qurt_thread_attr_t attr;

  /* set thread name */
  qurt_thread_attr_set_name(&attr, name);
}
#endif 

void osal_thread_exit(int status)
{
     qurt_thread_exit(status);
}

void osal_thread_join(osal_tcb_t *tcb_ptr, int *status)
{
    qurt_thread_join((unsigned int)tcb_ptr->thd_id, status);
}

osal_priority_type osal_thread_get_pri()
{
    unsigned int thread_id = qurt_thread_get_id();
    return (qurt_thread_get_priority(thread_id));
}

osal_priority_type osal_thread_set_pri(osal_priority_type pri)
{
    int ret_value = 0;
    unsigned int thread_id = qurt_thread_get_id();

    ret_value = qurt_thread_get_priority(thread_id);
    
    qurt_thread_set_priority(thread_id, pri);
    return ret_value;
}


static int osal_qurt_init (void)
{
    qurt_rmutex_init(&tcb_map_table_lock);
    return OSAL_SUCCESS;
}


/*================================================================
* Certain APIs related to interrupts and the watchdog 
==================================================================*/

int osal_autodog_enable(int dog_report_val)
{
  return 0;
}

int osal_autodog_disable(void)
{
  return 0;
}

int osal_ints_are_locked(void)
{
  return 0;
}

boolean os_is_in_irq_mode( void )
{
  return FALSE;
}

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
  if(qurt_tls_create_key(key, destructor) != QURT_EOK)
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
  if(qurt_tls_delete_key(key) != QURT_EOK)
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
  return qurt_tls_get_specific(key);
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
  if(qurt_tls_set_specific(key, value) != QURT_EOK)
    return OSAL_FAILURE;
  else
    return OSAL_SUCCESS;
}

