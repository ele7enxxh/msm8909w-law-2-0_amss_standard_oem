#ifndef CUSTNAS_H
#define CUSTNAS_H
/*===========================================================================

                           C U S T U S U R F

DESCRIPTION
  NAS File containing the Customized feature flags for the Target Baselines.

  Copyright (c) 1996-2009 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/cust/inc/custnas.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

 *
 * Initial revision.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/22/11    rajesh Defined "FEATURE_INC_GSM_NAS_UPLINK_MSG_SIZE "
10/11/11    HC     Defined RAT priority list FEATURE_RAT_PRIORITY_LIST to 
                   support HPLMN feature
05/29/09    vsm    created file
===========================================================================*/


/*===========================================================================
                         PUBLIC DATA DECLARATIONS
===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif
/** Internal to DEVELOPMENT team. Not to enable on Target.
 */

#define FEATURE_NAS_REL7
/** This feature governs changes introduced for incorporation all Rel-7 NAS Spec CRs.
 */

#define FEATURE_NAS_REL10
/** This feature governs changes introduced for incorporation all Rel-10 NAS Spec CRs.
 */

#ifdef FEATURE_LTE_REL11
/** This feature governs changes introduced for incorporation all Rel-11 NAS Spec CRs.
 */
#define FEATURE_NAS_REL11
#endif

#define FEATURE_NAS_REL12

#define FEATURE_NAS_EAB 

#define FEATURE_GW_GET_NET_ABORT
#define FEATURE_MANAGED_ROAMING_HIGHER_PRIORITY_SEARCH
/** All above three are VF specfic feature.
 * These feature allows UE to make additional CS domain registration attempt after
 * max failures if no other network is available.
 * This is also controlled through NV NV_MGRF_SUPPORTED_I (5895).
 * If NV # 5895 is 1 and NV # 947 is 0, this feature will get ON else remain OFF.
 */

#define FEATURE_DISABLE_MCC_FILTERING
/** To disable MCC filtering in HPLMN search. Disable filtering when feature is enabled.
 * This is ATT specific requirement and also gurded with ATT specific NV - NV_ENS_ENABLED_I (3461).
 * Another new NV NV_ENHANCED_HPLMN_SRCH_TBL_I (6844) is added to accommodate similar requirement
 * from all customers/carriers including ATT.
 * Recommending to use 6844 for ATT (and others) instead of using this old feature.
 */



#define FEATURE_EQUIVALENT_HPLMN
/** This feature flag gives EHPLMN specification feature as per R-7 3GPP release.
 */

#define FEATURE_NAS_REL6
/** This feature governs changes introduced for incorporation all Rel-6 NAS Spec CRs.
 */



// #define FEATURE_T3213_OPTIONAL_WAIT
/** GCF and Core-spec are out of sync. For passing GCF 26.7.4.3.1, this flag should be off.
 * No NV to control. */


#define FEATURE_MVS_UNDER_GHDI

#ifdef FEATURE_WCDMA


/** This feature was introduced to make MODEM compliant with the MVS ISOD. 
 *  After this feature, gsvoice module will be single interface for VOCODER (between MODEM and MVS).
 */
#endif

#define FEATURE_HEAP_SELECTION
/*Feature to reduce memory fragmentation in TMC Heap */

#define FEATURE_REL6_PS_FOP
/*Spec compliant PS Follow On Proceed bit Support */

#define FEATURE_ECSD_NOT_SUPPORTED
/*If this feature flag is defined, spec compliant Enhanced Circuit Switched Data is not supported*/


#define FEATURE_REL7_STEERING_OF_ROAMING
/* This feature lets UE to register on Higher priority PLMN when UE receives REFRESH command */

#define FEATURE_CCBS
/* Call Completion Busy Subscriber support*/

#define FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
/* Support for reception of Multiple PDUs in a single TTI on CTCH */

#define FEATURE_SEND_SEQUENCE_NUMBER

#define FEATURE_LTE_UICC_CALL_CONTROL
/*Support for UICC Call Control feature*/

/* dynamic dog registration support */
#ifdef FEATURE_WDOG_DYNAMIC
  #define FEATURE_NAS_WDOG_DYNAMIC
#endif

#ifdef FEATURE_WCDMA
  /* All WCDMA Specific feature flag goes here*/

  #define FEATURE_TC
  /* Enable TC Loopback Functionality in WCDMA mode. This is applicable only in WCDMA */

  #define FEATURE_TC_HSPA_OPTIMIZATION
 /**
  * FEATURE_TC_HSPA_OPTIMIZATION enables optimization in processing DL SDUs and forrming UL SDUs in  Loopback mode
  * DL SDU is de-queued and required number of DUPs are created.
  */

#define FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
/* RRC will explicitly indicate a new failure cause in case of such inter-LA/RA redirection. */
/* NAS will be able to put the voice call in silent redial to let UE retry automatically after successful redirection. */


#endif

/* Defining RAT priority list to support HPLMN feature */
#define FEATURE_RAT_PRIORITY_LIST 

/* This feature considers upto 1000 Preferred PLMNs for searching during Higher Priority PLMN search */
#define FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
   

#define FEATURE_RR_ACTIVATION_RSP_INCLUDES_REGISTERED_RAI
/* This feature flag will be used in GRR. If this feature flag is defined then GRR will use 
 * the registered_rai field in RR_ACTIVATION_RSP */

#define FEATURE_INC_GSM_NAS_UPLINK_MSG_SIZE
/* NAS will fill l3_message_size in rr_est_req if this FEATURE FLAG is defined */

#define FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
/* RRC to indicate RAI/LAI change to NAS in abort indication */

#define FEATURE_NAS_FREE_FLOAT
/* NAS feature FEATURE_NAS_FREE_FLOAT enabling will not really enable any free-floating behavior. 
** It is just to resolve compilation dependency between NAS-GERAN/WCDMA  
*/

#define FEATURE_LTE_NAS_DEEP_SEC
/* Enable robust decoding of incoming LTE NAS OTA messages and also enable LTE NAS Fuzzing Test Framework and Test Cases */

#define FEATURE_NAS_DIAG_INIT 
/* To Enable DIAG cmds for NAS*/

#define FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL 
/* To Remove PDP promotions when Primary PDP is deactivated*/

#define FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
/*To perform RAU when rat priority list changes*/

#endif /* CUSTNAS_H */


/*****************************************************************************/

