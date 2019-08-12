/*!
  @file
  ds_Utils_CreateInstance.cpp

  @brief
  This file provides implementation CreateInstance methods for objects of
  ds::Utils module when Component Services is not available.

  When CS is not available, internal ds::Utils implementation is used.
*/
/*===========================================================================
  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_CreateInstance.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-01-10 mt  Separated CS functionality into a different file.
  2008-07-14 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_CSignalBus.h"

#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CritSect.h"
#include "ds_Utils_SignalBus.h"
#include "ds_Utils_SignalFactory.h"
#include "ds_Errors_Def.h"


/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
ds::ErrorType DS_Utils_CreateInstance
(
  void *    env,
  AEECLSID  clsid,
  void **   newObj
)
{
  ds::ErrorType dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (clsid)
  {
    case AEECLSID_CSignalBus:
      dsErrno = ds::Utils::SignalBus::CreateInstance(env, clsid, newObj);
      break;

    case AEECLSID_CCritSect:
      dsErrno = ds::Utils::CritSect::CreateInstance(env, clsid, newObj);
      break;

    case AEECLSID_CSignalFactory:
      dsErrno = ds::Utils::SignalFactory::CreateInstance(env, clsid, newObj);
      break;

    default:
      dsErrno = AEE_ECLASSNOTSUPPORT;

  } /* switch(clsid) */

  return dsErrno;

} /* DS_Utils_CreateInstance() */


