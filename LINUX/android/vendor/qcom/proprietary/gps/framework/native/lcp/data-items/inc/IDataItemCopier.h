/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemCopier

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMCOPIER_H__
#define __IZAT_MANAGER_IDATAITEMCOPIER_H__

#include <comdef.h>

namespace izat_manager {

class IDataItem;


/**
 * @brief IDataItemCopier interface.
 * @details IDataItemCopier interface.
 */
class IDataItemCopier {
public:
    /**
     * @brief copy.
     * @details copy.
     *
     * @param src Where to copy from.
     * @param dataItemCopied Boolean flag indicated whether or not copied.
     *
     * @return Zero for success or non zero for failure.
     */
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL) = 0;

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemCopier () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMCOPIER_H__
