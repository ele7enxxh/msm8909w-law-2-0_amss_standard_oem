#ifndef DS_SOCK_SOCKETIPSEC_H
#define DS_SOCK_SOCKETIPSEC_H
/*===========================================================================
  @file ds_Sock_SocketIPSec.h

  This file defines a set of helper routines for IPSec related functionality.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_SocketIPSec.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Sock_Socket.h"
#include "ps_rt_meta_info.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class SocketIPSec
    {                                                         /*lint !e578 */
      public:
        static bool IsHandleInIfaceList
        (
          int32                   ifaceHandle,
          ps_rt_meta_info_type *  rtMetaInfoPtr
        );

        static bool IsIfaceListInReqState
        (
          ps_rt_meta_info_type *  rtMetaInfoPtr,
          uint32                  reqIfaceState
        );

        static void ClearIfaceList
        (
          ps_rt_meta_info_type *  rtMetaInfoPtr
        );

        static void TearDownIfaceList
        (
          ps_rt_meta_info_type *  rtMetaInfoPtr
        );

        static bool IsIfaceListFlowEnabled
        (
          ps_rt_meta_info_type *  rtMetaInfoPtr
        );

        static void FltrIPSecClient
        (
          ds::Sock::Socket *      sockPtr,
          ps_rt_meta_info_type *  newRtMetaInfoPtr,
          ps_rt_meta_info_type *  oldRtMetaInfoPtr
        );
    }; /* class SocketIPSec */

  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_SOCKETIPSEC_H */
