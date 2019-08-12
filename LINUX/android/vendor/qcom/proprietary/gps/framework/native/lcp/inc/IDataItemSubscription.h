/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemSubscription

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMSUBSCRIPTION_H__
#define __IZAT_MANAGER_IDATAITEMSUBSCRIPTION_H__

#include  <list>
#include  <DataItemId.h>

namespace izat_manager
{

class IDataItemObserver;

/**
 * @brief IDataItemSubscription interface
 * @details IDataItemSubscription interface;
 *          Defines an interface for operations such as subscribe, unsubscribe data items by their IDs.
 *          Must be implemented by OS dependent code.
 *
 */
class IDataItemSubscription {

public:
    /**
     * @brief Subscribe for data items by their IDs
     * @details Subscribe for data items by their IDs;
     *          An IDataItemObserver implementer invokes this method to subscribe
     *          for a list of DataItems by passing in their Ids.
     *          A symbolic invocation of this method in the following order
     *          subscribe ( {1,2,3}, &obj), subscribe ( {2,3,4,5}, &obj)
     *          where the numbers enclosed in braces indicate a list of data item Ids
     *          will cause this class implementer to update its subscription list for
     *          &obj to only contain the following Data Item Ids 1,2,3,4,5.
     *
     * @param l List of DataItemId
     * @param o Pointer to an instance of IDataItemObserver
     */
    virtual void subscribe (const std :: list <DataItemId> & l, IDataItemObserver * o = NULL) = 0;

    /**
     * @brief Update subscription for Data items
     * @details Update subscription for Data items;
     *          An IDataItemObserver implementer invokes this method to update their
     *          subscription for a list of DataItems by passing in their Ids
     *          A symbolic invocation of this method in the following order
     *          updateSubscription ( {1,2,3}, &obj),updateSubscription ( {2,3,4,5}, &obj)
     *          where the numbers enclosed in braces indicate a list of data item Ids
     *          will cause this class implementer to update its subscription list for
     *          &obj to only contain the following Data Item Ids 2,3,4,5.
     *          Note that this method may or may not be called.
     *
     * @param l List of DataItemId
     * @param o Pointer to an instance of IDataItemObserver
     */
    virtual void updateSubscription (const std :: list <DataItemId> & l, IDataItemObserver * o = NULL) = 0;

    /**
     * @brief Request Data
     * @details Request Data
     *
     * @param l List of DataItemId
     * @param o Pointer to an instance of IDataItemObserver
     */
    virtual void requestData (const std :: list <DataItemId> & l, IDataItemObserver * o = NULL) = 0;

    /**
     * @brief Unsubscribe Data items
     * @details Unsubscrbe Data items;
     *          An IDataItemObserver implementer invokes this method to unsubscribe their
     *          subscription for a list of DataItems by passing in their Ids
     *          Suppose this class implementor has a currently active subscription list
     *          containing 1,2,3,4,5,6,7 for &obj then a symbolic invocation of this
     *          method in the following order
     *          unsubscribe ( {1,2,3}, &obj), unsubscribe (  {1,2,3,4}, &obj),
     *          unsubscribe ( {7}, &obj)
     *          where the numbers enclosed in braces indicate a list of data item Ids
     *          will cause this class implementer to update its subscription list for
     *          &obj to only contain the following data item id 5,6.
     *
     * @param l List of DataItemId
     * @param o Pointer to an instance of IDataItemObserver
     */
    virtual void unsubscribe (const std :: list <DataItemId> & l, IDataItemObserver * o = NULL) = 0;

    /**
     * @brief Unsubscribe all data items
     * @details Unsubscribe all data items
     *
     * @param o Pointer to an instance of IDataItemObserver
     */
    virtual void unsubscribeAll (IDataItemObserver * o = NULL) = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IDataItemSubscription () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMSUBSCRIPTION_H__

