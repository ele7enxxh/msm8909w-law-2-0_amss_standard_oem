#ifndef __DSS_NETMONITOREDSCOPE_H__
#define __DSS_NETMONITOREDSCOPE_H__

/*===================================================

FILE:  DSS_NetMonitoredScope.h

SERVICES:
A utility class to facilitate DSSNetMonitored fetching
and releasing.

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetMonitoredScope.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-04-18 en  History added.

===========================================================================*/

#include "ds_Utils_StdErr.h"
#include "DSS_NetMonitored.h"
#include "DSS_Common.h"

#include "ds_Net_INetwork.h"

// This class provides an abstraction over DSSNetMonitored pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the DSSNetMonitored pointer is valid and can be used
// until the end of the scope where the DSSNetMonitored object's reference counting
// decremented.
class DSSNetMonitoredScope {
public:
  DSSNetMonitoredScope();
  ~DSSNetMonitoredScope();
  AEEResult Init(DSSNetMonitored* pDSSNetMonitored);
  AEEResult Init(DSSNetApp* pDSSNetApp);
  AEEResult Init(
    DSSNetApp* pDSSNetApp,
    dss_iface_id_type ifaceId,
    bool bAllowToCreateSecondary);
  DSSNetMonitored* Fetch();

private:
  DSSNetMonitored* mpDSSNetMonitored;
};

inline DSSNetMonitoredScope::DSSNetMonitoredScope() : mpDSSNetMonitored(NULL)
{
}

inline AEEResult DSSNetMonitoredScope::Init(DSSNetMonitored* pDSSNetMonitored)
{   
  mpDSSNetMonitored = pDSSNetMonitored;
  return AEE_SUCCESS;
}

inline AEEResult DSSNetMonitoredScope::Init(DSSNetApp* pDSSNetApp)
{   
  AEEResult res = AEE_SUCCESS;

  res = pDSSNetApp->GetPrimaryNetMonitored(&mpDSSNetMonitored);
  if (AEE_SUCCESS != res) { 
    LOG_MSG_ERROR_1(
"Failed to fetch NetMonitored from NetApp object: %d", res);

    return res;
  }

  return AEE_SUCCESS;
}

inline AEEResult DSSNetMonitoredScope::Init(
  DSSNetApp* pDSSNetApp,
  dss_iface_id_type ifaceId,
  bool bAllowToCreateSecondary)
{   
  AEEResult res = AEE_SUCCESS;

  res = pDSSNetApp->GetNetMonitoredByIfaceId(
    ifaceId,
    bAllowToCreateSecondary,
    &mpDSSNetMonitored);

  if (AEE_SUCCESS != res) { 
    LOG_MSG_ERROR_1(
"Failed to fetch NetMonitored from NetApp object: %d", res);
    return res;
  }

  return res;
}

inline DSSNetMonitored* DSSNetMonitoredScope::Fetch()
{   
  return mpDSSNetMonitored;
}

/*lint -e{1551} */
inline DSSNetMonitoredScope::~DSSNetMonitoredScope()
{
  DSSCommon::ReleaseIf((IQI**)&mpDSSNetMonitored);
}
/*lint –restore */

#endif // __DSS_NETMONITOREDSCOPE_H__ 
