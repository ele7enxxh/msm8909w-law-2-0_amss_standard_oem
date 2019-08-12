
/*!
  @file
  lte_lockless_q.h

  @brief
  This file contains inline functions for Fast Q

  @detail
  Fast Queue provides services similary to Queue services (see queue.c).
  It operates much faster than Queue services. The differences are -
  # THESE ARE NOT THREAD SAFE. CALLER MUST ENSURE THREAD SAFETY.
  # THESE ARE NOT COMPATIBLE WITH Queue Services.
  # Functions are inlined (for performance) -- they increase code size.
  #
*/

/*===========================================================================

   Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_lockless_q.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/10   ax      set the link.next_ptr to NULL for item taken out with q_get.
02/17/09   bqiu    Revert back to lte_lockless_q_check to avoid lock.
12/17/08   ar      Fixed the function lte_lockless_q_get() to return NULL if
                   queue is empty
06/27/08   av      Initial version.
===========================================================================*/

#ifndef LTE_LOCKLESS_Q_H
#define LTE_LOCKLESS_Q_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <queue.h>
/*lint -e613*/
/*lint -e528*/


/*===========================================================================

                     EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                      EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                         static inline FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_lockless_q_init

===========================================================================*/
/*!
    @brief
    Initializes a specified Fast Q.

    @detail
    This function should be called for each Fast Q prior to using it with
    other lte_lockless_q* functions.

    @return
    None.
*/
/*=========================================================================*/
static inline void lte_lockless_q_init
(
  q_type                    *q_ptr       /*!< Fast Q to operate on        */
)
{

  q_ptr->cnt = 0;
  q_ptr->link.next_ptr = NULL;
  q_ptr->link.prev_ptr = NULL;

} /* lte_lockless_q_init() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_cnt

===========================================================================*/
/*!
    @brief
    Returns the number of items in a specified Fast Q.

    @return
    The number of items in a specified Fast Q.
*/
/*=========================================================================*/
static inline int lte_lockless_q_cnt
(
  q_type                    *q_ptr          /*!<Fast Q to operate on       */
)
{

  return q_ptr->cnt;

} /* lte_lockless_q_cnt() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_put

===========================================================================*/
/*!
    @brief
    Enqueues an item onto a specified Fast Q using a specified link.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    None.
*/
/*=========================================================================*/
static inline void lte_lockless_q_put
(
  q_type                    *q_ptr,         /*!<Fast Q to operate on      */
  q_link_type               *item_link_ptr  /*!<Put this item at end      */
)
{

  if ( q_ptr->cnt != 0 )
  {
    q_ptr->link.prev_ptr->next_ptr = item_link_ptr;
    q_ptr->link.prev_ptr           = item_link_ptr;
  }
  else
  {
    q_ptr->link.next_ptr = q_ptr->link.prev_ptr = item_link_ptr;
  }
  item_link_ptr->next_ptr = NULL;
  q_ptr->cnt++;

} /* lte_lockless_q_put() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_get

===========================================================================*/
/*!
    @brief
    Removes an item from the head of a specified Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    A pointer to the dequeued item. If the specified Fast Q is empty, then
    NULL is returned.
*/
/*=========================================================================*/
static inline void* lte_lockless_q_get
(
  q_type                    *q_ptr          /*!<Fast Q to operate on      */
)
{

  q_link_type *link_ptr = NULL;

  if ( q_ptr->cnt > 0 )
  {
    link_ptr = q_ptr->link.next_ptr;

    /* If count is greater than 1 */
    if ( q_ptr->cnt != 1)
    {
      /* Update head */
      q_ptr->link.next_ptr = link_ptr->next_ptr;
    }
    else /* count is 1 */
    {
      /* Update head and tail */
      q_ptr->link.next_ptr = q_ptr->link.prev_ptr = NULL;
    }
    q_ptr->cnt--;

    /* this is necessary in case this needs to be used in dsm_enqueue()*/
    link_ptr->next_ptr = NULL;
  }

  return link_ptr;

} /* lte_lockless_q_get() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_check

===========================================================================*/
/*!
    @brief
    Returns a pointer to the item at the head of the Fast Q. The item is
    not removed from the Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    A pointer to the item. If the specified Fast Q is empty, then NULL is
    returned.
*/
/*=========================================================================*/
static inline void* lte_lockless_q_check
(
  q_type                    *q_ptr          /*!<Fast Q to operate on      */
)
{
  return (q_ptr->link.next_ptr);
} /* lte_lockless_q_check() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_last_check

===========================================================================*/
/*!
    @brief
    Returns a pointer to the item at the tail of the Fast Q. The item is
    not removed from the Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    A pointer to the item. If the specified Fast Q is empty, then NULL is
    returned.
*/
/*=========================================================================*/
static inline void* lte_lockless_q_last_check
(
  q_type                    *q_ptr          /*!<Fast Q to operate on      */
)
{
  return (q_ptr->link.prev_ptr);
} /* lte_lockless_q_last_check() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_next

===========================================================================*/
/*!
    @brief
    Returns a pointer to the next item on the Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    A pointer to the item. If the end of the Fast Q has been reached, then
    NULL is returned.
*/
/*=========================================================================*/
static inline void* lte_lockless_q_next
(
  q_type                    *q_ptr,         /*!< Fast Q to operate on     */
  q_link_type               *item_link_ptr  /*!< Get item next to this one*/
)
{
  if( item_link_ptr != NULL )
  {
    return item_link_ptr->next_ptr;
  }
  else
  {
    /* return the first one */
    return lte_lockless_q_check(q_ptr);
  }
} /* lte_lockless_q_next() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_insert

===========================================================================*/
/*!
    @brief
    Inserts an item after a specified item on a Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    None
*/
/*=========================================================================*/
static inline void lte_lockless_q_insert
(
  q_type                    *q_ptr,         /*!<Fast Q to operate on      */
  q_link_type               *item_link_ptr, /*!<Insert this item          */
  q_link_type               *prev_link_ptr  /*!<Insert after this item    */
)
{

  /* Insert in empty Q */
  if ( q_ptr->cnt == 0 )
  {
    item_link_ptr->next_ptr = NULL;
    q_ptr->link.next_ptr = item_link_ptr;
    q_ptr->link.prev_ptr = item_link_ptr;
  }
  /* Insert at head */
  else if ( prev_link_ptr == NULL )
  {
    item_link_ptr->next_ptr = q_ptr->link.next_ptr;
    q_ptr->link.next_ptr = item_link_ptr;
  }
  /* Insert at tail */
  else if ( prev_link_ptr->next_ptr == NULL )
  {
    item_link_ptr->next_ptr = NULL;
    prev_link_ptr->next_ptr = item_link_ptr;
    q_ptr->link.prev_ptr = item_link_ptr;
  }
  /* Insert in the middle */
  else
  {
    item_link_ptr->next_ptr = prev_link_ptr->next_ptr;
    prev_link_ptr->next_ptr = item_link_ptr;
  }
  q_ptr->cnt++;

} /* lte_lockless_q_insert() */


/*===========================================================================

  FUNCTION:  lte_lockless_q_delete

===========================================================================*/
/*!
    @brief
    Removes an item after a specified item on a Fast Q.

    @detail
    The specified Fast Q should have been previously initialized via a call
    to lte_lockless_q_init.

    @return
    None
*/
/*=========================================================================*/
static inline void lte_lockless_q_delete
(
  q_type                    *q_ptr,         /*!<Fast Q to operate on      */
  q_link_type               *prev_link_ptr  /*!<Delete the next item      */
)
{

  /* Delete first & last in Q */
  if ( q_ptr->cnt == 1 )
  {
    q_ptr->link.next_ptr = NULL;
    q_ptr->link.prev_ptr = NULL;
  }
  /* Delete at head */
  else if ( prev_link_ptr == NULL )
  {
    q_ptr->link.next_ptr = q_ptr->link.next_ptr->next_ptr;
  }
  /* Delete at tail */
  else if ( prev_link_ptr->next_ptr == NULL )
  {
    return;
  }
  /* Delete at tail */
  else if ( prev_link_ptr->next_ptr->next_ptr == NULL )
  {
    prev_link_ptr->next_ptr = NULL;
    q_ptr->link.prev_ptr = prev_link_ptr;
  }
  /* Delete in the middle */
  else
  {
    prev_link_ptr->next_ptr = prev_link_ptr->next_ptr->next_ptr;
  }
  q_ptr->cnt--;

} /* lte_lockless_q_delete() */

/*===========================================================================

  FUNCTION:  lte_lockless_q_link

===========================================================================*/
/*!
    @brief
    link the second link item behind the first.

    @detail
    This is a utility function that does not need a q_type as input param. it
    is merely a utility function that links two items. It does not maintain
    any q_type related internal data.

    @return
    None
*/
/*=========================================================================*/
static inline void lte_lockless_q_link
(
  q_link_type               *first_link_ptr, /*!<Insert this item          */
  q_link_type               *second_link_ptr  /*!<Insert after this item    */
)
{
  first_link_ptr->next_ptr = second_link_ptr;
} /* lte_lockless_q_link() */

/*===========================================================================

  FUNCTION:  lte_lockless_q_link

===========================================================================*/
/*!
    @brief
    link the second link item behind the first.

    @detail
    This is a utility function that does not need a q_type as input param. it
    is merely a utility function that links two items. It does not maintain
    any q_type related internal data.

    @return
    None
*/
/*=========================================================================*/
/*===========================================================================
FUNCTION lte_lockless_q_m_put

DESCRIPTION
  This function enqueues all items from a specified (source) queue into a
  specified (target) queue.

DEPENDENCIES
  The specified queues should have been previously initialized via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  The items in the specified (source) queue are placed at the tail of the
  specified (target) queue.
===========================================================================*/
static inline void lte_lockless_q_m_put
(
   q_type       *target_q_ptr,    /* Ptr to target queue. */
   q_type       *source_q_ptr     /* Ptr to source queue. */
)
{
  /* Head and Tail items of source queue */
  q_link_type *source_head_item, *source_tail_item;

  if (lte_lockless_q_cnt( source_q_ptr ) == 0)
  {
    return;
  }
  /* Get tail item of source queue */
  source_tail_item = (q_link_type*)lte_lockless_q_last_check( source_q_ptr );

  /* Get head item of source queue */
  source_head_item = (q_link_type*)lte_lockless_q_check( source_q_ptr );

  if ( target_q_ptr->cnt != 0 )
  {
    target_q_ptr->link.prev_ptr->next_ptr = source_head_item;
  }
  else
  {
    target_q_ptr->link.next_ptr = source_head_item;
  }

  target_q_ptr->link.prev_ptr           = source_tail_item;
  source_tail_item->next_ptr = NULL;

  target_q_ptr->cnt += source_q_ptr->cnt;

  lte_lockless_q_init( source_q_ptr );

} /* lte_lockless_q_m_put() */

#endif /* LTE_LOCKLESS_Q_H */
