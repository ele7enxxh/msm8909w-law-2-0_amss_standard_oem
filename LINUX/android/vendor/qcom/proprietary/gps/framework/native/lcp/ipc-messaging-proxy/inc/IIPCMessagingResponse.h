/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIPCMESSAGINGRESPONSE_H__
#define __IZAT_MANAGER_IIPCMESSAGINGRESPONSE_H__

#include <base_util/postcard.h>

namespace izat_manager
{

using namespace qc_loc_fw;

class IIPCMessagingResponse {

public:
  virtual void handleMsg(InPostcard * const in_card) = 0;

  virtual ~IIPCMessagingResponse() {}
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIPCMESSAGINGRESPONSE_H__
