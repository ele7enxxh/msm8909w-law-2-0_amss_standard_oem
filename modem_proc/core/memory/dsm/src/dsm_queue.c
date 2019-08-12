/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ Q U E U E . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_dequeue()
    Dequeue the next buffer item from the passed Watermark structure. Perform
    relevent 'get' event functions if appropriate.
    
  dsm_enqueue()
    Enqueue the passed buffer item onto the passed Watermark structure. 
    Perform any relevent 'put' event functions as appropriate.

  dsm_multiple_dequeue()
    Dequeue multiple items from the watermark queue

  dsm_multiple_enqueue()
    Enqueue multiple packets into watermark queue

  dsm_empty_queue()
    completely empty the dsm watermark queue.

  dsm_simple_enqueue_isr()
    This function will put the passed DSM item to the passed shared queue 
    then check for and perform any 'put' events.  This function does not 
    check for priority.  It simply enqueues to the tail of the queue.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.
  dsm_queue_init() must be called prior to using a DSM watermark queue.

Copyright (c) 2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsm_queue.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/31/15    sko     CR 871980 - Added Shailesh's provided changes
05/31/15    sko    Added multiple enqueue/dequeue functionality
08/07/13    sko    Added dne_q_cnt
06/06/12    sh      Added dne_statistics
06/06/12    sh      Added dne_callback
06/06/12    sh      Added priority insert optimization 
02/29/12    sh      Callback provided for each WM dequeue operation
04/06/2011  rj     Internalize private fields within watermark structure   
03/22/11    rj     Added debug code
03/08/11    ag     Remove FEATURE_DSM_WM_CB
08/01/11    hh     Memset queue to 0 only once to avoid mutex leaks.
04/02/10    hh     Used cookie to check the watermark initialization and set 
                   the queue buffer to 0 in dsm_queue_init.
03/09/10    ag     Removing modem_mcs_defs.h as its breaking apps build.
03/08/10    ag     Include modem_mcs_defs.h
02/22/10    ag     DSM CMI Changes.
01/12/10    ag     Merging DSM WM tracing changes from //depot.
09/17/09    ag     Fixed lint warnings.
04/27/09    ag     Replaced assert.h with amssassert.h
04/13/09    ag     Added a check in dsm_init to check if the lock is already 
                   initialized before then do not re-initialize it. 
01/26/07    mjb    Added queue-specific locking mechanism.
06/29/06    mjb    Added file/line tracing,promoted internal uint16 to uint32
01/18/05    pjb    Added dsm_is_wm_empty
01/01/05    pjb    Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "core_variation.h"
#include "comdef.h"
#include "queue_services.h"

#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_lock.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "amssassert.h"
#ifdef FEATURE_DSM_NATIVE_ASSERT
#define ASSERT( cond )      assert( cond )
#endif
#include "dsm_queue.h"



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
static boolean dsm_queue_global_debug_flag = FALSE;

/*===========================================================================

                MACROS AND DEFINES

===========================================================================*/
#define DSM_WM_CB(CB_FPTR,WM_PTR,CB_DATA) do {\
  if(CB_FPTR != NULL) (CB_FPTR)(WM_PTR,CB_DATA);}while(0)

#define DSM_QUEUE_SENTINEL (0x0ff51DE5)

#define DSMI_CHECK_WM_PTR_EXPENSIVE(wm_ptr)\
  do { if( wm_ptr->sentinel != DSM_QUEUE_SENTINEL ){\
         MSG_MED("DSM WM QUEUE UNINITIALIZED:Call dsm_queue_init on WMs before using.",0,0,0);\
         }} while(0)    

#define DSMI_CHECK_WM_PTR(wm_ptr)\
  do { if((wm_ptr == NULL) || (wm_ptr->q_ptr == NULL)){\
        ERR_FATAL("DSM WM POINTER or WM QUEUE POINTER 0x%x IS NULL",(int)wm_ptr,0,0);\
       }\
       } while(0)
       
/* Functions to trace DSM WM*/
uint32 __attribute__((weak))dsm_wm_trace_register(dsm_watermark_type *wm_ptr)
{
  /*  return WM ID to trace or log
     *  used for internal WM tracing functions
     */
  return 0;
};
void __attribute__((weak))dsm_wm_trace_deregister(dsm_watermark_type *wm_ptr)
{
  /*  Deregister for logging functions
     *  used for internal WM tracing functions
     */
};


/*===========================================================================
FUNCTION dsmi_wm_debug_verify_enqueue()

DESCRIPTION
   Perform sanity checks based on masks on items , used in multiple_enqueue
   and dequeue

DEPENDENCIES
  None

PARAMETERS
  wm_ptr - Pointer to the watermark item header.
  chain_head_ptr - Head of packet chain , linked together by calling dsm_chain_packet()
  chain_tail_ptr -   Tail of chain pointer, chain_tail_ptr=chain_head_ptr,
                           when num_pkts_in_chain = 1
  num_pkts_in_chain - Number of packets in chain
  total_pkt_len_of_chain - Total pkt_len of all packets in chain

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void dsmi_wm_debug_verify_enqueue(
  dsm_watermark_type *wm_ptr,
  dsm_item_type *chain_head_ptr,
  dsm_item_type *chain_tail_ptr,
  uint16 num_pkts_in_chain,
  uint32 total_pkt_len_of_chain,
  const char * file,
  uint32 line)
{
   dsm_item_type *chain_ptr = chain_head_ptr;
   dsm_item_type *tail_ptr = chain_tail_ptr ;
   uint32 num_items = 0;
   uint32 total_pkt_len = 0;

   while (chain_ptr !=NULL)
   {
#ifdef FEATURE_DSM_MEM_CHK
      if (dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_SET_TRACE_BIT0))
      {
         dsmi_touch_item_int(chain_ptr, file,line);
      }
#endif
      /* DSM_WM_DEBUG_LOG_WM_ENQUEUE_DEQUEUE_BIT3  not checked
        again*/
      DSM_WM_CB(wm_ptr->log_enq_func_ptr,wm_ptr,chain_ptr);
      if (dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_VERIFY_ENQ_PKT_LEN_BIT1))
      {
         if (total_pkt_len_of_chain !=0)
         {
            if(chain_ptr->dsm_pkt_len !=dsm_length_packet( chain_ptr))
            {
               ERR_FATAL("DSM WM DEBUG Packet length incorrect for item 0x%x in enqueued chain head 0x%x",
                                  (int)chain_ptr, (int)chain_head_ptr,0);
            }
         }
      }
      if (dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_VERIFY_MULTIPLE_ENQ_PKT_CHAIN_BIT2))
      {
        if ((chain_ptr->references == 0) ||
          (chain_ptr->priority != chain_head_ptr->priority) )
        {
         ERR_FATAL("DSM WM DEBUG Invalid reference count/priority for item 0x%x in chain head 0x%x ",
            (int)chain_ptr, (int)chain_head_ptr,0);
        }
#ifdef FEATURE_DSM_MEM_CHK
        dsmi_verify_pool_id(chain_ptr->pool_id);
#endif
         num_items++;
         tail_ptr = chain_ptr;
         total_pkt_len += dsm_length_packet( chain_ptr);
      }
      dsm_pkt_chain_next(&chain_ptr);
  }
  if (dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_VERIFY_MULTIPLE_ENQ_PKT_CHAIN_BIT2))
  {
      if ((num_items !=num_pkts_in_chain) ||
           (tail_ptr != chain_tail_ptr)||
           (total_pkt_len !=total_pkt_len_of_chain))
      {
        MSG_6(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "DSM WM DEBUG ERROR - Enqueue Packet chain 0x%x, num_packets %d cmp %d, pkt_len %d cmp %d check tail_ptr 0x%x",
        (int) chain_head_ptr,
        (int) num_pkts_in_chain,
        (int) num_items,
        (int) total_pkt_len_of_chain,
        (int) total_pkt_len,
        (int) tail_ptr
        );
        ERR_FATAL("DSM WM DEBUG ERROR - invalid packet chain in enqueue chain head 0x%x, num_packets %d, total_pkt_len %d",
          (int)chain_head_ptr,num_pkts_in_chain,total_pkt_len_of_chain);
      }
  }
}

/*===========================================================================
FUNCTION dsmi_wm_debug_verify_dequeue()

DESCRIPTION
   Perform sanity checks based on masks on items , used in multiple_dequeue

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark. 

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void dsmi_wm_debug_verify_dequeue(
  dsm_watermark_type *wm_ptr,
  dsm_item_type *chain_head_ptr,
  const char * file,
  uint32 line)
{
   while (chain_head_ptr !=NULL)
   {
#ifdef FEATURE_DSM_MEM_CHK
     if (dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_SET_TRACE_BIT0))
     {
       dsmi_touch_item_int(chain_head_ptr, file,line);
     }
#endif
     /* Packet length will be verified during enqueue if
      * DSM_WM_DEBUG_VERIFY_ENQ_PKT_LEN_BIT1 is on, no need to check in dequeue
      */
     /* DSM_WM_DEBUG_LOG_WM_ENQUEUE_DEQUEUE_BIT3  not checked
        again*/
     DSM_WM_CB(wm_ptr->log_deq_func_ptr,wm_ptr,chain_head_ptr);     
     dsm_pkt_chain_next(&chain_head_ptr);
  }
}
/*===========================================================================
FUNCTION DSMI_WM_CHECK_LEVELS_ENQUEUE()

DESCRIPTION
   Call Watermark item call back functions if water mark level change 
   appropriately.

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark.  

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void
dsmi_wm_check_levels_enqueue(dsm_watermark_type * wm_ptr, 
                     uint32 initial_count)
{
  boolean check_cb_hilo; 
  check_cb_hilo = (wm_ptr->hiwater_func_ptr || wm_ptr->lowater_func_ptr);
  
  if(check_cb_hilo)
  {
      if(initial_count <= wm_ptr->hi_watermark &&
              wm_ptr->current_cnt > wm_ptr->hi_watermark)
      {
         DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
      }
      if(initial_count >= wm_ptr->lo_watermark &&
          wm_ptr->current_cnt < wm_ptr->lo_watermark)
      {
         DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,
                wm_ptr->lowater_func_data);
      }
  }
  if(initial_count == 0)
  {
     if ( wm_ptr->current_cnt > 0)
     {
       DSM_WM_CB(wm_ptr->non_empty_func_ptr,wm_ptr,wm_ptr->non_empty_func_data);
     }          
  }    
} /* dsmi_wm_check_levels_enqueue */


/*===========================================================================
FUNCTION DSMI_WM_CHECK_LEVELS_ENQUEUE_OPT()

DESCRIPTION
   Call Watermark item call back functions if water mark level change 
   appropriately.

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark.  

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void
dsmi_wm_check_levels_enqueue_opt
(
  dsm_watermark_type * wm_ptr, 
  uint32 initial_count
)
{
  /* most of the time current count is going to be less than high so add this as
     first check */
  if((wm_ptr->current_cnt > wm_ptr->hi_watermark) &&
     (initial_count <= wm_ptr->hi_watermark))
  {
     DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
  }

  /* Orginal function before CR 871980
       if(initial_count >= wm_ptr->lo_watermark &&
          wm_ptr->current_cnt < wm_ptr->lo_watermark)
      {
         DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,
                wm_ptr->lowater_func_data);
      }
    */
  if(initial_count == 0)
  {
    DSM_WM_CB(wm_ptr->non_empty_func_ptr,wm_ptr,wm_ptr->non_empty_func_data);
  }    
} /* dsmi_wm_check_levels_enqueue_opt */

/*===========================================================================
FUNCTION DSMI_WM_CHECK_LEVELS_DEQUEUE()

DESCRIPTION
   Call Watermark item call back functions if water mark level change 
   appropriately for dequeue

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark.  
RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void
dsmi_wm_check_levels_dequeue(dsm_watermark_type * wm_ptr, 
                     uint32 initial_count)
{
  boolean check_cb_hilo;

  check_cb_hilo = (wm_ptr->hiwater_func_ptr || wm_ptr->lowater_func_ptr);
  
  if(check_cb_hilo)
  {
       if(initial_count >= wm_ptr->lo_watermark &&
          wm_ptr->current_cnt < wm_ptr->lo_watermark)
       {
          DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,
              wm_ptr->lowater_func_data);
       }
       if(initial_count <= wm_ptr->hi_watermark &&
           wm_ptr->current_cnt > wm_ptr->hi_watermark)
       {
            DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
       }      
  }
  if ( wm_ptr->current_cnt == 0)
  {
    DSM_WM_CB(wm_ptr->gone_empty_func_ptr,wm_ptr,
                wm_ptr->gone_empty_func_data);          
  }
} /* dsmi_wm_check_levels_dequeue */


/*===========================================================================
FUNCTION DSMI_WM_CHECK_LEVELS_DEQUEUE_OPT()

DESCRIPTION
   Call Watermark item call back functions if water mark level change 
   appropriately for dequeue

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark.  

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void
dsmi_wm_check_levels_dequeue_opt
(
  dsm_watermark_type * wm_ptr, 
  uint32 initial_count
)
{
  if(initial_count >= wm_ptr->lo_watermark &&
     wm_ptr->current_cnt < wm_ptr->lo_watermark)
  {
    DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,
              wm_ptr->lowater_func_data);
  }
  /* Original function before CR 871980
      if(initial_count <= wm_ptr->hi_watermark &&
           wm_ptr->current_cnt > wm_ptr->hi_watermark)
       {
            DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
       }      
   */
  if ( wm_ptr->current_cnt == 0)
  {
    DSM_WM_CB(wm_ptr->gone_empty_func_ptr,wm_ptr,
              wm_ptr->gone_empty_func_data);
  }
} /* dsmi_wm_check_levels_dequeue_opt */

/*===========================================================================
FUNCTION DSMI_WM_HANDLE_OVERFLOW()

DESCRIPTION
   Call Watermark item call back functions if water mark levels exceed

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   pkt_head_ptr - Pointer to pointer to item that may be deleted
   pkt_len - Packet length of packet

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
void
dsmi_wm_handle_overflow(dsm_watermark_type * wm_ptr, 
                        dsm_item_type **pkt_head_ptr,
                        uint32 pkt_len,
                        const char * file,
                        uint32 line)
{
  uint32 send_log;
  if(wm_ptr->q_ptr->cnt > wm_ptr->dne_q_cnt) 
  {
    wm_ptr->dne_q_cnt_discard_pkt_cnt++; 
    wm_ptr->dne_q_cnt_discard_bytes_cnt += pkt_len;
    /*----------------------------------------------------------------------- 
      Display message, put item to free queue. 
      -----------------------------------------------------------------------*/
    /* Message for every 16 packets */
    send_log = (!wm_ptr->dne_log_frequency) ?
      ((wm_ptr->dne_q_cnt_discard_pkt_cnt & 0xf) == 0):
      ((wm_ptr->dne_q_cnt_discard_pkt_cnt % wm_ptr->dne_log_frequency)==0);
	
    if (send_log)
    {
      MSG_6(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "WM full - dne_Q_cnt hit,Pkt freed 0x%x:WM 0x%x: Discard cnt %d :WM File 0x%x,line %d: dne_q_cnt %d",
          (int) *pkt_head_ptr,
          (int) wm_ptr,
          (int) wm_ptr->dne_q_cnt_discard_pkt_cnt,
          (int) wm_ptr->reserved[0],
          (int) wm_ptr->reserved[1],
          (int) wm_ptr->dne_q_cnt
          );
    }
  }
  else if (wm_ptr->current_cnt > wm_ptr->dont_exceed_cnt)
  {
    wm_ptr->dne_discard_pkt_cnt++; 
    wm_ptr->dne_discard_bytes_cnt += pkt_len;

    /*----------------------------------------------------------------------- 
      Display message, put item to free queue. 
      -----------------------------------------------------------------------*/
    /* Message for every 16 packets */
    send_log = (!wm_ptr->dne_log_frequency) ?
     ((wm_ptr->dne_discard_pkt_cnt & 0xf) == 0):
     ((wm_ptr->dne_discard_pkt_cnt % wm_ptr->dne_log_frequency)==0);
    if (send_log)
    {
      MSG_6(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "WM full,freeing packet 0x%x:Watermark 0x%x:Discard cnt %d:WM File 0x%x,line %d: dne %d",                  
          (int) *pkt_head_ptr,
          (int) wm_ptr,
          (int) wm_ptr->dne_discard_pkt_cnt,
          (int) wm_ptr->reserved[0],
          (int) wm_ptr->reserved[1],
          (int) wm_ptr->dont_exceed_cnt
          );
    }
  }
  
  /* free the packet outside watermark lock */
  (void)dsmi_free_packet(pkt_head_ptr,file,line);
  
} /* dsmi_wm_handle_overflow */

/*===========================================================================
FUNCTION DSMI_WM_GET_COUNT()

DESCRIPTION
   Traverse the watermark queue and get the actual count of bytes on it.
   This is EXPENSIVE and is only used internally for debug.

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.

RETURN VALUE
  Count of bytes on watermark queue.

SIDE EFFECTS
  None.
===========================================================================*/
uint32
dsmi_wm_get_count(dsm_watermark_type * wm_ptr)
{
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  uint32 count = 0;
  dsm_item_type * dsm_ptr;

  if( wm_ptr->q_ptr != NULL )
  {
    dsm_ptr = (dsm_item_type*)q_check_s_ll_i( wm_ptr->q_ptr );
    while( dsm_ptr != NULL )
    {
      count += dsm_ptr->used;
      dsm_ptr = (dsm_item_type*)q_next_s_ll_i(wm_ptr->q_ptr,&(dsm_ptr->link));
    }
  }
  return count;
#else
  MSG_FATAL("FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH needs to defined", 
             0, 0, 0);
  return 0;
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */
} /* dsmi_wm_get_count */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSM_QUEUE_INIT()

DESCRIPTION
   This function initializes a watermark queue.  Setting all the callbacks and 
   callback data to NULL, watermark levels to 0, and initializing the queue 
   that this will use. Since this potentially initializes a mutex, the
   dsm_queue_destroy() function needs to be called to tear down a WM queue.

DEPENDENCIES
   None

PARAMETERS
   wm_ptr - Pointer to the watermark to initialize
   dne - Do not exceed level for this watermark
   queue - Pointer to the queue header that this water mark should use

RETURN VALUE
   None

SIDE EFFECTS
   Queue is initialized
   Mutexes might be initialized.
===========================================================================*/
void dsmi_queue_init
(
  dsm_watermark_type *wm_ptr,
  int dne,
  q_type * queue,
  const char * file,
  uint16 line

)
{
  dword lock_init;
  uint32 sentinel;  /* Cookie to check watermark initialization */
  uint32 trace_id;

  ASSERT(wm_ptr != NULL);
  ASSERT(queue != NULL);        /* You must have a queue */

  sentinel = wm_ptr->sentinel;  /* Save the previous cookie */
  lock_init = wm_ptr->lock;
  trace_id = DSMI_GET_WM_TRACE_ID(wm_ptr);
  memset(wm_ptr, 0, sizeof(dsm_watermark_type));
  //Only create lock once. Check watermark is initialized or not.
  if (sentinel != DSM_QUEUE_SENTINEL) {
    DSMI_QUEUE_LOCK_CREATE(wm_ptr);
    memset(queue, 0, sizeof(q_type));
    (void)q_init_s(queue,TRUE);  
    wm_ptr->reserved[2]=dsm_wm_trace_register(wm_ptr);
  }
  else {
    wm_ptr->lock = lock_init;
    wm_ptr->reserved[2] = trace_id;
  }
  wm_ptr->dont_exceed_cnt = dne;
  wm_ptr->reserved[0] = (uint32)file;
  wm_ptr->reserved[1] = line;
  
  wm_ptr->q_ptr = queue;

  wm_ptr->q_prio_insert_ptr = &wm_ptr->q_ptr->link;

  wm_ptr->sentinel = DSM_QUEUE_SENTINEL;
  wm_ptr->dne_q_cnt = 0xFFFFFFFF;

  if (dsm_queue_global_debug_flag == TRUE)
  {
    dsm_wm_set_debug_flag(wm_ptr,DSM_WM_DEBUG_SET_TRACE_BIT0);
    dsm_wm_set_debug_flag(wm_ptr, DSM_WM_DEBUG_VERIFY_ENQ_PKT_LEN_BIT1);    
    dsm_wm_set_debug_flag(wm_ptr, DSM_WM_DEBUG_VERIFY_MULTIPLE_ENQ_PKT_CHAIN_BIT2); 
  }
}
/*===========================================================================
FUNCTION DSM_ENQUEUE()

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue then
  check for and perform any 'put' events.

DEPENDENCIES
  1) Both parameters must NOT be NULL.
  2) The prioritized queuing will always put a DSM_HIGHEST priority item to
     the head of the queue.

PARAMETERS
  wm_ptr - Pointer to Watermark item to put to
  pkt_head_ptr - Pointer to pointer to item to add to queue

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_enqueue
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  uint32 initial_count;          /* initial count of bytes in WM           */
  uint32 pkt_len ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);

  if((pkt_head_ptr == NULL) || (*pkt_head_ptr == NULL)
      || ((*pkt_head_ptr)->references == 0))
  {
    ERR_FATAL("dsm_enqueue: Invalid Parameter", 0, 0, 0);
  }

#ifdef FEATURE_DSM_MEM_CHK
  /* Check if this item is already enqueued in some queue. */
  ASSERT((*pkt_head_ptr)->link.next_ptr == NULL);
  if ((*pkt_head_ptr)->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
  {
    ERR_FATAL("DSM WM, DSM Item flag, dsm_q_use_pkt_len_bit set in the packet 0x%x ",
                                (int)(*pkt_head_ptr),0,0);
  }
#endif

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
  dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE */

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */


#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);
#endif
  /*-------------------------------------------------------------------------
    If the Watermark Item does not now have too many bytes in it then 
    proceed to stuff the passed item into it. Check 'priority'field to 
    determine whether or not the item should be put to front of queue.
  -------------------------------------------------------------------------*/
  pkt_len=dsm_length_packet(*pkt_head_ptr);
  if (DSM_LIKELY((*pkt_head_ptr)->dsm_pkt_len ==0))
  {    
    (*pkt_head_ptr)->dsm_pkt_len = pkt_len;
    (*pkt_head_ptr)->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_LIBARY;
  }  
  else if(dsm_wm_get_debug_flag(wm_ptr,DSM_WM_DEBUG_VERIFY_ENQ_PKT_LEN_BIT1))
  {
    /* dsm_pkt_len field set by client, do not believe it , 
          as it will result in several stability issues in data path- CR 898989*/
    if((*pkt_head_ptr)->dsm_pkt_len != pkt_len)
    {
       ERR_FATAL("DSM WM DEBUG Packet length incorrect for item 0x%x in enqueue",
                                  (int)(*pkt_head_ptr),0,0);
    }
  }
  
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  
  if (DSM_LIKELY((wm_ptr->q_ptr->cnt < wm_ptr->dne_q_cnt) &&
    (wm_ptr->current_cnt < wm_ptr->dont_exceed_cnt)))
  {
    /* CR 520541 - Current count could exceed DNE count. */ 
    initial_count = wm_ptr->current_cnt;
    wm_ptr->total_rcvd_cnt += pkt_len;
    

#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item_int(*pkt_head_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */

    if ((*pkt_head_ptr)->priority == DSM_NORMAL)
    {
      /*-------------------------------------------------------- 
           Put to queue at the tail.
      ----------------------------------------------------------*/
      q_put_s_ll_i(wm_ptr->q_ptr, &(*pkt_head_ptr)->link);      

    }
    else
    {
      if((*pkt_head_ptr)->priority == DSM_HIGHEST)
      {
        /*---------------------------------------------------
             The following functionality ensures that a message with dsm_highest
             is not added to a queue before another message with dsm_highest 
             priority. (ds_to_ps_q was in mind when adding this functionality).

             Inserts item before DSM_NORMAL priorty items on the queue.
             ---------------------------------------------------------*/
         q_insert_after_s_ll_i(wm_ptr->q_ptr, 
                  &(*pkt_head_ptr)->link,
                  wm_ptr->q_prio_insert_ptr);
         wm_ptr->q_prio_insert_ptr = &(*pkt_head_ptr)->link;      

      }  /* if (*pkt_head_ptr.........)*/
      else
      {
        if((*pkt_head_ptr)->priority == DSM_PRIORITY_ENQ_HIGHEST)
        {
      
         /*--------------------------------------------------------
              Inserts item at the head and so it will be the first one to be dequeued.
              These items are dequeued before DSM_NORMAL  and DSM_HIGHEST
              priority items.
                - follows LIFO rule for dequeuing these items 
                ---------------------------------------------------------*/

           q_insert_after_s_ll_i(wm_ptr->q_ptr, 
                  &(*pkt_head_ptr)->link,
                  &wm_ptr->q_ptr->link);
           if (wm_ptr->q_prio_insert_ptr ==  &wm_ptr->q_ptr->link)
           {
             wm_ptr->q_prio_insert_ptr = &(*pkt_head_ptr)->link;     
           }
     
        }
        else
        {
          /* default  case */
          /*-------------------------------------------------------- 
                 Put to queue at the tail.
                ----------------------------------------------------------*/
          q_put_s_ll_i(wm_ptr->q_ptr, &(*pkt_head_ptr)->link);      
        }
      }
    }
     
    wm_ptr->current_cnt += pkt_len;
    if(wm_ptr->current_cnt > wm_ptr->highest_cnt)
    {
      wm_ptr->highest_cnt = wm_ptr->current_cnt;
    }

    /*-----------------------------------------------------------------------
      Now check for the Non-empty and Hi-watermark events.
    -----------------------------------------------------------------------*/
    dsmi_wm_check_levels_enqueue(wm_ptr, initial_count);
    DSM_WM_CB(wm_ptr->each_enqueue_func_ptr,
              wm_ptr,wm_ptr->each_enqueue_func_data);
    DSM_WM_CB(wm_ptr->log_enq_func_ptr,wm_ptr,*pkt_head_ptr);
    DSMI_QUEUE_UNLOCK_WM(wm_ptr);
    *pkt_head_ptr = NULL;
  }
  else
  {
    DSM_WM_CB(wm_ptr->dne_func_ptr,wm_ptr,*pkt_head_ptr);
    DSMI_QUEUE_UNLOCK_WM(wm_ptr);
    dsmi_wm_handle_overflow(wm_ptr,pkt_head_ptr,pkt_len,file,line);
  }
} /* dsmi_enqueue() */

/*===========================================================================
FUNCTION DSM_SIMPLE_ENQUEUE_ISR()

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue then
  check for and perform any 'put' events.  This function does not check
  for priority.  It simply enqueues to the tail of the queue.

DEPENDENCIES
  1) Both parameters must NOT be NULL.
  2) Does not support packet chaining.
  3) Should only be called from ISR or from within critical section in which
     interrupts are disabled.

PARAMETERS
  wm_ptr - Pointer to watermark to put to
  pkt_head_ptr - Pointer to pointer to item to put.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_simple_enqueue_isr
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type *temp_ptr;      /* pointer to 1st item in queue */
  uint32 initial_count;     /* initial count of bytes in WM */
  uint32 item_length;       /* length of new item to add */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  ASSERT(pkt_head_ptr != NULL && *pkt_head_ptr != NULL);
  DSMI_CHECK_WM_PTR(wm_ptr);

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);
#endif

  #ifdef FEATURE_DSM_MEM_CHK
  /* Check if this item is already enqueued in some queue. */
  ASSERT((*pkt_head_ptr)->link.next_ptr == NULL);
  #endif

  temp_ptr = *pkt_head_ptr;

  ASSERT(temp_ptr->pkt_ptr == NULL);

  /*-------------------------------------------------------------------------
    If the Watermark Item does not now have too many bytes in it then 
    proceed to stuff the passed item into it. Check 'priority'field to 
    determine whether or not the item should be put to front of queue.
  -------------------------------------------------------------------------*/
  /* promote from 16bit to 32bit */
  item_length = temp_ptr->used;
  if (!(temp_ptr->dsm_pkt_len))
  {
    temp_ptr->dsm_pkt_len = item_length;
    temp_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_LIBARY;
  }  
  /* else do not believe it */
    
  #ifdef FEATURE_DSM_MEM_CHK
  if ((*pkt_head_ptr)->references == 0) 
    {
      ERR_FATAL("References are 0 for packet 0x%x while inserting within WM 0x%x",
                (int)*pkt_head_ptr, (int)wm_ptr, 0);
    }
  #endif
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  if((wm_ptr->current_cnt + item_length) <= wm_ptr->dont_exceed_cnt)
  {
    initial_count = wm_ptr->current_cnt;
    wm_ptr->total_rcvd_cnt += item_length;

#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item_int(*pkt_head_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */

    q_put_s_ll_i(wm_ptr->q_ptr, &(temp_ptr->link));
    wm_ptr->current_cnt += item_length;
    if(wm_ptr->current_cnt > wm_ptr->highest_cnt)
    {
      wm_ptr->highest_cnt = wm_ptr->current_cnt;
    }

    /*-----------------------------------------------------------------------
      Now check for the Non-empty and Hi-watermark events.
    -----------------------------------------------------------------------*/
    dsmi_wm_check_levels_enqueue(wm_ptr, initial_count); 
    DSM_WM_CB(wm_ptr->each_enqueue_func_ptr,
              wm_ptr,wm_ptr->each_enqueue_func_data);
  }
  else 
  {
   
   DSM_WM_CB(wm_ptr->dne_func_ptr,wm_ptr,*pkt_head_ptr);   
   dsmi_wm_handle_overflow(wm_ptr,pkt_head_ptr,item_length,file,line);
  }
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  *pkt_head_ptr = NULL;

} /* dsm_simple_enqueue_isr() */

/*===========================================================================
FUNCTION DSM_MULTIPLE_ENQUEUE() - (DSM_PKT_LEN_API)

DESCRIPTION
  The function is used to enqueue multiple packets to WM , to be
  used for performance critical paths. DSM Header field , "dsm_pkt_len" , if, set by client 
  is expected to be correct in this API and Library will *use* the field directly without error checking.
  
  
  Usage below.
    dsm_item_type *chain_tail_ptr,*chain_head_ptr = NULL;
    dsm_item_type* enq_pkts[5]; Packets to be enqueued
    uint32 total_pkt_len = 0;
    chain_head_ptr = chain_tail_ptr  = enq_pkts[0]; First pkt to enqueue
    for (i = 1; i<5;i++){
       dsm_pkt_chain_link_with_len(&chain_tail_ptr,enq_pkt[i],20);
       total_pkt_len+=20;
    }
    dsm_multiple_enqueue(wm_ptr,&chain_head_ptr,&chain_tail_ptr,5,total_pkt_len);
    
  1) Filestamps of packets in chain not updated by default
  2) Will only check for low_watermark and invoke callbacks once, if conditions are satisfied.
  3) DNE count checked once.

DEPENDENCIES  
  All conditions below to be met for packets in the packet chain.
  1) The chain should be made using using dsm_pkt_chain_link_with_len()/dsm_pkt_chain_link_update_pkt_len()
  2) Num_pkts_in_chain should be accurate, no error checking
  3) Priority of chain_head_ptr used for enqueue rules, and all packets inchain to have
      same type of priority, no error checking
  4) Total_pkt_len_of_chain should contain cumulative length of all the packets to be enqueued.
  5) Correct length of each packet in chain is expected to be
     in dsm_pkt_len field of header (no error checking for length of each packet)


PARAMETERS
  wm_ptr - Pointer to Watermark item to put to
  chain_head_ptr - Head of packet chain , linked together by calling dsm_chain_packet()
  chain_tail_ptr -   Tail of chain pointer
  num_pkts_in_chain - Number of packets in chain
  total_pkt_len_of_chain - Total pkt_len of all packets in chain
  Being a DSM_PKT_LEN_API, DSM Header "dsm_pkt_len" field is an input parameter - length of each packet
           in chain is expected to be in dsm_pkt_len field of header (no error checking for length of each packet)

RETURN VALUE
  None

SIDE EFFECTS
  WM_debug_mask for a watermark can be enabled to check for 
    * to check whether num_pkts_in_chain, total_pkt_len_of_chain is correct
    * to check whether packet length is correct for each enqueued packet
    * to check whether all packets have the same priority field
    * to set filestamp for each enqueued packet
 
  IMPORTANT: When wm_debug_mask is 0, the function avoids
  looping through the chain of packets
===========================================================================*/
void dsmi_multiple_enqueue
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **chain_head_ptr, 
  dsm_item_type **chain_tail_ptr, 
  uint16 num_pkts_in_chain, 
  uint32 total_pkt_len_of_chain, 
  const char * file,
  uint32 line
)
{
      uint32 initial_count;          /* initial count of bytes in WM           */
      dsm_item_type* chain_iter;
    
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      DSMI_CHECK_WM_PTR(wm_ptr);

      if((chain_head_ptr == NULL) || (*chain_head_ptr == NULL)  
         ||(chain_tail_ptr == NULL) || (*chain_tail_ptr == NULL))
      {
        ERR_FATAL(" DSM Multiple enqueue  Invalid parameter chain 0x%x, chain_tail 0x%x  num_pkts $%d",
            chain_head_ptr,chain_tail_ptr,num_pkts_in_chain);
      }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
      DSMI_QUEUE_LOCK_WM(wm_ptr);
      ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
      DSMI_QUEUE_UNLOCK_WM(wm_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */
    
#ifdef FEATURE_DSM_MEM_CHK
      dsmi_verify_pool_id((*chain_head_ptr)->pool_id);
#endif
      if (DSM_UNLIKELY(wm_ptr->wm_debug_mask !=0))
      {
         /* DO NOT Enable the mask unless needed specifically for error checking
         as it affects performance in data path */
         dsmi_wm_debug_verify_enqueue(wm_ptr,*chain_head_ptr,*chain_tail_ptr,num_pkts_in_chain,
                                           total_pkt_len_of_chain,file,line);
      }
       DSMI_QUEUE_LOCK_WM(wm_ptr);
      
      if (DSM_LIKELY((wm_ptr->q_ptr->cnt < wm_ptr->dne_q_cnt) &&
        (wm_ptr->current_cnt < wm_ptr->dont_exceed_cnt)))
      {
        /* CR 520541 - Current count could exceed DNE count by chain length */ 
        initial_count = wm_ptr->current_cnt;
        chain_iter = *chain_head_ptr ;
        while (chain_iter !=NULL)
        {
          /* Looping through the packets can be removed, 
                      when obsolete function, dsm_pkt_chain_link() is removed */
          if (chain_iter->dsm_pkt_len != 0)
          {
            chain_iter->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_PKT_LEN_APIS;   
          }
          else
          {
            ERR_FATAL("DSM Header field,dsm_pkt_len is 0 for packet 0x%x in chain_ptr 0x%x in dsmi_multiple_enqueue function",
                 chain_iter,chain_head_ptr,0);
          }
          dsm_pkt_chain_next(&chain_iter);          
        }        
        wm_ptr->total_rcvd_cnt += total_pkt_len_of_chain;
        
        /*-------------------------------------------------------------------------
         Check 'priority' field of chain head pointer to determine whether or not the item should be put to front of queue.
        -------------------------------------------------------------------------*/

        if ((*chain_head_ptr)->priority == DSM_NORMAL)
        {
          /*-------------------------------------------------------- 
               Put to queue at the tail.
          ----------------------------------------------------------*/
          q_chain_put_s_ll_i(wm_ptr->q_ptr, 
              &(*chain_head_ptr)->link,
              &(*chain_tail_ptr)->link,
              num_pkts_in_chain);

        }
        else
        {
          if((*chain_head_ptr)->priority == DSM_HIGHEST)
          {
            /*---------------------------------------------------
                 The following functionality ensures that a message with dsm_highest
                 is not added to a queue before another message with dsm_highest 
                 priority. (ds_to_ps_q was in mind when adding this functionality).
    
                 Inserts item before DSM_NORMAL priorty items on the queue.
                 ---------------------------------------------------------*/
             q_chain_insert_after_s_ll_i(wm_ptr->q_ptr, 
                      &(*chain_head_ptr)->link,
                      &(*chain_tail_ptr)->link,
                      num_pkts_in_chain,
                      wm_ptr->q_prio_insert_ptr);
             wm_ptr->q_prio_insert_ptr = &(*chain_tail_ptr)->link;
    
          }  /* if (*pkt_head_ptr.........)*/
          else
          {
            if((*chain_head_ptr)->priority == DSM_PRIORITY_ENQ_HIGHEST)
            {
          
             /*--------------------------------------------------------
                  Inserts item at the head and so it will be the first one to be dequeued.
                  These items are dequeued before DSM_NORMAL  and DSM_HIGHEST
                  priority items.
                    - follows LIFO rule for dequeuing these items 
                    ---------------------------------------------------------*/
    
               q_chain_insert_after_s_ll_i(wm_ptr->q_ptr, 
                                        &(*chain_head_ptr)->link,
                                        &(*chain_tail_ptr)->link,
                                        num_pkts_in_chain,
                                        &wm_ptr->q_ptr->link);
               if (wm_ptr->q_prio_insert_ptr ==  &wm_ptr->q_ptr->link)
               {
                 wm_ptr->q_prio_insert_ptr = &(*chain_tail_ptr)->link;     
               }
         
            }
            else
            {
              /* default  case */
              /*-------------------------------------------------------- 
              Put to queue at the tail.
          ----------------------------------------------------------*/
               q_chain_put_s_ll_i(wm_ptr->q_ptr, 
                                                  &(*chain_head_ptr)->link,
                                                  &(*chain_tail_ptr)->link,
                                                  num_pkts_in_chain);      
            }
          }
        }
         
        wm_ptr->current_cnt += total_pkt_len_of_chain;
        if(wm_ptr->current_cnt > wm_ptr->highest_cnt)
        {
          wm_ptr->highest_cnt = wm_ptr->current_cnt;
        }
    
        /*-----------------------------------------------------------------------
          Now check for the Non-empty and Hi-watermark events.
        -----------------------------------------------------------------------*/
        dsmi_wm_check_levels_enqueue_opt(wm_ptr, initial_count);
        DSM_WM_CB(wm_ptr->each_enqueue_func_ptr,
                  wm_ptr,wm_ptr->each_enqueue_func_data);
        DSMI_QUEUE_UNLOCK_WM(wm_ptr);
        *chain_head_ptr = NULL;
        *chain_tail_ptr = NULL;
      }
      else
      {
        DSM_WM_CB(wm_ptr->dne_func_ptr,wm_ptr,*chain_head_ptr);
        DSMI_QUEUE_UNLOCK_WM(wm_ptr);
        while (*chain_head_ptr !=NULL)
        {
          chain_iter = dsm_pkt_chain_delink(chain_head_ptr);
          dsmi_wm_handle_overflow(wm_ptr,&chain_iter,total_pkt_len_of_chain,file,line);
        }
        *chain_tail_ptr = NULL;
      }
}


/*==================================================================
FUNCTION DSM_ENQUEUE_SPECIAL()

DESCRIPTION
  This function will handle special cases for dsm_enqueue function. 

DEPENDENCIES
  1) Both parameters must NOT be NULL.

PARAMETERS
  wm_ptr - Pinter to Watermark item to put to
  pkt_head_ptr - Pointer to pointer to item to add to queue
  dsm_enqueue_rule – Enum that specifies overriding DSM enqueue rule 
                                   for this enqueue operation

RETURN VALUE
  None

SIDE EFFECTS:
  This function could break rules of WM including DNE , HI_WM and LO_WM setting and behaviour.
  Behaviour will be based on DSM_ENQUEUE rule for this enqueue operation.
  
  IMPORTANT : Should be used only for very special cases. Avoid using this function for 
   regular DSM WM enqueue operation, use dsm_enqueue() instead.
===================================================================*/
void dsmi_enqueue_special
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  dsm_wm_enq_rule_enum dsm_enqueue_rule,
  const char * file,
  uint32 line
)
{
  uint32 ldont_exceed_cnt, ldne_q_cnt;          /* initial count of bytes in WM           */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  switch (dsm_enqueue_rule)
  {
     case DSM_WM_ENQ_RULE_IGNORE_DNE:       
      /* CR 569252 - Requirement to ignore DNE count while enqueing in WM
             This could also be done using 2 WMs , instead of using this function. */

      /* Set DNE counts to MAX,  call enqueue function,
              Restore DNE counts.              
              This rule should be used only for very special cases.  
              Locks WM twice for the operation.
           */
      DSMI_QUEUE_LOCK_WM(wm_ptr);    
      /* Store DNE values */
      ldont_exceed_cnt = wm_ptr->dont_exceed_cnt;
      wm_ptr->dont_exceed_cnt = 0xFFFFFFFF;
      ldne_q_cnt = wm_ptr->dne_q_cnt;
      wm_ptr->dne_q_cnt = 0xFFFFFFFF;
      dsmi_enqueue(wm_ptr,pkt_head_ptr,file,line);

      /* Put back DNE values */
      wm_ptr->dont_exceed_cnt = ldont_exceed_cnt;
      wm_ptr->dne_q_cnt = ldne_q_cnt; 
      DSMI_QUEUE_UNLOCK_WM(wm_ptr);
      break;
    default:
      ASSERT(dsm_enqueue_rule < DSM_WM_ENQ_RULE_MAX);
    break;
  }/* end switch */
} /* dsmi_enqueue_special */

/*===========================================================================
FUNCTION DSM_DEQUEUE()

DESCRIPTION
  This function will return a pointer to the next item on the shared queue
  associated with the passed Watermark item. This function will also update
  the 'current_cnt' field in the passed Watermark item and check for and
  perform any relevent 'get' events.

DEPENDENCIES
  The parameter must NOT be NULL.

PARAMETERS
  wm_ptr - Pointer to watermark item to get item from 

RETURN VALUE
  A pointer to a 'dsm_item_type' or NULL if no item_array available.

SIDE EFFECTS
===========================================================================*/
dsm_item_type *dsmi_dequeue
(
 dsm_watermark_type *wm_ptr,
 const char * file, 
 uint32 line
)
{
  uint32 initial_count;     /* initial number of bytes in watermark */
  dsm_item_type *item_ptr;      /* pointer to item to retrieve */
  uint32 item_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */

  if((item_ptr = (dsm_item_type *)q_get_s_ll_i(wm_ptr->q_ptr)) != NULL)
  {
    if (item_ptr->references == 0) 
    {
      ERR_FATAL("References are 0 for item 0x%x accessed within WM 0x%x",
                (int)item_ptr, (int)wm_ptr, 0);
    }

    if (wm_ptr->q_prio_insert_ptr == &item_ptr->link)
    {
       /* If the last priority item is deleted, change the insert pointer to the q link */
       wm_ptr->q_prio_insert_ptr = &wm_ptr->q_ptr->link;
    }
    /*-----------------------------------------------------------------------
      First get current count then get the item and update current count.
      Check for and perform Lo water and Gone Empty events if appropriate.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item_int(item_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */
    
    initial_count = wm_ptr->current_cnt;
    /* Directly read the set length as no one touched the item
       already enqueued */
       
    if (item_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
    {
      item_length = item_ptr->dsm_pkt_len; 
      if (item_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit == DSM_PKT_LEN_SET_BY_LIBARY)
      {
        item_ptr->dsm_pkt_len = 0;
      }
      item_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_CLIENT;
    }
    else
    {
      /* dsm_pkt_len not set by DSM, do not believe the field  - CR 898989*/
      item_length = dsm_length_packet(item_ptr);
    }
    
    if (wm_ptr->current_cnt < item_length) 
    {
      ERR_FATAL("Current count for WM %x is less that length for item 0x%x"
      , (int)wm_ptr, (int)item_ptr, 0);
    }
    wm_ptr->current_cnt -= item_length;

    dsmi_wm_check_levels_dequeue(wm_ptr, initial_count);
    DSM_WM_CB(wm_ptr->each_dequeue_func_ptr,
              wm_ptr,wm_ptr->each_dequeue_func_data);
  }
  else
  {
    if (wm_ptr->current_cnt != 0) 
    {
      ERR_FATAL("WM 0x%x current_cnt is not 0 ", (int)wm_ptr, 0, 0);
    }
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
    if ( wm_ptr->q_prio_insert_ptr != &wm_ptr->q_ptr->link)
    {
        ERR_FATAL("WM 0x%x priority items dequeued without using dsm_dequeue ", (int)wm_ptr, 0, 0);
    }
#endif
  }
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  DSM_WM_CB(wm_ptr->log_deq_func_ptr,wm_ptr,item_ptr);
  return item_ptr;

} /* dsm_dequeue() */

/*===========================================================================
FUNCTION DSM_MULTIPLE_DEQUEUE()

DESCRIPTION
The function is used to dequeue multiple packets from WM , to be used for performance 
  critical paths.  
  Number of packets dequeued is MIN(num_pkts_to_dequeue, number of packets in WM)
  Need to call dsm_pkt_chain_delink()/dsm_pkt_chain_delink_reset_pkt_len() in a loop to retrieve the returned packets

  Usage below.
  dsm_item_type *chain_head_ptr = NULL;
  dsm_item_type *delinked_item;
  uint32 deq_pkt_len;
  dsm_multiple_dequeue(wm,5,&chain_head_ptr,&deq_pkt_len);
  while (chain_head_ptr !=NULL)
  {
     delinked_item = dsm_pkt_chain_delink_reset_pkt_len(&chain_head_ptr);
     ..use delinked item..
  }

DEPENDENCIES
  1) Filestamps of the dequeued packets not updated.
  2) Length of packet is expected for each dequeued packet  
  3) Will only check for hi_watermark, and invoke callbacks once, if conditions are satisfied.

PARAMETERS
  wm_ptr - Pointer to Watermark 
  num_pkts_to_dequeue - Pointer to pointer to item to add to queue
  chain_head_ptr - Dequeued packet chain's head, NULL, if no packets are dequeued
  dequeued_chain_length - Packet length of items dequeued 

RETURN VALUE
  Returns total number of items dequeued
  Parameter dequeued_chain_length contains cumulative packet length of items dequeued
  
SIDE EFFECTS
  WM_debug_mask for a watermark can be enabled 
    * to calculate and check whether packet length is correct for each dequeued packet
    * set file-stamp in each dequeued items     
===========================================================================*/
uint32 dsmi_multiple_dequeue
(
  dsm_watermark_type *wm_ptr,
  uint32 num_pkts_to_dequeue,  
  dsm_item_type** chain_head_ptr,
  uint32* dequeued_chain_length,
  const char * file,
  uint32 line
)
{
    uint32 initial_count;     /* initial number of bytes in watermark */
    dsm_item_type *tail_dsm_ptr;      /* pointer to item to retrieve */
    uint32 item_length = 0;
    uint32 total_items = 0;
    uint8 no_priority_items_flag = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    DSMI_CHECK_WM_PTR(wm_ptr);

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
    ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */

    /* Function below was expanded below for performance
       if((*chain_head_ptr = (dsm_item_type *)q_chain_get_s_ll(wm_ptr->q_ptr,num_pkts_to_dequeue)) != NULL) */
       
    if (num_pkts_to_dequeue)
    {
      DSMI_QUEUE_LOCK_WM(wm_ptr);
      total_items = q_cnt_s(wm_ptr->q_ptr);
      initial_count = wm_ptr->current_cnt;
      if (total_items > 0)
      {
        if (total_items <= num_pkts_to_dequeue)
        {
          *chain_head_ptr = (dsm_item_type *) wm_ptr->q_ptr->link.next_ptr;
          /* empty queue */
          wm_ptr->q_ptr->link.prev_ptr->next_ptr = NULL;
          wm_ptr->q_ptr->link.next_ptr = &wm_ptr->q_ptr->link;
          wm_ptr->q_ptr->link.prev_ptr = &wm_ptr->q_ptr->link;
          wm_ptr->q_ptr->cnt = 0;  

          /* If the last item is deleted, change the insert pointer to 
                 the q link */
          wm_ptr->q_prio_insert_ptr = &wm_ptr->q_ptr->link;
          /* If queue is emptied and there are no priority items do not loop 
                 through the deququed items */
          item_length = initial_count;        
          wm_ptr->current_cnt = 0;
        }
        /* if priority items are not present  - most common case*/
        else if (wm_ptr->q_prio_insert_ptr == &wm_ptr->q_ptr->link)
        {
          /* number of queue count is greater than requested items */
          total_items = num_pkts_to_dequeue;

          *chain_head_ptr = (dsm_item_type *) wm_ptr->q_ptr->link.next_ptr;

          wm_ptr->q_ptr->cnt -= num_pkts_to_dequeue;

          tail_dsm_ptr = *chain_head_ptr;

          if (tail_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
          {
            item_length += tail_dsm_ptr->dsm_pkt_len;
          }
          else
          {
            /* do not believe the field CR 898989 */
            item_length += dsm_length_packet(tail_dsm_ptr);
          }

          num_pkts_to_dequeue--;
      
          while (num_pkts_to_dequeue > 0)
          {
            tail_dsm_ptr = (dsm_item_type *) tail_dsm_ptr->link.next_ptr;
            if (tail_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
            {
              item_length += tail_dsm_ptr->dsm_pkt_len;
            }
            else
            {
              /* do not believe the field CR 898989 */
              item_length += dsm_length_packet(tail_dsm_ptr);
            }
            num_pkts_to_dequeue--;
          }
      
          wm_ptr->q_ptr->link.next_ptr = tail_dsm_ptr->link.next_ptr;

          tail_dsm_ptr->link.next_ptr = NULL;

          wm_ptr->current_cnt -= item_length;
        }
        else /* if priority items are present */
        {
          /* number of queue count is greater than requested items */
          total_items = num_pkts_to_dequeue;

          *chain_head_ptr = (dsm_item_type *) wm_ptr->q_ptr->link.next_ptr;

          wm_ptr->q_ptr->cnt -= num_pkts_to_dequeue;

          tail_dsm_ptr = *chain_head_ptr;

          no_priority_items_flag |= 
            (wm_ptr->q_prio_insert_ptr == &tail_dsm_ptr->link);

          if (tail_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
          {
            item_length += tail_dsm_ptr->dsm_pkt_len;
          }
          else
          {
            /* do not believe the field CR 898989 */
            item_length += dsm_length_packet(tail_dsm_ptr);
          }

          num_pkts_to_dequeue--;
      
          while (num_pkts_to_dequeue > 0)
          {
            tail_dsm_ptr = (dsm_item_type *) tail_dsm_ptr->link.next_ptr;

            no_priority_items_flag |= 
              (wm_ptr->q_prio_insert_ptr == &tail_dsm_ptr->link);

            if (tail_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
            {
              item_length += tail_dsm_ptr->dsm_pkt_len;
            }
            else
            {
              /* do not believe the field CR 898989 */
              item_length += dsm_length_packet(tail_dsm_ptr);
            }
            num_pkts_to_dequeue--;
          }
      
          if (no_priority_items_flag)
          {
            /* The last priority item has already been dequeued, change the insert pointer to
               the q link */ 
            wm_ptr->q_prio_insert_ptr = &wm_ptr->q_ptr->link;
          }

          wm_ptr->q_ptr->link.next_ptr = tail_dsm_ptr->link.next_ptr;

          tail_dsm_ptr->link.next_ptr = NULL;

          wm_ptr->current_cnt -= item_length;
      }

      /* item_length is same as initial_count */
      dsmi_wm_check_levels_dequeue_opt(wm_ptr, initial_count);
        
      DSM_WM_CB(wm_ptr->each_dequeue_func_ptr,
         wm_ptr,wm_ptr->each_dequeue_func_data);
      
      if (DSM_UNLIKELY(wm_ptr->wm_debug_mask !=0))
      {
        /* DO NOT ENABLE the mask unless needed specifically for error checking
                    as it affects performance in data path */
        dsmi_wm_debug_verify_dequeue(wm_ptr,*chain_head_ptr, file,line);
      }
    }
    else /* queue is empy */
    {
      *chain_head_ptr = NULL;      
      if (wm_ptr->current_cnt != 0)
      {
        ERR_FATAL("WM 0x%x current_cnt is not 0 ", (int)wm_ptr, 0, 0);
      }
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
      if ( wm_ptr->q_prio_insert_ptr != &wm_ptr->q_ptr->link)
      {
          ERR_FATAL("WM 0x%x priority items dequeued without using dsm_dequeue ", (int)wm_ptr, 0, 0);
      }
#endif
    }
    DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  }      

  if (dequeued_chain_length != NULL)
  {
    *dequeued_chain_length = item_length;
  }

  return total_items;
}
/*===========================================================================
FUNCTION DSM_EMPTY_QUEUE()

DESCRIPTION
  This routine completely empties a queue.
  
DEPENDENCIES
  None

PARAMETERS
  wm_ptr - Pointer to watermark queue to empty

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_empty_queue
( 
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  while((item_ptr = dsmi_dequeue(wm_ptr,file,line)) != NULL)
  {
    (void)dsmi_free_packet(&item_ptr,file,line);
  } 
} /* dsm_empty_queue() */

/*===========================================================================
FUNCTION DSM_WM_QUEUE_GET()

DESCRIPTION
  Get the front item of the given dsm queue. Item is NOT removed from queue.


DEPENDENCIES
  The parameter must NOT be NULL.

PARAMETERS
  wm_ptr - Pointer to watermark item to get item from 

RETURN VALUE
  A pointer to a 'dsm_item_type' or NULL if no item_array available.

SIDE EFFECTS
  The function provides additional checks on queue's current_cnt. However
   it is not recommended to use the function for error checking or debugging.
===========================================================================*/
dsm_item_type *dsm_wm_queue_get
(
dsm_watermark_type *wm_ptr
)
{
  dsm_item_type *item_ptr;      /* pointer to item to retrieve */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */

  if((item_ptr = (dsm_item_type *)q_check_s_ll_i(wm_ptr->q_ptr)) == NULL)
  {
    ASSERT(wm_ptr->current_cnt == 0);
  }

  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  return item_ptr;

} /* dsm_wm_queue_get() */

/*===========================================================================
FUNCTION dsm_wm_iterator_get_normal_prio_head_cb

DESCRIPTION
    DSM WM client function for DSM_WM_ITERATOR_WITH_DEQUEUE()
    to get the head of normal priority items
DEPENDENCIES
  None
PARAMETERS
  wm_ptr - Water Mark pointer
  *pkt_head_ptr,<= Queue Packet passed by DSM iterator
  req_flag <= Iterator request
  cb_usr_data -  cb_usr_data provided by usr
RETURN VALUE
  dsm_wm_iterator_enum_type
SIDE EFFECTS
===========================================================================*/
static dsm_wm_iterator_enum_type dsm_wm_iterator_get_normal_prio_head_cb
( const dsm_watermark_type *wm_ptr,
  const dsm_item_type *pkt_head_ptr,
  dsm_wm_iterator_req_enum_type *req_flag,
  void* client_data)
{
  if (pkt_head_ptr != NULL)
  {
    if (pkt_head_ptr->priority == DSM_NORMAL)
    {
      *req_flag = DSM_WM_DEQUEUE_PKT;
      return DSM_WM_ITERATOR_STOP;
    }
  }
  return DSM_WM_ITERATOR_CONTINUE;
}
/*===========================================================================
FUNCTION DSM_DEQUEUE_NORMAL_PRIO_HEAD()

DESCRIPTION
  The function returns head of Normal PRIORITY ITEMS

DEPENDENCIES
  None

PARAMETERS
  wm_ptr - Pointer to watermark queue for iteration and optional dequeue

RETURN VALUE
  Dequeued Normal Priority Head

SIDE EFFECTS
===========================================================================*/
dsm_item_type *dsmi_dequeue_normal_prio_head
( dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
)
{
  return dsmi_wm_iterator_with_dequeue_modify_pkts
          ( wm_ptr,
            DSM_WM_ITERATOR_NORMAL_PRIORITY_HEAD,
            &dsm_wm_iterator_get_normal_prio_head_cb,
            NULL,
            NULL,
            file,
            line);
}
/*===========================================================================
FUNCTION dsmi_wm_iterator_with_dequeue_modify_pkts()

DESCRIPTION
  The function provides an iterator over DSM WM queue with optional dequeue and/or
  modification of enqueued WM packets from any location in WM queue.
  If the enqueued packets are modified, necessary sanity checks are done,
    and the trace of enqueued packets are updated.

  Iterator traverses queue from head or from normal priority head of the queue.

  Iteration on queue is stopped
     1) when q_tail is reached
     2) when client iterator request to stop

DEPENDENCIES
  None

PARAMETERS
  wm_ptr - Pointer to watermark queue for iteration and optional dequeue
  start_iter - Defines where to start the iteration from
  dsm_wm_iterator_cb - User provided iterator function.
     User Function evaluates the packet provided to it and provides input
     for continuing the iteration and to optionally dequeue the packet
     dsm_wm_iterator_enum_type (*dsm_wm_iterator_cb_type)
       (  const dsm_watermark_type *wm_ptr, <= Pointer to Watermark
          const dsm_item_type *pkt_head_ptr,<= Queue Packet passed by DSM iterator
          dsm_wm_iterator_req_enum_type *req_type, <= Set by client to indicate dequeue,modify
          void* cb_usr_data) <= cb_usr_data provided by usr
  dsm_wm_iterator_modifier_cb <= Callback provided by client for modification of
          enqueued packets
          ( const dsm_watermark_type *wm_ptr,<= Pointer to Watermark
            dsm_item_type *pkt_head_ptr,<= Queue Packet passed by DSM iterator, modifiable
            void* cb_usr_data); <= cb_usr_data provided by usr
  cb_usr_data - Passed to dsm_wm_iterator_f_cb

RETURN VALUE
  Chain of dequeued dsm packets, linked using (dsm_item_type)pkt.link->next_ptr.
  Order of dequeue is maintained in the chain.
SIDE EFFECTS
  DSM Packets may be dequeued from the WM queue from any location
  Enqueued DSM Packets may be MODIFIED without being dequeued.

  This function involves queue traversal and modifying enqueued
  DSM Packets and should be used with caution.
  The function can be also used to dequeue all items from the queue. It is up
  to client to limit number of dequeues, to avoid locking WM for longer duration.

===========================================================================*/
dsm_item_type *dsmi_wm_iterator_with_dequeue_modify_pkts
( dsm_watermark_type *wm_ptr,
  dsm_start_iterator_enum_type start_iter,
  wm_iterator_cb_type dsm_wm_iterator_cb,
  wm_iterator_modifier_cb_type dsm_wm_iterator_modifier_cb,
  void* cb_usr_data,
  const char * file,
  uint32 line
)
{
  dsm_item_type *pkt_ptr,*prev_ptr;      /* pointer to item to retrieve */
  dsm_item_type * pkt_dequeue_ptr = NULL; /* chain of pkt_dequeue_ptr */
  q_link_type *reverse_chain = NULL;

  uint32 initial_count;          /* initial count of bytes in WM           */
  uint32 initial_priority;       /* initial priority of pkt_ptr */
  q_link_type* initial_link;      /* Link on the packet pointer */
  uint32 item_length;            /* length of new item to add              */


  dsm_wm_iterator_req_enum_type iter_req = DSM_WM_REQUEST_NONE;
  dsm_wm_iterator_enum_type iterator = DSM_WM_ITERATOR_CONTINUE;

  DSMI_CHECK_WM_PTR(wm_ptr);
  ASSERT(dsm_wm_iterator_cb != 0);

  #ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
    ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
  #endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */
  if(wm_ptr->q_ptr->cnt != 0)
  {
    DSMI_QUEUE_LOCK_WM(wm_ptr);
    if (start_iter == DSM_WM_ITERATOR_HEAD)
    {
      pkt_ptr = (dsm_item_type*)q_check_s_ll_i( wm_ptr->q_ptr );
      prev_ptr = (dsm_item_type*)&wm_ptr->q_ptr->link;
    }
    else
    {
      pkt_ptr = (dsm_item_type*)q_next_s_ll_i(wm_ptr->q_ptr,wm_ptr->q_prio_insert_ptr);
      prev_ptr = (dsm_item_type*)wm_ptr->q_prio_insert_ptr;
    }
    while ((DSM_WM_ITERATOR_CONTINUE == iterator) && (pkt_ptr != NULL))
    {
      iterator = dsm_wm_iterator_cb(wm_ptr,pkt_ptr, &iter_req,cb_usr_data);
      ASSERT(iter_req <= DSM_WM_REQUEST_MAX);
      if(iter_req == DSM_WM_REQUEST_NONE)
      {
          prev_ptr = pkt_ptr;
          pkt_ptr = (dsm_item_type*)q_next_s_ll_i(wm_ptr->q_ptr,&(pkt_ptr->link));
      }
      else /* modify or dequeue packet */
      {
         /*-----------------------------------------------------------------------
                  Modify or Dequeue packet
                  First get WM parameters for sanity checks
                  -----------------------------------------------------------------------*/
         initial_count = wm_ptr->current_cnt;
         if (pkt_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
         {
           item_length = pkt_ptr->dsm_pkt_len; 
         }
         else
         {
           /* dsm_pkt_len not set by DSM, do not believe the field  - CR 898989*/
           item_length = dsm_length_packet(pkt_ptr);
         }

         if (wm_ptr->current_cnt < item_length)
         {
            ERR_FATAL("Current count for WM %x is less that length for item 0x%x"
         , (int)wm_ptr, (int)pkt_ptr, 0);
         }
         if (pkt_ptr->references == 0)
         {
            ERR_FATAL("References are 0 for enqueued packet 0x%x accessed within WM 0x%x",
            (int)pkt_ptr, (int)wm_ptr, 0);
         }

         if(iter_req == DSM_WM_MODIFY_AND_DEQUEUE_PKT || iter_req == DSM_WM_MODIFY_PKT)
         {
            /* Modify enqueued packet */
            if (dsm_wm_iterator_modifier_cb)
            {
                initial_link = pkt_ptr->link.next_ptr;
                initial_priority = pkt_ptr->priority;
                dsm_wm_iterator_modifier_cb(wm_ptr,pkt_ptr,cb_usr_data );
                #ifdef FEATURE_DSM_MEM_CHK
                 dsmi_touch_item_int(pkt_ptr,file,line);
                #endif /* FEATURE_DSM_MEM_CHK */

                /* Do sanity checks that used count and packet length has not been changed */
                if ((item_length != dsm_length_packet(pkt_ptr)) ||
                    (initial_link != pkt_ptr->link.next_ptr) ||
                    (initial_priority != pkt_ptr->priority) ||
                    (pkt_ptr->references == 0))
                {
                    ERR_FATAL("WM parameters changed in enqueued Packet 0x%x in WM 0x%x"
                       , (int)pkt_ptr, (int)wm_ptr, 0);
                }
            }
            else
            {
              ERR_FATAL("DSM WM Iterator Modify request with empty client call back function in WM 0x%x"
                                       , (int)wm_ptr, 0, 0);
            }
         }

         if ( iter_req == DSM_WM_MODIFY_AND_DEQUEUE_PKT || iter_req == DSM_WM_DEQUEUE_PKT)
         {
          /* Dequeue pkt_ptr */
          if (prev_ptr == (dsm_item_type*)&wm_ptr->q_ptr->link)
          {
            /* dequeue item from head, need to use q_get*/
            q_get_s_ll_i(wm_ptr->q_ptr);
          }
          else
          {
            q_retrieve_after_s_ll(wm_ptr->q_ptr,&(prev_ptr->link));
          }
          #ifdef FEATURE_DSM_MEM_CHK
            dsmi_touch_item_int(pkt_ptr,file,line);
          #endif /* FEATURE_DSM_MEM_CHK */

          if (pkt_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
          {
            if (pkt_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit == DSM_PKT_LEN_SET_BY_LIBARY)
            {
              pkt_ptr->dsm_pkt_len = 0;
            }
            pkt_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_CLIENT;
          }       
          if (wm_ptr->q_prio_insert_ptr == &pkt_ptr->link)
          {
            /*Point to the prior item */
            wm_ptr->q_prio_insert_ptr = &(prev_ptr->link);
          }
          wm_ptr->current_cnt -= item_length;

          dsmi_wm_check_levels_dequeue(wm_ptr, initial_count);
          DSM_WM_CB(wm_ptr->each_dequeue_func_ptr,
              wm_ptr,wm_ptr->each_dequeue_func_data);
          DSM_WM_CB(wm_ptr->log_deq_func_ptr,wm_ptr,pkt_ptr);

          /* Chain the packets using its own link */
          if (!reverse_chain)
          {
            reverse_chain = &pkt_ptr->link;
            pkt_dequeue_ptr = pkt_ptr;
          }
          else
          {
              reverse_chain->next_ptr = &pkt_ptr->link;
          }
          reverse_chain = &pkt_ptr->link;

          /* Do not change prev_ptr */
          pkt_ptr = (dsm_item_type*)q_next_s_ll_i(wm_ptr->q_ptr,&(prev_ptr->link));
        }
        else
        {
          prev_ptr = pkt_ptr;
          pkt_ptr = (dsm_item_type*)q_next_s_ll_i(wm_ptr->q_ptr,&(pkt_ptr->link));
        }

        iter_req = DSM_WM_REQUEST_NONE;
      }
    }
    DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  }
  return pkt_dequeue_ptr;
}


/*===========================================================================
FUNCTION DSM_IS_WM_EMPTY()

DESCRIPTION
 This routine determines whether the input watermark has data queued in
 it or not.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark

RETURN VALUE
 TRUE if watermark has no data queued in it, FALSE if it does

SIDE EFFECTS
 None
===========================================================================*/
boolean dsm_is_wm_empty
(
 dsm_watermark_type *wm_ptr
)
{
 DSMI_CHECK_WM_PTR(wm_ptr);
 return (boolean) (q_cnt_s (wm_ptr->q_ptr) == 0);
} /* dsm_is_wm_empty */

/*===========================================================================
FUNCTION DSM_SET_WM_CB()

DESCRIPTION
 This routine sets/resets WM callbacks and their appropriate values after locking Watermark

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 dsm_wm_cb_type - Type of callback
 change_cb - True, to change callback function
 cb - Callback function
 change_cb_data - True, to change callback function data
 cb_data - Callback data, not applicable for DNE callbacks
 change_val - True, to change values that determine callback calls
 val - Values to change
 
RETURN VALUE
 VOID

SIDE EFFECTS
  WB callback triggered. Function locks the WM context for the extent of the
 callback.
===========================================================================*/
void dsm_set_wm_cb
(
  dsm_watermark_type *wm_ptr,
  dsm_wm_callback_type_enum dsm_wm_cb_type,
  boolean            change_cb,
  wm_cb_type         cb,
  boolean            change_cb_data,
  void *             cb_data,
  boolean            change_val,
  uint32             val  
)
{
  
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  switch(dsm_wm_cb_type)
  {
    case DSM_WM_LO:
     if (change_cb)
     {
       wm_ptr->lowater_func_ptr = cb;
     }
     if (change_cb_data)
     {
       wm_ptr->lowater_func_data = cb_data;
     }
     if (change_val)
     {
         if(wm_ptr->current_cnt >= wm_ptr->lo_watermark &&
            wm_ptr->current_cnt < val )
         {
           DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,wm_ptr->lowater_func_data);
         }
         wm_ptr->lo_watermark = val;
     }
     break;

    case DSM_WM_HI:
      if (change_cb)
      {
        wm_ptr->hiwater_func_ptr = cb;
      }
      if (change_cb_data)
      {
        wm_ptr->hiwater_func_data = cb_data;
      }
      if (change_val)
      {
          if(wm_ptr->current_cnt <= wm_ptr->hi_watermark &&
             wm_ptr->current_cnt > val )
          {
            DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
          }
          wm_ptr->hi_watermark = val;
      }
      break;

    case DSM_WM_EMPTY:
      if (change_cb)
      {
        wm_ptr->gone_empty_func_ptr= cb;
      }
      if (change_cb_data)
      {
        wm_ptr->gone_empty_func_data= cb_data;
      }
      /* change_val does not apply here */
      break;

    case DSM_WM_NON_EMPTY:
      if (change_cb)
      {
        wm_ptr->non_empty_func_ptr= cb;
      }
      if (change_cb_data)
      {
        wm_ptr->non_empty_func_data= cb_data;
      }   
      if(wm_ptr->current_cnt > 0)
      {
        DSM_WM_CB(wm_ptr->non_empty_func_ptr,wm_ptr,wm_ptr->non_empty_func_data);
      }
      /* change_val does not apply here */
      break;

    case DSM_WM_DNE_QCNT_PKTS:      
      if (change_cb)
      {
        wm_ptr->dne_func_ptr= cb;
      }
      /* change_cb_data  does not apply here */
      /* Callback data is the dsm item about to be deleted when dne function is called */
      if (change_val)
      {
        wm_ptr->dne_q_cnt = val;           
        wm_ptr->dne_q_cnt_discard_pkt_cnt = 0;
        wm_ptr->dne_q_cnt_discard_bytes_cnt = 0;        
      }
      break;
      
    case DSM_WM_DNE_BYTES:
      if (change_cb)
      {
        wm_ptr->dne_func_ptr = cb;
      }
      /* change_cb_data  does not apply here */
      /* Callback data is the dsm item about to be deleted when dne function is called */
      if (change_val)
      {
         wm_ptr->dont_exceed_cnt = val;
         wm_ptr->dne_discard_pkt_cnt = 0;
         wm_ptr->dne_discard_bytes_cnt = 0;
      }
      break;
    default: /* do nothing */;
  }
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);  
}

/*===========================================================================
FUNCTION DSM_QUEUE_CNT()

DESCRIPTION
 Returns the number of bytes on the watermark queue.
 
DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark

RETURN VALUE
 Number of bytes recorded on queue.

SIDE EFFECTS
 None.
===========================================================================*/
uint32 dsm_queue_cnt
(
  dsm_watermark_type *wm_ptr
)
{
  uint32 ret;
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  ret = wm_ptr->current_cnt;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  return ret;
} 
/*===========================================================================
FUNCTION DSM_QUEUE_DESTROY()

DESCRIPTION
   This function tears down a watermark queue.
DEPENDENCIES
   None

PARAMETERS
   wm_ptr - Pointer to the watermark to initialize

RETURN VALUE
   None

SIDE EFFECTS
   Locks might be destroyed.
   Packets might be freed.
   WM pointer will be non-initialized.
===========================================================================*/
void dsmi_queue_destroy
(
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type * item_ptr;
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  while ( wm_ptr->q_ptr != NULL &&
          ( item_ptr = (dsm_item_type *)q_get_s_ll_i(wm_ptr->q_ptr)) != NULL ) 
  {
    (void)dsmi_free_packet(&item_ptr,file,line);
  }
#ifdef FEATURE_DSM_Q_DESTROY
  q_destroy_s(wm_ptr->q_ptr,TRUE);
#endif
  dsm_wm_trace_deregister(wm_ptr);

  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  
  if (wm_ptr->lock)
  {
    DSMI_QUEUE_LOCK_DESTROY(wm_ptr);
    memset(wm_ptr, 0, sizeof(dsm_watermark_type));
  }
}

/*===========================================================================
FUNCTION DSM_QUEUE_SET_DEBUG_FLAG_FOR_ALL_WM()

DESCRIPTION
   This function used to set global debug flag for DSM WM,
       where all watermarks initialized after this flag is set will have 
       enabled/disabled DEBUG Masks set.
DEPENDENCIES
   None

PARAMETERS
   debug flag - Enable/Disable global flag

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
void dsm_queue_set_debug_flag_for_all_wm(boolean debug_flag)
{
  dsm_queue_global_debug_flag = debug_flag;
}

