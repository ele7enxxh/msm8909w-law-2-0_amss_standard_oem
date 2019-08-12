/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItem

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#ifndef __IZAT_MANAGER_IDATAITEM_H__
#define __IZAT_MANAGER_IDATAITEM_H__

#include <DataItemId.h>

namespace izat_manager {

class IDataItemSerializer;
class IDataItemDeSerializer;
class IDataItemCopier;
class IDataItemStringifier;

/**
 * @brief IDataItem interface.
 * @details IDataItem interface.
 *
 */

class IDataItem {
public:
    /**
     * @brief Gets Data item id.
     * @details Gets Data item id.
     * @return Data item id.
     */
    virtual DataItemId getId () = 0;
    /**
     * @brief Gets Serializer.
     * @details Gets Serializer.
     * @return Pointer to an instance of IDataItemSerializer.
     */
    virtual IDataItemSerializer * getSerializer () = 0;
    /**
     * @brief Gets Deserializer.
     * @details Gets Deserializer.
     * @return Pointer to an instance of IDataItemDeSerializer.
     */
    virtual IDataItemDeSerializer * getDeSerializer () = 0;
    /**
     * @brief Gets Copier.
     * @details Gets Copier.
     * @return Pointer to an instance of IDataItemCopier.
     */
    virtual IDataItemCopier * getCopier () = 0;
    /**
     * @brief Gets Stringifier.
     * @details Gets Stringifier.
     * @return Pointer to an instance of IDataItemStringifier.
     */
    virtual IDataItemStringifier * getStringifier () = 0;
    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItem () {}
};

} // namespace izat_manager

#endif // __IZAT_MANAGER_IDATAITEM_H__
