#ifndef CUSTDATAMODEM_H
#define CUSTDATAMODEM_H
/*===========================================================================
 
DESCRIPTION
  ** DATA SERVICES CUST FILE (DATA MODEM) **

  Copyright (c) 2002 - 2016  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/cust/inc/custdatamodem.h#1 $
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/05/16   ds      Moved FEATURE_DATA_PS_464XLAT to Datamodem.scons.
10/09/14   ds      Adding FEATURE_DATA_PS_ROHC
10/10/14   ssh     IP Translation Engine 464XLAT 
08/23/14   skc     Removing FEATURE_HDLC_HW_ACCEL
06/27/14   tk      Added support for Dynamic ATCoP.
05/10/13   ab      Added FEATURE_DATA_QMI_RESP_FILTERING
05/18/13   sk      Added FEATURE_DSAT_HIGH_TIER.
05/13/13   wc      Added FEATURE_DATA_MBIM
04/26/13   fn      Disable FEATURE_DATA_PS_IPHC
10/30/12   sach    Added feature FEATURE_DATA_REL10 
08/22/12   sk      Mainlining Nikel Changes.
07/26/12   msh     Remove FEATURE_DATA_PS_PAP_INTERNAL_AUTH    
07/02/12   sj      Enabled FEATURE_DATA_QMI_DTR_ON_CTRL_CHANNEL
05/14/12   pp      Custfiles consolidation (Collapsed all into one)
===========================================================================*/

/*===========================================================================
** DATA SERVICES CUST FILE (DATA MODEM)
** - Hosts all features for all Technologies: 
** - Hosts all common protocol, interface features

** ::Following Grouping/Ordering is Maintained::
   ---------------------------------------------
** -==> Data High Level Features
** -==> Data Interface
** -==> Data Protocols
** -==> Data 3GPP2
** -==> Data 3GPP
** -==> Data WLAN (ePDG)
** -==> Data Tiers (Tier Specific Features)
** -----==> Ultra Low Tier (For Mid/Low tier chipsets)
** -----==> MSM
** -----==> MDM-LE
** -----==> MDM-TN
** -----==> Fusion
** -==> Data Debug/Test Feature Set
===========================================================================*/

/*---------------------------------------------------------------------------
  Data High Level Features (Features common to all packages)
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data Top Level Features (Moved to custglobal.h)
  - Kept here for reference only!
---------------------------------------------------------------------------*/
//top level feature removed: #define FEATURE_DS
//top level feature removed: #define FEATURE_DATA_PS
//top level feature removed: #define FEATURE_DS_SOCKETS
//top level feature removed: #define FEATURE_DATA_RM_NET
//top level feature removed: #define FEATURE_QMI_MODEM_TASK
//top level feature removed: #define FEATURE_QMI_SERVICE_SPLIT
//top level feature removed: #define FEATURE_DATA_PS_DCC
//top level feature removed: #define FEATURE_DATA_PS_SOCK_REARCH
//top level feature removed: #define FEATURE_DATA_PS_RM_TASK
//top level feature removed: #define FEATURE_DATA_PS_META_INFO_V2
//top level feature removed: #define FEATURE_DATA_PS_IN_ALIASES
//top level feature removed: #define FEATURE_DATA_PS_IPV6
//top level feature removed: #define FEATURE_DATA_PS_EAP

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data Interface - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_PS_SYSTEM_HEAP
#define FEATURE_DATA_PS_ADDR_MGMT
#define FEATURE_DATACOMMON_PS_IFACE_IO
#define FEATURE_DATA_PS_QOS
#define FEATURE_DATA_QMI
#define FEATURE_DATA_QMI_QOS
#define FEATURE_DATA_QMI_MCAST
#define FEATURE_DATA_QMI_EAP
#define FEATURE_DATA_QMI_AT
#define FEATURE_DATA_QMI_DTR_ON_CTRL_CHANNEL
#define FEATURE_DATACOMMON_PACKAGE_MODEM

/* A2 / IPA Block */
#ifdef FEATURE_A2
#ifndef FEATURE_DATA_IPA 
  //Enable A2 features only if IPA is not defined!
  #define FEATURE_DATA_A2
  #define FEATURE_DATA_A2_FILTERING
  #define FEATURE_DATA_A2_DL_BRIDGE
#endif /* FEATURE_DATA_IPA */
#endif /* FEATURE_A2 */

#ifdef FEATURE_DYNAMIC_LOADING
/* Need to uncomment it once MPSS DLL framework is available */
//#define FEATURE_DSAT_DYNAMIC_LOADING
#endif /* FEATURE_DYNAMIC_LOADING */

#define FEATURE_DATA_DS_PROFILE_AMSS
#ifndef FEATURE_DSAT_DYNAMIC_LOADING
#define FEATURE_DSAT_EXT_CLIENT_SUPPORT
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */
#define FEATURE_DATA_MBIM
#define FEATURE_DATA_QMI_RESP_FILTERING
#define FEATURE_DATA_QMI_SVC_AVAILABILITY
#define FEATURE_DSAT_HIGH_TIER
//#define FEATURE_DATA_AUDIO_OFFLOAD - Moved to datamodem.scons
#define FEATURE_USER_CONFIGURE_EFS
/*---------------------------------------------------------------------------
  Data Interface - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data Protocols - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_PS_LLC
#define FEATURE_DATA_PS_PPP
#define FEATURE_DATA_PS_PPP_LOGGING
#define FEATURE_DATA_PS_HDLC
#define FEATURE_DATA_PS_EAP_AKA_SW_IMPL
#define FEATURE_DATA_PS_PPP_EAP
#define FEATURE_DATA_PS_EHRPD
#define FEATURE_DATA_PS_ENFORCE_AUTH
#define FEATURE_DATA_PS_IPHC
#define FEATURE_DATA_PS_DHCPV6
//#define FEATURE_DATA_PS_464XLAT - Moved to datamodem.scons
#define FEATURE_DATA_PS_ROHC

#if defined(FEATURE_CDMA) && defined(FEATURE_DS_MOBILE_IP)
  #define FEATURE_DS_MOBILE_IP_PERF
  #define FEATURE_DATA_PS_MIP
#endif

#if defined(FEATURE_WCDMA) && defined(FEATURE_DSM_DYNAMIC_POOL_SELECTION)
  #define FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
#endif
/*---------------------------------------------------------------------------
  Data Protocols - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data 3GPP2 - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
#define FEATURE_DATA_SV_MODE

#ifdef FEATURE_CDMA
///Cust DataC2K
#ifdef FEATURE_DS
    /* Feature to force dormancy on 1X to allow call on EVDO if available,
       if the call is up on CDMA for >= 5 mnts  */
    #define FEATURE_DS_CAM

   /* IF TC is brought up due to an incoming call, dormancy timer is reduced
       in order to avoid keeping the TC UP for unnecessarily long. */

  #define FEATURE_DS_RDUD

  #ifdef FEATURE_BUILD_JCDMA
     /* disable support for Mobile IP.*/
     #undef FEATURE_DS_MOBILE_IP

     /* Turn off Mobile IP dsregistration for non-JCDMA builds */
     #undef FEATURE_DS_MOBILE_IP_DEREG
     
     /* disable Mobile IP performance metrics.*/
     #undef FEATURE_DS_MOBILE_IP_PERF

     /* disable dynamic MIP key update */
     #undef FEATURE_DS_MOBILE_IP_DMU

     /* Support for Dynamic Mobile IP Key update modifies the AN password */
     #undef FEATURE_DATA_MOBILE_IP_DMU_MODIFIES_AN_AUTH

     /* disable FEATURE_DS_QNC in JCDMA 
      * Quick Net Connect originations */
     #undef FEATURE_DS_QNC

     /* Disable Throttling support for DCTM (Data call throttle manager */
     #undef FEATURE_CALL_THROTTLE 

      /* Disable IF TC is brought up due to an incoming call, dormancy timer is reduced
         in order to avoid keeping the TC UP for unnecessarily long. */
     #undef FEATURE_DS_RDUD

     /* Disable Feature to force dormancy on 1X to allow call on EVDO if available,
       if the call is up on CDMA for >= 5 mnts  */
     #undef FEATURE_DS_CAM

   #else /* Single Proc, Data ON APPS builds, DATA on Modem (Modem Proc) */

     /* Code hooks to support Mobile IP.*/
//top level feature removed:         #define FEATURE_DS_MOBILE_IP

        /* Turn on Mobile IP dsregistration */
        #define FEATURE_DS_MOBILE_IP_DEREG
     
        /* Enable Mobile IP performance metrics.*/
//top level feature removed:         #define FEATURE_DS_MOBILE_IP_PERF

        /* Support for dynamic Mobile IP Key update.*/
        #define FEATURE_DS_MOBILE_IP_DMU
        
        /* Support for Dynamic Mobile IP Key update modifies the AN password */
        #define FEATURE_DATA_MOBILE_IP_DMU_MODIFIES_AN_AUTH
        /* Feature for closing HDR session when MIP authentication fails(err 67) */
		#define FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS

     /* Enable FEATURE_DS_QNC in non JCDMA  
      * Quick Net Connect originations */
//top level feature removed:      #define FEATURE_DS_QNC

   #endif /* FEATURE_BUILD_JCDMA */

   /* Use default passwords for SN and AN PPP instances when no password has been written to NV. */
   // #define FEATURE_DATA_PPP_DEFAULT_PASSWD

   /* Support for new multimode data architecture.
    * Used by code which supports both the old and new data architectures.
   */
//top level feature removed:    #define FEATURE_DATA_MM

  /* features not defined for Apps Proc on Third Party OS Builds */
  #if !defined(IMAGE_APPS_PROC) || defined (FEATURE_DATA_ON_APPS)

     /* Support for IS-707 Data Services (Async & Packet).*/
     #define FEATURE_DATA_IS707

   #endif /* !defined(IMAGE_APPS_PROC) || defined (FEATURE_DATA_ON_APPS) */

   /* Support for IS-95/IS2000/1X Data Services.   */
//top level feature removed:    #define FEATURE_CDMA

  /* features not defined for Apps Proc on Third Party OS Builds */
  #if !defined(IMAGE_APPS_PROC) || defined (FEATURE_DATA_ON_APPS)

     /* IS 707-B EPZID Hysterisis  feature*/
     #define FEATURE_DS_EPZID_HYSTERESIS

     /* Replaces FEATURE_DS_EPZID_RECON from old data architecture.
      * Enables data reconnections on EPZID change.
     */
     #define FEATURE_DATA_IS707_EPZID_RECON

     /* Replaces FEATURE_DS_SID_CHNG_RECON from old data architecture.
      * Enables data reconnections on SID change.
     */
     #define FEATURE_DATA_IS707_SID_CHNG_RECON

     /* Replaces FEATURE_DS_NID_CHNG_RECON from old data architecture.
      * Enables data reconnections on NID change.
     */
     #define FEATURE_DATA_IS707_NID_CHNG_RECON

  #endif /* !defined(IMAGE_APPS_PROC) || defined (FEATURE_DATA_ON_APPS) */

   /* For non-test builds DQoS is always enabled and should not be
    * turned off. Defining this feature disables the NV item that 
    * allows D-QoS to be turned off */
   #define FEATURE_HDR_QOS_DQOS_SUPPORTED_ALWAYS


   /* Data on the Go AMPS data */
   #undef FEATURE_DS_DOTG_DATA

   /* Original IS-99 Command Reflection Scheme*/
   #undef  FEATURE_DS_IS99_REFL

   /* Enable Data Services memory checks*/
//top level feature removed:    #define FEATURE_DSM_MEM_CHK

   /* Enable large items in DSM. */
//top level feature removed:    #define FEATURE_DSM_LARGE_ITEMS

   /* Enable duplicate items in DSM.*/
//top level feature removed:    #define FEATURE_DSM_DUP_ITEMS

   /* Disables packet based Browser Interface*/
   #undef FEATURE_DS_BROWSER_INTERFACE

   /* Enables Network Model */
//top level feature removed:    #define FEATURE_DS_NET_MODEL

   /* Enables Password Authentication Protocal */
//top level feature removed:    #define FEATURE_DS_PAP

   /* Enable use of NV to store PAP user id and password
      Requires FEATURE_DS_PAP to be enabled.
   */
   #ifdef FEATURE_DS_PAP
//top level feature removed:      #define FEATURE_DS_PAP_NV
   #endif

   /* Enables Challenge Authentication Protocol*/
//top level feature removed:    #define FEATURE_DS_CHAP

   /* Enable use of NV to store CHAP user id and password
      Requires FEATURE_DS_CHAP to be enabled.
   */
   #ifdef FEATURE_DS_CHAP
//top level feature removed:      #define FEATURE_DS_CHAP_NV
   #endif

   /* Enables Protocol statistics for Data Services */
   //JCT   #define FEATURE_DS_PSTATS

   /* Enables Debug UI for Data Services */
   //JCT   #define FEATURE_DS_DEBUG_UI

   /* Enable Mobile to Mobile calls for IS-99 and IS-657 */
//top level feature removed:    #define FEATURE_DS_MTOM

   /*********************************************************************
     Exactly ONE of FEATURE_DS_ORIG_XXX_SO must be defined.
   *********************************************************************/
   /* Originate with IS-707.0 (Pre-707) service options
      (If FEATURE_DS_CHOOSE_SO is defined, this specifies the default set.)
   */
   #undef FEATURE_DS_ORIG_PRE707_SO

   /* Originate with proprietary service options
      (If FEATURE_DS_CHOOSE_SO is defined, this specifies the default set.)
   */
   #undef FEATURE_DS_ORIG_PROPTRY_SO

   /* Originate with IS-707 standard service options
      (If FEATURE_DS_CHOOSE_SO is defined, this specifies the default set.)
   */
//top level feature removed:    #define FEATURE_DS_ORIG_IS707_SO

   /* Support choosing the set of service options used for originations.*/
//top level feature removed:    #define FEATURE_DS_CHOOSE_SO

   /* Support Answer Voice As Data   */
   //JCT   #define FEATURE_DS_VOICE_AS_DATA
  // #if defined(T_QSC6055) || defined(T_QSC6085) || defined(T_MSM7500)
   //#else
      /* Enable Autobaud detection for rates between 1200 and 19200   */
      //#define FEATURE_AUTOBAUD
   //#endif

   /* Enable Call Manager support for Data Services   */
//top level feature removed:    #define FEATURE_DS_CM

   /* Avoid a call origination for a network model packet call in dormant mode when the
   ** MS receives PPP TERM_REQ from the RM interface.
   */
   //JCT   #undef FEATURE_DS_PPP_TERM_REQ_DROP

   /* Dormancy for laptop packet calls is enabled when AT+CTA is 0.  */
//top level feature removed:    #define FEATURE_DS_ENABLE_RM_DMNCY_IF_CTA_0

   /* Enable logging of PPP packets. */
   //JCT   #define FEATURE_DS_PPP_LOGGING

   //JCT catch-22, need MDR defined before DS and DS before HDR for
   //              this block

   /* MDR specific features. These are included only
   ** if FEATURE_IS95B_MDR is included
   */
   #if defined(FEATURE_IS95B_MDR) || defined(FEATURE_SPECIAL_MDR)
      /* MDR Data Pattern Test feature. This is an internal test feature  */
      //#undef FEATURE_MDR_DPT_MARKOV

      /* Sockets feature to restrict socket calls to LSPD service options.
      ** Since FEATURE_DS_IS2000 is always defined, do not define this.
      */
      //#undef FEATURE_DS_SOCKETS_USE_LSPD

      /* Support for the IS-707-A format of the AT+CMUX command. This command
      ** sets the maximum forward & reverse multiplex options.
      */
//top level feature removed:       #define FEATURE_DS_IS707A_CMUX

   #endif /* FEATURE_IS95B_MDR */

   /* Support for Multiple NAIs for SIP(CHAP) & MIP calls */
   #define FEATURE_DS_MULTIPLE_NAI
   
   /* Throttling support for DCTM (Data call throttle manager */
//top level feature removed:    #define FEATURE_CALL_THROTTLE 

   /* On moving from 1x to DO wait for LN Success Event before 
   changing the current data session to DO */
   #define FEATURE_DS_LN_UPDATE              

   #define FEATURE_DATA_DISABLE_IS95B_MDR

//top level feature removed:    #define FEATURE_IS707B_ADDENDUM

#endif /* FEATURE_DS */

/* Please leave the definition below as is at the end of the file 
   as it depends on another feature being defined */                                               
#if defined(FEATURE_DS_MOBILE_IP_DMU)
//top level feature removed:   #define FEATURE_DATA_PS_INTERNAL_AUTH
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#define FEATURE_DATA_DS_PROFILE_3GPP2

/* ------------------------------------------------------------- */
/*                ATCoP CDMA Features  Start                     */
/* ------------------------------------------------------------- */

#ifdef FEATURE_DS

/* Allows an OK to ATD #777, allows packet services for WIN95 */

/* Enables features used for test */
//top level feature removed: #define FEATURE_DS_AT_TEST_ONLY

#ifdef FEATURE_INTERNAL_USE_FFABUILDS_ONLY

#ifdef FEATURE_HDR
/* Enables HDR AT commands used for test */
//top level feature removed: #define FEATURE_HDR_AT_TEST_ONLY 
#endif /* FEATURE_HDR */ 

#endif /* FEATURE_INTERNAL_USE_FFABUILDS_ONLY */

#define FEATURE_CDMA_SMS

#ifdef  FEATURE_MMGSDI_SESSION_LIB
//#define FEATURE_DSAT_MMGSDI_AUTH_CMD 
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/* Enable CDMA PIN related commands; Assuming MMGSDI is always turned ON*/
#define FEATURE_DSAT_CDMA_PIN
/* ------------------------------------------------------------- */
/*                ATCoP CDMA Features  END                       */
/* ------------------------------------------------------------- */

/* --------------------------------------------------------------------------
** EHRPD dependent features.
** ------------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
#define FEATURE_DATA_EHRPD_DUAL_IP
#define FEATURE_DATA_EHRPD_PDN_THROTTLE
#define FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
#define FEATURE_DATA_PARTIAL_CONTEXT
#define FEATURE_DATA_3GPP2_VS
#define FEATURE_DATA_3GPP2_ALWAYS_ON
#define FEATURE_DATA_PS_3GPP2_ALWAYS_ON
#define FEATURE_EHRPD_HRPD_FALLBACK
#define FEATURE_DATA_SEND_PPP_CTL_ON_DOS
#define FEATURE_DATA_MANDATORY_PDN_LABEL
#define FEATURE_DATA_FALLBACK_LIST
#define FEATURE_DS_MTU_PCO_VSNCP
#define FEATURE_DATA_CM_CENTRIC_EMERG_MODE
#define FEATURE_NIQ_EHRPD
#endif /* FEATURE_EHRPD*/

/* --------------------------------------------------------------------------
** Flow Control features.
** ------------------------------------------------------------------------*/
#if defined (FEATURE_CPU_BASED_FLOW_CONTROL) && defined (FEATURE_FC_CPU_MONITOR) && defined (FEATURE_HDR)
//top level feature removed: #define FEATURE_CCM_FLOW_CONTROL 
#define FEATURE_DATA_RL_FLOW_CONTROL
#define FEATURE_DATA_FL_FLOW_CONTROL
#endif /* defined (FEATURE_CPU_BASED_FLOW_CONTROL) && defined (FEATURE_FC_CPU_MONITOR) */

#define FEATURE_CCM_THERM_FLOW_CONTROL

#endif /* FEATURE_DS */
/* -------------  ATCoP CDMA Features   END ----------------- */
#define FEATURE_HDRRLP_LOGGING
#endif /*FEATURE_CDMA*/

/*---------------------------------------------------------------------------
  Data 3GPP2 - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data 3GPP - Start
---------------------------------------------------------------------------*/
#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  /* Enables Network Model */
//top level feature removed:   #define FEATURE_DS_NET_MODEL
  #define FEATURE_DATA_IMS

  /*#define FEATURE_WLAN*/
  //#define FEATURE_WLAN_NEW_PS_QOS_VER
  //#define FEATURE_WLAN_PHG
  //#define FEATURE_WLAN_PHG_11G
  //#define FEATURE_WLAN_DIAG_SUPPORT


/* Enable ETSI specific functionality in PS 
*/
#ifdef FEATURE_WCDMA
//top level feature removed: #define FEATURE_DATA_WCDMA_PS
//top level feature removed: #define FEATURE_UMTS_PDCP
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_NAS_REL7
#define FEATURE_REL7
#endif /* FEATURE_NAS_REL7 */

//#undef FEATURE_DATA_WCDMA_PS_PDP_IP

/* Enables SMS for ATCOP
*/
#define FEATURE_ETSI_SMS
#define FEATURE_ETSI_SMS_TEMPLATE
/*Control CARD PUP/DOWN as part of +CFUN command*/
#define FEATURE_DSAT_CFUN_CARD_POWER_CTL

#ifdef FEATURE_UI_PBM
  /* Enables PhoneBook Manager for ATCOP 
  */
  #define FEATURE_ETSI_PBM
#endif

#define FEATURE_ETSI_ATTACH
#define FEATURE_ETSI_SMS_PS

#define FEATURE_DSAT_ETSI_RLP2

#define FEATURE_DSAT_KEYPAD_EMULATION
#define FEATURE_DATA_ETSI_PIN
#define FEATURE_DATA_RPM

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  #define FEATURE_DSAT_DEV_CMDS
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_WCDMA_HSUPA
  #define FEATURE_DATA_HSUPA
#endif

#if defined(FEATURE_DATA_WCDMA_PS) || defined ( FEATURE_GSM_GPRS ) \
    || defined ( FEATURE_DATA_LTE ) || defined (FEATURE_TDSCDMA )
#define FEATURE_DSAT_ETSI_DATA
#define FEATURE_DATA_TEST_LOOPBACK
#define FEATURE_DATA_3GPP
#endif /* (FEATURE_DATA_WCDMA_PS) ||( FEATURE_GSM_GPRS )||( FEATURE_DATA_LTE )*/

/* Enable LTE */
#ifdef FEATURE_LTE

//top level feature removed: #define FEATURE_DATA_LTE
/* LTE related ATCoP feature wrapping */
#define FEATURE_DSAT_LTE

#ifdef FEATURE_LTE_EMBMS
#define FEATURE_DATA_EMBMS
#endif /* FEATURE_LTE_EMBMS */

#endif /* FEATURE_LTE */  


#if defined(FEATURE_DATA_WCDMA_PS) || defined ( FEATURE_GSM_GPRS ) 
#define FEATURE_DSAT_UMTS_DATA
#endif /* (FEATURE_DATA_WCDMA_PS) || ( FEATURE_GSM_GPRS )*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_DSAT_LTE)
#define FEATURE_DSAT_ETSI_MODE
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_DSAT_LTE */

#ifdef FEATURE_DISABLE_MULTI_PDN
  #define FEATURE_DATA_DISABLE_MULTI_PDN
#endif /* FEATURE_DISABLE_MULTI_PDN */

#ifdef FEATURE_DATA_LTE
   #define FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER
#endif /* FEATURE_DATA_LTE */
                                         
#define FEATURE_NEW_PKT_INFO_DEF

//top level feature removed: #define FEATURE_SECONDARY_PDP

#define FEATURE_PS_DORMANT_PWR_SAVE

#if defined(FEATURE_DATA_ON_APPS) || defined(FEATURE_WCDMA)
//top level feature removed: #define FEATURE_COMP_TASK
#endif

#ifdef FEATURE_DATA_WCDMA_PS
//top level feature removed:   #define FEATURE_UE_INITIATED_DORMANCY
#endif /* FEATURE_DATA_WCDMA_PS */

//top level feature removed: #define FEATURE_DUAL_IP_BEARER 

#define FEATURE_ETSI_PBM
#define FEATURE_ETSI_ATTACH
#define FEATURE_ETSI_SMS
#define FEATURE_ETSI_SMS_CB
#define FEATURE_ETSI_SMS_PS

#ifdef FEATURE_DATAMODEM_DUAL_PROC_MODEM_NIKEL
#undef FEATURE_DSAT_KEYPAD_EMULATION
#endif /* FEATURE_DATAMODEM_DUAL_PROC_MODEM_NIKEL */

/* Need to be cleaned up */
//top level feature removed: #define FEATURE_DATA_PDCP
//top level feature removed: #define FEATURE_GSNDCP_USES_COMPTASK
//top level feature removed: #define FEATURE_DATA_MM
#endif /* FEATURE_GSM_GPRS || FEATURE_WCDMA*/


#if defined(FEATURE_WCDMA)
//top level feature removed:   #define FEATURE_DATA_WCDMA_PS
#endif

#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_WCDMA)
  #undef FEATURE_DATA_WCDMA_PS_PDP_IP
  #define FEATURE_PS_DORMANT_PWR_SAVE
#endif /* (GSM || WCDMA) */

#if defined(FEATURE_LTE) && defined(FEATURE_EHRPD)
 #ifdef FEATURE_LTE_TO_HDR_OH
  #define FEATURE_DATA_OPTHO
 #endif /* FEATURE_LTE_TO_HDR_OH */
#endif /* FEATURE_LTE && FEATURE_EHRPD*/

/* TBD: Get UMTS REL10 feature to also wrap this feature*/
#if defined(FEATURE_LTE_REL10) || defined(FEATURE_NAS_REL10)
  #define FEATURE_DATA_REL10
#endif /*FEATURE_LTE_REL10 || FEATURE_NAS_REL10*/

/* Enable Feature DATA RELEASE 11 if LTE RELEASE 11 is enabled */
#ifdef FEATURE_LTE_REL11
  #define FEATURE_DATA_REL11
#endif /* FEATURE_LTE_REL11 */

#ifdef FEATURE_DATA_LTE 
#define FEATURE_EMERGENCY_PDN
#endif /* FEATURE_DATA_LTE */
/*---------------------------------------------------------------------------
  Data 3GPP - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data ePDG - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_WLAN_MAPCON
//#define FEATURE_DATA_IWLAN_S2B

/*---------------------------------------------------------------------------
  Data ePDG - End
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data Tiers - Start
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>> Data Ultra Low Tier (Disables High-End features)
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MPSS_ULT
  #undef FEATURE_DATA_QMI_QOS
  #undef FEATURE_DATA_MBIM
  #undef FEATURE_DATA_QMI_RESP_FILTERING
  #undef FEATURE_DATA_QMI_SVC_AVAILABILITY
  #undef FEATURE_DSAT_HIGH_TIER
  #undef FEATURE_DATA_PS_IPHC
  #undef FEATURE_DS_MTU_PCO_VSNCP
  #undef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  #undef FEATURE_EMERGENCY_PDN
  #define FEATURE_DATA_3GPP2_ULT
  #undef FEATURE_DATA_WLAN_MAPCON
  #undef FEATURE_DATA_IWLAN_S2B  
#endif /* FEATURE_DATA_MPSS_ULT */

/*---------------------------------------------------------------------------
Data EPC feature
---------------------------------------------------------------------------*/
#if ((defined(FEATURE_DATA_LTE) && defined(FEATURE_EHRPD)) || \
    (defined(FEATURE_DATA_IWLAN_S2B) && defined(FEATURE_DATA_LTE)) || \
    (defined(FEATURE_DATA_IWLAN_S2B) && defined(FEATURE_EHRPD)))
  #define FEATURE_EPC_HANDOFF
#endif

/*---------------------------------------------------------------------------
>>> Data Fusion (Clean-up? TODO!)
---------------------------------------------------------------------------*/
/* >> SGLTE Fusion << */
#ifdef FEATURE_8960_SGLTE_FUSION
  #define FEATURE_DATA_MUX
  #define FEATURE_DATA_MUX_FUSION
  #define FEATURE_DATA_MUX_FUSION_RESPONDER
  #define FEATURE_DATA_MUX_SGLTE
  #define FEATURE_QMI_CLIENT
#endif  /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_MDM_FUSION
  #define FEATURE_DATA_FUSION_MDM
  #define FEATURE_QMI_CLIENT

  #ifdef FEATURE_8X60_SINGLE_RADIO_FUSION
    #error code not present
#endif

  #ifdef FEATURE_8X60_DUAL_RADIO_FUSION
    #error code not present
#endif
#endif

#ifndef FEATURE_MDM_FUSION
  #ifdef FEATURE_DATA_A2
    #define FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
  #endif /* FEATURE_DATA_A2 */
#endif /* FEATURE_MDM_FUSION */

#ifdef FEATURE_DATA_A2
#if !(defined(FEATURE_MDM_FUSION) || defined(FEATURE_MSM_FUSION))
 #define FEATURE_DATA_A2_MDM
#endif /*FEATURE_MDM_MSM_FUSION*/
#endif /* FEATURE_DATA_A2 */

#if (defined(FEATURE_MDM_FUSION) || defined(FEATURE_MSM_FUSION)) && !defined(FEATURE_8X60_SINGLE_RADIO_FUSION)
  #define FEATURE_DATA_FUSION
  #define FEATURE_DATA_DSD
#endif /* FEATURE_MDM_MSM_FUSION */

#ifdef FEATURE_MDM_FUSION
  #define FEATURE_DSAT_MDM_FUSION
#endif

#ifdef FEATURE_MSM_FUSION
  #define FEATURE_DSAT_MSM_FUSION
#endif

/* Moved from Target Cust files */
#if defined(FEATURE_MDM_FUSION) && (defined(FEATURE_8X60_SINGLE_RADIO_FUSION) || defined(FEATURE_8X60_DUAL_RADIO_FUSION))
  #error code not present
#endif

/*---------------------------------------------------------------------------
  Data Tiers - End
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data Debug/Test Features
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATAMODEM_SINGLE_PROC_DEBUG
  #define FEATURE_STA
  #define FEATURE_STA_DSS
  #define FEATURE_STA_DSSIOCTL
  #define FEATURE_STA_DNS
  #define FEATURE_STA_SCHED
  #define FEATURE_STA_RESTART
//#define FEATURE_STA_PS_IFACE
  #define FEATURE_STA_PS_MEM
  #define FEATURE_STA_PSSTATS
  #define FEATURE_STA_HDLC
  #define FEATURE_STA_PPP
  #define FEATURE_STA_ROHC
  #define FEATURE_STA_DSM
#endif /* FEATURE_DATAMODEM_SINGLE_PROC_DEBUG */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* Support PDCP SIM for loop back in off target environment */
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#define FEATURE_DATA_TEST_LOOPBACK_PDCPSIM
#endif

#ifdef FEATURE_DATA_JO_PRESIL
#undef FEATURE_DATA_EMBMS
#endif

#ifdef FEATURE_DATA_ATP
  // comment below two out for IMS
  #define FEATURE_DATA_ATP_DS_HTTP
  #define FEATURE_DATA_ATP_DS_XML

  #ifndef FEATURE_DATA_ATP_DS_HTTP
    #define FEATURE_DATA_ATP_IMS_HTTP
  #endif /* FEATURE_DATA_ATP_DS_HTTP */

  #ifndef FEATURE_DATA_ATP_DS_XML
    #define FEATURE_DATA_ATP_IMS_XML
  #endif /* FEATURE_DATA_ATP_DS_XML */
#endif /* FEATURE_DATA_ATP */


#define FEATURE_DATA_ANDSF

#ifdef FEATURE_DATA_ANDSF
  #define FEATURE_DATA_ANDSF_POLICY
  #define FEATURE_DATA_ANDSF_RULE_MGR
#endif /* FEATURE_DATA_ANDSF */

#endif /* CUSTDATAMODEM_H */
