/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  IPCMessagingProxy class serves as a hub for sending /receiving inter-process messages
  via location-mq. This class will have the connection to location-mq and will
  send/receive messages on behalf of other clients.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/
#ifndef __IZAT_MANAGER_IPCMESSAGINGPROXY_H__
#define __IZAT_MANAGER_IPCMESSAGINGPROXY_H__

#include "IIPCMessagingRequest.h"
#include <cstdlib>
#include <mq_client/mq_client_controller.h>
#include <base_util/sync.h>
#include <map>
#include <string>

using namespace qc_loc_fw;
using namespace std;

namespace izat_manager
{

struct s_IzatContext;

class IPCMessagingProxy : public IIPCMessagingRequest, public qc_loc_fw::MqClientControllerBase {

public:
    static IPCMessagingProxy * getInstance();
    static void destroyInstance();

    // IIPCMessagingRequest overrides
    virtual int registerResponseObj(const char* listener_name, IIPCMessagingResponse* respObj);
    virtual int sendMsg(OutPostcard * const out_card, const char* msgTo);

protected:
  // MqClientControllerBase overrides
    int _init();
    void _process(qc_loc_fw::InPostcard * const in_msg);
    void _shutdown();
    void handleKill(qc_loc_fw::InPostcard * const in_msg);


private:
    typedef map<string, IIPCMessagingResponse *> MessageResponseMap;

    static IPCMessagingProxy * mInstance;
    bool mInitialized;
    MessageResponseMap mMsgResponseMap;
    Mutex * mMsgResponseMapMutex;
    static const std::string mSelfName;
    static const std::string mConnectionSocketName;

    IPCMessagingProxy();
    virtual ~IPCMessagingProxy();
    int registerWithMessagingServer(string & listener_name);
    int broadcastToAllClients(qc_loc_fw::InPostcard * const in_card);
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IPCMESSAGINGPROXY_H__
