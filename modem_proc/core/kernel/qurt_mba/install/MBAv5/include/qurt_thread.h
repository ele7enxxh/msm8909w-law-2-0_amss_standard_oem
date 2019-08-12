#ifndef QURT_THREAD_H
#define QURT_THREAD_H
/*=============================================================================

                 qurt_thread.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
    Prototypes of Thread API  

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_thread.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/06/11   op      Add header file, Fix Doxygen warning messages
=============================================================================*/

#include <string.h>

/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/

/** 
 * Macros for Qurt Thread Configuration 
 *
 * Bitmask config is for selecting DSP hardware threads
 *
 * For selecting all the hardware threads, using
 *  QURT_THREAD_CFG_BITMASK_ALL and,
 *   - For QDSP6 V2/V3, it selects all 6 hw threads
 *   - For QDSP6 V3L, it selects all 4 hw threads
 *   - For QDSP6 V4, it selects all 3 hw threads 
 */  

#define QURT_THREAD_CFG_BITMASK_HT0      0x00000001
#define QURT_THREAD_CFG_BITMASK_HT1      0x00000002
#define QURT_THREAD_CFG_BITMASK_HT2      0x00000004
#define QURT_THREAD_CFG_BITMASK_HT3      0x00000008
#define QURT_THREAD_CFG_BITMASK_HT4      0x00000010
#define QURT_THREAD_CFG_BITMASK_HT5      0x00000020
#define QURT_THREAD_CFG_BITMASK_ALL      0x000000ff

#define QURT_THREAD_CFG_USE_RAM          0x00000000
#define QURT_THREAD_CFG_USE_TCM          0x00000100


/** 
 * Macros for Qurt Thread Attributes 
 *
 */  
#define QURT_HTHREAD_L1I_PREFETCH      0x1     /**<enable HW L1 Instruction cache prefetching */
#define QURT_HTHREAD_L1D_PREFETCH      0x2     /**<enable HW L1 Data cache prefetching */
#define QURT_HTHREAD_L2I_PREFETCH      0x4     /**<enable HW L2 Instruction cache prefetching */
#define QURT_HTHREAD_L2D_PREFETCH      0x8     /**<enable HW L2 Data cache prefetching */
#define QURT_HTHREAD_DCFETCH           0x10    /**<enable DC fetch. DCFETCH instructs HW that a data memory access is likely */
                                                /**<to be made to the provided virtual address. It's recommended DCFETCH */
                                                /**<instructions be dropped in case of high bus utilization */


#define QURT_THREAD_ATTR_NAME_MAXLEN            16
#define QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT  0
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT       256
#define QURT_THREAD_ATTR_ASID_DEFAULT           0
#define QURT_THREAD_ATTR_AFFINITY_DEFAULT      (-1)
#define QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT   (-2)

/*=============================================================================
												TYPEDEFS
=============================================================================*/

typedef enum {
    CCCC_PARTITION = 0,     /**< Use CCCC page attribute bits to determine MAIN or AUX paritition */
    MAIN_PARTITION = 1,     /**< Use Main Partition */
    AUX_PARTITION = 2,      /**< Use Aux partition */
    MINIMUM_PARTITION = 3   /**< Use Minimum. Allocate least amount of cache (no-allocate policy possible) for this thread */
} qurt_cache_partition_t;

/**
 * Thread ID type
 */
typedef unsigned int qurt_thread_t;

/**
 * Thread attributes 
 */
typedef struct _qurt_thread_attr {
    char name[QURT_THREAD_ATTR_NAME_MAXLEN]; /**< Thread name */
    unsigned char tcb_partition;  /**< Should the thread TCB reside in RAM or
                                       on chip memory (i.e. TCM) */
    unsigned char affinity;       /**< HW bitmask indicating the threads it
                                       can run on */
    unsigned short priority;      /**< Thread priority */
    unsigned short asid;          /**< Address space ID */
    unsigned short timetest_id;   /**< TIMETEST ID */
    unsigned int stack_size;      /**< Thread's stack size */
    void *stack_addr;             /**< Stack address base.
                                       (stack_addr, stack_addr+stack_size-1)
                                       is the range of the stack */
} qurt_thread_attr_t;

/*=============================================================================
												FUNCTIONS
=============================================================================*/

static inline void qurt_thread_attr_init (qurt_thread_attr_t *attr)
{

    attr->name[0] = 0;
    attr->tcb_partition = QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT;
    attr->priority = QURT_THREAD_ATTR_PRIORITY_DEFAULT;
    attr->asid = QURT_THREAD_ATTR_ASID_DEFAULT;
    attr->affinity = QURT_THREAD_ATTR_AFFINITY_DEFAULT;
    attr->timetest_id = QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT;
    attr->stack_size = 0;
    attr->stack_addr = 0;
}

static inline void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, char *name)
{
    strncpy (attr->name, name, QURT_THREAD_ATTR_NAME_MAXLEN);
    attr->name[QURT_THREAD_ATTR_NAME_MAXLEN - 1] = 0;
}

static inline void qurt_thread_attr_set_tcb_partition (qurt_thread_attr_t *attr, unsigned short tcb_partition)
{
    attr->tcb_partition = tcb_partition;
}

static inline void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, unsigned short priority)
{
    attr->priority = priority;
}

static inline void qurt_thread_attr_set_affinity (qurt_thread_attr_t *attr, unsigned char affinity)
{
    attr->affinity = affinity;
}

static inline void qurt_thread_attr_set_timetest_id (qurt_thread_attr_t *attr, unsigned short timetest_id)
{
    attr->timetest_id = timetest_id;
}

static inline void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, unsigned int stack_size)
{
    attr->stack_size = stack_size;
}

static inline void qurt_thread_attr_set_stack_addr (qurt_thread_attr_t *attr, void *stack_addr)
{
    attr->stack_addr = stack_addr;
}

void qurt_thread_get_name (char *name, unsigned char max_len);

/**
 * Create a QuRT thread  
 *
 * @param  attr  	pointer to the thread attributes such as stack addr
 * @param  entrypoint   Entry function of the thread
 * @param  arg  	Argument 
 *
 * @output thread_id	Thread ID
 *
 * @return Qurt thread ID 
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *), void *arg);

/**
 * Stop current thread
 *
 * Free kernel TCB and yield to the next highest ready thread. 
 *
 * @return void 
 */
void qurt_thread_stop(void);

/**
 * Resume a suspended thread
 *
 * A thread can be suspended due to following reasons:
 *  - suspended by other thread through calling qurt_thread_suspend() (not supported yet)
 *  - suspended by kernel due to page fault or other error
 *
 * @param  thread_id 
 * @return EOK: success
 * @return E_FATAL: failed
 */
int qurt_thread_resume(unsigned int thread_id);

/**
 * Get thread id of the current thread 
 *
 * @return Current qurt thread id 
 */
qurt_thread_t qurt_thread_get_id (void);

/**
 * Set timetest ID for this thread
 *
 * @param  tid 
 * @return void
 */
void qurt_thread_set_timetest_id (unsigned short tid);

#if 0
/**
 * Set prefectch value in ssr 
 *
 * @param   settings	The least significant 5 bits will be used to set prefetchability of the calling
 * thread. Please refer to the Q6 system manual for hw meanings
 */
void qurt_thread_set_prefetch(unsigned int settings);
#endif

/**
 * Get timetest ID of the current thread 
 *
 * @return thread id 
 */
unsigned short qurt_thread_get_timetest_id (void);

/**
 * exit current thread with status 
 *
 * @param   status status value of exit 
 */
void qurt_thread_exit(int status);

/**
 * Join a thread 
 *
 * @param   tid  thread id to join
 * @param   status [OUT]  exit status 
 *
 * @return  ENO_THREAD if not a valid thread id( thread has already exited) 
 * @return  EOK if join thread successfully with valid status value 
 */
int qurt_thread_join(unsigned int tid, int *status);

/**
 * Get the address of anysignal object in the UTCB of current sw thread 
 *
 * @return  address of anysignal object
 *
 */
unsigned int qurt_thread_get_anysignal(void);

/**
 * Get priority of a thread                
 *
 * @param threadid 	    Thread ID	
 * @return              -1 if thread id is not valid anymore(only for non-static version) 
 * @return 			 	0-255 priority of the thread	
 */
int qurt_thread_get_priority (qurt_thread_t threadid);

/**
 * Set priority of a thread                
 *
 * @param threadid 	    Thread ID	
 * @param newprio 	    priority to set	
 * @return              -1 if thread id is not valid anymore(only for non-static version) 
 * @return 			 	0 if successful	
 */
int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio);

/**
 * Get Qurt API version
 *
 * @return              Qurt API version
 */
unsigned int qurt_api_version(void);

#endif /* QURT_THREAD_H */

