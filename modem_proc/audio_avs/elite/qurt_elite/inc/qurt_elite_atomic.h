/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_atomic.h

@brief This file is for atomic operations. This file provides functions for
thread-safe operations on atomic variables.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_atomic.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo      (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc      Created file.
03/09/10   brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_ATOMIC_H
#define QURT_ELITE_ATOMIC_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "atomic_ops.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_atomic
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
/** Atomic variable type. */
typedef atomic_word_t qurt_elite_atomic_word_t;

/****************************************************************************
** Atomic Ops
*****************************************************************************/
/** Atomic initialization. */
#define QURT_ELITE_ATOMIC_INIT(i)  { (i) }

/**
  Sets a value atomically.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.
  @param[in] val    Value to which to set the atomic variable.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None. @newpage
*/
static inline void qurt_elite_atomic_set(qurt_elite_atomic_word_t *pWord, int val)
{
   (void) atomic_set(pWord, (atomic_plain_word_t)val);
}

/**
  Gets the value of an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.

  @return
 The value of the atomic variable.

  @dependencies
  None.
*/
static inline int qurt_elite_atomic_get(qurt_elite_atomic_word_t *pWord)
{
   return (int)(pWord->value);
}

/**
  Increments an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.

  @return
 The new atomic variable value.

  @dependencies
  None. @newpage
*/
static inline int qurt_elite_atomic_increment(qurt_elite_atomic_word_t *pWord)
{
   return (int)atomic_inc_return(pWord);
}

/**
  Adds to an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.
  @param[in] unVal  Number to add to the atomic variable.

  @return
 The new value of the atomic variable.

  @dependencies
  None.
*/
static inline void qurt_elite_atomic_add(qurt_elite_atomic_word_t *pWord, uint32_t unVal)
{
   atomic_add(pWord, unVal);
}

/**
  Decrements an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.

  @return
 The new atomic variable value.

  @dependencies
  None. @newpage
*/
static inline int qurt_elite_atomic_decrement(qurt_elite_atomic_word_t *pWord)
{
   return (int)atomic_dec_return(pWord);
}

/**
  Subtracts from an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] pWord  Pointer to the atomic variable.
  @param[in] unVal  Number to subtract from the atomic variable.

  @return
 The new atomic variable value.

  @dependencies
  None.
*/
static inline void qurt_elite_atomic_subtract(qurt_elite_atomic_word_t *pWord, uint32_t unVal)
{
   atomic_sub(pWord, unVal);
}

/**
  Bitwise ORs an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] word_ptr  Pointer to the atomic variable.
  @param[in] val       OR bitfield.

  @return
 The new value of the atomic variable.

  @dependencies
  None. @newpage
*/
static inline void qurt_elite_atomic_or(qurt_elite_atomic_word_t *word_ptr, uint32_t val)
{
   atomic_or(word_ptr, val);
}

/**
  Bitwise ANDs an atomic variable.

  @datatypes
  #qurt_elite_atomic_word_t

  @param[in] word_ptr  Pointer to the atomic variable.
  @param[in] val       AND bitfield.

  @return
 The new value of the atomic variable.

  @dependencies
  None.
*/
static inline void qurt_elite_atomic_and(qurt_elite_atomic_word_t *word_ptr, uint32_t val)
{
   atomic_and(word_ptr, val);
}

/** @} */ /* end_addtogroup qurt_elite_atomic */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //QURT_ELITE_ATOMIC_H
