/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemSerializer

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMSERIALIZER_H__
#define __IZAT_MANAGER_IDATAITEMSERIALIZER_H__

#include <comdef.h>

namespace qc_loc_fw {
    class OutPostcard;
}

namespace izat_manager {

using namespace ::qc_loc_fw;

class IDataItem;

/**
 * @brief IDataItemSerializer interface.
 * @details IDataItemSerializer interface.
 *
 * @param out Pointer to an instance of OutPostcard.
 * @return Zero for success or non zero for failure.
 */
class IDataItemSerializer {
public:
    /**
     * @brief Serialize.
     * @details Serialize.
     *
     * @param out Pointer to an instance of OutPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 serialize (OutPostcard * out) = 0;
    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemSerializer () {}
};

} //namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMSERIALIZER_H__
