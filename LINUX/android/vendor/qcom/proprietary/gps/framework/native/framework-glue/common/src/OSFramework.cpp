/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#define LOG_TAG "OSFramework"
#define LOG_NDEBUG 0

#include "utils/Log.h"
#include "OSFramework.h"
#include "utils_jni.h"
#include "Subscription.h"
#include "IzatDefines.h"
#include "OSFrameworkCleaner.h"
#include "OSNPGlue.h"
#include "NetworkLocationProviderGlue.h"
#include "FusedLocationProviderGlue.h"

OSFramework* OSFramework::mOSFramework = NULL;

OSFramework::OSFramework()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Create IzatManager
        IIzatManager *pIzatManager = getIzatManager(this);
        BREAK_IF_ZERO(1, pIzatManager);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

OSFramework::~OSFramework()
{
    ENTRY_LOG();

    // Destroy IzatManager
    (void) destroyIzatManager();

    EXIT_LOG_WITH_ERROR("%d", 0);
}

OSFramework* OSFramework::getOSFramework()
{
    int result = -1;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mOSFramework);

          mOSFramework = new (std::nothrow) OSFramework();
          BREAK_IF_ZERO(2, mOSFramework);

          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mOSFramework;
}

void OSFramework::destroyInstance()
{
    ENTRY_LOG();

    if (mOSFramework) {
        delete mOSFramework;
        mOSFramework = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// IOSFramework overrides
IDataItemSubscription* OSFramework::getDataItemSubscriptionObj()
{
    return Subscription::getSubscriptionObj();
}

IDataItemQuery* OSFramework::getDataItemQueryObj()
{
    return NULL;
}

IOSLocationProvider * OSFramework::getOSNP()
{
    return OSNPGlue::getInstance();
}

IOSFrameworkCleaner * OSFramework::getOSFrameworkCleaner()
{
    return OSFrameworkCleaner::getInstance();
}

LocMsg* OSFramework::getTaskSetupJob()
{
    struct AssociateVmMsg : public LocMsg {
        int (*mAssociator)() ;
        inline AssociateVmMsg(int (*associator)()) : mAssociator(associator) {
        }
        inline virtual void proc() const {
            mAssociator();
        }
    };
    return new AssociateVmMsg(&vmAssociator);
}
