#ifndef DS_SOCK_UTILS_H
#define DS_SOCK_UTILS_H

/*==========================================================================*/ 
/*! 
  @file
  ds_Sock_Utils.h 

  @brief
  This files provides common utility functions and macros used by the DSSOCK
  module.
  
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_Utils.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-05-29 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"
#include "ds_Sock_MemManager.h"

namespace DS
{
namespace Sock
{        

/**
  @brief      
  Utility macro for overloading the default new operator of a class.

  @details
  All of DSSOCK objects internally allocate memory using the ps_mem module.
  Hence the default new operator for each object is overloaded to support 
  allocation from PS Mem. Use this macro in the class definition to perform
  the new operator overloading.

  @param[in]  ps_mem_pool_id: PS Mem pool ID to use for the given class.

  @see        ps_mem.h:ps_mem_get_buf()
  @see        DSSockMemManager::GetBuf()
  @see        DSSOCK_OVERLOAD_OPERATOR_DELETE

  @return     None.
*/
#define DSSOCK_OVERLOAD_OPERATOR_NEW(ps_mem_pool_id)                        \
  inline void* operator new                                                 \
  (                                                                         \
    unsigned int num_bytes                                                  \
  )                                                                         \
  throw()                                                                   \
  {                                                                         \
    (void) num_bytes;                                                       \
    return ps_mem_get_buf (ps_mem_pool_id);                                 \
  }                                                                         \


/**
  @brief      
  Utility macro for overloading the default delete operator of a class.

  @details
  Use this macro inside the class definition (the header file) to overload 
  the default delete operator.

  @param      None.

  @see        ps_mem.h:ps_mem_get_buf()
  @see        DSSockMemManager::GetBuf()
  @see        DSSOCK_OVERLOAD_OPERATOR_NEW

  @return     None.
*/
#define DSSOCK_OVERLOAD_OPERATOR_DELETE()                                   \
  inline void operator delete                                               \
  (                                                                         \
    void* objPtr                                                            \
  )                                                                         \
  throw()                                                                   \
  {                                                                         \
    PS_MEM_FREE(objPtr);                                                    \
  }

/**
  @brief      
  Utility macro to perform overloading for both new and deletes operators.

  @param[in]  ps_mem_pool_id: PS Mem pool ID to use for the given class.

  @see        DSSOCK_OVERLOAD_OPERATOR_NEW
  @see        DSSOCK_OVERLOAD_OPERATOR_DELETE
*/
#define DSSOCK_OVERLOAD_OPERATORS(ps_mem_pool_id)                            \
  DSSOCK_OVERLOAD_OPERATOR_NEW(ps_mem_pool_id)                               \
  DSSOCK_OVERLOAD_OPERATOR_DELETE()                                          \


/**
  @brief      
  Utility macro to perform Release() on an interface pointer.

  @details
  This macro releases an interface if it is non-NULL. It then sets the 
  pointer to NULL.

  @param[in]  pInterface - Double pointer to the interface to be released.
  @see        IQI::Release()
*/

#define DSSOCK_RELEASEIF(pInterface)                                        \
  do                                                                        \
  {                                                                         \
    if (NULL != pInterface)                                                 \
    {                                                                       \
      (void) pInterface->Release();                                         \
      pInterface = NULL;                                                    \
    }                                                                       \
  } while (0)



}/* namespace Sock */
}/* namespace DS */

#endif /* DS_SOCK_UTILS_H */

