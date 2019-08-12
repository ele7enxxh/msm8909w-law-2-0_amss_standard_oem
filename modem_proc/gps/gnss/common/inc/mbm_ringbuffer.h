#ifndef __MBM_RINGBUFFER_H__
#define __MBM_RINGBUFFER_H__
/*============================================================================
  @file mbm_ringbuffer.h

                  Measurement Buffer Module (MBM) Ring buffer Module 

GENERAL DESCRIPTION

  This file implements generic ring buffer for MBM. The central idea of the
  implementation is to provide only mbm_RingBufferInfoType (and functions
  to modify it), which consists of only ring buffer length and indices, but
  not the buffered data itself. This enables us to manage the ringbuffer
  indices separately from one or more buffers for the data.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/mbm_ringbuffer.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   th      Initial version

=============================================================================*/

/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "customer.h"
#include "comdef.h"
#include "gps_types.h"

/* ---------------------------------------------------------------------------
 * Structures
 * ---------------------------------------------------------------------------*/
typedef struct
{
  /* Size of the buffer. Maximum number of items that can be buffered. */
  uint32 q_BufferSize;

  /* ReadIndex is where we can read from if buffer is not empty */
  uint32 q_ReadIndex;

  /* WriteIndex is where we can write if buffer is not full */
  uint32 q_WriteIndex;

  /* How many items out of BufferSize are used. */
  uint32 q_UsedItems;
} mbm_RingBufferInfoType;


/* ---------------------------------------------------------------------------
 * Public Functions
 * ---------------------------------------------------------------------------*/


/*=============================================================================

  FUNCTION      mbm_RingBufferInit()

  DESCRIPTION   Initialize MBM ring buffer

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info
                q_Size - Ring buffer size

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void mbm_RingBufferInit(mbm_RingBufferInfoType *pz_Info, uint32 q_Size);

/*=============================================================================

  FUNCTION      mbm_RingBufferIsEmpty

  DESCRIPTION   Determines whether ring buffer is empty or not

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  TRUE if buffer is empty, otherwise FALSE

  SIDE EFFECTS  None

=============================================================================*/
boolean mbm_RingBufferIsEmpty(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferIsFull

  DESCRIPTION   Determines whether ring buffer is full or not

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  TRUE if buffer is full, otherwise FALSE

  SIDE EFFECTS  None

=============================================================================*/
boolean mbm_RingBufferIsFull(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferIsValidIndex

  DESCRIPTION   Determines whether index is to a valid item in the buffer

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info
                l_Index - Index to ring buffer

  RETURN VALUE  TRUE if l_Index is to a valid item, otherwise FALSE

  SIDE EFFECTS  None

=============================================================================*/
boolean mbm_RingBufferIsValidIndex(const mbm_RingBufferInfoType *pz_Info, int32 l_Index);

/*=============================================================================

  FUNCTION      mbm_RingBufferUsedItems

  DESCRIPTION   Returns number of used ring buffer items

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Number of used ring buffer items

  SIDE EFFECTS  None

=============================================================================*/
uint32 mbm_RingBufferUsedItems(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferAvailItems

  DESCRIPTION   Returns number of available ring buffer items

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Number of available ring buffer items

  SIDE EFFECTS  None

=============================================================================*/
uint32 mbm_RingBufferAvailItems(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferTailIndex

  DESCRIPTION   Returns index to ring buffer tail, i.e. where we wrote first.
                Buffer tail is where reading is done.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Index to ring buffer tail. Negative if buffer is empty.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferTailIndex(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferHeadIndex

  DESCRIPTION   Returns index to ring buffer head, i.ke. where we wrote last.
                Buffer head is where writing is done.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Index to ring buffer head. Negative if buffer is empty.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferHeadIndex(const mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferNextIndex

  DESCRIPTION   Returns next index in ring buffer. The direction is from tail
                to head.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info
                l_Index - Current index

  RETURN VALUE  Next index that is closer to head. Negative if current index
                is head, or if current index is invalid.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferNextIndex(const mbm_RingBufferInfoType *pz_Info, int32 l_Index);

/*=============================================================================

  FUNCTION      mbm_RingBufferPrevIndex

  DESCRIPTION   Returns previous index in ring buffer. The direction is from
                head to tail.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info
                l_Index - Current index

  RETURN VALUE  Previous index that is closer to tail. Negative if current
                index is tail, or if current index is invalid.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferPrevIndex(const mbm_RingBufferInfoType *pz_Info, int32 l_Index);

/*=============================================================================

  FUNCTION      mbm_RingBufferWrite

  DESCRIPTION   Writes to ring buffer. The function does not write the data
                itself, but returns index to item where data can be written,
                and advances internal indices accordingly. The default behavior
                of this function is NOT to overwrite oldest data when buffer
                becomes full. Instead, user should explicitly check if the
                buffer is full and discard an item by reading the buffer
                to make room for new item.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Index to item that can be written to. Negative if buffer
                was full.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferWrite(mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferRead

  DESCRIPTION   Reads from ring buffer. The function does not read the data
                itself, but returns index to item that was read and advances
                internal indices accordingly.

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  Index to item that was read from the buffer. Negative if
                ring buffer was empty.

  SIDE EFFECTS  None

=============================================================================*/
int32 mbm_RingBufferRead(mbm_RingBufferInfoType *pz_Info);

/*=============================================================================

  FUNCTION      mbm_RingBufferFlush

  DESCRIPTION   Flushes ring buffer

  DEPENDENCIES  None

  PARAMETERS    pz_Info - Pointer to ring buffer info

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void mbm_RingBufferFlush(mbm_RingBufferInfoType *pz_Info);

#endif
