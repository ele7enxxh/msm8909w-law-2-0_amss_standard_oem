/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
           Q U E U E   D A T A   T Y P E   D E F I N I T I O N S

DESCRIPTION
  This file contains the externalized declarations and definitions for the 
  1x-EVDO Release B Packet Consolidation Protocol's Queue Data Type.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrpcp_queue.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/07   hal     Initial version

===========================================================================*/

#ifndef HDRPCP_QUEUE_H
#define HDRPCP_QUEUE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "comdef.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------
  Data structures and types
-----------------------------------------------------*/

/*
 * hdrpcp_queue_item_type
 * 
 * The queue item type is singly linked with 
 * next pointers. 
 *
 */
typedef struct hdrpcp_queue_item_type
{
  struct hdrpcp_queue_item_type *next;
  void *data;

} hdrpcp_queue_item_type;

/*
 * hdrpcp_queue_type
 * 
 * The queue type has a head, tail and a position
 * pointer; the position pointer is used by the
 * queue_walk() function which iterates through
 * the queue.
 *
 */
typedef struct
{
  hdrpcp_queue_item_type *head;
  hdrpcp_queue_item_type *tail;
  hdrpcp_queue_item_type **position;
  uint32 count;

} hdrpcp_queue_type;

/*
 * hdrpcp_queue_walk_enum_type
 *
 * The type of "walk" to perform; either
 *   a.) BEGIN at the head of the queue,
 *   b.) CONTINUE from the middle of the queue, or
 *   c.) END at the tail of the queue 
 *
 */
typedef enum
{
  HDRPCP_QUEUE_BEGIN = 0,
  HDRPCP_QUEUE_CONTINUE,
  HDRPCP_QUEUE_END

} hdrpcp_queue_walk_enum_type;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

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
extern void hdrpcp_queue_init
(
  hdrpcp_queue_type *q_ptr
);

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
extern hdrpcp_queue_item_type *hdrpcp_queue_walk
(
  hdrpcp_queue_type *q_ptr,
  hdrpcp_queue_walk_enum_type walk_type
);

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
extern void hdrpcp_queue_put_item
(
  hdrpcp_queue_type *q_ptr,
  hdrpcp_queue_item_type *q_item_ptr
);

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
extern hdrpcp_queue_item_type *hdrpcp_queue_del_item
(
  hdrpcp_queue_type *q_ptr,
  void *data
);

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
extern hdrpcp_queue_item_type *hdrpcp_queue_get_item
(
  hdrpcp_queue_type *q_ptr
);

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
extern hdrpcp_queue_item_type *hdrpcp_queue_check_front_item
(
  hdrpcp_queue_type *q_ptr
);

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
extern void hdrpcp_queue_link
(
  hdrpcp_queue_type *head_q_ptr,
  hdrpcp_queue_type *tail_q_ptr
);

#endif /* HDRPCP_QUEUE_H */
