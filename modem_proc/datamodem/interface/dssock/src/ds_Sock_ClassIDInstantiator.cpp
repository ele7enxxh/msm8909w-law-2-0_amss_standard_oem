/*===========================================================================
  FILE: ds_Sock_ClassIDInstantiator.cpp

  OVERVIEW: This file provides implementation for the methods defined in
            ds_Sock_ClassIDInstantiator.h

  DEPENDENCIES: None

            Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_ClassIDInstantiator.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Sock_ClassIDInstantiator.h"
#include "ds_Sock_SocketFactoryClient.h"

using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                        PUBLIC MEMBER FUNCTIONS

===========================================================================*/
extern "C"
int DSSockSocketFactoryCreateInstance
(
  void *    envPtr,
  AEECLSID  clsID,
  void **   newObjPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newObjPtrPtr)
  {
    LOG_MSG_ERROR_0("DSSockSocketFactoryCreateInstance(): "
                    "NULL obj");
    return QDS_EFAULT;
  }

  *newObjPtrPtr = SocketFactoryClient::CreateInstance(clsID);
  if (0 == *newObjPtrPtr)
  {
    LOG_MSG_ERROR_0("DSSockSocketFactoryCreateInstance(): "
                    "Couldn't allocate SocketFactoryClient");
    return AEE_ENOMEMORY;
  }

  return AEE_SUCCESS;

} /* DSSockSocketFactoryCreateInstance() */

