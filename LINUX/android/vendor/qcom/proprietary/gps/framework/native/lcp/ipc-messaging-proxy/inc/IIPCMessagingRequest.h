/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIPCMESSAGINGREQUEST_H__
#define __IZAT_MANAGER_IIPCMESSAGINGREQUEST_H__

#include <base_util/postcard.h>

namespace izat_manager
{

using namespace qc_loc_fw;

class IIPCMessagingResponse;

/**
  * IIPCMessagingRequest provides an interface to register with IIPCMessagingProxy object
  */

class IIPCMessagingRequest {

public:
  virtual int registerResponseObj(const char* listener_name, IIPCMessagingResponse* respObj) = 0;
  virtual int sendMsg(OutPostcard * const out_card, const char* msgTo) = 0;

  virtual ~IIPCMessagingRequest() {}
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIPCMESSAGINGREQUEST_H__
