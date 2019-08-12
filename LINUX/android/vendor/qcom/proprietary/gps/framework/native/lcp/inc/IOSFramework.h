/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IOSFramework

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IOSFRAMEWORK_H__
#define __IZAT_MANAGER_IOSFRAMEWORK_H__

#include <MsgTask.h>


namespace izat_manager {

class IDataItemObserver;
class IDataItemQuery;
class IDataItemSubscription;
class IOSLocationProvider;
class IOSFrameworkCleaner;

/**
 * @brief IOSFramework interface
 * @details Definition of an interface to be implemented by OS dependent code.
 *          An instance of an implementation of this interface is supplied to the Izat Subsystem
 *          at its initialization time.
 */
class IOSFramework {

public:

    /**
     * @brief Gets an instance of IDataItemSubscription
     * @details Gets an instance of IDataItemSubscription
     * @return Pointer to an instance of IDataItemSubscription
     */
    virtual IDataItemSubscription * getDataItemSubscriptionObj () = 0;

    /**
     * @brief Gets an instance of IDataItemQuery
     * @details Gets an instance of IDataItemQuery so that synchronous requests for Data items can be made
     * @return Pointer to an instance of IDataItemQuery
     */
    virtual IDataItemQuery * getDataItemQueryObj () = 0;

    /**
     * @brief Gets an instance of IOSLocationProvider
     * @details Gets an instance of IOSLocationProvider;
     *          This instance is the OS dependent instance of a network location provider
     * @return Pointer to an instance of IOSLocationProvider or NULL if OS network location provider does not exist.
     */
    virtual IOSLocationProvider * getOSNP () = 0;


    /**
     * @brief Gets an instance of IOSFrameworkCleaner
     * @details Gets an instance of IOSFrameworkCleaner;
     *          This instance of IOSFrameworkCleaner is responsible for freeing up memory in OS dependent code.
     * @return Pointer to an instance of IOSFrameworkcleaner
     */
    virtual IOSFrameworkCleaner * getOSFrameworkCleaner () = 0;

    /**
     * @brief Gets Virtual Machine Associator
     * @details Gets Virtual Machine associator
     */
    virtual LocMsg* getTaskSetupJob () = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IOSFramework   () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSFRAMEWORK_H__
