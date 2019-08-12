
/*!
  @file
  wwan_coex_efs.c

  @brief
  This file implements the EFS functions for WWAN COEX module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-16 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwan_coex_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/16   jm      Convert high-rate F3 messaging to off-target only
01/20/15   sg      Added partial support for WCDMA2 client
01/09/15   sg      EFS based spur simulation support
01/08/15   rj      Increase IM Blanking table length to 12 from 5
11/25/14   ag      Removed sys to rf band mapping from IM EFS lookup api
09/09/14   rj      Merged non-IM BackOff and Blanking EFS
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
07/17/14   sg      efs based coex simulation support
07/04/14   ag      data+mms support
04/28/14   jm      L+G Band Avoidance support
04/07/13   rj      Adding fix for L+G Blanking case, which was broken due to GPS support porting
03/15/13   jm      L+G Blanking Support
12/12/13   jm      GPS support for DDR coex
12/09/13   jm      DDR management phase 2 support (diversity power)
11/22/13   jm      Resolution of LLVM Warnings
11/21/13   jm      Adding DDR management support
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
04/06/13   ag      Support for DSDA WWAN Coexistence
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
#include "fs_errno.h"
#include "stringl.h"
#include "wwan_coex_mgr.h"
#include "wwcoex_action_iface.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_task.h"
#include "modem_mem.h"
#include "wwan_coex.h"

wwan_coex_efs_blank_fdd_victim_type  fatal_lut_rec;

extern uint32 wwcoex_simulation_input;

extern cxm_spur_info_s   wwcoex_sim_spur_info[WWCOEX_MAX_CONCURRENT_SPURS];

extern uint32            wwcoex_sim_spur_count;

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#define WWCOEX_IS_TDD_TECH(tech) ( ((tech)==CXM_TECH_GSM1) || ((tech)==CXM_TECH_GSM2) ||((tech)==CXM_TECH_TDSCDMA) || ((tech)==CXM_TECH_GSM3))
#define DEFAULT_DDR_FREQ 0xFF

#ifndef MCS_LIMITS_LOW_MEMORY

/* Coex LUT pointers for GSM(high priority) and LTE(low priority) */
wwan_coex_lut_type wwan_coex_gsm_lte_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_LTE_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm_lte_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm_lte_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_LTE_BANDS
};

/* Coex LUT pointers for GSM(high priority) and TDSCDMA(low priority) */
wwan_coex_lut_type wwan_coex_gsm_tdscdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_TDSCDMA_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm_tdscdma_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR,
    LIMITSMGR_COEX_SEM_THRESH_GSM_TDSCDMA,
  },
  (void *)&wwan_coex_gsm_tdscdma_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_TDSCDMA_BANDS
};

/*============================================================================
                          SVLTE/SVDO Coex tables
=============================================================================*/
/* SAR LUT pointers for 1X(high priority) and LTE(low priority) */
wwan_coex_lut_type wwan_coex_1x_lte_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_LTE_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_1x_lte_info = {
  {
    LIMITSMGR_COEX_BACKOFF_1X_VICTIM_LTE_AGGR,
    LIMITSMGR_COEX_DESENSE_GPS_VICTIM_1X_LTE_AGGR,
    LIMITSMGR_COEX_DESENSE_1X_VICTIM_LTE_AGGR,
    NULL,
  },
  (void *)&wwan_coex_1x_lte_luts,
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_LTE_BANDS
};

/* COEX LUT pointers for 1X(victim) and HDR(aggressor) */
wwan_coex_lut_type wwan_coex_1x_hdr_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_1X_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_1x_hdr_info = {
  {
    LIMITSMGR_COEX_BACKOFF_1X_VICTIM_HDR_AGGR,
    NULL,
    NULL,
    NULL,
  },
  (void *)&wwan_coex_1x_hdr_luts,
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_1X_BANDS
};

/* COEX LUT pointers for HDR(victim) and 1X(aggressor) */
wwan_coex_lut_type wwan_coex_hdr_1x_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_1X_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_hdr_1x_info = {
  {
    LIMITSMGR_COEX_BACKOFF_HDR_VICTIM_1X_AGGR,
    NULL,
    NULL,
    NULL,
  },
  (void *)&wwan_coex_hdr_1x_luts,
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_1X_BANDS
};

/* COEX LUT pointers for LTE(victim) and 1X(aggressor) */
wwan_coex_lut_type wwan_coex_lte_1x_luts[RFCOM_NUM_LTE_BANDS][RFCOM_NUM_1X_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_lte_1x_info = {
  {
    LIMITSMGR_COEX_BACKOFF_LTE_VICTIM_1X_AGGR,
    NULL,
    NULL,
    NULL,
  },
  (void *)&wwan_coex_lte_1x_luts,
  RFCOM_NUM_LTE_BANDS,
  RFCOM_NUM_1X_BANDS
};

/*============================================================================
                          DSDA Coex tables
=============================================================================*/
/* Coex LUT pointers for GSM(high priority) and 1x(low priority) */
wwan_coex_lut_type wwan_coex_gsm_1x_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_1X_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm_1x_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM_VICTIM_1X_AGGR,
    LIMITSMGR_COEX_SEM_THRESH_GSM_1X,
  },
  (void *)&wwan_coex_gsm_1x_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS
};

/* Coex LUT pointers for GSM(high priority) and HDR(low priority) */
wwan_coex_lut_type wwan_coex_gsm_hdr_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_1X_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm_hdr_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM_VICTIM_HDR_AGGR,
    LIMITSMGR_COEX_SEM_THRESH_GSM_HDR,
  },
  (void *)&wwan_coex_gsm_hdr_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_1X_BANDS
};

/* Coex LUT pointers for GSM(high priority) and WCDMA(low priority) */
wwan_coex_lut_type wwan_coex_gsm_wcdma_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_WCDMA_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm_wcdma_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM_VICTIM_WCDMA_AGGR,
    LIMITSMGR_COEX_SEM_THRESH_GSM_WCDMA,
  },
  (void *)&wwan_coex_gsm_wcdma_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_WCDMA_BANDS
};

/* Coex LUT pointers for 1X(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_1x_gsm_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_1x_gsm_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_1X_VICTIM_GSM_AGGR,
    NULL,
  },
  (void *)&wwan_coex_1x_gsm_luts,
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and HDR(low priority) */
wwan_coex_lut_type wwan_coex_hdr_gsm_luts[RFCOM_NUM_1X_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_hdr_gsm_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_HDR_VICTIM_GSM_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm_hdr_luts,
  RFCOM_NUM_1X_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and WCDMA(low priority) */
wwan_coex_lut_type wwan_coex_wcdma_gsm_luts[RFCOM_NUM_WCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_wcdma_gsm_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_WCDMA_VICTIM_GSM_AGGR,
    NULL,
  },
  (void *)&wwan_coex_wcdma_gsm_luts,
  RFCOM_NUM_WCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm2_gsm1_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm2_gsm1_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM1_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm2_gsm1_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm1_gsm2_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm1_gsm2_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM2_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm1_gsm2_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm3_gsm1_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm3_gsm1_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM1_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm3_gsm1_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm3_gsm2_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm3_gsm2_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM2_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm3_gsm2_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm1_gsm3_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm1_gsm3_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM3_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm1_gsm3_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and GSM(low priority) */
wwan_coex_lut_type wwan_coex_gsm2_gsm3_luts[RFCOM_NUM_GSM_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_gsm2_gsm3_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM3_AGGR,
    NULL,
  },
  (void *)&wwan_coex_gsm2_gsm3_luts,
  RFCOM_NUM_GSM_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and TDSCDMA(low priority) */
wwan_coex_lut_type wwan_coex_tdscdma_gsm_luts[RFCOM_NUM_TDSCDMA_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_tdscdma_gsm_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_TDSCDMA_VICTIM_GSM_AGGR,
    NULL,
  },
  (void *)&wwan_coex_tdscdma_gsm_luts,
  RFCOM_NUM_TDSCDMA_BANDS,
  RFCOM_NUM_GSM_BANDS
};

/* Coex LUT pointers for GSM(high priority) and LTE(low priority) */
wwan_coex_lut_type wwan_coex_lte_gsm_luts[RFCOM_NUM_LTE_BANDS][RFCOM_NUM_GSM_BANDS];
wwan_coex_tech_comb_lut_type wwan_coex_lte_gsm_info = {
  {
    NULL,
    NULL,
    LIMITSMGR_COEX_DESENSE_LTE_VICTIM_GSM_AGGR,
    NULL,
  },
  (void *)&wwan_coex_lte_gsm_luts,
  RFCOM_NUM_LTE_BANDS,
  RFCOM_NUM_GSM_BANDS
};

wwan_coex_tech_comb_lut_type* wwan_coex_tables[CXM_TECH_MAX][CXM_TECH_MAX] = {
/* LTE */  
  {NULL, NULL, &wwan_coex_lte_gsm_info, &wwan_coex_lte_1x_info, NULL, &wwan_coex_lte_gsm_info, &wwan_coex_lte_gsm_info},
/*TDSCDMA */
  {NULL, NULL, &wwan_coex_tdscdma_gsm_info, NULL, NULL, NULL, &wwan_coex_tdscdma_gsm_info, &wwan_coex_tdscdma_gsm_info},
/* GSM1 */
  {&wwan_coex_gsm_lte_info, &wwan_coex_gsm_tdscdma_info, NULL, &wwan_coex_gsm_1x_info,
    &wwan_coex_gsm_hdr_info, &wwan_coex_gsm_wcdma_info, &wwan_coex_gsm1_gsm2_info, &wwan_coex_gsm1_gsm3_info},
/* 1X */
  {&wwan_coex_1x_lte_info, NULL, &wwan_coex_1x_gsm_info, NULL, &wwan_coex_1x_hdr_info,
    NULL, &wwan_coex_1x_gsm_info, &wwan_coex_1x_gsm_info },
/* HDR */
  {NULL, NULL, &wwan_coex_hdr_gsm_info, &wwan_coex_hdr_1x_info, NULL, NULL, &wwan_coex_hdr_gsm_info,
     &wwan_coex_hdr_gsm_info },
/* WCDMA */
  {NULL, NULL, &wwan_coex_wcdma_gsm_info, NULL, NULL, NULL, &wwan_coex_wcdma_gsm_info, 
     &wwan_coex_wcdma_gsm_info },
/* GSM2 */
  {&wwan_coex_gsm_lte_info, &wwan_coex_gsm_tdscdma_info, &wwan_coex_gsm2_gsm1_info, 
   &wwan_coex_gsm_1x_info, &wwan_coex_gsm_hdr_info, &wwan_coex_gsm_wcdma_info, NULL, &wwan_coex_gsm2_gsm3_info},
/* GSM3 */
  {&wwan_coex_gsm_lte_info, &wwan_coex_gsm_tdscdma_info, &wwan_coex_gsm3_gsm1_info, 
   &wwan_coex_gsm_1x_info, &wwan_coex_gsm_hdr_info, &wwan_coex_gsm_wcdma_info, &wwan_coex_gsm3_gsm2_info, NULL},
/* WCDMA2 */
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

/* This Mask indicates if SW Coex mitigation is enabled or disabled. 
   Bit 1 - WWAN Coex Mitigation DISABLED
   Bit 2 - DDR Coex Mitigation DISABLED
*/
static uint32 wwcoex_mitigation_disable_mask = 0;

/* This Mask indicates if SW Coex mitigation is enabled or disabled. 
   Bit 3 - Band Avoidance Coex Mitigation ENABLED
*/
static uint32 wwcoex_mitigation_enable_mask = 0;

/* GSM Rx Power Limit to enable Idle Mode Hopping, 
     default value is set to -10000 dBm10
*/
static int16 wwcoex_idle_mode_hopping_rxpwr_lmt = LMTSMGR_GSM_RXPOWER_HOPPING_LIMIT;

#endif /* MCS_LIMITS_LOW_MEMORY */

#ifdef FEATURE_MODEM_DDR_MGMT
/* Coex LUT pointers for LTE tech */
/* LTE Primary Carrier */
wwcoex_ddr_desense_lut_type ddr_coex_lte_luts[RFCOM_NUM_LTE_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_lte_info = {
    LIMITSMGR_COEX_DDR_LTE_VICTIM_DDR_AGGR,
    DEFAULT_DDR_FREQ,
    (void *)&ddr_coex_lte_luts,
    RFCOM_NUM_LTE_BANDS,
    DDR_FREQ_DESENSE
};


/* LTE Secondary Carrier */
wwcoex_ddr_desense_lut_type ddr_coex_lte_ca_luts[RFCOM_NUM_LTE_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_lte_ca_info = {
    LIMITSMGR_COEX_DDR_LTE_SEC_CA_VICTIM_DDR_AGGR,
    DEFAULT_DDR_FREQ,
    (void *)&ddr_coex_lte_ca_luts,
    RFCOM_NUM_LTE_BANDS,
    DDR_FREQ_DESENSE
};

/* Coex LUT pointers for TDSCDMA tech */
wwcoex_ddr_desense_lut_type ddr_coex_tdscdma_luts[RFCOM_NUM_TDSCDMA_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_tdscdma_info = {
  LIMITSMGR_COEX_DDR_TDSCDMA_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_tdscdma_luts,
  RFCOM_NUM_TDSCDMA_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for GSM1 tech */
wwcoex_ddr_desense_lut_type ddr_coex_gsm1_luts[RFCOM_NUM_GSM_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_gsm1_info = {
  LIMITSMGR_COEX_DDR_GSM1_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_gsm1_luts,
  RFCOM_NUM_GSM_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for 1X tech */
wwcoex_ddr_desense_lut_type ddr_coex_1x_luts[RFCOM_NUM_1X_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_1x_info = {
  LIMITSMGR_COEX_DDR_1X_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_1x_luts,
  RFCOM_NUM_1X_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for HDR tech */
wwcoex_ddr_desense_lut_type ddr_coex_hdr_luts[RFCOM_NUM_1X_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_hdr_info = {
  LIMITSMGR_COEX_DDR_HDR_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_hdr_luts,
  RFCOM_NUM_1X_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for WCDMA tech */
wwcoex_ddr_desense_lut_type ddr_coex_wcdma_luts[RFCOM_NUM_WCDMA_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_wcdma_info = {
  LIMITSMGR_COEX_DDR_WCDMA_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_wcdma_luts,
  RFCOM_NUM_WCDMA_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for GSM2 tech */
wwcoex_ddr_desense_lut_type ddr_coex_gsm2_luts[RFCOM_NUM_GSM_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_gsm2_info = {
  LIMITSMGR_COEX_DDR_GSM2_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_gsm2_luts,
  RFCOM_NUM_GSM_BANDS,
  DDR_FREQ_DESENSE
};

/* Coex LUT pointers for GSM3 tech */
wwcoex_ddr_desense_lut_type ddr_coex_gsm3_luts[RFCOM_NUM_GSM_BANDS];
wwan_ddr_coex_tech_lut_type wwan_ddr_coex_gsm3_info = {
  LIMITSMGR_COEX_DDR_GSM3_VICTIM_DDR_AGGR,
  DEFAULT_DDR_FREQ,
  (void *)&ddr_coex_gsm3_luts,
  RFCOM_NUM_GSM_BANDS,
  DDR_FREQ_DESENSE
};

wwan_ddr_coex_tech_lut_type* wwan_ddr_coex_tables[CXM_TECH_MAX][DDR_LUT_ENTRY_MAX] = {
/* LTE */  
  { 
    &wwan_ddr_coex_lte_info,

    &wwan_ddr_coex_lte_ca_info
  },
/*TDSCDMA */
  {
    &wwan_ddr_coex_tdscdma_info,

    NULL
  },
/* GSM1 */
  {
    &wwan_ddr_coex_gsm1_info,

    NULL
  },
/* 1X */
  {
    &wwan_ddr_coex_1x_info,

    NULL
  },
/* HDR */
  {
    &wwan_ddr_coex_hdr_info,

    NULL
  },
/* WCDMA */
  {
    &wwan_ddr_coex_wcdma_info,

    NULL
  },
/* GSM2 */
  {
    &wwan_ddr_coex_gsm2_info,

    NULL
  },
/* GSM3 */
  {
    &wwan_ddr_coex_gsm3_info,

    NULL
  },
/* WCDMA2 */
  {
    NULL,

    NULL
  }

};

wwcoex_ddr_desense_gps_record_type wwan_ddr_coex_gps_record;

#endif /* DDR */

/* Default LUT state */                              
wwan_coex_lut_type wwan_coex_default_luts = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*=============================================================================

                                FUNCTIONS

=============================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwan_coex_im_efs_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN COEX PWR record in heap and pointer in the global space
 
    @note
    wwan_coex_efs_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_im_efs_store_record
(
  wwan_coex_efs_record_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  wwan_coex_desense_lut_type* desense_lut = NULL;
  wwan_coex_tech_comb_lut_type* wwan_coex_techs_lut;
  uint32 victim_band, aggr_band, index;
  wwan_coex_lut_type* wwan_coex_desense_lut;

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  desense_lut=(wwan_coex_desense_lut_type*)modem_mem_alloc(sizeof(wwan_coex_desense_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (desense_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */
  desense_lut->num_valid_lut_rows = desense_rec->num_valid_lut_rows;
  memscpy( (void *)desense_lut->aggressor_limits, 
           sizeof(desense_lut->aggressor_limits),
           (void *)desense_rec->aggressor_limit_lut,
           sizeof(desense_lut->aggressor_limits));

  victim_band = desense_rec->victim_band;
  aggr_band = desense_rec->aggressor_band;

  /* Get the simult tech lut pointer */
  wwan_coex_techs_lut = wwan_coex_tables[victim_tech][aggr_tech];
  if (wwan_coex_techs_lut == NULL || wwan_coex_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              victim_tech, aggr_tech);

    /* Free the pointer */
    modem_mem_free(desense_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  wwan_coex_desense_lut = (wwan_coex_lut_type*)(wwan_coex_techs_lut->luts);
  index = victim_band * wwan_coex_techs_lut->num_tech2_bands + aggr_band;
  
  /* Check if victim is high priority */
  if (desense_rec->victim_priority > desense_rec->aggressor_priority)
  {
    /* Store the pointer */
    if ( (wwan_coex_desense_lut+index)->im_desense_hp_victim_lut == NULL)
    {
      (wwan_coex_desense_lut+index)->im_desense_hp_victim_lut = desense_lut;
    }
    else
    {
      LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
                victim_tech, aggr_tech);

      /* Free the pointer */
      modem_mem_free(desense_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }
  else
  {
    /* Store the pointer */
    if ((wwan_coex_desense_lut+index)->im_desense_lp_victim_lut == NULL)
    {
      (wwan_coex_desense_lut+index)->im_desense_lp_victim_lut = desense_lut;
    }
    else
    {
      LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
                victim_tech, aggr_tech);

      /* Free the pointer */
      modem_mem_free(desense_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

}

/*=============================================================================

  FUNCTION:  wwan_coex_gps_im_efs_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN GPS COEX PWR record in heap and pointer in the global space
 
    @note
    wwan_coex_efs_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_gps_im_efs_store_record
(
  wwan_coex_efs_gps_record_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  wwan_coex_desense_lut_type* desense_lut = NULL;
  wwan_coex_tech_comb_lut_type* wwan_coex_techs_lut;
  uint32 victim_band, aggr_band, index;
  wwan_coex_lut_type* wwan_coex_desense_lut;

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Allocate memory in heap for the LUT */
  desense_lut=(wwan_coex_desense_lut_type*)modem_mem_alloc(sizeof(wwan_coex_desense_lut_type),
                                                     MODEM_MEM_CLIENT_MCS);

  if (desense_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */
  desense_lut->num_valid_lut_rows = desense_rec->num_valid_lut_rows;
  memscpy( (void *)desense_lut->aggressor_limits, 
           sizeof(desense_lut->aggressor_limits),
           (void *)desense_rec->aggressor_limit_lut,
           sizeof(desense_lut->aggressor_limits));

  victim_band = desense_rec->hptech_aggr_band;
  aggr_band = desense_rec->lptech_aggr_band;

  /* Get the simult tech lut pointer */
  wwan_coex_techs_lut = wwan_coex_tables[victim_tech][aggr_tech];
  if (wwan_coex_techs_lut == NULL || wwan_coex_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              victim_tech, aggr_tech);

    /* Free the pointer */
    modem_mem_free(desense_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  wwan_coex_desense_lut = (wwan_coex_lut_type*)(wwan_coex_techs_lut->luts);
  index = victim_band * wwan_coex_techs_lut->num_tech2_bands + aggr_band;
  
  /* Check if victim is high priority */
  /* Store the pointer */
  if ((wwan_coex_desense_lut+index)->gps_im_desense_lut == NULL)
  {
    (wwan_coex_desense_lut+index)->gps_im_desense_lut = desense_lut;
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
                  victim_tech, aggr_tech);

    /* Free the pointer */
    modem_mem_free(desense_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }
  
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  
}

/*=============================================================================

  FUNCTION:  wwan_coex_im_efs_validate_record

=============================================================================*/
/*!
    @brief
    Validates a DESENSE record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean wwan_coex_im_efs_validate_record 
(
  void*  efs_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech,
  wwan_coex_desense_type desense_type
)
{
  uint8  num_valid_rows = 0;
  int i;
  wwan_coex_efs_record_type  *desense_rec;
  wwan_coex_efs_gps_record_type *gps_desense_rec;

  if (efs_rec == NULL)
  {
    return FALSE;
  }

  if (desense_type == IM_DESENSE_GPS)
  {
    gps_desense_rec = (wwan_coex_efs_gps_record_type *)efs_rec;

    if (FALSE == lmtsmgr_efs_validate_tech_band(victim_tech,
                                                gps_desense_rec->hptech_aggr_band))
    {
      return FALSE;
    }

    if (FALSE == lmtsmgr_efs_validate_tech_band(aggr_tech,
                                                gps_desense_rec->lptech_aggr_band))
    {
      return FALSE;
    }

    /* Number of valid LUT rows */
    num_valid_rows = gps_desense_rec->num_valid_lut_rows;
    if ((num_valid_rows < 1) || (num_valid_rows > 5))
    {
      return FALSE;
    }

    /* Validate LUT-
       For IM_DESENSE: As the transmit power goes up, the Tx power limit should
       go down. If this condition is not met, then the LUT is invalid 
     */
    for (i = 0; i < num_valid_rows - 1; i++)
    {
      if ((gps_desense_rec->aggressor_limit_lut[i][0] >
                    gps_desense_rec->aggressor_limit_lut[i+1][0]) ||
           ( gps_desense_rec->aggressor_limit_lut[i][1] < 
                    gps_desense_rec->aggressor_limit_lut[i+1][1]) )
  {
    return FALSE;
  }
    }
  }
  else /* IM is not with GPS */
  {
    desense_rec = (wwan_coex_efs_record_type *)efs_rec;

  if (FALSE == lmtsmgr_efs_validate_tech_band(victim_tech,
                                          desense_rec->victim_band))
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(aggr_tech,
                                          desense_rec->aggressor_band))
  {
    return FALSE;
  }

  /* Check priorities */
  if (desense_rec->victim_priority == desense_rec->aggressor_priority)
  {
    return FALSE;
  }

  /* Number of valid LUT rows */
  num_valid_rows = desense_rec->num_valid_lut_rows;
  if ((num_valid_rows < 1) || (num_valid_rows > 5))
  {
    return FALSE;
  }

  /* Validate LUT-
     For IM_TYPE1_DESENSE: As the transmit power goes up, the Tx power limit should
     go down. If this condition is not met, then the LUT is invalid 
   */
    for (i = 0; i < num_valid_rows - 1; i++)
    {
      if ((desense_rec->aggressor_limit_lut[i][0] >
                    desense_rec->aggressor_limit_lut[i+1][0]) ||
           ( desense_rec->aggressor_limit_lut[i][1] < 
                    desense_rec->aggressor_limit_lut[i+1][1]) )
      {
        return FALSE;
      }
    }

  }

  
  return TRUE;
}


/*=============================================================================

  FUNCTION:  wwan_coex_read_backoff_efs

=============================================================================*/
/*!
    @brief
    Reads all records from the WWAN COEX EFS file
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_backoff_efs
( 
  const char *fname,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech,
  wwan_coex_desense_type desense_type
)
{
  int32      fd;
  wwan_coex_efs_record_type  desense_rec;
  wwan_coex_efs_gps_record_type gps_desense_rec;
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
    if (desense_type == IM_DESENSE_GPS)
    {
      /* Read a record from EFS */
      result = efs_read (fd, (void *)&gps_desense_rec, sizeof(wwan_coex_efs_gps_record_type));

      /* Check to see if the read failed. If it did, then deterime
       * why it failed. */
      if (result <= 0)
      {
        LMTSMGR_MSG_1(ERROR,"EFS error %d while reading DESENSE record",efs_errno);
        break ;
      }

      /* If read passed, check if we read the expected number of bytes */
      if (result != sizeof(wwan_coex_efs_gps_record_type))
      {
        LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                   sizeof(wwan_coex_efs_gps_record_type));
        break; 
      }
      
      /* Read was succesful. Now validate the data */
      rec_valid = wwan_coex_im_efs_validate_record(&gps_desense_rec, 
                                                   victim_tech, aggr_tech,
                                                   desense_type);
      
      if (rec_valid == TRUE)
      {
        /* Store the record in heap and assign pointer in global lut list */
        wwan_coex_gps_im_efs_store_record(&gps_desense_rec, victim_tech, aggr_tech);  
      }
    }
    else
    {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&desense_rec, sizeof(wwan_coex_efs_record_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading DESENSE record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_record_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_efs_record_type));
      break; 
    }
    
    /* Read was succesful. Now validate the data */
    rec_valid = wwan_coex_im_efs_validate_record(&desense_rec, 
                                                   victim_tech, aggr_tech,
                                                   desense_type);
    
    if (rec_valid == TRUE)
    {
      /* Store the record in heap and assign pointer in global lut list */
      wwan_coex_im_efs_store_record(&desense_rec, victim_tech, aggr_tech);  
    }
  }

  }

  (void)efs_close(fd);
}


/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_store_nonim_record

=============================================================================*/
/*!
    @brief
    Stores a Non-IM Record
 
    @note
    wwan_coex_blank_efs_<abc>_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_blank_efs_store_nonim_record
(
  wwan_coex_efs_nonim_lut_type*  desense_rec,
  wwan_coex_lut_type* wwan_coex_desense_lut,
  uint32 desense_mask
)
{
  uint32 rxbn_bit, harm_bit, block_bit;
  wwan_coex_desense_lut_type*    rxbn_lut = NULL;
  wwan_coex_desense_lut_type*    block_lut = NULL;
  wwan_coex_desense_lut_type*    harm_lut = NULL;
  int i;

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Check if non-IM bits are set in the mask... */
  rxbn_bit = desense_mask & (1<<NOISE_DESENSE);
  harm_bit = desense_mask & (1<<HARMONIC_DESENSE);
  block_bit = desense_mask & (1<<BLOCKING_DESENSE);

  if ( rxbn_bit )
  {
    /* Allocate memory in heap for the LUT */
    rxbn_lut =(wwan_coex_desense_lut_type*)modem_mem_alloc(sizeof(wwan_coex_desense_lut_type),
                                                            MODEM_MEM_CLIENT_MCS);

    if (rxbn_lut == NULL)
    {
      LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
      return ;
    }

    /* Copy the first two columns of the EFS record onto heap */
    rxbn_lut->num_valid_lut_rows = desense_rec->num_valid_rows;
    for (i=0; i < desense_rec->num_valid_rows; i++)
    {
      rxbn_lut->aggressor_limits[i][0] = desense_rec->blanking_threshs[i][0];
      rxbn_lut->aggressor_limits[i][1] = desense_rec->blanking_threshs[i][1];
    }
    
    /* Store the pointer */
    if ( wwan_coex_desense_lut->rxbn_lut == NULL)
    {
      wwan_coex_desense_lut->rxbn_lut = rxbn_lut;

#ifdef TEST_FRAMEWORK
      #error code not present
#endif
    }
    else
    {
      LMTSMGR_MSG_0(ERROR, "LUT already defined for this tech combination");

      /* Free the pointer */
      modem_mem_free(rxbn_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }

  if ( harm_bit )
  {
    /* Allocate memory in heap for the LUT */
    harm_lut =(wwan_coex_desense_lut_type*)modem_mem_alloc(sizeof(wwan_coex_desense_lut_type),
                                                            MODEM_MEM_CLIENT_MCS);

    if (harm_lut == NULL)
    {
      LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
      return ;
    }

    /* Copy the first  and third columns of the EFS record onto heap */
    harm_lut->num_valid_lut_rows = desense_rec->num_valid_rows;
    for (i=0; i < desense_rec->num_valid_rows; i++)
    {
      harm_lut->aggressor_limits[i][0] = desense_rec->blanking_threshs[i][0];
      harm_lut->aggressor_limits[i][1] = desense_rec->blanking_threshs[i][2];
    }
    
    /* Store the pointer */
    if ( wwan_coex_desense_lut->harmonics_lut == NULL)
    {
      wwan_coex_desense_lut->harmonics_lut = harm_lut;

#ifdef TEST_FRAMEWORK
      #error code not present
#endif
    }
    else
    {
      LMTSMGR_MSG_0(ERROR, "LUT already defined for this tech combination");

      /* Free the pointer */
      modem_mem_free(harm_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }

  if ( block_bit )
  {
    /* Allocate memory in heap for the LUT */
    block_lut =(wwan_coex_desense_lut_type*)modem_mem_alloc(sizeof(wwan_coex_desense_lut_type),
                                                            MODEM_MEM_CLIENT_MCS);

    if (block_lut == NULL)
    {
      LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
      return ;
    }

    /* Copy the first and fourth columns of the EFS record onto heap */
    block_lut->num_valid_lut_rows = desense_rec->num_valid_rows;
    for (i=0; i < desense_rec->num_valid_rows; i++)
    {
      block_lut->aggressor_limits[i][0] = desense_rec->blanking_threshs[i][0];
      block_lut->aggressor_limits[i][1] = desense_rec->blanking_threshs[i][3];
    }
    
    /* Store the pointer */
    if ( wwan_coex_desense_lut->blocking_lut == NULL)
    {
      wwan_coex_desense_lut->blocking_lut = block_lut;

#ifdef TEST_FRAMEWORK
      #error code not present
#endif
    }
    else
    {
      LMTSMGR_MSG_0(ERROR, "LUT already defined for this tech combination");

      /* Free the pointer */
      modem_mem_free(block_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }
}

/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_fdd_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN Blank EFS PWR record in heap and pointer in the global space
    for the TDD victim case
 
    @note
    wwan_coex_blank_efs_tdd_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_blank_efs_fdd_store_record
(
  wwan_coex_efs_blank_fdd_victim_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech,
  uint32  desense_mask
)
{
  wwan_coex_tech_comb_lut_type* wwan_coex_techs_lut;
  uint32 victim_band, aggr_band, index;
  wwan_coex_lut_type* wwan_coex_desense_lut;
  wwcoex_im_lut_type* im_desense_lut;

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Check if any entries are valid in the LUT */
  if (desense_mask == 0)
  {
    return;
  }

  /* Get the victim and aggressor bands */
  victim_band = desense_rec->victim_band;
  aggr_band = desense_rec->aggressor_band;

  /* Get the simult tech lut pointer */
  wwan_coex_techs_lut = wwan_coex_tables[victim_tech][aggr_tech];
  if (wwan_coex_techs_lut == NULL || wwan_coex_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              victim_tech, aggr_tech);

    return;
  }

  /* Get the pointer to the lut list for the specific band combination */
  wwan_coex_desense_lut = (wwan_coex_lut_type*)(wwan_coex_techs_lut->luts);
  index = victim_band * wwan_coex_techs_lut->num_tech2_bands + aggr_band;

  /* Check if IM bits are set in the mask... */
  if (desense_mask & ((1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE)))
  {
    /* Allocate memory in heap for the LUT */
    im_desense_lut=(wwcoex_im_lut_type*)modem_mem_alloc(sizeof(wwcoex_im_lut_type),
                                                            MODEM_MEM_CLIENT_MCS);

    if (im_desense_lut == NULL)
    {
      LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
      return ;
    }

    /* Copy the EFS record onto heap */
    im_desense_lut->num_valid_rows = desense_rec->im_lut.num_valid_rows;
    memscpy( (void *)im_desense_lut->tbl, 
           sizeof(im_desense_lut->tbl),
           (void *)desense_rec->im_lut.blanking_threshs,
           sizeof(desense_rec->im_lut.blanking_threshs));

    /* Store the pointer */
    if ( (wwan_coex_desense_lut+index)->im_lut == NULL)
    {
      (wwan_coex_desense_lut+index)->im_lut = im_desense_lut;

#ifdef TEST_FRAMEWORK
      #error code not present
#endif
    }
    else
    {
      LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
                    victim_tech, aggr_tech);

      /* Free the pointer */
      modem_mem_free(im_desense_lut, MODEM_MEM_CLIENT_MCS);

      return;
    }
  }

  /* Store the non-im record */
  wwan_coex_blank_efs_store_nonim_record( &desense_rec->nonim_lut,
                                          (wwan_coex_desense_lut+index),
                                          desense_mask );

  return;
}

/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_sem_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN Blank EFS PWR record in heap and pointer in the global space
    for SEM
 
    @note
    wwan_coex_blank_efs_fdd_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_blank_efs_sem_store_record
(
  wwan_coex_efs_sem_record_type*  sem_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  wwan_coex_desense_lut_type* sem_lut = NULL;
  wwan_coex_tech_comb_lut_type* wwan_coex_techs_lut;
  uint32 victim_band, aggr_band, index;
  wwan_coex_lut_type* wwan_coex_desense_lut;
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* Get the victim and aggressor bands */
  victim_band = sem_rec->victim_band;
  aggr_band = sem_rec->aggressor_band;

  /* Get the simult tech lut pointer */
  wwan_coex_techs_lut = wwan_coex_tables[victim_tech][aggr_tech];
  if (wwan_coex_techs_lut == NULL || wwan_coex_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              victim_tech, aggr_tech);

    return;
  }

  /* Get the pointer to the lut list for the specific band combination */
  wwan_coex_desense_lut = (wwan_coex_lut_type*)(wwan_coex_techs_lut->luts);
  index = victim_band * wwan_coex_techs_lut->num_tech2_bands + aggr_band;

  /* Allocate memory in heap for the LUT */
  sem_lut=(wwan_coex_desense_lut_type*)modem_mem_alloc(
                                          sizeof(wwan_coex_desense_lut_type),
                                          MODEM_MEM_CLIENT_MCS);

  if (sem_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Modem mem out of heap");
    return ;
  }

  /* Copy the EFS record onto heap */
  sem_lut->num_valid_lut_rows = sem_rec->sem_lut.num_valid_rows;
  memscpy( (void *)sem_lut->aggressor_limits, 
           sizeof(sem_lut->aggressor_limits),
           (void *)sem_rec->sem_lut.blanking_threshs,
           sizeof(sem_rec->sem_lut.blanking_threshs));

  /* Store the pointer */
  if ( (wwan_coex_desense_lut+index)->sem_lut == NULL)
  {
    (wwan_coex_desense_lut+index)->sem_lut = sem_lut;

#ifdef TEST_FRAMEWORK
    #error code not present
#endif
  }
  else
  {
    LMTSMGR_MSG_2(ERROR, "LUT already defined for this tech combination (%d,%d)",
                  victim_tech, aggr_tech);

    /* Free the pointer */
    modem_mem_free(sem_lut, MODEM_MEM_CLIENT_MCS);

    return;
  }

  return;
}
/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_tdd_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN Blank EFS PWR record in heap and pointer in the global space
    for the FDD victim case
 
    @note
    wwan_coex_blank_efs_fdd_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_blank_efs_tdd_store_record
(
  wwan_coex_efs_blank_tdd_victim_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech,
  uint32  desense_mask
)
{
  wwan_coex_tech_comb_lut_type* wwan_coex_techs_lut;
  uint32 victim_band, aggr_band, index;
  wwan_coex_lut_type* wwan_coex_desense_lut;

  /* Check if any entries are valid in the LUT */
  if (desense_mask == 0)
  {
    return;
  }

  /* Get the victim and aggressor bands */
  victim_band = desense_rec->victim_band;
  aggr_band = desense_rec->aggressor_band;

  /* Get the simult tech lut pointer */
  wwan_coex_techs_lut = wwan_coex_tables[victim_tech][aggr_tech];
  if (wwan_coex_techs_lut == NULL || wwan_coex_techs_lut->luts == NULL)
  {
    LMTSMGR_MSG_2(ERROR, "LUT not defined for this tech combination (%d,%d)",
              victim_tech, aggr_tech);

    return;
  }

  /* Get the pointer to the lut list for the specific band combination */
  wwan_coex_desense_lut = (wwan_coex_lut_type*)(wwan_coex_techs_lut->luts);
  index = victim_band * wwan_coex_techs_lut->num_tech2_bands + aggr_band;

  /* Store the non-im record */
  wwan_coex_blank_efs_store_nonim_record( &desense_rec->nonim_lut,
                                          (wwan_coex_desense_lut+index),
                                          desense_mask );
  return;
}

/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_validate_nonim_lut

=============================================================================*/
/*!
    @brief
    Validates a non-IM LUT 
 
    @return
    None
*/
/*===========================================================================*/
static boolean wwan_coex_blank_efs_validate_nonim_lut
(
  wwan_coex_efs_nonim_lut_type*  desense_rec,
  uint32* desense_valid_mask
)
{
  uint32 i;
  boolean retval = TRUE;

  if ( (desense_rec->num_valid_rows > 0) || 
         (desense_rec->num_valid_rows <= MAX_DESENSE_LUT_ROWS) )
  {
    for (i = 0; i < (desense_rec->num_valid_rows - 1); i++)
    {
      if (desense_rec->blanking_threshs[i] == NULL)
      {
        LMTSMGR_MSG_1(FATAL,"Unexpected: LUT is null for i=%d",i);

       (*desense_valid_mask) &= ~((1<<BLOCKING_DESENSE) | (1<<HARMONIC_DESENSE) | 
                                  (1<<NOISE_DESENSE));
        retval = FALSE;
        break;
      }
      if ( desense_rec->blanking_threshs[i][0] < 
                      desense_rec->blanking_threshs[i+1][0] )
      {
        (*desense_valid_mask) &= ~((1<<BLOCKING_DESENSE) | (1<<HARMONIC_DESENSE) | 
                                   (1<<NOISE_DESENSE));
        break;
      }

      if (desense_rec->blanking_threshs[i][1] <= 
                        desense_rec->blanking_threshs[i+1][1] )
      {
        (*desense_valid_mask) &= ~(1<<NOISE_DESENSE);
      }

      if ( desense_rec->blanking_threshs[i][2] <= 
                        desense_rec->blanking_threshs[i+1][2] )
      {
        (*desense_valid_mask) &= ~(1<<HARMONIC_DESENSE);
      }

      if ( desense_rec->blanking_threshs[i][3] <= 
                        desense_rec->blanking_threshs[i+1][3] )
      {
        (*desense_valid_mask) &= ~(1<<BLOCKING_DESENSE);
      }
    }
  }
  else
  {
    (*desense_valid_mask) &= ~((1<<BLOCKING_DESENSE) | (1<<HARMONIC_DESENSE) | 
                               (1<<NOISE_DESENSE));

  }

  return retval;

}
/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_fdd_validate_record

=============================================================================*/
/*!
    @brief
    Validates a DESENSE record 
 
    @return
    None
*/
/*===========================================================================*/
static uint32 wwan_coex_blank_efs_fdd_validate_record 
(
  wwan_coex_efs_blank_fdd_victim_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  uint8  num_im_valid_rows = 0;
  uint32 desense_valid_mask = 0x3E; /* All types of desense columns are valid */
  int i;
  boolean retval = TRUE;

  if (desense_rec == NULL)
  {
    return 0;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(victim_tech,
                                          desense_rec->victim_band))
  {
    return 0;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(aggr_tech,
                                          desense_rec->aggressor_band))
  {
    return 0;
  }


  /* Validate LUTs-
     Atleast one of the columns in the non-IM LUT or IM LUT should be in the 
     correct order. 
     Any receive power threshold column should be in descending order
     Any transmit power threshold column should also be in descending order
   
   5,210,220,-640,-630,195,190,-690,-680,170,170,-740,-730,140,145,-770,-765,120,115,-800,-810
  */
  /* Checking IM LUT first */
  num_im_valid_rows = desense_rec->im_lut.num_valid_rows;
  if ( (num_im_valid_rows > 0) && (num_im_valid_rows <= MAX_IM_DESENSE_LUT_ROWS) )
  {
    for (i = 0; i < num_im_valid_rows - 1; i++)
    {
      /* Validate first victim Tx power */
      if (desense_rec->im_lut.blanking_threshs[i][1] < 
                   desense_rec->im_lut.blanking_threshs[i+1][1])
      {
        desense_valid_mask &= ~((1<<IM_TYPE1_DESENSE)| (1<<IM_TYPE2_DESENSE));
        break;
      }
     
      /* If the Tx power is the same, validate other columns */
      if (desense_rec->im_lut.blanking_threshs[i][1] == 
                   desense_rec->im_lut.blanking_threshs[i+1][1])
      {
        if ( (desense_rec->im_lut.blanking_threshs[i][0] <
                   desense_rec->im_lut.blanking_threshs[i+1][0]) ||
             (desense_rec->im_lut.blanking_threshs[i][2] < 
                   desense_rec->im_lut.blanking_threshs[i+1][2] ) ||
             (desense_rec->im_lut.blanking_threshs[i][3] < 
                    desense_rec->im_lut.blanking_threshs[i+1][3]))
        {
          desense_valid_mask &= ~((1<<IM_TYPE1_DESENSE)| (1<<IM_TYPE2_DESENSE));
          break;
        }
      }
      else
      {
        /* [i] > [i+1] */
        continue;
      }
    }
  }
  else
  {
    desense_valid_mask &= ~((1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE));
  }

  LMTSMGR_MSG_1(HIGH, "Debug: Validating Non-IM with blanking threshs 0x%x", 
                desense_rec->nonim_lut.blanking_threshs);

  /* Checking Non-IM table */
  retval = wwan_coex_blank_efs_validate_nonim_lut(&desense_rec->nonim_lut,
                                                  &desense_valid_mask);

  if (retval == FALSE)
  {
    LMTSMGR_MSG_4(FATAL,"Debug: LUT invalid for vtech %d vband %d atech %d aband %d",
                  victim_tech, desense_rec->victim_band, 
                  aggr_tech, desense_rec->aggressor_band);
    /* Copy the LUT */
    memscpy((void*)&fatal_lut_rec,
            sizeof(fatal_lut_rec),
            (void*)desense_rec,
            sizeof(desense_rec));
    ERR_FATAL("Error in validating lut", 0,0,0);
  }

  return desense_valid_mask;
}

/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_tdd_validate_record

=============================================================================*/
/*!
    @brief
    Validates a DESENSE record 
 
    @return
    None
*/
/*===========================================================================*/
static uint32 wwan_coex_blank_efs_tdd_validate_record 
(
  wwan_coex_efs_blank_tdd_victim_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  uint32 desense_valid_mask = 0xE; /* All types of nonim desense columns are valid */
  boolean retval = TRUE;

  if (desense_rec == NULL)
  {
    return 0;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(victim_tech,
                                          desense_rec->victim_band))
  {
    return 0;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(aggr_tech,
                                          desense_rec->aggressor_band))
  {
    return 0;
  }

  /* Checking Non-IM table */
  retval = wwan_coex_blank_efs_validate_nonim_lut(&desense_rec->nonim_lut,
                                                  &desense_valid_mask);

  if (retval == FALSE)
  {
    LMTSMGR_MSG_4(FATAL,"Debug: LUT invalid for vtech %d vband %d atech %d aband %d",
                  victim_tech, desense_rec->victim_band, 
                  aggr_tech, desense_rec->aggressor_band);
    /* Copy the LUT */
    memscpy((void*)&fatal_lut_rec,
            sizeof(fatal_lut_rec),
            (void*)desense_rec,
            sizeof(desense_rec));
    ERR_FATAL("Error in validating lut", 0,0,0);
  }

  return desense_valid_mask;

}

/*=============================================================================

  FUNCTION:  wwan_coex_blank_efs_sem_validate_record

=============================================================================*/
/*!
    @brief
    Validates a DESENSE record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean wwan_coex_blank_efs_sem_validate_record 
(
  wwan_coex_efs_sem_record_type*  desense_rec,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  uint8  num_valid_rows = 0;
  int i;
  boolean ret=TRUE;

  if (desense_rec == NULL)
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(victim_tech,
                                          desense_rec->victim_band))
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(aggr_tech,
                                          desense_rec->aggressor_band))
  {
    return FALSE;
  }


  /* Validate LUTs-
     For SEM, TDD column should be in descending order while FDD column
     should be in ascending order
   */
  num_valid_rows = desense_rec->sem_lut.num_valid_rows;
  if ( (num_valid_rows > 0) && (num_valid_rows <= MAX_DESENSE_LUT_ROWS) )
  {
    for (i = 0; i < num_valid_rows - 1; i++)
    {
      if ( (desense_rec->sem_lut.blanking_threshs[i][0] <
                   desense_rec->sem_lut.blanking_threshs[i+1][0]) ||
           (desense_rec->sem_lut.blanking_threshs[i][1] > 
                   desense_rec->sem_lut.blanking_threshs[i+1][1]) )
      {
        ret = FALSE;
        break;
      }
    }
  }

  return ret;
}


/*=============================================================================

  FUNCTION:  wwan_coex_read_blank_efs_sem

=============================================================================*/
/*!
    @brief
    Reads all records from the WWAN Blanking COEX EFS file for checking SEM
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_blank_efs_sem
( 
  const char *fname,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  int32      fd;
  wwan_coex_efs_sem_record_type  sem_rec;
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
    result = efs_read (fd, (void *)&sem_rec, 
                       sizeof(wwan_coex_efs_sem_record_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading SEM record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_sem_record_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_efs_sem_record_type));
      break; 
    }
    
    /* Read was succesful. Now validate the data */
    rec_valid = wwan_coex_blank_efs_sem_validate_record(&sem_rec, 
                                                         victim_tech, aggr_tech);
    
    if (rec_valid)
    {
      /* Store the record in heap and assign pointer in global lut list */
      wwan_coex_blank_efs_sem_store_record(&sem_rec, victim_tech, aggr_tech);  
    }
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  wwan_coex_read_desense_efs_fdd

=============================================================================*/
/*!
    @brief
    Reads all records from the WWAN Blanking COEX EFS file for FDD tech as
    victim
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_desense_efs_fdd
( 
  const char *fname,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  int32      fd;
  wwan_coex_efs_blank_fdd_victim_type  fdd_desense_rec;
  int result;
  uint32 desense_mask;
  
    
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
    result = efs_read (fd, (void *)&fdd_desense_rec, 
                       sizeof(wwan_coex_efs_blank_fdd_victim_type));

    /* Check to see if the read failed. If it did, then determine
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_DBG_MSG_1(ERROR, "EFS error %d while reading DESENSE record", efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_blank_fdd_victim_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_efs_blank_fdd_victim_type));
      break; 
    }

    LMTSMGR_MSG_1(HIGH, "Debug:Read desense record of size %d",
                     sizeof(fdd_desense_rec));
    
    /* Read was successful. Now validate the data */
    desense_mask = wwan_coex_blank_efs_fdd_validate_record(&fdd_desense_rec, 
                                                   victim_tech, aggr_tech);
    
    /* Store the record in heap and assign pointer in global lut list */
    wwan_coex_blank_efs_fdd_store_record(&fdd_desense_rec, victim_tech, 
                                          aggr_tech, desense_mask);  
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  wwan_coex_read_desense_efs_tdd

=============================================================================*/
/*!
    @brief
    Reads all records from the WWAN Blanking COEX EFS file for TDD tech as
    victim
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_desense_efs_tdd
( 
  const char *fname,
  cxm_tech_type victim_tech,
  cxm_tech_type aggr_tech
)
{
  int32      fd;
  wwan_coex_efs_blank_tdd_victim_type  tdd_desense_rec;
  int result;
  uint32 desense_mask;
  
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
    result = efs_read (fd, (void *)&tdd_desense_rec, 
                       sizeof(wwan_coex_efs_blank_tdd_victim_type));

    /* Check to see if the read failed. If it did, then determine
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_DBG_MSG_1(ERROR, "EFS error %d while reading DESENSE record", efs_errno);
      break;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_blank_tdd_victim_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_efs_blank_tdd_victim_type));
      break; 
    }
    
    /* Read was successful. Now validate the data */
    desense_mask = wwan_coex_blank_efs_tdd_validate_record(&tdd_desense_rec, 
                                                   victim_tech, aggr_tech);
    
    /* Store the record in heap and assign pointer in global lut list */
    wwan_coex_blank_efs_tdd_store_record(&tdd_desense_rec, victim_tech, 
                                         aggr_tech, desense_mask);  

  }

  (void)efs_close(fd);
}

#endif /* MCS_LIMITS_LOW_MEMORY */


#ifdef FEATURE_MODEM_DDR_MGMT
/*=============================================================================

  FUNCTION:  wwan_coex_ddr_desense_store_record

=============================================================================*/
/*!
    @brief
    Stores a WWAN Blank EFS PWR record in heap and pointer in the global space
    for SEM
 
    @note
    wwan_coex_blank_efs_fdd_validate_record should be called before this
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_ddr_desense_store_record
(
  wwan_coex_ddr_band_spec_freq_plan_type*  desense_rec,
  cxm_tech_type tech,
  wwan_ddr_coex_table_entry carrier
)
{
  wwcoex_ddr_desense_lut_type* ddr_lut = NULL;
  wwan_ddr_coex_tech_lut_type* wwan_coex_tech_lut;
  uint32 band;

  /* Get the band info */
  band = desense_rec->band;

  /* Get the simult tech lut pointer */
  wwan_coex_tech_lut = wwan_ddr_coex_tables[tech][carrier];
  if (wwan_coex_tech_lut == NULL || wwan_coex_tech_lut->luts == NULL)
  {
    LMTSMGR_MSG_1(ERROR, "LUT not defined for this tech (%d)", tech);
    return;
  }

  /* Get the pointer to the lut list for the specific band combination */
  ddr_lut = ((wwcoex_ddr_desense_lut_type*)(wwan_coex_tech_lut->luts) + band);

  if (ddr_lut == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "ddr_lut NULL");
    return;
  }

  /* Time Thrshld check is added since we need to update 
        when we have valid info other wise for Band 0 it reads 
        incorrect since QCN would be 0 by default */

  if (ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].is_valid != TRUE &&
      (desense_rec->primary_upper_threshold != 0 ||
         desense_rec->primary_lower_threshold != 0))
  {
    ddr_lut->band = band;
    ddr_lut->channel_min = desense_rec->channel_min;
    ddr_lut->channel_max = desense_rec->channel_max;


    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].rxpower_threshold = 
       desense_rec->primary_rxpower_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].upper_threshold = 
       desense_rec->primary_upper_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].lower_threshold = 
       desense_rec->primary_lower_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].ddr_freq = 
       desense_rec->primary_ddr_freq;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_PRIMARY].is_valid = TRUE;
  }
  if (ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].is_valid != TRUE &&
      (desense_rec->primary_upper_threshold != 0 ||
         desense_rec->primary_lower_threshold != 0))
  {
    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].rxpower_threshold = 
       desense_rec->div_rxpower_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].upper_threshold = 
       desense_rec->div_upper_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].lower_threshold = 
       desense_rec->div_lower_threshold;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].ddr_freq = 
       desense_rec->div_ddr_freq;

    ddr_lut->ddr_threshold_table[COEX_DDR_TABLE_ENTRY_DIV].is_valid = TRUE;

  }

  return;
}

/*=============================================================================

  FUNCTION:  wwan_coex_ddr_desense_validate_record

=============================================================================*/
/*!
    @brief
    Validates a DESENSE record 
 
    @return
    None
*/
/*===========================================================================*/
static boolean wwan_coex_ddr_desense_validate_record 
(
  wwan_coex_ddr_band_spec_freq_plan_type*  desense_rec,
  cxm_tech_type tech
)
{
  boolean ret=TRUE;

  if (desense_rec == NULL)
  {
    return FALSE;
  }

  if (FALSE == lmtsmgr_efs_validate_tech_band(tech,
                                   desense_rec->band))
  {
    return FALSE;
  }

  return ret;
}


/*=============================================================================

  FUNCTION:  wwan_coex_read_default_ddr_desense_efs

=============================================================================*/
/*!
    @brief
    Reads all records for Default DDR Freq plan EFS and updates internal structure
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_default_ddr_desense_efs
( 
  const char *fname
)
{
  int32      fd;
  wwan_coex_default_ddr_freq_plan_type  ddr_freq;
  wwan_ddr_coex_tech_lut_type* wwan_coex_tech_lut;
  int result;
  uint8 index = 0;
  
  LMTSMGR_MSG_0(HIGH, "Entering wwan_coex_read_default_ddr_desense_efs");

  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return;
  }

  while (1)
  {
    /* Read  DDR Freq Plan for each Bands for this Tech */
    result = efs_read (fd, (void *)&ddr_freq, 
                      sizeof(wwan_coex_default_ddr_freq_plan_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading SEM record",efs_errno);
      break;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_default_ddr_freq_plan_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_default_ddr_freq_plan_type));
      break; 
    }
    
    for (index = 0; index < (uint8)CXM_TECH_MAX; index++)
    {
      wwan_coex_tech_lut = wwan_ddr_coex_tables[index][DDR_LUT_ENTRY_PRIMARY];
      if (wwan_coex_tech_lut != NULL)
      {
        wwan_coex_tech_lut->default_ddr_freq = ddr_freq.tech_ddr_freq[index];
        LMTSMGR_MSG_2(MED,"tech %d default_ddr_freq %d", index,
                 wwan_coex_tech_lut->default_ddr_freq);
      }

      /* LTE CA Entry */
      if ((cxm_tech_type)index == CXM_TECH_LTE) 
      {
        wwan_coex_tech_lut = wwan_ddr_coex_tables[index][DDR_LUT_ENTRY_SECONDARY];
        if (wwan_coex_tech_lut != NULL)
        {
          wwan_coex_tech_lut->default_ddr_freq = ddr_freq.tech_ddr_freq[index];
          LMTSMGR_MSG_2(MED,"tech %d (secondary) default_ddr_freq %d", index,
                   wwan_coex_tech_lut->default_ddr_freq);
        }
      }
    } /* End for */

  } /* End while */

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  wwan_coex_read_ddr_desense_efs

=============================================================================*/
/*!
    @brief
    Reads all records from the DDR COEX EFS file for checking tech desense due to DDR freq
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_ddr_desense_efs
( 
  const char *fname,
  cxm_tech_type tech, 
  wwan_ddr_coex_table_entry carrier
)
{
  int32      fd;
  wwan_coex_ddr_band_spec_freq_plan_type  ddr_freq_per_band;
  int result;
  
  boolean rec_valid = FALSE;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(fname, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_SPRINTF_2("Error opening EFS file %s (Error Code: %d)", fname, fd);
    return;
  }

  while (1)
  {
    /* Read  DDR Freq Plan for each Bands for this Tech */
    result = efs_read (fd, (void *)&ddr_freq_per_band, 
                      sizeof(wwan_coex_ddr_band_spec_freq_plan_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading DDR record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_ddr_band_spec_freq_plan_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                 sizeof(wwan_coex_ddr_band_spec_freq_plan_type));
      break; 
    }
    
    /* Read was succesful. Now validate the data */
    rec_valid = wwan_coex_ddr_desense_validate_record(&ddr_freq_per_band, 
                                                         tech);
    if (rec_valid)
    {
      /* Store the record in heap and assign pointer in global lut list */
      wwan_coex_ddr_desense_store_record(&ddr_freq_per_band, tech, carrier);  
    }
  }

  (void)efs_close(fd);
}

/*=============================================================================

  FUNCTION:  wwan_coex_read_gps_ddr_desense_efs

=============================================================================*/
/*!
    @brief
    Reads the GPS record DDR COEX EFS file for checking GPS desense due to DDR freq
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_read_gps_ddr_desense_efs(void)
{
  int32      fd;
  uint32 ddr_freq_plan;
  int result;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_DDR_GPS_VICTIM_DDR_AGGR, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file for DDR (GPS) %d", fd);
    return;
  }

  result = efs_read (fd, (void *)&ddr_freq_plan, sizeof(uint32));

  /* Check to see if the read failed. If it did, then deterime
   * why it failed. */
  if (result <= 0)
  {
    LMTSMGR_MSG_1(ERROR,"EFS error %d while reading DDR GPS record", efs_errno);
    (void)efs_close(fd);
    return;
  }

  /* If read passed, check if we read the expected number of bytes */
  if (result != sizeof(uint32))
  {
    LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
               sizeof(uint32));
    (void)efs_close(fd);
    return;
  }

  wwan_ddr_coex_gps_record.ddr_freq = ddr_freq_plan;
  wwan_ddr_coex_gps_record.is_valid = TRUE;

  (void)efs_close(fd);
}

#endif /* DDR */


/*=============================================================================

  FUNCTION:  wwan_coex_read_mitigation_efs

=============================================================================*/
/*!
    @brief
    Reads the Disable Mitigation EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwan_coex_read_mitigation_efs(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;

  /* Disable mask stored in the EFS... By default consider it to be 0 
     i.e assume sw mitigation is required */
  wwan_coex_efs_mitigation_mask_type     efs_content;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_DISABLE_MITIGATION, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 0;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(wwan_coex_efs_mitigation_mask_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading mitigation record",efs_errno);
      efs_content.disable_mask = 0;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_mitigation_mask_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(wwan_coex_efs_mitigation_mask_type));
      efs_content.disable_mask = 0;
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_1(MED, "Mitigation EFS file read passed with value 0x%x", 
                        efs_content.disable_mask );
    break;
  }

  (void)efs_close(fd);

  return (efs_content.disable_mask);
  
#else /* MCS_LIMITS_LOW_MEMORY */

  return 0;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwan_coex_read_en_mitigation_efs

=============================================================================*/
/*!
    @brief
    Reads the Enable Mitigation EFS file and return the value stored in it.
 
    @details
    This EFS file is currently only being used by Band Avoidance
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwan_coex_read_en_mitigation_efs(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;

  /* Disable mask stored in the EFS... By default consider it to be 0 
     i.e assume sw mitigation is required */
  wwan_coex_efs_mitigation_en_mask_type     efs_content;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_ENABLE_MITIGATION, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 0;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(wwan_coex_efs_mitigation_en_mask_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading mitigation record",efs_errno);
      efs_content.enable_mask = 0;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_mitigation_en_mask_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(wwan_coex_efs_mitigation_en_mask_type));
      efs_content.enable_mask = 0;
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_1(MED, "Mitigation EFS file read passed with value 0x%x", 
                        efs_content.enable_mask );
    break;
  }

  (void)efs_close(fd);

  return (efs_content.enable_mask);

#else /* MCS_LIMITS_LOW_MEMORY */
  
    return 0;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwan_coex_read_prio_update_efs

=============================================================================*/
/*!
    @brief
    Reads the priority update EFS file and return the value stored in it.
 
    @details
    This EFS file tells coex manager whether the super critical tiers need
    to be retained or not while updating priority tables
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwan_coex_read_prio_update_efs(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;

  uint32     efs_content = 0;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_PRIO_TABLE_UPD_ACTION, 
                O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 0;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(uint32));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading mitigation record",efs_errno);
      efs_content = 0;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(uint32))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(uint32));
      efs_content = 0;
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_1(MED, "Prio upodate EFS file read passed with value 0x%x", 
                        efs_content );
    break;
  }

  (void)efs_close(fd);

  return (efs_content);

#else /* MCS_LIMITS_LOW_MEMORY */
  
  return 0;
  
#endif /* MCS_LIMITS_LOW_MEMORY */
}

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwan_coex_read_idle_mode_rxpwr_efs

=============================================================================*/
/*!
    @brief
    Reads the Rx Power limit info in EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
static int16 wwan_coex_read_idle_mode_rxpwr_efs(void)
{
  int32      fd;
  int        result;

  /* Rx Power limit info in EFS file, By default consider it to be -850 */
  int16     efs_content;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_IDLE_MODE_HOPPING_RXPWR, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 0;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(int16));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading mitigation record",efs_errno);
      efs_content = LMTSMGR_GSM_RXPOWER_HOPPING_LIMIT;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(int16))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(int16));
      efs_content = LMTSMGR_GSM_RXPOWER_HOPPING_LIMIT;
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_1(MED, "Mitigation EFS file read passed with value %d", 
                        efs_content );
    break;
  }

  (void)efs_close(fd);

  return efs_content;
}
#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  wwan_coex_read_desense_ind_efs

=============================================================================*/
/*!
    @brief
    Reads the desense indication EFS and stores the thresholds 
 
    @details
    This EFS file tells coex manager about the thresholds for triggering
    desense indication and timers values to be used for desense indication
 
    @return
    None
*/
/*===========================================================================*/
void wwan_coex_read_desense_ind_thresh_efs(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;
  wwan_coex_efs_desense_ind_thresh_type desense_thresh;

  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_DESENSE_IND_THRESHOLD, 
                O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  /* Set default values */
  lmtsmgr.desense_stats.delta_t_msec           = 40;
  lmtsmgr.desense_stats.mild_coex_time         = 6;
  lmtsmgr.desense_stats.severe_coex_time       = 12;
  lmtsmgr.desense_stats.mild_coex_thresh       = 10;
  lmtsmgr.desense_stats.severe_coex_thresh     = 20;

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening Desense threshold EFS file %d", fd);
    return;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&desense_thresh, 
                       sizeof(wwan_coex_efs_desense_ind_thresh_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while desense threshold record",efs_errno);
      result = 0;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(wwan_coex_efs_desense_ind_thresh_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(wwan_coex_efs_desense_ind_thresh_type));
      result = 0;
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_0(MED, "Desense threshold EFS file read passed ");
    break;
  }

  (void)efs_close(fd);

  /* If EFS read is successful then copy the values read from EFS */
  if(result != 0)
  {
    /* Set default values for EFS */
    lmtsmgr.desense_stats.delta_t_msec           = desense_thresh.delta_t_msec;
    lmtsmgr.desense_stats.mild_coex_time         = desense_thresh.mild_coex_time;
    lmtsmgr.desense_stats.severe_coex_time       = desense_thresh.severe_coex_time;
    lmtsmgr.desense_stats.mild_coex_thresh       = desense_thresh.mild_coex_thresh;
    lmtsmgr.desense_stats.severe_coex_thresh     = desense_thresh.severe_coex_thresh;
  }

  return;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwan_coex_read_spur_sim_info

=============================================================================*/
/*!
    @brief
    Reads the spur info EFS and stores the enties 
 
    @details
    This EFS file tells coex manager about the spur level offset
    and notch depth to given to FW
 
    @return
    None
*/
/*===========================================================================*/
void wwan_coex_read_spur_sim_info(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;
  wwan_coex_efs_spur_sim_info_type  spur_info;

  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_SPUR_SIM_INPUT, 
                O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  wwcoex_sim_spur_count = 0;

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening spur sim EFS file %d", fd);
    return;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&spur_info, 
                       sizeof(wwan_coex_efs_spur_sim_info_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while desense threshold record",efs_errno);
      result = 0;
      break ;
    }
    /* If read passed, check if we read the expected number of bytes */
    else if (result != sizeof(wwan_coex_efs_spur_sim_info_type))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(wwan_coex_efs_spur_sim_info_type));
      result = 0;
      break; 
    }
    else
    {
      wwcoex_sim_spur_info[wwcoex_sim_spur_count].spur_freq_offset = spur_info.spur_freq_offset;
      wwcoex_sim_spur_info[wwcoex_sim_spur_count].spur_level       = spur_info.spur_level;
      wwcoex_sim_spur_info[wwcoex_sim_spur_count].notch_depth      = spur_info.notch_depth;

      wwcoex_sim_spur_count += 1;
      
      if(wwcoex_sim_spur_count >= WWCOEX_MAX_CONCURRENT_SPURS)
      {
        break;
      }
    }
    
  }

  (void)efs_close(fd);

  return;

#endif /* MCS_LIMITS_LOW_MEMORY */

}


#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwan_coex_efs_set_all_luts_to_default

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_efs_set_all_luts_to_default
(
  wwan_coex_tech_comb_lut_type* lut_list
)
{
  int i, j, index;
  wwan_coex_lut_type* wwan_coex_luts;
  uint32 hprio_bands;
  uint32 lprio_bands;

  if (lut_list == NULL || lut_list->luts == NULL)
  {
    return;
  }

  lut_list->updated = FALSE;

  hprio_bands = lut_list->num_tech1_bands;
  lprio_bands = lut_list->num_tech2_bands;

  for(i = 0; i < hprio_bands; i++)
  {
    for(j = 0; j < lprio_bands; j++)
    {
      index = (i * lprio_bands + j) ;

      /* Get the pointer to the LUT */
      wwan_coex_luts = (wwan_coex_lut_type*)lut_list->luts + index;
      memscpy((void *)wwan_coex_luts, 
             sizeof(wwan_coex_lut_type),
             (void *)&wwan_coex_default_luts, 
             sizeof(wwan_coex_lut_type)); 
    }
  }
}

/*=============================================================================

  FUNCTION:  wwan_coex_efs_set_default_lut

=============================================================================*/
/*!
    @brief
    Sets the global luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_coex_efs_set_default_lut (void)
{
  cxm_tech_type i, j;
  wwan_coex_tech_comb_lut_type* luts;

  for (i=0; i<CXM_TECH_MAX; i++)
  {
    for(j=0; j<CXM_TECH_MAX; j++)
    {
      /* Is this a valid scenario ? */
      if (wwan_coex_tables[i][j] != NULL)
      {
        luts = wwan_coex_tables[i][j];
          
        /* Set all the LUTs to default for this simult tech  */
        wwan_coex_efs_set_all_luts_to_default(luts);
      }
    }
  }
}

#endif /* MCS_LIMITS_LOW_MEMORY */

#ifdef FEATURE_MODEM_DDR_MGMT
/*=============================================================================

  FUNCTION:  wwan_ddr_coex_efs_set_default_lut

=============================================================================*/
/*!
    @brief
    Sets the global DDR luts to default 
 
    @return
    None
*/
/*===========================================================================*/
static void wwan_ddr_coex_efs_set_default_lut (void)
{
  cxm_tech_type i;
  uint16 j,k;
  wwan_ddr_coex_tech_lut_type* coex_table;
  wwcoex_ddr_desense_lut_type* coex_luts;
  uint32 bands;

  for (i=0; i<CXM_TECH_MAX; i++)
  {
    for (j=0; j<(uint16) DDR_LUT_ENTRY_MAX; j++)
    {
      /* Is this a valid scenario ? */
      coex_table = wwan_ddr_coex_tables[i][j];
      if (coex_table != NULL && coex_table->luts != NULL)
      {
        bands = coex_table->num_tech_bands;

        /* Set all the LUTs to default for tech  */
        for(k = 0; k < bands; k++)
        {
          /* Get the pointer to the LUT */
          coex_luts = (wwcoex_ddr_desense_lut_type*)coex_table->luts + k;
          /* Reset the global struct */
          memset((void *)coex_luts, 0, sizeof(wwcoex_ddr_desense_lut_type));
        }
      }
    } /* End for */
  } /* End for (i) */
}

#endif /* DDR */


/*=============================================================================

  FUNCTION:  wwan_coex_read_simulation_efs

=============================================================================*/

/*!
    @brief
    Reads the simulation input EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwan_coex_read_simulation_efs(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  int32      fd;
  int        result;
  uint32     efs_content = 0;
  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(LIMITSMGR_COEX_SIMULATION_INPUT, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    LMTSMGR_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 91;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(efs_content));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      LMTSMGR_MSG_1(ERROR,"EFS error %d while reading mitigation record",efs_errno);
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(efs_content))
    {
      LMTSMGR_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(efs_content));
      break; 
    }
    
    /* Read was succesful... */
    LMTSMGR_MSG_1(MED, "Mitigation EFS file read passed with value 0x%x", 
                        efs_content );
    break;
  }

  (void)efs_close(fd);

  return (efs_content);

#else /* MCS_LIMITS_LOW_MEMORY */

  return ACTION_NONE;

#endif /* MCS_LIMITS_LOW_MEMORY */
}


/*=============================================================================

  FUNCTION:  wwan_coex_efs_init

=============================================================================*/
/*!
    @brief
    Initializes WWAN COEX EFS
 
    @return
    None
*/
/*===========================================================================*/
void wwan_coex_efs_init (void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_tech_type tech1, tech2;
  wwan_coex_tech_comb_lut_type* tbl_entry;
#ifdef FEATURE_MODEM_DDR_MGMT
  wwan_ddr_coex_tech_lut_type* ddr_entry;
#endif
  /* Set the LUTs to default */
  wwan_coex_efs_set_default_lut();

#ifdef FEATURE_MODEM_DDR_MGMT
  /* Set the DDR LUTs to default */
  wwan_ddr_coex_efs_set_default_lut();

  /* Initiailize DDR record for GPS */
  wwan_ddr_coex_gps_record.is_valid = FALSE;
  wwan_ddr_coex_gps_record.ddr_freq = 0;
#endif

  /* Read the disable mitigation EFS... */
  wwcoex_mitigation_disable_mask = wwan_coex_read_mitigation_efs();

  /* Read the enable mitigation EFS... */
  wwcoex_mitigation_enable_mask = wwan_coex_read_en_mitigation_efs();
  
  /* Read the Idle Mode Hopping Rx Pwr Limit EFS */
  wwcoex_idle_mode_hopping_rxpwr_lmt = wwan_coex_read_idle_mode_rxpwr_efs();

  /* Check the input to coex simulator */
  wwcoex_simulation_input = wwan_coex_read_simulation_efs();

  /* Check if spur over-ride is present */
  wwan_coex_read_spur_sim_info();
  
  /* Read the files one by one... */
  for (tech1=0; tech1<CXM_TECH_MAX; tech1++)
  {
    for(tech2=0; tech2<CXM_TECH_MAX; tech2++)
    {
      tbl_entry = wwan_coex_tables[tech1][tech2];

      /* If there is no entry, then continue to the next */
      if (tbl_entry == NULL)
      {
        continue;
      }

      if (tbl_entry->updated == TRUE )
      {
        /* This table has been read, skip to the next */
        continue;
      }

      /* Read the IM DESENSE BACKOFF EFS file */
      if (tbl_entry->files.im_backoff_fname != NULL)
      {
        wwan_coex_read_backoff_efs(tbl_entry->files.im_backoff_fname, 
                              tech1, tech2, IM_TYPE1_DESENSE);
      }

      if (tbl_entry->files.desense_fname != NULL)
      {
        if (WWCOEX_IS_TDD_TECH(tech1))
        {
          wwan_coex_read_desense_efs_tdd(tbl_entry->files.desense_fname, 
                                       tech1, tech2);
        }
        else
        {
          wwan_coex_read_desense_efs_fdd(tbl_entry->files.desense_fname, 
                                       tech1, tech2);
        }
      }

      /* Read the GPS IM DESENSE EFS file */
      if (tbl_entry->files.gps_desense_fname != NULL)
      {
        wwan_coex_read_backoff_efs(tbl_entry->files.gps_desense_fname, 
                              tech1, tech2, IM_DESENSE_GPS);
      }

      if (tbl_entry->files.sem_fname != NULL)
      {
        wwan_coex_read_blank_efs_sem(tbl_entry->files.sem_fname, 
                                     tech1, tech2);
      }

      /* Set the updated flag so that we dont read this file again */
      tbl_entry->updated = TRUE;
    }
#ifdef FEATURE_MODEM_DDR_MGMT
    /* Update DDR entry for all techs which are band specific */
    ddr_entry = wwan_ddr_coex_tables[tech1][DDR_LUT_ENTRY_PRIMARY];

    /* If there is no entry, then continue to the next */
    if (ddr_entry == NULL)
    {
      continue;
    }

    if (ddr_entry->ddr_desense_fname != NULL)
    {
      wwan_coex_read_ddr_desense_efs(ddr_entry->ddr_desense_fname, 
                                      tech1, DDR_LUT_ENTRY_PRIMARY);
    }

    /* LTE CA entry */
    if (tech1 == CXM_TECH_LTE)
    {
      ddr_entry = wwan_ddr_coex_tables[tech1][DDR_LUT_ENTRY_SECONDARY];
      wwan_coex_read_ddr_desense_efs(ddr_entry->ddr_desense_fname, 
                                      tech1, DDR_LUT_ENTRY_SECONDARY);
    }
#endif
}

#ifdef FEATURE_MODEM_DDR_MGMT
  /* Read DDR desense frequency plan for GPS */
  wwan_coex_read_gps_ddr_desense_efs();

  /* Read Default DDR desense freq plans for each Tech */
  wwan_coex_read_default_ddr_desense_efs(LIMITSMGR_DEFAULT_TECH_DDR_FREQ_PLAN);
#endif

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_mitigation_mask_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the mitigation disable mask.
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwcoex_mitigation_mask_efs_lookup(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  return wwcoex_mitigation_disable_mask;

#else /* MCS_LIMITS_LOW_MEMORY */

  return 0;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_mitigation_mask_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the mitigation enable mask.
 
    @details
    This is currently used for Band Avoidance (Power Backoff/Blanking/DDR
    are enabled by default)
 
    @return
    None
*/
/*===========================================================================*/
uint32 wwcoex_mitigation_en_mask_efs_lookup(void)
{
#ifndef MCS_LIMITS_LOW_MEMORY

  return wwcoex_mitigation_enable_mask;

#else /* MCS_LIMITS_LOW_MEMORY */

  return 0;

#endif /* MCS_LIMITS_LOW_MEMORY */
}

/*=============================================================================

  FUNCTION:  wwcoex_idle_mode_rxpwr_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the GSM Rx Power Limit to enable Idle Mode Hopping.
 
    @details
    This is currently used to check whether Rx Pwr Limits is met for disabling Idle Mode
    Hopping, and based on that we indicate TRM
 
    @return
    Rx Power Limit
*/
/*===========================================================================*/
int16 wwcoex_idle_mode_rxpwr_efs_lookup(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  return wwcoex_idle_mode_hopping_rxpwr_lmt;

#else /* MCS_LIMITS_LOW_MEMORY */

  return 0;

#endif /* MCS_LIMITS_LOW_MEMORY */
  
}

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwcoex_desense_im_backoff_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the current Desense Tx limits.
 
    @return
    None
*/
/*===========================================================================*/
static void wwcoex_desense_im_backoff_efs_lookup
(
  wwan_coex_efs_lookup_in* input,
  wwan_desense_efs_lookup_out* output,
  wwan_coex_desense_lut_type*  desense_lut
)
{
  uint32 index = 0;
  int16 aplimit = DEFAULT_MAX_PLIMIT;

  /* Check input/output ptr*/
  if (input == NULL || output == NULL)
  {
    return;
         }

  if (!LMTSMGR_IS_TECH_VALID(input->vtech) || 
      !LMTSMGR_IS_TECH_VALID(input->atech) || 
      (desense_lut == NULL))/* There is no lookup table for this band combination */
         { 
    return;
     }

       /* Check if the first entry is sufficient */
       if ( (desense_lut->num_valid_lut_rows == 0) ||
            (input->input_power < desense_lut->aggressor_limits[0][0]) )
       {
         /* Apply max limit */
         aplimit = DEFAULT_MAX_PLIMIT;
       }
       else
       { 
    for (index=1; index < desense_lut->num_valid_lut_rows; index++)
         {
      if( input->input_power < desense_lut->aggressor_limits[index][0] )
           {
             /* Set the limit as the previous entry */
        aplimit = desense_lut->aggressor_limits[index-1][1];
             break;
           }
         }

    if (index == desense_lut->num_valid_lut_rows)
         { 
           /* If we reach here, that means the last power threshold has been crossed.
              So lets set the plimit to last entry i.e max */
      aplimit = desense_lut->aggressor_limits[index-1][1];
       }
     }
  
  /* Set the output struct limit value */
  output->aplimit = aplimit;
  output->aplimit_valid = TRUE;
}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  wwcoex_blank_efs_lookup_get_thresholds

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for blanking thresholds.
 
    @details
    [in] input - Tx/Rx tech, band, & power info
    [in] entry_type - read Tx or Rx Thresholds
    [out] txpwr_thresh - Threshold to be updated
    [out] rxpwr_thresh - Rx Threshold to be updated
 
    @return
      TRUE - Tables exist for tech/band combination
      FALSE - Tables do not exist for tech/band combination
      txpwr_thresh & rxpwr_thresh will be updated with associated LUT
      (CXM_UNKNOWN_POWER if table does not exist or power does not meet
      thresholds)
 
*/
/*===========================================================================*/
boolean wwcoex_blank_efs_lookup_get_thresholds
(
  wwcoex_blank_efs_lookup_in*  input,       /* Aggr/Victim Tx/Rx Band & Power Info */
  wwan_coex_desense_mode       entry_type,  /* Assumes AGGR or VICTIM */
  int16                     *txpwr_thresh,  /* Tx Power read from LUT */
  int16                     *rxpwr_thresh   /* Rx Power read from LUT */
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  wwan_coex_tech_comb_lut_type* tables;
  wwan_coex_lut_type* luts;

  wwcoex_im_lut_type*              im_lut;
  wwan_coex_desense_lut_type*      nonim_lut;

  boolean tables_exist = FALSE;

  int i, index = 0, bias = 0;
  
  //TODO: Utilize Bias EFS

  /* Check input validity */
  if ((input == NULL) || (input->vtech >= CXM_TECH_MAX) || (input->atech >= CXM_TECH_MAX) ||
      (txpwr_thresh == NULL) || (rxpwr_thresh == NULL))
  {
    return tables_exist;
  }

  *txpwr_thresh = CXM_UNKNOWN_POWER;
  *rxpwr_thresh = CXM_UNKNOWN_POWER;

  /* Check if tables exist */
  if ( wwan_coex_tables[input->vtech] == NULL  || 
       wwan_coex_tables[input->vtech][input->atech] == NULL )
  {
    return tables_exist;
  }
  tables = wwan_coex_tables[input->vtech][input->atech];

  /* check bands */
  if ( (input->vband >= tables->num_tech1_bands) || 
       (input->aband >= tables->num_tech2_bands) )
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect band %d %d", input->vband, input->aband);
    return tables_exist;
  }

  index = input->vband * tables->num_tech2_bands + input->aband;
  luts = (wwan_coex_lut_type *)(tables->luts);

  switch (input->desense)
  {
    case NOISE_DESENSE:
    case HARMONIC_DESENSE:
    case BLOCKING_DESENSE:
    {
      /* Select the appropriate LUT */
      if (input->desense == NOISE_DESENSE)
      {
        nonim_lut = (luts + index)->rxbn_lut;
      }
      else if (input->desense == HARMONIC_DESENSE)
      {
        nonim_lut = (luts + index)->harmonics_lut;
      }
      else // BLOCKING_DESENSE
      {
        nonim_lut = (luts + index)->blocking_lut;
      }

      if ((nonim_lut == NULL) || (nonim_lut->num_valid_lut_rows == 0))
      {
        /* No LUT specified for this band combination */
        return tables_exist;
      }

      if (entry_type == DESENSE_VICTIM)
      {
         /* Find the rx threshold based on Tx power level */
         if (input->aggr_txpower == CXM_UNKNOWN_POWER)
         {
           /* Cannot determine rx threshold with unknown Tx power level */
           break;
         }
         else if(input->aggr_txpower >= nonim_lut->aggressor_limits[0][0])
         {
           /* check the first entry */
           *rxpwr_thresh = nonim_lut->aggressor_limits[0][1] + bias;
           break;
         }
         else
         {
           /* Check if any other entry matches... */
           for (i=1; i < nonim_lut->num_valid_lut_rows; i++)
           {
              if( input->aggr_txpower >= nonim_lut->aggressor_limits[i][0] && 
                  input->aggr_txpower < nonim_lut->aggressor_limits[i-1][0] )
              {
                *rxpwr_thresh = nonim_lut->aggressor_limits[i][1] + bias;
                break;
              }
           }
         }
      }
      else // entry_type == DESENSE_AGGR
      {
        /* Find the Tx threshold based on Rx power level */
        if (input->victim_rxpower == CXM_UNKNOWN_POWER)
        {
          /* Cannot determine Tx threshold with unknown Rx power level */
          break;
        }
        else if(input->victim_rxpower <= nonim_lut->aggressor_limits[nonim_lut->num_valid_lut_rows-1][1])
        {
          /* Check the first entry*/
          *txpwr_thresh = nonim_lut->aggressor_limits[nonim_lut->num_valid_lut_rows-1][0] - bias;
          break;
        }
        else
        {
           /* Check if any other entry matches... */
           for (i=nonim_lut->num_valid_lut_rows-2; i >= 0; i--)
           {
              if( input->victim_rxpower <= nonim_lut->aggressor_limits[i][0] &&
                  input->victim_rxpower > nonim_lut->aggressor_limits[i+1][1] )
              {
                 *txpwr_thresh = nonim_lut->aggressor_limits[i][0] - bias;
                 break;
              }
           }
        }
      } // end else (entry_type == DESENSE_AGGR)
    } // end case
    break;

    case IM_TYPE1_DESENSE:
    case IM_TYPE2_DESENSE:
    {
      im_lut = (luts + index)->im_lut;

      if ((im_lut == NULL) || (im_lut->num_valid_rows == 0))
      {
        /* No LUT specified for this band combination */
        return tables_exist;
      }

      if (entry_type == DESENSE_VICTIM)
      {
        /* Find the rx threshold based on Tx power level */
        if (input->aggr_txpower == CXM_UNKNOWN_POWER)
        {
          /* Cannot determine rx threshold with unknown Tx power level */
          break;
        }
        else if(input->aggr_txpower >= im_lut->tbl[0].aggr_txpwr_thresh)
        {
          *rxpwr_thresh = (input->desense == IM_TYPE1_DESENSE) ? 
                          (im_lut->tbl[0].victim_rxpwr_imtype1_thresh + bias) :
                          (im_lut->tbl[0].victim_rxpwr_imtype2_thresh + bias);
          break;
        }
        else
        {
          /* Check if any other entry matches... */
          for (i=1; i < im_lut->num_valid_rows; i++)
          {
              if( input->aggr_txpower >= im_lut->tbl[i].aggr_txpwr_thresh && 
                  input->aggr_txpower < im_lut->tbl[i-1].aggr_txpwr_thresh )
              {
                *rxpwr_thresh = (input->desense == IM_TYPE1_DESENSE) ? 
                          (im_lut->tbl[0].victim_rxpwr_imtype1_thresh + bias) :
                          (im_lut->tbl[0].victim_rxpwr_imtype2_thresh + bias);
                break;
              }
          }
        }
      }
      else // entry_type == DESENSE_AGGR
      {
         /* Find the Tx threshold based on Rx power level */
        if (input->victim_rxpower == CXM_UNKNOWN_POWER)
        {
          /* Cannot determine Tx threshold with unknown Rx power level */
          break;
        }
        else if(((input->desense == IM_TYPE1_DESENSE) && 
                 (input->victim_rxpower <= im_lut->tbl[im_lut->num_valid_rows-1].victim_rxpwr_imtype1_thresh)) ||
                ((input->desense == IM_TYPE2_DESENSE) &&
                 (input->victim_rxpower <= im_lut->tbl[im_lut->num_valid_rows-1].victim_rxpwr_imtype2_thresh)))
        {
          /* Check the last entry - compare Rx power iwth rx power threshold for desense type I or II */
          *txpwr_thresh = im_lut->tbl[im_lut->num_valid_rows-1].aggr_txpwr_thresh - bias;
          break;
        }
        else
        {
           /* Check if any other entry matches... */
           for (i=im_lut->num_valid_rows-2; i >= 0; i--)
           {
              if( ((input->desense == IM_TYPE1_DESENSE) &&
                    input->victim_rxpower <= im_lut->tbl[i].victim_rxpwr_imtype1_thresh &&
                    input->victim_rxpower > im_lut->tbl[i+1].victim_rxpwr_imtype1_thresh ) ||
                  ((input->desense == IM_TYPE2_DESENSE) &&
                    input->victim_rxpower <= im_lut->tbl[i].victim_rxpwr_imtype2_thresh &&
                    input->victim_rxpower > im_lut->tbl[i+1].victim_rxpwr_imtype2_thresh ))
              {
                 *txpwr_thresh = im_lut->tbl[i].aggr_txpwr_thresh - bias;
                 break;
              }
           }
        }
      } // end else (entry_type == DESENSE_AGGR)
    } // end case (IM)
    break;

    case NO_DESENSE:
    case SPURIOUS_EMISSIONS:
    default:
    {
       LMTSMGR_MSG_1(ERROR, "Tables do not exist for desense type %d", input->desense);
       tables_exist = FALSE;
       return tables_exist;
    }
  }

  tables_exist = TRUE;
  return tables_exist;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(input);
  WWCOEX_DUMMY_CHECK(entry_type);
  WWCOEX_DUMMY_CHECK(txpwr_thresh);
  WWCOEX_DUMMY_CHECK(rxpwr_thresh);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */
}

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwcoex_desense_nonim_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for Non-IM Desense thresholds.
 
    @return
    TRUE/FALSE - If Desense combination is present in EFS or not
*/
/*===========================================================================*/
static boolean wwcoex_desense_nonim_efs_lookup
(
  int16	     v_rxpower,
  int16      a_txpower,
  wwan_desense_efs_lookup_out* output,
  wwan_coex_desense_lut_type*  lut,
  cxm_action_type   action
)
{
  uint8 index = 0;

  /* Check input ptr*/
  if (output == NULL)
  {
    return FALSE;
  }

  if (lut == NULL)
  {
    /* No LUT specified for this band combination and set output as NONE/FALSE*/
    return TRUE;
  }

  switch (action)
  {
    case ACTION_BACKOFF:
    {
      if (v_rxpower == CXM_UNKNOWN_POWER)
      {
        /* If power is unknown, then dont trigger backoffs, so aplimit_valid as FALSE */
        return TRUE;
  }
      /* Check if the first entry is sufficient */
      else if ( (lut->num_valid_lut_rows == 0) ||
           (v_rxpower > lut->aggressor_limits[0][1]) )
  {
        /* Apply max limit */
        output->aplimit = DEFAULT_MAX_PLIMIT;
        output->aplimit_valid = TRUE;
  }
      else
  {
        for (index=1; index < lut->num_valid_lut_rows; index++)
    {
          if( v_rxpower > lut->aggressor_limits[index][1] )
      {
            /* Set the limit as the previous entry */
            output->aplimit_valid = TRUE;
            output->aplimit = lut->aggressor_limits[index-1][0];
            break;
          }
        }

        if (index == lut->num_valid_lut_rows)
        { 
          /* If we reach here, that means the last power threshold has been crossed.
                       So lets set the plimit to last entry i.e max */
          output->aplimit_valid = TRUE;
          output->aplimit = lut->aggressor_limits[index-1][0];
      }
      }
      }
    break;

    case ACTION_BLANK:
    {
      /* Check if we have UNKNOWN power values... */
      if (v_rxpower == CXM_UNKNOWN_POWER || 
          a_txpower == CXM_UNKNOWN_POWER)
      {
        output->action = ACTION_UNKNOWN;
      }
      else
      {
        /* Check if there are any entries in the LUT */
        if (lut->num_valid_lut_rows == 0)
        {
          output->action = ACTION_NONE;
        }
        /* Check the first entry */
        else if( a_txpower >= lut->aggressor_limits[0][0] && 
                 v_rxpower <= lut->aggressor_limits[0][1])
        {
          output->action = ACTION_BLANK;
        }
        else
        {
          /* Check if any other entry matches... */
          for (index=1; index < lut->num_valid_lut_rows; index++)
          {
            if( a_txpower >= lut->aggressor_limits[index][0] && 
                a_txpower < lut->aggressor_limits[index-1][0] &&
                v_rxpower <= lut->aggressor_limits[index][1] )
            {
              output->action = ACTION_BLANK;
              break;
            }
          }
        }
      }
    }
    break;

    default:
    {
      LMTSMGR_MSG_3(FATAL, "Incorrect action type: %d, v_rx: %d, a_tx: %d",
                                                 action, v_rxpower, a_txpower);
    }
    break;
      
      }

  return TRUE;
}

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

  FUNCTION:  wwcoex_desense_im_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for IM Desense thresholds.
 
    @return
    TRUE/FALSE - If Desense combination is present in EFS or not
*/
/*===========================================================================*/
extern boolean wwcoex_desense_im_efs_lookup
(
  wwcoex_blank_efs_lookup_in*  input,
  wwan_desense_efs_lookup_out* output,
  wwan_coex_lut_type*  luts,
  uint32 lut_index
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint8 index = 0;
  wwcoex_im_lut_type*              blank_lut = NULL;
  wwan_coex_desense_lut_type*      backoff_lut = NULL;
  wwan_coex_efs_lookup_in          backoff_input;
  cxm_tech_type lprio_tech;

  /* Check input ptr*/
  if ((input == NULL) || (output == NULL))
  {
    return FALSE;
  }

  if (luts == NULL)
  {
    /* No LUT specified for this band combination and set output as NONE/FALSE*/
    return FALSE;
  }

  switch (input->action)
  {
    case ACTION_BACKOFF:
    {

      /// get the coex input here wwan_coex_efs_lookup(input, output)  and later this can be called and priotiy value can eb found out

      if (input->hprio_tech == input->vtech)
      {
        lprio_tech = input->atech;
      }
      else
      {
        lprio_tech = input->vtech;
      }

      /* Choose the EFS file based on the High Prio victim or Low Prio victim, 
                         Aggr_TX                Victim_TX                Victim_Rx

               IM1    Low_Prio               High_Prio                 High_Prio   -->  (Victim_TX, Victim_RX) --> Aggr_TX (Limits)
               Eg.         (LTE)                    (1X)                         (1X)       -->  (1X_Tx, 1X_Rx) --> Limits LTE_Tx (EFS:   1X_Victim_LTE_Aggr)
               
               IM2    High_Prio               Low_Prio                 High_Prio   -->  (Aggr_TX, Victim_RX)   --> Victim_TX (Limits)
               Eg.         (1X)                    (LTE)                        (LTE)       -->  (1X_Tx, LTE_Rx) --> Limits LTE_Tx (EFS:   LTE_Victim_1X_Aggr)
      
           */
      if (input->desense == IM_TYPE2_DESENSE) /* Low prio tech is Victim */
      {
        /* IM Desense Low Prio tech */
        backoff_input.vtech = lprio_tech;
        backoff_input.atech = input->hprio_tech;

        if (lprio_tech == input->vtech)
        {
          backoff_input.input_power = input->aggr_txpower;
          backoff_input.vband = input->vband;
          backoff_input.aband = input->aband;
        }
        else
        {
          backoff_input.input_power = input->victim_txpower;
          backoff_input.vband = input->aband;
          backoff_input.aband = input->vband;
        }
        backoff_lut = (luts + lut_index)->im_desense_lp_victim_lut;
      }
      else if ( (input->desense == IM_TYPE1_DESENSE) /* High prio tech is Victim */ ||
                    (input->desense == IM_DESENSE_GPS) )
      {
        /* IM Desense High prio tech */
        backoff_input.vtech = input->hprio_tech;
        backoff_input.atech = lprio_tech;
        if (input->hprio_tech == input->vtech)
        {
          backoff_input.input_power = input->victim_txpower;
          backoff_input.vband = input->vband;
          backoff_input.aband = input->aband;
        } 
        else
        {
          backoff_input.input_power = input->aggr_txpower;
          backoff_input.vband = input->aband;
          backoff_input.aband = input->vband;
        }
        /* Get correct LUT */
        if (input->desense == IM_DESENSE_GPS)
          {
          backoff_lut = (luts + lut_index)->gps_im_desense_lut;
        }
        else
        {
          backoff_lut = (luts + lut_index)->im_desense_hp_victim_lut;
        }
      }

      if (backoff_lut == NULL)
      {
        /* No LUT specified for this band combination and set output as NONE/FALSE*/
        return FALSE;
      }

      /* If power is unknown, then dont trigger backoffs, so aplimit_valid as FALSE */
      if (backoff_input.input_power != CXM_UNKNOWN_POWER)
      {
        /* Query for the output from IM BackOff function */
        wwcoex_desense_im_backoff_efs_lookup(&backoff_input, output, backoff_lut);
      }
    }
    break;

    case ACTION_BLANK:
    {
      blank_lut = (luts + lut_index)->im_lut;
      
      if (blank_lut == NULL)
      {
        /* No LUT specified for this band combination */
        output->action = ACTION_NONE;
      }
      /* Check if we have UNKNOWN power values... */
      else if (input->victim_rxpower == CXM_UNKNOWN_POWER || 
               input->victim_txpower == CXM_UNKNOWN_POWER ||
               input->aggr_txpower == CXM_UNKNOWN_POWER)
      {
        output->action = ACTION_UNKNOWN;
      }
      else
      {
        /* Check if there are any entries in the LUT */
        if (blank_lut->num_valid_rows == 0)
        {
          output->action = ACTION_NONE;
        }
        /* Check the first entry */
        else if( input->aggr_txpower >= blank_lut->tbl[0].aggr_txpwr_thresh &&
                 input->victim_txpower >= blank_lut->tbl[0].victim_txpwr_thresh && 
                 ( ((input->desense == IM_TYPE1_DESENSE) &&
                    (input->victim_rxpower <= blank_lut->tbl[0].victim_rxpwr_imtype1_thresh)) ||
                   ((input->desense == IM_TYPE2_DESENSE) &&
                    (input->victim_rxpower <= blank_lut->tbl[0].victim_rxpwr_imtype2_thresh)) ) )
        {
          output->action = ACTION_BLANK;
        }
        else
        {
          /* Check if any other entry matches... */
          for (index = 1; index < blank_lut->num_valid_rows; index++)
          {
            if (blank_lut->tbl[index].victim_txpwr_thresh == blank_lut->tbl[index-1].victim_txpwr_thresh)
            {
              /* Equal entries in victim Tx power */

              if( input->aggr_txpower >= blank_lut->tbl[index].aggr_txpwr_thresh && 
                  input->aggr_txpower < blank_lut->tbl[index-1].aggr_txpwr_thresh &&
                  input->victim_txpower >= blank_lut->tbl[index].victim_txpwr_thresh &&
                  ( ((input->desense == IM_TYPE1_DESENSE) &&
                     (input->victim_rxpower <= blank_lut->tbl[index].victim_rxpwr_imtype1_thresh)) ||
                    ((input->desense == IM_TYPE2_DESENSE) &&
                     (input->victim_rxpower <= blank_lut->tbl[index].victim_rxpwr_imtype2_thresh)) ) )
              {
                output->action = ACTION_BLANK;
                break;    
              }
            }
            else if ((blank_lut->tbl[index].victim_txpwr_thresh < blank_lut->tbl[index-1].victim_txpwr_thresh))
            {
              /* Previous row has higher threshold of victim Tx power */

              if( input->aggr_txpower >= blank_lut->tbl[index].aggr_txpwr_thresh &&
                  input->victim_txpower >= blank_lut->tbl[index].victim_txpwr_thresh && 
                  input->victim_txpower < blank_lut->tbl[index-1].victim_txpwr_thresh && 
                  ( ((input->desense == IM_TYPE1_DESENSE) &&
                     (input->victim_rxpower <= blank_lut->tbl[index].victim_rxpwr_imtype1_thresh)) ||
                    ((input->desense == IM_TYPE2_DESENSE) &&
                     (input->victim_rxpower <= blank_lut->tbl[index].victim_rxpwr_imtype2_thresh)) ) )
              {
                output->action = ACTION_BLANK;
                break;   
              }
            }
            else
            {
              /* Should not reach here... This means table not validated correctly */
              LMTSMGR_MSG_4(ERROR,"Incorrect IM Table for vtech %d, vband %d, atech %d, aband %d",
                            input->vtech, input->vband, input->atech, input->aband);
              output->action = ACTION_NONE;
              break;
            }
          }
        }
      }
    }
    break;

    default:
    {
      LMTSMGR_MSG_3(FATAL, "Incorrect action type: %d, vtech: %d, atech: %d",
                                       input->action, input->vtech, input->atech);
    }
    break;

  }

  return TRUE;
  
#else /* MCS_LIMITS_LOW_MEMORY */
  
  WWCOEX_DUMMY_CHECK(input);
  WWCOEX_DUMMY_CHECK(output);
  WWCOEX_DUMMY_CHECK(luts);
  WWCOEX_DUMMY_CHECK(lut_index);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}
/*=============================================================================

  FUNCTION:  wwcoex_desense_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for Desense thresholds.
 
    @return
    NONE - Victim is not desensed
    UNKNOWN - Power is unknown so accurate assesment cannot be made
    BLANK - Victim is desensed, so blanking needs to be applied
*/
/*===========================================================================*/
extern boolean wwcoex_desense_efs_lookup
(
  wwcoex_blank_efs_lookup_in*  input,
  wwan_desense_efs_lookup_out* output
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  wwan_coex_tech_comb_lut_type* tables;
  wwan_coex_lut_type* luts;

  wwan_coex_desense_lut_type*      harmonics_lut;
  wwan_coex_desense_lut_type*      rxbn_lut;
  wwan_coex_desense_lut_type*      blocking_lut;

  uint32 index = 0;
  boolean retval = FALSE;

  /* Check input/output ptr*/
  if (input == NULL)
  {
    return retval;
  }

  if ((input->vtech >= CXM_TECH_MAX) || (input->atech >= CXM_TECH_MAX))
  {
    return retval;
  }

  /* Initialize output values */
  output->action = ACTION_NONE;
  output->aplimit_valid = FALSE;

  /* Check if tables exist */
  if ( wwan_coex_tables[input->vtech] == NULL  || 
       wwan_coex_tables[input->vtech][input->atech] == NULL )
  {
    return retval;
  }
  tables = wwan_coex_tables[input->vtech][input->atech];

  /* check bands */
  if ( (input->vband >= tables->num_tech1_bands) || 
       (input->aband >= tables->num_tech2_bands) )
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect band %d %d", input->vband, input->aband);
    return retval;
  }

  index = input->vband * tables->num_tech2_bands + input->aband;
  luts = (wwan_coex_lut_type *)(tables->luts);

  switch (input->desense)
  {
    case NOISE_DESENSE:
    {
      rxbn_lut = (luts + index)->rxbn_lut;
      if (rxbn_lut != NULL)
      {
        /* Call Non-IM EFS lookup */
        retval = wwcoex_desense_nonim_efs_lookup(input->victim_rxpower,
                                       input->aggr_txpower, output,
                                       rxbn_lut, input->action);
      }
    }
    break;

    case HARMONIC_DESENSE:
    {
      harmonics_lut = (luts + index)->harmonics_lut;
      if (harmonics_lut != NULL)
      {
        /* Call Non-IM EFS lookup */
        retval = wwcoex_desense_nonim_efs_lookup(input->victim_rxpower,
                                       input->aggr_txpower, output,
                                       harmonics_lut, input->action);
      }
    }
    break;

    case BLOCKING_DESENSE:
    {
      blocking_lut = (luts + index)->blocking_lut;
      if (blocking_lut != NULL)
      {
        /* Call Non-IM EFS lookup */
        retval = wwcoex_desense_nonim_efs_lookup(input->victim_rxpower,
                                       input->aggr_txpower, output,
                                       blocking_lut, input->action);
      }
    }
    break;

    case IM_TYPE1_DESENSE:
    case IM_TYPE2_DESENSE:
    case IM_DESENSE_GPS:
    {
      if (luts != NULL)
      {
        /* Call IM EFS lookup */
        retval = wwcoex_desense_im_efs_lookup(input, output,
                                               luts, index);
      }
    }
    break;

    case NO_DESENSE:
    case SPURIOUS_EMISSIONS:
    default:
       LMTSMGR_MSG_3(FATAL, "Incorrect desense type %d in table for techs %d %d",
                 input->desense, input->vtech, input->atech);
       retval = FALSE;
       break;
  }
  
  return retval;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(input);
  WWCOEX_DUMMY_CHECK(output);
  return FALSE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */
}

/*=============================================================================

  FUNCTION:  wwcoex_emission_efs_lookup

=============================================================================*/
/*!
    @brief
    Checks the efs lookup tables for emission thresholds.
 
    @return
    NO_DESENSE - No emissions issue
    SPURIOUS_EMISSIONS - emission problem

*/
/*===========================================================================*/
extern cxm_action_type wwcoex_emission_efs_lookup
(
  wwcoex_sem_lookup_in       input
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  wwan_coex_tech_comb_lut_type* tables;
  wwan_coex_lut_type* luts;
  wwan_coex_desense_lut_type*      sem_lut;
  cxm_action_type   action = ACTION_NONE;
  uint32 index = 0, temp;
  int16 temp_txpwr;
  int i;

  if ((input.tech1 >= CXM_TECH_MAX) || (input.tech2 >= CXM_TECH_MAX))
  {
    return action;
  }

  /* Check if tables exist */
  if ( wwan_coex_tables[input.tech1][input.tech2] == NULL )
  {
    if (wwan_coex_tables[input.tech2][input.tech1] != NULL)
    {
      /* Swap tech2, tech1 */
      temp = (uint32)input.tech2;
      input.tech2 = input.tech1;
      input.tech1 = (cxm_tech_type)temp;

      temp = input.t1band;
      input.t1band = input.t2band;
      input.t2band = temp;

      temp_txpwr = input.t1_txpwr;
      input.t1_txpwr = input.t2_txpwr;
      input.t2_txpwr = temp_txpwr;
    }
    else
    {
      return action;
    }
  }

  tables = wwan_coex_tables[input.tech1][input.tech2];

  /* check bands */
  if ( (input.t1band >= tables->num_tech1_bands) || 
       (input.t2band >= tables->num_tech2_bands) )
  {
    LMTSMGR_MSG_2(ERROR,"Incorrect band %d %d", input.t1band, input.t2band);
    return action;
  }

  index = input.t1band * tables->num_tech2_bands + input.t2band;
  luts = (wwan_coex_lut_type *)(tables->luts);

  sem_lut = (luts + index)->sem_lut;
  
  if (sem_lut == NULL)
  {
    /* There is no LUT for this band comvbination */
    action = ACTION_NONE;
  }
  /* Check if we have UNKNOWN power values... */
  else if (input.t1_txpwr == CXM_UNKNOWN_POWER || 
           input.t2_txpwr == CXM_UNKNOWN_POWER)
  {
    action = ACTION_UNKNOWN;
  }
  else
  {
    /* Check if there are any entries in the LUT */
    if (sem_lut->num_valid_lut_rows == 0)
    {
      action = ACTION_NONE;
    }
    /* Check the first entry */
    else if( input.t1_txpwr >= sem_lut->aggressor_limits[0][0] && 
             input.t2_txpwr >= sem_lut->aggressor_limits[0][1] )
    {
      action = ACTION_BLANK;
    } 
    else
    {
      /* Check if any other entry matches... */
      for (i=1; i < sem_lut->num_valid_lut_rows; i++)
      {
        if( input.t1_txpwr >= sem_lut->aggressor_limits[i][0] && 
            input.t1_txpwr < sem_lut->aggressor_limits[i-1][0] &&
            input.t2_txpwr >= sem_lut->aggressor_limits[i][1])
        {
          action = ACTION_BLANK;
          break;
        }
      }
    }
  }

  return action;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(&input);

  return ACTION_NONE;

#endif /* MCS_LIMITS_LOW_MEMORY */
}

/*=============================================================================

  FUNCTION:  wwan_ddr_efs_lookup_gps

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the DDR plan specific to GPS
 
    @return
    TRUE - EFS entry for GPS DDR coex valid; ddr_freq_plan set to value.
    FALSE - Invalid EFS entry for GPS DDR coex; ddr_freq_plan value unknown.
*/
/*===========================================================================*/
boolean wwan_ddr_efs_lookup_gps
(
  uint32* ddr_freq_plan
)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  if (wwan_ddr_coex_gps_record.is_valid == TRUE)
  {
     *ddr_freq_plan = wwan_ddr_coex_gps_record.ddr_freq;
     return TRUE;
  }
  else
  {
#endif
     return FALSE;
#ifdef FEATURE_MODEM_DDR_MGMT
  }
#endif
}



/*=============================================================================

  FUNCTION:  wwan_ddr_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the DDR plan for the band.
 
    @return
    None
*/
/*===========================================================================*/
boolean wwan_ddr_efs_lookup
(
  wwan_ddr_efs_lookup_in* input,
  wwan_ddr_efs_lookup_out* output
)
{

#ifdef FEATURE_MODEM_DDR_MGMT
  wwan_ddr_coex_tech_lut_type* tables = NULL;
  wwcoex_ddr_desense_lut_type* ddr_lut = NULL;
#endif

  /* Check input/output ptr*/
  if (input == NULL || output == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Invalid pointers.");
    return FALSE;
  }

#ifdef FEATURE_MODEM_DDR_MGMT
  if (input->tech >= CXM_TECH_MAX)
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech input %d", input->tech);
    return FALSE;
  }

  /* Check if tables exist */
  tables = wwan_ddr_coex_tables[input->tech][input->carrier];

  if (tables == NULL)
  {
    LMTSMGR_MSG_2(LOW, "DDR Coex Tables Do Not Exist for Tech %d Carrier %d.",
                         input->tech,input->carrier);
    return FALSE;
  }

  /* check bands */
  if (input->band >= tables->num_tech_bands)
  {
    LMTSMGR_MSG_1(ERROR, "Invalid band input %d", input->band);
    return FALSE;
  }

  /* check if valid chain (primary/diversity) */
  if (input->chain >= COEX_DDR_TABLE_ENTRY_MAX)
  {
    LMTSMGR_MSG_1(ERROR, "Invalid chain input %d", input->chain);
    return FALSE;
  }

  /* Get the pointer to the lut list for the specific band combination */
  ddr_lut = ((wwcoex_ddr_desense_lut_type*)(tables->luts) + input->band);

  if (tables->default_ddr_freq == DEFAULT_DDR_FREQ)
  {
    LMTSMGR_MSG_0(LOW,"Tables set to default DDR Frequency.");
    return FALSE;
  }

  output->default_ddr_freq  = tables->default_ddr_freq;
  if (ddr_lut != NULL)
  {
    output->channel_min = ddr_lut->channel_min;
    output->channel_max = ddr_lut->channel_max;
    output->band_spec_plan_valid = ddr_lut->ddr_threshold_table[input->chain].is_valid;
    output->rxpower_threshold = ddr_lut->ddr_threshold_table[input->chain].rxpower_threshold;
    output->upper_threshold   = ddr_lut->ddr_threshold_table[input->chain].upper_threshold;
    output->lower_threshold   = ddr_lut->ddr_threshold_table[input->chain].lower_threshold;
    output->ddr_freq          = ddr_lut->ddr_threshold_table[input->chain].ddr_freq;
  }

  LMTSMGR_MSG_8(MED, "DDR lookup o/p valid %d, chan %d-%d, ddr_def %d, Rx_thrshd %d, upr_ths %d, lwr_ths %d, ddr_fq %d", 
        output->band_spec_plan_valid, 
                output->channel_min, output->channel_max,
        output->default_ddr_freq, output->rxpower_threshold,
                output->upper_threshold, 
                output->lower_threshold,
                output->ddr_freq);

  return TRUE;  
#else
return FALSE;
#endif /* DDR */
}

/*=============================================================================

  FUNCTION:  wwcoex_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all the memory associated with LUTs
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_efs_deinit( void )
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
}

