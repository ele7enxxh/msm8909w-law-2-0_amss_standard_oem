/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I T   Q U E U E    S E R V I C E S

GENERAL DESCRIPTION

  A hit_queue is a simple data structure used for logically storing and re-
  trieving data blocks, in a first in - first out (FIFO) order without
  physically copying them.

          +-------+     +-------+     +-------+
          | DATA  |     | DATA  |     | DATA  |
          | BLOCK |     | BLOCK |     | BLOCK |
          +-------+     +-------+     +-------+
   |----->| LINK  |---->| LINK  |---->| LINK  |---- |
   |      +-------+     +-------+     +-------+     |
   |                                                |
   |------------------------------------------------|

  The Queue Services provide a small set of declarations and functions for
  placing a data block at the tail of a queue and removing a data block from
  the head of a queue.

  The HIT Queue Services impose no restrictions on the size of structure of 
  data blocks used with with queues. This allows software to pass virtually 
  any type of data on queues.

EXTERNALIZED FUNCTIONS
  hitqueue_init
  hitqueue_put
  hitqueue_get

REGIONAL FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to use, a call to hitqueue_init should be made.

  Copyright (c) 2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                              Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitqueue.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
01/15/2013   smd     Featurized hit cmd and hit diag.
05/07/2007   rmg     Changed dynamic memory allocation to static one
03/28/2007   vh      Created

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_HIT
#ifdef FEATURE_HIT_DIAG_AND_CMD
#include "comdef.h"
#include "rex.h"
#include "hitqueue.h"
#include "msg.h"
#include "msgcfg.h"

/*==========================================================================
FUNCTION hitqueue_init

DESCRIPTION
  This procedure initializes a HIT queue

DEPENDENCIES
  None

PARAMETERS
  q_ptr  - pointer to the queue that is to be intialized
  q_base - pointer to allocated memory for queue
  q_size - queue size

RETURN VALUE
  TRUE  - if queue is successfully initialized
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hitqueue_init
(
  hitqueue_type *q_ptr, /* pointer to the queue that is to be intialized */
  hitqueue_element_type *q_base, /* pointer to allocated memory for queue */
  uint8 q_size /* queue size */
)
{
  uint8 i; /* loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( q_ptr != NULL )
  {
    q_ptr->q_base_ptr = q_base;
    q_ptr->q_get_idx  = 0;
    q_ptr->q_put_idx  = 0;
    q_ptr->q_size     = q_size;

    for (i=0; i<q_size; i++)
    {
      *(q_ptr->q_base_ptr + i) = NULL;
    }

    rex_init_crit_sect( &( q_ptr->q_crit_sect ) );   

    return TRUE;
  }

  /* if ( q_ptr == NULL ) */
  return FALSE;

} /* hitqueue_init() */


/*===========================================================================
FUNCTION hitqueue_put

DESCRIPTION
  This procedure puts a new element in queue

DEPENDENCIES
  None

PARAMETERS
  q_ptr - pointer to the queue to which the elem is to be appended
  elem  - element to be queued

RETURN VALUE
  TRUE  - if the element is successfully queued
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hitqueue_put
(
  hitqueue_type *q_ptr,
    /* pointer to the queue to which the elem to be appended */

  hitqueue_element_type elem
    /* elem to be queued */
)
{
  hitqueue_element_type *q_put_loc;
  /* memory location in queue where the new elem will be put */

  boolean q_error = FALSE;
  /* queue error flag */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((q_ptr != NULL) && (elem != NULL))
  {
    /* Enter critical section */
    rex_enter_crit_sect( &(q_ptr->q_crit_sect) );

    q_put_loc = q_ptr->q_base_ptr + q_ptr->q_put_idx;
    
    /* Check if the queue is not full */
    if (*(q_put_loc) == NULL)
  {
      /* Queue new element */
      *(q_put_loc) = elem;
  
      /* Increment q_put_idx */
      q_ptr->q_put_idx = (q_ptr->q_put_idx + 1) % q_ptr->q_size;

  }
  else
  {
      /* Queue full! */
      MSG( MSG_SSID_HIT, MSG_LEGACY_ERROR, "Queue full!" );

      /* Set queue error flag */
      q_error = TRUE;
  }

    /* Leave critical section */
  rex_leave_crit_sect( &(q_ptr->q_crit_sect) );

  }
  else
{
    /* Either q_ptr or elem or both are NULL */
    MSG_2( MSG_SSID_HIT, MSG_LEGACY_ERROR, 
           "Queue error: q_ptr = %d, elem = %d",
           q_ptr, elem );

    /* Set queue error flag */
    q_error = TRUE;
  }

  if (!q_error)
  {
    /* No errors */
    return TRUE;
  }
  else
  {
    /* Element not successfully queued */
    return FALSE;
  }

} /* hitqueue_put */


/*===========================================================================
FUNCTION HITQUEUE_GET

DESCRIPTION
  This procedure returns the first (least recent) element from the queue

DEPENDENCIES
  None

PARAMETERS
  q_ptr    - pointer to the queue from which the element is requested
  elem_ptr - pointer to the location where the retrieved element is to be
             copied

RETURN VALUE
  TRUE  - if the element is successfully retrieved
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hitqueue_get
(
  hitqueue_type *q_ptr,
    /* pointer to the queue from which the element is requested */

  hitqueue_element_type *elem_ptr
    /* pointer to location where the retrieved queue element is copied */
)
{
  hitqueue_element_type *q_get_loc;
  /* memory location in queue from which the element will be pulled */

  boolean q_error = FALSE;
  /* queue error flag */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((q_ptr != NULL) && (elem_ptr != NULL))
  {
    /* Enter critical section */
    rex_enter_crit_sect( &(q_ptr->q_crit_sect) );

    q_get_loc = q_ptr->q_base_ptr + q_ptr->q_get_idx;

    /* Check if the queue is not empty */
    if (*(q_get_loc) != NULL)
    {
      /* Copy element from queue */
      *(elem_ptr) = *(q_get_loc);

      /* Clear element from queue */
      *(q_get_loc) = NULL;

      /* Increment q_get_idx */
      q_ptr->q_get_idx = (q_ptr->q_get_idx + 1) % q_ptr->q_size;

    }
    else
{
      /* Queue Empty! */
      MSG( MSG_SSID_HIT, MSG_LEGACY_HIGH, "Queue empty!" );

      /* Set queue error flag */
      q_error = TRUE;
  }

    /* Leave critical section */
    rex_leave_crit_sect( &(q_ptr->q_crit_sect) );

  }
  else
  {
    /* Either q_ptr or elem_ptr or both are NULL */
    MSG_2( MSG_SSID_HIT, MSG_LEGACY_ERROR, 
           "Queue error: q_ptr = %d, elem_ptr = %d",
           q_ptr, elem_ptr );

    /* Set queue error flag */
    q_error = TRUE;
    }

  if (!q_error)
    {
    /* No errors */
    return TRUE;
  }
  else
  {
    /* Command not copied successfully */
    return FALSE;
  }

} /* hitqueue_get */

#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* FEATURE_HIT */
