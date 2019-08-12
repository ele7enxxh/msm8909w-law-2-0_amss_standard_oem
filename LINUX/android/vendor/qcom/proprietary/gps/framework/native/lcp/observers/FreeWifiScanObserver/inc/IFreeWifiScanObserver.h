/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IFreeWifiScanObserver

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__
#define __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__

namespace qc_loc_fw {
class LOWIAsyncDiscoveryScanResultResponse;
}

namespace izat_manager {

using namespace qc_loc_fw;

// Implement this interface to get notified of wifi scans.
// use this interface with IFreeWifiScanSubscription
class IFreeWifiScanObserver {

public:

    virtual void notify
    (
        const LOWIAsyncDiscoveryScanResultResponse * response
    ) = 0;

    virtual ~IFreeWifiScanObserver () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IFREEWIFISCANOBSERVER_H__
