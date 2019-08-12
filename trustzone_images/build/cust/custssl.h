#ifndef CUSTSSL_H
#define CUSTSSL_H
/*===========================================================================

            " C u s t - S S L "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Secure Sockets Layer Feature.

  Copyright (c) 2001-2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custssl.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/02   om      Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Security Services
---------------------------------------------------------------------------*/
/* Security Services
*/

#ifdef FEATURE_SEC_SSL

  /* Use DNS lookups for server names */
  #define FEATURE_SEC_SSL_DNS_LOOKUP

  /* Enable large records by increasing the DSM small item count */
  #define FEATURE_SEC_SSL_LARGE_RECORDS

  /* Allow async execution callbacks for error conditions only */
  #define FEATURE_SEC_SSL_ERROR_CB_ONLY
	
  /* SSL Key Exchange debugging via F3 screen */
  #define FEATURE_SEC_SSL_KEYX_DEBUG

  /* SSL logging support */
  #undef FEATURE_SEC_SSL_LOGGING

  /* Periodic logging of SSL session status */
  #undef FEATURE_SEC_SSL_PERIODIC_LOGGING

  /* SSL test app */
  #ifdef FEATURE_SEC_TESTAPPS
  #define FEATURE_SEC_SSL_TESTAPP
  #else
  #undef FEATURE_SEC_SSL_TESTAPP
  #endif

  /* Log the certificate .DER data */
  #undef FEATURE_SEC_SSL_LOG_CERT

  /* SSL uses the EFS to store its trusted root CAs */
  #ifdef FEATURE_EFS

    /* EFS Certificate Authorities */
    #define FEATURE_SEC_SSL_EFS_CA

    /* Allow the NULL Root CA in EFS for certificate 
    ** processing, to assist the debugging. */
    #define FEATURE_SEC_SSL_ALLOW_NULL_CA

    /* Remove the invalid CA from the EFS. */
    #define FEATURE_SEC_SSL_REMOVE_INVALID_CA

  #endif /* FEATURE_EFS */

#endif  /* FEATURE_SEC_SSL */

#endif /* CUSTSSL_H */
