#ifndef DSMI_H
#define DSMI_H
/*===========================================================================

                                  D S M I . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsmi.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/2011  rj     Internalize private fields within watermark structure   
06/09/2010  hh     Included INTLOCK/INTFREE for L4 feature.
02/22/10    ag     DSM CMI changes.
01/01/05    pjb    Created
===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "amssassert.h"
#include "dsm_lock.h"
#include "dsm_queue.h"


#if defined(__GNUC__) && !defined(_lint)
  #define DSM_LIKELY(x) __builtin_expect((x), 1)
  #define DSM_UNLIKELY(x) __builtin_expect((x), 0)
#else
  #define DSM_LIKELY(x) (x)
  #define DSM_UNLIKELY(x) (x)
#endif /*(__GNUC__) */

/* GNU C89 and C99 standards
   have different interpretations for "inline" keyword.
   Since DSM code is used with different compilers
   this piece of code makes it portable !!!
   If any other compiler other than GNU C (C89 standard)
   or LLVM (C99 standard) is used
   then this part may need revision....
 */
 

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/* ------------- Runtime Debug Features ------------- */

#ifdef FEATURE_DSM_MEM_CHK
/* Only fill freed items with AA if this is non-zero. */
extern boolean DSM_Fill_AA;
#endif /* FEATURE_DSM_MEM_CHK */

/* --------- Cookies --------- */
/* Cookie must have this value */
#define DSM_COOKIE                             (0x4EAD4EAD)
#define DSM_TAIL_COOKIE                        (0xF007F007)
#define DSM_POOL_MGMT_TABLE_COOKIE             (0x7AB1E5E7)

#ifdef FEATURE_DSM_MEM_CHK
  #define DSM_TAIL_COOKIE_LEN                  (4) 
  #define DSM_HEAD_COOKIE_LEN                  (0)
#else
  #define DSM_TAIL_COOKIE_LEN                  (0) 
  #define DSM_HEAD_COOKIE_LEN                  (0)
#endif /* FEATURE_DSM_MEM_CHK */


/* a single dsm item can not hold more than 65535 bytes of data */
#define DSM_POOL_MAX_ITEM_SIZE ((uint16)-1)


#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE

/* Most EXPENSIVE memory checks require the basic DSM_MEM_CHK feature
 * be enabled
 */
#ifndef FEATURE_DSM_MEM_CHK
#define FEATURE_DSM_MEM_CHK
#endif

/* Expensive check for double free packet */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE
#endif

/*  Verify all packets for all operations */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
#endif

/*  Verify packets on enqueue */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
#endif

/* Verify that IOV packets are not duplicates. This check
 * will fail on some targets
 */
//#define FEATURE_DSM_MEM_CHK_EXPENSIVE_IOV_DUP


/* Verify that wm and actual bytes on queue match up */
//#define FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH

/* Verify that wm and actual bytes on queue match up */
//#define FEATURE_DSM_MEM_CHK_QUEUE_CACHE_LENGTH


#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE */

/************* FEATURE_DSM_AUGMENT START*****************/

/* To configure the augmented pools for a target enable the flags below
 * used only by dsm team
 * do not enable it for production version */
/* #define FEATURE_DSMI_AUG_POOL_CONFIG
   #define FEATURE_DSM_MEM_CHK_EXPENSIVE_DSM_AUGMENT */

#ifdef FEATURE_DSMI_AUG_POOL_CONFIG
  #define MSG_HIGH_DSM_CONFIG(fmt,a,b,c) MSG_HIGH(fmt,a,b,c)
#else
  #define MSG_HIGH_DSM_CONFIG(fmt,a,b,c) 
#endif

/****************DSM Library configuration*******************/
/* DSM will release memory to external memory
 * external memory allocation is
 * max. external memory
 * - DSMI_AUGMENT_EXT_MEM_BACKOFF
 */
#ifndef DSMI_AUGMENT_EXT_MEM_BACK_OFF
#define DSMI_AUGMENT_EXT_MEM_BACK_OFF 0x60000
#endif

/* DSM will not allocate from external memory if
 * external memory allocation is
 * max. external memory
 * -DSMI_AUGMENT_EXT_MEM_BACKOFF
 * -DSMI_AUGMENT_EXT_MEM_ALLOC_BACK_OFF
 */
#ifndef DSMI_AUGMENT_EXT_MEM_ALLOC_BACK_OFF
#define DSMI_AUGMENT_EXT_MEM_ALLOC_BACK_OFF 0x60000
#endif

/* Minimum free count in an augment buffer
 to recycle the buffer */
/* Only for JO.1.2, these values does not have any effect.
See coments in dsm_init.c */
#ifndef DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12
#define DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12 70 
#endif 

/* Maximum number of reuse buffers held by
    DSM library for any pool */
#ifndef DSMI_AUGMENT_TOTAL_REUSE_BUFFERS
#define DSMI_AUGMENT_TOTAL_REUSE_BUFFERS 3
#endif


/* DSM Task timer duration for free request for augment 
    buffer*/
#ifndef DSMI_AUGMENT_BUFFER_FREE_TIMER_DURATION
#define DSMI_AUGMENT_BUFFER_FREE_TIMER_DURATION 30
#endif

/* For a pool, when free_dbuffer_level is reached,
 reuse buffers are returned to external memory.
 level  = MIN((augment_count +
             (DSM_AUGMENT_REUSE_TRIGGER_FACTOR *
             augment_count/100)),
             DSM_AUGMENT_REUSE_TRIGGER_FACTOR_MAX)
*/
#ifndef DSM_AUGMENT_REUSE_TRIGGER_FACTOR 
#define DSM_AUGMENT_REUSE_TRIGGER_FACTOR  100
#endif

#ifndef DSM_AUGMENT_REUSE_TRIGGER_FACTOR_MAX
#define DSM_AUGMENT_REUSE_TRIGGER_FACTOR_MAX 200
#endif


/* Number of DSM pools to be augmented */
#ifndef DSMI_MAX_AUG_POOLS
#define DSMI_MAX_AUG_POOLS  3
#endif /* DSMI_MAX_AUG_POOLS */

/* Maximum number of augment buffers for any pool */
#ifndef DSMI_AUGMENT_TOTAL_BLOCKS
#define DSMI_AUGMENT_TOTAL_BLOCKS  32
#endif /* DSMI_AUGMENT_TOTAL_BLOCKS */


/**********************DSMA MACROS **********************/
#define DSMA_GET_TOP_FREE_BUFFER_INDEX(X) \
    !(X) ? DSMI_AUGMENT_TOTAL_BLOCKS: ((X)->pool_id_ext-1)

#define DSMA_GET_AUGMENT_BUFFER_SIZE(X,Y) \
        (DSMI_POOL_OBJECT_SIZE_OPT((X)) * (Y)) + 32 + DSM_TAIL_COOKIE_LEN + 32
        
typedef enum
{
  DSMA_FREE = 0,
  DSMA_REUSE,
  DSMA_ALLOC,
  DSMA_USED,
  DSMA_ALLOC_RECYCLED,
  DSMA_USED_RECYCLED,
  DSMA_START_RECYCLE,
  DSMA_START_REUSE,
  DSMA_START_ALLOC
}dsma_state_enum_type;

typedef struct dsma_free_list_type
{
  uint32 cnt;           /* Free count in buffer */
  dsm_item_type* top_item;     /* Pointer to top free item in buffer */
}dsma_free_list_type;

typedef struct dsm_augment_buffer_type
{
  dsma_state_enum_type dsma_state; /* state of buffer */
  uint8 *item_array;                /* Pointer to augment buffer memory array */
  /* top free item and free count are updated atomically */
  union{
  uint64 free_list;
  dsma_free_list_type dsma_free_list;
  };
} dsm_augment_buffer_type;

typedef struct dsm_augment_pool_data_s
{
  dsm_item_type* alloc_top_free;    /* Augment allocation head*/
  struct dsm_pool_mgmt_table_s * pool; /* Pool reference */
  uint32 augment_cnt;               /* Dynamic Augment count for the pool */
  uint32 free_dbuffer_level;       /* Count to free reuse buffers in system */

  dsm_item_type* recycle_top_free;  /* Preallocated buffer 1 - reuse/recycle dbuffer*/  
  dsm_item_type* dynamic_top_free;  /* Preallocated buffer 2 - reuse/new dbuffer */
  dsm_item_type* static_top_free;   /* Static Augment Buffer Head */  
  uint32 dbuffer_count;             /* current dynamic buffer count */
  
  dsm_augment_buffer_type buffer[DSMI_AUGMENT_TOTAL_BLOCKS];
                                    /* Array of augment buffers, for the pool
                                                                  Static augment buffers followed by
                                                                 dynamic augment buffers */
  uint32 recycle_count;             /* recycle count */                                                  
  uint32 new_alloc;                 /* New allocation flag */                                              
  uint32 reuse_buffer_cnt;          /* number of reuse buffers in the pool  */
  uint32 reuse_trigger_cnt;         /* if 0 , do not trigger reuse
                                                                   else used to track augment buffer free activity  */        
  uint32 static_pool_level_to_reuse_cnt;    /* if Free count in static pool reaches this level
                                                      stop reusing buffers */
  boolean dsm_task_alloc;           /* Signal DSM task to allocate dbuffer */
  boolean dsm_task_free;            /* Signal DSM task to free dbuffer */
  
  uint32 dbuffer_limit;            /* Max allowed dynamic buffers
                                                                   based on pool count */
  uint32 sbuffer_trigger;   /* Static augment alloc buffer trigger count */
  uint32 sbuffer_cnt;          /* static augment items */                                                                  
  /* Statistics */  
  uint32 max_dbuffer_count_interval; /* Max dbuffers used after no reuse */
  uint32 stats_max_dbuffer_used; /* Max dynamic augment buffers used */

}dsm_augment_pool_data_type;


/********************* FEATURE_DSM_AUGMENT END ****************************/

/*===========================================================================
                      MACRO DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
MACRO DSMI_POOL_OBJECT_SIZE(item_size,pad_size)

DESCRIPTION
  This takes the size of an item and calculates the size of the whole 
  item with overhead.

  In DSM 3 this should be a multiple of 32.  Item size should already be a 
  multiple of 32. 
---------------------------------------------------------------------------*/
/* Optimized calculation for DSMI_POOL_OBJECT_SIZE
  * Need to change DSM_ITEM_HEADER_SIZE
 *  if there is a change in header size */
#define DSMI_POOL_OBJECT_SIZE_OPT(item_size)\
 (DSM_ITEM_HEADER_SIZE + item_size)

#define DSMI_POOL_OBJECT_SIZE(item_size, pad_size)\
 ( sizeof(dsm_item_type) +  \
   DSM_HEAD_COOKIE_LEN   +  \
   item_size             +  \
   DSM_TAIL_COOKIE_LEN   +  \
   pad_size )




/*---------------------------------------------------------------------------
MACRO DSMI_ITEM_HEAD(item_ptr)

DESCRIPTION
  Return the address of the first bit of data for this item
---------------------------------------------------------------------------*/
#define DSMI_ITEM_HEAD(item_ptr)                                            \
  ((uint8*)(((uint8 *)((item_ptr) + 1)) + DSM_HEAD_COOKIE_LEN))



/*---------------------------------------------------------------------------
MACRO DSMI_ITEM_TAIL(item_ptr)

DESCRIPTION
 Return the address just past the last bit of data for this item. 
---------------------------------------------------------------------------*/
#define DSMI_ITEM_TAIL(item_ptr)                                            \
  (DSMI_ITEM_HEAD(item_ptr) +                                               \
   DSM_POOL_ITEM_SIZE(DSM_ITEM_POOL(item_ptr)))



/*---------------------------------------------------------------------------
MACRO DSMI_DUP(item_ptr)

DESCRIPTION
 Return true if this item is from a being used as a DUP or from is from
 a DUPLICATE pool.
---------------------------------------------------------------------------*/
#define DSMI_DUP(item_ptr)                            \
  ( (item_ptr)->dup_ptr != NULL ||                    \
    DSM_POOL_ITEM_SIZE(DSM_ITEM_POOL(item_ptr)) == 0 )

#define DSMI_IMUTABLE(item_ptr)               \
  ( ((item_ptr)->references != 1) || DSMI_DUP(item_ptr) )



/*---------------------------------------------------------------------------
MACRO DSMI_HEAD_SIZE(item_ptr)

DESCRIPTION
  Returns the size between the head pointer and the data pointer
---------------------------------------------------------------------------*/
#define DSMI_HEAD_SIZE(item_ptr) \
  ( (item_ptr)->data_ptr - DSMI_ITEM_HEAD(item_ptr) )



/*---------------------------------------------------------------------------
MACRO DSMI_TAIL_SIZE(item_ptr)

DESCRIPTION
  Returns the size of unused space between the current data and the end
  of the packet.
---------------------------------------------------------------------------*/
#define DSMI_TAIL_SIZE(item_ptr) \
  ( (DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr) - (item_ptr)->used )



/*---------------------------------------------------------------------------
MACRO DSMI_SIZE(item_ptr)

DESCRIPTION
  Returns the value in the local size field of the item, or what it should
  be if the size field is not defined for the dsm_item_type.
---------------------------------------------------------------------------*/
#define DSMI_SIZE(item_ptr) \
  ( DSMI_DUP(item_ptr) ? (item_ptr)->used : \
    ( DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr ) )



/*---------------------------------------------------------------------------
MACRO DSM_NOTUSED(x)

DESCRIPTION
  Tricks lint into thinking that the variable is used when it's not
---------------------------------------------------------------------------*/
#define DSM_NOTUSED(i) if(i){}


/* -------------------------------------------------------------------------
MACRO DSMI_POOL_LOCK(pool), DSMI_POOL_UNLOCK(pool) 
 
DESCRIPTION 
  Macros for obtaining and releasing the pool mutex.
---------------------------------------------------------------------------*/ 
#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_POOL_LOCK( pool ) \
    DSM_LOCK( &(((dsm_pool_mgmt_table_type*)(pool))->lock) ) 
#elif defined FEATURE_L4
  #define DSMI_POOL_LOCK( pool ) \
    INTLOCK() 
#else 
  #define DSMI_POOL_LOCK( pool ) \
    INTLOCK(); \
    TASKLOCK()
#endif    

#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_POOL_UNLOCK( pool ) \
    DSM_UNLOCK( &(((dsm_pool_mgmt_table_type*)(pool))->lock) )
#elif defined FEATURE_L4 
  #define DSMI_POOL_UNLOCK( pool ) \
    INTFREE()
#else 
  #define DSMI_POOL_UNLOCK( pool ) \
    TASKFREE(); \
    INTFREE() 
#endif 
  
/* -------------------------------------------------------------------------
MACRO DSMI_QUEUE_LOCK_WM(pool), DSMI_QUEUE_UNLOCK_WM(pool) 
 
DESCRIPTION 
  Macros for obtaining and releasing the watermark mutex.
---------------------------------------------------------------------------*/ 

#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    DSM_LOCK( &((wm_ptr)->lock) )
#elif defined FEATURE_DSM_QUBE 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    qmutex_lock((qmutex_t)(wm_ptr)->lock)
#elif defined FEATURE_L4 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    INTLOCK()
#else 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    INTLOCK(); \
    TASKLOCK()
#endif  
  
#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    DSM_UNLOCK( &((wm_ptr)->lock) )
#elif defined FEATURE_DSM_QUBE 
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    qmutex_unlock((qmutex_t)(wm_ptr)->lock)
#elif defined FEATURE_L4  
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    INTFREE() 
#else
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    TASKFREE(); \
    INTFREE()
#endif   

#define DSMI_QUEUE_LOCK_CREATE(wm_ptr) \
  DSM_LOCK_CREATE( &((wm_ptr)->lock) )

#define DSMI_QUEUE_LOCK_DESTROY(wm_ptr) \
  DSM_LOCK_DESTROY( &((wm_ptr)->lock) )

/*---------------------------------------------------------------------------
                            FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
uint8 dsmi_ref_inc(dsm_item_type * item_ptr);
uint8 dsmi_ref_dec(dsm_item_type * item_ptr);
void dsmi_verify_buffer(dsm_item_type * item_ptr);
void dsmi_verify_mem_level(dsm_mem_level_enum_type mem_level);
void dsmi_verify_mem_op(dsm_mem_op_enum_type mem_op);
void dsmi_check_mem_all_pools(void);
/*---------------------------------------------------------------------------
                            FUNCTION DECLARATIONS
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION DSMI_VERIFY_POOL_ID()

DESCRIPTION 
  This function verifies that the pool ID handed in is a valid one.
  (ie. in the set of pools we are currently managing.

DEPENDENCIES
  None
  
PARAMETERS
  pool_id - Pool id to check range of. 

RETURN VALUE
  None (This function won't return in pool_id is bogus.)

SIDE EFFECTS
  If the pool_id is bogus ERR_FATAL is called, otherwise none.
===========================================================================*/
static __inline void dsmi_verify_pool_id(dsm_mempool_id_type pool_id)
{
  dsm_pool_mgmt_table_type *table=(dsm_pool_mgmt_table_type*)pool_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* ASSERT((unsigned int)table > 256); */
  if(table->cookie != DSM_POOL_MGMT_TABLE_COOKIE)
  {
    ERR_FATAL("dsm: Invalid Pool ID = %d", (int)pool_id, 0, 0);
  }
}
/*===========================================================================
FUNCTION DSMI_UPDATE_CB_LEVELS()

DESCRIPTION
  Update CB levels for the pool

DEPENDENCIES
  None.

PARAMETERS
  pool_id - Pool from which new item should be allocated/freed.
  level - Next MEM event callback level/table index

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_update_cb_levels(
    dsm_pool_mgmt_table_type*table,
    dsm_mem_level_enum_type level);
#ifdef FEATURE_DSM_AUGMENT

/*========================================================================
FUNCTION dsmi_augment_check_flow_control_levels()

DESCRIPTION
 Check whether the flow control levels matches the current dsm augment
   configuration

DEPENDENCIES
 None

PARAMETERS
  table - pool
  available_item_count - Flow control level

RETURN VALUE
  Return whether the available item count matches the current dsm augment
  configuration

SIDE EFFECTS None
=========================================================================*/
boolean dsmi_augment_check_flow_control_levels(dsm_pool_mgmt_table_type* table,
          uint32 available_item_count);

/*===========================================================================
FUNCTION DSM_AUGMENT_EXTERNAL_MEM_INIT()

DESCRIPTION
  This function will initialize DSM Pool, if augmented from
  external memory
  This function is called
   - if Heap memory is not available when DSM is being initialized.
   - if heap is not initialized
   - if static DSM memory is only needed.
DEPENDENCIES
  None
PARAMETERS
  None
RETURN VALUE
  None
SIDE EFFECTS
  Should be called after external memory and dsm pool is initialized
===========================================================================*/
void dsm_augment_external_mem_init(void);

/*========================================================================
FUNCTION dsmi_augment_init_pool()

DESCRIPTION
 Initialize DSM augment data structures for the pool.

DEPENDENCIES
 Pool is initialized.
 External memory manager is initialized.

PARAMETERS
  table - DSM pool to augment
  augment_count - Augment item count for the pool
  augment_items - Total DSM items to be allocated from augment pool
  static_augment_items - number of static augment items
  static_augment_item_array - Address of static pool, from which Static Augment buffer can be allocated.

RETURN VALUE
  None

SIDE EFFECTS
  This function is NOT threadsafe.
=========================================================================*/
void dsmi_augment_init_pool(dsm_pool_mgmt_table_type* table,
        uint32 augment_count,
        uint32 augment_items,
        uint32 static_augment_items,
        uint8* static_augment_item_array);

/*========================================================================
FUNCTION dsma_alloc_dynamic_reuse_recycle_buffer()

DESCRIPTION
  Traverse through reused/used dynamic buffers and try to reuse and
  recycle buffers.

DEPENDENCIES

PARAMETERS
  aug_data - Augment data of the pool

RETURN VALUE  None

SIDE EFFECTS
=========================================================================*/
void dsma_alloc_dynamic_reuse_recycle_buffer(
        dsm_augment_pool_data_type *aug_data);

/*========================================================================
FUNCTION DSM_AUGMENT_POOLS()

DESCRIPTION
  This API is called from High Priority DSM task to augment pool memory from external memory.
  Also called from dsm_init()

DEPENDENCIES

PARAMETERS

RETURN VALUE  None

SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe.
  Pool's item  count and free count are modified.
=========================================================================*/
void dsm_augment_pools(void);

/*========================================================================
FUNCTION DSM_AUGMENT_FREE_BUFFER()

DESCRIPTION
    This API is called from High Priority DSM task after augment pool memory
    to perform necessary cleanup of freed buffers.

DEPENDENCIES

PARAMETERS

RETURN VALUE  
  boolean - False, indicates, there are more reuse buffers to be freed,
                 True , indicates all reuse buffers are freed.
                 
SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe.
  Pool's item count and free count are modified.
=========================================================================*/
boolean dsm_augment_free_buffer(void);

#endif /* FEATURE_DSM_AUGMENT */

#endif /* DSMI_H */
