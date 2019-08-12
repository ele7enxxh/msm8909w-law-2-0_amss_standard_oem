/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemStringifier

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMSTRINGIFIER_H__
#define __IZAT_MANAGER_IDATAITEMSTRINGIFIER_H__

#include <string>

namespace izat_manager {

using namespace std;
class IDataItem;

/**
 * @brief IDataItemStringifier interface.
 * @details IDataItemStringifier interface.
 *
 */
class IDataItemStringifier {
public:
    /**
     * @brief Stringify.
     * @details Stringify.
     *
     * @param valueStr Reference to string.
     */
    virtual void stringify (string & valueStr) = 0;

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemStringifier () {}
};

} //namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMSTRINGIFIER_H__
