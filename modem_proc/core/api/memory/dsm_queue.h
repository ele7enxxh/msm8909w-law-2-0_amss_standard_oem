#ifndef DSM_QUEUE_H
#define DSM_QUEUE_H
/*===========================================================================

                                  D S M _ Q U E U E . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/memory/dsm_queue.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
07/31/15    sh     CR 871980 - Added Shailesh's provided changes - WM level checks are changed in 
                          dsm_enqueue and dsm_dequeue and it affects existing functionality of checking both levels
                          during enqueue/dequeue
05/31/15    sh     Added multiple enqueue/dequeue functionality
09/05/12    sh     New dsm_priority value added for dsm_enqueue
06/06/12    sh     Added dne_statistics
06/06/12    sh     Added dne_callback
06/06/12    sh     Added priority insert optimization 
04/06/11    rj     Internalize private fields within watermark structure   
03/08/11    ag     Remove FEATURE_DSM_WM_CB
07/24/09    ag     Updation of hi,lo,dne,empty event counts for WM tracing.
06/12/09    ag     Updation of min max counts for watermark tracing.  
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue_services.h"
#include "dsm_item.h"


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Watermark Type definition. This structure provides the means for 2 software
  entities to convey data and control its flow. Note that the function
  pointer fields must 1st be checked to non-NULL prior to invocation.
---------------------------------------------------------------------------*/

struct dsm_watermark_type_s; 
typedef void(*wm_cb_type)(struct dsm_watermark_type_s *, void *);

/* Debug setting for each Watermark*/
typedef enum dsm_wm_debug_flag_type
{
   DSM_WM_DEBUG_SET_TRACE_BIT0,
   /* set filestamp for enqueued/dequeued packets */
   DSM_WM_DEBUG_VERIFY_ENQ_PKT_LEN_BIT1,
   /* check whether packet length, packet chain is correct for each enqueued/dequeued packet */
   DSM_WM_DEBUG_VERIFY_MULTIPLE_ENQ_PKT_CHAIN_BIT2,
   /* check whether num_pkts_in_chain, total_pkt_len_of_chain,
    *  reference count,priority field and pool_id of enqueued packets */
   DSM_WM_DEBUG_LOG_WM_ENQUEUE_DEQUEUE_BIT3
   /*log enq and deq of each packet */
} dsm_wm_debug_flag_type;
 
typedef struct dsm_watermark_type_s
{
  q_type *q_ptr;		 /* Pointer to associated queue */

  q_link_type *q_prio_insert_ptr; /* Position in Q to insert DSM_HIGHEST prio item*/
                                  /* Pointer to link item to insert AFTER */
  uint32  current_cnt;		/* Current total byte count */

  uint32  dont_exceed_cnt;	/* Discard count for RX bytes */
  uint32  dne_q_cnt;        /* Discard count for WM Packet count (q_cnt)*/
  

  uint32  lo_watermark;		/* Low watermark for total bytes */
  uint32  hi_watermark;		/* Hi watermark for total bytes */
  uint32  highest_cnt;		/* largest count since powerupp */
  uint64  total_rcvd_cnt;   /* Total RX bytes thru watermark */

  dword lock;               /* Lock for the queue */

  wm_cb_type each_enqueue_func_ptr; /* Called when any item enqueued  */
  void * each_enqueue_func_data;

  wm_cb_type each_dequeue_func_ptr; /* Called when any item dequeued  */
  void * each_dequeue_func_data;

  wm_cb_type lowater_func_ptr;	/* Lo Watermark function pointer, Invoke
                                   function when queue hits the Lo watermrk*/
  void * lowater_func_data;

  wm_cb_type gone_empty_func_ptr; /* Gone empty function pointer, invoke when
                                     queue goes empty */
  void * gone_empty_func_data;

  wm_cb_type non_empty_func_ptr; /* Non-empty function pointer, Invoke when
                                    queue goes nonempty */
  void * non_empty_func_data;

  wm_cb_type hiwater_func_ptr;   /* Hi Watermark function pointer, Invoke
                                    function when queue hits the Hi wm     */
  void * hiwater_func_data;

  uint32 sentinel;       /* cookie to verify initialization */

  /* Statistics */
  uint32  dne_discard_pkt_cnt; /* Total pkts discarded when dne cnt is hit */
  uint32  dne_q_cnt_discard_pkt_cnt; /* Total pkts discarded when dne_q_cnt is hit */
  uint32  dne_discard_bytes_cnt;/* Total byte cnt in dne_discard_pkt(s) */
  uint32  dne_q_cnt_discard_bytes_cnt;/* Total byte cnt when dne_q_cnt is hit	*/
  
  wm_cb_type dne_func_ptr;   /* Do Not Exceed Wm function pointer, Invoke
                                    function, when DNE count is reached
                                    in enqueue(), before dsm pkt is deleted.
                                    parameters - wm ptr,enqueued dsm pkt */

  uint32 dne_log_frequency; /* Log message, "WM Full" will be sent once
                                                   for every 'dne_log_frequency'-th dropped packet
                                                   Default value - 0, means log for every 16th packet */
  
  uint32 wm_debug_mask; /* WM debug mask 
                         Should be 0 for production version, any other 
                         value affects performance with additional checks*/

  /* START  Fields used only for debugging/profiling watermarks */
   /* Important : Do NOT USE  log_enq_func_ptr,log_deq_func_ptr
   fields for regular WM operation.
   May be overwritten as and when needed. */
  wm_cb_type log_enq_func_ptr; /* each enqueue log callback*/
  wm_cb_type log_deq_func_ptr; /* each dequeue log callback */

  uint32 reserved [3];  /* Initialized with file, line, WM ID.*/
  /* END-Fields used only for debugging/profiling watermarks*/

} dsm_watermark_type;


/*---------------------------------------------------------------------------
  Priority Field Values
  ------------------------------------------------
  Priority values determine how items are enqueued in WM Queue 
  Items are dequeued from head of WM queue, irrespective of priority value.
---------------------------------------------------------------------------*/

  #define DSM_NORMAL          0
  /* DSM_NORMAL priority item is enqueued to tail of WM Queue - 
        follows FIFO rule for dequeueing these items */   
  
  #define DSM_HIGHEST         0x1
  /* DSM_HIGHEST priority item is enqueued to head of WM Queue, 
        before DSM_NORMAL priority items ,
        after DSM_PRIORITY_ENQ_HIGHEST items and 
        after previously inserted DSM_HIGHEST priority items.
        - follows FIFO rule for dequeueing DSM_HIGHEST priority items*/

  #define DSM_PRIORITY_ENQ_HIGHEST  0x2
  /* DSM_PRIORITY_ENQ_HIGHEST priority items enqueued
        to head of WM queue, irrespective of other items in queue. 
        - follows LIFO rule for dequeuing these items */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  typedef enum
  {
    DSM_WM_LO = 0,
    DSM_WM_HI,
    DSM_WM_EMPTY,
    DSM_WM_NON_EMPTY,
    DSM_WM_DNE_QCNT_PKTS,
    DSM_WM_DNE_BYTES
  } dsm_wm_callback_type_enum;

  typedef enum
  {
    DSM_WM_ENQ_RULE_IGNORE_DNE = 0,
    DSM_WM_ENQ_RULE_MAX = 1
  } dsm_wm_enq_rule_enum;

  typedef enum
  {
    DSM_WM_REQUEST_NONE = 0,
    DSM_WM_DEQUEUE_PKT = 1,
    DSM_WM_MODIFY_PKT = 2,
    DSM_WM_MODIFY_AND_DEQUEUE_PKT = 3,
    DSM_WM_REQUEST_MAX = 3
  } dsm_wm_iterator_req_enum_type;

  typedef enum
  {
    DSM_WM_ITERATOR_STOP = 0,
    DSM_WM_ITERATOR_CONTINUE = 1
  }dsm_wm_iterator_enum_type;

  typedef enum
  {
    DSM_WM_ITERATOR_HEAD = 0,
    DSM_WM_ITERATOR_NORMAL_PRIORITY_HEAD = 1
  } dsm_start_iterator_enum_type;


  typedef dsm_wm_iterator_enum_type (*wm_iterator_cb_type)
          ( const dsm_watermark_type *wm_ptr,
            const dsm_item_type *pkt_head_ptr,
            dsm_wm_iterator_req_enum_type *req_type,
            void* cb_usr_data);

  typedef void (*wm_iterator_modifier_cb_type)
          ( const dsm_watermark_type *wm_ptr,
            dsm_item_type *pkt_head_ptr,
            void* cb_usr_data);


#define DSM_GET_WM_CURRENT_CNT(wm_ptr) \
  (((wm_ptr)->current_cnt)) 



/* if using dsm_wm logging */
#define DSMI_GET_WM_TRACE_FILE(wm_ptr) \
  ((uint32)((wm_ptr)->reserved[0])) 

#define DSMI_GET_WM_TRACE_LINE(wm_ptr) \
    ((uint32)((wm_ptr)->reserved[1])) 

#define DSMI_GET_WM_TRACE_ID(wm_ptr) \
    ((uint32)((wm_ptr)->reserved[2])) 

/*===========================================================================
INLINE FUNCTION  DSM_WM_SET_DEBUG()/DSM_WM_RESET_DEBUG_FLAG

DESCRIPTION
 Sets/Resets DSM WM debug flags.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to Watermark
 debug_flag - dsm_wm_debug_bit_field_type
 
RETURN VALUE
 None 
SIDE EFFECTS
 No error checking in inline functions. 
===========================================================================*/
static __inline void dsm_wm_set_debug_flag(
  dsm_watermark_type *wm_ptr,
  dsm_wm_debug_flag_type debug_flag
)
{
    wm_ptr->wm_debug_mask |= (1<<debug_flag);
}

static __inline void dsm_wm_reset_debug_flag(
  dsm_watermark_type *wm_ptr,
  dsm_wm_debug_flag_type debug_flag
)
{
    wm_ptr->wm_debug_mask &= ~(1<<debug_flag);
}

static __inline boolean dsm_wm_get_debug_flag(
  dsm_watermark_type *wm_ptr,
  dsm_wm_debug_flag_type debug_flag
)
{
    return (((wm_ptr->wm_debug_mask)>>debug_flag)& 1);
}


/*===========================================================================
INLINE FUNCTION  DSM_PKT_CHAIN_LINK_WITH_LEN() - (DSM_PKT_LEN_API)
                            DSM_PKT_CHAIN_LINK_UPDATE_PKT_LEN()

DESCRIPTION
 These routines link "chain_link" chain to "second" dsm_packet. 
 These functions can be used *only* for chaining packets before calling 
 dsm_multiple_enqueue() function.
 
 Check usage in dsm_multiple_enqueue() function
 In DSM_PKT_CHAIN_LINK_WITH_LEN API(),
 DSM Library expects that the value passed by client is correct/valid,
 being part of DSM_PKT_LEN_API category.

 In DSM_PKT_CHAIN_LINK_UPDATE_PKT_LEN, DSM library updates packet length
 of each packet in "dsm_pkt_len" field of header, calling 
 dsm_length_packet() API to go through the packet chains and calculate 
 the current packet length

API dsm_pkt_chain_link() will be made obsolete soon. Do not use this API.

DEPENDENCIES
 None

PARAMETERS
 dsm_item_type** chain_link - tail of a chain to which second is attached.
 dsm_item_type* second
 second_dsm_pkt_len - Packet length of "second" dsm_packet to be added to chain
     Client to pass correct packet length for "second" packet.
     If not known, clients can call DSM_PKT_CHAIN_LINK_UPDATE_PKT_LEN() API
     DSM Library expects that the value passed by client is correct/valid in the API
     There is no error checking for the field.

RETURN VALUE *chain_link points to "second"

SIDE EFFECTS
 No error checking in inline functions.
 chain_link or *chain_link or second cannot be NULL.

 DSM Library make no assumptions that the value passed by client is correct/valid.
 There is no error chcking for the field.
===========================================================================*/
static __inline void dsm_pkt_chain_link_with_len(
  dsm_item_type **chain_link,
  dsm_item_type *second,
  uint16 second_dsm_pkt_len
)
{
  /*  chain_link or *chain_link cannot be NULL */
  /* second cannot be NULL */
  q_chain_link_s_i((q_link_type*)*chain_link,(q_link_type*)second);
  dsm_item_set_pkt_len(second,second_dsm_pkt_len);
  second->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_PKT_LEN_APIS;
  *chain_link = second;
}

static __inline void dsm_pkt_chain_link_update_pkt_len(
  dsm_item_type **chain_link,
  dsm_item_type *second
)
{
  /*  chain_link or *chain_link cannot be NULL */
  /* second cannot be NULL */
  q_chain_link_s_i((q_link_type*)*chain_link,(q_link_type*)second);
  dsm_item_update_pkt_len(second);
  second->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_PKT_LEN_APIS;
  *chain_link = second;
}


/* Function below will be made obsolete soon. Please use above function*/
static __inline void dsm_pkt_chain_link(
  dsm_item_type **chain_link,
  dsm_item_type *second
)
{
  /*  chain_link or *chain_link cannot be NULL */
  q_chain_link_s_i((q_link_type*)*chain_link,(q_link_type*)second);
  *chain_link = second;
}

/*===========================================================================
INLINE FUNCTION  DSM_PKT_CHAIN_DELINK()/DSM_PKT_CHAIN_DELINK_RESET_PKT_LEN()

DESCRIPTION
 The routine , DSM_PKT_CHAIN_DELINK () delinks each item from pack chain returned by
 dsm_multiple_dequeue() function.  

 The routine, DSM_PKT_CHAIN_DELINK_RESET_PKT_LEN(), each item from pack chain returned by
 dsm_multiple_dequeue() function. Also it will reset dsm_pkt_len field in of the delinked DSM Packet Header

 These functions can be used *only* for delinkng packets
 after calling  dsm_multiple_dequeue() function. Check usage in  dsm_multiple_dequeue() function

DEPENDENCIES
 None

PARAMETERS
 pkt_chain -Packet chain

RETURN VALUE - Delinked packet from top of chain
               Modified Packet chain with delinked packet removed
SIDE EFFECTS
 No error checking in inline functions
  pkt_chain or *pkt_chain cannot be NULL.
===========================================================================*/
#define dsm_pkt_chain_delink(pkt_chain) \
  dsmi_pkt_chain_delink(pkt_chain, dsm_file, __LINE__)

static __inline dsm_item_type* dsmi_pkt_chain_delink(
  dsm_item_type **pkt_chain,
  const char * filename,
  int line
)
{
  dsm_item_type* head_dsm_ptr =  (dsm_item_type*)q_chain_delink_s_i((q_link_type**)pkt_chain);
  if (head_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit)
  {
    if (head_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit == DSM_PKT_LEN_SET_BY_LIBARY)
	{
      dsm_item_reset_pkt_len(head_dsm_ptr);
	}
	head_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_CLIENT;
  }	
  dsmi_touch_item(head_dsm_ptr,filename,line);
  return head_dsm_ptr;
}

#define dsm_pkt_chain_delink_reset_pkt_len(pkt_chain) \
  dsmi_pkt_chain_delink_reset_pkt_len(pkt_chain, dsm_file,__LINE__)
static __inline dsm_item_type* dsmi_pkt_chain_delink_reset_pkt_len(
  dsm_item_type **pkt_chain,
  const char * filename,
  int line
)
{
  dsm_item_type* head_dsm_ptr =  (dsm_item_type*)q_chain_delink_s_i((q_link_type**)pkt_chain);
  head_dsm_ptr->pkt_bit_field.dsm_bit_field.dsm_q_use_pkt_len_bit = DSM_PKT_LEN_SET_BY_CLIENT;
  dsm_item_reset_pkt_len(head_dsm_ptr);
  dsmi_touch_item(head_dsm_ptr,filename,line);
  return head_dsm_ptr;
}

/*===========================================================================
INLINE FUNCTION  DSM_PKT_CHAIN_NEXT()

DESCRIPTION
 This routine iterates through a chain of DSM items 
 and returns the top item, without delinking

DEPENDENCIES
 None

PARAMETERS
 dsm_item_type** chain_link - tail of a chain to which second is attached.
 dsm_item_type* second

RETURN VALUE *chain_link points to "second"

SIDE EFFECTS
 No error checking in inline functions.
  chain_link or *chain_link cannot be NULL.
===========================================================================*/
static __inline void dsm_pkt_chain_next (
  dsm_item_type **chain_link)
{
  *chain_link = (dsm_item_type*)q_chain_next_s_i((q_link_type*)(*chain_link));
}


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/


/*================ QUEUE MANAGMENT FUNCTIONS ==============================*/
/*===========================================================================
FUNCTION DSM_QUEUE_INIT()

DESCRIPTION
   This function initalizes a watermark queue.  Setting all the callbacks and 
   callback data to NULL, watermark levels to 0, and initializing the queue 
   that this will use.  Since this potentially initializes a mutex, the
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
===========================================================================*/

#define dsm_queue_init(wm_ptr,dne,queue) \
  dsmi_queue_init(wm_ptr,dne,queue,dsm_file,__LINE__)

extern void dsmi_queue_init
(
  dsm_watermark_type *wm_ptr,
  int dne,
  q_type * queue,
  const char* file,
  uint16 line
);


  
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
  wm_ptr - Pinter to Watermark item to put to
  pkt_head_ptr - Pointer to pointer to item to add to queue

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_enqueue(wm_ptr,pkt_head_ptr) \
  dsmi_enqueue(wm_ptr,pkt_head_ptr,dsm_file,__LINE__)

extern void dsmi_enqueue
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
);


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
#define dsm_simple_enqueue_isr(wm_ptr,pkt_head_ptr) \
  dsmi_simple_enqueue_isr(wm_ptr,pkt_head_ptr,dsm_file,__LINE__)

extern void dsmi_simple_enqueue_isr
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
);

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
       dsm_pkt_chain_link_update_pkt_len(&chain_tail_ptr,enq_pkt[i]);
       total_pkt_len+=dsm_item_get_pkt_len(enq_pkt[i]);
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
  Being a DSM_PKT_LEN_API, DSM Header "dsm_pkt_len" field input parameter - length of each packet in chain is expected to be
		  in dsm_pkt_len field of header (no error checking for length of each packet)

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
#define dsm_multiple_enqueue(wm_ptr,chain_head_ptr, chain_tail_ptr,num_pkts_in_chain,total_pkt_len_of_chain) \
  dsmi_multiple_enqueue(wm_ptr,chain_head_ptr,chain_tail_ptr,num_pkts_in_chain,total_pkt_len_of_chain,dsm_file,__LINE__)
extern void dsmi_multiple_enqueue
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **chain_head_ptr, 
  dsm_item_type **chain_tail_ptr, 
  uint16 num_pkts_in_chain, 
  uint32 total_pkt_len_of_chain, 
  const char * file,
  uint32 line
);


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
#define dsm_enqueue_special(wm_ptr,pkt_head_ptr,dsm_enqueue_rule) \
  dsmi_enqueue_special(wm_ptr,pkt_head_ptr, dsm_enqueue_rule, dsm_file,__LINE__)

extern void dsmi_enqueue_special
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  dsm_wm_enq_rule_enum dsm_enqueue_rule,
  const char * file,
  uint32 line
);


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
#define dsm_dequeue(wm_ptr) \
  dsmi_dequeue(wm_ptr,dsm_file,__LINE__)

extern dsm_item_type *dsmi_dequeue
(
  dsm_watermark_type *wm_ptr,
  const char * file, 
  uint32 line
);


/*===========================================================================
FUNCTION DSM_MULTIPLE_DEQUEUE()

DESCRIPTION
The function is used to dequeue multiple packets from WM , to be used for performance 
  critical paths.  
  Number of packets dequeued is MIN(num_pkts_to_dequeue, number of packets in WM)
  Need to call dsm_pkt_chain_delink() in a loop to retrieve the returned packets

  Usage below.
  dsm_item_type *chain_head_ptr = NULL;
  dsm_item_type *delinked_item;
  uint32 deq_pkt_len;
  dsm_multiple_dequeue(wm,5,&chain_head_ptr,&deq_pkt_len);
  while (chain_head_ptr !=NULL)
  {
     delinked_item = dsm_pkt_chain_delink(&chain_head_ptr);
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
#define dsm_multiple_dequeue(wm_ptr,num_pkts_to_dequeue,chain_head_ptr,dequeued_chain_length) \
  dsmi_multiple_dequeue(wm_ptr,num_pkts_to_dequeue,chain_head_ptr,dequeued_chain_length,dsm_file,__LINE__)
uint32 dsmi_multiple_dequeue
(
  dsm_watermark_type *wm_ptr,
  uint32 num_pkts_to_dequeue,  
  dsm_item_type** chain_head_ptr,
  uint32* dequeued_chain_length,
  const char * file,
  uint32 line
);

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
#define dsm_empty_queue(wm_ptr) \
  dsmi_empty_queue(wm_ptr,dsm_file,__LINE__)

extern void dsmi_empty_queue
( 
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
);

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
);

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
);


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
#define dsm_dequeue_normal_prio_head(wm_ptr) \
		dsmi_dequeue_normal_prio_head(wm_ptr, \
         dsm_file,\
         __LINE__)
extern  dsm_item_type *dsmi_dequeue_normal_prio_head
( dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
);


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
#define dsm_wm_iterator_with_dequeue_modify_pkts(wm_ptr,start_iter,dsm_wm_iterator_cb,\
                         dsm_wm_iterator_modifier_cb, cb_usr_data) \
        dsmi_wm_iterator_with_dequeue_modify_pkts(wm_ptr, \
         start_iter, dsm_wm_iterator_cb, dsm_wm_iterator_modifier_cb, cb_usr_data,\
         dsm_file, __LINE__)

extern  dsm_item_type *dsmi_wm_iterator_with_dequeue_modify_pkts
( dsm_watermark_type *wm_ptr,
  dsm_start_iterator_enum_type start_iter,
  wm_iterator_cb_type dsm_wm_iterator_cb,
  wm_iterator_modifier_cb_type dsm_wm_iterator_modifier_cb,
  void* cb_usr_data,
  const char * file,
  uint32 line
);



/*===========================================================================
FUNCTION DSM_SET_LOW_WM()

DESCRIPTION
 This routine resets the low watermark value. This change may trigger
 watermark callbacks.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for low watermark.

RETURN VALUE
 TRUE if watermark has no data queued in it, FALSE if it does

SIDE EFFECTS
 None
===========================================================================*/

#define dsm_set_low_wm(wm_ptr,val) \
  dsm_set_wm_cb(wm_ptr,DSM_WM_LO,FALSE,0,FALSE,0,TRUE,val)  


/*===========================================================================
FUNCTION DSM_SET_HI_WM()

DESCRIPTION
 This routine resets the high watermark value. This change may trigger
 watermark callbacks.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for hi watermark.

RETURN VALUE
 None.

SIDE EFFECTS
 WB callback triggered. Function locks the context for the extent of the
 callback.
===========================================================================*/
#define dsm_set_hi_wm(wm_ptr,val) \
  dsm_set_wm_cb(wm_ptr,DSM_WM_HI,FALSE,0,FALSE,0,TRUE,val)  


/*===========================================================================
FUNCTION DSM_SET_DNE()

DESCRIPTION
 This routine resets the DNE (do not exceed) value.
 

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for hi watermark.

RETURN VALUE
 None.

SIDE EFFECTS
 WB callback triggered. Function locks the context for the extent of the
 callback.
===========================================================================*/
#define dsm_set_dne(wm_ptr,val) \
  dsm_set_wm_cb(wm_ptr,DSM_WM_DNE_BYTES,FALSE,0,FALSE,0,TRUE,val)  
  
/*===========================================================================
FUNCTION DSM_SET_DNE_Q_CNT()

DESCRIPTION
 This routine resets the dne_q_cnt (do not exceed) Queue Count value.
 

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for dne_q_cnt.

RETURN VALUE
 None.

SIDE EFFECTS
 WB callback triggered. Function locks the context for the extent of the
 callback.
===========================================================================*/

#define dsm_set_dne_q_cnt(wm_ptr,val) \
  dsm_set_wm_cb(wm_ptr,DSM_WM_DNE_QCNT_PKTS,FALSE,0,FALSE,0,TRUE,val)  

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
 change_cb_data - True to change callback function data
 cb_data - Callback data, not applicable for DNE callbacks
 change_val - True to change values that determine callback calls
 val - Values to change
 
RETURN VALUE
 VOID

SIDE EFFECTS
  WB callback triggered. Function locks the context for the extent of the
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
);


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
 Locks watermark mutex lock before reading the queue cnt. Use 
 DSM_GET_WM_CURRENT_CNT Macro for most purposes.
===========================================================================*/
uint32 dsm_queue_cnt
(
  dsm_watermark_type *wm_ptr
);

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
void dsm_queue_set_debug_flag_for_all_wm(boolean debug_flag);

/*===========================================================================
FUNCTION DSM_QUEUE_DESTROY()

DESCRIPTION
   This function tears down a watermark queue.
DEPENDENCIES
   None

PARAMETERS
   wm_ptr - Pointer to the watermark to tear down.

RETURN VALUE
   None

SIDE EFFECTS
   Locks might be destroyed.
   Packets might be freed.
   WM pointer will be non-initialized.
===========================================================================*/
#define dsm_queue_destroy(wm_ptr) \
  dsmi_queue_destroy(wm_ptr,dsm_file,__LINE__)

void dsmi_queue_destroy
(
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
);


#endif /* DSM_QUEUE_H */
