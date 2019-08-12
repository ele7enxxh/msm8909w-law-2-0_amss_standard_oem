#ifndef _DSMIP_CFG_H
#define _DSMIP_CFG_H

/*===========================================================================

                         D S M I P _ C F G . H

DESCRIPTION

 Configurable parameters for A/DMSS Mobile IP client

Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.h_v   1.9   10 Oct 2002 15:54:16   jayanthm  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_cfg.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/16/02    jd     Created module
===========================================================================*/

#include "datamodem_variation.h"
#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================

                             PUBLIC CONSTANTS

===========================================================================*/

/*---------------------------------------------------------------------------
  MIP_MAX_SESSIONS is the number of MIP sessions allowed to run
  concurrently on the mobile.  This is hard coded to one, as multiple
  sessions are not supported in the current software revision.
---------------------------------------------------------------------------*/
#define MIP_MAX_SESSIONS  (1)

#else 

/*---------------------------------------------------------------------------
  This must be defined even if MIP is unfeaturized, so sockets module can
  determine the number of scb's required for the system.
---------------------------------------------------------------------------*/
#define MIP_MAX_SESSIONS  (0)

#endif /* FEATURE_DS_MOBILE_IP */

#endif /* _DSMIP_CFG_H */
