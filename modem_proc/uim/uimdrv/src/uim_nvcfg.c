/*==============================================================================
  FILE:         uim_nvcfg.c

  OVERVIEW:     FIle conatins the functions to intilalize, set and get the nv
                items.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2016 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_nvcfg.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
07/05/16    ks         Reading'disable_proactive_polling' member from NV 67330
06/10/16    na         Removed F3 traces
05/23/16    dd         Enhancing the voltage supply indication to QMI feature
05/18/16    ssr        Remove msg for malloc failure
04/28/16    sam        Disable UIMDRV_FEATURE_LOG_APDU_TO_EFS by default
03/31/16    ks         Header file inclusion
01/25/16    ssr        Delay local card power up in Modem SSR in BTSAP mode
12/01/15    ll         Removing compile time disable polling feature flag 
11/05/15    ks         Rel12 polling enhancement
10/06/15    ll         Enable UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY by default
09/08/15    ll         UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
07/17/15    ks         Fixing issue during writing default value in the NV4205
05/13/15    ll         Splitting the slot specific NVs from MCFG refreshed NVs
03/12/15    ks         SIM tray by tieing the card detect gpios
01/19/15    kr         Fixed Compilation warnings
01/08/15    xj         Add DISABLE_CARD_STATUS_CHECK_AT_POWER_UP in features NV
11/17/14    na         Support of le for stream 7816 apdu controlled with NV
10/10/14    ll         Do not register for MCFG if slots are disabled
09/29/14    ks         Attempting power-up with 3v after few recoveries
08/27/14    sam        Adding support to trigger recovery on reciept of bad status word
08/25/14    ll         Fix compile warnings medium
08/14/14    sam        Support for extended recovery
07/25/14    ll         Fix link errors for ASSERT and ERR_FATAL
07/25/14    lxu        Increase usb uicc command time out to 16 seconds by default
07/25/14    lxu        Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                       voltage_class to ap when send usb power up to ap
07/15/14    ll         NV refresh upon hotswap feature
05/28/14    ll         Write NV 4205 with default values if not initialized
04/10/14    ak         Fix compile error when FEATURE_UIM_SUPPORT_DUAL_SLOTS is defined
03/31/14    ll         Change stop at infinite NULL flag name for readability
03/27/14    lxu        USB UICC modem implement
03/21/14    ll         Remove return_ptr and remove/change uim_free to UIM_FREE
02/20/14    ks         Provision for explicit MF or ADF selction
                       And provision for inverse convention bootup
02/18/14    akv        Version needed updating for UIMDRV features NV
02/08/14    sam        Fix for Klocwork issues
01/15/14    yk         The first revision
==============================================================================*/
#include "rex.h"
#include "fs_public.h"
#include "err.h"
#include "uim_common_efs.h"

#include "uim_variation.h"
#include "uimi.h"
#include "uimgen.h"
#include "uimglobals.h"
#include "uim_remote.h"
#include "uim_hotswap.h"
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
#include "uimsub_manager.h"
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
#include "uim_nvcfg.h"
#include "uimdrv_main.h"

/* This should be incremented whenever there's a change to struct
   uim_features_status_list_type */
#define UIM_FEATURES_STATUS_LIST_CURRENT_VERSION    12
/* This should be incremented whenever there's a change to struct
   uim_feature_support_hotswap_type */
#define UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION 0

/* Total # of features at each version of NVITEM.. starting with VERSION 0 */
#define UIM_FEATURES_STATUS_LIST_VERSION0_COUNT    15
#define UIM_FEATURES_STATUS_LIST_VERSION1_COUNT    16
#define UIM_FEATURES_STATUS_LIST_VERSION2_COUNT    17
#define UIM_FEATURES_STATUS_LIST_VERSION3_COUNT    18
#define UIM_FEATURES_STATUS_LIST_VERSION4_COUNT    19
#define UIM_FEATURES_STATUS_LIST_VERSION5_COUNT    21
#define UIM_FEATURES_STATUS_LIST_VERSION6_COUNT    23
#define UIM_FEATURES_STATUS_LIST_VERSION7_COUNT    24
#define UIM_FEATURES_STATUS_LIST_VERSION8_COUNT    25
#define UIM_FEATURES_STATUS_LIST_VERSION9_COUNT    26
#define UIM_FEATURES_STATUS_LIST_VERSION10_COUNT   27
#define UIM_FEATURES_STATUS_LIST_VERSION11_COUNT   28
#define UIM_FEATURES_STATUS_LIST_VERSION12_COUNT   29

#define UIM_FEATURE_SUPPORT_HOTSWAP_VERSION0_COUNT 1

#define UIM_FEATURES_VERSION_COUNT                 13
#define UIM_HOTSWAP_FEATURES_VERSION_COUNT         1

/* Default value of each uimdrv feature controlled by
  UIM_FEATURES_STATUS_LIST NVITEM */
#define UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES_DEFAULT         FALSE
#define UIMDRV_FEATURE_LOG_TO_EFS_DEFAULT                                TRUE
#define UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL_DEFAULT       FALSE
#define UIMDRV_FEATURE_DEBUG_LOG_DEFAULT                                 TRUE
#define UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL_DEFAULT                        FALSE
#define UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE_DEFAULT                 FALSE
#define UIMDRV_FEATURE_SUPPORT_NO_ICCID_DEFAULT                          TRUE
#define UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT_DEFAULT                     FALSE
#define UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT_DEFAULT  FALSE
#define UIMDRV_FEATURE_INTERFACE_NOT_USED_DEFAULT                        FALSE
#define UIMDRV_FEATURE_LOG_APDU_TO_EFS_DEFAULT                           FALSE
#define UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY_DEFAULT              FALSE
#define UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH_DEFAULT                   FALSE
#define UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD_DEFAULT   FALSE
#define UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH_DEFAULT             FALSE
#define UIMDRV_FEATURE_SM_USE_SLOT_1_DEFAULT                             FALSE
#define UIMDRV_FEATURE_USE_DUAL_LDO_DEFAULT                              FALSE
#define UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY_DEFAULT  TRUE
#define UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ_DEFAULT               TRUE
#define UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME_DEFAULT      FALSE
#define UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET_DEFAULT     FALSE
#define UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF_DEFAULT    FALSE
#define UIMDRV_FEATURE_ENABLE_BOOT_UP_IN_INVERSE_CONVENTION_DEFAULT      FALSE
#define UIMDRV_PDOWN_CONSECUTIVE_TECH_PROB_DEFAULT                       FALSE
#define UIMDRV_REPEATED_TECH_PROBLEMS_DEFAULT                            3
#define UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD_DEFAULT               FALSE
#define UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY_DEFAULT          0
#define UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU_DEFAULT           TRUE
#define UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP_DEFAULT     FALSE
#define UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED_DEFAULT                   FALSE
#define UIMDRV_FEATURE_DISABLE_PROACTIVE_POLLING_DEFAULT                 FALSE

/*=============================================================================
  ENUM:   UIM_NV_FEATURE_SUPPORT_HOTSWAP_ENUM_TYPE
=======================================================================*/
typedef enum {
  UIMDRV_FEATURE_SUPPORT_HOTSWAP = 0
} uim_nv_feature_support_hotswap_enum_type;

/*=============================================================================
  ENUM:   For all the "version" based efs nv items
=======================================================================*/
typedef enum {
  UIMDRV_FEATURES_STATUS_NVITEM_ENUM = 0,
  UIMDRV_SUPPORT_HOTSWAP_NVITEM_ENUM,
  UIMDRV_MAX_NVITEM_ENUM
} uim_nvitems_enum;

/* Default value of uim_ind_timer in case the EFS item is not set */
#define UIM_BUSY_IND_TIMER_VAL_MS_DEFAULT              15000
/* Default value of uim_trans_timer in case the EFS item is not set */
#define UIM_TRANS_TIMER_VAL_MS_DEFAULT                 120000

/* Default value of uim_rel12_polling_enhancement in case the EFS item is not set */
#define UIM_REL12_POLLING_ENHANCEMENT_DEFAULT          FALSE

/* Deafault waiting period of 1sec before the acknowledgement is received */
#define UIM_DEFAULT_DELAY_QMI_ACK_FOR_VOLTAGE_SUPPLY_IND 1000 /* 1000 msec */

/* Maximum waiting period of 10 sec before the acknowledgement is received */
#define UIM_MAX_DELAY_QMI_ACK_FOR_VOLTAGE_SUPPLY_IND 10000 /* 10,000msec (10sec) */

/* All of UIM instances shall have to read the NV file separately. Although the
   contents may be same but it will help in avoding race conditions.
   Else there has to be a signalling mechanism between two tasks
   to convey if the NV files have been read or not.
   */

/*---------------------------------------------------------------------------
  List of total number of UIMDRV features present in NVITEM in each version
  of uimdrv_feature_status_list. Whenever a newer version of the NVITEM is
  exposed, size of the array grows by 1...So, if 8 features/items were added
  in VERSION0 and 2 in VERSION1, the array should have 2 items listing the
  number of total features at each version.. {8,10} in this case...
---------------------------------------------------------------------------*/
uint8 uimdrv_features_total_till_version[UIM_FEATURES_VERSION_COUNT] =
{
  UIM_FEATURES_STATUS_LIST_VERSION0_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION1_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION2_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION3_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION4_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION5_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION6_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION7_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION8_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION9_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION10_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION11_COUNT,
  UIM_FEATURES_STATUS_LIST_VERSION12_COUNT
};

uint8 uimdrv_feature_support_hotswap_total_till_version[UIM_HOTSWAP_FEATURES_VERSION_COUNT] =
{
  UIM_FEATURE_SUPPORT_HOTSWAP_VERSION0_COUNT
};


/**
 * DECLARATIONS OF INTERNAL FUNCTIONS
 */
static void uim_read_nv_from_efs_slot(uim_instance_global_type  *uim_ptr);

static void uim_read_nv_from_efs_device(uim_instance_global_type  *uim_ptr);

static void uim_init_nv_globals_with_read_values_from_efs
(
  uim_instance_global_type *uim_ptr
);

static void uim_read_legacy_nv_device
(
  uim_instance_global_type *uim_ptr,
  rex_tcb_type             *task_ptr,
  rex_sigs_type            task_wait_sig,
  void                    (*task_wait_handler)( rex_sigs_type, uim_instance_global_type*)
);

static void uim_compare_and_update_conf_file
(
  uim_common_efs_item_type    efs_item,
  const char *                buf_ptr,
  uint16                      buf_len,
  uim_instance_global_type   *uim_ptr
);

static void uim_update_conf_file(uim_instance_global_type *uim_ptr);

static void uim_nv_feature_write_default_value
(
  uint8                  item_subfeature,
  uim_common_efs_item_type   efs_item,
  uim_instance_global_type *uim_ptr
);

static void uim_nv_write_default_value_of_delta_features
(
  uint8                  version_in_nv,
  uint8                  version_in_build,
  uim_common_efs_item_type  efs_item,
  uim_instance_global_type *uim_ptr
);

static void uim_nv_write_default_value_of_all_features
(
  uim_common_efs_item_type  efs_item,
  uim_instance_global_type *uim_ptr
);

static void uim_nv_features_status_list_update_version
(
  uim_common_efs_item_type  efs_item,
  uim_instance_global_type *uim_ptr
);

static void uim_validate_and_set_config_params
(
  nv_uim_config_params_type        *out_uim_config_params,
  nv_uim_config_params_type const  *read_from_nv_uim_config_params,
  nv_stat_enum_type                 nv_read_status,
  uim_instance_global_type         *uim_ptr
);


/**
 * DEFINITIONS OF FUNCTIONS
 */

/**
 * This procedure is called by all UIM tasks to initialize all
 * UIMDRV NV items but the NV's are read only once by the task
 * that initializes first.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_nv_init( uim_instance_global_type *uim_ptr )
{
  uim_shared_global_type *shared_globals_ptr = uim_get_common_globals_ptr();

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  /* Read device based NVs: read only once and same values copy to all tasks */
  /* Enter critical section */
  rex_enter_crit_sect(&shared_globals_ptr->nv_read_crit_section);

  if (FALSE == shared_globals_ptr->nv_read_completed)
  {
    shared_globals_ptr->nv_read_by_instance = uim_ptr->id;
    uim_read_nv_from_efs_device(uim_ptr);
    uim_read_legacy_nv_device(uim_ptr, rex_self(),
                       UIM_NV_CMD_SIG,
                       (void (*)(rex_sigs_type, uim_instance_global_type*))uim_pet_and_wait);
    uim_update_conf_file(uim_ptr);
    shared_globals_ptr->nv_read_completed = TRUE;
  }
  else if (TRUE == shared_globals_ptr->nv_read_completed)
  {
    /* Device based NV's have already been read from efs. Use the values read
       by the task that initialized first.*/
    uim_init_nv_globals_with_read_values_from_efs(uim_ptr);
  }

  rex_leave_crit_sect(&shared_globals_ptr->nv_read_crit_section);

  /* Read (packed)slot/context based NVs: read it's own set of NVs by each task */
  /* The reason to have slot/context based NVs is to have the capability to have
     different carrier settings on each slot/context */
  uim_read_nv_from_efs_slot(uim_ptr);
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "UIM_NV_INIT completed");
}  /* uim_nv_init */


/**
 * This procedure is called to read all the carrier dependent MCFG refreshed slot based 
 * UIMDRV NV items using common API uim_common_efs_read() and uim_common_efs_write().
 *
 * Current Carrier dependent slot/context based NVs:
 *      1. PDOWN ON CONSECUTIVE TECH PROB 6F00
 *      2. UIM JCDMA Mode
 * And then inform MCGF once done refresh.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uimdrv_read_mcfg_refreshed_nv( uim_instance_global_type *uim_ptr )
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  /* === Read the Power down configuration from NV === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_UIMDRV_PDOWN_NV_EF,
                                UIM_COMMON_EFS_ITEM_FILE_TYPE,
                               (uim_common_efs_context_type)uim_ptr->id,
                               (uint8 *)&(uim_ptr->setting.pdown_on_tech_prob),
                                sizeof(uim_ptr->setting.pdown_on_tech_prob)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Power down configuration on technical problems read from EFS");
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Failed to read EFS power down configuration");
    uim_ptr->setting.pdown_on_tech_prob.to_pdown         = UIMDRV_PDOWN_CONSECUTIVE_TECH_PROB_DEFAULT;
    uim_ptr->setting.pdown_on_tech_prob.cnt_tech_problem = UIMDRV_REPEATED_TECH_PROBLEMS_DEFAULT;
  }

  /* === Read the JCDMA configuration from NV === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_UIMDRV_JCDMA_MODE,
                                UIM_COMMON_EFS_ITEM_FILE_TYPE,
                               (uim_common_efs_context_type)uim_ptr->id,
                               (uint8 *)&(uim_ptr->setting.jcdma_mode),
                                sizeof(uim_ptr->setting.jcdma_mode)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM JCDMA mode read from EFS");
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Failed to read EFS item UIM JCDMA mode");
    uim_ptr->setting.jcdma_mode.service  = UIM_JCDMA_SERVICE_DISABLED;
  }
} /*uimdrv_read_mcfg_refreshed_nv*/


/**
 * This function runs within the context of MCFG. It is called in the UIMDRV NV_REFRESH
 * callback by MCFG when the specific slot based NV refresh event happens. This function
 * will set UIM_MCGF_NV_REFRESH_SIG and save the reference id.
 * @param uim_ptr Pointer to the global data common.
 */
boolean uimdrv_handle_mcgf_nv_refresh_cb(mcfg_refresh_info_s_type *mcfg_refresh_info_ptr)
{
  uim_instance_global_type * uim_ptr = NULL;

  if (NULL == mcfg_refresh_info_ptr)
  {
    UIM_MSG_ERR_0("mcfg_refresh_info_ptr is NULL");
    return FALSE;
  }

  /* Get the uim_ptr for the current refresh event triggered task */
  switch (mcfg_refresh_info_ptr->slot_index)
  {
    /*slot index from MCFG: MCFG_REFRESH_INDEX_0 maps to UIM_INSTANCE_1*/
    case MCFG_REFRESH_INDEX_0:
      uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_1);
      break;
    case MCFG_REFRESH_INDEX_1:
      uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_2);
      break;
    case MCFG_REFRESH_INDEX_2:
      uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_3);
      break;
    default:
      UIM_MSG_ERR_1("Invalid refresh index 0x%x",
                    mcfg_refresh_info_ptr->slot_index);
      break;
  }

  if (NULL == uim_ptr)
  {
    return FALSE;
  }
  /* Store the reference id in the global*/
  uim_ptr->state.nv_refresh_reference_id = mcfg_refresh_info_ptr->reference_id;
  /* Set the sig on the task/slot that need to refresh*/
  (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_MCGF_NV_REFRESH_SIG );
  return TRUE;
}/*uimdrv_handle_mcgf_nv_refresh_cb*/


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
)
{
  boolean ret_value = FALSE;
  switch(nv_subfeature)
  {
    case UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES:
#ifdef FEATURE_UIM_BTSAP_CLIENT
      if(!(IS_BTSAP_HANDLE_ACTIVE(uim_ptr)))
#endif
      {
        ret_value = uim_ptr->setting.features_status_list.handle_no_atr_in_40000_clk_cycles;
      }
      break;
    case UIMDRV_FEATURE_LOG_TO_EFS:
      ret_value = uim_ptr->setting.features_status_list.log_to_efs;
      break;
    case UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL:
#ifdef FEATURE_UIM_BTSAP_CLIENT
      if(!(IS_BTSAP_HANDLE_ACTIVE(uim_ptr)))
#endif
      {
        ret_value = uim_ptr->setting.features_status_list.disable_recovery_upon_infinite_null;
      }
      break;
    case UIMDRV_FEATURE_DEBUG_LOG:
      ret_value = uim_ptr->setting.features_status_list.debug_log;
      break;
    case UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL:
      ret_value = uim_ptr->setting.features_status_list.rpt_bad_sw_on_poll;
      break;
    case UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE:
      ret_value = uim_ptr->setting.features_status_list.handle_iccid_read_failure;
      break;
    case UIMDRV_FEATURE_SUPPORT_NO_ICCID:
      ret_value = uim_ptr->setting.features_status_list.support_no_iccid;
      break;
    case UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT:
      ret_value = uim_ptr->setting.features_status_list.min_tpl_iccid_support;
      break;
    case UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT:
      ret_value = uim_ptr->setting.features_status_list.handle_unknown_proc_bytes_as_cmd_timeout;
      break;
    case UIMDRV_FEATURE_INTERFACE_NOT_USED:
#ifdef T_RUMI_EMULATION
/* during RUMI bringup we don't want the UIM interface to
   be enabled until all UIM aspects have been verified */
      ret_value = TRUE;
#else
      ret_value = uim_ptr->setting.features_status_list.interface_not_used;
#endif
      break;
    case UIMDRV_FEATURE_LOG_APDU_TO_EFS:
      ret_value = uim_ptr->setting.features_status_list.log_apdu_to_efs;
      break;
    case UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY:
      ret_value = uim_ptr->setting.features_status_list.no_switch_inst_on_wwt_expiry;
      break;
    case UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH:
      ret_value = uim_ptr->setting.features_status_list.send_status_before_auth;
      break;
    case UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD:
      ret_value = uim_ptr->setting.features_status_list.try_default_baud_rate_for_f372_d12_card;
      break;
    case UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH:
      ret_value = uim_ptr->setting.features_status_list.cold_reset_due_to_card_switch;
      break;
    case UIMDRV_FEATURE_SM_USE_SLOT_1:
      ret_value = uim_ptr->setting.features_status_list.sm_prefer_slot1;
      break;
    case UIMDRV_FEATURE_USE_DUAL_LDO:
      ret_value = uim_ptr->setting.features_status_list.use_dual_ldo;
      break;
    case UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY:
      ret_value = uim_ptr->setting.features_status_list.uim_polling_only_at_polling_timer_expiry;
      break;
    case UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ:
      ret_value = uim_ptr->setting.features_status_list.uim_set_clk_freq_at_4_8_MHz;
      break;
    case UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME:
      ret_value = uim_ptr->setting.features_status_list.handle_tc1_byte_for_extra_guard_time;
      break;
    case UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET:
      ret_value = uim_ptr->setting.features_status_list.enable_sim_mode_change_via_warm_reset;
      break;
    case UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF:
      ret_value = uim_ptr->setting.features_status_list.enable_explicit_selection_of_mf_or_adf;
      break;
    case UIMDRV_FEATURE_ENABLE_BOOT_UP_IN_INVERSE_CONVENTION:
      ret_value =  uim_ptr->setting.features_status_list.enable_boot_up_in_inverse_convention;
      break;
    case UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD:
      ret_value =  uim_ptr->setting.features_status_list.recovery_on_bad_status_word ;
      break;
    case UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY:
      ret_value =  uim_ptr->setting.features_status_list.attempt_pup_3v_from_nth_recovery ;
      break;
    case UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU:
      ret_value =  uim_ptr->setting.features_status_list.enable_le_support_for_stream_7816_apdu ;
      break;
    case UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP:
      ret_value =  uim_ptr->setting.features_status_list.disable_card_status_check_at_power_up;
      break;
    case UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED:
      ret_value =  uim_ptr->setting.features_status_list.simtray_with_gpios_tied;
      break;
    case UIMDRV_FEATURE_DISABLE_PROACTIVE_POLLING:
      ret_value = uim_ptr->setting.features_status_list.disable_proactive_polling;
      break;
    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid Feature 0x%x", nv_subfeature);
      break;
  }
  return ret_value;
} /* uim_nv_is_feature_enabled */


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
)
{
  nv_cmd_type nv_cmd_buf;           /* nv command buffer */

  if(data_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"ERR: data_ptr is NULL!!!");
    return FALSE;
  }
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_WRITE_F;     /* Write request        */
  nv_cmd_buf.tcb_ptr    = rex_self();     /* Notify back to uim   */
  nv_cmd_buf.sigs       = UIM_NV_CMD_SIG; /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;           /* No buffer to return  */
  nv_cmd_buf.item       = item;           /* Item to write        */
  nv_cmd_buf.data_ptr   = data_ptr;       /* Data to be written   */

  /* Clear signal, issue the command, and wait for the respone. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( rex_self(), UIM_NV_CMD_SIG );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  (void)uim_pet_and_wait((UIM_NV_CMD_SIG), uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( rex_self(), UIM_NV_CMD_SIG );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,"Failed to write item to NV 0x%x",
                     nv_cmd_buf.item);
    return FALSE;
  }
  return TRUE;
} /* uim_put_nv */


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
)
{
  nv_cmd_type nv_cmd_buf;           /* nv command buffer */

  if(data_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"ERR: data_ptr is NULL!!!");
    return FALSE;
  }
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;      /* Read request         */
  nv_cmd_buf.tcb_ptr    = rex_self();     /* Notify back to uim   */
  nv_cmd_buf.sigs       = UIM_NV_CMD_SIG; /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;           /* No buffer to return  */
  nv_cmd_buf.item       = item;           /* Item to write        */
  nv_cmd_buf.data_ptr   = data_ptr;       /* Data to be written   */

  /* Clear signal, issue the command, and wait for the respone. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( rex_self(), UIM_NV_CMD_SIG );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  (void)uim_pet_and_wait((UIM_NV_CMD_SIG), uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( rex_self(), UIM_NV_CMD_SIG );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,"Failed to read item to NV 0x%x",
                     nv_cmd_buf.item);
    return FALSE;
  }
  return TRUE;
} /* uim_get_nv */


/**
 * Reads all the slot based UIM DRV NV items(carrier and
 * non-carrier dependent) using common API
 * uim_common_efs_read() and uim_common_efs_write().
 *
 * Current slot/context based NVs:
 *      1. PDOWN ON CONSECUTIVE TECH PROB 6F00[carrier dependent]
 *      2. UIM JCDMA Mode [carrier dependent]
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_read_nv_from_efs_slot(uim_instance_global_type  *uim_ptr)
{
#ifdef FEATURE_UIM_BTSAP_CLIENT
  uim_last_mode_and_config_delay_for_remote_connect_type last_context;
#endif /* FEATURE_UIM_BTSAP_CLIENT */
  
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  /* === Read the Power down configuration from NV === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_UIMDRV_PDOWN_NV_EF,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           (uim_common_efs_context_type)uim_ptr->id,
                           (uint8 *)&(uim_ptr->setting.pdown_on_tech_prob),
                           sizeof(uim_ptr->setting.pdown_on_tech_prob)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Power down configuration on technical problems read from EFS");
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Failed to read EFS power down configuration");
    uim_ptr->setting.pdown_on_tech_prob.to_pdown         = UIMDRV_PDOWN_CONSECUTIVE_TECH_PROB_DEFAULT;
    uim_ptr->setting.pdown_on_tech_prob.cnt_tech_problem = UIMDRV_REPEATED_TECH_PROBLEMS_DEFAULT;
  }

  /* === Read the JCDMA from NV === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_UIMDRV_JCDMA_MODE,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           (uim_common_efs_context_type)uim_ptr->id,
                           (uint8 *)&(uim_ptr->setting.jcdma_mode),
                           sizeof(uim_ptr->setting.jcdma_mode)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM JCDMA mode read from EFS");
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Failed to read EFS item UIM JCDMA mode");
    uim_ptr->setting.jcdma_mode.service  = UIM_JCDMA_SERVICE_DISABLED;
  }

#ifdef FEATURE_UIM_BTSAP_CLIENT
  /* === Read the onnection mode (ISO\Remote) and Delay time for remote connection from NV === */
  /* This is a slot based NV item which will read using common EFS read API */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT,
                                                     UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                     (uim_common_efs_context_type)uim_ptr->id,
                                                     (uint8 *)&(last_context),
                                                     sizeof(last_context)))
  {
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"UIMDRV last operation mode %d wait time 0x%x is read from EFS",
		                   last_context.connection_mode, last_context.delay_time_insec);
    uim_ptr->state.last_connection_mode = (uim_slot_connection_enum_type)last_context.connection_mode;
    uim_ptr->state.local_card_pup_delay = last_context.delay_time_insec;
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIMDRV last operation mode read failed");
  }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

}/*uim_read_nv_from_efs_slot*/


/**
 * Reads all the device based required UIMDRV NV items using
 * uim_common_efs_read().
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_read_nv_from_efs_device(uim_instance_global_type  *uim_ptr)
{

#ifdef FEATURE_UIM_REMOTE_UIM
  uim_remote_cmd_rsp_timer_nv_type uim_remote_cmd_rsp_timer_nv;
#endif /* FEATURE_UIM_REMOTE_UIM */
  uint32 uim_supply_voltage_ind_ack_sanity_timer = 0;

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  /* === Read UIMDRV Features Status List from EFS === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->setting.features_status_list),
                                                    sizeof(uim_ptr->setting.features_status_list)))
  {
    uim_nv_features_status_list_update_version(UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,
                                               uim_ptr);
  }
  else
  {
    uim_nv_write_default_value_of_all_features(UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,uim_ptr);
    uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         UIM_COMMON_EFS_DEVICE,
                        (char *)&(uim_ptr->setting.features_status_list),
                         sizeof(uim_ptr->setting.features_status_list));
  }

    /* === Read HOTSWAP Debounce Config from EFS === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->hotswap.hotswap_me_conf),
                                                    sizeof(uim_ptr->hotswap.hotswap_me_conf)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Hotswap Debounce Config read from NV");
  }
  else
  {
    uim_ptr->hotswap.hotswap_me_conf.auxiliary_period_for_card_detect = UIM_HOTSWAP_AUX_SLOT_PERIOD;
    uim_ptr->hotswap.hotswap_me_conf.maximum_debounce_retry_counter   = UIM_HOTSWAP_MAX_DEBOUNCE_RETRY;
    uim_ptr->hotswap.hotswap_me_conf.num_of_sample_for_insertion      = UIM_HOTSWAP_NUM_OF_SAMPLES_TO_DETECT_CARD_INSERTION;
    uim_ptr->hotswap.hotswap_me_conf.num_of_sample_for_removal        = UIM_HOTSWAP_NUM_OF_SAMPLES_TO_DETECT_CARD_REMOVAL;
    if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG,
                                                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                       UIM_COMMON_EFS_DEVICE,
                                                      (char *)&(uim_ptr->hotswap.hotswap_me_conf),
                                                       sizeof(uim_ptr->hotswap.hotswap_me_conf)))
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Writing default values to Hotswap Debounce Config EFS");
    }
    else
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Writing default value to Hotswap Debounce Config EFS failed");
    }
  }

  /* === Read Active slot configuration for Sub Manager from EFS === */
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  if(UIM_SLOT_NONE == uim_sm_data.new_active_slot)
  {
    if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_ACTIVE_SLOT,
                                                      UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                      UIM_COMMON_EFS_DEVICE,
                                                     (uint8 *)&(uim_active_slot_configuration),
                                                      sizeof(uim_active_slot_configuration)))
    {
      uim_sm_data.new_active_slot = uim_active_slot_configuration.active_slot;
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Active slot configuration read from NV, %x ",
                        uim_sm_data.new_active_slot);
    }
    else
    {
      uim_sm_data.new_active_slot = UIM_SLOT_1;
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Read to NV active slot configuration unsuccessful");
    }
    if (uim_sm_data.new_active_slot < UIM_SLOT_1 || uim_sm_data.new_active_slot > UIM_NUM_PHY_SLOTS)
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Invalid active slot value read from NV %x, resetting to slot1",
                        uim_sm_data.new_active_slot);
      uim_sm_data.new_active_slot = UIM_SLOT_1;
    }
    uim_sm_data.uim_slot_info[uim_sm_data.new_active_slot -1].activity_status = UIM_SLOT_STATE_ACTIVE;
  }
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

  /* === Read the UIM_DISABLE_POLLING_EFS_FILE configuration from EFS === */
  if (UIM_COMMON_EFS_SUCCESS != uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_DISABLE_POLLING,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->flag.uim_disable_polling),
                                                    sizeof(uim_ptr->flag.uim_disable_polling)))
  {
    /* if cannot read EFS value set to enable polling by default */
    uim_ptr->flag.uim_disable_polling = FALSE;
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Failed to read UIM_DISABLE_POLLING_EFS_FILE from EFS, set to enable polling");
  }

  /* === Read the UIM_RESPONSE_TIMERS_CONFIG from EFS === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_BUSY_RESPONSE,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->setting.uim_feature_busy_response),
                                                    sizeof(uim_ptr->setting.uim_feature_busy_response)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Successfully read the UIM_RESPONSE_TIMERS_CONFIG from EFS");
    /* Validate timer values and set both to default if invalid*/
    if ( uim_ptr->setting.uim_feature_busy_response.uim_busy_ind_timer_val_ms >= uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms
         && uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms != 0 )
    {
      uim_ptr->setting.uim_feature_busy_response.uim_busy_ind_timer_val_ms = (uint32)UIM_BUSY_IND_TIMER_VAL_MS_DEFAULT;
      uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms    = (uint32)UIM_TRANS_TIMER_VAL_MS_DEFAULT;
      uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_BUSY_RESPONSE,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           UIM_COMMON_EFS_DEVICE,
                          (char *)&(uim_ptr->setting.uim_feature_busy_response),
                           sizeof(uim_ptr->setting.uim_feature_busy_response));
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Invalid inputs, uim_ind_timer_val_ms > uim_trans_timer_val_ms, force set to defaults.");
    }
  }
  else
  {
    uim_ptr->setting.uim_feature_busy_response.uim_busy_ind_timer_val_ms = (uint32)UIM_BUSY_IND_TIMER_VAL_MS_DEFAULT;
    uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms    = (uint32)UIM_TRANS_TIMER_VAL_MS_DEFAULT;
    uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_BUSY_RESPONSE,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         UIM_COMMON_EFS_DEVICE,
                        (char *)&(uim_ptr->setting.uim_feature_busy_response),
                         sizeof(uim_ptr->setting.uim_feature_busy_response));
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_RESPONSE_TIMERS_CONFIG, force set to defaults");
  }

  /* Read the UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE from EFS */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->debug.uim_feature_busy_response_simulate_null),
                                                    sizeof(uim_ptr->debug.uim_feature_busy_response_simulate_null)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Successfully read the UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE from EFS");
  }
  else
  {
    /* simulate null timer is disabled by default */
    uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null = TRUE;
    uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins = UIM_SIMULATE_NULL_INS_DEFAULT;
    uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         UIM_COMMON_EFS_DEVICE,
                        (char *)&(uim_ptr->debug.uim_feature_busy_response_simulate_null),
                         sizeof(uim_ptr->debug.uim_feature_busy_response_simulate_null));
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE, force set to defaults");
  }

  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);
  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);
  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_simulate_null_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);

#ifdef FEATURE_UIM_REMOTE_UIM
#ifdef FEATURE_UIM_BTSAP_CLIENT
  /* Read remote command response time from NV */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_REMOTE_COMMAND_RESP_TIMER,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_remote_cmd_rsp_timer_nv),
                                                    sizeof(uim_remote_cmd_rsp_timer_nv)))
  {
    uim_ptr->remote.btsap.btsap_cmd_rsp_timer_value = uim_remote_cmd_rsp_timer_nv.timer;
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"BTSAP:Successfully read the remote command response timer from NV, 0x%x ",
                      uim_remote_cmd_rsp_timer_nv.timer);
  }
  else
  {
    uim_ptr->remote.btsap.btsap_cmd_rsp_timer_value = UIM_BTSAP_TRANSACTION_TIME_DURATION;
    UIMDRV_MSG_HIGH_0(uim_ptr->id," BTSAP:Fail to read the remote command response timer from NV ");
  }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_USB_UICC
  /* Read remote command response time from NV */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_REMOTE_COMMAND_RESP_TIMER,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_remote_cmd_rsp_timer_nv),
                                                    sizeof(uim_remote_cmd_rsp_timer_nv)))
  {
    uim_ptr->usb.usb_interface_status.cmd_rsp_time  = uim_remote_cmd_rsp_timer_nv.timer;
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"USB UICC:Successfully read the remote command response timer from NV, 0x%x ",
                      uim_remote_cmd_rsp_timer_nv.timer);
  }
  else
  {
    uim_ptr->usb.usb_interface_status.cmd_rsp_time = UIM_USB_DEFAULT_CMD_RSP_TIME;
    UIMDRV_MSG_HIGH_0(uim_ptr->id," USB UICC:Fail to read the remote command response timer from NV ");
  }
#endif /* FEATURE_UIM_USB_UICC */
#endif /* FEATURE_UIM_REMOTE_UIM */

if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE,
                                                  UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                  UIM_COMMON_EFS_DEVICE,
                                                 (uint8 *)&(uim_ptr->extended_recovery.recovery_config),
                                                  sizeof(uim_feature_extended_recovery_nv_type)))
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Successfully read the UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE from EFS");
  if( uim_ptr->extended_recovery.recovery_config.no_of_extended_powerups > 
                                                       UIM_ERECOV_NUM_PUPS_MAX)
  {
    uim_ptr->extended_recovery.recovery_config.no_of_extended_powerups = 
                                                       UIM_ERECOV_NUM_PUPS_MAX;
  }
  if(uim_ptr->extended_recovery.recovery_config.delay_value < 
                                                      UIM_ERECOV_DEFAULT_DELAY)
  {
    uim_ptr->extended_recovery.recovery_config.delay_value = 
		                                              UIM_ERECOV_DEFAULT_DELAY;
  }
  if(uim_ptr->extended_recovery.recovery_config.delay_value > 
  	                                              UIM_ERECOV_DEFAULT_DELAY_MAX)
  {
    uim_ptr->extended_recovery.recovery_config.delay_value = 
		                                          UIM_ERECOV_DEFAULT_DELAY_MAX;
  }
}
else
{
  uim_ptr->extended_recovery.recovery_config.no_of_extended_powerups = UIM_ERECOV_NUM_DEFAULT_PUPS;
  uim_ptr->extended_recovery.recovery_config.delay_value = UIM_ERECOV_DEFAULT_DELAY;
  uim_ptr->extended_recovery.recovery_config.incremental_delay = UIM_ERECOV_DEFAULT_INCREMENT;
  uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE,
                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                       UIM_COMMON_EFS_DEVICE,
                      (char *)&(uim_ptr->extended_recovery.recovery_config),
                       sizeof(uim_feature_extended_recovery_nv_type));
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE, force set to defaults");
} 
uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_NV_INIT);

if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE,
                                                  UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                  UIM_COMMON_EFS_DEVICE,
                                                 (uint8 *)&(uim_ptr->poll.rel12_polling_enhancement_enable),
                                                  sizeof(uim_ptr->poll.rel12_polling_enhancement_enable)))
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Successfully read the UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE from EFS");
}
else
{
  uim_ptr->poll.rel12_polling_enhancement_enable = UIM_REL12_POLLING_ENHANCEMENT_DEFAULT;
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE, force set to defaults");
} 

/* Read voltage supply indication acknowledgement wait timer value from NV */
 if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER,
                                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                   UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_supply_voltage_ind_ack_sanity_timer),
                                                   sizeof(uim_supply_voltage_ind_ack_sanity_timer)))
 {
   if(uim_supply_voltage_ind_ack_sanity_timer > UIM_MAX_DELAY_QMI_ACK_FOR_VOLTAGE_SUPPLY_IND)
   {
     uim_supply_voltage_ind_ack_sanity_timer = UIM_MAX_DELAY_QMI_ACK_FOR_VOLTAGE_SUPPLY_IND;
   }
   uim_ptr->setting.voltage_indication_to_qmi.sanity_timer_value  = uim_supply_voltage_ind_ack_sanity_timer;
 }
 else
 {
   uim_ptr->setting.voltage_indication_to_qmi.sanity_timer_value = UIM_DEFAULT_DELAY_QMI_ACK_FOR_VOLTAGE_SUPPLY_IND;
   UIMDRV_MSG_HIGH_0(uim_ptr->id," Fail to read the voltage supply indivation acknowledgement wait timer from NV ");
 }


}/* uim_read_nv_from_efs_device */


/**
 * NV items are read only once during power up by the task that
 * attempts to initialize first. This procedure initializes the
 * nv globals of the uim tasks that follow, with the values read
 * by the uim instance that initializes first.
 *
 * @param uim_ptr Pointer to the global common data.
 */
static void uim_init_nv_globals_with_read_values_from_efs( uim_instance_global_type *uim_ptr )
{
  uim_shared_global_type   *shared_globals_ptr = uim_get_common_globals_ptr();
  uim_instance_global_type *uim_temp_ptr =
                               uim_get_instance_ptr(shared_globals_ptr->nv_read_by_instance);

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if (uim_temp_ptr == NULL)
  {
    return;
  }

  /* === NV_UIM_FIRST_INST_CLASS_I === */
  uim_ptr->state.first_inst_class = uim_temp_ptr->state.first_inst_class;

  /* === NV_GPRS_ANITE_GCF_I === */
  uim_ptr->flag.gcf_testing_flag = uim_temp_ptr->flag.gcf_testing_flag;

  /* === NV_UIM_PREF_PROTOCOL_I === */
  uim_ptr->t1.nv_pref_protocol = uim_temp_ptr->t1.nv_pref_protocol;

  /* === NV_UIM_PAR_ERR_WORKAROUND_I === */
  uim_ptr->setting.par_err_workaround = uim_temp_ptr->setting.par_err_workaround;

  /* === NV_UIM_CONFIG_PARAMS_I === */
  uim_memscpy(&uim_ptr->setting.config_params,
          sizeof(uim_ptr->setting.config_params),
          &uim_temp_ptr->setting.config_params,
          sizeof(uim_temp_ptr->setting.config_params));

  /* === UIM_FEATURES_STATUS_LIST_FILE === */
  uim_memscpy(&uim_ptr->setting.features_status_list,
              sizeof(uim_features_status_list_type),
              &uim_temp_ptr->setting.features_status_list,
              sizeof(uim_features_status_list_type));

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
                               uim_ptr) == TRUE)
  {
    if(NULL == uim_ptr->state.iccid_tpl_ptr)
    {
      uim_ptr->state.iccid_tpl_ptr = uim_malloc(sizeof(nv_uim_iccid_tpl_type));
      if(NULL != uim_ptr->state.iccid_tpl_ptr)
      {
        memset(uim_ptr->state.iccid_tpl_ptr->iccid_data,0x00,NV_ICCID_MAX_SIZE);
        uim_ptr->state.iccid_tpl_ptr->terminal_profile_len = UIM_MINIMUM_TP_LENGTH;
      }
    }
    if(NULL != uim_temp_ptr->state.iccid_tpl_ptr &&
       NULL != uim_ptr->state.iccid_tpl_ptr )
    {
      uim_memscpy( uim_ptr->state.iccid_tpl_ptr->iccid_data,
                   sizeof(uim_ptr->state.iccid_tpl_ptr->iccid_data),
                   uim_temp_ptr->state.iccid_tpl_ptr->iccid_data,
                   sizeof(uim_temp_ptr->state.iccid_tpl_ptr->iccid_data) );
      uim_ptr->state.iccid_tpl_ptr->terminal_profile_len =
                                uim_temp_ptr->state.iccid_tpl_ptr->terminal_profile_len;
    }
  }

  /* === UIM_HOTSWAP_DEBOUNCE_CONFIG_FILE === */
  uim_memscpy( &uim_ptr->hotswap.hotswap_me_conf,
               sizeof(uim_ptr->hotswap.hotswap_me_conf),
               &uim_temp_ptr->hotswap.hotswap_me_conf,
               sizeof(uim_temp_ptr->hotswap.hotswap_me_conf) );

  /* === UIM_RESPONSE_TIMERS_CONFIG === */
  uim_memscpy( &uim_ptr->setting.uim_feature_busy_response,
               sizeof(uim_ptr->setting.uim_feature_busy_response),
               &uim_temp_ptr->setting.uim_feature_busy_response,
               sizeof(uim_temp_ptr->setting.uim_feature_busy_response) );
  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);
  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);

  /* === UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE === */
  uim_memscpy( &uim_ptr->debug.uim_feature_busy_response_simulate_null,
               sizeof(uim_ptr->debug.uim_feature_busy_response_simulate_null),
               &uim_temp_ptr->debug.uim_feature_busy_response_simulate_null,
               sizeof(uim_temp_ptr->debug.uim_feature_busy_response_simulate_null) );
  /* Define timer and Set the timer state to NOT_STARTED from NOT_USE*/
  uim_simulate_null_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NV_INIT);
  uim_ptr->flag.uim_disable_polling           = uim_temp_ptr->flag.uim_disable_polling;

#ifdef FEATURE_UIM_BTSAP_CLIENT
  uim_ptr->remote.btsap.btsap_cmd_rsp_timer_value =
                               uim_temp_ptr->remote.btsap.btsap_cmd_rsp_timer_value;
#endif /* FEATURE_UIM_BTSAP_CLIENT */
#ifdef FEATURE_UIM_USB_UICC
  uim_ptr->usb.usb_interface_status.cmd_rsp_time =
                               uim_temp_ptr->usb.usb_interface_status.cmd_rsp_time;
#endif /* FEATURE_UIM_USB_UICC */

/* === UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE === */
uim_memscpy( &uim_ptr->extended_recovery.recovery_config,
			 sizeof(uim_ptr->extended_recovery.recovery_config),
			 &uim_temp_ptr->extended_recovery.recovery_config,
			 sizeof(uim_temp_ptr->extended_recovery.recovery_config) );
uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_NV_INIT);

uim_memscpy( &uim_ptr->poll.rel12_polling_enhancement_enable,
			 sizeof(uim_ptr->poll.rel12_polling_enhancement_enable),
			 &uim_temp_ptr->poll.rel12_polling_enhancement_enable,
			 sizeof(uim_temp_ptr->poll.rel12_polling_enhancement_enable) );

}/* uim_init_nv_globals_with_read_values_from_efs */


/**
 * Reads the legacy device NV items that cannot be read using
 * uim_common_efs_read() but using nv_cmd() instead.
 *
 * @param uim_ptr Pointer to the global data common.
 * @param task_ptr Pointer to the current TCB.
 * @param task_wait_sig Signal mask to be waited.
 * @param task_wait_handler Handler for waiting task_wait_sig
 */
static void uim_read_legacy_nv_device
(
  uim_instance_global_type *uim_ptr,
  rex_tcb_type             *task_ptr,
  rex_sigs_type            task_wait_sig,
  void                    (*task_wait_handler)( rex_sigs_type, uim_instance_global_type*)
)
{
  /* Command buffer to NV */
  nv_cmd_type  *nv_cmd_buf_ptr      = NULL;
  /* NV data buffer */
  nv_item_type *uim_nv_data_buf_ptr = NULL;

  nv_cmd_buf_ptr      = uim_malloc(sizeof(nv_cmd_type));
  uim_nv_data_buf_ptr = uim_malloc(sizeof(nv_item_type));

  if(  NULL == nv_cmd_buf_ptr ||
      NULL == uim_nv_data_buf_ptr || 
       NULL == task_wait_handler  )
  {
     return;
  }

  /* Prepare command buffer to NV. */
  nv_cmd_buf_ptr->cmd        = NV_READ_F;             /* Read request        */
  nv_cmd_buf_ptr->tcb_ptr    = task_ptr;              /* Notify back to me   */
  nv_cmd_buf_ptr->sigs       = task_wait_sig;         /* With this signal    */
  nv_cmd_buf_ptr->done_q_ptr = NULL;                  /* No buffer to return */
  nv_cmd_buf_ptr->item       = NV_UIM_FIRST_INST_CLASS_I; /* Item to get     */
  nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( nv_cmd_buf_ptr );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig, uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf_ptr->status != NV_DONE_S)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Bad NV read status 0x%x",
                      nv_cmd_buf_ptr->status );
    uim_ptr->state.first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
  }
  else
  {
    /* Check if valid value is read from NV (NV value is always >= 0 hence
       that check is not necessary */
    if(uim_nv_data_buf_ptr->uim_first_inst_class <=
      NV_UIM_FIRST_INST_CLASS_UMTS_SIM)
    {
      uim_ptr->state.first_inst_class = uim_nv_data_buf_ptr->uim_first_inst_class;
    }
    else /* Set default value for instruction class */
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Undefined value for 1st inst class %d, setting to default",
                        uim_nv_data_buf_ptr->uim_first_inst_class );
      uim_ptr->state.first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
    }
  }

  /* Get the GCF testing FLAG */
  /* Prepare command buffer to NV. */
  nv_cmd_buf_ptr->cmd        = NV_READ_F;           /* Read request        */
  nv_cmd_buf_ptr->tcb_ptr    = task_ptr;            /* Notify back to m    */
  nv_cmd_buf_ptr->sigs       = task_wait_sig;       /* With this signal    */
  nv_cmd_buf_ptr->done_q_ptr = NULL;                /* No buffer to return */
  nv_cmd_buf_ptr->item       = NV_GPRS_ANITE_GCF_I; /* Item to get         */
  nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( nv_cmd_buf_ptr );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig, uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  uim_ptr->flag.gcf_testing_flag = (nv_cmd_buf_ptr->status == NV_DONE_S) ?
    uim_nv_data_buf_ptr->gprs_anite_gcf : FALSE;

#ifdef FEATURE_UIM_T_1_SUPPORT
/* Prepare command buffer to NV. */
  nv_cmd_buf_ptr->cmd        = NV_READ_F;               /* Read request         */
  nv_cmd_buf_ptr->tcb_ptr    = task_ptr;                /* Notify back to me    */
  nv_cmd_buf_ptr->sigs       = task_wait_sig;           /* With this signal     */
  nv_cmd_buf_ptr->done_q_ptr = NULL;                    /* No buffer to return  */
  nv_cmd_buf_ptr->item       = NV_UIM_PREF_PROTOCOL_I;  /* Item to get        */
  nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;       /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( nv_cmd_buf_ptr );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig, uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf_ptr->status != NV_DONE_S)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Bad NV read status 0x%x",
                      nv_cmd_buf_ptr->status );
    uim_ptr->t1.nv_pref_protocol = NV_UIM_PREF_PROTOCOL_T_0;
  }
  else
  {
    uim_ptr->t1.nv_pref_protocol = uim_nv_data_buf_ptr->uim_pref_protocol;
  }
#endif /* FEATURE_UIM_T_1_SUPPORT */

  /* Prepare command buffer to NV. */
  nv_cmd_buf_ptr->cmd        = NV_READ_F;                    /* Read request         */
  nv_cmd_buf_ptr->tcb_ptr    = task_ptr;                     /* Notify back to me    */
  nv_cmd_buf_ptr->sigs       = task_wait_sig;                /* With this signal     */
  nv_cmd_buf_ptr->done_q_ptr = NULL;                         /* No buffer to return  */
  nv_cmd_buf_ptr->item       = NV_UIM_PAR_ERR_WORKAROUND_I;  /* Item to get  */
  nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;            /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( nv_cmd_buf_ptr );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig, uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf_ptr->status != NV_DONE_S)
  {
    /* If the NV read status is a failure, then the SW workaround for
       the GCF 27.11.1.5 testcase doesn't execute. */
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Bad NV read status 0x%x",
                      nv_cmd_buf_ptr->status );
    uim_ptr->setting.par_err_workaround = FALSE;
  }
  else
  {
    uim_ptr->setting.par_err_workaround = uim_nv_data_buf_ptr->uim_par_err_workaround;
  }

  /* Read the configurable parameters from NV */

  nv_cmd_buf_ptr->cmd        = NV_READ_F;              /* Read request         */
  nv_cmd_buf_ptr->tcb_ptr    = task_ptr;               /* Notify back to me    */
  nv_cmd_buf_ptr->sigs       = task_wait_sig;          /* With this signal     */
  nv_cmd_buf_ptr->done_q_ptr = NULL;                   /* No buffer to return  */
  nv_cmd_buf_ptr->item       = NV_UIM_CONFIG_PARAMS_I; /* Item to get        */
  nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;      /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( nv_cmd_buf_ptr );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig, uim_ptr);

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  uim_validate_and_set_config_params(&uim_ptr->setting.config_params,
                                    &(uim_nv_data_buf_ptr->uim_config_params),
                                    nv_cmd_buf_ptr->status,
                                     uim_ptr);
  /* Write back NV4205 if NV is not initialized */
  if (NV_DONE_S != nv_cmd_buf_ptr->status)
  {
    if (NV_NOTACTIVE_S == nv_cmd_buf_ptr->status)
    {
      /* Copy the default values from uim config params back to the nv_cmd_buf */
      uim_memscpy(&(uim_nv_data_buf_ptr->uim_config_params),
                   sizeof(uim_nv_data_buf_ptr->uim_config_params),
                  &uim_ptr->setting.config_params,
                   sizeof(uim_ptr->setting.config_params));
      /* In case if the NV Item is inactivated state  */
      uim_put_nv( NV_UIM_CONFIG_PARAMS_I,
                  (nv_item_type *)(&(uim_nv_data_buf_ptr->uim_config_params)),
                  uim_ptr);
    }
    else
    {
      /* NV item is not supported, freeing memory */
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Bad NV read status for NV_UIM_CONFIG_PARAMS_I 0x%x",
                        nv_cmd_buf_ptr->status);
    }
  }
  /* Check NV read status */
  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
                               uim_ptr) == TRUE)
  {
    if(NULL == uim_ptr->state.iccid_tpl_ptr)
    {
      uim_ptr->state.iccid_tpl_ptr = uim_malloc(sizeof(nv_uim_iccid_tpl_type));
      if(NULL != uim_ptr->state.iccid_tpl_ptr)
      {
        memset(uim_ptr->state.iccid_tpl_ptr->iccid_data,0x00,NV_ICCID_MAX_SIZE);
        uim_ptr->state.iccid_tpl_ptr->terminal_profile_len = UIM_MINIMUM_TP_LENGTH;
      }
    }

    /* Read ICCID and Terminal Profile length from nv */
    nv_cmd_buf_ptr->cmd        = NV_READ_F;           /* Read request         */
    nv_cmd_buf_ptr->tcb_ptr    = task_ptr;            /* Notify back to me    */
    nv_cmd_buf_ptr->sigs       = task_wait_sig;       /* With this signal     */
    nv_cmd_buf_ptr->done_q_ptr = NULL;                /* No buffer to return  */
    nv_cmd_buf_ptr->item       = NV_UIM_ICCID_TPL_I;  /* Item to get          */
    nv_cmd_buf_ptr->data_ptr   = uim_nv_data_buf_ptr ;   /* Where to return it   */

    /* Clear signal, issue the command, and wait for the response. */
    /* Clear signal for NV  */
    (void) rex_clr_sigs( task_ptr, task_wait_sig );

    /* Issue the request    */
    nv_cmd( nv_cmd_buf_ptr );

    /* Wait for completion  */
    task_wait_handler( task_wait_sig, uim_ptr);

    /* clear the signal again */
    (void) rex_clr_sigs( task_ptr, task_wait_sig );

     /* Check NV read status */
    if (nv_cmd_buf_ptr->status != NV_DONE_S)
    {
      if (NV_NOTACTIVE_S == nv_cmd_buf_ptr->status)
      {
        /* In case if the NV Item is inactivated state  */
        if ( FALSE == uim_put_nv(
                       NV_UIM_ICCID_TPL_I,
                       (nv_item_type *)uim_ptr->state.iccid_tpl_ptr,
                       uim_ptr)
            )
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"Error during writing the TPL to NV_UIM_ICCID_TPL_I");
          /* NV item is not supported, freeing memory */
          UIM_FREE(uim_ptr->state.iccid_tpl_ptr);
        }
      }
      else
      {
        /* NV item is not supported, freeing memory */
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"Bad NV read status for NV_UIM_ICCID_TPL_I  0x%x",
                          nv_cmd_buf_ptr->status);
        UIM_FREE(uim_ptr->state.iccid_tpl_ptr);
      }
    }
    else
    {
      if(NULL != uim_ptr->state.iccid_tpl_ptr)
      {
        /* Reading ICCID and Terminal profile length */
        uim_memscpy(uim_ptr->state.iccid_tpl_ptr->iccid_data,
               sizeof(uim_ptr->state.iccid_tpl_ptr->iccid_data),
               uim_nv_data_buf_ptr->uim_iccid_tpl.iccid_data,
               NV_ICCID_MAX_SIZE);
        uim_ptr->state.iccid_tpl_ptr->terminal_profile_len =
                                uim_nv_data_buf_ptr->uim_iccid_tpl.terminal_profile_len;
      }
    }
  } /* endif uim_nv_is_feature_enabled */

  UIM_FREE(nv_cmd_buf_ptr);
  UIM_FREE(uim_nv_data_buf_ptr);
}/* uim_read_legacy_nv_device */


/**
 * This function opens the file, compares its contents against
 * the passed buffer and if the two are different, empties the
 * file and writes back the buffer data to the file. It also
 * writes the buffer data to the file if file couldn't be read.
 *
 * @param efs_item file path enum.
 * @param buf_ptr Buffer to compare against the file data.
 * @param buf_len Buffer length.
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_compare_and_update_conf_file(
  uim_common_efs_item_type    efs_item,
  const char *                buf_ptr,
  uint16                      buf_len,
  uim_instance_global_type   *uim_ptr
)
{
  char                   *file_data_ptr         = NULL;
  uint32                  file_size             = 0;

  if (!buf_ptr || buf_len == 0)
  {
    return;
  }

  /* Get the size of the file */
  if(UIM_COMMON_EFS_SUCCESS == uim_common_efs_get_file_size(efs_item,UIM_COMMON_EFS_DEVICE,&file_size) &&
     file_size == buf_len)
    {
      /* If the file size is non-zero, read data from file to compare against the
         buffer and write back to the file only if the data is different. If file
         read fails, still write the buffer to the file  */
      file_data_ptr = uim_malloc(file_size);
      if(file_data_ptr != NULL)
      {
        if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( efs_item,
                                                         UIM_COMMON_EFS_REGULAR_FILE_TYPE,
                                                         UIM_COMMON_EFS_DEVICE,
                                                        (uint8 *)file_data_ptr,
                                                         file_size))
        if(memcmp(file_data_ptr, buf_ptr, buf_len) == 0)
        {
          /* file data and the buffer data are same,
             so return without updating the file */
          UIM_FREE(file_data_ptr);
          return;
        }
      }
      UIM_FREE(file_data_ptr);
    }
  uim_common_efs_write(efs_item, UIM_COMMON_EFS_ITEM_FILE_TYPE, UIM_COMMON_EFS_DEVICE, (char *)buf_ptr,buf_len);

} /* uim_compare_and_update_conf_file */


/**
 * Creates the .conf file if not present
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_update_conf_file(uim_instance_global_type *uim_ptr)
{
  char           *buffer      = NULL;
  uint16          index       = 0;
  uint16          buf_len     = 0;
  uint32          file_size   = 0;

  buf_len = strlen(UIM_COMMON_EFS_PATH_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURES_STATUS_LIST)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_HW_CONFIG)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_SUPPORT_HOTSWAP)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_ACTIVE_SLOT)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_DISABLE_POLLING)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_BUSY_RESPONSE)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL)
#ifdef FEATURE_UIM_REMOTE_UIM
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_REMOTE_COMMAND_RESP_TIMER)
#endif
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_EXTENDED_RECOVERY_FILE)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE)
            + strlen(UIM_COMMON_EFS_PATH_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER)
            + 50;

  /*Allocating memory to the buffer to hold all the string names plus additional 50 bytes */
  buffer = uim_malloc(buf_len);

  if(NULL == buffer)
  {
    /* if allocation fails return */
    return;
  }

  /* Copy the HOTSWAP Debounce Config NV item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG);
  buffer[index++] = '\n';

  /* Copy the uim_features_status NV item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_FEATURES_STATUS_LIST, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURES_STATUS_LIST);
  buffer[index++] = '\n';

  /* Copy the UIM_HW_CONFIG NV item file name*/
  (void)strlcpy((char*)&buffer[index],(void *)UIM_COMMON_EFS_PATH_UIMDRV_HW_CONFIG, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_HW_CONFIG);
  buffer[index++] = '\n';

    /* Copy the feature_support_hotswap NV item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_SUPPORT_HOTSWAP, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_SUPPORT_HOTSWAP);
  buffer[index++] = '\n';

  /* Copy the uim_current_active_slot NV item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_ACTIVE_SLOT, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_ACTIVE_SLOT);
  buffer[index++] = '\n';

  /* Copy the uim_disable_polling NV item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_DISABLE_POLLING, buf_len - index);
   index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_DISABLE_POLLING);
   buffer[index++] = '\n';

  /* Copy the uim_feature_busy_response EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_BUSY_RESPONSE, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_BUSY_RESPONSE);
  buffer[index++] = '\n';

  /* Copy the uim_feature_busy_response_simulate_null EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL);
  buffer[index++] = '\n';

#ifdef FEATURE_UIM_REMOTE_UIM
    /* Copy the uim_remote_command_response_timer EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_REMOTE_COMMAND_RESP_TIMER, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_REMOTE_COMMAND_RESP_TIMER);
  buffer[index++] = '\n';
#endif /* FEATURE_UIM_REMOTE_UIM */

  /* Copy the UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_EXTENDED_RECOVERY_FILE, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_EXTENDED_RECOVERY_FILE);
  buffer[index++] = '\n';

  /* Copy the UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE);
  buffer[index++] = '\n';

   /* Copy the uim_remote_command_response_timer EFS item file name*/
  (void)strlcpy((char*)&buffer[index], UIM_COMMON_EFS_PATH_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER, buf_len - index);
  index += strlen(UIM_COMMON_EFS_PATH_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER);
  buffer[index++] = '\n';

  buffer[index++] = 0x0;
  /* Create directory, if needed */
  if (UIM_COMMON_EFS_ERROR == uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR1, &file_size))
  {
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR1, 0777);
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR2, 0777);
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3, 0777);
  }
  else if (UIM_COMMON_EFS_ERROR == uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR2, &file_size))
  {
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR2, 0777);
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3, 0777);
  }
  else if (UIM_COMMON_EFS_ERROR == uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR3, &file_size))
  {
    (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3, 0777);
  }

  uim_compare_and_update_conf_file(UIM_COMMON_EFS_UIMDRV_CONF,
                                   buffer,
                                   (uint16) strlen(buffer),
                                   uim_ptr);

  UIM_FREE(buffer);
} /* uim_update_conf_file */


/**
 * Function used to write the default value of UIMDRV Features
 * (controlled by various NVITEMs such as
 * UIM_FEATURES_STATUS_LIST_FILE to the respective globals
 * controlling the nvitem.
 *
 * @param item_subfeature Sub feature item number.
 * @param uim_common_efs_item_type   efs_item,.
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_nv_feature_write_default_value(
  uint8                  item_subfeature,
  uim_common_efs_item_type   efs_item,
  uim_instance_global_type *uim_ptr  )
{
  switch(efs_item)
  {
    case UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST:
      switch((uim_nv_features_enum_type) item_subfeature)
      {
        case UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES:
          uim_ptr->setting.features_status_list.handle_no_atr_in_40000_clk_cycles = UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES_DEFAULT;
          break;
        case UIMDRV_FEATURE_LOG_TO_EFS:
          uim_ptr->setting.features_status_list.log_to_efs = UIMDRV_FEATURE_LOG_TO_EFS_DEFAULT;
          break;
        case UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL:
          uim_ptr->setting.features_status_list.disable_recovery_upon_infinite_null =  UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL_DEFAULT;
          break;
        case UIMDRV_FEATURE_DEBUG_LOG:
          uim_ptr->setting.features_status_list.debug_log = UIMDRV_FEATURE_DEBUG_LOG_DEFAULT;
          break;
        case UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL:
          uim_ptr->setting.features_status_list.rpt_bad_sw_on_poll = UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL_DEFAULT;
          break;
        case UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE:
          uim_ptr->setting.features_status_list.handle_iccid_read_failure = UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE_DEFAULT;
          break;
        case UIMDRV_FEATURE_SUPPORT_NO_ICCID:
          uim_ptr->setting.features_status_list.support_no_iccid = UIMDRV_FEATURE_SUPPORT_NO_ICCID_DEFAULT;
          break;
        case UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT:
          uim_ptr->setting.features_status_list.min_tpl_iccid_support= UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT_DEFAULT;
          break;
        case UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT:
          uim_ptr->setting.features_status_list.handle_unknown_proc_bytes_as_cmd_timeout = UIMDRV_FEATURE_HANDLE_UNKNOWN_PROC_BYTES_AS_CMD_TIMEOUT_DEFAULT;
          break;
        case UIMDRV_FEATURE_INTERFACE_NOT_USED:
          uim_ptr->setting.features_status_list.interface_not_used = UIMDRV_FEATURE_INTERFACE_NOT_USED_DEFAULT;
          break;
        case UIMDRV_FEATURE_LOG_APDU_TO_EFS:
          uim_ptr->setting.features_status_list.log_apdu_to_efs = UIMDRV_FEATURE_LOG_APDU_TO_EFS_DEFAULT;
          break;
        case UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY:
          uim_ptr->setting.features_status_list.no_switch_inst_on_wwt_expiry = UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY_DEFAULT;
          break;
        case UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH:
          uim_ptr->setting.features_status_list.send_status_before_auth = UIMDRV_FEATURE_SEND_STATUS_BEFORE_AUTH_DEFAULT;
          break;
        case UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD:
          uim_ptr->setting.features_status_list.try_default_baud_rate_for_f372_d12_card = UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD_DEFAULT;
          break;
        case UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH:
          uim_ptr->setting.features_status_list.cold_reset_due_to_card_switch = UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH_DEFAULT;
          break;
        case UIMDRV_FEATURE_SM_USE_SLOT_1:
          uim_ptr->setting.features_status_list.sm_prefer_slot1 = UIMDRV_FEATURE_SM_USE_SLOT_1_DEFAULT;
          break;
        case UIMDRV_FEATURE_USE_DUAL_LDO:
          uim_ptr->setting.features_status_list.use_dual_ldo = UIMDRV_FEATURE_USE_DUAL_LDO_DEFAULT;
          break;
        case UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY:
          uim_ptr->setting.features_status_list.uim_polling_only_at_polling_timer_expiry = UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY_DEFAULT;
          break;
        case UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ:
          uim_ptr->setting.features_status_list.uim_set_clk_freq_at_4_8_MHz = UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ_DEFAULT;
          break;
        case UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME:
          uim_ptr->setting.features_status_list.handle_tc1_byte_for_extra_guard_time = UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME_DEFAULT;
          break;
        case UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET:
          uim_ptr->setting.features_status_list.enable_sim_mode_change_via_warm_reset = UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET_DEFAULT;
          break;
        case UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF:
          uim_ptr->setting.features_status_list.enable_explicit_selection_of_mf_or_adf = UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF_DEFAULT;
          break;
        case UIMDRV_FEATURE_ENABLE_BOOT_UP_IN_INVERSE_CONVENTION:
          uim_ptr->setting.features_status_list.enable_boot_up_in_inverse_convention = UIMDRV_FEATURE_ENABLE_BOOT_UP_IN_INVERSE_CONVENTION_DEFAULT;
          break;
        case UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD:
	  uim_ptr->setting.features_status_list.recovery_on_bad_status_word = UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD_DEFAULT;
          break;
        case UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY:
          uim_ptr->setting.features_status_list.attempt_pup_3v_from_nth_recovery = UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY_DEFAULT;
          break;
        case UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU:
          uim_ptr->setting.features_status_list.enable_le_support_for_stream_7816_apdu = UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU_DEFAULT;
          break;
        case UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP:
          uim_ptr->setting.features_status_list.disable_card_status_check_at_power_up = UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP_DEFAULT;
          break;
        case UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED:
          uim_ptr->setting.features_status_list.simtray_with_gpios_tied = UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED_DEFAULT;
          break;
        case UIMDRV_FEATURE_DISABLE_PROACTIVE_POLLING:
          uim_ptr->setting.features_status_list.disable_proactive_polling = UIMDRV_FEATURE_DISABLE_PROACTIVE_POLLING_DEFAULT;
          break;
        default:
          UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid Feature 0x%x", item_subfeature);
      }
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP:
      switch((uim_nv_feature_support_hotswap_enum_type) item_subfeature)
      {
        case UIMDRV_FEATURE_SUPPORT_HOTSWAP:
          uim_ptr->hotswap.feature_support_hotswap.feature_support_hotswap = FALSE;
          break;
        default:
          UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid Feature 0x%x", item_subfeature);
      }
      break;
    default:
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Write attempted on invalid uim NVITEM ");
      break;
  }
} /* uim_nv_feature_write_default_value */


/**
 * Function used to write the default value of subfeatures
 * controlled by a nvitem that are delta between the two
 * versions of the nvitem to the respective globals.
 *
 * @param version_in_nv NV version in NV file itself.
 * @param version_in_build NV version in Build.
 * @param uim_common_efs_item_type   efs_item
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_nv_write_default_value_of_delta_features
(
  uint8                  version_in_nv,
  uint8                  version_in_build,
  uim_common_efs_item_type   efs_item,
  uim_instance_global_type *uim_ptr
)
{
  uint8 uimdrv_feature;
  uint8 num_features_in_nv;
  uint8 num_features_in_build;

  switch(efs_item)
  {
    case UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST:
      if(
           (version_in_nv >= sizeof(uimdrv_features_total_till_version)) ||
           (version_in_build >= sizeof(uimdrv_features_total_till_version))
        )
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Array out-of-bounds while writing default values for UIMDRV features");
        return;
      }

      num_features_in_nv    = uimdrv_features_total_till_version[version_in_nv];
      num_features_in_build = uimdrv_features_total_till_version[version_in_build];
      /* Since uimdrv_features_total_till_version array is indexed by the
         feature_status_list *version*, it's value at version version_in_nv is
         essentially the first feature of the delta list...no need to bail out if
         case writing fails... */
      for(uimdrv_feature = num_features_in_nv; uimdrv_feature < num_features_in_build; uimdrv_feature++)
      {
        uim_nv_feature_write_default_value(uimdrv_feature, efs_item, uim_ptr);
      }
      return;

    case UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP:
      if(
           (version_in_nv >=
            sizeof(uimdrv_feature_support_hotswap_total_till_version)) ||
           (version_in_build >=
            sizeof(uimdrv_feature_support_hotswap_total_till_version))
        )
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Array out-of-bounds while writing default value for HOTSWAP SUPPORT feature");
        return;
      }

      num_features_in_nv    = uimdrv_feature_support_hotswap_total_till_version[version_in_nv];
      num_features_in_build = uimdrv_feature_support_hotswap_total_till_version[version_in_build];
      for(uimdrv_feature = num_features_in_nv; uimdrv_feature < num_features_in_build; uimdrv_feature++)
      {
        uim_nv_feature_write_default_value(uimdrv_feature, efs_item, uim_ptr);
      }
      return;
    default:
      return;
  }
} /* uim_nv_write_default_value_of_delta_features */


/**
 * Function used to write the default values of all the
 * sub-features controlled by various uimdrv NVITEMs to their
 * respective globals.
 *
 * @param uim_common_efs_item_type   efs_item.
 * @param uim_ptr Pointer to the global data common
 */
static void uim_nv_write_default_value_of_all_features
(
  uim_common_efs_item_type   efs_item,
  uim_instance_global_type *uim_ptr
)
{
  uint8 uimdrv_feature;
  uint8 num_features_in_build;

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  switch(efs_item)
  {
    case UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST:
      num_features_in_build = uimdrv_features_total_till_version[UIM_FEATURES_STATUS_LIST_CURRENT_VERSION];

      memset(&uim_ptr->setting.features_status_list, 0x00, sizeof(uim_features_status_list_type));
      /* Update the Version in features_status_list to the current build version */
      uim_ptr->setting.features_status_list.version = UIM_FEATURES_STATUS_LIST_CURRENT_VERSION;

      /* Update uim_features_status_list with default values */
      for(uimdrv_feature = 0; uimdrv_feature < num_features_in_build;
          uimdrv_feature++)
      {
        uim_nv_feature_write_default_value(uimdrv_feature, efs_item, uim_ptr);
      }
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP:
      num_features_in_build = uimdrv_feature_support_hotswap_total_till_version[UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION];

      memset(&uim_ptr->hotswap.feature_support_hotswap, 0x00, sizeof(uim_ptr->hotswap.feature_support_hotswap));
      /* Update the Version in features_status_list to the current build version */
      uim_ptr->hotswap.feature_support_hotswap.version = UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION;

      /* Update uim_feature_support_hotswap with default values */
      for(uimdrv_feature = 0; uimdrv_feature < num_features_in_build; uimdrv_feature++)
      {
        uim_nv_feature_write_default_value(uimdrv_feature, efs_item, uim_ptr);
      }
      break;
    default:
      break;
  }
} /* uim_nv_write_default_value_of_all_features */



/**
 * Function used to validate the various NVITEM's versions that
 * support it.If what's in the NV is not the one currently
 * defined in the build, updates it to the current version
 * defined and writes it back to the NV.
 * Version gets updated in the NV if it doesn't match the one in
 * the build. If the version is updated, next time when the user
 * reads the NVITEM, a dropdown of items corresponding to the
 * updated version is shown instead of what the user saw the
 * first time the NV read was done (corresponding to the version
 * present in the NV at that point).
 *
 * @param uim_common_efs_item_type   efs_item.
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_nv_features_status_list_update_version
(
  uim_common_efs_item_type  efs_item,
  uim_instance_global_type *uim_ptr
)
{
  switch(efs_item)
  {
    case UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST:
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"UIMDRV Features Status NV ITEM version - in NV: 0x%x, in build: 0x%x",
                        uim_ptr->setting.features_status_list.version,
                        UIM_FEATURES_STATUS_LIST_CURRENT_VERSION);
      if ((int)uim_ptr->setting.features_status_list.version < UIM_FEATURES_STATUS_LIST_CURRENT_VERSION)
      {
        /* If the uim_features_status_list version in NV is less than current
           uim_features_status_list version in our code, meaning new features
           have been added since the version in NV till the current version, we
           write back the default value of all those delta features to the NV. */
        uim_nv_write_default_value_of_delta_features(
          uim_ptr->setting.features_status_list.version,
          UIM_FEATURES_STATUS_LIST_CURRENT_VERSION,
          efs_item,
          uim_ptr);

        uim_ptr->setting.features_status_list.version = UIM_FEATURES_STATUS_LIST_CURRENT_VERSION;
        if (UIM_COMMON_EFS_SUCCESS != uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,
                                                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                           UIM_COMMON_EFS_DEVICE,
                                                          (char *)&(uim_ptr->setting.features_status_list),
                                                           sizeof(uim_ptr->setting.features_status_list)))
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"UIMDRV Features Status NV ITEM version update failed!");
        }
      }
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP:
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"UIMDRV Hotswap Support NV ITEM version - in NV: 0x%x, in build: 0x%x",
                        uim_ptr->hotswap.feature_support_hotswap.version,
                        UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION);
      if ((int)uim_ptr->hotswap.feature_support_hotswap.version < UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION)
      {
        /* If the uim_feature_support_hotswap version in NV is less than current
           uim_feature_support_hotswap version in our code, meaning new features
           have been added since the version in NV till the current version, we
           write back the default value of all those delta features to the NV. */
        uim_nv_write_default_value_of_delta_features(
          uim_ptr->setting.features_status_list.version,
          UIM_FEATURES_STATUS_LIST_CURRENT_VERSION,
          efs_item,
          uim_ptr);

        uim_ptr->hotswap.feature_support_hotswap.version = UIM_FEATURE_SUPPORT_HOTSWAP_CURRENT_VERSION;
        if (UIM_COMMON_EFS_SUCCESS != uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP,
                                                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                           UIM_COMMON_EFS_DEVICE,
                                                          (char *)&(uim_ptr->hotswap.feature_support_hotswap),
                                                           sizeof(uim_ptr->hotswap.feature_support_hotswap)))
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"UIMDRV Hotswap Support NV ITEM version update failed!");
        }
      }
      break;
    default:
      break;
  }
} /* uim_nv_features_status_list_update_version */


/**
 * Set config params with default value or with values from NVs.
 *
 * @param out_uim_config_params Output data of config params
 * @param read_from_nv_uim_config_params Config params from NVs.
 * @param nv_read_status NV status.
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_validate_and_set_config_params(
  nv_uim_config_params_type        *out_uim_config_params,
  nv_uim_config_params_type const  *read_from_nv_uim_config_params,
  nv_stat_enum_type                 nv_read_status,
  uim_instance_global_type         *uim_ptr
)
{
  uint8 rfu_bytes_index = 0;
  if (nv_read_status != NV_DONE_S)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Nv Read UIM CONFIGURATION PARAMS failed - Setting defaults");
    /* Additional delay to receive ATR over and above 40000 clk cycles */
    out_uim_config_params->additional_delay_for_atr = 0;
    /* Number of times an external uim command shall be re-tried over and above the default 2 */
    out_uim_config_params->ext_cmd_additional_re_try_cnt = 0;
    /* Number of parity errors that would be tolerated over and above 20 */
    out_uim_config_params->additional_parity_err_cnt = 0;
    /* Number of rx_break errors that would be tolerated over and above 10 */
    out_uim_config_params->additional_rx_break_err_cnt = 0;
    /* Additional delay when direction of transmission changes from card to ME above 150 micro seconds */
    out_uim_config_params->additional_delay_when_dir_change = 0;
    /* Boolean to indicate whether or not a poll time out would be recovered if in a call */
    out_uim_config_params->uim_recover_poll_t_out_while_in_call = 1;
    /* Whether or not CPHS is read after internal reset if UICC */
    out_uim_config_params->read_usim_cphs = TRUE;
    /* Whether or not UST is read after internal reset if UICC */
    out_uim_config_params->read_usim_ust = TRUE;
    /* Whether or not EST is read after internal reset if UICC */
    out_uim_config_params->read_usim_est = TRUE;
    /* Whether or not NETPAR is read after internal reset if UICC */
    out_uim_config_params->read_usim_netpar = TRUE;
    /* Whether or not RPLMNACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_rplmnact = TRUE;
    /* Whether or not USIM_AD is read after internal reset if UICC */
    out_uim_config_params->read_usim_ad = TRUE;
    /* Whether or not IMSI is read after internal reset if UICC */
    out_uim_config_params->read_usim_imsi = TRUE;
    /* Whether or not USIM_ACC is read after internal reset if UICC */
    out_uim_config_params->read_usim_acc = FALSE;
    /* Whether or not USIM_HPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmn = FALSE;
    /* Whether or not HPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmnwact = FALSE;
    /* Whether or not PLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_plmnwact = FALSE;
    /* Whether or not OPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_oplmnwact = FALSE;
    /* Whether or not LOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_loci = FALSE;
    /* Whether or not PSLOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_psloci = FALSE;
    /* Whether or not KEYS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keys = FALSE;
    /* Whether or not KEYSPS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keysps = FALSE;
    /* Whether or not FPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_fplmn = FALSE;
    /* Whether or not START_HFN is read after internal reset if UICC */
    out_uim_config_params->read_usim_start_hfn = FALSE;
    /* Whether or not THRESHOLD is read after internal reset if UICC */
    out_uim_config_params->read_usim_threshold = FALSE;
    /* Whether or not CBMID is read after internal reset if UICC */
    out_uim_config_params->read_usim_cbmid = FALSE;
    /* Whether or not KC is read after internal reset if UICC */
    out_uim_config_params->read_usim_kc = FALSE;
    /* Whether or not KCGPRS is read after internal reset if UICC */
    out_uim_config_params->read_usim_kcgprs = FALSE;
    /* Whether or not ACM is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm = FALSE;
    /* Whether or not ACM_MAX is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm_max = FALSE;
    /* Whether or not EHPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_ehplmn = FALSE;


    /* Whether or not CPHS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cphs = TRUE;
    /* Whether or not SST is read after internal reset if GSM */
    out_uim_config_params->read_gsm_sst = TRUE;
    /* Whether or not HPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmn = TRUE;
    /* Whether or not AD is read after internal reset if GSM */
    out_uim_config_params->read_gsm_ad = TRUE;
    /* Whether or not IMSI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_imsi = TRUE;
    /* Whether or not ACC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acc = FALSE;
    /* Whether or not LOCI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_loci = FALSE;
    /* Whether or not KC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kc = FALSE;
    /* Whether or not FPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_fplmn = FALSE;
    /* Whether or not PLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmn = FALSE;
    /* Whether or not HPLMNACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmnact = FALSE;
    /* Whether or not PLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmnwact = FALSE;
    /* Whether or not OPLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_oplmnwact = FALSE;
    /* Whether or not LOCIGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_locigprs = FALSE;
    /* Whether or not KCGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kcgprs = FALSE;
    /* Whether or not CBMID is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cbmid = FALSE;
    /* Whether or not ACM is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm = FALSE;
    /* Whether or not ACM_MAX is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm_max = FALSE;
    /* Configuration parameters to add extra wwt, bwt, cwt, guard time,
       delay before and after clock stop, extra Vreg voltage and few
       flags to control power up delay, switching instruction class,
       setting status length, selecting GSM DF */
    for( rfu_bytes_index = 0; rfu_bytes_index < UIM_MAX_CONFIG_PARAMS_RFU_BYTES; rfu_bytes_index++)
    {
      out_uim_config_params->future_use[rfu_bytes_index] = 0;
    }
    out_uim_config_params->future_use[UIM_RFU_INDEX_DELAY_BEFORE_CLK_STOP]   = UIM_DELAY_BEFORE_CLOCK_STOP;
    out_uim_config_params->future_use[UIM_RFU_INDEX_DELAY_AFTER_CLK_RESTART] = UIM_DELAY_AFTER_CLOCK_RESTART;
  }
  else
  {
    /* Additional delay to receive ATR over and above 40000 clk cycles
     * Could go to the max limit of 65536 additional clock cycles
     */
    out_uim_config_params->additional_delay_for_atr =
      read_from_nv_uim_config_params->additional_delay_for_atr;
    /* Number of times an external uim command shall be re-tried over and above the default 2
     * let us limit it to a maximum of 10
     */
    out_uim_config_params->ext_cmd_additional_re_try_cnt =
      ((read_from_nv_uim_config_params->ext_cmd_additional_re_try_cnt > 10)
      ? 10 : read_from_nv_uim_config_params->ext_cmd_additional_re_try_cnt);

    /* Number of parity errors that would be tolerated over and above 20
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_parity_err_cnt =
      read_from_nv_uim_config_params->additional_parity_err_cnt;

    /* Number of rx_break errors that would be tolerated over and above 10
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_rx_break_err_cnt =
      read_from_nv_uim_config_params->additional_rx_break_err_cnt;

    /* Additional delay when direction of transmission changes from card to ME above 150 micro seconds
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_delay_when_dir_change =
      read_from_nv_uim_config_params->additional_delay_when_dir_change;
    /* Add this to the var which uses the delay */
    uim_ptr->setting.response_byte_delay +=
      out_uim_config_params->additional_delay_when_dir_change;

    /* Boolean to indicate whether or not a poll time out would be recovered if in a call */
    out_uim_config_params->uim_recover_poll_t_out_while_in_call =
      read_from_nv_uim_config_params->uim_recover_poll_t_out_while_in_call;


    /* Whether or not CPHS is read after internal reset if UICC */
    out_uim_config_params->read_usim_cphs =
      read_from_nv_uim_config_params->read_usim_cphs;
    /* Whether or not UST is read after internal reset if UICC */
    out_uim_config_params->read_usim_ust =
      read_from_nv_uim_config_params->read_usim_ust;
    /* Whether or not EST is read after internal reset if UICC */
    out_uim_config_params->read_usim_est =
      read_from_nv_uim_config_params->read_usim_est;
    /* Whether or not NETPAR is read after internal reset if UICC */
    out_uim_config_params->read_usim_netpar =
      read_from_nv_uim_config_params->read_usim_netpar;
    /* Whether or not RPLMNACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_rplmnact =
      read_from_nv_uim_config_params->read_usim_rplmnact;
    /* Whether or not USIM_AD is read after internal reset if UICC */
    out_uim_config_params->read_usim_ad =
      read_from_nv_uim_config_params->read_usim_ad;
    /* Whether or not IMSI is read after internal reset if UICC */
    out_uim_config_params->read_usim_imsi =
      read_from_nv_uim_config_params->read_usim_imsi;
    /* Whether or not USIM_ACC is read after internal reset if UICC */
    out_uim_config_params->read_usim_acc =
      read_from_nv_uim_config_params->read_usim_acc;
    /* Whether or not USIM_HPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmn =
      read_from_nv_uim_config_params->read_usim_hplmn;
    /* Whether or not HPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmnwact =
      read_from_nv_uim_config_params->read_usim_hplmnwact;
    /* Whether or not PLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_plmnwact =
      read_from_nv_uim_config_params->read_usim_plmnwact;
    /* Whether or not OPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_oplmnwact =
      read_from_nv_uim_config_params->read_usim_oplmnwact;
    /* Whether or not LOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_loci =
      read_from_nv_uim_config_params->read_usim_loci;
    /* Whether or not PSLOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_psloci =
      read_from_nv_uim_config_params->read_usim_psloci;
    /* Whether or not KEYS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keys =
      read_from_nv_uim_config_params->read_usim_keys;
    /* Whether or not KEYSPS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keysps =
      read_from_nv_uim_config_params->read_usim_keysps;
    /* Whether or not FPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_fplmn =
      read_from_nv_uim_config_params->read_usim_fplmn;
    /* Whether or not START_HFN is read after internal reset if UICC */
    out_uim_config_params->read_usim_start_hfn =
      read_from_nv_uim_config_params->read_usim_start_hfn;
    /* Whether or not THRESHOLD is read after internal reset if UICC */
    out_uim_config_params->read_usim_threshold =
      read_from_nv_uim_config_params->read_usim_threshold;
    /* Whether or not CBMID is read after internal reset if UICC */
    out_uim_config_params->read_usim_cbmid =
      read_from_nv_uim_config_params->read_usim_cbmid;
    /* Whether or not KC is read after internal reset if UICC */
    out_uim_config_params->read_usim_kc =
      read_from_nv_uim_config_params->read_usim_kc;
    /* Whether or not KCGPRS is read after internal reset if UICC */
    out_uim_config_params->read_usim_kcgprs =
      read_from_nv_uim_config_params->read_usim_kcgprs;
    /* Whether or not ACM is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm =
      read_from_nv_uim_config_params->read_usim_acm;
    /* Whether or not ACM_MAX is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm_max =
      read_from_nv_uim_config_params->read_usim_acm_max;
    /* Whether or not EHPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_ehplmn =
      read_from_nv_uim_config_params->read_usim_ehplmn;


    /* Whether or not CPHS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cphs =
      read_from_nv_uim_config_params->read_gsm_cphs;
    /* Whether or not SST is read after internal reset if GSM */
    out_uim_config_params->read_gsm_sst =
      read_from_nv_uim_config_params->read_gsm_sst;
    /* Whether or not HPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmn =
      read_from_nv_uim_config_params->read_gsm_hplmn;
    /* Whether or not AD is read after internal reset if GSM */
    out_uim_config_params->read_gsm_ad =
      read_from_nv_uim_config_params->read_gsm_ad;
    /* Whether or not IMSI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_imsi =
      read_from_nv_uim_config_params->read_gsm_imsi;
    /* Whether or not ACC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acc =
      read_from_nv_uim_config_params->read_gsm_acc;
    /* Whether or not LOCI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_loci =
      read_from_nv_uim_config_params->read_gsm_loci;
    /* Whether or not KC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kc =
      read_from_nv_uim_config_params->read_gsm_kc;
    /* Whether or not FPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_fplmn =
      read_from_nv_uim_config_params->read_gsm_fplmn;
    /* Whether or not PLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmn =
      read_from_nv_uim_config_params->read_gsm_plmn;
    /* Whether or not HPLMNACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmnact =
      read_from_nv_uim_config_params->read_gsm_hplmnact;
    /* Whether or not PLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmnwact =
      read_from_nv_uim_config_params->read_gsm_plmnwact;
    /* Whether or not OPLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_oplmnwact =
      read_from_nv_uim_config_params->read_gsm_oplmnwact;
    /* Whether or not LOCIGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_locigprs =
      read_from_nv_uim_config_params->read_gsm_locigprs;
    /* Whether or not KCGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kcgprs =
      read_from_nv_uim_config_params->read_gsm_kcgprs;
    /* Whether or not CBMID is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cbmid =
      read_from_nv_uim_config_params->read_gsm_cbmid;
    /* Whether or not ACM is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm =
      read_from_nv_uim_config_params->read_gsm_acm;
    /* Whether or not ACM_MAX is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm_max =
      read_from_nv_uim_config_params->read_gsm_acm_max;
    /* Configuration parameters to add extra wwt, bwt, cwt, guard time,
       delay before and after clock stop, extra Vreg voltage and few
       flags to control power up delay, switching instruction class,
       setting status length, selecting GSM DF */
    for( rfu_bytes_index = 0; rfu_bytes_index < UIM_MAX_CONFIG_PARAMS_RFU_BYTES; rfu_bytes_index++)
    {
      out_uim_config_params->future_use[rfu_bytes_index] = read_from_nv_uim_config_params->future_use[rfu_bytes_index];
    }
  }
} /* uim_validate_and_set_config_params */


/**
 * UIMDRV MCFG Registration
 * @param uim_ptr Pointer to the global data common.
 * @return bool Success or Fail
 */
boolean uimdrv_mcfg_refresh_register
(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_refresh_registration_s_type mcfg_reg  = {0};

  if(NULL == uim_ptr)
  {
    UIM_MSG_ERR_0("uim_ptr is NULL");
    return FALSE;
  }

  if (uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    UIM_MSG_ERR_1("Invalid input param instance id 0x%x", uim_ptr->id);
    return FALSE;
  }

  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Slot is disabled */
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM slot disabled via NV do not register for MCFG NV refresh");
    return FALSE;
  }

  mcfg_reg.cb        = uimdrv_handle_mcgf_nv_refresh_cb;
  mcfg_reg.group     = 0; /* RFU - defaulting it to 0*/
  mcfg_reg.order     = MCFG_REFRESH_ORDER_50;
  /* each UIM task is responsible to register its own interested signal */
  mcfg_reg.slot_mask = 1 << (uim_ptr->id);
  mcfg_reg.sub_mask  = MCFG_REFRESH_INDEX_MASK_NONE;

  return mcfg_refresh_register(&mcfg_reg);
#else /* FEATURE_MODEM_CONFIG_REFRESH */
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Not register with MCFG for NV refresh due to FEATURE_MODEM_CONFIG_REFRESH disabled");
  return TRUE;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}/* uimdrv_mcfg_refresh_register */


