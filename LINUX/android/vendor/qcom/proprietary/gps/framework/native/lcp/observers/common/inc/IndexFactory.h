/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IClientIndex

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_INDEXFACTORY_H__
#define __IZAT_MANAGER_INDEXFACTORY_H__


namespace izat_manager
{

// Forward declarations
template <typename CT, typename DIT>
class IClientIndex;

template <typename CT, typename DIT>
class IDataItemIndex;

template <typename CT, typename DIT>
class IndexFactory {

public:
    static IClientIndex <CT, DIT> * createClientIndex ();
    static IDataItemIndex <CT, DIT> * createDataItemIndex ();
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_INDEXFACTORY_H__
