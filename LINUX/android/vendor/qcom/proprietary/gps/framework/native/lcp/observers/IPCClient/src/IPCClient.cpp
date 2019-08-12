/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCClient implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDDEBUG 0
#define LOG_TAG "IPCClient"

#include <algorithm>
#include <iterator>
#include <string>
#include <log_util.h>
#include <MsgTask.h>
#include <IzatDefines.h>
#include <DataItemId.h>
#include <IDataItem.h>
#include <DataItemsFactory.h>
#include <IDataItemSerializer.h>
#include <IDataItemCopier.h>
#include <IDataItemStringifier.h>
#include <postcard.h>
#include <IPCHandler.h>
#include <IPCClient.h>
#include <IndexFactory.h>

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;

// ctors
IPCClient :: IPCClient (const string &clientName,
                     MsgTask * msgTask, IPCHandler *ipcHandler)
:
mClientName(clientName),
mMsgTask (msgTask),
mIpcHandler(ipcHandler)
{}

IPCClient :: LocMsgBase :: LocMsgBase (IPCClient * parent)
: mParent (parent) {}

IPCClient :: NotifyLocMsg :: NotifyLocMsg
 (IPCClient * parent, list <IDataItem *> dlist)
:
LocMsgBase (parent), mDList (dlist)
{}


// dtors
IPCClient :: ~IPCClient () {
}

IPCClient :: LocMsgBase :: ~LocMsgBase () {}

IPCClient :: NotifyLocMsg :: ~NotifyLocMsg () {
    list <IDataItem *> :: iterator it = mDList.begin ();
    for (; it != mDList.end (); ++it) {
        if (*it) {
            delete *it;
            *it = NULL;
        }
    }
}

void IPCClient :: logMe (const list <DataItemId> & l) {
    list <DataItemId> :: const_iterator it = l.begin ();
    for (;it != l.end (); ++it) {
        LOC_LOGD ("DataItem %d",*it);
    }
}
void IPCClient :: NotifyLocMsg :: proc () const {

    ENTRY_LOG ();
    int result = 0;
    OutPostcard * outcard = NULL;

    do {
        if (mDList.empty ()) {
            result = 1;
            break;
        }

        outcard = OutPostcard :: createInstance ();
        BREAK_IF_ZERO (2, outcard);
        BREAK_IF_NON_ZERO (3, outcard->init ());
        BREAK_IF_NON_ZERO (4, outcard->addString ("FROM", "OS-Agent"));
        BREAK_IF_NON_ZERO (5, outcard->addString ("TO", this->mParent->mClientName.c_str ()));

        list <IDataItem*> :: const_iterator diter = mDList.begin ();
        for (; diter != mDList.end (); ++diter) {
            IDataItemSerializer * serializer = (*diter)->getSerializer ();
            if (serializer != NULL) {
                serializer->serialize (outcard);
            }
         }

        BREAK_IF_NON_ZERO (6, outcard->finalize ());
        BREAK_IF_NON_ZERO (7, this->mParent->mIpcHandler->send (outcard,
            this->mParent->mClientName.c_str ()));
    } while (0);

    delete outcard;
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void IPCClient :: getName (string & name) {
    name = mClientName;
}

void IPCClient :: notify (const list <IDataItem *> & dlist) {
    int result = 0;
    ENTRY_LOG ();
    do {
        if (dlist.empty ()) {
            result = 0;
            break;
        }

        list <IDataItem *> :: const_iterator it = dlist.begin ();
        list <IDataItem *> dataItemList;
        list <DataItemId> ids;
        for (; it != dlist.end (); ++it) {
            if (*it != NULL) {
                IDataItem * dataitem = DataItemsFactory :: createNewDataItem ( (*it)->getId ());
                BREAK_IF_ZERO (2, dataitem);
                // Copy contents into the newly created data item
                IDataItemCopier * copier = dataitem->getCopier ();
                if (copier != NULL) {
                    copier->copy (*it);
                }
                dataItemList.push_back (dataitem);
                ids.push_back ( (*it)->getId ());
            }
        }
        mMsgTask->sendMsg (new  (nothrow) NotifyLocMsg (this, dataItemList));
        LOC_LOGD ("OSObserver Notification received for the following data items at IPCClient %s", mClientName.c_str());
        logMe (ids);
    } while  (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}
