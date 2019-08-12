/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemObserver

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMOBSERVER_H__
#define __IZAT_MANAGER_IDATAITEMOBSERVER_H__

#include  <list>
#include <string>

namespace izat_manager
{

using namespace std;

class IDataItem;

/**
 * @brief IDataItemObserver interface
 * @details IDataItemObserver interface;
 *          In OS dependent code this type serves as a handle to an OS independent instance of this interface.
 */
class IDataItemObserver {

public:

    /**
     * @brief Gets name of Data Item Observer
     * @details Gets name of Data Item Observer
     *
     * @param name reference to name of Data Item Observer
     */
    virtual void getName (string & name) = 0;

    /**
     * @brief Notify updated values of Data Items
     * @details Notifys updated values of Data items
     *
     * @param dlist List of updated data items
     */
    virtual void notify (const std :: list <IDataItem *> & dlist) = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IDataItemObserver () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMOBSERVER_H__
