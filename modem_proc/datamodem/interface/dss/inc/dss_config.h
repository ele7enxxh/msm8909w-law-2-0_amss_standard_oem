#ifndef _DSS_CONFIG_H
#define _DSS_CONFIG_H

/*===========================================================================

             D A T A   S E R V I C E S   S O C K E T   C O N F I G
                            H E A D E R   F I L E

DESCRIPTION

 The Data Services Socket Configurations Header File.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/dss_config.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/06/09    pp     Created file as part CMI: Public/Private split.
                   (Split from dssocket_defs.h)
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Ranges of sockfd and sockfdbase:
  NOTE THAT THE  SOCKFDBASE_MIN_VAL SHOULD NOT EQUAL ZERO.
  Note these should be within range of sockfdbase.
---------------------------------------------------------------------------*/
#define SOCKFDBASE_MIN_VAL  DSS_MAX_SOCKS
#define SOCKFDBASE_MAX_VAL  ((1<<15) - 1 - DSS_MAX_SOCKS)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
/*---------------------------------------------------------------------------
                     Specify the number of applications
---------------------------------------------------------------------------*/
#define DSS_MAX_SYS_APPS     1 /* max no of sys apps, 1 for lingering sock */
#define DSS_MAX_APPS         (25 + DSS_MAX_SYS_APPS)     /* max no of apps */

/*---------------------------------------------------------------------------
  Constants to be defined for Multiple Sockets
    SOMAXCONN is the maximum number of unopened or half opened tcp sockets
        we will allow at any one time.
    MAX_TCP_SOCKS is the maximum number of tcp sockets total.  Each listening
        socket will use at least 2 of these, and will need backlog+1
        available for the listen call to succeed.
---------------------------------------------------------------------------*/
#define DSS_SOMAXCONN        (3)       /* Maximum value for listen backlog */
#define DSS_MAX_TCP_SOCKS    (20)           /* Maximum TCP sockets allowed */

#define DSS_MAX_UDP_APP_SOCKS (45)          /* Maximum UDP sockets allowed */

#else /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */
/*---------------------------------------------------------------------------
                     Specify the number of applications
---------------------------------------------------------------------------*/
#define DSS_MAX_SYS_APPS     1 /* max no of sys apps, 1 for lingering sock */
#define DSS_MAX_APPS         (3 + DSS_MAX_SYS_APPS)      /* max no of apps */

/*---------------------------------------------------------------------------
  Constants to be defined for Multiple Sockets
    SOMAXCONN is the maximum number of unopened or half opened tcp sockets
        we will allow at any one time.
    MAX_TCP_SOCKS is the maximum number of tcp sockets total.  Each listening
        socket will use at least 2 of these, and will need backlog+1
        available for the listen call to succeed.
---------------------------------------------------------------------------*/
#define DSS_SOMAXCONN        (3)       /* Maximum value for listen backlog */
#define DSS_MAX_TCP_SOCKS    (10)           /* Maximum TCP sockets allowed */


#ifdef FEATURE_DTV_DVBH
#define DSS_MAX_UDP_APP_SOCKS (25 + 20)
#else
#define DSS_MAX_UDP_APP_SOCKS (25)
#endif

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*-------------------------------------------------------------------------
  Maximum ping sessions
--------------------------------------------------------------------------*/
#define DSS_PING_MAX_PING_SESSIONS (10)

/*-------------------------------------------------------------------------
  Maximum number of sockets available system wide -- application sockets
  plus one for each MIP session (Currently set to 1).
--------------------------------------------------------------------------*/
#define DSS_MAX_UDP_SOCKS    (DSS_MAX_UDP_APP_SOCKS + 1)

/* Maximum ICMP sockets allowed */
#define DSS_MAX_ICMP_SOCKS   (2 + DSS_PING_MAX_PING_SESSIONS)

#define DSS_MAX_SOCKS       (DSS_MAX_TCP_SOCKS + DSS_MAX_UDP_SOCKS + \
                             DSS_MAX_ICMP_SOCKS)
#define DSS_MAX_TCBS         (DSS_MAX_TCP_SOCKS + 10)   /* Max # of TCBs */

#endif  /* _DSS_CONFIG_H */
