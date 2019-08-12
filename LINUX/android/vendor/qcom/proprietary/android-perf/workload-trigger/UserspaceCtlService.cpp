/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtlService.cpp
  @brief  Server side implementation for FDCC binder
*******************************************************************************/

#include "UserspaceCtlService.h"

#undef LOG_TAG
#define LOG_TAG "FDCC-CTL-SERVICE"

using namespace android;
using namespace userspaceCtlCore;

namespace userspaceCtl {
UserspaceCtlService UserspaceCtlService::sUserspaceCtlService;

// Service
status_t BnUserspaceCtl::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
    status_t ret = NO_ERROR;
    int32_t inData = 0;

    data.checkInterface(this);

    UserspaceCtl& userspaceCtlObj = UserspaceCtl::getInstance();

    switch (code) {
        case DISP_HAL:
            inData = data.readInt32();
            QLOGI("Got %d data from DISP_HAL", inData);
            userspaceCtlObj.Feed(inData);
            break;
        case VID_HAL:
            break;
        case AUD_HAL:
            break;
        default:
            ret = BBinder::onTransact(code, data, reply, flags);
            break;
    } // switch (code)
    return ret;
}

void UserspaceCtlService::Run() {
    UserspaceCtl& userspaceCtlObj = UserspaceCtl::getInstance();
    userspaceCtlObj.Run();
}

UserspaceCtlService& UserspaceCtlService::getInstance() {
    return sUserspaceCtlService;
}

UserspaceCtlService::UserspaceCtlService() {
}

UserspaceCtlService::~UserspaceCtlService() {
}

void UserspaceCtlService::Dispatch(int32_t data __attribute__((unused))) {
}

}; // namespace userspaceCtl

