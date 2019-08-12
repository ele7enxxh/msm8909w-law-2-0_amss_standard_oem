/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemQuery

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMQUERY_H__
#define __IZAT_MANAGER_IDATAITEMQUERY_H__

#include  <list>
#include <DataItemId.h>


namespace izat_manager {

class IDataItem;

/**
 * @brief IDataItemQuery interface
 * @details IDataItemQuery interface;
 *          Defines an interface so that syncrhonous query of Data items values can be performed.
 *          This interface may or may not be implemented by OS dependent code.
 */
class IDataItemQuery {

public:
    /**
     * @brief Query a list of data item values by their ids
     * @details Query a list of data item values by their ids
     *
     * @param l List of DataItemId
     * @param out List of pointers to IDataItem
     * @return Zero for success or non zero for failure
     */
    virtual int query (const std :: list <DataItemId> & l, std :: list <IDataItem *> & out) = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IDataItemQuery () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMQUERY_H__
