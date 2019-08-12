/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemIndex

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMINDEX_H__
#define __IZAT_MANAGER_IDATAITEMINDEX_H__

#include <list>

namespace izat_manager
{

template <typename CT, typename DIT>

class IDataItemIndex {

public:

    // gets std :: list of subscribed clients
    virtual void getListOfSubscribedClients
    (
        DIT id,
        std :: list <CT> & out
    ) = 0;

    // removes an entry from
    virtual int remove (DIT id) = 0;

    // removes list of clients and returns a list of data items
    // removed if any.
    virtual void remove
    (
        const std :: list <CT> & r,
        std :: list <DIT> & out
    ) = 0;

    // removes list of clients indexed by data item and returns list of
    // clients removed if any.
    virtual void remove
    (
        DIT id,
        const std :: list <CT> & r,
        std :: list <CT> & out
    ) = 0;

    // adds/modifies entry and returns new clients added
    virtual void add
    (
        DIT id,
        const std :: list <CT> & l,
        std :: list <CT> & out
    ) = 0;

    // adds/modifies entry and returns yet to subscribe list of data items
    virtual void add
    (
        CT client,
        const std :: list <DIT> & l,
        std :: list <DIT> & out
    ) = 0;

    // dtor
    virtual ~IDataItemIndex () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMINDEX_H__

