/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
                 Q U E U E   D A T A   T Y P E   M O D U L E

DESCRIPTION
  This file contains the implementation of the 1x-EVDO Release B Packet 
  Consolidation Protocol's Queue Data Type.

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrpcp_queue.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/07   hal     Updated hdrpcp_queue_link() function from code review comments
08/06/07   hal     Fixed hdrpcp_queue_link() function when head queue is empty
06/27/07   hal     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "comdef.h"


/*-----------------------------------------------------
  HDR specific include files
-----------------------------------------------------*/

#include "hdrpcp_queue.h"



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRPCP_QUEUE_INIT

DESCRIPTION
  This function initializes a queue type.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr  - A pointer to the queue to be initialized

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_queue_init
(
  hdrpcp_queue_type *q_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( q_ptr )
  {
    q_ptr->head = NULL;
    q_ptr->tail = NULL;
    q_ptr->position = &q_ptr->head;
    q_ptr->count = 0;
  }

} /* hdrpcp_queue_init() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_WALK

DESCRIPTION
  This function "walks", or iterates through a queue.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr      - A pointer to the queue
  walk_type  - The style of iteration; BEGIN, CONTINUE or END

RETURN VALUE
  hdrpcp_queue_item_type *  - A pointer to the queue item at the 
                              iterator position

SIDE EFFECTS
  None.

===========================================================================*/
hdrpcp_queue_item_type *hdrpcp_queue_walk
(
  hdrpcp_queue_type *q_ptr,
  hdrpcp_queue_walk_enum_type walk_type
)
{
  hdrpcp_queue_item_type *q_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( q_ptr )
  {
    switch( walk_type )
    {
      case HDRPCP_QUEUE_BEGIN:
        q_item_ptr = q_ptr->head;
        break;
  
      case HDRPCP_QUEUE_CONTINUE:
        q_item_ptr = *(q_ptr->position);
        break;
  
      case HDRPCP_QUEUE_END:
        q_item_ptr = q_ptr->tail;
        break;
  
      default:
        break;
    }
  
    /* Only continue if the link is non-NULL */
    if( q_item_ptr )
    {
      q_ptr->position = &(q_item_ptr->next);
    }
  }

  return q_item_ptr;

} /* hdrpcp_queue_walk() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_PUT_ITEM

DESCRIPTION
  This function enqueues an item at the end of a queue.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr       - A pointer to the queue
  q_item_ptr  - A pointer to the item to be enqueued

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_queue_put_item
(
  hdrpcp_queue_type *q_ptr,
  hdrpcp_queue_item_type *q_item_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( q_ptr && q_item_ptr )
  {
    /* If the queue is empty, set the item to be both head and tail */
    if( q_ptr->head == NULL )
    {
      q_ptr->head = q_item_ptr;
      q_ptr->tail = q_item_ptr;
    }
    else /* otherwise link the item to the tail */
    {
      q_ptr->tail->next = q_item_ptr;
      q_ptr->tail = q_item_ptr;
    }

    q_ptr->count++;
  }

} /* hdrpcp_queue_put_item() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_DEL_ITEM

DESCRIPTION
  This function deletes an item from the queue while maintaining the
  connectivity of the queue.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr  - A pointer to the queue
  data   - A void pointer to be compared against the items in the queue

RETURN VALUE
  hdrpcp_queue_item_type *  - A pointer to the queue item that was deleted

SIDE EFFECTS
  None.

===========================================================================*/
hdrpcp_queue_item_type *hdrpcp_queue_del_item
(
  hdrpcp_queue_type *q_ptr,
  void *data
)
{
  hdrpcp_queue_item_type *q_item_ptr = NULL;
  hdrpcp_queue_item_type *q_item_left_ptr = NULL;
  hdrpcp_queue_item_type *q_item_right_ptr = NULL;

  boolean found_item_data = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start with the first item in the queue */
  q_item_ptr = hdrpcp_queue_walk( q_ptr, HDRPCP_QUEUE_BEGIN );

  while( q_item_ptr )
  {
    /* If the item is found we need to remove it and connect the links */
    if( q_item_ptr->data == data )
    {
      q_item_right_ptr = q_item_ptr->next;

      found_item_data = TRUE;
      break;
    }
    else
    {
      q_item_left_ptr = q_item_ptr;
      q_item_ptr = 
        hdrpcp_queue_walk( q_ptr, HDRPCP_QUEUE_CONTINUE );
    }
  }

  if( found_item_data )
  {
    /* If there are items to the left and right */
    if( q_item_left_ptr && q_item_right_ptr )
    {
      q_item_left_ptr->next = q_item_right_ptr;
    }
    else if( q_item_left_ptr ) /* or if only the left has an item */
    {
      q_item_left_ptr->next = NULL;
      q_ptr->tail = q_item_left_ptr;
    }
    else if( q_item_right_ptr ) /* or if only the right has an item */
    {
      q_ptr->head = q_item_right_ptr;
    }
    else /* otherwise the queue is empty */
    {
      q_ptr->head = NULL;
      q_ptr->tail = NULL;
    }

    q_ptr->count--;
  }

  return q_item_ptr;

} /* hdrpcp_queue_del_item() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_GET_ITEM

DESCRIPTION
  This function dequeues an item from the head of the queue.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr  - A pointer to the queue

RETURN VALUE
  hdrpcp_queue_item_type *  - A pointer to the queue item that was dequeued

SIDE EFFECTS
  None.

===========================================================================*/
hdrpcp_queue_item_type *hdrpcp_queue_get_item
(
  hdrpcp_queue_type *q_ptr
)
{
  hdrpcp_queue_item_type *item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( q_ptr && q_ptr->head )
  {
    /* Grab the first item in the queue */
    item_ptr = q_ptr->head;

    /* If the queue only has one item, empty the queue */
    if( q_ptr->head == q_ptr->tail )
    {
      q_ptr->head = NULL;
      q_ptr->tail = NULL;
    }
    else /* otherwise move the head pointer to the next item */
    {
      q_ptr->head = q_ptr->head->next;
    }

    /* Ensure the item is unlinked from the queue */
    item_ptr->next = NULL;
    
    q_ptr->count--;
  }
  
  return item_ptr;

} /* hdrpcp_queue_get_item() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_CHECK_FRONT_ITEM

DESCRIPTION
  This function returns the item at the head, or front, of a queue.

DEPENDENCIES
  None.

PARAMETERS
  q_ptr  - A pointer to the queue

RETURN VALUE
  hdrpcp_queue_item_type *  - A pointer to the queue item at the front of
                              the queue

SIDE EFFECTS
  None.

===========================================================================*/
hdrpcp_queue_item_type *hdrpcp_queue_check_front_item
(
  hdrpcp_queue_type *q_ptr
)
{
  hdrpcp_queue_item_type *q_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( q_ptr )
  {
    q_item_ptr = q_ptr->head;
  }

  return q_item_ptr;

} /* hdrpcp_queue_check_front_item() */

/*===========================================================================

FUNCTION HDRPCP_QUEUE_LINK

DESCRIPTION
  This function links two queues together.

DEPENDENCIES
  None.

PARAMETERS
  head_q_ptr  - A pointer to the queue that belongs in front
  tail_q_ptr  - A pointer to the queue that belongs next

RETURN VALUE
  None.

SIDE EFFECTS
  The tail_q_ptr will be emptied (its contents moved to the head_q_ptr) and 
  will no longer be a valid queue.

===========================================================================*/
void hdrpcp_queue_link
(
  hdrpcp_queue_type *head_q_ptr,
  hdrpcp_queue_type *tail_q_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( head_q_ptr && tail_q_ptr )
  {
    if( head_q_ptr->head && tail_q_ptr->head )
    {
      /* Link the last item of the new queue to the first item of the 
         tail queue */
      head_q_ptr->tail->next = tail_q_ptr->head;

      /* Set the last item of the new queue to be the last item of the
         tail queue */
      head_q_ptr->tail = tail_q_ptr->tail;

      /* Combine the count of the number items in both queues */
      head_q_ptr->count += tail_q_ptr->count;

      /* Reinitialize the tail queue pointer because it is now empty */
      hdrpcp_queue_init( tail_q_ptr );
    }
    else if( tail_q_ptr->head ) /* the head queue is empty, so replace it with the tail queue */
    {
      head_q_ptr->head = tail_q_ptr->head;
      head_q_ptr->tail = tail_q_ptr->tail;
      head_q_ptr->count = tail_q_ptr->count;

      hdrpcp_queue_init( tail_q_ptr );
    }
  }

} /* hdrpcp_queue_link() */
