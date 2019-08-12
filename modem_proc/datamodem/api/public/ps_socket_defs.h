#ifndef PS_SOCKET_DEFS_H
#define PS_SOCKET_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S O C K E T _ D E F S . H

DESCRIPTION
  Header containing declarations pertaining to sockets which are used through
  layers.

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_socket_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/08    pp     Common Modem Interface: Public/Private API split.
06/13/05    vp     Definition of ps_sock_extended_err and additional constants.
04/18/05    vp     Definition of ps_route_scope_type and related definitions.
06/11/04    vp     Created the file.
===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Socket extended error structure for ICMP(v6) error reporting and related 
  constants.
---------------------------------------------------------------------------*/
#define PS_SO_EE_ORIGIN_LOCAL 1  /* Locally originated error               */
#define PS_SO_EE_ORIGIN_ICMP  2  /* ICMP error                             */ 
#define PS_SO_EE_ORIGIN_ICMP6 3  /* ICMPv6 error                           */ 

struct ps_sock_extended_err 
{ 
  uint32 ee_errno;                           /* error number               */
  uint8  ee_origin;                          /* where the error originated */ 
  uint8  ee_type;                            /* ICMP(v6) type              */ 
  uint8  ee_code;                            /* ICMP(v6) code              */ 
  uint8  ee_pad;                             /* padding                    */ 
  uint32 ee_info;                            /* additional information     */ 
  uint32 ee_data;                            /* other data                 */ 
  /* More data may follow                                                  */ 
}; 


#endif /* PS_SOCKET_DEFS_H */

