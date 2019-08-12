/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  ClientIndex Implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include <algorithm>
#include <iterator>
#include <string>
#include <log_util.h>
#include <IzatDefines.h>
#include <ClientIndex.h>
#include <IDataItemObserver.h>
#include <DataItemId.h>

using namespace std;
using namespace izat_manager;

template <typename CT, typename DIT>
inline ClientIndex <CT,DIT> :: ClientIndex ()
{}

template <typename CT, typename DIT>
inline ClientIndex <CT,DIT> :: ~ClientIndex () {}

template <typename CT, typename DIT>
bool ClientIndex <CT,DIT> :: isSubscribedClient (CT client) {
    bool result = false;
    ENTRY_LOG ();
    typename map < CT, list <DIT> > :: iterator it =
        mDataItemsPerClientMap.find (client);
    if (it != mDataItemsPerClientMap.end ()) {
        result = true;
    }
    EXIT_LOG_WITH_ERROR ("%d",result);
    return result;
}

template <typename CT, typename DIT>
void ClientIndex <CT,DIT> :: getSubscribedList (CT client, list <DIT> & out) {
    ENTRY_LOG ();
    typename map < CT, list <DIT> > :: iterator it =
        mDataItemsPerClientMap.find (client);
    if (it != mDataItemsPerClientMap.end ()) {
        out = it->second;
    }
    EXIT_LOG_WITH_ERROR ("%d",0);
}

template <typename CT, typename DIT>
int ClientIndex <CT,DIT> :: remove (CT client) {
    int result = 0;
    ENTRY_LOG ();
    mDataItemsPerClientMap.erase (client);
    EXIT_LOG_WITH_ERROR ("%d",result);
    return result;
}

template <typename CT, typename DIT>
void ClientIndex <CT,DIT> :: remove (const list <DIT> & r, list <CT> & out) {
    ENTRY_LOG ();
    typename map < CT, list <DIT> > :: iterator dicIter =
        mDataItemsPerClientMap.begin ();
    while (dicIter != mDataItemsPerClientMap.end()) {
        typename list <DIT> :: const_iterator it = r.begin ();
        for (; it != r.end (); ++it) {
            typename list <DIT> :: iterator iter =
                find (dicIter->second.begin (), dicIter->second.end (), *it);
            if (iter != dicIter->second.end ()) {
                dicIter->second.erase (iter);
            }
        }
        if (dicIter->second.empty ()) {
            out.push_back (dicIter->first);
            // Post-increment operator increases the iterator but returns the
            // prevous one that will be invalidated by erase()
            mDataItemsPerClientMap.erase (dicIter++);
        } else {
            ++dicIter;
        }
    }
    EXIT_LOG_WITH_ERROR ("%d",0);
}

template <typename CT, typename DIT>
void ClientIndex <CT,DIT> :: remove
(
    CT client,
    const list <DIT> & r,
    list <DIT> & out
)
{
    ENTRY_LOG ();
    typename map < CT, list <DIT> > :: iterator dicIter =
        mDataItemsPerClientMap.find (client);
    if (dicIter != mDataItemsPerClientMap.end ()) {
        set_intersection (dicIter->second.begin (), dicIter->second.end (),
                         r.begin (), r.end (),
                         inserter (out,out.begin ()));
        if (!out.empty ()) {
            typename list <DIT> :: iterator it = out.begin ();
            for (; it != out.end (); ++it) {
                dicIter->second.erase (find (dicIter->second.begin (),
                                            dicIter->second.end (),
                                            *it));
            }
        }
        if (dicIter->second.empty ()) {
            mDataItemsPerClientMap.erase (dicIter);
            EXIT_LOG_WITH_ERROR ("%d",0);
        }
    }
    EXIT_LOG_WITH_ERROR ("%d",0);
}

template <typename CT, typename DIT>
void ClientIndex <CT,DIT> :: add
(
    CT client,
    const list <DIT> & l,
    list <DIT> & out
)
{
    ENTRY_LOG ();
    list <DIT> difference;
    typename map < CT, list <DIT> > :: iterator dicIter =
        mDataItemsPerClientMap.find (client);
    if (dicIter != mDataItemsPerClientMap.end ()) {
        set_difference (l.begin (), l.end (),
                       dicIter->second.begin (), dicIter->second.end (),
                       inserter (difference,difference.begin ()));
        if (!difference.empty ()) {
            difference.sort ();
            out = difference;
            dicIter->second.merge (difference);
            dicIter->second.unique ();
        }
    } else {
        out = l;
        pair < CT, list <DIT> > dicnpair (client, out);
        mDataItemsPerClientMap.insert (dicnpair);
    }
    EXIT_LOG_WITH_ERROR ("%d",0);
}

// Explicit instantiation must occur in same namespace where class is defined
namespace izat_manager
{
  template class ClientIndex <IDataItemObserver *, DataItemId>;
  template class ClientIndex <string, DataItemId>;
}
