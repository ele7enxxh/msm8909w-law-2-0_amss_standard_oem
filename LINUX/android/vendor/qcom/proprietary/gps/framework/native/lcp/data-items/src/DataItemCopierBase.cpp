/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemCopierBase Implementation

    Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include <IDataItem.h>
#include <DataItemCopierBase.h>

using namespace izat_manager;

// Ctor
DataItemCopierBase :: DataItemCopierBase
 (IDataItem * d)
:
mD (d)
{}

// Dtor
DataItemCopierBase :: ~DataItemCopierBase (){}

