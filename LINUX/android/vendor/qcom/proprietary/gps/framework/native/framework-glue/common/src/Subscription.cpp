/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#define LOG_TAG "Subscription"
#define LOG_NDEBUG 0

#include "utils/Log.h"
#include "Subscription.h"
#include "Subscription_jni.h"
#include "IzatDefines.h"
#include <list>

Subscription* Subscription::mSubscriptionObj = NULL;
IDataItemObserver* Subscription::mObserverObj = NULL;

using namespace std;

// Subscription class implementation
Subscription* Subscription::getSubscriptionObj()
{
    int result = 0;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mSubscriptionObj);

          mSubscriptionObj = new (std::nothrow) Subscription();
          BREAK_IF_ZERO(2, mSubscriptionObj);
          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mSubscriptionObj;
}

void Subscription::destroyInstance()
{
    ENTRY_LOG();

    delete mSubscriptionObj;
    mSubscriptionObj = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

//IDataItemSubscription overrides
void Subscription::subscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

    update_subscribeJNI(l, true);
}

void Subscription::updateSubscription(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
}

void Subscription::requestData(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

    requestDataJNI(l);
}

void Subscription::unsubscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
     // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

    update_subscribeJNI(l, false);
}

void Subscription::unsubscribeAll(IDataItemObserver * observerObj)
{
     // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

    unsubscribeAllJNI();
}
