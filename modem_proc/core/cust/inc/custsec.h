#ifndef CUSTSEC_H
#define CUSTSEC_H
/*===========================================================================

            " C u s t - S E C "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Secure Services Livrary.

  Copyright (c) 2001-2013  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/cust/inc/custsec.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/13   nk      Removed HW crypto feature flag [should be moved to coomon.builds]
12/28/11   nk      Turning on HW Crypto for 8960 2.0
12/15/11   nk      Turning off the HW Crypto on 8960 2.0 ES release
08/10/11   nk      Turning off SSL, X509, HW CRYPTO for bringup 9x15
06/01/11   ejt     Turned off FEATURE_SEC_SSL_TESTAPP
09/03/10   ejt     Original file

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                        Security Services Modules
---------------------------------------------------------------------------*/

/* General cryptographic library */
#define FEATURE_SEC

/* IPSec protocol for IPv4 and VPN remote access client */

#ifdef IMAGE_MODEM_PROC
   /* secrnd task */
   #define FEATURE_SECRND
#endif


/*---------------------------------------------------------------------------
                    Cryptographic library configuration
---------------------------------------------------------------------------*/

#ifdef FEATURE_SEC

  /* Enable X.509 certificate handling */
  #define FEATURE_SEC_X509

  /* Enable storage of root certs in EFS */
  #define FEATURE_SEC_EFS_CA

  /* ARM implementations of the algorithms */
  #define FEATURE_SEC_ARM_CRYPTO

  /* Enabled for CMI builds */
  #define FEATURE_SEC_CMI

  #define FEATURE_SEC_NO_HW_KEY_READ

  /* ---------------------------------------------------- */
  /* ARM platform-specific comfiguration                  */
  /* ---------------------------------------------------- */
  #ifdef FEATURE_SEC_ARM_CRYPTO
  #endif

  /* Crypto test app */
  /* FEATURE_SEC_SSL defined in custglobal.h */ 
  #ifdef FEATURE_SEC_TESTAPPS
    #define SECAPITEST_TIMETEST
    #define FEATURE_SEC_CRYPT_TESTAPP
    #ifdef FEATURE_SEC_SSL
      #ifndef CUSTCOREIMG_Q_H
      #define FEATURE_SEC_SSL_TESTAPP
      #endif
    #endif
  #else
    #undef FEATURE_SEC_CRYPT_TESTAPP
  #endif

  /* SEC test diag event reporting support */
  #define FEATURE_SECTEST_DIAG_EVENTS

  /* SEC crypto layer diag event reporting support */
  #undef FEATURE_SECCRYPT_DIAG_EVENTS

#endif  /*FEATURE_SEC */

/* FEATURE_SEC_SFS defined in custglobal.h */ 
#ifdef FEATURE_SEC_SFS
#include "custsfs.h"
#endif

#ifdef FEATURE_SEC_IPSEC
#include "custipsec.h"
#endif

#endif /* CUSTSEC_H */
