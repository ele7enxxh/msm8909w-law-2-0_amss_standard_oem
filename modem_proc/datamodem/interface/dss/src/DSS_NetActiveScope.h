#ifndef __DSS_NETACTIVESCOPE_H__
#define __DSS_NETACTIVESCOPE_H__

/*===================================================

FILE:  DSS_NetActiveScope.h

SERVICES:
A utility class to facilitate DSSNetActive fetching
and releasing.

=====================================================

Copyright (c) 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetActiveScope.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-04-18 en  History added.

===========================================================================*/

#include "ds_Utils_StdErr.h"
#include "DSS_Common.h"
#include "DSS_NetApp.h"

class DSSNetActive;

// This class provides an abstraction over DSSNetActive pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the DSSNetActive pointer is valid and can be used
// until the end of the scope where the DSSNetActive object's reference counting
// decremented.
class DSSNetActiveScope {
public:
   DSSNetActiveScope();
   ~DSSNetActiveScope();
   AEEResult Init(DSSNetApp* pDSSNetApp);
   DSSNetActive* Fetch();

private:
   DSSNetActive* mpDSSNetActive;
};

inline DSSNetActiveScope::DSSNetActiveScope() : mpDSSNetActive(NULL)
{
}

inline AEEResult DSSNetActiveScope::Init(DSSNetApp* pDSSNetApp)
{   
   AEEResult res = pDSSNetApp->GetNetActive(&mpDSSNetActive);
   if (AEE_SUCCESS != res) { 
      LOG_MSG_INFO1_1("Failed to fetch NetActive from NetApp object: %d", res);
      return res;
   }

   return AEE_SUCCESS;
}

inline DSSNetActive* DSSNetActiveScope::Fetch()
{
   return mpDSSNetActive;
}

/*lint -e{1551} */
inline DSSNetActiveScope::~DSSNetActiveScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpDSSNetActive);
}
/*lint –restore */

#endif // __DSS_NETACTIVESCOPE_H__ 
