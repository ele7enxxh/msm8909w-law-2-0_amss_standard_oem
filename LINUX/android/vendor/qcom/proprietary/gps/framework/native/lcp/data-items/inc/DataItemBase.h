/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemBase

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMBASE_H__
#define __IZAT_MANAGER_DATAITEMBASE_H__

#include <DataItemId.h>
#include <IDataItem.h>

namespace izat_manager
{

class IDataItemSerializer;
class IDataItemDeSerializer;
class IDataItemCopier;
class IDataItemStringifier;

/**
 * @brief DataItemBase class
 * @details DataItemBase class;
 *          Provides base functionality.
 *
 */
class DataItemBase : public IDataItem {
public:

    /**
     * @brief Constructor.
     * @details Constructor.
     *
     * @param Id Data Item ID.
     * @param s Pointer to an instance of IDataItemSerializer.
     * @param d Pointer to an instance of IDataItemDeSerializer.
     * @param c Pointer to an instance of IDataItemCopier.
     * @param sr Pointer to an instance of IDataItemStringifier.
     */
    DataItemBase (DataItemId Id, IDataItemSerializer * s = NULL, IDataItemDeSerializer * d = NULL, IDataItemCopier * c = NULL, IDataItemStringifier * sr = NULL);

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~DataItemBase ();

    /**
     * @brief Gets Data item id.
     * @details Gets Data item id.
     * @return Data item id.
     */
    virtual DataItemId getId ();

    /**
     * @brief Gets Serializer.
     * @details Gets Serializer.
     * @return Pointer to an instance of IDataItemSerializer.
     */
    virtual IDataItemSerializer * getSerializer ();

    /**
     * @brief Gets Deserializer.
     * @details Gets Deserializer.
     * @return Pointer to an instance of IDataItemDeSerializer.
     */
    virtual IDataItemDeSerializer * getDeSerializer ();

    /**
     * @brief Gets Copier.
     * @details Gets Copier.
     * @return Pointer to an instance of IDataItemCopier.
     */
    virtual IDataItemCopier * getCopier ();

    /**
     * @brief Gets Stringifier.
     * @details Gets Stringifier.
     * @return Pointer to an instance of IDataItemStringifier.
     */
    virtual IDataItemStringifier * getStringifier ();
protected:
    /**
     * Data item id.
     */
    DataItemId mId;

    /**
     * Serializer.
     */
    IDataItemSerializer * mSerializer;

    /**
     * Deserializer.
     */
    IDataItemDeSerializer * mDeSerializer;

    /**
     * Copier.
     */
    IDataItemCopier * mCopier;

    /**
     * Stringifier.
     */
    IDataItemStringifier * mStringifier;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMBASE_H__
