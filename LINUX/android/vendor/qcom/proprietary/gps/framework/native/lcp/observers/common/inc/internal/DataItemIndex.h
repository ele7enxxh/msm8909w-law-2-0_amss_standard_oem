/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemIndex

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMINDEX_H__
#define __IZAT_MANAGER__DATAITEMINDEX_H__

#include <list>
#include <map>
#include <IDataItemIndex.h>

namespace izat_manager
{

template <typename CT, typename DIT>

class DataItemIndex : public IDataItemIndex  <CT, DIT> {

public:

    DataItemIndex ();

    ~DataItemIndex ();

    void getListOfSubscribedClients (DIT id, std :: list <CT> & out);

    int remove (DIT id);

    void remove (const std :: list <CT> & r, std :: list <DIT> & out);

    void remove (DIT id, const std :: list <CT> & r, std :: list <CT> & out);

    void add (DIT id, const std :: list <CT> & l, std :: list <CT> & out);

    void add (CT client, const std :: list <DIT> & l, std :: list <DIT> & out);

private:
    std :: map < DIT, std :: list <CT> > mClientsPerDataItemMap;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMINDEX_H__
