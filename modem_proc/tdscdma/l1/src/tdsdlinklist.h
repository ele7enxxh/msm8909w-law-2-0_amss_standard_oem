#ifndef TDSDLINKLIST_H
#define TDSDLINKLIST_H

/*============================================================================
                            TDSD L I N K L I S T . H
DESCRIPTION

Copyright (c) 2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlinklist.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
1/12/14     sarao  Refactoring code from WCDMA to TDSCDMA
11/06/03    gs      Changed the node count in link list to int32
10/24/03    gs      Created header file for double link list definition and
                    declaration
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* link list node position type */
typedef enum
{
  LL_POS_HEAD,        /* Head node of link list */
  LL_POS_TAIL,        /* Tail node of link list */
  LL_POS_BEFORE_NODE, /* Before specified node in link list */
  LL_POS_AFTER_NODE,  /* After specified node in link list */
  LL_POS_AT_NODE      /* At the specified node (only for seek at) */
} tdsdll_pos_enum_type;

/* link list node structure */
typedef struct double_link_list_node
{
  /* link to data pointer to which this node points to */
  void   *data_ptr;
  
  /* prev node pointer */
  struct double_link_list_node *prev;
  /* next node pointer */
  struct double_link_list_node *next;

  /* host link list pointer that has this node linked to */
  void   *host_link_list_ptr;
} tdsdll_node_struct_type;

/* link list structure */
typedef struct
{
  /* number of nodes in the link list */
  int32 count;

  /* head node pointer */
  tdsdll_node_struct_type *head;
  /* tail node pointer */
  tdsdll_node_struct_type *tail;
  /* current node pointer */
  tdsdll_node_struct_type *cur_link;

  /* indicate circular mode */
  boolean mode_circular;
} tdsdll_struct_type;


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdsdll_init

DESCRIPTION
  This function initialize link list. It initialize the following element
  of link list.
  - Set node count to 0
  - Head, tail, current node pointer to NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All element of link list structure are reset
===========================================================================*/

extern void tdsdll_init(
  /* double link list pointer to initialize */
  tdsdll_struct_type *ll_ptr);

/*===========================================================================
FUNCTION tdsdll_link_node

DESCRIPTION
  This function links link list node to its host buffer. It sets node data
  pointer to the buffer pointer passed as function argument. One typical
  use of this will be set data_ptr element to pointer of structure carrying
  this link list node.
  This function must be used before linking (inserting) this node in any
  link list.

DEPENDENCIES
  Node structure pointer passed in function as argument

RETURN VALUE
  None

SIDE EFFECTS
  Previous and Next link list node pointer are set to NULL.
  It assumes that node has not yet been linked to any link list therefore
  lost link list pointer is set to NULL.
===========================================================================*/

extern tdsdll_node_struct_type* tdsdll_link_node(
  /* link list node to link */
  tdsdll_node_struct_type *ll_node,
  /* data pointer to link */
  void *this_data_ptr);

/*===========================================================================
FUNCTION tdsdll_insert_node

DESCRIPTION
  This function inserts (links) link list node to a given link list. Node
  can be inserted at head or tail of link list OR before or after any node
  that belongs to that link list.
  
  In case insert pos is set to before or after node and if that node doesn't
  belong to this link list then code does ERR_FATAL. Allowing such operation
  will result unpredictable behavior.
  
  Also it is assumed that node to be inserted is not a member of other link
  list. In case it is already a member then function will return with
  ERR_FATAL. This is to prevent insert node in 2 link lists or insert in one
  link list before deleting in other link list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates node elements prev, next node pointer and sets host link list
  pointer so that node it self knows to which list it belongs to.
===========================================================================*/

extern void tdsdll_insert_node(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* link list node pointer to insert */
  tdsdll_node_struct_type *ll_node,
  /* insert position */
  tdsdll_pos_enum_type insert_pos,
  /* link list node pointer if insert before of after node */
  tdsdll_node_struct_type *ll_node_before_after);

/*===========================================================================
FUNCTION tdsdll_remove_node

DESCRIPTION
  This function removes node from link list. Node must be member of the link
  list from where it is removed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Node info elements is reset.
===========================================================================*/

extern void tdsdll_remove_node(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* link list node pointer to insert */
  tdsdll_node_struct_type *ll_node);

/*===========================================================================
FUNCTION tdsdll_get_cur_node

DESCRIPTION
  This function returns currrent node pointer

DEPENDENCIES
  None

RETURN VALUE
  Pointer to link list node structure type.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdll_node_struct_type* tdsdll_get_cur_node(
    /* link list pointer */
  tdsdll_struct_type *ll_ptr);

/*===========================================================================
FUNCTION tdsdll_get_next_node

DESCRIPTION
  This function returns the next node pointer of current node. If seek is
  set to TRUE then it moves link list current node to next.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to link list node structure type. NULL if link list is empty.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdll_node_struct_type* tdsdll_get_next_node(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* set the current node to next node */
  boolean seek);

/*===========================================================================
FUNCTION tdsdll_get_prev_node

DESCRIPTION
  This function returns the previous node pointer of current node. If seek is
  set to TRUE then it moves link list current node to previous.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to link list node structure type. NULL if link list is empty.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdll_node_struct_type* tdsdll_get_prev_node(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* set the current node to next node */
  boolean seek);

/*===========================================================================
FUNCTION tdsdll_seek_node

DESCRIPTION
  This function sets current node pointer to specific location (head/tail)
  or to any node that belongs to this link list.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to link list node structure type. NULL if link list is empty.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdll_node_struct_type* tdsdll_seek_node(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* insert position */
  tdsdll_pos_enum_type seek_pos,
  /* link list node pointer to seek */
  tdsdll_node_struct_type *ll_node_to_seek);

/*===========================================================================
FUNCTION tdsdll_mode_circular

DESCRIPTION
  This function put link list in or out of circular mode by linking or
  delinking head and tail together repectively.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Head and tail node prev and next are updated.
===========================================================================*/

extern void tdsdll_mode_circular(
  /* link list pointer */
  tdsdll_struct_type *ll_ptr,
  /* mode circular or not */
  boolean mode);

#endif /* DLINKLIST_H */
