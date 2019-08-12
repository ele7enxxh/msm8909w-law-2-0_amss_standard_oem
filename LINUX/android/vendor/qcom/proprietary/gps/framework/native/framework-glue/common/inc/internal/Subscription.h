/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __SUBSCRIPTION_H__
#define __SUBSCRIPTION_H__

#include <IDataItemSubscription.h>
#include <IDataItemObserver.h>
#include <DataItemId.h>
#include <log_util.h>

using namespace izat_manager;

class Subscription : public IDataItemSubscription {

public:
    static IDataItemObserver *mObserverObj;

    static Subscription* getSubscriptionObj();
    static void destroyInstance();

    // IDataItemSubscription overrides
    void subscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void updateSubscription(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void requestData(const std :: list <DataItemId> & l, IDataItemObserver * client = NULL);
    void unsubscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void unsubscribeAll(IDataItemObserver * observerObj = NULL);

protected:

private:
    static Subscription *mSubscriptionObj;

    Subscription() {
      mObserverObj = NULL;
    }
    ~Subscription() {}
};

#endif // #ifndef __SUBSCRIPTION_H__
