#ifndef TRM_STRUCT_H
#define TRM_STRUCT_H
/*===========================================================================

                   T R M    S T R U C T   H E A D E R    F I L E

DESCRIPTION
   This file contains the declaration of TRM data structure.

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
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
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_struct.h#1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
06/08/2015   sp      Changes for 7mode dynamic PBR bias (825586)
02/18/2015   sk      Update log v8 for eMBMS logging(796676)
01/23/2015   jm      Adding page request attempts to TRM metrics (CR: 778020)
01/21/2015   jm      Adding TRM metric query support (CR: 778020)
12/17/2014   mn      Removing unwanted files from TRM. (CR: 687777).
12/12/2014   sk      Logging Enhancement(737879)
10/13/2014   sr      Storing alt path information(728163)
10/08/2014   sk      Added support for cross WTR PBR(708194)
09/17/2014   sk      Support for Tx Sharing DSDA changes
07/24/2014   mn      TRM needs to keep change reason pending if it is not 
                      allowed immediately. (CR: 694927).
07/18/2014   mn      1xSRLTE:- Requirement to default PBR bias values fro 1x 
                      and SLTE in SW code (CR: 681568).
07/01/2014   sk      Added support for SIM swap without reset(665739) 
06/26/2014   mn      NV control to enable/disable DR-DSDS (CR: 685806)
06/24/2014   mn      Adding support for num pending callbacks in req and not,
                      req and not enhanced and unlock callbacks. (CR: 684638).
04/11/2014   mn      Refactoring TRM

===========================================================================*/

#include "customer.h"
#include "trm_client.h"
#include "trm_client_array.h"
#include "trmlog.h"
#include "trm_band_handler.h"

#define TRM_CLIENT_SEARCH_ACTIVITY_Q_SIZE                           35
#define TRM_METRICS_MAX_TBL_SIZE                                    50
#define TRM_MAX_METRIC_MASK_SIZE                                     8

/*===========================================================================

                              HEADER CONSTANTS

  Numeric values for the identifiers in this section should not be changed.

===========================================================================*/

/*----------------------------------------------------------------------------
  TRM Initialization State
----------------------------------------------------------------------------*/

enum trm_init_status
{
  /* TRM init has not been called */
  TRM_NOT_INITIALIZED,

  /* TRM init is in progress (waiting for NV read?) */
  TRM_INITIALIZING,

  /* TRM init has completed */
  TRM_INITIALIZED
};

/*----------------------------------------------------------------------------
  RF h/w capabilities
----------------------------------------------------------------------------*/
struct trm_rf_hw_struct
{
  /* Configuration of all RF devices  */
  rfm_devices_configuration_type  config;

  /* RF configuration (single, dual dependent/independent, ... ) */
  trm_rf_enum_t             rf_type;

  /* Number of antenna */
  uint8                     num_ant;

  /* bit mask to determine capability of RF. We are reusing the 
     configuration from get simultaneous capability which uses SHDR for
     bit 0. It is no-op here 
     Bit0       : PAM
     Bit1-SVDO  : TRM_SVDO_IS_ENABLED
     Bit2-SVLTE : TRM_SVLTE_IS_ENABLED
     Bit3       : Reserved.
     Bit4-DSDA  : TRM_DSDA_IS_ENABLED
     Bit 5      : TRM_SGLTE_SGTDS_IS_ENABLED
  */
  uint32                     capability;

  /* Dual receive capability 
      bit 0: SLTE_1X
      bit 1: SLTE_GSM
      bit 2: DR_DSDS 
  */
  uint32                    dual_receive_capability;
};

/*----------------------------------------------------------------------------
  RF Chain structure type
----------------------------------------------------------------------------*/

struct trm_chain_struct
{
  /* Current owner of the RF chain */
  trm_client_enum_t         owner;

  /* Time when the chain would be available */
  int32                     available;

  /* Alt Path Information Stored from RF */
  uint8                     rf_alt_path;

};

/*----------------------------------------------------------------------------
  tracks data w.r.t first and second chain/group to try for the local run
  based on local prediction.Please note that this is different to primary and
  secondary configurable chains. This is filled based on the global chain
  data and the scheduler run time assignment data. 
----------------------------------------------------------------------------*/
typedef struct
{
  trm_group first_group;
  trm_group second_group;
  trm_chain_enum first_chain;
  trm_chain_enum second_chain;
} trm_any_bp_bookkeeper_type;

/*----------------------------------------------------------------------------
  Per RAT information.
----------------------------------------------------------------------------*/
typedef struct
{
  /* Number of grant callbacks pending for a given RAT */
  uint8            num_pending_req_and_not_cbs;

  /* Number of grant enhanced callbacks pending for a given RAT */
  uint8            num_pending_req_and_not_enh_cbs;

  /* Number of unlock callbacks pending for a given RAT */
  uint8            num_pending_unlock_cbs;
} trm_per_rat_info_type;

/*----------------------------------------------------------------------------
  Current and new Dual Receive Information.
----------------------------------------------------------------------------*/
typedef struct
{
  /* The current vote enforced by TRM */
  trm_dual_receive_enum_type curr_info;

  /* The new vote to be enforced by TRM */
  trm_dual_receive_enum_type new_info;

  /* Flag to indicate that async callback is pending */
  boolean                    async_cb_pending;

  /* The client enforcing the dual receive restriction */
  trm_client_enum_t          client_id;
} trm_dual_receive_info_type;

/*----------------------------------------------------------------------------
  TRM NV Refresh struct type
----------------------------------------------------------------------------*/
typedef struct
{
   /* indicates whether refresh logic is enabled */
   boolean  is_enabled;

   /* indicates if NV update is required */
   boolean  is_refresh_needed;

}trm_nv_refresh_struct_type;


typedef struct
{
  /* 
     Identifier for associated counter.
   
     Combination is packed as follows for denials:
     - 8 MSB for Losing Client (trm_client_enum_t)
     - 8 bits for Losing Client Reason (trm_reason_enum_t)
     - 8 bits for Winning Client (trm_client_enum_t) 
     - 8 LSB for Winning Client Reason (trm_reason_enum_t)
   
     Combination is packed as follows for requests:
     - 8 bits for requesting client (trm_client_enum_t)
     - 8 LSB for requesting client reason (trm_reason_enum_t)
   */ 
  uint32 combination;

  /* Associated counter for this given winning/losing client combo */
  uint16 counter;

}trm_metrics_entry_type;

/*----------------------------------------------------------------------------
  TRM Metrics Info struct type
----------------------------------------------------------------------------*/
typedef struct
{
  /* Current size of denial table */
  uint8                   denial_tbl_size;

  /* Current size of attempt table */
  uint8                   attempt_tbl_size;

  /* Table which holds denial count based
     off of combination of winning/losing client */
  trm_metrics_entry_type  denials[TRM_METRICS_MAX_TBL_SIZE];

  /* Table which holds attempts based on
     requesting client/reason */
  trm_metrics_entry_type  attempts[TRM_METRICS_MAX_TBL_SIZE];

}trm_metrics_info_type;

/*----------------------------------------------------------------------------
  TRM Metrics Mapping Mask Info Type
----------------------------------------------------------------------------*/
typedef struct
{
  uint32            mask[TRM_MAX_METRIC_MASK_SIZE];
  uint8             size;
  trm_metric_type_t metric_type;
}trm_metric_mapping_mask_info_type;

/*----------------------------------------------------------------------------
  TRM data storage structure
----------------------------------------------------------------------------*/

typedef struct
{
  /* Initialization state of TRM */
  trm_init_status           init_status;

  /* RF configuration (single, dual dependent/independent, ... ) */
  trm_rf_enum_t             rf_type;

  /* TRM Config mask (from NV) */
  uint32                    mask;

  /* Actual rf h/w capability information */
  trm_rf_hw_struct          rf_hw;

  /* All clients of the Transceiver Resource Manager */
  TRMClient*                client;

  /* All RF chains */
  trm_chain_struct          chain[ TRM_MAX_CHAINS ];

  /* Number of available RF chains */
  uint32                    max_num_chains;

  /* Number of available RF chains */
  uint32                    max_groups;

  /* Priority order sorted list of request clients */
  TRMClientOrderArray       order;

  /* Array of conflicting clients */
  TRMConflictArray          conflicts;

  /* The earliest lock request times for each RF group */
  trm_earliest_usage_info_type earliest[TRM_MAX_CHAINS];

  /* Lock extension data */
  trm_extension_struct      extend;

  /* Critical section - for initialization serialization, etc. */
  rex_crit_sect_type        crit_sect;

  /* Critical section - for extension APIs and trm_get_simult_cap */
  rex_crit_sect_type        extns_flag_crit_sect;

  /* Array having the priority inversion status */
  boolean                   priority_inv_status[TRM_REASON_MAX];

  /* flag to store information on if pam is enabled in nv item */
  boolean                   is_pam_enabled;

  boolean                   gps_has_independent_receive_chain;

  /* Variable to store slte information
    Bit 0: 1X SLTE is enabled
    Bit 1: GSM SLTE is enabled 
    Bit 3: Dual Receive DSDS is enabled 
  */
  trm_slte_bitmask          dr_mask;

  /* Chain 1 can be given to two clients - this flag keeps stores info 
     on gps client if more than one client has chain 1. */
  boolean                   gps_on_chain2_status;

  /* bit mask to determine modes enabled with NV. We are reusing the 
     configuration from get simultaneous capability which uses SHDR for
     bit 0. It is no-op here
     bit 0 : None
     bit 1 : SVLTE
     bit 2 : SVDO
   */
  uint32                    modes_enabled;

  trm_rf_mode_map_type      rf_map[TRM_MAX_CLIENTS];

  /* This data is maintained throughout the life-time for a given
     scheduler run */
  boolean                   second_config_assigned;
  boolean                   first_config_assigned;

  /* Array storage for extension flags, one bitmap per reason */
  uint8                     extension_flags[TRM_REASON_MAX];

  /* Specifies the SMODE device for DSDA mode */
  trm_chain_enum            smode_chain;

  /* Subscription capability tech mapping as per multimode subs */
  trm_tech_map_table_t      tech_asid_map;

  /* PBR efs settings data from the EFS */
  trm_pbr_efs_data_type     pbr_init_settings;

  /* TRM RF config */
  uint32                    rf_config;

  trm_tech_state_update_callback_type   lmtsmgr_cb;

  trm_hopping_config_info_type hop_config;

  /* Previous chain owner state */
  trm_chain_struct          last_chain[ TRM_MAX_CHAINS ];

  trm_any_bp_bookkeeper_type any_bp_bookkeeper;

  trm_vco_coupling_info_type vco_coupling_info[TRM_NUM_CHAIN_PAIRS];

  trm_log_struct_type        log_struct;

  TRMBandHandler             band_handler;

  trm_dual_receive_info_type dual_receive_info;

  /* feature list: list of techs with active feature */
  TRMFeatureArray            feat_info;

  /* temporary variables, should be cleaned after testing phase */
  boolean                    is_qta_logic_enabled;
  boolean                    is_aps_logic_enabled;

  /* Queue to hold free command buffers. */
  q_type                               search_activity_free_q;

  /* QUEUE Buffers. */
  trm_client_search_activity_data_type search_activity_q_buffer[TRM_CLIENT_SEARCH_ACTIVITY_Q_SIZE];

  /* Information maintained per RAT */
  trm_per_rat_info_type                per_rat_info[TRM_RAT_GROUP_MAX];

  /* nv refresh info */
  trm_nv_refresh_struct_type           nv_refresh_info;

  /* default PBR bias needed flag */
  boolean                              default_pbr_bias_needed;

  /* dynamic PBR bias needed flag */
  boolean                              dynamic_pbr_bias_needed;

  /* ensure that PBR update happens only once after scheduler runs */
  boolean                   pbr_need_update;

  /* en/disable single TX dSDA
     temporary till RF has the support */
  boolean                              is_tx_sharing_enabled;

  /* Collection of performance metrics related to TRM */
  trm_metrics_info_type                metrics;

  /* Array of combination masks that map to the metric enum type */
  trm_metric_mapping_mask_info_type    metric_enum_masks[TRM_METRIC_MAX];

} trm_struct;

#endif /* TRM_STRUCT_H */
