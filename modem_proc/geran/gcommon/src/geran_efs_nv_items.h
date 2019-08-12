#ifndef GERAN_EFS_NV_ITEMS_H
#define GERAN_EFS_NV_ITEMS_H

/*============================================================================
  @file geran_efs_nv_items.h

  @brief This header file contains a list of the GERAN EFS-NV items

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/geran_efs_nv_items.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

const char* const geran_efs_nv_items[] =
{
 "/nv/item_files/gsm/gl1/search_all_w_cell",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_en",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_mdm_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_mdm_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_sens",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_sens",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_large_delta",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_small_delta",
  "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_hysteriesis_time",
  "/nv/item_files/gsm/gl1/antenna_switch_tx_div_min_delta_btw_antenna",
  "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_threshold",
  "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_enable",
  "/nv/item_files/gsm/gl1/gsm_rx_diversity",
  "/nv/item_files/gsm/gl1/gsm_commercial_recovery_restart",
  "/nv/item_files/gsm/l1/l1_debug",
  "/nv/item_files/gsm/l1/l1_q6_clock",
  "/nv/item_files/modem/geran/gfw_debug",
  "/nv/item_files/modem/geran/vamos_support",
  "/nv/item_files/modem/geran/gfw_diag_group1",
  "/nv/item_files/modem/geran/gfw_diag_group2",
  "/nv/item_files/modem/geran/gfw_diag_group3",
  "/nv/item_files/modem/geran/gfw_diag_group4",
  "/nv/item_files/modem/geran/grr/g2l_blind_redir_control",
  "/nv/item_files/modem/geran/g2x_ta_support",
  "/nv/item_files/modem/geran/gfw_aeq_phase4_control",
  "/nv/item_files/gsm/gl1/lif_feature_control"
};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#endif /* GERAN_EFS_NV_ITEMS_H */

/* EOF */

