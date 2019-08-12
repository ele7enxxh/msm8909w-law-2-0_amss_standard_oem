#ifndef QMI_SIM_INTERNAL_UTIL_H
#define QMI_SIM_INTERNAL_UTIL_H
/*==========================================================================

                         Q M I_S I M_I N T E R N A L_U T I L. C

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

Copyright (c) 2012 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_sim_internal_util.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/15    sp     Updated copyright
04/16/14    tl     Move globals to be declared in source file
09/30/13    bcho   Changes for tracking transactions between QSC and MSM/MDM
06/10/13    yt     Support for sending FDN event
04/30/13    yt     Support for NAA REFRESH 
10/02/12    tl     Removed incorrect MMGSDI result code conversion
09/30/12    vs     Added utility for TAL command group translation  
07/31/12    tkl    Added additional MMGSDI APIs support
07/26/12    tkl    Add refresh and get protocol APIs
07/26/12    tkl    Add support for EONS cache ready event
07/26/12    tkl    Add new MMGSDI and GSTK APIs
03/12/12    tkl    Initial check-in of QMI_SIM_INTERNAL
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"

#include "err.h"
#include "rex.h"
#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "subscriber_identity_module_internal_v01.h"

#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "gstk_exp.h"

/* Test only */
#include "qmi_sap.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/
#define QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB        (100)
#define QMI_SIM_INTERNAL_MAX_REMOTE_GSTK_EVT_CB   (2)

#define QMI_SIM_INTERNAL_MAX_TXN                  (50)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      qmi_sim_internal_txn_state_type

   DESCRIPTION:
     Enumeration for the state of each transaction
-------------------------------------------------------------------------------*/
typedef enum {
  QMI_SIM_INTERNAL_TXN_UNUSED = 0,
  QMI_SIM_INTERNAL_TXN_REQ_RCVD,
  QMI_SIM_INTERNAL_TXN_RESP_SENT
} qmi_sim_internal_txn_state_type;

typedef struct {
  qmi_sim_internal_txn_state_type state;
  uint16                          token;
} qmi_sim_internal_txn_struct_type;

typedef struct {
  boolean                            mmgsdi_sync_call;
  boolean                            test_mcc_status_sent;
  boolean                            cached_test_mcc_status;
  qmi_sim_internal_txn_struct_type   txn_list[QMI_SIM_INTERNAL_MAX_TXN];
  uint8                              txn_index;
}qmi_sim_internal_state_type;

typedef struct
{
  boolean                         in_use;
  qmi_client_handle               client_handle;
} qmi_sim_internal_client_info_type;

/* struct for storing remote mmgsdi response callback pointer for client id & evt reg*/
typedef struct {
  uint32                              remote_cmd_cb;
  uint32                              remote_evt_cb;
  uint32                              remote_client_ref;
  uint8                               msg_id;
  qmi_sim_internal_client_info_type * qcsi_client;
  boolean                             token_valid;
  uint16                              token;
} qmi_sim_internal_local_client_ref_type;

typedef struct {
  boolean                     in_use;
  uint64                      client_id;
  uint64                      session_id;
  uint32                      remote_evt_cb;
  qmi_sim_internal_client_info_type  * qcsi_client;
} qmi_sim_internal_evt_cb_client_ref_type;

extern qmi_sim_internal_evt_cb_client_ref_type    qmi_sim_internal_remote_evt_cb_table[QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB];

typedef struct {
  boolean                                 in_use;
  uint32                                  remote_evt_cb;
  qmi_sim_internal_client_info_type     * qcsi_client;
} qmi_sim_internal_gstk_evt_cb_type;

qmi_sim_internal_gstk_evt_cb_type qmi_sim_internal_remote_gstk_evt_cb;

/* Critical section to be used to protect accesses to Client, Session,
   Channel, Application, PIN and Slot info globals */
extern rex_crit_sect_type           qmi_sim_internal_data_crit_sect;
extern rex_crit_sect_type          *qmi_sim_internal_data_crit_sect_ptr;

extern qmi_sim_internal_state_type  qmi_sim_internal_state;

/*===========================================================================

                            MACROS

===========================================================================*/
/* ===========================================================================
   MACRO:       QMI_SIM_INTERNAL_ENTER_CRIT_SECT
   DESCRIPTION: To enter rex critical section
   INPUT:       crit_sect_ptr:  critical section pointer
   ===========================================================================*/
#define QMI_SIM_INTERNAL_ENTER_CRIT_SECT(crit_sect_ptr)              \
  rex_enter_crit_sect(crit_sect_ptr);

  /* ===========================================================================
   MACRO:       QMI_SIM_INTERNAL_LEAVE_CRIT_SECT
   DESCRIPTION: To leave rex critical section
   INPUT:       crit_sect_ptr:  critical section pointer
   ===========================================================================*/
#define QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(crit_sect_ptr)                    \
  rex_leave_crit_sect(crit_sect_ptr);


/*===========================================================================

                         REQUEST CONVERT FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_FILE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_file_enum
    mmgsdi_file_enum

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_file
(
  sim_internal_mmgsdi_file_enum_v01   sim_internal_mmgsdi_file_enum,
  mmgsdi_file_enum_type              *mmgsdi_file_enum
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_FILE_STRUCTURE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_file
    mmgsdi_file

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_file_structure
(
  sim_internal_mmgsdi_file_structure_enum_v01  sim_internal_mmgsdi_file,
  mmgsdi_file_structure_enum_type             *mmgsdi_file
);



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_TC_MODE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_tc_mode
    mmgsdi_tc_mode

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_tc_mode
(
  sim_internal_mmgsdi_in_tc_mode_enum_v01   sim_internal_mmgsdi_tc_mode,
  mmgsdi_in_tc_mode_enum_type              *mmgsdi_tc_mode
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_SESSION_INFO

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_session_info
    mmgsdi_session_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_session_info
(
  sim_internal_mmgsdi_session_info_enum_v01   sim_internal_mmgsdi_session_info,
  mmgsdi_session_info_enum_type              *mmgsdi_session_info
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_SESSION_TYPE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_session_type
    mmgsdi_session_type

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_session_type
(
  sim_internal_mmgsdi_session_type_enum_v01   sim_internal_mmgsdi_session_type,
  mmgsdi_session_type_enum_type              *mmgsdi_session_type
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_AUTH_CONTEXT

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_auth_context
    mmgsdi_auth_context

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_auth_context
(
  sim_internal_mmgsdi_autn_context_enum_v01   sim_internal_mmgsdi_auth_context,
  mmgsdi_autn_context_enum_type              *mmgsdi_auth_context
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_SERVICE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    sim_internal_mmgsdi_service
    mmgsdi_service

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_service
(
  sim_internal_mmgsdi_service_enum_v01   sim_internal_mmgsdi_service,
  mmgsdi_service_enum_type              *mmgsdi_service
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SEARCH_TYPE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_mmgsdi_search_type
(
  sim_internal_mmgsdi_search_enum_v01        sim_internal_search_type,
  mmgsdi_search_enum_type                   *search_type
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_PERSO_FEATURE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_perso_feature
    sim_internal_mmgsdi_perso_feature

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_perso_feature
(
  sim_internal_mmgsdi_perso_feature_enum_v01  sim_internal_mmgsdi_perso_feature,
  mmgsdi_perso_feature_enum_type             *mmgsdi_perso_feature
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SEARCH_DIRECTION

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_mmgsdi_search_direction
(
  sim_internal_mmgsdi_search_direction_enum_v01  sim_internal_search_direction,
  mmgsdi_search_direction_enum_type             *search_direction
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SEARCH_OPTION

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_mmgsdi_search_option
(
  sim_internal_mmgsdi_search_option_enum_v01  sim_internal_search_option,
  mmgsdi_search_option_enum_type             *search_option
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SEEK_DIRECTION

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_mmgsdi_seek_direction
(
  sim_internal_mmgsdi_seek_direction_enum_v01  sim_internal_seek_direction,
  mmgsdi_seek_direction_enum_type             *seek_direction
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_SLOT_ID

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
	sim_internal_slot_id
	mmgsdi_slot_id

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_slot_id
(
  sim_internal_mmgsdi_slot_id_enum_v01     sim_internal_slot_id,
  mmgsdi_slot_id_enum_type               * mmgsdi_slot_id
);


/*===========================================================================
  FUNCTION qmi_sim_internal_convert_mmgsdi_protocol

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
	mmgsdi_protocol
	sim_internal_mmgsdi_protocol

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_mmgsdi_protocol
(
  mmgsdi_protocol_enum_type                mmgsdi_protocol,
  sim_internal_mmgsdi_protocol_enum_v01  * sim_internal_mmgsdi_protocol
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_CMD_GROUP

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_gstk_cmd_group
(
  gstk_cmd_group_enum_type                    gstk_cmd_group,
  sim_internal_gstk_cmd_group_enum_v01      * sim_internal_gstk_cmd_group
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_RPT_STATUS

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_gstk_rpt_status
(
  gstk_tal_rpt_status_enum_type               gstk_rpt_status,
  sim_internal_gstk_tal_rpt_status_enum_v01 * sim_internal_gstk_rpt_status
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_GSTK_CLIENT_TYPE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS
    sim_internal_gstk_client_type
    gstk_client_type

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_gstk_client_type
(
  sim_internal_gstk_tal_client_enum_v01  sim_internal_gstk_client_type,
  gstk_tal_client_type                  *gstk_client_type
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_GSTK_SLOT_ID

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_gstk_slot_id
(
  sim_internal_gstk_slot_id_enum_v01     sim_internal_slot_id,
  gstk_slot_id_enum_type               * gstk_slot_id
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_GSTK_TAL_CMD_TYPE

  DESCRIPTION
    To obtain GSTK specific enum value from QMI SIM INTERNAL enum

  PARAMETERS
    sim_internal_gstk_cmd_group_enum_v01,
    gstk_cmd_group_enum_type
 
  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_gstk_tal_cmd_type
(
  sim_internal_gstk_cmd_group_enum_v01        sim_internal_gstk_cmd_group,
  gstk_cmd_group_enum_type                    *gstk_cmd_group
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_SLOT_ID

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_gstk_slot_id
(
  gstk_slot_id_enum_type                 gstk_slot_id,
  sim_internal_gstk_slot_id_enum_v01   * sim_internal_slot_id
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_ACCESS_TECHNOLOGY

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_gstk_access_technology
(
  sim_internal_gstk_access_technology_enum_v01   sim_internal_gstk_rat,
  gstk_access_technology_type                   *gstk_rat
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_LOCATION_STATUS

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS



  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_gstk_location_status
(
  sim_internal_gstk_location_status_enum_v01        sim_internal_gstk_ls,
  gstk_location_status_enum_type                   *gstk_ls
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_SIM_INTERNAL_MMGSDI_AID_TYPE

  DESCRIPTION
    To populate sim internal mmgsdi aid type to mmgsdi aid type

  PARAMETERS
    sim_internal_aid_info
    mmgsdi_aid_info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_populate_sim_internal_mmgsdi_aid_type
(
  const sim_internal_mmgsdi_aid_type_v01 *sim_internal_aid_info,
  mmgsdi_aid_type                        *mmgsdi_aid_info
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_REFRESH_MODE

  DESCRIPTION
    To obtain MMGSDI enum from QMI SIM client-specific enum

  PARAMETERS
    mmgsdi_refresh_mode
    sim_internal_mmgsdi_refresh_mode

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_convert_sim_internal_mmgsdi_refresh_mode
(
  sim_internal_mmgsdi_refresh_mode_enum_v01   sim_internal_mmgsdi_refresh_mode,
  mmgsdi_refresh_mode_enum_type              *mmgsdi_refresh_mode
);

/*===========================================================================

                         RESPONSE FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_MMGSDI_STATUS()

  DESCRIPTION
    Function called to populate mmgsdi status

  PARAMETERS
    mmgsdi_status
    mmgsdi_status_valid
    sim_internal_mmgsdi_status

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_response_mmgsdi_status
(
  mmgsdi_return_enum_type               mmgsdi_status,
  boolean                             * mmgsdi_status_valid,
  sim_internal_mmgsdi_return_enum_v01 * sim_internal_mmgsdi_status
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_GSTK_RESULT_CODE()

  DESCRIPTION
    Function called to populate gstk response result

  PARAMETERS
    mmgsdi_status
    resp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_response_gstk_result_code
(
  gstk_status_enum_type   gstk_status,
  qmi_response_type_v01  *resp
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_GSTK_STATUS()

  DESCRIPTION
    Function called to populate gstk status

  PARAMETERS
    mmgsdi_status
    mmgsdi_status_valid
    sim_internal_mmgsdi_status

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_response_gstk_status
(
  gstk_status_enum_type                 gstk_status,
  boolean                             * gstk_status_valid,
  sim_internal_gstk_status_enum_v01   * sim_internal_gstk_status
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_GET_ALL_AVAILABLE_APPS_SYNC()

  DESCRIPTION
    Function called to populate response get all available apps sync

  PARAMETERS
    avail_apps
    resp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_response_get_all_available_apps_sync
(
  mmgsdi_available_apps_type                                   *avail_apps,
  sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01 *resp
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_GET_INFO_SYNC()

  DESCRIPTION
    Function called to populate response get info sync

  PARAMETERS
    get_info_ptr
    resp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_response_get_info_sync
(
  mmgsdi_session_get_info_type                           *get_info_ptr,
  sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01 *resp
);


/*===========================================================================

                         RESPONSE INDICATION FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_CLIENT_ID_AND_EVT_REG_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_client_id_and_evt_reg_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_client_id_and_evt_reg_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_CLIENT_ID_AND_EVT_DEREG_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_client_id_and_deevt_reg_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_client_id_and_evt_dereg_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_SESSION_OPEN_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_session_open_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_session_open_cnf_type           *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_SESSION_CLOSE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_session_close_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_session_close_cnf_type           *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_GET_ALL_AVAI_APPS_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_get_all_avai_apps_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_get_all_available_apps_cnf_type *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_GET_FILE_ATTR_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_get_file_attr_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_get_file_attr_cnf_type          *cnf_ptr
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_READ_TRANSPARENT_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_read_transparent_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_read_cnf_type                   *cnf_ptr
);



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_READ_RECORD_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_read_record_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_read_cnf_type                   *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_WRITE_TRANSPARENT_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_write_transparent_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_write_cnf_type                   *cnf_ptr
);



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_WRITE_RECORD_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_write_record_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_write_cnf_type                   *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_IS_SERVICE_AVAILABLE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_is_service_available_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_srv_available_cnf_type                   *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_GET_APP_CAPABILITIES_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_get_app_capabilities_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_session_app_capabilities_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_USIM_AUTHENTICATE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_usim_authenticate_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_usim_auth_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_RUN_GSM_ALGO_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_run_gsm_algo_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_session_run_gsm_algo_cnf_type     *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_REGISTER_FOR_REFRESH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_register_for_refresh_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_DEREGISTER_FOR_REFRESH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_deregister_for_refresh_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_OK_TO_PROCEED_WITH_REFRESH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_ok_to_proceed_with_refresh_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_register_for_refresh_ok_to_proceed_voting_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_OK_TO_REFRESH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_ok_to_refresh_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_REFRESH_COMPLETE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
======================================================================== ===*/
qmi_csi_cb_error qmi_sim_internal_send_refresh_complete_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_SESSION_PERSO_OTA_DEPERSO_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_session_perso_ota_deperso_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_session_perso_cnf_type            *cnf_ptr
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_SEARCH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_search_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_search_cnf_type                   *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_REGISTER_FOR_FILE_UPDATE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_register_for_file_update_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_session_reg_file_update_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_INCREASE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_increase_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_increase_cnf_type                 *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_GET_ALL_PIN_STATUS_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_get_all_pin_status_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_get_all_pin_status_cnf_type       *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_NAA_REFRESH_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_naa_refresh_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
);


/*===========================================================================

                         EVENT INDICATION FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_CARD_INSERTED_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    card_inserted_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_card_inserted_evt_ind
( const mmgsdi_card_inserted_evt_info_type           *card_inserted_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01       *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_CARD_ERROR_EVT_IND()

  DESCRIPTION
    Function called to send indication back to client

  PARAMETERS
    card_error_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_card_error_evt_ind
( const mmgsdi_card_err_evt_info_type                *card_error_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01       *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_PIN_EVT_IND()

  DESCRIPTION
    Function called to send indication back to client

  PARAMETERS
    evt
    pin_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_pin_evt_ind
( mmgsdi_events_enum_type                             evt,
  const mmgsdi_pin_evt_info_type                     *pin_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01       *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_SESSION_CHANGED_EVT_IND()

  DESCRIPTION
    Function called to send indication back to client

  PARAMETERS
    mmgsdi_session_changed_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_session_changed_evt_ind
( const mmgsdi_session_changed_evt_info_type   *mmgsdi_session_changed_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01 *evt_ind
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_SUBSCRIPTION_READY_EVT_IND()

  DESCRIPTION
    Function called to send indication back to client

  PARAMETERS
    mmgsdi_sub_ready_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_subscription_ready_evt_ind
( const mmgsdi_subscription_ready_evt_info_type   *mmgsdi_sub_ready_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01    *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_SESSION_CLOSE_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    session_close_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_session_close_evt_ind
( const mmgsdi_session_close_evt_info_type           *session_close_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01       *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_PERSO_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    perso
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_perso_evt_ind
( const mmgsdi_perso_evt_info_type                    perso,
  sim_internal_mmgsdi_event_report_ind_msg_v01       *evt_ind
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_ILLEGAL_SESSION_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    illegal_session
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_illegal_session_evt_ind
( const mmgsdi_session_illegal_subscription_evt_info_type      illegal_session,
  sim_internal_mmgsdi_event_report_ind_msg_v01                *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_REFRESH_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    refresh_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_refresh_evt_ind
( const mmgsdi_refresh_evt_info_type             *refresh_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01   *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_TERMINAL_PROFILE_DL_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    refresh_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_terminal_profile_dl_evt_ind
( const mmgsdi_tp_dl_evt_info_type               *tp_dl_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01   *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_FILE_UPDATE_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    refresh_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_file_update_evt_ind
( const mmgsdi_file_update_evt_info_type         *file_update_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01   *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_CLIENT_ID_AND_EVT_DEREG_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_client_id_and_evt_dereg_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_client_id_and_evt_dereg_cnf_type  *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_SEND_SESSION_CLOSE_IND()

  DESCRIPTION
    Function called to send response indication back to client

  PARAMETERS
    local_client_ref_ptr
    cnf_ptr

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_session_close_ind
(
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_session_close_cnf_type           *cnf_ptr
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT_IND()

  DESCRIPTION
    Function called to send response message back to client

  PARAMETERS
    eons_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_eons_opl_pnn_cache_ready_evt_ind
( const mmgsdi_eons_opl_pnn_cache_ready_evt_info_type     *eons_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01            *evt_ind
);


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_PROCESS_MMGSDI_FDN_EVT_IND()

  DESCRIPTION
    Function called to send FDN event to client

  PARAMETERS
    fdn_ptr
    evt_ind

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_sim_internal_process_mmgsdi_fdn_evt_ind
( const mmgsdi_fdn_evt_info_type                        *fdn_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01          *evt_ind
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_ADD_TOKEN_TO_TXN_LIST

  DESCRIPTION
    Add the received token to the transactions array

  PARAMETERS
    token   : token value

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_add_token_to_txn_list
(
  uint16     token
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_REMOVE_TOKEN_FROM_TXN_LIST

  DESCRIPTION
    Remove the transmitted token from the transactions array

  PARAMETERS
    token   : token value

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_remove_token_from_txn_list
(
  uint16     token
);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_UPDATE_TXN_RESP_SENT

  DESCRIPTION
    Update the state of transaction to RESPONSE SENT

  PARAMETERS
    token   : token value

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_update_txn_resp_sent
(
  uint16     token
);

/* ===========================================================================
   MACRO:       QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT
   DESCRIPTION: To check if token is valid and update txn response sent
   INPUT:       creq_token_valid, req_token, resp_token_valid, resp_token
   ===========================================================================*/
#define   QMI_SIM_INTERNAL_VALIDATE_AND_UPDATE_TXN_RESP_SENT(                 \
                                                     req_token_valid,         \
                                                     req_token,               \
                                                     resp_token_valid,        \
                                                     resp_token)              \
  if (req_token_valid)                                                        \
  {                                                                           \
    qmi_sim_internal_update_txn_resp_sent(req_token);                         \
    resp_token_valid = TRUE;                                                  \
    resp_token       = req_token;                                             \
  }

/* ===========================================================================
   MACRO:       QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST
   DESCRIPTION: To check if token is valid and remove token from txn list
   INPUT:       token_valid, token
   ===========================================================================*/
#define   QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(           \
                                                     token_valid,             \
                                                     token)                   \
  if (token_valid)                                                            \
  {                                                                           \
    qmi_sim_internal_remove_token_from_txn_list(token);                       \
  }

#endif /* QMI_SIM_INTERNAL_UTIL_H */
