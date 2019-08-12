/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemDeSerializerBase

   Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMDESERIALIZERBASE_H__
#define __IZAT_MANAGER_DATAITEMDESERIALIZERBASE_H__

#include <string>
#include <IDataItemDeSerializer.h>

namespace izat_manager {

// DataItemDeSerializerBase provides base functionality

class DataItemDeSerializerBase : public IDataItemDeSerializer {

public:
    DataItemDeSerializerBase (IDataItem * d, std::string cardName = "");
    virtual const std::string & getCardName () const;
    virtual bool hasCardName () const;
    virtual ~DataItemDeSerializerBase ();

protected:
    IDataItem * mD;

private:
    std::string mCardName;
    bool mHasCardName;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMDESERIALIZERBASE_H__
