#ifndef _HITQUEUE_H
#define _HITQUEUE_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H I T   Q U E U E    S E R V I C E S    H E A D E R    F I L E

GENERAL DESCRIPTION
 This file contains types and declarations associated with the HIT Queue
 Services.

EXTERNALIZED FUNCTIONS
    hitqueue_init
    hitqueue_put
    hitqueue_get
    
REGIONAL FUNCTIONS
    None
    
  Copyright (c) 2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitqueue.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
01/15/2013   smd     Featurized hit cmd and hit diag.
05/07/2007   rmg     Changed dynamic memory allocation to static one
03/28/2007   vh      Created

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "rex.h"
#ifdef FEATURE_HIT_DIAG_AND_CMD
/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

typedef void *hitqueue_element_type;

typedef struct hitqueue_struct
{
  hitqueue_element_type *q_base_ptr;
    /* pointer to the allocated memory for queue */

  uint8 q_get_idx;
    /* index of next command to fetch from queue */

  uint8 q_put_idx;
    /* index of next command to queue-up in queue */

  uint8 q_size;
    /* queue size */

  rex_crit_sect_type q_crit_sect;
    /* rex critical section for this queue */

} hitqueue_type;


/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


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
);


/*===========================================================================
FUNCTION hitqueue_put

DESCRIPTION
  This procedure puts a new element in queue

DEPENDENCIES
  None

PARAMETERS
  q_ptr - pointer to the queue to which the element is to be appended
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
    /* pointer to the queue to which the element to be appended */

  hitqueue_element_type elem
    /* elem to be queued */
);


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
);
#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* _HITQUEUE_H */
