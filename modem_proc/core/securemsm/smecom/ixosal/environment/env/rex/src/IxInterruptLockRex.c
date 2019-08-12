/*===========================================================================
                          I X   W r a p p e r 
               f o r   C r i t i c a l  S e c t i o n  R e x  O b j e c t

*//** @file IxInterruptLockRex.c
  This file implements a utility class that can be used to synchronize access 
  to data in multithreaded environment

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*//*========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixosal/environment/env/rex/src/IxInterruptLockRex.c#1 $

when       who         what, where, why
--------   ---         -------------------------------------------------------
12/01/10    nk         Initial Revision.

============================================================================*/

/*===========================================================================
 Include Files
============================================================================*/
#include "IxInterruptLock.h"


#if !defined(FEATURE_BLAST)
#include "rex.h"
#include "secdiag.h"
#include "UxCommon.h"
#include "IxObjectRegistry.h"


static DEBUGFLAG m_ildebug = 0;

/**
   @brief 
       Initializes the interrupt lock
 
   @param[in, out] pHandle - a reference to the interrupt lock handle
  
   @return zero value is success else failure
 */
IxErrnoType IxInterruptLock_Create
(
  LOCKHANDLE*  piHandle
)
{
  IxErrnoType retval = E_FAILURE;
  REGISTRYHANDLE p_ObjReg = call_IxObjectRegistry_Instance();
  
  if (piHandle)
  {
    dword *psavvar = NULL;
    int iHandle = (int)*piHandle;
    sLockInfo *p_lock = call_IxObjectRegistry_GetObject(p_ObjReg, iHandle);
    
    psavvar = (dword *) UxMalloc(sizeof(dword), UX_ALLOC_LOCAL);
    
    if (psavvar)
    {
      p_lock->lockhandle = psavvar;
      if(IxInterruptLock_getDebug_Msg())
      {
        SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxInterruptLock_Create Rex::handle id %d", iHandle );
      }
      retval = E_SUCCESS;
    }
  }

  return retval;
}

/**
   @brief 
       Releases interrupt locks

   @detail
       Releases the resources associated with the interrupt lock
 
   @param[in] handle - the interrupt lock handle
  
   @return zero value on success else failure
 */
IxErrnoType IxInterruptLock_Release
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
       UxFree(p_lock->lockhandle);
       if(IxInterruptLock_getDebug_Msg())
       {
         SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxInterruptLock_Release Rex::handle id %d", iHandle );
       }
       retval = E_SUCCESS;
    }
  }

  return retval;
}

/**
   @brief 
       Enter Interrupt lock
  
   @detail
       Enter a critical section, blocks if another thread is inside the interrupt lock 
       until the other thread leaves the critical section. 
 
   @param[in] handle - the interrupt lock handle
 
   @return zero value on success else failure
 */
IxErrnoType IxInterruptLock_Enter
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
      dword *psavvar = p_lock->lockhandle;
      if(NULL != psavvar)          // NULL check psavvar
      {
        *psavvar = rex_int_lock();
         if(IxInterruptLock_getDebug_Msg())
         {
           SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxInterruptLock_Enter Rex::handle id %d", iHandle );
         }
        retval = E_SUCCESS;
      }
      else
      {
        retval = E_FAILURE;
      }
    } // else
 
  }// if handle

  return retval;
}

/**
   @brief 
       Leave interrupt lock

   @detail
       Leave a interrupt lock (releases the lock). 
 
   @param[in] handle - the interrupt lock handle
  
   @return zero value on success else failure
 */
IxErrnoType IxInterruptLock_Leave
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
      dword *psavvar = p_lock->lockhandle;
      if(NULL != psavvar)          // NULL check psavvar
      {
        if(!(*psavvar))
        { 
          rex_int_free();
          if(IxInterruptLock_getDebug_Msg())
          {
            SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxInterruptLock_Leave Rex::handle id %d", iHandle );
          }
          retval = E_SUCCESS;
        }
        else
        {
          retval = E_FAILURE;
        }
      }
      else
      {
        retval = E_FAILURE;
      }
      
    }// else

  }// if handle
 
  return retval;
}

/**
   @brief 
       Tries to get an interrupt lock.

   @detail
       Tries to acquire the lock, return with failure immediately if not able to 
       acquire the lock unlike Enter which blocks until the lock is acquired.  
 
   @param[in] handle - the interrupt lock handle
 
   @return zero value is success else failure
 */
IxErrnoType IxInterruptLock_TryEnter
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
      dword *psavvar = p_lock->lockhandle;
      if(NULL != psavvar)          // NULL check psavvar
      {
        *psavvar = rex_int_lock();
        if(IxInterruptLock_getDebug_Msg())
        {
          SEC_DIAG_MSG1( SECDIAG_IXCOMMON, MSG_SEC_MED,
                      "IxInterruptLock_TryEnter Rex::handle id %d", iHandle );
        }
        retval = E_SUCCESS;
      }
      else
      {
        retval = E_FAILURE;
      }
    } // else
 
  }// if handle

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
void IxInterruptLock_setDebug_Msg( DEBUGFLAG dflag )
{
  m_ildebug = dflag;
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

DEBUGFLAG IxInterruptLock_getDebug_Msg()
{
  return m_ildebug;
}
#endif /* NOT FEATURE_BLAST */
