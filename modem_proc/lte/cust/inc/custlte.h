/*!
  @file
  custlte.h

  @brief
  This file contains definitions for the features tech areas like NAS, 1X, etc.
  may need and  featuriztion differences that LTE Access Stratum(AS)
  may need to control.

*/

/*=============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/cust/inc/custlte.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/12/2015 ar      CR 803880: Define FEATURE_LTE_ELS_ENABLED and
                   FEATURE_LTE_UDC_ENABLED
08/27/14   mshete  CR 715971: Define FEATURE_LTE_TDD_CA and FEATURE_LTE_EICIC
                   under FEATURE_BOLT_MODEM
02/05/13   gxiao   CR448999: undef FEATURE_LTE_WDOG_DYNAMIC: remove wdog logic
10/07/11   gpotnuruAdded FEATURE_LTE_1XCSFB_SUPPORT_DEFAULT flag to make 1xCSFB
                   feautre enabled by default
12/11/09   vatsac  initial version
=============================================================================*/

#ifndef CUSTLTE_H
#define CUSTLTE_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/


/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief LTE supported by the UE
*/
#if defined(FEATURE_LTE) && defined (FEATURE_WCDMA)
//top level feature removed:   #define FEATURE_LTE_TO_WCDMA
#endif /* defined(FEATURE_LTE) && defined (FEATURE_WCDMA) */

#if defined (FEATURE_LTE) && defined (FEATURE_HDR)
//top level feature removed: #define FEATURE_LTE_TO_HDR
#endif

#if defined (FEATURE_LTE) && defined (FEATURE_CDMA)
//top level feature removed: #define FEATURE_LTE_TO_1X
#endif

#if defined (FEATURE_LTE) && defined (FEATURE_GSM)
//top level feature removed: #define FEATURE_LTE_TO_GSM
#endif

//top level feature removed: #define FEATURE_LTE_3GPP_REL8_MAR09
#define FEATURE_LTE_3GPP_REL8_JUN09
#define FEATURE_LTE_3GPP_REL8_SEP09
//top level feature removed: #define FEATURE_LTE_3GPP_REL8_DEC09
#define FEATURE_LTE_ML1_FTM_WORKAROUND
#define FEATURE_LTE_ML1_IDLE_LOG

/* @brief Feature to support connected mode DRX */
#define FEATURE_LTE_CDRX

/* @brief Interact with RF even on offtarget */
//#define FEATURE_LTE_RF

/* @brief Feature to support generic IRAT operations */
#define FEATURE_LTE_TO_IRAT \
  ( defined(FEATURE_LTE_TO_GSM) || defined(FEATURE_LTE_TO_WCDMA) || \
    defined(FEATURE_LTE_TO_1X)  || defined(FEATURE_LTE_TO_HDR) )

#define FEATURE_LTE_SIB_COMB

/* @brief Feature to enable PLMN Search */
#define FEATURE_LTE_BPLMN

#define FEATURE_LTE_AS_RESET

#if defined (FEATURE_TDSCDMA)
#define FEATURE_LTE_TO_TDSCDMA
#endif
//top level feature removed: #define FEATURE_3GPP_CSFB

//top level feature removed: #define FEATURE_LTE_REL9

#define FEATURE_LTE_RRC_CEP_OPT

#define FEATURE_LTE_RF

#undef FEATURE_LTE_WDOG_DYNAMIC

#if ( defined(FEATURE_LTE_TO_GSM) || defined(FEATURE_LTE_TO_WCDMA) || \
    defined(FEATURE_LTE_TO_1X)  || defined(FEATURE_LTE_TO_HDR) )
  #define LTE_MEAS_EVENT_B1_DEFINED  TRUE
#else
  #define LTE_MEAS_EVENT_B1_DEFINED FALSE
#endif

// Features related to rrc caps for 3.3 and 4.0 PLs.
#ifdef FEATURE_LTE_TO_WCDMA
  #define LTE_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED TRUE
  #define LTE_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED      TRUE
#else
  #define LTE_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED FALSE
  #define LTE_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED      FALSE
#endif /* FEATURE_LTE_TO_WCDMA */

#ifdef  FEATURE_LTE_TO_1X
  #define LTE_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED  TRUE /* Only for SRVCC*/
  #define LTE_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED         TRUE
  #define FEATURE_LTE_1XCSFB_SUPPORT_DEFAULT            TRUE
#else
  #define LTE_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED  FALSE
  #define LTE_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED         FALSE
#endif /* FEATURE_LTE_TO_1X */

#if defined (FEATURE_LTE_TO_HDR)
  #define LTE_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED        TRUE
  #define LTE_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED         TRUE
#else
  #define LTE_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED        FALSE
  #define LTE_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED         FALSE
#endif

// Default Release 8.
//top level feature removed: #define LTE_3GPP_DEFAULT_RELEASE_VERSION LTE_3GPP_REL9_SEP10


#ifdef FEATURE_LTE_TO_GSM
//top level feature removed:
  #define LTE_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED         TRUE
  #define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED      TRUE
  #define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED    TRUE
//top level feature removed:   #define FEATURE_LTE_TO_GSM_CGI                          TRUE
#else
  #define LTE_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED         FALSE
  #define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED      FALSE
  #define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED    FALSE
//top level feature removed:   #define FEATURE_LTE_TO_GSM_CGI                          FALSE
#endif

/* Maximum number of LTE instances that can be up at any given time */
#define LTE_MEM_MAX_INST                 1

#define FEATURE_LTE_ML1_POS_PRS_IC
#define FEATURE_LTE_ML1_POS_ECID
#define FEATURE_LTE_ML1_POS_RXTX
#ifdef FEATURE_LOC_GTS_SUPPORT
#define FEATURE_LTE_ML1_POS_GTS
#endif

#ifdef FEATURE_LOC_LTE_OTDOA_OPCRS
#define FEATURE_LTE_ML1_POS_OPCRS
#endif

#if (!defined(FEATURE_TABASCO_MODEM) && !defined(FEATURE_LTE_ATLAS_MODEM))
#define FEATURE_LTE_ML1_POS_PRS_FD_BUF_LOG
#endif

#define FEATURE_LTE_ML1_POS_PRS
#define FEATURE_LTE_ML1_POS_INTER_FREQUENCY_PRS

#ifdef FEATURE_CGPS
#define FEATURE_LTE_ML1_POS_TIME_TRANSFER
#endif

#define FEATURE_LTE_ML1_POS_TIMETAG

#define FEATURE_LTE_DRX_OPT

#ifdef T_WINNT
  #error code not present
#endif

/*Define DI-2.x feature flags*/


#define FEATURE_LTE_TDD_TO_UMTS_WITH_MEAS  TRUE
#define LTE_HANDOVER_BETWEEN_FDD_AND_TDD_DEFINED TRUE

#ifdef FEATURE_BOLT_MODEM
 #define FEATURE_LTE_TDD_CA  // Enabled TDD CA on Bolt targets only.
 #define FEATURE_LTE_EICIC
#endif

 #define FEATURE_LTE_TDD_TO_GSM_SRVCC


#define LTE_ML1_SM_CONN_NEW_IMPL
#define LTE_ML1_SM_CONN_NEW_PBCH

/*
#ifdef FEATURE_CUST_1
#define FEATURE_LTE_CDRX_IMS_VOLTE_OPT
#endif
*/

/* Flag for indicating that only few BWs are supported */
/* All BWs are now supported and this flag will not be needed until at least the next target
#ifdef FEATURE_THOR_MODEM
  #define FEATURE_LTE_BRINGUP_RESTRICTED_BW
#endif
*/

/*! @brief Feature flag for LTE EMBMBS */
#define FEATURE_LTE_EMBMS

/*! @brief If target has defined FEATURE_LTE_ELS_DISABLED then
  undefine FEATURE_LTE_ELS_ENABLED feature */
#ifdef FEATURE_LTE_ELS_DISABLED
    #undef FEATURE_LTE_ELS_ENABLED
#endif /* FEATURE_LTE_ELS_DISABLED */


/*! @brief If target has defined FEATURE_LTE_UDC_DISABLED then
  undefine FEATURE_LTE_UDC_ENABLED feature */
#ifdef FEATURE_LTE_UDC_DISABLED
    #undef FEATURE_LTE_UDC_ENABLED
#endif /* FEATURE_LTE_UDC_DISABLED */

#ifdef FEATURE_LTE_UDC_ENABLED
  #ifndef FEATURE_LTE_ELS_ENABLED
    #error “ELS Disabled and UDC Enabled – wrong feature flag setting”
  #endif /* FEATURE_LTE_ELS_ENABLED */
#endif /* FEATURE_LTE_UDC_ENABLED */

#define FEATURE_LTE_HST
/* Disable IF-OTDOA by default */
#define FEATURE_DISABLE_IF_PRS

#endif /* CUSTLTE_H */

