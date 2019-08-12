/*!
  @file lte_rrc_diag.h

  @brief
  Macros and defines for RRC specific diagnostic messages

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/11   np      Masks for From 1x STM
09/07/10   sk      Masks for From DO STM
01/26/10   np      Masks for From WCDMA STM
01/24/10   sk      Added maskes for From-G RAT Mgr
01/14/10   sk      Added masks for To-G RAT Mgr
12/22/09   np      Masks for LTE to WCDMA STM
10/06/09   np      Masks for LTE to 1x redirection
10/02/09   sk      Fixed mask name to indicate To DO
09/17/09   sureshs Added masks for sprintf low
09/11/09   sureshs Added masks for Capabilities
09/02/09   sk      Added masks for DO RAT STM
08/26/09   sureshs Added masks for STM
10/08/08   amit    Added parenthesis to macros
10/08/08   amit    Corrected LLC and MH mask typos
07/23/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_DIAG_H
#define LTE_RRC_DIAG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msg.h>

/* Diag level Masks  (xx_ss_mask) for the SIB Module */
#define LTE_RRC_SIB_MASK  MSG_MASK_8 
#define LTE_RRC_SIB_LOW   (LTE_RRC_SIB_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_SIB_MED   (LTE_RRC_SIB_MASK | MSG_LEGACY_MED)
#define LTE_RRC_SIB_HIGH  (LTE_RRC_SIB_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_SIB_ERROR (LTE_RRC_SIB_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CSP Module */
#define LTE_RRC_CSP_MASK MSG_MASK_9 
#define LTE_RRC_CSP_LOW   (LTE_RRC_CSP_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CSP_MED   (LTE_RRC_CSP_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CSP_HIGH  (LTE_RRC_CSP_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CSP_ERROR (LTE_RRC_CSP_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CEP Module */
#define LTE_RRC_CEP_MASK MSG_MASK_10 
#define LTE_RRC_CEP_LOW   (LTE_RRC_CEP_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CEP_MED   (LTE_RRC_CEP_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CEP_HIGH  (LTE_RRC_CEP_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CEP_ERROR (LTE_RRC_CEP_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CTRL Module */
#define LTE_RRC_CTRL_MASK MSG_MASK_11 
#define LTE_RRC_CTRL_LOW   (LTE_RRC_CTRL_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CTRL_MED   (LTE_RRC_CTRL_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CTRL_HIGH  (LTE_RRC_CTRL_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CTRL_ERROR (LTE_RRC_CTRL_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CRP Module */
#define LTE_RRC_CRP_MASK MSG_MASK_12 
#define LTE_RRC_CRP_LOW   (LTE_RRC_CRP_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CRP_MED   (LTE_RRC_CRP_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CRP_HIGH  (LTE_RRC_CRP_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CRP_ERROR (LTE_RRC_CRP_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the PAG Module */
#define LTE_RRC_PAG_MASK MSG_MASK_13 
#define LTE_RRC_PAG_LOW   (LTE_RRC_PAG_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_PAG_MED   (LTE_RRC_PAG_MASK | MSG_LEGACY_MED)
#define LTE_RRC_PAG_HIGH  (LTE_RRC_PAG_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_PAG_ERROR (LTE_RRC_PAG_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CFG Module */
#define LTE_RRC_CFG_MASK MSG_MASK_14 
#define LTE_RRC_CFG_LOW   (LTE_RRC_CFG_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CFG_MED   (LTE_RRC_CFG_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CFG_HIGH  (LTE_RRC_CFG_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CFG_ERROR (LTE_RRC_CFG_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the LLC Module */
#define LTE_RRC_LLC_MASK MSG_MASK_15 
#define LTE_RRC_LLC_LOW   (LTE_RRC_LLC_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_LLC_MED   (LTE_RRC_LLC_MASK | MSG_LEGACY_MED)
#define LTE_RRC_LLC_HIGH  (LTE_RRC_LLC_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_LLC_ERROR (LTE_RRC_LLC_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the MH Module */
#define LTE_RRC_MH_MASK MSG_MASK_16 
#define LTE_RRC_MH_LOW   (LTE_RRC_MH_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_MH_MED   (LTE_RRC_MH_MASK | MSG_LEGACY_MED)
#define LTE_RRC_MH_HIGH  (LTE_RRC_MH_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_MH_ERROR (LTE_RRC_MH_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the DT Module */
#define LTE_RRC_DT_MASK MSG_MASK_17 
#define LTE_RRC_DT_LOW   (LTE_RRC_DT_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_DT_MED   (LTE_RRC_DT_MASK | MSG_LEGACY_MED)
#define LTE_RRC_DT_HIGH  (LTE_RRC_DT_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_DT_ERROR (LTE_RRC_DT_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CRE Module */
#define LTE_RRC_CRE_MASK MSG_MASK_18 
#define LTE_RRC_CRE_LOW   (LTE_RRC_CRE_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CRE_MED   (LTE_RRC_CRE_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CRE_HIGH  (LTE_RRC_CRE_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CRE_ERROR (LTE_RRC_CRE_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the SEC Module */
#define LTE_RRC_SEC_MASK MSG_MASK_19
#define LTE_RRC_SEC_LOW   (LTE_RRC_SEC_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_SEC_MED   (LTE_RRC_SEC_MASK | MSG_LEGACY_MED)
#define LTE_RRC_SEC_HIGH  (LTE_RRC_SEC_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_SEC_ERROR (LTE_RRC_SEC_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the MEAS Module */
#define LTE_RRC_MEAS_MASK MSG_MASK_20
#define LTE_RRC_MEAS_LOW   (LTE_RRC_MEAS_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_MEAS_MED   (LTE_RRC_MEAS_MASK | MSG_LEGACY_MED)
#define LTE_RRC_MEAS_HIGH  (LTE_RRC_MEAS_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_MEAS_ERROR (LTE_RRC_MEAS_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the STM Module */
#define LTE_RRC_STM_MASK MSG_MASK_21
#define LTE_RRC_STM_LOW   (LTE_RRC_STM_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_STM_MED   (LTE_RRC_STM_MASK | MSG_LEGACY_MED)
#define LTE_RRC_STM_HIGH  (LTE_RRC_STM_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_STM_ERROR (LTE_RRC_STM_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the PLMN_SEARCH Module */
#define LTE_RRC_PLMN_SEARCH_MASK MSG_MASK_22
#define LTE_RRC_PLMN_SEARCH_LOW   (LTE_RRC_PLMN_SEARCH_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_PLMN_SEARCH_MED   (LTE_RRC_PLMN_SEARCH_MASK | MSG_LEGACY_MED)
#define LTE_RRC_PLMN_SEARCH_HIGH  (LTE_RRC_PLMN_SEARCH_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_PLMN_SEARCH_ERROR (LTE_RRC_PLMN_SEARCH_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the Capabilities Module */
#define LTE_RRC_CAP_MASK MSG_MASK_23
#define LTE_RRC_CAP_LOW   (LTE_RRC_CAP_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CAP_MED   (LTE_RRC_CAP_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CAP_HIGH  (LTE_RRC_CAP_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CAP_ERROR (LTE_RRC_CAP_MASK | MSG_LEGACY_ERROR)

/* Diag level generic Masks  (xx_ss_mask) for RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_MGR_LOW   (LTE_RRC_IRAT_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_GR_MED    (LTE_RRC_IRAT_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_MGR_HIGH  (LTE_RRC_IRAT_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_MGR_ERROR (LTE_RRC_IRAT_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for To DO RAT manager Module - use MASK_24 for IRAT modules*/
#define LTE_RRC_IRAT_TO_DO_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_TO_DO_MGR_LOW   (LTE_RRC_IRAT_TO_DO_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_TO_DO_MGR_MED   (LTE_RRC_IRAT_TO_DO_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_TO_DO_MGR_HIGH  (LTE_RRC_IRAT_TO_DO_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_TO_DO_MGR_ERROR (LTE_RRC_IRAT_TO_DO_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for To 1x RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_TO_1X_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_TO_1X_MGR_LOW   (LTE_RRC_IRAT_TO_1X_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_TO_1X_MGR_MED   (LTE_RRC_IRAT_TO_1X_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_TO_1X_MGR_HIGH  (LTE_RRC_IRAT_TO_1X_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_TO_1X_MGR_ERROR (LTE_RRC_IRAT_TO_1X_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for To WCDMA RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_TO_W_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_TO_W_MGR_LOW   (LTE_RRC_IRAT_TO_W_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_TO_W_MGR_MED   (LTE_RRC_IRAT_TO_W_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_TO_W_MGR_HIGH  (LTE_RRC_IRAT_TO_W_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_TO_W_MGR_ERROR (LTE_RRC_IRAT_TO_W_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for From WCDMA RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_FROM_W_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_FROM_W_MGR_LOW   (LTE_RRC_IRAT_FROM_W_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_FROM_W_MGR_MED   (LTE_RRC_IRAT_FROM_W_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_FROM_W_MGR_HIGH  (LTE_RRC_IRAT_FROM_W_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_FROM_W_MGR_ERROR (LTE_RRC_IRAT_FROM_W_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for To TDSCDMA RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_TO_TDS_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_TO_TDS_MGR_LOW   (LTE_RRC_IRAT_TO_TDS_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_TO_TDS_MGR_MED   (LTE_RRC_IRAT_TO_TDS_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_TO_TDS_MGR_HIGH  (LTE_RRC_IRAT_TO_TDS_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_TO_TDS_MGR_ERROR (LTE_RRC_IRAT_TO_TDS_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for From WCDMA RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_FROM_TDS_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_FROM_TDS_MGR_LOW   (LTE_RRC_IRAT_FROM_TDS_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_FROM_TDS_MGR_MED   (LTE_RRC_IRAT_FROM_TDS_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_FROM_TDS_MGR_HIGH  (LTE_RRC_IRAT_FROM_TDS_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_FROM_TDS_MGR_ERROR (LTE_RRC_IRAT_FROM_TDS_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for to G RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_TO_G_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_TO_G_MGR_LOW   (LTE_RRC_IRAT_TO_G_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_TO_G_MGR_MED   (LTE_RRC_IRAT_TO_G_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_TO_G_MGR_HIGH  (LTE_RRC_IRAT_TO_G_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_TO_G_MGR_ERROR (LTE_RRC_IRAT_TO_G_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for From G RAT manager Module - use MASK_24 for IRAT modules  */
#define LTE_RRC_IRAT_FROM_G_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_FROM_G_MGR_LOW   (LTE_RRC_IRAT_FROM_G_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_FROM_G_MGR_MED   (LTE_RRC_IRAT_FROM_G_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_FROM_G_MGR_HIGH  (LTE_RRC_IRAT_FROM_G_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_FROM_G_MGR_ERROR (LTE_RRC_IRAT_FROM_G_MGR_MASK | MSG_LEGACY_ERROR)


/* Diag level Masks  (xx_ss_mask) for the From DO RAT manager Module - use MASK_24 for IRAT modules  */
#define LTE_RRC_IRAT_FROM_DO_MGR_MASK MSG_MASK_24
#define LTE_RRC_IRAT_FROM_DO_MGR_LOW   (LTE_RRC_IRAT_FROM_DO_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_FROM_DO_MGR_MED   (LTE_RRC_IRAT_FROM_DO_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_FROM_DO_MGR_HIGH  (LTE_RRC_IRAT_FROM_DO_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_FROM_DO_MGR_ERROR (LTE_RRC_IRAT_FROM_DO_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the From 1X RAT manager Module - use MASK_24 for IRAT modules */
#define LTE_RRC_IRAT_FROM_1X_MGR_MASK MSG_MASK_24 
#define LTE_RRC_IRAT_FROM_1X_MGR_LOW   (LTE_RRC_IRAT_FROM_1X_MGR_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_IRAT_FROM_1X_MGR_MED   (LTE_RRC_IRAT_FROM_1X_MGR_MASK | MSG_LEGACY_MED)
#define LTE_RRC_IRAT_FROM_1X_MGR_HIGH  (LTE_RRC_IRAT_FROM_1X_MGR_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_IRAT_FROM_1X_MGR_ERROR (LTE_RRC_IRAT_FROM_1X_MGR_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the From 1X RAT manager Module - use MASK_25 for EMBMS modules */
#define LTE_RRC_EMBMS_MASK MSG_MASK_25 
#define LTE_RRC_EMBMS_LOW   (LTE_RRC_EMBMS_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_EMBMS_MED   (LTE_RRC_EMBMS_MASK | MSG_LEGACY_MED)
#define LTE_RRC_EMBMS_HIGH  (LTE_RRC_EMBMS_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_EMBMS_ERROR (LTE_RRC_EMBMS_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the UE Info Module */
#define LTE_RRC_UEINFO_MASK MSG_MASK_26
#define LTE_RRC_UEINFO_LOW   (LTE_RRC_UEINFO_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_UEINFO_MED   (LTE_RRC_UEINFO_MASK | MSG_LEGACY_MED)
#define LTE_RRC_UEINFO_HIGH  (LTE_RRC_UEINFO_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_UEINFO_ERROR (LTE_RRC_UEINFO_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the CSG_ASF Module */
#define LTE_RRC_CSG_ASF_MASK MSG_MASK_27
#define LTE_RRC_CSG_ASF_LOW   (LTE_RRC_CSG_ASF_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_CSG_ASF_MED   (LTE_RRC_CSG_ASF_MASK | MSG_LEGACY_MED)
#define LTE_RRC_CSG_ASF_HIGH  (LTE_RRC_CSG_ASF_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_CSG_ASF_ERROR (LTE_RRC_CSG_ASF_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the From MISC Module - use MASK_26 for MISC modules */
#define LTE_RRC_MISC_MASK MSG_MASK_28 
#define LTE_RRC_MISC_LOW   (LTE_RRC_MISC_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_MISC_MED   (LTE_RRC_MISC_MASK | MSG_LEGACY_MED)
#define LTE_RRC_MISC_HIGH  (LTE_RRC_MISC_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_MISC_ERROR (LTE_RRC_MISC_MASK | MSG_LEGACY_ERROR)

/* Diag level Masks  (xx_ss_mask) for the DIAG Module */
#define LTE_RRC_DIAG_MASK MSG_MASK_29
#define LTE_RRC_DIAG_LOW   (LTE_RRC_DIAG_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_DIAG_MED   (LTE_RRC_DIAG_MASK | MSG_LEGACY_MED)
#define LTE_RRC_DIAG_HIGH  (LTE_RRC_DIAG_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_DIAG_ERROR (LTE_RRC_DIAG_MASK | MSG_LEGACY_ERROR)


/* Diag level Masks  (xx_ss_mask) for the From MISC Module - use MASK_30 for MDT modules */
#define LTE_RRC_MDT_MASK MSG_MASK_30 
#define LTE_RRC_MDT_LOW   (LTE_RRC_MDT_MASK | MSG_LEGACY_LOW)
#define LTE_RRC_MDT_MED   (LTE_RRC_MDT_MASK | MSG_LEGACY_MED)
#define LTE_RRC_MDT_HIGH  (LTE_RRC_MDT_MASK | MSG_LEGACY_HIGH)
#define LTE_RRC_MDT_ERROR (LTE_RRC_MDT_MASK | MSG_LEGACY_ERROR)


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_MSG(xx_ss_mask, xx_fmt)                                 \
  MSG(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt)

#define LTE_RRC_MSG_1(xx_ss_mask, xx_fmt, a)                            \
  MSG_1(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a)

#define LTE_RRC_MSG_2(xx_ss_mask, xx_fmt, a, b)                         \
  MSG_2(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b)

#define LTE_RRC_MSG_3(xx_ss_mask, xx_fmt, a, b, c)                      \
  MSG_3(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c)

#define LTE_RRC_MSG_4(xx_ss_mask, xx_fmt, a, b, c, d)                   \
  MSG_4(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c, d)

#define LTE_RRC_MSG_5(xx_ss_mask, xx_fmt, a, b, c, d, e)                \
  MSG_5(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c, d, e)

#define LTE_RRC_MSG_6(xx_ss_mask, xx_fmt, a, b, c, d, e, f)             \
  MSG_6(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c, d, e, f)

#define LTE_RRC_MSG_7(xx_ss_mask, xx_fmt, a, b, c, d, e, f, g)          \
  MSG_7(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c, d, e, f, g)

#define LTE_RRC_MSG_8(xx_ss_mask, xx_fmt, a, b, c, d, e, f, g, h)       \
  MSG_8(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b, c, d, e, f, g, h)

#define LTE_RRC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, a)                    \
  MSG_SPRINTF_1(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a)

#define LTE_RRC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, a, b)                 \
  MSG_SPRINTF_2(MSG_SSID_LTE_RRC, (xx_ss_mask), xx_fmt, a, b)


#define LTE_RRC_MSG_MED(xx_fmt)                                         \
  MSG(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt)

#define LTE_RRC_MSG_1_MED(xx_fmt, a)                                    \
  MSG_1(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a)

#define LTE_RRC_MSG_2_MED(xx_fmt, a, b)                                 \
  MSG_2(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b)

#define LTE_RRC_MSG_3_MED(xx_fmt, a, b, c)                              \
  MSG_3(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c)

#define LTE_RRC_MSG_4_MED(xx_fmt, a, b, c, d)                           \
  MSG_4(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c, d)

#define LTE_RRC_MSG_5_MED(xx_fmt, a, b, c, d, e)                        \
  MSG_5(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e)

#define LTE_RRC_MSG_6_MED(xx_fmt, a, b, c, d, e, f)                     \
  MSG_6(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e, f)

#define LTE_RRC_MSG_7_MED(xx_fmt, a, b, c, d, e, f, g)                  \
  MSG_7(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e, f, g)

#define LTE_RRC_MSG_8_MED(xx_fmt, a, b, c, d, e, f, g, h)               \
  MSG_8(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b, c, d, e, f, g, h)


#define LTE_RRC_MSG_HIGH(xx_fmt)                                        \
  MSG(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt)

#define LTE_RRC_MSG_1_HIGH(xx_fmt, a)                                   \
  MSG_1(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a)

#define LTE_RRC_MSG_2_HIGH(xx_fmt, a, b)                                \
  MSG_2(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b)

#define LTE_RRC_MSG_3_HIGH(xx_fmt, a, b, c)                             \
  MSG_3(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c)

#define LTE_RRC_MSG_4_HIGH(xx_fmt, a, b, c, d)                          \
  MSG_4(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d)

#define LTE_RRC_MSG_5_HIGH(xx_fmt, a, b, c, d, e)                       \
  MSG_5(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e)

#define LTE_RRC_MSG_6_HIGH(xx_fmt, a, b, c, d, e, f)                    \
  MSG_6(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e, f)

#define LTE_RRC_MSG_7_HIGH(xx_fmt, a, b, c, d, e, f, g)                 \
  MSG_7(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e, f, g)

#define LTE_RRC_MSG_8_HIGH(xx_fmt, a, b, c, d, e, f, g, h)              \
  MSG_8(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e, f, g, h)


#define LTE_RRC_MSG_FATAL(xx_fmt)                                       \
  MSG(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt)

#define LTE_RRC_MSG_1_FATAL(xx_fmt, a)                                  \
  MSG_1(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a)

#define LTE_RRC_MSG_2_FATAL(xx_fmt, a, b)                               \
  MSG_2(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b)

#define LTE_RRC_MSG_3_FATAL(xx_fmt, a, b, c)                            \
  MSG_3(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c)

#define LTE_RRC_MSG_4_FATAL(xx_fmt, a, b, c, d)                         \
  MSG_4(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c, d)

#define LTE_RRC_MSG_5_FATAL(xx_fmt, a, b, c, d, e)                      \
  MSG_5(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c, d, e)

#define LTE_RRC_MSG_6_FATAL(xx_fmt, a, b, c, d, e, f)                   \
  MSG_6(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c, d, e, f)

#define LTE_RRC_MSG_7_FATAL(xx_fmt, a, b, c, d, e, f, g)                \
  MSG_7(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c, d, e, f, g)

#define LTE_RRC_MSG_8_FATAL(xx_fmt, a, b, c, d, e, f, g, h)             \
  MSG_8(MSG_SSID_LTE_RRC, MSG_LEGACY_FATAL, xx_fmt, a, b, c, d, e, f, g, h)


#define LTE_RRC_MSG_SPRINTF_1_LOW(xx_fmt, a)                            \
  MSG_SPRINTF_1(MSG_SSID_LTE_RRC, MSG_LEGACY_LOW, xx_fmt, a)

#define LTE_RRC_MSG_SPRINTF_2_LOW(xx_fmt, a, b)                         \
  MSG_SPRINTF_2(MSG_SSID_LTE_RRC, MSG_LEGACY_LOW, xx_fmt, a, b)

#define LTE_RRC_MSG_SPRINTF_1_MED(xx_fmt, a)                            \
  MSG_SPRINTF_1(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a)

#define LTE_RRC_MSG_SPRINTF_2_MED(xx_fmt, a, b)                         \
  MSG_SPRINTF_2(MSG_SSID_LTE_RRC, MSG_LEGACY_MED, xx_fmt, a, b)


#define LTE_RRC_MSG_SPRINTF_1_HIGH(xx_fmt, a)                            \
  MSG_SPRINTF_1(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a)

#define LTE_RRC_MSG_SPRINTF_2_HIGH(xx_fmt, a, b)                         \
  MSG_SPRINTF_2(MSG_SSID_LTE_RRC, MSG_LEGACY_HIGH, xx_fmt, a, b)


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* __LTE_RRC_DIAG_H__ */
