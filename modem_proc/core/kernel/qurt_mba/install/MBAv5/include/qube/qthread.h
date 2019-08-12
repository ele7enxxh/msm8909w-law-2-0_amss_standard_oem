#ifndef QTHREAD_H
#define QTHREAD_H
/*=============================================================================

                 qthread.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qthread API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qthread.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <stddef.h>
#include <string.h>
#include <qtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        TYPEDEFS
=============================================================================*/
typedef unsigned int qthread_t;
typedef struct qthread_attr_t
{
    size_t  stacksize;
    unsigned int  stackaddr;
    int     priority;
    int     tid;
    char    name[QTHREAD_NAME_LEN];
    unsigned int hw_bitmask;
} qthread_attr_t;


/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**
 * Creates a thread with specific attributes
 *
 * The qthread_create is used to create a thread in suspended state
 * with attribtues specified by attr. If attr is NULL, then default
 * values are used. The attributes that can be specified are thread
 * name, stack size, stack address and priority. If stack address is
 * non-zero, it will used as a stack for the thread. Otherwise the
 * value of stack size is used to dynamically allocate stack.
 *
 * @param thr   [OUT] Thread identifier
 * @param attr        Thread attributes
 *
 * @return            QURT_EOK if thread creation is successful, 
 * @return            QURT_EVAL if thread object is NULL or invalid values
 *                     are specified for attributes,
 * @return            QURT_EMEM if memory allocation for stack fails,
 * @return            QURT_EUNKNOWN if misc. error occurs
 */
int qthread_create(qthread_t *thr, qthread_attr_t *attr);

/**
 * Initialize thread attributes. The attributes stack size and priority
 * are set to their default values (QTHREAD_MIN_STACKSIZE and QTHREAD_
 * MIN_PRIORITY)
 *
 * @param attr  [OUT] Thread attributes object
 */
static inline void qthread_attr_init(qthread_attr_t *attr)
{
    /* Set default values for all attributes */
    memset(attr->name, '\0', QTHREAD_NAME_LEN);
    attr->stacksize = QTHREAD_MIN_STACKSIZE;
    attr->stackaddr = 0;
    attr->priority  = QTHREAD_MIN_PRIORITY;
    attr->tid  = -2; // Default TID
    attr->hw_bitmask = 0xff;  //QURT_THREAD_CFG_BITMASK_ALL 
}

/**
 * Set stack size of a thread
 *
 * @param attr  Thread attributes object
 * @param hw_bitmask  HW bitmask attribute
 */
static inline void qthread_attr_sethwbitmask(qthread_attr_t *attr, unsigned int hw_bitmask)
{
    /* store hw bitmask attribute into attributes object */
    attr->hw_bitmask = hw_bitmask;
}

/**
 * Set name of a thread. The name is used only for debugging purpose. The name
 * should be null terminated and must not exceed QTHREAD_NAME_LEN characters (
 * including null character). Otherwise it will be truncated.
 *
 * @param attr  Thread attributes object
 * @param name  Name of the thread. The maximum length is limited to
 *              QTHREAD_NAME_LEN
 */
static inline void qthread_attr_setname(qthread_attr_t *attr, 
      const char *name)
{
   unsigned int dest_size;
   char *dest_name = attr->name;

   for (dest_size = sizeof(attr->name)-1; dest_size != 0;
        dest_size--)
   {
      // copy till the entire source string is copied or
      // only 1 byte is left in the destination buffer for the NULL terminator
      *dest_name = *name;
      dest_name++;

      if ('\0' == *name)
      {
         break;
      }
      name++;
   }

   if (0 == dest_size) 
   {
      // add the NULL terminator at the end of destination buffer
      *dest_name = '\0';
   }
}

/**
 * Get name of a thread
 *
 * @param attr  Thread attributes object
 * @param name  Location where name is copied
 * @param size  Size of buffer where name is copied. If size is less than length
 *              of the name, it will be truncated
 */
static inline void qthread_attr_getname(const qthread_attr_t *attr, char *name, int size)
{
   const char *source_name = attr->name;

   if (0 == size)
   {
      // destination buffer size should never be zero
      return;
   }

   size--;
   for (; size != 0; size--)
   {
      // copy till the entire source string is copied or
      // only 1 byte is left in the destination buffer for the NULL terminator
      *name= *source_name;
      name++;
      if ('\0' == *source_name)
      {
         break;
      }
      source_name++;
   }

   if (0 == size) 
   {
      // add the NULL terminator at the end of destination buffer
      *name = '\0';
   }
}

/**
 * Set stack size of a thread
 *
 * @param attr  Thread attributes object
 * @param size  Size of the stack. The value should be between
 *              QTHREAD_MIN_STACKSIZE and QTHREAD_MAX_STACKSIZE
 */
static inline void qthread_attr_setstacksize(qthread_attr_t *attr, size_t size)
{
    /* store stack size into attributes object */
    attr->stacksize = size;
}

/**
 * Get stack size of a thread
 *
 * @param attr  Thread attributes object
 * @param size  Location where stack size is stored
 */
static inline void qthread_attr_getstacksize(qthread_attr_t *attr, size_t *size)
{
    /* copy stack size */
    *size = attr->stacksize;
}

/**
 * Set stack address of a thread. This elimates the need for dynamically
 * allocating stakc when qthread_create is invoked
 *
 * @param attr  Thread attributes object
 * @param addr  Address of stack
 */
static inline void qthread_attr_setstackaddr(qthread_attr_t *attr, unsigned int addr)
{
    /* store stack address into attributes object */
    attr->stackaddr = addr;
}

/**
 * Get stack address
 *
 * @param attr  Thread attributes object
 * @param addr  Address of the stack
 */
static inline void qthread_attr_getstackaddr(qthread_attr_t *attr, unsigned int *addr)
{
    /* copy stack address */
    *addr = attr->stackaddr;
}

/**
 * Set priority of a thread. The valid range for priority is
 * QTHREAD_MIN_PRIO to QTHREAD_MAX_PRIO
 *
 * @param attr      Thread attributes object
 * @param priority  Thread Priority. The valid range for priority is
 *                  QTHREAD_MIN_PRIO and QTHREAD_MAX_PRIO
 */
static inline void qthread_attr_setpriority(qthread_attr_t *attr, int priority)
{
    /* store priority into attributes object */
    attr->priority = priority;
}

/**
 * Get priority
 *
 * @param attr      Thread attributes object
 * @param priority  Location where priority is stored
 */
static inline void qthread_attr_getpriority(qthread_attr_t *attr, int *priority)
{
    /* copy priority */
    *priority = attr->priority;
}

/**
 * get tid
 *
 * @param attr      Thread attributes object
 * @param tid       location where tid is stored
 */
static inline void qthread_attr_gettid(qthread_attr_t *attr, unsigned int *tid)
{
    /* copy tid */
    *tid = attr->tid;
}

/**
 * Set tid of a thread. The valid range for tid is 0-255
 *
 * @param attr      Thread attributes object
 * @param tid       Thread tid. 
 */
static inline void qthread_attr_settid(qthread_attr_t *attr, unsigned int tid)
{
    /* store tid into attributes object */
    attr->tid = tid;
}

/**
 * Get attributes of a thread
 *
 * @param thr         Thread identifier
 * @param attr  [OUT] Thread attributes
 *
 * @return            QURT_EOK if successful,
 * @return            QURT_EINVALID if thr is not a valid handle
 * @return            QURT_EVAL if attr is NULL
 */
int qthread_get_attr(qthread_t thr, qthread_attr_t *attr);

/**
 * Starts a thread
 *
 * @param thr         Thread identifier
 * @param start_func  Startup function where thread starts executing
 * @param arg         Argument to startup function
 *
 * @return            QURT_EOK if successful,
 * @return            QURT_EVAL if starting function address is NULL
 * @return            QURT_EINVALID if thr is invalid
 */
int qthread_start(qthread_t thr, void (*start_func) (void *), void *arg);

/**
 * Deletes a thread
 *
 * The qthread_delete is used to forcibly terminates thread specified by thr.
 *
 * @param thr   Thread identifier
 *
 * @return      QURT_EOK thread is deleted successfully,
 */
static inline int qthread_delete(qthread_t thr) 
{ return QURT_EOK; }

/**
 * Exit from a thread. This function should be in the context of the thread
 * that should exit.
 *
 * @param status  Exit status of the thread. This is available to other threads
 *                within the same PD until another thread joins the thread
 */
void qthread_exit(int status);

/**
 * Waits for a thread to finish
 *
 * When qthread_join is called, the caller thread is blocked until the thread
 * thr exits either by calling qthread_exit or by being terminated (using
 * qthread_delete).  If thread thr has already exited, then the call returns
 * immediately with status set to EINVALID
 *
 * @param thr     Thread identifier
 * @param status  Location where exit status of the thread is stored. The status
 *                is set to QTHREAD_STATUS_DELETED if the thread is forcibly
 *                terminated.
 *
 * @return        QURT_EOK thread is joined successfully,
 * @return        QURT_EINVALID if thr is invalid,
 * @return        QURT_EVAL if status is NULL or another thread is already waiting
 *                 for thr to exit
 */
int  qthread_join(qthread_t thr, int *status);

#define qthread_set_name qurt_thread_set_name

/**
 * Get current thread's identifier
 *
 * @return  Current thread's identifier
 */
qthread_t qthread_myself(void);

/**
 * Sleep till no other thread is ready to run in the system. It gets schedule
 * when it becomes the only thread runnable. The other threads are either
 * waiting on a resource or waiting on timer.
 *
 * @return             void
 */
void qthread_wait_for_idle (void);

/**
 * Sleep till any other thread is ready to run in the system. The calling threads
 * gets immediately scheduled if any of threads is already running. If no other
 * thread runs when the API is called, the thread goes to sleep and wake up with
 * an interrupt.
 *
 * @return             void
 */
void qthread_wait_for_active (void);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QTHREAD_H */
