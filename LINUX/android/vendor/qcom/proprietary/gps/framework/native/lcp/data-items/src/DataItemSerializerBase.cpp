/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemSerializerBase

    Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#include <string>
#include <DataItemSerializerBase.h>

using namespace std;
using namespace izat_manager;

// Ctor
DataItemSerializerBase :: DataItemSerializerBase
 (IDataItem * d, string cardName)
:
mD (d),
mCardName (cardName),
mHasCardName (!mCardName.empty ())
{}

// Dtor
DataItemSerializerBase :: ~DataItemSerializerBase (){}

// getCardName
const string DataItemSerializerBase :: getCardName () const {
    return mCardName;
}

// hasCardName
bool DataItemSerializerBase :: hasCardName () const {
    return mHasCardName;
}


