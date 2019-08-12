/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/src/uimdiag.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
05/22/16   vdc   Remove F3 messages for memory allocation failure
07/01/15   bcho  Support for whitelist and blacklist codes in same category
07/10/14   tl    Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/12/14   vv    Remove FEATURE_SIMLOCK
03/18/14   tl    Introduce new SIM Lock feature
01/27/14   df    Featurise file with FEATURE_UIMDIAG
01/10/14   df    Use v2.0 diag macros
12/11/13   df    Remove unused memory paging feature
11/05/13   df    Remove unnecessary include tmc.h
05/06/13   sw    Changes made for secure version of memcpy
04/24/13   rr    Featurized optional logs
11/09/12   yt    Remove parsing of length field for PERSO_GET_DATA_CMD
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
11/17/11   kk    Added get all available apps async support
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/04/11   adp   Adding support for is service available sync cmd
09/29/11   kk    Added support for get all available apps sync cmd
10/14/11   bcho  Added support for Get Operator Name API
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
08/05/11   adp   Adding support to trigger refresh in uimdiag
09/15/11   nb    Support for new refresh stage
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
04/26/11   yt    Replaced references to TMC heap with Modem heap
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
12/22/10   yt    Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/30/10   yt    Including private UIMDRV header
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
06/09/10   adp   Fixing Compiler Warnings
04/07/10   yt    Fixed compilation warnings
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,get_cphs_info
                 isim_authenticate,send_card_status,
                 create_pkcs15_lookup_table
03/24/10   adp   Fixing Compiler Warnings, the function call to check whether
                 the pointer is in the queue or not is not needed for uimdiag
03/19/10   rk    Added support for JCDMA get info
02/22/10   adp   Adding support for Perso Commands
02/04/10   yt    Fixed compilation warnings
12/27/09   ap    Adding support for Refresh Commands
12/17/09   rk    Fix for uimdiag small heap memory alloction issue
12/03/09   rk    Adding support for get file attributes, read and write commands
10/19/09   adp   Adding support for pin commands
10/18/09   adp   Moving MMGSDI specific util functions to uimdiag_mmgsdi_util.c
                 Adding support for Activate_or_switch_prov, session_get_info
                 Adding functionality for uimdiag_free
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#ifdef FEATURE_UIMDIAG
#include "diagcmd.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_simlock.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"
#include "uimdiag_util.h"
#include "uim_p.h"
#include "modem_mem.h"

/*=============================================================================

                       GLOBAL DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   UIMDIAG_SUBSYS_DELAYED_TBL

   DESCRITPION:
     Table contains uimdiag subsystem command ids and corresponding
     call back functions. This table is used during uimdiag initialization
     for registering

   ------------------------------------------------------------------------- */
static const diagpkt_user_table_entry_type uimdiag_subsys_delayed_tbl[] =
{
  {(word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD,
   (word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD,
   uimdiag_mmgsdi_client_id_and_evt_reg_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_OPEN_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_OPEN_CMD,
   uimdiag_mmgsdi_session_open_cmd },
  {(word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD,
   (word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD,
   uimdiag_mmgsdi_client_id_and_evt_dereg_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_CLOSE_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_CLOSE_CMD,
   uimdiag_mmgsdi_session_close_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD,
   uimdiag_mmgsdi_session_act_or_sw_prov_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD,
   uimdiag_mmgsdi_session_get_info_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD,
   uimdiag_mmgsdi_session_get_all_pin_status_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD,
   uimdiag_mmgsdi_session_verify_pin_cmd },
  {(word) UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD,
   uimdiag_mmgsdi_session_disable_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD,
   uimdiag_mmgsdi_session_enable_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD,
   uimdiag_mmgsdi_session_change_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD,
   uimdiag_mmgsdi_session_unblock_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD,
   uimdiag_mmgsdi_session_get_file_attr_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD,
   uimdiag_mmgsdi_session_read_record_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD,
   uimdiag_mmgsdi_session_read_transparent_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD,
   uimdiag_mmgsdi_session_write_record_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD,
   uimdiag_mmgsdi_session_write_transparent_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD ,
    (word)UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD,
     uimdiag_mmgsdi_session_reg_for_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD,
   uimdiag_mmgsdi_session_dereg_for_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD,
   uimdiag_mmgsdi_session_ok_to_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD,
   uimdiag_mmgsdi_session_refresh_complete_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD,
   uimdiag_mmgsdi_session_perso_activate_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD,
   uimdiag_mmgsdi_session_perso_deactivate_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD,
   uimdiag_mmgsdi_session_perso_get_ind_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD,
   uimdiag_mmgsdi_session_perso_set_data_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD,
   uimdiag_mmgsdi_session_perso_get_data_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD,
   uimdiag_mmgsdi_session_perso_get_key_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD,
   uimdiag_mmgsdi_session_perso_unblock_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD,
   uimdiag_mmgsdi_session_perso_get_dck_retries_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD,
   uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD,
   uimdiag_mmgsdi_session_jcdma_get_card_info_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD,
   uimdiag_mmgsdi_session_deact_prov_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD,
   uimdiag_mmgsdi_session_get_app_cap_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD,
   uimdiag_mmgsdi_session_get_cphs_info_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD,
   uimdiag_mmgsdi_session_isim_auth_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD,
   uimdiag_mmgsdi_session_send_card_status_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD,
   uimdiag_mmgsdi_session_disable_service_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD,
   uimdiag_mmgsdi_session_enable_service_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD,
   uimdiag_mmgsdi_session_is_service_avail_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SEEK_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEEK_CMD,
   uimdiag_mmgsdi_session_seek_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SEARCH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEARCH_CMD,
   uimdiag_mmgsdi_session_search_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD,
   uimdiag_mmgsdi_session_prov_app_init_complete_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD,
   uimdiag_mmgsdi_session_compute_ip_auth_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD,
   uimdiag_mmgsdi_session_ssd_update_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD,
   uimdiag_mmgsdi_session_confirm_ssd_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD,
   uimdiag_mmgsdi_session_bs_chal_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD,
   uimdiag_mmgsdi_session_read_prl_cmd},
   {(word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD ,
   (word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
   uimdiag_check_diag_support_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD,
   uimdiag_mmgsdi_session_get_info_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD,
   uimdiag_mmgsdi_session_read_cache_file_size_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD,
   uimdiag_mmgsdi_session_read_cache_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD,
   uimdiag_mmgsdi_session_write_cache_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD,
   uimdiag_mmgsdi_session_illegal_subscription_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD,
   uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD,
   uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_GET_ATR_CMD,
   (word)UIMDIAG_MMGSDI_SAP_GET_ATR_CMD,
   uimdiag_mmgsdi_sap_get_atr_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD,
   (word)UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD,
   uimdiag_mmgsdi_sap_send_apdu_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_POWER_ON_CMD,
   (word)UIMDIAG_MMGSDI_SAP_POWER_ON_CMD,
   uimdiag_mmgsdi_sap_power_on_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD,
   (word)UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD,
   uimdiag_mmgsdi_sap_power_off_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD,
   (word)UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD,
   uimdiag_mmgsdi_sap_card_reader_status_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_RESET_CMD,
   (word)UIMDIAG_MMGSDI_SAP_RESET_CMD,
   uimdiag_mmgsdi_sap_reset_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_CONNECT_CMD,
   (word)UIMDIAG_MMGSDI_SAP_CONNECT_CMD,
   uimdiag_mmgsdi_sap_connect_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD,
   (word)UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD,
   uimdiag_mmgsdi_sap_disconnect_cmd},
   {(word)UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD,
   (word)UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD,
   uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD,
   uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd},
   {(word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   (word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   uimdiag_mmgsdi_naa_refresh_cmd},
   {(word)UIMDIAG_MMGSDI_CARD_PDOWN_CMD,
   (word)UIMDIAG_MMGSDI_CARD_PDOWN_CMD,
   uimdiag_mmgsdi_card_pdown_cmd},
   {(word)UIMDIAG_MMGSDI_CARD_PUP_CMD,
   (word)UIMDIAG_MMGSDI_CARD_PUP_CMD,
   uimdiag_mmgsdi_card_pup_cmd},
   {(word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD,
   (word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD,
   uimdiag_mmgsdi_activate_onchip_sim_cmd},
   {(word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD,
   (word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD,
   uimdiag_mmgsdi_activate_onchip_usim_cmd},
   {(word)UIMDIAG_MMGSDI_SEND_APDU_CMD,
   (word)UIMDIAG_MMGSDI_SEND_APDU_CMD,
   uimdiag_mmgsdi_send_apdu_cmd},
   {(word)UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD,
   (word)UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD,
   uimdiag_mmgsdi_get_all_available_apps_async_cmd},
   {(word)UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD,
   uimdiag_mmgsdi_get_all_available_apps_sync_cmd},
   {(word)UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
   (word)UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
   uimdiag_mmgsdi_is_service_available_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD,
   uimdiag_mmgsdi_session_get_operator_name_table_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD,
    (word)UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD,
    uimdiag_mmgsdi_session_open_ext_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
   uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd},
   {(word)UIMDIAG_SIMLOCK_SET_POLICY_CMD,
   (word)UIMDIAG_SIMLOCK_SET_POLICY_CMD,
   uimdiag_simlock_set_policy_cmd },
   {(word)UIMDIAG_SIMLOCK_CK_LOCK_CMD,
   (word)UIMDIAG_SIMLOCK_CK_LOCK_CMD,
   uimdiag_simlock_ck_lock_cmd },
   {(word)UIMDIAG_SIMLOCK_HCK_LOCK_CMD,
   (word)UIMDIAG_SIMLOCK_HCK_LOCK_CMD,
   uimdiag_simlock_hck_lock_cmd },
   {(word)UIMDIAG_SIMLOCK_UNLOCK_CMD,
   (word)UIMDIAG_SIMLOCK_UNLOCK_CMD,
   uimdiag_simlock_unlock_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_STATUS_CMD,
   (word)UIMDIAG_SIMLOCK_GET_STATUS_CMD,
   uimdiag_simlock_get_status_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_CMD,
   (word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_CMD,
   uimdiag_simlock_get_category_data_cmd },
   {(word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_CK_CMD,
   (word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_CK_CMD,
   uimdiag_simlock_add_lock_codes_ck_cmd },
   {(word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_HCK_CMD,
   (word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_HCK_CMD,
   uimdiag_simlock_add_lock_codes_hck_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_EXT_CMD,
   (word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_EXT_CMD,
   uimdiag_simlock_get_category_data_ext_cmd }
};

uimdiag_req_table_elem_type *uimdiag_req_table_ptr = NULL;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION UIMDIAG_REQ_TABLE_INIT

DESCRIPTION
    Initializes the req_table to NULL.

ARGUMENTS
    None

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void uimdiag_req_table_init(void)
{
  /* Initialize table. Set it to NULL */
  uint32 max_client_req_tbl_size          = UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uimdiag_req_table_ptr,
    (sizeof(uimdiag_req_table_elem_type) * max_client_req_tbl_size));
}/* uimdiag_req_table_init */


/*===========================================================================

FUNCTION UIMDIAG_INIT

DESCRIPTION
  The uimdiag init function initializes sub system commands to DIAG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void uimdiag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(
    DIAG_SUBSYS_CMD_VER_2_F,
    (uint8)DIAG_SUBSYS_UIM,
    uimdiag_subsys_delayed_tbl);
    uimdiag_req_table_init();
}/* uimdiag_init */


/*===========================================================================

FUNCTION UIMDIAG_MALLOC

DESCRIPTION
  The uimdiag_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uimdiag_malloc(dword size)
{
  return modem_mem_calloc(1,(size),MODEM_MEM_CLIENT_UIM);
}/* uimdiag_malloc */


/*===========================================================================
FUNCTION: UIMDIAG_PARSE_CMD_DATA

DESCRIPTION:
  Populates the request structure with data from the Diag packet.

INPUTS:
  *req_ptr:       Generic_command struct request pointer
                  which will be populated with data from the diag_req_ptr
  *diag_req_ptr:  Pointer to raw Diag request packet
  diag_req_len:   Length of the diag request packet

DEPENDENCIES:
  None

RETURN VALUE:
  uimdiag_return_enum_type

OUTPUT:
  None
===========================================================================*/
uimdiag_return_enum_type uimdiag_parse_cmd_data (
  uimdiag_generic_req_type  *req_ptr,
  PACKED void               *diag_req_ptr,
  uint32                     diag_req_len
)
{
  uimdiag_subsystem_cmd_enum_type uimdiag_subsys_cmd = UIMDIAG_MAX_CMD;
  uint32                          offset             = 0;

  UIMDIAG_MSG_HIGH_0("uimdiag_parse_cmd_data");

  if ((req_ptr == NULL) ||
      (diag_req_ptr == NULL) ||
      (diag_req_len < sizeof(uimdiag_cmd_header_type)))
  {
    UIMDIAG_MSG_ERROR_0("req_ptr or diag_req_ptr is NULL or diag_req_len to small:");
    return UIMDIAG_ERROR;
  }

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. COMMAND CODE
  2. SUBSYSTEM ID
  3. SUBSYSTEM CMD CODE
  4. COMMAND Specific params
  ----------------------------------------------------- */

  /* command code */
  (void)memscpy( ((uint8*)&(req_ptr->cmd_header.command_code)),
                 sizeof(req_ptr->cmd_header.command_code),
                 (uint8*)diag_req_ptr, sizeof(uint8));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(uint8);
  offset       +=  sizeof(uint8);
  UIMDIAG_ASSERT((offset < diag_req_len), UIMDIAG_ERROR);

  /* subsystem id */
  (void)memscpy(((uint8*)&(req_ptr->cmd_header.subsys_id)),
                sizeof(req_ptr->cmd_header.subsys_id),
                (uint8*)diag_req_ptr, sizeof(diagpkt_subsys_id_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_id_type);
  offset       +=  sizeof(diagpkt_subsys_id_type);
  UIMDIAG_ASSERT((offset < diag_req_len), UIMDIAG_ERROR);

  /* subsystem command code */
  (void) memscpy( ((void*)&(req_ptr->cmd_header.subsys_cmd_code)),
                  sizeof(req_ptr->cmd_header.subsys_cmd_code),
                  (void*)diag_req_ptr,sizeof(diagpkt_subsys_cmd_code_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_cmd_code_type);
  offset       +=  sizeof(diagpkt_subsys_cmd_code_type);
  UIMDIAG_ASSERT((offset <= diag_req_len), UIMDIAG_ERROR);

  uimdiag_subsys_cmd = (uimdiag_subsystem_cmd_enum_type)req_ptr->
                                                cmd_header.subsys_cmd_code;
  switch(uimdiag_subsys_cmd)
  {
      case UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD:
        return UIMDIAG_SUCCESS;

      case UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD:
        return uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd(
                       &req_ptr->request.client_id_and_evt_dereg,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OPEN_CMD:
        return uimdiag_mmgsdi_util_populate_session_open_cmd(
                       &req_ptr->request.session_open,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_CLOSE_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                       &req_ptr->request.session_close,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD:
        return uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd(
                     &req_ptr->request.session_act_or_sw_prov,
                     (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_info_cmd(
                     &req_ptr->request.session_get_info,
                     (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_all_pin_stat,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_verify_pin_cmd(
                 &req_ptr->request.session_verify_pin,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_disable_pin_cmd(
                 &req_ptr->request.session_disable_pin,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_enable_pin_cmd(
                &req_ptr->request.session_enable_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_change_pin_cmd(
                &req_ptr->request.session_change_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd(
                &req_ptr->request.session_unblock_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd(
                &req_ptr->request.session_get_file_attr,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD:
        return uimdiag_mmgsdi_util_populate_session_read_record_cmd(
                &req_ptr->request.session_read_record,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD:
        return uimdiag_mmgsdi_util_populate_session_read_transparent_cmd(
                &req_ptr->request.session_read_transparent,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD:
        return uimdiag_mmgsdi_util_populate_session_write_record_cmd(
                &req_ptr->request.session_write_record,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD:
        return uimdiag_mmgsdi_util_populate_session_write_transparent_cmd(
                &req_ptr->request.session_write_transparent,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD:
      case UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_cmd(
                 &req_ptr->request.session_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd(
                 &req_ptr->request.session_refresh_proceed_voting,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd(
                 &req_ptr->request.session_ok_to_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd(
                 &req_ptr->request.session_refresh_ok_to_proceed,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_NAA_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
                 &req_ptr->request.session_trigger_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd(
                 &req_ptr->request.session_refresh_complete,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD:
      case UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD:
      case UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD:
      case UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_cmd(
                &req_ptr->request.session_perso,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_get_data_cmd(
                &req_ptr->request.session_perso,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_get_ind_cmd(
                &req_ptr->request.session_perso_get_ind,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_get_key_cmd(
                &req_ptr->request.session_perso_get_key,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_get_dck_retries_cmd(
                 &req_ptr->request.session_perso_get_dck_retries,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD:
        return uimdiag_mmgsdi_util_populate_session_perso_lock_dwn_enable_ltd_access_cmd(
                 &req_ptr->request.session_perso_lock_dwn_enable_ltd_access,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_jcdma_get_card_info,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_deact_prov,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_app_cap,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_cphs_info,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD:
        return uimdiag_mmgsdi_util_populate_session_isim_auth_cmd(
                 &req_ptr->request.session_isim_auth,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_session_send_card_status_cmd(
                 &req_ptr->request.session_send_crd_status,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_disable_srv,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_enable_srv,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_is_srv_avail,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEEK_CMD:
        return uimdiag_mmgsdi_util_populate_session_seek_cmd(
                 &req_ptr->request.session_seek_req,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEARCH_CMD:
        return uimdiag_mmgsdi_util_populate_session_search_cmd(
                 &req_ptr->request.session_search_req,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD:
        return uimdiag_mmgsdi_util_populate_app_init_complete(
          &req_ptr->request.session_prov_app_init_complete,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD:
        return uimdiag_mmgsdi_util_populate_compute_ip_auth(
          &req_ptr->request.session_compute_ip_auth,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD:
        return uimdiag_mmgsdi_util_populate_ssd_update(
          &req_ptr->request.session_ssd_update,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD:
        return uimdiag_mmgsdi_util_populate_ssd_confirm(
          &req_ptr->request.session_cnf_ssd,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD:
        return uimdiag_mmgsdi_util_populate_bs_chal(
          &req_ptr->request.session_bs_chal,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_read_prl,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_get_info_sync(
          &req_ptr->request.session_get_info_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD:
        return uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd(
          &req_ptr->request.session_cache_file_size_type,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD:
        return uimdiag_mmgsdi_util_populate_read_cache_cmd(
          &req_ptr->request.session_read_cache,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD:
        return uimdiag_mmgsdi_util_populate_write_cache(
          &req_ptr->request.session_write_cache,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_illegal_suscr_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_pwr_ctrl_sync(
          &req_ptr->request.session_toggle_uim_pwr_ctrl,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_get_app_cap_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_GET_ATR_CMD:
        return uimdiag_mmgsdi_util_populate_sap_get_atr(
          &req_ptr->request.sap_get_atr,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD:
        return uimdiag_mmgsdi_util_populate_sap_send_apdu(
          &req_ptr->request.sap_send_apdu,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_POWER_ON_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_power_on,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_power_off,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_card_reader_status,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_RESET_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_reset,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_CARD_PDOWN_CMD:
        return uimdiag_mmgsdi_util_populate_card_pdown(
          &req_ptr->request.card_pdown,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_CARD_PUP_CMD:
        return uimdiag_mmgsdi_util_populate_card_pup(
          &req_ptr->request.card_pup,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD:
      case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD:
        return uimdiag_mmgsdi_util_populate_activate_onchip_sim(
          &req_ptr->request.activate_onchip_sim,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SEND_APDU_CMD:
        return uimdiag_mmgsdi_util_populate_send_apdu(
          &req_ptr->request.send_apdu,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD:
      case UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_get_all_available_app_cmd(
          &req_ptr->request.get_all_available_apps,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.is_service_available_sync,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_CONNECT_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_connect,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD:
        return uimdiag_mmgsdi_util_populate_sap_disconnect(
          &req_ptr->request.sap_disconnect,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd(
          &req_ptr->request.session_get_operator_name,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD:
         return uimdiag_mmgsdi_util_populate_session_open_ext_cmd(
                       &req_ptr->request.session_open_ext,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_nvruim_data_sync(
          &req_ptr->request.session_get_nvruim_data_sync,
          (void*)diag_req_ptr, diag_req_len);


      default:
        UIMDIAG_MSG_ERROR_1("Invalid subcmd 0x%x", uimdiag_subsys_cmd);
        return UIMDIAG_ERROR;
  }
} /* uimdiag_parse_cmd_data */


/*===========================================================================
FUNCTION UIMDIAG_GET_INDEX

DESCRIPTION
  Gets an available space from the req_table. This row is then marked as
  used. When it is no longer necessary,   the row should be freed by
  calling uimdiag_free_index.

DEPENDENCIES
  None

RETURN VALUE
  index of the request table

SIDE EFFECTS
  None
===========================================================================*/
int uimdiag_get_index(void)
{
  int i;

  if (uimdiag_req_table_ptr != NULL)
  {
    for (i = 0; i< UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE; i++)
    {
      if (uimdiag_req_table_ptr[i].use_flag == UIMDIAG_REQ_TABLE_INDEX_FREE)
      {
        /* found an empty row. assign this */
        uimdiag_req_table_ptr[i].use_flag = UIMDIAG_REQ_TABLE_INDEX_USED;
        return i;
      }
    }
  }
  return UIMDIAG_GET_FN_ERR;
}/* uimdiag_get_index */


/*===========================================================================
FUNCTION UIMDIAG_FREE_INDEX

DESCRIPTION:
  Frees up space on req_table that is no longer being used.

INPUTS:
  int index: index into the request table

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void uimdiag_free_index(int index)
{
   if ((uimdiag_req_table_ptr != NULL) &&
       (index >= 0) &&
       (index < UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE))
   {
     /* Initialize the row entry */
     (void)memset (&uimdiag_req_table_ptr[index],
                   0x00, sizeof(uimdiag_req_table_elem_type));
   }
   else
   {
     UIMDIAG_MSG_ERROR_0("uimdiag_free_index: uimdiag_req_table_ptr is NULL or index out of range");
   }
}/* uimdiag_free_index */


/*===========================================================================

FUNCTION UIMDIAG_FREE

DESCRIPTION
  The uimdiag_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_UIMDIAG_USE_SMALL_HEAP is defined
  and FEATURE_LOW_MEMORY_USAGE IS NOT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void uimdiag_free(void * ptr)
{
  if (ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("Attempt to free null ptr, return");
    return;
  }

  modem_mem_free((void*)(ptr),MODEM_MEM_CLIENT_UIM);
}/* uimdiag_free */
#endif /* FEATURE_UIMDIAG */


