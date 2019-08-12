/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemStringifierBase Implementation

    Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include <IDataItem.h>
#include <DataItemStringifierBase.h>

using namespace izat_manager;

// Ctor
DataItemStringifierBase :: DataItemStringifierBase
 (IDataItem * d)
:
mD (d)
{}

// Dtor
DataItemStringifierBase :: ~DataItemStringifierBase (){}

