/*===========================================================================

             N V   I T E M S   M E T A - C O M M E N T S   F I L E
DESCRIPTION

  This file contains HTORPC meta-comments that describe the union type
  nv_item_type defined in nv_items.h

REFERENCES
Copyright (c)  2005-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/nv_items_meta.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/07   hn      Exported NV_DS_AT_CSAS_[CSCA|CSCB|CSMP]_SETTINGS_I items.
09/19/07   hn      Exported NV_PROCESS_RESTART_SWITCH_I and NV_SMS_CFG_ROUTING_I
09/05/07   hn      Exported NV_DHCP4_OPTIONS_MASK_I and NV_DHCP6_OPTIONS_MASK_I.
09/05/07   hn      Exported NV_HS_USB_DIAG_ON_LEGACY_USB_PORT_I.
09/05/07   hn      Fixed typo in name of mm_speaker_lvl field.
08/22/07   hn      Exported NV_DS_AT_V250_REGISTERS_I and NV_DS_AT_S_REGISTERS_I.
08/21/07   hn      Exported NV_HDR_EMPA_SUPPORTED_I.
06/15/07   hn      Exported NV_ICMP6_ND_CONFIG_I.
06/15/07   hn      Exported NV_JCDMA_RUIM_ID_I and NV_JCDMA_UIM_LOCK_I items.
06/05/07   hn      Exported NV_DISPLAY_UPDATE_POWER_TEST_MODE_I.
05/24/07   hn      Exported NV_HS_USB_REMOTE_WAKEUP_DELAY_I item.
05/23/07   hn      Exported NV_DS707_GO_NULL_TIMER_.. items.
05/21/07   hn      Exported NV_DIAG_FTM_MODE_SWITCH_I item.
05/08/07   hn      Exported NV_WLAN_MULTIDOMAIN_CAPABILITY_PREF_I item.
04/11/07   hn      Exported NV_GPS1_PDE_ADDRESS_I item.
04/11/07   hn      Exported NV_GPS1_PDE_PORT_I item.
04/11/07   hn      Exported NV_GPS1_PDE_TRANSPORT_I item.
04/11/07   hn      Exported NV_GPS1_SEC_UPDATE_RATE_I item.
04/11/07   hn      Exported NV_GPSONE_PASSWORD_I item.
04/11/07   hn      Exported NV_RUIM_CHV_1_I item.
04/11/07   hn      Exported NV_RUIM_CHV_2_I item.
04/11/07   hn      Exported NV_HDRMRLP_NUM_BYTES_TO_LOG_I item.
04/11/07   hn      Exported NV_HDR_QOS_FLOW_PRIORITY_I item.
04/11/07   hn      Exported NV_IPV6_PRIVACY_EXTENSIONS_ENABLED_I item.
04/11/07   hn      Exported NV_IPV6_PRIVATE_ADDRESS_CONFIG_I item.
03/08/07   hn      Exported NV_FTM_MODE_I item.
01/22/07   hn      Exported NV_WLAN_ENABLE_BT_COEX_I item.
01/16/07   hn      Exported NV_HS_USB_CURRENT_COMPOSITION_I item.
12/08/06   hn      Exported NV_UMTS_NBNS_ENABLE_I item.
10/11/06   hn      Exported WLAN NV items.
10/10/06   hn      Exported 7200's NV items
10/05/06   hn      Exported NV items for BREW 4.0
09/15/06   hn      Exported NV_DCVS_ACPU_ENABLE item for Dynamic Voltag Scaling.
04/24/06   ~SN     Added VoIP NV Items.
06/20/06   dsn     Added NV_BD_ADDR_I to the list.
04/20/06   lp      Added metacomments for NV_JCDMA_M511_MODE_I, 
                   NV_JCDMA_M512_MODE_I, and NV_JCDMA_M513_MODE_I
06/02/05   hn      Added metacomments for MANUAL_PLMN & RTSP_PROXY_SERVER_ADDR
                   items.
05/17/05   hn      Added NV_DIAG_SPC_UNLOCK_TTL_I to the list.
05/16/05   cr      Added NV_PPP_CCP_REJECT_I to the list.
04/05/05   hn      First revision of this file.
===========================================================================*/

/*===========================================================================
  
  W A R N I N G   |   W A R N I N G   |   W A R N I N G   |   W A R N I N G

  Nothing should appear in this header file before the following block of
  HTORPC meta-comments except for regular C comments and #define's.
  (ie. no #include's, no C declarations etc.)

  W A R N I N G   |   W A R N I N G   |   W A R N I N G   |   W A R N I N G

===========================================================================*/

/*~ PARTIAL nv_item_type */

/*~ CASE NV_ACCOLC_I nv_item_type.accolc */
/*~ CASE NV_ACTIVE_EPZID_TYPE_I nv_item_type.active_epzid_type */
/*~ CASE NV_AIR_CNT_I nv_item_type.air_cnt */
/*~ CASE NV_AIR_TIMER_I nv_item_type.air_timer */
/*~ CASE NV_ALERTS_LVL_I nv_item_type.alerts_lvl */
/*~ CASE NV_ALERTS_LVL_SHADOW_I nv_item_type.alerts_lvl_shadow */
/*~ CASE NV_ANALOG_FIRSTCHP_I nv_item_type.analog_firstchp */
/*~ CASE NV_ANALOG_HOME_SID_I nv_item_type.analog_home_sid */
/*~ CASE NV_ANALOG_REG_I nv_item_type.analog_reg */
/*~ CASE NV_ANALOG_SID_ACQ_I nv_item_type.analog_sid_acq */
/*~ CASE NV_ANALOG_SID_LOCK_I nv_item_type.analog_sid_lock */
/*~ CASE NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I
    nv_item_type.auth_require_password_encryption */
/*~ CASE NV_AUTO_ANSWER_I nv_item_type.auto_answer */
/*~ CASE NV_AUTO_HYPHEN_I nv_item_type.auto_hyphen */
/*~ CASE NV_AUTO_LOCK_I nv_item_type.auto_lock */
/*~ CASE NV_AUTO_MUTE_I nv_item_type.auto_mute */
/*~ CASE NV_AUTO_NAM_I nv_item_type.auto_nam */
/*~ CASE NV_AUTO_REDIAL_I nv_item_type.auto_redial */
/*~ CASE NV_AUTO_VOLUME_ENABLED_I nv_item_type.auto_volume_enabled */
/*~ CASE NV_A_KEY_CHKSUM_I nv_item_type.a_key_chksum */
/*~ CASE NV_A_KEY_I nv_item_type.a_key */
/*~ CASE NV_BACK_LIGHT_HFK_I nv_item_type.back_light_hfk */
/*~ CASE NV_BACK_LIGHT_I nv_item_type.back_light */
/*~ CASE NV_BACK_LIGHT_INTENSITY_I nv_item_type.back_light_intensity */
/*~ CASE NV_BANNER_I nv_item_type.banner */
/*~ CASE NV_BD_ADDR_I nv_item_type.bd_addr */
/*~ CASE NV_BEEP_LVL_I nv_item_type.beep_level */
/*~ CASE NV_BEEP_LVL_SHADOW_I nv_item_type.beep_lvl_shadow */
/*~ CASE NV_BEEP_SDAC_LVL_I nv_item_type.beep_sdac_lvl */
/*~ CASE NV_BEEP_SPKR_LVL_I nv_item_type.beep_spkr_lvl */
/*~ CASE NV_CALL_BEEP_I nv_item_type.call_beep */
/*~ CASE NV_CALL_FADE_ALERT_I nv_item_type.call_fade_alert */
/*~ CASE NV_CALL_RSTRC_I nv_item_type.call_rstrc */
/*~ CASE NV_CALL_TIMER_G_I nv_item_type.call_timer_g */
/*~ CASE NV_CCBS_SEL_I nv_item_type.ccbs_sel */
/*~ CASE NV_CDMA_DYNAMIC_RANGE_I nv_item_type.cdma_dynamic_range */
/*~ CASE NV_CDMA_MIN_RX_RSSI_I nv_item_type.cdma_min_rx_rssi */
/*~ CASE NV_CDMA_RX_DIVERSITY_CTRL_I nv_item_type.cdma_rx_diversity_ctrl */
/*~ CASE NV_CDMA_SID_ACQ_I nv_item_type.cdma_sid_acq */
/*~ CASE NV_CDMA_SID_LOCK_I nv_item_type.cdma_sid_lock */
/*~ CASE NV_CONFIG_CHKSUM_I nv_item_type.config_chksum */
/*~ CASE NV_CONT_KEY_DTMF_I nv_item_type.cont_key_dtmf */
/*~ CASE NV_CONT_STR_DTMF_I nv_item_type.cont_str_dtmf */
/*~ CASE NV_CURR_NAM_I nv_item_type.curr_nam */
/*~ CASE NV_DATA_FORCE_CDMA_PKT_STR_I nv_item_type.data_force_cdma_pkt_str */
/*~ CASE NV_DATA_MDR_MODE_I nv_item_type.data_mdr_mode */
/*~ CASE NV_DATA_PKT_ORIG_STR_I nv_item_type.data_pkt_orig_str */
/*~ CASE NV_DATA_QNC_ENABLED_I nv_item_type.data_qnc_enabled */
/*~ CASE NV_DATA_SCRM_ENABLED_I nv_item_type.data_scrm_enabled */
/*~ CASE NV_DATA_SO_SET_I nv_item_type.data_so_set */
/*~ CASE NV_DATA_TRTL_ENABLED_I nv_item_type.data_trtl_enabled */
/*~ CASE NV_DIAG_DEFAULT_BAUDRATE_I nv_item_type.diag_default_baudrate */
/*~ CASE NV_DIAG_SPC_UNLOCK_TTL_I nv_item_type.diag_spc_unlock_ttl */
/*~ CASE NV_DIAL_I nv_item_type.dial */
/*~ CASE NV_DIR_NUMBER_I nv_item_type.dir_number */
/*~ CASE NV_DIR_NUMBER_PCS_I nv_item_type.mob_dir_number */
/*~ CASE NV_DISPLAY_DURATION_I nv_item_type.display_duration */
/*~ CASE NV_DM_ADDR_I nv_item_type.dm_addr */
/*~ CASE NV_DOMAIN_NAME_I nv_item_type.domain_name */
/*~ CASE NV_DS_DEFAULT_BAUDRATE_I nv_item_type.ds_default_baudrate */
/*~ CASE NV_DS_DEFAULT_INACTIVITY_RESTORE_TIMEOUT_I
    nv_item_type.ds_default_inactivity_restore_timeout */
/*~ CASE NV_DS_INCOMING_DATA_RLP_TIMEOUT_I
    nv_item_type.ds_incoming_data_rlp_timeout */
/*~ CASE NV_DS_MIP_2002BIS_MN_HA_AUTH_I nv_item_type.ds_mip_2002bis_mn_ha_auth */
/*~ CASE NV_DS_MIP_ACTIVE_PROF_I nv_item_type.ds_mip_active_prof */
/*~ CASE NV_DS_MIP_DMU_MN_AUTH_I nv_item_type.ds_mip_dmu_mn_auth */
/*~ CASE NV_DS_MIP_DMU_PKOID_I nv_item_type.ds_mip_dmu_pkoid */
/*~ CASE NV_DS_MIP_ENABLE_PROF_I nv_item_type.ds_mip_enable_prof */
/*~ CASE NV_DS_MIP_GEN_USER_PROF_I nv_item_type.ds_mip_gen_user_prof */
/*~ CASE NV_DS_MIP_MN_HA_TIME_DELTA_I nv_item_type.ds_mip_mn_ha_time_delta */
/*~ CASE NV_DS_MIP_PRE_RE_RRQ_TIME_I nv_item_type.ds_mip_pre_re_rrq_time */
/*~ CASE NV_DS_MIP_QC_DRS_OPT_I nv_item_type.ds_mip_qc_drs_opt */
/*~ CASE NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I
    nv_item_type.ds_mip_qc_handdown_to_1x_opt */
/*~ CASE NV_DS_MIP_RETRIES_I nv_item_type.ds_mip_retries */
/*~ CASE NV_DS_MIP_DEREG_RETRIES_I nv_item_type.ds_mip_dereg_retries */
/*~ CASE NV_DS_MIP_RETRY_INT_I nv_item_type.ds_mip_retry_int */
/*~ CASE NV_DS_MIP_RRQ_IF_TFRK_I nv_item_type.ds_mip_rrq_if_tfrk */
/*~ CASE NV_DS_MIP_RM_NAI_I nv_item_type.ds_mip_rm_nai */
/*~ CASE NV_DS_MIP_SS_USER_PROF_I nv_item_type.ds_mip_ss_user_prof */
/*~ CASE NV_DS_QCMIP_I nv_item_type.ds_qcmip */
/*~ CASE NV_DS_SIP_RM_NAI_I nv_item_type.ds_sip_rm_nai */
/*~ CASE NV_DTMF_I nv_item_type.dtmf */
/*~ CASE NV_EAR_LVL_I nv_item_type.ear_level */
/*~ CASE NV_EAR_LVL_SHADOW_I nv_item_type.ear_lvl_shadow */
/*~ CASE NV_ECC_LIST_I nv_item_type.ecc_list */
/*~ CASE NV_ERR_FATAL_OPTIONS_I nv_item_type.err_fatal_options */
/*~ CASE NV_ERR_LOG_I nv_item_type.err_log */
/*~ CASE NV_ESN_CHKSUM_I nv_item_type.esn_chksum */
/*~ CASE NV_ESN_I nv_item_type.esn */
/*~ CASE NV_FACTORY_INFO_I nv_item_type.fact_info */
/*~ CASE NV_FSC2_CHKSUM_I nv_item_type.fsc2_chksum */
/*~ CASE NV_FSC2_I nv_item_type.fsc2 */
/*~ CASE NV_FSC_I nv_item_type.fsc */
/*~ CASE NV_GPRS_ANITE_GCF_I nv_item_type.gprs_anite_gcf */
/*~ CASE NV_GSM_AMR_CALL_CONFIG_I nv_item_type.gsm_amr_call_config */
/*~ CASE NV_HDR_AN_AUTH_NAI_I nv_item_type.hdr_an_auth_nai */
/*~ CASE NV_HDR_AN_AUTH_PASSWD_LONG_I nv_item_type.hdr_an_auth_passwd_long */
/*~ CASE NV_HDR_AN_AUTH_PASSWORD_I nv_item_type.hdr_an_auth_password */
/*~ CASE NV_HDR_AN_PPP_PASSWORD_I nv_item_type.hdr_an_ppp_password */
/*~ CASE NV_HDR_RX_DIVERSITY_CTRL_I nv_item_type.hdr_rx_diversity_ctrl */
/*~ CASE NV_HOME_SID_NID_I nv_item_type.home_sid_nid */
/*~ CASE NV_HORN_ALERT_I nv_item_type.horn_alert */
/*~ CASE NV_HYSTERISIS_ACTIVATION_TIMER_I
    nv_item_type.hysterisis_activation_timer */
/*~ CASE NV_HYSTERISIS_TIMER_I nv_item_type.hysterisis_timer */
/*~ CASE NV_IMSI_11_12_I nv_item_type.imsi_11_12 */
/*~ CASE NV_IMSI_ADDR_NUM_I nv_item_type.imsi_addr_num */
/*~ CASE NV_IMSI_MCC_I nv_item_type.imsi_mcc */
/*~ CASE NV_IMSI_T_11_12_I nv_item_type.imsi_t_11_12 */
/*~ CASE NV_IMSI_T_ADDR_NUM_I nv_item_type.imsi_t_addr_num */
/*~ CASE NV_IMSI_T_MCC_I nv_item_type.imsi_t_mcc */
/*~ CASE NV_IMSI_T_S1_I nv_item_type.imsi_t_s1 */
/*~ CASE NV_IMSI_T_S2_I nv_item_type.imsi_t_s2 */
/*~ CASE NV_INI_MUTE_I nv_item_type.init_mute */
/*~ CASE NV_IPV6_ENABLED_I nv_item_type.ipv6_enabled */
/*~ CASE NV_IPV6_FAILOVER_CONFIG_I nv_item_type.ipv6_failover_config */
/*~ CASE NV_IPV6_PRIMARY_DNS_I nv_item_type.ipv6_primary_dns */
/*~ CASE NV_IPV6_SECONDARY_DNS_I nv_item_type.ipv6_secondary_dns */
/*~ CASE NV_IPV6_SM_CONFIG_I nv_item_type.ipv6_sm_config */
/*~ CASE NV_JCDMA_M511_MODE_I nv_item_type.jcdma_m511_mode */
/*~ CASE NV_JCDMA_M512_MODE_I nv_item_type.jcdma_m512_mode */
/*~ CASE NV_JCDMA_M513_MODE_I nv_item_type.jcdma_m513_mode */
/*~ CASE NV_KEY_SOUND_I nv_item_type.key_sound */
/*~ CASE NV_LANGUAGE_SELECTION_I nv_item_type.language_selection */
/*~ CASE NV_LAST_RX_DATA_COUNT_I nv_item_type.last_rx_data_count */
/*~ CASE NV_LAST_TX_DATA_COUNT_I nv_item_type.last_tx_data_count */
/*~ CASE NV_LCD_I nv_item_type.lcd */
/*~ CASE NV_LIFE_CNT_I nv_item_type.life_cnt */
/*~ CASE NV_LIFE_TIMER_G_I nv_item_type.life_timer_g */
/*~ CASE NV_LOCK_CODE_I nv_item_type.lock_code */
/*~ CASE NV_LOCK_I nv_item_type.lock */
/*~ CASE NV_MANUAL_PLMN_SEL_I nv_item_type.manual_plmn_sel */
/*~ CASE NV_MANUAL_PLMN_SELECTION_I nv_item_type.manual_plmn_selection */
/*~ CASE NV_MAX_TX_ADJ_I nv_item_type.max_tx_adj */
/*~ CASE NV_MEID_I nv_item_type.meid */
/*~ CASE NV_MENU_FORMAT_I nv_item_type.menu_format */
/*~ CASE NV_MIN1_I nv_item_type.min1 */
/*~ CASE NV_MIN2_I nv_item_type.min2 */
/*~ CASE NV_MIN_CHKSUM_I nv_item_type.min_chksum */
/*~ CASE NV_MM_LVL_I nv_item_type.mm_lvl */
/*~ CASE NV_MM_LVL_SHADOW_I nv_item_type.mm_lvl_shadow */
/*~ CASE NV_MM_RINGER_FILE_I nv_item_type.mm_ringer_file */
/*~ CASE NV_MM_SDAC_LVL_I nv_item_type.mm_sdac_lvl */
/*~ CASE NV_MM_SPEAKER_LVL_I nv_item_type.mm_speaker_lvl */
/*~ CASE NV_MOB_FIRM_REV_I nv_item_type.mob_firm_rev */
/*~ CASE NV_MOB_MODEL_I nv_item_type.mob_model */
/*~ CASE NV_MOB_TERM_FOR_NID_I nv_item_type.mob_term_for_nid */
/*~ CASE NV_MOB_TERM_FOR_SID_I nv_item_type.mob_term_for_sid */
/*~ CASE NV_MOB_TERM_HOME_I nv_item_type.mob_term_home */
/*~ CASE NV_NAME_NAM_I nv_item_type.name_nam */
/*~ CASE NV_NAM_CHKSUM_I nv_item_type.nam_chksum */
/*~ CASE NV_ONETOUCH_DIAL_I nv_item_type.onetouch_dial */
/*~ CASE NV_OTKSL_I nv_item_type.otksl */
/*~ CASE NV_PAGE_MSG_I nv_item_type.page_msg */
/*~ CASE NV_PAGE_SET_I nv_item_type.page_set */
/*~ CASE NV_PAP_PASSWORD_I nv_item_type.pap_password */
/*~ CASE NV_PAP_USER_ID_I nv_item_type.pap_user_id */
/*~ CASE NV_PCDMACH_I nv_item_type.pcdmach */
/*~ CASE NV_PDSN_AS_PROXY_IPV6_DNS_SERVER_I
    nv_item_type.pdsn_as_proxy_ipv6_dns_server */
/*~ CASE NV_PKT_DIAL_STRINGS_I nv_item_type.pkt_dial_strings */
/*~ CASE NV_PPP_CCP_REJECT_I nv_item_type.ppp_ccp_reject */
/*~ CASE NV_PPP_CONFIG_DATA_I nv_item_type.ppp_config_data */
/*~ CASE NV_PPP_PASSWORD_I nv_item_type.ppp_password */
/*~ CASE NV_PPP_USER_ID_I nv_item_type.ppp_user_id */
/*~ CASE NV_PREF_MODE_I nv_item_type.pref_mode */
/*~ CASE NV_PRIMARY_DNS_I nv_item_type.primary_dns */
/*~ CASE NV_PRL_ENABLED_I nv_item_type.prl_enabled */
/*~ CASE NV_PS_IPV6_IID_I nv_item_type.ps_ipv6_iid */
/*~ CASE NV_PWR_DWN_CNT_I nv_item_type.pwr_dwn_cnt */
/*~ CASE NV_RF_CAL_DATE_I nv_item_type.rf_cal_date */
/*~ CASE NV_RF_CAL_VER_I nv_item_type.rf_cal_ver */
/*~ CASE NV_RINGER_LVL_I nv_item_type.ringer_level */
/*~ CASE NV_RINGER_SPKR_LVL_I nv_item_type.ringer_spkr_lvl */
/*~ CASE NV_RINGER_TYPE_I nv_item_type.ringer_type */
/*~ CASE NV_RMNET_AUTOCONNECT_I nv_item_type.rmnet_autoconnect */
/*~ CASE NV_ROAMING_LIST_683_I nv_item_type.roaming_list_683 */
/*~ CASE NV_ROAMING_LIST_I nv_item_type.roaming_list */
/*~ CASE NV_ROAM_CNT_I nv_item_type.roam_cnt */
/*~ CASE NV_ROAM_RINGER_I nv_item_type.roam_ringer */
/*~ CASE NV_ROAM_TIMER_I nv_item_type.roam_timer */
/*~ CASE NV_RTRE_CONFIG_I nv_item_type.rtre_config */
/*~ CASE NV_RTSP_PROXY_SERVER_ADDR_ALTERNATE_I
    nv_item_type.rtsp_proxy_server_addr_alternate */
/*~ CASE NV_RTSP_PROXY_SERVER_ADDR_I nv_item_type.rtsp_proxy_server_addr */
/*~ CASE NV_RUN_TIMER_I nv_item_type.run_timer */
/*~ CASE NV_SCDMACH_I nv_item_type.scdmach */
/*~ CASE NV_SCM_I nv_item_type.scm */
/*~ CASE NV_SDAC_LVL_I nv_item_type.sdac_lvl */
/*~ CASE NV_SECONDARY_DNS_I nv_item_type.secondary_dns */
/*~ CASE NV_SEC_CODE_I nv_item_type.sec_code */
/*~ CASE NV_SEC_DEVICE_KEY_I nv_item_type.sec_device_key */
/*~ CASE NV_SID_NID_I nv_item_type.sid_nid */
/*~ CASE NV_SID_NID_LOCK_I nv_item_type.sid_nid_lock */
/*~ CASE NV_SLOT_CYCLE_INDEX_I nv_item_type.slot_cycle_index */
/*~ CASE NV_SMS_UTC_I nv_item_type.sms_utc */
/*~ CASE NV_SMS_VM_NUMBER_I nv_item_type.sms_vm_number */
/*~ CASE NV_SPARE_2_I nv_item_type.spare_2 */
/*~ CASE NV_SPEAKER_LVL_I nv_item_type.speaker_level */
/*~ CASE NV_SP_ECC_ENABLED_I nv_item_type.sp_ecc_enabled */
/*~ CASE NV_SSD_A_I nv_item_type.ssd_a */
/*~ CASE NV_SSD_B_I nv_item_type.ssd_b */
/*~ CASE NV_SSPR_P_REV_I nv_item_type.sspr_p_rev */
/*~ CASE NV_STACK_I nv_item_type.stack */
/*~ CASE NV_STACK_IDX_I nv_item_type.stack_idx */
/*~ CASE NV_SVC_AREA_ALERT_I nv_item_type.svc_area_alert */
/*~ CASE NV_SYSTEM_PREF_I nv_item_type.system_pref */
/*~ CASE NV_TCP_GRACEFUL_DORMANT_CLOSE_I
    nv_item_type.tcp_graceful_dormant_close */
/*~ CASE NV_TCP_KEEPALIVE_IDLE_TIME_I nv_item_type.tcp_keepalive_idle_time */
/*~ CASE NV_TIMED_PREF_MODE_I nv_item_type.timed_pref_mode */
/*~ CASE NV_TOTAL_RX_DATA_COUNT_I nv_item_type.total_rx_data_count */
/*~ CASE NV_TOTAL_TX_DATA_COUNT_I nv_item_type.total_tx_data_count */
/*~ CASE NV_TTY_I nv_item_type.tty */
/*~ CASE NV_USB_CURRENT_DEVICE_I nv_item_type.usb_current_device */
/*~ CASE NV_UE_IMEI_I nv_item_type.ue_imei */
/*~ CASE NV_UIM_PREF_PROTOCOL_I nv_item_type.uim_pref_protocol */
/*~ CASE NV_UP_KEY_I nv_item_type.up_key */
/*~ CASE NV_VOICE_PRIV_I nv_item_type.voice_priv */
/*~ CASE NV_WDC_I nv_item_type.wdc */
/*~ CASE NV_MEID_I nv_item_type.meid */
/*~ CASE NV_GAME_MODE_PERSIST_I nv_item_type.game_mode_persist */
/*~ CASE NV_GAME_MODE_I nv_item_type.game_mode */
/*~ CASE NV_BACK_LIGHT_INTENSITY_I nv_item_type.back_light_intensity */
/*~ CASE NV_AUTO_TIME_ENABLE_I nv_item_type.auto_time_enable */
/*~ CASE NV_NITZ_NW_INFO_I nv_item_type.nitz_nw_info */
/*~ CASE NV_SMS_MO_RETRY_PERIOD_I nv_item_type.sms_mo_retry_period */
/*~ CASE NV_SMS_MO_RETRY_INTERVAL_I nv_item_type.sms_mo_retry_interval */
/*~ CASE NV_QVP_APP_DEFAULT_CAPABILITY_TYPE_I
    nv_item_type.qvp_app_default_capability_type */
/*~ CASE NV_DQOS_SUPPORTED_I nv_item_type.dqos_supported */
/*~ CASE NV_HDRAMP_ADDRESS_DATA_I nv_item_type.hdramp_address_data */
/*~ CASE NV_HDR_AN_AUTH_USER_ID_LONG_I nv_item_type.hdr_an_auth_user_id_long */
/*~ CASE NV_VOIP_QOS_EVDO_SIP_PROFILE_ID_I
    nv_item_type.voip_qos_evdo_sip_profile_id */
/*~ CASE NV_VOIP_QOS_EVDO_RTP_PROFILE_ID_I
    nv_item_type.voip_qos_evdo_rtp_profile_id */
/*~ CASE NV_VOIP_QOS_EVDO_DSCP_I nv_item_type.voip_qos_evdo_dscp */
/*~ CASE NV_VOIP_DTX_ENABLED_I nv_item_type.voip_dtx_enabled */
/*~ CASE NV_VOIP_WLAN_PRFRD_CODEC_I nv_item_type.voip_wlan_prfrd_codec */
/*~ CASE NV_VOIP_EVDO_PRFRD_CODEC_I nv_item_type.voip_evdo_prfrd_codec */
/*~ CASE NV_VOIP_CONFRD_URI_I nv_item_type.voip_confrd_uri */
/*~ CASE NV_VOIP_REGISTRATION_MODE_I nv_item_type.voip_registration_mode */
/*~ CASE NV_VOIP_CALLERID_MODE_I nv_item_type.voip_callerid_mode */
/*~ CASE NV_SMS_MAX_PAYLOAD_LENGTH_I nv_item_type.sms_max_payload_length */
/*~ CASE NV_SMS_MO_ON_ACCESS_CHANNEL_I nv_item_type.sms_mo_on_access_channel */
/*~ CASE NV_SMS_MO_ON_TRAFFIC_CHANNEL_I nv_item_type.sms_mo_on_traffic_channel */
/*~ CASE NV_VOIP_QOS_ENABLED_I nv_item_type.voip_qos_enabled */
/*~ CASE NV_VOIP_READ_MEDIA_FILE_ENABLED_I
    nv_item_type.voip_read_media_file_enabled */
/*~ CASE NV_VOIP_WRITE_MEDIA_FILE_ENABLED_I
    nv_item_type.voip_write_media_file_enabled */
/*~ CASE NV_DCVS_ACPU_ENABLE_I nv_item_type.dcvs_acpu_enable */
/*~ CASE NV_BREW_CARRIER_ID_I nv_item_type.brew_carrier_id */
/*~ CASE NV_BREW_PLATFORM_ID_I nv_item_type.brew_platform_id */
/*~ CASE NV_BREW_BKEY_I nv_item_type.brew_bkey */
/*~ CASE NV_BREW_SERVER_I nv_item_type.brew_server */
/*~ CASE NV_BREW_DOWNLOAD_FLAGS_I nv_item_type.brew_download_flags */
/*~ CASE NV_BREW_AUTH_POLICY_I nv_item_type.brew_auth_policy */
/*~ CASE NV_BREW_PRIVACY_POLICY_I nv_item_type.brew_privacy_policy */
/*~ CASE NV_BREW_SUBSCRIBER_ID_I nv_item_type.brew_subscriber_id */
/*~ CASE NV_GPS1_LOCK_I nv_item_type.gps1_lock */
/*~ CASE NV_DATA_INCOMING_CSD_CALL_IS_INTERNAL_I
    nv_item_type.data_incoming_csd_call_is_internal */
/*~ CASE NV_DEVICE_SERIAL_NO_I nv_item_type.device_serial_no */
/*~ CASE NV_DIAG_SPC_TIMEOUT_I nv_item_type.diag_spc_timeout */
/*~ CASE NV_DS_ATCOP_RLP_VERSION_DEFAULT_I
    nv_item_type.ds_atcop_rlp_version_default */
/*~ CASE NV_DS_UCSD_NT_CONNECT_STATE_I nv_item_type.ds_ucsd_nt_connect_state */
/*~ CASE NV_DS_UCSD_RECOVERY_MODE_SREJ_I
    nv_item_type.ds_ucsd_recovery_mode_srej */
/*~ CASE NV_DS_UCSD_RLP_ADM_IDLE_I nv_item_type.ds_ucsd_rlp_adm_idle */
/*~ CASE NV_DS_UCSD_RLP_VERSION_GSM_I nv_item_type.ds_ucsd_rlp_version_gsm */
/*~ CASE NV_DS_UCSD_RLP_VERSION_WCDMA_I nv_item_type.ds_ucsd_rlp_version_wcdma */
/*~ CASE NV_EPLMN_ENABLED_I nv_item_type.eplmn_enabled */
/*~ CASE NV_GSM_A5_ALGORITHMS_SUPPORTED_I
    nv_item_type.gsm_a5_algorithms_supported */
/*~ CASE NV_HDRSCMDB_SECURE_CONFIG_I nv_item_type.hdrscmdb_secure_config */
/*~ CASE NV_LONG_PDPACT_TOLERANCE_FOR_TE_I
    nv_item_type.long_pdpact_tolerance_for_te */
/*~ CASE NV_SERVICE_DOMAIN_PREF_I nv_item_type.service_domain_pref */
/*~ CASE NV_SET_AUTO_TIME_I nv_item_type.set_auto_time*/
/*~ CASE NV_SET_RINGER_VOLUME_I nv_item_type.set_ringer_volume */
/*~ CASE NV_VIBRATOR_I nv_item_type.vibrator */
/*~ CASE NV_WCDMACS_SYNC_TEST_APP_ENABLED_I
    nv_item_type.wcdmacs_sync_test_app_enabled*/
/*~ CASE NV_DIAG_DEBUG_CONTROL_I nv_item_type.diag_debug_control */
/*~ CASE NV_DIAG_DEBUG_DETAIL_I nv_item_type.diag_debug_detail */
/*~ CASE NV_DIAG_DEBUG_MAX_FILES_I nv_item_type.diag_debug_max_files */
/*~ CASE NV_DIAG_DEBUG_CALL_TRACE_I nv_item_type.diag_debug_call_trace */
/*~ CASE NV_WLAN_ENABLE_PS_MODE_I nv_item_type.wlan_enable_ps_mode */
/*~ CASE NV_WLAN_QOS_MODE_I nv_item_type.wlan_qos_mode */
/*~ CASE NV_WLAN_OEM_INDEX_I nv_item_type.wlan_oem_index */
/*~ CASE NV_DCVSAPPS_NV_I nv_item_type.dcvsapps_nv */
/*~ CASE NV_SN_PPP_STATUS_I nv_item_type.sn_ppp_status */
/*~ CASE NV_LONG_PDPACT_TOLERANCE_FOR_TE_I
    nv_item_type.long_pdpact_tolerance_for_te */
/*~ CASE NV_UMTS_NBNS_ENABLE_I nv_item_type.umts_nbns_enable */
/*~ CASE NV_BT_DISABLED_I nv_item_type.bt_disabled */
/*~ CASE NV_HS_USB_CURRENT_COMPOSITION_I
    nv_item_type.hs_usb_current_composition */
/*~ CASE NV_WLAN_ENABLE_BT_COEX_I nv_item_type.wlan_enable_bt_coex */
/*~ CASE NV_WLAN_MAC_ADDRESS_I nv_item_type.wlan_mac_address */
/*~ CASE NV_FTM_MODE_I nv_item_type.ftm_mode */
/*~ CASE NV_GPS1_PDE_ADDRESS_I nv_item_type.gps1_pde_address */
/*~ CASE NV_GPS1_PDE_PORT_I nv_item_type.gps1_pde_port */
/*~ CASE NV_GPS1_PDE_TRANSPORT_I nv_item_type.gps1_pde_transport */
/*~ CASE NV_GPS1_SEC_UPDATE_RATE_I nv_item_type.gps1_sec_update_rate */
/*~ CASE NV_GPSONE_PASSWORD_I nv_item_type.gpsone_password */
/*~ CASE NV_RUIM_CHV_1_I nv_item_type.ruim_chv_1 */
/*~ CASE NV_RUIM_CHV_2_I nv_item_type.ruim_chv_2 */
/*~ CASE NV_HDRMRLP_NUM_BYTES_TO_LOG_I nv_item_type.hdrmrlp_num_bytes_to_log */
/*~ CASE NV_HDR_QOS_FLOW_PRIORITY_I nv_item_type.hdr_qos_flow_priority */
/*~ CASE NV_IPV6_PRIVACY_EXTENSIONS_ENABLED_I
    nv_item_type.ipv6_privacy_extensions_enabled */
/*~ CASE NV_IPV6_PRIVATE_ADDRESS_CONFIG_I
    nv_item_type.ipv6_private_address_config */
/*~ CASE NV_WLAN_MULTIDOMAIN_CAPABILITY_PREF_I
    nv_item_type.wlan_multidomain_capability_pref */
/*~ CASE NV_IPV6_UMTS_FAILOVER_CONFIG_I nv_item_type.ipv6_umts_failover_config */
/*~ CASE NV_VOIP_REFER_SUBSCRIPTION_EXPIRES_DURATION_I
    nv_item_type.voip_refer_subscription_expires_duration */
/*~ CASE NV_VOIP_NOTIFY_REFER_RESPONSE_DURATION_I
    nv_item_type.voip_notify_refer_response_duration */
/*~ CASE NV_VOIP_PRECONDITION_ENABLE_I nv_item_type.voip_precondition_enable */
/*~ CASE NV_SW_VERSION_INFO_I nv_item_type.sw_version_info */
/*~ CASE NV_DIAG_FTM_MODE_SWITCH_I nv_item_type.diag_ftm_mode_switch */
/*~ CASE NV_DS707_GO_NULL_TIMER_1X_I nv_item_type.ds707_go_null_timer_1x */
/*~ CASE NV_DS707_GO_NULL_TIMER_DO_I nv_item_type.ds707_go_null_timer_do */
/*~ CASE NV_HS_USB_REMOTE_WAKEUP_DELAY_I
    nv_item_type.hs_usb_remote_wakeup_delay */
/*~ CASE NV_DISPLAY_UPDATE_POWER_TEST_MODE_I
    nv_item_type.display_update_power_test_mode */
/*~ CASE NV_JCDMA_RUIM_ID_I nv_item_type.jcdma_ruim_id */
/*~ CASE NV_JCDMA_UIM_LOCK_I nv_item_type.jcdma_uim_lock */
/*~ CASE NV_ICMP6_ND_CONFIG_I nv_item_type.icmp6_nd_config */
/*~ CASE NV_HDR_EMPA_SUPPORTED_I nv_item_type.hdr_empa_supported */
/*~ CASE NV_DS_AT_V250_REGISTERS_I nv_item_type.ds_at_v250_registers */
/*~ CASE NV_DS_AT_S_REGISTERS_I nv_item_type.ds_at_s_registers */
/*~ CASE NV_HS_USB_DIAG_ON_LEGACY_USB_PORT_I
    nv_item_type.hs_usb_diag_on_legacy_usb_port */
/*~ CASE NV_DHCP4_OPTIONS_MASK_I nv_item_type.dhcp4_options_mask */
/*~ CASE NV_DHCP6_OPTIONS_MASK_I nv_item_type.dhcp6_options_mask */
/*~ CASE NV_SMS_CFG_ROUTING_I nv_item_type.sms_cfg_routing */
/*~ CASE NV_PROCESS_RESTART_SWITCH_I nv_item_type.process_restart_switch */
/*~ CASE NV_DS_AT_CSAS_CSCA_SETTINGS_I nv_item_type.ds_at_csas_csca_settings */
/*~ CASE NV_DS_AT_CSAS_CSMP_SETTINGS_I nv_item_type.ds_at_csas_csmp_settings */
/*~ CASE NV_DS_AT_CSAS_CSCB_SETTINGS_I nv_item_type.ds_at_csas_cscb_settings */
/*~ CASE NV_WWAN_ACCESS_OVER_WIFI_PREFERRED_I nv_item_type.wwan_access_over_wifi_preferred */           
/*~ CASE NV_WIFI_LOCAL_BREAKOUT_ALLOWED_I nv_item_type.wifi_local_breakout_allowed */           
/*~ CASE NV_IWLAN_OPERATOR_ID_I nv_item_type.iwlan_operator_id */           
/*~ CASE NV_IWLAN_REALM_I nv_item_type.iwlan_realm */           
/*~ CASE NV_IWLAN_AUTH_MODE_I nv_item_type.iwlan_auth_mode */           
/*~ CASE NV_IWLAN_SOFT_IKE_REKEY_TIME_I nv_item_type.iwlan_soft_ike_rekey_time */           
/*~ CASE NV_IWLAN_HARD_IKE_REKEY_TIME_I nv_item_type.iwlan_hard_ike_rekey_time */           
/*~ CASE NV_IWLAN_CHILD_SA_SOFT_LIFETIME_SEC_I nv_item_type.iwlan_child_sa_soft_lifetime_sec */           
/*~ CASE NV_IWLAN_CHILD_SA_HARD_LIFETIME_SEC_I nv_item_type.iwlan_child_sa_hard_lifetime_sec */           
/*~ CASE NV_IWLAN_CHILD_SA_SOFT_LIFETIME_BYTES_I nv_item_type.iwlan_child_sa_soft_lifetime_bytes */           
/*~ CASE NV_IWLAN_CHILD_SA_HARD_LIFETIME_BYTES_I nv_item_type.iwlan_child_sa_hard_lifetime_bytes */           
/*~ CASE NV_IWLAN_NAT_KEEPALIVE_INTERVAL_I nv_item_type.iwlan_nat_keepalive_interval */           
/*~ CASE NV_IWLAN_IKE_RETRANSMISSION_INTERVAL_I nv_item_type.iwlan_ike_retransmission_interval */           
/*~ CASE NV_IWLAN_IKE_MAX_RETRANSMISSIONS_I nv_item_type.iwlan_ike_max_retransmissions */           
/*~ CASE NV_IWLAN_IKEV2_IP_CONFIG_MASK_I nv_item_type.iwlan_ikev2_ip_config_mask */           
/*~ CASE NV_IWLAN_DHCP_IP_CONFIG_MASK_I nv_item_type.iwlan_dhcp_ip_config_mask */           
/*~ CASE NV_IWLAN_CHILD_SA_ENC_ALGO_PROPOSAL_MASK_I nv_item_type.iwlan_child_sa_enc_algo_proposal_mask */           
/*~ CASE NV_IWLAN_IKE_SA_ENC_ALGO_PROPOSAL_MASK_I nv_item_type.iwlan_ike_sa_enc_algo_proposal_mask */           
/*~ CASE NV_IWLAN_IKE_SA_PRF_ALGO_PROPOSAL_MASK_I nv_item_type.iwlan_ike_sa_prf_algo_proposal_mask */           
/*~ CASE NV_IWLAN_CHILD_SA_AUTH_ALGO_PROPOSAL_MASK_I nv_item_type.iwlan_child_sa_auth_algo_proposal_mask */           
/*~ CASE NV_IWLAN_IKE_SA_AUTH_ALGO_PROPOSAL_MASK_I nv_item_type.iwlan_ike_sa_auth_algo_proposal_mask */           
/*~ CASE NV_IWLAN_IKE_DH_MODE_I nv_item_type.iwlan_ike_dh_mode */           
/*~ CASE NV_IWLAN_PDIF_ADDRESS_I nv_item_type.iwlan_pdif_address */           
/*~ CASE NV_IWLAN_MULTI_AUTH_SUPPORTED_I nv_item_type.iwlan_multi_auth_supported */           
/*~ CASE NV_IWLAN_PDIF_FQDN_I nv_item_type.iwlan_pdif_fqdn */        
/*~ CASE NV_IWLAN_DPD_ENABLED_I nv_item_type.iwlan_dpd_enabled */
/*~ CASE NV_IWLAN_DPD_TIME_SECS_I nv_item_type.iwlan_dpd_time_secs */
/*~ CASE NV_BT_SOC_REFCLOCK_TYPE_I  nv_item_type.bt_soc_refclock_type */
/*~ CASE NV_BT_SOC_CLK_SHARING_TYPE_I   nv_item_type.bt_soc_clk_sharing_type */
/*~ CASE NV_BT_SOC_INBAND_SLEEP_I    nv_item_type.bt_soc_inband_sleep */
/*~ CASE NV_BT_SOC_LOGGING_ENABLED_I     nv_item_type.bt_soc_logging_enabled */
/*~ CASE NV_BT_SOC_PM_MODE_I      nv_item_type.bt_soc_pm_mode */
/*~ CASE NV_IWLAN_ENFORCE_PEER_CERT_AUTH_I      nv_item_type.iwlan_enforce_peer_cert_auth */
/*~ CASE NV_VBATT_I      nv_item_type.vbatt */
/*~ CASE NV_MGRF_SUPPORTED_I      nv_item_type.mgrf_supported */
/*~ CASE NV_HS_USB_USE_PMIC_OTG_COMPARATORS_I      nv_item_type.hs_usb_use_pmic_otg_comparators */
/*~ CASE NV_HS_USB_NUMBER_OF_SDCC_LUNS_I      nv_item_type.hs_usb_number_of_sdcc_luns */
/*~ CASE NV_WLAN_ATHEROS_SPECIFIC_CFG_I      nv_item_type.wlan_atheros_specific_cfg */
/*~ CASE NV_WLAN_CPU_FLOW_CONTROL_CFG_I      nv_item_type.wlan_cpu_flow_control_cfg */
/*~ CASE NV_HS_USB_RECONNECT_ON_RESET_DURING_SUSPEND_I      nv_item_type.hs_usb_reconnect_on_reset_during_suspend */
/*~ CASE NV_GAN_MODE_PREFERENCE_I      nv_item_type.gan_mode_preference */
/*~ CASE NV_HS_USB_DISABLE_SLEEP_VOTING_I      nv_item_type.hs_usb_disable_sleep_voting */
/*~ CASE NV_ENS_ENABLED_I      nv_item_type.ens_enabled */
/*~ CASE NV_IRDA_PNP_MANUFACTURER_I      nv_item_type.irda_pnp_manufacturer */
/*~ CASE NV_IRDA_PNP_NAME_I      nv_item_type.irda_pnp_name */
/*~ CASE NV_WLAN_UUID_I      nv_item_type.wlan_uuid */
/*~ CASE NV_WCDMA_RRC_VERSION_I      nv_item_type.wcdma_rrc_version */
/*~ CASE NV_HS_USB_HOST_MODE_ENABLED_I      nv_item_type.hs_usb_host_mode_enabled */
/*~ CASE NV_DB_LP_SEC_I      nv_item_type.db_lp_sec */
/*~ CASE NV_DB_LTM_OFF_I      nv_item_type.db_ltm_off */
/*~ CASE NV_DB_DAYLT_I      nv_item_type.db_daylt */
/*~ CASE NV_BROWSER_MAILTO_EMAIL_I      nv_item_type.browser_mailto_email */
/*~ CASE NV_COUNT_I      nv_item_type.count */
/*~ CASE NV_PREF_VOICE_SO_I      nv_item_type.pref_voice_so */
/*~ CASE NV_SPC_CHANGE_ENABLED_I      nv_item_type.spc_change_enabled */
/*~ CASE NV_MOB_CAI_REV_I      nv_item_type.mob_cai_rev */
/*~ CASE NV_UE_IMEISV_SVN_I      nv_item_type.ue_imeisv_svn */
/*~ CASE NV_HYBRID_PREF_I      nv_item_type.hybrid_pref */
/*~ CASE NV_HDRSCP_FORCE_REL0_CONFIG_I      nv_item_type.hdrscp_force_rel0_config */
/*~ CASE NV_SMS_BC_SERVICE_TABLE_SIZE_I      nv_item_type.sms_bc_service_table_size */
/*~ CASE NV_SMS_BC_SERVICE_TABLE_I      nv_item_type.sms_bc_service_table */
/*~ CASE NV_BT_QSOC_CLASS_TYPE_I      nv_item_type.bt_qsoc_class_type */
/*~ CASE NV_WLAN_PAL_LINK_PREF_I      nv_item_type.wlan_pal_link_pref */
/*~ CASE NV_HS_USB_DISABLE_LPM_I      nv_item_type.hs_usb_disable_lpm */
/*~ CASE NV_DIAG_DIAGBUF_TX_SLEEP_TIME_NV_I nv_item_type.diag_diagbuf_tx_sleep_time_nv */
/*~ CASE NV_DIAG_DIAGBUF_TX_SLEEP_THRESHOLD_EXT_I nv_item_type.diag_diagbuf_tx_sleep_threshold_ext */
/*~ CASE NV_OEM_ITEM_1_I      nv_item_type.oem_item_1 */
/*~ CASE NV_OEM_ITEM_2_I      nv_item_type.oem_item_2 */
/*~ CASE NV_OEM_ITEM_3_I      nv_item_type.oem_item_3 */
/*~ CASE NV_OEM_ITEM_4_I      nv_item_type.oem_item_4 */
/*~ CASE NV_OEM_ITEM_5_I      nv_item_type.oem_item_5 */
/*~ CASE NV_OEM_ITEM_6_I      nv_item_type.oem_item_6 */
/*~ CASE NV_OEM_ITEM_7_I      nv_item_type.oem_item_7 */
/*~ CASE NV_OEM_ITEM_8_I      nv_item_type.oem_item_8 */
/*~ CASE NV_EHRPD_IMSI_I      nv_item_type.ehrpd_imsi */
/*~ CASE NV_PPP_EAP_SHARED_SECRET_I      nv_item_type.ppp_eap_shared_secret */
/*~ CASE NV_EHRPD_MILENAGE_OP_I      nv_item_type.ehrpd_milenage_op */
/*~ CASE NV_PPP_VSNCP_CONFIG_DATA_I      nv_item_type.ppp_vsncp_config_data */
/*~ CASE NV_EHRPD_AUTH_IN_USIM_I      nv_item_type.ehrpd_auth_in_usim */
/*~ CASE NV_DATA_CALL_OVER_EHRPD_ONLY_I nv_item_type.data_call_over_ehrpd_only */
/*~ CASE NV_HDRSCP_FORCE_AT_CONFIG_I nv_item_type.hdrscp_force_at_config */
/*~ CASE NV_EHRPD_ENABLED_I nv_item_type.ehrpd_enabled */
/*~ CASE NV_QDJ_DEQUEUE_LOG_ENABLED_I nv_item_type.qdj_dequeue_log_enabled */
/*~ CASE NV_CHG_USB_NVDISABLE_VAL_I nv_item_type.chg_usb_nvdisable_val */
/*~ CASE NV_DS_SIP_NUM_VALID_PROFILES_I nv_item_type.ds_sip_num_valid_profiles */
/*~ CASE NV_DS_SIP_ACTIVE_PROFILE_INDEX_I nv_item_type.ds_sip_active_profile_index */
/*~ CASE NV_DS_SIP_PROFILE_I nv_item_type.ds_sip_profile */
/*~ CASE NV_DS_SIP_NAI_INFO_I nv_item_type.ds_sip_nai_info */
/*~ CASE NV_DS_SIP_PPP_SS_INFO_I nv_item_type.ds_sip_ppp_ss_info */
/*~ CASE NV_HS_USB_PHY_CONFIG_I nv_item_type.hs_usb_phy_config */
/*~ CASE NV_HS_USB_PERFORMANCE_SETTING_I nv_item_type.hs_usb_performance_setting */
/*~ CASE NV_QVP_APP_PSVT_ENABLE_FLAG_I nv_item_type.qvp_app_psvt_enable_flag */
/*~ CASE NV_HDRSCP_SESSION_STATUS_I nv_item_type.hdrscp_session_status */
/*~ CASE NV_TRM_CONFIG_I nv_item_type.trm_config */
/*~ CASE NV_BT_ACTIVE_I nv_item_type.bt_active */
/*~ CASE NV_BT_VISIBLE_I  nv_item_type.bt_visible */
/*~ CASE NV_BT_SAP_ENABLE_I nv_item_type.bt_sap_enable */
/*~ CASE NV_BT_LISBON_DISABLED_I nv_item_type.bt_lisbon_disabled */
/*~ CASE NV_BT_QSOC_NVM_MODE_I nv_item_type.bt_qsoc_nvm_mode */
/*~ CASE NV_TIMEZONE_CITY_I nv_item_type.timezone_city */
/*~ CASE NV_TIMEZONE_I nv_item_type.timezone */

