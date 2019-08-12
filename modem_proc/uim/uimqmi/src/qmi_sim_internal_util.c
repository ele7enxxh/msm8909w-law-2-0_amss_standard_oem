/*==========================================================================

                         Q M I_S I M_I N T E R N A L_util. C

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_sim_internal_util.c#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/15    sp     Updated copyright
05/28/14    gm     Support of CSG Rel-11 feature 
04/16/14    tl     Move globals to be declared in source file
01/10/14    df     Use v2.0 diag macros
11/28/13    gm     Remove "Remove "GSTK_TERMINAL_PROFILE_EXT_CMD"
09/30/13    bcho   Changes for tracking transactions between QSC and MSM/MDM
08/28/13    tkl    Rename FEATURE_UIM_SGLTE
06/10/13    yt     Support for sending FDN event
05/14/13    gm     Replaced instances of memcpy with memscpy
05/13/13    tl     Incorrect variable being checked by ASSERT
04/30/13    yt     Support for NAA REFRESH
10/02/12    tl     Removed incorrect MMGSDI result code conversion
09/30/12    vs     Added utility for TAL command group translation
09/05/12    tl     Fixed enum translation for refresh event
07/31/12    tkl    Added additional MMGSDI APIs support
07/26/12    tkl    Add refresh and get protocol APIs
07/26/12    tkl    Add support for EONS cache ready event
07/26/12    tkl    Add new MMGSDI and GSTK APIs
07/19/12    vv     Fixed compile error
05/22/12    tkl    Fixed compile error & warnings
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

#include "qmi_sim_internal_util.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdilib_v.h"
/* Test only */
#include "qmi_sap.h"

qmi_sim_internal_evt_cb_client_ref_type    qmi_sim_internal_remote_evt_cb_table[QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB];

rex_crit_sect_type                         qmi_sim_internal_data_crit_sect;
rex_crit_sect_type                       * qmi_sim_internal_data_crit_sect_ptr = &qmi_sim_internal_data_crit_sect;

qmi_sim_internal_state_type                qmi_sim_internal_state;

//Test only
static void print_response_header
(
  uint8                                        ind_type,
  sim_internal_mmgsdi_response_header_type_v01 resp_header

)
{
  UIM_MSG_HIGH_1("print_resp_header for ind 0x%x", ind_type);
  UIM_MSG_HIGH_1("resp_header.mmgsdi_status 0x%x", resp_header.mmgsdi_status );
  UIM_MSG_HIGH_1("resp_header.client_id 0x%x", resp_header.client_id );
  UIM_MSG_HIGH_1("resp_header.session_id 0x%x", resp_header.session_id );
  UIM_MSG_HIGH_1("resp_header.response_len 0x%x", resp_header.response_len );
  UIM_MSG_HIGH_1("resp_header.payload_len 0x%x", resp_header.payload_len );
  UIM_MSG_HIGH_1("resp_header.status_word.valid 0x%x",
                 resp_header.status_word.valid );
  UIM_MSG_HIGH_1("resp_header.status_word.sw1 0x%x",
                 resp_header.status_word.sw1 );
  UIM_MSG_HIGH_1("resp_header.status_word.sw2 0x%x",
                 resp_header.status_word.sw2 );
  UIM_MSG_HIGH_1("resp_header.client_ref 0x%x", resp_header.client_ref );
  UIM_MSG_HIGH_1("resp_header.response_handle 0x%x",
                 resp_header.response_handle );
  UIM_MSG_HIGH_1("resp_header.slot_id 0x%x", resp_header.slot_id );

  return ;
}/* print_response_header() */

/*===========================================================================

            DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/
typedef struct
{
  sim_internal_mmgsdi_perso_feature_enum_v01  sim_internal_mmgsdi_perso_feature;
  mmgsdi_perso_feature_enum_type              mmgsdi_perso_feature;
} sim_internal_mmgsdi_perso_feature_enum_table_type;

const sim_internal_mmgsdi_perso_feature_enum_table_type sim_internal_mmgsdi_perso_feature_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_PERSO_NW_V01, MMGSDI_PERSO_NW},
  { SIM_INTERNAL_MMGSDI_PERSO_NS_V01, MMGSDI_PERSO_NS},
  { SIM_INTERNAL_MMGSDI_PERSO_SP_V01, MMGSDI_PERSO_SP},
  { SIM_INTERNAL_MMGSDI_PERSO_CP_V01, MMGSDI_PERSO_CP},
  { SIM_INTERNAL_MMGSDI_PERSO_SIM_V01, MMGSDI_PERSO_SIM}
};

typedef struct
{
  sim_internal_mmgsdi_slot_id_enum_v01   sim_internal_slot_id;
  mmgsdi_slot_id_enum_type               mmgsdi_slot_id;
} sim_internal_mmgsdi_slot_id_enum_table_type;

const sim_internal_mmgsdi_slot_id_enum_table_type sim_internal_mmgsdi_slot_id_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_SLOT_1_V01, MMGSDI_SLOT_1},
  { SIM_INTERNAL_MMGSDI_SLOT_2_V01, MMGSDI_SLOT_2},
  { SIM_INTERNAL_MMGSDI_SLOT_AUTOMATIC_V01, MMGSDI_SLOT_AUTOMATIC}
};

typedef struct
{
  sim_internal_gstk_status_enum_v01    sim_internal_gstk_status;
  gstk_status_enum_type                gstk_status;
} sim_internal_gstk_status_enum_table_type;

const sim_internal_gstk_status_enum_table_type sim_internal_gstk_status_enum_table[] =
{
  { SIM_INTERNAL_GSTK_SUCCESS_V01, GSTK_SUCCESS},
  { SIM_INTERNAL_GSTK_ERROR_V01, GSTK_ERROR},
  { SIM_INTERNAL_GSTK_DUPLICATE_REGISTRATION_V01, GSTK_DUPLICATE_REGISTRATION},
  { SIM_INTERNAL_GSTK_CLIENT_NOT_REGISTERED_V01, GSTK_CLIENT_NOT_REGISTERED},
  { SIM_INTERNAL_GSTK_NO_CLIENT_REG_FOUND_V01, GSTK_NO_CLIENT_REG_FOUND},
  { SIM_INTERNAL_GSTK_INVALID_CLIENT_TYPE_V01, GSTK_INVALID_CLIENT_TYPE},
  { SIM_INTERNAL_GSTK_PROFILE_VALUE_NOT_ALLOWED_V01, GSTK_PROFILE_VALUE_NOT_ALLOWED},
  { SIM_INTERNAL_GSTK_PROFILE_ALREADY_SENT_V01, GSTK_PROFILE_ALREADY_SENT},
  { SIM_INTERNAL_GSTK_MEMORY_ERROR_V01, GSTK_MEMORY_ERROR},
  { SIM_INTERNAL_GSTK_CLIENT_SPACE_FULL_V01, GSTK_CLIENT_SPACE_FULL},
  { SIM_INTERNAL_GSTK_BAD_PARAM_V01, GSTK_BAD_PARAM},
  { SIM_INTERNAL_GSTK_NULL_INPUT_PARAM_V01, GSTK_NULL_INPUT_PARAM},
  { SIM_INTERNAL_GSTK_PARAM_EXCEED_BOUNDARY_V01, GSTK_PARAM_EXCEED_BOUNDARY},
  { SIM_INTERNAL_GSTK_INVALID_LENGTH_V01, GSTK_INVALID_LENGTH},
  { SIM_INTERNAL_GSTK_EXTRA_PARAM_MISSING_V01, GSTK_EXTRA_PARAM_MISSING},
  { SIM_INTERNAL_GSTK_EXTRA_PARAM_NOT_REQUIRED_V01, GSTK_EXTRA_PARAM_NOT_REQUIRED},
  { SIM_INTERNAL_GSTK_UNSUPPORTED_COMMAND_V01, GSTK_UNSUPPORTED_COMMAND},
  { SIM_INTERNAL_GSTK_INVALID_COMMAND_V01, GSTK_INVALID_COMMAND},
  { SIM_INTERNAL_GSTK_INSUFFICIENT_INFO_V01, GSTK_INSUFFICIENT_INFO},
  { SIM_INTERNAL_GSTK_FILE_ACCESS_FAIL_V01, GSTK_FILE_ACCESS_FAIL},
  { SIM_INTERNAL_GSTK_CONTRADICTING_TIMER_REQUEST_V01, GSTK_CONTRADICTING_TIMER_REQUEST},
  { SIM_INTERNAL_GSTK_NOT_INIT_V01, GSTK_NOT_INIT},
  { SIM_INTERNAL_GSTK_INVALID_DEVICE_COMMAND_V01, GSTK_INVALID_DEVICE_COMMAND},
  { SIM_INTERNAL_GSTK_RPC_ERROR_V01, GSTK_RPC_ERROR}
};

typedef struct
{
  sim_internal_gstk_tal_client_enum_v01   sim_internal_gstk_client;
  gstk_tal_client_type                    gstk_client;
} sim_internal_gstk_client_enum_table_type;

const sim_internal_gstk_client_enum_table_type sim_internal_gstk_client_enum_table[] =
{
  { SIM_INTERNAL_GSTK_TAL_LOCAL_CLIENT_V01, GSTK_TAL_LOCAL_CLIENT},
  { SIM_INTERNAL_GSTK_TAL_REMOTE_CLIENT_V01, GSTK_TAL_REMOTE_CLIENT}
};

typedef struct
{
  sim_internal_gstk_cmd_group_enum_v01        sim_internal_gstk_cmd_group;
  gstk_cmd_group_enum_type                    gstk_cmd_group;
} sim_internal_gstk_cmd_group_enum_table_type;

const sim_internal_gstk_cmd_group_enum_table_type sim_internal_gstk_cmd_group_enum_table[] =
{
  { SIM_INTERNAL_GSTK_PROACTIVE_CMD_V01, GSTK_PROACTIVE_CMD},
  { SIM_INTERNAL_GSTK_TERMINAL_RSP_V01, GSTK_TERMINAL_RSP},
  { SIM_INTERNAL_GSTK_ENVELOPE_CMD_V01, GSTK_ENVELOPE_CMD},
  { SIM_INTERNAL_GSTK_ENVELOPE_RSP_V01, GSTK_ENVELOPE_RSP},
  { SIM_INTERNAL_GSTK_TERMINAL_PROFILE_CMD_V01, GSTK_TERMINAL_PROFILE_CMD},
  { SIM_INTERNAL_GSTK_TERMINAL_PROFILE_RSP_V01, GSTK_TERMINAL_PROFILE_RSP},
  { SIM_INTERNAL_GSTK_STOP_CMD_V01, GSTK_STOP_CMD},
  { SIM_INTERNAL_GSTK_MMGSDI_CMD_V01, GSTK_MMGSDI_CMD},
  { SIM_INTERNAL_GSTK_TIMER_EXP_CMD_V01, GSTK_TIMER_EXP_CMD},
  { SIM_INTERNAL_GSTK_MM_IDLE_CMD_V01, GSTK_MM_IDLE_CMD},
  { SIM_INTERNAL_GSTK_RRC_CELL_CHANGE_IND_V01, GSTK_RRC_CELL_CHANGE_IND},
  { SIM_INTERNAL_GSTK_MODE_CHANGE_IND_V01, GSTK_MODE_CHANGE_IND},
  { SIM_INTERNAL_GSTK_PH_NW_SEL_PREF_CHANGE_IND_V01, GSTK_CM_PH_IND},
  { SIM_INTERNAL_GSTK_CLIENT_REG_REQ_CMD_V01, GSTK_CLIENT_REG_REQ_CMD},
  { SIM_INTERNAL_GSTK_GET_CURR_ACCESS_TECH_CMD_V01, GSTK_GET_CURR_ACCESS_TECH_CMD},
  { SIM_INTERNAL_GSTK_RETRY_ENVELOPE_CMD_V01, GSTK_RETRY_ENVELOPE_CMD},
  { SIM_INTERNAL_GSTK_SEND_POSTPONE_ENVELOPE_CMD_V01, GSTK_SEND_POSTPONE_ENVELOPE_CMD},
  { SIM_INTERNAL_GSTK_TIMER_FOR_ENV_RETRY_CMD_V01, GSTK_TIMER_FOR_ENV_RETRY_CMD},
  { SIM_INTERNAL_GSTK_SETUP_OTASP_CALL_CMD_V01, GSTK_SETUP_OTASP_CALL_CMD},
  { SIM_INTERNAL_GSTK_TAL_CMD_V01, GSTK_TAL_CMD},
  { SIM_INTERNAL_GSTK_TAL_SEND_LS_CMD_V01, GSTK_TAL_SEND_LS_CMD}
};

typedef struct
{
  sim_internal_gstk_tal_rpt_status_enum_v01   sim_internal_gstk_rpt_status;
  gstk_tal_rpt_status_enum_type               gstk_rpt_status;
} sim_internal_gstk_tal_rpt_status_enum_table_type;

const sim_internal_gstk_tal_rpt_status_enum_table_type sim_internal_gstk_tal_rpt_status_enum_table[] =
{
  { SIM_INTERNAL_GSTK_TAL_UIM_FAIL_V01, GSTK_TAL_UIM_FAIL},
  { SIM_INTERNAL_GSTK_TAL_UIM_PASS_V01, GSTK_TAL_UIM_PASS}
};

typedef struct
{
  sim_internal_mmgsdi_slot_id_enum_v01   sim_internal_slot_id;
  gstk_slot_id_enum_type                 gstk_slot_id;
} sim_internal_gstk_slot_id_enum_table_type;

const sim_internal_gstk_slot_id_enum_table_type sim_internal_gstk_slot_id_enum_table[] =
{
  { SIM_INTERNAL_GSTK_SLOT_1_V01, GSTK_SLOT_1},
  { SIM_INTERNAL_GSTK_SLOT_2_V01, GSTK_SLOT_2},
  { SIM_INTERNAL_GSTK_SLOT_AUTOMATIC_V01, GSTK_SLOT_AUTO}
};

typedef struct
{
  sim_internal_gstk_access_technology_enum_v01   sim_internal_gstk_rat;
  gstk_access_technology_type                    gstk_rat;
} sim_internal_gstk_access_technology_enum_table_type;

const sim_internal_gstk_access_technology_enum_table_type sim_internal_gstk_access_technology_enum_table[] =
{
  { SIM_INTERNAL_GSTK_ACCESS_TECH_GSM_V01, GSTK_ACCESS_TECH_GSM},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_UTRAN_V01, GSTK_ACCESS_TECH_UTRAN},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_CDMA_V01, GSTK_ACCESS_TECH_CDMA},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_LTE_V01, GSTK_ACCESS_TECH_LTE},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_NONE_V01, GSTK_ACCESS_NONE}
};

typedef struct
{
  sim_internal_gstk_location_status_enum_v01   sim_internal_gstk_ls;
  gstk_location_status_enum_type               gstk_ls;
} sim_internal_gstk_location_status_enum_table_type;

const sim_internal_gstk_location_status_enum_table_type sim_internal_gstk_location_status_enum_table[] =
{
  { SIM_INTERNAL_GSTK_ACCESS_TECH_GSM_V01, GSTK_NORMAL_SERVICE},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_UTRAN_V01, GSTK_LIMITED_SERVICE},
  { SIM_INTERNAL_GSTK_ACCESS_TECH_CDMA_V01, GSTK_NO_SERVICE}
};

typedef struct
{
  sim_internal_mmgsdi_search_enum_v01   sim_internal_search_type;
  mmgsdi_search_enum_type               search_type;
} sim_internal_mmgsdi_search_type_enum_table_type;

const sim_internal_mmgsdi_search_type_enum_table_type sim_internal_mmgsdi_search_type_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_SEARCH_NONE_V01, MMGSDI_SEARCH_NONE},
  { SIM_INTERNAL_MMGSDI_ICC_SEARCH_V01, MMGSDI_ICC_SEARCH},
  { SIM_INTERNAL_MMGSDI_UICC_SIMPLE_SEARCH_V01, MMGSDI_UICC_SIMPLE_SEARCH},
  { SIM_INTERNAL_MMGSDI_UICC_ENHANCED_SEARCH_V01, MMGSDI_UICC_ENHANCED_SEARCH}
};

typedef struct
{
  sim_internal_mmgsdi_search_direction_enum_v01   sim_internal_search_direction;
  mmgsdi_search_direction_enum_type               search_direction;
} sim_internal_mmgsdi_search_direction_enum_table_type;

const sim_internal_mmgsdi_search_direction_enum_table_type sim_internal_mmgsdi_search_direction_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_SEARCH_DIRECTION_NONE_V01, MMGSDI_SEARCH_DIRECTION_NONE},
  { SIM_INTERNAL_MMGSDI_SEARCH_FORWARD_FROM_REC_NUM_V01, MMGSDI_SEARCH_FORWARD_FROM_REC_NUM},
  { SIM_INTERNAL_MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM_V01, MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM}
};

typedef struct
{
  sim_internal_mmgsdi_search_option_enum_v01   sim_internal_search_option;
  mmgsdi_search_option_enum_type               search_option;
} sim_internal_mmgsdi_search_option_enum_table_type;

const sim_internal_mmgsdi_search_option_enum_table_type sim_internal_mmgsdi_search_option_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_SEARCH_OPTION_NONE_V01, MMGSDI_SEARCH_OPTION_NONE},
  { SIM_INTERNAL_MMGSDI_SEARCH_BY_OFFSET_V01, MMGSDI_SEARCH_BY_OFFSET},
  { SIM_INTERNAL_MMGSDI_SEARCH_BY_CHAR_V01, MMGSDI_SEARCH_BY_CHAR}
};

typedef struct
{
  sim_internal_mmgsdi_seek_direction_enum_v01   sim_internal_seek_direction;
  mmgsdi_seek_direction_enum_type               seek_direction;
} sim_internal_mmgsdi_seek_direction_enum_table_type;

const sim_internal_mmgsdi_seek_direction_enum_table_type sim_internal_mmgsdi_seek_direction_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_SEEK_DIRECTION_NONE_V01, MMGSDI_SEEK_DIRECTION_NONE},
  { SIM_INTERNAL_MMGSDI_SEEK_BEGINNING_FORWARDS_V01, MMGSDI_SEEK_BEGINNING_FORWARDS},
  { SIM_INTERNAL_MMGSDI_SEEK_END_BACKWARDS_V01, MMGSDI_SEEK_END_BACKWARDS}
};

typedef struct
{
  sim_internal_mmgsdi_protocol_enum_v01   sim_internal_mmgsdi_protocol;
  mmgsdi_protocol_enum_type               mmgsdi_protocol;
} sim_internal_mmgsdi_protocol_enum_table_type;

const sim_internal_mmgsdi_protocol_enum_table_type sim_internal_mmgsdi_protocol_enum_table[] =
{
  { SIM_INTERNAL_MMGSDI_NO_PROTOCOL_V01, MMGSDI_NO_PROTOCOL},
  { SIM_INTERNAL_MMGSDI_ICC_V01, MMGSDI_ICC},
  { SIM_INTERNAL_MMGSDI_UICC_V01, MMGSDI_UICC}
};

/*===========================================================================

                           LOCAL FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from MMGSDI error message

  PARAMETERS
    mmgsdi_status   : MMGSDI return value

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_sim_internal_convert_mmgsdi_status_to_errval
(
  mmgsdi_return_enum_type mmgsdi_status
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_status 0x%x", mmgsdi_status);

  switch(mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      return QMI_ERR_NONE_V01;

    case MMGSDI_WARNING_NO_INFO_GIVEN:
    case MMGSDI_INCOMPAT_PIN_STATUS:
    case MMGSDI_PERSO_CHECK_FAILED:
      return QMI_ERR_NO_EFFECT_V01;

    case MMGSDI_INCORRECT_CODE:
    case MMGSDI_PERSO_INVALID_CK:
      return QMI_ERR_INCORRECT_PIN_V01;

    case MMGSDI_CODE_BLOCKED:
    case MMGSDI_PERSO_CK_BLOCKED:
      return QMI_ERR_PIN_BLOCKED_V01;

    case MMGSDI_CODE_PERM_BLOCKED:
      return QMI_ERR_PIN_PERM_BLOCKED_V01;

    case MMGSDI_PIN_NOT_INITIALIZED:
      return QMI_ERR_SIM_NOT_INITIALIZED_V01;

    case MMGSDI_ACCESS_DENIED:
      return QMI_ERR_ACCESS_DENIED_V01;

    case MMGSDI_NOT_FOUND:
      return QMI_ERR_SIM_FILE_NOT_FOUND_V01;

    case MMGSDI_INCORRECT_PARAMS:
      return QMI_ERR_INTERNAL_V01;

    case MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP:
    case MMGSDI_AUTH_ERROR_INCORRECT_MAC:
    case MMGSDI_AUTH_FAIL:
      return QMI_ERR_AUTHENTICATION_FAILED_V01;

    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_status received");
      break;
  }
  return QMI_ERR_INTERNAL_V01;
} /* qmi_sim_internal_convert_mmgsdi_status_to_errval() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from GSTK error message

  PARAMETERS
    gstk_status   : GSTK return value

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_sim_internal_convert_gstk_status_to_errval
(
  gstk_status_enum_type gstk_status
)
{
  UIM_MSG_HIGH_1("Received gstk_status 0x%x", gstk_status);

  switch(gstk_status)
  {
    case GSTK_SUCCESS:
      return QMI_ERR_NONE_V01;

    case GSTK_EXTRA_PARAM_NOT_REQUIRED:
    case GSTK_EXTRA_PARAM_MISSING:
    case GSTK_INVALID_LENGTH:
    case GSTK_BAD_PARAM:
      return QMI_ERR_INVALID_ARG_V01;

    case GSTK_MEMORY_ERROR:
    case GSTK_CLIENT_SPACE_FULL:
      return QMI_ERR_NO_MEMORY_V01;

    default:
      UIM_MSG_HIGH_0("Unhandled gstk_status received");
      break;
  }

  return QMI_ERR_INTERNAL_V01;
} /* qmi_sim_internal_convert_gstk_status_to_errval() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_STATUS

  DESCRIPTION
    To obtain QMI client-specific error message from MMGSDI error message

  PARAMETERS
    mmgsdi_status   : MMGSDI return value

  RETURN VALUE
    sim_internal_mmgsdi_return_enum_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static sim_internal_mmgsdi_return_enum_v01 qmi_sim_internal_convert_mmgsdi_status
(
  mmgsdi_return_enum_type mmgsdi_status
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_status 0x%x", mmgsdi_status);

  switch(mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      return SIM_INTERNAL_MMGSDI_SUCCESS_V01;

    case MMGSDI_INCORRECT_PARAMS:
      return SIM_INTERNAL_MMGSDI_INCORRECT_PARAMS_V01;

    case MMGSDI_CMD_QUEUE_FULL:
      return SIM_INTERNAL_MMGSDI_CMD_QUEUE_FULL_V01;

    case MMGSDI_ERROR:
      return SIM_INTERNAL_MMGSDI_ERROR_V01;

    case MMGSDI_ACCESS_DENIED:
      return SIM_INTERNAL_MMGSDI_ACCESS_DENIED_V01;

    case MMGSDI_NOT_FOUND:
      return SIM_INTERNAL_MMGSDI_NOT_FOUND_V01;

    case MMGSDI_INCOMPAT_PIN_STATUS:
      return SIM_INTERNAL_MMGSDI_INCOMPAT_PIN_STATUS_V01;

    case MMGSDI_INCORRECT_CODE:
      return SIM_INTERNAL_MMGSDI_INCORRECT_CODE_V01;

    case MMGSDI_CODE_BLOCKED:
      return SIM_INTERNAL_MMGSDI_CODE_BLOCKED_V01;

    case MMGSDI_INCREASE_IMPOSSIBLE:
      return SIM_INTERNAL_MMGSDI_INCREASE_IMPOSSIBLE_V01;

    case MMGSDI_NOT_SUPPORTED:
      return SIM_INTERNAL_MMGSDI_NOT_SUPPORTED_V01;

    case MMGSDI_NOT_INIT:
      return SIM_INTERNAL_MMGSDI_NOT_INIT_V01;

    case MMGSDI_SUCCESS_BUT_ILLEGAL_SIM:
      return SIM_INTERNAL_MMGSDI_SUCCESS_BUT_ILLEGAL_SIM_V01;

    case MMGSDI_AUTH_ERROR_INCORRECT_MAC:
      return SIM_INTERNAL_MMGSDI_AUTH_ERROR_INCORRECT_MAC_V01;

    case MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP:
      return SIM_INTERNAL_MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP_V01;

    case MMGSDI_SIM_TECHNICAL_PROBLEMS:
      return SIM_INTERNAL_MMGSDI_SIM_TECHNICAL_PROBLEMS_V01;

    case MMGSDI_NO_EF_SELECTED:
      return SIM_INTERNAL_MMGSDI_NO_EF_SELECTED_V01;

    case MMGSDI_EF_INCONSISTENT:
      return SIM_INTERNAL_MMGSDI_EF_INCONSISTENT_V01;

    case MMGSDI_ERROR_NO_EVENT_NEEDED:
      return SIM_INTERNAL_MMGSDI_ERROR_NO_EVENT_NEEDED_V01;

    case MMGSDI_PIN_NOT_INITIALIZED:
      return SIM_INTERNAL_MMGSDI_PIN_NOT_INITIALIZED_V01;

    case MMGSDI_UNKNOWN_INST_CLASS:
      return SIM_INTERNAL_MMGSDI_UNKNOWN_INST_CLASS_V01;

    case MMGSDI_WARNING_NO_INFO_GIVEN:
      return SIM_INTERNAL_MMGSDI_WARNING_NO_INFO_GIVEN_V01;

    case MMGSDI_WARNING_POSSIBLE_CORRUPTION:
      return SIM_INTERNAL_MMGSDI_WARNING_POSSIBLE_CORRUPTION_V01;

    case MMGSDI_INCORRECT_LENGTH:
      return SIM_INTERNAL_MMGSDI_INCORRECT_LENGTH_V01;

    case MMGSDI_UIM_CMD_TIMEOUT:
      return SIM_INTERNAL_MMGSDI_UIM_CMD_TIMEOUT_V01;

    case MMGSDI_CODE_PERM_BLOCKED:
      return SIM_INTERNAL_MMGSDI_CODE_PERM_BLOCKED_V01;

    case MMGSDI_REFRESH_SUCCESS:
      return SIM_INTERNAL_MMGSDI_REFRESH_SUCCESS_V01;

    case MMGSDI_REFRESH_IN_PROGRESS:
      return SIM_INTERNAL_MMGSDI_REFRESH_IN_PROGRESS_V01;

    case MMGSDI_CAN_NOT_REFRESH:
      return SIM_INTERNAL_MMGSDI_CAN_NOT_REFRESH_V01;

    case MMGSDI_REFRESH_LATER:
      return SIM_INTERNAL_MMGSDI_REFRESH_LATER_V01;

    case MMGSDI_PATHS_DECODE_ERROR:
      return SIM_INTERNAL_MMGSDI_PATHS_DECODE_ERROR_V01;

    case MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE:
      return SIM_INTERNAL_MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE_V01;

    case MMGSDI_SIM_BUSY:
      return SIM_INTERNAL_MMGSDI_SIM_BUSY_V01;

    case MMGSDI_INVALIDATION_CONTRADICTION_STATUS:
      return SIM_INTERNAL_MMGSDI_INVALIDATION_CONTRADICTION_STATUS_V01;

    case MMGSDI_INCREASE_MAX_REACHED:
      return SIM_INTERNAL_MMGSDI_INCREASE_MAX_REACHED_V01;

    case MMGSDI_AUTH_FAIL:
      return SIM_INTERNAL_MMGSDI_AUTH_FAIL_V01;

    case MMGSDI_AUTS_FAIL:
      return SIM_INTERNAL_MMGSDI_AUTS_FAIL_V01;

    case MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED:
      return SIM_INTERNAL_MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED_V01;

    case MMGSDI_UIM_INTERNALLY_RESET:
      return SIM_INTERNAL_MMGSDI_UIM_INTERNALLY_RESET_V01;

    case MMGSDI_AUTH_ERROR_KEY_FRESHNESS_FAIL:
      return SIM_INTERNAL_MMGSDI_AUTH_ERROR_KEY_FRESHNESS_FAIL_V01;

    case MMGSDI_AUTH_ERROR_NO_MEM_SPACE:
      return SIM_INTERNAL_MMGSDI_AUTH_ERROR_NO_MEM_SPACE_V01;

    case MMGSDI_REF_DATA_NOT_FOUND:
      return SIM_INTERNAL_MMGSDI_REF_DATA_NOT_FOUND_V01;

    case MMGSDI_LOCKED_INIT_PARAM:
      return SIM_INTERNAL_MMGSDI_LOCKED_INIT_PARAM_V01;

    case MMGSDI_APP_NOT_INIT:
      return SIM_INTERNAL_MMGSDI_APP_NOT_INIT_V01;

    case MMGSDI_PUBLIC_KEY_CERT_NOT_INIT:
      return SIM_INTERNAL_MMGSDI_PUBLIC_KEY_CERT_NOT_INIT_V01;

    case MMGSDI_UCAST_AUTH_KEY_NOT_INIT:
      return SIM_INTERNAL_MMGSDI_UCAST_AUTH_KEY_NOT_INIT_V01;

    case MMGSDI_UCAST_DATA_BLOCK_NOT_INIT:
      return SIM_INTERNAL_MMGSDI_UCAST_DATA_BLOCK_NOT_INIT_V01;

    case MMGSDI_VERIFY_UCAST_FAILED:
      return SIM_INTERNAL_MMGSDI_VERIFY_UCAST_FAILED_V01;

    case MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE:
      return SIM_INTERNAL_MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE_V01;

    case MMGSDI_REFRESH_ADDITIONAL_FILES_READ:
      return SIM_INTERNAL_MMGSDI_REFRESH_ADDITIONAL_FILES_READ_V01;

    case MMGSDI_AUTH_ERROR_CAVE_NOT_RUN:
      return SIM_INTERNAL_MMGSDI_AUTH_ERROR_CAVE_NOT_RUN_V01;

    case MMGSDI_REFRESH_FAIL_INVALID_AID:
      return SIM_INTERNAL_MMGSDI_REFRESH_FAIL_INVALID_AID_V01;

    case MMGSDI_RPC_ERROR:
      return SIM_INTERNAL_MMGSDI_RPC_ERROR_V01;

    case MMGSDI_WRONG_CLASS:
      return SIM_INTERNAL_MMGSDI_WRONG_CLASS_V01;

    case MMGSDI_PERSO_CHECK_FAILED:
      return SIM_INTERNAL_MMGSDI_PERSO_CHECK_FAILED_V01;

    case MMGSDI_PERSO_INVALID_CK:
      return SIM_INTERNAL_MMGSDI_PERSO_INVALID_CK_V01;

    case MMGSDI_PERSO_CK_BLOCKED:
      return SIM_INTERNAL_MMGSDI_PERSO_CK_BLOCKED_V01;

    case MMGSDI_PERSO_INVALID_DATA:
      return SIM_INTERNAL_MMGSDI_PERSO_INVALID_DATA_V01;

    case MMGSDI_PERSO_PERM_DISABLED:
      return SIM_INTERNAL_MMGSDI_PERSO_PERM_DISABLED_V01;

    case MMGSDI_MANAGE_CHANNEL_FAILED:
      return SIM_INTERNAL_MMGSDI_MANAGE_CHANNEL_FAILED_V01;

    case MMGSDI_SELECT_AID_FAILED:
      return SIM_INTERNAL_MMGSDI_SELECT_AID_FAILED_V01;

    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_status received");
      return SIM_INTERNAL_MMGSDI_ERROR_V01;
  }
  return SIM_INTERNAL_MMGSDI_SUCCESS_V01;
} /* qmi_sim_internal_convert_mmgsdi_status */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SLOT_ID

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    mmgsdi_slot_id   : MMGSDI slot value
    sim_internal_mmgsdi_slot_id : SIM Internal slot value

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_slot_id
(
  mmgsdi_slot_id_enum_type              mmgsdi_slot_id,
  sim_internal_mmgsdi_slot_id_enum_v01 *sim_internal_mmgsdi_slot_id
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_slot_id 0x%x", mmgsdi_slot_id);
  ASSERT(sim_internal_mmgsdi_slot_id != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_slot_id = SIM_INTERNAL_MMGSDI_SLOT_ID_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *sim_internal_mmgsdi_slot_id = SIM_INTERNAL_MMGSDI_SLOT_1_V01;
      break;
    case MMGSDI_SLOT_2:
      *sim_internal_mmgsdi_slot_id = SIM_INTERNAL_MMGSDI_SLOT_2_V01;
      break;
    case MMGSDI_SLOT_AUTOMATIC:
      *sim_internal_mmgsdi_slot_id = SIM_INTERNAL_MMGSDI_SLOT_AUTOMATIC_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_slot_id received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_slot() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_APP_TYPE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_app_type   : MMGSDI App type

  RETURN VALUE
    sim_internal_mmgsdi_app_enum_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static sim_internal_mmgsdi_app_enum_v01 qmi_sim_internal_convert_mmgsdi_app_type
(
  mmgsdi_app_enum_type mmgsdi_app_type
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_app_type 0x%x", mmgsdi_app_type);

  switch(mmgsdi_app_type)
  {
    case MMGSDI_APP_NONE:
      return SIM_INTERNAL_MMGSDI_APP_NONE_V01;

    case MMGSDI_APP_SIM:
      return SIM_INTERNAL_MMGSDI_APP_SIM_V01;

    case MMGSDI_APP_RUIM:
      return SIM_INTERNAL_MMGSDI_APP_RUIM_V01;

    case MMGSDI_APP_USIM:
      return SIM_INTERNAL_MMGSDI_APP_USIM_V01;

    case MMGSDI_APP_CSIM:
      return SIM_INTERNAL_MMGSDI_APP_CSIM_V01;

    case MMGSDI_APP_UNKNOWN:
      return SIM_INTERNAL_MMGSDI_APP_UNKNOWN_V01;

    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_app_type received");
      break;
  }

  return SIM_INTERNAL_MMGSDI_APP_NONE_V01;
} /* qmi_sim_internal_convert_mmgsdi_app_type() */



/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_SIM_INTERNAL_MMGSDI_APP_TYPE

  DESCRIPTION
    To obtain MMGSDI enum from QMI SIM client-specific enum

  PARAMETERS
    sim_internal_mmgsdi_app_type   : QMI SIM MMGSDI App type

  RETURN VALUE
    mmgsdi_app_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static mmgsdi_app_enum_type qmi_sim_internal_convert_sim_internal_mmgsdi_app_type
(
  sim_internal_mmgsdi_app_enum_v01 sim_internal_mmgsdi_app_type
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_app_type 0x%x",
                 sim_internal_mmgsdi_app_type);

  switch(sim_internal_mmgsdi_app_type)
  {
    case SIM_INTERNAL_MMGSDI_APP_NONE_V01:
      return MMGSDI_APP_NONE;

    case SIM_INTERNAL_MMGSDI_APP_SIM_V01:
      return MMGSDI_APP_SIM;

    case SIM_INTERNAL_MMGSDI_APP_RUIM_V01:
      return MMGSDI_APP_RUIM;

    case SIM_INTERNAL_MMGSDI_APP_USIM_V01:
      return MMGSDI_APP_USIM;

    case SIM_INTERNAL_MMGSDI_APP_CSIM_V01:
      return MMGSDI_APP_CSIM;

    case SIM_INTERNAL_MMGSDI_APP_UNKNOWN_V01:
      return MMGSDI_APP_UNKNOWN;

    default:
      UIM_MSG_HIGH_0("Unhandled sim_internal_mmgsdi_app_type received");
      break;
  }

  return MMGSDI_APP_NONE;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_app_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_CARD_ERR_INFO

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_card_err_info

  RETURN VALUE
    sim_internal_mmgsdi_card_err_info_enum_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static sim_internal_mmgsdi_card_err_info_enum_v01 qmi_sim_internal_convert_mmgsdi_card_err_info
(
  mmgsdi_card_err_info_enum_type mmgsdi_card_err_info
)
{
  switch(mmgsdi_card_err_info)
  {
    case MMGSDI_CARD_ERR_UNKNOWN_ERROR :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_UNKNOWN_ERROR_V01;

    case MMGSDI_CARD_ERR_POLL_ERROR :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_POLL_ERROR_V01;

    case MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT_V01;

    case MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET_V01;

    case MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_V01;

    case MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_V01;

    case MMGSDI_CARD_ERR_VOLT_MISMATCH :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_VOLT_MISMATCH_V01;

    case MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_V01;

    case MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS_V01;

    case MMGSDI_CARD_ERR_MAXED_PARITY_ERROR :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAXED_PARITY_ERROR_V01;

    case MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR_V01;

    case MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR_V01;

    case MMGSDI_CARD_ERR_TRANS_TIMER_EXP :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_TRANS_TIMER_EXP_V01;

    case MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY_V01;

    case MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE_V01;

    case MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE_V01;

    case MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_V01;

    case MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_V01;

    case MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_V01;

    case MMGSDI_CARD_ERR_NO_PROTOCOL_AT_CARD_PUP :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_NO_PROTOCOL_AT_CARD_PUP_V01;

    case MMGSDI_CARD_ERR_CARD_REMOVED :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_CARD_REMOVED_V01;

    case MMGSDI_CARD_ERR_TECHNICAL_PROBLEM :
      return SIM_INTERNAL_MMGSDI_CARD_ERR_TECHNICAL_PROBLEM_V01;

    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_card_err_info received");
      break;
  }
  return SIM_INTERNAL_MMGSDI_CARD_ERR_UNKNOWN_ERROR_V01;
} /* qmi_sim_internal_convert_mmgsdi_card_err_info() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PIN_EVT

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_evt
    sim_internal_mmgsdi_pin_events

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_pin_evt
(
  mmgsdi_events_enum_type mmgsdi_evt,
  sim_internal_mmgsdi_pin_events_enum_v01 *sim_internal_mmgsdi_pin_events
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_app_type 0x%x", mmgsdi_evt);
  ASSERT(sim_internal_mmgsdi_pin_events != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_pin_events = SIM_INTERNAL_MMGSDI_PIN_EVENTS_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_evt)
  {
    case MMGSDI_PIN1_EVT:
      *sim_internal_mmgsdi_pin_events = SIM_INTERNAL_MMGSDI_PIN1_EVT_V01;
      break;
    case MMGSDI_PIN2_EVT:
      *sim_internal_mmgsdi_pin_events = SIM_INTERNAL_MMGSDI_PIN2_EVT_V01;
      break;
    case MMGSDI_UNIVERSAL_PIN_EVT:
      *sim_internal_mmgsdi_pin_events = SIM_INTERNAL_MMGSDI_UNIVERSAL_PIN_EVT_V01;
      break;
    case MMGSDI_HIDDENKEY_EVT:
      *sim_internal_mmgsdi_pin_events = SIM_INTERNAL_MMGSDI_HIDDENKEY_EVT_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_evt received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_pin_evt() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PIN_EVT

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmmgsdi_pin_id
    sim_internal_mmgsdi_pin_id

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_pin_id
(
  mmgsdi_pin_enum_type mmgsdi_pin_id,
  sim_internal_mmgsdi_pin_enum_v01 *sim_internal_mmgsdi_pin_id
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_pin_id 0x%x", mmgsdi_pin_id);
  ASSERT(sim_internal_mmgsdi_pin_id != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_PIN_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_pin_id)
  {
    case MMGSDI_PIN1:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_PIN1_V01;
      break;
    case MMGSDI_PIN2:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_PIN2_V01;
      break;
    case MMGSDI_UNIVERSAL_PIN:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_UNIVERSAL_PIN_V01;
      break;
    case MMGSDI_ADM1:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM1_V01;
      break;
    case MMGSDI_ADM2:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM2_V01;
      break;
    case MMGSDI_ADM3:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM3_V01;
      break;
    case MMGSDI_ADM4:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM4_V01;
      break;
    case MMGSDI_ADM5:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM5_V01;
      break;
    case MMGSDI_ADM6:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM6_V01;
      break;
    case MMGSDI_ADM7:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM7_V01;
      break;
    case MMGSDI_ADM8:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM8_V01;
      break;
    case MMGSDI_ADM9:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM9_V01;
      break;
    case MMGSDI_ADM10:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM10_V01;
      break;
    case MMGSDI_ADM11:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM11_V01;
      break;
    case MMGSDI_ADM12:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM12_V01;
      break;
    case MMGSDI_ADM13:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM13_V01;
      break;
    case MMGSDI_ADM14:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM14_V01;
      break;
    case MMGSDI_ADM15:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_ADM15_V01;
      break;
    case MMGSDI_HIDDENKEY:
      *sim_internal_mmgsdi_pin_id = SIM_INTERNAL_MMGSDI_HIDDENKEY_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_pin_id received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_pin_id() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PIN_STATUS

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_pin_status : MMGSDI Pin status
    sim_internal_mmgsdi_pin_status

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_pin_status
(
  mmgsdi_pin_status_enum_type mmgsdi_pin_status,
  sim_internal_mmgsdi_pin_status_enum_v01 *sim_internal_mmgsdi_pin_status
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_pin_status 0x%x", mmgsdi_pin_status);
  ASSERT(sim_internal_mmgsdi_pin_status != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_STATUS_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_pin_status)
  {
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_STATUS_NOT_INITIALIZED_V01;
      break;
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_ENABLED_NOT_VERIFIED_V01;
      break;
    case MMGSDI_PIN_ENABLED_VERIFIED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_ENABLED_VERIFIED_V01;
      break;
    case MMGSDI_PIN_DISABLED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_DISABLED_V01;
      break;
    case MMGSDI_PIN_BLOCKED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_BLOCKED_V01;
      break;
    case MMGSDI_PIN_PERM_BLOCKED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_PERM_BLOCKED_V01;
      break;
    case MMGSDI_PIN_UNBLOCKED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_UNBLOCKED_V01;
      break;
    case MMGSDI_PIN_CHANGED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_CHANGED_V01;
      break;
    case MMGSDI_PIN_NOT_FOUND:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_NOT_FOUND_V01;
      break;
    case MMGSDI_PIN_RETRY_COUNT_DECREASED:
      *sim_internal_mmgsdi_pin_status = SIM_INTERNAL_MMGSDI_PIN_RETRY_COUNT_DECREASED_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_pin_status received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_pin_status() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PIN_REPLACEMENT

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_pin_replacement
    sim_internal_mmgsdi_pin_replacement

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_pin_replacement
(
  mmgsdi_pin_replace_enum_type              mmgsdi_pin_replacement,
  sim_internal_mmgsdi_pin_replace_enum_v01 *sim_internal_mmgsdi_pin_replacement
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_pin_replacement 0x%x",
                 mmgsdi_pin_replacement);
  ASSERT(sim_internal_mmgsdi_pin_replacement != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_pin_replacement = SIM_INTERNAL_MMGSDI_PIN_REPLACE_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_pin_replacement)
  {
    case MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL:
      *sim_internal_mmgsdi_pin_replacement = SIM_INTERNAL_MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL_V01;
      break;
    case MMGSDI_PIN_REPLACED_BY_UNIVERSAL:
      *sim_internal_mmgsdi_pin_replacement = SIM_INTERNAL_MMGSDI_PIN_REPLACED_BY_UNIVERSAL_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_pin_replacement received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_pin_replacement() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PERSO_FEATURE

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

static qmi_csi_error qmi_sim_internal_convert_mmgsdi_perso_feature
(
  mmgsdi_perso_feature_enum_type              mmgsdi_perso_feature,
  sim_internal_mmgsdi_perso_feature_enum_v01 *sim_internal_mmgsdi_perso_feature
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_perso_feature_enum_table) /
                         sizeof(sim_internal_mmgsdi_perso_feature_enum_table_type));

  UIM_MSG_HIGH_1("Received mmgsdi_perso_feature 0x%x", mmgsdi_perso_feature);
  ASSERT(sim_internal_mmgsdi_perso_feature != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_perso_feature_enum_table[i].mmgsdi_perso_feature == mmgsdi_perso_feature)
    {
       *sim_internal_mmgsdi_perso_feature = sim_internal_mmgsdi_perso_feature_enum_table[i].sim_internal_mmgsdi_perso_feature;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
}/* qmi_sim_internal_convert_mmgsdi_perso_feature() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PERSO_STATUS

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_perso_status
    sim_internal_mmgsdi_perso_status

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_error qmi_sim_internal_convert_mmgsdi_perso_status
(
  mmgsdi_perso_status_enum_type              mmgsdi_perso_status,
  sim_internal_mmgsdi_perso_status_enum_v01 *sim_internal_mmgsdi_perso_status
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_perso_status 0x%x", mmgsdi_perso_status);
  ASSERT(sim_internal_mmgsdi_perso_status != NULL);

  switch(mmgsdi_perso_status)
  {
    case MMGSDI_PERSO_STATUS_NONE:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_NONE_V01;
      break;
    case MMGSDI_PERSO_STATUS_DONE:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_DONE_V01;
      break;
    case MMGSDI_PERSO_STATUS_LOCKED:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_LOCKED_V01;
      break;
    case MMGSDI_PERSO_STATUS_UNLOCKED:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_UNLOCKED_V01;
      break;
    case MMGSDI_PERSO_STATUS_BLOCKED:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_BLOCKED_V01;
      break;
    case MMGSDI_PERSO_STATUS_UNBLOCKED:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_UNBLOCKED_V01;
      break;
    case MMGSDI_PERSO_STATUS_SANITY_ERROR:
      *sim_internal_mmgsdi_perso_status = SIM_INTERNAL_MMGSDI_PERSO_STATUS_SANITY_ERROR_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_perso_status received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_perso_status() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_REFRESH_STAGE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_refresh_stage
    sim_internal_mmgsdi_refresh_stage

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_refresh_stage
(
  mmgsdi_refresh_stage_enum_type              mmgsdi_refresh_stage,
  sim_internal_mmgsdi_refresh_stage_enum_v01 *sim_internal_mmgsdi_refresh_stage
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_refresh_stage 0x%x", mmgsdi_refresh_stage);
  ASSERT(sim_internal_mmgsdi_refresh_stage != NULL);

  switch(mmgsdi_refresh_stage)
  {
    case MMGSDI_REFRESH_STAGE_START:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_START_V01;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED_V01;
      break;
    case MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED_V01;
      break;
    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_END_SUCCESS_V01;
      break;
    case MMGSDI_REFRESH_STAGE_END_FAILED:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_END_FAILED_V01;
      break;
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT_V01;
      break;
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN_V01;
      break;
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH:
      *sim_internal_mmgsdi_refresh_stage = SIM_INTERNAL_MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_refresh_stage received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_refresh_stage() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_REFRESH_MODE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

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
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_refresh_mode
(
  mmgsdi_refresh_mode_enum_type               mmgsdi_refresh_mode,
  sim_internal_mmgsdi_refresh_mode_enum_v01  *sim_internal_mmgsdi_refresh_mode
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_refresh_mode 0x%x", mmgsdi_refresh_mode);
  ASSERT(sim_internal_mmgsdi_refresh_mode != NULL);

  switch(mmgsdi_refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_RESET_V01;
      break;
    case MMGSDI_REFRESH_NAA_INIT:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_V01;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FCN:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_FCN_V01;
      break;
    case MMGSDI_REFRESH_NAA_FCN:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_NAA_FCN_V01;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_FULL_FCN_V01;
      break;
    case MMGSDI_REFRESH_NAA_APP_RESET:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_NAA_APP_RESET_V01;
      break;
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_3G_SESSION_RESET_V01;
      break;
    case MMGSDI_REFRESH_STEERING_OF_ROAMING:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_STEERING_OF_ROAMING_V01;
      break;
    case MMGSDI_REFRESH_RESET_AUTO:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_RESET_AUTO_V01;
      break;
    case MMGSDI_REFRESH_APP_RESET_AUTO:
      *sim_internal_mmgsdi_refresh_mode = SIM_INTERNAL_MMGSDI_REFRESH_APP_RESET_AUTO_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_refresh_mode received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_refresh_mode() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_interna_mmgsdi_refresh_mode 0x%x",
                 sim_internal_mmgsdi_refresh_mode);
  ASSERT(mmgsdi_refresh_mode != NULL);

  switch(sim_internal_mmgsdi_refresh_mode)
  {
    case SIM_INTERNAL_MMGSDI_REFRESH_RESET_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_RESET;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_FCN_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FCN;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_NAA_FCN_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_FCN;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_NAA_INIT_FULL_FCN_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_INIT_FULL_FCN;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_NAA_APP_RESET_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_NAA_APP_RESET;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_3G_SESSION_RESET_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_3G_SESSION_RESET;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_STEERING_OF_ROAMING_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_STEERING_OF_ROAMING;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_RESET_AUTO_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_RESET_AUTO;
      break;
    case SIM_INTERNAL_MMGSDI_REFRESH_APP_RESET_AUTO_V01:
      *mmgsdi_refresh_mode = MMGSDI_REFRESH_APP_RESET_AUTO;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_refresh_mode received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_refresh_mode */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_FILE_STRUCTURE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_file
    sim_internal_mmgsdi_file

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_file_structure
(
  mmgsdi_file_structure_enum_type              mmgsdi_file,
  sim_internal_mmgsdi_file_structure_enum_v01 *sim_internal_mmgsdi_file
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_file 0x%x", mmgsdi_file);
  ASSERT(sim_internal_mmgsdi_file != NULL);

  switch(mmgsdi_file)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      *sim_internal_mmgsdi_file = SIM_INTERNAL_MMGSDI_LINEAR_FIXED_FILE_V01;
      break;
    case MMGSDI_CYCLIC_FILE:
      *sim_internal_mmgsdi_file = SIM_INTERNAL_MMGSDI_CYCLIC_FILE_V01;
      break;
    case MMGSDI_TRANSPARENT_FILE:
      *sim_internal_mmgsdi_file = SIM_INTERNAL_MMGSDI_TRANSPARENT_FILE_V01;
      break;
    case MMGSDI_MASTER_FILE:
      *sim_internal_mmgsdi_file = SIM_INTERNAL_MMGSDI_MASTER_FILE_V01;
      break;
    case MMGSDI_DEDICATED_FILE:
      *sim_internal_mmgsdi_file = SIM_INTERNAL_MMGSDI_DEDICATED_FILE_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_file received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_file_structure() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_PROTECTION_METHOD

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_file_security
    sim_internal_mmgsdi_file_security

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_protection_method
(
  mmgsdi_file_security_enum_type              mmgsdi_file_security,
  sim_internal_mmgsdi_file_security_enum_v01 *sim_internal_mmgsdi_file_security
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_file_security 0x%x", mmgsdi_file_security);
  ASSERT(sim_internal_mmgsdi_file_security != NULL);

  switch(mmgsdi_file_security)
  {
    case MMGSDI_ALWAYS_ALLOWED:
      *sim_internal_mmgsdi_file_security = SIM_INTERNAL_MMGSDI_ALWAYS_ALLOWED_V01;
      break;
    case MMGSDI_NEVER_ALLOWED:
      *sim_internal_mmgsdi_file_security = SIM_INTERNAL_MMGSDI_NEVER_ALLOWED_V01;
      break;
    case MMGSDI_AND_ALLOWED:
      *sim_internal_mmgsdi_file_security = SIM_INTERNAL_MMGSDI_AND_ALLOWED_V01;
      break;
    case MMGSDI_OR_ALLOWED:
      *sim_internal_mmgsdi_file_security = SIM_INTERNAL_MMGSDI_OR_ALLOWED_V01;
      break;
    case MMGSDI_SINGLE_ALLOWED:
      *sim_internal_mmgsdi_file_security = SIM_INTERNAL_MMGSDI_SINGLE_ALLOWED_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled mmgsdi_file_security received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_protection_method() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_FILE

  DESCRIPTION
    To obtain QMI SIM client-specific enum from MMGSDI enum

  PARAMETERS
    mmgsdi_file_enum
    sim_internal_mmgsdi_file_enum

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_file
(
  mmgsdi_file_enum_type                     mmgsdi_file_enum,
  sim_internal_mmgsdi_file_enum_v01        *sim_internal_mmgsdi_file_enum
)
{
  UIM_MSG_HIGH_1("Received qmi_sim_internal_convert_mmgsdi_file 0x%x",
                 mmgsdi_file_enum);
  ASSERT(sim_internal_mmgsdi_file_enum != NULL);

  switch(mmgsdi_file_enum)
  {
    case MMGSDI_NO_FILE_ENUM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_NO_FILE_ENUM_V01;
      break;

    case MMGSDI_ICCID:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_ICCID_V01;
      break;

    case MMGSDI_ELP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_ELP_V01;
      break;

    case MMGSDI_DIR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_DIR_V01;
      break;

    case MMGSDI_ARR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_ARR_V01;
      break;

    case MMGSDI_GSM_LP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_LP_V01;
      break;

    case MMGSDI_GSM_IMSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMSI_V01;
      break;

    case MMGSDI_GSM_KC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_KC_V01;
      break;

    case MMGSDI_GSM_PLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_PLMN_V01;
      break;

    case MMGSDI_GSM_HPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_HPLMN_V01;
      break;

    case MMGSDI_GSM_ACM_MAX:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ACM_MAX_V01;
      break;

    case MMGSDI_GSM_SST:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SST_V01;
      break;

    case MMGSDI_GSM_ACM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ACM_V01;
      break;

    case MMGSDI_GSM_GID1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_GID1_V01;
      break;

    case MMGSDI_GSM_GID2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_GID2_V01;
      break;

    case MMGSDI_GSM_SPN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SPN_V01;
      break;

    case MMGSDI_GSM_PUCT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_PUCT_V01;
      break;

    case MMGSDI_GSM_CBMI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CBMI_V01;
      break;

    case MMGSDI_GSM_BCCH:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_BCCH_V01;
      break;

    case MMGSDI_GSM_ACC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ACC_V01;
      break;

    case MMGSDI_GSM_FPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_FPLMN_V01;
      break;

    case MMGSDI_GSM_LOCI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_LOCI_V01;
      break;

    case MMGSDI_GSM_AD:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_AD_V01;
      break;

    case MMGSDI_GSM_PHASE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_PHASE_V01;
      break;

    case MMGSDI_GSM_VGCS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_VGCS_V01;
      break;

    case MMGSDI_GSM_VGCSS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_VGCSS_V01;
      break;

    case MMGSDI_GSM_VBS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_VBS_V01;
      break;

    case MMGSDI_GSM_VBSS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_VBSS_V01;
      break;

    case MMGSDI_GSM_EMLPP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_EMLPP_V01;
      break;

    case MMGSDI_GSM_AAEM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_AAEM_V01;
      break;

    case MMGSDI_GSM_CBMID:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CBMID_V01;
      break;

    case MMGSDI_GSM_ECC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ECC_V01;
      break;

    case MMGSDI_GSM_CBMIR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CBMIR_V01;
      break;

    case MMGSDI_GSM_DCK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_DCK_V01;
      break;

    case MMGSDI_GSM_CNL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CNL_V01;
      break;

    case MMGSDI_GSM_NIA:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_NIA_V01;
      break;

    case MMGSDI_GSM_KCGPRS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_KCGPRS_V01;
      break;

    case MMGSDI_GSM_LOCIGPRS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_LOCIGPRS_V01;
      break;

    case MMGSDI_GSM_SUME:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SUME_V01;
      break;

    case MMGSDI_GSM_PLMNWACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_PLMNWACT_V01;
      break;

    case MMGSDI_GSM_OPLMNWACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_OPLMNWACT_V01;
      break;

    case MMGSDI_GSM_HPLMNACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_HPLMNACT_V01;
      break;

    case MMGSDI_GSM_CPBCCH:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CPBCCH_V01;
      break;

    case MMGSDI_GSM_INVSCAN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_INVSCAN_V01;
      break;

    case MMGSDI_GSM_RPLMNAT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_RPLMNAT_V01;
      break;

    case MMGSDI_GSM_PNN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_PNN_V01;
      break;

    case MMGSDI_GSM_OPL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_OPL_V01;
      break;

    case MMGSDI_GSM_MBDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_MBDN_V01;
      break;

    case MMGSDI_GSM_EXT6:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_EXT6_V01;
      break;

    case MMGSDI_GSM_MBI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_MBI_V01;
      break;

    case MMGSDI_GSM_MWIS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_MWIS_V01;
      break;

    case MMGSDI_GSM_EXT1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_EXT1_V01;
      break;

    case MMGSDI_GSM_SPDI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SPDI_V01;
      break;

    case MMGSDI_GSM_CFIS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CFIS_V01;
      break;

    case MMGSDI_GSM_VMWI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_VMWI_V01;
      break;

    case MMGSDI_GSM_SVC_STR_TBL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SVC_STR_TBL_V01;
      break;

    case MMGSDI_GSM_CFF:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CFF_V01;
      break;

    case MMGSDI_GSM_ONS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ONS_V01;
      break;

    case MMGSDI_GSM_CSP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CSP_V01;
      break;

    case MMGSDI_GSM_CPHSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_CPHSI_V01;
      break;

    case MMGSDI_GSM_MN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_MN_V01;
      break;

    case MMGSDI_GSM_SAI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SAI_V01;
      break;

    case MMGSDI_GSM_SLL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_SLL_V01;
      break;

    case MMGSDI_GSM_MEXE_ST:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_MEXE_ST_V01;
      break;

    case MMGSDI_GSM_ORPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ORPK_V01;
      break;

    case MMGSDI_GSM_ARPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ARPK_V01;
      break;

    case MMGSDI_GSM_TPRPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_TPRPK_V01;
      break;

    case MMGSDI_GSM_IMG:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG_V01;
      break;

    case MMGSDI_GSM_IMG1INST1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG1INST1_V01;
      break;

    case MMGSDI_GSM_IMG1INST2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG1INST2_V01;
      break;

    case MMGSDI_GSM_IMG1INST3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG1INST3_V01;
      break;

    case MMGSDI_GSM_IMG2INST1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG2INST1_V01;
      break;

    case MMGSDI_GSM_IMG2INST2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG2INST2_V01;
      break;

    case MMGSDI_GSM_IMG2INST3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG2INST3_V01;
      break;

    case MMGSDI_GSM_IMG3INST1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG3INST1_V01;
      break;

    case MMGSDI_GSM_IMG3INST2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG3INST2_V01;
      break;

    case MMGSDI_GSM_IMG3INST3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_IMG3INST3_V01;
      break;

    case MMGSDI_GSM_ONS_SHORT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_GSM_ONS_SHORT_V01;
      break;

    case MMGSDI_IMAGE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_IMAGE_V01;
      break;

    case MMGSDI_USIM_LI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_LI_V01;
      break;

    case MMGSDI_USIM_IMSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_IMSI_V01;
      break;

    case MMGSDI_USIM_KEYS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_KEYS_V01;
      break;

    case MMGSDI_USIM_KEYSPS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_KEYSPS_V01;
      break;

    case MMGSDI_USIM_PLMNWACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PLMNWACT_V01;
      break;

    case MMGSDI_USIM_UPLMNSEL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_UPLMNSEL_V01;
      break;

    case MMGSDI_USIM_HPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_HPLMN_V01;
      break;

    case MMGSDI_USIM_ACM_MAX:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ACM_MAX_V01;
      break;

    case MMGSDI_USIM_UST:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_UST_V01;
      break;

    case MMGSDI_USIM_ACM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ACM_V01;
      break;

    case MMGSDI_USIM_GID1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_GID1_V01;
      break;

    case MMGSDI_USIM_GID2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_GID2_V01;
      break;

    case MMGSDI_USIM_SPN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SPN_V01;
      break;

    case MMGSDI_USIM_PUCT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PUCT_V01;
      break;

    case MMGSDI_USIM_CBMI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CBMI_V01;
      break;

    case MMGSDI_USIM_ACC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ACC_V01;
      break;

    case MMGSDI_USIM_FPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_FPLMN_V01;
      break;

    case MMGSDI_USIM_LOCI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_LOCI_V01;
      break;

    case MMGSDI_USIM_AD:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_AD_V01;
      break;

    case MMGSDI_USIM_CBMID:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CBMID_V01;
      break;

    case MMGSDI_USIM_ECC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ECC_V01;
      break;

    case MMGSDI_USIM_CBMIR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CBMIR_V01;
      break;

    case MMGSDI_USIM_PSLOCI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PSLOCI_V01;
      break;

    case MMGSDI_USIM_FDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_FDN_V01;
      break;

    case MMGSDI_USIM_SMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SMS_V01;
      break;

    case MMGSDI_USIM_MSISDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MSISDN_V01;
      break;

    case MMGSDI_USIM_SMSP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SMSP_V01;
      break;

    case MMGSDI_USIM_SMSS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SMSS_V01;
      break;

    case MMGSDI_USIM_SDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SDN_V01;
      break;

    case MMGSDI_USIM_EXT2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT2_V01;
      break;

    case MMGSDI_USIM_EXT3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT3_V01;
      break;

    case MMGSDI_USIM_SMSR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SMSR_V01;
      break;

    case MMGSDI_USIM_ICI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ICI_V01;
      break;

    case MMGSDI_USIM_OCI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_OCI_V01;
      break;

    case MMGSDI_USIM_ICT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ICT_V01;
      break;

    case MMGSDI_USIM_OCT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_OCT_V01;
      break;

    case MMGSDI_USIM_EXT5:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT5_V01;
      break;

    case MMGSDI_USIM_CCP2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CCP2_V01;
      break;

    case MMGSDI_USIM_EMLPP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EMLPP_V01;
      break;

    case MMGSDI_USIM_AAEM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_AAEM_V01;
      break;

    case MMGSDI_USIM_GMSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_GMSI_V01;
      break;

    case MMGSDI_USIM_HIDDENKEY:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_HIDDENKEY_V01;
      break;

    case MMGSDI_USIM_BDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_BDN_V01;
      break;

    case MMGSDI_USIM_EXT4:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT4_V01;
      break;

    case MMGSDI_USIM_CMI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CMI_V01;
      break;

    case MMGSDI_USIM_EST:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EST_V01;
      break;

    case MMGSDI_USIM_ACL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ACL_V01;
      break;

    case MMGSDI_USIM_DCK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_DCK_V01;
      break;

    case MMGSDI_USIM_CNL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CNL_V01;
      break;

    case MMGSDI_USIM_START_HFN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_START_HFN_V01;
      break;

    case MMGSDI_USIM_THRESHOLD:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_THRESHOLD_V01;
      break;

    case MMGSDI_USIM_OPLMNWACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_OPLMNWACT_V01;
      break;

    case MMGSDI_USIM_OPLMNSEL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_OPLMNSEL_V01;
      break;

    case MMGSDI_USIM_HPLMNWACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_HPLMNWACT_V01;
      break;

    case MMGSDI_USIM_ARR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ARR_V01;
      break;

    case MMGSDI_USIM_RPLMNACT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_RPLMNACT_V01;
      break;

    case MMGSDI_USIM_NETPAR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_NETPAR_V01;
      break;

    case MMGSDI_USIM_PNN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PNN_V01;
      break;

    case MMGSDI_USIM_OPL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_OPL_V01;
      break;

    case MMGSDI_USIM_MBDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MBDN_V01;
      break;

    case MMGSDI_USIM_EXT6:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT6_V01;
      break;

    case MMGSDI_USIM_MBI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MBI_V01;
      break;

    case MMGSDI_USIM_MWIS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MWIS_V01;
      break;

    case MMGSDI_USIM_SPDI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SPDI_V01;
      break;

    case MMGSDI_USIM_EHPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EHPLMN_V01;
      break;

    case MMGSDI_USIM_CFIS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CFIS_V01;
      break;

    case MMGSDI_USIM_VGCSCA:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_VGCSCA_V01;
      break;

    case MMGSDI_USIM_VBSCA:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_VBSCA_V01;
      break;

    case MMGSDI_USIM_GBABP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_GBABP_V01;
      break;

    case MMGSDI_USIM_GBANL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_GBANL_V01;
      break;

    case MMGSDI_USIM_MSK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MSK_V01;
      break;

    case MMGSDI_USIM_MUK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MUK_V01;
      break;

    case MMGSDI_USIM_VMWI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_VMWI_V01;
      break;

    case MMGSDI_USIM_SVC_STR_TBL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SVC_STR_TBL_V01;
      break;

    case MMGSDI_USIM_CFF:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CFF_V01;
      break;

    case MMGSDI_USIM_ONS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ONS_V01;
      break;

    case MMGSDI_USIM_CSP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CSP_V01;
      break;

    case MMGSDI_USIM_CPHSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CPHSI_V01;
      break;

    case MMGSDI_USIM_MN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MN_V01;
      break;

    case MMGSDI_USIM_PBR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PBR_V01;
      break;

    case MMGSDI_USIM_PSC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PSC_V01;
      break;

    case MMGSDI_USIM_CC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CC_V01;
      break;

    case MMGSDI_USIM_PUID:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PUID_V01;
      break;

    case MMGSDI_USIM_KC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_KC_V01;
      break;

    case MMGSDI_USIM_KCGPRS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_KCGPRS_V01;
      break;

    case MMGSDI_USIM_CPBCCH:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CPBCCH_V01;
      break;

    case MMGSDI_USIM_INVSCAN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_INVSCAN_V01;
      break;

    case MMGSDI_USIM_MEXE_ST:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MEXE_ST_V01;
      break;

    case MMGSDI_USIM_ORPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ORPK_V01;
      break;

    case MMGSDI_USIM_ARPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ARPK_V01;
      break;

    case MMGSDI_USIM_TPRPK:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_TPRPK_V01;
      break;

    case MMGSDI_USIM_MMSN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MMSN_V01;
      break;

    case MMGSDI_USIM_MMSICP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MMSICP_V01;
      break;

    case MMGSDI_USIM_MMSUP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MMSUP_V01;
      break;

    case MMGSDI_USIM_MMSUCP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_MMSUCP_V01;
      break;

    case MMGSDI_USIM_EXT8:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EXT8_V01;
      break;

    case MMGSDI_USIM_EPSLOCI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EPSLOCI_V01;
      break;

    case MMGSDI_USIM_EPSNSC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EPSNSC_V01;
      break;

    case MMGSDI_USIM_SPNI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_SPNI_V01;
      break;

    case MMGSDI_USIM_PNNI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_PNNI_V01;
      break;

    case MMGSDI_USIM_NCP_IP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_NCP_IP_V01;
      break;

    case MMGSDI_USIM_HPLMNDAI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_HPLMNDAI_V01;
      break;

    case MMGSDI_USIM_ACSGL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ACSGL_V01;
      break;

    case MMGSDI_USIM_CSGT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_CSGI_V01;
      break;

    case MMGSDI_USIM_HNBN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_HNBN_V01;
      break;

    case MMGSDI_USIM_EHPLMNPI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_EHPLMNPI_V01;
      break;

    case MMGSDI_USIM_LRPLMNSI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_LRPLMNSI_V01;
      break;

    case MMGSDI_USIM_ONS_SHORT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_ONS_SHORT_V01;
      break;

    case MMGSDI_SIM_7F40_PROP1_DFS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_DFS_V01;
      break;

    case MMGSDI_SIM_7F40_PROP1_D2FS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_D2FS_V01;
      break;

    case MMGSDI_SIM_7F40_PROP1_CSP2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_CSP2_V01;
      break;

    case MMGSDI_SIM_7F40_PROP1_PARAMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_PARAMS_V01;
      break;

    case MMGSDI_USIM_7F40_PROP1_DFS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_DFS_V01;
      break;

    case MMGSDI_USIM_7F40_PROP1_D2FS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_D2FS_V01;
      break;

    case MMGSDI_USIM_7F40_PROP1_CSP2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_CSP2_V01;
      break;

    case MMGSDI_USIM_7F40_PROP1_PARAMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_PARAMS_V01;
      break;

    case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F66_PROP1_ACT_HPLMN_V01;
      break;

    case MMGSDI_SIM_7F66_PROP1_SPT_TABLE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F66_PROP1_SPT_TABLE_V01;
      break;

    case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_ACT_HPLMN_V01;
      break;

    case MMGSDI_USIM_7F66_PROP1_SPT_TABLE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_SPT_TABLE_V01;
      break;

    case MMGSDI_USIM_7F66_PROP1_RAT:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RAT_V01;
      break;

    case MMGSDI_USIM_7F66_PROP1_RPM_EF:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RPM_EF_V01;
      break;

    case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RPM_PARAM_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_HZ:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_HZ_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_CACHE1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE1_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_CACHE2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE2_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_CACHE3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE3_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_CACHE4:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE4_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_TAGS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_TAGS_V01;
      break;

    case MMGSDI_SIM_7F43_PROP1_SETTINGS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SETTINGS_V01;
      break;

    case MMGSDI_TELECOM_ADN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_ADN_V01;
      break;

    case MMGSDI_TELECOM_FDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_FDN_V01;
      break;

    case MMGSDI_TELECOM_SMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SMS_V01;
      break;

    case MMGSDI_TELECOM_CCP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_CCP_V01;
      break;

    case MMGSDI_TELECOM_ECCP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_ECCP_V01;
      break;

    case MMGSDI_TELECOM_MSISDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MSISDN_V01;
      break;

    case MMGSDI_TELECOM_SMSP:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SMSP_V01;
      break;

    case MMGSDI_TELECOM_SMSS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SMSS_V01;
      break;

    case MMGSDI_TELECOM_LND:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_LND_V01;
      break;

    case MMGSDI_TELECOM_SDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SDN_V01;
      break;

    case MMGSDI_TELECOM_EXT1:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_EXT1_V01;
      break;

    case MMGSDI_TELECOM_EXT2:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_EXT2_V01;
      break;

    case MMGSDI_TELECOM_EXT3:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_EXT3_V01;
      break;

    case MMGSDI_TELECOM_BDN:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_BDN_V01;
      break;

    case MMGSDI_TELECOM_EXT4:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_EXT4_V01;
      break;

    case MMGSDI_TELECOM_SMSR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SMSR_V01;
      break;

    case MMGSDI_TELECOM_CMI:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_CMI_V01;
      break;

    case MMGSDI_TELECOM_SUME:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_SUME_V01;
      break;

    case MMGSDI_TELECOM_ARR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_ARR_V01;
      break;

    case MMGSDI_IMAGE_FILE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_IMAGE_FILE_V01;
      break;

    case MMGSDI_TELECOM_PBR:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_PBR_V01;
      break;

    case MMGSDI_TELECOM_PSC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_PSC_V01;
      break;

    case MMGSDI_TELECOM_CC:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_CC_V01;
      break;

    case MMGSDI_TELECOM_PUID:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_PUID_V01;
      break;

    case MMGSDI_TELECOM_MML:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MML_V01;
      break;

    case MMGSDI_TELECOM_MMDF:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MMDF_V01;
      break;

    case MMGSDI_TELECOM_MLPL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MLPL_V01;
      break;

    case MMGSDI_TELECOM_MSPL:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MSPL_V01;
      break;

    case MMGSDI_TELECOM_MMSSMODE:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_TELECOM_MMSSMODE_V01;
      break;

    case MMGSDI_CDMA_SMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CDMA_SMS_V01;
      break;

    case MMGSDI_CDMA_SMS_PARAMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CDMA_SMS_PARAMS_V01;
      break;

    case MMGSDI_CDMA_SMS_STATUS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CDMA_SMS_STATUS_V01;
      break;

    case MMGSDI_CSIM_SMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CSIM_SMS_V01;
      break;

    case MMGSDI_CSIM_SMS_PARAMS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CSIM_SMS_PARAMS_V01;
      break;

    case MMGSDI_CSIM_SMS_STATUS:
      *sim_internal_mmgsdi_file_enum = SIM_INTERNAL_MMGSDI_CSIM_SMS_STATUS_V01;
      break;

    default:
      UIM_MSG_HIGH_0("Unhandled sim_internal_sim_internal_sim_internal_mmgsdi_file_enum received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_mmgsdi_file() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SESSION_TYPE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    mmgsdi_session_type
    sim_internal_mmgsdi_session_type

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_session_type
(
  mmgsdi_session_type_enum_type               mmgsdi_session_type,
  sim_internal_mmgsdi_session_type_enum_v01  *sim_internal_mmgsdi_session_type
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_session_type 0x%x", mmgsdi_session_type);
  ASSERT(sim_internal_mmgsdi_session_type != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_SESSION_TYPE_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_GW_PROV_PRI_SESSION_V01;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_1X_PROV_PRI_SESSION_V01;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_GW_PROV_SEC_SESSION_V01;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_1X_PROV_SEC_SESSION_V01;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_1_V01;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_2_V01;
      break;
    case MMGSDI_CARD_SESSION_SLOT_1:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_CARD_SESSION_SLOT_1_V01;
      break;
    case MMGSDI_CARD_SESSION_SLOT_2:
      *sim_internal_mmgsdi_session_type = SIM_INTERNAL_MMGSDI_CARD_SESSION_SLOT_2_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled session_type received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_session_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_APP_STATE

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    mmgsdi_app_state
    sim_internal_mmgsdi_app_state

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_app_state
(
  mmgsdi_app_state_enum_type               mmgsdi_app_state,
  sim_internal_mmgsdi_app_state_enum_v01  *sim_internal_mmgsdi_app_state
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_app_state 0x%x", mmgsdi_app_state);
  ASSERT(sim_internal_mmgsdi_app_state != NULL);

  //note: Temporary Fix (need to make IDL changes which impact both QSC and 8960)
  *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_ENUM_MAX_ENUM_VAL_V01;

  switch(mmgsdi_app_state)
  {
    case   MMGSDI_APP_STATE_UNKNOWN:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_UNKNOWN_V01;
      break;

    case   MMGSDI_APP_STATE_DETECTED:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_DETECTED_V01;
      break;

    case   MMGSDI_APP_STATE_DEACTIVATED:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_DEACTIVATED_V01;
      break;

    case   MMGSDI_APP_STATE_ACTIVATED:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_ACTIVATED_V01;
      break;

    case   MMGSDI_APP_STATE_PIN_EVT_SENT:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_PIN_EVT_SENT_V01;
      break;

    case   MMGSDI_APP_STATE_READY_FOR_PERSO:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_READY_FOR_PERSO_V01;
      break;

    case   MMGSDI_APP_STATE_PERSO_VERIFIED:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_PERSO_VERIFIED_V01;
      break;

    case   MMGSDI_APP_STATE_READY:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_READY_V01;
      break;

    case   MMGSDI_APP_STATE_ILLEGAL:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_ILLEGAL_V01;
      break;

    case   MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
      *sim_internal_mmgsdi_app_state =   SIM_INTERNAL_MMGSDI_APP_STATE_WAIT_FOR_SUB_OK_V01;
      break;

    default:
      UIM_MSG_HIGH_0("Unhandled session_type received");
      break;
      //return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_app_state() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_REFRESH_ORIG

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from MMGSDI enum

  PARAMETERS
    mmgsdi_refresh_orig
    sim_internal_mmgsdi_refresh_orig

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_convert_mmgsdi_refresh_orig
(
  mmgsdi_refresh_orig_enum_type               mmgsdi_refresh_orig,
  sim_internal_mmgsdi_refresh_orig_enum_v01  *sim_internal_mmgsdi_refresh_orig
)
{
  UIM_MSG_HIGH_1("Received mmgsdi_refresh_orig 0x%x", mmgsdi_refresh_orig);
  ASSERT(sim_internal_mmgsdi_refresh_orig != NULL);

  switch(mmgsdi_refresh_orig)
  {
    case MMGSDI_REFRESH_ORIG_REFRESH_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_REFRESH_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_PUP_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_PUP_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_REG_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_REG_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_DEREG_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_DEREG_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_COMPLETE_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ_V01;
      break;
    case MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ:
      *sim_internal_mmgsdi_refresh_orig = SIM_INTERNAL_MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ_V01;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled auth_context received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_mmgsdi_refresh_orig() */


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
  mmgsdi_protocol_enum_type               mmgsdi_protocol,
  sim_internal_mmgsdi_protocol_enum_v01  *sim_internal_mmgsdi_protocol
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_protocol_enum_table) /
                         sizeof(sim_internal_mmgsdi_protocol_enum_table_type));

  UIM_MSG_HIGH_1("Received qmi_sim_internal_convert_mmgsdi_protocol 0x%x",
                 mmgsdi_protocol);
  ASSERT(sim_internal_mmgsdi_protocol != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_protocol_enum_table[i].mmgsdi_protocol == mmgsdi_protocol)
    {
       *sim_internal_mmgsdi_protocol = sim_internal_mmgsdi_protocol_enum_table[i].sim_internal_mmgsdi_protocol;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_protocol() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_MMGSDI_SEARCH_TYPE

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
qmi_csi_error qmi_sim_internal_convert_mmgsdi_search_type
(
  sim_internal_mmgsdi_search_enum_v01        sim_internal_search_type,
  mmgsdi_search_enum_type                   *search_type
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_search_type_enum_table) /
                         sizeof(sim_internal_mmgsdi_search_type_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_search_type 0x%x",
                 sim_internal_search_type);
  ASSERT(search_type != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_search_type_enum_table[i].sim_internal_search_type == sim_internal_search_type)
    {
       *search_type = sim_internal_mmgsdi_search_type_enum_table[i].search_type;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_search_type() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_search_direction_enum_table) /
                         sizeof(sim_internal_mmgsdi_search_direction_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_search_type 0x%x",
                 sim_internal_search_direction);
  ASSERT(search_direction != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_search_direction_enum_table[i].sim_internal_search_direction == sim_internal_search_direction)
    {
       *search_direction = sim_internal_mmgsdi_search_direction_enum_table[i].search_direction;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_search_direction() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_search_option_enum_table) /
                         sizeof(sim_internal_mmgsdi_search_option_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_search_option 0x%x",
                 sim_internal_search_option);
  ASSERT(search_option != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_search_option_enum_table[i].sim_internal_search_option == sim_internal_search_option)
    {
       *search_option = sim_internal_mmgsdi_search_option_enum_table[i].search_option;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_search_option() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_seek_direction_enum_table) /
                         sizeof(sim_internal_mmgsdi_seek_direction_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_seek_type 0x%x",
                 sim_internal_seek_direction);
  ASSERT(seek_direction != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_seek_direction_enum_table[i].sim_internal_seek_direction == sim_internal_seek_direction)
    {
       *seek_direction = sim_internal_mmgsdi_seek_direction_enum_table[i].seek_direction;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_seek_direction() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CONVERT_GSTK_STATUS

  DESCRIPTION
    To obtain QMI SIM client-specific enum value from GSTK enum

  PARAMETERS
    gstk_status

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static sim_internal_gstk_status_enum_v01 qmi_sim_internal_convert_gstk_status
(
  gstk_status_enum_type                        gstk_status
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_status_enum_table) /
                          sizeof(sim_internal_gstk_status_enum_table_type));

  UIM_MSG_HIGH_1("Received gstk_status 0x%x", gstk_status);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_status_enum_table[i].gstk_status == gstk_status)
    {
       return sim_internal_gstk_status_enum_table[i].sim_internal_gstk_status;
    }
  }
  return SIM_INTERNAL_GSTK_ERROR_V01;
} /* qmi_sim_internal_convert_gstk_status() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_cmd_group_enum_table) /
                          sizeof(sim_internal_gstk_cmd_group_enum_table_type));

  UIM_MSG_HIGH_1("Received gstk_cmd_group 0x%x", gstk_cmd_group);
  ASSERT(sim_internal_gstk_cmd_group != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_cmd_group_enum_table[i].gstk_cmd_group == gstk_cmd_group)
    {
       *sim_internal_gstk_cmd_group = sim_internal_gstk_cmd_group_enum_table[i].sim_internal_gstk_cmd_group;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_cmd_group() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_tal_rpt_status_enum_table) /
                         sizeof(sim_internal_gstk_tal_rpt_status_enum_table_type));

  UIM_MSG_HIGH_1("Received gstk_rpt_status 0x%x", gstk_rpt_status);
  ASSERT(sim_internal_gstk_rpt_status != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_tal_rpt_status_enum_table[i].gstk_rpt_status == gstk_rpt_status)
    {
       *sim_internal_gstk_rpt_status = sim_internal_gstk_tal_rpt_status_enum_table[i].sim_internal_gstk_rpt_status;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_rpt_status() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_slot_id_enum_table) /
                         sizeof(sim_internal_gstk_slot_id_enum_table_type));

  UIM_MSG_HIGH_1("Received gstk_slot_id 0x%x", gstk_slot_id);
  ASSERT(sim_internal_slot_id != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_slot_id_enum_table[i].gstk_slot_id == gstk_slot_id)
    {
       *sim_internal_slot_id = sim_internal_gstk_slot_id_enum_table[i].sim_internal_slot_id;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_slot_id() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_access_technology_enum_table) /
                         sizeof(sim_internal_gstk_access_technology_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_gstk_rat 0x%x", sim_internal_gstk_rat);
  ASSERT(gstk_rat != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_access_technology_enum_table[i].sim_internal_gstk_rat == sim_internal_gstk_rat)
    {
       *gstk_rat = sim_internal_gstk_access_technology_enum_table[i].gstk_rat;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_access_technology() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_location_status_enum_table) /
                         sizeof(sim_internal_gstk_location_status_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_gstk_ls 0x%x", sim_internal_gstk_ls);
  ASSERT(gstk_ls != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_location_status_enum_table[i].sim_internal_gstk_ls == sim_internal_gstk_ls)
    {
       *gstk_ls = sim_internal_gstk_location_status_enum_table[i].gstk_ls;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_location_status() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_AID_TYPE

  DESCRIPTION
    To populate mmgsdi aid type to sim internal mmgsdi aid type

  PARAMETERS
    aid_type_len
    mmgsdi_aid_info
    sim_internal_aid_info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_sim_internal_populate_mmgsdi_aid_type
(
  uint32                            aid_type_len,
  const mmgsdi_aid_type            *mmgsdi_aid_info,
  sim_internal_mmgsdi_aid_type_v01 *sim_internal_aid_info
)
{
  uint8    index = 0;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_aid_type");
  ASSERT(mmgsdi_aid_info != NULL);
  ASSERT(sim_internal_aid_info != NULL);

  for (index=0; index < aid_type_len; index++)
  {
    sim_internal_aid_info[index].app_type
      = qmi_sim_internal_convert_mmgsdi_app_type(mmgsdi_aid_info[index].app_type);
    sim_internal_aid_info[index].aid_len  = mmgsdi_aid_info[index].aid.data_len;
    if(sim_internal_aid_info[index].aid_len > 0)
    {
      (void)memscpy(&sim_internal_aid_info[index].aid,
                    sizeof(sim_internal_aid_info[index].aid),
                    mmgsdi_aid_info[index].aid.data_ptr,
                    sim_internal_aid_info[index].aid_len);
    }
    sim_internal_aid_info[index].label_len  = mmgsdi_aid_info[index].label.data_len;
    if(sim_internal_aid_info[index].label_len > 0)
    {
      (void)memscpy(&sim_internal_aid_info[index].label,
                    sizeof(sim_internal_aid_info[index].label),
                    mmgsdi_aid_info[index].label.data_ptr,
                    sim_internal_aid_info[index].label_len);
    }
  }

  return;
}/* qmi_sim_internal_populate_mmgsdi_aid_type() */


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
)
{
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_sim_internal_mmgsdi_aid_type");
  ASSERT(mmgsdi_aid_info != NULL);
  ASSERT(sim_internal_aid_info != NULL);

  mmgsdi_aid_info->app_type
    = qmi_sim_internal_convert_sim_internal_mmgsdi_app_type(sim_internal_aid_info->app_type);
  mmgsdi_aid_info->aid.data_len  = sim_internal_aid_info->aid_len;
  if(mmgsdi_aid_info->aid.data_len > 0)
  {
    (void)memscpy(mmgsdi_aid_info->aid.data_ptr,
                  sizeof(mmgsdi_aid_info->aid.data_ptr),
                  &sim_internal_aid_info->aid,
                  mmgsdi_aid_info->aid.data_len);
  }
  mmgsdi_aid_info->label.data_len = sim_internal_aid_info->label_len;
  if(mmgsdi_aid_info->label.data_len > 0)
  {
    (void)memscpy(mmgsdi_aid_info->label.data_ptr,
                  sizeof(mmgsdi_aid_info->label.data_ptr),
                  &sim_internal_aid_info->label,
                  mmgsdi_aid_info->label.data_len);
  }

  return;
}/* qmi_sim_internal_populate_sim_internal_mmgsdi_aid_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_PIN_INFO_TYPE

  DESCRIPTION
    To populate mmgsdi pin info type to sim internal mmgsdi pin info type

  PARAMETERS
    mmgsdi_pin_info
    sim_internal_pin_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_mmgsdi_pin_info_type
(
  mmgsdi_pin_info_type                   mmgsdi_pin_info,
  sim_internal_mmgsdi_pin_info_type_v01 *sim_internal_pin_info
)
{
  qmi_csi_error resp_err   = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_pin_info_type");
  ASSERT(sim_internal_pin_info != NULL);

  resp_err = qmi_sim_internal_convert_mmgsdi_pin_id(mmgsdi_pin_info.pin_id,
                                            &sim_internal_pin_info->pin_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_convert_mmgsdi_pin_status(mmgsdi_pin_info.status,
                                                &sim_internal_pin_info->status);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_convert_mmgsdi_pin_replacement(mmgsdi_pin_info.pin_replacement,
                                                             &sim_internal_pin_info->pin_replacement);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  sim_internal_pin_info->num_retries               = mmgsdi_pin_info.num_retries;
  sim_internal_pin_info->num_unblock_retries       = mmgsdi_pin_info.num_unblock_retries;
  sim_internal_pin_info->valid_num_retries         = mmgsdi_pin_info.valid_num_retries;
  sim_internal_pin_info->valid_num_unblock_retries = mmgsdi_pin_info.valid_num_unblock_retries;

  return resp_err;
}/* qmi_sim_internal_populate_mmgsdi_pin_info_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_APP_INFO_TYPE

  DESCRIPTION
    To populate mmgsdi app info type to sim internal mmgsdi app info type

  PARAMETERS
    mmgsdi_app_info
    sim_internal_app_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_mmgsdi_app_info_type
(
  const mmgsdi_app_info_type            *mmgsdi_app_info,
  sim_internal_mmgsdi_app_info_type_v01 *sim_internal_app_info
)
{
  qmi_csi_error resp_err = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_aid_type");
  ASSERT(mmgsdi_app_info != NULL);
  ASSERT(sim_internal_app_info != NULL);

  sim_internal_app_info->prov_app  = mmgsdi_app_info->prov_app;
  sim_internal_app_info->dir_index = mmgsdi_app_info->dir_index;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(mmgsdi_app_info->slot,
                                                     &sim_internal_app_info->slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(mmgsdi_app_info->pin1,
                                                            &sim_internal_app_info->pin1);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(mmgsdi_app_info->pin2,
                                                            &sim_internal_app_info->pin2);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(mmgsdi_app_info->universal_pin,
                                                            &sim_internal_app_info->universal_pin);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(mmgsdi_app_info->hiddenkey,
                                                            &sim_internal_app_info->hiddenkey);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  qmi_sim_internal_populate_mmgsdi_aid_type(1,
                                            &mmgsdi_app_info->app_data,
                                            &sim_internal_app_info->app_data);

  return resp_err;
}/* qmi_sim_internal_populate_mmgsdi_app_info_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_SECURITY_ACCESS_TYPE

  DESCRIPTION
    To populate mmgsdi security access type to sim internal mmgsdi security
    access type

  PARAMETERS
    mmgsdi_file_security
    sim_internal_file_security

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_security_access_type
(
  mmgsdi_file_security_access_type                   mmgsdi_file_security,
  sim_internal_mmgsdi_file_security_access_type_v01 *sim_internal_file_security
)
{
  qmi_csi_error resp_err   = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_security_access_type");
  ASSERT(sim_internal_file_security != NULL);

  qmi_sim_internal_convert_mmgsdi_protection_method(mmgsdi_file_security.read.protection_method,
                                                    &sim_internal_file_security->read.protection_method);
  sim_internal_file_security->read.protection_pin_len
         = mmgsdi_file_security.read.num_protection_pin;
  (void)memscpy(sim_internal_file_security->read.protection_pin,
                sizeof(sim_internal_file_security->read.protection_pin),
                mmgsdi_file_security.read.protection_pin_ptr,
                sim_internal_file_security->read.protection_pin_len);

  qmi_sim_internal_convert_mmgsdi_protection_method(mmgsdi_file_security.write.protection_method,
                                                    &sim_internal_file_security->write.protection_method);
  sim_internal_file_security->write.protection_pin_len
         = mmgsdi_file_security.write.num_protection_pin;
  (void)memscpy(sim_internal_file_security->write.protection_pin,
                sizeof(sim_internal_file_security->write.protection_pin),
                mmgsdi_file_security.write.protection_pin_ptr,
                sim_internal_file_security->write.protection_pin_len);

  qmi_sim_internal_convert_mmgsdi_protection_method(mmgsdi_file_security.increase.protection_method,
                                                    &sim_internal_file_security->increase.protection_method);
  sim_internal_file_security->increase.protection_pin_len
         = mmgsdi_file_security.increase.num_protection_pin;
  (void)memscpy(sim_internal_file_security->increase.protection_pin,
                sizeof(sim_internal_file_security->increase.protection_pin),
                mmgsdi_file_security.increase.protection_pin_ptr,
                sim_internal_file_security->increase.protection_pin_len);

  qmi_sim_internal_convert_mmgsdi_protection_method(mmgsdi_file_security.invalidate_deactivate.protection_method,
                                                    &sim_internal_file_security->invalidate_deactivate.protection_method);
  sim_internal_file_security->invalidate_deactivate.protection_pin_len
         = mmgsdi_file_security.invalidate_deactivate.num_protection_pin;
  (void)memscpy(sim_internal_file_security->invalidate_deactivate.protection_pin,
                sizeof(sim_internal_file_security->invalidate_deactivate.protection_pin),
                mmgsdi_file_security.invalidate_deactivate.protection_pin_ptr,
                sim_internal_file_security->invalidate_deactivate.protection_pin_len);

  qmi_sim_internal_convert_mmgsdi_protection_method(mmgsdi_file_security.rehabilitate_activate.protection_method,
                                                    &sim_internal_file_security->rehabilitate_activate.protection_method);
  sim_internal_file_security->rehabilitate_activate.protection_pin_len
         = mmgsdi_file_security.rehabilitate_activate.num_protection_pin;
  (void)memscpy(sim_internal_file_security->rehabilitate_activate.protection_pin,
                sizeof(sim_internal_file_security->rehabilitate_activate.protection_pin),
                mmgsdi_file_security.rehabilitate_activate.protection_pin_ptr,
                sim_internal_file_security->rehabilitate_activate.protection_pin_len);

  return resp_err;
}/* qmi_sim_internal_populate_security_access_type() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_LINEAR_FIXED_FILE

  DESCRIPTION
    To populate mmgsdi linear fixed file to sim internal mmgsdi linear fixed file

  PARAMETERS
    mmgsdi_file_info
    sim_internal_mmgsdi_file_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_mmgsdi_linear_fixed_file
(
  mmgsdi_linear_fixed_file_info_type                   mmgsdi_file_info,
  sim_internal_mmgsdi_linear_fixed_file_info_type_v01 *sim_internal_mmgsdi_file_info
)
{
  qmi_csi_error resp_err   = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_linear_fixed_file");
  ASSERT(sim_internal_mmgsdi_file_info != NULL);

  resp_err = qmi_sim_internal_populate_security_access_type(mmgsdi_file_info.file_security,
                                                            &sim_internal_mmgsdi_file_info->file_security);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  sim_internal_mmgsdi_file_info->file_status.file_invalidated
                                            = mmgsdi_file_info.file_status.file_invalidated;
  sim_internal_mmgsdi_file_info->file_status.read_write_when_invalidated
                                            = mmgsdi_file_info.file_status.read_write_when_invalidated;
  sim_internal_mmgsdi_file_info->num_of_rec = mmgsdi_file_info.num_of_rec;
  sim_internal_mmgsdi_file_info->rec_len    = mmgsdi_file_info.rec_len;

  return resp_err;
}/* qmi_sim_internal_populate_mmgsdi_linear_fixed_file() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_CYCLIC_FILE

  DESCRIPTION
    To populate mmgsdi cycle file to sim internal mmgsdi cycle file

  PARAMETERS
    mmgsdi_file_info
    sim_internal_mmgsdi_file_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_mmgsdi_cyclic_file
(
  mmgsdi_cyclic_file_info_type                   mmgsdi_file_info,
  sim_internal_mmgsdi_cyclic_file_info_type_v01 *sim_internal_mmgsdi_file_info
)
{
  qmi_csi_error resp_err   = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_cyclic_file");
  ASSERT(sim_internal_mmgsdi_file_info != NULL);

  resp_err = qmi_sim_internal_populate_security_access_type(mmgsdi_file_info.file_security,
                                                            &sim_internal_mmgsdi_file_info->file_security);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  sim_internal_mmgsdi_file_info->file_status.file_invalidated
                                            = mmgsdi_file_info.file_status.file_invalidated;
  sim_internal_mmgsdi_file_info->file_status.read_write_when_invalidated
                                            = mmgsdi_file_info.file_status.read_write_when_invalidated;
  sim_internal_mmgsdi_file_info->num_of_rec = mmgsdi_file_info.num_of_rec;
  sim_internal_mmgsdi_file_info->rec_len    = mmgsdi_file_info.rec_len;
  sim_internal_mmgsdi_file_info->increase_allowed = mmgsdi_file_info.increase_allowed;

  return resp_err;
}/* qmi_sim_internal_populate_mmgsdi_cyclic_file() */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_POPULATE_MMGSDI_TRANSPARENT_FILE

  DESCRIPTION
    To populate mmgsdi transparent file to sim internal mmgsdi
    transparent file

  PARAMETERS
    mmgsdi_file_info
    sim_internal_mmgsdi_file_info

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_sim_internal_populate_mmgsdi_transparent_file
(
  mmgsdi_transparent_file_info_type                   mmgsdi_file_info,
  sim_internal_mmgsdi_transparent_file_info_type_v01 *sim_internal_mmgsdi_file_info
)
{
  qmi_csi_error resp_err   = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_populate_mmgsdi_transparent_file");
  ASSERT(sim_internal_mmgsdi_file_info != NULL);

  resp_err = qmi_sim_internal_populate_security_access_type(mmgsdi_file_info.file_security,
                                                            &sim_internal_mmgsdi_file_info->file_security);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  sim_internal_mmgsdi_file_info->file_status.file_invalidated
                                            = mmgsdi_file_info.file_status.file_invalidated;
  sim_internal_mmgsdi_file_info->file_status.read_write_when_invalidated
                                            = mmgsdi_file_info.file_status.read_write_when_invalidated;

  return resp_err;
}/* qmi_sim_internal_populate_mmgsdi_transparent_file() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_file_enum 0x%x",
                 sim_internal_mmgsdi_file_enum);
  ASSERT(mmgsdi_file_enum != NULL);

  switch(sim_internal_mmgsdi_file_enum)
  {
    case SIM_INTERNAL_MMGSDI_NO_FILE_ENUM_V01:
      *mmgsdi_file_enum = MMGSDI_NO_FILE_ENUM;
      break;

    case SIM_INTERNAL_MMGSDI_ICCID_V01:
      *mmgsdi_file_enum = MMGSDI_ICCID;
      break;

    case SIM_INTERNAL_MMGSDI_ELP_V01:
      *mmgsdi_file_enum = MMGSDI_ELP;
      break;

    case SIM_INTERNAL_MMGSDI_DIR_V01:
      *mmgsdi_file_enum = MMGSDI_DIR;
      break;

    case SIM_INTERNAL_MMGSDI_ARR_V01:
      *mmgsdi_file_enum = MMGSDI_ARR;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_LP_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_LP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMSI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMSI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_KC_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_KC;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_PLMN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_PLMN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_HPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_HPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ACM_MAX_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ACM_MAX;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SST_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SST;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ACM_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ACM;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_GID1_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_GID1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_GID2_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_GID2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SPN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SPN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_PUCT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_PUCT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CBMI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CBMI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_BCCH_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_BCCH;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ACC_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ACC;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_FPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_FPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_LOCI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_LOCI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_AD_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_AD;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_PHASE_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_PHASE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_VGCS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_VGCS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_VGCSS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_VGCSS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_VBS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_VBS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_VBSS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_VBSS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_EMLPP_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_EMLPP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_AAEM_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_AAEM;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CBMID_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CBMID;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ECC_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ECC;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CBMIR_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CBMIR;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_DCK_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_DCK;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CNL_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CNL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_NIA_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_NIA;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_KCGPRS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_KCGPRS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_LOCIGPRS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_LOCIGPRS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SUME_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SUME;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_PLMNWACT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_PLMNWACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_OPLMNWACT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_OPLMNWACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_HPLMNACT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_HPLMNACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CPBCCH_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CPBCCH;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_INVSCAN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_INVSCAN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_RPLMNAT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_RPLMNAT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_PNN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_PNN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_OPL_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_OPL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_MBDN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_MBDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_EXT6_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_EXT6;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_MBI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_MBI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_MWIS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_MWIS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_EXT1_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_EXT1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SPDI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SPDI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CFIS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CFIS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_VMWI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_VMWI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SVC_STR_TBL_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SVC_STR_TBL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CFF_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CFF;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ONS_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ONS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CSP_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CSP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_CPHSI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_CPHSI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_MN_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_MN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SAI_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SAI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SLL_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_SLL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_MEXE_ST_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_MEXE_ST;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ORPK_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ORPK;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ARPK_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ARPK;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_TPRPK_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_TPRPK;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG1INST1_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG1INST1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG1INST2_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG1INST2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG1INST3_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG1INST3;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG2INST1_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG2INST1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG2INST2_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG2INST2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG2INST3_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG2INST3;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG3INST1_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG3INST1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG3INST2_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG3INST2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_IMG3INST3_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_IMG3INST3;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_ONS_SHORT_V01:
      *mmgsdi_file_enum = MMGSDI_GSM_ONS_SHORT;
      break;

    case SIM_INTERNAL_MMGSDI_IMAGE_V01:
      *mmgsdi_file_enum = MMGSDI_IMAGE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_LI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_LI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_IMSI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_IMSI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_KEYS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_KEYS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_KEYSPS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_KEYSPS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PLMNWACT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PLMNWACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_UPLMNSEL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_UPLMNSEL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_HPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_HPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ACM_MAX_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ACM_MAX;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_UST_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_UST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ACM_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ACM;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_GID1_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_GID1;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_GID2_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_GID2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SPN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SPN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PUCT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PUCT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CBMI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CBMI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ACC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ACC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_FPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_FPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_LOCI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_LOCI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_AD_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_AD;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CBMID_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CBMID;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ECC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ECC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CBMIR_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CBMIR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PSLOCI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PSLOCI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_FDN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_FDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SMS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MSISDN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MSISDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SMSP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SMSP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SMSS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SMSS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SDN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT2_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT3_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT3;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SMSR_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SMSR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ICI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ICI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_OCI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_OCI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ICT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ICT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_OCT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_OCT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT5_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT5;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CCP2_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CCP2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EMLPP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EMLPP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_AAEM_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_AAEM;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_GMSI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_GMSI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_HIDDENKEY_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_HIDDENKEY;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_BDN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_BDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT4_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT4;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CMI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CMI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EST_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ACL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ACL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_DCK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_DCK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CNL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CNL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_START_HFN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_START_HFN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_THRESHOLD_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_THRESHOLD;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_OPLMNWACT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_OPLMNWACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_OPLMNSEL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_OPLMNSEL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_HPLMNWACT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_HPLMNWACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ARR_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ARR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_RPLMNACT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_RPLMNACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_NETPAR_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_NETPAR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PNN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PNN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_OPL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_OPL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MBDN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MBDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT6_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT6;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MBI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MBI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MWIS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MWIS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SPDI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SPDI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EHPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EHPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CFIS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CFIS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_VGCSCA_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_VGCSCA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_VBSCA_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_VBSCA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_GBABP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_GBABP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_GBANL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_GBANL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MSK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MSK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MUK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MUK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_VMWI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_VMWI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SVC_STR_TBL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SVC_STR_TBL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CFF_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CFF;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ONS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ONS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CSP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CSP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CPHSI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CPHSI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PBR_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PBR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PSC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PSC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PUID_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PUID;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_KC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_KC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_KCGPRS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_KCGPRS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CPBCCH_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CPBCCH;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_INVSCAN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_INVSCAN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MEXE_ST_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MEXE_ST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ORPK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ORPK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ARPK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ARPK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_TPRPK_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_TPRPK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MMSN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MMSN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MMSICP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MMSICP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MMSUP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MMSUP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_MMSUCP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_MMSUCP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EXT8_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EXT8;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EPSLOCI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EPSLOCI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EPSNSC_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EPSNSC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SPNI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_SPNI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_PNNI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_PNNI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_NCP_IP_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_NCP_IP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_HPLMNDAI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_HPLMNDAI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ACSGL_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ACSGL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_CSGI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_CSGT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_HNBN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_HNBN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_EHPLMNPI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_EHPLMNPI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_LRPLMNSI_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_LRPLMNSI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_ONS_SHORT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_ONS_SHORT;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_DFS_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F40_PROP1_DFS;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_D2FS_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F40_PROP1_D2FS;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_CSP2_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F40_PROP1_CSP2;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F40_PROP1_PARAMS_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F40_PROP1_PARAMS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_DFS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F40_PROP1_DFS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_D2FS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F40_PROP1_D2FS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_CSP2_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F40_PROP1_CSP2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F40_PROP1_PARAMS_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F40_PROP1_PARAMS;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F66_PROP1_ACT_HPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F66_PROP1_ACT_HPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F66_PROP1_SPT_TABLE_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F66_PROP1_SPT_TABLE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_ACT_HPLMN_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F66_PROP1_ACT_HPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_SPT_TABLE_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F66_PROP1_SPT_TABLE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RAT_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F66_PROP1_RAT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RPM_EF_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F66_PROP1_RPM_EF;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_7F66_PROP1_RPM_PARAM_V01:
      *mmgsdi_file_enum = MMGSDI_USIM_7F66_PROP1_RPM_PARAM;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_HZ_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_HZ;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE1_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_CACHE1;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE2_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_CACHE2;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE3_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_CACHE3;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_CACHE4_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_CACHE4;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_TAGS_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_TAGS;
      break;

    case SIM_INTERNAL_MMGSDI_SIM_7F43_PROP1_SETTINGS_V01:
      *mmgsdi_file_enum = MMGSDI_SIM_7F43_PROP1_SETTINGS;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_ADN_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_ADN;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_FDN_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_FDN;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SMS_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_CCP_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_CCP;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_ECCP_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_ECCP;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MSISDN_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MSISDN;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SMSP_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SMSP;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SMSS_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SMSS;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_LND_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_LND;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SDN_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SDN;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_EXT1_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_EXT1;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_EXT2_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_EXT2;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_EXT3_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_EXT3;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_BDN_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_BDN;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_EXT4_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_EXT4;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SMSR_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SMSR;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_CMI_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_CMI;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_SUME_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_SUME;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_ARR_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_ARR;
      break;

    case SIM_INTERNAL_MMGSDI_IMAGE_FILE_V01:
      *mmgsdi_file_enum = MMGSDI_IMAGE_FILE;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_PBR_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_PBR;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_PSC_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_PSC;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_CC_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_CC;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_PUID_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_PUID;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MML_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MML;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MMDF_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MMDF;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MLPL_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MLPL;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MSPL_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MSPL;
      break;

    case SIM_INTERNAL_MMGSDI_TELECOM_MMSSMODE_V01:
      *mmgsdi_file_enum = MMGSDI_TELECOM_MMSSMODE;
      break;

    case SIM_INTERNAL_MMGSDI_CDMA_SMS_V01:
      *mmgsdi_file_enum = MMGSDI_CDMA_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_CDMA_SMS_PARAMS_V01:
      *mmgsdi_file_enum = MMGSDI_CDMA_SMS_PARAMS;
      break;

    case SIM_INTERNAL_MMGSDI_CDMA_SMS_STATUS_V01:
      *mmgsdi_file_enum = MMGSDI_CDMA_SMS_STATUS;
      break;

    case SIM_INTERNAL_MMGSDI_CSIM_SMS_V01:
      *mmgsdi_file_enum = MMGSDI_CSIM_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_CSIM_SMS_PARAMS_V01:
      *mmgsdi_file_enum = MMGSDI_CSIM_SMS_PARAMS;
      break;

    case SIM_INTERNAL_MMGSDI_CSIM_SMS_STATUS_V01:
      *mmgsdi_file_enum = MMGSDI_CSIM_SMS_STATUS;
      break;

    default:
      UIM_MSG_HIGH_0("Unhandled sim_internal_mmgsdi_file_enum received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_sim_internal_mmgsdi_file() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_file 0x%x",
                 sim_internal_mmgsdi_file);
  ASSERT(mmgsdi_file != NULL);

  switch(sim_internal_mmgsdi_file)
  {
    case SIM_INTERNAL_MMGSDI_LINEAR_FIXED_FILE_V01:
      *mmgsdi_file = MMGSDI_LINEAR_FIXED_FILE;
      break;
    case SIM_INTERNAL_MMGSDI_CYCLIC_FILE_V01:
      *mmgsdi_file = MMGSDI_CYCLIC_FILE;
      break;
    case SIM_INTERNAL_MMGSDI_TRANSPARENT_FILE_V01:
      *mmgsdi_file = MMGSDI_TRANSPARENT_FILE;
      break;
    case SIM_INTERNAL_MMGSDI_MASTER_FILE_V01:
      *mmgsdi_file = MMGSDI_MASTER_FILE;
      break;
    case SIM_INTERNAL_MMGSDI_DEDICATED_FILE_V01:
      *mmgsdi_file = MMGSDI_DEDICATED_FILE;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled sim_internal_mmgsdi_file received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
}/* qmi_sim_internal_convert_sim_internal_mmgsdi_file_structure() */



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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_tc_mode 0x%x",
                 sim_internal_mmgsdi_tc_mode);
  ASSERT(mmgsdi_tc_mode != NULL);

  switch(sim_internal_mmgsdi_tc_mode)
  {
    case SIM_INTERNAL_MMGSDI_NOT_IN_TC_V01:
      *mmgsdi_tc_mode = MMGSDI_NOT_IN_TC;
      break;
    case SIM_INTERNAL_MMGSDI_IN_TC_GW_V01:
      *mmgsdi_tc_mode = MMGSDI_IN_TC_GW;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled tc_mode received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_tc_mode() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_session_info 0x%x",
                 sim_internal_mmgsdi_session_info);
  ASSERT(mmgsdi_session_info != NULL);

  switch(sim_internal_mmgsdi_session_info)
  {
    case SIM_INTERNAL_MMGSDI_SESSION_INFO_QUERY_BY_ID_V01:
      *mmgsdi_session_info = MMGSDI_SESSION_INFO_QUERY_BY_ID;
      break;
    case SIM_INTERNAL_MMGSDI_SESSION_INFO_QUERY_BY_TYPE_V01:
      *mmgsdi_session_info = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled session_info received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_session_info() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_session_type 0x%x",
                 sim_internal_mmgsdi_session_type);
  ASSERT(mmgsdi_session_type != NULL);

  switch(sim_internal_mmgsdi_session_type)
  {
    case SIM_INTERNAL_MMGSDI_GW_PROV_PRI_SESSION_V01:
      *mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case SIM_INTERNAL_MMGSDI_1X_PROV_PRI_SESSION_V01:
      *mmgsdi_session_type = MMGSDI_1X_PROV_PRI_SESSION;
      break;
    case SIM_INTERNAL_MMGSDI_GW_PROV_SEC_SESSION_V01:
      *mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
      break;
    case SIM_INTERNAL_MMGSDI_1X_PROV_SEC_SESSION_V01:
      *mmgsdi_session_type = MMGSDI_1X_PROV_SEC_SESSION;
      break;
    case SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_1_V01:
      *mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      break;
    case SIM_INTERNAL_MMGSDI_NON_PROV_SESSION_SLOT_2_V01:
      *mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
      break;
    case SIM_INTERNAL_MMGSDI_CARD_SESSION_SLOT_1_V01:
      *mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_1;
      break;
    case SIM_INTERNAL_MMGSDI_CARD_SESSION_SLOT_2_V01:
      *mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_2;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled session_type received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_session_type() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_auth_context 0x%x",
                 sim_internal_mmgsdi_auth_context);
  ASSERT(mmgsdi_auth_context != NULL);

  switch(sim_internal_mmgsdi_auth_context)
  {
    case SIM_INTERNAL_MMGSDI_AUTN_USIM_GSM_CONTEXT_V01:
      *mmgsdi_auth_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;
      break;
    case SIM_INTERNAL_MMGSDI_AUTN_USIM_3G_CONTEXT_V01:
      *mmgsdi_auth_context = MMGSDI_AUTN_USIM_3G_CONTEXT;
      break;
    default:
      UIM_MSG_HIGH_0("Unhandled auth_context received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_auth_context() */


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
)
{
  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_service 0x%x",
                 sim_internal_mmgsdi_service);
  ASSERT(mmgsdi_service != NULL);

  switch(sim_internal_mmgsdi_service)
  {
    case SIM_INTERNAL_MMGSDI_NONE_V01:
      *mmgsdi_service =   MMGSDI_NONE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CHV1_DISABLE_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CHV1_DISABLE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_ADN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_ADN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_FDN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_FDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SMS_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_AOC_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_AOC;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CCP_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CCP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_PLMN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_PLMN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MSISDN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MSISDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_EXT1_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_EXT1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_EXT2_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_EXT2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SMSP_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SMSP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_LND_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_LND;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CBMI_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CBMI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_GID1_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_GID1;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_GID2_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_GID2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SPN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SPN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SDN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_EXT3_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_EXT3;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_RFU2_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_RFU2;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_VGCS_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_VGCS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_VBS_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_VBS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_ENH_ML_SVC_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_ENH_ML_SVC;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_AA_EMLPP_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_AA_EMLPP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_DATA_DL_SMSCB_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_DATA_DL_SMSCB;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_DATA_DL_SMSPP_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_DATA_DL_SMSPP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MENU_SEL_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MENU_SEL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CALL_CONTROL_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CALL_CONTROL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_PROACTIVE_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_PROACTIVE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CBMIR_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CBMIR;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_BDN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_BDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_EXT4_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_EXT4;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_DCK_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_DCK;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CNL_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CNL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SMSR_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SMSR;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_NIA_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_NIA;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MOSMS_CONTROL_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MOSMS_CONTROL;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_GPRS_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_GPRS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_IMAGE_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_IMAGE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SOLSA_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SOLSA;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_USSD_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_USSD;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_RUN_AT_CMD_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_RUN_AT_CMD;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_UPLMN_SEL_WACT_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_UPLMN_SEL_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_OPLMN_SEL_WACT_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_OPLMN_SEL_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_HPLMN_WACT_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_HPLMN_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CPBCCH_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CPBCCH;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_INV_SCAN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_INV_SCAN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_ECCP_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_ECCP;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MEXE_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MEXE;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_RPLMN_LACT_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_RPLMN_LACT;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_PLMN_NTWRK_NAME_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_PLMN_NTWRK_NAME;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_OPLMN_LIST_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_OPLMN_LIST;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MDN_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MDN;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MWI_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MWI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_CFI_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_CFI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_SPDI_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_SPDI;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MMS_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MMS;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_EXT8_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_EXT8;
      break;

    case SIM_INTERNAL_MMGSDI_GSM_SRV_MMS_USR_P_V01:
      *mmgsdi_service =   MMGSDI_GSM_SRV_MMS_USR_P;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_LOCAL_PHONEBOOK_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_LOCAL_PHONEBOOK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_FDN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_FDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EXT2_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EXT2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SDN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EXT3_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EXT3;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_BDN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_BDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EXT4_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EXT4;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OCI_OCT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OCI_OCT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_ICI_ICT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_ICI_ICT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SMS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SMS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SMSR_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SMSR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SMSP_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SMSP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_AOC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_AOC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CCP_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CCP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CBMI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CBMI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CBMIR_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CBMIR;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GID1_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GID1;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GID2_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GID2;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SPN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SPN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_UPLMN_SEL_WACT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_UPLMN_SEL_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MSISDN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MSISDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_IMAGE_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_IMAGE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SOLSA_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SOLSA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_ENH_ML_SVC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_ENH_ML_SVC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_AA_EMLPP_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_AA_EMLPP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_RFU1_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_RFU1;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GSM_ACCESS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GSM_ACCESS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_DATA_DL_SMSPP_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_DATA_DL_SMSPP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_DATA_DL_SMSCB_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_DATA_DL_SMSCB;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CALL_CONTROL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CALL_CONTROL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MOSMS_CONTROL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MOSMS_CONTROL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_RUN_AT_CMD_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_RUN_AT_CMD;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SET_TO_1_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SET_TO_1;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EST_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_ACL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_ACL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_DCK_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_DCK;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CNL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CNL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CPBCCH_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CPBCCH;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_INV_SCAN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_INV_SCAN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MEXE_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MEXE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OPLMN_SEL_WACT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OPLMN_SEL_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_HPLMN_WACT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_HPLMN_WACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EXT5_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EXT5;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_PLMN_NTWRK_NAME_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_PLMN_NTWRK_NAME;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OPLMN_LIST_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OPLMN_LIST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MDN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MDN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MWI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MWI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_CFI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_CFI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_RPLMN_LACT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_RPLMN_LACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SPDI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SPDI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MMS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MMS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EXT8_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EXT8;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GPRS_CC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GPRS_CC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MMS_USR_P_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MMS_USR_P;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_NIA_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_NIA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_VGCS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_VGCS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_VBS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_VBS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_PSEUDONYM_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_PSEUDONYM;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_UPLMN_WLAN_ACC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_UPLMN_WLAN_ACC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OPLMN_WLAN_ACC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OPLMN_WLAN_ACC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_USER_CTRL_WSID_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_USER_CTRL_WSID;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OPER_CTRL_WSID_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OPER_CTRL_WSID;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_VGCS_SECURITY_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_VGCS_SECURITY;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_VBS_SECURITY_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_VBS_SECURITY;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_WLAN_REAUTH_ID_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_WLAN_REAUTH_ID;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MM_STORAGE_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MM_STORAGE;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GBA_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GBA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_MBMS_SECURITY_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_MBMS_SECURITY;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_DATA_DL_USSD_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_DATA_DL_USSD;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EHPLMN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EHPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_TP_AFTER_UICC_ACT_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_TP_AFTER_UICC_ACT;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EHPLMNI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EHPLMNI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_LAST_RPMN_SEL_IND_V01:
      *mmgsdi_service =   MMGSDI_USIM_LAST_RPMN_SEL_IND;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_OMA_BCAST_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_OMA_BCAST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_GBA_KEY_EST_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_GBA_KEY_EST;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_TERMINAL_APPS_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_TERMINAL_APPS;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_SPNI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_SPNI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_PNNI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_PNNI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_NCP_IP_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_NCP_IP;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_HWSIDL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_HWSIDL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_WEHPLMNPI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_WEHPLMNPI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_WHPI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_WHPI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_WLRPLMN_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_WLRPLMN;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EPS_MMI_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EPS_MMI;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_ACSGL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_ACSGL;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_EPS_PDN_CC_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_EPS_PDN_CC;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_HPLMN_DA_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_HPLMN_DA;
      break;

    case SIM_INTERNAL_MMGSDI_USIM_SRV_ECALL_V01:
      *mmgsdi_service =   MMGSDI_USIM_SRV_ECALL;
      break;

    default:
      UIM_MSG_HIGH_0("Unhandled service type received");
      return QMI_CSI_INTERNAL_ERR;
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_convert_sim_internal_mmgsdi_service() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_perso_feature_enum_table) /
                         sizeof(sim_internal_mmgsdi_perso_feature_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_mmgsdi_perso_feature 0x%x",
                 sim_internal_mmgsdi_perso_feature);
  ASSERT(mmgsdi_perso_feature != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_perso_feature_enum_table[i].sim_internal_mmgsdi_perso_feature == sim_internal_mmgsdi_perso_feature)
    {
       *mmgsdi_perso_feature = sim_internal_mmgsdi_perso_feature_enum_table[i].mmgsdi_perso_feature;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
}/* qmi_sim_internal_convert_sim_internal_mmgsdi_perso_feature() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_mmgsdi_slot_id_enum_table) /
                         sizeof(sim_internal_mmgsdi_slot_id_enum_table_type));

  UIM_MSG_HIGH_1("Received mmgsdi_slot_id 0x%x", mmgsdi_slot_id);
  ASSERT(mmgsdi_slot_id != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_mmgsdi_slot_id_enum_table[i].sim_internal_slot_id == sim_internal_slot_id)
    {
       *mmgsdi_slot_id = sim_internal_mmgsdi_slot_id_enum_table[i].mmgsdi_slot_id;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_mmgsdi_slot_id() */


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
  sim_internal_gstk_tal_client_enum_v01  sim_internal_gstk_client,
  gstk_tal_client_type                  *gstk_client
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_client_enum_table) /
                          sizeof(sim_internal_gstk_client_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_gstk_client 0x%x",
                 sim_internal_gstk_client);
  ASSERT(gstk_client != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_client_enum_table[i].sim_internal_gstk_client == sim_internal_gstk_client)
    {
       *gstk_client = sim_internal_gstk_client_enum_table[i].gstk_client;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_sim_internal_gstk_client_type() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_cmd_group_enum_table) /
                          sizeof(sim_internal_gstk_cmd_group_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_cmd_group 0x%x",
                 sim_internal_gstk_cmd_group);
  ASSERT(gstk_cmd_group != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_cmd_group_enum_table[i].sim_internal_gstk_cmd_group ==
          sim_internal_gstk_cmd_group)
    {
       *gstk_cmd_group = sim_internal_gstk_cmd_group_enum_table[i].gstk_cmd_group;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_sim_internal_gstk_tal_cmd_type() */


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
)
{
  uint8  i     = 0;
  uint8  count = (uint8)(sizeof(sim_internal_gstk_slot_id_enum_table) /
                         sizeof(sim_internal_gstk_slot_id_enum_table_type));

  UIM_MSG_HIGH_1("Received sim_internal_slot_id 0x%x", sim_internal_slot_id);
  ASSERT(gstk_slot_id != NULL);

  for (i = 0; i< count; i++)
  {
    if (sim_internal_gstk_slot_id_enum_table[i].sim_internal_slot_id == sim_internal_slot_id)
    {
       *gstk_slot_id = sim_internal_gstk_slot_id_enum_table[i].gstk_slot_id;
       return QMI_CSI_NO_ERR;
    }
  }
  return QMI_CSI_INTERNAL_ERR;
} /* qmi_sim_internal_convert_gstk_slot_id() */


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
)
{
  UIM_MSG_HIGH_1("mmgsdi_status : 0x%x", mmgsdi_status);
  ASSERT(mmgsdi_status_valid != NULL);
  ASSERT(sim_internal_mmgsdi_status != NULL);

  *mmgsdi_status_valid = TRUE;
  *sim_internal_mmgsdi_status = qmi_sim_internal_convert_mmgsdi_status(mmgsdi_status);
  return;
} /* qmi_sim_internal_response_mmgsdi_status */


/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_RESPONSE_GSTK_RESULT_CODE()

  DESCRIPTION
    Function called to populate response gstk result

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
)
{
  UIM_MSG_HIGH_1("gstk_status : 0x%x", gstk_status);
  ASSERT(resp != NULL);

  /* Convert MMGSDI status in QMI code */
  resp->error  = qmi_sim_internal_convert_gstk_status_to_errval(gstk_status);
  resp->result = ((resp->error) == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                                    : QMI_RESULT_FAILURE_V01);
} /* qmi_sim_internal_response_gstk_result_code */


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
)
{
  UIM_MSG_HIGH_1("mmgsdi_status : 0x%x", gstk_status);
  ASSERT(gstk_status_valid != NULL);
  ASSERT(sim_internal_gstk_status != NULL);

  *gstk_status_valid = TRUE;
  *sim_internal_gstk_status = qmi_sim_internal_convert_gstk_status(gstk_status);
  return;
} /* qmi_sim_internal_response_gstk_status */


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
)
{
  qmi_csi_cb_error        resp_err;
  uint8_t                 index             = 0;

  ASSERT(avail_apps != NULL);
  ASSERT(resp != NULL);

  if(avail_apps->card1_num_app > 0)
  {
    resp->card_1_app_info_valid = TRUE;
    resp->card_1_app_info_len = avail_apps->card1_num_app;
    for(index=0; index < resp->card_1_app_info_len; index++)
    {
      resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&avail_apps->card1_app_info[index],
                                                                &resp->card_1_app_info[index]);
      if(resp_err != QMI_CSI_NO_ERR)
      {
        resp->card_1_app_info_valid = FALSE;
        return;
      }
    }
  }

  if(avail_apps->card2_num_app > 0)
  {
    resp->card_2_app_info_valid = TRUE;
    resp->card_2_app_info_len = avail_apps->card1_num_app;
    for(index=0; index < resp->card_2_app_info_len; index++)
    {
      resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&avail_apps->card2_app_info[index],
                                                                &resp->card_2_app_info[index]);
      if(resp_err != QMI_CSI_NO_ERR)
      {
        resp->card_2_app_info_valid = FALSE;
        return;
      }
    }
  }
} /* qmi_sim_internal_response_get_all_available_apps_sync */


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
)
{
  qmi_csi_cb_error resp_err;

  ASSERT(get_info_ptr != NULL);
  ASSERT(resp != NULL);

  resp->get_info_valid      = FALSE;
  resp_err = qmi_sim_internal_convert_mmgsdi_session_type(get_info_ptr->session_type,
                                                          &resp->get_info.session_type);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(get_info_ptr->slot_id,
                                                     &resp->get_info.slot_id);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return;
  }
  resp_err = qmi_sim_internal_convert_mmgsdi_app_state(get_info_ptr->app_state,
                                                       &resp->get_info.app_state);
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return;
  }
  qmi_sim_internal_populate_mmgsdi_aid_type(1, &get_info_ptr->app_data,
                                            &resp->get_info.app_data);
  resp->get_info.channel_id = get_info_ptr->channel_id;

  resp->get_info_valid      = TRUE;
  return;
} /* qmi_sim_internal_response_get_info_sync */


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
)
{
  sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01        ind;
  qmi_csi_error                          resp_err = QMI_CSI_NO_ERR;
  uint8                                  index    = 0;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_client_id_and_evt_reg_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  //Test only
  UIM_MSG_HIGH_2(" QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_IND_V01: sizeof ind %d : msg_01 %d",
                 sizeof(ind),
                 sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01));

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_IND_V01");
  }

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  /* set client id in remote_evt_cb_table */
  for (index=0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
  {
    if (qmi_sim_internal_remote_evt_cb_table[index].in_use &&
        qmi_sim_internal_remote_evt_cb_table[index].client_id == 0 &&
        qmi_sim_internal_remote_evt_cb_table[index].remote_evt_cb == local_client_ref_ptr->remote_evt_cb)
    {
      qmi_sim_internal_remote_evt_cb_table[index].client_id = ind.response_header.client_id;
      qmi_sim_internal_remote_evt_cb_table[index].session_id = ind.response_header.session_id;
      break;
    }
  }

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  return resp_err;
} /* qmi_sim_internal_send_client_id_and_evt_reg_ind */


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
)
{
  sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01        ind;
  qmi_csi_error                          resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_client_id_and_evt_dereg_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  //Test only
  UIM_MSG_HIGH_2(" QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_IND_V01: sizeof ind %d : msg_01 %d",
                 sizeof(ind),
                 sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01));

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_client_id_and_evt_dereg_ind */


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
)
{
  sim_internal_mmgsdi_session_open_ind_msg_v01 ind;
  qmi_csi_error                                resp_err = QMI_CSI_NO_ERR;
  uint8                                        index    = 0;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_session_open_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_open_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                    &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.session_open_cnf.session_id       = cnf_ptr->session_id;
  ind.session_open_cnf.channel_id       = cnf_ptr->channel_id;
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->slot_id,
                                                     &ind.session_open_cnf.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  qmi_sim_internal_populate_mmgsdi_aid_type(1,
                                            &cnf_ptr->app_info,
                                            &ind.session_open_cnf.app_info);

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_open_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_IND_V01");
  }

  /* Protect access to the remote event callback table */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  /* set session id in remote_evt_cb_table */
  for (index=0; index < QMI_SIM_INTERNAL_MAX_REMOTE_EVT_CB; index++)
  {
    if (qmi_sim_internal_remote_evt_cb_table[index].in_use &&
        qmi_sim_internal_remote_evt_cb_table[index].client_id == cnf_ptr->response_header.client_id &&
        qmi_sim_internal_remote_evt_cb_table[index].session_id == 0 &&
        qmi_sim_internal_remote_evt_cb_table[index].remote_evt_cb == local_client_ref_ptr->remote_evt_cb)
    {
      qmi_sim_internal_remote_evt_cb_table[index].client_id = ind.response_header.client_id;
      qmi_sim_internal_remote_evt_cb_table[index].session_id = ind.response_header.session_id;
      break;
    }
  }

  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);

  return resp_err;
} /* qmi_sim_internal_send_session_open_ind */


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
)
{
  sim_internal_mmgsdi_session_close_ind_msg_v01 ind;
  qmi_csi_error                                resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_session_close_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_close_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                    &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.session_close_cnf.session_id       = cnf_ptr->session_id;
  qmi_sim_internal_populate_mmgsdi_aid_type(1,
                                            &cnf_ptr->app_info,
                                            &ind.session_close_cnf.app_info);

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_close_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_IND_V01");
  }


  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_session_close_ind */


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
)
{
  sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01 ind;
  qmi_csi_error                                          resp_err = QMI_CSI_NO_ERR;
  uint8                                                  index    = 0;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_get_all_avai_apps_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                    &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  if(cnf_ptr->card1_num_app > 0)
  {
    ind.card_1_app_info_valid = TRUE;
    ind.card_1_app_info_len = cnf_ptr->card1_num_app;
    for(index=0; index < ind.card_1_app_info_len; index++)
    {
      resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&cnf_ptr->card1_app_info[index],
                                                                &ind.card_1_app_info[index]);
      if(resp_err != QMI_CSI_NO_ERR)
      {
        return resp_err;
      }
    }
  }

  if(cnf_ptr->card2_num_app > 0)
  {
    ind.card_2_app_info_valid = TRUE;
    ind.card_2_app_info_len = cnf_ptr->card1_num_app;
    for(index=0; index < ind.card_2_app_info_len; index++)
    {
      resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&cnf_ptr->card2_app_info[index],
                                                                &ind.card_2_app_info[index]);
      if(resp_err != QMI_CSI_NO_ERR)
      {
        return resp_err;
      }
    }
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_get_all_avai_apps_ind */


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
)
{
  sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01  ind;
  qmi_csi_error                                          resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_get_file_attr_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                    &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate file path */
  /* QSC doesn't support file path, set value = 0 and skip*/
  ind.file_path_len = 0;
#if 0
  for(index = 0; index < ind.file_path_len; index++)
  {
    ind.file_path[index].path_buf_len = cnf_ptr->file_path[index].path_len;
    (void)memscpy(&(ind.file_path[index].path_buf),
                  ind.file_path[index].path_buf_len,
                  &(cnf_ptr->file_path[index].path_buf),
                  ind.file_path[index].path_buf_len);
  }
#endif
  /* populate file attr */
  if(cnf_ptr->file_attrib.file_size > 0)
  {
    ind.file_attr_valid = TRUE;
    ind.file_attr.file_size = cnf_ptr->file_attrib.file_size;

#if 0
      for(index=0; index < QMI_SIM_INTERNAL_MMGSDI_FILE_ID_SIZE_V01; index++)
      {
        (void)memscpy(&ind.file_attr.file_id[index],
                      sizeof(uint8),
                      &cnf_ptr->file_attrib.file_id[index],
                      sizeof(uint8);
      }
#endif

    (void)memscpy(ind.file_attr.file_id,
                  sizeof(ind.file_attr.file_id),
                  cnf_ptr->file_attrib.file_id,
                  QMI_SIM_INTERNAL_MMGSDI_FILE_ID_SIZE_V01);

    resp_err = qmi_sim_internal_convert_mmgsdi_file_structure(cnf_ptr->file_attrib.file_type,
                                                              &ind.file_attr.file_type);
    if(resp_err != QMI_CSI_NO_ERR)
    {
      return resp_err;
    }

    switch(ind.file_attr.file_type)
    {
      case SIM_INTERNAL_MMGSDI_LINEAR_FIXED_FILE_V01:
        ind.linear_fixed_file_info_valid = TRUE;
        qmi_sim_internal_populate_mmgsdi_linear_fixed_file(cnf_ptr->file_attrib.file_info.linear_fixed_file,
                                                           &ind.linear_fixed_file_info);
        break;
      case SIM_INTERNAL_MMGSDI_CYCLIC_FILE_V01:
        ind.cyclic_file_info_valid = TRUE;
        qmi_sim_internal_populate_mmgsdi_cyclic_file(cnf_ptr->file_attrib.file_info.cyclic_file,
                                                     &ind.cyclic_file_info);
        break;
      case SIM_INTERNAL_MMGSDI_TRANSPARENT_FILE_V01:
        ind.transparent_file_info_valid = TRUE;
        qmi_sim_internal_populate_mmgsdi_transparent_file(cnf_ptr->file_attrib.file_info.transparent_file,
                                                           &ind.transparent_file_info);
        break;
      /* what's for MF and DF? */
      case SIM_INTERNAL_MMGSDI_MASTER_FILE_V01:
      case SIM_INTERNAL_MMGSDI_DEDICATED_FILE_V01:
        break;
      default:
        UIM_MSG_HIGH_0("Unhandled file_type received");
    }
  }

  /* populate raw file attr */
  if(cnf_ptr->raw_file_attr_data.data_len > 0 &&
     cnf_ptr->raw_file_attr_data.data_len < QMI_SIM_INTERNAL_MMGSDI_DATA_MAX_LENGTH_V01)
  {
      ind.raw_file_attr_data_valid = TRUE;
      ind.raw_file_attr_data.data_len = cnf_ptr->raw_file_attr_data.data_len;
      (void)memscpy(ind.raw_file_attr_data.data,
                    sizeof(ind.raw_file_attr_data.data),
                    cnf_ptr->raw_file_attr_data.data_ptr,
                    ind.raw_file_attr_data.data_len);
  }

  /* check for access method and populate access type*/
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    case MMGSDI_DF_ENUM_ACCESS:
    case MMGSDI_BY_APP_ID_ACCESS:
    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01));

  UIM_MSG_HIGH_1("resp err 0x%x", resp_err);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_get_file_attr_ind */


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
)
{
  sim_internal_mmgsdi_session_read_transparent_ind_msg_v01  ind;
  qmi_csi_error                                             resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_read_transparent_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate read resp data*/
  ind.read_resp_data_valid = TRUE;
  ind.read_resp_data.accessed_rec_num = cnf_ptr->accessed_rec_num;
  ind.read_resp_data.accessed_offset = cnf_ptr->accessed_offset;
  ind.read_resp_data.read_data.data_len = cnf_ptr->read_data.data_len;
  (void)memscpy(ind.read_resp_data.read_data.data,
                sizeof(ind.read_resp_data.read_data.data),
                cnf_ptr->read_data.data_ptr,
                ind.read_resp_data.read_data.data_len);

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_read_transparent_ind */



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
)
{
  sim_internal_mmgsdi_session_read_record_ind_msg_v01       ind;
  qmi_csi_error                                             resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_read_record_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_read_record_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate read resp data*/
  ind.read_resp_data_valid = TRUE;
  ind.read_resp_data.accessed_rec_num = cnf_ptr->accessed_rec_num;
  ind.read_resp_data.accessed_offset = cnf_ptr->accessed_offset;
  ind.read_resp_data.read_data.data_len = cnf_ptr->read_data.data_len;
  (void)memscpy(ind.read_resp_data.read_data.data,
                sizeof(ind.read_resp_data.read_data.data),
                cnf_ptr->read_data.data_ptr,
                ind.read_resp_data.read_data.data_len);

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_read_record_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_read_record_ind */


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
)
{
  sim_internal_mmgsdi_session_write_transparent_ind_msg_v01  ind;
  qmi_csi_error                                              resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_write_transparent_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate write resp data*/
  ind.write_resp_data_valid = TRUE;
  ind.write_resp_data.accessed_rec_num = cnf_ptr->accessed_rec_num;
  ind.write_resp_data.accessed_offset = cnf_ptr->accessed_offset;
  ind.write_resp_data.write_data_len = cnf_ptr->written_data_len;

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_write_transparent_ind */



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
)
{
  sim_internal_mmgsdi_session_write_record_ind_msg_v01  ind;
  qmi_csi_error                                              resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_write_record_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_write_record_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate write resp data*/
  ind.write_resp_data_valid = TRUE;
  ind.write_resp_data.accessed_rec_num = cnf_ptr->accessed_rec_num;
  ind.write_resp_data.accessed_offset = cnf_ptr->accessed_offset;
  ind.write_resp_data.write_data_len = cnf_ptr->written_data_len;

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_write_record_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_write_record_ind */


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
  mmgsdi_srv_available_cnf_type          *cnf_ptr
)
{
  sim_internal_mmgsdi_session_is_service_available_ind_msg_v01  ind;
  qmi_csi_error       resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_is_service_available_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  //TEST only
  print_response_header(QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_IND_V01,
                        ind.response_header);

  ind.srvc_available_valid = TRUE;
  ind.srvc_available = cnf_ptr->srv_available;

  //Test only
  UIM_MSG_HIGH_2("srvc_available 0x%x: 0x%x",
                 ind.srvc_available_valid, ind.srvc_available);
  UIM_MSG_HIGH_2("QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_IND_V01 sizeof ind %d : msg_01 %d",
                 sizeof(ind),
                 sizeof(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01));

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_IND_V01 0x%x",
                  resp_err);
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_is_service_available_ind */


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
)
{
  sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01  ind;
  qmi_csi_error       resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_get_app_capabilities_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.app_capabilities_valid = TRUE;
  ind.app_capabilities.acl_enabled = cnf_ptr->acl_enabled;
  ind.app_capabilities.bdn_enabled = cnf_ptr->bdn_enabled;
  ind.app_capabilities.fdn_enabled = cnf_ptr->fdn_enabled;
  ind.app_capabilities.imsi_invalidated = cnf_ptr->imsi_invalidated;

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_get_app_capabilities_ind */


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
  mmgsdi_usim_auth_cnf_type                *cnf_ptr
)
{
  sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01  ind;
  qmi_csi_error       resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_usim_authenticate_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.data_valid = TRUE;
  ind.data_len = cnf_ptr->auth_response.data_len;
  (void)memscpy(ind.data,
                sizeof(ind.data),
                cnf_ptr->auth_response.data_ptr,
                ind.data_len);

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;

} /* qmi_sim_internal_send_usim_authenticate_ind */


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
)
{
  sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01  ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_session_perso_ota_deperso_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  if(cnf_ptr->perso_cnf_data.ota_deperso_cnf.data_len > 0
     && cnf_ptr->perso_cnf_data.ota_deperso_cnf.data_len < QMI_SIM_INTERNAL_MMGSDI_DATA_MAX_LENGTH_V01
     && cnf_ptr->perso_cnf_data.ota_deperso_cnf.data_ptr!=NULL)
  {
    ind.ota_deperso_resp_valid = TRUE;
    ind.ota_deperso_resp.data_len = cnf_ptr->perso_cnf_data.ota_deperso_cnf.data_len;
    (void)memscpy(ind.ota_deperso_resp.data,
                  sizeof(ind.ota_deperso_resp.data),
                  cnf_ptr->perso_cnf_data.ota_deperso_cnf.data_ptr,
                  ind.ota_deperso_resp.data_len);
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;

} /* qmi_sim_internal_send_session_perso_ota_deperso_ind */


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
)
{
  sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01  ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_run_gsm_algo_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.run_gsm_algo_resp_valid = TRUE;
  (void)memscpy(ind.run_gsm_algo_resp.kc_resp,
                sizeof(ind.run_gsm_algo_resp.kc_resp),
                cnf_ptr->kc_resp,
                QMI_SIM_INTERNAL_MMGSDI_GSM_ALGO_KC_LEN_V01);
  (void)memscpy(ind.run_gsm_algo_resp.sres_resp,
                sizeof(ind.run_gsm_algo_resp.sres_resp),
                cnf_ptr->sres_resp,
                QMI_SIM_INTERNAL_MMGSDI_GSM_ALGO_SRES_LEN_V01);

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;

} /* qmi_sim_internal_send_run_gsm_algo_ind */


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
)
{
  sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01  ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_register_for_refresh_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.orig_refresh_req_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_orig(cnf_ptr->orig_refresh_req,
                                                          &ind.orig_refresh_req);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;

} /* qmi_sim_internal_send_register_for_refresh_ind */


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
)
{
  sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01  ind;
  qmi_csi_error                                                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_deregister_for_refresh_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_deregister_for_refresh_ind */


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
)
{
  sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01 ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_ok_to_proceed_with_refresh_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.orig_refresh_req_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_orig(cnf_ptr->orig_refresh_req,
                                                          &ind.orig_refresh_req);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_ok_to_proceed_with_refresh_ind */


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
)
{
  sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01 ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_register_for_refresh_ok_to_proceed_voting_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.orig_refresh_req_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_orig(cnf_ptr->orig_refresh_req,
                                                          &ind.orig_refresh_req);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_IND");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_register_for_refresh_ok_to_proceed_voting_ind */


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
)
{
  sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01 ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_ok_to_refresh_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.orig_refresh_req_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_orig(cnf_ptr->orig_refresh_req,
                                                          &ind.orig_refresh_req);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_IND");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_ok_to_refresh_ind */


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
===========================================================================*/
qmi_csi_cb_error qmi_sim_internal_send_refresh_complete_ind
(
  qmi_sim_internal_local_client_ref_type   *local_client_ref_ptr,
  mmgsdi_refresh_cnf_type                  *cnf_ptr
)
{
  sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01 ind;
  qmi_csi_error                                             resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_refresh_complete_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.orig_refresh_req_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_orig(cnf_ptr->orig_refresh_req,
                                                          &ind.orig_refresh_req);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_IND");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_refresh_complete_ind */


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
)
{
  sim_internal_mmgsdi_session_search_ind_msg_v01  ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_search_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_search_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate searched_record_nums */
  if(cnf_ptr->searched_record_nums.data_len > 0 &&
     cnf_ptr->searched_record_nums.data_len < QMI_SIM_INTERNAL_MMGSDI_DATA_MAX_LENGTH_V01 &&
     cnf_ptr->searched_record_nums.data_ptr != NULL)
  {
      ind.searched_record_nums_valid = TRUE;
      ind.searched_record_nums.data_len = cnf_ptr->searched_record_nums.data_len;
      (void)memscpy(ind.searched_record_nums.data,
                    sizeof(ind.searched_record_nums.data),
                    cnf_ptr->searched_record_nums.data_ptr,
                    ind.searched_record_nums.data_len);
  }

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_search_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_search_ind */


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
)
{
  sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01  ind;
  qmi_csi_error                   resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_register_for_file_update_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01));
  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_register_for_file_update_ind */


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
  qmi_sim_internal_local_client_ref_type *local_client_ref_ptr,
  mmgsdi_increase_cnf_type               *cnf_ptr
)
{
  sim_internal_mmgsdi_session_increase_ind_msg_v01  ind;
  qmi_csi_error                                     resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_increase_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_increase_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  /* populate write resp data*/
  ind.increase_data_valid = TRUE;
  ind.increase_data.data_len = cnf_ptr->increased_data.data_len;
  (void)memscpy(ind.increase_data.data,
                sizeof(ind.increase_data.data),
                cnf_ptr->increased_data.data_ptr,
                ind.increase_data.data_len);

  /* populate file attr */
  switch(cnf_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      ind.file_enum_file_valid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(cnf_ptr->access.file.file_enum,
                                           &ind.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      ind.path_type_file_valid = TRUE;
      ind.path_type_file.path_buf_len = cnf_ptr->access.file.path_type.path_len;
      (void)memscpy(ind.path_type_file.path_buf,
                    sizeof(ind.path_type_file.path_buf),
                    cnf_ptr->access.file.path_type.path_buf,
                    (sizeof(uint16) * ind.path_type_file.path_buf_len));
      break;

    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    cnf_ptr->access.access_method);
      break;
  }

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_increase_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_increase_ind */


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
)
{
  sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01  ind;
  qmi_csi_error                                               resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_get_all_pin_status_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.status_word.valid = cnf_ptr->response_header.status_word.present;
  ind.response_header.status_word.sw1   = cnf_ptr->response_header.status_word.sw1;
  ind.response_header.status_word.sw2   = cnf_ptr->response_header.status_word.sw2;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(cnf_ptr->response_header.slot_id,
                                                     &ind.response_header.slot_id);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(cnf_ptr->pin1,
                                                            &(ind.all_pin_info.pin1));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(cnf_ptr->pin2,
                                                            &(ind.all_pin_info.pin2));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(cnf_ptr->universal_pin,
                                                            &(ind.all_pin_info.universal_pin));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(cnf_ptr->hiddenkey,
                                                            &(ind.all_pin_info.hiddenkey));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  ind.all_pin_info_valid = TRUE;

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_get_all_pin_status_ind */


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
)
{
  sim_internal_mmgsdi_naa_refresh_ind_msg_v01  ind;
  qmi_csi_error                                resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_send_naa_refresh_ind");
  ASSERT(local_client_ref_ptr != NULL);
  ASSERT(cnf_ptr != NULL);

  memset(&ind, 0, sizeof(sim_internal_mmgsdi_naa_refresh_ind_msg_v01));

  ind.response_header.mmgsdi_status     = qmi_sim_internal_convert_mmgsdi_status(cnf_ptr->response_header.mmgsdi_status);
  ind.response_header.client_id         = cnf_ptr->response_header.client_id;
  ind.response_header.session_id        = cnf_ptr->response_header.session_id;
  ind.response_header.response_len      = cnf_ptr->response_header.response_len;
  ind.response_header.payload_len       = cnf_ptr->response_header.payload_len;
  ind.response_header.client_ref        = local_client_ref_ptr->remote_client_ref;
  ind.response_header.response_handle   = local_client_ref_ptr->remote_cmd_cb;
  ind.token_valid                       = (local_client_ref_ptr->token_valid) ? 1 : 0;
  ind.token                             = local_client_ref_ptr->token;

  ind.orig_refresh_req_valid = TRUE;
  ind.orig_refresh_req       = MMGSDI_REFRESH_ORIG_REFRESH_REQ;

  UIM_MSG_HIGH_0("qmi_csi_send_ind");
  resp_err = qmi_csi_send_ind(local_client_ref_ptr->qcsi_client->client_handle,
                              QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_IND_V01,
                              &ind,
                              sizeof(sim_internal_mmgsdi_naa_refresh_ind_msg_v01));

  if(resp_err != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_0("Failed to send ind for QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_IND_V01");
  }

  /* Remove token from txn list */
  QMI_SIM_INTERNAL_VALIDATE_AND_REMOVE_TOKEN_FROM_TXN_LIST(
    local_client_ref_ptr->token_valid,
    local_client_ref_ptr->token);

  return resp_err;
} /* qmi_sim_internal_send_naa_refresh_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_card_inserted_evt_ind");
  ASSERT(card_inserted_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_card_inserted_valid          = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(card_inserted_ptr->slot,
                                                     &evt_ind->evt_card_inserted.slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  evt_ind->evt_card_inserted.aid_info_len   = card_inserted_ptr->num_aids_avail;
  qmi_sim_internal_populate_mmgsdi_aid_type(evt_ind->evt_card_inserted.aid_info_len,
                                            card_inserted_ptr->aid_info,
                                            evt_ind->evt_card_inserted.aid_info);
  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_card_inserted_evt_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_card_error_evt_ind");
  ASSERT(card_error_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_card_error_valid = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(card_error_ptr->slot,
                                                     &evt_ind->evt_card_error.slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  evt_ind->evt_card_error.info  = qmi_sim_internal_convert_mmgsdi_card_err_info(card_error_ptr->info);

  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_card_error_evt_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_pin_evt_ind");
  ASSERT(pin_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_pin_valid           = TRUE;
  resp_err = qmi_sim_internal_convert_mmgsdi_pin_evt(evt, &evt_ind->evt_pin.pin_event_enum);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_populate_mmgsdi_pin_info_type(pin_ptr->pin_info,
                                                            &evt_ind->evt_pin.pin_info);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(pin_ptr->slot,
                                                     &evt_ind->evt_pin.slot);

  evt_ind->evt_pin.aid_type_len                       = pin_ptr->num_aids;
  evt_ind->evt_pin.dir_index                          = pin_ptr->dir_index;
  qmi_sim_internal_populate_mmgsdi_aid_type(evt_ind->evt_pin.aid_type_len,
                                            pin_ptr->aid_type,
                                            evt_ind->evt_pin.aid_type);
  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_pin_evt_ind */



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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_session_changed_evt_ind");
  ASSERT(mmgsdi_session_changed_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_session_changed_valid     = TRUE;
  evt_ind->evt_session_changed.activated = mmgsdi_session_changed_ptr->activated;
  resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&mmgsdi_session_changed_ptr->app_info,
                                                            &evt_ind->evt_session_changed.app_info);

  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_session_changed_evt_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;
  ASSERT(mmgsdi_sub_ready_ptr != NULL);
  ASSERT(evt_ind != NULL);

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_subscription_ready_evt_ind");

  evt_ind->evt_subscription_ready_valid = TRUE;
  resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&mmgsdi_sub_ready_ptr->app_info,
                                                            &evt_ind->evt_subscription_ready.app_info);

  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_subscription_ready_evt_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_session_close_evt_ind");
  ASSERT(session_close_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_session_close_valid          = TRUE;
  evt_ind->evt_session_close.session_id     = session_close_ptr->session_id;
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(session_close_ptr->slot,
                                                     &evt_ind->evt_card_inserted.slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  evt_ind->evt_session_close.app_id.data_len   = session_close_ptr->app_id.data_len;
  (void)memscpy(&evt_ind->evt_session_close.app_id.data,
                sizeof(evt_ind->evt_session_close.app_id.data),
                session_close_ptr->app_id.data_ptr,
                evt_ind->evt_session_close.app_id.data_len);

  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_session_close_evt_ind */


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
)
{
  qmi_csi_error                        resp_err = QMI_CSI_NO_ERR;

  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_perso_evt_ind");
  ASSERT(evt_ind != NULL);

  evt_ind->evt_perso_valid                  = TRUE;
  evt_ind->evt_perso.num_retries            = perso.num_retries;
  evt_ind->evt_perso.num_unblock_retries    = perso.num_unblock_retries;
  qmi_sim_internal_convert_mmgsdi_perso_feature(perso.feature,
                                                &evt_ind->evt_perso.feature);
  qmi_sim_internal_convert_mmgsdi_perso_status(perso.status,
                                                &evt_ind->evt_perso.status);
  return resp_err;
} /* qmi_sim_internal_process_mmgsdi_perso_evt_ind */


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
)
{
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_illegal_session_evt_ind");
  ASSERT(evt_ind != NULL);

  evt_ind->evt_illegal_session_valid          = TRUE;
  evt_ind->evt_illegal_session.session_id     = illegal_session.session_id;

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_illegal_session_evt_ind */


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
)
{
  qmi_csi_error resp_err  = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_refresh_evt_ind");
  ASSERT(refresh_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_refresh_valid                       = TRUE;
  /* refresh file list TLV*/
  evt_ind->evt_refresh.refresh_files.file_list_len = refresh_ptr->refresh_files.num_files;
  if(evt_ind->evt_refresh.refresh_files.file_list_len)
  {
    uint8         i         = 0;
    uint32        offset    = 0;
    for(i = 0; i < evt_ind->evt_refresh.refresh_files.file_list_len; i++)
    {
      qmi_sim_internal_convert_mmgsdi_file(
                *(refresh_ptr->refresh_files.file_list_ptr + offset),
                &evt_ind->evt_refresh.refresh_files.file_list[i]);
      offset += sizeof(mmgsdi_file_enum_type);
    }
  }
  evt_ind->evt_refresh.refresh_files.file_path_len = refresh_ptr->refresh_files.file_path_len;
  (void)memscpy(&evt_ind->evt_refresh.refresh_files.file_path,
                sizeof(evt_ind->evt_refresh.refresh_files.file_path),
                refresh_ptr->refresh_files.file_path_ptr,
                evt_ind->evt_refresh.refresh_files.file_path_len);
  /* PLMN list TLV*/
  evt_ind->evt_refresh.plmn_list.data_len = refresh_ptr->plmn_list.data_len;
  (void)memscpy(&evt_ind->evt_refresh.plmn_list.data,
                sizeof(evt_ind->evt_refresh.plmn_list.data),
                refresh_ptr->plmn_list.data_ptr,
                evt_ind->evt_refresh.plmn_list.data_len);
  /* aid */
  qmi_sim_internal_populate_mmgsdi_aid_type(1,
                                            &refresh_ptr->aid,
                                            &evt_ind->evt_refresh.aid);
  /* app info*/
  resp_err = qmi_sim_internal_populate_mmgsdi_app_info_type(&refresh_ptr->app_info,
                                                 &evt_ind->evt_refresh.app_info);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  /* stage */
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_stage(refresh_ptr->stage,
                                                           &evt_ind->evt_refresh.stage);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  /* mode */
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_mode(refresh_ptr->mode,
                                                          &evt_ind->evt_refresh.mode);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  /* orig mode*/
  resp_err = qmi_sim_internal_convert_mmgsdi_refresh_mode(refresh_ptr->orig_mode,
                                                          &evt_ind->evt_refresh.orig_mode);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  /* slot id */
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(refresh_ptr->slot,
                                                     &evt_ind->evt_refresh.slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }
  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_refresh_evt_ind */


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
)
{
  qmi_csi_error resp_err  = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_terminal_profile_dl_evt_ind");
  ASSERT(tp_dl_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_terminal_profile_valid = TRUE;

  /* slot id */
  resp_err = qmi_sim_internal_convert_mmgsdi_slot_id(tp_dl_ptr->slot,
                                                     &evt_ind->evt_terminal_profile.slot);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_protocol(tp_dl_ptr->protocol,
                                                      &evt_ind->evt_terminal_profile.protocol);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  evt_ind->evt_terminal_profile.ok_dl = tp_dl_ptr->ok_dl;

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_terminal_profile_dl_evt_ind */


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
)
{
  qmi_csi_error resp_err  = QMI_CSI_NO_ERR;
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_file_update_evt_ind");
  ASSERT(file_update_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_update_valid = TRUE;

  /* check for access method and populate access type*/
  switch(file_update_ptr->file.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      evt_ind->evt_update.file_enum_file_isvalid = TRUE;
      qmi_sim_internal_convert_mmgsdi_file(file_update_ptr->file.file.file_enum,
                                           &evt_ind->evt_update.file_enum_file);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      evt_ind->evt_update.path_type_file_isvalid = TRUE;
      evt_ind->evt_update.path_type_file.path_buf_len = file_update_ptr->file.file.path_type.path_len;
      (void)memscpy(evt_ind->evt_update.path_type_file.path_buf,
                    sizeof(evt_ind->evt_update.path_type_file.path_buf),
                    file_update_ptr->file.file.path_type.path_buf,
                    (sizeof(uint16) * evt_ind->evt_update.path_type_file.path_buf_len));
      break;

    case MMGSDI_DF_ENUM_ACCESS:
    case MMGSDI_BY_APP_ID_ACCESS:
    default:
      UIM_MSG_ERR_1("Failed to handle access method 0x%x",
                    file_update_ptr->file.access_method);
      break;
  }

  resp_err = qmi_sim_internal_convert_mmgsdi_file_structure(file_update_ptr->file_type,
                                                            &evt_ind->evt_update.file_type);
  if(resp_err != QMI_CSI_NO_ERR)
  {
    return resp_err;
  }

  switch(evt_ind->evt_update.file_type)
  {
    case SIM_INTERNAL_MMGSDI_LINEAR_FIXED_FILE_V01:
    case SIM_INTERNAL_MMGSDI_CYCLIC_FILE_V01:
      evt_ind->evt_update.rec_num_isvalid = TRUE;
      evt_ind->evt_update.rec_num = file_update_ptr->file_index.rec_num;
      break;
    case SIM_INTERNAL_MMGSDI_TRANSPARENT_FILE_V01:
      evt_ind->evt_update.offset_isvalid = TRUE;
      evt_ind->evt_update.offset = file_update_ptr->file_index.offset;
      break;
    default:
      UIM_MSG_HIGH_0("no file index is needed");
      break;
  }

  evt_ind->evt_update.file_data.data_len = file_update_ptr->file_data.data_len;
  (void)memscpy(evt_ind->evt_update.file_data.data,
                sizeof(evt_ind->evt_update.file_data.data),
                file_update_ptr->file_data.data_ptr,
                evt_ind->evt_update.file_data.data_len);

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_file_update_evt_ind */


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
( const mmgsdi_eons_opl_pnn_cache_ready_evt_info_type           *eons_ptr,
  sim_internal_mmgsdi_event_report_ind_msg_v01                  *evt_ind
)
{
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_eons_opl_pnn_cache_ready_evt_ind");
  ASSERT(eons_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_eons_opl_pnn_cache_ready_valid = TRUE;

  evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.num_of_rec = eons_ptr->opl_info.num_of_rec;
  evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.rec_len = eons_ptr->opl_info.rec_len;
  if(eons_ptr->opl_info.valid_record_list.data_len > 0 &&
     eons_ptr->opl_info.valid_record_list.data_ptr != NULL)
  {
    evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.valid_record_list.data_len
      = eons_ptr->opl_info.valid_record_list.data_len;
    (void)memscpy(evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.valid_record_list.data,
                  sizeof(evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.valid_record_list.data),
                  eons_ptr->opl_info.valid_record_list.data_ptr,
                  evt_ind->evt_eons_opl_pnn_cache_ready.opl_info.valid_record_list.data_len);
  }

  evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.num_of_rec = eons_ptr->pnn_info.num_of_rec;
  evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.rec_len = eons_ptr->pnn_info.rec_len;
  if(eons_ptr->pnn_info.valid_record_list.data_len > 0 &&
     eons_ptr->pnn_info.valid_record_list.data_ptr != NULL)
  {
    evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data_len
      = eons_ptr->pnn_info.valid_record_list.data_len;
    (void)memscpy(evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data,
                  sizeof(evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data),
                  eons_ptr->pnn_info.valid_record_list.data_ptr,
                  evt_ind->evt_eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data_len);
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_eons_opl_pnn_cache_ready_evt_ind */


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
)
{
  UIM_MSG_HIGH_0("qmi_sim_internal_process_mmgsdi_fdn_evt_ind");
  ASSERT(fdn_ptr != NULL);
  ASSERT(evt_ind != NULL);

  evt_ind->evt_fdn_valid = TRUE;

  qmi_sim_internal_convert_mmgsdi_slot_id(fdn_ptr->slot,
                                          &(evt_ind->evt_fdn.slot));
  evt_ind->evt_fdn.enabled = (uint8_t)fdn_ptr->enabled;
  evt_ind->evt_fdn.rec_num = (uint16_t)fdn_ptr->rec_num;
  if(fdn_ptr->rec_data.data_len > 0 &&
     fdn_ptr->rec_data.data_len <= sizeof(evt_ind->evt_fdn.rec_data.data) &&
     fdn_ptr->rec_data.data_ptr != NULL)
  {
    evt_ind->evt_fdn.rec_data.data_len = (uint32_t)fdn_ptr->rec_data.data_len;
    (void)memscpy(evt_ind->evt_fdn.rec_data.data,
                  sizeof(evt_ind->evt_fdn.rec_data.data),
                  fdn_ptr->rec_data.data_ptr,
                  evt_ind->evt_fdn.rec_data.data_len);
  }

  return QMI_CSI_NO_ERR;
} /* qmi_sim_internal_process_mmgsdi_fdn_evt_ind */


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
)
{
  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  qmi_sim_internal_state.txn_list[qmi_sim_internal_state.txn_index].state = QMI_SIM_INTERNAL_TXN_REQ_RCVD;
  qmi_sim_internal_state.txn_list[qmi_sim_internal_state.txn_index].token = token;
  if(qmi_sim_internal_state.txn_index == QMI_SIM_INTERNAL_MAX_TXN - 1)
  {
    qmi_sim_internal_state.txn_index = 0;
  }
  else
  {
    qmi_sim_internal_state.txn_index++;
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
} /* qmi_sim_internal_add_token_to_txn_list */


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
)
{
  uint8   txn_index = 0;

  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  for (txn_index = 0; txn_index < QMI_SIM_INTERNAL_MAX_TXN; txn_index++)
  {
    if ((qmi_sim_internal_state.txn_list[txn_index].state == QMI_SIM_INTERNAL_TXN_RESP_SENT) &&
        (qmi_sim_internal_state.txn_list[txn_index].token == token))
    {
      qmi_sim_internal_state.txn_list[txn_index].state = QMI_SIM_INTERNAL_TXN_UNUSED;
      QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
      return;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
} /* qmi_sim_internal_remove_token_from_txn_list */


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
)
{
  uint8   txn_index = 0;

  QMI_SIM_INTERNAL_ENTER_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
  for (txn_index = 0; txn_index < QMI_SIM_INTERNAL_MAX_TXN; txn_index++)
  {
    if ((qmi_sim_internal_state.txn_list[txn_index].state == QMI_SIM_INTERNAL_TXN_REQ_RCVD) &&
        (qmi_sim_internal_state.txn_list[txn_index].token == token))
    {
      qmi_sim_internal_state.txn_list[txn_index].state = QMI_SIM_INTERNAL_TXN_RESP_SENT;
      QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
      return;
    }
  }
  QMI_SIM_INTERNAL_LEAVE_CRIT_SECT(qmi_sim_internal_data_crit_sect_ptr);
} /* qmi_sim_internal_update_txn_resp_sent */

#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */
