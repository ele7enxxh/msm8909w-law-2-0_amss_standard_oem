/*=========================================================================*/
/*!
  @file
  ds_Sock_CreateInstance.cpp

  @brief
  This file provides implementation of DSSockCreateInstance().

            Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_CreateInstance.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-07-14 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_Sock_ClassIDInstantiator.h"

#include "ds_Sock_CreateInstance.h"
#include "ds_Sock_SocketFactoryClient.h"
#include "ds_Sock_CSocketFactory.h"
#include "ds_Sock_CSocketFactoryService.h"

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

int DSSockCreateInstance
(
   IEnv* env,
   AEECLSID clsid,
   void** newObj
 )
{
  int ret = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Note: in the cases below, the same method is being called
  // both for AEECLSID_CX and for AEECLSID_CXService.
  // AEECLSID_CX is used when the DS requests to create the object.
  // AEECLSID_CXService is used when the CS requests to create the object.
  switch (clsid)
  {
    case ds::Sock::AEECLSID_CSocketFactory:
    case ds::Sock::AEECLSID_CSocketFactoryService:
      ret = DSSockSocketFactoryCreateInstance (env, clsid, newObj);
      break;

    default:
      ret = AEE_ECLASSNOTSUPPORT;
      break;

  } /* switch (clsid) */

  LOG_MSG_INFO1_3 ("DSSockCreateInstance(): "
                   "Clsid 0x%x, retval 0x%x, obj 0x%p", clsid, ret, newObj);

  return ret;

} /* DSSockCreateInstance() */


