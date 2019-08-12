#ifndef WMSCFG_H
#define WMSCFG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscfg.h -- Configuration Group Definitions

  The WMS module which implements the User API for SMS. This source file
  implements the configuration group functions which process the client
  commands of the configuration group.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscfg.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"
#include "wmsmsg.h"
#include "wmspriv.h"
#include "cm.h"

#if defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS_RUIM) || defined(FEATURE_CSIM)
#include "uim.h"
#endif

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif

#ifdef FEATURE_GWSMS
#include "CMCNas_MN.h"
#endif

/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/

/* MMGSDI Init Status */
typedef enum
{
   WMS_SIM_MMGSDI_NOT_INIT = 0,
   WMS_SIM_MMGSDI_INIT_IN_PROGRESS,
   WMS_SIM_MMGSDI_INIT_COMPLETE
} wms_sim_mmgsdi_init_state_e_type;

/* Configuration data
*/
typedef struct wms_cfg_struct
{
  wms_device_mode_e_type            device_mode; // Device Mode: 0=SS, 1=DSDS, 2=DSDA
  sys_ue_mode_e_type                ue_mode;
  sys_subs_feature_t                sub_feature_mode[MAX_AS_IDS];

  // Global NV cmd err
  wms_cmd_err_e_type                nv_cmd_err;

#ifdef FEATURE_GSTK
  /* Refresh Functionality common to both GW and CDMA */
  boolean                           refresh_registered;
  boolean                           refresh_in_progress;
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI
  wms_sim_mmgsdi_init_state_e_type  wms_sim_mmgsdi_init_state;
  mmgsdi_client_id_type             wms_sim_mmgsdi_client;
#endif /* FEATURE_MMGSDI */

  wms_cm_ph_info_s_type             wms_ph_info;
  wms_cm_subs_pref_info_s_type      wms_cm_subs_info;
  
  wms_prov_mmgsdi_app_info_s_type   wms_mmgsdi_app0_info;
  wms_prov_mmgsdi_app_info_s_type   wms_mmgsdi_app1_info;
  wms_prov_mmgsdi_app_info_s_type   wms_mmgsdi_app2_info;
  wms_prov_mmgsdi_app_info_s_type   wms_mmgsdi_app3_info;
  
  wms_prov_app_info_s_type         *wms_app0_info;  // 1X_PRI, subs1
  wms_prov_app_info_s_type         *wms_app1_info;  // GW_PRI, subs1
  wms_prov_app_info_s_type         *wms_app2_info;  // GW_SEC, subs2
  wms_prov_app_info_s_type         *wms_app3_info;  // GW_TER, subs3
  

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_DUP_DETECTION
  // Cache for duplicate detection processing.
  wms_msg_dup_detect_info_s_type   *dup_detect[WMS_MAX_CDMA_MESSAGES_IN_MEM];
#endif /* FEATURE_CDSMS_DUP_DETECTION */

  wms_message_index_type            cdma_voice_mail_index;
  boolean                           cdma_voice_mail_active;
  wms_message_number_type           cdma_last_message_number;

  // Message list for NV
  uint32                            nv_cdma_max_slots;
  wms_message_tag_e_type            nv_cdma_tags[WMS_MAX_NV_CDMA_MESSAGES];

  // Message storage in RAM
  //wms_msg_ruim_sms_s_type           ram_cdma_sms[WMS_MAX_RAM_MESSAGES];

  uint32                            phone_esn;
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
  boolean                           ens_flag;       /* read from NV */

#if defined (FEATURE_CPHS) && defined(FEATURE_ALS)
  wms_message_index_type            sim_voice_mail_index_line2;
  boolean                           sim_voice_mail_active_line2;
#endif /* defined (FEATURE_CPHS) && defined(FEATURE_ALS) */

  // Message list for NV
  uint32                            nv_gw_max_slots;
  wms_message_tag_e_type            nv_gw_tags[WMS_MAX_NV_GW_MESSAGES];

  // Message storage in RAM
  wms_msg_sim_sms_s_type           *ram_gw_sms[WMS_MAX_RAM_MESSAGES];

  boolean                           ram_status_rpts[WMS_MAX_RAM_STATUS_REPORTS];
  /* Status Report Storage in RAM */
  wms_msg_sim_smsr_s_type           ram_gw_smsr[WMS_MAX_RAM_STATUS_REPORTS];

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS
  /* link control information */
  wms_cfg_link_control_s_type       link_control;
  uint8                             link_control_timer;
#endif /* FEATURE_GWSMS */

  /* Bearers and Preferences
  */
  wms_bearer_prefs_arr_type         bearer_prefs;
  boolean                           active_bearers[WMS_BEARER_MAX];
  wms_bearer_e_type                 last_used_bearer;

  boolean                           lpm;

  cm_hs_based_plus_dial_e_type      hs_based_plus_dial_setting;
  cm_country_code_type              home_mobile_country_code;
  cm_country_code_type              current_mobile_country_code;

#ifdef FEATURE_CDSMS
  boolean                           mo_on_data_channel;
#endif

#ifdef FEATURE_GWSMS
  boolean                           enable_fdn_control;
  boolean                           store_to_sim_if_nv_full;
  boolean                           cs_domain_fallback_on_lte;
  sys_sys_mode_e_type               sys_mode;                       // System modes
  sys_sys_mode_e_type               gw_sys_mode;                    // HYBR System mode
  boolean                           is_hybr_gw_operational;
#endif

  boolean                           admin_sms_only; // only admin sms msg is allowed to be sent over IMS
  boolean                           simul_sms_voice_over_1x_allowed; // whether SMS is allowed to be send
                                                                // over 1x while voice call is active or not
  uint8                        sms_retry_limit; // Limit number of retries within the config retry timer interval                                                                 // over 1x while voice call is active or not
  boolean                      lte_cb_dup_detection_is_disabled; //whether LTE cell broadcast duplicate detection is disabled or not
  boolean                      sms_over_s102;
  sys_sms_domain_pref_e_type   lte_domain_pref;
  boolean                      presidential_bc_alert_is_disabled;   //whether presidential cell broadcast is disabled or not

  // Per Subs info
  // Routing configuration, per subs (modem stack)
  wms_routes_s_type                 routes[SYS_MODEM_AS_ID_MAX];

  gstk_gwl_loc_info                 umts_loc_info;

  wms_ss_stack_info_s_type          stack_info[CM_NO_STACKS];

  sys_modem_as_id_e_type            lte_sub_info;   //Tells on which sub, LTE_BEARER info received

  boolean                           stk_sms_retry_enable;        //whether STK SMS is enabled or not

  uint8                             mt_msg_limit_in_ecbm_mode;  // Maximum number of MT messages allowed in Emergency callback mode.
  boolean                           sms_allowed_in_ecbm;
  cm_hybr_pref_e_type               hybr_pref;
} wms_cfg_s_type;

/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_cfg_s_ptr

DESCRIPTION
  Return the pointer to the configuration data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal configuration data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cfg_s_type* wms_cfg_s_ptr
(
  void
);


void wms_cfg_do_memory_full
(
  sys_modem_as_id_e_type  as_id,
  wms_memory_store_e_type mem_store
);

/*===========================================================================
FUNCTION wms_cfg_process_cmd

DESCRIPTION
  Dispatch the configuration command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_process_cmd
(
  wms_cmd_type  *cmd_ptr
);

void wms_cfg_cleanup(
  void
);

/*
*/
void wms_cfg_pre_init
(
  void
);


/*===========================================================================
FUNCTION wms_cfg_init

DESCRIPTION
  Do configuration-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Configuration data initialized.
===========================================================================*/
void wms_cfg_init
(
  void
);



#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_cfg_get_ruim_next_message_number

DESCRIPTION
  Get the next message number from RUIM to use for an MO message.

DEPENDENCIES
  none

RETURN VALUE
  The next message number to use

SIDE EFFECTS
  The current message number is incremented.
===========================================================================*/
wms_message_number_type wms_cfg_get_ruim_next_message_number
(
  void
);
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_get_sim_next_message_number

DESCRIPTION
  Get the next message number from SIM to use for an MO message.

DEPENDENCIES
  none

RETURN VALUE
  The next message number to use

SIDE EFFECTS
  The current message number is incremented.
===========================================================================*/
wms_message_number_type wms_cfg_get_sim_next_message_number(
  wms_prov_app_info_s_type *appInfo_ptr
);
#endif /* FEATURE_GWSMS */



/*===========================================================================
FUNCTION wms_cfg_do_memory_status

DESCRIPTION
  Build data for memory status event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_memory_status(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_memory_store_e_type   mem_store,
  wms_message_tag_e_type    tag,
  wms_cmd_type             *cmd_ptr
);


#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_determine_pp_route

DESCRIPTION
  Determine message route according to message class.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
wms_routing_s_type wms_cfg_determine_pp_route(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_gw_deliver_s_type    *deliver
);

/*===========================================================================
FUNCTION wms_cfg_determine_sts_rpt_pp_route

DESCRIPTION
  Determine message route according to message class.

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_determine_sts_rpt_pp_route(
  wms_prov_app_info_s_type    *appInfo_ptr,
  wms_gw_status_report_s_type *status_report,
  wms_routing_s_type          *route,
  wms_message_index_type      *rec_id,
  wms_write_mode_e_type       *write_mode,
  wms_message_index_type      *smsr_index
);
#endif /* FEATURE_GWSMS */



void wms_cfg_update_voice_mail_index(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *sim_data,
  wms_message_index_type     i
);



/*===========================================================================
FUNCTION wms_cfg_do_message_list

DESCRIPTION
  Build data for message list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_message_list(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_memory_store_e_type     mem_store,
  wms_message_tag_e_type      tag,  
  wms_cmd_type       	     *cmd_ptr
);

/*===========================================================================
FUNCTION wms_cfg_do_template_list

DESCRIPTION
  Build data for template list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_template_list(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_memory_store_e_type    mem_store,
  wms_cmd_type              *cmd_ptr
);

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_do_sts_report_list

DESCRIPTION
  Build data for template list event from a memory store.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_sts_report_list(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_memory_store_e_type     mem_store,
  wms_cmd_type               *cmd_ptr
);
#endif /*FEATURE_GWSMS*/

#ifdef FEATURE_CDSMS

/* Update CDMA voice mail info
*/
void wms_cfg_cdma_update_voice_mail_index
(
  wms_format_e_type         format,
  wms_message_tag_e_type    tag,
  uint16                    data_len,
  const uint8                     * data,
  wms_message_index_type    i
);

/*===========================================================================
FUNCTION wms_cfg_update_cdma_memory_full_flag

DESCRIPTION
  Update memory full flag in RUIM/CSIM

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_cdma_memory_full_flag(
  wms_prov_app_info_s_type     *appInfo_ptr,
  wms_sim_mem_full_flag_e_type  mem_full_flag
);

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cfg_do_ready_event

DESCRIPTION
  Notifies UI when card has been initialised

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_do_ready_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
FUNCTION wms_cfg_report_not_ready_event

DESCRIPTION
  Notifies UI when WMS has not been initialised

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_report_not_ready_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
FUNCTION wms_cfg_report_sim_init_start_event

DESCRIPTION
  Notifies UI when WMS starts reading SIM files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_sim_init_start_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
FUNCTION wms_cfg_report_sim_init_complete_event

DESCRIPTION
  Notifies UI when WMS has finished reading SIM files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_sim_init_complete_event(
  wms_prov_app_info_s_type *appInfo_ptr,
  boolean                   sim_ready
);

/*===========================================================================
FUNCTION wms_cfg_report_start_refresh_event

DESCRIPTION
  Notifies UI when refresh process starts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_start_refresh_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
FUNCTION wms_cfg_report_end_refresh_event

DESCRIPTION
  Notifies UI when refresh process ends.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_end_refresh_event(
  wms_prov_app_info_s_type         *appInfo_ptr,
  boolean                          is_successful
);

/*===========================================================================
FUNCTION wms_cfg_report_network_service_avail_event

DESCRIPTION
  Notifies UI when whether WMS OTA is available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_network_service_avail_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

/*===========================================================================
FUNCTION wms_cfg_report_network_service_not_avail_event

DESCRIPTION
  Notifies UI when whether WMS OTA is unavailable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_report_network_service_not_avail_event(
  wms_prov_app_info_s_type *appInfo_ptr
);

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_link_control_ind_proc

DESCRIPTION
  Perform action on Link Control Indication

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_link_control_ind_proc(
  SM_RL_LINK_CONTROL_REPORT_T *ind_ptr
);

#endif /* FEATURE_GWSMS */


void wms_cfg_activate_client( wms_client_id_type client_id );

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

void wms_cfg_send_sim_file_read_complete_event_to_pbm(
  wms_prov_app_info_s_type  *appInfo_ptr
);


#ifdef FEATURE_CDSMS
void wms_cfg_update_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index,
  const wms_tl_message_type             *new_tl_msg
);

void wms_cfg_delete_dup_info_cache
(
  wms_memory_store_e_type         mem_store,
  uint32                          index
);
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
boolean wms_cfg_check_smsr_data(uint8 *smsr_data);

/*===========================================================================
FUNCTION wms_cfg_mm_set_domain_pref

DESCRIPTION
  Sets the GW Domain Preference to MM Layer

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean wms_cfg_mm_set_domain_pref
(
  sys_modem_as_id_e_type     as_id,
  wms_gw_domain_pref_e_type  gw_domain_pref
);
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
void wms_cfg_cell_change_ind_proc
(
  sys_modem_as_id_e_type as_id,
  cell_change_type_T     cb_cell_change
);
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_DOMAIN_SELECTION
/*===========================================================================
FUNCTION wms_cfg_set_lte_domain_pref_proc

DESCRIPTION
  Set the GW Domain Preference Procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_cfg_set_lte_domain_pref_proc(
  wms_cmd_type      *cmd_ptr
);
#endif /* FEATURE_DOMAIN_SELECTION */
/*=========================================================================
FUNCTION
wms_cfg_ss_change_info

DESCRIPTION
  Process SS info change.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ss_change_info
(
  wms_cmd_type *ss_info_ptr
);

/*=========================================================================
FUNCTION
wms_cfg_ipapp_change_info

DESCRIPTION
  Process IPApp info change.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_ipapp_change_info
(
  const cm_ipapp_info_s_type    * ipapp_info_ptr
);

/*=========================================================================
FUNCTION
  wms_cfg_lpm_proc

DESCRIPTION
  Process LPM event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updated CFG database.

=========================================================================*/
void wms_cfg_lpm_proc(
  wms_cmd_type    *cmd_ptr
);

/*===========================================================================
FUNCTION wms_cfg_check_1xsrlte_or_ehrpd_oper_mode

DESCRIPTION
  This function check wether a device operates in 1XSRLTE or not. 
 
DEPENDENCIES
  none

RETURN VALUE
  FALSE:  Not in 1XSRLTE mode
  TRUE:   In 1XSRLTE mode or EHRPD Mode
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_1xsrlte_or_ehrpd_oper_mode( 
sys_modem_as_id_e_type asid 
);

/*===========================================================================
FUNCTION wms_cfg_check_hvolte_oper_mode

DESCRIPTION
  This function check wether a device operates in hVOLTE or not. 
 
DEPENDENCIES
  none

RETURN VALUE
  FALSE:  Not in hVOLTE mode
  TRUE:   In hVOLTE mode 
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_hvolte_oper_mode( 
  void
);

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_cfg_update_msg_ref_for_mo_msg

DESCRIPTION
  This function fills in the appropriate slot of the message reference array of the
  configuration manager at startup. The message reference number is required only for
  those MO messages which are waiting on the STATUS report

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_msg_ref_for_mo_msg(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *sim_data,
  wms_message_index_type     i
);
/*===========================================================================
FUNCTION wms_cfg_update_mwis_info

DESCRIPTION
  This function updates the EFmwis with the MWI information.
  EFmwis file layout:
         Bytes  Description                                 M/O  Length
         1      Message Waiting Indicator Status            M    1 byte
         2      Number of Voicemail Messages Waiting        M    1 byte
         3      Number of Fax Messages Waiting              M    1 byte
         4      Number of Electronic Mail Messages Waiting  M    1 byte
         5      Number of Other Messages Waiting            M    1 byte
         6      Number of Videomail Messages waiting        O    1 byte

DEPENDENCIES
  none

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cfg_update_mwis_info(
  wms_prov_app_info_s_type      *appInfo_ptr,
  const wms_raw_ts_data_s_type  *message
);

void wms_cfg_gw_cb_error_ind_proc
(
  cb_error_indication_type * cb_error_ind_ptr
);

void wms_cfg_rrc_cell_change_cb_proc( 
  wms_cmd_type  *cmd_ptr 
);

/*=========================================================================
FUNCTION  wms_cfg_check_appport_reg_client

DESCRIPTION
  On MT SMS reception that has  UDH carrying application port ,check if any client registered for "application_port" 
  if it has some clients then check for the match of source & dest ports.
 
DEPENDENCIES
  none
 
RETURN VALUE
  Clinet id -- Which client is registered and matches the source and dest port. \n 
  WMS_CLIENT_TYPE_MAX -- None of the client is matched
 
SIDE EFFECTS
  None.
===========================================================================*/
wms_client_type_e_type wms_cfg_check_appport_reg_client
(
  const wms_client_message_s_type     *msg_ptr
);


#endif /* FEATURE_GWSMS */

#endif /* WMSCFG_H */
