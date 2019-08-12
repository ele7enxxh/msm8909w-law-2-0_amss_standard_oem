#ifndef DS_UTILS_SINGLETON_H
#define DS_UTILS_SINGLETON_H

/*===========================================================================
  @file ds_Utils_Singleton.h

  This file provides the ds::Utils::Singleton Template.

  InstanceInit()
    Initialize the Instance.

  GetInstance()
    retrieve the Instance.

  InstanceRelease()
    Clear the Instance.

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_Singleton.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-02-04 sb  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_StdErr.h"

#include "ps_mem_ext.h"
#include "ps_mem.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Utils
  {
    template <typename T>
    class Singleton
    {
    public:
      static AEEResult InstanceInit(void)
      {
        if ( NULL != pmInstance)
        {
          return AEE_EFAILED;
        }
        pmInstance = new T();
        return AEE_SUCCESS;
      }
      static T* GetInstance(void)
      {
        return pmInstance;
      }
      static AEEResult InstanceRelease(void)
      {
        if ( NULL == pmInstance)
        {
          return AEE_EFAILED;
        }
        delete (pmInstance);
        pmInstance = NULL;
        return AEE_SUCCESS;
      }
    protected:
      void* operator new (unsigned int num_bytes) throw();
      void  operator delete (void * bufPtr) throw();
      static T* pmInstance;

    };
    template <typename T>
    T* Singleton<T>::pmInstance = NULL;
    // T must be the derived Class below

  } /* namespace Utils */
}/* namespace ds */


#endif /* DS_UTILS_SINGLETON_H */

