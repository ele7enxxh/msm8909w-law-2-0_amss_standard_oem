#ifndef DS_UTILS_ATOMIC_H
#define DS_UTILS_ATOMIC_H

/*==========================================================================*/
/*!
  @file
  ds_Utils_Atomic.h

  @brief
  This provides a set of atomic for Add and CompareOrAdd.

  @see AEEatomic.h

  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_Atomic.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  ------------------------------------------------------------
  2011-03-11 hm   Use single DS implementation for all configurations. 
  2009-11-29 mt   Created module.

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  @brief
  Initialize the ds_Utils_Atomic module. 

  @param None.
  @see   None.
*/
void ds_utils_atomic_Init
(
  void
);


/**
  @brief
  De-Initialize the ds_Utils_Atomic module. 

  @param None.
  @see   None.
*/
void ds_utils_atomic_DeInit
(
  void 
);


/**
  @brief
  Atomically add to a given value. 

  @param[out] pDest - Destination value
  @param[in]  valueToAdd - Value to add

  @return The result of atomic addition.
  @see   None.
*/
uint32 ds_utils_atomic_Add
(
  uint32*   pDest, 
  int       valueToAdd
);


/**
  @brief
  Perform and atomic comparison and add a given value.

  @details
  Used in places where a atomic comparison followed by an 
  increment is needed. For example, in case of weak-references, 
  we would like to compare the destination weak-reference count, 
  ensure it is non-zero, then perform an increment.

  @param[out] pDest - Destination value
  @param[in]  valueToCompare - Value to compare against.
  @param[in]  valueToAdd - Value to add

  @return The result of atomic addition.
  @see   ds_Utils_IWeakRef
*/
uint32 ds_utils_atomic_CompareOrAdd
(
  uint32*   pDest, 
  uint32    valueToCompare, 
  int       valueToAdd
);

#ifdef __cplusplus
}
#endif


#endif /* DS_UTILS_ATOMIC_H */

  
