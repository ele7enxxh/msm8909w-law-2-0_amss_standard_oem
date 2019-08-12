/*==========================================================================

                STM_BM.C    STM Buffer Management Subsystem 

Description:
  This module contains the functions needed to create a pool of buffers and
  to allocate and deallocate the buffers and manage the pool.  A pool consists
  of a pre-determined maximum number of buffer lists each containing buffers
  of a different size.

  An example of how to use this follows:

    #define MAX_SIZES  2     maximum number of different buffer sizes
    #define BUF1_SIZE  16    number of data bytes in buffer list 1 buffers  
    #define BUF1_CNT   5     number of buffers in buffer list 1
    #define BUF2_SIZE  80    number of data bytes in buffer list 2 buffers
    #define BUF2_CNT   3     number of buffers in buffer list 2

    stm_bm_handle handle;    handle for calls to stm_bm_ functions for this pool 

    stm_bm_list_type node_mem[MAX_SIZES];    memory for buffer list nodes

  To declare memory for the buffers themselves use this method:

    uint8 buf1_mem[STM_BM_BUF_MEM_SIZE(BUF1_SIZE, BUF1_CNT)];

  or this method:

    struct
    {
      stm_bm_hdr_type hdr;
      uint8           buf[BUF2_SIZE];
    }
    buf2_mem[BUF2_CNT];

  To create an instance of STM_BM and add buffer lists to it:

    handle = stm_bm_create( MAX_SIZES, node_mem );
    stm_bm_add_buffer_list( handle, BUF1_SIZE, BUF1_CNT, buf1_mem );
    stm_bm_add_buffer_list( handle, BUF2_SIZE, BUF2_CNT, buf2_mem );

  To allocate and deallocate buffers:

    buf_p = stm_bm_get_buffer( handle, BUF1_SIZE );
    stm_bm_return_buffer( handle, buf_p );

  Buffers can be added to an existing buffer list via stm_bm_add_buffer_list()
  with the same buffer size as an existing buffer list.  Newbuffer lists can be
  added at any time provided that there at most MAX_SIZES differnet buffer
  sizes.  A buffer list can be deleted by calling stm_bm_remove_buffer_list().

  Although STM_BM is similar to malloc() and free(), it cannot be fragmented and
  the address of each buffer can be calculated from the base address of the
  buffer list memory which is in the memory map.

Externalized Functions:
  stm_bm_create
  stm_bm_add_buffer_list
  stm_bm_remove_buffer_list
  stm_bm_get_buffer
  stm_bm_return_buffer

Initialization and Sequencing Requirements:
  stm_bm_create() must be called first

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/stm/src/stm_bm.c#1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
09/22/11   ag      Avoid re-initializing critical section mutex.
03/23/09   ag      Replaced INTLOCKs with REX_ISR macros
10/15/06   trc     Convert to STM service (rename to STM_BM)
11/14/05   awj     Lint and standard compliance
10/26/05   awj     Lint compliance
07/25/05   awj     Added new functionality for resetting and reporting.
                   Fixed stm_bm_remove_buffer_list() to work properly.
07/21/05   kwo     Added atomic sections so funcs are re-entrant
08/27/03   sfm     new file
===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/

#include "mcs_variation.h"
#include "comdef.h"
#include "rex.h"
#include "stm_bm.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

#undef STM_BM_DEBUG

/* rex critical section lock */
static rex_crit_sect_type stm_bm_crit_sect;

/* Has the lock been initialized once */
static boolean stm_bm_init = FALSE;

/* STM_BM critical section macros that map to REX ISR macros.
   Since stm_bm_get_buffer is being called from an interrupt context
   REX_IS_LOCK/UNLOCK are more suited to use here */
#define STM_BM_INIT_CRIT_SECT(lock) \
{ \
  REX_DISABLE_PREMPTION(); \
  if (stm_bm_init == FALSE) \
  { \
    rex_init_crit_sect(&(lock)); \
	stm_bm_init = TRUE; \
  } \
  REX_ENABLE_PREMPTION(); \
}
#define STM_BM_ENTER_CRIT_SECT(lock)  REX_ISR_LOCK(&(lock)) 
#define STM_BM_LEAVE_CRIT_SECT(lock)  REX_ISR_UNLOCK(&(lock))



/*===========================================================================
                FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION STM_BM_CREATE

DESCRIPTION    Creates the list nodes linked list using list_mem.   max_lists
               is the maximum number of different buffer sizes allowed.  The
               returned handle is a pointer to the first node in the list of
               buffer lists.
               
DEPENDENCIES   list must point to a memory area that is at least
               (sizeof(stm_bm_list_type) * max_lists) bytes.

RETURN VALUE   A handle if parameters are valid, NULL if not

SIDE EFFECTS   None

===========================================================================*/
stm_bm_handle stm_bm_create
(
  uint32            max_lists,   /* maximum number of different buffer sizes */
  stm_bm_list_type* list         /* pointer to memory for buffer list nodes */
)
{
  uint32 i;                    /* loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( list == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: list is NULL");
    return NULL;
  }

  if ( max_lists == 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: max_lists is 0");
    return NULL;
  }
  
  #ifdef STM_BM_DEBUG
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "stm_bm_create(%ld, 0x%lx)", max_lists, (uint32)list);
  #endif

  /* initialize the list nodes to be in a doubly linked list */
  for ( i = 0; i < max_lists; i++ )
  {
    list[i].buf_size = 0;
    list[i].used = 0;
    list[i].max_used = 0;
    list[i].tot_max_used = 0;
    list[i].head = NULL;
    list[i].tail = NULL;
    if ( i > 0 )
    {
      list[i].previous = &list[i - 1];
    }
    if ( i < max_lists - 1 )
    {
      list[i].next = &list[i + 1];
    }
  }
  list[0].previous = NULL;
  list[max_lists - 1].next = NULL;

  /* Initialize the critical section if its not already */
  STM_BM_INIT_CRIT_SECT(stm_bm_crit_sect)

  return (stm_bm_handle)list;

}  /* stm_bm_create */


/*===========================================================================

FUNCTION UNLINK_NODE

DESCRIPTION    Un-links a buffer list node from the doubly linked list
               
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
static void unlink_node
(
  stm_bm_list_type* node    /* ptr to the node to be un-linked */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  STM_BM_NULL_PTR_CHK( node, RETURN_VOID );

  if ( node->next != NULL )
  {
    if ( node->previous != NULL )
    {
      node->previous->next = node->next;
    }
    node->next->previous = node->previous;
  }
  else    /* node is the last node in the list */
  {
    if ( node->previous != NULL )
    {
      node->previous->next = NULL;
    }
  }

}  /* unlink_node */


/*===========================================================================

FUNCTION STM_BM_ADD_BUFFER_LIST

DESCRIPTION    Adds a list of buffers.  If the buffers are the same size as an
               existing list, the buffers are appended to that list.  Otherwise
               an unused buffer list node is found and populated with the new
               buffer list.  The buffer lists are maintained in ascending order
               by buffer size thus it is fastest if buffer lists are added in
               this order (but not required).
               
DEPENDENCIES   There must be an unused buffer list node unless the buffers are
               the same size as an existing list

RETURN VALUE   TRUE if the buffers were added, FALSE if not

SIDE EFFECTS   None

===========================================================================*/
boolean stm_bm_add_buffer_list
( 
  stm_bm_handle     handle,       /* handle for this buffer pool */
  uint32            buf_size,     /* buffer size in bytes */
  uint32            n_buf,        /* number of buffers */
  uint8            *buf_mem       /* ptr to memory for the buffers */
)
{
  stm_bm_list_type *list;         /* ptr to buffer list nodes */
  stm_bm_list_type *new_list;     /* ptr to buffer list node for new list */
  stm_bm_list_type *p;            /* for traversing the buffer list nodes */
  stm_bm_hdr_type  *hdr_p;        /* for calculating buffer addresses */
  stm_bm_hdr_type  *next_hdr_p;   /* ptr to last buffer in the buffer list */
  boolean           next_ok;      /* used for positioning a buffer list node */
  boolean           prev_ok;      /* used for positioning a buffer list node */
  uint8            *s;            /* ptr to start of a buffer */
  uint32            i;            /* loop counter */
  boolean          ret_val = TRUE;/* Value to be returned */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( handle == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: handle is NULL");
    return FALSE;
  }

  if ( buf_mem == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: buf_mem is NULL");
    return FALSE;
  }

  if ( buf_size == 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: buf_size = 0");
    return FALSE;
  }

  if ( n_buf == 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: n_buf is 0");
    return FALSE;
  }
  
  #ifdef STM_BM_DEBUG
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "stm_bm_add_buffer_list(0x%lx, %ld, %ld, 0x%lx)",
        (uint32)handle, buf_size, n_buf, (uint32)buf_mem);
  #endif

  STM_BM_ENTER_CRIT_SECT(stm_bm_crit_sect);

  /* find an empty list or a list of buffers with the same size */
  list = new_list = (stm_bm_list_type*)handle;
  while ( new_list != NULL )
  {
    if ( new_list->buf_size == buf_size || new_list->buf_size == 0 )
    {
      break;
    }
    new_list = new_list->next;
  }

  do
  {
    /* make sure we found a spot for the new buffer list */
    if ( new_list == NULL )
    {
        ret_val = FALSE;
        break;
    }
  
    /* carve the buffer list memory up into a linked list of buffers */
    s = buf_mem;
    hdr_p = (stm_bm_hdr_type*)buf_mem;
    for ( i = 0; i < n_buf; i++ )
    {
      hdr_p->buf_size = buf_size;              /* set buf_size in header */
      s += sizeof(stm_bm_hdr_type) + buf_size; /* point to start of next buffer */
      next_hdr_p = (stm_bm_hdr_type*)s;
      hdr_p->next = (i < n_buf - 1) ? next_hdr_p : NULL;
      hdr_p = next_hdr_p;
    } 
  
    /* save ptr to last node in the list */
    next_hdr_p = (stm_bm_hdr_type *)(s - (buf_size + sizeof(stm_bm_hdr_type)));
    hdr_p = (stm_bm_hdr_type*)buf_mem; /* make hdr_p point to list of buffers */
  
    /* adding to an existing list? */
    if ( new_list->buf_size == buf_size )
    {
      next_hdr_p->next = new_list->head;  /* append existing list to new list */
      new_list->head = hdr_p;             /* point head pointer to new list */
    }
  
    /* adding a new buffer size? */
    if ( new_list->buf_size == 0 )
    {
      new_list->buf_size = buf_size;
      new_list->head = hdr_p;
      new_list->tail = next_hdr_p;
  
      /* determine if new_list is already in the correct location in the list */
      prev_ok = new_list->previous == NULL ||
                new_list->buf_size > new_list->previous->buf_size;
      next_ok = new_list->next == NULL ||
                new_list->next->buf_size > new_list->buf_size ||
                new_list->next->buf_size == 0;
      if ( prev_ok && next_ok )
      {  
        break;
      }      

      /* unlink the node from the list */
      unlink_node( new_list );
  
      /* put the new list in the correct location */
      p = list;
      while ( p != NULL )
      {
        if ( new_list->buf_size < p->buf_size || p->buf_size == 0 )
        {
          /* put new_list before p */
          new_list->next = p;
          new_list->previous = p->previous;
          if ( p->previous != NULL )
          {
            p->previous->next = new_list;
          }
          p->previous = new_list;
          break;
        }
  
        if ( p->next == NULL )
        {
          /* put new_list at the end of the list */
          p->next = new_list;
          new_list->previous = p;
          new_list->next = NULL;
          break;
        }
        p = p->next;
      }
    }
  } while (0);

  STM_BM_LEAVE_CRIT_SECT(stm_bm_crit_sect);
  return ret_val;    /* buffer list was added */

}  /* stm_bm_add_buffer list */


/*===========================================================================

FUNCTION STM_BM_REMOVE_BUFFER_LIST

DESCRIPTION    Removes a list of buffers and makes the buffer list node
               available for re-use by setting the buffer list size to 0
               amd moving it to the end of the node list.
               
DEPENDENCIES   buf_size must exactly match an existing buffer list size

RETURN VALUE   TRUE if the buffer list was removed, FALSE if not

SIDE EFFECTS   Changes 'handle'

===========================================================================*/
boolean stm_bm_remove_buffer_list
(
  stm_bm_handle *handle,           /* handle for this buffer pool */
  uint32         buf_size          /* size of buffer in list to remove */
)
{
  stm_bm_list_type* node;          /* ptr to node of buffer list to remove */
  stm_bm_list_type* p;             /* for traversing the list of nodes */
  stm_bm_list_type* new_handle;    /* new value of handle */
  boolean ret_val = FALSE;         /* Value to be returned */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( handle == NULL || *handle == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: handle is NULL");
    return FALSE;
  }

  if ( buf_size == 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: buf_size is 0");
    return FALSE;
  }

  #ifdef STM_BM_DEBUG
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "stm_bm_remove_buffer_list(0x%lx, %ld)", (uint32)handle, buf_size);
  #endif

  STM_BM_ENTER_CRIT_SECT(stm_bm_crit_sect);

  /* find the buffer list */
  node = p = *(stm_bm_list_type **)handle;
  new_handle = node;
  while ( node != NULL && node->buf_size != 0 )
  {
    if ( node->buf_size == buf_size )
    {
      /* clear pointers and set buffer size to 0 */
      node->buf_size = 0;
      node->head = node->tail = NULL;

      /* save new ptr to the start of the list */
      if (node == new_handle)
      {
        new_handle = node->next;
      }

      if ( node->next != NULL )    /* already at the end of the list? */
      {
        /* find the end of the list */
        p = node->next;
        while ( p->next != NULL )
        {
          p = p->next;
        }

        /* move the node to the end of the list for reuse */
        unlink_node( node );
        p->next = node;
        node->previous = p;
        node->next = NULL;
        
        /* change handle to point to the new start of the list */
        *(stm_bm_list_type **)handle = new_handle;
        ret_val = TRUE;
        break;
      }
    }
    p = p->next;
  }

  STM_BM_LEAVE_CRIT_SECT(stm_bm_crit_sect);
  return ret_val;

}  /* stm_bm_remove_buffer_list */


/*===========================================================================

FUNCTION STM_BM_GET_BUFFER

DESCRIPTION    Gets a buffer whose size is >= 'size'.
               
DEPENDENCIES   None

RETURN VALUE   Pointer to the buffer if one was found, NULL otherwise

SIDE EFFECTS   None

===========================================================================*/
uint8* stm_bm_get_buffer
(
  stm_bm_handle handle,     /* handle for this buffer pool */
  uint32        size        /* minimum buffer size */
)
{
  stm_bm_list_type *p;      /* for traversing the list of nodes */
  uint8            *s;      /* ptr to buffer to be returned */
  uint32            cnt;    /* current number of buffers checked out */
  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( handle == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: handle is NULL");
    return NULL;
  }

  if ( size == 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: size is 0");
    return NULL;
  }

  STM_BM_ENTER_CRIT_SECT(stm_bm_crit_sect);
  p = (stm_bm_list_type*)handle;
  s = NULL;

  /* find the first buffer list with buffers >= size */
  while ( p != NULL && p->buf_size != 0 )
  {
    if ( p->buf_size >= size )
    {
      if ( p->head != NULL )     /* at least one buffer? */
      {
        p->used++;
        if (p->max_used < p->used)
        {
          p->max_used++;
        }
        s = (uint8*) p->head + sizeof(stm_bm_hdr_type);  /* start of data */
        p->head = p->head->next;                         /* unlink the buffer */
        if ( p->head == NULL )
        {
          p->tail = NULL;        /* list is now empty */
        }
        break;
      }
    }
    p = p->next;
  }

  p = (stm_bm_list_type*)handle;
  cnt = 0;

  while ( p != NULL && p->buf_size != 0 )
  {
    cnt += p->used;
    p = p->next;
  }

  p = (stm_bm_list_type*)handle;

  if (p->tot_max_used < cnt)
  {
    p->tot_max_used = cnt;
  }

  STM_BM_LEAVE_CRIT_SECT(stm_bm_crit_sect);

  #ifdef STM_BM_DEBUG
  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "stm_bm_get_buffer(0x%lx, %ld) returned 0x%lx",
        (uint32)handle, size, (uint32)s );
  #endif

  return s;

}  /* stm_bm_get_buffer */


/*===========================================================================

FUNCTION STM_BM_RETURN_BUFFER

DESCRIPTION    Returns a buffer to the buffer pool.
               
DEPENDENCIES   None

RETURN VALUE   Pointer to the buffer if one was found, NULL otherwise

SIDE EFFECTS   None

===========================================================================*/
boolean stm_bm_return_buffer
(
  stm_bm_handle  handle,      /* handle for this buffer pool */
  uint8     *buf          /* ptr to buffer to return */
)
{
  stm_bm_list_type* p;        /* for traversing the node list */
  stm_bm_hdr_type*  hdr_p;    /* ptr to the header for this buffer */
  boolean ret_val = FALSE;    /* Value to be returned */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( handle == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: handle is NULL");
    return FALSE;
  }

  if ( buf == NULL )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "invalid parameter: buffer ptr is NULL");
    return FALSE;
  }

  #ifdef STM_BM_DEBUG
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "stm_bm_return_buffer(0x%lx, 0x%lx)", (uint32)handle, (uint32)buf);
  #endif

  STM_BM_ENTER_CRIT_SECT(stm_bm_crit_sect);

  p = (stm_bm_list_type*)handle;

  /* make hdr_p point to the buffer header */
  hdr_p = (stm_bm_hdr_type*)(buf - sizeof(stm_bm_hdr_type));

  /* find the correct buffer list */
  while ( p != NULL && p->buf_size != 0 )
  {
    if ( p->buf_size == hdr_p->buf_size )
    {
      hdr_p->next = NULL;

      /* is the buffer list currently empty? */
      if ( p->head == NULL )
      {
        p->head = hdr_p;
        p->tail = hdr_p;     /* now it has one buffer */
      }
      else    /* add the buffer to the end of the buffer list */
      {
        p->tail->next = hdr_p;
        p->tail = hdr_p;
      }

      /* decrement the number of checked out buffers */
      p->used--;

      ret_val = TRUE;
      break;
    }
    p = p->next;
  }

  STM_BM_LEAVE_CRIT_SECT(stm_bm_crit_sect);
  return ret_val;

}  /* stm_bm_return buffer */

/*===========================================================================

FUNCTION       STM_BM_REPORT_STATS

DESCRIPTION    Report usage and reset statistics trackers

DEPENDENCIES   None

RETURN VALUE   False if there are buffers still checked out

SIDE EFFECTS   None

===========================================================================*/
void stm_bm_report_stats
(
  stm_bm_handle handle
)
{

/*---------------------------------------------------------------------------*/

  STM_BM_NULL_PTR_CHK( handle, RETURN_VOID );

  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_MED,
        "Buffer usage- Total:%d | 32b:%d | 64b:%d | 128b:%d | 256b:%d ",
        handle->tot_max_used, handle->max_used, handle->next->max_used,
        handle->next->next->max_used, handle->next->next->next->max_used);

} /* stm_bm_report_stats */
