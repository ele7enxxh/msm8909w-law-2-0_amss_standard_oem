/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            Ring Buffer and Queue utility (RBQ - delicious!)

General Description
  This file implements a utility to manage a buffer as a circulr buffer/queue.
  
  Each allocation is treated as a separate queue entry.  Enqueing occurs at
  time of allocation.  
  
Initialization and Sequencing Requirements
  rbq_init() must be called to initialize a given buffer.

Copyright (c) 2003-2010, 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/rtos/src/rbq.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
08/25/10   sg      Fixed compiler warnings
06/29/10   vs      Removed INTLOCK in the file
12/23/09   vg      Fixed alignment issue when PACK macro was removed. 
10/11/09   vg      8200 Warning Clean Up
10/03/08   vg      Updated code to use PACK() vs. PACKED
10/31/05   as      Fixed lint errors.
06/23/03   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" 
#else
#include "assert.h"
#endif 
#include "err.h"

#include "rbq.h"

#include "osal.h"
#include "msg.h"

osal_mutex_arg_t         rbq_mutex;

/*---------------------------------------------------------------------------
  Terminology
  
  Buffer - The linear blk of memory to be managed by this utility.
  Size - The size, in bytes, of the buffer.
  Item - A memory blk allocated using this utility.
  Length - Number of bytes of an allocated item.
  
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  
  The following are indices for the ring buffer used for streaming diag
  output from the DMSS.  Each packet will be stored in the following format.
  Numbers are expressed as bytes.

  -----------------------------
  |                           |
  |  Header (4)               |  <-- Status, length of spzce used (not of packet)
  |                           |
  -----------------------------
  |                           |
  |                           |
  |  Output Data (Length - 4) |  <-- Buffer given to caller (ptr to this)
  |                           |
  |                           |
  -----------------------------
  |                           |
  |  CRC (2)                  |  <-- CRC required for transport protocol
  |                           |
  -----------------------------

  //Rajeev: If CRC calc is still in there because it is used for overrun detection
---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  
  The following variables are used to manage each buffer as a circular 
  buffer/queue.
  
  Each buffer uses a a linear buffer of size 2*N where N is the size of the
  buffer.  The index to the buffer = idx % N.

  tail (T) - The end of the used buffer.

  head (H) - The front of buffer (where writers allocate)

  With this system, the following calculations hold true:

  Empty condition:        H == T
  Full condition:         H - T == N
  Amnt used:              H - T

----------------------------------------------------------------------------*/


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*===========================================================================

MACRO RBQ_EMPTY
MACRO RBQ_FULL

DESCRIPTION
  Conditional macros for diag streaming buffer.

RETURN VALUE
  Conditional values.

===========================================================================*/
#define RBQ_EMPTY(h, t) (h == t)




/*===========================================================================

MACRO RBQ_USED
MACRO RBQ_FREE

DESCRIPTION
  Status calculating macros for the diag ring buffer.

PARAMETERS
  h - head index
  t - tail index

RETURN VALUE
  Number of bytes.

===========================================================================*/
#define RBQ_USED(h, t, s) ((h >= t) ? (h - (t)) : (h - (t) + 2*s))

#define RBQ_FREE(h, t, s) (s - RBQ_USED(h, t, s))



/*===========================================================================

MACRO RBQ_CALC_IDX

DESCRIPTION
  Calculates an index in RBQ buffer.

PARAMETERS
  Descriptor ptr, abstract buffer index

DEPENDENCIES
  0 <= x < (2 * desc->size)

RETURN VALUE
  Index in RBQ buffer.

===========================================================================*/
#define RBQ_CALC_IDX(x, s) ((x >= s) ? (x - s) : x)



/*===========================================================================

MACRO RBQ_WRAP

DESCRIPTION
  Wraps abstract buffer index.

PARAMETERS
  Abstract buffer index.

DEPENDENCIES
  0 <= x < (2 * RBQ_SIZE)

===========================================================================*/
#define RBQ_WRAP(x, s)         \
do {                           \
  x += s - RBQ_CALC_IDX(x, s); \
  if (x >= 2 * s)              \
    x -= 2 * s;                \
/*lint -e717 */                \
} while(0)                     \
				/*lint +e717 */



/*===========================================================================

MACRO RBQ_MOVE

DESCRIPTION
  Moves the buffer index.

DEPENDENCIES
  0 <= x < (2 * size)

===========================================================================*/
#define RBQ_MOVE(i, l, s) \
do {                      \
  i += l;                 \
  if (i >= 2 * s)         \
    i -= 2 * s;           \
/*lint -e717 */           \
} while(0)                \
				/*lint +e717 */



/*===========================================================================

MACRO RBQ_NEXT_ALIGNED_BYTE

DESCRIPTION
  Moves the variable the next aligned byte

===========================================================================*/
typedef struct
{
  uint32 pattern;		/* used to verify the buffer */
  int size;
  int head;
  int tail;

}
rbq_buf_desc_type;

typedef enum
{
  RBQ_UNCOMMIT_S,		/* Notify that the buffer is not commited    */
  RBQ_COMMIT_S,			/* Notify that the buffer is commited        */
  RBQ_DEQD_S,                   /* Item is dequeued */
  RBQ_FREE_S,                   /* Item was freed */
  RBQ_WRAP_S,			/* Block header indicicates wrap */
  RBQ_HOLE_S			/* Block is a hole */
}
rbq_status_enum_type;

typedef struct
{
  uint8 status;			/* stores the header status */
  uint8 pad;			/* stores the pad to make it byte aligned */
  uint16 length;		/* stores the length of the buffer */

}
rbq_header_type;


#define RBQ_INIT_PATTERN (0x06108626)

//Stubs from diagbuf
#define RBQ_BALANCE_PAD 0
#define nice 0

static rbq_buf_desc_type *rbq_cvt_buf2desc (void *rbq);
static boolean rbq_write_ovrn_pattern (rbq_header_type *blk_hdr);
static boolean rbq_check_ovrn_pattern (rbq_header_type *blk_hdr);

#define RBQ_NEXT_ALIGNED_BYTE(p) \
 (                               \
   (                             \
     (                           \
       (                         \
         (uint32) (p)            \
       ) + 3                     \
     ) & ~3                      \
   )                             \
 )

#define RBQ_INIT_DESC_PTR(xx_buf) \
  ((rbq_buf_desc_type *) RBQ_NEXT_ALIGNED_BYTE (xx_buf))



void rbq_mutex_init(void)
{
	int return_val = 0;

	return_val = osal_init_mutex(&rbq_mutex);
	ASSERT(OSAL_SUCCESS == return_val);
}

/*=========================================================================*/
boolean
rbq_init (void *rbq, int size)
{
  static boolean first_rbq_init = TRUE;

  rbq_buf_desc_type *desc = RBQ_INIT_DESC_PTR (rbq);
  boolean success = FALSE;

  if (first_rbq_init)
  {
	  first_rbq_init = FALSE;
	  rbq_mutex_init();	  
  }

  if (desc && (uint32)size >= sizeof (*desc))
  {
    desc->pattern = RBQ_INIT_PATTERN;
    desc->size = (int)((uint32)size - sizeof (*desc));
    desc->head = 0;
    desc->tail = 0;

    memset ((void *) (desc + 1), 0xAA, (uint32)size - sizeof (*desc));

    success = TRUE;
  }

  return success;
}				/* rbq_init */

/*===========================================================================

FUNCTION RBQ_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag
  output buffer.

DEPENDENCIES
  rbq_commit(), or diagbuf_shorten(0), must be called to commit/return an
  item to the RBQ system.

============================================================================*/
void *
rbq_alloc (void *rbq, int length)	//, int nice)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);
  int size;
  uint8 *buf = NULL;
  int blk_length;		//includes overhead
  rbq_header_type *blk_hdr = NULL;
  void *item = NULL;

  /* Local indices to manipulate buffer */
  int iHead;
  int iHeadIdx;
  int iTail;
  int iTailIdx;

  if (desc)
  {
    size = desc->size;
    buf = (uint8 *) (desc + 1);

    blk_length = (int)(sizeof (rbq_header_type) + (uint32)length + sizeof (uint16));

    blk_length = (int)RBQ_NEXT_ALIGNED_BYTE(blk_length);

    /*-------------------------------------------------
      To avoid critical section for tail, read once
      and use that value.  This operation is atomic.
    -------------------------------------------------*/
    iTail = desc->tail;
    iTailIdx = RBQ_CALC_IDX (iTail, size);

    osal_lock_mutex(&rbq_mutex);

    iHead = desc->head;
    iHeadIdx = RBQ_CALC_IDX (iHead, size);

    blk_hdr = ((rbq_header_type *) & buf[iHeadIdx]);

    /* First check if the nice pad is available.  If OK, then alloc */
    if (RBQ_BALANCE_PAD + nice < RBQ_FREE (iHead, iTail, size))
    {
      /* if not wrapped and no room here. */
      if (iTailIdx <= iHeadIdx &&	/* Not wrapped &&  */
	  iHead - iTail < size &&	/* Not full &&     */
	  /* No room here!   */
	  iHeadIdx + blk_length >= size)
      {

	/* Mark header for wrap */
	blk_hdr->status = RBQ_WRAP_S;

	/* Wrap */
	RBQ_WRAP (iHead, size);

	/* Recalculate head index */
	iHeadIdx = RBQ_CALC_IDX (iHead, size);
      }

      if (blk_length <= RBQ_FREE (iHead, iTail, size))
      {
	/* Move head */
	RBQ_MOVE (iHead, blk_length, size);

	/* Update value of global head index */
	desc->head = iHead;

	/* Sanity check */
	if (RBQ_USED (iHead, iTail, size) > size)
	{
	  ERR_FATAL ("Ring buffer currupt!", 0, 0, 0);
	}

	/* Set header field of buffer */
	blk_hdr = ((rbq_header_type *) & buf[iHeadIdx]);

	blk_hdr->status = RBQ_UNCOMMIT_S;
	blk_hdr->pad = (uint8) ((uint32)blk_length - (uint32)length -
		sizeof (rbq_header_type));
	blk_hdr->length = (uint16)length;

	/* Set 'ptr' to the byte following the header */
	item = blk_hdr + 1;
      }
    }				/* If requested length + nice pad is free */

    osal_unlock_mutex(&rbq_mutex);

    if (item)
    {
      (void) rbq_write_ovrn_pattern (blk_hdr);
    }

  }
  return item;
}				/* rbq_alloc */

/*==========================================================================*/
void
rbq_shorten (void *rbq, void *item, unsigned int new_length)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);
  rbq_header_type *blk_hdr;	/* Points to packet header */
  rbq_header_type *hole;	/* This will point to new header field. */

  unsigned int curr_length;
  unsigned int hole_length;

  if (!desc)
  {
    return;
  }

#if 0				//XXX
  /* Sanity check.  Make sure it is within boundaries of ring buffer */
  if ((byte *) item < rbq_buf ||
      (byte *) item >= (&rbq_buf[RBQ_SIZE] - sizeof (uint16)))
  {
    /* This is bad.  Incorrect usage. Report error and do nothing. */
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid packet address: item:0x%x, l:%d",
                                            item, new_length);
    return;
  }
#endif

  /* Cast pointer to do arithmetic with rbq_header_type */
  blk_hdr = (rbq_header_type *) item;

  /* Subtract 1 rbq_header_type (4 bytes) to get header */
  blk_hdr -= 1;

  curr_length = blk_hdr->length;

  if (new_length == 0)
  {
    /* Special case: shorten to 0.  The entire blk is just now a hole. */
    hole = blk_hdr;

    /* length of the hole is current length + CRC + pad */
    hole->length = (uint16) RBQ_NEXT_ALIGNED_BYTE (curr_length + sizeof (uint16));

    hole->status = RBQ_HOLE_S;
    hole->pad = 0;
  }
  else
  {
    /* If new length is longer or the same, we do nothing. */
    if (curr_length > new_length)
    {
      /* Compute hole size and record new packet length */

      blk_hdr->length = (uint16)new_length;

      /* Adjust length fields to include CRC fields */
      new_length += sizeof (uint16);
      curr_length += sizeof (uint16);

      /* Calcualte pad for the packet */
      blk_hdr->pad = (uint8) 
        (RBQ_NEXT_ALIGNED_BYTE (new_length) - new_length);

      /* Now change length fields to be the length of entire buffer used */
      new_length += blk_hdr->pad;
      curr_length = RBQ_NEXT_ALIGNED_BYTE (curr_length);

      hole = (rbq_header_type *) item;

      /* Move to next aligned address past the new packet length */
      hole += new_length / sizeof (*hole);

      hole_length = curr_length - new_length;

      if (hole_length >= sizeof (rbq_header_type))
      {
	hole->status = RBQ_HOLE_S;
	hole->length = (uint16) (hole_length - sizeof (rbq_header_type));
	hole->pad = 0;
      }

      (void) rbq_write_ovrn_pattern (blk_hdr);

    }
    else if (new_length > curr_length)
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempted shorten %d to %d ignored.",
           curr_length, new_length);
    }
    else
    {
      /* If shortening to same size, do nothing. */
    }
  }
}				/* rbq_shorten */

/*===========================================================================

FUNCTION RBQ_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'item' must be the address that was returned by a prior call to
  rbq_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
boolean
rbq_commit (void *rbq, void *item)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);
  uint8 *buf = NULL;
  rbq_header_type *blk_hdr;	/* Pointer to the header of the buffer. */
  boolean success = FALSE;

  /* Cast pointer to do arithmetic with rbq_header_type */
  blk_hdr = (rbq_header_type *) item;

  /* Subtract 1 rbq_header_type to get header field */
  blk_hdr -= 1;

  /* Sanity checks */
  if (desc && item)
  {
    buf = (uint8 *) (desc + 1);

    if ((uint8 *) blk_hdr >= buf && 
        (uint8 *) blk_hdr <= &buf[(uint32)desc->size - sizeof (*blk_hdr)])
    {

      switch (blk_hdr->status)
      {
        case RBQ_UNCOMMIT_S:
          success = TRUE;

          if (rbq_check_ovrn_pattern (blk_hdr))
          {
            /* Set status to "committed" */
            blk_hdr->status = RBQ_COMMIT_S;
          }
          else
          {
            //TODO
          }
          break;

        default:
          break;
      }
    }
  }
  
  return success;
}				/* rbq_commit */

/*=========================================================================*/
void *
rbq_dequeue (void *rbq, int32 *length)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);
  int size;
  uint8 *buf = NULL;
  rbq_header_type *blk_hdr;	/* Pointer to the header of the buffer. */
  void *item = NULL;
  int iTailIdx;			/* Byte array index to tail of diag ring buffer */
  rbq_status_enum_type status;
  boolean found = FALSE;

  buf = (uint8 *)(desc + 1);

  if (desc)
  {
    size = desc->size;

    osal_lock_mutex(&rbq_mutex);

    /* Get the index of the ring buffer's tail */
    iTailIdx = RBQ_CALC_IDX (desc->tail, size);
    
    /* Walk down the tail until action can be determined. */
    do
    {
      blk_hdr = (rbq_header_type *) (buf + iTailIdx);

      status = (rbq_status_enum_type)blk_hdr->status;

      if (RBQ_EMPTY (desc->head, desc->tail) ||
          status == RBQ_UNCOMMIT_S)
      {
        break;
      }
      else if (status == RBQ_COMMIT_S)
      {
        found = TRUE;
        blk_hdr->status = RBQ_DEQD_S;
      }
      else if (status == RBQ_WRAP_S)
      {
        RBQ_WRAP (desc->tail, size);
        iTailIdx = RBQ_CALC_IDX (desc->tail, size);
        continue;
      }
      else if (status == RBQ_HOLE_S)
      {
        blk_hdr->status = RBQ_FREE_S;

        RBQ_MOVE (desc->tail, 
          (int)(blk_hdr->length + sizeof (*blk_hdr) + blk_hdr->pad), size);

        iTailIdx = RBQ_CALC_IDX (desc->tail, size);
        continue;
      }
      else if (status == RBQ_DEQD_S)
      {
        //Item already dequeued.  Return failed.
        item = NULL;
        break;
      }

      //If uncommitted, return

    } while (!found);

    osal_unlock_mutex(&rbq_mutex);

    if (found)
    {
      item = (void *) (blk_hdr + 1);

      if (length)
      {
        *length = blk_hdr->length;
      }
    }
  }				/* if start of new packet */

  return item;
}

/*=========================================================================*/
boolean
rbq_dequeue_return (void *rbq, void *item, boolean free)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);
  uint8 *buf = NULL;
  rbq_header_type *blk_hdr;
  int iTailIdx;
  int size;
  int blk_length;
  boolean success = FALSE;

  buf = (uint8 *)(desc + 1);

  if (item && desc)
  {
    size = desc->size;

    /* Cast pointer to do arithmetic with rbq_header_type */
    blk_hdr = (rbq_header_type *) item;

    /* Subtract 1 rbq_header_type to get header field */
    blk_hdr -= 1;

    blk_length = blk_hdr->length + sizeof (*blk_hdr) + blk_hdr->pad;

    osal_lock_mutex(&rbq_mutex);

    ASSERT (blk_hdr->status == RBQ_DEQD_S);

    if (!free)
    {
      //Return to the commit state.
      blk_hdr->status = RBQ_COMMIT_S;
    }
    else
    {
      /* Get the index of the ring buffer's tail */
      iTailIdx = RBQ_CALC_IDX (desc->tail, size);

      if (&buf[iTailIdx] == (uint8 *) blk_hdr)
      {
        /* Move the tail. */
        RBQ_MOVE (desc->tail, blk_length, size);

        success = TRUE;

        /* Sanity check */
        if (RBQ_USED (desc->head, desc->tail, size) > size)
        {
          ERR_FATAL ("RBQ corrupt!", 0, 0, 0);
        }
      }
      else
      {
        ERR_FATAL ("TODO", 0, 0, 0);
      }
    }

    osal_unlock_mutex(&rbq_mutex);
 }

  return success;
}

/*=========================================================================*/
int
rbq_used (void *rbq)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);

  if (desc)
  {
    return RBQ_USED (desc->head, desc->tail, desc->size);
  }

  return -1;
}

/*=========================================================================*/
int
rbq_free (void *rbq)
{
  rbq_buf_desc_type *desc = rbq_cvt_buf2desc (rbq);

  if (desc)
  {
    return RBQ_FREE (desc->head, desc->tail, desc->size);
  }

  return -1;
}

/* Private functions */

/*===========================================================================

FUNCTION RBQ_CVT_BUF2DESC

DESCRIPTION
  This routine coverts the caller's buffer pointer to an internal buffer
  descriptor pointer.  It also verifies the buffer has been initialized.

===========================================================================*/
static rbq_buf_desc_type *
rbq_cvt_buf2desc (void *rbq)
{
  rbq_buf_desc_type *desc = RBQ_INIT_DESC_PTR (rbq);

  if (rbq && desc)
  {
    if (desc->pattern != RBQ_INIT_PATTERN)
    {
      desc = NULL;
    }
  }
  else
  {
    desc = NULL;
  }

  return desc;
}

/*===========================================================================

FUNCTION RBQ_WRITE_OVRN_PATTERN

DESCRIPTION
  Writes the ovrn check pattern in the CRC field at the end of the bufer
  pointed to by 'item'.  rbq_commit() checks this field to determine if
  buffer ovrn has occured.

  The work to check this is done in rbq_commit() rather than a seperate
  function to save the cost of calculations and the function call.

  'item' points to the length field before the packet, not the beginning of
  the packet itself.

===========================================================================*/
static boolean
rbq_write_ovrn_pattern (rbq_header_type * blk_hdr)
{
  uint8 *pattern = NULL;
  boolean success = FALSE;

  if (blk_hdr)
  {
    pattern = (uint8 *) (((uint8 *) blk_hdr) + blk_hdr->length 
                        + sizeof (*blk_hdr));

    *pattern = 0xDE;
    *(pattern+1) = 0xAD;

    success = TRUE;
  }

  return success;
}				/* rbq_write_ovrn_pattern */

static boolean
rbq_check_ovrn_pattern (rbq_header_type *blk_hdr)
{
  uint8 * pattern = NULL;
  boolean success = FALSE;

  if (blk_hdr)
  {
    pattern = (uint8 *) (((uint8 *) blk_hdr) + blk_hdr->length +
                sizeof (rbq_header_type));
    //TODO: Allow custom FPs to handle ovrns and other failures.
    if (*pattern == 0xDE &&  *(pattern+1) == 0xAD)
    {
      success = TRUE;
    }
    else
    {
      ERR_FATAL ("Buffer Overrun", 0, 0, 0);
    }

  }

  return success;
}



