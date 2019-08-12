#ifndef CUSTWMS_H
#define CUSTWMS_H
/*===========================================================================

DESCRIPTION
  Configuration for WMS

  Copyright (c) 2002-2006,2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/cust/inc/custwms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/11   TMS     Mainlined FEATURE_SMS_ENHANCED_DOMAIN_SELECTION, 
                   Verified FEATURE_BROADCAST_SMS mainlined in WMS,
                   Document pending mainlining status of both by NAS.
10/04/11   TMS     Removed unused features, aligned to dev version
09/22/11   TMS     Mainlined FEATURE_SMS_UDH
09/22/11   TMS     Mainlined FEATURE_WMS_DEBUG_STRING
09/22/11   TMS     Mainlined FEATURE_WMS_DIAG
09/08/08   ABA     Initial version.
===========================================================================*/

/*---------------------------------------------------------------------------
                         Wireless Messaging Services
---------------------------------------------------------------------------*/

#ifdef FEATURE_CDMA
  /* As of 20111011, FEATURE_BROADCAST_SMS is not used in WMS */
  /* It remains defined here until NAS mainlining has completed */
  #define FEATURE_BROADCAST_SMS

  #define FEATURE_CDSMS_BROADCAST
#endif

#if defined(FEATURE_CDMA) || defined(FEATURE_LTE)
  #define FEATURE_CDSMS
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) 
   #define FEATURE_GWSMS
#endif

#ifdef FEATURE_SMS_PWS_OVER_LTE
   #define FEATURE_GWSMS_BROADCAST
#endif

/* Common Features across CDMA and GW Modes */
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

  /* FEATURE_WMS is defined when either of the following flags is defined.
   * This is convenient sometimes for developers to check if WMS API is
   * enabled or not regardless of the modes.
   */
  #define FEATURE_WMS
  #ifdef FEATURE_DUAL_SIM
  /* Enables SMS sending even when data call is Subscription
  */
    #define FEATURE_SMS_SUSPENSE_PS_CALL  
  #endif /* FEATURE_DUAL_SIM */
  /* Features specific to CDMA Mode */
  #ifdef FEATURE_CDSMS

    #define FEATURE_CDSMS_DUP_DETECTION

  #endif /* FEATURE_CDSMS */

  /* Features specific to GW Mode */
  #ifdef FEATURE_GWSMS

    /* Used by TMC sources */
    #define FEATURE_UMTS_SMS

    #ifdef FEATURE_GSTK
      /* Support GSM/WCDMA SMS Point-to-point download to SIM/USIM */
      #define FEATURE_GWSMS_PP_DOWNLOAD

      /* Support GSM/WCDMA SMS MO control by SIM/USIM */
      #define FEATURE_GWSMS_MO_CONTROL
    #endif

    /* Support SMS Depersonalization Message from Network */
    #define FEATURE_GWSMS_DEPERSO

    #ifdef FEATURE_GSM_CB
      /* Support for GSM Cell Broadcast and UMTS BMC */
      #define FEATURE_GWSMS_BROADCAST
    #endif

    #ifndef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      /* As of 20111011, FEATURE_SMS_ENHANCED_DOMAIN_SELECTION has been mainlined in WMS */
      /* It remains defined here until NAS mainlining has completed */
      #define FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    #endif

  #endif /* FEATURE_GWSMS */

  #if (defined(FEATURE_GWSMS_BROADCAST)||defined(FEATURE_CDSMS_BROADCAST))
    /* Multimode Broadcast Support */
    #define FEATURE_BROADCAST_SMS_MULTIMODE
  #endif

  /* Enables QMI WMS */
  #define FEATURE_DATA_QMI_WMS

  /* Enable TRANSFER_ONLY routing for WAP Push SMS */
  #define FEATURE_SMS_WAP_PUSH_TRASFER_ONLY

#endif /*defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)*/

#endif /* CUSTWMS_H */
