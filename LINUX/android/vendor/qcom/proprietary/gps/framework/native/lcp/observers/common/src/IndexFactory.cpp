/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IndexFactory implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include <string>
#include <IndexFactory.h>
#include <ClientIndex.h>
#include <DataItemIndex.h>
#include <IDataItemObserver.h>
#include <DataItemId.h>

using namespace std;
using namespace izat_manager;

template <typename CT, typename DIT>
inline IClientIndex <CT, DIT> * IndexFactory <CT, DIT> :: createClientIndex
()
{
    return new (nothrow) ClientIndex <CT, DIT> ();
}

template <typename CT, typename DIT>
inline IDataItemIndex <CT, DIT> * IndexFactory <CT, DIT> :: createDataItemIndex
()
{
    return new (nothrow) DataItemIndex <CT, DIT> ();
}

// Explicit instantiation must occur in same namespace where class is defined
namespace izat_manager
{
  template class IndexFactory <IDataItemObserver *, DataItemId>;
  template class IndexFactory <string, DataItemId>;
}
