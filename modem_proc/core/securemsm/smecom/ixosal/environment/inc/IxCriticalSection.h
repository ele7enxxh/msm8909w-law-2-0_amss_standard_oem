#ifndef IXCRITICALSECTION_H
#define IXCRITICALSECTION_H
/*===========================================================================
                          SMECOM    W r a p p e r 
               f o r   C r i t i c a l  S e c t i o n  O b j e c t

*//** @file IXCriticalSection.h
  This file defines a methods that can be used to synchronize access 
  to data in multithreaded environment

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*//*========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixosal/environment/inc/IxCriticalSection.h#1 $

when       who         what, where, why
--------   ---         -------------------------------------------------------
12/01/10   nk          Initial Revision.
============================================================================*/

/* =======================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */
#include "comdef.h"
#include "IxErrno.h"
#include "IxOSLock.h"


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
   @brief 
      creates critical section.

   @detail
      Initializes the critical section.
 
   @param[in, out] pHandle - a reference to the critical section handle.
 
   @return zero value is success else failure.
 */
IxErrnoType IxCriticalSection_Create
(
  LOCKHANDLE*  piHandle
);

/**
   @brief 
      Critical section release

   @detail   
	  Releases the resources associated with the cirtical section
 
   @param[in] handle - the critical section handle
 
   @return zero value on success else failure
 */
IxErrnoType IxCriticalSection_Release
(
  LOCKHANDLE  handle
);

/**
   @brief 
       Enter critical section.

   @deatil
       Enter a critical section, blocks if another thread is inside the critical 
       section until the other thread leaves the critical section. 
 
   @param[in] handle - the critical section handle
 
   @return zero value on success else failure
 */
IxErrnoType IxCriticalSection_Enter
(
  LOCKHANDLE  handle
);

/** 
   @brief 
       Leaves the critical section.

   @deatil
       Leave a critical section (releases the lock). 
 
   @param[in] handle - the critical section handle
  
   @return zero value on success else failure
 */
IxErrnoType IxCriticalSection_Leave
(
  LOCKHANDLE  handle
);

/**
   @brief 
       Tries to enter critical section.

   @detail
       Tries to acquire the lock, return with failure immediately if not able to 
       acquire the lock unlike Enter which blocks until the lock is acquired.  
 
   @param[in] handle - the critical section handle
 
   @return zero value is success else failure
 */
IxErrnoType IxCriticalSection_TryEnter
(
  LOCKHANDLE  handle
);

/**
   @brief 
       Set the debugger message.

   @detail
       The function sets the debug message. This function
       can be used to enable all debug diagnostic message.
 
   @param[in] dflag - debugger flag sets true or false.
 
   @return
       None.
 */

void IxCriticalSection_setDebug_Msg
(
  DEBUGFLAG dflag 
);

/**
   @brief 
       Get the debugger message.

   @detail
      The function gets the debug message. The function
      returns true or false an is used as the checker under if condition
      to enable debug mesage or not. 
  
   @param
       None.
 
   @return
       Returns the debug flag state true or false.
 */

DEBUGFLAG IxCriticalSection_getDebug_Msg
(
  void 
);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* IXCRITICALSECTION_H */
