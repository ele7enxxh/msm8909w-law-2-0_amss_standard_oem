#ifndef SECSSLCONF_H
#define SECSSLCONF_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

    S E S S I O N   C O N F I G U R A T I O N   D E F I N I T I O N S

GENERAL DESCRIPTION
  The Secure Sockets Layer ...

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslconf.h_v   1.11   12 Jun 2003 10:45:24   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslconf.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
03/10/10   yh      TLS1.1
06/11/05   sv/ssm  changes to support EAP-TLS
07/15/02   rwh     Updated comment.
05/24/02   om      Added TLS1.0 as our default version.
06/01/01   om      Added logging support.
05/30/01   om      Changed max sessions to 2, turned Nagle off
12/12/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */ 
/*===========================================================================

                 SESSION CONFIGURATION DEFINITIONS

===========================================================================*/

/* Initial value for the SSL protocol version. 
SECSSL_SSL_VER_TLS10/11/12 can be negotiated down to SECSSL_SSL_VER_30.
Starting at SECSSL_SSL_VER_30 leaves no room for negotiation.
*/
#define SECSSLCONF_INITIAL_SSL_VERSION  SECSSL_SSL_VER_TLS12

/* Maximum Number of concurrent Sessions */
#define SECSSLCONF_MAX_CONC_SESSIONS     3

/* Period for logging the session state                  */
/* (only active if FEATURE_SEC_SSL_LOGGING is enabled).  */
#define SECSSLCONF_LOG_PERIOD_MS         10000

/* Nagle mode off by default */
#define SECSSLCONF_NAGLE_MODE            FALSE

/* Renegotiation triggers and limitations */
#define SECSSLCONF_RENEGOTIAION_BYTE_TRIGGER     0 /* disable byte trigger */
#define SECSSLCONF_RENEGOTIAION_MAX_COUNT       10
#define SECSSLCONF_RENEGOTIAION_MAX_TIME      1000  

/* Logging en-/disable */
#define SECSSLCONF_LOG_SSL_MESSAGES      TRUE
#define SECSSLCONF_LOG_SSL_RECORDS       TRUE

#endif /* FEATURE_SEC_SSL */

#endif
