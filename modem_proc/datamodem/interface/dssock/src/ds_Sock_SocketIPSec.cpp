/*===========================================================================
  FILE: ds_Sock_SocketIPSec.cpp

  OVERVIEW: This file provides implementation of the SocketIPSec class.

  DEPENDENCIES: None

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_SocketIPSec.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-06-15 sy  Featurized inclusion of secips.h
  2009-03-16 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include <stringl.h>

#ifdef FEATURE_SEC_IPSEC
extern "C"
{
#include "secips.h"
}
#endif /* FEATURE_SEC_IPSEC */

#include "ds_Sock_SocketIPSec.h"
#include "ds_Net_Platform.h"


using namespace ds::Sock;
using namespace ds::Error;
using namespace ds::Utils;


/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
bool SocketIPSec::IsHandleInIfaceList
(
  int32                   ifaceHandle,
  ps_rt_meta_info_type *  rtMetaInfoPtr
)
{
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type *  ipsecInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ipsecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( rtMetaInfoPtr));

  /*-------------------------------------------------------------------------
    Since data doesn't match IPSec filters all the time, check further only
    if iface_cnt != 0.

    Return true if
      1. Handle matches iface at index 0 OR
      2. iface_cnt is 2 and handle matched iface at index 1

    Theoretically iface_cnt can be any number, but in current configuration,
    it can't be more than 2
  -------------------------------------------------------------------------*/
  if (0 == ipsecInfoPtr->iface_cnt)
  {
    return false;
  }

  ASSERT( 2 >= ipsecInfoPtr->iface_cnt);

  if (ifaceHandle == (int32) ipsecInfoPtr->iface_list[0] ||
      ( 2 == ipsecInfoPtr->iface_cnt &&
        ifaceHandle == (int32) ipsecInfoPtr->iface_list[1]))
  {
    return true;
  }

  return false;
#else
  return false;
#endif /* FEATURE_SEC_IPSEC */

} /* SocketIPSec::IsHandleInIfaceList() */


bool SocketIPSec::IsIfaceListInReqState
(
  ps_rt_meta_info_type *  rtMetaInfoPtr,
  uint32                  reqIfaceState
)
{
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type     *  ipsecInfoPtr;
  int32                     ipsecIfaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ipsecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( rtMetaInfoPtr));

  /*-------------------------------------------------------------------------
    Since data doesn't match IPSec filters all the time, check further only
    if iface_cnt != 0.

    Return true if
      1. Handle matches iface at index 0 OR
      2. iface_cnt is 2 and handle matched iface at index 1

    Theoretically iface_cnt can be any number, but in current configuration,
    it can't be more than 2
  -------------------------------------------------------------------------*/
  if (0 == ipsecInfoPtr->iface_cnt)
  {
    return true;
  }

  ASSERT( 2 >= ipsecInfoPtr->iface_cnt);

  ipsecIfaceHandle = reinterpret_cast <int32> ( ipsecInfoPtr->iface_list[0]);
  if (0 != ( reqIfaceState & NetPlatform::GetIfaceState( ipsecIfaceHandle)))
  {
    if ( 2 == ipsecInfoPtr->iface_cnt)
    {
      ipsecIfaceHandle =
        reinterpret_cast <int32> ( ipsecInfoPtr->iface_list[1]);
      if (0 != ( reqIfaceState & NetPlatform::GetIfaceState( ipsecIfaceHandle)))
      {
        return true;
      }
    }
    else
    {
      return true;
    }
  }

  return false;
#else
  return true;
#endif /* FEATURE_SEC_IPSEC */

} /* SocketIPSec::IsIfaceListInReqState() */


void SocketIPSec::ClearIfaceList
(
  ps_rt_meta_info_type *  rtMetaInfoPtr
)
{
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type *  ipsecInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 != rtMetaInfoPtr)
  {
    ipsecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( rtMetaInfoPtr));

    memset( ipsecInfoPtr, 0, sizeof( ps_ipsec_info_type));
  }
#endif /* FEATURE_SEC_IPSEC */

  return;
} /* SocketIPSec::ClearIfaceList() */


void SocketIPSec::TearDownIfaceList
(
  ps_rt_meta_info_type *  rtMetaInfoPtr
)
{
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type     *  ipsecInfoPtr;
  int32                     idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ipsecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( rtMetaInfoPtr));

  for (idx = 0; idx < ipsecInfoPtr->iface_cnt; idx++)
  {
    (void) NetPlatform::IfaceTearDownCmd( (int32) ipsecInfoPtr->iface_list[idx],
                                          ipsecInfoPtr);
  }

  memset( ipsecInfoPtr, 0, sizeof( ps_ipsec_info_type));
#endif /* FEATURE_SEC_IPSEC */

  return;
} /* SocketIPSec::TearDownIfaceList() */


bool SocketIPSec::IsIfaceListFlowEnabled
(
  ps_rt_meta_info_type *  rtMetaInfoPtr
)
{
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type     *  ipsecInfoPtr;
  int32                     ipsecIfaceHandle;
  int32                     idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ipsecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( rtMetaInfoPtr));

  for (idx = 0; idx < ipsecInfoPtr->iface_cnt; idx++)
  {
    ipsecIfaceHandle =
      reinterpret_cast <int32> ( ipsecInfoPtr->iface_list[idx]);

    if (false == NetPlatform::IsPSIfaceTxEnabled( ipsecIfaceHandle))
    {
      return false;
    }
  }
#endif /* FEATURE_SEC_IPSEC */

  return true;
} /* SocketIPSec::IsIfaceListFlowEnabled() */


void SocketIPSec::FltrIPSecClient
(
  Socket *                sockPtr,
  ps_rt_meta_info_type *  newRtMetaInfoPtr,
  ps_rt_meta_info_type *  oldRtMetaInfoPtr
)
{
#ifdef FEATURE_SEC_IPSEC
  ip_pkt_info_type *        pktInfoPtr;
  ps_ipsec_info_type     *  oldIPSecInfoPtr;
  ps_ipsec_info_type        ipsecInfo;
  ps_ipsec_info_type        tmpIPsecInfo = {0};
  uint32                    ipsecHandle;
  int32                     idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newRtMetaInfoPtr)
  {
    DATA_ERR_FATAL("SocketIPSec::FltrIPSecClient(): NULL args");
    return;
  }

  pktInfoPtr = &( PS_RT_META_GET_PKT_INFO( newRtMetaInfoPtr));

  /*-------------------------------------------------------------------------
    Initialize ipsecInfo in new rt meta info to 0 and perform filtering
  -------------------------------------------------------------------------*/
  memset( &( PS_RT_META_GET_IPSEC_INFO( newRtMetaInfoPtr)),
          0,
          sizeof( ps_ipsec_info_type));

  ipsecHandle =
    NetPlatform::IPFltrExecute( 0, IP_FLTR_CLIENT_IPSEC_OUTPUT, pktInfoPtr);

  /*-------------------------------------------------------------------------
    Generate fresh ipsecInfo if filters matched and update newRtMetaInfoPtr
  -------------------------------------------------------------------------*/
  memset( &ipsecInfo, 0, sizeof( ipsecInfo));

  if (0 != ipsecHandle)
  {
    (void) secips_generate_ipsec_info
           (
             reinterpret_cast <void *> ( ipsecHandle),
             pktInfoPtr,
             &ipsecInfo,
             reinterpret_cast <uint32> ( sockPtr)
           );

    PS_RT_META_SET_IPSEC_INFO( newRtMetaInfoPtr, ipsecInfo);

   /* TODO: check if both ESP/AH is set, AH should be protecting ESP as well
      so if AH is non-null then ESP need not be checked */
    /*-------------------------------------------------------------------------
     update ESP information in packet info
    -------------------------------------------------------------------------*/
    if (ipsecInfo.esp_sa_ptr != NULL)
    {
      if (pktInfoPtr->ip_vsn == IP_V6)
      {
        pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPPROTO_ESP;
      }
      else if (pktInfoPtr->ip_vsn == IP_V4)
      {
        pktInfoPtr->ip_hdr.v4.protocol = PS_IPPROTO_ESP;
      }
      else
      {
        LOG_MSG_ERROR_1("SocketIPSec::FltrIPSecClient(): "
                        "Invalid IP version", pktInfoPtr->ip_vsn);
      }

      pktInfoPtr->esp_hdr.spi = 
       ((secipssa_ipsec_sa_type*)ipsecInfo.esp_sa_ptr)->out.spi;
    }

    /*-------------------------------------------------------------------------
     update AH information in packet info
    -------------------------------------------------------------------------*/
    if (ipsecInfo.ah_sa_ptr != NULL)
    {
      if (pktInfoPtr->ip_vsn == IP_V6)
      {
        pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPPROTO_AH;
      }
      else if (pktInfoPtr->ip_vsn == IP_V4)
      {
        pktInfoPtr->ip_hdr.v4.protocol = PS_IPPROTO_AH;
      }
      else
      {
        LOG_MSG_ERROR_1("SocketIPSec::FltrIPSecClient(): "
                        "Invalid IP version", pktInfoPtr->ip_vsn);
      }

      pktInfoPtr->ah_hdr.spi = 
       ((secipssa_ipsec_sa_type*)ipsecInfo.ah_sa_ptr)->out.spi;
    }

  } /* if (0 != ipsecHandle) */

  /*-------------------------------------------------------------------------
    If new IPSEC filter result differs from old IPSEC filter result,
    tear down old IPSEC ifaces as they are no longer used.

    This has to happen even if new IPSEC filter result is 0 as even in this
    case old IPSEC filter result is no longer is used.
  -------------------------------------------------------------------------*/
  if (0 != oldRtMetaInfoPtr)
  {
    oldIPSecInfoPtr = &( PS_RT_META_GET_IPSEC_INFO( oldRtMetaInfoPtr));
  }
  else
  {
    oldIPSecInfoPtr = &tmpIPsecInfo;
  }

  /*lint -save -e644 */
  if (ipsecInfo.ipsec_handle != oldIPSecInfoPtr->ipsec_handle)
  {
    for (idx = 0; idx < oldIPSecInfoPtr->iface_cnt; idx++)
    {
      (void) NetPlatform::IfaceTearDownCmd
             (
               reinterpret_cast <int32> ( oldIPSecInfoPtr->iface_list[idx]),
               oldIPSecInfoPtr
             );
    }

    /*-----------------------------------------------------------------------
      If IPSEC filters are matched, bring up new IPSEC ifaces
    -----------------------------------------------------------------------*/
    if (0 != ipsecHandle)
    {
      for (idx = 0; idx < ipsecInfo.iface_cnt; idx++)
      {
        (void) NetPlatform::IfaceBringUpCmd
               (
                 reinterpret_cast <int32> ( ipsecInfo.iface_list[idx]),
                 &ipsecInfo
               );
      }
    }
  }
  /*lint -restore -e644 */
#endif /* FEATURE_SEC_IPSEC */

  return;
} /* SocketIPSec::FltrIPSecClient() */

