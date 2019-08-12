/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCClientRegistration

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#ifndef __IZAT_MANAGER_IIPCCLIENTREGISTRATION_H__
#define __IZAT_MANAGER_IIPCCLIENTREGISTRATION_H__

#include <string>

namespace izat_manager {

// Forward declarations
class IDataItemSubscription;

// Interface to register with IPCClient
class IIPCClientRegistration {
public:
    // If you get messages for std :: string, call on pointer
    virtual void reg
    (
        const std :: string & name,
        IDataItemSubscription * oIface
    ) = 0;

    // dtor
    virtual ~IIPCClientRegistration () {}
};

}

#endif // #ifndef __IZAT_MANAGER_IIPCCLIENTREGISTRATION_H__
