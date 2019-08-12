/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemCopierBase

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMCOPIERBASE_H__
#define __IZAT_MANAGER_DATAITEMCOPIERBASE_H__

#include <IDataItemCopier.h>

namespace izat_manager {

class IDataItem;

// DataItemCopierBase provides base functionality
class DataItemCopierBase : public IDataItemCopier {

public:
    DataItemCopierBase (IDataItem * d);
    virtual ~DataItemCopierBase ();

protected:
    IDataItem * mD;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMCOPIERBASE_H__
