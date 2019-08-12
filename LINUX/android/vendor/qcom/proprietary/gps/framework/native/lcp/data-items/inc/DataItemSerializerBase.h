/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemSerializerBase

    Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSERIALIZERBASE_H__
#define __IZAT_MANAGER_DATAITEMSERIALIZERBASE_H__

#include <string>
#include <IDataItemSerializer.h>

namespace izat_manager
{

// DataItemSerializerBase provides base functionality
class DataItemSerializerBase : public IDataItemSerializer {

public:
    DataItemSerializerBase (IDataItem * d, std::string cardName = "");
    virtual const std::string getCardName () const;
    virtual bool hasCardName () const;
    virtual ~DataItemSerializerBase ();

protected:
    IDataItem * mD;

private:
    std::string mCardName;
    bool mHasCardName;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSERIALIZERBASE_H__
