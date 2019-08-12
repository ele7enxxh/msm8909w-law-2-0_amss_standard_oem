/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#include "XtraSubscriber.h"
#include "OSObserver.h"
#include "IzatDefines.h"
#include "IDataItem.h"
#include "DataItemId.h"
#include "IDataItemCopier.h"
#include "DataItemConcreteTypes.h"
#include "DataItemsFactory.h"
#include "XtraApis.h"

#define LOG_NDDEBUG 0

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "[XTRA2]XtraSubscriber"
#endif

using namespace izat_xtra;

namespace izat_manager
{

XtraSubscriber::XtraSubscriber(const struct s_IzatContext * izatContext) :
        mIzatContext(izatContext)
{
    ENTRY_LOG();

    subscribe(true);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

XtraSubscriber::~XtraSubscriber()
{
    ENTRY_LOG();

    subscribe(false);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void XtraSubscriber::subscribe(bool yes)
{
    ENTRY_LOG();

    // Subscription data list
    std::list<DataItemId> subItemIdList;
    subItemIdList.push_back(NETWORKINFO_DATA_ITEM_ID);
    subItemIdList.push_back(MCCMNC_DATA_ITEM_ID);

    if (yes) {
        LOC_LOGD(LOG_TAG" subscribe to NETWORKINFO, MCCMNC data items");

        mIzatContext->mOSObserverObj->subscribe(subItemIdList, this);

        // request data list
        LOC_LOGD(LOG_TAG" request to TAC data items");

        std::list<DataItemId> reqItemIdList;
        reqItemIdList.push_back(TAC_DATA_ITEM_ID);

        mIzatContext->mOSObserverObj->requestData(reqItemIdList, this);

    } else {
        LOC_LOGD(LOG_TAG" unsubscribe to NETWORKINFO, MCCMNC data items");

        mIzatContext->mOSObserverObj->unsubscribe(subItemIdList, this);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// IDataItemObserver overrides
void XtraSubscriber::getName (string & name)
{
    name = "XtraSubscriber";
}

void XtraSubscriber::notify(const std::list<IDataItem *> & dlist)
{
    ENTRY_LOG();

    mIzatContext->mMsgTask->sendMsg(new (nothrow) handleOsObserverUpdateMsg(this, dlist));

    EXIT_LOG_WITH_ERROR("%d", 0);
}

XtraSubscriber::handleOsObserverUpdateMsg::handleOsObserverUpdateMsg(XtraSubscriber* xtraSubscriber,
           const std::list<IDataItem *> & dataItemList) : mXtraSubscriberObj(xtraSubscriber)
{
    ENTRY_LOG();

    int result = -1;
    do {
        std::list<IDataItem *>::const_iterator it = dataItemList.begin();
        for (; it != dataItemList.end(); it++) {
            IDataItem *updatedDataItem = *it;

            IDataItem * dataitem = DataItemsFactory::createNewDataItem(updatedDataItem->getId());
            BREAK_IF_ZERO(2, dataitem);
            // Copy the contents of the data item
            dataitem->getCopier()->copy(updatedDataItem);

            mDataItemList.push_back(dataitem);
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void XtraSubscriber::handleOsObserverUpdateMsg::proc() const
{

    ENTRY_LOG();

    int result = -1;

    do {
        std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
        for (; it != mDataItemList.end(); it++) {
            IDataItem* dataItem = *it;
            switch (dataItem->getId())
            {
                case NETWORKINFO_DATA_ITEM_ID:
                {
                    NetworkInfoDataItem *networkInfo= reinterpret_cast<NetworkInfoDataItem*>(dataItem);

                    LOC_LOGD(LOG_TAG" NETWORKINFO connected=%d type=%d", networkInfo->mConnected, networkInfo->mType);
                    IXtraInfoCache& infoCache = getXtraInfoCache();
                    infoCache.updateConnectionStatus(networkInfo->mConnected, networkInfo->mType);
                }
                break;

                case TAC_DATA_ITEM_ID:
                {
                    TacDataItem *tac= reinterpret_cast<TacDataItem*>(dataItem);

                    LOC_LOGD(LOG_TAG" TAC=%s", tac->mValue.c_str());
                    IXtraInfoCache& infoCache = getXtraInfoCache();
                    infoCache.updateXtraTac(tac->mValue);
                }
                break;

                case MCCMNC_DATA_ITEM_ID:
                {
                    MccmncDataItem *mccmnc= reinterpret_cast<MccmncDataItem*>(dataItem);

                    LOC_LOGD(LOG_TAG" MCCMNC=%s", mccmnc->mValue.c_str());
                    IXtraInfoCache& infoCache = getXtraInfoCache();
                    infoCache.updateXtraMccmnc(mccmnc->mValue.c_str());
                }
                break;

                default:
                break;
            }
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

XtraSubscriber::handleOsObserverUpdateMsg::~handleOsObserverUpdateMsg()
{
    ENTRY_LOG();

    std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++)
    {
        delete *it;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

}
