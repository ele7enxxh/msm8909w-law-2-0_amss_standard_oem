#ifndef _POLICYMAN_DIAG_H_
#define _POLICYMAN_DIAG_H_

/**
  @file policyman_diag.h

  @brief Policy Manager diag message definitions.
*/

/*
    Copyright (c) 2014,2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_diag.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include <sys/types.h>
#include "comdef.h"    /* Definition for basic types and macros */
#include "log.h"
#include "event_defs.h"
#include "log_codes.h"

#include "mre_diag.h"
#include "policyman_i.h"
#include "sys.h"
#include "cm.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

/*---------------------------------------------------------------------------
  EVENT_PM_RAT_CHANGE structure
---------------------------------------------------------------------------*/

// Event definition EVENT_PM_RAT_CHANGE  0xA72==2674
// Comes from: core/api/services/event_defs.h
//#define EVENT_PM_RAT_CHANGE  0xA72

typedef PACK(struct) {
  uint32   ratMask;
  int8     subs;     // maps to sys_modem_as_id_e_type
} policyman_diag_event_rat_cap_t;

/*---------------------------------------------------------------------------
  LOG_PM_CONFIG_INFO structure
---------------------------------------------------------------------------*/
#define LOG_PM_CONFIG_INFO_LOG_PACKET_VERSION   2

// Log definition LOG_PM_CONFIG_INFO_C
// Comes from: core/api/services/log_codes.h
//#define LOG_PM_CONFIG_INFO_C   0x18A9

typedef PACK(struct) {
  uint8  subs_feature;         // sys_subs_feature_t
  uint8  reserved_1;
  uint8  reserved_2;
  uint8  reserved_3;
  uint32 rat_info;
} policyman_diag_subs_cfg_info_t;

// Define log record: LOG_PM_CONFIG_INFO_type
typedef PACK(struct) {
  log_hdr_type                    hdr;             // Required first, do not access directly (192 bits)
  /* 64-bit align */
  uint32                          version;         // Structure version, required first by diag after header
  uint32                          changed_mask;
  /* 64-bit align */
  uint8                           num_sim_cfg;     // MSB: cfgs:4
                                                   // LSB: sims:4
  uint8                           max_active;      // MSB: data:4
                                                   // LSB: voice:4
  uint8                           modified;        // MSB: reserved:4
                                                   //  |   max_data:1
                                                   //  |   feature:1
                                                   //  |   subs:1
                                                   // LSB: sims:1
  uint8                           specialization;  // sys_specialization_t
  uint32                          lte_band_count;  // number of lte band groups filled in below (1 or 4)
  /* 64-bit align */
  uint64                          hw_gw_bands;     // sys_band_mask_type
  uint64                          hw_lte_bands_0;  // sys_band_mask_type
  uint64                          hw_lte_bands_1;  // sys_band_mask_type
  uint64                          hw_lte_bands_2;  // sys_band_mask_type
  uint64                          hw_lte_bands_3;  // sys_band_mask_type
  uint64                          hw_tds_bands;    // sys_band_mask_type
  uint32                          hw_rat_mask;
  policyman_diag_subs_cfg_info_t  subs_cfg_info[2];
} LOG_PM_CONFIG_INFO_type;


/*---------------------------------------------------------------------------
  LOG_PM_SUBS_INFO structure
---------------------------------------------------------------------------*/
#define LOG_PM_SUBS_INFO_LOG_PACKET_VERSION   2

// Log definition LOG_PM_SUBS_INFO_C
// Comes from: core/api/services/log_codes.h
//#define LOG_PM_SUBS_INFO     0x18AA

// Mask for LOG_PM_SUBS_INFO:changed_mask
#define PM_SUBS_INFO_CHANGED_ITEM_1_MASK      BM(POLICYMAN_ITEM_DEVICE_CONFIGURATION)
#define PM_SUBS_INFO_CHANGED_ITEM_2_MASK      BM(POLICYMAN_ITEM_RAT_CAPABILITY)
#define PM_SUBS_INFO_CHANGED_ITEM_3_MASK      BM(POLICYMAN_ITEM_RF_BANDS)
#define PM_SUBS_INFO_CHANGED_ITEM_4_MASK      BM(POLICYMAN_ITEM_SVC_MODE)
#define PM_SUBS_INFO_CHANGED_ITEM_5_MASK      BM(POLICYMAN_ITEM_UE_MODE)
#define PM_SUBS_INFO_CHANGED_ITEM_6_MASK      BM(POLICYMAN_ITEM_VOICE_DOMAIN_PREF)
#define PM_SUBS_INFO_CHANGED_ITEM_7_MASK      BM(POLICYMAN_ITEM_RAT_ACQ_ORDER)
#define PM_SUBS_INFO_CHANGED_ITEM_8_MASK      BM(POLICYMAN_ITEM_CALL_MODE)
#define PM_SUBS_INFO_CHANGED_ITEM_9_MASK      BM(POLICYMAN_ITEM_SCAN_OPTIMIZATION)

// Mask for LOG_PM_SUBS_INFO:flags
#define PM_SUBS_INFO_FLAGS_DOMAIN_PREF_MANDATORY_MASK   0x01
#define PM_SUBS_INFO_FLAGS_RF_FORCE_REGISTRATION_MASK   0x02

// Define log record: LOG_PM_SUBS_INFO_type
typedef PACK(struct) {
  log_hdr_type                    hdr;             // Required first, do not access directly (192 bits)
  /* 64-bit align */
  uint32                          version;         // Structure version, required first by diag
  uint32                          changed_mask;    // Bitmask of changed items: BM(item_id)
  /* 64-bit align */
  int8                            subs;            // sys_modem_as_id_e_type
  int8                            call_mode;       // sys_call_mode_t
  uint8                           svc_mode;        // policyman_svc_mode_t
  int8                            ue_mode;         // sys_ue_mode_e_type
  uint32                          lte_band_count;  // number of lte band groups filled in below (1 or 4)
  /* 64-bit align */
  uint64                          gw_bands;
  uint64                          lte_bands_0;
  uint64                          lte_bands_1;
  uint64                          lte_bands_2;
  uint64                          lte_bands_3;
  uint64                          tds_bands;
  uint32                          rat_mask;
  uint8                           flags;           // MSB: reserved:6
                                                   //  |   rf_force_registration:1
                                                   // LSB: domain_pref_mandatory:1
  int8                            voice_dom_pref;  // sys_voice_domain_pref_e_type
  int8                            feature0;
  int8                            rat_order_table_len;   // max number of entries in rat_order_entries[] below
  uint16                          num_rat_order_entries;
  uint8                           rat_order_entries[10];
  uint8                           tgpp_scan_opt;
  uint8                           tgpp2_scan_opt;
  uint8                           freq_list;       ///< frequency list type to use
} LOG_PM_SUBS_INFO_type;


/*---------------------------------------------------------------------------
  LOG_PM_DUMP_INFO structure
---------------------------------------------------------------------------*/
#define LOG_PM_DUMP_INFO_LOG_PACKET_VERSION   1

// Log definition LOG_PM_DUMP_INFO_C
// Comes from: core/api/services/log_codes.h
//#define LOG_PM_DUMP_INFO     0x19FF

#define PM_NUM_SUBS_CONFIGS    2
#define PM_SUBS_CONFIG_SIZE    (PM_NUM_SUBS_CONFIGS * sizeof(policyman_diag_subs_cfg_info_t))
#define PM_CONFIG_BUF_SIZE     (sizeof(LOG_PM_CONFIG_INFO_type) + PM_SUBS_CONFIG_SIZE)
#define PM_SUBS_BUF_SIZE       (sizeof(LOG_PM_SUBS_INFO_type) * POLICYMAN_NUM_SUBS)
#define PM_DUMP_HISTORY_DEPTH  5

typedef PACK(struct) pm_dump_plmn_id_s {
  uint8                                identity[3];
} pm_dump_plmn_id_t;

typedef PACK(struct) {
  uint8     count;
  uint32    mcc_list[SD_PLMN_MCC_LIST_MAX];
} pm_dump_plmn_mcc_list;

typedef PACK(struct) {
  boolean                              is_operational;
  byte                                 pending_reject;        // pending reject cause
  byte                                 reject_cause;          // registration reject cause
  uint8                                id_type;               // sys_sys_id_type_e_type; PLMN id type
  boolean                              serving_info_valid;    // Whether PLMN should be used
  pm_dump_plmn_id_t                    plmn;                  // PLMN of the serving system
  pm_dump_plmn_mcc_list                mcc_list_3gpp2;        // List of 3GPP2 MCCs
  uint32                               mnc_3gpp2;             // sys_mnc_type; 3GPP2 MNC
  int8                                 sys_mode;              // sys_sys_mode_e_type; System's mode
  int8                                 srv_status;            // sys_srv_status_e_type; Service Status
  int8                                 active_band;           // sys_band_class_e_type; serving RF band
  int8                                 srv_domain;            // sys_srv_domain_e_type; Serving domain
} pm_dump_cm_stack_info_t;

typedef PACK(struct) { // policyman_ss_info_t
  int8                                 asubs_id;              // sys_modem_as_id_e_type
  pm_dump_cm_stack_info_t              stack[SYS_MODEM_STACK_ID_MAX];
} pm_dump_cm_ss_info_t;

typedef PACK(struct) { // policyman_cmph_info_s
  uint32                               changed_fields;
  int8                                 asubs_id;              // sys_modem_as_id_e_type; Subs ID to which preferences apply
  int8                                 mode_pref;             // cm_mode_pref_e_type; Indicates the current mode preference
  int8                                 srv_domain_pref;       // cm_srv_domain_pref_e_type;
                                                              //    The service domain preferred by the client. (GSM/WCDMA/TDS only)
  int8                                 oprt_mode;             // sys_oprt_mode_e_type; current operating mode
  /* 64-bit align */
  int8                                 network_sel_mode_pref; // cm_network_sel_mode_pref_e_type;
                                                              //    Current network selection mode preference (AUTOMATIC/MANUAL)
  pm_dump_plmn_id_t                    user_plmn;             // user selected PLMN if network_sel_mode_pref = MANUAL
  uint8                                volte_state;           // cm_volte_enable_e_type; Current VoLTE state
  uint8                                lte_band_count;        // number of lte band groups, 1-4, filled in below (lte_bands[0-3])
  uint16                               dummy_fill;
  /* 64-bit align */
  uint64                               gw_bands;              // cm_band_pref_e_type; user preference for GW bands
  uint64                               lte_bands0;            // cm_band_pref_e_type; user preference for LTE bands
  uint64                               lte_bands1;            // cm_band_pref_e_type; user preference for LTE bands
  uint64                               lte_bands2;            // cm_band_pref_e_type; user preference for LTE bands
  uint64                               lte_bands3;            // cm_band_pref_e_type; user preference for LTE bands
  uint64                               tds_bands;             // cm_band_pref_e_type; user preference for TDS bands
} pm_dump_cm_ph_info_t;

typedef PACK(struct) { // policyman_uim_info_t
  pm_dump_plmn_id_t                    plmn;
  uint8                                sim_app_type;      // mmgsdi_app_enum_type
  uint8                                cdma_app_type;     // mmgsdi_app_enum_type
  boolean                              sim_refreshed;
} pm_dump_uim_info_t;

typedef PACK(struct) { // policyman_cmcall_info_s
  int8                                 asubs_id;          // sys_modem_as_id_e_type
  int8                                 call_event;        // cm_call_event_e_type; Indicates the current call event
  int8                                 call_type;         // cm_call_type_e_type; Indicates the call type
} pm_dump_cm_call_info_t;

typedef PACK(struct) {
  uint32                               timestamp;
  LOG_PM_SUBS_INFO_type                subsItems[POLICYMAN_NUM_SUBS];
  LOG_PM_CONFIG_INFO_type              deviceConfig;
} pm_dump_notify_history_t;

typedef PACK(struct) {
  uint32                               timestamp;
  pm_dump_cm_ss_info_t                 cm_ss_info;
} pm_dump_cm_ss_info_history_t;

typedef PACK(struct) {
  uint32                               timestamp;
  int8                                 cm_ph_evt;         // cm_ph_event_e_type
  pm_dump_cm_ph_info_t                 cm_ph_info;
} pm_dump_cm_ph_info_history_t;

typedef PACK(struct) {
  uint32                               timestamp;
  pm_dump_cm_call_info_t               cm_call_info;
} pm_dump_cm_call_info_history_t;

typedef PACK(struct) {
  boolean                              xml_initialized;
} pm_dump_file_info_t;

typedef PACK(struct) {
  uint8                                session_type;  // mmgsdi_session_type_enum_type; the key
  int8                                 asubs_id;      // sys_modem_as_id_e_type;        PM assigns this
  uint64                               session_id;    // mmgsdi_session_id_type;        comes from mmgsdi
  int8                                 app_type;      // mmgsdi_app_enum_type;          comes from mmgsdi
  boolean                              in_use;        // PM manages -- True if session open
  boolean                              active;        // PM manages -- True if session active
} pm_dump_uim_session_info_t;


typedef PACK(struct) {
  MRE_DUMP_POLICY_INFO_BASE;
  uint16                               suspend_count;
  boolean                              is_policy_init;
} pm_dump_policy_info_t;

/* Device configuration data:
 *   Configuration item 0.
 *   Contains two device configurations specified in the XML.
 */
typedef PACK(union) {
  LOG_PM_CONFIG_INFO_type              config_info;
} pm_dump_config_info_t;

/* Per-subs data:
 *   Internal subs state variables
 *   SUBS log message content
 */
typedef PACK(struct) {
  /* From policyman_subs_state_t
   * This is not using the policyman_subs_state_t type directly because the actual
   * storage for the sub-structures is needed here instead of a pointer to them.
   */
  int8                                 asubs_id;                // sys_modem_as_id_e_type
  uint32                               preconditions_met;
  pm_dump_cm_ss_info_t                 cm_ss_info;
  pm_dump_cm_ph_info_t                 cm_ph_info;
  pm_dump_uim_info_t                   uim_info;
  pm_dump_cm_call_info_t               cm_call_info;

  /* Log message info */
  LOG_PM_SUBS_INFO_type                subs_log_info;
} pm_dump_subs_info_t;


typedef PACK(struct) {
  log_hdr_type                    hdr;               // Diag Log Header. Required first, do not access directly.
  uint32                          version;           // Version of this structure. Required as first after log header.

  time_t                          dump_time;         // The time() when this data was collected.
  boolean                         is_crash;          // Indicates whether collected under crash conditions or not.

  pm_dump_config_info_t           device_config;

  pm_dump_subs_info_t             subs[POLICYMAN_NUM_SUBS];  // Minimum 2 subs (3 if TRIPLE_SIM is defined)

  /* UIM session information
   *   Four entries, in this order:
   *     MMGSDI_1X_PROV_PRI_SESSION
   *     MMGSDI_GW_PROV_PRI_SESSION
   *     MMGSDI_GW_PROV_SEC_SESSION
   *     MMGSDI_GW_PROV_TER_SESSION
   */
  pm_dump_uim_session_info_t      uim_session_info[4];

  pm_dump_cm_call_info_history_t  history_cm_call[PM_DUMP_HISTORY_DEPTH];

  pm_dump_cm_ss_info_history_t    history_cm_ss[PM_DUMP_HISTORY_DEPTH];

  pm_dump_cm_ph_info_history_t    history_cm_ph[PM_DUMP_HISTORY_DEPTH];

  pm_dump_notify_history_t        history_pm_notify[PM_DUMP_HISTORY_DEPTH];

  pm_dump_file_info_t             file_info;
  pm_dump_policy_info_t           policy_info;

} LOG_PM_DUMP_INFO_type;

/*
  MINI DUMP log format
*/
typedef PACK(struct) {
  log_hdr_type                    hdr;               // Diag Log Header. Required first, do not access directly.
  uint32                          version;           // Version of this structure. Required as first after log header.

  time_t                          dump_time;         // The time() when this data was collected.
  boolean                         is_crash;          // Indicates whether collected under crash conditions or not.

  pm_dump_config_info_t           device_config;

  pm_dump_subs_info_t             subs[POLICYMAN_NUM_SUBS];  // Minimum 2 subs (3 if TRIPLE_SIM is defined)

  pm_dump_uim_session_info_t      uim_session_info[4]; // 1X_PRI, GW_PRI, GW_SEC, GW_TER

} LOG_PM_MINI_DUMP_INFO_type;

#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

#endif /* _POLICYMAN_DIAG_H_ */
