/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemDeSerializer

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMDESERIALIZER_H__
#define __IZAT_MANAGER_IDATAITEMDESERIALIZER_H__

#include <comdef.h>

namespace qc_loc_fw {
    class InPostcard;
}

namespace izat_manager {

class IDataItem;

using namespace ::qc_loc_fw;

/**
 * @brief IDataItemSerializer interface.
 * @details IDataItemSerializer interface.
 *
 * @param in Pointer to an instance of InPostcard.
 * @return Zero for success or non zero for failure.
 */
class IDataItemDeSerializer {
public:
    /**
     * @brief Deserialize.
     * @details Deserialize.
     *
     * @param in Pointer to an instance of InPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 deSerialize (InPostcard * in) = 0;
    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemDeSerializer () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMDESERIALIZER_H__
