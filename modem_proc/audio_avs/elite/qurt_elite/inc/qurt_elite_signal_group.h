/*========================================================================
Copyright (c) 2010-2012, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_signal_group.h

@brief This file contains signal utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
01/10/12   rkc     Created file.
========================================================================== */

#ifndef QURT_ELITE_SIGNAL_GROUP_H
#define QURT_ELITE_SIGNAL_GROUP_H

#include "qurt_elite_types.h"

/** @addtogroup qurt_elite_signal_group
@{ */

/** Signal group type is ANY. */
#define ANY_SIGNAL_GROUP     (0)

/** Signal group type is ALL. */
#define ALL_SIGNAL_GROUP     (1)

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
  Creates and initializes a signal group. All signals in the group are cleared.

  @param[out] ppSigGrp  Double pointer to the signal group to create and
                        initialize.
  @param[in]  type      Type of signal group:
                        - #ANY_SIGNAL_GROUP
                        - #ALL_SIGNAL_GROUP @tablebulletend

  @return
  ADSP_EOK -- Signal group was successfully created and initialized. \n
  ADSP_EBADPARAM -- ppSigGrp is NULL or an invalid type. \n
  ADSP_ENOMEMORY -- Signal group could not be created.
  @par
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_signal_group_create(void** ppSigGrp, uint32_t type);

/**
  Destroys a signal group.

  @param[in] pSigGrp    Signal group to destroy.
 
  @return
  ADSP_EOK -- Signal group was successfully destroyed. \n
  ADSP_EBADPARAM -- pSigGrp is NULL. \n
  ADSP_EBUSY -- Another thread is currently waiting on the signal group, and
                the signal group could not be destroyed.
  @par
  If the return value is not ADSP_EOK, the signal group is not modified.
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  The signal group must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_signal_group_destroy(void* pSigGrp);

/**
  Sets the specified signals in a signal group.

  @param[in]     pSigGrp       Pointer to the signal group.
  @param[in,out] bit_mask_ptr  Pointer to a mask value that identifies the
                               individual signals to be set in the signal group.

  @par Details for bit_mask_ptr
  Signals are represented as bits 0 to 31 in the 32-bit mask value.
  A mask bit value of 1 indicates that a signal is to be set, and a value
  of 0 indicates that a signal is not to be set.
  @par
  The signal bitmask value just before setting the given bitmask is returned
  through this same variable.

  @return
  ADSP_EOK -- Signals were successfully set. \n
  ADSP_EFAILED -- Signals were not successfully set, \n
  ADSP_EBADPARAM -- pSigGrp or bit_mask_ptr is NULL.
  @par
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  The signal group must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_signal_group_set(void* pSigGrp, uint32_t *bit_mask_ptr);

/**
  Clears the specified signals in a signal group.

  @param[in]     pSigGrp      Pointer to the signal group.
  @param[in,out] bit_mask_ptr Pointer to a mask value that identifies the
                              individual signals to be cleared in the signal
                              group.

  @par Details for bit_mask_ptr
  Signals are represented as bits 0 to 31 in the 32-bit mask value.
  A mask bit value of 1 indicates that a signal is to be set, and a value of 0
  indicates that a signal is not to be set.
  @par
  The signal bitmask value just before clearing the given bitmask is returned
  through this same variable.

  @return
  ADSP_EOK -- Signals were successfully cleared. \n
  ADSP_EFAILED -- Signals were not successfully cleared. \n
  ADSP_EBADPARAM -- pSigGrp or bit_mask_ptr is NULL.
  @par
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  The signal group must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_signal_group_clear(void* pSigGrp, uint32_t *bit_mask_ptr);

/**
  Gets signal values from a signal group.

  @param[in]  pSigGrp      Pointer to the signal group.
  @param[out] bit_mask_ptr Pointer to the bitmask of the current signal values.

  @return
  ADSP_EOK -- Signal values were successfully obtained. \n
  ADSP_EFAILED -- Signal values were not successfully obtained. \n
  ADSP_EBADPARAM -- pSigGrp or bit_mask_ptr is NULL.
  @par
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  The signal group must have been created and initialized before calling this
  function. @newpage
*/
ADSPResult qurt_elite_signal_group_get(void* pSigGrp, uint32_t* bit_mask_ptr);

/**
  Waits until the specified signals in the signal group are set. 
  No two threads can wait on the same signal group. 

  @param[in]     pSigGrp       Pointer to the signal group.
  @param[in,out] bit_mask_ptr  Pointer to a mask value that identifies the
                               individual signals to wait upon.

  @par Details for bit_mask_ptr
  If the signal object is the ANY type of signal group, the function unblocks
  when any of the signals specified in the bitmask are set.
  @par
  If the signal object is the ALL type of signal group, the function unblocks
  when all of the signals specified in the bitmask are set.
  @par
  After unblocking, the bitmask of set signals is returned through this
  variable.

  @return
  ADSP_EOK -- The wait is successful. \n
  ADSP_EBADPARAM -- pSigGrp or bit_mask_ptr is NULL. \n
  ADSP_EBUSY -- Another thread is already waiting on the same signal group.
  @par
  For more information, refer to @xhyperref{Q4,[Q4]}.

  @dependencies
  The signal group must have been created and initialized before calling this
  function.
*/
ADSPResult qurt_elite_signal_group_wait(void* pSigGrp, uint32_t* bit_mask_ptr);

/** @} */ /* end_addtogroup qurt_elite_signal_group */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_SIGNAL_GROUP_H

