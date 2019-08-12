/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCClient

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IPCCLIENT_H__
#define __IZAT_MANAGER__IPCCLIENT_H__

#include <string>
#include <IDataItemObserver.h>
#include <MsgTask.h>
#include <DataItemId.h>
#include <IDataItem.h>

namespace qc_loc_fw {
class InPostcard;
}


namespace izat_manager {

// using declarations
using namespace std;
using namespace qc_loc_fw;

// Forward declarations
class IDataItem;

class IPCClient :
    public IDataItemObserver {

public:

    // ctor
    IPCClient (const string & clientName,
        MsgTask * msgTask, IPCHandler * ipcHandler);
    // dtor
    ~IPCClient ();

    // IDataItemObserver overrides
    virtual void getName (string & name);
    virtual void notify (const list <IDataItem *> & dlist);

private:

     // Loc Messages
    struct LocMsgBase : public LocMsg {
        LocMsgBase (IPCClient * parent);
        virtual ~LocMsgBase ();
        // Data members
        IPCClient * mParent;
    };

    struct NotifyLocMsg : public LocMsgBase {
        NotifyLocMsg (IPCClient * parent, list <IDataItem *> dlist);
        virtual ~NotifyLocMsg ();
        void proc () const;
        // Data members
        list <IDataItem *> mDList;
    };

    void logMe (const list <DataItemId> & l);

    // Data Members
    string                                 mClientName;
    MsgTask                              * mMsgTask;
    IPCHandler                           * mIpcHandler;
};

}

#endif // #ifndef __IZAT_MANAGER_IPCCLIENT_H__
