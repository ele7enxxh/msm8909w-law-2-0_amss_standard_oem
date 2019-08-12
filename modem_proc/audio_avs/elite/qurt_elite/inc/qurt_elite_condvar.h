/*========================================================================
Copyright (c) 2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_condvar.h

@brief This file contains the ConditionVariables utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/elite/common/qurt_elite/main/latest/inc/qurt_elite_condvar.h#1

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
02/03/11   sudhir     Created file.

========================================================================== */

#ifndef QURT_ELITE_CONDITIONVARIABLE_H
#define QURT_ELITE_CONDITIONVARIABLE_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** @addtogroup qurt_elite_condvar
@{ */

/****************************************************************************
** ConditionVariables
*****************************************************************************/

/** Condition variable. */
typedef qurt_cond_t qurt_elite_condvar_t;


/**
  Initializes a QuRT_Elite condtion variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  None. @newpage
 */
static inline void qurt_elite_condvar_init(qurt_elite_condvar_t *condition_var_ptr)
{
     qurt_cond_init (condition_var_ptr);
}


/**
  Signals a condition variable object. This utility is used to awaken a single
  waiting thread.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and intialized before calling this
  function.
 */
static inline void qurt_elite_condvar_signal(qurt_elite_condvar_t *condition_var_ptr)
{
    qurt_cond_signal(condition_var_ptr);
}


/**
  Broadcasts a condition variable object. This utility is used to awaken
  multiple threads waiting for a condition variable.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies 
  The object must have been created and intialized before calling this
  function. @newpage
 */
static inline void qurt_elite_condvar_broadcast(qurt_elite_condvar_t *condition_var_ptr)
{
    qurt_cond_broadcast(condition_var_ptr);

}

/**
  Waits for a condition variable object. This utility suspends the current
  thread until the specified condition is true.

  @datatypes
  #qurt_elite_condvar_t \n
  #qurt_elite_mutex_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.
  @param[in] mutex              Mutex associated with the condition variable.

  @return
  None.

  @dependencies 
  The object must have been created and intialized before calling this
  function.
 */
static inline void qurt_elite_condvar_wait(qurt_elite_condvar_t *condition_var_ptr, qurt_elite_mutex_t *mutex)
{
    qurt_cond_wait(condition_var_ptr,(qurt_mutex_t*)mutex);
}

/**
  Destroys a condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and intialized before calling this
  function. @newpage
*/
static inline void qurt_elite_condvar_destroy(qurt_elite_condvar_t *condition_var_ptr)
{
   qurt_cond_destroy(condition_var_ptr);
}

/** @} */ /* end_addtogroup qurt_elite_condvar */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CONDITIONVARIABLE_H

