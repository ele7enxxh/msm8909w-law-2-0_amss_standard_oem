#ifndef WMSI_H
#define WMSI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsi.h -- Internal type definitions

  This module defines the internal data types and functions that support the
  Short Message Services (SMS) UAPI implementation.

  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmsi.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "wms.h"
#include "wmspm.h"
#include "wmspriv.h"
#include "rex.h"
#include "queue.h"
#include "cm.h"

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_SMS_PWS_OVER_LTE
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_SMS_PWS_OVER_LTE */

#include "cm_msgr_msg.h"

#include "mmoc_msgr.h"

#include "cm_dualsim.h"

#include "nv.h"

#include "policyman_msg.h"

#ifdef FEATURE_WCDMA
#include "rrcmmif.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "tdsrrcmmif.h"
#endif /*FEATURE_TDSCDMA*/

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
=============================================================================
========================  DATA TYPES  =======================================
=============================================================================
===========================================================================*/

enum { WMS_MAX_CMDS  = 30 };
enum { WMS_MAX_RPTS  = 20 };

enum { WMS_MAX_RAM_MESSAGES = 75 };
enum { WMS_MAX_RAM_STATUS_REPORTS = 30 };

enum { WMS_MAX_NV_CDMA_MESSAGES = 99 }; /* slots 0..98 */
enum { WMS_MAX_GCF_NV_GW_MESSAGES = 23 }; /* slots 0..22 */
enum { WMS_MAX_NV_GW_MESSAGES   = 255 }; /* slots 0..254 */

enum { WMS_MAX_CDMA_MESSAGES_IN_MEM =
         WMS_MAX_NV_CDMA_MESSAGES + WMS_MESSAGE_LIST_MAX };
enum { WMS_MAX_GW_MESSAGES_IN_MEM =
         WMS_MAX_NV_GW_MESSAGES + WMS_MESSAGE_LIST_MAX + WMS_MAX_RAM_MESSAGES };

enum { WMS_SIM_CBMI_LIST_MAX  = 127 };
enum { WMS_SIM_CBMIR_LIST_MAX = 63 };
enum { WMS_SIM_CBMID_LIST_MAX = 127 };

enum { WMS_GW_CB_MAX_LANGUAGES = 255 };

enum { WMS_UDH_OTHER_SIZE_GW = 137 }; /* 140 - 1(UDHL) - 1(UDH ID) - 1(UDH Len) */

enum { WMS_SIM_MWI_REC_LEN_MAX = 6 };

enum { WMS_SIM_MWI_REC_LEN_MIN = 5 };

typedef enum
{
  WMS_DEVICE_MODE_SS   = 0,
  WMS_DEVICE_MODE_DSDS = 1,
  WMS_DEVICE_MODE_DSDA = 2,
  WMS_DEVICE_MODE_TSTS = 3,
  WMS_DEVICE_MODE_NUM_MODES  // Must be after last valid mode
}wms_device_mode_e_type;

#ifdef FEATURE_CSIM
#define WMS_CARD_NUM_REFRESH_FILES 17
#else /* FEATURE_CSIM */
#define WMS_CARD_NUM_REFRESH_FILES 14
#endif /* FEATURE_CSIM */

/* SIM FILE INDICES - These indices are used to map to the Init state of corresponding SIM EF file.
*/
enum
{
  CBMI_FILE_INDEX,
  CBMIR_FILE_INDEX,
  CBMID_FILE_INDEX,
  SMSS_FILE_INDEX ,
  SMSP_FILE_INDEX,
  SMS_FILE_INDEX,
  SMSR_FILE_INDEX,
  MWIS_FILE_INDEX,
  MAX_FILE_INDEX
};

/* SMS task signals
*/
enum
{
  WMS_CMD_Q_SIG               =  0x001,
  WMS_MT_ACK_TIMER_SIG        =  0x002,
  WMS_DC_CALL_IN_TIMER_SIG    =  0x008,
  WMS_DC_CALL_OUT_TIMER_SIG   =  0x010,
  WMS_DC_IDLE_TIMER_SIG       =  0x020,
  WMS_AWISMS_ACK_TIMER_SIG    =  0x040,
  WMS_NV_SIG                  =  0x080,
  WMS_RPT_Q_SIG               =  0x100,
  WMS_RPT_TIMER_SIG           =  0x200,
  WMS_DELAY_TIMER_SIG         =  0x400,
  WMS_RUIM_ACCESS_SIG         =  0x800,
  WMS_RESERVED_SIG            =  0x1000, /* reserved for SIM server */
  WMS_MMGSDI_RESPONSE_SIG     =  0x2000,
  WMS_CM_RESPONSE_SIG         =  0x8000,
  WMS_MO_RETRY_TIMER_SIG      =  0x10000,
  WMS_MSGR_Q_SIG              =  0x20000,
  WMS_SIM_BUSY_SIG            =  0x40000
};

/* SIM/RUIM EFsmss memory full flag
*/
typedef enum
{
  WMS_SIM_MEM_FULL_MASK   = 0x01,
  WMS_SIM_MEM_FULL_CLEAR  = 0x01,
  WMS_SIM_MEM_FULL_SET    = 0xFE
} wms_sim_mem_full_flag_e_type;

typedef struct
{
  boolean                                nv_mode; /* whether 1x subs is in nv mode*/

  sys_modem_dual_standby_pref_e_type     standby_pref;
      /* Standby preference of the Phone */

  sys_modem_as_id_e_type                 active_subs;
      /* The active subscription in Single Standby mode
      ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY
      */

  sys_modem_as_id_e_type                 default_voice_subs;
      /* default voice subscription */

  sys_modem_as_id_e_type                 default_data_subs;
      /* default data subscription */

  sys_modem_as_id_e_type                 priority_subs;

} wms_cm_ph_info_s_type;

/*=========================================================================
STRUCT: wms_cm_subs_pref_info_s_type
 
 Holds the state of the current subscription pref info.
 Each time a SUBSCRIPTION_PREF_INFO event arrives, the previous state
 is needed to compare with in order to detect transitions between the
 available and unavailable states.
=========================================================================*/
typedef struct
{
  struct
  {
    mmgsdi_session_type_enum_type          session_type_1x;
    mmgsdi_session_type_enum_type          is_available_1x;
    mmgsdi_session_type_enum_type          session_type_gw;
    mmgsdi_session_type_enum_type          is_available_gw;
    boolean                                is_operational;
  } sub[MAX_AS_IDS];
} wms_cm_subs_pref_info_s_type;

/* MMGSDI Session State */
typedef enum
{
   WMS_SIM_MMGSDI_SESSION_CLOSED = 0,
   WMS_SIM_MMGSDI_SESSION_OPENING,
   WMS_SIM_MMGSDI_SESSION_OPENED
} wms_sim_mmgsdi_session_state_e_type;

/* Relevant data when doing the SEEK operation */
typedef struct
{
   wms_status_e_type   status;               /* status of the SEEK operation */
   uint8               *in_use_records;      /* Each index specifies whether the record number
                                                corresponding to that index is filled or empty */
   int                 num_of_records;       /* size of in_use_records[] */
   int                 record_length;        /* length of each record */
} wms_seek_data_type;

typedef struct {
  sys_modem_as_id_e_type               subscription_id;
  mmgsdi_session_type_enum_type        session_type;
  boolean                              is_available;
} wms_cm_subs_info_s_type;

typedef struct
{
  mmgsdi_len_type    data_len;
  uint8             *data_ptr;
} wms_raw_smsp_data_rec_s_type;

#ifdef FEATURE_GWSMS_BROADCAST
typedef struct wms_gw_bc_info_s
{
  /* GW Cell Broadcast (CB) information */
  wms_bc_config_e_type             gw_cb_config;
  wms_bc_pref_e_type               gw_cb_pref;                       // ::protected by wms_bc_crit_sect
  uint16                           gw_cb_table_size;                 // ::protected by wms_bc_crit_sect
  uint16                           gw_cb_num_active;                 // Number of services active
  uint16                           gw_cb_num_selected;               // Number of services selected
  boolean                          gw_cb_enable;                     // ::protected by wms_bc_crit_sect
  nv_sms_gw_cb_service_table_type  gw_cb_table[WMS_BC_MM_TABLE_MAX]; // ::protected by wms_bc_crit_sect
  boolean                          in_nv[WMS_BC_MM_TABLE_MAX];       // Indicates gw_cb_table[] entry is in NV
  boolean                          in_sim[WMS_BC_MM_TABLE_MAX];      // Indicates gw_cb_table[] entry is in sim

  uint16                           gw_cb_languages[WMS_GW_CB_MAX_LANGUAGES];

  boolean                          reading_advised;
  boolean                          reading_optional;

  /* EFcbmi information */
  uint32                           sim_cbmi_rec_len;
  uint8                            sim_cbmi_list_size;
  wms_gw_cb_srv_id_type            sim_cbmi_list[WMS_SIM_CBMI_LIST_MAX];

  /* EFcbmir information */
  uint32                           sim_cbmir_rec_len;
  uint8                            sim_cbmir_list_size;
  wms_gw_cb_srv_range_s_type       sim_cbmir_list[WMS_SIM_CBMIR_LIST_MAX];

  /* EFcbmid information */
  uint32                           sim_cbmid_rec_len;
  uint8                            sim_cbmid_list_size;
  wms_gw_cb_srv_id_type            sim_cbmid_list[WMS_SIM_CBMID_LIST_MAX];
} wms_gw_bc_info_s_type;
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_CDSMS_BROADCAST
typedef struct wms_cd_bc_info_s
{
  /* CDMA Cell Broadcast Service Table */
  wms_bc_config_e_type                  bc_config;
  wms_bc_pref_e_type                    bc_pref;                    // ::protected by wms_bc_crit_sect
  uint16                                bc_table_size;              // ::protected by wms_bc_crit_sect
  uint16                                bc_num_active;              // Number of services active
  nv_sms_bc_service_table_type          bc_table[WMS_BC_TABLE_MAX]; // ::protected by wms_bc_crit_sect
  uint16                                bc_num_selected;            // Number of services selected
  boolean                               bc_enable;                  // ::protected by wms_bc_crit_sect
  nv_sms_bc_service_table_options_type  bc_table_options[WMS_BC_TABLE_MAX];
} wms_cd_bc_info_s_type;
#endif /* FEATURE_CDSMS_BROADCAST */

typedef struct
{
  sys_modem_as_id_e_type       asubs_id;
  boolean                      is_stack_operational;
  boolean                      sys_srv_has_changed;
  sys_sys_mode_e_type          sys_mode;
  sys_srv_domain_e_type        srv_domain;
  sys_srv_status_e_type        srv_status;
  sys_roam_status_e_type       roam_status;
  sys_sys_id_s_type            sys_id;

} wms_ss_stack_info_s_type;

typedef struct
{
  // MMGSDI state
  mmgsdi_session_id_type               session_id;        // Unique session id provided by MMGSDI
  mmgsdi_session_type_enum_type        session_type;      // Subscription session type (1X_PRI, GW_SEC, etc)
  mmgsdi_app_enum_type                 app_type;          // Subscription APP type from SUBS_READY (SIM, USIM, CSIM, etc)
  mmgsdi_slot_id_enum_type             slot_id;           // Subscription slot id from SUBS_READY
  wms_sim_mmgsdi_session_state_e_type  session_state;     // MMGSDI session state (CLOSED, OPENING, OPEN)
  wms_cfg_efs_read_evt_s_type          sim_file_read_complete_param;
} wms_prov_mmgsdi_app_info_s_type;

typedef struct
{
  // MMGSDI state
  boolean                              session_activated; // MMSGDI session is mapped to a provisioning app
  boolean                              sim_init_complete; // SIM initialization is completed
  wms_prov_mmgsdi_app_info_s_type      mmgsdi_appInfo;    // MMGSDI Session APP Information
  
  // CM state
  sys_modem_as_id_e_type               asid;              // Modem AS id from CM SUBS_PREF event (maps session to AS)
  boolean                              is_available;      // CM subs is available for use

  boolean                              in_roaming;
  boolean                              bcsms_enabled;
  boolean                              smscap_enabled;
  boolean                              fdn_enabled;
  boolean                              mo_control_enabled;
  boolean                              mo_waiting_for_gstk_resp;
  boolean                              pp_data_download_enabled;
  boolean                              cb_data_download_enabled;
  mmgsdi_file_enum_type                sms_file_enum;
  mmgsdi_rec_num_type                  sms_rec_count;
  mmgsdi_len_type                      sms_rec_len;
  wms_message_tag_e_type               sms_rec_tags[WMS_MESSAGE_LIST_MAX];
  wms_message_number_type              sms_reference[WMS_MESSAGE_LIST_MAX];
  wms_message_index_type               voice_mail_index;
  boolean                              voice_mail_active;
  wms_seek_data_type                   wms_seek_data;

  mmgsdi_file_enum_type                smsp_file_enum;
  mmgsdi_rec_num_type                  smsp_rec_count;
  mmgsdi_len_type                      smsp_rec_len;
  boolean                              smsp_rec_templates[WMS_MAX_TEMPLATES];
  wms_raw_smsp_data_rec_s_type         smsp_raw_rec[WMS_MAX_TEMPLATES];
  wms_gw_template_s_type               gw_template;
  wms_cdma_template_s_type             cdma_template;

  mmgsdi_file_enum_type                smss_file_enum;
  wms_message_number_type              smss_last_message_number;
  wms_message_number_type              smss_last_wap_message_number;
  uint8                                smss_mem_full_flag;

  mmgsdi_file_enum_type                smsr_file_enum;
  mmgsdi_rec_num_type                  smsr_rec_count;
  mmgsdi_len_type                      smsr_rec_len;
  boolean                              smsr_status_rpts[WMS_MESSAGE_LIST_MAX];
  wms_message_index_type               smsr_sms_index[WMS_MESSAGE_LIST_MAX];

 
  mmgsdi_file_enum_type                mwis_file_enum;
  mmgsdi_rec_num_type                  mwis_rec_count;
  mmgsdi_len_type                      mwis_rec_len;

  mmgsdi_file_enum_type                cbmi_file_enum;

  mmgsdi_file_enum_type                cbmir_file_enum;
  mmgsdi_file_enum_type                cbmid_file_enum;
  mmgsdi_file_enum_type                lang_file_enum;
  mmgsdi_len_type                      lang_file_len;

  boolean                              refresh_in_progress;  //  This flag (refresh_in_progress) will be set when when a  refresh voting event arrives and WMS votes positively.
                                                             //  It remains set until refresh has entirely completed either successfully or unsuccessfully.
  mmgsdi_refresh_mode_enum_type        refresh_mode;
  boolean                              refresh_sc_addr_changed;

  boolean                              refresh_started;  //  This flag (refresh_started) will be set when refresh_start event arrives and remains set until refresh has entirely completed.
  boolean                              refresh_voted_for_fcn;

  wms_gw_domain_pref_e_type            gw_domain_pref;
  wms_gw_domain_pref_e_type            curr_gw_domain_pref;
  boolean                              gw_domain_pref_has_been_read;

  /* link control information */
  wms_cfg_link_control_mode_e_type     link_control_mode;
  wms_cfg_link_control_status_e_type   link_control_status;
  uint8                                link_control_timer;

  wms_client_id_type                   primary_client;
  boolean                              use_client_memory;
  boolean                              client_memory_full;

  boolean                              limited_bc_config_sent;
  union
  {
    /* Cell Broadcast service table data structure
     *
     * Defines the internal structure used to hold the runtime cell broadcast
     * configuration.  There are two groups of entries within the structure, one
     * is for CDMA, the other for GW.  The table contents are initially read from
     * NV and SIM card entries, depending on the technology.  After initialization
     * entries can be modified via client interaction, card refresh, etc.
     *
     * Under most conditions, WMS clients will interact with WMS via the public
     * WMS API. Some use cases require that fast, non-blocking searches of the
     * table data be performed, so a set of internal, non-public API's exists
     * (defined in wms_modem.h) for use by internal modem clients.
     * The three API methods are:
     *    wms_get_1x_bc_config()
     *    wms_bc_addr_match()
     *    wms_bc_gw_cb_filter()
     *
     * These API methods directly access internal table data, bypassing the WMS
     * command processing queue. Because they can be called asynchronously from
     * normal WMS processing, the table elements they reference must be maintained
     * and accessed in a manner such that they are stable/consistent while being
     * accessed through the internal methods.
     *
     * Protected table entries:
     *    bc_pref             (CDMA, single variable)
     *    bc_enable           (CDMA, single variable)
     *    bc_table_size       (CDMA, relates to bc_table)
     *    bc_table
     *    gw_cb_pref          (GW, single variable)
     *    gw_cb_enable        (GW, single variable)
     *    gw_cb_table_size    (GW, relates to gw_cb_table)
     *    gw_cb_table
     *
     * A critical section (see definition of wms_bc_crit_sect later in the code)
     * is defined to protect the data accesses.
     */
#ifdef FEATURE_CDSMS_BROADCAST
    wms_cd_bc_info_s_type              cd;
#endif /* FEATURE_CDSMS_BROADCAST */
#ifdef FEATURE_GWSMS_BROADCAST
    wms_gw_bc_info_s_type              gw;
#endif /* FEATURE_GWSMS_BROADCAST */
  } bc;

  boolean                              sim_file_read_is_complete;
  boolean                              mmgsdi_is_busy;
  boolean                              sim_command_is_queued;
  mmgsdi_rec_num_type                  accessed_rec_num; // Record number being accessed currently for linear fixed or cyclic files
  boolean                              lte_is_available; 
  int8                                 number_of_stacks;
  wms_ss_stack_info_s_type             stack_info[CM_NO_STACKS];
  
  boolean                              srv_available;
  boolean                              sim_file_init_is_done[MAX_FILE_INDEX]; // SIM files INIT states.   
  wms_memory_store_e_type              gw_template_mem_store; // Default SMSC memory store (NV/SIM).
  uint8                                gw_template_index;     // Default SMSC storage index. 
  boolean                              ecbm_mode_is_enabled;
  boolean                              mt_msg_rec_count_in_ecbm;
  uint8                                mo_sms_count;
} wms_prov_app_info_s_type;


typedef struct
{
  mmgsdi_event_data_type  event;
} wms_cmd_mmgsdi_event_cb_s_type;

typedef struct
{
  mmgsdi_cnf_enum_type    cnf;
  mmgsdi_cnf_type         cnf_type;
} wms_cmd_mmgsdi_response_cb_s_type;

typedef struct
{
  mmgsdi_cnf_enum_type    cnf;
  mmgsdi_cnf_type         cnf_type;
} wms_cmd_mmgsdi_seek_cb_s_type;
typedef struct
{
  cm_ph_event_e_type      ph_event;
  cm_ph_info_s_type       ph_info;
} wms_cmd_cm_ph_cb_s_type;

typedef struct
{
  cm_ph_event_e_type      subs_event;
  cm_subs_info_s_type     subs_info;
} wms_cmd_cm_subs_cb_s_type;

typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;
} wms_policyman_cfg_s_type;

#define WMS_SIGNALS ( WMS_CMD_Q_SIG \
                        | WMS_MT_ACK_TIMER_SIG \
                        | WMS_DC_CALL_OUT_TIMER_SIG \
                        | WMS_DC_IDLE_TIMER_SIG \
                        | WMS_AWISMS_ACK_TIMER_SIG \
                        | WMS_RPT_Q_SIG \
                        | WMS_MSGR_Q_SIG \
                        | WMS_RPT_TIMER_SIG \
                        | WMS_MO_RETRY_TIMER_SIG )



enum{ WMS_TIMER_RETRY_INTERVAL  = (5) /* seconds */ };
enum{ WMS_MAX_RETRY_PERIOD = (240) /* seconds */ };
enum{ WMS_MAX_ACK_RETRY_PERIOD_JCDMA = (18) /* seconds */ };
enum{ WMS_MAX_ACK_RETRY_PERIOD = (30) /* seconds */ };
// Max wait time for MO status/report indication from lower layer
// if WMS retry logic is not enabled.
enum{ WMS_MAX_MO_STATUS_WAIT_TIME = (60) /* seconds */ };
#ifdef FEATURE_CDSMS

/* Timer values
*/
enum{ WMS_TIMER_DC_CALL_OUT    = ( 20 * 1000 )  /* ms */ };

enum{ WMS_TIMER_MT_ACK          = 2000  /* ms */ };

enum{ WMS_TIMER_AWISMS_ACK      = 700  /* ms */ };

enum{ WMS_TIMER_MO_ACK          = ( 20 * 1000 )  /* ms */ };


#endif /* FEATURE_CDSMS */


/* Define the usage preference for SMS bearers
*/
typedef enum
{
  WMS_BEARER_USAGE_PREFERRED,
  WMS_BEARER_USAGE_ALLOWED,
  WMS_BEARER_USAGE_BLOCKED,
  WMS_BEARER_USAGE_MAX,
  WMS_BEARER_USAGE_MAX32       = 0x1FFFFFFF
} wms_bearer_usage_e_type;

/* Define the SMS Bearer Preference array
*/
typedef wms_bearer_usage_e_type wms_bearer_prefs_arr_type[WMS_BEARER_MAX];



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Generic report header type - all reportss to WMS must start with this */
typedef struct
{
  q_link_type          link;         /* Queue link */
  rex_tcb_type         *task_ptr;    /* Pointer to requesting task TCB */
  rex_sigs_type        sigs;         /* Signal to set upon cmd completion */
  q_type               *done_q_ptr;  /* Queue to place this cmd on when done */

  wms_cmd_id_e_type         cmd_id;
  wms_client_id_type        client_id;
  wms_cmd_cb_type           cmd_cb;
  void                     *user_data;
  sys_modem_as_id_e_type    as_id;
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_prov_mmgsdi_app_info_s_type      *mmgsdi_appInfo;
} wms_cmd_hdr_type;



/*-------------------------------------------------------------------------*/

/* Configuration group commands
*/
typedef struct
{
  wms_routes_s_type                routes;
} wms_cmd_cfg_set_routes_type;


typedef struct
{
  int dummy;
} wms_cmd_cfg_get_routes_type;


typedef struct
{
  wms_memory_store_e_type          mem_store;
  wms_message_tag_e_type           tag;
} wms_cmd_cfg_get_mem_status_type;


typedef struct
{
  wms_memory_store_e_type          mem_store;
  wms_message_tag_e_type           tag;
} wms_cmd_cfg_get_msg_list_type;

#ifdef FEATURE_GWSMS
typedef struct
{
  wms_gw_domain_pref_e_type        gw_domain_pref;
} wms_cmd_cfg_set_gw_domain_pref_type;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_DOMAIN_SELECTION
typedef struct
{
  sys_sms_domain_pref_e_type       lte_domain_pref;
} wms_cmd_cfg_set_lte_domain_pref_type;
#endif /* FEATURE_DOMAIN_SELECTION */

typedef struct
{
  boolean                          set_primary;
  boolean                          use_client_memory;
} wms_cmd_cfg_set_primary_client_type;

typedef struct
{
  boolean                          memory_full;
} wms_cmd_cfg_set_memory_full_type;

#ifdef FEATURE_GWSMS
typedef struct
{
  wms_cfg_link_control_s_type       link_control;
  uint8                 link_control_timer;
} wms_cmd_cfg_set_link_control_type;
#endif /* FEATURE_GWSMS */

typedef struct
{
  wms_gw_mwi_info_s_type  mwi_info[WMS_GW_MSG_WAITING_KIND_MAX];
  uint8                   num_mwi;
} wms_cmd_cfg_set_mwi_info_type;

/* Message group commands
*/
typedef struct
{
  wms_send_mode_e_type               send_mode;
  wms_client_message_s_type          message;
  boolean                            resend_msg;
  wms_message_number_type            client_msg_id;
  wms_msg_transport_type_e_type      transport_type;
#ifdef FEATURE_CDSMS
  wms_client_ch_pref_e_type          ch_pref;
  wms_dc_so_e_type                   dc_so;
  boolean                            follow_on_dc;
#endif /* FEATURE_CDSMS */
} wms_cmd_msg_send_type;


typedef struct
{
  wms_ack_info_s_type             ack_info;
  wms_msg_transport_type_e_type   transport_type;
#ifdef FEATURE_CDSMS
  wms_client_ch_pref_e_type       ch_pref;
#endif /* FEATURE_CDSMS */
} wms_cmd_msg_ack_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_type;


typedef struct
{
  wms_write_mode_e_type              write_mode;
  wms_client_message_s_type          message;
} wms_cmd_msg_write_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_tag_e_type          tag;
} wms_cmd_msg_delete_all_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
  wms_message_tag_e_type          tag;
} wms_cmd_msg_modify_tag_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_template_type;


typedef struct
{
  wms_write_mode_e_type            write_mode;
  wms_client_message_s_type        message;
} wms_cmd_msg_write_template_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_template_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
} wms_cmd_msg_delete_template_all_type;

typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_sts_report_type;


typedef struct
{
  wms_client_message_s_type        message;
  wms_write_mode_e_type            write_mode;
} wms_cmd_msg_write_sts_report_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_sts_report_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
} wms_cmd_msg_delete_sts_report_all_type;

typedef struct
{
   wms_msg_transport_id_type         transport_id;
   boolean                           cb_updated;
   wms_msg_transport_cap_type        transport_cap;
   wms_msg_transport_mo_sms_cb_type  mo_sms_cb;
} wms_cmd_msg_transport_reg_type;

#ifdef FEATURE_GSTK
typedef struct
{
  gstk_cmd_from_card_type   gstk_evt;
} wms_cmd_msg_gstk_evt_type;
#endif /* FEATURE_GSTK */


#ifdef FEATURE_CDSMS
/* DC commands
*/
typedef struct
{
  wms_dc_so_e_type     so;
} wms_cmd_dc_connect_type;

typedef struct
{
  uint32                 timeout;
} wms_cmd_dc_enable_auto_disc_type;
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_CDSMS_BROADCAST
typedef struct
{
  cm_sms_cmd_err_e_type       sms_cmd_err;
} wms_cmd_bc_enable_failure_type;
#endif /* FEATURE_CDSMS_BROADCAST */


#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

/* BC MM commands
*/
typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_config_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_pref_e_type              bc_pref;
} wms_cmd_bc_mm_set_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_table_type;


typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_srv_id_type           srv_id;
  boolean                         selected;
} wms_cmd_bc_mm_select_srv_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_all_srv_ids_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_srv_id_type           srv_id;
} wms_cmd_bc_mm_get_srv_info_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  uint8                           num_entries;
  wms_bc_mm_service_info_s_type   * entries;
} wms_cmd_bc_mm_add_srv_type;


typedef struct
{
  wms_message_mode_e_type         message_mode;
  uint8                           num_entries;
  wms_bc_mm_srv_id_type           * srv_ids;
} wms_cmd_bc_mm_delete_srv_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_service_info_s_type   srv_info;
} wms_cmd_bc_mm_change_srv_info_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_delete_all_srvs_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  boolean                         selected;
} wms_cmd_bc_mm_select_all_srvs_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_priority_e_type             priority;
} wms_cmd_bc_mm_set_priority_all_srvs_type;

typedef struct
{
  wms_gw_cb_page_header_s_type    page_hdr;
} wms_cmd_bc_mm_msg_delete_indication_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_reading_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  boolean                         reading_advised;
  boolean                         reading_optional;
} wms_cmd_bc_mm_set_reading_pref_type;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

typedef struct
{
  uint32                          retry_interval;
} wms_cmd_dbg_set_retry_interval_type;

typedef struct
{
  uint32                          retry_period;
} wms_cmd_dbg_set_retry_period_type;

#ifdef FEATURE_CDSMS

typedef struct
{
  wms_OTA_message_type      ota;
  wms_bearer_e_type         bearer;
  boolean                   ack_pending;
} wms_cmd_mc_mt_msg_type;


typedef struct
{
  wms_error_class_e_type        error_class;
  wms_status_e_type             status;
  wms_bearer_e_type             bearer;
} wms_cmd_mc_mo_status_type;

typedef struct
{
  uint8                        msg_count;
} wms_cmd_mc_mwi;

typedef struct
{
  cm_call_cmd_e_type          call_cmd;
  cm_call_cmd_err_e_type      call_cmd_err;
} wms_cmd_call_error_type;
#endif /* FEATURE_CDSMS */



#ifdef FEATURE_CDSMS_BROADCAST
typedef struct
{
  wms_tl_message_type   cdma_tl;
  wms_msg_transport_type_e_type transport_type;
  wms_client_ch_pref_e_type ch_pref;
} wms_cmd_bc_scpt_data_type;
#endif /* FEATURE_CDSMS_BROADCAST */


typedef struct
{
  boolean       in_lpm;
} wms_cmd_cm_oprt_mode;

typedef struct
{
  boolean                      umts_plmn_changed;
  cm_hs_based_plus_dial_e_type hs_based_plus_dial_setting;
  cm_country_code_type         home_mobile_country_code;
  cm_country_code_type         current_mobile_country_code;

  uint8                        number_of_stacks;
  wms_ss_stack_info_s_type     stack_info[CM_NO_STACKS];
} wms_cmd_ss_change_info_s_type;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
#define WMS_RRC_MAX_MCC_DIGITS         RRC_MAX_MCC_DIGITS
#define WMS_RRC_MAX_MNC_DIGITS         RRC_MAX_MNC_DIGITS
#define WMS_RRC_MAX_GSMMAP_LAC_LENGTH  RRC_MAX_GSMMAP_LAC_LENGTH

typedef struct
{
  /* Mobile Country Code  */
  uint8 mcc[WMS_RRC_MAX_MCC_DIGITS];

    /* Number of MNC digits */
  uint8 num_mnc_digits;

  /* Mobile Network Code */
  uint8 mnc[WMS_RRC_MAX_MNC_DIGITS];
}wms_plmn_identity_s_type;

typedef struct
{
  sys_modem_as_id_e_type        as_id;
  gstk_access_technology_type   rat;
  uint32                        cell_id;
  wms_plmn_identity_s_type      plmn_id;
  uint8                         lac[WMS_RRC_MAX_GSMMAP_LAC_LENGTH];
} wms_rrc_cell_info_s_type;

typedef struct
{
  wms_rrc_cell_info_s_type  cell_info;  
}wms_cmd_rrc_cb_s_type; 
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)) */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
typedef struct
{
  mcfg_refresh_info_s_type refresh_info;
} wms_cmd_nv_refresh_cb_s_type;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/* Union of all command types.  The header is always present and           */
/* specifies the command type and attributes.  If the command has          */
/* arguments, they follow in the union.                                    */
typedef struct
{
  wms_cmd_hdr_type                    hdr;       /* header */

  union
  {
    /* Configuration group commands
    */
    wms_cmd_cfg_set_routes_type       cfg_set_routes;
    wms_cmd_cfg_get_routes_type       cfg_get_routes;
    wms_cmd_cfg_get_mem_status_type   cfg_get_mem_status;
    wms_cmd_cfg_get_msg_list_type     cfg_get_msg_list;
    wms_cmd_cfg_set_mwi_info_type     cfg_set_mwi_info;
#ifdef FEATURE_GWSMS
    wms_cmd_cfg_set_gw_domain_pref_type cfg_set_gw_domain_pref;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_DOMAIN_SELECTION
    wms_cmd_cfg_set_lte_domain_pref_type cfg_set_lte_domain_pref;
#endif

    wms_cmd_cfg_set_primary_client_type cfg_set_primary_client;
    wms_cmd_cfg_set_memory_full_type  cfg_set_memory_full;
#ifdef FEATURE_GWSMS
    wms_cmd_cfg_set_link_control_type cfg_set_link_control;
#endif /* FEATURE_GWSMS */
    wms_cfg_efs_read_evt_s_type       cfg_send_sim_efs_read_complete_evt;

    /* Message group commands
    */
    wms_cmd_msg_send_type             msg_send;
    wms_cmd_msg_ack_type              msg_ack;
    wms_cmd_msg_read_type             msg_read;
    wms_cmd_msg_write_type            msg_write;
    wms_cmd_msg_delete_type           msg_delete;
    wms_cmd_msg_delete_all_type       msg_delete_all;
    wms_cmd_msg_modify_tag_type       msg_modify_tag;
    wms_cmd_msg_read_template_type    msg_read_template;
    wms_cmd_msg_write_template_type   msg_write_template;
    wms_cmd_msg_delete_template_type  msg_delete_template;
    wms_cmd_msg_delete_template_all_type msg_delete_template_all;
    wms_cmd_msg_read_sts_report_type       msg_read_sts_report;
    wms_cmd_msg_write_sts_report_type      msg_write_sts_report;
    wms_cmd_msg_delete_sts_report_type     msg_delete_sts_report;
    wms_cmd_msg_delete_sts_report_all_type msg_delete_sts_report_all;
    wms_cmd_msg_transport_reg_type         msg_transport_reg;

#ifdef FEATURE_GSTK
    wms_cmd_msg_gstk_evt_type              msg_gstk_evt;
#endif /* FEATURE_GSTK */


    /* DC commands
    */
#ifdef FEATURE_CDSMS
    wms_cmd_dc_connect_type           dc_connect;
    wms_cmd_dc_enable_auto_disc_type  dc_enable_auto_disc;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_CDSMS_BROADCAST
    wms_cmd_bc_enable_failure_type    bc_enable_failure; /* from CM */
    wms_cmd_bc_scpt_data_type         bc_scpt_data; /* from WMS itself */
#endif /* FEATURE_CDSMS_BROADCAST */


#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
    wms_cmd_bc_mm_get_config_type       bc_mm_get_config;
    wms_cmd_bc_mm_get_pref_type         bc_mm_get_pref;
    wms_cmd_bc_mm_set_pref_type         bc_mm_set_pref;
    wms_cmd_bc_mm_get_table_type        bc_mm_get_table;
    wms_cmd_bc_mm_select_srv_type       bc_mm_select_srv;
    wms_cmd_bc_mm_get_all_srv_ids_type  bc_mm_get_all_srv_ids;
    wms_cmd_bc_mm_get_srv_info_type     bc_mm_get_srv_info;
    wms_cmd_bc_mm_add_srv_type          bc_mm_add_srv;
    wms_cmd_bc_mm_delete_srv_type       bc_mm_delete_srv;
    wms_cmd_bc_mm_change_srv_info_type  bc_mm_change_srv_info;
    wms_cmd_bc_mm_delete_all_srvs_type  bc_mm_delete_all_srvs;
    wms_cmd_bc_mm_select_all_srvs_type  bc_mm_select_all_srvs;
    wms_cmd_bc_mm_set_priority_all_srvs_type bc_mm_set_priority_all_srvs;
    wms_cmd_bc_mm_msg_delete_indication_type bc_mm_msg_delete_indication;
    wms_cmd_bc_mm_get_reading_pref_type bc_mm_get_reading_pref;
    wms_cmd_bc_mm_set_reading_pref_type bc_mm_set_reading_pref;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


    /* DBG commands
    */
    wms_cmd_dbg_set_retry_interval_type dbg_set_retry_interval;
    wms_cmd_dbg_set_retry_period_type   dbg_set_retry_period;

#ifdef FEATURE_CDSMS
    /* events from MC through CM
    */
    wms_cmd_mc_mt_msg_type                  mt_msg;
    wms_cmd_mc_mo_status_type               mc_mo_status;
    wms_cmd_mc_mwi                          mwi;

    /* Call events from CM
    */
    wms_cmd_call_error_type                 call_error;
    cm_mm_call_info_s_type                  call_info;
#endif /* FEATURE_CDSMS */

    wms_cmd_mmgsdi_event_cb_s_type          mmgsdi_event_cb;
    wms_cmd_mmgsdi_response_cb_s_type       mmgsdi_response_cb;
    wms_cmd_mmgsdi_seek_cb_s_type           mmgsdi_seek_cb;
    wms_cmd_cm_oprt_mode                    oprt_mode;

    wms_cmd_ss_change_info_s_type           ss_change_info;
    cm_ipapp_info_s_type                    ipapp_change_info;
#ifdef FEATURE_FUZZ_TEST
    wms_msg_transport_rpt_ind_s_type        transport_rpt_ind;
#endif /* FEATURE_FUZZ_TEST */

    wms_cmd_cm_ph_cb_s_type                 cm_ph_cb;
    wms_cmd_cm_subs_cb_s_type               cm_subs_cb;

#if (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA))
    wms_cmd_rrc_cb_s_type                   rrc_cb;
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
    wms_cmd_nv_refresh_cb_s_type            nv_refresh_info;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  } cmd;

}  wms_cmd_type;



typedef union wms_msgr_cmd_u {
  msgr_hdr_s                  msg_hdr;  /*!< Message router header */
#ifdef FEATURE_SMS_PWS_OVER_LTE
  lte_rrc_etws_prim_ind_s     lte_etws_prim;
  lte_rrc_etws_sec_ind_s      lte_etws_sec;
  lte_rrc_cmas_ind_s          lte_cmas;
  lte_rrc_service_ind_s       lte_service_ind; /* Notification from LTE RRC for Cell change event (PLMN/ TAC/ Cell Change) */
#endif /* FEATURE_SMS_PWS_OVER_LTE */
  cm_sds_rsp_u_type           dom_sel_cmd;
  mmoc_offline_ind_s_type     offline_ind;     /* Notification sent from MMOC when it goes to OFFLINE */
  wms_policyman_cfg_s_type    policyman_cfg;
} wms_msgr_cmd_u_type;

/* Union of all command types.  The header is always present and           */
/* specifies the command type and attributes.  If the command has          */
/* arguments, they follow in the union.                                    */
typedef struct
{
  q_link_type          link;         /* Queue link */
  byte                 reserved[4];
  wms_msgr_cmd_u_type  cmd;          /* CM command information */
} wms_msgr_cmd_s_type;

/* Event payload to Diag
*/
typedef PACKED struct
{
  uint8    mem_type;
  uint32   msg_index;
} wms_evt_mt_sms_notify_s_type;

typedef PACKED struct
{
  uint32   transaction_id;
  uint8    mem_type;
  uint32   msg_index;
  uint8    report_status;
  uint8    cause_code;
} wms_evt_mo_sms_status_s_type;

typedef PACKED struct
{
  uint32   transaction_id;
  uint32   record_id;
  uint8    prev_report_status;
  uint8    prev_cause_code;
  uint32   retry_time_left;
} wms_evt_mo_sms_retry_attempt_s_type;

#ifdef FEATURE_GWSMS_BROADCAST

typedef PACKED struct
{
  uint8   num_selected;
} wms_evt_cb_sms_search_req_s_type;

#endif /*FEATURE_GWSMS_BROADCAST*/

#ifdef FEATURE_FUZZ_TEST
typedef PACKED struct
{
  uint32   cmd_id;
} wms_evt_sms_command_processed_s_type;

typedef PACKED struct
{
  uint8    user_data[16];
} wms_evt_sms_mt_notify_s_type;
#endif /* FEATURE_FUZZ_TEST */

#if defined(FEATURE_SMS_PWS_OVER_LTE) || (defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)) || defined(FEATURE_GWSMS_BROADCAST)
typedef PACKED struct
{
  uint16   serial_number;
  uint16   message_id;
  uint16   warning_type;
  boolean  security_info_present;
} wms_evt_etws_prim_msg_s_type;

typedef PACKED struct
{
  uint16   message_id;
  uint16   message_code;
  uint8    geo_scope;
  uint8    update_number;
  uint8    total_pages;
  uint8    page_number;
  uint8    dcs_data;
  boolean  user_alert;
  boolean  popup;
  uint8    format; // format (GSM or UMTS) used to deliver the message
  uint16   data_len;
} wms_evt_cbs_msg_s_type;

#endif /* defined(FEATURE_SMS_PWS_OVER_LTE) || (defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)) */


/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/
#ifndef FEATURE_CMI
void wms_task(
  uint32 dummy
);
#endif /* ifndef FEATURE_CMI */

/*=========================================================================
FUNCTION wms_client_cmd_status

DESCRIPTION
  Issue client command status (error or no-error) to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_client_cmd_status(
  wms_cmd_type                       *cmd_ptr,
  wms_cmd_err_e_type                  cmd_err
);

/*===========================================================================

FUNCTION wms_get_cmd_buf

DESCRIPTION
  Allocate a WMS Task command buffer.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  Pointer to allocated WMS command buffer.
  NULL pointer is returned if no more buffer is available.

SIDE EFFECTS
  Remove a command from the free queue.

===========================================================================*/
wms_cmd_type * wms_get_cmd_buf(
  void
);

/*===========================================================================
FUNCTION wms_put_cmd

DESCRIPTION
  The API functions calls uses this function to put commands to the WMS.

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Add a command to the command queue.

===========================================================================*/
void wms_put_cmd(
  wms_cmd_type                       *cmd_ptr
);

/*===========================================================================
FUNCTION wms_get_cmd_count

DESCRIPTION
  The API function call returns the number of commands to WMS

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  Count of the number of commands

SIDE EFFECTS
  None.

===========================================================================*/
int wms_get_cmd_count(
  void
);

/*===========================================================================
FUNCTION wms_kick_dog

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wms_kick_dog(
  void
);

/*===========================================================================
FUNCTION wms_signal_wait

DESCRIPTION
  Suspend WMS from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
wms_status_e_type wms_signal_wait(
  rex_sigs_type                       mask
);

/*===========================================================================
FUNCTION wms_init

DESCRIPTION
  Initialize the WMS module.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Initialization to internal data is done.

===========================================================================*/
void wms_init(
  void
);

/*=========================================================================
FUNCTION wms_process_signals

DESCRIPTION
  Process the signals for SMS related commands and timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_process_signals(
  rex_sigs_type                       sigs
);

/*==============================================================================
FUNCTION NAME WMS_CHECK_IDLE

DESCRIPTION
  Function that gets to check if WMS is idle or not
  for determining whether to refresh or not

RETURN VALUE
  TRUE: WMS is idle; refresh can proceed
  FALSE: WMS is not idle; refresh cannot proceed

PARAMETER
  allow_current_command: to return Idle even if the
  currently executed command is calling it
DEPENDENCIES
  None
==============================================================================*/
boolean wms_check_idle(
  sys_modem_as_id_e_type              as_id,
  boolean                             allow_current_command
);

sys_modem_as_id_e_type wms_get_1x_as_id(
  void
);

sys_modem_as_id_e_type wms_get_default_gw_as_id(
  void
);
sys_modem_as_id_e_type wms_get_as_id_from_message_mode(
  wms_message_mode_e_type message_mode
);

sys_modem_as_id_e_type wms_get_as_id_from_mem_store(
  wms_memory_store_e_type mem_store
);

sys_modem_as_id_e_type wms_get_appinfo_1x_asid(
  void
);

boolean wmsi_msg_check_for_validity(
  wms_message_mode_e_type             message_mode,
  wms_memory_store_e_type             mem_store
);


void wms_cfg_notify_mwi_data(
  uint8* sim_data ,
  wms_prov_app_info_s_type* appInfo_ptr ,
  wms_mwi_state_e_type state
);
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSI_H */
