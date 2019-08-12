#ifndef __DSS_IDSNETWORKEXTSCOPE_H__
#define __DSS_IDSNETWORKEXTSCOPE_H__

/*===================================================

FILE:  DSS_IDSNetworkScope.h

SERVICES:
   A utility class to facilitate IDSNetwork fetching
   and releasing.

=====================================================

Copyright (c) 2008 - 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IDSNetworkExtScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "ds_Utils_StdErr.h"
#include "DSS_Common.h"

#include "ds_Net_INetwork.h"

// This class provides an abstraction over IDSNetwork pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the IDSNetwork pointer is valid and can be used
// until the end of the scope where the IDSNetwork object's reference counting
// decremented.
class DSSIDSNetworkExtScope {
public:
   DSSIDSNetworkExtScope();
   ~DSSIDSNetworkExtScope();
   AEEResult Init(DSSNetActive* pDSSNetActive);
   AEEResult Init(DSSNetMonitored* pDSSNetMonitored);
   ds::Net::INetworkExt* Fetch();
   
private:
   ds::Net::INetworkExt* mpIDSNetworkExt;   
};

inline DSSIDSNetworkExtScope::DSSIDSNetworkExtScope() : mpIDSNetworkExt(NULL)
{
}

inline AEEResult DSSIDSNetworkExtScope::Init(DSSNetActive* pDSSNetActive)
{   
  AEEResult res = pDSSNetActive->GetIDSNetworkExtObject(&mpIDSNetworkExt);
  if (AEE_SUCCESS != res) { 
     LOG_MSG_ERROR_1("Failed to fetch IDSNetworkExt object: %d", res);
     return res;
  }

  return AEE_SUCCESS;
}

inline AEEResult DSSIDSNetworkExtScope::Init(DSSNetMonitored* pDSSNetMonitored)
{   
   AEEResult res = pDSSNetMonitored->GetIDSNetworkExtObject(&mpIDSNetworkExt);
   if (AEE_SUCCESS != res) { 
      LOG_MSG_ERROR_1("Failed to fetch IDSNetworkExt object: %d", res);
      return res;
   }

   return AEE_SUCCESS;
}

inline ds::Net::INetworkExt* DSSIDSNetworkExtScope::Fetch()
{   
   return mpIDSNetworkExt;
}

/*lint -e{1551} */
inline DSSIDSNetworkExtScope::~DSSIDSNetworkExtScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkExt);
}
/*lint –restore */

#endif // __DSS_IDSNETWORKEXTSCOPE_H__
