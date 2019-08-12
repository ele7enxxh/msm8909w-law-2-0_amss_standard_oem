#ifndef STM_BM_H
#define STM_BM_H
/*==========================================================================

                STM_BM.H   STM Buffer Management Subsystem Header

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/stm_bm.h#1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
10/15/06   trc     Convert to STM service (STM_BM)
11/14/05   awj     Lint and standard compliance
10/26/05   awj     Lint compliance
07/25/05   awj     Added new functionality for resetting and reporting.
08/27/03   sfm     new file
===========================================================================*/

/*===========================================================================
                INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* Header that appears at the start of every data buffer */
struct stm_bm_hdr
{
  uint32             buf_size;      /* size of the buffer in bytes */
  struct stm_bm_hdr* next;          /* for linking buffers in a list */  
};
typedef struct stm_bm_hdr stm_bm_hdr_type;

/* List nodes.  Each points to a linked list of data buffers.  The list
   nodes themselves are in doubly linked list in order by ascending buffer
   size with the unused nodes at the end (buf_size is 0 in these) */ 
struct stm_bm_list
{
  uint32              buf_size;     /* buffer size for this list */
  stm_bm_hdr_type*    head;         /* ptr to first buffer in the list */
  stm_bm_hdr_type*    tail;         /* ptr to last buffer in the list */
  uint32              used;         /* current number of buffers checked out */
  uint32              max_used;     /* maximum number of buffers ever c/o */
  uint32              tot_max_used; /* maximum buffers ever c/o from all lists */
  struct stm_bm_list* next;         /* ptr to next list node */
  struct stm_bm_list* previous;     /* ptr to previous list node */
};
typedef struct stm_bm_list stm_bm_list_type;

typedef stm_bm_list_type* stm_bm_handle;    /* handle is a ptr to the list of pools */

/* macro to determine the amount of memory needed for buf_count buffers
   each containing room for buf_size bytes of data */
#define STM_BM_BUF_MEM_SIZE(buf_size, buf_count) \
  ((sizeof(stm_bm_hdr_type) + (buf_size)) * (buf_count))

/* This Macro should be used to null check interfaces.  If a Void return
   value is desired, then pass in the RETURN_VOID macro */
#define STM_BM_NULL_PTR_CHK(ptr, retval)                                 \
  if (ptr == NULL)                                                       \
  {                                                                      \
    MSG_ERROR("Unexpected Null Ptr", 0, 0, 0); { return retval; }        \
  }

#define RETURN_VOID /* null macro */


/*===========================================================================
		FUNCTION DECLARATIONS
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
extern stm_bm_handle stm_bm_create
(
  uint32            max_lists,   /* maximum number of different buffer sizes */
  stm_bm_list_type* list         /* pointer to memory for buffer list nodes */
);

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
extern boolean stm_bm_add_buffer_list
( 
  stm_bm_handle  handle,             /* handle for this buffer pool */
  uint32         buf_size,           /* buffer size in bytes */
  uint32         n_buf,              /* number of buffers */
  uint8         *buf_mem             /* ptr to memory for the buffers */
);

/*===========================================================================

FUNCTION STM_BM_REMOVE_BUFFER_LIST

DESCRIPTION    Removes a list of buffers and makes the buffer list node
               available for re-use by setting the buffer list size to 0
               amd moving it to the end of the node list.
               
DEPENDENCIES   buf_size must exactly match an existing buffer list size

RETURN VALUE   TRUE if the buffer list was removed, FALSE if not

SIDE EFFECTS   Changes 'handle'

===========================================================================*/
extern boolean stm_bm_remove_buffer_list
(
  stm_bm_handle *handle,           /* handle for this buffer pool */
  uint32         buf_size          /* size of buffer in list to remove */
);

/*===========================================================================

FUNCTION STM_BM_GET_BUFFER

DESCRIPTION    Gets a buffer whose size is >= 'size'.
               
DEPENDENCIES   None

RETURN VALUE   Pointer to the buffer if one was found, NULL otherwise

SIDE EFFECTS   None

===========================================================================*/
extern uint8* stm_bm_get_buffer
(
  stm_bm_handle handle,     /* handle for this buffer pool */
  uint32        size        /* minimum buffer size */
);

/*===========================================================================

FUNCTION STM_BM_RETURN_BUFFER

DESCRIPTION    Returns a buffer to the buffer pool.
               
DEPENDENCIES   None

RETURN VALUE   Pointer to the buffer if one was found, NULL otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean stm_bm_return_buffer
(
  stm_bm_handle  handle,       /* handle for this buffer pool */
  uint8         *buf           /* ptr to buffer to return */
);

/*===========================================================================

FUNCTION       STM_BM_REPORT_STATS

DESCRIPTION    Report usage and reset statistics trackers

DEPENDENCIES   None

RETURN VALUE   False if there are buffers still checked out

SIDE EFFECTS   None

===========================================================================*/
extern void stm_bm_report_stats
(
  stm_bm_handle handle
);

#endif

