#ifndef __DSS_IDSNETWORKPRIVSCOPE_H__
#define __DSS_IDSNETWORKPRIVSCOPE_H__

/*===================================================

FILE:  DSS_IDSNetworkPrivScope.h

SERVICES:
   A utility class to facilitate IDSNetworkPriv fetching
   and releasing.

=====================================================

Copyright (c) 2008-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IDSNetworkPrivScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "ds_Utils_StdErr.h"
#include "DSS_NetMonitored.h"
#include "DSS_Common.h"

#include "ds_Net_INetworkPriv.h"

// This class provides an abstraction over IDSNetworkPriv pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the IDSNetworkPriv pointer is valid and can be used
// until the end of the scope where the IDSNetworkPriv object's reference counting
// decremented.
class DSSIDSNetworkPrivScope {
public:
   DSSIDSNetworkPrivScope();
   ~DSSIDSNetworkPrivScope();
   AEEResult Init(DSSNetMonitored* pDSSNetMonitored);
   AEEResult Init(DSSNetActive* pDSSNetActive);
   ds::Net::INetworkPriv* Fetch();
   
private:
   ds::Net::INetworkPriv* mpIDSNetworkPriv;   
};

inline DSSIDSNetworkPrivScope::DSSIDSNetworkPrivScope() : mpIDSNetworkPriv(NULL)
{
}

inline AEEResult DSSIDSNetworkPrivScope::Init(DSSNetMonitored* pDSSNetMonitored)
{   
   AEEResult res = pDSSNetMonitored->GetIDSNetworkPrivObject(&mpIDSNetworkPriv);
   if (AEE_SUCCESS != res) { 
     LOG_MSG_ERROR_2("DSSIDSNetworkPrivScope::Init: "
                     "Failed to fetch IDSNetworkPriv object from DSSNetMonitored 0x%p, res=%d",
                     pDSSNetMonitored, res);
      return res;
   }

   return AEE_SUCCESS;
}

inline AEEResult DSSIDSNetworkPrivScope::Init(DSSNetActive* pDSSNetActive)
{   
  AEEResult res = pDSSNetActive->GetIDSNetworkPrivObject(&mpIDSNetworkPriv);
  if (AEE_SUCCESS != res) { 
    LOG_MSG_ERROR_2("DSSIDSNetworkPrivScope::Init: "
                    "Failed to fetch IDSNetworkPriv object from DSSNetActive 0x%p, res=%d",
                    pDSSNetActive, res);
    return res;
  }

  return AEE_SUCCESS;
}

inline ds::Net::INetworkPriv* DSSIDSNetworkPrivScope::Fetch()
{   
   return mpIDSNetworkPriv;
}

/*lint -e{1551} */
inline DSSIDSNetworkPrivScope::~DSSIDSNetworkPrivScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkPriv);
}
/*lint –restore */

#endif // __DSS_IDSNETWORKPRIVSCOPE_H__
