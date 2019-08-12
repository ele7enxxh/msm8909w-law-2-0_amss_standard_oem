/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_SUBSCRIBER_H__
#define __XTRA_SUBSCRIBER_H__

#include "IzatContext.h"
#include "IDataItemObserver.h"

namespace izat_manager
{

class XtraSubscriber : public IDataItemObserver{
public:
    XtraSubscriber(const struct s_IzatContext * izatContext);
    ~XtraSubscriber();

    // IDataItemObserver overrides
    virtual void getName (string & name) override;
    virtual void notify(const std::list<IDataItem *> & dlist) override;

private:
    struct handleOsObserverUpdateMsg : public LocMsg {
        XtraSubscriber *mXtraSubscriberObj;
        std::list <IDataItem *> mDataItemList;

        handleOsObserverUpdateMsg(XtraSubscriber *xtraSubscriberObj,
            const std::list<IDataItem *> & dataItemList);

        ~handleOsObserverUpdateMsg();
        virtual void proc() const;
    };

    const struct s_IzatContext * mIzatContext;
    void subscribe(bool yes);
};

}

#endif //__XTRA_SUBSCRIBER_H__
