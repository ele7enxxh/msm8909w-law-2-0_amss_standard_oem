/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  ClientIndex

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_CLIENTINDEX_H__
#define __IZAT_MANAGER_CLIENTINDEX_H__

#include <list>
#include <map>
#include <IClientIndex.h>


namespace izat_manager
{


template <typename CT, typename DIT>

class ClientIndex : public IClientIndex  <CT, DIT> {

public:

    ClientIndex ();

    ~ClientIndex ();

    bool isSubscribedClient (CT client);

    void getSubscribedList (CT client, std :: list <DIT> & out);

    int remove (CT client);

    void remove (const std :: list <DIT> & r, std :: list <CT> & out);

    void remove (CT client, const std :: list <DIT> & r, std :: list <DIT> & out);

    void add (CT client, const std :: list <DIT> & l, std :: list <DIT> & out);

private:
    //Data members
    std :: map < CT , std :: list <DIT> > mDataItemsPerClientMap;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_CLIENTINDEX_H__
