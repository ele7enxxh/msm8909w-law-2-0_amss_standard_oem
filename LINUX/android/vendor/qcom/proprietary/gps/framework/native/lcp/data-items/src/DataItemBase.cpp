/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemBase

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#include <string>
#include <DataItemId.h>
#include <DataItemConcreteTypeFieldNames.h>
#include <DataItemConcreteTypeDefaultValues.h>
#include <DataItemBase.h>
#include <IDataItemSerializer.h>
#include <IDataItemDeSerializer.h>
#include <IDataItemCopier.h>

using namespace izat_manager;

// ctor
DataItemBase :: DataItemBase
 (
    DataItemId Id,
    IDataItemSerializer * s,
    IDataItemDeSerializer * d,
    IDataItemCopier * c,
    IDataItemStringifier * sr
)
:
mId (Id),
mSerializer (s),
mDeSerializer (d),
mCopier (c),
mStringifier (sr)
{}

// getId
DataItemId DataItemBase :: getId () { return mId; }

// getSerializer
IDataItemSerializer * DataItemBase :: getSerializer () { return mSerializer; }

// getDeSerializer
IDataItemDeSerializer * DataItemBase :: getDeSerializer () { return mDeSerializer; }

// getCopier
IDataItemCopier * DataItemBase :: getCopier () { return mCopier; }

// getStringifier
IDataItemStringifier * DataItemBase :: getStringifier () { return mStringifier; }

// dtor
DataItemBase :: ~DataItemBase () {
    delete mSerializer;
    mSerializer = NULL;
    delete mDeSerializer;
    mDeSerializer = NULL;
    delete mCopier;
    mCopier = NULL;
    delete mStringifier;
    mStringifier = NULL;
}

