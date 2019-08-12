#ifndef _QMI_NAS_INTERNAL_H
#define _QMI_NAS_INTERNAL_H
/*===========================================================================

                         QMI_NAS_INTERNAL.H

DESCRIPTION

 QMI_NAS header file for internal definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2004-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_internal.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/18/11    hs     Created file
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#include "qm_comdef.h"
#include "qmi_nas_hdr.h"

#include "cm.h"
#include "qmi_svc_defs.h"
#include "mmgsdilib_common.h"
#include "sys.h"
#include "rex.h"
#include "qmi_mmode_task_cmd.h"

#include "lte_rrc_ext_msg.h"
#include "msgr_rex.h"
#ifdef FEATURE_DUAL_SIM
  #include "cm_dualsim.h"
#endif
#include "qmi_idl_lib.h"

#include "network_access_service_v01.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif // FEATURE_MODEM_CONFIG_REFRESH

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define NASI_MAX_CLIDS   (10)
#define QMI_NAS_ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )
#define QMI_NAS_XOR(a,b) ((!((a)&&(b))) && ((a)||(b)))

/*--------------------------------------------------------------------------- 
  MMGSDI relaged defines, structs and enums
---------------------------------------------------------------------------*/
#define QMI_NAS_MMGSDI_LEN_EF_SPN      (17)
#define QMI_NAS_MMGSDI_REC_SIZE_EF_OPL (8)
#define QMI_NAS_MMGSDI_LEN_EF_OPL      (512)
#define QMI_NAS_MMGSDI_LEN_EF_PNN      (512)
#define QMI_NAS_MMGSDI_LEN_EF_ONS      (128)
#define QMI_NAS_MMGSDI_LEN_EF_CSP      (2)
#define QMI_NAS_MMGSDI_LEN_EF_AD       (4) // saving only first 4 bytes till length of MNC
#define QMI_NAS_MMGSDI_LEN_EF_IMSI     (9)
#define QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN (35)

#define QMI_NAS_MMGSDI_BITMAP_EF_SPN  0x00000001
#define QMI_NAS_MMGSDI_BITMAP_EF_OPL  0x00000002
#define QMI_NAS_MMGSDI_BITMAP_EF_PNN  0x00000004
#define QMI_NAS_MMGSDI_BITMAP_EF_ONS  0x00000008
#define QMI_NAS_MMGSDI_BITMAP_EF_CSP  0x00000010
#define QMI_NAS_MMGSDI_BITMAP_EF_AD   0x00000020
#define QMI_NAS_MMGSDI_BITMAP_EF_IMSI 0x00000040



/*--------------------------------------------------------------------------
  MMODE QMI NAS msgr definitions
---------------------------------------------------------------------------*/

#define MSGR_MODULE_NAS    0x03
#define MSGR_QMI_NAS       MSGR_TECH_MODULE( MSGR_TECH_QMI, MSGR_MODULE_NAS )

struct valid_success_s {
  boolean valid;
  boolean success;
};

enum qmi_nas_mmgsdi_tristate_s
{
  QMI_NAS_MMGSDI_NOT_INIT = 0,
  QMI_NAS_MMGSDI_VALID, // cache valid
  QMI_NAS_MMGSDI_ERROR  // file does not exist, etc.
};

enum qmi_nas_mmgsdi_cache_ef_e
{
  QMI_NAS_MMGSDI_EF_NONE,
  QMI_NAS_MMGSDI_EF_AD,   // moved to front so they get read first
  QMI_NAS_MMGSDI_EF_IMSI, // moved to front so they get read first
  QMI_NAS_MMGSDI_EF_PLMNWACT,
  QMI_NAS_MMGSDI_EF_PLMN,
  QMI_NAS_MMGSDI_EF_SPN,
  QMI_NAS_MMGSDI_EF_OPL,
  QMI_NAS_MMGSDI_EF_PNN,
  QMI_NAS_MMGSDI_EF_ONS,
  QMI_NAS_MMGSDI_EF_CSP,
  QMI_NAS_MMGSDI_EF_CDMA_SPN,
  QMI_NAS_MMGSDI_EF_ALL
};

struct qmi_nas_mmgsdi_cache_s
{
  boolean                        updating; // is in the middle of updating cache files

  enum qmi_nas_mmgsdi_tristate_s ef_spn_valid;
  uint32                         ef_spn_size;
  uint8                          ef_spn[QMI_NAS_MMGSDI_LEN_EF_SPN];
  boolean                        ef_spn_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_opl_valid;
  uint32                         ef_opl_num_recs; // # of recs
  uint8                          ef_opl[QMI_NAS_MMGSDI_LEN_EF_OPL];
  boolean                        ef_opl_changed;
  unsigned int                   ef_opl_bytes_read;

  enum qmi_nas_mmgsdi_tristate_s ef_pnn_valid;
  uint32                         ef_pnn_rec_size; // size of a rec
  uint32                         ef_pnn_num_recs; // # of recs
  uint8                          ef_pnn[QMI_NAS_MMGSDI_LEN_EF_PNN];
  boolean                        ef_pnn_changed;
  uint32                         ef_pnn_bytes_read;

  enum qmi_nas_mmgsdi_tristate_s ef_ons_valid;
  uint32                         ef_ons_size;
  uint8                          ef_ons[QMI_NAS_MMGSDI_LEN_EF_ONS];
  boolean                        ef_ons_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_csp_valid;
  uint32                         ef_csp_size;
  uint8                          ef_csp[QMI_NAS_MMGSDI_LEN_EF_CSP];
  boolean                        ef_csp_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_ad_valid;
  uint32                         ef_ad_size;
  uint8                          ef_ad[QMI_NAS_MMGSDI_LEN_EF_AD];
//  boolean                        ef_ad_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_imsi_valid;
  uint32                         ef_imsi_size;
  uint8                          ef_imsi[QMI_NAS_MMGSDI_LEN_EF_IMSI];
//  boolean                        ef_imsi_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_cdma_spn_valid;
  uint32                         ef_cdma_spn_size;
  uint8                          ef_cdma_spn[QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN];

  // EFs for SET_PREFERRED_NETWORKS, only need to cache size
  enum qmi_nas_mmgsdi_tristate_s ef_plmnwact_valid;
  uint32                         ef_plmnwact_size;

  enum qmi_nas_mmgsdi_tristate_s ef_plmn_valid;
  uint32                         ef_plmn_size;
};

typedef enum
{
  QMI_NAS_MMGSDI_SESSION_MIN    = -1,
  QMI_NAS_MMGSDI_GW_PRI_SESSION = 0,
  QMI_NAS_MMGSDI_1x_PRI_SESSION,
  QMI_NAS_MMGSDI_GW_SEC_SESSION,
  #ifdef FEATURE_TRIPLE_SIM    
  QMI_NAS_MMGSDI_GW_TER_SESSION,
  #endif
  QMI_NAS_MMGSDI_SESSION_MAX
} qmi_nas_mmgsdi_session_e_type;


struct qmi_nas_mmgsdi_info_s
{
  mmgsdi_client_id_type         client_id;
  mmgsdi_session_id_type        session_id[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  mmgsdi_session_type_enum_type session_type[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  mmgsdi_app_enum_type          app_type[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  boolean                       fcn_refresh[QMI_NAS_MMGSDI_SESSION_MAX]; // is the REFRESH due to FCN?
  struct qmi_nas_mmgsdi_cache_s cache[QMI_NAS_MMGSDI_SESSION_MAX]; // cache for files of interest
  mmgsdi_events_enum_type       latest_evt[QMI_NAS_MMGSDI_SESSION_MAX]; // last received event to see if the session is usable or not
  boolean                       operation_ready[QMI_NAS_MMGSDI_SESSION_MAX]; // if last received event indicates SIM intialized and ready
  boolean                       session_active[QMI_NAS_MMGSDI_SESSION_MAX];
  
  boolean                       csp_req_buffer_valid;
  qmi_nas_mmgsdi_session_e_type csp_req_buffer_session;
  qmi_cmd_buf_type             *csp_req_buffer;
};

typedef struct qmi_nas_time_and_time_zone_s
{
  uint8                                     year;       /**< Year field. */
  uint8                                     month;      /**< Month field. */
  uint8                                     day;        /**< Day field. */
  uint8                                     hour;       /**< Hour field. */
  uint8                                     minute;     /**< Minute field. */
  uint8                                     second;     /**< Second field. */
  int8                                     time_zone;  /**< Timezone field. */
} qmi_nas_time_and_time_zone_s_type;


struct qmi_nas_nitz_time_info_s
{
  uint8 radio_if;
  struct
  {
    boolean              is_valid;
    byte                 lp_sec;
    int8                time_zone;
    byte                 daylt_savings;
  } cdma_cache;
  
  struct
  {
    boolean             is_valid;
    byte lp_sec;
    int8 time_zone;
    byte daylt_savings;
  } hdr_cache;

  struct
  {
      boolean             is_valid;

      boolean                                   univ_time_and_time_zone_avail;
      qmi_nas_time_and_time_zone_s_type         univ_time_and_time_zone;

      boolean                                   time_zone_avail;
      int8                        time_zone;

      boolean                                   daylight_saving_adj_avail;
      uint8            daylight_saving_adj;

  } umts_cache;

};


struct qmi_nas_ssac_change_info_s
{
  boolean is_valid;
  lte_rrc_ssac_params_s ssac_cache;
};

/*--------------------------------------------------------------------------- 
  QMI NAS defines, structs and enums
---------------------------------------------------------------------------*/
#define QMI_NAS_NET_DESC_LEN_MAX  256

#define MAX_NUM_SIG_INFO_THRESHOLDS 16
#define MAX_NUM_SIG_INFO2_THRESHOLDS 32
#define MAX_RSSI_EVENTS 10
#define MAX_CM_SS_EVENTS 50
#define MAX_MODEM_STATS_EVENT 20
#define QMI_NAS_WCDMA_MAX_MONITORED_LTE_CELLS 32

/*---------------------------------------------------------------------------
  3GPP PLMN name encoding
---------------------------------------------------------------------------*/
typedef enum
{
  NASI_PLMN_NAME_ENC_GSM_DEFAULT = 0,
  NASI_PLMN_NAME_ENC_UCS2_16     = 1,
  NASI_PLMN_NAME_ENC_MAX,
  NASI_PLMN_NAME_ENC_MAX32       = 0x10000000
} qmi_nasi_plmn_name_enc_e_type;

/* Need to update this struct whenever tlvs/fields are added/modified in 
   phone msg/ind */
typedef struct qmi_nasi_ph_info_s
{
  uint8       curr_nam;
  uint8       cur_ddtm_status;
  uint8       ddtm_pref;
  uint16      ddtm_act_mask;
  uint8       ddtm_num_srv_opt;
  uint32      ddtm_srv_opt_mask;
} qmi_nasi_ph_info_type; 

#define NASI_MAX_LEN_NETWORK_NAME (255)

typedef struct qmi_nasi_rssi_info_stack_s
{
  uint64              signal_strength_changed_fields;
  sys_sys_mode_e_type sys_mode;
  uint16              rssi;
  byte                sinr;
  uint16              ecio;
  int                 io;
  int16               rsrp;
  int8                rsrq;
  uint8               bit_err_rate;
  uint16              frame_err_rate;
  boolean             hdr_hybrid;
  uint16              hdr_packet_err_rate;
} qmi_nasi_rssi_info_stack_type;

typedef enum
{
  QMI_NASI_MMGSDI_PLMN_UNKNOWN = 0,
  QMI_NASI_MMGSDI_PLMN_EF_NOT_FOUND,
  QMI_NASI_MMGSDI_PLMN_EF_FOUND,
  QMI_NASI_MMGSDI_PLMN_EF_READ_REQUESTED,
  QMI_NASI_MMGSDI_PLMN_EF_FOUND_AND_READ,
  QMI_NASI_MMGSDI_PLMN_EF_WRITE_REQUESTED,
  QMI_NASI_MMGSDI_PLMN_MAX = 0xff
} qmi_nasi_mmgsdi_plmn_ef_status_e_type;

typedef struct
{
  sys_plmn_id_s_type plmn;
#define QMI_NASI_MMGSDI_ACT_LENGTH (2)
  uint8              act[QMI_NASI_MMGSDI_ACT_LENGTH];
} qmi_nasi_mmgsdi_plmn_wact_type;

enum init_state {
  INIT_STATE_NOT_INITIALIZED         = 0,
  INIT_STATE_PENDING_MODE_CAPABILITY,
  INIT_STATE_INITIALIZED
};

enum qmi_nas_subs_e
{
  QMI_NAS_SUBS_NONE      = -1,
  QMI_NAS_SUBS_PRIMARY   = 0,
#ifdef FEATURE_DUAL_SIM
  QMI_NAS_SUBS_SECONDARY = 1,
#endif
#ifdef FEATURE_TRIPLE_SIM
  QMI_NAS_SUBS_TERTIARY  = 2,
#endif
  QMI_NAS_SUBS_MAX
};

enum qmi_nas_stack_e
{
  QMI_NAS_STACK_NONE = -1,
  QMI_NAS_STACK_MAIN = 0,
  QMI_NAS_STACK_HYBRID,
  QMI_NAS_STACK_GW_HYB,
  QMI_NAS_STACK_GW_HYB3
};

typedef struct qmi_nasi_rssi_info_s
{
  uint8 number_of_stacks;
  qmi_nasi_rssi_info_stack_type stack_info[STACK_INDEX_MAX];
} qmi_nasi_rssi_info_type;

struct qmi_nas_nw_desc_s
{
  uint8 desc[QMI_NAS_NET_DESC_LEN_MAX];
};

struct current_plmn_name_s
{
  boolean               current_plmn_valid;
  sys_plmn_id_s_type    plmn;
  mmgsdi_eons_name_type srv_prov_name;
  mmgsdi_plmn_name_type plmn_short_name;
  mmgsdi_plmn_name_type plmn_long_name;
};

struct qmi_nas_queue_management_s
{
  // counters signed due to possibility of being -1 for short period of time
  int                rssi_cnt;
  int                cm_ss_event_cnt;
  int                modem_stats_cnt;
  rex_crit_sect_type cm_ss_cs;
  rex_crit_sect_type cm_stats_cs;
};

struct lte_sib16_nw_time_s
{
  boolean                is_valid;
  sys_modem_as_id_e_type asubs_id;
  boolean                lte_sib16_acquired;
  boolean                lte_dayLightSavingTime_enabled;
  uint8                  dayLightSavingTime;
  boolean                lte_leapSeconds_enabled;
  int8                   leapSeconds;
  boolean                lte_localTimeOffset_enabled;
  int8                   localTimeOffset;
} ;

/** @brief structure contains number of builtin plmn ids and 
    mmgsdi_builtin_plmn_id_type list
*/
typedef struct qmi_nas_builtin_plmn_list_s
{
  uint32                       oplmn_list_len;  /**< Number of plmn ids. */
  mmgsdi_builtin_plmn_id_type  oplmn_entry[NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01];    /**< Pointer to plmn id list. */
} qmi_nas_builtin_plmn_list_type;

typedef enum 
{
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_NONE = 0,

  /* Search is still in progress, send indication */  
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_INPROGRESS, 
  /* Search is aborted, last indication */  
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_AS_ABORT,
  /* Search is done, last indication */
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_DONE, 
}qmi_nas_incremental_net_scan_status_e_type;

/** @brief Type to hold phone information. received in phone
    events.
*/

typedef struct qmi_cm_ph_info_s {

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Phone State
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_oprt_mode_e_type              oprt_mode;
    /**< Indicates the current operating mode. */

  sys_sys_mode_mask_e_type          mode_capability;
    /**< Indicates the mode capability. */

  sys_band_mask_e_type              band_capability;
    /**< Indicates 2G and 3G (excluding TD-SCDMA) band capability. */

  sys_lte_band_mask_e_type              lte_band_capability;
    /**< Indicates the LTE band capability. */

  sys_band_mask_e_type              tds_band_capability;
    /**< Indicates the TD-SCDMA band capability. */

  cm_nam_e_type                     curr_nam;
    /**< Indicates the current NAM - needed when NAM selection
         is set to AUTO-NAM.\ CDMA only. */

  sys_ue_usage_setting_e_type       ue_usage_setting;
    /**< UE Usage setting*/

  sys_voice_domain_pref_e_type      voice_domain_pref;
    /**< Voice domain pref*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Preferences per Subscription
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_as_id_e_type            asubs_id;
    /**< Subscription ID to which these preferences should apply. */
	
  cm_mode_pref_e_type               mode_pref;
    /**< Indicates the current mode preference. */

  cm_pref_term_e_type               pref_term;
    /**< Indicates the current preference term (e.g., one-call, power-cycle).*/

  cm_prl_pref_e_type                prl_pref;
    /**< Indicates the current PRL preferred setting. */

  cm_gw_acq_order_pref_e_type       acq_order_pref;
    /**< The acquisition order preferred by the client (GSM/WCDMA/TDS only). */

  cm_srv_domain_pref_e_type         srv_domain_pref;
    /**< The service domain preferred by the client. This field is set after a
         call to cm_ph_cmd_sys_sel_pref (GSM/WCDMA/TDS only). */

  cm_band_pref_e_type               band_pref;
    /**< Indicates the current 2G and 3G (excluding TD-SCDMA)
         band preference. Used with CM_BAND_PREF_* values.  */

  sys_lte_band_mask_e_type               lte_band_pref;
    /**< Indicates the current LTE band preference. Used
         with CM_BAND_PREF_LTE_* values. */

  cm_band_pref_e_type               tds_band_pref;
    /**< Indicates the current TD-SCDMA band preference. Used
         with CM_BAND_PREF_TDS_* values. */

  cm_roam_pref_e_type               roam_pref;
    /**< Indicates the current roam preference. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< Defines whether the network should be selected automatically
         or manually. */

  sys_plmn_id_s_type                plmn;
    /**< If the network selection is set to manual,
         this specifies the PLMN ID. */

  sys_csg_id_type                   csg_id;
    /**< If the MANUAL CSG Selection is successful,
         this specifies the CSG ID selected */

  sys_radio_access_tech_e_type       csg_rat;
    /**< RAT specified for CSG */

  sys_found_plmn_list_u_type     *p_available_networks;
    /**< Available networks (GSM/WCDMA/TDS only). */

  cm_network_list_type_e_type    network_list_type;   
 
  sys_plmn_list_status_e_type       available_networks_list_cnf;
    /**< Provides the status of a PLMS list request (success/abort).\
         Clients must check for CM_API_AVAIL_NET_LIST_CNF
         before using this field. */

  cm_acq_pri_order_pref_s_type      acq_pri_order_pref;
    /**< Rat acquisition priority order preference */

  sys_drx_cn_coefficient_s1_e_type  drx_coefficient;
    /**< The DRX value NAS cnf for GET_DRX_REQ */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Subscription Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rtre_config_e_type             rtre_config;
    /**< Current RTRE configuration; RUIM, NV, or RUIM fallback NV (CDMA only). */

  cm_rtre_control_e_type            rtre_control;
    /**< Current RTRE control; RUIM or NV (CDMA only). */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            DDTM Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ddtm_pref_e_type               ddtm_pref;
    /**< Data Dedicated Tranmission Mode (DDTM) preference. */

  boolean                           cur_ddtm_status;
    /**< DDTM status. */

  sys_ddtm_act_mask_e_type          ddtm_act_mask;
    /**< DDTM action mask settings. */

  uint16                            ddtm_num_srv_opt;
    /**< Number of service options specified in ddtm_srv_opt_list. */


  sys_srv_opt_type             ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /**< List of service options whose pages have to be ignored, provided
         ddtm_pref and ddtm_act_mask allow it. */

	
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Dual Standby Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_dual_standby_pref_e_type     standby_pref;
      /**< Standby preference of the phone. */

  sys_modem_as_id_e_type                 active_subs;
      /**< The active subscription in Single Standby mode.\
           Only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY. */

  sys_modem_as_id_e_type                 default_voice_subs;
      /**< Default voice subscription. */

  sys_modem_as_id_e_type                 default_data_subs;
      /**< Default data subscription. */

  sys_modem_as_id_e_type                 priority_subs;
      /**< Priority subscription. */

  sys_data_priority_e_type  priority_type; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   **            Device Mode
   **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  uint32                                 hplmn_timer;
    /**< hplmn timer value */

} qmi_cm_ph_info_s_type;


struct qmi_nasi_global_s
{
  enum init_state  inited;
  qmi_idl_service_object_type   svc_obj;
  boolean  net_scan_started;
  sys_modem_as_id_e_type net_scan_asubs_id;
  cm_network_list_type_e_type net_scan_type;
  qmi_nas_incremental_net_scan_status_e_type  incremental_net_scan_status;
  uint8    incremental_net_scan_clid;
  uint8    min_rssi_delta;
  uint8    min_io_delta;
  uint8    min_rsrq_delta;
  int16    min_rsrp_delta;
  qmi_nasi_ph_info_type last_reported_nasi_ph_info;
  qmi_nasi_ph_info_type current_nasi_ph_info;
  qmi_cmd_buf_type      *p_rtre_set_status_cmd_buf;
  boolean rtre_set_status_pending;
  boolean  get_pref_nwks_req_pending;
  struct
  {
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn_wact;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type oplmn_wact;
    sys_plmn_id_s_type *plmn_list;
    qmi_nasi_mmgsdi_plmn_wact_type *plmn_wact_list;
    qmi_nasi_mmgsdi_plmn_wact_type *oplmn_wact_list;
    uint8 plmn_list_size;
    uint8 plmn_wact_list_size;
    uint8 oplmn_wact_list_size;
  } pref_nw_scratch;
  struct
  {
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn_wact;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type oplmn_wact;
  } mmgsdi_ef_info;

  struct get_plmn_name_s
  {
    mmgsdi_eons_name_type last_reported_srv_prov_name;
    mmgsdi_plmn_name_type last_reported_plmn_short_name;
    mmgsdi_plmn_name_type last_reported_plmn_long_name;
  } get_plmn_name;

  cm_client_id_type            cm_clnt_id;
  dword                        t3402_value[QMI_NAS_SUBS_MAX];
  /* Store the current T3402 value in secs */


  struct qmi_nas_mmgsdi_info_s mmgsdi;

  // latest CM SS information
  boolean                    rcvd_ss_info; // whether at least one ss event is received
  cm_mm_msim_ss_info_s_type       cm_msim_ss_info[QMI_NAS_SUBS_MAX];

  struct qmi_nas_nw_desc_s   prev_nw_desc[QMI_NAS_SUBS_MAX]; // last reported network description
//  uint8                      prev_nw_desc[QMI_NAS_SUBS_MAX][QMI_NAS_NET_DESC_LEN_MAX]; // last reported network description

  // latest CM PH information
  boolean             rcvd_ph_info; // whether at least one ph event for asid 1 is received
  qmi_cm_ph_info_s_type   cm_ph_info;      /* for asid 1 */
  cm_mode_pref_e_type last_mode_pref;
#ifdef  FEATURE_DUAL_SIM
  boolean             rcvd_ph_info2; // whether at least one ph event for asid 2 is received
  qmi_cm_ph_info_s_type   cm_ph_info2;     /* for asid 2 */
  cm_mode_pref_e_type last_mode_pref2;
#endif
#ifdef FEATURE_TRIPLE_SIM
  boolean             rcvd_ph_info3; // whether at least one ph event for asid 2 is received
  qmi_cm_ph_info_s_type   cm_ph_info3;     /* for asid 2 */
  cm_mode_pref_e_type last_mode_pref3;
#endif

  // bolleans to ensure that cm_ph_cmd_signal_strength_delta is called only
  // once
  boolean is_set_cm_ph_cmd_signal_strength_delta;
  boolean is_pending_config_sig_info;
#ifdef FEATURE_DUAL_SIM
  // latest CM SUBS information
  cm_subs_info_s_type subs_info[QMI_NAS_SUBS_MAX];
#endif

  // Current PLMN information
  struct current_plmn_name_s current_plmn_name_cache;
  struct current_plmn_name_s current_plmn_name_cache_ignore;
  struct current_plmn_name_s current_plmn_name_cache_sec_stack;
  struct current_plmn_name_s current_plmn_name_cache_sec_stack_ignore;

  // embms status
  boolean embms_status; // true: enabled, false: disabled
  int16   trace_id;
  void    *cmd_buf_p;
  
  // save last state before CM moves sim_state to SYS_SIM_STATE_NOT_AVAILABLE
  // [0] for sim_state and [1] for gw_sim_state
  sys_sim_state_e_type sim_state_before_not_avail[QMI_NAS_SUBS_MAX];

  // CM SS Event management
  struct qmi_nas_queue_management_s cm_event_queue;
  
  struct ims_pref_s
  {
    boolean lte_mask_valid;
    uint64  lte_call_mask;

    boolean hdr_mask_valid;
    uint64  hdr_call_mask;

    boolean gsm_mask_valid;
    uint64  gsm_call_mask;

    boolean wcdma_mask_valid;
    uint64  wcdma_call_mask;

    boolean cdma_mask_valid;
    uint64  cdma_call_mask;

    boolean tds_mask_valid;
    uint64  tds_call_mask;
  } ims_pref_cache;

  qmi_cmd_buf_type *ims_cmd_buf_p;

  qmi_cmd_buf_type *get_lte_band_p;

  qmi_cmd_buf_type *get_serv_cell_sib;

  qmi_cmd_buf_type *get_hplmn_cmd_buf_p;

  qmi_cmd_buf_type *set_drx_scaling_cmd_buf_p;
  qmi_cmd_buf_type *gcell_info_cmd_buf_p;

  qmi_cmd_buf_type *set_cell_lock_config_req_buf_p;

  qmi_cmd_buf_type *rpm_parameters_cmd_buf_p;

  struct builtin_plmn_list_s
  {
    uint8 clid;
    uint32 list_id;
    uint16 oplmn_len_total; //cumulative when list is recv in multiple msgs
    uint16 total_entries;//Total list entries expected
    boolean list_op_in_prog;
    qmi_nas_builtin_plmn_list_type *list_ptr;
    boolean ind_tok_valid;
    uint32 ind_token;
  }builtin_plmn_list[QMI_NAS_SUBS_MAX];

  struct lte_sib16_nw_time_s lte_sib16_nw_time_cache;

  qmi_cmd_buf_type *set_lte_band_p;

  void             *embms_ext_cmd_buf_p;

  qmi_cmd_buf_type *get_drx_cmd_buf_p;
  mmgsdi_sim_busy_evt_info_type sim_busy_status;

  lte_rrc_ac_barring_info_s lte_rrc_ac_barring_info_cache;

  sys_data_priority_e_type data_subs_priority[QMI_NAS_SUBS_MAX];

  sys_data_roaming_e_type  data_roam_cache[QMI_NAS_SUBS_MAX];
  boolean                  data_roam_cache_valid[QMI_NAS_SUBS_MAX];

  sys_data_roaming_e_type  data_roam_temp;
  enum qmi_nas_subs_e      data_roam_temp_subs;
  boolean                  data_roam_temp_valid;

  boolean          call_mode_valid[QMI_NAS_SUBS_MAX];
  sys_call_mode_t  call_mode[QMI_NAS_SUBS_MAX];


  boolean                  voice_roam_cache[QMI_NAS_SUBS_MAX];
  boolean                  voice_roam_cache_valid[QMI_NAS_SUBS_MAX];

  boolean                  voice_roam_temp;
  enum qmi_nas_subs_e 	   voice_roam_temp_subs;
  boolean 		   voice_roam_temp_valid;

};

/*---------------------------------------------------------------------------
  QMI Network Access Service instance state definition &
  Network Access Service client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_nasi_client_state_s
{
  qmi_common_client_state_type common; // must be first since we alias
  int16 instance;
  int16 service_id;

  struct report_status_s
  {
    // QMI_NAS_SET_EVENT_REPORT_REQ TLV

    // TLV 0x10 Signal Strength Indicator
    boolean     report_sig_str;
    uint8       num_thresholds;
    signed char sig_str_thresholds[NAS_SIG_STRENGTH_THRESHOLD_LIST_MAX_V01];

    // TLV 0x11 RF Band Information
    boolean     report_rf_band_info;

    // TLV 0x12 Registration Reject Reasons
    boolean     report_reg_reject_reason;

    // TLV 0x13 RSSI Indicator
    boolean     report_rssi;
    uint8       report_rssi_delta;
    signed char last_reported_rssi; 
    uint16      last_reported_rssi_ex; 
    signed char last_reported_hdr_rssi;
    uint16      last_reported_hdr_rssi_ex; 

    // TLV 0x14 and TLV 0x19 ECIO Indicator
    boolean     report_ecio;
    uint8       report_ecio_delta; //TLV 0x14 only
    uint16      last_reported_ecio; 
    uint16      last_reported_hdr_ecio;
    uint8       reporting_mode_ecio;
    uint8       num_thresholds_ecio; //TLV 0x19 only
    int16       ecio_thresholds[NAS_ECIO_THRESHOLD_LIST_MAX_V01]; //TLV 0x19 only

    // TLV 0x15 IO Indicator
    boolean     report_io;
    uint8       report_io_delta;
    int         last_reported_io; 
    int         last_reported_hdr_io;

    // TLV 0x16 and TLV 0x20 SINR Indicator
    boolean     report_sinr;
    uint8       report_sinr_delta; //TLV 0x16 only
    uint8       last_reported_sinr; 
    uint8       last_reported_hdr_sinr;
    uint8       reporting_mode_sinr; 
    uint8       num_thresholds_sinr;//TLV 0x20 only 
    uint8       sinr_thresholds[NAS_SINR_THRESHOLD_LIST_MAX_V01]; //TLV 0x20 only

    // TLV 0x17 Error Rate Indicator
    boolean     report_error_rate;

    // TLV 0x18 RSRQ Indicator
    boolean     report_rsrq;
    uint8       report_rsrq_delta;
    uint8       last_reported_rsrq; 

    // TLV 0x19
    boolean     report_lte_snr;
    uint8       report_lte_snr_delta;
    byte        last_reported_lte_snr; 

    // TLV 0x1A
    boolean     report_lte_rsrp;
    uint8       report_lte_rsrp_delta;
    int16       last_reported_lte_rsrp; 

    boolean     reg_sys_sel_pref;
    boolean     reg_current_nam;
    boolean     reg_ddtm_events;
    boolean     not_report_serving_system; // 0 (default) implies reporting by default
    boolean     report_dual_standby_pref_ind;
    boolean     report_subscription_info_ind;
    boolean     report_thermal_emergency_state;
    boolean     report_network_time;
    boolean     report_sys_info;
    boolean     report_sig_info;
    boolean     report_err_rate;
    boolean     report_uati;          // HDR uati indication
    boolean     report_session_close; // HDR session close indication
    boolean     report_managed_roaming;
    boolean     report_current_plmn_name;
    boolean     report_embms_status;        
    boolean     report_rf_band_info_ind;
    enum qmi_nas_subs_e  bound_subs; // subscription which the client is bound to

    struct network_reject_s
    {
      boolean     report_network_reject_ind;
      boolean     suppress_sys_info_on_reject;
    } network_reject;

    boolean     report_operator_name_data;
    boolean     report_csp_plmn_mode_bit;  
    boolean     report_rtre_config;

    uint64      limit_sys_info_reporting_mask;
    
    boolean     report_ims_pref_status;  
    boolean     report_e911_state_ready;
    boolean     report_lte_sib16_network_time;
    boolean     report_lte_cphy_ca_status;
    boolean     ignore_plmn_display_cond_in_ind;
    boolean     report_subscription_change;
    boolean     report_ssac_info;
    boolean     report_t3402_timer_change;
    boolean     reg_acb_info_change;
    boolean     report_data_priority_subs_change;
    boolean     report_t3346_timer_status_change;
    boolean     report_call_mode_change;
    boolean     report_ssac_change_info;
    boolean     report_manual_scan_fail;
    boolean     report_gcell_info_ind;
  } report_status; 
  
} qmi_nasi_client_state_type;

extern struct qmi_nasi_global_s qmi_nasi_global;

/*---------------------------------------------------------------------------
  Structure to maintain NAS state
---------------------------------------------------------------------------*/
typedef struct qmi_nasi_state_s
{
  qmi_common_svc_state_type   common; // must be first since we alias
  qmi_nasi_client_state_type *client[NASI_MAX_CLIDS];
  uint16                      num_instances;
} qmi_nasi_state_type;

extern qmi_nasi_state_type qmi_nas_state;

/*---------------------------------------------------------------------------
  BCD digit extraction
---------------------------------------------------------------------------*/
#define QMI_NAS_BCD_LOW_DIGIT(a)   ((a) & 0x0F)
#define QMI_NAS_BCD_HIGH_DIGIT(a) (((a) >> 4) & 0x0F)

struct access_tech_s
{
  int c; // cdma
  int h; // hdr
  int g; // gsm
  int w; // wcdma
  int l; // lte
  int t; // tdscdma
};

/*--------------------------------------------------------------------------- 
  MMGSDI get_operator_name client data information
---------------------------------------------------------------------------*/
enum qmi_nasi_calling_cmd
{
  NAS_CMD_MIN = -1,
  NAS_CMD_PERFORM_NETWORK_SCAN,
  NAS_CMD_GET_PLMN_NAME,
  NAS_CMD_GET_HOME_NETWORK,
  NAS_CMD_MAX
};

struct qmi_nasi_mmgsdi_client_data_s
{
  enum qmi_nasi_calling_cmd reason;
  void                     *cmd_buf_ptr;
  boolean                   send_all_info;
};

#define QMI_NAS_INVALID_LAC  (0xFFFF)

typedef struct
{
  uint64  umts_band_pref;
  uint64  lte_band_pref;
  uint32  search_timer;
  uint32  list_type;
  uint32  network_type;
  uint8  search_mode;
  uint8  asubs_id;
  uint8  select_config;
} qmi_nas_csg_search_select_config_s_type;


void qmi_nas_mmgsdi_cmd_rsp_cb( mmgsdi_return_enum_type status, mmgsdi_cnf_enum_type cmd, const mmgsdi_cnf_type *p_info );

void qmi_nas_cmph_evt_cb( cm_ph_event_e_type evt, const cm_ph_info_s_type *p_info);


//=========================================================================== 
// QMI NAS command handler types
//=========================================================================== 
enum qmi_nas_cmd_handler_type {
   QMI_NAS_CMD_HDR = 1,  // Process an HDR event

   QMI_NAS_CMD_MAX // Max enum value
};

//=========================================================================== 
// QMI NAS command handler payloads
//
// Define payloads here for each command handler type. We have choose to not
// combine these into a union here since some payloads are much larger (4k)
// than most others.
// Note: first parameter of each payload must be qmi_nas_cmd_handler_type
//=========================================================================== 

// HDR payload
struct qmi_nas_cmd_hdr_payload {
   enum qmi_nas_cmd_handler_type id;
#ifdef FEATURE_QMI_NAS_HDR
   hdrmc_cb_params_rsp_union_type parms;
#endif
};

/*===========================================================================
  FUNCTION QMI_NAS_INIT()

  DESCRIPTION
    Register the Wireless Data Service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_nas_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_NAS_GEN_SERVING_SYSTEM_IND()

  DESCRIPTION
    Called when the serving system info changes. 
    Causes serving_system_ind to be sent to all clients
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_serving_system_ind
(
  void
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_RSSI_CHANGE_IND()

  DESCRIPTION
    Called when CM reports change in RSSI. It sends event report ind to 
    notify rssi change to registered control points
        
  PARAMETERS
    cur_rssi : current signal strength
    radio_if : radio technology in use

  RETURN VALUE
    None
                
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_rssi_change_ind
( 
  signed char  cur_rssi,
  uint8        radio_if
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_SYS_SEL_PREF_IND()

  DESCRIPTION
    Called when the system selection preference changes. 
    Causes serving_system_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_sys_sel_pref_ind
(
  qmi_cm_ph_info_s_type  *p_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_CURRENT_NAM_IND()

  DESCRIPTION
    Called when the current nam changes. 
    Causes qmi_nasi_current_nam_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_current_nam_ind
(
  qmi_cm_ph_info_s_type  *p_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_DDTM_IND()

  DESCRIPTION
    Called when the ddtm(Data Dedicated Transmission Model)status changes. 
    Causes qmi_nasi_ddtm_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_ddtm_ind
(
  qmi_cm_ph_info_s_type  *p_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_REG_REJECT_IND()

  DESCRIPTION
    Called when we receive a REG_REJECT event from CM to cache the reject
    reason.
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_reg_reject_ind
(
  const cm_reg_reject_info_s_type *   reg_reject_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_REG_ATTACH_RESP()

  DESCRIPTION
    Called when error_cb() supplied in cm_ph_cmd_sys_sel_pref() is called 
    by CM. Sends response to control point for the correspoding
    Register or Attach requests.
        
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_reg_attach_resp
(
  void *                user_data,
  cm_ph_cmd_err_e_type  ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_NET_SCAN_ERR_RESP()

  DESCRIPTION
    Called when error_cb() supplied in cm_ph_cmd_get_networks() is called 
    by CM. Sends error response to control point if error is reported by CM
    
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : Error type

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_net_scan_err_resp
(
  void *               user_data,
  cm_ph_cmd_err_e_type ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_ERROR_RATE_IND()

  DESCRIPTION
    Called when we receive a RSSI change event from CM with changes to the
    error rate values.
    
  PARAMETERS
    fer - the frame error rate value

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_error_rate_ind
(
  uint16              err_rate,
  sys_sys_mode_e_type mode
);

#if defined(FEATURE_MMGSDI_SESSION_LIB)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*===========================================================================
  FUNCTION QMI_NAS_FIND_PLMN_STATUS()

  DESCRIPTION
    Function to trigger GET_FILE_ATTR requests to MMGSDI to verify the
    presence of PLMN and PLMN_WACT files

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_find_plmn_status
(
  void
);
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#endif /* defined(FEATURE_MMGSDI_SESSION_LIB) */

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_RF_INFO_IND()

  DESCRIPTION
    Called when either the current active band or channel changes.
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_rf_info_ind
(
  void
);

void qmi_nas_operator_name_data_ind( mmgsdi_session_type_enum_type session, sys_mm_information_s_type *p_mm_info );
void qmi_nas_csp_plmn_mode_bit_ind( mmgsdi_session_type_enum_type session );
void qmi_nas_set_rtre_config_resp(  boolean   result_status );

/*===========================================================================
  FUNCTION QMI_NAS_CMSS_CMD_CB()

  DESCRIPTION
    Handle CM SS command callback

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_cmss_cmd_cb
( void *p_data, 
  cm_ss_cmd_e_type cmd, 
  cm_ss_cmd_err_e_type err
);

/*===========================================================================
  FUNCTION QMI_NAS_CMPH_CMD_CB()

  DESCRIPTION
    Handle CM PH command callback

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_cmph_cmd_cb( void *p_data, cm_ph_cmd_e_type cmd, cm_ph_cmd_err_e_type err);

void qmi_nas_gen_serving_system_ind( cm_mm_msim_ss_info_s_type *p_ss_info, qmi_cm_ph_info_s_type *p_ph_info, qmi_cm_ph_info_s_type *p_ph_info2 );
void qmi_nas_gen_current_plmn_name_ind( cm_mm_msim_ss_info_s_type *p_ss_info, mmgsdi_session_type_enum_type session );

/*===========================================================================
  FUNCTION qmi_nas_get_subs_from_asid()

  DESCRIPTION
    Retrieve subscription information that is mapped to provided asubs_id

  PARAMETERS
    asubs_id

  RETURN VALUE
    subscription information that is mapped to provided asubs_id.
    QMI_NAS_SUBS_NONE if asubs_id is not operational
===========================================================================*/
enum qmi_nas_subs_e qmi_nas_get_subs_from_asid( sys_modem_as_id_e_type asubs_id );

/*===========================================================================
  FUNCTION qmi_nas_get_asid_from_subs()

  DESCRIPTION
    Retrieve asubs_id information that is mapped to provided subscription type

  PARAMETERS
    subscription type

  RETURN VALUE
    asubs_id information that is mapped to provided subscription information.
    SYS_MODEM_AS_ID_NONE if there is no asubs_id mapped to subscription type
===========================================================================*/
sys_modem_as_id_e_type qmi_nas_get_asid_from_subs( enum qmi_nas_subs_e subs_type );

extern boolean qmi_nas_mmgsdi_register_to_amss(void);
extern void qmi_nas_mmgsdi_init( struct qmi_nas_mmgsdi_info_s *p_data);
extern boolean qmi_nas_mmgsdi_ef_ready ( qmi_nas_mmgsdi_session_e_type qmi_session, uint32 bitmap );
extern boolean qmi_nas_mmgsdi_is_valid_session( mmgsdi_session_type_enum_type session );
extern boolean qmi_nas_mmgsdi_is_valid_qmi_session( qmi_nas_mmgsdi_session_e_type session );
extern boolean qmi_nas_mmgsdi_is_valid_3gpp_session( mmgsdi_session_type_enum_type session );
extern mmgsdi_session_type_enum_type qmi_nas_map_mmgsdi_session( qmi_nas_mmgsdi_session_e_type qmi_session );
extern qmi_nas_mmgsdi_session_e_type qmi_nas_map_mmgsdi_session_to_qmi( mmgsdi_session_type_enum_type session );

//=========================================================================== 
// FUNCTION: qmi_nas_hdr_process_evt_payload
//
// DESCRIPTION:
//   Process HDR event payload (called once per QMI CLID instance)
//
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_hdr_process_evt_payload( qmi_nasi_client_state_type *cl_sp,
                                      const void *payload);

/*=========================================================================== 
  FUNCTION qmi_nas_process_mmgsdi_get_operator_name()
 
  DESCRIPTION
    Process information from mmgsdi_get_operator_name

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_process_mmgsdi_get_operator_name ( 
  const mmgsdi_cnf_type        *cnf_data
);

/*=========================================================================== 
  FUNCTION qmi_nas_process_mmgsdi_get_operator_name()
 
  DESCRIPTION
    Process information from mmgsdi_get_operator_name

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_process_mmgsdi_get_se13_plmn_names ( 
  const mmgsdi_cnf_type        *cnf_data
);

extern void qmi_nas_nw_name_init(void);

extern void qmi_nas_nw_name_add(
  mmgsdi_session_type_enum_type session,
  mmgsdi_eons_evt_info_type     evt_info
);

extern mmgsdi_eons_evt_info_type * qmi_nas_nw_name_find(
  mmgsdi_session_type_enum_type session,
  sys_plmn_id_s_type plmn_id,
  sys_lac_type lac_tac,
  mmgsdi_rat_enum_type rat,
  sys_csg_id_type csg_id
);

extern enum qmi_nas_stack_e qmi_nas_nw_name_info_stack(
  mmgsdi_eons_evt_info_type *eons_info,
  sys_plmn_id_s_type plmn_id,
  sys_lac_type lac_tac,
  mmgsdi_rat_enum_type rat,
  sys_csg_id_type csg_id
);

extern void qmi_nas_mmgsdi_process_cmd_rsp( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr );
extern void qmi_nas_mmgsdi_process_evt( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr );

//=========================================================================== 
// FUNCTION: qmi_nas_cmd_handler
//
// DESCRIPTION:
//   Generic handler function for QMI NAS event handlers
//
// SIDE-EFFECTS: 
//   Frees memory the payload points to
//   Sends QMI indication messages according to data in payload and
//   may update internal QMI_NAS states depending on the command handler thats
//   invoked
//
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_cmd_handler( qmi_mmode_cmd_enum_type cmd, void *payload );

/*========================================================================
  FUNCTION qmi_nas_msgr_handler

  DESCRIPTION
    process messages received from msgr

  PARAMETERS
    msg pointer in *void. cast to dcc_msgr_type before use.
		*void type is to avoid circular reference of headers

  RETURN VALUE
    None
===========================================================================*/
void qmi_nas_msgr_handler( void *param );

/*========================================================================
  FUNCTION qmi_nas_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void qmi_nas_msgr_register( msgr_client_t *msgr_client );

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_CDMA_AVOID_SYSTEM_RESP()

  DESCRIPTION
    Called when cmd_cb() supplied in cm_ph_cmd_avoid_system() is called 
    by CM. Sends response to control point.
        
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_cdma_avoid_system_resp
(
  void *                user_data,
  cm_ph_cmd_err_e_type  ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_SET_BUILTIN_PLMN_LIST_IND()

  DESCRIPTION
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_set_builtin_plmn_list_ind
(
  qmi_error_type_v01 err, enum qmi_nas_subs_e as_id
);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION  QMI_NAS_MCFG_REFRESH_EVT_CB

DESCRIPTION
   This function is the callback to process mcfg refresh ind.
   CB returns TRUE- MCFG will wait for DONE api.
   CB returns FALSE- MCFG will not wait for DONE api
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qmi_nas_mcfg_refresh_evt_cb
(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);
#endif // FEATURE_MODEM_CONFIG_REFRESH

#endif // !_QMI_NAS_INTERNAL_H
