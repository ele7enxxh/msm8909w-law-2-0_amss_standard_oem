/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  LocationReport

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_ILOCATIONPROVIDER_H__
#define __IZAT_MANAGER_ILOCATIONPROVIDER_H__

#include <sys/types.h>
#include "ILocationResponse.h"

namespace izat_manager
{

class ILocationProvider {

public:

    typedef enum LocationRequestAction {
        LocationRequestAction_SingleShot,
        LocationRequestAction_Start,
        LocationRequestAction_Stop
    } LocationRequestAction;

    typedef struct {
        LocationRequestAction  action;
        uint32_t               intervalInMsec;
    } LocationRequest;

    virtual int setRequest(const LocationRequest *request) = 0;
    virtual void subscribe(const ILocationResponse * respObject) = 0;
    virtual void unsubscribe(const ILocationResponse * respObject) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual ~ILocationProvider() {}
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_ILOCATIONPROVIDER_H__
