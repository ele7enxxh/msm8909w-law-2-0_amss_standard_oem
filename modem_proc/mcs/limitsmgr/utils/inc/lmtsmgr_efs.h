#ifndef __LMTSMGR_EFS_H__
#define __LMTSMGR_EFS_H__
/*!
  @file lmtsmgr_efs.h

  @brief
   EFS Item support for the LMTSMGR EFS

*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/inc/lmtsmgr_efs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/02/15   sg      EFS based spur simulation support
09/09/14   rj      Merged non-IM BackOff and Blanking EFS
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
07/17/14   sg      efs based coex simulation support
07/04/14   ag      data+mms support
04/28/14   jm      L+G Band Avoidance support
03/15/13   jm      L+G Blanking Support
12/19/13   jm      DDR management phase 2 support (diversity power + GPS)
11/21/13   jm      Adding DDR management support
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
05/20/13   rj      mcs_limitsmgr.conf EFS file optimization
08/24/12   ag      EFS Restructuring
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "lmtsmgr_i.h"
#include "rfcom.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define LMTSMGR_CONF_EFS_FILE  "/nv/item_files/conf/mcs_limitsmgr.conf"

/* SAR related EFS files */
#define LIMITSMGR_PWR_LTE_HIGH_GSM1_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_lte_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM1_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_lte_high_gsm1_low"

#define LIMITSMGR_PWR_LTE_HIGH_GSM2_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_lte_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM2_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_lte_high_gsm2_low"

#define LIMITSMGR_PWR_TDSCDMA_HIGH_GSM1_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_tdscdma_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM1_LOW       "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_tdscdma_high_gsm1_low"

#define LIMITSMGR_PWR_TDSCDMA_HIGH_GSM2_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_tdscdma_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM2_LOW       "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_tdscdma_high_gsm2_low"

#define LIMITSMGR_PWR_GSM1_HIGH_LTE_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_lte_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_LTE_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_lte_low"

#define LIMITSMGR_PWR_GSM1_HIGH_TDSCDMA_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_tdscdma_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_TDSCDMA_LOW       "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_tdscdma_low"

#define LIMITSMGR_PWR_GSM1_HIGH_1X_LOW                 "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_1x_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_1X_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_1x_low"

#define LIMITSMGR_PWR_GSM1_HIGH_WCDMA_LOW              "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_wcdma_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_WCDMA_LOW         "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_wcdma_low"

#define LIMITSMGR_PWR_GSM1_HIGH_HDR_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_hdr_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_HDR_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_hdr_low"

#define LIMITSMGR_PWR_GSM1_HIGH_GSM2_LOW               "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm1_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_GSM1_HIGH_GSM2_LOW          "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm1_high_gsm2_low"

#define LIMITSMGR_PWR_1X_HIGH_LTE_LOW                  "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_1x_high_lte_low"
#define LIMITSMGR_DSI_BIAS_1X_HIGH_LTE_LOW             "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_1x_high_lte_low"

#define LIMITSMGR_PWR_1X_HIGH_GSM1_LOW                 "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_1x_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_1X_HIGH_GSM1_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_1x_high_gsm1_low"

#define LIMITSMGR_PWR_1X_HIGH_HDR_LOW                  "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_1x_high_hdr_low"
#define LIMITSMGR_DSI_BIAS_1X_HIGH_HDR_LOW             "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_1x_high_hdr_low"

#define LIMITSMGR_PWR_1X_HIGH_GSM2_LOW                 "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_1x_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_1X_HIGH_GSM2_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_1x_high_gsm2_low"

#define LIMITSMGR_PWR_HDR_HIGH_GSM1_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_hdr_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM1_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_hdr_high_gsm1_low"

#define LIMITSMGR_PWR_HDR_HIGH_GSM2_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_hdr_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM2_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_hdr_high_gsm2_low"

#define LIMITSMGR_PWR_WCDMA_HIGH_GSM1_LOW              "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_wcdma_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM1_LOW         "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_wcdma_high_gsm1_low"

#define LIMITSMGR_PWR_WCDMA_HIGH_GSM2_LOW              "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_wcdma_high_gsm2_low"
#define LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM2_LOW         "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_wcdma_high_gsm2_low"

#define LIMITSMGR_PWR_GSM2_HIGH_LTE_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_lte_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_LTE_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_lte_low"

#define LIMITSMGR_PWR_GSM2_HIGH_TDSCDMA_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_tdscdma_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_TDSCDMA_LOW       "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_tdscdma_low"

#define LIMITSMGR_PWR_GSM2_HIGH_GSM1_LOW               "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_gsm1_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_GSM1_LOW          "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_gsm1_low"

#define LIMITSMGR_PWR_GSM2_HIGH_1X_LOW                 "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_1x_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_1X_LOW            "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_1x_low"

#define LIMITSMGR_PWR_GSM2_HIGH_HDR_LOW                "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_hdr_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_HDR_LOW           "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_hdr_low"

#define LIMITSMGR_PWR_GSM2_HIGH_WCDMA_LOW              "/nv/item_files/mcs/lmtsmgr/sar/sar_pwr_gsm2_high_wcdma_low"
#define LIMITSMGR_DSI_BIAS_GSM2_HIGH_WCDMA_LOW         "/nv/item_files/mcs/lmtsmgr/sar/sar_dsi_bias_gsm2_high_wcdma_low"

/* Power Backoff IM COEX EFS Files */
#define LIMITSMGR_COEX_BACKOFF_1X_VICTIM_LTE_AGGR      "/nv/item_files/mcs/lmtsmgr/coex/desense/im_backoff_1x_victim_lte_aggr"
#define LIMITSMGR_COEX_BACKOFF_1X_VICTIM_HDR_AGGR      "/nv/item_files/mcs/lmtsmgr/coex/desense/im_backoff_1x_victim_hdr_aggr"
#define LIMITSMGR_COEX_BACKOFF_HDR_VICTIM_1X_AGGR      "/nv/item_files/mcs/lmtsmgr/coex/desense/im_backoff_hdr_victim_1x_aggr"
#define LIMITSMGR_COEX_BACKOFF_LTE_VICTIM_1X_AGGR      "/nv/item_files/mcs/lmtsmgr/coex/desense/im_backoff_lte_victim_1x_aggr"

#define LIMITSMGR_COEX_DESENSE_GPS_VICTIM_1X_LTE_AGGR  "/nv/item_files/mcs/lmtsmgr/coex/desense/gps_victim_1x_lte_aggr"

/* Power Backoff/Blank merged COEX EFS Files */
#define LIMITSMGR_COEX_DESENSE_1X_VICTIM_HDR_AGGR             "/nv/item_files/mcs/lmtsmgr/coex/desense/1x_victim_hdr_aggr"
#define LIMITSMGR_COEX_DESENSE_1X_VICTIM_LTE_AGGR             "/nv/item_files/mcs/lmtsmgr/coex/desense/1x_victim_lte_aggr"
#define LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR            "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm_victim_lte_aggr"
#define LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm_victim_tdscdma_aggr"

#define LIMITSMGR_COEX_SEM_THRESH_GSM_TDSCDMA             "/nv/item_files/mcs/lmtsmgr/sem/sem_thresh_gsm_tdscdma"


#define LIMITSMGR_COEX_DESENSE_GSM_VICTIM_1X_AGGR           "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm_victim_1x_aggr"
#define LIMITSMGR_COEX_SEM_THRESH_GSM_1X                  "/nv/item_files/mcs/lmtsmgr/sem/sem_thresh_gsm_1x"

#define LIMITSMGR_COEX_DESENSE_GSM_VICTIM_HDR_AGGR          "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm_victim_hdr_aggr"
#define LIMITSMGR_COEX_SEM_THRESH_GSM_HDR                 "/nv/item_files/mcs/lmtsmgr/sem/sem_thresh_gsm_hdr"

#define LIMITSMGR_COEX_DESENSE_GSM_VICTIM_WCDMA_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm_victim_wcdma_aggr"
#define LIMITSMGR_COEX_SEM_THRESH_GSM_WCDMA               "/nv/item_files/mcs/lmtsmgr/sem/sem_thresh_gsm_wcdma"

#define LIMITSMGR_COEX_DESENSE_1X_VICTIM_GSM_AGGR           "/nv/item_files/mcs/lmtsmgr/coex/desense/1x_victim_gsm_aggr"

#define LIMITSMGR_COEX_DESENSE_HDR_VICTIM_GSM_AGGR          "/nv/item_files/mcs/lmtsmgr/coex/desense/hdr_victim_gsm_aggr"

#define LIMITSMGR_COEX_DESENSE_WCDMA_VICTIM_GSM_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/wcdma_victim_gsm_aggr"

#define LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM1_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm2_victim_gsm1_aggr"

#define LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM2_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm1_victim_gsm2_aggr"

#define LIMITSMGR_COEX_DESENSE_TDSCDMA_VICTIM_GSM_AGGR      "/nv/item_files/mcs/lmtsmgr/coex/desense/tdscdma_victim_gsm_aggr"
#define LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM1_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm3_victim_gsm1_aggr"
#define LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM2_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm3_victim_gsm2_aggr" 
#define LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM3_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm1_victim_gsm3_aggr"  
#define LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM3_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/desense/gsm2_victim_gsm3_aggr"  
#define LIMITSMGR_COEX_DESENSE_LTE_VICTIM_GSM_AGGR          "/nv/item_files/mcs/lmtsmgr/coex/desense/lte_victim_gsm_aggr"

/* BCL related EFS files */
#define LIMITSMGR_BCL_GSM2_HIGH_LTE_LOW                   "/nv/item_files/mcs/lmtsmgr/battery/bcl_gsm2_high_lte_low"

#define LIMITSMGR_BCL_GSM2_HIGH_TDSCDMA_LOW               "/nv/item_files/mcs/lmtsmgr/battery/bcl_gsm2_high_tdscdma_low"

/* GPIO SAR related EFS files */
#define LIMITSMGR_SAR_DSI_GPIO_EFS_FILE                   "/nv/item_files/mcs/lmtsmgr/sar/sar_gpio_settings"

/*QMI SAR Compatibility related files*/
#define LIMITSMGR_SAR_COMPATIBILITY_KEY                   "/nv/item_files/mcs/lmtsmgr/sar/sar_qmi_comp_key"

#define LIMITSMGR_COEX_DISABLE_MITIGATION                 "/nv/item_files/mcs/lmtsmgr/coex/desense/disable_sw_mitigation" 

#define LIMITSMGR_COEX_ENABLE_MITIGATION                  "/nv/item_files/mcs/lmtsmgr/coex/ba/enable_sw_mitigation"

#define LIMITSMGR_COEX_PRIO_TABLE_UPD_ACTION              "/nv/item_files/mcs/lmtsmgr/coex/set_prio_tbl_action"

#define LIMITSMGR_COEX_DESENSE_IND_THRESHOLD              "/nv/item_files/mcs/lmtsmgr/coex/desense_ind_thresholds"

#define LIMITSMGR_COEX_SPUR_SIM_INPUT                     "/nv/item_files/mcs/lmtsmgr/coex/spur_sim_input"

#define LIMITSMGR_COEX_SIMULATION_INPUT                   "/nv/item_files/mcs/lmtsmgr/coex/coex_simulation_input"

/* EFS file to get IDLE Mode Hopping Rx power limit */
#define LIMITSMGR_COEX_IDLE_MODE_HOPPING_RXPWR            "/nv/item_files/mcs/lmtsmgr/coex/idle_mode_hopping_rxpwr_limit"

#ifdef FEATURE_MODEM_DDR_MGMT
/* DDR Coex related EFS files */
#define LIMITSMGR_DEFAULT_TECH_DDR_FREQ_PLAN              "/nv/item_files/mcs/lmtsmgr/coex/ddr/default_tech_ddr_freq_plan"
#define LIMITSMGR_COEX_DDR_LTE_VICTIM_DDR_AGGR            "/nv/item_files/mcs/lmtsmgr/coex/ddr/lte_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_TDSCDMA_VICTIM_DDR_AGGR        "/nv/item_files/mcs/lmtsmgr/coex/ddr/tdscdma_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_GSM1_VICTIM_DDR_AGGR           "/nv/item_files/mcs/lmtsmgr/coex/ddr/gsm1_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_1X_VICTIM_DDR_AGGR             "/nv/item_files/mcs/lmtsmgr/coex/ddr/1x_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_HDR_VICTIM_DDR_AGGR            "/nv/item_files/mcs/lmtsmgr/coex/ddr/hdr_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_WCDMA_VICTIM_DDR_AGGR          "/nv/item_files/mcs/lmtsmgr/coex/ddr/wcdma_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_GSM2_VICTIM_DDR_AGGR           "/nv/item_files/mcs/lmtsmgr/coex/ddr/gsm2_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_GSM3_VICTIM_DDR_AGGR           "/nv/item_files/mcs/lmtsmgr/coex/ddr/gsm3_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_LTE_SEC_CA_VICTIM_DDR_AGGR     "/nv/item_files/mcs/lmtsmgr/coex/ddr/lte_sec_ca_victim_ddr_aggr"
#define LIMITSMGR_COEX_DDR_GPS_VICTIM_DDR_AGGR            "/nv/item_files/mcs/lmtsmgr/coex/ddr/gps_victim_ddr_aggr"

 /* buffer for the list of file paths to be be written to the conf file */
#define LIMITSMGR_UPDATE_CONF_FILE_WITH_EFS_NAMES      LIMITSMGR_PWR_LTE_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_PWR_LTE_HIGH_GSM2_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM2_LOW "\n"\
                                                       LIMITSMGR_PWR_TDSCDMA_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_TDSCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_HDR_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_HDR_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_WCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_WCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_1X_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_1X_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_HDR_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_LTE_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_TDSCDMA "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_1X "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_HDR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_WCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_WCDMA "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_HDR_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_WCDMA_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM1_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM2_AGGR "\n" \
                                                       LIMITSMGR_BCL_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_BCL_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_SAR_DSI_GPIO_EFS_FILE "\n" \
                                                       LIMITSMGR_SAR_COMPATIBILITY_KEY "\n" \
                                                       LIMITSMGR_COEX_DISABLE_MITIGATION "\n" \
                                                       LIMITSMGR_COEX_ENABLE_MITIGATION "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM1_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM2_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM3_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM3_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_TDSCDMA_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_LTE_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GPS_VICTIM_1X_LTE_AGGR "\n" \
                                                       LIMITSMGR_DEFAULT_TECH_DDR_FREQ_PLAN "\n" \
                                                       LIMITSMGR_COEX_DDR_LTE_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_LTE_SEC_CA_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_TDSCDMA_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_GSM1_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_1X_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_HDR_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_WCDMA_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_GSM2_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_GSM3_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_DDR_GPS_VICTIM_DDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_PRIO_TABLE_UPD_ACTION "\n" \
                                                       LIMITSMGR_COEX_DESENSE_IND_THRESHOLD "\n" \
                                                       LIMITSMGR_COEX_IDLE_MODE_HOPPING_RXPWR "\n"
#else
 /* buffer for the list of file paths to be be written to the conf file */
#define LIMITSMGR_UPDATE_CONF_FILE_WITH_EFS_NAMES      LIMITSMGR_PWR_LTE_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_PWR_LTE_HIGH_GSM2_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_LTE_HIGH_GSM2_LOW "\n"\
                                                       LIMITSMGR_PWR_TDSCDMA_HIGH_GSM1_LOW "\n"\
                                                       LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_TDSCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_TDSCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM1_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM1_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_1X_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_1X_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_HDR_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_HDR_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_HDR_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_WCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_WCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_WCDMA_HIGH_GSM2_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_GSM1_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_1X_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_HDR_LOW "\n" \
                                                       LIMITSMGR_PWR_GSM2_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_DSI_BIAS_GSM2_HIGH_WCDMA_LOW "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_1X_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_1X_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_HDR_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_BACKOFF_LTE_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_TDSCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_TDSCDMA "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_1X_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_1X "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_HDR_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_HDR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_WCDMA_AGGR "\n" \
                                                       LIMITSMGR_COEX_SEM_THRESH_GSM_WCDMA "\n" \
                                                       LIMITSMGR_COEX_DESENSE_1X_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_HDR_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_WCDMA_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM1_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM2_AGGR "\n" \
                                                       LIMITSMGR_BCL_GSM2_HIGH_LTE_LOW "\n" \
                                                       LIMITSMGR_BCL_GSM2_HIGH_TDSCDMA_LOW "\n" \
                                                       LIMITSMGR_SAR_DSI_GPIO_EFS_FILE "\n" \
                                                       LIMITSMGR_SAR_COMPATIBILITY_KEY "\n" \
                                                       LIMITSMGR_COEX_DISABLE_MITIGATION "\n" \
                                                       LIMITSMGR_COEX_ENABLE_MITIGATION "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM1_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM3_VICTIM_GSM2_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM2_VICTIM_GSM3_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM1_VICTIM_GSM3_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_TDSCDMA_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GPS_VICTIM_1X_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_GSM_VICTIM_LTE_AGGR "\n" \
                                                       LIMITSMGR_COEX_DESENSE_LTE_VICTIM_GSM_AGGR "\n" \
                                                       LIMITSMGR_COEX_PRIO_TABLE_UPD_ACTION "\n" \
                                                       LIMITSMGR_COEX_DESENSE_IND_THRESHOLD "\n" \
                                                       LIMITSMGR_COEX_IDLE_MODE_HOPPING_RXPWR "\n"
#endif /* DDR */

/*=============================================================================

                             TYPEDEFS

=============================================================================*/



/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  lmtsmgr_efs_init

=============================================================================*/
/*!
    @brief
    Initializes LIMITSMGR EFS
 
    @return
    None
*/
/*===========================================================================*/
extern void lmtsmgr_efs_init (void);

/*=============================================================================

  FUNCTION:  lmtsmgr_efs_validate_tech_band

=============================================================================*/
/*!
    @brief
    Validates tech, band information 
    @return
    None
*/
/*===========================================================================*/
extern boolean lmtsmgr_efs_validate_tech_band 
(
  cxm_tech_type  tech,
  uint32         band
);

#endif /* __LMTSMGR_EFS_H__ */
