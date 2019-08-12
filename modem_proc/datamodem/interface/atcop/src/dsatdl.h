#ifndef DSATDL_H
#define DSATDL_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

         D Y N A M I C   L O A D I N G   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defines and
  enumerated constants, and function prototypes required for the
  data services AT command processor dynamic loading.

  Copyright (c) 2014 - 2015 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatdl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk/sc   Initial revision.

===========================================================================*/

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsatcmif.h"
#include "dsatetsicmif.h"
#include "dsatetsime.h"
#include "dsatme.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_IS707_ASYNC
typedef dsat_result_enum_type (*dsat707i_unrec_func_ptr_f_type)
(
  byte *
);

typedef void (*dsat707i_send_config_f_type)
(
  const byte *
);
#endif /* FEATURE_DATA_IS707_ASYNC */

typedef boolean (*dsat707_is_status_f_type)
(
  byte *,
  dsat_result_enum_type
);
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_CDMA_SMS
typedef void (*dsat707smsa_send_unsol_mt_msg_f_type)
(
  sms_event_info_s_type *
);
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DATA_MUX
typedef void (*dsatact_cmux_handler_f_type)(void);
#endif /* FEATURE_DATA_MUX */

typedef dsat_dial_val_e_type (*dsatact_proc_dial_str_f_type)
(
  const byte *,
  byte *
);

typedef void (*dsatcmdp_init_f_type)(void);

typedef void (*dsatcmdp_deinit_f_type)(void);

typedef void (*dsatcmdp_process_cmd_line_f_type)
(
  dsat_mode_enum_type,
  byte *
);

typedef void (*dsatcmdpi_queue_cmd_results_f_type)
(
  dsm_item_type *
);

typedef void (*dsatcmdp_process_async_cmd_f_type)
(
  ds_cmd_type *,
  dsat_result_enum_type
);

typedef void (*dsatcmdp_process_async_signal_f_type)
(
  dsat_result_enum_type
);

typedef boolean (*dsatcmdp_block_indications_f_type)(void);

typedef void (*dsatcmdp_sio_is_busy_f_type)
(
  ds3g_siolib_port_e_type
);

typedef void (*dsatcmdp_sio_is_free_f_type)
(
  ds3g_siolib_port_e_type
);

typedef dsat_result_enum_type (*dsatcmdp_send_respose_to_te_ex_f_type)
(
  void *,
  size_t,
  dsat_rsp_enum_type
);

typedef void (*dsatcmdp_rmsm_done_handler_f_type)
(
  dsat_rmsm_cmd_type,
  dsat_rmsm_info_s_type
);

typedef void (*dsatcmdp_refresh_subs_based_nv_f_type)
( 
  sys_modem_as_id_e_type
);

typedef dsat_result_enum_type (*dsatcmif_cm_call_event_handler_f_type)
(
  ds_at_cm_call_event_type *,
  dsat_result_enum_type,
  sys_modem_as_id_e_type
);

typedef dsat_result_enum_type (*dsatcmif_cm_call_cmd_handler_f_type)
(
  ds_at_cm_call_cmd_type *,
  dsat_result_enum_type
);

typedef dsat_result_enum_type (*dsatcmif_cm_ss_event_handler_f_type)
(
  dsat_cm_msg_s_type*,
  dsat_result_enum_type,
  sys_modem_as_id_e_type,
  dsat_subs_mode_e_type
);

typedef dsat_result_enum_type (*dsatcmif_cm_ph_cmd_handler_f_type)
(
  ds_at_cm_ph_cmd_type *,
  dsat_result_enum_type
);

typedef dsat_result_enum_type (*dsatcmif_cm_ph_event_handler_f_type)
(
  ds_at_cm_ph_event_type *,
  dsat_result_enum_type,
  sys_modem_as_id_e_type
);

typedef dsat_result_enum_type (*dsatclient_deregister_fwd_cmd_f_type)
(
  int32
);

typedef void (*dsatclient_ext_send_response_f_type)
(
  boolean,
  int32,
  dsat_client_result_enum_type,
  dsat_fwd_resp_enum_type,
  dsm_item_type *,
  uint32
);

typedef void (*dsatclient_ext_send_urc_f_type)
(
  uint8,
  int32,
  dsm_item_type *
);

#ifdef FEATURE_DSAT_ETSI_MODE
typedef void (*dsatetsicall_send_cr_result_f_type)
(
  dsat_cr_service_e_type
);

typedef void (*dsatetsicall_send_cr_result_ex_f_type)
(
  dsat_cr_service_e_type,
  ds3g_siolib_port_e_type
);

typedef void (*dsatetsicall_report_creg_result_f_type)
(
  dsat_num_item_type,
  sys_modem_as_id_e_type,
  dsat_creg_report_e_type
);

typedef void (*etsicall_report_aoc_ccm_f_type)
(
  dsm_item_type *
);

typedef dsat_result_enum_type (*etsicmif_report_ussd_result_f_type)
(
  const ds_at_cm_sups_event_type *,
  const cusd_result_e_type,
  sys_modem_as_id_e_type,
  boolean *
);

typedef dsat_result_enum_type (*etsicmif_send_sups_error_f_type)
(
  dsat_cme_error_e_type,
  sys_modem_as_id_e_type
);

typedef dsat_result_enum_type (*dsatetsicmif_cm_sups_event_handler_f_type)
(
  ds_at_cm_sups_event_type *,
  dsat_result_enum_type,
  sys_modem_as_id_e_type 
);

typedef dsat_result_enum_type (*dsatetsicmif_cm_sups_cmd_handler_f_type)
(
  ds_at_cm_sups_cmd_type *,
  dsat_result_enum_type,
  sys_modem_as_id_e_type
);

typedef void (*dsatetsime_report_ciev_result_f_type)
(
  uint8,
  dsat_ind_val_type,
  sys_modem_as_id_e_type
);

typedef dsat_result_enum_type (*dsat_pin_mmgsdi_event_handler_f_type)
(
  ds_at_mmgsdi_event_type *
);
#endif /* FEATURE_DSAT_ETSI_MODE */

typedef dsat_result_enum_type (*dsatme_cmd_status_handler_f_type)
(
  ds_at_cmd_status_type *
);

#ifdef FEATURE_MMGSDI
typedef dsat_result_enum_type (*dsatme_mmgsdi_sim_event_handler_f_type)
(
  ds_at_mmgsdi_sim_info_type *
);
#endif /* FEATURE_MMGSDI */

typedef void (*dsatme_report_pacsp_result_f_type) 
( 
  uint8
);

typedef void (*dsatprepi_set_preprocessor_mode_f_type)
(
  dsati_preproc_mode_enum_type,
  ds3g_siolib_port_e_type
);

typedef boolean (*dsatprep_process_sio_command_f_type)
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

typedef void (*dsatrsp_send_intermediate_result_code_f_type)
(
  dsat_result_enum_type
);

typedef void (*dsatrsp_send_result_code_f_type)
(
  dsat_result_enum_type
);

typedef void (*dsatrsp_send_result_code_ex_f_type)
(
  dsat_result_enum_type,
  ds3g_siolib_port_e_type
);

#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) )
typedef void (*dsatrsp_send_dr_result_ex_f_type)
(
  dsat_dr_mode_e_type,
  ds3g_siolib_port_e_type
);
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

typedef void (*dsatrsp_report_rate_on_connect_f_type)
(
  dsat_connect_rate_s_type
);

#ifdef FEATURE_DSAT_MDM_FUSION
typedef void (*dsatrsp_process_proxy_response_f_type)
(
  dsm_watermark_type *
);

typedef void (*dsatrsp_process_proxy_urc_f_type)
(
  dsm_watermark_type *
);
#endif /* FEATURE_DSAT_MDM_FUSION */

typedef void (*dsatrsp_send_urc_f_type)
(
  dsat_result_enum_type
);

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
typedef void (*dsatsms_flush_sms_indications_f_type)(void);

typedef void (*dsatsmsi_cmd_err_cb_func_f_type)
(
  wms_cmd_id_e_type,
  wms_cmd_err_e_type
);

typedef void (*dsatsmsu_send_async_indication_to_te_f_type)
(
  sms_mt_msg_e_type,
  wms_message_index_type,
  wms_memory_store_e_type,
  dsati_operating_cmd_mode_type,
  sys_modem_as_id_e_type
);
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_ETSI_SMS_CB
typedef void (*send_cb_msg_to_te_f_type)
(
  const wms_raw_ts_data_s_type *,
  wms_client_ts_data_s_type *,
  uint8
);
#endif /* FEATURE_ETSI_SMS_CB */

typedef void (*voice_send_ring_result_f_type)
(  
  sys_modem_as_id_e_type,
  cm_call_mode_info_e_type
);

typedef struct dsatdl_vtable_type
{
  dsati_async_event_table_type                *async_event_table_ptr;

  pbm_cb_handler_table_type                   *pbm_cb_handler_table_ptr;

#ifdef FEATURE_ETSI_SMS
  dsatsms_msg_handler_tab_type                *dsatetsisms_msg_handler_table_ptr;
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
  dsatsms_msg_handler_tab_type                *dsat707sms_msg_handler_table_ptr;
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_IS707_ASYNC
  dsat707i_unrec_func_ptr_f_type               dsat707i_unrec_func_ptr_fp;

  dsat707i_send_config_f_type                  dsat707i_send_config_fp;
#endif /* FEATURE_DATA_IS707_ASYNC */

  dsat707_is_status_f_type                     dsat707_is_status_fp;
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_CDMA_SMS
  dsat707smsa_send_unsol_mt_msg_f_type         dsat707smsa_send_unsol_mt_msg_fp;
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DATA_MUX
  dsatact_cmux_handler_f_type                  dsatact_cmux_handler_fp;
#endif /* FEATURE_DATA_MUX */

  dsatact_proc_dial_str_f_type                 dsatact_proc_dial_str_fp;

  dsatcmdp_init_f_type                         dsatcmdp_init_fp;

  dsatcmdp_deinit_f_type                       dsatcmdp_deinit_fp;

  dsatcmdp_process_cmd_line_f_type             dsatcmdp_process_cmd_line_fp;

  dsatcmdpi_queue_cmd_results_f_type           dsatcmdpi_queue_cmd_results_fp;

  dsatcmdp_process_async_cmd_f_type            dsatcmdp_process_async_cmd_fp;

  dsatcmdp_process_async_signal_f_type         dsatcmdp_process_async_signal_fp;

  dsatcmdp_block_indications_f_type            dsatcmdp_block_indications_fp;

  dsatcmdp_sio_is_busy_f_type                  dsatcmdp_sio_is_busy_fp;

  dsatcmdp_sio_is_free_f_type                  dsatcmdp_sio_is_free_fp;

  dsatcmdp_send_respose_to_te_ex_f_type        dsatcmdp_send_respose_to_te_ex_fp;

  dsatcmdp_rmsm_done_handler_f_type            dsatcmdp_rmsm_done_handler_fp;

  dsatcmdp_refresh_subs_based_nv_f_type        dsatcmdp_refresh_subs_based_nv_fp;

  dsatcmif_cm_call_event_handler_f_type        dsatcmif_cm_call_event_handler_fp;

  dsatcmif_cm_call_cmd_handler_f_type          dsatcmif_cm_call_cmd_handler_fp;

  dsatcmif_cm_ss_event_handler_f_type          dsatcmif_cm_ss_event_handler_fp;

  dsatcmif_cm_ph_cmd_handler_f_type            dsatcmif_cm_ph_cmd_handler_fp;

  dsatcmif_cm_ph_event_handler_f_type          dsatcmif_cm_ph_event_handler_fp;

  dsatclient_deregister_fwd_cmd_f_type         dsatclient_deregister_fwd_cmd_fp;

  dsatclient_ext_send_response_f_type          dsatclient_ext_send_response_fp;

  dsatclient_ext_send_urc_f_type               dsatclient_ext_send_urc_fp;

#ifdef FEATURE_DSAT_ETSI_MODE
  dsatetsicall_send_cr_result_f_type           dsatetsicall_send_cr_result_fp;

  dsatetsicall_send_cr_result_ex_f_type        dsatetsicall_send_cr_result_ex_fp;

  dsatetsicall_report_creg_result_f_type       dsatetsicall_report_creg_result_fp;

  etsicall_report_aoc_ccm_f_type               etsicall_report_aoc_ccm_fp;

  etsicmif_report_ussd_result_f_type           etsicmif_report_ussd_result_fp;

  etsicmif_send_sups_error_f_type              etsicmif_send_sups_error_fp;

  dsatetsicmif_cm_sups_event_handler_f_type    dsatetsicmif_cm_sups_event_handler_fp;

  dsatetsicmif_cm_sups_cmd_handler_f_type      dsatetsicmif_cm_sups_cmd_handler_fp;

  dsatetsime_report_ciev_result_f_type         dsatetsime_report_ciev_result_fp;

  dsat_pin_mmgsdi_event_handler_f_type         dsat_pin_mmgsdi_event_handler_fp;
#endif /* FEATURE_DSAT_ETSI_MODE */

  dsatme_cmd_status_handler_f_type             dsatme_cmd_status_handler_fp;

#ifdef FEATURE_MMGSDI
  dsatme_mmgsdi_sim_event_handler_f_type       dsatme_mmgsdi_sim_event_handler_fp;
#endif /* FEATURE_MMGSDI */

  dsatme_report_pacsp_result_f_type            dsatme_report_pacsp_result_fp;

  dsatprepi_set_preprocessor_mode_f_type       dsatprepi_set_preprocessor_mode_fp;

  dsatprep_process_sio_command_f_type          dsatprep_process_sio_command_fp;

  dsatrsp_send_intermediate_result_code_f_type dsatrsp_send_intermediate_result_code_fp;

  dsatrsp_send_result_code_f_type              dsatrsp_send_result_code_fp;

  dsatrsp_send_result_code_ex_f_type           dsatrsp_send_result_code_ex_fp;

#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) )
  dsatrsp_send_dr_result_ex_f_type             dsatrsp_send_dr_result_ex_fp;
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

  dsatrsp_report_rate_on_connect_f_type        dsatrsp_report_rate_on_connect_fp;

#ifdef FEATURE_DSAT_MDM_FUSION
  dsatrsp_process_proxy_response_f_type        dsatrsp_process_proxy_response_fp;

  dsatrsp_process_proxy_urc_f_type             dsatrsp_process_proxy_urc_fp;
#endif /* FEATURE_DSAT_MDM_FUSION */

  dsatrsp_send_urc_f_type                      dsatrsp_send_urc_fp;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  dsatsms_flush_sms_indications_f_type         dsatsms_flush_sms_indications_fp;

  dsatsmsi_cmd_err_cb_func_f_type              dsatsmsi_cmd_err_cb_func_fp;

  sms_msg_event_f_type                         dsatsmsi_memory_status_cfg_event_handler_fp;

  dsatsmsu_send_async_indication_to_te_f_type  dsatsmsu_send_async_indication_to_te_fp;
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_ETSI_SMS
  sms_msg_event_f_type                         write_msg_event_handler_fp;
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_CB
  send_cb_msg_to_te_f_type                     send_cb_msg_to_te_fp;
#endif /* FEATURE_ETSI_SMS_CB */

  voice_send_ring_result_f_type                voice_send_ring_result_fp;
} dsatdl_vtable_s_type;

typedef struct dsatdl_symbol_table_entry_type
{
  void  *addr;
  void **vptr_addr;
} dsatdl_symbol_table_entry_s_type;

typedef struct dsatdl_symbol_table_type
{
  const dsatdl_symbol_table_entry_s_type *base_ptr;
  size_t                                  size;
} dsatdl_symbol_table_s_type;

extern dsatdl_vtable_s_type dsatdl_vtable;

#define DSAT_DL_CHECK_SYMBOL_ADDR(vptr) \
  dsatdl_check_symbol_addr((void **) &vptr)

#define DSAT_DL_VALIDATE_SYMBOL_ADDR(vptr) \
  (TRUE == dsatdl_validate_symbol_addr((void **) &vptr))

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
#define DSAT_DL_LOCK()   dsatdl_lock()

#define DSAT_DL_UNLOCK() dsatdl_unlock()
#else
#define DSAT_DL_LOCK()

#define DSAT_DL_UNLOCK()
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================

                                FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION  DSATDL_INIT

DESCRIPTION
  This function performs initializations for dynamic loading of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_init(void);

/*===========================================================================
FUNCTION  DSATDL_LOCK

DESCRIPTION
  This function locks the access of DL info.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_lock(void);

/*===========================================================================
FUNCTION  DSATDL_UNLOCK

DESCRIPTION
  This function unlocks the access of DL info.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_unlock(void);

/*===========================================================================
FUNCTION  DSATDL_SYMBOL_ADDR_CHK

DESCRIPTION
  This function validates a symbol address and calls ERR_FATAL if it's invalid.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_check_symbol_addr
(
  void **vptr_addr
);

/*===========================================================================
FUNCTION  DSATDL_VALIDATE_SYMBOL_ADDR

DESCRIPTION
  This function validates a symbol address and returns if it's valid.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatdl_validate_symbol_addr
(
  void **vptr_addr
);

#endif /* DSATDL_H */

