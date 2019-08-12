#ifndef IPCSVC_H
#define IPCSVC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipcsvc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/09  sun      Added IPCSVC_MMGSDI
08/31/09   yh      Added IPCSVC_ISS
06/03/09   rv      Added IPCSVC_IXCONTENTOMADRMMSG
04/02/07   jg      Added IPCSVC_SECIPS
02/07/07  vdr      Added IPCSVC_SECTEST
12/11/06   rv      Added IPCSVC_IXCONTENT_AMSS
10/27/06   gr      Removed DRM client IDs as these are no longer needed
10/26/06  vdr      Added DRM service IDs
09/20/06   rv      reverted back changes over written by #13
08/28/06   om      Added IXCONTENTPLAYER Svc
06/05/18   gr      Addition for SEC Diag Client/Server
06/04/21   gr      Addition for IxStream Client/Server
06/01/30   ws      Additions for Ix Clients/Servers
03/10/13   et      Created module.
04/03/04   et      Restore overwritten security services
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ipctypes.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define IPCSVC_MAKE_INTERNAL_SERVICE(o) (IPCSVC_SVC_FIRST_INTERNAL + o)
#define IPCSVC_MAKE_SERVICE(o)          (IPCSVC_SVC_FIRST_EXTERNAL + o)
#define IPCSVC_MAKE_BASE_MSG_ID(s)      (s<<16)
#define IPCSVC_GET_SVC_ID(o)            (((o) >> 16) & 0x0000ffff)

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===              G L O B A L   M E S S A G E   B A S E                    ===
===                                                                       ===
=============================================================================
=============================================================================
=============================================================================
**
** Messages are allocated in specific ranges according to the service to
** which they belong to.
**
** The message ID is a 32 bits number and constructed as follow:
**
**          0              16              32
**          |---------------|---------------|
**          |      SID      |      MID      |
**          |---------------|---------------|
**
** - The upper 16 bits contain the service ID defining the message (SID)
** 
** - The next 8 bits define the Interface ID (IFID) in the service
**   This number is allocated in sequence for a maximum of 255 interfaces
**   per service (0 is not allowed).
**
** - The lower 16 bits define the message ID (MID) in the service.
**   This number is partitioned by the service designer as they see fit.
**   (Message ID 0 is not allowed).
**
**/
#define IPCSVC_MSG_RESERVED_SID_ID                0x0000
#define IPCSVC_MSG_RESERVED_MID_ID                  0x00


/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===              G L O B A L   S E R V I C E S   R A N G E S              ===
===                                                                       ===
=============================================================================
=============================================================================
=============================================================================
** Services are divided in 3 main categories:
** 
** - RESERVED:
**     0 is reserved by the environment for internal use
** - INTERNAL:
**     Services (such as tsvc) provided by the environment
** - EXTERNAL:
**     Application services
**
** They are partitioned as follow:
**
**                              1023 (0x03ff) IPC_NODE_LAST_DYNAMIC_ID
**                                |
**   Dynamic IDS                  |
**                                |
**                               256 (0x0100) IPC_NODE_FIRST_DYNAMIC_ID
**                               255 (0x00ff) IPCSVC_SVC_LAST_EXTERNAL
**                                |
**   External Services            |
**                                |
**                                64 (0x0040) IPCSVC_SVC_FIRST_EXTERNAL
**                                63 (0x003f) IPCSVC_SVC_LAST_INTERNAL
**                                |
**   Internal Services            |
**                                |
**                                1  (0x0001) IPCSVC_SVC_FIRST_INTERNAL
**                                0  (0x0000) RESERVED
**
**/

/*
** Reserved Service ID */
#define IPCSVC_SVC_RESERVED           IPC_NODE_RESERVED_ID

/*
** Internal Services Range */
#define IPCSVC_SVC_INTERNAL_COUNT     63
#define IPCSVC_SVC_FIRST_INTERNAL     IPC_NODE_FIRST_STATIC_ID
#define IPCSVC_SVC_LAST_INTERNAL      IPCSVC_SVC_INTERNAL_COUNT

/*
** External Services Range */
#define IPCSVC_SVC_EXTERNAL_COUNT     (IPC_NODE_LAST_STATIC_ID     \
                                       -                           \
                                       IPCSVC_SVC_INTERNAL_COUNT)
#define IPCSVC_SVC_FIRST_EXTERNAL     (IPCSVC_SVC_LAST_INTERNAL+1)
#define IPCSVC_SVC_LAST_EXTERNAL      IPC_NODE_LAST_STATIC_ID


/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===                I N T E R N A L   S E R V I C E S                      ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*
** T S V C
*/
#define IPCSVC_TSVC               IPCSVC_MAKE_INTERNAL_SERVICE(0)
#define IPCSVC_TSVC_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_TSVC)


/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===                E X T E R N A L   S E R V I C E S                      ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*
** Security Services
*/
#define IPCSVC_SEC                    IPCSVC_MAKE_SERVICE(0)
#define IPCSVC_SEC_BASE_MSG_ID        IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SEC)
#define IPCSVC_SECSSL                 IPCSVC_MAKE_SERVICE(1)
#define IPCSVC_SECSSL_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECSSL)
#define IPCSVC_SECDRM                 IPCSVC_MAKE_SERVICE(2)
#define IPCSVC_SECDRM_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECDRM)
#define IPCSVC_IXHTTP                 IPCSVC_MAKE_SERVICE(3)
#define IPCSVC_IXHTTP_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXHTTP)
#define IPCSVC_QXAPI                  IPCSVC_MAKE_SERVICE(4)
#define IPCSVC_QXAPI_BASE_MSG_ID      IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_QXAPI)

#define IPCSVC_SECDIAG                IPCSVC_MAKE_SERVICE(6)
#define IPCSVC_SECDIAG_BASE_MSG_ID    IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECDIAG)

#define IPCSVC_SECRND                 IPCSVC_MAKE_SERVICE(7)
#define IPCSVC_SECRND_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECRND)
#define IPCSVC_SFS                    IPCSVC_MAKE_SERVICE(8)
#define IPCSVC_SFS_BASE_MSG_ID        IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SFS)
/* MAINLINE FEATURE_IPC_SECTEST - CMI */
#define IPCSVC_SECTEST                IPCSVC_MAKE_SERVICE(9)

#define IPCSVC_SECAGENT               IPCSVC_MAKE_SERVICE(10)
#define IPCSVC_SECAGENT_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECAGENT)
#define IPCSVC_AMSSAGENT              IPCSVC_MAKE_SERVICE(11)
#define IPCSVC_AMSSAGENT_BASE_MSG_ID  IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_AMSSAGENT)

#define IPCSVC_REX_TIMER_SIM          IPCSVC_MAKE_SERVICE(12)
#define IPCSVC_REX_TIMER_SIM_BASE_MSG_ID\
  IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_REX_TIMER_SIM)

/* MAINLINE FEATURE_SEC_IPC - CMI */
#define IPCSVC_SECIPS                 IPCSVC_MAKE_SERVICE(13)
#define IPCSVC_SECIPS_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SECIPS)

/* ISS, SIMLOCK server, SIM server are for 7x27 Symbian build */
#define IPCSVC_ISS                         IPCSVC_MAKE_SERVICE(14)
#define IPCSVC_ISS_BASE_MSG_ID             IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_ISS)
#define IPCSVC_SIMLOCK_SERVER              IPCSVC_MAKE_SERVICE(15)
#define IPCSVC_SIMLOCK_SERVER_BASE_MSG_ID  IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SIMLOCK_SERVER)
#define IPCSVC_SIM_SERVER                  IPCSVC_MAKE_SERVICE(16)
#define IPCSVC_SIM_SERVER_BASE_MSG_ID      IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_SIM_SERVER)
#define IPCSVC_MMGSDI                      IPCSVC_MAKE_SERVICE(17)
#define IPCSVC_MMGSDI_BASE_MSG_ID          IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_MMGSDI)

/*
** X X X   Test Service
*/
#define IPCSVC_XXX                    IPCSVC_MAKE_SERVICE(0x3f)
#define IPCSVC_XXX_BASE_MSG_ID        IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_XXX)
#define IPCSVC_IPCTEST_S              IPCSVC_MAKE_SERVICE(40)
#define IPCSVC_IPCTEST_S_BASE_MSG_ID  IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IPCTEST_S)
#define IPCSVC_IPCTEST_C              IPCSVC_MAKE_SERVICE(41)
#define IPCSVC_IPCTEST_C_BASE_MSG_ID  IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IPCTEST_C)

/*
**  AEE FILE Service
*/
#define IPCSVC_IXFILE                  IPCSVC_MAKE_SERVICE(50)
#define IPCSVC_IXFILE_BASE_MSG_ID      IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXFILE)

#define IPCSVC_IXFILEMGR               IPCSVC_MAKE_SERVICE(51)
#define IPCSVC_IXFILEMGR_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXFILEMGR)

#define IPCSVC_IXCONTENTBROWSER               IPCSVC_MAKE_SERVICE(52)
#define IPCSVC_IXCONTENTBROWSER_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTBROWSER)

#define IPCSVC_IXCONTENTMGR                   IPCSVC_MAKE_SERVICE(53)
#define IPCSVC_IXCONTENTMGR_BASE_MSG_ID       IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTMGR)

#define IPCSVC_IXCONTENT                      IPCSVC_MAKE_SERVICE(54)
#define IPCSVC_IXCONTENT_BASE_MSG_ID          IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENT)

#define IPCSVC_IXCONTENTPLAYER                IPCSVC_MAKE_SERVICE(55)
#define IPCSVC_IXCONTENTPLAYER_BASE_MSG_ID    IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTPLAYER)

#define IPCSVC_IXTOD                   IPCSVC_MAKE_SERVICE(56)
#define IPCSVC_IXTOD_BASE_MSG_ID       IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXTOD)

#define IPCSVC_IDIAG                   IPCSVC_MAKE_SERVICE(57)
#define IPCSVC_IDIAG_BASE_MSG_ID       IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IDIAG)

#define IPCSVC_IXDRMPERMISSIONSCONTAINER               IPCSVC_MAKE_SERVICE(58)
#define IPCSVC_IXDRMPERMISSIONSCONTAINER_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXDRMPERMISSIONSCONTAINER)

#define IPCSVC_IXDRMPERMISSION               IPCSVC_MAKE_SERVICE(59)
#define IPCSVC_IXDRMPERMISSION_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXDRMPERMISSION)

#define IPCSVC_IXSTREAM               IPCSVC_MAKE_SERVICE(60)
#define IPCSVC_IXSTREAM_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXSTREAM)

#define IPCSVC_IXSQL               IPCSVC_MAKE_SERVICE(61)
#define IPCSVC_IXSQL_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXSQL)

#define IPCSVC_IXCONTENTHANDLER             IPCSVC_MAKE_SERVICE(62)
#define IPCSVC_IXCONTENTHANDLER_BASE_MSG_ID IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTHANDLER)

#define IPCSVC_IXCONTENT_AMSS                     IPCSVC_MAKE_SERVICE(49)
#define IPCSVC_IXCONTENT__AMSS_BASE_MSG_ID        IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENT_AMSS)

#define IPCSVC_IXCONTENTRIGHTSINFO               IPCSVC_MAKE_SERVICE(63)
#define IPCSVC_IXCONTENTRIGHTSINFO_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTRIGHTSINFO)

#define IPCSVC_IXCONTENTMETAINFO               IPCSVC_MAKE_SERVICE(64)
#define IPCSVC_IXCONTENTMETAINFO_BASE_MSG_ID   IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTMETAINFO)

#define IPCSVC_IXBCASTCONTENT                 IPCSVC_MAKE_SERVICE(65)
#define IPCSVC_IXBCASTCONTENT_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXBCASTCONTENT)

#define IPCSVC_IXBCASTCONTENTCLIENT           IPCSVC_MAKE_SERVICE(66)
#define IPCSVC_IXBCASTCONTENTCLIENT_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXBCASTCONTENTCLIENT)

#define IPCSVC_IXMTPDRM						IPCSVC_MAKE_SERVICE(67)
#define IPCSVC_IXMTPDRM_BASE_MSG_ID         IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXMTPDRM)

#define IPCSVC_IXBCASTKEYTPORTSVC           IPCSVC_MAKE_SERVICE(68)
#define IPCSVC_IXBCASTKEYTPORTSVC_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXBCASTKEYTPORTSVC)

#define IPCSVC_IXSTREAMSOURCE                   IPCSVC_MAKE_SERVICE(69)
#define IPCSVC_IXSTREAMSOURCE_BASE_MSG_ID       IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXSTREAMSOURCE)

#define IPCSVC_IXCONTENTOMADRMMSG                  IPCSVC_MAKE_SERVICE(70)                       
#define IPCSVC_IXCONTENTOMADRMMSG_BASE_MSG_ID      IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTOMADRMMSG)

#define IPCSVC_IXDESCRAMBLER_S             IPCSVC_MAKE_SERVICE(160)
#define IPCSVC_IXDESCRAMBLER_S_BASE_MSG_ID IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXDESCRAMBLER_S)

#define IPCSVC_IXGBAMANAGER                IPCSVC_MAKE_SERVICE(161)
#define IPCSVC_IXGBAMANAGER_BASE_MSG_ID    IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXGBAMANAGER)

#define IPCSVC_IXCONTENTRIGHTSMGR                IPCSVC_MAKE_SERVICE(162)
#define IPCSVC_IXCONTENTRIGHTSMGR_BASE_MSG_ID    IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXCONTENTRIGHTSMGR)

#define IPCSVC_IXDRMRIGHTSMGR                 IPCSVC_MAKE_SERVICE(163)
#define IPCSVC_IXDRMRIGHTSMGR_BASE_MSG_ID     IPCSVC_MAKE_BASE_MSG_ID(IPCSVC_IXDRMRIGHTSMGR)
/*-------------------------------------------------------------------------*/

#endif /* IPCSVC_H */
