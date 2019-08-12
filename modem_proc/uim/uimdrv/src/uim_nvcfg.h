#ifndef UIM_NVCFG_H
#define UIM_NVCFG_H
/*==============================================================================
  FILE:         uim_nvcfg.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                intilalization and access of UIMDRV nv items.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_nvcfg.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
07/05/16    ks         Reading'disable_proactive_polling' member from NV 67330
03/12/15    ks         SIM tray by tieing the card detect gpios
01/08/15    xj         Add DISABLE_CARD_STATUS_CHECK_AT_POWER_UP in features NV
11/17/14    na         support of le for stream 7816 apdu controlled with NV
10/10/14    ll         Do not register for MCFG if slots are disabled
09/29/14    ks         Attempting power-up with 3v after few recoveries
08/27/14    sam        Adding support to trigger recovery on reciept of bad status word
08/14/14    sam        Support for extended recovery
03/31/14    ll         Change stop at infinite NULL flag name for readability
01/15/14    yk         The first revision
==============================================================================*/

#define UIM_MAX_CONFIG_PARAMS_RFU_BYTES                         32 /* Max number of RFU bytes */
/* Following are the indexes of RFU bytes in UIM Config Params NV item */
#define UIM_RFU_INDEX_DELAY_BEFORE_CLK_STOP                     0  /* RFU[0]-Delay before clock stop */
#define UIM_RFU_INDEX_DELAY_AFTER_CLK_RESTART                   1  /* RFU[1]-Delay after clock stop  */
#define UIM_RFU_INDEX_EXTRA_GUARD_TIME                          2  /* RFU[2]-Extra guard time */
#define UIM_RFU_INDEX_EXTRA_WWT                                 3  /* RFU[3]-Extra work waiting time */
#define UIM_RFU_INDEX_EXTRA_BWT                                 4  /* RFU[4]-Extra block waiting time */
#define UIM_RFU_INDEX_EXTRA_BGT                                 5  /* RFU[5]-Extra block guard time */
#define UIM_RFU_INDEX_EXTRA_CWT                                 6  /* RFU[6]-Extra character waiting time*/
#define UIM_RFU_INDEX_EXTRA_VREG_VOLT                           7  /* RFU[7]-Extra voltage to PMIC Vreg */
#define UIM_RFU_INDEX_BOOLEAN_FLAGS                             8  /* RFU[8]-Boolean flags */
#define UIM_RFU_INDEX_POWER_UP_DELAY_VAL                        9  /* RFU[9]-Power up delay value */

/* Following are the masks for Boolean flags of RFU byte with index 8 */
#define UIM_RFU_MASK_CHANGE_INST_CLASS_IF_TECH_PROB             0x01 /* RFU[8]&0x01 - Change instruction class if card returns technical problem */
#define UIM_RFU_MASK_NOT_SELECT_GSM_DF                          0x02 /* RFU[8]&0x02 - Select GSM DF during initialization */
#define UIM_RFU_MASK_STATUS_MAX_LEN_RETURN_EXP                  0x04 /* RFU[8]&0x04 - Set max length expected from card for status command */
#define UIM_RFU_MASK_DO_NOT_IGNORE_RX_BREAK_IN_IDLE             0x08 /* RFU[8]&0x08 - Do not ignore RX Break errors in idle state */

/* Max possible feature-elements in UIM_FEATURES_STATUS_LIST_TYPE NVITEM struct */
#define UIM_MAX_ITEMS_IN_FEATURE_NVITEM             127
/* Current total # of features in UIM_FEATURES_STATUS_LIST_TYPE NVITEM struct*/
#define UIM_FEATURES_STATUS_LIST_CURRENT_COUNT      29
/* Default value of uim_trans_timer in case the EFS item is not set */
#define UIM_SIMULATE_NULL_INS_DEFAULT                  0x88

/*macro definitions for extended recovery feature*/
#define UIM_ERECOV_NUM_DEFAULT_PUPS             0         /*default extended powerup attempts*/
#define UIM_ERECOV_DEFAULT_DELAY                1000      /*default delay value in msecs between powerup attempts*/
#define UIM_ERECOV_DEFAULT_INCREMENT            FALSE     /*default increment*/

#define UIM_ERECOV_NUM_PUPS_MAX                 8         /*max extended powerup attempts*/

#define UIM_ERECOV_DEFAULT_DELAY_MAX            30000     /*maximum delay value in msecs between powerup attempts*/

/* The subsequent enums and struct are for the EFS-based NV ITEM
   UIM_FEATURES_STATUS_LIST_TYPE */
/*==============================================================================
  ENUM:   UIM_NV_FEATURES_ENUM_TYPE
==============================================================================*/
typedef enum {
  UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES = 0,
  UIMDRV_FEATURE_LOG_TO_EFS,
  UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL,
  UIMDRV_FEATURE_DEBUG_LOG,
  UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL,
  UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE,
  UIMDRV_FEATURE_SUPPORT_NO_ICCID,
  UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
  UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT,
  UIMDRV_FEATURE_INTERFACE_NOT_USED,
  UIMDRV_FEATURE_LOG_APDU_TO_EFS,
  UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY,
  UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH,
  UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD,
  UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH,
  UIMDRV_FEATURE_SM_USE_SLOT_1,
  UIMDRV_FEATURE_USE_DUAL_LDO,
  UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
  UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ,
  UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME,
  UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET,
  UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF,
  UIMDRV_FEATURE_ENABLE_BOOT_UP_IN_INVERSE_CONVENTION,
  UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD,
  UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY,
  UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU,
  UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP,
  UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED,
  UIMDRV_FEATURE_DISABLE_PROACTIVE_POLLING
} uim_nv_features_enum_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURES_STATUS_LIST_TYPE

   DESCRIPTION:
     Data Structure to hold status of uim features as it reads it from
     efs NVITEM "UIM_FEATURES_STATUS_LIST"
     * Size of this struct should remain the SAME at all times.
     * "reserved_for_future_use" array items will be hidden from QXDM users at
       all times (hence not writable).
     * "version" should always be the FIRST item.
     * "reserved_for_future_use" shold always be the LAST item.
     * "version" is the key here. First implementation of this struct sets it
       to 0.
     * Everytime new uimdrv item(s) is/are required to be exposed to the
       users via this NVITEM,
       - the item(s) should be added in this struct just before the
         "reserved_for_future_use" item.
       - make sure to decrease the size of "reserved_for_future_use" array by
         the number of items(say, n) added to the struct. That is, increase the
         value of UIM_FEATURES_STATUS_LIST_CURRENT_COUNT by n.
       - increase the value of UIM_FEATURES_STATUS_LIST_CURRENT_VERSION by 1
       - define UIM_FEATURES_STATUS_LIST_VERSIONx_COUNT equal to
         how many ever features/items are added until now (including the ones
         we are currently adding in version x).
       - Update global uimdrv_features_total_till_version[] by adding an element
         UIM_FEATURES_STATUS_LIST_VERSIONx_COUNT to the array.
         uimdrv_features_total_till_version[]
       - in uimdrv_nv_feature_write_default_value(), add case(s) for default
         value of the newly added item(s)/feature(s).
       - in uimdrv_nv_get_feature_status(), add case(s) for new item(s).
       - add item in enum "uimdrv_nv_features_enum_type "
       - Notify QXDM team of the modification.
       - In the release notes, mention the new version of this NVITEM.
       - In the release notes, suggest upgrading QXDM to the version to be
         released two weeks from that date (assuming that QXDM would pick our
         updated NVITEM by then). Otherwise, the user won't be able to see and
         update(set) the new entry in the NVITEM. The UIMDRV code would by
         default set the unset entries to their default values.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8 version;
  /* version 0: NEW FEATURE ITEMS = 15; TOTAL CURRENT FEATURE ITEMS = 15. */
  /* version 1: NEW FEATURE ITEMS = 1;  TOTAL CURRENT FEATURE ITEMS = 16. */
  /* version 2: NEW FEATURE ITEMS = 1;  TOTAL CURRENT FEATURE ITEMS = 17. */
  /* version 3: NEW FEATURE ITEMS = 1;  TOTAL CURRENT FEATURE ITEMS = 18. */
  /* version 4: NEW FEATURE ITEMS = 1;  TOTAL CURRENT FEATURE ITEMS = 19. */
  /* version 5: NEW FEATURE ITEMS = 2;  TOTAL CURRENT FEATURE ITEMS = 21. */
  boolean  handle_no_atr_in_40000_clk_cycles;
  boolean  log_to_efs;
  boolean  disable_recovery_upon_infinite_null;
  boolean  debug_log;
  boolean  rpt_bad_sw_on_poll;
  boolean  handle_iccid_read_failure;
  boolean  support_no_iccid;
  boolean  min_tpl_iccid_support;
  boolean  handle_unknown_proc_bytes_as_cmd_timeout;
  boolean  interface_not_used;
  boolean  log_apdu_to_efs;
  boolean  no_switch_inst_on_wwt_expiry;
  boolean  send_status_before_auth;
  boolean  try_default_baud_rate_for_f372_d12_card;
  boolean  cold_reset_due_to_card_switch;
  boolean  sm_prefer_slot1;
  uint8    use_dual_ldo;
  boolean  uim_polling_only_at_polling_timer_expiry;
  boolean  uim_set_clk_freq_at_4_8_MHz;
  boolean  handle_tc1_byte_for_extra_guard_time;
  boolean  enable_sim_mode_change_via_warm_reset;
  boolean  enable_explicit_selection_of_mf_or_adf;
  boolean  enable_boot_up_in_inverse_convention;
  boolean  recovery_on_bad_status_word;
  uint8    attempt_pup_3v_from_nth_recovery;
  uint8    enable_le_support_for_stream_7816_apdu;
  uint8    disable_card_status_check_at_power_up;
  boolean  simtray_with_gpios_tied;
  /* This flag is not used in code currently,
     we are just storing the NV here */
  boolean  disable_proactive_polling;
  /* version 9: RESERVED_FOR_FUTURE_USE size is 127 - 28 = 99 */
  uint8    reserved_for_future_use
   [UIM_MAX_ITEMS_IN_FEATURE_NVITEM - UIM_FEATURES_STATUS_LIST_CURRENT_COUNT];
} uim_features_status_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURE_SUPPORT_HOTSWAP_TYPE

   DESCRIPTION:
     Data Structure to hold status of feature support_hotswap as it reads it
     from efs NVITEM "UIM_FEATURE_SUPPORT_HOTSWAP"
     * Size of this struct should remain the SAME at all times.
     * "reserved_for_future_use" array items will be hidden from QXDM users at
       all times (hence not writable).
     * "version" should always be the FIRST item.
     * "reserved_for_future_use" shold always be the LAST item.
     * "version" is the key here. First implementation of this struct sets it
       to 0.
     * Everytime new uimdrv item(s) is/are required to be exposed to the
       users via this NVITEM,
       - the item(s) should be added in this struct just before the
         "reserved_for_future_use" item.
       - make sure to decrease the size of "reserved_for_future_use" array by
         the number of items(say, n) added to the struct.
       - increase the value of UIM_FEATURE_SUPPORT_HOTSWAP_VERSION by 1
       - define UIM_FEATURE_SUPPORT_HOTSWAP_VERSIONx_COUNT equal to
         how many ever features/items are added until now (including the ones
         we are currently adding in version x).
       - Update global uimdrv_feature_support_hotswap_total_till_version[] by
         adding an element UIM_FEATURE_SUPPORT_HOTSWAP_VERSIONx_COUNT to the
         array uimdrv_feature_support_hotswap_total_till_version[]
       - Notify QXDM team of the modification.
       - In the release notes, mention the new version of this NVITEM.
       - In the release notes, suggest upgrading QXDM to the version to be
         released two weeks from that date (assuming that QXDM would pick our
         updated NVITEM by then). Otherwise, the user won't be able to see and
         update(set) the new entry in the NVITEM. The UIMDRV code would by
         default set the unset entries to their default values.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8    version;
  /* version 0: NEW FEATURE ITEMS = 1; TOTAL CURRENT FEATURE ITEMS = 1. */
  boolean  feature_support_hotswap;
  /* version 0: RESERVED_FOR_FUTURE_USE size is 10 - 1 = 9 */
  uint8    reserved_for_future_use[9];
} uim_feature_support_hotswap_type;

/**
 * This procedure is called by all UIM tasks to initialize all
 * UIMDRV NV items but the NV's are read only once by the task
 * that initializes first.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_nv_init( uim_instance_global_type *uim_ptr );

/**
 * Get the status of UIMDRV subfeatures controlled by various
 * nvitems such as UIM_FEATURES_STATUS_LIST_FILE,
 * UIM_FEATURE_SUPPORT_HOTSWAP_FILE.
 *
 * @param nv_subfeature NV enum of sub feature.
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Enabled or not.
 */
boolean uim_nv_is_feature_enabled
(
  uim_nv_features_enum_type  nv_subfeature,
  uim_instance_global_type  *uim_ptr
);

/**
 * Put an item to nonvolatile memory.
 *
 * @param item NV item to be saved.
 * @param data_ptr Pointer to the NV data.
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_put_nv(
  nv_items_enum_type     item,
  nv_item_type          *data_ptr,
  uim_instance_global_type *uim_ptr
);

/**
 * Get an item to nonvolatile memory.
 *
 * @param item NV item to get.
 * @param data_ptr Pointer to the data to be saved.
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_get_nv(
  nv_items_enum_type     item,
  nv_item_type          *data_ptr,
  uim_instance_global_type *uim_ptr
);

/**
 * Register MCFG NV REFRESH event.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uimdrv_mcfg_refresh_register
(
  uim_instance_global_type *uim_ptr
);
#endif /* UIM_NVCFG_H */
