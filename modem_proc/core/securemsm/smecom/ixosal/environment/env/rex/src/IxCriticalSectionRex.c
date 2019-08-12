/*===========================================================================
                          I X   W r a p p e r 
               f o r   C r i t i c a l  S e c t i o n  R e x  O b j e c t

*//** @file IxCriticalSectionRex.c
  This file implements a utility class that can be used to synchronize access 
  to data in multithreaded environment

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*//*========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixosal/environment/env/rex/src/IxCriticalSectionRex.c#1 $

when       who         what, where, why
--------   ---         -------------------------------------------------------
12/01/10    nk         Initial Revision.

============================================================================*/

/*===========================================================================
 Include Files
============================================================================*/
#include "IxCriticalSection.h"

#if !defined(FEATURE_BLAST)
#include "rex.h"
#include "secdiag.h"
#include "UxCommon.h"
#include "IxObjectRegistry.h"

static DEBUGFLAG m_csdebug = 0;

/**
   @brief 
      creates critical section.

   @detail
      Initializes the critical section.
 
   @param[out] pHandle - a reference to the critical section handle.
 
   @return zero value is success else failure.
 */
IxErrnoType IxCriticalSection_Create
(
  LOCKHANDLE*  piHandle
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();
  
  if (piHandle)
  {
    rex_crit_sect_type *pCriticalSection =  NULL;
    int iHandle = (int)*piHandle;

    sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
    

    pCriticalSection = (rex_crit_sect_type*)UxMalloc(sizeof(rex_crit_sect_type), UX_ALLOC_LOCAL);
    if (pCriticalSection)
    {
      rex_init_crit_sect(pCriticalSection);
      p_lock->lockhandle = pCriticalSection;
      if(IxCriticalSection_getDebug_Msg())
      {
        SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxCriticalSection_Create Rex::handle id %d", iHandle );
      }
      retval = E_SUCCESS;
    }
  }

  return retval;
}

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
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();

  if (handle)
  {
    int iHandle = handle;
    
    if(!call_IxObjectRegistry_IsRegistered(p_ObjReg, iHandle))
    {
       retval = E_FAILURE;
       return retval;
    }
    else   /* if registered handle */
    {
       sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
       rex_del_crit_sect((rex_crit_sect_type *)p_lock->lockhandle);
       UxFree(p_lock->lockhandle);
       if(IxCriticalSection_getDebug_Msg())
       {
         SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxCriticalSection_Release Rex::handle id %d", iHandle );
       }
       retval = E_SUCCESS;
    }
  }

  return retval;
}

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
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();

 if (handle)
  {
    int iHandle = handle;
     
    if(!call_IxObjectRegistry_IsRegistered(p_ObjReg, iHandle))
    {
       retval = E_FAILURE;
       return retval;
    }
    else   /* if registered handle */
    {
      sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
      rex_enter_crit_sect((rex_crit_sect_type *)p_lock->lockhandle);
      if(IxCriticalSection_getDebug_Msg())
      {
        SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxCriticalSection_Enter Rex::handle id %d", iHandle );
      }
      retval = E_SUCCESS;
    }
  }

  return retval;
}

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
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();

  if (handle)
  {
    int iHandle = handle;

    if(!call_IxObjectRegistry_IsRegistered(p_ObjReg, iHandle))
    {
       retval = E_FAILURE;
       return retval;
    }
    else   /* if registered handle */
    {
      sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
      rex_leave_crit_sect((rex_crit_sect_type *)p_lock->lockhandle);
      if(IxCriticalSection_getDebug_Msg())
      {
         SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxCriticalSection_Leave Rex::handle id %d", iHandle );
      }
      retval = E_SUCCESS;
    }
  }
 
  return retval;
}

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
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();

  if (handle)
  {
    int iHandle = handle;

    if(!call_IxObjectRegistry_IsRegistered(p_ObjReg, iHandle))
    {
       retval = E_FAILURE;
       return retval;
    }
    else   /* if registered handle */
    {
      sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
      if ( p_lock->lockhandle && rex_try_enter_crit_sect((rex_crit_sect_type *)p_lock->lockhandle))
      {
        if(IxCriticalSection_getDebug_Msg())
        {
          SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxCriticalSection_TryEnter Rex::handle id %d", iHandle );
        }
        retval = E_SUCCESS;
      }
    }
  }
  return retval;
}

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
void IxCriticalSection_setDebug_Msg( DEBUGFLAG dflag )
{
  m_csdebug = dflag;
}

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

DEBUGFLAG IxCriticalSection_getDebug_Msg()
{
  return m_csdebug;
}
#endif /* NOT FEATURE_BLAST */
