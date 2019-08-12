/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IClientIndex

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_ICLIENTINDEX_H__
#define __IZAT_MANAGER_ICLIENTINDEX_H__

#include <list>

namespace izat_manager
{

template  <typename CT, typename DIT>

class IClientIndex {
public:

    // Checks if client is subscribed
    virtual bool isSubscribedClient (CT client) = 0;

    // gets subscription list
    virtual void getSubscribedList (CT client, std :: list <DIT> & out) = 0;

    // removes an entry
    virtual int remove (CT client) = 0;

    // removes std :: list of data items and returns a list of clients
    // removed if any.
    virtual void remove
    (
        const std :: list <DIT> & r,
        std :: list <CT> & out
    ) = 0;

    // removes list of data items indexed by client and returns list
    // of data items removed if any.
    virtual void remove
    (
        CT client,
        const std :: list <DIT> & r,
        std :: list <DIT> & out
    ) = 0;

    // adds/modifies entry in  map and returns new data items added.
    virtual void add
    (
        CT client,
        const std :: list <DIT> & l,
        std :: list <DIT> & out
    ) = 0;

    // dtor
    virtual ~IClientIndex () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_ICLIENTINDEX_H__
