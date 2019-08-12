/*!
  @file
  ds_Utils_CreateInstance_CS.cpp

  @brief
  This file provides implementation of CreateInstance for objects of
  ds::Utils module.

  When Component Services is present, the objects implemented by CS are
  created through it, and the internal ds::Utils implementation is not used.

*/
/*===========================================================================
  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_CreateInstance_cs.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-02-28 mt  Separated out CS-only functionality.
  2008-07-14 hm  Created module.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Errors_Def.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_CSignalBus.h"
#include "ds_Utils_IEnv.h"
#include "ds_Utils_env.h"
#include "ds_Utils_CEnv.h"

/*===========================================================================

                      STATIC VARIABLES FOR MODULE

===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
int DS_Utils_CreateInstance
(
  void *    env,
  AEECLSID  clsid,
  void **   newObj
)
{
  IEnv* piEnv = (IEnv*)env;
  int  nErr = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == piEnv) {
    nErr = env_GetCurrent(&piEnv);
    if (AEE_SUCCESS != nErr) {
      LOG_MSG_ERROR_1("DS_Utils_CreateInstance(): "
                      "Cannot recover a CS environment pointer, CS ret=%d",
                      nErr);
      return nErr;
    }
  }

  switch (clsid)
  {
    case AEECLSID_CSignalBus:
    case AEECLSID_CCritSect:
    case AEECLSID_CSignalFactory:
      nErr = piEnv->CreateInstance(clsid, newObj);
      if (AEE_SUCCESS != nErr)
      {
        LOG_MSG_ERROR_1("DS_Utils_CreateInstance(): "
                        "CS object creation failed, CS ret=%d", nErr);
      }
      break;

    default:
      nErr = AEE_ECLASSNOTSUPPORT;

  } /* switch(clsid) */

  LOG_MSG_INFO1_3("DS_Utils_CreateInstance(): "
                  "Clsid 0x%x, retval 0x%x, obj 0x%p", clsid, nErr, newObj);
  return nErr;

} /* DS_Utils_CreateInstance() */


