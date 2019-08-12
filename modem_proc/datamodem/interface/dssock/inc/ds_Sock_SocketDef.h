#ifndef DS_SOCK_SOCKETDEF_H
#define DS_SOCK_SOCKETDEF_H
/*===========================================================================
  @file ds_Sock_SocketDef.h

  This file defines the class that implements the ISocket interface.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_SocketDef.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define  SO_KEEPALIVE_DEF_VAL         FALSE
#define  SO_REUSEADDR_DEF_VAL         FALSE
#define  SO_ERROR_ENABLE_DEF_VAL      FALSE
#define  TCP_NODELAY_DEF_VAL          FALSE
#define  TCP_DELAYED_ACK_DEF_VAL      FALSE
#define  TCP_SACK_DEF_VAL             FALSE
#define  TCP_TIMESTAMP_DEF_VAL        FALSE
#define  UDP_ENCAPS_DEF_VAL           FALSE
#define  IP_RECVIF_DEF_VAL            FALSE
#define  IP_RECVERR_DEF_VAL           FALSE
#define  IPV6_RECVERR_DEF_VAL         FALSE
#define  IP_MCAST_LOOP_DEF_VAL        FALSE
#define  SO_SKIP_ROUTE_SCOPE_DEF_VAL  FALSE
#define  SO_SNDBUF_DEF_VAL            (10 * 1024)
#define  SO_SNDBUF_MIN_VAL            536
#define  SO_SNDBUF_MAX_VAL            (256 * 1024)
#define  IP_TOS_DEF_VAL               0
#define  IP_TTL_DEF_VAL               255
#define  IP_MCAST_TTL_DEF_VAL         1
#define  IPV6_TCLASS_DEF_VAL          0
#define  TCP_MAXSEG_DEF_VAL           1460
#define  TCP_MAXSEG_MIN_VAL           536
#define  TCP_MAXSEG_MAX_VAL           (18 * 1024)

#ifndef FEATURE_HDR
  #define  SO_RCVBUF_DEF_VAL       (16 * 1024)
#else
#ifdef FEATURE_JCDMA_2
  #define  SO_RCVBUF_DEF_VAL       (44 * TCP_MAXSEG_DEF_VAL)
#else
  #define  SO_RCVBUF_DEF_VAL       (45 * 1024)
#endif /* FEATURE_JCDMA_2 */
#endif /* FEATURE_HDR */

#define  SO_RCVBUF_MIN_VAL        536
#define  SO_RCVBUF_MAX_VAL        (256 * 1024)

#define  MAX_SOCKET_FACTORY              1

#endif /* DS_SOCK_SOCKETDEF_H */
