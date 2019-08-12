/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemStringifierBase

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSTRINGIFIERBASE_H__
#define __IZAT_MANAGER_DATAITEMSTRINGIFIERBASE_H__

#include <IDataItemStringifier.h>

namespace izat_manager {

class IDataItem;

// DataItemStringifierBase provides base functionality
class DataItemStringifierBase : public IDataItemStringifier {

public:
    DataItemStringifierBase (IDataItem * d);
    virtual ~DataItemStringifierBase ();

protected:
    IDataItem * mD;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSTRINGIFIERBASE_H__
