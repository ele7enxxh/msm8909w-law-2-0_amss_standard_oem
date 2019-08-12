/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_queue.h

@brief This file contains the queue utilities. Queues must be created and added
to a channel before they can be used. Queues are pushed from the back and can be
popped from either front(FIFO) or back(LIFO). Queues must be destroyed when
they are no longer needed.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_queue.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_QUEUE_H
#define QURT_ELITE_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_queue
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** Limitation on the number of characters to save in resource
    names (e.g., thread names).
*/
#define QURT_ELITE_DEFAULT_NAME_LEN 16

/** Gets the memory size required by the queue with a certain number of
    items. @newpage
*/
#define QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(nItems) \
                (sizeof(qurt_elite_queue_t) +  ((nItems-1)  * sizeof(uint64)))



/****************************************************************************
** Queues
*****************************************************************************/

/** Queue that transfers messages between threads.

  The queue implementation is signal-based and thread safe. Each queue node is
  always 8 bytes. The number of queue nodes is sized up to powers of 2. A queue
  must be associated with a channel.
 */
struct qurt_elite_queue_t
{
   qurt_elite_mutex_t        mqLock;
   /**< Mutex for thread-safe access to the queue. */

   qurt_elite_channel_t      *pChannel;
   /**< Pointer to the associated channel. */

   uint64_t             *data;
   /**< Pointer to the message payload. */

   uint32_t             myChannelBit;
   /**< Channel bitfield of this queue. */

   int                  nModuloOperator;
   /**< Used for circular addressing of queue nodes; nNumNodes-1. */

   int                  nBackIndex;
   /**< Updated if one queue node is pushed or popped from the back. */

   int                  nFrontIndex;
   /**< Updated if one queue node is popped from the front. */

   int                  nNumNodes;
   /**< Number of queue nodes. */

   bool_t               fDisableQ;
   /**< Specifies whether the queue is disabled. */

   char                 name[QURT_ELITE_DEFAULT_NAME_LEN];
   /**< Name of the queue. */

   uint64_t ullDataPlaceholder;
   /**< Start of the array pointed to by *data. */
   /* Dont add any variable below ullDataPlaceholder. */
};


/**
  Creates a queue of 64-bit unsigned integers. nMaxItems must be a
  power of 2.

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_queue_init() must be used instead.

  @datatypes
  qurt_elite_queue_t

  @param[in] pzQName     Pointer to the name of the queue.
  @param[in]  nMaxItems  Maximum number of items that can be pushed into the
                         queue.
  @param[out] pQ         Double pointer to the created queue.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_queue_create(char *pzQName, int nMaxItems, qurt_elite_queue_t** pQ);

/**
  Destroys a queue.

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_queue_deinit() must be used instead.

  @datatypes
  qurt_elite_queue_t

  @param[in,out] pQ   Pointer to the queue to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this function.
*/
void qurt_elite_queue_destroy(qurt_elite_queue_t* pQ);

/**
Initializes a queue of 64-bit unsigned integers. nMaxItems must be a power of 2.
The client must allocate the memory for the queue and pass the pointer.

  @datatypes
 qurt_elite_queue_t

  @param[in]     pzQ2Name   Pointer to the name of the queue.
  @param[in]     nMaxItems  Maximum number of items that can be pushed into
                            the queue.
  @param[in,out] pQ         Pointer to the initialized queue.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_queue_init(char *pzQ2Name, int nMaxItems, qurt_elite_queue_t* pQ);

/**
  Deinitializes a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in,out] pQ   Pointer to the queue to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_queue_deinit(qurt_elite_queue_t* pQ);

/**
  Disables a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue to disable.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_queue_disable(qurt_elite_queue_t* pQ);


/**
  Pushes an item into a queue.

  This function is nonblocking. The user is responsible for not overflowing
  the queue to avoid getting an undesirable assertion.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue to which to push an item.
  @param[in] pPayload  Pointer to the address (location) of the item.
                       The item is pushed (copied) into the queue, not the
                       queue address.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_queue_push_back(qurt_elite_queue_t* pQ, uint64_t* pPayload);

/**
  Pops an item out of a queue.

  This function is nonblocking and returns ADSP_ENOMORE if empty. Typically,
  this function is called only after waiting for a channel and checking if
  this queue contains any items.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue from which to pop an item.
  @param[in] pPayload  Pointer to the target address (location) to which to
                       copy the item being popped from the front of the queue.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_queue_pop_front(qurt_elite_queue_t* pQ,  uint64_t* pPayload);

/**
  Pops an item out of a queue from the back.

  This function is for LIFO queues. It is nonblocking and returns AEE_ENOMORE
  if empty. Typically, this function is called only after waiting for a
  channel and checking if this queue contains any items.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue from which to pop an item.
  @param[in] pPayload  Pointer to the target address (location) to which to copy
                       the item being popped from the back of the queue.
  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_queue_pop_back(qurt_elite_queue_t* pQ,  uint64_t* pPayload );

/**
  Queries a queue for its channel.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue to query.

  @return
  A handle to the channel containing the queue.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline qurt_elite_channel_t* qurt_elite_queue_get_channel(qurt_elite_queue_t* pQ)
{
   return pQ->pChannel;
}

/**
  Queries a queue for its channel bitfield.

  This is a 32-bit value with a single bit=1. Use this function to find the
  values to be ORed together to form an enable bitfield for a combination of
  queues in a channel.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue to query.

  @return
  A bitfield with this queue's bit set to 1.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline uint32_t qurt_elite_queue_get_channel_bit(qurt_elite_queue_t* pQ)
{
   return pQ->myChannelBit;
}

/** @} */ /* end_addtogroup qurt_elite_queue */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_QUEUE_H

