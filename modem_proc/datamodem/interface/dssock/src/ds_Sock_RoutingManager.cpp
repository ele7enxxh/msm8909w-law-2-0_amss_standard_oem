/*===========================================================================
  FILE: ds_Sock_RoutingManager.cpp

  OVERVIEW: This file provides implementation of the RoutingManager class.

  DEPENDENCIES: None

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_RoutingManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "amssassert.h"
#include <string.h>
#include "ds_Sock_RoutingManager.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_DebugMsg.h"

using namespace ds::Sock;
using namespace ds::Net;
using namespace ds::Error;


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
ds::ErrorType RoutingManager::RoutePacket
(
  ds::Sock::Socket *      sockPtr,
  bool                    isSystemSocket,
  IPolicy *               policyPtr,
  ps_rt_meta_info_type *  newRtMetaInfoPtr
)
{
  ip_pkt_info_type *  pktInfoPtr;
  int32               routingCache;
  int32               retVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newRtMetaInfoPtr)
  {
    LOG_MSG_ERROR_1("RoutingManager::RoutePacket(): "
                    "NULL meta info, sock 0x%x", sockPtr);
    ASSERT( 0);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Perform routing look up
  -------------------------------------------------------------------------*/
  pktInfoPtr = &( PS_RT_META_GET_PKT_INFO( newRtMetaInfoPtr));

  retVal = NetPlatform::RouteDataPathLookup( pktInfoPtr,
                                             isSystemSocket,
                                             policyPtr,
                                             FALSE,
                                             &routingCache);
  if (DSS_ERROR == retVal)
  {
    LOG_MSG_INFO1_1("RoutingManager::RoutePacket(): "
                    "Rt lookup failed, sock 0x%x", sockPtr);
    return QDS_ENOROUTE;
  }

  /*-------------------------------------------------------------------------
    Update rt meta info with routing cache
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_2("RoutingManager::RoutePacket(): "
                  "Rt cache 0x%x, sock 0x%x", routingCache, sockPtr);
  PS_RT_META_SET_ROUTING_CACHE( newRtMetaInfoPtr,
                                reinterpret_cast <void *> ( routingCache));

  return AEE_SUCCESS;

} /* RoutingManager::RoutePacket() */


void RoutingManager::FltrClient
(
  ds::Sock::Socket *                   sockPtr,
  ps_iface_ipfltr_client_id_enum_type  fltrClient,
  ps_rt_meta_info_type *               newRtMetaInfoPtr
)
{
  ip_pkt_info_type *  pktInfoPtr;
  int32               fltrResult;
  int32               routingCache;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("RoutingManager::FltrClient(): "
                           "Sock 0x%x fltr client %d", sockPtr, fltrClient);

  if (0 == newRtMetaInfoPtr)
  {
    DATA_ERR_FATAL("RoutingManager::FltrClient(): NULL args");
    return;
  }

  pktInfoPtr = &( PS_RT_META_GET_PKT_INFO( newRtMetaInfoPtr));
  routingCache =
    reinterpret_cast <int32> ( PS_RT_META_GET_ROUTING_CACHE( newRtMetaInfoPtr));

  switch (fltrClient)
  {
    case IP_FLTR_CLIENT_QOS_OUTPUT:
    {
      /*---------------------------------------------------------------------
        Filter QoS client
      ---------------------------------------------------------------------*/
      fltrResult =
        NetPlatform::IPFltrExecute( routingCache, fltrClient, pktInfoPtr);

      /*---------------------------------------------------------------------
        Update rt meta info with fltr result. Use default ps_flow if filters
        didn't match
      ---------------------------------------------------------------------*/
      if (0 == fltrResult)
      {
        (void) NetPlatform::PSGetDefaultFlow( routingCache, &fltrResult);
      }

      PS_RT_META_SET_FILTER_RESULT( newRtMetaInfoPtr, fltrClient, fltrResult);
      break;
    }

    case IP_FLTR_CLIENT_HEADER_COMP:
    {
      /*---------------------------------------------------------------------
        Filter HC client
      ---------------------------------------------------------------------*/
      fltrResult =
        NetPlatform::IPFltrExecute( routingCache, fltrClient, pktInfoPtr);

      /*---------------------------------------------------------------------
        Update rt meta info with fltr result
      ---------------------------------------------------------------------*/
      PS_RT_META_SET_FILTER_RESULT( newRtMetaInfoPtr, fltrClient, fltrResult);
      break;
    }

    case IP_FLTR_CLIENT_IPSEC_OUTPUT:
    default:
    {
      LOG_MSG_ERROR_1("RoutingManager::FltrClient(): "
                      "Unknown fltr client %d", fltrClient);
      ASSERT( 0);
      break;
    }
  }

  return;
} /* RoutingManager::FltrClient() */

