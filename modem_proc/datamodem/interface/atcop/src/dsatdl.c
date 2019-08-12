/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D

                        D Y N A M I C   L O A D I N G

GENERAL DESCRIPTION
  This module contains functions that are used for dynamic loading of
  AT commands.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2014 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatdl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk/sc   Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsatdl.h"

#include "dsatact.h"
#include "dsatctab.h"
#include "dsatvoice.h"

#ifdef FEATURE_DATA_IS707 
#include  "dsat707extctab.h"
#include  "dsat707faxctab.h"
#include  "dsat707hdrctab.h"
#include  "dsat707mipctab.h"
#include  "dsat707pstatsctab.h"
#include  "dsat707smsctab.h"
#include  "dsat707util.h"
#include  "dsat707vendctab.h"
#endif /* FEATURE_DATA_IS707   */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#include "dsat707mip.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/

#ifdef FEATURE_ETSI_SMS
extern const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table;
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
extern const dsatsms_msg_handler_tab_type dsat707sms_msg_handler_table;
#endif /* FEATURE_CDMA_SMS */

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/

LOCAL const dsatdl_symbol_table_entry_s_type dsatdl_symbol_table_entries[] =
{
  { (void *) &async_event_table,
    (void **) &dsatdl_vtable.async_event_table_ptr },

  { (void *) &pbm_cb_handler_table,
    (void **) &dsatdl_vtable.pbm_cb_handler_table_ptr },

#ifdef FEATURE_ETSI_SMS
  { (void *) &dsatetsisms_msg_handler_table,
    (void **) &dsatdl_vtable.dsatetsisms_msg_handler_table_ptr },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
  { (void *) &dsat707sms_msg_handler_table,
    (void **) &dsatdl_vtable.dsat707sms_msg_handler_table_ptr },
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_IS707_ASYNC
  { (void *) dsat707i_unrec_func_ptr,
    (void **) &dsatdl_vtable.dsat707i_unrec_func_ptr_fp },

  { (void *) dsat707i_send_config,
    (void **) &dsatdl_vtable.dsat707i_send_config_fp },
#endif /* FEATURE_DATA_IS707_ASYNC */

  { (void *) dsat707_is_status,
    (void **) &dsatdl_vtable.dsat707_is_status_fp },
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_CDMA_SMS
  { (void *) dsat707smsa_send_unsol_mt_msg,
    (void **) &dsatdl_vtable.dsat707smsa_send_unsol_mt_msg_fp },
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DATA_MUX
  { (void *) dsatact_cmux_handler,
    (void **) &dsatdl_vtable.dsatact_cmux_handler_fp },
#endif /* FEATURE_DATA_MUX */

  { (void *) dsatact_proc_dial_str,
    (void **) &dsatdl_vtable.dsatact_proc_dial_str_fp },

  { (void *) dsatcmdp_init,
    (void **) &dsatdl_vtable.dsatcmdp_init_fp },

  { (void *) dsatcmdp_deinit,
    (void **) &dsatdl_vtable.dsatcmdp_deinit_fp },

  { (void *) dsatcmdp_process_cmd_line,
    (void **) &dsatdl_vtable.dsatcmdp_process_cmd_line_fp },

  { (void *) dsatcmdpi_queue_cmd_results,
    (void **) &dsatdl_vtable.dsatcmdpi_queue_cmd_results_fp },

  { (void *) dsatcmdp_process_async_cmd,
    (void **) &dsatdl_vtable.dsatcmdp_process_async_cmd_fp },

  { (void *) dsatcmdp_process_async_signal,
    (void **) &dsatdl_vtable.dsatcmdp_process_async_signal_fp },

  { (void *) dsatcmdp_block_indications,
    (void **) &dsatdl_vtable.dsatcmdp_block_indications_fp },

  { (void *) dsatcmdp_sio_is_busy,
    (void **) &dsatdl_vtable.dsatcmdp_sio_is_busy_fp },

  { (void *) dsatcmdp_sio_is_free,
    (void **) &dsatdl_vtable.dsatcmdp_sio_is_free_fp },

  { (void *) dsatcmdp_send_respose_to_te_ex,
    (void **) &dsatdl_vtable.dsatcmdp_send_respose_to_te_ex_fp },

  { (void *) dsatcmdp_rmsm_done_handler,
    (void **) &dsatdl_vtable.dsatcmdp_rmsm_done_handler_fp },

  { (void *) dsatcmdp_refresh_subs_based_nv,
    (void **) &dsatdl_vtable.dsatcmdp_refresh_subs_based_nv_fp },

  { (void *) dsatcmif_cm_call_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_call_event_handler_fp },

  { (void *) dsatcmif_cm_call_cmd_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp },

  { (void *) dsatcmif_cm_ss_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp },

  { (void *) dsatcmif_cm_ph_cmd_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp },

  { (void *) dsatcmif_cm_ph_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp },

  { (void *) dsatclient_deregister_fwd_cmd,
    (void **) &dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp },

  { (void *) dsatclient_ext_send_response,
    (void **) &dsatdl_vtable.dsatclient_ext_send_response_fp },

  { (void *) dsatclienti_ext_send_urc,
    (void **) &dsatdl_vtable.dsatclient_ext_send_urc_fp },

#ifdef FEATURE_DSAT_ETSI_MODE
  { (void *) dsatetsicall_send_cr_result,
    (void **) &dsatdl_vtable.dsatetsicall_send_cr_result_fp },

  { (void *) dsatetsicall_send_cr_result_ex,
    (void **) &dsatdl_vtable.dsatetsicall_send_cr_result_ex_fp },

  { (void *) dsatetsicall_report_creg_result,
    (void **) &dsatdl_vtable.dsatetsicall_report_creg_result_fp },

  { (void *) etsicall_report_aoc_ccm,
    (void **) &dsatdl_vtable.etsicall_report_aoc_ccm_fp },

  { (void *) etsicmif_report_ussd_result,
    (void **) &dsatdl_vtable.etsicmif_report_ussd_result_fp },  
    
  { (void *) etsicmif_send_sups_error,
    (void **) &dsatdl_vtable.etsicmif_send_sups_error_fp },  
    
  { (void *) dsatetsicmif_cm_sups_event_handler,
    (void **) &dsatdl_vtable.dsatetsicmif_cm_sups_event_handler_fp },

  { (void *) dsatetsicmif_cm_sups_cmd_handler,
    (void **) &dsatdl_vtable.dsatetsicmif_cm_sups_cmd_handler_fp },

  { (void *) dsatetsime_report_ciev_result,
    (void **) &dsatdl_vtable.dsatetsime_report_ciev_result_fp },

  { (void *) dsat_pin_mmgsdi_event_handler,
    (void **) &dsatdl_vtable.dsat_pin_mmgsdi_event_handler_fp },
#endif /* FEATURE_DSAT_ETSI_MODE */

  { (void *) dsatme_cmd_status_handler,
    (void **) &dsatdl_vtable.dsatme_cmd_status_handler_fp },

#ifdef FEATURE_MMGSDI
  { (void *) dsatme_mmgsdi_sim_event_handler,
    (void **) &dsatdl_vtable.dsatme_mmgsdi_sim_event_handler_fp },
#endif /* FEATURE_MMGSDI */

  { (void *) dsatme_report_pacsp_result,
    (void **) &dsatdl_vtable.dsatme_report_pacsp_result_fp },

  { (void *) dsatprepi_set_preprocessor_mode,
    (void **) &dsatdl_vtable.dsatprepi_set_preprocessor_mode_fp },

  { (void *) dsatprep_process_sio_command,
    (void **) &dsatdl_vtable.dsatprep_process_sio_command_fp },

  { (void *) dsatrsp_send_intermediate_result_code,
    (void **) &dsatdl_vtable.dsatrsp_send_intermediate_result_code_fp },

  { (void *) dsatrsp_send_result_code,
    (void **) &dsatdl_vtable.dsatrsp_send_result_code_fp },

  { (void *) dsatrsp_send_result_code_ex,
    (void **) &dsatdl_vtable.dsatrsp_send_result_code_ex_fp },

#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) )
  { (void *) dsatrsp_send_dr_result_ex,
    (void **) &dsatdl_vtable.dsatrsp_send_dr_result_ex_fp },
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

  { (void *) dsatrsp_report_rate_on_connect,
    (void **) &dsatdl_vtable.dsatrsp_report_rate_on_connect_fp },

#ifdef FEATURE_DSAT_MDM_FUSION
  { (void *) dsatrsp_process_proxy_response,
    (void **) &dsatdl_vtable.dsatrsp_process_proxy_response_fp },

  { (void *) dsatrsp_process_proxy_urc,
    (void **) &dsatdl_vtable.dsatrsp_process_proxy_urc_fp },
#endif /* FEATURE_DSAT_MDM_FUSION */

  { (void *) dsatrsp_send_urc,
    (void **) &dsatdl_vtable.dsatrsp_send_urc_fp },

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  { (void *) dsatsms_flush_sms_indications,
    (void **) &dsatdl_vtable.dsatsms_flush_sms_indications_fp },

  { (void *) dsatsmsi_cmd_err_cb_func,
    (void **) &dsatdl_vtable.dsatsmsi_cmd_err_cb_func_fp },

  { (void *) dsatsmsi_memory_status_cfg_event_handler,
    (void **) &dsatdl_vtable.dsatsmsi_memory_status_cfg_event_handler_fp },

  { (void *) dsatsmsu_send_async_indication_to_te,
  	(void **) &dsatdl_vtable.dsatsmsu_send_async_indication_to_te_fp },
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_ETSI_SMS
  { (void *) write_msg_event_handler,
    (void **) &dsatdl_vtable.write_msg_event_handler_fp },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_CB
  { (void *) send_cb_msg_to_te,
    (void **) &dsatdl_vtable.send_cb_msg_to_te_fp },
#endif /* FEATURE_ETSI_SMS_CB */

  { (void *) voice_send_ring_result,
    (void **) &dsatdl_vtable.voice_send_ring_result_fp }
};

const dsatdl_symbol_table_s_type dsatdl_symbol_table =
{
  dsatdl_symbol_table_entries,
  ARR_SIZE(dsatdl_symbol_table_entries)
};

