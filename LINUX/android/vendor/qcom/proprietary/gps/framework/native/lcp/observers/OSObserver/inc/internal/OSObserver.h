/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OSObserver Class definition

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_OSOBSERVER_H__
#define __IZAT_MANAGER_OSOBSERVER_H__

#include <string>
#include <list>
#include <map>
#include <new>
#include <log_util.h>
#include <DataItemId.h>
#include <MsgTask.h>
#include <IDataItemObserver.h>
#include <IDataItemSubscription.h>
#include <IzatContext.h>

namespace izat_manager
{

// using declarations
using namespace std;
using namespace qc_loc_fw;

// Forward Declarations
class IDataItem;

template <typename CT, typename DIT>
class IClientIndex;

template <typename CT, typename DIT>
class IDataItemIndex;

class IPCHandler;

// Clients wanting to get data from OS/Framework would need to
// subscribe with OSObserver using IDataItemSubscription interface.
// Such clients would need to implement IDataItemObserver interface
// to receive data when it becomes available.
class OSObserver :
    public IDataItemObserver,
    public IDataItemSubscription {

public:

    // Static methods
    static OSObserver * getInstance (struct s_IzatContext * izatContext);
    static void destroyInstance ();

    // IDataItemObserver Overrides
    void getName (string & name);
    void notify (const list <IDataItem *> & dlist);

    // IDataItemSubscription Overrides
    void subscribe (const list <DataItemId> & l, IDataItemObserver * client);
    void updateSubscription
    (
        const list <DataItemId> & l,
        IDataItemObserver * client
    );
    void requestData
    (
        const std :: list <DataItemId> & l,
        IDataItemObserver * client
    );
    void unsubscribe (const list <DataItemId> & l, IDataItemObserver * client);
    void unsubscribeAll (IDataItemObserver * client);

private:

    // Nested types
    // Messages
    struct HandleMsgBase : public LocMsg {
        HandleMsgBase (OSObserver * parent);
        virtual ~HandleMsgBase ();
        // Data members
        OSObserver * mParent;
    };

    struct HandleClientMsg : public LocMsg {
        HandleClientMsg (OSObserver * parent, IDataItemObserver * client);
        virtual ~HandleClientMsg ();
        // Data Members
        OSObserver * mParent;
        IDataItemObserver * mClient;
    };

    struct HandleSubscribeReq : public HandleClientMsg  {
        HandleSubscribeReq (OSObserver * parent,
                           const list <DataItemId> & l,
                           IDataItemObserver * client);
        virtual ~HandleSubscribeReq ();
        void proc () const;
        // Data members
        const list <DataItemId> mDataItemList;
    };

    struct HandleUpdateSubscriptionReq : public HandleClientMsg  {
        HandleUpdateSubscriptionReq (OSObserver * parent,
                                    const list <DataItemId> & l,
                                    IDataItemObserver * client);
        virtual ~HandleUpdateSubscriptionReq ();
        void proc () const;
        // Data members
        const list <DataItemId> mDataItemList;
    };

    struct HandleRequestData : public HandleClientMsg {
       HandleRequestData (OSObserver * parent,
                          const list <DataItemId> & l,
                          IDataItemObserver * client);
       virtual ~HandleRequestData ();
       void proc () const;
       const list <DataItemId> mDataItemList;
    };

    struct HandleUnsubscribeReq : public HandleClientMsg  {
        HandleUnsubscribeReq (OSObserver * parent,
                             const list <DataItemId> & l,
                             IDataItemObserver * client);
        virtual ~HandleUnsubscribeReq ();
        void proc () const;
        // Data members
        const list <DataItemId> mDataItemList;
    };

    struct HandleUnsubscribeAllReq : public HandleClientMsg  {
        HandleUnsubscribeAllReq
        (
            OSObserver * parent,
            IDataItemObserver * client
        );
        virtual ~HandleUnsubscribeAllReq ();
        void proc () const;
    };

    struct HandleNotify : public HandleMsgBase {
        HandleNotify (OSObserver * parent, list <IDataItem *> dlist);
        virtual ~HandleNotify ();
        void getListOfClients
        (
            const list <DataItemId> & dlist,
            list <IDataItemObserver *> & clients
        ) const;
        void proc () const;
        // Data members
        list <IDataItem *> mDList;
    };

    // ctor
    OSObserver
    (
        struct s_IzatContext * izatContext
    );
    // dtor
    ~OSObserver ();

    // Helpers
    int sendFirstResponse
    (
        const list <DataItemId> & l,
        IDataItemObserver * to
    );

    int sendCachedDataItems
    (
        const list <DataItemId> & l,
        IDataItemObserver * to
    );

    int updateCache (IDataItem * d, bool &dataItemUpdated);
    void logMe (const list <DataItemId> & l);

    // Data members
    static OSObserver                                * mInstance;
    const string                                       mAddress;
    struct s_IzatContext                             * mIzatContext;
    IClientIndex <IDataItemObserver *, DataItemId>   * mClientIndex;
    IDataItemIndex <IDataItemObserver *, DataItemId> * mDataItemIndex;
    map < DataItemId, IDataItem * >                    mCache;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_OSOBSERVER_H__
