/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_signal.h

@brief This file contains signal utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_signal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_SIGNAL_H
#define QURT_ELITE_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_signal
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
** Signals
*****************************************************************************/

/** Signal to be triggered by, or to trigger, events.

  The signal coalesces on a channel bit. The only way to receive a signal is
  through its associated channel.
 */
struct qurt_elite_signal_t
{
   qurt_elite_channel_t         *pChannel;
   /**< Pointer to the associated channel. */

   uint32_t                    unMyChannelBit;
   /**< Channel bitfield of this signal. */
};

/**
  Creates a signal.

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_signal_init() must be used instead.

  @datatypes
  qurt_elite_signal_t

  @param[out] ppSignal  Double pointer to the signal to create.

  @return
  Error code result.

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_signal_create(qurt_elite_signal_t** ppSignal);

/**
  Destroys a signal.

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_signal_deinit() must be used instead.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_signal_destroy(qurt_elite_signal_t* pSignal);


/**
  Initializes a signal. The client must allocate the memory for the signal and
  pass the pointer to this function.

  @datatypes
  qurt_elite_signal_t

  @param[out] pSignal  Pointer to the signal to create.

  @return
  Error code result.

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_signal_init(qurt_elite_signal_t* pSignal);

/**
  Deinitialize a signal.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_signal_deinit(qurt_elite_signal_t* pSignal);

/**
  Queries a signal for its channel.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to query.

  @return
  A handle to the channel containing the signal.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline qurt_elite_channel_t* qurt_elite_signal_get_channel(qurt_elite_signal_t* pSignal)
{
   return pSignal->pChannel;
}

/**
  Queries a signal for its channel bitfield.

  This is a 32-bit value with a single bit=1. Use this function to find the
  values to be ORed together to form an enable bitfield for a combination of
  signals in a channel.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to query.

  @return
  A bitfield with this signal's bit set to 1.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline uint32_t qurt_elite_signal_get_channel_bit(qurt_elite_signal_t* pSignal)
{
   return pSignal->unMyChannelBit;
}

/**
  Sends a signal.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to send.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline void qurt_elite_signal_send(qurt_elite_signal_t* pSignal)
{
   (void) qurt_signal2_set(&pSignal->pChannel->anysig, pSignal->unMyChannelBit);
}

/**
Clears a signal that is active in a channel.

  @datatypes
 qurt_elite_signal_t

  @param[in] pSignal   Pointer to the signal to clear.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline void qurt_elite_signal_clear(qurt_elite_signal_t* pSignal)
{
   (void) qurt_signal2_clear(&pSignal->pChannel->anysig, pSignal->unMyChannelBit);
}

/** @} */ /* end_addtogroup qurt_elite_signal */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_SIGNAL_H

