/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Implementation of OSObserver

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_OSObserver"

#include <cstdlib>
#include <new>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <MsgTask.h>
#include <IDataItemSubscription.h>
#include <IDataItemSubscription.h>
#include <OSObserver.h>
#include <IOSFramework.h>
#include <IzatContext.h>
#include <IzatDefines.h>
#include <DataItemId.h>
#include <IDataItem.h>
#include <DataItemsFactory.h>
#include <postcard.h>
#include <IDataItemSerializer.h>
#include <IDataItemCopier.h>
#include <IClientIndex.h>
#include <IDataItemIndex.h>
#include <IDataItemStringifier.h>
#include <IndexFactory.h>
#include <IPCClient.h>
#include <IPCHandler.h>

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;

class IPCHandler;


/******************* Init and Cleanup BEGIN **************************/


OSObserver * OSObserver :: mInstance = NULL;


OSObserver * OSObserver :: getInstance (struct s_IzatContext * izatContext)
{
    if (!mInstance) {
        // Instantiating for the first time
        if (  (izatContext->mOSFrameworkObj == NULL) ||
             (izatContext->mIPCMessagingProxyObj == NULL) ) {
            return NULL;
        }
        mInstance = new  (nothrow) OSObserver (izatContext);
    }
    return mInstance;
}


void OSObserver :: destroyInstance ()
{
    delete mInstance;
    mInstance = NULL;
}


OSObserver :: OSObserver
(
    struct s_IzatContext * izatContext
)
:
mAddress ("OS-Agent"),
mIzatContext (izatContext),
mClientIndex
(
    IndexFactory <IDataItemObserver *, DataItemId> :: createClientIndex ()
),
mDataItemIndex
(
    IndexFactory <IDataItemObserver *, DataItemId> :: createDataItemIndex ()
)
{
    int result = -1;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, mClientIndex);
        BREAK_IF_ZERO (2, mDataItemIndex);
        BREAK_IF_ZERO (3, mIzatContext->mSubscriptionObj);

        // init the IPCHandler
        IPCHandler *pIPCHandler =
            IPCHandler :: getInstance(mIzatContext->mIPCMessagingProxyObj,
            mIzatContext->mMsgTask);
        BREAK_IF_ZERO(4, pIPCHandler);

        pIPCHandler->reg(mAddress, this);
        result = 0;
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}

OSObserver :: ~OSObserver ()
{
    // Destroy cache
    map <DataItemId, IDataItem *> :: iterator citer = mCache.begin ();
    for (; citer != mCache.end (); ++citer) {
        if (citer->second != NULL) { delete citer->second; }
    }
    mCache.clear ();
    delete mClientIndex;
    delete mDataItemIndex;
    mClientIndex = NULL;
    mDataItemIndex = NULL;

    IPCHandler::destroyInstance();
}



/******************* Init and Cleanup END **************************/



/******************* Messages BEGIN **************************/

// Ctors

inline OSObserver :: HandleMsgBase :: HandleMsgBase (OSObserver * parent)
:
mParent (parent)
{}


inline OSObserver :: HandleClientMsg :: HandleClientMsg
(
    OSObserver * parent,
    IDataItemObserver * client
)
:
mParent (parent),
mClient (client)
{}


inline OSObserver :: HandleSubscribeReq :: HandleSubscribeReq
 (OSObserver * parent, const list <DataItemId> & l, IDataItemObserver * client)
:
HandleClientMsg (parent, client), mDataItemList (l)
{}


inline OSObserver :: HandleUpdateSubscriptionReq :: HandleUpdateSubscriptionReq
 (OSObserver * parent, const list <DataItemId> & l, IDataItemObserver * client)
:
HandleClientMsg (parent, client), mDataItemList (l)
{}

inline OSObserver :: HandleRequestData :: HandleRequestData
 (OSObserver * parent, const list <DataItemId> & l, IDataItemObserver * client)
:
HandleClientMsg (parent, client), mDataItemList (l)
{}

inline OSObserver :: HandleUnsubscribeReq :: HandleUnsubscribeReq
 (OSObserver * parent, const list <DataItemId> & l, IDataItemObserver * client)
:
HandleClientMsg (parent, client), mDataItemList (l)
{}


inline OSObserver :: HandleUnsubscribeAllReq :: HandleUnsubscribeAllReq
 (OSObserver * parent, IDataItemObserver * client)
:
HandleClientMsg (parent, client)
{}


inline OSObserver :: HandleNotify :: HandleNotify
 (OSObserver * parent, list <IDataItem *> dlist)
:
HandleMsgBase (parent), mDList (dlist)
{}

// Dtors

inline OSObserver :: HandleMsgBase :: ~HandleMsgBase () {}

inline OSObserver :: HandleClientMsg :: ~HandleClientMsg () {}


inline OSObserver :: HandleSubscribeReq :: ~HandleSubscribeReq () {}


inline OSObserver :: HandleUpdateSubscriptionReq :: ~HandleUpdateSubscriptionReq
() {}

inline OSObserver :: HandleRequestData :: ~HandleRequestData
() {}

inline OSObserver :: HandleUnsubscribeReq :: ~HandleUnsubscribeReq () {}


inline OSObserver :: HandleUnsubscribeAllReq :: ~HandleUnsubscribeAllReq () {}


OSObserver :: HandleNotify :: ~HandleNotify () {
    list <IDataItem *> :: iterator it = mDList.begin ();
    for (; it != mDList.end (); ++it) {
        delete *it;
        *it = NULL;
    }
}

// proc

void OSObserver :: HandleSubscribeReq :: proc () const {

    int result = 0;
    ENTRY_LOG ();
    do {
        if (mDataItemList.empty ()) {
            LOC_LOGV("mDataItemList is empty. Nothing to do. Exiting");
            result = 0;
            break;
        }
        //mDataItemList.sort ();
        // Handle First Response
        list <DataItemId> pendingFirstResponseList;
        this->mParent->mClientIndex->add (this->mClient, mDataItemList, pendingFirstResponseList);

        // Do not send first response for only pendingFirstResponseList,
        // instead send for all the data items  (present in the cache) that
        // have been subscribed for each time.
        this->mParent->sendFirstResponse (mDataItemList, this->mClient);

        list <DataItemId> yetToSubscribeDataItemsList;
        this->mParent->mDataItemIndex->add (this->mClient, mDataItemList, yetToSubscribeDataItemsList);
        // Send subscription list to framework
        if (!yetToSubscribeDataItemsList.empty ()) {
            this->mParent->mIzatContext->mSubscriptionObj->subscribe
            (
                yetToSubscribeDataItemsList,
                this->mParent
            );
            LOC_LOGD ("Subscribe Request sent to framework for the following data items");
            this->mParent->logMe (yetToSubscribeDataItemsList);
        }

    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
    return;
}


void OSObserver :: HandleUpdateSubscriptionReq :: proc () const {
    int result = 0;
    ENTRY_LOG ();
    do {
        if (mDataItemList.empty ()) {
            LOC_LOGV("mDataItemList is empty. Nothing to do. Exiting");
            result = 0;
            break;
        }
        //mDataItemList.sort ();
        list <DataItemId> currentlySubscribedList;
        this->mParent->mClientIndex->getSubscribedList (this->mClient, currentlySubscribedList);
        list <DataItemId> removeDataItemList;
        set_difference (currentlySubscribedList.begin (), currentlySubscribedList.end (),
                       mDataItemList.begin (), mDataItemList.end (),
                       inserter (removeDataItemList,removeDataItemList.begin ()));
        // Handle First Response
        list <DataItemId> pendingFirstResponseList;
        this->mParent->mClientIndex->add (this->mClient, mDataItemList, pendingFirstResponseList);
        // Send First Response
        this->mParent->sendFirstResponse (pendingFirstResponseList, this->mClient);

        list <DataItemId> yetToSubscribeDataItemsList;
        this->mParent->mDataItemIndex->add (this->mClient, mDataItemList, yetToSubscribeDataItemsList);
        // Send subscription list to framework
        if (!yetToSubscribeDataItemsList.empty ()) {
            this->mParent->mIzatContext->mSubscriptionObj->subscribe
            (
                yetToSubscribeDataItemsList,
                this->mParent
            );
            LOC_LOGD ("Subscribe Request sent to framework for the following data items");
            this->mParent->logMe (yetToSubscribeDataItemsList);
        }

        list <DataItemId> unsubscribeList;
        list <DataItemId> unused;
        this->mParent->mClientIndex->remove (this->mClient, removeDataItemList, unused);

        if (!this->mParent->mClientIndex->isSubscribedClient (this->mClient)) {
            this->mParent->mDataItemIndex->remove (list <IDataItemObserver *> (1,this->mClient), unsubscribeList);
        }
        if (!unsubscribeList.empty ()) {
            // Send unsubscribe to framework
            this->mParent->mIzatContext->mSubscriptionObj->unsubscribe
            (
                unsubscribeList,
                this->mParent
            );
            LOC_LOGD ("Unsubscribe Request sent to framework for the following data items");
            this->mParent->logMe (unsubscribeList);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}

void OSObserver :: HandleRequestData :: proc () const {
    int result = 0;
    ENTRY_LOG ();

    do {
        if (mDataItemList.empty ()) {
            LOC_LOGV("mDataItemList is empty. Nothing to do. Exiting");
            result = 0;
            break;
        }
        //mDataItemList.sort ();
        list <DataItemId> yetToSubscribeDataItemsList;
        this->mParent->mClientIndex->add (this->mClient, mDataItemList, yetToSubscribeDataItemsList);
        this->mParent->mDataItemIndex->add (this->mClient, mDataItemList, yetToSubscribeDataItemsList);
        // Send subscription list to framework
        if (!mDataItemList.empty ()) {
            this->mParent->mIzatContext->mSubscriptionObj->requestData
            (
                mDataItemList,
                this->mParent
            );
            LOC_LOGD ("Subscribe Request sent to framework for the following data items");
            this->mParent->logMe (yetToSubscribeDataItemsList);
        }

    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}


void OSObserver :: HandleUnsubscribeReq :: proc () const {
    int result = 0;
    ENTRY_LOG ();
    do {
        if (mDataItemList.empty ()) {
            LOC_LOGV("mDataItemList is empty. Nothing to do. Exiting");
            result = 0;
            break;
        }
        //mDataItemList.sort ();
        list <DataItemId> unsubscribeList;
        list <DataItemId> unused;
        this->mParent->mClientIndex->remove (this->mClient, mDataItemList, unused);

        if (!this->mParent->mClientIndex->isSubscribedClient (this->mClient)) {
            this->mParent->mDataItemIndex->remove (list <IDataItemObserver *> (1,this->mClient), unsubscribeList);
        }
        if (!unsubscribeList.empty ()) {
            // Send unsubscribe to framework
            this->mParent->mIzatContext->mSubscriptionObj->unsubscribe
            (
                unsubscribeList,
                this->mParent
            );
            LOC_LOGD ("Unsubscribe Request sent to framework for the following data items");
            this->mParent->logMe (unsubscribeList);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}


void OSObserver :: HandleUnsubscribeAllReq :: proc () const {
    int result = 0;
    ENTRY_LOG ();
    do {
        list <IDataItemObserver *> clients (1, this->mClient);
        list <DataItemId> unsubscribeList;
        BREAK_IF_NON_ZERO (2, this->mParent->mClientIndex->remove (this->mClient));


        this->mParent->mDataItemIndex->remove (clients, unsubscribeList);
        if (!unsubscribeList.empty ()) {
            // Send unsubscribe to framework
            this->mParent->mIzatContext->mSubscriptionObj->unsubscribe
            (
                unsubscribeList,
                this->mParent
            );
            LOC_LOGD ("Unsubscribe Request sent to framework for the following data items");
            this->mParent->logMe (unsubscribeList);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}


void OSObserver :: HandleNotify :: getListOfClients
 (const list <DataItemId> & dlist, list <IDataItemObserver *> & clients ) const {

     list <DataItemId> :: const_iterator it = dlist.begin ();
     for (; it != dlist.end (); ++it) {
         list <IDataItemObserver *> clientList;
         this->mParent->mDataItemIndex->getListOfSubscribedClients ( (*it), clientList);
         list <IDataItemObserver *> :: iterator citer = clientList.begin ();
         for (; citer != clientList.end (); ++citer) {
             clients.push_back (*citer);
         }
         clientList.clear ();
     }
     // remove duplicates
     clients.unique ();
}


void OSObserver :: HandleNotify :: proc () const {

    int result = 0;
    ENTRY_LOG ();
    do {
        // Update Cache with received data items and prepare
        // list of data items to be sent.
        list <IDataItem *> :: const_iterator it = mDList.begin ();
        list <DataItemId> dataItemIdsToBeSent;
        for (; it != mDList.end (); ++it) {
            bool dataItemUpdated = false;
            this->mParent->updateCache (*it, dataItemUpdated);
            if (dataItemUpdated) {
                dataItemIdsToBeSent.push_back ( (*it)->getId ());
            }
        }

        list <IDataItemObserver *> clientList;
        this->getListOfClients (dataItemIdsToBeSent, clientList);
        list <IDataItemObserver *> :: iterator citer = clientList.begin ();
        // Send data item to all subscribed clients
        LOC_LOGD ("LocTech-Label :: OSOBSERVER :: Data Items Out");
        for (; citer != clientList.end (); ++citer) {
            do {
                list <DataItemId> dataItemIdsSubscribedByThisClient;
                list <DataItemId> dataItemIdsToBeSentForThisClient;
                this->mParent->mClientIndex->getSubscribedList (*citer, dataItemIdsSubscribedByThisClient);
                dataItemIdsSubscribedByThisClient.sort ();
                dataItemIdsToBeSent.sort ();
                set_intersection (dataItemIdsToBeSent.begin (),
                                 dataItemIdsToBeSent.end (),
                                 dataItemIdsSubscribedByThisClient.begin (),
                                 dataItemIdsSubscribedByThisClient.end (),
                                 inserter (dataItemIdsToBeSentForThisClient,
                                         dataItemIdsToBeSentForThisClient.begin ()));
                BREAK_IF_NON_ZERO (4,this->mParent->sendCachedDataItems (dataItemIdsToBeSentForThisClient, *citer));
                dataItemIdsSubscribedByThisClient.clear ();
                dataItemIdsToBeSentForThisClient.clear ();
            } while (0);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

/******************* Messages END **************************/



/*******************  Overrides BEGIN  **************************/


void OSObserver :: subscribe (const list <DataItemId> & l, IDataItemObserver * client) {
    int result = 0;
    ENTRY_LOG ();
    do {
        HandleSubscribeReq * msg = new  (nothrow) HandleSubscribeReq (this, l, client);
        mIzatContext->mMsgTask->sendMsg (msg);
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}


void OSObserver :: updateSubscription (const list <DataItemId> & l, IDataItemObserver * client) {
    int result = 0;
    ENTRY_LOG ();
    do {
        mIzatContext->mMsgTask->sendMsg (new  (nothrow) HandleUpdateSubscriptionReq (this, l, client));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}

void OSObserver :: requestData (const list <DataItemId> & l, IDataItemObserver * client) {
    int result = 0;
    ENTRY_LOG ();
    do {
        mIzatContext->mMsgTask->sendMsg (new  (nothrow) HandleRequestData (this, l, client));
    } while (0);

    EXIT_LOG_WITH_ERROR ("%d",result);
}
void OSObserver :: unsubscribe (const list <DataItemId> & l, IDataItemObserver * client) {
    int result = 0;
    ENTRY_LOG ();
    do {
        mIzatContext->mMsgTask->sendMsg (new  (nothrow) HandleUnsubscribeReq (this, l, client));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}


void OSObserver :: unsubscribeAll (IDataItemObserver * client) {
    int result = 0;
    ENTRY_LOG ();
    do {
        mIzatContext->mMsgTask->sendMsg (new  (nothrow) HandleUnsubscribeAllReq (this, client));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d",result);
}

void OSObserver :: getName (string & name) {
    name = mAddress;
}

void OSObserver :: notify (const list <IDataItem *> & dlist)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        list <IDataItem *> :: const_iterator it = dlist.begin ();
        list <IDataItem *> dataItemList;
        list <DataItemId> ids;
        LOC_LOGI ("LocTech-Label :: OSOBSERVER :: Data Items In");
        for (; it != dlist.end (); ++it) {
            if (*it != NULL) {
                string dv;
                (*it)->getStringifier ()->stringify (dv);
                LOC_LOGI ("LocTech-Value :: Data Item Value: %s", dv.c_str ());
                IDataItem * dataitem = DataItemsFactory :: createNewDataItem ( (*it)->getId ());
                BREAK_IF_ZERO (2, dataitem);
                // Copy contents into the newly created data item
                dataitem->getCopier ()->copy (*it);
                dataItemList.push_back (dataitem);
                ids.push_back ( (*it)->getId ());
            }
        }
        mIzatContext->mMsgTask->sendMsg (new  (nothrow) HandleNotify (this, dataItemList));
    } while  (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

/*******************  Overrides END  **************************/



/*******************  Helpers BEGIN  **************************/

void OSObserver :: logMe (const list <DataItemId> & l) {
    list <DataItemId> :: const_iterator it = l.begin ();
    for (;it != l.end (); ++it) {
        LOC_LOGD ("DataItem %d",*it);
    }
}


int OSObserver :: sendFirstResponse (const list <DataItemId> & l, IDataItemObserver * to) {
    int result = 0;
    ENTRY_LOG ();
    do {
        if (l.empty ()) {
            LOC_LOGV("list is empty. Nothing to do. Exiting");
            result = 0;
            break;
        }

        string clientName;
        to->getName (clientName);
        LOC_LOGD ("First response sent for the following data items To Client: %s", clientName.c_str());

        list <IDataItem *> dataItems;
        list <DataItemId> :: const_iterator diditer = l.begin ();
        for (; diditer != l.end (); ++diditer) {
            map <DataItemId, IDataItem*> :: const_iterator citer = mCache.find (*diditer);
            if (citer != mCache.end ()) {
                string dv;
                IDataItem * di = citer->second;
                di->getStringifier ()->stringify (dv);
                LOC_LOGD ("LocTech-Value :: Data Item: %s", dv.c_str ());
                dataItems.push_back (citer->second);
            }
        }
        if (dataItems.empty ()) {
            LOC_LOGV("No items to notify. Nothing to do. Exiting");
            result = 0;
            break;
        }

        // Notify Client
        to->notify (dataItems);

    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
    return result;
}


int OSObserver :: sendCachedDataItems (const list <DataItemId> & l, IDataItemObserver * to) {
    int result = 0;
    ENTRY_LOG ();
    do {
        list <IDataItem *> dataItems;
        list <DataItemId> :: const_iterator it = l.begin ();
        string clientName;
        to->getName (clientName);
        LOC_LOGD ("LocTech-Value :: To Client: %s", clientName.c_str ());
        for (; it != l.end (); ++it) {
            string dv;
            IDataItem * di = this->mCache [ (*it) ];
            di->getStringifier ()->stringify (dv);
            LOC_LOGD ("LocTech-Value :: Data Item: %s", dv.c_str ());
            dataItems.push_back (di);
        }

        to->notify (dataItems);

    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
    return result;
}


int OSObserver :: updateCache (IDataItem * d, bool &dataItemUpdated) {
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, d);
        // Check if data item exists in cache
        map <DataItemId, IDataItem*> :: iterator citer = mCache.find (d->getId ());
        if (citer == mCache.end ()) {
            // New data item; not found in cache
            IDataItem * dataitem = DataItemsFactory :: createNewDataItem (d->getId ());
            BREAK_IF_ZERO (2, dataitem);
            // Copy the contents of the data item
            dataitem->getCopier ()->copy (d);
            pair <DataItemId, IDataItem*> cpair (d->getId (), dataitem);
            // Insert in mCache
            mCache.insert (cpair);
            dataItemUpdated = true;
        } else {
            // Found in cache; Update cache if necessary
            BREAK_IF_NON_ZERO(3, citer->second->getCopier ()->copy (d, &dataItemUpdated));
        }

        if (dataItemUpdated) {
            LOC_LOGV("DataItem:%d updated:%d", d->getId (), dataItemUpdated);
        }
    } while (0);

    EXIT_LOG_WITH_ERROR ("%d", result);
    return result;
}

/*******************  Helpers END  **************************/

