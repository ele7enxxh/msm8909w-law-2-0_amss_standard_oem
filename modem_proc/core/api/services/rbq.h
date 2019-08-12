#ifndef RBQ_H
#define RBQ_H

/*!
    @ingroup diag_misc
    @file rbq.h
    @brief
 	  This file implements a utility to manage a buffer as a circular buffer/queue.
      Each allocation is treated as a separate queue entry.  Enqueing occurs at
      time of allocation. 
 
      @par Initialization and Sequencing Requirements
        rbq_init() must be called to initialize a given buffer.
 
      @par Terminology
        Buffer - The linear block of memory to be managed by this utility.
        Size - The size, in bytes, of the buffer.
        Item - A memory block allocated using this utility.
        Length - Number of bytes of an allocated item.
 
      @par
        The following are indices for the ring buffer used for streaming diag
        output from the DMSS.  Each packet will be stored in the following 
        format. Numbers are expressed as bytes.
       
      @verbatim
  
       ------------------------
       |                      |
       |  Header (4)          |  <-- Status, length of space used (not of packet)
       |                      |
       ------------------------
       |                      |
       |  Output Data         |
       |     (Length - 4)     |  <-- Buffer given to caller (ptr to this)
       |                      |
       ------------------------
       |                      |
       |  CRC (2)             |  <-- CRC required for transport protocol
       |                      |
       ------------------------
      @endverbatim
 */
 
/* 
    Copyright (c) 2003-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 */
 
/*===========================================================================

                          Edit History

      $Header: //components/rel/core.mpss/3.5.c12.3/api/services/rbq.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/18/10   is      Doxygenated.
01/10/09   as      Removed inclusion of customer.h 
06/21/03   lad     Created File

===========================================================================*/
            

#include "comdef.h"



/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION RBQ_INIT
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Initializes a buffer to be managed by RBQ. After initialization, do not 
	manipulate the buffer directly.
	
  @param[in,out] *rbq		Pointer to buffer to be managed by RBQ.
  @param[in] size       	The size, in bytes, of the RBQ buffer.
  
  @dependencies
    None
	
  @sideeffects
    None
	
  @return	
    <ul>
	  <li> True - Successfully initialized RBQ buffer
	  <li> False - Failed to initialize RBQ buffer
	</ul>
*/
/*==========================================================================*/
boolean
rbq_init (void *rbq, int size);

/*===========================================================================
FUNCTION RBQ_ALLOC
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Allocates the specified amount of space in the diag output buffer item.

  @param[in,out] *rbq		Pointer to RBQ buffer.
  @param[in] length			Amount of space to allocate for new RBQ item.
  
  @dependencies
    rbq_commit() or diagbuf_shorten(0) must be called to commit/return a 
	buffer to the diag pool.  This must happen reasonably soon after this 
	call completes.  If the buffer is needed to accumulate data over time, 
	then you should use your own buffer use this just long enough to copy 
	the data.

  @sideeffects
    rbq_head may be updated.
	
  @return		
    <ul>
	  <li> Pointer to allocated RBQ item buffer. 
	  <li> NULL if space is not available.
	</ul>
*/
/*==========================================================================*/
void *
rbq_alloc (void *rbq, int length);

/*===========================================================================
FUNCTION RBQ_SHORTEN
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Reduces the length field of a previously allocated buffer item.

    Allocating too much and using this to shorten the packet is ideal for
    situations in which the length of the packet is not known prior to
    allocation.  Using this scheme does, however, consume resources that would
    otherwise be used to buffer outbound diagnostics data.  Please use this
    scheme sparingly.
	
  @param[in,out] *rbq		Pointer to RBQ buffer.
  @param[in,out] *ptr       Pointer to previously allocated buffer item to 
                            shorten.
  @param[in] length         Length of buffer item to shorten by.
  
  @dependencies
    'ptr' must point to the same address that was returned by a prior call to
    diagpkt_alloc().
	
  @sideeffects
    None
  
  @return
    None
*/
/*==========================================================================*/
void
rbq_shorten (void *rbq, void *ptr, unsigned int length);

/*===========================================================================
FUNCTION RBQ_COMMIT
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Commits previously allocated space in the diag output buffer.

  @param[in,out] *rbq		Pointer to RBQ buffer.
  @param[in,out] *ptr  		Pointer to previously allocated buffer item.
	
  @dependencies
    'ptr' must be the address that was returned by a prior call to rbq_alloc().
	
  @sideeffects
    This function signals the diag task and may cause a context switch.
	
  @return	
    <ul>
	  <li> True - Success
	  <li> False - Failed
	</ul>
*/
/*==========================================================================*/
boolean
rbq_commit (void *rbq, void *ptr);

/*===========================================================================
FUNCTION RBQ_DEQUEUE
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Marks the last item from RBQ as dequeued and returns a pointer to the 
	item.

  @param[in,out] *rbq		Pointer to RBQ buffer.
  @param[out] *length       Length of the dequeued item.
	
  @dependencies
    None
	
  @sideeffects
    None
    	
  @return	
    <ul>
	  <li> Pointer to dequeued RBQ buffer item
	  <li> Null if failed to dequeue
	</ul>
*/
/*==========================================================================*/
void *
rbq_dequeue (void *rbq, int32 *length);

/*===========================================================================
FUNCTION RBQ_DEQUEUE_RETURN
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
	If free is set to True, removes the previously dequeued item from the RBQ.
	If free is set to False, returns the item back to the committed state.

  @param[in,out] *rbq		Pointer to RBQ buffer.
  @param[in,out] *item      Pointer to a specific item buffer to free.
  @param[in] free           If True, removes the previously dequeued item from the RBQ.
							If False, returns the item back to the committed state.
	
  @dependencies
    None
	
  @sideeffects
    None
    	
  @return	
    <ul>
	  <li> True - Success
	  <li> False - Failed
	</ul>
*/
/*==========================================================================*/
boolean
rbq_dequeue_return (void *rbq, void *item, boolean free);

/*===========================================================================
FUNCTION RBQ_USED
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Returns the number of bytes in use for the given RBQ.
	
  @param[in] *rbq		Pointer to RBQ buffer.
	
  @dependencies
    None
	
  @sideeffects
    None
    	
  @return	
    <ul>
	  <li> >=0 - Number of bytes used in the RBQ
	  <li> -1 - Invalid parameter
	</ul>
*/
/*==========================================================================*/
int
rbq_used (void *rbq);

/*===========================================================================
FUNCTION RBQ_FREE
============================================================================*/
/*!
  @ingroup diag_misc
  @brief 
    Returns the number of free bytes in use for the given RBQ.
	
  @param[in] *rbq		Pointer to RBQ buffer.
	
  @dependencies
    None
	
  @sideeffects
    None
    	
  @return	
    <ul>
	  <li> Number of bytes free in the RBQ
	  <li> -1 - Invalid parameter
	</ul>
*/
/*==========================================================================*/
int
rbq_free (void *rbq);

#ifdef __cplusplus
}
#endif

#endif				/* RBQ_H  */
