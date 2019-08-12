
/*!
  @file
  sar_efs.c

  @brief
  This file implements the EFS functions for LMTSMGR SAR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/src/sar_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/16   jm      Convert high-rate F3 messaging to off-target only
02/12/14   rj      Added support for GPIO polarity and BootUp BackOff
05/29/13   jk      Helper fuctions to read and store the compatibility key from EFS
05/20/13   rj      mcs_limitsmgr.conf EFS file optimization
04/05/13   rj      GPIO Based SAR Control added
02/04/13   rj      Migrate SVDO/SVLTE SAR/IM3 support from RF SW to MCS
09/19/12   ag      DSI 0 should be considered default with bias as 0
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "fs_sys_types.h"
#include "fs_public.h"
#include "fs_lib.h"
#include "fs_errno.h"
#include "stringl.h"
#include "cxm.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_task.h"
#include "modem_mem.h"
#include "sar_efs.h"


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

/* SAR LUT pointers for 1X(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)    
sar_lut_type sar_1x_gsm1_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_CDMA1X */
sar_tech_comb_lut_type sar_1x_gsm1_info = {
  {
    LIMITSMGR_PWR_1X_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_1X_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)    
  (void *)&sar_1x_gsm1_luts,
  #else                                                                        
  NULL,
  #endif /* FEATURE_GSM && FEATURE_CDMA1X */
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for 1X(high priority) and LTE(low priority) */
#if defined(FEATURE_LTE) && defined(FEATURE_CDMA1X)
sar_lut_type sar_1x_lte_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_LTE_BANDS];
#endif /* FEATURE_LTE && FEATURE_CDMA1X */ 
sar_tech_comb_lut_type sar_1x_lte_info = {
  {
    LIMITSMGR_PWR_1X_HIGH_LTE_LOW,
    LIMITSMGR_DSI_BIAS_1X_HIGH_LTE_LOW,
  },
  #if defined(FEATURE_LTE) && defined(FEATURE_CDMA1X)
  (void *)&sar_1x_lte_luts,
  #else                                                                        
  NULL,
  #endif /* FEATURE_LTE && FEATURE_CDMA1X */ 
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_LTE_BANDS,
};

/* SAR LUT pointers for 1X(high priority) and HDR(low priority) */
#if defined(FEATURE_HDR) && defined(FEATURE_CDMA1X)
sar_lut_type sar_1x_hdr_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_1X_BANDS];
#endif /* FEATURE_HDR && FEATURE_CDMA1X */
sar_tech_comb_lut_type sar_1x_hdr_info = {
  {
    LIMITSMGR_PWR_1X_HIGH_HDR_LOW,
    LIMITSMGR_DSI_BIAS_1X_HIGH_HDR_LOW,
  },
  #if defined(FEATURE_HDR) && defined(FEATURE_CDMA1X)
  (void *)&sar_1x_hdr_luts,
  #else                                                                        
  NULL,
  #endif /* FEATURE_HDR && FEATURE_CDMA1X */
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_1X_BANDS,
};

/* SAR LUT pointers for 1X(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
sar_lut_type sar_1x_gsm2_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_CDMA1X */
sar_tech_comb_lut_type sar_1x_gsm2_info = {
  {
    LIMITSMGR_PWR_1X_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_1X_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
  (void *)&sar_1x_gsm2_luts,
  #else  
  NULL,
  #endif /* FEATURE_GSM && FEATURE_CDMA1X */
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and 1x(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
sar_lut_type sar_gsm1_1x_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_1X_BANDS];
#endif /* FEATURE_GSM && FEATURE_CDMA1X */
sar_tech_comb_lut_type sar_gsm1_1x_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_1X_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_1X_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
  (void *)&sar_gsm1_1x_luts,
  #else  
  NULL,
  #endif /* FEATURE_GSM && FEATURE_CDMA1X */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and WCDMA(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
sar_lut_type sar_gsm1_wcdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_WCDMA_BANDS];
#endif /* FEATURE_GSM && FEATURE_WCDMA */
sar_tech_comb_lut_type sar_gsm1_wcdma_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_WCDMA_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_WCDMA_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
  (void *)&sar_gsm1_wcdma_luts,
  #else  
  NULL,
  #endif /* FEATURE_GSM && FEATURE_WCDMA */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_WCDMA_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and LTE(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_LTE)
sar_lut_type sar_gsm1_lte_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_LTE_BANDS];
#endif /* FEATURE_GSM && FEATURE_LTE */
sar_tech_comb_lut_type sar_gsm1_lte_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_LTE_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_LTE_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_LTE)
  (void *)&sar_gsm1_lte_luts,
  #else  
  NULL,
  #endif /* FEATURE_GSM && FEATURE_LTE */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_LTE_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and TDSCDMA(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
sar_lut_type sar_gsm1_tdscdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_TDSCDMA_BANDS];
#endif /* FEATURE_GSM && FEATURE_TDSCDMA */
sar_tech_comb_lut_type sar_gsm1_tdscdma_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_TDSCDMA_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_TDSCDMA_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
  (void *)&sar_gsm1_tdscdma_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_TDSCDMA */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_TDSCDMA_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and HDR(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_HDR)
sar_lut_type sar_gsm1_hdr_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_1X_BANDS];
#endif /* FEATURE_GSM && FEATURE_HDR */
sar_tech_comb_lut_type sar_gsm1_hdr_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_HDR_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_HDR_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_HDR)
  (void *)&sar_gsm1_hdr_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_HDR */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS,
};

/* SAR LUT pointers for GSM1(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM)
sar_lut_type sar_gsm1_gsm2_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM */
sar_tech_comb_lut_type sar_gsm1_gsm2_info = {
  {
    LIMITSMGR_PWR_GSM1_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_GSM1_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM)
  (void *)&sar_gsm1_gsm2_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for WCDMA(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
sar_lut_type sar_wcdma_gsm1_luts[RFCOM_NUM_WCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_WCDMA */
sar_tech_comb_lut_type sar_wcdma_gsm1_info = {
  {
    LIMITSMGR_PWR_WCDMA_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
  (void *)&sar_wcdma_gsm1_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_WCDMA */
  RFCOM_NUM_WCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for WCDMA(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
sar_lut_type sar_wcdma_gsm2_luts[RFCOM_NUM_WCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_WCDMA */
sar_tech_comb_lut_type sar_wcdma_gsm2_info = {
  {
    LIMITSMGR_PWR_WCDMA_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
  (void *)&sar_wcdma_gsm2_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_WCDMA */
  RFCOM_NUM_WCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for LTE(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_LTE)
sar_lut_type sar_lte_gsm1_luts[RFCOM_NUM_LTE_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_LTE */
sar_tech_comb_lut_type sar_lte_gsm1_info = {
  {
    LIMITSMGR_PWR_LTE_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_LTE)
  (void *)&sar_lte_gsm1_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_LTE */
  RFCOM_NUM_LTE_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for LTE(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_LTE)
sar_lut_type sar_lte_gsm2_luts[RFCOM_NUM_LTE_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_LTE */
sar_tech_comb_lut_type sar_lte_gsm2_info = {
  {
    LIMITSMGR_PWR_LTE_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_LTE)
  (void *)&sar_lte_gsm2_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_LTE */
  RFCOM_NUM_LTE_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for TDSCDMA(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
sar_lut_type sar_tdscdma_gsm1_luts[RFCOM_NUM_TDSCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_TDSCDMA */
sar_tech_comb_lut_type sar_tdscdma_gsm1_info = {
  {
    LIMITSMGR_PWR_TDSCDMA_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
  (void *)&sar_tdscdma_gsm1_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_TDSCDMA */
  RFCOM_NUM_TDSCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for TDSCDMA(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
sar_lut_type sar_tdscdma_gsm2_luts[RFCOM_NUM_TDSCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_TDSCDMA */
sar_tech_comb_lut_type sar_tdscdma_gsm2_info = {
  {
    LIMITSMGR_PWR_TDSCDMA_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
  (void *)&sar_tdscdma_gsm2_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_TDSCDMA */
  RFCOM_NUM_TDSCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for HDR(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_HDR)
sar_lut_type sar_hdr_gsm1_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_HDR */
sar_tech_comb_lut_type sar_hdr_gsm1_info = {
  {
    LIMITSMGR_PWR_HDR_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_HDR)
  (void *)&sar_hdr_gsm1_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_HDR */
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for HDR(high priority) and GSM2(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_HDR)
sar_lut_type sar_hdr_gsm2_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_HDR */
sar_tech_comb_lut_type sar_hdr_gsm2_info = {
  {
    LIMITSMGR_PWR_HDR_HIGH_GSM2_LOW,
    LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM2_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_HDR)
  (void *)&sar_hdr_gsm2_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_HDR */
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and 1x(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
sar_lut_type sar_gsm2_1x_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM && FEATURE_CDMA1X */
sar_tech_comb_lut_type sar_gsm2_1x_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_1X_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_1X_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
  (void *)&sar_gsm2_1x_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_CDMA1X */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and GSM1(low priority) */
#if defined(FEATURE_GSM)
sar_lut_type sar_gsm2_gsm1_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
#endif /* FEATURE_GSM */
sar_tech_comb_lut_type sar_gsm2_gsm1_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_GSM1_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_GSM1_LOW,
  },
  #if defined(FEATURE_GSM)
  (void *)&sar_gsm2_gsm1_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and wcdma(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
sar_lut_type sar_gsm2_wcdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_WCDMA_BANDS];
#endif /* FEATURE_GSM && FEATURE_WCDMA */
sar_tech_comb_lut_type sar_gsm2_wcdma_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_WCDMA_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_WCDMA_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
  (void *)&sar_gsm2_wcdma_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_WCDMA */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_WCDMA_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and LTE(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_LTE)
sar_lut_type sar_gsm2_lte_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_LTE_BANDS];
#endif /* FEATURE_GSM && FEATURE_LTE */
sar_tech_comb_lut_type sar_gsm2_lte_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_LTE_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_LTE_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_LTE)
  (void *)&sar_gsm2_lte_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_LTE */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_LTE_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and HDR(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_HDR)
sar_lut_type sar_gsm2_hdr_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_1X_BANDS];
#endif /* FEATURE_GSM && FEATURE_HDR */
sar_tech_comb_lut_type sar_gsm2_hdr_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_HDR_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_HDR_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_HDR)
  (void *)&sar_gsm2_hdr_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_HDR */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS,
};

/* SAR LUT pointers for GSM2(high priority) and TDSCDMA(low priority) */
#if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
sar_lut_type sar_gsm2_tdscdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_TDSCDMA_BANDS];
#endif /* FEATURE_GSM && FEATURE_TDSCDMA */
sar_tech_comb_lut_type sar_gsm2_tdscdma_info = {
  {
    LIMITSMGR_PWR_GSM2_HIGH_TDSCDMA_LOW,
    LIMITSMGR_DSI_BIAS_GSM2_HIGH_TDSCDMA_LOW,
  },
  #if defined(FEATURE_GSM) && defined(FEATURE_TDSCDMA)
  (void *)&sar_gsm2_tdscdma_luts,
  #else
  NULL,
  #endif /* FEATURE_GSM && FEATURE_TDSCDMA */
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_TDSCDMA_BANDS,
};

sar_tech_comb_lut_type* sar_tables[CXM_TECH_MAX][CXM_TECH_MAX] = {
  {NULL, NULL, &sar_lte_gsm1_info, NULL, NULL, NULL, &sar_lte_gsm2_info},
  {NULL, NULL, &sar_tdscdma_gsm1_info, NULL, NULL, NULL, &sar_tdscdma_gsm2_info},
  {&sar_gsm1_lte_info, &sar_gsm1_tdscdma_info, NULL, &sar_gsm1_1x_info, &sar_gsm1_hdr_info, &sar_gsm1_wcdma_info, &sar_gsm1_gsm2_info},
  {&sar_1x_lte_info, NULL, &sar_1x_gsm1_info, NULL, &sar_1x_hdr_info, NULL, &sar_1x_gsm2_info},
  {NULL, NULL, &sar_hdr_gsm1_info, NULL, NULL, NULL, &sar_hdr_gsm2_info},
  {NULL, NULL, &sar_wcdma_gsm1_info, NULL, NULL, NULL, &sar_wcdma_gsm2_info},
  {&sar_gsm2_lte_info, &sar_gsm2_tdscdma_info, &sar_gsm2_gsm1_info, &sar_gsm2_1x_info, &sar_gsm2_hdr_info, &sar_gsm2_wcdma_info, NULL},
};


/* Default LUT state */                              
sar_lut_type sar_default_luts = {
  NULL,
  NULL
};


/*Compatibility Key for SAR QMI*/
uint32 sar_qmi_comp_key=0;

/*Variable that tells if the key is present in the EFS or not*/
uint8 sar_qmi_key_valid=0;

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*=============================================================================

                                FUNCTIONS

=============================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*=============================================================================

  FUNCTION:  sar_store_sar_pwr_gplusx_rec

=============================================================================*/
/*!
    @brief
    Stores a SAR PWR record in heap and pointer in the global space
 
    @note
    sar_efs_validate_sar_pwr_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void sar_store_sar_pwr_gplusx_rec
(
  sar_efs_pwr_gplusx_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  sar_tech_comb_lut_type* sar_techs_lut;
  sar_pwr_lut_type* sar_pwr_lut = NULL;
  uint32 hband, lband, index;
  sar_lut_type* sar_lut;
  uint8  index_i, index_j;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  sar_pwr_lut=(sar_pwr_lut_type*)modem_mem_alloc(sizeof(sar_pwr_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (sar_pwr_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */

  /* Is there a high prio tech backoff to be applied ? */
  if((boolean)(sar_pwr_rec->apply_hpriotech_limit) == FALSE)
  {
    sar_pwr_lut->num_valid_hprio_limits = 0;
  }
  else
  {
    /* Copy all the 8 limits */
    sar_pwr_lut->num_valid_hprio_limits = SAR_GSM_MAX_LIMIT_TYPE;
    memscpy((void *)sar_pwr_lut->hprio_limits,
            sizeof(sar_pwr_lut->hprio_limits), 
            (void *)sar_pwr_rec->hprio_limits,
            sizeof(sar_pwr_rec->hprio_limits));
  }

  /* Copy the LUT... */
  sar_pwr_lut->num_valid_lut_rows = sar_pwr_rec->num_valid_lut_rows;
  sar_pwr_lut->num_valid_lut_column = 2;

  for (index_i = 0; index_i < sar_pwr_lut->num_valid_lut_rows; index_i++)
  {
    for (index_j = 0; index_j < sar_pwr_lut->num_valid_lut_column; index_j++)
    {
      sar_pwr_lut->lprio_limits[index_i][index_j] = sar_pwr_rec->lprio_limit_lut[index_i][index_j];
    }
  }


  /* Store the heap memory pointer in global db */
  hband = sar_pwr_rec->hprio_band;
  lband = sar_pwr_rec->lprio_band;

  /* Get the simult tech lut pointer */
  sar_techs_lut = sar_tables[hprio_tech][lprio_tech];

  if (sar_techs_lut == NULL || sar_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  /* Store the pointer */
  sar_lut = (sar_lut_type*)(sar_techs_lut->luts);
  index = hband * sar_techs_lut->num_tech2_bands + lband;
  if ((sar_lut + index)->sar_pwr_lut == NULL)
  {
    (sar_lut + index)->sar_pwr_lut = sar_pwr_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif
}

/*=============================================================================

  FUNCTION:  sar_validate_sar_pwr_gplusx_rec

=============================================================================*/
/*!
    @brief
    Validates a SAR PWR record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean sar_validate_sar_pwr_gplusx_rec 
(
  sar_efs_pwr_gplusx_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  uint8  num_valid_rows = 0;
  int i;

  if (sar_pwr_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band(hprio_tech, 
                                           sar_pwr_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(lprio_tech, 
                                          sar_pwr_rec->lprio_band) )
  {
    return FALSE;
  }

  /* High priority tech power backoff flag should be a boolean */
  if ((uint8)sar_pwr_rec->apply_hpriotech_limit > 1)
  {
    return FALSE;
  }  

  /* Number of valid LUT rows */
  num_valid_rows = sar_pwr_rec->num_valid_lut_rows;
  if ( (num_valid_rows < 1) || (num_valid_rows > 5))
  {
    return FALSE;
  }
  
  /* Validate LUT. As the Tx input power goes up, the Tx output power limit
     should go down. If this is not the case, then the entry is invalid. */
  for (i=0; i<num_valid_rows-1; i++)
  {
    if ( ( sar_pwr_rec->lprio_limit_lut[i][0] > 
                  sar_pwr_rec->lprio_limit_lut[i+1][0]) ||
         ( sar_pwr_rec->lprio_limit_lut[i][1] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][1]) )
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*=============================================================================

  FUNCTION:  sar_store_sar_pwr_gplusx_rec

=============================================================================*/
/*!
    @brief
    Stores a SAR PWR record in heap and pointer in the global space
 
    @note
    sar_efs_validate_sar_pwr_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void sar_store_sar_pwr_xplusx_rec
(
  sar_efs_pwr_xplusx_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  sar_tech_comb_lut_type* sar_techs_lut;
  sar_pwr_lut_type* sar_pwr_lut = NULL;
  uint32 hband, lband, index;
  sar_lut_type* sar_lut;
  uint8  index_i, index_j;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  sar_pwr_lut=(sar_pwr_lut_type*)modem_mem_alloc(sizeof(sar_pwr_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (sar_pwr_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */

  /* Is there a high prio tech backoff to be applied ? */
  if((boolean)(sar_pwr_rec->apply_hpriotech_limit) == FALSE)
  {
    sar_pwr_lut->num_valid_hprio_limits = 0;
  }
  else
  {
    /* Copy all the 8 limits */
    sar_pwr_lut->num_valid_hprio_limits = 1;
    sar_pwr_lut->hprio_limits[0] = sar_pwr_rec->hprio_limit;
  }

  /* Copy the LUT... */
  sar_pwr_lut->num_valid_lut_rows = sar_pwr_rec->num_valid_lut_rows;
  sar_pwr_lut->num_valid_lut_column = 2;

  for (index_i = 0; index_i < sar_pwr_lut->num_valid_lut_rows; index_i++)
  {
    for (index_j = 0; index_j < sar_pwr_lut->num_valid_lut_column; index_j++)
    {
      sar_pwr_lut->lprio_limits[index_i][index_j] = sar_pwr_rec->lprio_limit_lut[index_i][index_j];
    }
  }

  /* Store the heap memory pointer in global db */
  hband = sar_pwr_rec->hprio_band;
  lband = sar_pwr_rec->lprio_band;

  /* Get the simult tech lut pointer */
  sar_techs_lut = sar_tables[hprio_tech][lprio_tech];

  if (sar_techs_lut == NULL || sar_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  /* Store the pointer */
  sar_lut = (sar_lut_type*)(sar_techs_lut->luts);
  index = hband * sar_techs_lut->num_tech2_bands + lband;
  if ((sar_lut + index)->sar_pwr_lut == NULL)
  {
    (sar_lut + index)->sar_pwr_lut = sar_pwr_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

}

/*=============================================================================

  FUNCTION:  sar_validate_sar_pwr_xplusx_rec

=============================================================================*/
/*!
    @brief
    Validates a SAR PWR record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean sar_validate_sar_pwr_xplusx_rec 
(
  sar_efs_pwr_xplusx_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  uint8  num_valid_rows = 0;
  int i;

  if (sar_pwr_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band(hprio_tech, 
                                           sar_pwr_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(lprio_tech, 
                                          sar_pwr_rec->lprio_band) )
  {
    return FALSE;
  }

  /* High priority tech power backoff flag should be a boolean */
  if ((uint8)sar_pwr_rec->apply_hpriotech_limit > 1)
  {
    return FALSE;
  }  

  /* Number of valid LUT rows */
  num_valid_rows = sar_pwr_rec->num_valid_lut_rows;
  if ( (num_valid_rows < 1) || (num_valid_rows > 5))
  {
    return FALSE;
  }
  
  /* Validate LUT. As the Tx input power goes up, the Tx output power limit
     should go down. If this is not the case, then the entry is invalid. */
  for (i=0; i<num_valid_rows-1; i++)
  {
    if ( ( sar_pwr_rec->lprio_limit_lut[i][0] > 
                  sar_pwr_rec->lprio_limit_lut[i+1][0]) ||
         ( sar_pwr_rec->lprio_limit_lut[i][1] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][1]) )
    {
      return FALSE;
    }
  }

  return TRUE;
}


/*=============================================================================

  FUNCTION:  sar_store_sar_pwr_gplusg_rec

=============================================================================*/
/*!
    @brief
    Stores a SAR PWR record in heap and pointer in the global space
 
    @note
    sar_efs_validate_sar_pwr_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void sar_store_sar_pwr_gplusg_rec
(
  sar_efs_pwr_gplusg_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  sar_tech_comb_lut_type* sar_techs_lut;
  sar_pwr_lut_type* sar_pwr_lut = NULL;
  uint32 hband, lband, index;
  sar_lut_type* sar_lut;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  sar_pwr_lut=(sar_pwr_lut_type*)modem_mem_alloc(sizeof(sar_pwr_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (sar_pwr_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */

  /* Is there a high prio tech backoff to be applied ? */
  if((boolean)(sar_pwr_rec->apply_hpriotech_limit) == FALSE)
  {
    sar_pwr_lut->num_valid_hprio_limits = 0;
  }
  else
  {
    /* Copy all the 8 limits */
    sar_pwr_lut->num_valid_hprio_limits = SAR_GSM_MAX_LIMIT_TYPE;
    memscpy((void *)sar_pwr_lut->hprio_limits,
             sizeof(sar_pwr_lut->hprio_limits),
            (void *)sar_pwr_rec->hprio_limits,
             sizeof(sar_pwr_rec->hprio_limits));
  }

  /* Copy the LUT... */
  sar_pwr_lut->num_valid_lut_rows = sar_pwr_rec->num_valid_lut_rows;
  sar_pwr_lut->num_valid_lut_column = MAX_SAR_PWR_LUT_COLUMNS;
  memscpy( (void *)sar_pwr_lut->lprio_limits, 
           sizeof(sar_pwr_lut->lprio_limits),
           (void *)sar_pwr_rec->lprio_limit_lut,
           sizeof(sar_pwr_lut->lprio_limits) );

  LMTSMGR_MSG_1(HIGH,"sar_store_sar_pwr_gplusg_rec column  %d", 
			 sar_pwr_lut->num_valid_lut_column);

  /* Store the heap memory pointer in global db */
  hband = sar_pwr_rec->hprio_band;
  lband = sar_pwr_rec->lprio_band;

  /* Get the simult tech lut pointer */
  sar_techs_lut = sar_tables[hprio_tech][lprio_tech];

  if (sar_techs_lut == NULL || sar_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  /* Store the pointer */
  sar_lut = (sar_lut_type*)(sar_techs_lut->luts);
  index = hband * sar_techs_lut->num_tech2_bands + lband;
  if ((sar_lut + index)->sar_pwr_lut == NULL)
  {
    (sar_lut + index)->sar_pwr_lut = sar_pwr_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif
}

/*=============================================================================

  FUNCTION:  sar_validate_sar_pwr_gplusg_rec

=============================================================================*/
/*!
    @brief
    Validates a SAR PWR record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean sar_validate_sar_pwr_gplusg_rec 
(
  sar_efs_pwr_gplusg_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  uint8  num_valid_rows = 0;
  int i;

  if (sar_pwr_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band(hprio_tech, 
                                           sar_pwr_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(lprio_tech, 
                                          sar_pwr_rec->lprio_band) )
  {
    return FALSE;
  }

  /* High priority tech power backoff flag should be a boolean */
  if ((uint8)sar_pwr_rec->apply_hpriotech_limit > 1)
  {
    return FALSE;
  }  

  /* Number of valid LUT rows */
  num_valid_rows = sar_pwr_rec->num_valid_lut_rows;
  if ( (num_valid_rows < 1) || (num_valid_rows > 5))
  {
    return FALSE;
  }
  

  //@TODO: This need to be revisited as it may be double dimension array currently it is MAX_SAR_PWR_LUT_COLUMNS (3)
  
  /* Validate LUT. As the Tx input power goes up, the Tx output power limit
     should go down. If this is not the case, then the entry is invalid. */
  for (i=0; i<num_valid_rows-1; i++)
  {
    if ( ( sar_pwr_rec->lprio_limit_lut[i][0] > 
                  sar_pwr_rec->lprio_limit_lut[i+1][0]) ||
         ( sar_pwr_rec->lprio_limit_lut[i][1] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][1]) || 
         ( sar_pwr_rec->lprio_limit_lut[i][2] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][2]))
    {
      return FALSE;
    }
  }

  return TRUE;
}


/*=============================================================================

  FUNCTION:  sar_store_sar_pwr_xplusg_rec

=============================================================================*/
/*!
    @brief
    Stores a SAR PWR record in heap and pointer in the global space
 
    @note
    sar_efs_validate_sar_pwr_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void sar_store_sar_pwr_xplusg_rec
(
  sar_efs_pwr_xplusg_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  sar_tech_comb_lut_type* sar_techs_lut;
  sar_pwr_lut_type* sar_pwr_lut = NULL;
  uint32 hband, lband, index;
  sar_lut_type* sar_lut;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  sar_pwr_lut=(sar_pwr_lut_type*)modem_mem_alloc(sizeof(sar_pwr_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (sar_pwr_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */

  /* Is there a high prio tech backoff to be applied ? */
  if((boolean)(sar_pwr_rec->apply_hpriotech_limit) == FALSE)
  {
    sar_pwr_lut->num_valid_hprio_limits = 0;
  }
  else
  {
    /* Copy the hprio limit */
    sar_pwr_lut->num_valid_hprio_limits = 1;
    memscpy((void *)sar_pwr_lut->hprio_limits,
            sizeof(sar_pwr_lut->hprio_limits),
            (void *)&sar_pwr_rec->hprio_limit,
            sizeof(sar_pwr_rec->hprio_limit));
  }

  /* Copy the LUT... */
  sar_pwr_lut->num_valid_lut_rows = sar_pwr_rec->num_valid_lut_rows;
  sar_pwr_lut->num_valid_lut_column = MAX_SAR_PWR_LUT_COLUMNS;
  memscpy( (void *)sar_pwr_lut->lprio_limits, 
           sizeof(sar_pwr_lut->lprio_limits),
           (void *)sar_pwr_rec->lprio_limit_lut,
           sizeof(sar_pwr_lut->lprio_limits) );

  LMTSMGR_MSG_1(HIGH,"sar_store_sar_pwr_gplusg_rec column  %d", 
			 sar_pwr_lut->num_valid_lut_column);

  /* Store the heap memory pointer in global db */
  hband = sar_pwr_rec->hprio_band;
  lband = sar_pwr_rec->lprio_band;

  /* Get the simult tech lut pointer */
  sar_techs_lut = sar_tables[hprio_tech][lprio_tech];

  if (sar_techs_lut == NULL || sar_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  /* Store the pointer */
  sar_lut = (sar_lut_type*)(sar_techs_lut->luts);
  index = hband * sar_techs_lut->num_tech2_bands + lband;
  if ((sar_lut + index)->sar_pwr_lut == NULL)
  {
    (sar_lut + index)->sar_pwr_lut = sar_pwr_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(sar_pwr_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif
}

/*=============================================================================

  FUNCTION:  sar_validate_sar_pwr_xplusg_rec

=============================================================================*/
/*!
    @brief
    Validates a SAR PWR record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean sar_validate_sar_pwr_xplusg_rec 
(
  sar_efs_pwr_xplusg_rec_type*  sar_pwr_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  uint8  num_valid_rows = 0;
  int i;

  if (sar_pwr_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band(hprio_tech, 
                                           sar_pwr_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(lprio_tech, 
                                          sar_pwr_rec->lprio_band) )
  {
    return FALSE;
  }

  /* High priority tech power backoff flag should be a boolean */
  if ((uint8)sar_pwr_rec->apply_hpriotech_limit > 1)
  {
    return FALSE;
  }  

  /* Number of valid LUT rows */
  num_valid_rows = sar_pwr_rec->num_valid_lut_rows;
  if ( (num_valid_rows < 1) || (num_valid_rows > 5))
  {
    return FALSE;
  }
  

  //@TODO: This need to be revisited as it may be double dimension array currently it is MAX_SAR_PWR_LUT_COLUMNS (3)
  
  /* Validate LUT. As the Tx input power goes up, the Tx output power limit
     should go down. If this is not the case, then the entry is invalid. */
  for (i=0; i<num_valid_rows-1; i++)
  {
    if ( ( sar_pwr_rec->lprio_limit_lut[i][0] > 
                  sar_pwr_rec->lprio_limit_lut[i+1][0]) ||
         ( sar_pwr_rec->lprio_limit_lut[i][1] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][1]) || 
         ( sar_pwr_rec->lprio_limit_lut[i][2] < 
                  sar_pwr_rec->lprio_limit_lut[i+1][2]))
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*=============================================================================

  FUNCTION:  sar_read_sar_pwr_efs

=============================================================================*/
/*!
    @brief
    Reads all the sar SAR PWR EFS file  
 
    @return
    None
*/
/*===========================================================================*/
void sar_read_sar_pwr_efs 
( 
  const char *fname,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  int32      fd;
  sar_efs_pwr_gplusx_rec_type  sar_pwr_gplusx_rec;
  sar_efs_pwr_xplusx_rec_type  sar_pwr_xplusx_rec;
  sar_efs_pwr_gplusg_rec_type  sar_pwr_gplusg_rec;
  sar_efs_pwr_xplusg_rec_type  sar_pwr_xplusg_rec;
  int result;

  boolean rec_valid = FALSE;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_2(ERROR, "Error opening EFS file %d %d", fd, efs_errno);
    return;
  }

  /* Reading of EFS depends on the techs as the structures can be different...
     Need to identify between G+G, G+X, X+G, X+X cases 
     where X = any tech except G
   */
  if ( (hprio_tech == CXM_TECH_GSM1 || hprio_tech == CXM_TECH_GSM2) &&
       (lprio_tech == CXM_TECH_GSM1 || lprio_tech == CXM_TECH_GSM2) )
  {
    /* G+G case */

    while (1)
    {
      /* Read a record from EFS */
      result = efs_read (fd, (void *)&sar_pwr_gplusg_rec, 
                                  sizeof(sar_pwr_gplusg_rec));

      /* Check to see if the read failed. If it did, then deterime
       * why it failed. */
      if (result <= 0)
      {
        LMTSMGR_MSG_1(HIGH, "EFS error %d while reading SAR PWR record",efs_errno);
        break ;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (result != sizeof(sar_efs_pwr_gplusg_rec_type))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(sar_efs_pwr_gplusg_rec_type));
        break; 
      }
      
      /* Read was succesful. Now validate the data */
      rec_valid = sar_validate_sar_pwr_gplusg_rec(&sar_pwr_gplusg_rec, 
                                                  hprio_tech, lprio_tech);
      
      if (rec_valid == TRUE)
      {
        /* Store the record in heap and assign pointer in global lut list */
        sar_store_sar_pwr_gplusg_rec(&sar_pwr_gplusg_rec, 
                                       hprio_tech, lprio_tech);  
      }
    }
  }
  else if ( (hprio_tech == CXM_TECH_GSM1 || hprio_tech == CXM_TECH_GSM2) &&
            (lprio_tech != CXM_TECH_GSM1 && lprio_tech != CXM_TECH_GSM2) )
  {
    /* G+X case */

    while (1)
    {
      /* Read a record from EFS */
      result = efs_read (fd, (void *)&sar_pwr_gplusx_rec, 
                                  sizeof(sar_pwr_gplusx_rec));

      /* Check to see if the read failed. If it did, then determine
       * why it failed. */
      if (result <= 0)
      {
        LMTSMGR_DBG_MSG_1(HIGH, "EFS error %d while reading SAR PWR record", efs_errno);
        break;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (result != sizeof(sar_efs_pwr_gplusx_rec_type))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(sar_efs_pwr_gplusx_rec_type));
        break; 
      }
      
      /* Read was succesful. Now validate the data */
      rec_valid = sar_validate_sar_pwr_gplusx_rec(&sar_pwr_gplusx_rec, 
                                                  hprio_tech, lprio_tech);
      
      if (rec_valid == TRUE)
      {
        /* Store the record in heap and assign pointer in global lut list */
        sar_store_sar_pwr_gplusx_rec(&sar_pwr_gplusx_rec, 
                                       hprio_tech, lprio_tech);  
      }
    }
  }
  else if ( (hprio_tech != CXM_TECH_GSM1 && hprio_tech != CXM_TECH_GSM2) &&
            (lprio_tech == CXM_TECH_GSM1 || lprio_tech == CXM_TECH_GSM2))
  {
    /* X+G case */

    while (1)
    {
      /* Read a record from EFS */
      result = efs_read (fd, (void *)&sar_pwr_xplusg_rec, 
                                  sizeof(sar_pwr_xplusg_rec));

      /* Check to see if the read failed. If it did, then determine
       * why it failed. */
      if (result <= 0)
      {
        LMTSMGR_DBG_MSG_1(HIGH, "EFS error %d while reading SAR PWR record", efs_errno);
        break;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (result != sizeof(sar_efs_pwr_xplusg_rec_type))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(sar_efs_pwr_xplusg_rec_type));
        break; 
      }
      
      /* Read was succesful. Now validate the data */
      rec_valid = sar_validate_sar_pwr_xplusg_rec(&sar_pwr_xplusg_rec, 
                                                  hprio_tech, lprio_tech);
      
      if (rec_valid == TRUE)
      {
        /* Store the record in heap and assign pointer in global lut list */
        sar_store_sar_pwr_xplusg_rec(&sar_pwr_xplusg_rec, 
                                       hprio_tech, lprio_tech);  
      }
    }
  }
  else
  {
    /* X+X case */
    while (1)
    {
      /* Read a record from EFS */
      result = efs_read (fd, (void *)&sar_pwr_xplusx_rec, 
                                  sizeof(sar_pwr_xplusx_rec));

      /* Check to see if the read failed. If it did, then deterime
       * why it failed. */
      if (result <= 0)
      {
        LMTSMGR_MSG_1(HIGH, "EFS error %d while reading SAR PWR record",efs_errno);
        break ;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (result != sizeof(sar_efs_pwr_xplusx_rec_type))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(sar_efs_pwr_xplusx_rec_type));
        break; 
      }
      
      /* Read was succesful. Now validate the data */
      rec_valid = sar_validate_sar_pwr_xplusx_rec(&sar_pwr_xplusx_rec, 
                                                  hprio_tech, lprio_tech);
      
      if (rec_valid == TRUE)
      {
        /* Store the record in heap and assign pointer in global lut list */
        sar_store_sar_pwr_xplusx_rec(&sar_pwr_xplusx_rec, 
                                       hprio_tech, lprio_tech);  
      }
    }
    /* Do nothing for now */
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  sar_efs_store_dsi_bias_record

=============================================================================*/
/*!
    @brief
    Stores a SAR DSI BIAS record in heap and pointer in the global space
 
    @note
    sar_efs_validate_dsi_bias_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void sar_efs_store_dsi_bias_record
(
  sar_efs_dsi_record_type*  dsi_bias_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  sar_dsi_lut_type* dsi_bias_lut = NULL;
  sar_tech_comb_lut_type* sar_techs_lut;
  uint32 hband, lband, index;
  sar_lut_type* sar_dsi_bias_lut;
  

  /* Allocate memory in heap for the LUT */
  dsi_bias_lut=(sar_dsi_lut_type*)modem_mem_alloc(sizeof(sar_dsi_lut_type), 
                                                        MODEM_MEM_CLIENT_MCS);

  if (dsi_bias_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* The bias for DSI 0 will be 0 */
  dsi_bias_lut->bias[0][0] = 0;
  dsi_bias_lut->bias[0][1] = 0;

  /* Copy the EFS record onto heap */
  memscpy( (void *)(int16 *)&dsi_bias_lut->bias[1][0],
           2 * (MAX_DSI_LUT_ROWS - 1) * sizeof(int16),
           (void *)dsi_bias_rec->bias,
           sizeof(dsi_bias_rec->bias));

  hband = dsi_bias_rec->hprio_band;
  lband = dsi_bias_rec->lprio_band;

  /* Get the simult tech lut pointer */
  sar_techs_lut = sar_tables[hprio_tech][lprio_tech];

  if (sar_techs_lut == NULL || sar_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(dsi_bias_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  /* Store the pointer */
  sar_dsi_bias_lut = (sar_lut_type *)(sar_techs_lut->luts);
  index = hband * sar_techs_lut->num_tech2_bands + lband;
  if ((sar_dsi_bias_lut+index)->dsi_lut == NULL)
  {
    (sar_dsi_bias_lut+index)->dsi_lut = dsi_bias_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
              hprio_tech, lprio_tech);

    /* Free the pointer */
    modem_mem_free(dsi_bias_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }
  
}

/*=============================================================================

  FUNCTION:  sar_efs_validate_dsi_bias_record

=============================================================================*/
/*!
    @brief
    Validates a SAR DSI BIAS record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean sar_efs_validate_dsi_bias_record 
(
  sar_efs_dsi_record_type*  dsi_bias_rec,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  if (dsi_bias_rec == NULL)
  {
    return FALSE;
  }

  if ( FALSE == lmtsmgr_efs_validate_tech_band(hprio_tech, 
                                           dsi_bias_rec->hprio_band) )
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(lprio_tech, 
                                          dsi_bias_rec->lprio_band) )
  {
    return FALSE;
  }

  return TRUE;
}


/*=============================================================================

  FUNCTION:  sar_read_dsi_bias_efs

=============================================================================*/
/*!
    @brief
    Reads all the sar SAR DSI BIAS file  
 
    @return
    None
*/
/*===========================================================================*/
void sar_read_dsi_bias_efs 
( 
  const char *fname,
  cxm_tech_type hprio_tech,
  cxm_tech_type lprio_tech
)
{
  int32      fd;
  sar_efs_dsi_record_type  dsi_bias_rec;
  int result;

  boolean rec_valid = FALSE;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&dsi_bias_rec, sizeof(sar_efs_dsi_record_type));

    /* Check to see if the read failed. If it did, then determine
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_DBG_MSG_1(HIGH," EFS error %d while reading SAR PWR record", efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(sar_efs_dsi_record_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(sar_efs_dsi_record_type));
      break; 
    }
    
    /* Read was succesful. Now validate the data */
    rec_valid = sar_efs_validate_dsi_bias_record(&dsi_bias_rec, 
                                                 hprio_tech, lprio_tech);
    
    if (rec_valid == TRUE)
    {
      /* Store the record in heap and assign pointer in global lut list */
      sar_efs_store_dsi_bias_record(&dsi_bias_rec, hprio_tech, lprio_tech);
    }
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  sar_read_gpio_dsi_efs_data

=============================================================================*/
/*!
    @brief
    Reads data related to GPIO for SAR DSI
 
    @return
    GPIO DSI EFS data 
*/
/*===========================================================================*/
sar_gpio_dsi_efs_data sar_read_gpio_dsi_efs_data
( 
  const char *fname
)
{
  int32      fd;
  sar_gpio_dsi_efs_data_type  gpio_efs_data;
  sar_gpio_dsi_efs_data       gpio_dsi = {0, 0, FALSE, FALSE, FALSE};
  int result;

  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return gpio_dsi;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&gpio_efs_data, sizeof(sar_gpio_dsi_efs_data_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(HIGH,"EFS error %d while reading SAR GPIO data record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(sar_gpio_dsi_efs_data_type))
    {
      LMTSMGR_MSG_2(MED,"Could not read a complete record of size %d, result %d", 
                 sizeof(sar_gpio_dsi_efs_data_type), result);
      break; 
    }
    
    if ((gpio_efs_data.is_gpio_dsi_active == TRUE) && (gpio_efs_data.polling_time_ms > 0))
    {
      gpio_dsi.is_gpio_dsi_active = gpio_efs_data.is_gpio_dsi_active;
      gpio_dsi.is_gpio_active_low = gpio_efs_data.is_gpio_active_low;
      gpio_dsi.is_init_backoff = gpio_efs_data.is_init_backoff;
      gpio_dsi.gpio_num = gpio_efs_data.gpio_num;
      gpio_dsi.polling_time_ms = MAX(gpio_efs_data.polling_time_ms, SAR_GPIO_MIN_POLLING_TIME);
    }
    LMTSMGR_MSG_3(HIGH,"GPIO EFS : %d %d %d ",gpio_efs_data.is_gpio_dsi_active, gpio_efs_data.gpio_num, gpio_efs_data.polling_time_ms);
  }

  (void)efs_close(fd);

  return gpio_dsi;
}

/*=============================================================================

  FUNCTION:  sar_efs_set_simult_tech_luts_to_default

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void sar_efs_set_all_luts_to_default
(
  sar_tech_comb_lut_type* lut_list
)
{
  int i, j, index;
  sar_lut_type* sar_luts;
  uint32 hprio_bands;
  uint32 lprio_bands;

  if (lut_list == NULL || lut_list->luts == NULL)
  {
    return;
  }

  hprio_bands = lut_list->num_tech1_bands;
  lprio_bands = lut_list->num_tech2_bands;

  for(i = 0; i < hprio_bands; i++)
  {
    for(j = 0; j < lprio_bands; j++)
    {
      index = (i * lprio_bands + j) ;

      /* Get the pointer to the LUT */
      sar_luts = (sar_lut_type*)lut_list->luts + index;
      memscpy((void *)sar_luts, 
              sizeof(sar_lut_type),
              (void *)&sar_default_luts, 
              sizeof(sar_lut_type)); 
    }
  }
}

/*=============================================================================

  FUNCTION:  sar_efs_set_default_lut

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void sar_efs_set_default_lut (void)
{
  cxm_tech_type i, j;
  sar_tech_comb_lut_type* luts;

  for (i=0; i<CXM_TECH_MAX; i++)
  {
    if (sar_tables[i] != NULL)
    {
      for(j=0; j<CXM_TECH_MAX; j++)
      {
        /* Is this a valid scenario ? */
        if (sar_tables[i][j] != NULL)
        {
          luts = sar_tables[i][j];
          
          /* Set all the LUTs to default for this simult tech  */
          sar_efs_set_all_luts_to_default(luts);
        }
      }
    }
  }
}

/*=============================================================================

  FUNCTION:  sar_efs_init

=============================================================================*/
/*!
    @brief
    Initializes sar EFS
 
    @return
    None
*/
/*===========================================================================*/
void sar_efs_init (void)
{
  cxm_tech_type tech1, tech2;
  sar_tech_comb_lut_type* tbl_entry;
  int32 fd_comp_key;

  /* Set the LUTs to default */
  sar_efs_set_default_lut();

  /* Read the files one by one... */
  for (tech1=0; tech1<CXM_TECH_MAX; tech1++)
  {
    for(tech2=0; tech2<CXM_TECH_MAX; tech2++)
    {
      tbl_entry = sar_tables[tech1][tech2];

      /* If there is no entry, then continue to the next */
      if (tbl_entry == NULL)
      {
        continue;
      }

      /* Read the SAR PWR EFS file */
      if (tbl_entry->files.sar_pwr_fname != NULL)
      {
        sar_read_sar_pwr_efs(tbl_entry->files.sar_pwr_fname, tech1, tech2);
      }

      /* Read the DSI BIAS EFS file */
      if (tbl_entry->files.dsi_bias_fname != NULL)
      {
        sar_read_dsi_bias_efs(tbl_entry->files.dsi_bias_fname, tech1, tech2);
      }
    }
  }


  /*Read the compatibility key from the EFS*/
  /*The key is used for SAR DSI change requests over QMI from the client*/
  
     
	   /* Open the EFS file. If its not present then create it. */
  fd_comp_key = efs_open(LIMITSMGR_SAR_COMPATIBILITY_KEY, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd_comp_key < 0)
  {
    LMTSMGR_MSG_2(ERROR, "Error opening EFS file %d %d", fd_comp_key, efs_errno);
    return;
  }
  
  
  sar_qmi_key_valid = efs_read (fd_comp_key, (void *)&sar_qmi_comp_key, 
                                  sizeof(sar_qmi_comp_key));

      /* Check to see if the read failed. If it did, then deterime
       * why it failed. */
      if (sar_qmi_key_valid <= 0)
      {
        LMTSMGR_MSG_1(HIGH, "EFS error %d while reading SAR QMI Compatibility Key",efs_errno);
        return;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (sar_qmi_key_valid != sizeof(sar_qmi_comp_key))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(sar_qmi_comp_key));
        return; 
      }

}

/*=============================================================================

  FUNCTION:  sar_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the current SAR Tx limits.
 
    @return
    None
*/
/*===========================================================================*/
boolean sar_efs_lookup
(
  sar_efs_lookup_in* input,
  sar_efs_lookup_out* output
)
{
  sar_tech_comb_lut_type* tables;
  sar_lut_type* luts;
  sar_pwr_lut_type*      sar_pwr_lut;
  sar_dsi_lut_type*      dsi_lut;
  int16 lplimit[SAR_GSM_MAX_LIMIT_TYPE] = {DEFAULT_MAX_PLIMIT,DEFAULT_MAX_PLIMIT};
  int i, j;

  uint32 index = 0;
  int16 x1_bias, x2_bias;

  /* Check input/output ptr*/
  if (input == NULL || output == NULL)
  {
    return FALSE;
  }
  
  if ((input->hptech >= CXM_TECH_MAX) || (input->lptech >= CXM_TECH_MAX))
  {
    return FALSE;
  }

  /* Check if tables exist */
  if ( sar_tables[input->hptech] == NULL  || 
       sar_tables[input->hptech][input->lptech] == NULL )
  {
    return FALSE;
  }

  tables = sar_tables[input->hptech][input->lptech];

  /* check bands */
  if ( (input->hpband >= tables->num_tech1_bands) || 
       (input->lpband >= tables->num_tech2_bands) )
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect band %d %d", input->hpband, input->lpband);
    return FALSE;
  }
 
  /* check dsi */
  if (input->dsi >= MAX_DSI_LUT_ROWS )
  {
    LMTSMGR_MSG_1(ERROR,"Incorrect dsi %d", input->dsi);
    return FALSE;
  }

  /* Assume initially both limits wont be available */
  output->num_hplimits = 0;
  output->lplimit_valid = FALSE;

  /* Get the SAR PWR and DSI LUT */
  index = input->hpband * tables->num_tech2_bands + input->lpband;
  luts = (sar_lut_type *)(tables->luts);
  sar_pwr_lut = (luts + index)->sar_pwr_lut;
  dsi_lut = (luts + index)->dsi_lut;

  if (sar_pwr_lut == NULL)
  {
    /* There is no lookup table for this band combination */
    return FALSE;
  }


  /* Get the in/out bias */
  if (dsi_lut != NULL)
  {
    x1_bias = dsi_lut->bias[input->dsi][0];
    x2_bias = dsi_lut->bias[input->dsi][1];
  }
  else
  {
    /* If there is no bias lut defined, use 0 as the bias */
    x1_bias = 0;
    x2_bias = 0;
  }

  /* Go through the SAR PWR LUT to get the lplimit */

  /* Check if the first entry is sufficient */
  if ((sar_pwr_lut->num_valid_lut_rows == 0) ||
      (input->tx_power < (sar_pwr_lut->lprio_limits[0][0] + x1_bias)))
  {
    /* Apply max limit */
    lplimit[0] = DEFAULT_MAX_PLIMIT;
	
	/* Check if number of valid lut column are 3 */
    if (sar_pwr_lut->num_valid_lut_column == MAX_SAR_PWR_LUT_COLUMNS)
      lplimit[1] = DEFAULT_MAX_PLIMIT;
  }
  else
  {
    for (i=1; i < sar_pwr_lut->num_valid_lut_rows; i++)
    {
      if(input->tx_power < (sar_pwr_lut->lprio_limits[i][0] + x1_bias))
      {
        /* Set the limit as the previous entry */
        for (j=1; j < sar_pwr_lut->num_valid_lut_column; j++)
        {
          lplimit[j-1] = sar_pwr_lut->lprio_limits[i-1][j] + x2_bias;
        }
        break;
      }
    }

    if (i == sar_pwr_lut->num_valid_lut_rows)
    {
      /* If we reach here, that means the last power threshold has been crossed.
         So lets set the plimit to max */
      for (j=1; j < sar_pwr_lut->num_valid_lut_column; j++)
      {
        lplimit[j-1] = sar_pwr_lut->lprio_limits[i-1][j] + x2_bias;
      }
    }
  }

  memscpy (&output->lplimit,
           sizeof(output->lplimit),
           &lplimit, 
           sizeof (lplimit));

  output->lplimit_valid = TRUE;

  /* Check if number of valid lut column are 3 */
  if (sar_pwr_lut->num_valid_lut_column == MAX_SAR_PWR_LUT_COLUMNS)
  {
    output->num_lplimits = SAR_GSM_MAX_LIMIT_TYPE;
  }
  else
  {
    output->num_lplimits = 1;
  }
  
  return TRUE;
}

/*=============================================================================

  FUNCTION:  sar_efs_check_if_key_present

=============================================================================*/
/*!
    @brief
    Checks if the EFS in the modem has a valid compatibility key
 
    @return
    1 if key is present
    0 if key is not present
*/
/*===========================================================================*/
uint8 sar_efs_check_if_key_present
(
  void
)
{
   if(sar_qmi_key_valid >=1)
   {
     return 1;
   }
   else
   {
     return 0;
   }
}


/*=============================================================================

  FUNCTION:  sar_efs_get_comp_key

=============================================================================*/
/*!
    @brief
    Used to get the compatibility key stored in Modem's EFS
 
    @return
    The 32-bit compatibility key
*/
/*===========================================================================*/
uint32 sar_efs_get_comp_key
(
  void
)
{
   return sar_qmi_comp_key;
}

/*=============================================================================

  FUNCTION:  sar_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all the memory associated with LUTs
 
    @return
    None
*/
/*===========================================================================*/
void sar_efs_deinit( void )
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
}
