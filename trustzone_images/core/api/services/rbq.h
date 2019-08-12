#ifndef RBQ_H
#define RBQ_H

/*==========================================================================

            Ring Buffer and Queue utility (RBQ - delicious!)

Description
  Type and function definitions for the RBQ utility.
  
  TODO

Copyright (c) 2003-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                          Edit History

      $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/rbq.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/09   as      Removed inclusion of customer.h 
06/21/03   lad     Created File

===========================================================================*/

#include "comdef.h"



typedef enum
{
  RBQ_EMPTY_S
}
rbq_dequeue_status_enum_type;


/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION RBQ_INIT

DESCRIPTION
  This function initializes a buffer to be managed by RBQ.
  
  After initialization, do not manipulate the buffer directly.

============================================================================*/
boolean
rbq_init (void *rbq, int size);

/*===========================================================================

FUNCTION RBQ_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.

DEPENDENCIES
  rbq_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

RETURN VALUE
  Pointer to allocated buffer.  NULL if space is not available.

SIDE EFFECTS
  rbq_head may be updated.

============================================================================*/
void *
rbq_alloc (void *rbq, int length);

/*===========================================================================

FUNCTION RBQ_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagpkt_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.

============================================================================*/
void
rbq_shorten (void *rbq, void *ptr, unsigned int length);

/*===========================================================================

FUNCTION RBQ_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  rbq_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
boolean
rbq_commit (void *rbq, void *ptr);

void *
rbq_dequeue (void *rbq, int32 *length);

boolean
rbq_dequeue_return (void *rbq, void *item, boolean free);

/*===========================================================================

FUNCTION RBQ_USED

DESCRIPTION
  This function returns the number of bytes in use for the given RBQ.

RETURN VALUE
  >=0 - # of bytes used in the RBQ
  -1 - Invalid paramter
============================================================================*/
int
rbq_used (void *rbq);

/*===========================================================================

FUNCTION RBQ_FREE

DESCRIPTION
  This function returns the number of free bytes in use for the given RBQ.

RETURN VALUE
  >=0 - # of bytes free in the RBQ
  -1 - Invalid paramter
============================================================================*/
int
rbq_free (void *rbq);

#ifdef __cplusplus
}
#endif

#endif				/* RBQ_H  */
