/*===========================================================================
  FILE: ds_Net_ClassIDInstantiator.cpp

  OVERVIEW: This file provides implementation for the methods defined in
            ds_Net_ClassIDInstantiator.h

  DEPENDENCIES: None

            Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_ClassIDInstantiator.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-06-13 vm Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_ClassIDInstantiator.h"
#include "ds_Net_NetworkFactory.h"
#include "ds_Net_CNetworkFactory.h"

using namespace ds::Net;
using namespace ds::Error;


/*===========================================================================

                        PUBLIC MEMBER FUNCTIONS

===========================================================================*/
extern "C"
int dsNetNetworkFactoryCreateInstance
(
  void *    envPtr,
  AEECLSID  clsID,
  void **   newObjPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newObjPtrPtr)
  {
    LOG_MSG_ERROR_0("dsNetNetworkFactoryCreateInstance(): "
                    "NULL obj");
    return QDS_EFAULT;
  }

  if (AEECLSID_CNetworkFactory == clsID)
  {
    *newObjPtrPtr =
      NetworkFactory::CreateInstance(clsID);

    if (0 == *newObjPtrPtr)
    {
      LOG_MSG_ERROR_0("dsNetNetworkFactoryCreateInstance(): "
                      "Couldn't allocate NetworkFactory");
      return AEE_ENOMEMORY;
    }
  }

  return AEE_SUCCESS;

} /* dsNetNetworkFactoryCreateInstance() */

