/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_channel.h
@brief This file contains a utility to form a channel of a combination
of up to 32 signals, queues, and timers. The client can wait on
any combination of signals, queues, and timers and be awakened
when any desired element is active.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_channel.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo      (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc      Created file.
03/09/10   brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_CHANNEL_H
#define QURT_ELITE_CHANNEL_H


#include "qurt.h"
#include <stdio.h>
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"
#include "qurt_elite_diag.h"
#include "qurt_elite_profile.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_channel
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
/* Forward declaration.*/
typedef struct qurt_elite_queue_t qurt_elite_queue_t;

/* Forward declaration*/
typedef struct qurt_elite_channel_t qurt_elite_channel_t;

/* Forward declaration*/
typedef struct qurt_elite_signal_t qurt_elite_signal_t;

/** Mask of all channel bits. */
#define QURT_ELITE_CHANNEL_ALL_BITS 0xFFFFFFFFL

/****************************************************************************
** Channels
*****************************************************************************/

/** Structure that combines up to 32 queues and signals. Each queue and
    signal is assigned a bit in the 32-bit field.
*/
struct qurt_elite_channel_t
{
   qurt_signal2_t    anysig;
   /**< 32-bit signal channel. */

   uint32_t               unBitsUsedMask;
   /**< Mask bookkeeping used bits (1=used; 0=available). */
};

/**
  Initializes a channel.

  There can be only one receiver thread in this channel implementation. A
  channel is a combination of up to 32 signals, queues, and timers. Any mask
  of the associated elements can be set up to block until one of the nonmasked
  queues receives an element. Typically, the receiver blocks on a channel,
  checks which queue or queues have data, and pops the data off at least one
  of the queues.

  Channel operations are NOT thread-protected. Only the owner-receiver thread
  is to touch them.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Pointer to the channel to initialize.

  @return
  None.

  @dependencies
  None. @newpage
*/
static inline void qurt_elite_channel_init(qurt_elite_channel_t* pChannel)
{
   pChannel->unBitsUsedMask = 0;
   qurt_signal2_init(&pChannel->anysig);
}

/**
  Destroys a channel. This function must be called to clean up the
  resources from a corresponding call to qurt_elite_channel_init().

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Pointer to the channel to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline void qurt_elite_channel_destroy(qurt_elite_channel_t* pChannel)
{
   qurt_signal2_destroy(&pChannel->anysig);
   pChannel->unBitsUsedMask = 0;
}

/**
  Adds a queue to a channel. Each queue can belong to only one channel.

  @datatypes
  qurt_elite_channel_t \n
  qurt_elite_queue_t

  @param[in] pChannel   Pointer to the channel to which to add a queue.
  @param[in] pQ         Pointer to the queue to add.
  @param[in] unBitMask  Indicates the bit to which to add the queue.

  @return
  Handle to the channel containing the queue.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_channel_addq(qurt_elite_channel_t* pChannel, qurt_elite_queue_t* pQ, uint32_t unBitMask);

/**
  Adds a signal to a channel.

  @datatypes
  qurt_elite_channel_t \n
  qurt_elite_signal_t

  @param[in] pChannel   Pointer to the channel to which to add a signal.
  @param[in] pSignal    Pointer to the signal to add.
  @param[in] unBitMask  Indicates the bit to which to add the signal.

  @return
  Indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_channel_add_signal(qurt_elite_channel_t* pChannel, qurt_elite_signal_t* pSignal, uint32_t unBitMask);

/**
  Waits for a combination of queues/signals/timers in a channel to
  receive an item.

  @note1hang Only one thread can wait on a channel at a time.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel          Pointer to the channel upon which to wait.
  @param[in] unEnableBitfield  Mask indicating to which queues and signals
                               to listen.

  @return
  A bitfield indicating which queues have items.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline uint32_t qurt_elite_channel_wait(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield)
{
   //MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "WAIT: Channelptr=0x%x, ENABLE=0x%x", pChannel, unEnableBitfield);
   uint32_t status = (uint32_t) qurt_signal2_wait_any(&pChannel->anysig, unEnableBitfield);
   //MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "GOT: Channelptr=0x%x, STATUS=0x%x", pChannel, qurt_signal2_get(&pChannel->anysig));
   return (status & unEnableBitfield);
}

/**
  Checks which of the channel queues and signals have received active triggers.

  A bit setting of 1 indicates there is something to receive. This operation
  is like a poll for a particular set of bits in cases where the blocking from
  qurt_elite_channel_wait() is not wanted.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel          Pointer to the channel in which to check.
  @param[in] unEnableBitfield  Mask indicating to which queues and signals
                               to listen.

  @return
  A 32-bit field indicating which of the queues and signals have received
  active triggers.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline uint32_t qurt_elite_channel_poll(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield)
{
   return (unEnableBitfield & qurt_signal2_get(&pChannel->anysig));
}

/** @} */ /* end_addtogroup qurt_elite_channel */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CHANNEL_H

