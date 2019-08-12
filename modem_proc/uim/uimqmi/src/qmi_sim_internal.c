/*==========================================================================

                         Q M I_S I M_I N T E R N A L. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface SIM Service source file. This file
 provides implementation for service module that processes qmi sim requests
 from the client module running on another modem. It does this by remoting
 these calls to the modem proc over QMI using QCCI framework. With each request
 from the client the service task gets signalled which then invokes the service
 module. The service proceses requests on the modem and sends responses to the
 client through the QCSI framework.

EXTERNALIZED FUNCTIONS


                        COPYRIGHT INFORMATION

Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_sim_internal.c#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------
05/22/16    vdc    Remove F3 messages for memory allocation failure
05/17/16    bcho   Deregister as SAP client with correct handle
11/25/15    sp     Updated copyright
11/05/15    ks     Rel12 polling enhancement
07/14/14    tl     Remove always ON feature FEATURE_MODEM_RCINIT
04/18/14    tl     Add to function to initialize globals
03/17/14    kb     Update function assert_on_crit_sect_owner
01/28/14    kb     Disable function assert_on_crit_sect_owner
01/10/14    df     Use v2.0 diag macros
10/28/13    kb     Add offtarget support to check mutex is owned or not
08/28/13    tkl    Rename FEATURE_UIM_SGLTE
09/30/13    bcho   Changes for tracking transactions between QSC and MSM/MDM
09/23/13    bcho   Null check for dereferenced output parameter removed
06/20/13    gm     Passing proactive_cmd_pending through TAL
07/02/13    kb     Fix compilation error
06/21/13    kb     Added QMI malloc and free wrapper functions
06/20/13    yt     Support for sending FDN event
06/07/13    yt     Send intermediate status words with envelope response
05/21/13    yt     Fix Klocwork errors
05/15/13    gm     Replaced instances of memcpy with memscpy
04/30/13    yt     Support for NAA REFRESH
04/24/13    yt     Send test_mcc_status event after first SUBSCRIPTION_READY
02/25/13    tl     Fix Klockwork errors
02/15/13    vs     Skip sending evt to remote modem if evt handler is NULL
01/02/12    tl     Replace QMI_RESULT_SUCCESS with QMI_RESULT_SUCCESS_V01
12/10/12    tl     Add error handling to command handler functions
11/12/12    tl     Clear the remost_evt_cb_table upon session close or dereg
11/06/12    tl     Fixed memory leak caused by incorrect NULL check
10/05/12    tl     Fix register file update for by path access
10/02/12    tl     Removed incorrect MMGSDI result code conversion
10/01/12    tl     Updating QCSI registration to be processor specific
10/01/12    vs     Added utility for context check including logic for SGLTE
09/30/12    vs     Fixed TAL command group translation
07/31/12    tkl    Added additional MMGSDI APIs support
07/26/12    tkl    Add refresh and get protocol APIs
07/26/12    tkl    Add support for EONS cache ready event
07/26/12    tkl    Add new MMGSDI and GSTK APIs
05/22/12    tkl    Fixed compile warnings
03/12/12    tkl    Initial check-in of QMI_SIM_INTERNAL
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
#include "err.h"
#include "rex.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

#include "modem_mem.h"
#include "amssassert.h"
#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "subscriber_identity_module_internal_v01.h"

#include "qmi_sim_internal.h"
#include "qmi_sim_internal_util.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdilib_v.h"
#include "mmgsdilib_p.h"
#include "gstk_exp.h"
#include "ipc_policy.h"

#include "qmi_sap.h"
#include "qmi_uim_cat_common.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

#define QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT        (10)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/* Globals to hold handles used across different QCSI calls. */
static qmi_csi_service_handle     qmi_sim_internal_provider   = NULL;

static qmi_sap_client_handle      qmi_sim_internal_sap_handle = NULL;

qmi_sim_internal_client_info_type qmi_sim_internal_client_registry[QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT];

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* Prototype of MMGSDI response callback function */
static void qmi_sim_internal_mmgsdi_response_callback
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type  *cnf_ptr
);

/* Prototype of MMGSDI event callback function */
static void qmi_sim_internal_mmgsdi_evt_callback
(
  const mmgsdi_event_data_type * event
);

/* Prototype of GSTK event callback function */
static void qmi_sim_internal_gstk_evt_callback
(
  gstk_tal_cmd_details_type * gstk_evt
);

/* Prototype of handler function */
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_client_id_and_evt_reg_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

/* Prototype of handler function */
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_client_id_and_evt_dereg_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

/* Prototype of handler function */
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_open_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

/* Prototype of handler function */
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_close_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

/* Prototype of handler function */
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_all_available_apps_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_all_available_apps_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_file_attr_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_transparent_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_record_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_transparent_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_record_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_cache_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_cache_file_size_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_cache_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_is_service_available_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_toggle_uim_power_control_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_info_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_usim_authenticate_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_refresh_complete_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_perso_ota_deperso_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_search_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_seek_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_gstk_tal_client_registration_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_gstk_tal_send_cmd_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_gstk_tal_notify_rat_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_gstk_tal_notify_ls_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_file_update_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_protocol_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_increase_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

static qmi_csi_cb_error qmi_sim_internal_mmgsdi_naa_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
);

/*===========================================================================

                         REQUEST HANDLER FUNCTIONS

===========================================================================*/
/* request handler fucntion prototype */
typedef qmi_csi_cb_error (* const qmi_sim_internal_req_handler)
(qmi_sim_internal_client_info_type *connection_handle,
 qmi_req_handle                     req_handle,
 void                              *req_c_struct,
 int                                req_c_struct_len);

/* Table to handle the dispatch of request handler functions. */
static qmi_sim_internal_req_handler qmi_sim_internal_req_handle_table[] =
{
  qmi_sim_internal_mmgsdi_client_id_and_evt_reg_handler,            /* Request handler for message ID 0x20 */
  qmi_sim_internal_mmgsdi_session_open_handler,                     /* Request handler for message ID 0x21 */
  qmi_sim_internal_mmgsdi_get_all_available_apps_handler,           /* Request handler for message ID 0x22 */
  qmi_sim_internal_mmgsdi_get_all_available_apps_sync_handler,      /* Request handler for message ID 0x23 */
  qmi_sim_internal_mmgsdi_get_file_attr_handler,                    /* Request handler for message ID 0x24 */
  qmi_sim_internal_mmgsdi_read_transparent_handler,                 /* Request handler for message ID 0x25 */
  qmi_sim_internal_mmgsdi_read_record_handler,                      /* Request handler for message ID 0x26 */
  qmi_sim_internal_mmgsdi_write_transparent_handler,                /* Request handler for message ID 0x27 */
  qmi_sim_internal_mmgsdi_write_record_handler,                     /* Request handler for message ID 0x28 */
  qmi_sim_internal_mmgsdi_read_cache_handler,                       /* Request handler for message ID 0x29 */
  qmi_sim_internal_mmgsdi_read_cache_file_size_handler,             /* Request handler for message ID 0x2A */
  qmi_sim_internal_mmgsdi_write_cache_handler,                      /* Request handler for message ID 0x2B */
  qmi_sim_internal_mmgsdi_session_is_service_available_handler,     /* Request handler for message ID 0x2C */
  qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler,     /* Request handler for message ID 0x2D */
  qmi_sim_internal_mmgsdi_toggle_uim_power_control_sync_handler,    /* Request handler for message ID 0x2E */
  qmi_sim_internal_mmgsdi_session_get_info_sync_handler,            /* Request handler for message ID 0x2F */
  NULL,  /* MMGSDI_SESSION_UICC_AUTHENTICATE */                     /* Request handler for message ID 0x30 */
  qmi_sim_internal_mmgsdi_session_usim_authenticate_handler,        /* Request handler for message ID 0x31 */
  qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler,             /* Request handler for message ID 0x32 */
  qmi_sim_internal_mmgsdi_session_register_for_refresh_handler,     /* Request handler for message ID 0x33 */
  NULL,  /* MMGSDI Event Report IND */                              /* Request handler for message ID 0x34 */
  qmi_sim_internal_mmgsdi_client_id_and_evt_dereg_handler,          /* Request handler for message ID 0x35*/
  qmi_sim_internal_mmgsdi_session_close_handler,                    /* Request handler for message ID 0x36 */
  qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler,/* Request handler for message ID 0x37 */
  qmi_sim_internal_mmgsdi_session_increase_handler,                 /* Request handler for message ID 0x38 */
  qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler,       /* Request handler for message ID 0x39 */
  qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler,/* Request handler for message ID 0x3A */
  qmi_sim_internal_mmgsdi_session_perso_ota_deperso_handler,        /* Request handler for message ID 0x3B */
  qmi_sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_handler,
                                                                    /* Request handler for message ID 0x3C */
  qmi_sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_handler,
                                                                    /* Request handler for message ID 0x3D */
  qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler,            /* Request handler for message ID 0x3E */
  qmi_sim_internal_mmgsdi_session_refresh_complete_handler,         /* Request handler for message ID 0x3F */
  qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler,   /* Request handler for message ID 0x40 */
  qmi_sim_internal_mmgsdi_session_search_handler,                   /* Request handler for message ID 0x41 */
  qmi_sim_internal_mmgsdi_session_seek_handler,                     /* Request handler for message ID 0x42 */
  qmi_sim_internal_gstk_tal_client_registration_handler,            /* Request handler for message ID 0x43 */
  NULL, /* TAL Event Report IND */                                  /* Request handler for message ID 0x44 */
  qmi_sim_internal_gstk_tal_send_cmd_handler,                       /* Request handler for message ID 0x45 */
  qmi_sim_internal_gstk_tal_notify_rat_handler,                     /* Request handler for message ID 0x46 */
  qmi_sim_internal_gstk_tal_notify_ls_handler,                      /* Request handler for message ID 0x47 */
  qmi_sim_internal_mmgsdi_session_register_for_file_update_handler, /* Request handler for message ID 0x48 */
  qmi_sim_internal_mmgsdi_get_protocol_handler,                     /* Request handler for message ID 0x49 */
  qmi_sim_internal_mmgsdi_naa_refresh_handler                       /* Request handler for message ID 0x4A */
};


/*===========================================================================

                         CALLBACK HANDLE FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_STORE_REMOTE_CB_HANDLE

  DESCRIPTION
    To store remote callback information before sending request to mmgsdi

  PARAMETERS
    local_client_ref_ptr : client ref data ptr for storage
    req_data : request data with remote client ref and remote response cb
    connection_handle : qcsi client handle
    msg_id : message id
    event_handle : remote event cb

  RETURN VALUE
    qmi_sim_internal_local_client_ref_type*

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_sim_internal_local_client_ref_type* qmi_sim_internal_store_remote_cb_handle
(
  sim_internal_mmgsdi_request_data_type_v01    req_data,
  qmi_sim_internal_client_info_type           *connection_handle,
  uint8                                        msg_id,
  uint32                                       event_handle,
  boolean                                      token_valid,
  uint16                                       token
)
{
  qmi_sim_internal_local_client_ref_type     *local_client_ref_ptr = NULL;

  UIM_MSG_HIGH_0("Received qmi_sim_internal_store_remote_cb_handle");
  local_client_ref_ptr = (qmi_sim_internal_local_client_ref_type*)
                          uimqmi_malloc(sizeof(qmi_sim_internal_local_client_ref_type));
  if (local_client_ref_ptr != NULL)
  {
    memset(local_client_ref_ptr, 0, sizeof(qmi_sim_internal_local_client_ref_type));
    local_client_ref_ptr->remote_cmd_cb        = req_data.response_handle;
    local_client_ref_ptr->remote_client_ref    = req_data.client_ref;
    local_client_ref_ptr->remote_evt_cb        = event_handle;
    local_client_ref_ptr->msg_id               = msg_id;
    local_client_ref_ptr->qcsi_client          = connection_handle;
    if (token_valid)
    {
      qmi_sim_internal_add_token_to_txn_list(token);
      local_client_ref_ptr->token            = token;
      local_client_ref_ptr->token_valid      = TRUE;
    }
  }
  return local_client_ref_ptr;
} /* qmi_sim_internal_store_remote_cb_handle */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_STORE_EVT_CB_HANDLE

  DESCRIPTION
    To store event callback information before sending request to mmgsdi

  PARAMETERS
    connection_handle : qcsi client handle
    event_handle : remote event cb
    session_id : session id
    client_id : client id
    evt_handle_index : event handle index

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_store_evt_cb_handle
(
  qmi_sim_internal_client_info_type          *connection_handle,
  uint32                                      event_handle,
  uint64                                      session_id,
  uint64                                      client_id,
  uint8                                      *evt_handle_index
)
{
  uint8 index = 0;

  UIM_MSG_HIGH_0("Received qmi_sim_internal_store_evt_cb_handle");

  if (!connection_handle|| !evt_handle_index)
  {
    UIM_MSG_ERR_0("Invalid parameter");
    return QMI_CSI_INTERNAL_ERR;
  }

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  /* no client_id info until cmd cb is received. set to 0*/
  for (index=0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
  {
    if (!qmi_sim_internal_remote_evt_cb_table[index].in_use)
    {
      qmi_sim_internal_remote_evt_cb_table[index].qcsi_client = connection_handle;
      qmi_sim_internal_remote_evt_cb_table[index].client_id = client_id;
      qmi_sim_internal_remote_evt_cb_table[index].session_id = session_id;
      qmi_sim_internal_remote_evt_cb_table[index].in_use = TRUE;
      qmi_sim_internal_remote_evt_cb_table[index].remote_evt_cb = event_handle;
      break;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if (index >= QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_store_evt_cb_handle : No Slot availbale to store event_handle");
    return QMI_CSI_NO_MEM;
  }

  UIM_MSG_HIGH_3("remote_evt_cb_table[%d]: qcsi_client 0x%x, client id 0x%x",
                 index,
                 qmi_sim_internal_remote_evt_cb_table[index].qcsi_client,
                 qmi_sim_internal_remote_evt_cb_table[index].client_id);
  UIM_MSG_HIGH_3("in use 0x%x, session_id 0x%x, event handle 0x%x",
                 qmi_sim_internal_remote_evt_cb_table[index].in_use,
                 qmi_sim_internal_remote_evt_cb_table[index].session_id,
                 qmi_sim_internal_remote_evt_cb_table[index].remote_evt_cb);
  *evt_handle_index = index;

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_store_evt_cb_handle */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CLIENT_SESSION_ID_TO_INDEX

  DESCRIPTION
    return an index of an event handle entry in remost_evt_cb_table
    to be deleted using the client id or session id

  PARAMETERS
    session id
    client id

  RETURN VALUE
    uint8 index : index of remost_evt_cb_table to be cleared

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static uint8 qmi_sim_internal_client_session_id_to_index
(
  uint64                                   session_id,
  uint64                                   client_id
)
{
  uint8     index = QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB;

  if (session_id)
  {
    for (index = 0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
    {
      if (qmi_sim_internal_remote_evt_cb_table[index].session_id ==
          session_id)
      {
        return index;
      }
    }
  }

  if (client_id)
  {
    for (index = 0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
    {
      if (qmi_sim_internal_remote_evt_cb_table[index].client_id ==
          client_id)
      {
        return index;
      }
    }
  }

  UIM_MSG_HIGH_2("Invalid ID: session id: 0x%x, client id: 0x%x",
                 session_id, client_id);

  return index;
} /* qmi_sim_internal_client_session_id_to_index */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CLEAR_EVENT_HANDLE_ENTRY

  DESCRIPTION
    To clear the event handle entry in remost_evt_cb_table

  PARAMETERS
    index   : index which event cb table needs to be cleared

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_sim_internal_clear_event_handle_entry
(
  uint8     index
)
{
  if(index >= QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB)
  {
    UIM_MSG_ERR_0("index out of range ");
    return;
  }
  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  /* clean up evt cb storage as there won't be any cb indication*/
  qmi_sim_internal_remote_evt_cb_table[index].in_use = FALSE;
  qmi_sim_internal_remote_evt_cb_table[index].session_id = 0;
  qmi_sim_internal_remote_evt_cb_table[index].client_id = 0;
  qmi_sim_internal_remote_evt_cb_table[index].qcsi_client = 0;
  qmi_sim_internal_remote_evt_cb_table[index].remote_evt_cb = 0;

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
} /* qmi_sim_internal_clear_event_handle_entry */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_STORE_GSTK_EVT_CB_HANDLE

  DESCRIPTION
    To store event callback information before sending request to gstk

  PARAMETERS
    connection_handle : qcsi client handle
    client_type : gstk tal client type
    event_handle : remote gstk event cb

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_store_gstk_evt_cb_handle
( qmi_sim_internal_client_info_type          *connection_handle,
  uint32                                      event_handle
)
{
  qmi_csi_error      resp_err = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("Received qmi_sim_internal_store_gstk_evt_cb_handle");

  if (!connection_handle)
  {
    UIM_MSG_ERR_0("Invalid parameter");
    return QMI_CSI_INTERNAL_ERR;
  }

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  if (qmi_sim_internal_remote_gstk_evt_cb.in_use)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_store_evt_cb_handle : No Slot availbale to store event_handle");
    resp_err = QMI_CSI_NO_MEM ;
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  qmi_sim_internal_remote_gstk_evt_cb.qcsi_client = connection_handle;
  qmi_sim_internal_remote_gstk_evt_cb.in_use = TRUE;
  qmi_sim_internal_remote_gstk_evt_cb.remote_evt_cb = event_handle;

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  UIM_MSG_HIGH_3("remote_gstk_evt_cb_table[%d]: qcsi_client 0x%x, in_use 0x%x",
                 qmi_sim_internal_remote_gstk_evt_cb.qcsi_client,
                 qmi_sim_internal_remote_gstk_evt_cb.in_use,
                 qmi_sim_internal_remote_gstk_evt_cb.remote_evt_cb);

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_store_gstk_evt_cb_handle */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CLEAR_GSTK_EVENT_HANDLE_ENTRY

  DESCRIPTION
    To clear the event handle entry in remote_gstk_evt_cb_table

  PARAMETERS
    index   : index which event cb table needs to be cleared

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_clear_gstk_event_handle_entry
(
  uint8     index
)
{
  if(index >= QMI_SIM_INTERNAL_MAX_REMOTE_GSTK_EVT_CB)
  {
    UIM_MSG_ERR_0("index out of range ");
    return;
  }
  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  /* clean up evt cb storage as there won't be any cb indication*/
  qmi_sim_internal_remote_gstk_evt_cb.in_use = FALSE;
  qmi_sim_internal_remote_gstk_evt_cb.qcsi_client = 0;
  qmi_sim_internal_remote_gstk_evt_cb.remote_evt_cb = 0;

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
} /* qmi_sim_internal_clear_event_handle_entry */


/*===========================================================================

                         REQUEST HANDLER FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_HANDLER()

  DESCRIPTION
    Function called to client id and event registration

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_client_id_and_evt_reg_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01   resp;
  mmgsdi_client_id_and_evt_reg_cnf_type                    client_id_and_evt_reg_cnf;
  sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                  *local_client_ref_ptr = NULL;
  mmgsdi_return_enum_type                                  mmgsdi_status        = MMGSDI_SUCCESS;
  uint8                                                    evt_handle_index     = 0;
  qmi_csi_error                                            resp_err             = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_client_id_and_evt_reg_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01));
  memset(&client_id_and_evt_reg_cnf, 0, sizeof(client_id_and_evt_reg_cnf));

  if (!connection_handle||!req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_mmgsdi_client_id_and_evt_reg_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_REQ_V01,
                                     req_msg->event_handle,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  /* store remote evt cb info and qcsi client handle for indication*/
  resp_err = qmi_sim_internal_store_evt_cb_handle(connection_handle,
                                                  req_msg->event_handle,
                                                  0,
                                                  0,
                                                  &evt_handle_index);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to store evt cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  /* Get a MMGSDI client id */
  mmgsdi_status = mmgsdi_client_id_and_evt_reg( qmi_sim_internal_mmgsdi_evt_callback,
                                                qmi_sim_internal_mmgsdi_response_callback,
                                                (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    client_id_and_evt_reg_cnf.response_header.mmgsdi_status = mmgsdi_status;
    client_id_and_evt_reg_cnf.response_header.response_type = MMGSDI_CLIENT_ID_AND_EVT_REG_CNF;

    client_id_and_evt_reg_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    client_id_and_evt_reg_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_client_id_and_evt_reg_ind(local_client_ref_ptr,
                                                    &client_id_and_evt_reg_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }

    qmi_sim_internal_clear_event_handle_entry(evt_handle_index);
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_client_id_and_evt_reg_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_HANDLER()

  DESCRIPTION
    Function called to session open

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_open_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_open_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type         *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_open_resp_msg_v01   resp;
  mmgsdi_session_open_cnf_type                    session_open_cnf;
  mmgsdi_non_prov_app_info_type                   non_prov_app_data;
  mmgsdi_non_prov_app_info_type                  *non_prov_app_data_ptr = NULL;
  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  uint8                                           evt_handle_index  = 0;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_open_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_open_resp_msg_v01));
  memset(&session_open_cnf, 0, sizeof(mmgsdi_session_open_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_open_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_open_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                        (req_msg->req_data,
                                         connection_handle,
                                         QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_REQ_V01,
                                         req_msg->session_open.event_handle,
                                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("client id 0x%x", req_msg->session_open.client_id);

  /* store remote evt cb info and qcsi client handle for indication*/
  resp_err = qmi_sim_internal_store_evt_cb_handle(connection_handle,
                                                  req_msg->session_open.event_handle,
                                                  0,
                                                  req_msg->session_open.client_id,
                                                  &evt_handle_index);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to store evt cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->session_open.session_type == SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_1_V01 ||
     req_msg->session_open.session_type == SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_2_V01)
  {
    if(req_msg->non_prov_app_data_valid)
    {
      /* set non prov app data*/
      non_prov_app_data.exclusive_channel = req_msg->non_prov_app_data.exclusive_channel;
      non_prov_app_data.app_id_data.data_len = req_msg->non_prov_app_data.app_info.data_len;
      non_prov_app_data.app_id_data.data_ptr = req_msg->non_prov_app_data.app_info.data;
      non_prov_app_data_ptr = &non_prov_app_data;
    }
    else
    {
      UIM_MSG_ERR_1("missing non_prov_app_data TLV for session type 0x%x",
                    req_msg->session_open.session_type);
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
  }
  else
  {
    if(req_msg->non_prov_app_data_valid)
    {
      UIM_MSG_ERR_1("invalid non_prov_app_data TLV for session type 0x%x",
                    req_msg->session_open.session_type);
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
  }

  mmgsdi_status = mmgsdi_session_open(req_msg->session_open.client_id,
                                      req_msg->session_open.session_type,
                                      non_prov_app_data_ptr,
                                      qmi_sim_internal_mmgsdi_evt_callback,
                                      req_msg->session_open.set_notify,
                                      qmi_sim_internal_mmgsdi_response_callback,
                                      (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_open failed");
    /* return callback indication with mmgsdi error*/
    session_open_cnf.response_header.mmgsdi_status = mmgsdi_status;
    session_open_cnf.response_header.response_type = MMGSDI_SESSION_OPEN_CNF;

    session_open_cnf.response_header.client_id     = req_msg->session_open.client_id;
    session_open_cnf.response_header.slot_id       = MMGSDI_SLOT_AUTOMATIC;
    session_open_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    session_open_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_session_open_ind(local_client_ref_ptr,
                                           &session_open_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }

    /* clean up evt cb storage as there won't be any cb indication*/
    qmi_sim_internal_clear_event_handle_entry(evt_handle_index);
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_open_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_HANDLER()

  DESCRIPTION
    Function called to session close

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_close_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_close_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type          *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_close_resp_msg_v01   resp;
  mmgsdi_session_close_cnf_type                    session_close_cnf;
  mmgsdi_return_enum_type                          mmgsdi_status        = MMGSDI_SUCCESS;
  qmi_csi_error                                    resp_err             = QMI_CSI_NO_ERR;
  uint8                                            index                = QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_close_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_close_resp_msg_v01));
  memset(&session_close_cnf, 0, sizeof(mmgsdi_session_close_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_close_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_close_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                        (req_msg->req_data,
                                         connection_handle,
                                         QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_REQ_V01,
                                         0,
                                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                                         req_msg->token);

  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  /* clear event handle */
  index = qmi_sim_internal_client_session_id_to_index(req_msg->session_id, 0);
  qmi_sim_internal_clear_event_handle_entry(index);

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);

  mmgsdi_status = mmgsdi_session_close(req_msg->session_id,
                                       qmi_sim_internal_mmgsdi_response_callback,
                                       (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_close failed");
    /* return callback indication with mmgsdi error*/
    session_close_cnf.response_header.mmgsdi_status = mmgsdi_status;
    session_close_cnf.response_header.response_type = MMGSDI_SESSION_CLOSE_CNF;

    session_close_cnf.response_header.session_id    = req_msg->session_id;
    session_close_cnf.response_header.slot_id       = MMGSDI_SLOT_AUTOMATIC;
    session_close_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    session_close_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_session_close_ind(local_client_ref_ptr, &session_close_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_close_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_HANDLER()

  DESCRIPTION
    Function called to session get all available apps

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_all_available_apps_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_get_all_available_apps_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                   *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01   resp;
  mmgsdi_get_all_available_apps_cnf_type                    get_all_avail_apps_cnf;

  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_get_all_available_apps_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01));
  memset(&get_all_avail_apps_cnf, 0, sizeof(mmgsdi_get_all_available_apps_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_all_available_apps_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_get_all_available_apps_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("client id 0x%x", req_msg->client_id);
  mmgsdi_status = mmgsdi_get_all_available_apps(req_msg->client_id,
                                                qmi_sim_internal_mmgsdi_response_callback,
                                                (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    get_all_avail_apps_cnf.response_header.mmgsdi_status = mmgsdi_status;
    get_all_avail_apps_cnf.response_header.response_type = MMGSDI_GET_ALL_AVAILABLE_APPS_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    get_all_avail_apps_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    get_all_avail_apps_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_get_all_avai_apps_ind(local_client_ref_ptr,
                                                &get_all_avail_apps_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_get_all_available_apps_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC_HANDLER()

  DESCRIPTION
    Function called to session get all available apps sync

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_all_available_apps_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01 *req_msg = NULL;
  sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01 resp;
  mmgsdi_available_apps_type                                   avail_apps;

  mmgsdi_return_enum_type mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error           resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_get_all_available_apps_sync_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01));
  memset(&avail_apps, 0, sizeof(mmgsdi_available_apps_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_all_available_apps_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01*)req_c_struct;

  UIM_MSG_HIGH_1("client id 0x%x", req_msg->client_id);
  mmgsdi_status = mmgsdi_get_all_available_apps_sync(req_msg->client_id, &avail_apps);

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01)
  {
    qmi_sim_internal_response_get_all_available_apps_sync(&avail_apps, &resp);
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return resp_err;
} /* qmi_sim_internal_mmgsdi_get_all_available_apps_sync_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_GET_FILE_ATTR_HANDLER()

  DESCRIPTION
    Function called to session get file attribute

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_file_attr_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_get_file_attr_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                  *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01   resp;
  mmgsdi_access_type                                       file_name;
  mmgsdi_get_file_attr_cnf_type                            get_file_attr_cnf;

  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_get_file_attr_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01));
  memset(&get_file_attr_cnf, 0, sizeof(mmgsdi_get_file_attr_cnf_type));
  memset(&file_name, 0, sizeof(mmgsdi_access_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_file_attr_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_get_file_attr_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
    UIM_MSG_HIGH_3("path len 0x%x: [0] 0x%x, [1] 0x%x ",
                   req_msg->path_type_file.path_buf_len,
                   req_msg->path_type_file.path_buf[0],
                   req_msg->path_type_file.path_buf[1]);

    UIM_MSG_HIGH_3("[2] 0x%x, [3] 0x%x, [4] 0x%x ",
                   req_msg->path_type_file.path_buf[2],
                   req_msg->path_type_file.path_buf[3],
                   req_msg->path_type_file.path_buf[4]);
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);
  mmgsdi_status = mmgsdi_session_get_file_attr(req_msg->session_id,
                                               file_name,
                                               qmi_sim_internal_mmgsdi_response_callback,
                                               (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_file_attr_handler failed");
    /* return callback indication with mmgsdi error*/
    get_file_attr_cnf.response_header.mmgsdi_status = mmgsdi_status;
    get_file_attr_cnf.response_header.response_type = MMGSDI_GET_FILE_ATTR_CNF;
    get_file_attr_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    get_file_attr_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_get_file_attr_ind(local_client_ref_ptr,
                                            &get_file_attr_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_get_file_attr_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_READ_TRANSPARENT_HANDLER()

  DESCRIPTION
    Function called to session read transparent

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_transparent_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_read_transparent_req_msg_v01   *req_msg               = NULL;
  qmi_sim_internal_local_client_ref_type                      *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_read_transparent_resp_msg_v01   resp;
  mmgsdi_access_type                                          file_name;
  mmgsdi_read_cnf_type                                        read_cnf;

  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_read_transparent_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01));
  memset(&read_cnf, 0, sizeof(mmgsdi_read_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_read_transparent_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_read_transparent_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                (req_msg->req_data,
                                 connection_handle,
                                 QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_REQ_V01,
                                 0,
                                 (req_msg->token_valid == 0) ? FALSE : TRUE,
                                 req_msg->token);

  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->read_req.session_id);
  mmgsdi_status = mmgsdi_session_read_transparent(req_msg->read_req.session_id,
                                                  file_name,
                                                  req_msg->read_req.offset,
                                                  req_msg->read_req.req_len,
                                                  qmi_sim_internal_mmgsdi_response_callback,
                                                  (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_read_transparent failed");
    /* return callback indication with mmgsdi error*/
    read_cnf.response_header.mmgsdi_status = mmgsdi_status;
    read_cnf.response_header.response_type = MMGSDI_READ_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    read_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    read_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_read_transparent_ind(local_client_ref_ptr,
                                               &read_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_read_transparent_handler */



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_READ_RECORD_HANDLER()

  DESCRIPTION
    Function called to session read record

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_record_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_read_record_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_read_record_resp_msg_v01   resp;
  mmgsdi_access_type                                     file_name;
  mmgsdi_read_cnf_type                                   read_cnf;

  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err          = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_read_record_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_read_record_resp_msg_v01));
  memset(&read_cnf, 0, sizeof(mmgsdi_read_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_read_record_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_read_record_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                              (req_msg->req_data,
                               connection_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_REQ_V01,
                               0,
                               (req_msg->token_valid == 0) ? FALSE : TRUE,
                               req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->read_req.session_id);
  mmgsdi_status = mmgsdi_session_read_record(req_msg->read_req.session_id,
                                                  file_name,
                                                  req_msg->read_req.record_num,
                                                  req_msg->read_req.req_len,
                                                  qmi_sim_internal_mmgsdi_response_callback,
                                                  (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_read_record failed");
    /* return callback indication with mmgsdi error*/
    read_cnf.response_header.mmgsdi_status = mmgsdi_status;
    read_cnf.response_header.response_type = MMGSDI_READ_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    read_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    read_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_read_record_ind(local_client_ref_ptr,
                                               &read_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_read_record_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_WRITE_TRANSPARENT_HANDLER()

  DESCRIPTION
    Function called to session write transparent

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_transparent_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_write_transparent_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                      *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_write_transparent_resp_msg_v01   resp;
  mmgsdi_access_type                                           file_name;
  mmgsdi_data_type                                             write_data;
  mmgsdi_write_cnf_type                                        write_cnf;

  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_write_transparent_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01));
  memset(&write_cnf, 0, sizeof(mmgsdi_write_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_write_transparent_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_write_transparent_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                  (req_msg->req_data,
                                   connection_handle,
                                   QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_REQ_V01,
                                   0,
                                   (req_msg->token_valid == 0) ? FALSE : TRUE,
                                   req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  write_data.data_len = req_msg->write_req.write_data.data_len;
  write_data.data_ptr = req_msg->write_req.write_data.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->write_req.session_id);
  mmgsdi_status = mmgsdi_session_write_transparent(req_msg->write_req.session_id,
                                                  file_name,
                                                  req_msg->write_req.offset,
                                                  write_data,
                                                  qmi_sim_internal_mmgsdi_response_callback,
                                                  (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_write_transparent failed");
    /* return callback indication with mmgsdi error*/
    write_cnf.response_header.mmgsdi_status = mmgsdi_status;
    write_cnf.response_header.response_type = MMGSDI_WRITE_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    write_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    write_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_write_transparent_ind(local_client_ref_ptr,
                                               &write_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_write_transparent_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_WRITE_RECORD_HANDLER()

  DESCRIPTION
    Function called to session write record

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_record_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_write_record_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                 *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_write_record_resp_msg_v01   resp;
  mmgsdi_access_type                                      file_name;
  mmgsdi_file_structure_enum_type                         record_type;
  mmgsdi_data_type                                        write_data;
  mmgsdi_write_cnf_type                                   write_cnf;

  mmgsdi_return_enum_type                         mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_write_record_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_write_record_resp_msg_v01));
  memset(&write_cnf, 0, sizeof(mmgsdi_write_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_write_record_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_write_record_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                  (req_msg->req_data,
                                   connection_handle,
                                   QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_REQ_V01,
                                   0,
                                   (req_msg->token_valid == 0) ? FALSE : TRUE,
                                   req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file_structure(req_msg->write_req.record_type,
                                                                         &record_type);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  write_data.data_len = req_msg->write_req.write_data.data_len;
  write_data.data_ptr = req_msg->write_req.write_data.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->write_req.session_id);
  mmgsdi_status = mmgsdi_session_write_record(req_msg->write_req.session_id,
                                              file_name,
                                              record_type,
                                              req_msg->write_req.record_num,
                                              write_data,
                                              qmi_sim_internal_mmgsdi_response_callback,
                                              (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_write_record failed");
    /* return callback indication with mmgsdi error*/
    write_cnf.response_header.mmgsdi_status = mmgsdi_status;
    write_cnf.response_header.response_type = MMGSDI_WRITE_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    write_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    write_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_write_record_ind(local_client_ref_ptr,
                                               &write_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_write_record_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_READ_CACHE_HANDLER()

  DESCRIPTION
    Function called to session read cache

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_cache_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_read_cache_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_read_cache_resp_msg_v01   resp;
  mmgsdi_file_enum_type                                 file_enum;
  mmgsdi_data_type                                      read_data;
  mmgsdi_read_cnf_type                                  read_cnf;

  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err          = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_read_cache_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_read_cache_resp_msg_v01));
  memset(&read_cnf, 0, sizeof(mmgsdi_read_cnf_type));
  memset(&read_data, 0, sizeof(mmgsdi_data_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_read_cache_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_read_cache_req_msg_v01*)req_c_struct;

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->read_req.file_num,
                                                               &file_enum);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->read_req.session_id);

  read_data.data_len = req_msg->read_data_len;
  read_data.data_ptr = uimqmi_malloc((sizeof(uint8) * read_data.data_len));
  if(read_data.data_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  memset(read_data.data_ptr, 0, (sizeof(uint8)* read_data.data_len));

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_read_cache(req_msg->read_req.session_id,
                                            file_enum,
                                            read_data);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01 )
  {
    resp.read_data_valid = TRUE;
    resp.read_data.data_len = read_data.data_len;
    if(read_data.data_ptr != NULL)
    {
      (void)memscpy(resp.read_data.data,
            sizeof(resp.read_data.data),
                    read_data.data_ptr,
                    resp.read_data.data_len);
    }
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_RESP_V01,
                               &resp,
                               sizeof(resp));
  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  if(read_data.data_ptr)
  {
    uimqmi_free(read_data.data_ptr);
    read_data.data_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_read_cache_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_READ_CACHE_FILE_SIZE_HANDLER()

  DESCRIPTION
    Function called to session read cache file size

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_read_cache_file_size_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01   *req_msg       = NULL;
  sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01   resp;
  mmgsdi_file_enum_type                                           file_enum;
  mmgsdi_len_type                                                *file_size_ptr = NULL;
  mmgsdi_read_cnf_type                                            read_cnf;

  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err          = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_read_cache_file_size_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01));
  memset(&read_cnf, 0, sizeof(mmgsdi_read_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_read_cache_file_size_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01*)req_c_struct;

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->read_req.file_num,
                                                               &file_enum);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  file_size_ptr = uimqmi_malloc(sizeof(mmgsdi_len_type));
  if(file_size_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }
  memset(file_size_ptr, 0, sizeof(mmgsdi_len_type));
  UIM_MSG_HIGH_1("session id 0x%x", req_msg->read_req.session_id);

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_read_cache_file_size(req_msg->read_req.session_id,
                                                      file_enum,
                                                      file_size_ptr);

send_response:
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01 )
  {
    resp.file_size_valid = TRUE;
    if(file_size_ptr != NULL)
    {
      resp.file_size = (uint16)*file_size_ptr;
    }
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  /* Free of file_size_ptr */
  if(file_size_ptr)
  {
    uimqmi_free(file_size_ptr);
    file_size_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_read_cache_file_size_handler */



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_WRITE_CACHE_HANDLER()

  DESCRIPTION
    Function called to session write cache

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_write_cache_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_write_cache_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_write_cache_resp_msg_v01   resp;
  mmgsdi_file_enum_type                                  file_enum;
  mmgsdi_data_type                                       write_data;

  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_write_cache_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_write_cache_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_write_cache_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_write_cache_req_msg_v01*)req_c_struct;

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->write_req.file_num,
                                                               &file_enum);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  write_data.data_len = req_msg->write_data.data_len;
  write_data.data_ptr = req_msg->write_data.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->write_req.session_id);

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_write_cache(req_msg->write_req.session_id,
                                             file_enum,
                                             write_data);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_CACHE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_write_cache_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_HANDLER()

  DESCRIPTION
    Function called to session is service available

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_is_service_available_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_is_service_available_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                         *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_is_service_available_resp_msg_v01   resp;
  mmgsdi_service_enum_type                                        srv_type;
  mmgsdi_srv_available_cnf_type                                   srv_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_is_service_available_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01));
  memset(&srv_cnf, 0, sizeof(mmgsdi_srv_available_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_is_service_available_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_is_service_available_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_service(req_msg->srvc_req.srvc_type,
                                                                  &srv_type);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

    UIM_MSG_HIGH_1("session id 0x%x", req_msg->srvc_req.session_id);
  mmgsdi_status = mmgsdi_session_is_service_available(req_msg->srvc_req.session_id,
                                                      srv_type,
                                                      qmi_sim_internal_mmgsdi_response_callback,
                                                      (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    srv_cnf.response_header.mmgsdi_status = mmgsdi_status;
    srv_cnf.response_header.response_type = MMGSDI_SRV_AVAILABLE_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    srv_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    srv_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_is_service_available_ind(local_client_ref_ptr,
                                                   &srv_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_is_service_available_handler */



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_HANDLER()

  DESCRIPTION
    Function called to session get all capabilities

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                         *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01   resp;
  mmgsdi_session_app_capabilities_cnf_type                        app_capabilities_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01));
  memset(&app_capabilities_cnf, 0, sizeof(mmgsdi_session_app_capabilities_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                       (req_msg->req_data,
                        connection_handle,
                        QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ_V01,
                        0,
                        (req_msg->token_valid == 0) ? FALSE : TRUE,
                        req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);
  mmgsdi_status = mmgsdi_session_get_app_capabilities(req_msg->session_id,
                                                      qmi_sim_internal_mmgsdi_response_callback,
                                                      (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    app_capabilities_cnf.response_header.mmgsdi_status = mmgsdi_status;
    app_capabilities_cnf.response_header.response_type = MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    app_capabilities_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    app_capabilities_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_get_app_capabilities_ind(local_client_ref_ptr,
                                                   &app_capabilities_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_get_app_capabilities_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_TOGGLE_UIM_POWER_CONTROL_SYNC_HANDLER()

  DESCRIPTION
    Function called to session toggle uim power control sync

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_toggle_uim_power_control_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01   resp;
  boolean                                                                  in_TC   = TRUE;
  mmgsdi_in_tc_mode_enum_type                                              mode;
  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_toggle_uim_power_control_sync_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("toggle_uim_power_control_sync:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01*)req_c_struct;

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_tc_mode(req_msg->req_data.mode,
                                                                  &mode);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->req_data.session_id);

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_toggle_uim_power_control_sync(req_msg->req_data.session_id,
                                                               in_TC,
                                                               mode,
                                                               UIM_CALL_TYPE_NOT_APPLICABLE);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp.in_TC_valid         = TRUE;
  resp.in_TC               = in_TC;

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_TOGGLE_UIM_POWER_CONTROL_SYNC_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_toggle_uim_power_control_sync_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_INFO_SYNC_HANDLER()

  DESCRIPTION
    Function called to session get info sync

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_info_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_get_info_sync_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01   resp;
  mmgsdi_session_info_query_type                           query_info;
  mmgsdi_session_get_info_type                             get_info;
  mmgsdi_return_enum_type        mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                  resp_err          = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_get_info_sync_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01));
  memset(&get_info, 0, sizeof(mmgsdi_session_get_info_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_info_sync_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_get_info_sync_req_msg_v01*)req_c_struct;

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_session_info(req_msg->query_info,
                                                                       &query_info.query_type);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  if((req_msg->session_id_valid && req_msg->session_type_valid) ||
     (!req_msg->session_id_valid && !req_msg->session_type_valid))
  {
    UIM_MSG_ERR_0("Invalid input parmeters");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }
  else if(req_msg->session_id_valid)
  {
    query_info.query_key.session_id = req_msg->session_id;
  }
  else if(req_msg->session_type_valid)
  {
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_session_type(req_msg->session_type,
                                                                         &query_info.query_key.session_type);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_get_info_sync(query_info, &get_info);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01)
  {
    qmi_sim_internal_response_get_info_sync(&get_info, &resp);
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_INFO_SYNC_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_get_info_sync_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_HANDLER()

  DESCRIPTION
    Function called to session USIM authenticate

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_usim_authenticate_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                      *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01   resp;
  mmgsdi_autn_context_enum_type                                auth_context;
  mmgsdi_data_type                                             req_data;
  mmgsdi_usim_auth_cnf_type                                    auth_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_usim_authenticate_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01));
  memset(&auth_cnf, 0, sizeof(mmgsdi_usim_auth_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_usim_authenticate_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_auth_context(req_msg->auth_req_data.auth_context,
                                                                       &auth_context);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  req_data.data_len = req_msg->auth_req_data.auth_req.data_len;
  req_data.data_ptr = req_msg->auth_req_data.auth_req.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->auth_req_data.session_id);
  mmgsdi_status = mmgsdi_session_usim_authenticate(req_msg->auth_req_data.session_id,
                                                   auth_context,
                                                   req_data,
                                                   qmi_sim_internal_mmgsdi_response_callback,
                                                   (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    auth_cnf.response_header.mmgsdi_status = mmgsdi_status;
    auth_cnf.response_header.response_type = MMGSDI_USIM_AUTH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    auth_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    auth_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_usim_authenticate_ind(local_client_ref_ptr,
                                                &auth_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_usim_authenticate_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_HANDLER()

  DESCRIPTION
    Function called to session USIM authenticate

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_perso_ota_deperso_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                      *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01   resp;

  mmgsdi_perso_feature_enum_type                               perso_feature;
  mmgsdi_perso_client_data_type                                deperso_data;
  mmgsdi_session_perso_cnf_type                                perso_cnf;
  mmgsdi_return_enum_type                                      mmgsdi_status = MMGSDI_SUCCESS;
  qmi_csi_error                                                resp_err      = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_usim_authenticate_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01));
  memset(&deperso_data, 0, sizeof(mmgsdi_perso_client_data_type));
  memset(&perso_cnf, 0, sizeof(mmgsdi_session_perso_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_usim_authenticate_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_perso_feature(req_msg->ota_deperso_req.feature,
                                                                        &perso_feature);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  if(req_msg->ota_deperso_req.deperso_data.data_len > 0)
  {
    deperso_data.data_len = req_msg->ota_deperso_req.deperso_data.data_len;
    deperso_data.data_ptr = uimqmi_malloc(sizeof(uint8) * deperso_data.data_len);
  }
  if(deperso_data.data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Can't allocate memory");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }
  memset(deperso_data.data_ptr, 0, sizeof(uint8) * deperso_data.data_len);
  (void)memscpy(deperso_data.data_ptr,
                sizeof(uint8) * deperso_data.data_len,
                req_msg->ota_deperso_req.deperso_data.data,
                (sizeof(uint8) * deperso_data.data_len));

  mmgsdi_status = mmgsdi_session_perso_ota_deperso(req_msg->ota_deperso_req.session_id,
                                                   perso_feature,
                                                   &deperso_data,
                                                   qmi_sim_internal_mmgsdi_response_callback,
                                                   (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("perso_ota_deperso failed");
    /* return callback indication with mmgsdi error*/
    perso_cnf.response_header.mmgsdi_status = mmgsdi_status;
    perso_cnf.response_header.response_type = MMGSDI_SESSION_PERSO_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    perso_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    perso_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_session_perso_ota_deperso_ind(local_client_ref_ptr,
                                                        &perso_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  /* Free of search pattern data */
  if(deperso_data.data_ptr)
  {
    uimqmi_free(deperso_data.data_ptr);
    deperso_data.data_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_perso_ota_deperso_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_HANDLER()

  DESCRIPTION
    Function called to session run GSM algo

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                 *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01   resp;
  mmgsdi_data_type                                        req_data;
  mmgsdi_session_run_gsm_algo_cnf_type                    run_gsm_algo_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01));
  memset(&run_gsm_algo_cnf, 0, sizeof(mmgsdi_session_run_gsm_algo_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                         (req_msg->req_data,
                          connection_handle,
                          QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_REQ_V01,
                          0,
                          (req_msg->token_valid == 0) ? FALSE : TRUE,
                          req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  req_data.data_len = req_msg->rand_data.data_len;
  req_data.data_ptr = req_msg->rand_data.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);
  mmgsdi_status = mmgsdi_session_run_gsm_algo(req_msg->session_id,
                                              req_data,
                                              qmi_sim_internal_mmgsdi_response_callback,
                                              (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    run_gsm_algo_cnf.response_header.mmgsdi_status = mmgsdi_status;
    run_gsm_algo_cnf.response_header.response_type = MMGSDI_SESSION_RUN_GSM_ALGO_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    run_gsm_algo_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    run_gsm_algo_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_run_gsm_algo_ind(local_client_ref_ptr,
                                           &run_gsm_algo_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_run_gsm_algo_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_HANDLER()

  DESCRIPTION
    Function called to session register for refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                         *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01   resp;
  mmgsdi_refresh_file_list_type                                   refresh_files;
  mmgsdi_refresh_cnf_type                                         refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;
  uint8                      index             = 0;


  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_register_for_refresh_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));
  memset(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_register_for_refresh_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  refresh_files.num_files = req_msg->refresh_req.refresh_files.file_list_len;
  refresh_files.file_list_ptr = NULL;
  if(refresh_files.num_files > 0)
  {
    refresh_files.file_list_ptr = (mmgsdi_file_enum_type*)
                                  uimqmi_malloc((sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);
    if (refresh_files.file_list_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
    memset(refresh_files.file_list_ptr, 0, (sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);

    for(index=0; index<refresh_files.num_files; index++)
    {

      resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->refresh_req.refresh_files.file_list[index],
                                                                   &(refresh_files.file_list_ptr[index]));
      if (resp_err != QMI_CSI_NO_ERR)
      {
        UIM_MSG_ERR_1("unknown file enum: 0x%x", resp_err);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        goto send_response;
      }
    }
  }

  refresh_files.file_path_len = req_msg->refresh_req.refresh_files.file_path_len;
  refresh_files.file_path_ptr = NULL;
  if(refresh_files.file_path_len > 0)
  {
    refresh_files.file_path_ptr = req_msg->refresh_req.refresh_files.file_path;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->refresh_req.session_id);
  mmgsdi_status = mmgsdi_session_register_for_refresh(req_msg->refresh_req.session_id,
                                                      refresh_files,
                                                      req_msg->refresh_req.vote_for_init,
                                                      qmi_sim_internal_mmgsdi_response_callback,
                                                      (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Free the file list ptr */
  if(refresh_files.file_list_ptr)
  {
    uimqmi_free(refresh_files.file_list_ptr);
    refresh_files.file_list_ptr = NULL;
  }

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_register_for_refresh_ind(local_client_ref_ptr,
                                                   &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_register_for_refresh_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_HANDLER()

  DESCRIPTION
    Function called to session ok to proceed voting with refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01
                                                                       *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                               *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01
                                                                        resp;
  mmgsdi_refresh_file_list_type                                         refresh_files;
  mmgsdi_refresh_cnf_type                                               refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;
  uint8                      index             = 0;


  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));
  memset(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("register_for_refresh_ok_to_proceed_voting_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  refresh_files.num_files = req_msg->refresh_req.refresh_files.file_list_len;
  refresh_files.file_list_ptr = NULL;
  if(refresh_files.num_files > 0)
  {
    refresh_files.file_list_ptr = (mmgsdi_file_enum_type*)
                                  uimqmi_malloc((sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);
    if (refresh_files.file_list_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
    memset(refresh_files.file_list_ptr, 0, (sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);

    for(index=0; index<refresh_files.num_files; index++)
    {

      resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->refresh_req.refresh_files.file_list[index],
                                                                   &(refresh_files.file_list_ptr[index]));
      if (resp_err != QMI_CSI_NO_ERR)
      {
        UIM_MSG_ERR_1("unknown file enum: 0x%x", resp_err);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        goto send_response;
      }
    }
  }

  refresh_files.file_path_len = req_msg->refresh_req.refresh_files.file_path_len;
  refresh_files.file_path_ptr = NULL;
  if(refresh_files.file_path_len > 0)
  {
    refresh_files.file_path_ptr = req_msg->refresh_req.refresh_files.file_path;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->refresh_req.session_id);
  mmgsdi_status = mmgsdi_session_register_for_refresh_ok_to_proceed_voting(req_msg->refresh_req.session_id,
                                                                           refresh_files,
                                                                           req_msg->refresh_req.vote_for_init,
                                                                           req_msg->proceed_with_refresh,
                                                                           qmi_sim_internal_mmgsdi_response_callback,
                                                                           (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Free the file list ptr */
  if(refresh_files.file_list_ptr)
  {
    uimqmi_free(refresh_files.file_list_ptr);
    refresh_files.file_list_ptr = NULL;
  }

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_RESP_V01,
                               &resp,
                               sizeof(resp));
  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_register_for_refresh_ok_to_proceed_voting_ind(local_client_ref_ptr,
                                                                        &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_HANDLER()

  DESCRIPTION
    Function called to session ok to refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01                *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                               *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01                resp;
  mmgsdi_refresh_cnf_type                                               refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->refresh_req.session_id);
  mmgsdi_status = mmgsdi_session_ok_to_refresh(req_msg->refresh_req.session_id,
                                               req_msg->refresh_req.ok_to_refresh,
                                               qmi_sim_internal_mmgsdi_response_callback,
                                               (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_RESP_V01,
                               &resp,
                               sizeof(resp));
  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_ok_to_refresh_ind(local_client_ref_ptr,
                                            &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_ok_to_refresh_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_HANDLER()

  DESCRIPTION
    Function called to session refresh complete

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_refresh_complete_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_refresh_complete_req_msg_v01             *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                               *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01             resp;
  mmgsdi_refresh_cnf_type                                               refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_refresh_complete_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_refresh_complete_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_refresh_complete_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }


  UIM_MSG_HIGH_1("session id 0x%x", req_msg->refresh_req.session_id);
  mmgsdi_status = mmgsdi_session_refresh_complete(req_msg->refresh_req.session_id,
                                                  req_msg->refresh_req.pass_fail,
                                                  qmi_sim_internal_mmgsdi_response_callback,
                                                  (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_refresh_complete_ind(local_client_ref_ptr,
                                            &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_refresh_complete_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_HANDLER()

  DESCRIPTION
    Function called to session ok to proceed with refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                               *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01   resp;
  mmgsdi_refresh_cnf_type                                               refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_register_for_refresh_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->refresh_req.session_id);
  mmgsdi_status = mmgsdi_session_ok_to_proceed_with_refresh(req_msg->refresh_req.session_id,
                                                            req_msg->refresh_req.ok_to_proceed,
                                                            qmi_sim_internal_mmgsdi_response_callback,
                                                            (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_ok_to_proceed_with_refresh_ind(local_client_ref_ptr,
                                                         &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_HANDLER()

  DESCRIPTION
    Function called to session deregister for refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                           *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01   resp;
  mmgsdi_refresh_file_list_type                                     refresh_files;
  mmgsdi_refresh_cnf_type                                           refresh_cnf;

  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error              resp_err          = QMI_CSI_NO_ERR;
  uint8                      index             = 0;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));
  memset(&refresh_files, 0, sizeof(mmgsdi_refresh_file_list_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                        (req_msg->req_data,
                         connection_handle,
                         QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_REQ_V01,
                         0,
                         (req_msg->token_valid == 0) ? FALSE : TRUE,
                         req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  refresh_files.num_files = req_msg->deregister_req.refresh_files.file_list_len;
  refresh_files.file_list_ptr = NULL;
  if(refresh_files.num_files > 0)
  {
    refresh_files.file_list_ptr = (mmgsdi_file_enum_type*)
                                  uimqmi_malloc((sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);
    if (refresh_files.file_list_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
    memset(refresh_files.file_list_ptr, 0, (sizeof(mmgsdi_file_enum_type)) * refresh_files.num_files);

    for(index=0; index<refresh_files.num_files; index++)
    {

      resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->deregister_req.refresh_files.file_list[index],
                                                                   &(refresh_files.file_list_ptr[index]));
      if (resp_err != QMI_CSI_NO_ERR)
      {
        UIM_MSG_ERR_1("unknown file enum: 0x%x", resp_err);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        goto send_response;
      }
    }
  }

  refresh_files.file_path_len = req_msg->deregister_req.refresh_files.file_path_len;
  refresh_files.file_path_ptr = NULL;
  if(refresh_files.file_path_len > 0)
  {
    refresh_files.file_path_ptr = req_msg->deregister_req.refresh_files.file_path;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->deregister_req.session_id);
  mmgsdi_status = mmgsdi_session_deregister_for_refresh(req_msg->deregister_req.session_id,
                                                      refresh_files,
                                                      req_msg->deregister_req.vote_for_init,
                                                      qmi_sim_internal_mmgsdi_response_callback,
                                                      (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Free the file list ptr */
  if(refresh_files.file_list_ptr)
  {
    uimqmi_free(refresh_files.file_list_ptr);
    refresh_files.file_list_ptr = NULL;
  }

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("register for refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_register_for_refresh_ind(local_client_ref_ptr,
                                                   &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_deregister_for_refresh_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_HANDLER()

  DESCRIPTION
    Function called to client id and event deregistration

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_client_id_and_evt_dereg_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01   resp;
  mmgsdi_client_id_and_evt_dereg_cnf_type                    client_id_and_evt_dereg_cnf;
  sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                    *local_client_ref_ptr = NULL;
  mmgsdi_return_enum_type                                    mmgsdi_status        = MMGSDI_SUCCESS;
  qmi_csi_error                                              resp_err             = QMI_CSI_NO_ERR;
  uint8                                                      index                = QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_client_id_and_evt_dereg_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01));
  memset(&client_id_and_evt_dereg_cnf, 0, sizeof(client_id_and_evt_dereg_cnf));

  if (!connection_handle||!req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_mmgsdi_client_id_and_evt_dereg_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  /* clear event handle */
  index = qmi_sim_internal_client_session_id_to_index(0, req_msg->client_id);
  qmi_sim_internal_clear_event_handle_entry(index);

  /* Get a MMGSDI client id */
  mmgsdi_status = mmgsdi_client_id_and_evt_dereg( req_msg->client_id,
                                                  qmi_sim_internal_mmgsdi_response_callback,
                                                  (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_and_evt_reg failed");
    /* return callback indication with mmgsdi error*/
    client_id_and_evt_dereg_cnf.response_header.mmgsdi_status = mmgsdi_status;
    client_id_and_evt_dereg_cnf.response_header.response_type = MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF;

    client_id_and_evt_dereg_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    client_id_and_evt_dereg_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_client_id_and_evt_dereg_ind(local_client_ref_ptr,
                                                    &client_id_and_evt_dereg_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_client_id_and_evt_dereg_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_HANDLER()

  DESCRIPTION
    Function called to search

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_search_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_search_resp_msg_v01   resp;
  mmgsdi_search_cnf_type                            search_cnf;
  sim_internal_mmgsdi_session_search_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type           *local_client_ref_ptr = NULL;
  mmgsdi_access_type                                file_access;
  mmgsdi_search_enum_type                           search_type          = MMGSDI_SEARCH_NONE;
  mmgsdi_search_direction_enum_type                 search_direction     = MMGSDI_SEARCH_DIRECTION_NONE;
  mmgsdi_search_offset_data_type                    offset_data;
  mmgsdi_data_type                                  search_pattern;
  mmgsdi_return_enum_type                           mmgsdi_status        = MMGSDI_SUCCESS;
  qmi_csi_error                                     resp_err             = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_search_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_search_resp_msg_v01));
  memset(&search_cnf, 0, sizeof(mmgsdi_search_cnf_type));
  memset(&file_access, 0, sizeof(mmgsdi_access_type));
  memset(&offset_data, 0, sizeof(mmgsdi_search_offset_data_type));
  memset(&search_pattern, 0, sizeof(mmgsdi_data_type));

  if (!connection_handle||!req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_search_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_search_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_access.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_access.access_method = MMGSDI_BY_PATH_ACCESS;
    file_access.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_access.file.path_type.path_buf,
                  sizeof(file_access.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_access.file.path_type.path_len));
    UIM_MSG_HIGH_3("path len 0x%x: [0] 0x%x, [1] 0x%x ",
                   req_msg->path_type_file.path_buf_len,
                   req_msg->path_type_file.path_buf[0],
                   req_msg->path_type_file.path_buf[1]);

    UIM_MSG_HIGH_3("[2] 0x%x, [3] 0x%x, [4] 0x%x ",
                   req_msg->path_type_file.path_buf[2],
                   req_msg->path_type_file.path_buf[3],
                   req_msg->path_type_file.path_buf[4]);
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_search_type(req_msg->search_req.search_type, &search_type);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to convert search_type: 0x%x",
                  req_msg->search_req.search_type);
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_search_direction(req_msg->search_req.search_direction,
                                                              &search_direction);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to convert search_direction: 0x%x",
                  req_msg->search_req.search_direction);
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_search_option(req_msg->search_req.enhanced_search_offset_data.offset_type,
                                                           &offset_data.offset_type);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to convert search_option: 0x%x",
                  req_msg->search_req.search_direction);
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  offset_data.offset_data = req_msg->search_req.enhanced_search_offset_data.offset_data;

  search_pattern.data_len = req_msg->search_req.search_pattern.data_len;
  search_pattern.data_ptr = uimqmi_malloc((sizeof(uint8) * search_pattern.data_len));
  if(search_pattern.data_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }
  memset(search_pattern.data_ptr, 0, (sizeof(uint8)* search_pattern.data_len));
  (void)memscpy(search_pattern.data_ptr,
                (sizeof(uint8) * search_pattern.data_len),
                req_msg->search_req.search_pattern.data,
                search_pattern.data_len);

  mmgsdi_status = mmgsdi_session_search( req_msg->search_req.session_id,
                                         file_access,
                                         search_type,
                                         req_msg->search_req.search_rec_num,
                                         search_direction,
                                         offset_data,
                                         search_pattern,
                                         qmi_sim_internal_mmgsdi_response_callback,
                                         (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_search failed");
    /* return callback indication with mmgsdi error*/
    search_cnf.response_header.mmgsdi_status = mmgsdi_status;
    search_cnf.response_header.response_type = MMGSDI_SEARCH_CNF;

    search_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    search_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_search_ind(local_client_ref_ptr,
                                     &search_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }
  /* Free of search pattern data */
  if(search_pattern.data_ptr)
  {
    uimqmi_free(search_pattern.data_ptr);
    search_pattern.data_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_search_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_SEEK_HANDLER()

  DESCRIPTION
    Function called to search

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_seek_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_seek_resp_msg_v01   resp;
  mmgsdi_search_cnf_type                          search_cnf;
  sim_internal_mmgsdi_session_seek_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type         *local_client_ref_ptr = NULL;
  mmgsdi_access_type                              file_access;
  mmgsdi_seek_direction_enum_type                 seek_direction     = MMGSDI_SEARCH_DIRECTION_NONE;
  mmgsdi_data_type                                seek_pattern;
  mmgsdi_return_enum_type                         mmgsdi_status        = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err             = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_seek_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_seek_resp_msg_v01));
  memset(&search_cnf, 0, sizeof(mmgsdi_search_cnf_type));
  memset(&file_access, 0, sizeof(mmgsdi_access_type));
  memset(&seek_pattern, 0, sizeof(mmgsdi_data_type));

  if (!connection_handle||!req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_seek_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_seek_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_access.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_access.access_method = MMGSDI_BY_PATH_ACCESS;
    file_access.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_access.file.path_type.path_buf,
                  sizeof(file_access.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_access.file.path_type.path_len));
    UIM_MSG_HIGH_3("path len 0x%x: [0] 0x%x, [1] 0x%x ",
                   req_msg->path_type_file.path_buf_len,
                   req_msg->path_type_file.path_buf[0],
                   req_msg->path_type_file.path_buf[1]);

    UIM_MSG_HIGH_3("[2] 0x%x, [3] 0x%x, [4] 0x%x ",
                   req_msg->path_type_file.path_buf[2],
                   req_msg->path_type_file.path_buf[3],
                   req_msg->path_type_file.path_buf[4]);
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_seek_direction(req_msg->seek_req.seek_direction,
                                                              &seek_direction);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to convert seek_direction: 0x%x",
                  req_msg->seek_req.seek_direction);
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  seek_pattern.data_len = req_msg->seek_req.seek_pattern.data_len;
  seek_pattern.data_ptr = uimqmi_malloc((sizeof(uint8) * seek_pattern.data_len));
  if(seek_pattern.data_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }
  memset(seek_pattern.data_ptr, 0, (sizeof(uint8)* seek_pattern.data_len));
  (void)memscpy(seek_pattern.data_ptr,
                sizeof(uint8) * seek_pattern.data_len,
                req_msg->seek_req.seek_pattern.data,
                seek_pattern.data_len);

  mmgsdi_status = mmgsdi_session_seek( req_msg->seek_req.session_id,
                                       file_access,
                                       seek_direction,
                                       seek_pattern,
                                       qmi_sim_internal_mmgsdi_response_callback,
                                       (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_SEEK_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_seek failed");
    /* return callback indication with mmgsdi error*/
    search_cnf.response_header.mmgsdi_status = mmgsdi_status;
    search_cnf.response_header.response_type = MMGSDI_SEARCH_CNF;

    search_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    search_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_search_ind(local_client_ref_ptr,
                                     &search_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }
  /* Free of seek pattern data */
  if(seek_pattern.data_ptr)
  {
    uimqmi_free(seek_pattern.data_ptr);
    seek_pattern.data_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_seek_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_HANDLER()

  DESCRIPTION
    Function called to register for file update

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_register_for_file_update_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01   resp;
  mmgsdi_session_reg_file_update_cnf_type                             update_cnf;
  sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01   *req_msg = NULL;
  mmgsdi_file_update_list_type                    file_update_list;
  qmi_sim_internal_local_client_ref_type         *local_client_ref_ptr = NULL;
  mmgsdi_return_enum_type                         mmgsdi_status        = MMGSDI_SUCCESS;
  qmi_csi_error                                   resp_err             = QMI_CSI_NO_ERR;
  uint8                                           i                    = 0;
  uint8                                           offset               = 0;

  UIM_MSG_HIGH_0("register_for_file_update_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01));
  memset(&update_cnf, 0, sizeof(mmgsdi_session_reg_file_update_cnf_type));
  memset(&file_update_list, 0, sizeof(mmgsdi_file_update_list_type));

  if (!connection_handle||!req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("register_for_file_update_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                    (req_msg->req_data,
                                     connection_handle,
                                     QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_REQ_V01,
                                     0,
                                     (req_msg->token_valid == 0) ? FALSE : TRUE,
                                     req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_update_list.num_files += req_msg->file_enum_file_len;
  }

  if(req_msg->path_type_file_valid)
  {
    file_update_list.num_files += req_msg->path_type_file_len;
  }

  if(file_update_list.num_files > 0)
  {
    file_update_list.file_list_ptr = uimqmi_malloc((sizeof(mmgsdi_access_type) * file_update_list.num_files));
    if(file_update_list.file_list_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      goto send_response;
    }
    memset(file_update_list.file_list_ptr, 0, (sizeof(mmgsdi_access_type) * file_update_list.num_files));
  }

  if(req_msg->file_enum_file_valid &&
     file_update_list.file_list_ptr != NULL)
  {
    for(i=0; i < req_msg->file_enum_file_len; i++)
    {
      file_update_list.file_list_ptr[i].access_method = MMGSDI_EF_ENUM_ACCESS;
      resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file[i],
                                                                   &file_update_list.file_list_ptr[i].file.file_enum);
      if (resp_err != QMI_CSI_NO_ERR)
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        goto send_response;
      }
    }
  }
  offset = req_msg->file_enum_file_len;
  if(req_msg->path_type_file_valid &&
     file_update_list.file_list_ptr != NULL)
  {
    for(i=0; i < req_msg->path_type_file_len; i++)
    {
      file_update_list.file_list_ptr[i+offset].access_method = MMGSDI_BY_PATH_ACCESS;
      file_update_list.file_list_ptr[i+offset].file.path_type.path_len = req_msg->path_type_file[i].path_buf_len;
      (void)memscpy(file_update_list.file_list_ptr[i+offset].file.path_type.path_buf,
                    sizeof(file_update_list.file_list_ptr[i+offset].file.path_type.path_buf),
                    req_msg->path_type_file[i].path_buf,
                    (sizeof(uint16) * req_msg->path_type_file[i].path_buf_len));
    }
  }

  mmgsdi_status = mmgsdi_session_register_for_file_update( req_msg->session_id,
                                                           &file_update_list,
                                                           qmi_sim_internal_mmgsdi_response_callback,
                                                           (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_seek failed");
    /* return callback indication with mmgsdi error*/
    update_cnf.response_header.mmgsdi_status = mmgsdi_status;
    update_cnf.response_header.response_type = MMGSDI_SESSION_REGISTER_FILE_UPDATE_CNF;

    update_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    update_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_register_for_file_update_ind(local_client_ref_ptr,
                                     &update_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }
  /* Free of file_update_list.file_list_ptr */
  if(file_update_list.file_list_ptr)
  {
    uimqmi_free(file_update_list.file_list_ptr);
    file_update_list.file_list_ptr = NULL;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_register_for_file_update_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_GET_PROTOCOL_HANDLER()

  DESCRIPTION
    Function called to get protocol

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_get_protocol_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_get_protocol_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_get_protocol_resp_msg_v01   resp;

  mmgsdi_slot_id_enum_type     slot_id            = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_protocol_enum_type    protocol           = MMGSDI_NO_PROTOCOL;
  mmgsdi_return_enum_type      mmgsdi_status      = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err           = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_get_protocol_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_get_protocol_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_get_protocol_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_get_protocol_req_msg_v01*)req_c_struct;
  resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_slot_id(req_msg->slot,
                                                                  &slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert slot id ");
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  mmgsdi_status = mmgsdi_get_protocol(req_msg->client_id,
                                      slot_id,
                                      &protocol);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01 )
  {
    resp.protocol_valid = TRUE;
    resp_err = qmi_sim_internal_convert_mmgsdi_protocol(protocol, &resp.protocol);
    if(resp_err != QMI_CSI_NO_ERR)
    {
      UIM_MSG_ERR_0("failed to convert protocol ");
      resp.protocol_valid = FALSE;
    }
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_GET_PROTOCOL_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return resp_err;
} /* qmi_sim_internal_mmgsdi_get_protocol_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_HANDLER()

  DESCRIPTION
    Function called to get app capabilities (sync)

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01   resp;
  mmgsdi_session_app_capabilities_cnf_type                             app_capabilities;

  mmgsdi_return_enum_type      mmgsdi_status      = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err           = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01));
  memset(&app_capabilities, 0, sizeof(mmgsdi_session_app_capabilities_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01*)req_c_struct;

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_get_app_capabilities_sync(req_msg->session_id, &app_capabilities);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if(resp.resp.result == QMI_RESULT_SUCCESS_V01 )
  {
    resp.app_capabilities_valid = TRUE;
    resp.app_capabilities.acl_enabled = app_capabilities.acl_enabled;
    resp.app_capabilities.bdn_enabled = app_capabilities.bdn_enabled;
    resp.app_capabilities.fdn_enabled = app_capabilities.fdn_enabled;
    resp.app_capabilities.imsi_invalidated = app_capabilities.imsi_invalidated;
  }

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return resp_err;
} /* qmi_sim_internal_mmgsdi_session_get_app_capabilities_sync_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_HANDLER()

  DESCRIPTION
    Function called to illegal subscription (sync)

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01   *req_msg = NULL;
  sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01   resp;

  mmgsdi_return_enum_type      mmgsdi_status      = MMGSDI_SUCCESS;
  qmi_csi_error                resp_err           = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01*)req_c_struct;

  qmi_sim_internal_state.mmgsdi_sync_call = TRUE;
  mmgsdi_status = mmgsdi_session_illegal_subscription_sync(req_msg->session_id);
  qmi_sim_internal_state.mmgsdi_sync_call = FALSE;

  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  return resp_err;
} /* qmi_sim_internal_mmgsdi_session_illegal_subscription_sync_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_INCREASE_HANDLER()

  DESCRIPTION
    Function called to session increase

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_increase_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_increase_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type             *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_increase_resp_msg_v01   resp;
  mmgsdi_access_type                                  file_name;
  mmgsdi_data_type                                    increase_data;
  mmgsdi_increase_cnf_type                            increase_cnf;

  mmgsdi_return_enum_type                             mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                       resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_increase_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_increase_resp_msg_v01));
  memset(&increase_cnf, 0, sizeof(mmgsdi_increase_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_increase_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_increase_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                  (req_msg->req_data,
                                   connection_handle,
                                   QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_REQ_V01,
                                   0,
                                   (req_msg->token_valid == 0) ? FALSE : TRUE,
                                   req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid && req_msg->path_type_file_valid)
  {
    UIM_MSG_ERR_0("Received multiple access method");
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    goto send_response;
  }

  if(req_msg->file_enum_file_valid)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    resp_err = qmi_sim_internal_convert_sim_internal_mmgsdi_file(req_msg->file_enum_file,
                                                                 &file_name.file.file_enum);
    if (resp_err != QMI_CSI_NO_ERR)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      goto send_response;
    }
  }
  else if(req_msg->path_type_file_valid)
  {
    file_name.access_method = MMGSDI_BY_PATH_ACCESS;
    file_name.file.path_type.path_len = req_msg->path_type_file.path_buf_len;
    (void)memscpy(file_name.file.path_type.path_buf,
                  sizeof(file_name.file.path_type.path_buf),
                  req_msg->path_type_file.path_buf,
                  (sizeof(uint16) * file_name.file.path_type.path_len));
  }
  else
  {
    UIM_MSG_ERR_0("access method not provided");
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  increase_data.data_len = req_msg->increase_req_data.increase_data.data_len;
  increase_data.data_ptr = req_msg->increase_req_data.increase_data.data;

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->increase_req_data.session_id);
  mmgsdi_status = mmgsdi_session_increase(req_msg->increase_req_data.session_id,
                                          file_name,
                                          increase_data,
                                          qmi_sim_internal_mmgsdi_response_callback,
                                          (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_increase failed");
    /* return callback indication with mmgsdi error*/
    increase_cnf.response_header.mmgsdi_status = mmgsdi_status;
    increase_cnf.response_header.response_type = MMGSDI_INCREASE_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    increase_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    increase_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_increase_ind(local_client_ref_ptr,
                                       &increase_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_increase_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_HANDLER()

  DESCRIPTION
    Function called to session get all pin status

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type                       *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01   resp;
  mmgsdi_get_all_pin_status_cnf_type                            get_all_pin_status_cnf;

  mmgsdi_return_enum_type                             mmgsdi_status     = MMGSDI_SUCCESS;
  qmi_csi_error                                       resp_err          = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01));
  memset(&get_all_pin_status_cnf, 0, sizeof(mmgsdi_get_all_pin_status_cnf_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                  (req_msg->req_data,
                                   connection_handle,
                                   QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_REQ_V01,
                                   0,
                                   (req_msg->token_valid == 0) ? FALSE : TRUE,
                                   req_msg->token);
  if (resp_err != QMI_CSI_NO_ERR || local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  UIM_MSG_HIGH_1("session id 0x%x", req_msg->session_id);
  mmgsdi_status = mmgsdi_session_get_all_pin_status(req_msg->session_id,
                                          qmi_sim_internal_mmgsdi_response_callback,
                                          (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_session_get_all_pin_status failed");
    /* return callback indication with mmgsdi error*/
    get_all_pin_status_cnf.response_header.mmgsdi_status = mmgsdi_status;
    get_all_pin_status_cnf.response_header.response_type = MMGSDI_GET_ALL_PIN_STATUS_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    get_all_pin_status_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    get_all_pin_status_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_get_all_pin_status_ind(local_client_ref_ptr,
                                                 &get_all_pin_status_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_session_get_all_pin_status_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_HANDLER()

  DESCRIPTION
    Function called to handle NAA Refresh

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_mmgsdi_naa_refresh_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_mmgsdi_naa_refresh_req_msg_v01   *req_msg              = NULL;
  qmi_sim_internal_local_client_ref_type        *local_client_ref_ptr = NULL;
  sim_internal_mmgsdi_naa_refresh_resp_msg_v01   resp;

  mmgsdi_refresh_cnf_type       refresh_cnf;
  mmgsdi_return_enum_type       mmgsdi_status       = MMGSDI_SUCCESS;
  qmi_csi_error                 resp_err            = QMI_CSI_NO_ERR;
  mmgsdi_option_type            option;
  mmgsdi_aid_type               aid;
  mmgsdi_data_type              file_list;
  mmgsdi_refresh_mode_enum_type refresh_mode;
  mmgsdi_slot_id_enum_type      slot_id;

  UIM_MSG_HIGH_0("qmi_sim_internal_mmgsdi_naa_refresh_handler");

  memset(&resp, 0, sizeof(sim_internal_mmgsdi_naa_refresh_resp_msg_v01));
  memset(&refresh_cnf, 0, sizeof(mmgsdi_refresh_cnf_type));
  memset(&option, 0, sizeof(mmgsdi_option_type));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_mmgsdi_naa_refresh_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_mmgsdi_naa_refresh_req_msg_v01*)req_c_struct;

  /* store remote resp cb info and qcsi client handle for indication*/
  local_client_ref_ptr = qmi_sim_internal_store_remote_cb_handle
                                  (req_msg->req_data,
                                   connection_handle,
                                   QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_REQ_V01,
                                   0,
                                   (req_msg->token_valid == 0) ? FALSE : TRUE,
                                   req_msg->token);
  if (local_client_ref_ptr == NULL)
  {
    UIM_MSG_ERR_1("unable to store remote cb handle: 0x%x", resp_err);
    mmgsdi_status = MMGSDI_ERROR;
    goto send_response;
  }

  if (qmi_sim_internal_convert_sim_internal_mmgsdi_slot_id(
        req_msg->refresh_req.slot,
        &slot_id) != QMI_CSI_NO_ERR)
  {
    goto send_response;
  }

  if (qmi_sim_internal_convert_sim_internal_mmgsdi_refresh_mode(
        req_msg->refresh_req.mode,
        &refresh_mode) != QMI_CSI_NO_ERR)
  {
    goto send_response;
  }

  qmi_sim_internal_populate_sim_internal_mmgsdi_aid_type(&req_msg->refresh_req.aid,
                                                         &aid);

  file_list.data_len = req_msg->refresh_req.file_list.data_len;
  file_list.data_ptr = (uint8 *)req_msg->refresh_req.file_list.data;

  mmgsdi_status = mmgsdi_naa_refresh(req_msg->refresh_req.client_id,
                                     slot_id,
                                     aid,
                                     refresh_mode,
                                     file_list,
                                     req_msg->refresh_req.num_files,
                                     NULL,
                                     option,
                                     qmi_sim_internal_mmgsdi_response_callback,
                                     (mmgsdi_client_data_type)local_client_ref_ptr);

send_response:
  /* Encode MMGSDI status to QMI SIM INTERNAL */
  qmi_sim_internal_response_mmgsdi_status(mmgsdi_status, &resp.mmgsdi_status_valid, &resp.mmgsdi_status);

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_RESP_V01,
                               &resp,
                               sizeof(resp));

  UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);

  QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(
    req_msg->token_valid && (resp_err == QMI_CSI_NO_ERR),
    req_msg->token,
    resp.token_valid,
    resp.token);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_0("mmgsdi_naa_refresh failed");
    /* return callback indication with mmgsdi error*/
    refresh_cnf.response_header.mmgsdi_status = mmgsdi_status;
    refresh_cnf.response_header.response_type = MMGSDI_REFRESH_CNF;

    /* will client check rest of the CNF if mmgsdi_status != error none? */
    refresh_cnf.response_header.response_len  = sizeof(mmgsdi_response_header_type);
    refresh_cnf.response_header.payload_len   = sizeof(mmgsdi_response_header_type);

    qmi_sim_internal_send_naa_refresh_ind(local_client_ref_ptr,
                                          &refresh_cnf);
    /* Free of local_client_ref_ptr */
    if(local_client_ref_ptr)
    {
      uimqmi_free(local_client_ref_ptr);
      local_client_ref_ptr = NULL;
    }
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_mmgsdi_naa_refresh_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_GSTK_TAL_CLIENT_REGISTRATION_HANDLER()

  DESCRIPTION
    Function called to gstk tal client registration

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_gstk_tal_client_registration_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_gstk_tal_client_registration_req_msg_v01   *req_msg              = NULL;
  sim_internal_gstk_tal_client_registration_resp_msg_v01   resp;

  gstk_status_enum_type               gstk_status              = GSTK_SUCCESS;
  qmi_csi_error                       resp_err                 = QMI_CSI_NO_ERR;
  //gstk_tal_client_type                gstk_tal_client          = GSTK_TAL_REMOTE_CLIENT;


  UIM_MSG_HIGH_0("qmi_sim_internal_gstk_tal_client_registration_handler");

  memset(&resp, 0, sizeof(sim_internal_gstk_tal_client_registration_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_gstk_tal_client_registration_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_gstk_tal_client_registration_req_msg_v01*)req_c_struct;

  if (req_msg->token_valid)
  {
    qmi_sim_internal_add_token_to_txn_list(req_msg->token);
  }

  /* client type should be remote; local client should have call gstk_tal directly */
  /* resp_err = qmi_sim_internal_convert_sim_internal_gstk_client_type(req_msg->client_type,
                                                                    &gstk_tal_client);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unknown client_type enum: 0x%x", resp_err);
    return resp_err;
  } */

  if(req_msg->client_type != SIM_INTERNAL_GSTK_TAL_REMOTE_CLIENT_V01)
  {
    UIM_MSG_ERR_0("client type should be remote; local client should have call gstk_tal directly ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  /* store remote evt cb info and qcsi client handle for indication*/
  /* client type should be remote client*/
  resp_err = qmi_sim_internal_store_gstk_evt_cb_handle(connection_handle,
                                                       req_msg->gstk_tal_handle);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("unable to store evt cb handle: 0x%x", resp_err);
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  /* assume the support of remote client only in this case */
  gstk_status = gstk_tal_client_registration(GSTK_TAL_REMOTE_CLIENT,
                                             qmi_sim_internal_gstk_evt_callback,
                                             req_msg->tech_support_mask);

send_response:
  /* Convert GSTK status in QMI code */
  qmi_sim_internal_response_gstk_result_code(gstk_status, &resp.resp);
  qmi_sim_internal_response_gstk_status(gstk_status, &resp.gstk_status_valid, &resp.gstk_status);

  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    req_msg->token_valid,
    req_msg->token);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_GSTK_TAL_CLIENT_REGISTRATION_RESP_V01,
                               &resp,
                               sizeof(resp));

  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_HIGH_1("resp_err : 0x%x", resp_err);
    qmi_sim_internal_clear_gstk_event_handle_entry((uint8)req_msg->client_type);
    return resp_err;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_gstk_tal_client_registration_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_GSTK_TAL_SEND_CMD_HANDLER()

  DESCRIPTION
    Function called to gstk tal send command

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_gstk_tal_send_cmd_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_gstk_tal_send_cmd_req_msg_v01   *req_msg  = NULL;
  sim_internal_gstk_tal_send_cmd_resp_msg_v01    resp;

  gstk_tal_uim_cmd_type             * tal_uim_cmd              = NULL;
  gstk_status_enum_type               gstk_status              = GSTK_SUCCESS;
  qmi_csi_error                       resp_err                 = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_gstk_tal_send_cmd_handler");

  memset(&resp, 0, sizeof(sim_internal_gstk_tal_send_cmd_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_gstk_tal_send_cmd_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_gstk_tal_send_cmd_req_msg_v01*)req_c_struct;

  if (req_msg->token_valid)
  {
    qmi_sim_internal_add_token_to_txn_list(req_msg->token);
  }

  /* cmd type should be remote client*/
  if(req_msg->client_type != SIM_INTERNAL_GSTK_TAL_REMOTE_CLIENT_V01)
  {
    UIM_MSG_ERR_0("client type should be remote; local client should have call gstk_tal directly ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  tal_uim_cmd = uimqmi_malloc(sizeof(gstk_tal_uim_cmd_type));
  if (tal_uim_cmd == NULL)
  {
    gstk_status = GSTK_ERROR;
    goto send_response;
  }
  memset(tal_uim_cmd, 0, sizeof(gstk_tal_uim_cmd_type));
  tal_uim_cmd->user_data  = req_msg->tal_cmd.user_data;
  resp_err = qmi_sim_internal_convert_sim_internal_gstk_tal_cmd_type(req_msg->tal_cmd.cmd_type,
                                                                     &(tal_uim_cmd->cmd_type));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert gstk tal cmd type ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_sim_internal_gstk_slot_id(req_msg->tal_cmd.slot_id,
                                                                &(tal_uim_cmd->slot));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert gstk slot id ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  if(req_msg->tal_cmd.data_len > 0)
  {
    tal_uim_cmd->num_bytes  = req_msg->tal_cmd.data_len;
    tal_uim_cmd->data_ptr = uimqmi_malloc(sizeof(uint8)*tal_uim_cmd->num_bytes);
    if (tal_uim_cmd->data_ptr == NULL)
    {
      gstk_status = GSTK_ERROR;
      goto send_response;
    }
    (void)memscpy(tal_uim_cmd->data_ptr,
                  sizeof(uint8)*tal_uim_cmd->num_bytes,
                  req_msg->tal_cmd.data,
                  tal_uim_cmd->num_bytes);
  }

  /* support of remote client only */
  gstk_status = gstk_tal_send_cmd(GSTK_TAL_REMOTE_CLIENT,tal_uim_cmd);

send_response:
  /* Convert GSTK status in QMI code */
  qmi_sim_internal_response_gstk_result_code(gstk_status, &resp.resp);
  qmi_sim_internal_response_gstk_status(gstk_status, &resp.gstk_status_valid, &resp.gstk_status);

  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    req_msg->token_valid,
    req_msg->token);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_GSTK_TAL_SEND_CMD_RESP_V01,
                               &resp,
                               sizeof(resp));

  /* Free of tal_uim_cmd */
  if (tal_uim_cmd != NULL)
  {
    uimqmi_free(tal_uim_cmd->data_ptr);
    uimqmi_free(tal_uim_cmd);
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_gstk_tal_send_cmd_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_RAT_HANDLER()

  DESCRIPTION
    Function called to gstk tal notify RAT

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_gstk_tal_notify_rat_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_gstk_tal_notify_rat_req_msg_v01   *req_msg  = NULL;
  sim_internal_gstk_tal_notify_rat_resp_msg_v01   resp;

  gstk_access_technology_type         gstk_rat                 = GSTK_ACCESS_NONE;
  gstk_status_enum_type               gstk_status              = GSTK_SUCCESS;
  qmi_csi_error                       resp_err                 = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_gstk_tal_notify_rat_handler");

  memset(&resp, 0, sizeof(sim_internal_gstk_tal_notify_rat_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_gstk_tal_notify_rat_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_gstk_tal_notify_rat_req_msg_v01*)req_c_struct;

  if (req_msg->token_valid)
  {
    qmi_sim_internal_add_token_to_txn_list(req_msg->token);
  }

  /* cmd type should be remote client*/
  if(req_msg->client_type != SIM_INTERNAL_GSTK_TAL_REMOTE_CLIENT_V01)
  {
    UIM_MSG_ERR_0("client type should be remote; local client should have call gstk_tal directly ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_gstk_access_technology(req_msg->rat, &gstk_rat);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert gstk rat ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  /* support of remote client only */
  gstk_status = gstk_tal_notify_rat(GSTK_TAL_REMOTE_CLIENT, gstk_rat);

send_response:
  /* Convert GSTK status in QMI code */
  qmi_sim_internal_response_gstk_result_code(gstk_status, &resp.resp);
  qmi_sim_internal_response_gstk_status(gstk_status, &resp.gstk_status_valid, &resp.gstk_status);

  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    req_msg->token_valid,
    req_msg->token);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_RAT_RESP_V01,
                               &resp,
                               sizeof(resp));

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_gstk_tal_notify_rat_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_LS_HANDLER()

  DESCRIPTION
    Function called to gstk tal notify location status

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    req_c_struct
    req_c_struct_len

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_gstk_tal_notify_ls_handler
(
  qmi_sim_internal_client_info_type *connection_handle,
  qmi_req_handle                     req_handle,
  void                              *req_c_struct,
  int                                req_c_struct_len
)
{
  sim_internal_gstk_tal_notify_ls_req_msg_v01   *req_msg  = NULL;
  sim_internal_gstk_tal_notify_ls_resp_msg_v01   resp;

  gstk_location_status_enum_type      gstk_ls                  = GSTK_NO_SERVICE;
  gstk_status_enum_type               gstk_status              = GSTK_SUCCESS;
  qmi_csi_error                       resp_err                 = QMI_CSI_NO_ERR;


  UIM_MSG_HIGH_0("qmi_sim_internal_gstk_tal_notify_ls_handler");

  memset(&resp, 0, sizeof(sim_internal_gstk_tal_notify_ls_resp_msg_v01));

  if (!connection_handle|| !req_handle || !req_c_struct || !req_c_struct_len)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_gstk_tal_notify_ls_handler:Illegal parameters");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  req_msg = (sim_internal_gstk_tal_notify_ls_req_msg_v01*)req_c_struct;

  if (req_msg->token_valid)
  {
    qmi_sim_internal_add_token_to_txn_list(req_msg->token);
  }

  /* cmd type should be remote client*/
  if(req_msg->client_type != SIM_INTERNAL_GSTK_TAL_REMOTE_CLIENT_V01)
  {
    UIM_MSG_ERR_0("client type should be remote; local client should have call gstk_tal directly ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  resp_err = qmi_sim_internal_convert_gstk_location_status(req_msg->ls, &gstk_ls);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert gstk ls ");
    gstk_status = GSTK_ERROR;
    goto send_response;
  }

  /* support of remote client only */
  gstk_status = gstk_tal_notify_ls(GSTK_TAL_REMOTE_CLIENT,gstk_ls);

send_response:
  /* Convert GSTK status in QMI code */
  qmi_sim_internal_response_gstk_result_code(gstk_status, &resp.resp);
  qmi_sim_internal_response_gstk_status(gstk_status, &resp.gstk_status_valid, &resp.gstk_status);

  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    req_msg->token_valid,
    req_msg->token);

  if (req_msg->token_valid)
  {
    resp.token_valid = TRUE;
    resp.token       = req_msg->token;
  }

  resp_err = qmi_csi_send_resp(req_handle,
                               QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_LS_RESP_V01,
                               &resp,
                               sizeof(resp));

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_gstk_tal_notify_ls_handler */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_TEST_MCC_STATUS_IND()

  DESCRIPTION
    Function called to send status of test MCC.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_send_test_mcc_status_ind
(
  void
)
{
  sim_internal_mmgsdi_event_report_ind_msg_v01 *evt_ind_ptr           = NULL;
  qmi_sim_internal_evt_cb_client_ref_type      *evt_cb_client_ref_ptr = NULL;
  uint8                                         index                 = 0;
  qmi_csi_cb_error                              cb_err                = QMI_CSI_CB_NO_ERR;
  mmgsdi_cached_nv_item_data                    anite_nv_item;

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  for (index=0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
  {
    /* Use the handle of first client available. This indication is not a
       result of an MMGSDI event and hence it needs to be sent to the first
       available client with a NULL event callback. On the remote modem, this
       indication will only result in writing of an NV item by MMGSDI. */
    if (qmi_sim_internal_remote_evt_cb_table[index].in_use)
    {
      evt_cb_client_ref_ptr = &qmi_sim_internal_remote_evt_cb_table[index];
      break;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if ((index >= QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB) ||
      (evt_cb_client_ref_ptr == NULL) ||
      (evt_cb_client_ref_ptr->qcsi_client == NULL))
  {
    return;
  }

  anite_nv_item.nv_item_enum = MMGSDI_NV_GPRS_ANITE_GCF_I;
  if (mmgsdi_get_cached_nv_item(&anite_nv_item) != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to get NV_GPRS_ANITE_GCF_I from MMGSDI cache");
    return;
  }

  /* Send test_mcc_status indication only if the value of NV_GPRS_ANITE_GCF_I
     has changed and the indication was not sent previously. */
  if (qmi_sim_internal_state.test_mcc_status_sent &&
      (qmi_sim_internal_state.cached_test_mcc_status ==
       anite_nv_item.nv_item_data.gprs_anite_gcf))
  {
    UIM_MSG_HIGH_0("test_mcc_status already updated");
    return;
  }
  else
  {
    qmi_sim_internal_state.cached_test_mcc_status =
      anite_nv_item.nv_item_data.gprs_anite_gcf;
  }

  evt_ind_ptr = uimqmi_malloc(sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));
  if(evt_ind_ptr == NULL)
  {
    return;
  }

  memset(evt_ind_ptr, 0, sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));
  evt_ind_ptr->evt_test_mcc_status_valid = TRUE;
  evt_ind_ptr->evt_test_mcc_status = anite_nv_item.nv_item_data.gprs_anite_gcf ;

  cb_err = qmi_csi_send_ind(evt_cb_client_ref_ptr->qcsi_client->client_handle,
                            QMI_SIM_INTERNAL_MMGSDI_EVENT_REPORT_IND_V01,
                            evt_ind_ptr,
                            sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));

  if(cb_err != QMI_CSI_CB_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_EVENT_REPORT_IND_V01 ");
  }
  else
  {
    UIM_MSG_HIGH_1("event indication sent for test_mcc_status 0x%x",
                   anite_nv_item.nv_item_data.gprs_anite_gcf);
    qmi_sim_internal_state.test_mcc_status_sent = TRUE;
  }

  uimqmi_free(evt_ind_ptr);
  evt_ind_ptr = NULL;
} /* qmi_sim_internal_send_test_mcc_status_ind */


/*===========================================================================

                             CALLBACK FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_RESPONSE_CALLBACK()

  DESCRIPTION
    This function is executed as callback of MMGSDI API. This is
    executed in the context of MMGSDI task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS
    status : mmgsdi status
    cnf_type : confirmation type
    cnf_ptr  : pointer to cnf

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_mmgsdi_response_callback
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf_type,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  qmi_sim_internal_local_client_ref_type  *local_client_data_ptr = NULL;

  ASSERT(cnf_ptr != NULL);
  UIM_MSG_HIGH_1("Got response from MMGSDI for cnf_type: 0x%x", cnf_type);
  UIM_MSG_HIGH_2("client id 0x%x | session id 0x%x",
                 cnf_ptr->response_header.client_id,
                 cnf_ptr->response_header.session_id);

  local_client_data_ptr = (qmi_sim_internal_local_client_ref_type *)cnf_ptr->response_header.client_data;

  switch(cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF :
      qmi_sim_internal_send_client_id_and_evt_reg_ind(local_client_data_ptr,
                                             (mmgsdi_client_id_and_evt_reg_cnf_type*)cnf_ptr);
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF :
      qmi_sim_internal_send_client_id_and_evt_dereg_ind(local_client_data_ptr,
                                             (mmgsdi_client_id_and_evt_dereg_cnf_type *)cnf_ptr);
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      qmi_sim_internal_send_session_open_ind(local_client_data_ptr,
                                             (mmgsdi_session_open_cnf_type*)cnf_ptr);
      break;

    case MMGSDI_SESSION_CLOSE_CNF:
      qmi_sim_internal_send_session_close_ind(local_client_data_ptr,
                                             (mmgsdi_session_close_cnf_type*)cnf_ptr);
      break;

    case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
      qmi_sim_internal_send_get_all_avai_apps_ind(local_client_data_ptr,
                                                  (mmgsdi_get_all_available_apps_cnf_type*)cnf_ptr);
      break;

    case MMGSDI_GET_FILE_ATTR_CNF:
      qmi_sim_internal_send_get_file_attr_ind(local_client_data_ptr,
                                              (mmgsdi_get_file_attr_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_READ_CNF:
      switch(local_client_data_ptr->msg_id)
      {
        case QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_REQ_V01:
          qmi_sim_internal_send_read_transparent_ind(local_client_data_ptr,
                                                     (mmgsdi_read_cnf_type*) cnf_ptr);
          break;
        case QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_REQ_V01:
          qmi_sim_internal_send_read_record_ind(local_client_data_ptr,
                                                (mmgsdi_read_cnf_type*) cnf_ptr);
          break;
      }
      break;

    case MMGSDI_WRITE_CNF:
      switch(local_client_data_ptr->msg_id)
      {
        case QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_REQ_V01:
          qmi_sim_internal_send_write_transparent_ind(local_client_data_ptr,
                                                      (mmgsdi_write_cnf_type*) cnf_ptr);
          break;
        case QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_REQ_V01:
          qmi_sim_internal_send_write_record_ind(local_client_data_ptr,
                                                 (mmgsdi_write_cnf_type*) cnf_ptr);
          break;
      }
      break;

    case MMGSDI_SRV_AVAILABLE_CNF:
      qmi_sim_internal_send_is_service_available_ind(local_client_data_ptr,
                                                     (mmgsdi_srv_available_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
      qmi_sim_internal_send_get_app_capabilities_ind(local_client_data_ptr,
                                                     (mmgsdi_session_app_capabilities_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_USIM_AUTH_CNF:
      qmi_sim_internal_send_usim_authenticate_ind(local_client_data_ptr,
                                                     (mmgsdi_usim_auth_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_SESSION_RUN_GSM_ALGO_CNF:
      qmi_sim_internal_send_run_gsm_algo_ind(local_client_data_ptr,
                                              (mmgsdi_session_run_gsm_algo_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_REFRESH_CNF:
      switch(local_client_data_ptr->msg_id)
      {
        case QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_REQ_V01:
          qmi_sim_internal_send_register_for_refresh_ind(local_client_data_ptr,
                                                         (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_REQ_V01:
          qmi_sim_internal_send_deregister_for_refresh_ind(local_client_data_ptr,
                                                           (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_REQ_V01:
          qmi_sim_internal_send_register_for_refresh_ok_to_proceed_voting_ind(local_client_data_ptr,
                                                                              (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_REQ_V01:
          qmi_sim_internal_send_ok_to_proceed_with_refresh_ind(local_client_data_ptr,
                                                               (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_REQ_V01:
          qmi_sim_internal_send_ok_to_refresh_ind(local_client_data_ptr,
                                                  (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_REQ_V01:
          qmi_sim_internal_send_refresh_complete_ind(local_client_data_ptr,
                                                     (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;

        case QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_REQ_V01:
          qmi_sim_internal_send_naa_refresh_ind(local_client_data_ptr,
                                                (mmgsdi_refresh_cnf_type*) cnf_ptr);
          break;
      }
      break;

    case MMGSDI_SEARCH_CNF:
      qmi_sim_internal_send_search_ind(local_client_data_ptr,
                                       (mmgsdi_search_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_CNF:
      qmi_sim_internal_send_register_for_file_update_ind(local_client_data_ptr,
                                                         (mmgsdi_session_reg_file_update_cnf_type*) cnf_ptr);
      break;

   case MMGSDI_SESSION_PERSO_CNF:
      switch(cnf_ptr->session_perso_cnf.perso_action)
      {
        case MMGSDI_PERSO_OTA_DEPERSO:
          qmi_sim_internal_send_session_perso_ota_deperso_ind(local_client_data_ptr,
                                                              (mmgsdi_session_perso_cnf_type*) cnf_ptr);
          break;
        default:
          UIM_MSG_ERR_0("perso cnf not handled");
          break;
      }
      break;

    case MMGSDI_INCREASE_CNF:
      qmi_sim_internal_send_increase_ind(local_client_data_ptr,
                                         (mmgsdi_increase_cnf_type*) cnf_ptr);
      break;

    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      qmi_sim_internal_send_get_all_pin_status_ind(local_client_data_ptr,
                                                   (mmgsdi_get_all_pin_status_cnf_type*) cnf_ptr);
      break;

    default:
      UIM_MSG_ERR_0("Message cnf not handled");
      break;
  }

  /* FREE local_client_ref here */

  if(local_client_data_ptr != NULL)
  {
    uimqmi_free(local_client_data_ptr);
    local_client_data_ptr = NULL;
  }
} /* qmi_sim_internal_mmgsdi_response_callback */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_MMGSDI_EVT_CALLBACK()

  DESCRIPTION
    This function is executed as callback from MMGSDI whenever there
    is an event. It is executed in the context of MMGSDI task and needs
    to make a deep copy of the event and post a command to the QMI task
    to be processed.

  PARAMETERS
    event

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_mmgsdi_evt_callback
(
  const mmgsdi_event_data_type * event_ptr
)
{
  sim_internal_mmgsdi_event_report_ind_msg_v01 *evt_ind_ptr           = NULL;
  qmi_sim_internal_evt_cb_client_ref_type      *evt_cb_client_ref_ptr = NULL;
  uint8                                         index                 = 0;
  qmi_csi_error                                 resp_err              = QMI_CSI_NO_ERR;
  qmi_csi_cb_error                              cb_err                = QMI_CSI_CB_NO_ERR;

  if(event_ptr == NULL)
  {
    UIM_MSG_ERR_0("event_ptr == NULL");
    return;
  }
  UIM_MSG_HIGH_1("Received event_cback from MMGSDI for event 0x%x",
                 event_ptr->evt);

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  for (index=0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
  {
    if ((qmi_sim_internal_remote_evt_cb_table[index].in_use) &&
         (event_ptr->evt == MMGSDI_CARD_INSERTED_EVT ||
          event_ptr->evt == MMGSDI_CARD_ERROR_EVT))
    {
      if(event_ptr->client_id == qmi_sim_internal_remote_evt_cb_table[index].client_id)
      {
        evt_cb_client_ref_ptr = &qmi_sim_internal_remote_evt_cb_table[index];
        UIM_MSG_HIGH_1("evt ind for client id 0x%x", event_ptr->client_id);
        break;
      }
    }
    else if(qmi_sim_internal_remote_evt_cb_table[index].in_use &&
            event_ptr->evt != MMGSDI_CARD_INSERTED_EVT &&
            event_ptr->evt != MMGSDI_CARD_ERROR_EVT &&
            qmi_sim_internal_remote_evt_cb_table[index].session_id == event_ptr->session_id &&
            qmi_sim_internal_remote_evt_cb_table[index].client_id == event_ptr->client_id )
    {
      evt_cb_client_ref_ptr = &qmi_sim_internal_remote_evt_cb_table[index];
      UIM_MSG_HIGH_1("evt ind for session id 0x%x", event_ptr->session_id);
      break;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if ((index >= QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB) ||
      (evt_cb_client_ref_ptr == NULL) ||
      (evt_cb_client_ref_ptr->remote_evt_cb == NULL))
  {
    UIM_MSG_HIGH_3("Can't find remote evt cb for client id: 0x%x, session id 0x%x, index 0x%x",
                   event_ptr->client_id, event_ptr->session_id, index);
    return;
  }

  evt_ind_ptr = uimqmi_malloc(sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));
  if(evt_ind_ptr == NULL)
  {
    return;
  }

  memset(evt_ind_ptr, 0, sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));
  evt_ind_ptr->evt_info.event_handle = evt_cb_client_ref_ptr->remote_evt_cb;
  evt_ind_ptr->evt_info.client_id    = event_ptr->client_id;
  evt_ind_ptr->evt_info.session_id   = event_ptr->session_id;

  switch(event_ptr->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_card_inserted_evt_ind(&event_ptr->data.card_inserted,
                                                                       evt_ind_ptr);
      break;
    case MMGSDI_CARD_ERROR_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_card_error_evt_ind(&event_ptr->data.card_error,
                                                                    evt_ind_ptr);
      break;
    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_pin_evt_ind(event_ptr->evt,
                                                             &event_ptr->data.pin,
                                                             evt_ind_ptr);
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_session_changed_evt_ind(&event_ptr->data.session_changed,
                                                                         evt_ind_ptr);
      break;
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_subscription_ready_evt_ind(&event_ptr->data.subscription_ready,
                                                                            evt_ind_ptr);
      qmi_sim_internal_send_test_mcc_status_ind();
      break;
    case MMGSDI_SESSION_CLOSE_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_session_close_evt_ind(&event_ptr->data.session_close,
                                                                       evt_ind_ptr);
      break;
    case MMGSDI_PERSO_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_perso_evt_ind(event_ptr->data.perso,
                                                               evt_ind_ptr);
      break;
    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_illegal_session_evt_ind(event_ptr->data.illegal_session,
                                                                         evt_ind_ptr);
      break;
    case MMGSDI_REFRESH_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_refresh_evt_ind(&event_ptr->data.refresh,
                                                                 evt_ind_ptr);
      break;

    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_terminal_profile_dl_evt_ind(&event_ptr->data.terminal_profile,
                                                                             evt_ind_ptr);
      break;

    case MMGSDI_FILE_UPDATE_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_file_update_evt_ind(&event_ptr->data.file_update,
                                                                 evt_ind_ptr);
      break;

    case MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_eons_opl_pnn_cache_ready_evt_ind(&event_ptr->data.eons_opl_pnn_cache_ready,
                                                                                  evt_ind_ptr);
      break;

    case MMGSDI_FDN_EVT:
      resp_err = qmi_sim_internal_process_mmgsdi_fdn_evt_ind(&event_ptr->data.fdn,
                                                             evt_ind_ptr);
      break;

    /* All other cases are not handled. Simply drop unnecessary events */
    default:
      UIM_MSG_HIGH_1("Unhandled event: 0x%x", event_ptr->evt);
      resp_err = QMI_CSI_CB_INTERNAL_ERR;
      break;
  }

  if(resp_err == QMI_CSI_NO_ERR)
  {
    cb_err = qmi_csi_send_ind(evt_cb_client_ref_ptr->qcsi_client->client_handle,
                                QMI_SIM_INTERNAL_MMGSDI_EVENT_REPORT_IND_V01,
                                evt_ind_ptr,
                                sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01));

    UIM_MSG_HIGH_1("event indication sent: resp_err : 0x%x", resp_err);
    if(cb_err != QMI_CSI_CB_NO_ERR)
    {
      UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_EVENT_REPORT_IND_V01 ");
    }
  }
  else
  {
    UIM_MSG_ERR_0("Error returned from process event indication ");
  }

  /* Free of local_client_ref_ptr */
  if(evt_ind_ptr)
  {
    uimqmi_free(evt_ind_ptr);
    evt_ind_ptr = NULL;
  }
} /* qmi_sim_internal_mmgsdi_evt_callback */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_GSTK_EVT_CALLBACK()

  DESCRIPTION
    This function is executed as callback from GST whenever there
    is an event. It is executed in the context of GST task and needs
    to make a deep copy of the event and post a command to the QMI task
    to be processed.

  PARAMETERS
    event

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_gstk_evt_callback
(
  gstk_tal_cmd_details_type *gstk_evt_ptr
)
{
  /* gstk_tal_evt_callback for remote tal client */
  sim_internal_gstk_tal_event_report_ind_msg_v01 *evt_ind_ptr           = NULL;
  qmi_sim_internal_client_info_type             * qcsi_client           = NULL;
  qmi_csi_error                                   resp_err              = QMI_CSI_NO_ERR;
  qmi_csi_cb_error                                cb_err                = QMI_CSI_CB_NO_ERR;

  if(gstk_evt_ptr == NULL)
  {
    UIM_MSG_ERR_0("gstk_evt_ptr == NULL");
    return;
  }
  UIM_MSG_HIGH_0("Received event_cback from GSTK for event.");

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  if (qmi_sim_internal_remote_gstk_evt_cb.in_use)
  {
    evt_ind_ptr = uimqmi_malloc(sizeof(sim_internal_gstk_tal_event_report_ind_msg_v01));
    if(evt_ind_ptr == NULL)
    {
      resp_err = QMI_CSI_INTERNAL_ERR;
    }
    else
    {
      memset(evt_ind_ptr, 0, sizeof(sim_internal_gstk_tal_event_report_ind_msg_v01));
      evt_ind_ptr->event_handle = qmi_sim_internal_remote_gstk_evt_cb.remote_evt_cb;
      qcsi_client = qmi_sim_internal_remote_gstk_evt_cb.qcsi_client;
    }
  }
  else
  {
    UIM_MSG_ERR_0("No remote gstk evt callback available");
    resp_err = QMI_CSI_INVALID_HANDLE;
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Can't find remote evt cb");
    /* Free of local_client_ref_ptr */
    if(evt_ind_ptr)
    {
      uimqmi_free(evt_ind_ptr);
      evt_ind_ptr = NULL;
    }
    return;
  }

  resp_err = qmi_sim_internal_convert_gstk_cmd_group(gstk_evt_ptr->cmd_group,
                                                    &(evt_ind_ptr->cmd_group));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("failed to convert gstk_cmd_group");
    /* Free of local_client_ref_ptr */
    if(evt_ind_ptr)
    {
      uimqmi_free(evt_ind_ptr);
      evt_ind_ptr = NULL;
    }
    return;
  }

  switch(gstk_evt_ptr->cmd_group)
  {
    case GSTK_TERMINAL_PROFILE_RSP:
    case GSTK_ENVELOPE_RSP:
    case GSTK_TERMINAL_RSP:
      /* for tal_uim_rpt */
      evt_ind_ptr->tal_uim_report_valid = TRUE;
      evt_ind_ptr->tal_uim_report.user_data = gstk_evt_ptr->cmd_details.tal_uim_rpt.user_data;
      evt_ind_ptr->tal_uim_report.sw1 = gstk_evt_ptr->cmd_details.tal_uim_rpt.sw1;
      evt_ind_ptr->tal_uim_report.sw2 = gstk_evt_ptr->cmd_details.tal_uim_rpt.sw2;
      evt_ind_ptr->tal_intermediate_sw_valid = TRUE;
      evt_ind_ptr->tal_intermediate_sw.sw1 = (uint8_t)gstk_evt_ptr->cmd_details.tal_uim_rpt.get_rsp_sw1;
      evt_ind_ptr->tal_intermediate_sw.sw2 = (uint8_t)gstk_evt_ptr->cmd_details.tal_uim_rpt.get_rsp_sw2;
      evt_ind_ptr->proactive_cmd_pending_valid = TRUE;
      evt_ind_ptr->proactive_cmd_pending =
        gstk_evt_ptr->cmd_details.tal_uim_rpt.proactive_cmd_pending;
      resp_err = qmi_sim_internal_convert_gstk_rpt_status(gstk_evt_ptr->cmd_details.tal_uim_rpt.rpt_status,
                                               &(evt_ind_ptr->tal_uim_report.rpt_status));
      if (resp_err != QMI_CSI_NO_ERR)
      {
        UIM_MSG_ERR_0("failed to convert gstk_rpt_status");
        break;
      }
      if(gstk_evt_ptr->cmd_details.tal_uim_rpt.data_length > 0 &&
         gstk_evt_ptr->cmd_details.tal_uim_rpt.data_length < QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01 &&
         gstk_evt_ptr->cmd_details.tal_uim_rpt.data_ptr != NULL)
      {
        evt_ind_ptr->tal_uim_report.data_len = gstk_evt_ptr->cmd_details.tal_uim_rpt.data_length;
        (void)memscpy(evt_ind_ptr->tal_uim_report.data,
                      sizeof(evt_ind_ptr->tal_uim_report.data),
                      gstk_evt_ptr->cmd_details.tal_uim_rpt.data_ptr,
                      evt_ind_ptr->tal_uim_report.data_len);
      }
      break;
    case GSTK_PROACTIVE_CMD:
      /* for tal_pro_cmd */
      evt_ind_ptr->tal_pro_cmd_valid = TRUE;
      resp_err = qmi_sim_internal_convert_gstk_slot_id(gstk_evt_ptr->cmd_details.tal_pro_cmd.slot,
                                                       &(evt_ind_ptr->tal_pro_cmd.slot));
      if (resp_err != QMI_CSI_NO_ERR)
      {
        UIM_MSG_ERR_0("failed to convert gstk_slot");
        break;
      }
      if(gstk_evt_ptr->cmd_details.tal_pro_cmd.data_length > 0 &&
         gstk_evt_ptr->cmd_details.tal_pro_cmd.data_length < QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01 &&
         gstk_evt_ptr->cmd_details.tal_pro_cmd.data_ptr != NULL)
      {
        evt_ind_ptr->tal_pro_cmd.data_len = gstk_evt_ptr->cmd_details.tal_pro_cmd.data_length;
        (void)memscpy(evt_ind_ptr->tal_pro_cmd.data,
                      sizeof(evt_ind_ptr->tal_pro_cmd.data),
                      gstk_evt_ptr->cmd_details.tal_pro_cmd.data_ptr,
                      evt_ind_ptr->tal_pro_cmd.data_len);
      }

      break;

    /* All other cases are not handled. Simply drop unnecessary events */
    default:
      UIM_MSG_HIGH_1("Unhandled event: 0x%x", gstk_evt_ptr->cmd_group);
      resp_err = QMI_CSI_CB_INTERNAL_ERR;
      break;
  }

  if(resp_err == QMI_CSI_NO_ERR)
  {
    cb_err = qmi_csi_send_ind(qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_GSTK_TAL_EVENT_REPORT_IND_V01,
                              evt_ind_ptr,
                              sizeof(sim_internal_gstk_tal_event_report_ind_msg_v01));

    UIM_MSG_HIGH_1("event indication sent: resp_err : 0x%x", resp_err);
    if(cb_err != QMI_CSI_CB_NO_ERR)
    {
      UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_GSTK_TAL_EVENT_REPORT_IND_V01 ");
    }
  }
  else
  {
    UIM_MSG_ERR_0("Error returned from process event indication ");
  }

  /* Free of local_client_ref_ptr */
  if(evt_ind_ptr)
  {
    uimqmi_free(evt_ind_ptr);
    evt_ind_ptr = NULL;
  }
} /* qmi_sim_internal_gstk_evt_callback */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONNECT_CB()

  DESCRIPTION
    Connect callback function registered with the framework to be invoked when
    client sends the first request message to be service.

  PARAMETERS
    cli_handle
    service_cookie
    connection_handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_connect_cb
(
  qmi_client_handle   cli_handle,
  void               *service_cookie,
  void              **connection_handle
)
{
  uint8  index = 0;

  /* there is no need to store service specific context information */
  (void) service_cookie;

  UIM_MSG_HIGH_0("qmi_sim_internal_connect_cb");

  if(connection_handle == NULL)
  {
    UIM_MSG_ERR_0("Invalid connection handle");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

/* Protect access to the client registry table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  for (index=0; index < QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT; index++)
  {
    if (!qmi_sim_internal_client_registry[index].in_use)
    {
      qmi_sim_internal_client_registry[index].in_use         = TRUE;
      qmi_sim_internal_client_registry[index].client_handle  = cli_handle;
      *connection_handle = &qmi_sim_internal_client_registry[index];
      UIM_MSG_HIGH_1("qmi_sim_internal_connect_cb: ClientHandle=%x",
                     cli_handle);
      break;
    }
  }

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if (index >= QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_connect_cb: No Slot availbale for new client");
    return QMI_CSI_NO_MEM;
  }

  return QMI_CSI_CB_NO_ERR;
} /* qmi_sim_internal_connect_cb */

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_DISCONNECT_CB()

  DESCRIPTION
    Callback function registered with the framework to be invoked when a client
    requests to remove connection with service.

  PARAMETERS
    connection_handle
    service_cookie

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_disconnect_cb (void *conn_handle, void *service_cookie)
{
  uint8                                 index = 0;
  uint8                                 j     = 0;
  qmi_sim_internal_client_info_type*    pClient_handle = NULL;

  UIM_MSG_HIGH_0("qmi_sim_internal_disconnect_cb");
  (void)service_cookie;

  if (NULL == conn_handle)
  {
    UIM_MSG_ERR_0("qmi_sim_internal_disconnect_cb: conn_handle NULL");
    return;
  }

  pClient_handle = (qmi_sim_internal_client_info_type*)conn_handle;

  /* Protect access to the client registry table and remote evt cb table */
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  for (index=0; index < QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT; index++)
  {
    if ((qmi_sim_internal_client_registry[index].in_use) &&
        (qmi_sim_internal_client_registry[index].client_handle == pClient_handle->client_handle))
    {
      qmi_sim_internal_client_registry[index].client_handle = 0;
      qmi_sim_internal_client_registry[index].in_use = FALSE;

      for(j=0; j < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; j++)
      {
        if((qmi_sim_internal_remote_evt_cb_table[j].in_use) &&

           (qmi_sim_internal_remote_evt_cb_table[j].qcsi_client->client_handle == pClient_handle->client_handle))
        {
/*          mmgsdi_status = mmgsdi_client_id_and_evt_dereg(qmi_sim_internal_remote_evt_cb_table[j].client_id,
                                                         qmi_sim_internal_mmgsdi_response_callback,
                                                         0);
*/
          UIM_MSG_HIGH_2("clean up remote_evt_cb_table: client id 0x%x, session id 0x%x",
                         qmi_sim_internal_remote_evt_cb_table[j].client_id,
                         qmi_sim_internal_remote_evt_cb_table[j].session_id);
          qmi_sim_internal_remote_evt_cb_table[j].in_use = FALSE;
          qmi_sim_internal_remote_evt_cb_table[j].client_id = 0;
          qmi_sim_internal_remote_evt_cb_table[j].session_id = 0;
          qmi_sim_internal_remote_evt_cb_table[j].remote_evt_cb = 0;
          qmi_sim_internal_remote_evt_cb_table[j].qcsi_client = NULL;
        }
      }
      break;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  if (index >= QMI_SIM_INTERNAL_MAX_SERVICE_CLIENT)
  {
    UIM_MSG_ERR_2("qmi_sim_internal_disconnect_cb: NOT FOUND conn_handle=%x client_handle=%x",
                  conn_handle, pClient_handle->client_handle);
  }
} /* qmi_sim_internal_disconnect_cb */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_HANDLE_REQ_CB()

  DESCRIPTION
    Request callback function registered with the framework to be invoked when
    client sends request message to service.

  PARAMETERS
    connection_handle : qcsi client handle
    qmi_req_handle : qmi request handle
    msg_id : message id
    req_c_struct
    req_c_struct_len
    service_cookie

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_sim_internal_handle_req_cb
(
  void *connection_handle,
  qmi_req_handle req_handle,
  unsigned int msg_id,
  void *req_struct,
  unsigned int req_struct_len,
  void *service_cookie
)
{
  qmi_sim_internal_client_info_type   *client_info = NULL;
  qmi_csi_cb_error                     cb_status   = QMI_CSI_CB_INTERNAL_ERR;
  uint8                                msg_index   = 0;

  UIM_MSG_HIGH_0("qmi_mmgsdi_handle_req_cb");

  /* there is no need to store service sepcific context information */
  (void) service_cookie;

  if(!connection_handle)
  {
    UIM_MSG_ERR_0("qmi_mmgsdi_handle_req_cb: connection_handle = NULL");
    return cb_status;
  }
  client_info = (qmi_sim_internal_client_info_type*) connection_handle;

  /* Verify that the msg_id received is within range, msg_id starts from 0x20 */
  if(msg_id >= QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_REQ_V01)
  {
    msg_index = (uint8) msg_id - QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_REQ_V01;
    if (msg_index < sizeof(qmi_sim_internal_req_handle_table) / sizeof(*qmi_sim_internal_req_handle_table))
    {
      if(qmi_sim_internal_req_handle_table[msg_index])
      {
        cb_status = qmi_sim_internal_req_handle_table[msg_index](client_info, req_handle,
                                                                 req_struct, req_struct_len);
      }
      else
      {
        UIM_MSG_ERR_1("No function defined to handle request for message ID: 0x%x",
                      msg_id);
      }
    }
    else
    {
      UIM_MSG_ERR_1("received unknown msg id=0x%x", msg_id);
    }
  }

  return cb_status;
} /* qmi_sim_internal_handle_req_cb */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_INIT_GLOBALS()

  DESCRIPTION
    Initalize QMI SIM internal globals

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_init_globals(void)
{
  UIM_MSG_HIGH_0("qmi_sim_internal_init_globals");

  rex_init_crit_sect(qmi_sim_internal_data_crit_sect_ptr);

  memset(qmi_sim_internal_remote_evt_cb_table, 0x00, sizeof(qmi_sim_internal_remote_evt_cb_table));
  memset(&qmi_sim_internal_state, 0x00, sizeof(qmi_sim_internal_state_type));
} /* qmi_sim_internal_init_globals */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_INIT()

  DESCRIPTION
    Called when task gets started.

  PARAMETERS
    tbc
    sig


  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_sim_internal_init(rex_tcb_type *tcb, rex_sigs_type sig)
{
  qmi_csi_error err      = QMI_CSI_NO_ERR;
  qmi_sap_error sap_err  = QMI_SAP_NO_ERR;
  qmi_idl_service_object_type service_obj = sim_internal_get_service_object_v01();
  qmi_csi_os_params os_params;
  qmi_csi_options options;

  UIM_MSG_HIGH_0("qmi_sim_internal_init");
  /*
    Should only get clled once. Initialize globals here
  */
  memset(&os_params, 0, sizeof(qmi_csi_os_params));

  os_params.tcb = tcb;
  os_params.sig = sig;

  /* Should not fail */
  if (!service_obj)
  {
    UIM_MSG_ERR_0("QMI get service obj failed. Check .c and .h");
  }

  QMI_CSI_OPTIONS_INIT(options);
  QMI_CSI_OPTIONS_SET_SCOPE(options, IPC_POLICY_SCOPE_LOCAL_PROCESSOR);

  err = qmi_csi_register_with_options (service_obj,
                                       qmi_sim_internal_connect_cb,
                                       qmi_sim_internal_disconnect_cb,
                                       qmi_sim_internal_handle_req_cb,
                                       NULL,
                                       &os_params,
                                       &options,
                                       &qmi_sim_internal_provider);

  /* add SAP registration for testing with QMI Test tool*/
  sap_err = qmi_sap_register( service_obj, NULL, &qmi_sim_internal_sap_handle);
  if (sap_err != QMI_SAP_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_sap_register register failed with error=%d", err);
    return FALSE;
  }

  /* cannot fail registration */
  if (err != QMI_CSI_NO_ERR || qmi_sim_internal_provider == NULL)
  {
    UIM_MSG_ERR_1("qmi_csi_register_with_options register failed with error=%d",
                  err);
    return FALSE;
  }

  return TRUE;
} /* qmi_sim_internal_init */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CLOSE()

  DESCRIPTION
    Call to unregister a server.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_close(void)
{
  qmi_csi_error err       = QMI_CSI_NO_ERR;
  qmi_sap_error sap_error = QMI_SAP_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_close:");

  err = qmi_csi_unregister (qmi_sim_internal_provider);
  if (err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_csi_unregister failed with error=0x%x", err);
  }
  else
  {
    qmi_sim_internal_provider = NULL;
  }

  sap_error = qmi_sap_deregister (qmi_sim_internal_sap_handle);
  if (sap_error != QMI_SAP_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_sap_deregister failed with error=0x%x", sap_error);
    return;
  }
  qmi_sim_internal_sap_handle = NULL;
}/* qmi_sim_internal_close */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_HANDLE_EVENT()

  DESCRIPTION
    Called to handle an event after the server thread receives an event
    notification. Callbacks from qmi_csi_register will be invoked in the
    server's context.

  PARAMETERS
    tcb
    sig

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_handle_event (rex_tcb_type *tcb, rex_sigs_type sig)
{
  qmi_csi_error err;
  qmi_csi_os_params os_params;

  UIM_MSG_HIGH_0("qmi_sim_internal_handle_event");

  memset(&os_params, 0, sizeof(qmi_csi_os_params));
  os_params.tcb = tcb;
  os_params.sig = sig;

  err = qmi_csi_handle_event(qmi_sim_internal_provider, &os_params);

  if (err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("qmi_sim_internal_handle_event: event failed with error=%d",
                  err);
    return;
  }
} /* qmi_sim_internal_handle_event */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_ACTIVE_MMGSDI_SYNC_CALL()

  DESCRIPTION
    Returns whether there is an active MMGSDI sync call from QMI_SIM_INTERNAL

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_sim_internal_active_mmgsdi_sync_call(void)
{
  return qmi_sim_internal_state.mmgsdi_sync_call;
} /* qmi_sim_internal_active_mmgsdi_sync_call */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */
