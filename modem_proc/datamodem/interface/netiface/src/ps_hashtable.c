/*!
  @file
  ps_hashtable.c

  @brief
  PS Hash table data structure
*/
/*===========================================================================

  Copyright (c) 2013-14 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_hashtable.c#1 $
$DateTime: 2016/12/13 08:00:03 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/14   ash     Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include "ps_hashtable.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ps_mem.h"

/*===========================================================================

                          MACRO DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  PS memory size for Hashtable Control Blocks
---------------------------------------------------------------------------*/
#define PS_HASHTABLE_CB_SIZE  ((sizeof(ps_ht_cb_type) + 3) & ~3)

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
/*----------------------------------------------------------------------------
  Allocate memory to hold ps_hashtable along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_hashtable_cb_mem[PS_MEM_GET_TOT_SIZE_OPT
                                 (
                                   PS_HASHTABLE_CB_NUM,
                                   PS_HASHTABLE_CB_SIZE
                                 )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the second points to actual buf
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_hashtable_hdr[PS_HASHTABLE_CB_SIZE];
static ps_ht_cb_type * ps_hashtable_ptr[PS_HASHTABLE_CB_SIZE];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================
                    Linked list operations
===========================================================================*/
/*! Add a node to the tail of a doubly linked list */
#define ps_ht_add_node(tail_p, entry_p)                       \
{                                                             \
  (entry_p)->next = NULL;                                     \
  (entry_p)->prev = NULL;                                     \
                                                              \
  if (tail_p)                                                 \
  {                                                           \
    (tail_p)->next = (entry_p);                               \
  }                                                           \
                                                              \
  (entry_p)->prev = (tail_p);                                 \
  (tail_p) = (entry_p);                                       \
}              

/*! Remove a node from a doubly linked list */
#define ps_ht_rm_node(entry_p)                                \
{                                                             \
  if ((entry_p)->next)                                        \
  {                                                           \
    ((entry_p)->next)->prev =                                 \
     (entry_p)->prev;                                         \
  }                                                           \
  if ((entry_p)->prev)                                        \
  {                                                           \
    ((entry_p)->prev)->next =                                 \
     (entry_p)->next;                                         \
  }                                                           \
} 

/*==============================================================================

                    INTERNAL FUNCTIONS

==============================================================================*/

/*===========================================================================
INLINE FUNCTION HASHTBL_CB_IS_VALID()

DESCRIPTION
  This function checks the validity of Hashtable control block. Hashtable uses
  this MACRO for APIs where it expects the control block to be valid (allocated)
   & initialized.

PARAMETERS
  (IN) instance_ptr: Pointer to the HASHTABLE instance ptr

RETURN VALUE
  TRUE  if the passed instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define HT_CB_IS_VALID hti_cb_is_valid
INLINE boolean hti_cb_is_valid
(
  ps_ht_cb_type * cb_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cb_ptr == NULL ) ||
      ( ps_mem_is_valid( cb_ptr,
                         PS_MEM_HASHTABLE_CB_TYPE ) == FALSE ) )
  {
    return FALSE;
  }


  if( cb_ptr->this_cb_ptr != cb_ptr  )
  {
    return FALSE;
  }

  return TRUE;
} /* hti_cb_is_valid() */

/*===========================================================================
FUNCTION PS_HT_POWERUP_INIT()

DESCRIPTION
  This function initializes PS mem control blocks for the Hashtables.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ht_powerup_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize memory for all NCP control blocks
  -------------------------------------------------------------------------*/
  if( PS_MEM_POOL_INIT_OPT( PS_MEM_HASHTABLE_CB_TYPE,
                            ps_hashtable_cb_mem,
                            PS_HASHTABLE_CB_SIZE,
                            PS_HASHTABLE_CB_NUM,
                            PS_HASHTABLE_CB_HIGH_WM,
                            PS_HASHTABLE_CB_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) ps_hashtable_hdr,
                            (int *) ps_hashtable_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                            ) == -1)
  {
    DATA_ERR_FATAL("ps_ht_powerup_init(): Instance mem init error");
    return;
  }
} /* ps_ht_powerup_init() */

/*===========================================================================
FUNCTION PS_HT_CALC_HASH()

DESCRIPTION
  This function creates hash from key.

PARAMETERS
  ht_cb_ptr

RETURN VALUE
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 ps_ht_calc_hash
(
  ps_ht_cb_type *ht_cb_ptr,
  uint64         key
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Primitive Mod Hash function
  -------------------------------------------------------------------------*/
  return (key % ht_cb_ptr->table_size);

} /* ps_ht_calc_hash() */

/*===========================================================================
FUNCTION PS_HT_LOOKUP_NODE()

DESCRIPTION
  This function does hash lookup and return the node. 

PARAMETERS
  (IN)  ht_cb_ptr : Handle to Hashtable control block.
  (IN)  key       : Lookup key
  (OUT) node      : Node to return

RETURN VALUE 
  TRUE on success
  FALSE on failure
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_ht_return_type ps_ht_lookup_node
(
   ps_ht_bkt_type   *bucket,
   uint64            key,
   ps_ht_node_type **node
)
{
  ps_ht_node_type *lookup_node;
  boolean          node_found = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
    Traverse the buckets to get the right node.
  -------------------------------------------------------------------------*/
  lookup_node = bucket->head;
  while (lookup_node)
  {
    if (lookup_node->key == key)
    {
     /*--------------------------------
        Indicate that node is found
     --------------------------------*/
      node_found = TRUE;
      break;
    }
    lookup_node = lookup_node->next;
  }

  /*-------------------------------------------------------------------------
    Node not found return FALSE;
  -------------------------------------------------------------------------*/
  if (FALSE == node_found)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Return Node
  -------------------------------------------------------------------------*/
  *node = lookup_node;
  return TRUE;
} /* ps_hashtablei_lookup_node */

/*==============================================================================

                    EXTERNAL FUNCTIONS

==============================================================================*/
/*===========================================================================
FUNCTION PS_HASHTABLE_CREATE()

DESCRIPTION
  This function is used to allocate & initialize Hashtable control block

PARAMETERS
  (IN) table_size: The size of hash table.
  (IN) bucket_depth: The max depth of each bucket.

RETURN VALUE
  Handle on success
  NULL on failures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_hashtable_type ps_hashtable_create
(
   uint16 table_size,
   uint16 bucket_depth,
   void (*free_f_cb)(void*)
)
{
  ps_ht_cb_type *ht_cb_ptr;
  uint32         idx;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_HASHTABLE_MAX_TBL_SIZE < table_size)
  {
    table_size = PS_HASHTABLE_MAX_TBL_SIZE;
    LOG_MSG_INFO1_1("ps_hashtable_create(): Requested table size is more than "
                    "systems limit, using max %d", table_size);
  }

  if (PS_HASHTABLE_MAX_BKT_SIZE < bucket_depth)
  {
    bucket_depth = PS_HASHTABLE_MAX_BKT_SIZE;
    LOG_MSG_INFO1_1("ps_hashtable_create(): Requested bucket depth is more than "
                    "systems limit, using max %d", bucket_depth);
  }

  if (NULL == free_f_cb)
  {
    LOG_MSG_INFO1_0("ps_hashtable_create(): NULL free_f_cb");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Allocate memory to hold the control block
  -------------------------------------------------------------------------*/
  ht_cb_ptr = (ps_ht_cb_type *)ps_mem_get_buf(PS_MEM_HASHTABLE_CB_TYPE);
  if( NULL == ht_cb_ptr )
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    memset the control block.
  -------------------------------------------------------------------------*/
  memset(ht_cb_ptr, 
          0,
          sizeof(ps_ht_cb_type));

  ht_cb_ptr->this_cb_ptr = ht_cb_ptr;
  PS_SYSTEM_HEAP_MEM_ALLOC(ht_cb_ptr->hashtbl, 
                           sizeof(ps_ht_bkt_type) * table_size,
                           ps_ht_bkt_type*);
  if( NULL == ht_cb_ptr->hashtbl )
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize each bucket.
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < table_size; idx++)
  {
    memset(&ht_cb_ptr->hashtbl[idx], 
            0,
            sizeof(ps_ht_bkt_type));
  }

  ht_cb_ptr->table_size   = table_size;
  ht_cb_ptr->bucket_depth = bucket_depth;
  ht_cb_ptr->free_f_cb    = free_f_cb;
  ht_cb_ptr->last_node    = NULL;
  ht_cb_ptr->last_idx     = -1;

  return (ps_hashtable_type) ht_cb_ptr;
}


/*===========================================================================
FUNCTION PS_HASHTABLE_ADD()

DESCRIPTION
  This function is used to add a node to Hashtable.

PARAMETERS 
  (IN) handle    : Hashtable handle.  
  (IN) key       : Key for the node
  (IN) value     : pointer to the node.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_add
(
   ps_hashtable_type  handle,
   uint64             key,
   void               *value
)
{
  ps_ht_cb_type      *ht_cb_ptr = NULL;
  ps_ht_node_type    *node      = NULL;
  ps_ht_bkt_type     *bucket    = NULL;
  uint64              hash;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_add(): Invalid handle");
    return FALSE;
  }

  if (NULL == value)
  {
    LOG_MSG_ERROR_0("ps_hashtable_add(): NULL value pointer");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Allocate node
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(node, sizeof(ps_ht_node_type),
                           ps_ht_node_type*);
  if( NULL == node )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Set Key & value in node
  -------------------------------------------------------------------------*/
  node->key   = key;
  node->value = value;

  /*-------------------------------------------------------------------------
    Calculate Hash from key
  -------------------------------------------------------------------------*/
  hash = ps_ht_calc_hash(ht_cb_ptr, key);

  /*-------------------------------------------------------------------------
    Lookup the hashtable based on hash.
  -------------------------------------------------------------------------*/
  bucket = &ht_cb_ptr->hashtbl[hash];

  /*-------------------------------------------------------------------------
     Add the new entry to the hashtable
  -------------------------------------------------------------------------*/
  ps_ht_add_node(bucket->tail, node);

  /*-------------------------------------------------------------------------
    Special case handling, if this is the first node in bucket.
  -------------------------------------------------------------------------*/
  if (NULL == bucket->head)
  {
    bucket->head = node;
  }

  /*-------------------------------------------------------------------------
    Increment node count
  -------------------------------------------------------------------------*/
  bucket->cnt++;

  /*-------------------------------------------------------------------------
    New node is added, thus reset state.
  -------------------------------------------------------------------------*/
  ht_cb_ptr->last_idx  = -1;
  ht_cb_ptr->last_node = NULL;

  return TRUE;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_LOOKUP()

DESCRIPTION
  This function is used for lookup in Hashtable.

PARAMETERS
  (IN) handle    : Hashtable handle.  
  (IN) key       : Lookup key
  (OUT) value    : pointer to return node.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_lookup
(
   ps_hashtable_type  handle,
   uint64             key,
   void             **value
)
{
  ps_ht_cb_type     *ht_cb_ptr;
  ps_ht_node_type   *node;
  ps_ht_bkt_type    *bucket;
  uint64             hash;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_lookup(): Invalid handle");
    return FALSE;
  }

  if (NULL == value)
  {
    LOG_MSG_ERROR_0("ps_hashtable_lookup(): NULL value pointer");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Calculate Hash from key
  -------------------------------------------------------------------------*/
  hash = ps_ht_calc_hash(ht_cb_ptr, key);

  /*-------------------------------------------------------------------------
    Lookup the hashtable based on hash.
  -------------------------------------------------------------------------*/
  bucket = &ht_cb_ptr->hashtbl[hash];

  /*-------------------------------------------------------------------------
    Lookup node in bucket.
  -------------------------------------------------------------------------*/
  if ( FALSE == ps_ht_lookup_node(bucket, key, &node) )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Return value.
  -------------------------------------------------------------------------*/
  *value = node->value;
  return TRUE;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_RESET_STATE()

DESCRIPTION
  This function resets the internal state to start get next kay val pair
  search from begining.

PARAMETERS
  (IN) handle    : Hashtable handle.  
 
RETURN VALUE
  0  on SUCCESS
  -1 on FAILURE
  1  on End of List

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_hashtable_reset_state
(
  ps_hashtable_type  handle
)
{
  ps_ht_cb_type     *ht_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_reset_state(): Invalid handle");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Reset state.
  -------------------------------------------------------------------------*/
  ht_cb_ptr->last_idx  = -1;
  ht_cb_ptr->last_node = NULL;

  return TRUE;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_GET_NEXT_KEY_VALUE_PAIR()

DESCRIPTION
  This function returns the next key value pair in the hashtable.

PARAMETERS
  (IN) handle    : Hashtable handle.  
  (OUT) key      : Pointer to Key
  (OUT) value    : pointer to Value.

RETURN VALUE
  0  on SUCCESS
  -1 on FAILURE
  1  on End of List

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_ht_return_type ps_hashtable_get_next_key_value_pair
(
  ps_hashtable_type  handle,
  uint64            *key,
  void             **value
)
{
  ps_ht_cb_type     *ht_cb_ptr;
  ps_ht_bkt_type    *bucket;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_get_next_key_value_pair(): Invalid handle");
    return PS_HT_FAILURE;
  }

  if (NULL == value && NULL == key)
  {
    LOG_MSG_ERROR_0("ps_hashtable_get_next_key_value_pair(): NULL key & value pointer");
    return PS_HT_FAILURE;
  }

  if (NULL != ht_cb_ptr->last_node && 
      NULL != ht_cb_ptr->last_node->next)
  {
    /*-------------------------------------------------------------------------
      If there are mode nodes in the buckets, update last_node to new node.
    -------------------------------------------------------------------------*/
    ht_cb_ptr->last_node = ht_cb_ptr->last_node->next;
  }
  else
  {
    /*-------------------------------------------------------------------------
      If no more nodes left in bucket, search for next available.
    -------------------------------------------------------------------------*/
    while (TRUE)
    {
      ht_cb_ptr->last_idx++;
      if (ht_cb_ptr->last_idx >= ht_cb_ptr->table_size)
      {
        /*-------------------------------------------------------------------------
          If end of list is reached, return EOL to client.
        -------------------------------------------------------------------------*/
        ht_cb_ptr->last_idx = -1;
        ht_cb_ptr->last_node = NULL;
        return PS_HT_EOL;
      }

      /*-------------------------------------------------------------------------
        If non empty bucket is found, set the node as last_node.
      -------------------------------------------------------------------------*/
      bucket = &ht_cb_ptr->hashtbl[ht_cb_ptr->last_idx];

      if (NULL != bucket->head)
      {
        ht_cb_ptr->last_node = bucket->head;
        break;
      }
    };
  }

  /*-------------------------------------------------------------------------
    If key is requested, return it.
  -------------------------------------------------------------------------*/
  if (NULL != key)
  {
    *key = ht_cb_ptr->last_node->key;
  }

  /*-------------------------------------------------------------------------
    If value is requested, return it.
  -------------------------------------------------------------------------*/
  if (NULL != value)
  {
    *value = ht_cb_ptr->last_node->value;
  }

  return PS_HT_SUCCESS;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_REMOVE()

DESCRIPTION
  This function is used remove node from Hashtable.

PARAMETERS 
  (IN) handle    : Hashtable handle.  
  (IN) key       : Lookup key
  (IN) value     : pointer to return removed node.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_remove
(
   ps_hashtable_type  handle,
   uint64             key,
   void             **value
)
{
  ps_ht_cb_type      *ht_cb_ptr;
  ps_ht_node_type    *node;
  ps_ht_bkt_type     *bucket;
  uint64              hash;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_remove(): Invalid handle");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Calculate Hash from key
  -------------------------------------------------------------------------*/
  hash = ps_ht_calc_hash(ht_cb_ptr, key);

  /*-------------------------------------------------------------------------
    Lookup the hashtable based on hash.
  -------------------------------------------------------------------------*/
  bucket = &ht_cb_ptr->hashtbl[hash];

  /*-------------------------------------------------------------------------
    Lookup node in bucket.
  -------------------------------------------------------------------------*/
  if ( FALSE == ps_ht_lookup_node(bucket, key, &node) )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Remove node from bucket
  -------------------------------------------------------------------------*/
  ps_ht_rm_node(node);

  /*-------------------------------------------------------------------------
    Special case handling if this is the only node in the bucket.
  -------------------------------------------------------------------------*/
  if (bucket->head == node)
  {
    bucket->head = node->next;
  }

  if (bucket->tail == node)
  {
    bucket->tail = node->prev;
  }

  /*-------------------------------------------------------------------------
    Return Value.
  -------------------------------------------------------------------------*/
  if ( NULL != value )
  {
    *value = node->value;
  }

  /*-------------------------------------------------------------------------
    Update state.
  -------------------------------------------------------------------------*/
  if ( node == ht_cb_ptr->last_node )
  {
    ht_cb_ptr->last_node = node->prev;
  }

  /*-------------------------------------------------------------------------
    Free node
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_FREE(node);

  /*-------------------------------------------------------------------------
    Decrement node count
  -------------------------------------------------------------------------*/
  bucket->cnt--;
  return TRUE;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_REMOVE_ALL()

DESCRIPTION
  This function is used to free Hashtable entries.

PARAMETERS
  (IN) handle: hashtable handle

RETURN VALUE
  TRUE on success
  FALSE on failures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_remove_all
(
   ps_hashtable_type  handle
)
{
  ps_ht_cb_type   *ht_cb_ptr    = (ps_ht_cb_type *)handle;
  uint8            idx          = 0;
  ps_ht_bkt_type  *bucket       = NULL;
  ps_ht_node_type *node         = NULL;
  ps_ht_node_type *node_to_free = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_remove_all(): Invalid handle");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Allocate memory to hold the control block
  -------------------------------------------------------------------------*/
  ht_cb_ptr = (ps_ht_cb_type *)handle;

  for (idx = 0; idx < ht_cb_ptr->table_size; idx++)
  {
    /*-------------------------------------------------------------------------
      Iterate through each bucket
    -------------------------------------------------------------------------*/
    bucket = &ht_cb_ptr->hashtbl[idx];
    node = bucket->head;
    while (node)
    {
      /*-------------------------------------------------------------------------
        Iterate through each node in bucket and free it.
      -------------------------------------------------------------------------*/
      node_to_free = node;
      node         = node->next;

      /*-------------------------------------------------------------------------
        Free value.
      -------------------------------------------------------------------------*/
      ht_cb_ptr->free_f_cb(node_to_free->value);

      /*-------------------------------------------------------------------------
        Free node.
      -------------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_FREE(node_to_free);
    }
  }

  /*-------------------------------------------------------------------------
    Clear state.
  -------------------------------------------------------------------------*/
  ht_cb_ptr->last_idx  = -1;
  ht_cb_ptr->last_node = NULL;

  return TRUE;
}

/*===========================================================================
FUNCTION PS_HASHTABLE_DESTROY()

DESCRIPTION
  This function is used to destroy Hashtable and free the control block.

PARAMETERS
  (IN) handle: hashtable handle

RETURN VALUE
  TRUE on success
  FALSE on failures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_destroy
(
   ps_hashtable_type  handle
)
{
  ps_ht_cb_type   *ht_cb_ptr    = (ps_ht_cb_type *)handle;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!HT_CB_IS_VALID(ht_cb_ptr))
  {
    LOG_MSG_ERROR_0("ps_hashtable_destroy(): Invalid handle");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Free all nodes of hashtable.
  -------------------------------------------------------------------------*/
  ps_hashtable_remove_all(handle);

  /*-------------------------------------------------------------------------
    Clear state.
  -------------------------------------------------------------------------*/
  ht_cb_ptr->last_idx  = -1;
  ht_cb_ptr->last_node = NULL;

  /*-------------------------------------------------------------------------
    Free hashtable control block.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_FREE(ht_cb_ptr->hashtbl);

  PS_MEM_FREE(ht_cb_ptr);
  return TRUE;
}

