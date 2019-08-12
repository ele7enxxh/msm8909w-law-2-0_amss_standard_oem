/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemsFactory

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSFACTORY_H__
#define __IZAT_MANAGER_DATAITEMSFACTORY_H__

#include <DataItemId.h>

namespace izat_manager
{

class IDataItem;

// DataItemsFactory Factory
/**
 * @brief DataItemsFactory class
 * @details DataItemsFactory class
 *
 */
class DataItemsFactory {

public:
    /**
     * @brief Create new data item (on heap)
     * @details Create new data item (on heap)
     *
     * @param id Data item id to be created
     * @return Pointer to a heap instance of IDataItem.
     */
    static IDataItem * createNewDataItem (DataItemId id);
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSFACTORY_H__
