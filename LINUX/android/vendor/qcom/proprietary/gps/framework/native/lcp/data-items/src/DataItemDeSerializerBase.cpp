/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemDeSerializerBase Implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#include <string>
#include <DataItemDeSerializerBase.h>

using namespace izat_manager;

// Ctor
DataItemDeSerializerBase :: DataItemDeSerializerBase
 (IDataItem * d, std :: string cardName)
:
mD (d),
mCardName (cardName),
mHasCardName (!mCardName.empty ())
{}

// Dtor
DataItemDeSerializerBase :: ~DataItemDeSerializerBase (){}

// getCardName
const std :: string & DataItemDeSerializerBase :: getCardName () const {
    return mCardName;
}

// hasCardName
bool DataItemDeSerializerBase :: hasCardName () const {
    return mHasCardName;
}


