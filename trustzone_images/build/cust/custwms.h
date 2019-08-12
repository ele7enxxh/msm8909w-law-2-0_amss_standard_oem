#ifndef CUSTWMS_H
#define CUSTWMS_H
/*===========================================================================

DESCRIPTION
  Configuration for WMS

  Copyright (c) 2002,2003,2004, 2005, 2006 by Qualcomm Technologies Incorporated. 
  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custwms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   ABA     Initial version.
===========================================================================*/

/*---------------------------------------------------------------------------
                         Wireless Messaging Services
---------------------------------------------------------------------------*/
#ifdef FEATURE_CSIM
  #define FEATURE_SMS_USE_MMGSDI_SESSION_API
#endif

#if defined(FEATURE_CDMA) || defined(FEATURE_LTE)
  #define FEATURE_CDSMS
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
  #define FEATURE_GWSMS
#endif

/* Common Features across CDMA and GW Modes */
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

  /* FEATURE_WMS is defined when either of the following flags is defined. 
   * This is convenient sometimes for developers to check if WMS API is 
   * enabled or not regardless of the modes.
   */
  #define FEATURE_WMS
  
  /* Enables user data headers to support voice and email address.
  */
  #define FEATURE_SMS_UDH

  /* WMS Diag Support
  */
  #define FEATURE_WMS_DIAG

  #ifdef FEATURE_WMS_APP
    #define FEATURE_IWMSDIAG
  #endif

  /* Enable WMS debug string
  */
  #define FEATURE_WMS_DEBUG_STRING
  
  /* Increase MO SMS success rate
  */
  #define FEATURE_SMS_RETRY  

  /* Features specific to CDMA Mode */
  #ifdef FEATURE_CDSMS

    /* Broadcast SMS support
    */
    #define FEATURE_BROADCAST_SMS

    /* New Implementation of CDMA Broadcast API
    */
    #define FEATURE_CDSMS_BROADCAST

    #define FEATURE_CDSMS_DUP_DETECTION

    /* Support message info cache for message list display    
    #define FEATURE_CDSMS_CACHE
    */

    #ifdef FEATURE_BUILD_JCDMA
      /* CDMA SMS JCDMA support
      */
      #define FEATURE_CDSMS_JCDMA
    #endif /*FEATURE_BUILD_JCDMA*/

  #endif /* FEATURE_CDSMS */

  /* Features specific to GW Mode */
  #ifdef FEATURE_GWSMS
    
    /* Used by TMC sources
    */
    #define FEATURE_UMTS_SMS

    /* Enable the message passing scheme for WMS
    */
    #define FEATURE_WMS_GS_SEND
    
    /* Support message info cache for message list display
    #define FEATURE_GWSMS_CACHE
    */

    /* Support GSM/WCDMA SMS Point-to-point download to SIM/USIM
    */
    #define FEATURE_GWSMS_PP_DOWNLOAD

    /* Support GSM/WCDMA SMS MO control by SIM/USIM
    */
    #define FEATURE_GWSMS_MO_CONTROL

    /* Support SMS Depersonalization Message from Network
    */
    #define FEATURE_GWSMS_DEPERSO

    #ifdef FEATURE_GSM_CB
      /* Support for GSM Cell Broadcast and UMTS BMC 
      */
      #define FEATURE_GWSMS_BROADCAST
    #endif

    #ifndef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      #define FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    #endif

  #endif /* FEATURE_GWSMS */

  #if (defined(FEATURE_GWSMS_BROADCAST)||defined(FEATURE_CDSMS_BROADCAST))
    /* Multimode Broadcast Support 
    */
    #define FEATURE_BROADCAST_SMS_MULTIMODE
  #endif

#endif /*defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)*/

#endif /* CUSTWMS_H */
