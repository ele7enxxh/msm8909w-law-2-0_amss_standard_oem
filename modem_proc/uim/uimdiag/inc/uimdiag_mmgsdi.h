#ifndef UIMDIAG_MMGSDI_H
#define UIMDIAG_MMGSDI_H

/*===========================================================================


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2014 by QUALCOMM Technologies, Inc (QTI).
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/inc/uimdiag_mmgsdi.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
07/10/14   tl    Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
09/24/13   bcho  Parsing for SPN disp bit, RPLMN disp bit and roaming status
05/24/13   bcho  Support for perso get feature data Cnf
04/12/13   spo   Updated variable to hold dynamic data for Send Card Status
11/23/12   bcho  get operator name command rsp structure modified
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
08/31/12   abg   Updated UPIN replacement in Get All Pin Status response
02/29/12   bcho  Q6 compilation warnings removed
01/05/12   nb    Updated read response data buffer size
11/17/11   kk    Added get all available apps async support
10/24/11   adp   Removing Num_of_files from request field for naa_refresh
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
10/10/11   nb    Support for SAP CONNECT/SAP DISCONNECT
10/08/11   adp   Changing uimdiag_mmgsdi_rsp_session_get_info_pyld_type and
                 uimdiag_mmgsdi_session_read_cache_rsp_type to match ICD.
09/26/11   adp   Fixed crash, where memory pointer is NULL, but was being
                 assigned a value
09/12/11   adp   Adding support to trigger refresh in uimdiag,
                 removing some paramters
05/27/11   nb    Support for new refresh Related APIs
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
06/09/11   adp   Fixing get file attr by path access, Fixing search and seek
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
11/09/10   adp   Adding Event support in UIMDIAG
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/26/10   adp   Fixing Compiler Warning.
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
07/14/10   js    Fixed compiler warnings
07/07/10   ps    Fix compiler warnings
04/27/10   shr   Added Hiddenkey support
04/15/10   adp   Added support for PACKED structs for Q6. PACKED_POST keyword
                 is added for each packed union/structure.
04/15/10   adp   Compilation error on target for uimdiag code. Creating dummy
                 structures in order to comply with the PACKED structures
03/25/10   adp   Adding support for deactivate_provisioning,
                 get_app_capabilities, session_get_cphs_info,
                 isim_authenticate,mmgsdi_session_send_card_status
                 create_pkcs15_loopkup_table
03/19/10   rk    Added support for JCDMA card info command and event handling
02/22/10   adp   Adding support for Perso Commands
12/27/09   ap    Adding support for Refresh Commands
12/03/09   rk    Adding support for get file attributes, read and write commands
10/19/09   adp   Adding pin commands
10/18/09   adp   Moving session_opn,session_close, activate_or_switch,
                 session_get_info, client_id_and_evt_reg,
                 client_id_and_evt_dereg and corresponding structures to
                 appropriate header file. Adding support for session_get_info
                 and activate_and_switch_provisioning commands
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uimdiag.h"
#include "uim.h"
#include "uim_v.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdilib_common.h"
#include "diagpkt.h"

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
#define UIMDIAG_MAX_PIN                         34
#define UIMDIAG_ISIM_DATA_LEN                   256
#define EVENT_MAX_PAYLOAD                       15
#define UIMDIAG_MAX_PERSO_KEY_LENGTH            0x0F
#define UIMDIAG_MAX_IMEI_LENGTH                 0x20
#define UIMDIAG_MAX_DATA_BLOCK_LEN              512
#define UIMDIAG_SESSION_ID_SIZE                 4
#define UIMDIAG_MAX_ROAMING_LIST_LENGTH         0x20
#define UIMDIAG_MAX_READ_DATA_LEN               600
#define UIMDIAG_MAX_SESSIONS                    0x06
#define UIMDIAG_MAX_NVRUIM_DATA_SYNC            0x0A

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:     uimdiag_mmgsdi_session_open_cmd_type

   DESCRIPTION:      Request params for session open command
   client_id_high:   Upper 32 bits of the client ID
   client_id_low:    Lower 32 bits of the client ID
   session_type:     Indicates the type of session (GW, 1X or non-prov App.)
   set_notify:       Boolean indicating whether clients should call
                     mmgsdi_session_prov_app_init_complete() when they
                     finish their initialization procedure for the
                     applications. This is only applicable to the
                     provisioning-session types.
   exclusive_channel:Indicates whether the channel associated with the session
                     must be exclusive for the requesting client
   aid_data_len:     Length of Application Identifier
   aid_data_ptr:     Unsigned integer Pointer to Application Identifier
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
  uint32                  session_type;
  uint32                  set_notify;
  uint32                  exclusive_channel;
  uint32                  aid_data_len;
  uint8                   * aid_data_ptr;
}uimdiag_mmgsdi_session_open_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_sessionid_type

   DESCRIPTION:     Request params for session id. Used for
                    close command, deact_prov_cmd, get_app_cap,
                    get_cphs_info_cmd, create_pkcs15_lookup_tbl,
                    session_get_all_pin_status,
                    jcdma_get_card_info_cmd
                    Also used in response payload types:
                    rsp_session_act_or_sw_prov_pyld,
                    rsp_session_refresh_pyld,
                    rsp_session_perso_data_pyld
   session_id_high: Higher 32 bits of session ID for session to be closed
   session_id_low:  Lower  32 bits of session ID for session to be closed
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
}uimdiag_mmgsdi_sessionid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_nvruim_data_req_type

   DESCRIPTION:     Request params for nvruim data request

   session_id_high: Higher 32 bits of session ID for session to be closed
   session_id_low: Lower  32 bits of session ID for session to be closed
   nvruim_data_req_enum: NVRUIM global data request type
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                               session_id_high;
  uint32                               session_id_low;
  mmgsdi_nvruim_data_req_enum_type     nvruim_data_req_enum;
}uimdiag_mmgsdi_nvruim_data_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_client_id_and_evt_dereg_cmd_type

   DESCRIPTION:     Request params for client_id_and_evt_dereg command
   client_id_high: Higher 32 bits of Client ID
   client_id_low:  Lower  32 bits of Client ID
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
}uimdiag_mmgsdi_client_id_and_evt_dereg_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_session_act_or_sw_prov_cmd_type

   DESCRIPTION:     Request params for session_act_or_sw_prov command
   session_id_high: Higher value of the session ID for the session
   session_id_low:  Lower value of the session ID for the session
   slot_id:         Physical slot for multiple slot targets.
   app_id_data:     Application Identifier.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                    session_id_high;
  uint32                    session_id_low;
  uint32                    slot_id;
  uint32                    data_len;
  uint8                     * data_ptr;
}uimdiag_mmgsdi_session_act_or_sw_prov_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_session_get_info_cmd_type

   DESCRIPTION:     Request params for session get info command
   session_id_high: Higher 32 bits of session ID
   session_id_low:  Lower  32 bits of session ID
   session_info:    This has query key information, by which a client query
                    MMGSDI for session information
   session_type:    Indicates the type of session (GW, 1X or non-prov App.)
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                        session_id_high;
  uint32                        session_id_low;
  uint32                        session_info;
  uint32                        session_type;
}uimdiag_mmgsdi_session_get_info_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:       uimdiag_mmgsdi_session_verify_pin_cmd_type

   DESCRIPTION:     Used to perform a PIN verification procedure against a card
                    for PIN1, PIN2 or UPIN
   session_id_high: Higher 32 bits of session ID
   session_id_low:  Lower  32 bits of session ID
   pin_id:          Pin ID
   pin_len:         Length of Pin Value
   pin_value_ptr:   PIN value in ASCII format with length specified by PIN_LEN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pin_id;
  uint32                  pin_len;
  uint8                 * pin_value_ptr;
}uimdiag_mmgsdi_session_verify_pin_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:       uimdiag_mmgsdi_session_disable_pin_cmd_type

   DESCRIPTION:     Used to build request to MMGSDI to disable the PIN provided
   session_id_high: Higher 32 bits of session ID
   session_id_low:  Lower  32 bits of session ID
   pin_id:          Pin ID
   pin_replace:     Replacement option, indicating whether the PIN should be
                    replaced by the Universal PIN
   pin_len:         Length of Pin Value
   pin_value_ptr:   PIN value in ASCII format with length specified by PIN_LEN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pin_id;
  uint32                  pin_replace;
  uint32                  pin_len;
  uint8                 * pin_value_ptr;
}uimdiag_mmgsdi_session_disable_pin_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_session_enable_pin_cmd_type

   DESCRIPTION:     Used to build request to MMGSDI to enable the PIN provided
   session_id_high: Higher 32 bits of session ID
   session_id_low:  Lower  32 bits of session ID
   pin_id:          Pin ID
   pin_len:         Length of Pin Value
   pin_value_ptr:   PIN value in ASCII format with length specified by PIN_LEN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pin_id;
  uint32                  pin_len;
  uint8                 * pin_value_ptr;
}uimdiag_mmgsdi_session_enable_pin_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_session_change_pin_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to change the PIN provided
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   pin_id:            Pin ID
   pin_len:           Length of PIN_VALUE
   pin_value_ptr:     PIN value in ASCII format with length specified by PIN_LEN
   new_pin_len:       Length of New PIN_VALUE
   new_pin_value_ptr: PIN value in ASCII format with length specified by
                      NEW_PIN_LEN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pin_id;
  uint32                  pin_len;
  uint8                 * pin_value_ptr;
  uint32                  new_pin_len;
  uint8                 * new_pin_value_ptr;
}uimdiag_mmgsdi_session_change_pin_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_unblock_pin_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to unblock the PIN provided
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   pin_id:            Pin ID
   puk_len:           Length of PUK
   puk_value_ptr:     PUK value in ASCII format with length specified by PUK_LEN
   new_pin_len:       Length of New PIN_VALUE
   new_pin_value_ptr: PIN value in ASCII format with length specified by
                      NEW_PIN_LEN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pin_id;
  uint32                  puk_len;
  uint8                 * puk_value_ptr;
  uint32                  new_pin_len;
  uint8                 * new_pin_value_ptr;
}uimdiag_mmgsdi_session_unblock_pin_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_get_file_attr_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to get file attr
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   access_method:     Indication of access method
   ef_enum:           File enum to access SIM/USIM/RUIM elementary Files
   df_enum:           Dir enum to access directories.
   path_len:          path len to access path
   path:              path to access EFs/DFs
   aid_len;           length of the application id
   aid:               application id
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  access_method;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint8                 * path_ptr;
  uint32                  aid_len;
  uint8                 * aid_ptr;
}uimdiag_mmgsdi_session_get_file_attr_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_read_record_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to read record
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   access_method:     Indication of access method
   ef_enum:           File enum to access SIM/USIM/RUIM Elementary Files.
   df_enum:           Dir enum to access SIM/USIM/RUIM Directories.
   path_len:          path len to access path
   path:              path to access EFs/DFs
   aid_len;           length of the application id
   aid:               application id
   rec_num:           record number where the read should be performed
   rec_len:           length of the requested data to read
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  access_method;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint8                 * path_ptr;
  uint32                  aid_len;
  uint8                 * aid_ptr;
  uint32                  rec_num;
  uint32                  rec_len;
}uimdiag_mmgsdi_session_read_record_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_read_transparent_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to read transparent
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   access_method:     Indication of access method
   ef_enum:           File enum to access SIM/USIM/RUIM Elementary Files.
   df_enum:           Dir enum to access SIM/USIM/RUIM Directories.
   path_len:          path len to access path
   path:              path to access EFs/DFs
   aid_len;           length of the application id
   aid:               application id
   offset:            offset from the start of the file from where the read should start
   rec_len:           length of the requested data to read
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  access_method;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint8                 * path_ptr;
  uint32                  aid_len;
  uint8                 * aid_ptr;
  uint32                  offset;
  uint32                  req_len;
}uimdiag_mmgsdi_session_read_transparent_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_write_record_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to write record
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   access_method:     Indication of access method
   ef_enum:           File enum to access SIM/USIM/RUIM Elementary Files.
   df_enum:           Dir enum to access SIM/USIM/RUIM Directories.
   path_len:          path len to access path
   path:              path to access EFs/DFs
   aid_len;           length of the application id
   aid:               application id
   rec_type:          record type
   rec_num:           record number where the write should be performed
   data_len:          length of data to write
   data_ptr:          The data to write, with a length specified by data_len
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  access_method;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint8                 * path_ptr;
  uint32                  aid_len;
  uint8                 * aid_ptr;
  uint32                  rec_type;
  uint32                  rec_num;
  uint32                  data_len;
  uint8                 * data_ptr;
 }uimdiag_mmgsdi_session_write_record_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_write_transparent_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to write to transparent file.
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   access_method:     Indication of access method
   ef_enum:           File enum to access SIM/USIM/RUIM Elementary Files.
   df_enum:           Dir enum to access SIM/USIM/RUIM Directories.
   path_len:          path len to access path
   path:              path to access EFs/DFs
   aid_len;           lenght of application id
   aid:               application id
   offset:            The offset location from where the write should start
   data_len:          length of data to write
   data_ptr:          The data to write, with a length specified by data_len
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  access_method;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint8                 * path_ptr;
  uint32                  aid_len;
  uint8                 * aid_ptr;
  uint32                  offset;
  uint32                  data_len;
  uint8                 * data_ptr;
}uimdiag_mmgsdi_session_write_transparent_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_refresh_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to register or deregister
                      for refresh.
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   num_of_files:       Num of files
   file_list_ptr:     File list with the length specified in the File_list_len
   file_path:         Length of File Path
   file_path_ptr:     File Path
   vote_for_init:     Indicate if the script wants to vote for init or not
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  num_of_files;
  uint32                  *file_list_ptr;
  uint32                  file_path_len;
  uint8                   *file_path_ptr;
  uint32                  vote_for_init;
}uimdiag_mmgsdi_session_refresh_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_refresh_proceed_voting_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to register for refresh and
                      also receive notification before proceedig with refresh
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   num_of_files:      Num of files
   file_list_ptr:     File list with the length specified in the File_list_len
   file_path:         Length of File Path
   file_path_ptr:     File Path
   vote_for_init:     Indicate if the script wants to vote for init or not
   proceed_refresh:   Indicate if the script wants to vote for proceed with refresh
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  num_of_files;
  uint32                  *file_list_ptr;
  uint32                  file_path_len;
  uint8                   *file_path_ptr;
  uint32                  vote_for_init;
  uint32                  proceed_refresh;
}uimdiag_mmgsdi_session_refresh_proceed_voting_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_ok_to_refresh_cmd_type

   DESCRIPTION:       Used to build request to Simualate OK to Refresh to MMGSDI
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   ok_to_refresh:     Client informs MMGSDI if it is ok to continue with
                      refresh or not
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  ok_to_refresh;
}uimdiag_mmgsdi_session_ok_to_refresh_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_proceed_refresh_cmd_type

   DESCRIPTION:       Used to build request to Simualate OK to Refresh to MMGSDI
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   ok_to_proceed:     Client informs MMGSDI if it is ok to continue with
                      refresh or not
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  ok_to_proceed;
}uimdiag_mmgsdi_session_proceed_refresh_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_naa_refresh_cmd_type

   DESCRIPTION:       Used to build request to Trigger Refresh to MMGSDI
   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   slot_id:          Slot Id
   sim_app_id_len:   App ID length
   sim_app_id:       App ID
   refresh_mode:     Refresh Mode
   path_len:         Path Length
   path:             Path
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
  uint32                  slot_id;
  uint32                  sim_app_id_len;
  uint8                 * sim_app_id;
  uint32                  refresh_mode;
  uint32                  path_len;
  uint8                 * path;
}uimdiag_mmgsdi_naa_refresh_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_refresh_complete_cmd_type

   DESCRIPTION:       Used to build request to indicate to MMGSDI that the
                      client has finished its refresh process
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   pass_fail:         Indicate simulation of the refresh "process" result
                      from the script side.
                        -   0x00: FAILED
                        -   0x01: SUCCEED
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  pass_fail;
}uimdiag_mmgsdi_session_refresh_complete_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_perso_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to activate/deactivate/
                      unblock personalization feature by an external source.
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   feature_name:      Personalization feature to activate
   data_len:          Perso client data length
   data_ptr:          Perso client data
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  feature_name;
  uint32                  data_len;
  uint8                   *data_ptr;
}uimdiag_mmgsdi_session_perso_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_perso_get_key_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to get perso key

   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   feature_ind:       Personalization feature indicator
   key_type:          Type of Key
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              feature;
  uint32              key_type;
}uimdiag_mmgsdi_session_perso_get_key_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_perso_get_ind_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to retrieve the
                      information about personalization Indicators
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   perso_feat_ind :   Identifier to get all or permanently disabled feature
                      indicators
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  perso_feat_ind;
}uimdiag_mmgsdi_session_perso_get_ind_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_perso_get_dck_retries_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to retrieve the number of
                      depersonaliz?ation attempts left/unblock DCK retries left
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   dck_retries_type:  Depersonalisation retires or unblock retries
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              dck_retries_type;
}uimdiag_mmgsdi_session_perso_get_dck_retries_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to lock down some of
                      the perso commands.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   action_locked:     Enum that defines what action can be locked down
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              action_locked;
}uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_isim_auth_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to perform a "Network"
                      Initiated Authentication with the ISIM Application.
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   auth_data_len:          Auth Challenge length
   auth_data_ptr:          Data of auth challenge
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  auth_data_len;
  uint8                   *auth_data_ptr;
}uimdiag_mmgsdi_session_isim_auth_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_send_crd_status_cmd_type

   DESCRIPTION:       Used to build request to MMGSDI to information
                      concerning the current directory
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   me_app_status:     The status of the application in the handset
  ret_data_struct:    Structure in which the data should be returned in
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  me_app_status;
  uint32                  ret_data_struct;
}uimdiag_mmgsdi_session_send_crd_status_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_service_cmd_type

   DESCRIPTION:       Used to build request to disable/ enable service
                      Also used to find out if service is available (Async / Sync API)
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   service:           Service Type
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              service;
}uimdiag_mmgsdi_session_service_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_seek_cmd_type

   DESCRIPTION:       Used to build request that performs GSM Seek command
                      on card.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower 32 bits of session ID
   access_method:     Method of Access
   ef_enum:           File enumeration
   df_enum:           DF enumeration
   path_len:          Path Length provided in the request
   path_ptr:          Path provided in the request
   aid_len:           Application ID Length
   aid__ptr:          Application ID
   seek_direction:    Seek Direction
   seek_pattern_len:  Seek Pattern Length
   seek_pattern_ptr:  Seek Pattern
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              access_method;
  uint32              ef_enum;
  uint32              df_enum;
  uint32              path_len;
  uint8             * path_ptr;
  uint32              aid_len;
  uint8             * aid_ptr;
  uint32              seek_direction;
  uint32              seek_pattern_len;
  uint8             * seek_pattern_ptr;
}uimdiag_mmgsdi_session_seek_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_search_cmd_type

   DESCRIPTION:       Used to build request that performs Search command on
                      UICC card.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower 32 bits of session ID
   access_method:     Method of Access
   ef_enum:           File enumeration
   df_enum:           DF enumeration
   path_len:          Path provided in the request
   path_ptr:          Path provided in the request
   aid_len:           Application ID
   aid__ptr:          Application ID
   search_record_num  Record Number, from where search will start in file.
   seek_direction:    Seek Direction
   offset_type:       Offset, from where search start in a record.
   offset_data:       Offset, from where search start in a record.
   search_pattern_len:Search Pattern
   search_pattern_ptr:Search Pattern
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{

  uint32              session_id_high;
  uint32              session_id_low;
  uint32              access_method;
  uint32              ef_enum;
  uint32              df_enum;
  uint32              path_len;
  uint8             * path_ptr;
  uint32              aid_len;
  uint8             * aid_ptr;
  uint32              search_type;
  uint32              search_record_num;
  uint32              search_direction;
  uint32              offset_type;
  uint32              offset_data;
  uint32              search_pattern_len;
  uint8             * search_pattern_ptr;
}uimdiag_mmgsdi_session_search_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_prov_app_init_compl_cmd_type

   DESCRIPTION:       Used to send prov_app_init_complete_status
   session_id_high:   Higher 32 bits of session ID
   session_id_low :   Lower  32 bits of session ID
   init_status: Initialization status (booean)
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              init_status;
}uimdiag_mmgsdi_session_prov_app_init_compl_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_compute_ip_auth_cmd_type

   DESCRIPTION:   Used to compute IP related authentication
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   operation_type:    Type of authentication operation
   chap_id, chap_nai_entry_index, chap_chal_len, chap_challenge :
                               CHAP operation for the SIP.
   mn_ha_nai_entry_index,mn_ha_reg_data_len,mn_ha_reg_data:
                               MN-HA authenticator.
   rrq_data_len,rrq_data : MIP-RRQ hash
   mn_aaa_nai_entry_index, mn_aaa_chal_len, mn_aaa_challenge:
                               MN-AAA authenticator.
   hrpd_chap_id, hrpd_chap_chal_len, hrpd_chap_challenge:
                                CHAP operation for High Rate Packet Data
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              operation_type;
  uint8               chap_id;
  uint8               chap_nai_entry_index;
  uint32              chap_chal_len;
  uint8*              chap_challenge;
  uint8               mn_ha_nai_entry_index;
  uint32              mn_ha_reg_data_len;
  uint8*              mn_ha_reg_data;
  uint32              rrq_data_len;
  uint8*              rrq_data;
  uint8               mn_aaa_nai_entry_index;
  uint32              mn_aaa_chal_len;
  uint8*              mn_aaa_challenge;
  uint8               hrpd_chap_id;
  uint32              hrpd_chap_chal_len;
  uint8*              hrpd_chap_challenge;
}uimdiag_mmgsdi_session_compute_ip_auth_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_ssd_update_cmd_type

   DESCRIPTION:    Used to initiate the calculation of a new SSD value
  for authentication purposes.

   session_id_high:  Higher 32 bits of session ID
   session_id_low:   Lower  32 bits of session ID
   randssd:              56-bit random number from the base station for SSD
                                 generation.
   process_control:  Process control bits
   esn:                     Electronic Serial Number of the ME
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              randssd_high;
  uint32              randssd_low;
  uint8               process_control;
  uint32              esn;
}uimdiag_mmgsdi_session_ssd_update_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_confirm_ssd_cmd_type

   DESCRIPTION:   Performs a Confirm SSD command.
  The authentication signature calculated by the network is compared to that
  calculated by the RUIM. If the results match, they both had the same SSD input,
  so the RUIM is authenticated.

   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   authbs:                 Authentication signature from the base station.
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              authbs;
}uimdiag_mmgsdi_session_confirm_ssd_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_bs_chal_cmd_type

   DESCRIPTION :   Used to perform an OTASP base station challenge before
  updating its SSD to ensure that the update process is being initiated by a
  legitimate source
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   rand_seed:           Random number generator seed.
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              randseed;
}uimdiag_mmgsdi_session_bs_chal_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_get_info_sync_cmd_type

   DESCRIPTION :   Query session-related information

   query_type:          0 - By Session_id, 1 - By session type
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   session_type:       Session Type
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              query_type;
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              session_type;
}uimdiag_mmgsdi_session_get_info_sync_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_read_cache_file_size_cmd_type

   DESCRIPTION :   Gets the size of the file to be cached for a read operation/Read the cached file.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   file_name:           Enumeration used to access the SIM, USIM, RUIM, CSIM EFs.
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              file_name;
}uimdiag_mmgsdi_session_read_cache_file_size_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_read_cache_cmd_type

   DESCRIPTION :   Gets the size of the file to be cached for a read operation/Read the cached file.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   file_name:           Enumeration used to access the SIM, USIM, RUIM, CSIM EFs.
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              file_name;
  uint32              data_len;
}uimdiag_mmgsdi_session_read_cache_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_write_cache_cmd_type

   DESCRIPTION :   Used to write data to a cached file.
   This function can be used to write only into a transparent file  in the cache.
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   file_name:            Enumeration used to access the SIM, USIM, RUIM, CSIM EFs.
   data:                    data to write
    ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint32              file_name;
  uint32              data_len;
  uint8*              data;
}uimdiag_mmgsdi_session_write_cache_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_cmd_type

   DESCRIPTION :   Notifies the UIM when the modem is in or out of the traffic channel.
   When the modem is in the traffic channel, power control is turned on.

   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   in_TC:           Indicates whether the modem is in or out of the traffic channel.
   mode:           Mode on which traffic channel established GW or 1X
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              session_id_high;
  uint32              session_id_low;
  uint8               in_TC;
  uint32              mode;
}uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_get_atr_cmd_type

   DESCRIPTION :     Retrieves the ATR Information which pertains to the
                     card(s) currently inserted in the ME.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   card_slot:        Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              card_slot;
}uimdiag_mmgsdi_sap_get_atr_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_send_apdu_cmd_type

   DESCRIPTION :     Allows to send an APDU to the SAP Client.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   card_slot:        Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
   channel_id:       The channel id.
   apdu_data_len:    Length of APDU sent to the card
   apdu_data_ptr:    The APDU to send to the card
  ------------------------------------------------------------------------------*/

typedef PACKED struct
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              card_slot;
  uint32              channel_id;
  uint32              apdu_data_len;
  uint8              *apdu_data_ptr;
}uimdiag_mmgsdi_sap_send_apdu_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_generic_cmd_type

   DESCRIPTION :     perform a SAP power on
                               SAP power off
                               SAP card reader status
                               SAP RESET
                               SAP initialization

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   card_slot:        Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
   sap_mode:         Indicates the SAP Mode
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              card_slot;
  uint32              sap_mode;
}uimdiag_mmgsdi_sap_generic_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_disconnect_cmd_type

   DESCRIPTION :     perform a SAP Disconnect

   client_id_high:     Higher 32 bits of client ID
   client_id_low:      Lower  32 bits of client ID
   card_slot:          Indicates the Slot ID, which tells the MMGSDI task and
                       drivers which slot  to communicate with.
   sap_mode:           Indicates the SAP Mode
   sap_disconnect_mode:Indicates SAP disconnect modes
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              card_slot;
  uint32              sap_mode;
  uint32              sap_disconnect_mode;
}uimdiag_mmgsdi_sap_disconnect_cmd_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_session_get_operator_name_cmd_type

   DESCRIPTION :   Request data for get operator name command

   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   no_of_plmns:       No of PLMN IDs
   plmn_id_list_ptr:   PLMN IDs
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                session_id_high;
  uint32                session_id_low;
  uint32                no_of_plmns;
  uint8                *plmn_list_ptr;
}uimdiag_mmgsdi_session_get_operator_name_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_card_pdown_cmd_type

   DESCRIPTION :     perform a power down for the SIM/USIM/RUIM.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   slot_id:          Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
   power_down_option:Indicates whether to just power down the CARD or do a
                     power down and notify GSDI about card removal.
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              slot_id;
  uint32              power_down_option;
}uimdiag_mmgsdi_card_pdown_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_card_pup_cmd_type

   DESCRIPTION :     perform a power down for the SIM/USIM/RUIM.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   slot_id:          Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
   power_up_option:  Power up and switch to passive.
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              slot_id;
  uint32              power_up_option;
}uimdiag_mmgsdi_card_pup_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_activate_onchip_sim_cmd_type

   DESCRIPTION :      To configure the GSDI SIM Cache, the Authentication
                      Algorithm Key and Activate the ONCHIP SIM Functionality.

   client_id_high:    Higher 32 bits of client ID
   client_id_low:     Lower  32 bits of client ID
   slot_id:           Indicates the Slot ID, which tells the MMGSDI task and
                      drivers which slot  to communicate with.
   onchip_data_length:TLV data length
   onchip_data_ptr:   TLV data
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              slot_id;
  uint32              onchip_data_length;
  uint8              *onchip_data_ptr;
}uimdiag_mmgsdi_activate_onchip_sim_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_send_apdu_cmd_type

   DESCRIPTION :     To send raw APDUs.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
   slot_id:          Indicates the Slot ID, which tells the MMGSDI task and
                     drivers which slot  to communicate with.
   apdu_data_length: APDU buffer length
   apdu_data_ptr:    APDU buffer
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              client_id_high;
  uint32              client_id_low;
  uint32              slot_id;
  uint32              apdu_data_length;
  uint8              *apdu_data_ptr;
}uimdiag_mmgsdi_send_apdu_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_get_all_available_apps_cmd_type

   DESCRIPTION :    Request struct for get all available apps.

   client_id_high:   Higher 32 bits of client ID
   client_id_low:    Lower  32 bits of client ID
  ------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32              client_id_high;
  uint32              client_id_low;
}uimdiag_mmgsdi_get_all_available_apps_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:     uimdiag_mmgsdi_session_open_ext_cmd_type

   DESCRIPTION:       Request params for session open command
   client_id_high:    Upper 32 bits of the client ID
   client_id_low:     Lower 32 bits of the client ID
   session_type_mask: Indicates the type of sessions (GW, 1X or non-prov App.)
   set_notify_mask:   Mask indicating whether clients should call
                      mmgsdi_session_prov_app_init_complete() when they
                      finish their initialization procedure for the
                      applications. This is only applicable to the
                      provisioning-session types.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
  uint32                  session_type_mask;
  uint32                  set_notify_mask;
}uimdiag_mmgsdi_session_open_ext_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_client_id_and_evt_reg_pyld_type

   DESCRIPTION:    Client id and event reg response parameters
   client_id_high: Upper value of the Client ID
   client_id_low:  Lower value of the Client ID
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
}uimdiag_mmgsdi_rsp_client_id_and_evt_reg_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_client_id_and_evt_dereg_pyld_type

   DESCRIPTION:    Client id and event de-reg response parameters
   client_id_high: Upper value of the Client ID
   client_id_low:  Lower value of the Client ID
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32                        client_id_high;
   uint32                        client_id_low;
}uimdiag_mmgsdi_rsp_client_id_and_evt_dereg_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_open_pyld_type

   DESCRIPTION:    Response parameters for Session open
   session_id_high:The higher value resulting session ID
   session_id_low: The lower value resulting session ID
   channel_id:     Channel ID
   app_type:       Application Type
   aid_length:     Application ID length
   aid_ptr:        Pointer to Application ID
   label_len:      Length of the label
   label_ptr:      Application Label
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  channel_id;
  uint32                  app_type;
  uint32                  aid_length;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  label_len;
  uint8                   label_ptr[MMGSDI_MAX_AID_LEN];
}uimdiag_mmgsdi_rsp_session_open_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_close_pyld_type

   DESCRIPTION:      Response parameters for Session close
   session_id_high:  Higher 32 bits of resulting session ID
   session_id_low:   Lower 32 bits of resulting session ID
   app_tpye:         Application type.
   aid_length:       Length of aid.
   aid_ptr:          Application Identifier with a length specified by AID_LEN.
   label_len:        Length of label.
   label_ptr:        Application Label with a length specified by LABEL_LEN
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_id_high;
  uint32                  session_id_low;
  uint32                  app_type;
  uint32                  aid_length;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  label_len;
  uint8                   label_ptr[MMGSDI_MAX_AID_LEN];
}uimdiag_mmgsdi_rsp_session_close_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_get_info_pyld_type

   DESCRIPTION:      Response parameters for Session Get Payload Info
   session_id_high:  Higher 32 bits of resulting session ID
   session_id_low:   Lower 32 bits of resulting session ID
   session_type:     Session Type, by which a client queries MMGSDI for
                     session info.
   slot_id:          Slot number.
   channel_id:       Channel id
   app_tpye:         Application type.
   aid_length:       Length of aid.
   aid_ptr:          Application Identifier with a length specified by AID_LEN.
   label_len:        Length of label.
   label_ptr:        Application Label with a length specified by LABEL_LEN
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  session_type;
  uint32                  slot_id;
  uint32                  channel_id;
  uint32                  app_type;
  uint32                  aid_length;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  label_len;
  uint8                   label_ptr[MMGSDI_MAX_AID_LEN];
}uimdiag_mmgsdi_rsp_session_get_info_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_get_all_pin_stat_plyd_type

   DESCRIPTION:      Response parameters for Get All Pin Status
   pin_id            pin_id
   pin_status        pin status
   pin_replacement   replacement status for the PIN
   pin_num_retries   PIN number of retries remaining
   puk_num_retries   PUK number of retries remaining
   pin2_id           pin_id for PIN2
   pin2_status       pin status for PIN2
   pin2_replacement  replacement status for the PIN 2
   pin2_num_retries  PIN 2 number of retries remaining
   puk2_num_retries  PUK 2 number of retries remaining
   upin_id           Pin_id for universal PIN
   upin_status       Universal pin status
   upin_replacement  replacement status for the UPIN
   upin_num_retries  Universal PIN number of retries remaining
   upuk_num_retries  Universal PUK number of retries remaining
   hiddenkey_id      Pin_id for Hiddenkey
   hiddenkey_status  Hiddenkey status
   dir_index         EF-DIR record number corresponding to the app
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  pin_id;
  uint32                  pin_status;
  uint32                  pin_replacement;
  uint32                  pin_num_retries;
  uint32                  puk_num_retries;
  uint32                  pin2_pin_id;
  uint32                  pin2_status;
  uint32                  pin2_replacement;
  uint32                  pin2_num_retries;
  uint32                  puk2_num_retries;
  uint32                  upin_id;
  uint32                  upin_status;
  uint32                  upin_replacement;
  uint32                  upin_num_retries;
  uint32                  upuk_num_retries;
  uint32                  hiddenkey_id;
  uint32                  hiddenkey_status;
  uint8                   dir_index;
}uimdiag_mmgsdi_rsp_session_get_all_pin_stat_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:             uimdiag_mmgsdi_rsp_session_pin_operation_pyld_type

   DESCRIPTION:           Response parameters for Disable Pin / Verify Pin /
                          Enable Pin
   pin_id:                pin_id
   pin_status:            pin status
   pin_replacement:       Replacement status for the PIN
   pin_num_retries:       PIN number of retries remaining
   puk_num_retries:       PUK number of retries remaining
   vld_pin_num_retries:   Valid PIN number of retries remaining
   vld_puk_num_retries:   Valid PUK number of retries remaining
   pin_op:                Operation on the PIN
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  pin_id;
  uint32                  pin_status;
  uint32                  pin_replacement;
  uint32                  pin_num_retries;
  uint32                  puk_num_retries;
  uint32                  vld_pin_num_retries;
  uint32                  vld_puk_num_retries;
  uint32                  pin_op;
}uimdiag_mmgsdi_rsp_session_pin_operation_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_get_file_attr_pyld_type

   DESCRIPTION:           Response parameters for get file attr
   access_method_type:    Type of access method used
   ef_enum:               file identifier
   df_enum:               directory identifier
   path_len:              length of the path to access
   path_ptr:              file path with length specified by path_len
   aid_len:               length of the application identifier
   aid_ptr:               application identifier
   file_size:             total size of the file
   file_id:               EF id of the file
   file_type:             type of file(transparent,linear fixed, cyclic..)
   read_cond:             access condition to read
   read_no_pin:           number of read_pin_info required to be verified
   read_pin_info:         PINs that need to be verified in order to allow read access
   write_cond:            access condition to read
   write_no_pin:          number of write_pin_info required to be verified
   write_pin_info:        PINs that need to be verified in order to allow write access
   inc_cond:              access condition to increase
   inc_no_pin:            number of inc_pin_info required to be verified
   inc_pin_info:          PINs that need to be verified in order to allow increase access
   inact_cond:            access condition to inactivate
   inact_no_pin:          number of inact_pin_info required to be verified
   inact_pin_info:        PINs that need to be verified in order to allow inactivate
   act_cond:              access condition to activate
   act_no_pin:            number of act_pin_info required to be verified
   act_pin_info:          PINs that need to be verified in order to allow activation
   file_invalid_flg:      file invalidation state flag
   rw_allowed_when_inv:   read/write is allowed or not when file is invalidated
   num_of_rec:            number of records
   rec_len:               length of the record
   cf_increase_allowed:   increase allowed or not for cyclic file
   data_len:              length of data
   data_ptr:              data
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  access_method_type;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint16                  path_ptr[MMGSDI_MAX_PATH_LEN];
  uint32                  aid_len;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  file_size;
  uint8                   file_id[MMGSDI_FILE_ID_SIZE];
  uint32                  file_type;
  uint32                  read_cond;
  uint32                  read_no_pin;
  uint8                   read_pin_info[UIMDIAG_MAX_PIN];
  uint32                  write_cond;
  uint32                  write_no_pin;
  uint8                   write_pin_info[UIMDIAG_MAX_PIN];
  uint32                  inc_cond;
  uint32                  inc_no_pin;
  uint8                   inc_pin_info[UIMDIAG_MAX_PIN];
  uint32                  inact_cond;
  uint32                  inact_no_pin;
  uint8                   inact_pin_info[UIMDIAG_MAX_PIN];
  uint32                  act_cond;
  uint32                  act_no_pin;
  uint8                   act_pin_info[UIMDIAG_MAX_PIN];
  uint32                  file_invalid_flg;
  uint32                  rw_allowed_when_inv;
  uint32                  num_of_rec;
  uint32                  rec_len;
  uint32                  cf_increase_allowed;
}uimdiag_mmgsdi_rsp_session_get_file_attr_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_read_record_pyld_type

   DESCRIPTION:    Response parameters for read
   access_method_type:    Type of access method used
   ef_enum:               file identifier
   df_enum:               directory identifier
   path_len:              length of the path to access
   path_ptr:              file path with length specified by path_len
   aid_len:               length of the application identifier
   aid_ptr:               application identifier
   rec_num:               record number where the read was performed
   offset:                offset location from where the read should start
   data_len:              length of the data
   data_ptr:              data read from the card
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  access_method_type;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint16                  path_ptr[MMGSDI_MAX_PATH_LEN];
  uint32                  aid_len;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  rec_num;
  uint32                  offset;
  uint32                  data_len;
  uint8                   data_ptr[MMGSDI_MAX_DATA_BLOCK_LEN];
}uimdiag_mmgsdi_rsp_session_read_record_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_write_record_pyld_type

   DESCRIPTION:    Response parameters for write
   access_method_type:    Type of access method used
   ef_enum:               file identifier
   df_enum:               directory identifier
   path_len:              length of the path to access
   path_ptr:              file path with length specified by path_len
   aid_len:               length of the application identifier
   aid_ptr:               application identifier
   rec_num:               record number where the write was performed
   offset:                offset location from where the write should start
   data_len:              length of the data written
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  access_method_type;
  uint32                  ef_enum;
  uint32                  df_enum;
  uint32                  path_len;
  uint16                  path_ptr[MMGSDI_MAX_PATH_LEN];
  uint32                  aid_len;
  uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32                  rec_num;
  uint32                  offset;
  uint32                  data_len;
}uimdiag_mmgsdi_rsp_session_write_record_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_perso_get_data_pyld_type

   DESCRIPTION:    Response parameters for perso get feature data
   perso_action:   Personalization action performed
   data_len:       Length of feature data
   data_ptr:       Feature data ptr
-------------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32                  perso_action;
  uint32                  data_len;
  uint8                   data_ptr[1];
}uimdiag_mmgsdi_rsp_session_perso_get_data_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_perso_pyld_type

   DESCRIPTION:    Response parameters for perso activate / deactivate/ unblock
   perso_action:   Personalization action performed
   perso_features: Personalization feature operated on
   num_retries:    Number of Retries left
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  perso_action;
  uint32                  perso_feature;
  uint32                  num_retries;
}uimdiag_mmgsdi_rsp_session_perso_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_perso_get_ind_pyld_type

   DESCRIPTION:    Response parameters to retrieve the information about
                   personalization Indicators

   nw_ind_status:  Network Indicator Status
   ns_ind_status:  NW Subset Indicator Status
   sp_ind_status:  Servic Provider Ind Status
   cp_ind_status:  Corporate Ind Status
   sim_ind_status: SIM Indicator Status
   ruim_nw1_ind:   RUIM Network Indicator Status
   ruim_nw2_ind:   RUIM Network Indicator Status
   ruim_hrpd_ind:  RUIM NW Subset Indicator Status
   ruim_sp_ind:    RUIM Service Provider Ind Status
   ruim_cp_ind:    RUIM Corporate Ind Status
   ruim_ruim_ind:  RUIM Indicator Status
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32          nw_ind_status;
  uint32          ns_ind_status;
  uint32          sp_ind_status;
  uint32          cp_ind_status;
  uint32          sim_ind_status;
  uint32          ruim_nw1_ind;
  uint32          ruim_nw2_ind;
  uint32          ruim_hrpd_ind;
  uint32          ruim_sp_ind;
  uint32          ruim_cp_ind;
  uint32          ruim_ruim_ind;
}uimdiag_mmgsdi_rsp_session_perso_get_ind_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type

   DESCRIPTION:    Response parameters to retrieve the information about
                   Perso feature indicator key

   feature:        Network Indicator Status
   key_data_len:   key length
   key_data_ptr:   key data
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  feature;
  uint32                  imei_length;
  uint32                  key_length;
  uint8                   imei_data_ptr[UIMDIAG_MAX_IMEI_LENGTH];
  uint8                   key_data_ptr[UIMDIAG_MAX_PERSO_KEY_LENGTH];
}uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:         uimdiag_mmgsdi_rsp_session_perso_get_dck_retries_pyld_type

   DESCRIPTION:           Used to build payload to get the DCK number of retries
   unblock_retries:       Indicates if the retries retrieved are retries/unblock
                          retries left.
   nw_num_retries:        Number of network retries.
   ns_num_retries:        Number of network subset retries.
   sp_num_retries:        Number of service provider retries.
   cp_num_retries:        Number of corporate retries.
   sim_num_retries:       Number of SIM user retries.
   ruim_nw1_num_retries:  Number of RUIM network 1 retries.
   ruim_nw2_num_retries:  Number of RUIM network 2 retries.
   ruim_hrpd_num_retries: Number of RUIM High Rate Packet Data retries.
   ruim_sp_num_retries:   Number of RUIM service provider retries.
   ruim_cp_num_retries:   Number of RUIM corporate retries.
   ruim_ruim_num_retries: Number of RUIM RUIM user retries.
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean           unblock_retries;  /**<Indicates if retries retrieved are
                                       retries/unblock retries left.. */
  uint32            nw_num_retries;   /**< Number of network retries. */
  uint32            ns_num_retries;   /**< Number of network subset retries.  */
  uint32            sp_num_retries;   /**< Number of service provider retries. */
  uint32            cp_num_retries;   /**< Number of corporate retries. */
  uint32            sim_num_retries;  /**< Number of SIM user retries. */
  uint32            ruim_nw1_num_retries;  /**< Number of RUIM network 1 retries. */
  uint32            ruim_nw2_num_retries;  /**< Number of RUIM network 2 retries. */
  uint32            ruim_hrpd_num_retries; /**< Number of RUIM High Rate Packet Data retries. */
  uint32            ruim_sp_num_retries;   /**< Number of RUIM service provider retries. */
  uint32            ruim_cp_num_retries;   /**< Number of RUIM corporate retries. */
  uint32            ruim_ruim_num_retries;  /**< Number of RUIM RUIM user retries. */
}uimdiag_mmgsdi_rsp_session_perso_get_dck_retries_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:  uimdiag_mmgsdi_rsp_session_perso_lock_dwn_enable_ltd_access_pyld_type

   DESCRIPTION:Confirmation/response for the GSDI_PERSO_LOCK_DOWN_REQ
               request/command. It returns the action that is locked down
   locked_cmd :Action that is being locked down.
  ------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              action_locked;
}uimdiag_mmgsdi_rsp_session_perso_lock_dwn_enable_ltd_access_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_deact_prov_pyld_type

   DESCRIPTION:      Response parameters for deactivate provisioning
   session_id_high:  Higher 32 bits of resulting session ID
   session_id_low:   Lower 32 bits of resulting session ID
   app_type:         Application Type
   aid_length:       Application ID length
   aid_ptr:          Pointer to Application ID
   label_len:        Length of the label
   label_ptr:        Application Label
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32                  session_id_high;
   uint32                  session_id_low;
   uint32                  app_type;
   uint32                  aid_length;
   uint8                   aid_ptr[MMGSDI_MAX_AID_LEN];
   uint32                  label_len;
   uint8                   label_ptr[MMGSDI_MAX_AID_LEN];
}uimdiag_mmgsdi_rsp_session_deact_prov_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_jcdma_get_card_info_pyld_type

   DESCRIPTION:      Response parameters for session jcdma get card info
   card_info:             JCDMA card information for UIM power up
   me_esn_change:   Indicates whether ME ESN changed
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  card_info;
  uint32                  me_esn_change;
}uimdiag_mmgsdi_rsp_session_jcdma_get_card_info_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_rsp_session_get_app_cap_pyld_type

   DESCRIPTION:                 Response parameters for
                                UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD
                                and
                                UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD
   session_id_high:             Higher 32 bits of resulting session ID
   session_id_low:              Lower 32 bits of resulting session ID
   fdn_enabled:                 FDN enabled for this app.
   bdn_enabled:                 BDN enabled for this app
   acl_enabled:                 ACL enabled for this app
   imsi_invalidated:            IMSI invalidated
   gbl_pb_present:              Global phonebook is present
   gbl_pb_multiple_pbr_entries: Global phonebook, number of PBRs TAGs identified
   lcl_pb_present:              Local phonebook is present
   lcl_pb_multiple_pbr_entries: Local phonebook, number of PBRs TAGs identified
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   boolean                 fdn_enabled;
   boolean                 bdn_enabled;
   boolean                 acl_enabled;
   boolean                 imsi_invalidated;
}uimdiag_mmgsdi_rsp_session_get_app_cap_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_rsp_session_get_cphs_info_pyld_type

   DESCRIPTION:                 Response parameters used when returning the
                                repsonse from the MMGSDI Task on the request to
                                get cphs information.

   session_id_high:                     Higher 32 bits of resulting session ID
   session_id_low:                      Lower 32 bits of resulting session ID
   cphs_capable:                        CPHS capability information
   phase:                               CPHS Phase
   csp:                                 Customer Service Profile
   sst:                                 Phase 1 String Service Table
   mbn:                                 Mailbox Numbers
   ops:                                 Operator Name Short Form
   ifn:                                 Information Numbers
   call_offering_CFU:                   CPHS Customer Service Profile status
   call_offering_CFB:                   CPHS Customer Service Profile status
   call_offering_CFNRy:                 CPHS Customer Service Profile status
   call_offering_CFNRc:                 CPHS Customer Service Profile status
   call_offering_CT:                    CPHS Customer Service Profile status
   call_restrict_BOAC:                  CPHS Customer Service Profile status
   call_restrict_BOIC:                  CPHS Customer Service Profile status
   call_restrict_BOIC_exHC:             CPHS Customer Service Profile status
   call_restrict_BAIC:                  CPHS Customer Service Profile status
   call_restrict_BIC_Roam:              CPHS Customer Service Profile status
   other_supps_MPTY:                    CPHS Customer Service Profile status
   other_supps_CUG:                     CPHS Customer Service Profile status
   other_supps_AoC:                     CPHS Customer Service Profile status
   other_supps_Pref_CUG:                CPHS Customer Service Profile status
   other_supps_CUG_OA:                  CPHS Customer Service Profile status
   call_completion_HOLD:                CPHS Customer Service Profile status
   call_completion_CW:                  CPHS Customer Service Profile status
   call_completion_CCBS:                CPHS Customer Service Profile status
   call_completion_User_Signalling:     CPHS Customer Service Profile status
   teleservices_SM_MT:                  CPHS Customer Service Profile status
   teleservices_SM_MO:                  CPHS Customer Service Profile status
   teleservices_SM_CB:                  CPHS Customer Service Profile status
   teleservices_Reply_Path:             CPHS Customer Service Profile status
   teleservices_Del_Conf:               CPHS Customer Service Profile status
   teleservices_Protocol_ID:            CPHS Customer Service Profile status
   teleservices_Validity_Period:        CPHS Customer Service Profile status
   cphs_teleservices_als:               CPHS Customer Service Profile status
   cphs_features_SST:                   CPHS Customer Service Profile status
   number_identify_CLIP:                CPHS Customer Service Profile status
   number_identify_CoLR:                CPHS Customer Service Profile status
   number_identify_CoLP:                CPHS Customer Service Profile status
   number_identify_MCI:                 CPHS Customer Service Profile status
   number_identify_CLI_send:            CPHS Customer Service Profile status
   number_identify_CLI_block:           CPHS Customer Service Profile status
   p2plus_services_GPRS:                CPHS Customer Service Profile status
   p2plus_services_HSCSD:               CPHS Customer Service Profile status
   p2plus_services_Voice_Group_call:    CPHS Customer Service Profile status
   p2plus_services_Voice_Broadcast_Svc: CPHS Customer Service Profile status
   p2plus_services_Muli_Sub_Profile:    CPHS Customer Service Profile status
   p2plus_services_Multiband:           CPHS Customer Service Profile status
   value_added_PLMN_MODE:               CPHS Customer Service Profile status
   value_added_VPS:                     CPHS Customer Service Profile status
   value_added_SM_MO_PAGING:            CPHS Customer Service Profile status
   value_added_SM_MO_EMAIL:             CPHS Customer Service Profile status
   value_added_FAX:                     CPHS Customer Service Profile status
   value_added_DATA:                    CPHS Customer Service Profile status
   valued_added_Language:               CPHS Customer Service Profile status
   information_numbers:                 CPHS Customer Service Profile status
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32      session_id_high;
   uint32      session_id_low;
   boolean     cphs_capable;
   uint8       phase;
   uint32      csp;
   uint32      sst;
   uint32      mbn;
   uint32      ops;
   uint32      ifn;
   uint32      call_offering_CFU;
   uint32      call_offering_CFB;
   uint32      call_offering_CFNRy;
   uint32      call_offering_CFNRc;
   uint32      call_offering_CT;
   uint32      call_restrict_BOAC;
   uint32      call_restrict_BOIC;
   uint32      call_restrict_BOIC_exHC;
   uint32      call_restrict_BAIC;
   uint32      call_restrict_BIC_Roam;
   uint32      other_supps_MPTY;
   uint32      other_supps_CUG;
   uint32      other_supps_AoC;
   uint32      other_supps_Pref_CUG;
   uint32      other_supps_CUG_OA;
   uint32      call_completion_HOLD;
   uint32      call_completion_CW;
   uint32      call_completion_CCBS;
   uint32      call_completion_User_Signalling;
   uint32      teleservices_SM_MT;
   uint32      teleservices_SM_MO;
   uint32      teleservices_SM_CB;
   uint32      teleservices_Reply_Path;
   uint32      teleservices_Del_Conf;
   uint32      teleservices_Protocol_ID;
   uint32      teleservices_Validity_Period;
   uint32      cphs_teleservices_als;
   uint32      cphs_features_SST;
   uint32      number_identify_CLIP;
   uint32      number_identify_CoLR;
   uint32      number_identify_CoLP;
   uint32      number_identify_MCI;
   uint32      number_identify_CLI_send;
   uint32      number_identify_CLI_block;
   uint32      p2plus_services_GPRS;
   uint32      p2plus_services_HSCSD;
   uint32      p2plus_services_Voice_Group_call;
   uint32      p2plus_services_Voice_Broadcast_Svc;
   uint32      p2plus_services_Muli_Sub_Profile;
   uint32      p2plus_services_Multiband;
   uint32      value_added_PLMN_MODE;
   uint32      value_added_VPS;
   uint32      value_added_SM_MO_PAGING;
   uint32      value_added_SM_MO_EMAIL;
   uint32      value_added_FAX;
   uint32      value_added_DATA;
   uint32      valued_added_Language;
   uint32      information_numbers;
}uimdiag_mmgsdi_rsp_session_get_cphs_info_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_isim_auth_pyld_type

   DESCRIPTION:      Response parameters for Network" Initiated Authentication
   session_id_high:  Higher 32 bits of resulting session ID
   session_id_low:   Lower 32 bits of resulting session ID
   res_length:       Authentication response length
   res_ptr:          Pointer to Authentication response
   ck_len:           Length of Authentication response
   ck_ptr:           Pointer to Authentication response
   ik_length:        Authentication response length
   ik_ptr:           Pointer to Authentication response
   auts_len:         Length of Authentication response
   auts_ptr:         Authentication response pointer
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32                  session_id_high;
   uint32                  session_id_low;
   uint32                  res_length;
   uint8                   res_ptr[UIMDIAG_ISIM_DATA_LEN];
   uint32                  ck_len;
   uint8                   ck_ptr[UIMDIAG_ISIM_DATA_LEN];
   uint32                  ik_len;
   uint8                   ik_ptr[UIMDIAG_ISIM_DATA_LEN];
   uint32                  auts_len;
   uint8                   auts_ptr[UIMDIAG_ISIM_DATA_LEN];
}uimdiag_mmgsdi_rsp_session_isim_auth_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_send_crd_status_pyld_type

   DESCRIPTION:      The data to be read/written from/to the card.

-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32                  status_data_len;
   uint8                   status_data[1];
}uimdiag_mmgsdi_rsp_session_send_crd_status_pyld_type;

/*=============================================================================
  STRUCTURE:   uimdiag_path_type

  int32              path_len;
  uint16             path_buf;

  PURPOSE To hold the path info for a file.
=============================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_len_type    path_len;                        /* Length of path */
  uint16             path_buf[MMGSDI_MAX_PATH_LEN];   /* path */
} uimdiag_mmgsdi_path_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: UIMDIAG_MMGSDI_PKCS15_LOOKUP_TABLE_ENTRIES
   DESCRIPTION:
   The mmgsdi_pkcs15_lookup_table consists of two categories
   1. EF Enum – List of PKCS15 EFs whose path is extracted from EF-ODF
   2. File Path Info – File path info of the PKCS15 EF
   This is a duplicte PACKED structure declaration similar to the one
   in mmgsdi_common.h
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type      pkcs15_file_enum;
  uimdiag_mmgsdi_path_type   pkcs15_file_path;
} uimdiag_mmgsdi_pkcs15_lookup_table_entries;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_create_pkcs15_lkp_tbl_pyld_type

   DESCRIPTION:      This structure is used when returning a create PKCS15 Table
                     response from the MMGSDI Task.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint8 no_of_pkcs15_lookup_table_entries;
   uimdiag_mmgsdi_pkcs15_lookup_table_entries
        pkcs15_lookup_table_pyld[MMGSDI_MAX_PKCS15_TABLE_ENTRIES];
}uimdiag_mmgsdi_rsp_session_create_pkcs15_lkp_tbl_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_srv_pyld_type

   DESCRIPTION:      Response parameters for Service Disable/ Enable/ Is service
                     Available (sync API)
   service_type:     Type of service
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32            service_type;
}uimdiag_mmgsdi_rsp_session_srv_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_is_srv_avail_pyld_type

   DESCRIPTION:      Response parameters for is service available
   srv_available:    Is service available
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint32            srv_available;
}uimdiag_mmgsdi_rsp_session_is_srv_avail_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_search_seek_pyld_type

   DESCRIPTION:      Response parameters for mmgsdi session seek and mmgsdi
                     session search

   access_method:            Access_method
   file_enum:                File Enum
   df_enum:                  DF Enum
   path_type_len:            Path Type Length
   path_type_ptr:            Path Type Data
   app_id_len:               App Id Length
   app_id_ptr:               App ID Data
   searched_record_nums_len: Searched Record Numbers
   searched_record_nums_ptr: Searched Record Pointer
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              access_method;
  uint32              file_enum;
  uint32              df_enum;
  uint32              path_type_len;
  uint16              path_type_ptr[MMGSDI_MAX_PATH_LEN];
  uint32              app_id_len;
  uint8               app_id_ptr[MMGSDI_MAX_AID_LEN];
  uint32              searched_record_nums_len;
  uint8               searched_record_nums_ptr[UIMDIAG_MAX_DATA_BLOCK_LEN];
}uimdiag_mmgsdi_rsp_session_search_seek_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_compute_ip_auth_rsp_type

   DESCRIPTION:                response for mmgsdi_session_compute_ip_auth_cmd
   uint8 cmpt_ip_rsp_data:  response data
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                  cmpt_ip_rsp_data_len;
    uint8                   cmpt_ip_rsp_data[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
}uimdiag_mmgsdi_session_compute_ip_auth_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_confirm_ssd_rsp_type

   DESCRIPTION:                response for uimdiag_mmgsdi_session_confirm_ssd_cmd_type
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                  ssd_update_ok; /* boolean */
}uimdiag_mmgsdi_session_confirm_ssd_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_bs_chal_rsp_type

   DESCRIPTION:                response for uimdiag_mmgsdi_session_bs_chal_cmd_type
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                  randbs;          /**< 36-bit random number. */
}uimdiag_mmgsdi_session_bs_chal_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_read_prl_rsp_type

   DESCRIPTION:                response for uimdiag_mmgsdi_session_read_prl_cmd_type
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                           prl_version;     /**< IS-683-A or IS-683-C PRL. */
    uint32                           size;            /**< Size of PRL. */
    uint32                           valid;           /**< PRL valid or invalid. */
    uint8                            roaming_list[UIMDIAG_MAX_ROAMING_LIST_LENGTH];
                                                      /**< PR List. */
}uimdiag_mmgsdi_session_read_prl_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_cache_file_size_rsp_type

   DESCRIPTION:                response for UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                  file_size;          /**< 36-bit random number. */
}uimdiag_mmgsdi_session_cache_file_size_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:                   uimdiag_mmgsdi_session_cache_file_size_rsp_type

   DESCRIPTION:                response for UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
    uint32                 read_data_len;
    uint8                  read_data[UIMDIAG_MAX_READ_DATA_LEN];
}uimdiag_mmgsdi_session_read_cache_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_get_atr_rsp_type

   DESCRIPTION:      Response for UIMDIAG_MMGSDI_SAP_GET_ATR_CMD

   atr_data_len:    Length of the ATR
   client_id_low:   ATR data

-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                    atr_data_len;
  uint8                     atr_data[UIM_MAX_ATR_CHARS];
}uimdiag_mmgsdi_sap_get_atr_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_send_apdu_rsp_type

   DESCRIPTION:      Response for UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD

   max_size:                Maximum size of the data
   channel_id:              Logical channel ID
   implicit_get_rsp_sw1:    Implicit Get Response
                            Status Word 1
   implicit_get_rsp_sw2:    Implicit Get Response
                            Status Word 2
   apdu_data_len:           Length of APDU data
   apdu_data_ptr:           APDU Data
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                    max_size;
  uint32                    channel_id;
  uint16                    implicit_get_rsp_sw1;
  uint16                    implicit_get_rsp_sw2;
  uint32                    apdu_data_len;
  uint8                     apdu_data[UIMDIAG_MAX_DATA_BLOCK_LEN];
}uimdiag_mmgsdi_sap_send_apdu_rsp_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_disconnect_rsp_type

   DESCRIPTION:      Response for UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD

   client_id_high: Upper value of the Client ID
   client_id_low:  Lower value of the Client ID
   sap_mode:       Mode of disconnection

-----------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32                  client_id_high;
  uint32                  client_id_low;
  uint32                  sap_mode;
}uimdiag_mmgsdi_sap_disconnect_rsp_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_generic_rsp_type

   DESCRIPTION:      Response for UIMDIAG_MMGSDI_SAP_POWER_ON_CMD
                                  UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD
                                  UIMDIAG_MMGSDI_SAP_RESET_CMD

   client_id_high: Upper value of the Client ID
   client_id_low:  Lower value of the Client ID

-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  client_id_high;
  uint32                  client_id_low;
}uimdiag_mmgsdi_sap_generic_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_sap_card_reader_status_rsp_type

   DESCRIPTION:      Response for UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD

   data_len:          Length of Card reader status data
   data_ptr:          Card reader status data in the
                      SAP context
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{

  uint32                  data_len;
  uint8                   *data_ptr;
}uimdiag_mmgsdi_sap_card_reader_status_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_rsp_session_get_operator_name_type

   DESCRIPTION:      Response for session get operator name command

   uint8    encoding_info;
   uint32   operator_name_len;
   uint8    operator_name[1];
   uint8    lang_ind;
   uint8    spn_display_bit;
   uint8    rplmn_display_bit;
   uint8    roaming_status;
   uint32   no_plmn_id;
   uint32   total_buff_size_of_operator_name_info;

   Dynamic part encoding
   uint8    plmn_id[3]
   uint8    rat_type
   uint8    long_name_ci;
   uint8    long_name_spare_bytes;
   uint8    long_name_encoding;
   uint32   long name len
   uint8    *long name
   uint8    short_name_ci;
   uint8    short_name_spare_bytes;
   uint8    short_name_encoding;
   uint32   short name len
   uint8    *short name
   -----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   uint8    encoding_info;
   uint32   operator_name_len;
   uint8    operator_name[1]; /* dynamically alllocated data based on operator name len*/
   uint8    lang_ind;
   uint8    spn_display_bit;
   uint8    rplmn_display_bit;
   uint8    roaming_status;
   uint32   no_plmn_id;
   uint32   total_buff_size_of_operator_name_info;
}uimdiag_mmgsdi_rsp_session_get_operator_name_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:              uimdiag_mmgsdi_card_pdown_rsp_type

   DESCRIPTION:            To hold the payload info for power down
   power_down_option:      Power down option
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                    power_down_option;
}uimdiag_mmgsdi_card_pdown_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:              uimdiag_mmgsdi_card_pup_rsp_type

   DESCRIPTION:            To hold the payload info for power up
   power_up_option:        Power up option
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                    power_up_option;
}uimdiag_mmgsdi_card_pup_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:              uimdiag_mmgsdi_activate_onchip_sim_rsp_type

   DESCRIPTION:            To hold the payload info for sim data state
   state:                  State
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                   onchip_state;
}uimdiag_mmgsdi_activate_onchip_sim_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:              uimdiag_mmgsdi_send_apdu_rsp_type

   DESCRIPTION:
   max_size:               max size
   channel_id:             Channel ID
   implicit_sw1:           Status Word 1
   implicit_sw2:           Status word 2
   apdu_data_length:       APDU buffer length
   apdu_data_ptr:          APDU buffer
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32              max_size;
  uint32              channel_id;
  uint16              implicit_sw1;
  uint16              implicit_sw2;
  uint32              apdu_data_length;
  uint8               apdu_data_ptr[UIMDIAG_MAX_DATA_BLOCK_LEN];
}uimdiag_mmgsdi_send_apdu_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_app_info_type

   DESCRIPTION:      App info used in get all available apps handling

-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32        cached_status;
  uint32        slot_id;
  uint32        pin1_id;
  uint32        pin1_status;
  uint32        pin1_replacement_status;
  uint32        pin1_num_of_retries;
  uint32        pin1_num_of_unblock_retires;
  uint32        pin1_num_of_valid_reties;
  uint32        pin1_num_of_valid_unblock_retries;
  uint32        pin2_id;
  uint32        pin2_status;
  uint32        pin2_replacement_status;
  uint32        pin2_num_of_retries;
  uint32        pin2_num_of_unblock_retires;
  uint32        pin2_num_of_valid_reties;
  uint32        pin2_num_of_valid_unblock_retries;
  uint32        upin_id;
  uint32        upin_status;
  uint32        upin_replacement_status;
  uint32        upin_num_of_retries;
  uint32        upin_num_of_unblock_retires;
  uint32        upin_num_of_valid_reties;
  uint32        upin_num_of_valid_unblock_retries;
  uint32        app_type;
  uint32        aid_len;
  uint8          app_id_ptr[MMGSDI_MAX_AID_LEN];
  uint32        label_len;
  uint8          label_ptr[MMGSDI_MAX_AID_LEN];
  uint32        hkey_id;
  uint32        hkey_status;
  uint32        hkey_replacement_status;
  uint32        hkey_num_of_retries;
  uint32        hkey_num_of_unblock_retires;
  uint32        hkey_num_of_valid_reties;
  uint32        hkey_num_of_valid_unblock_retries;
  uint32        dir_index;
}uimdiag_mmgsdi_app_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:        uimdiag_mmgsdi_get_all_available_apps_rsp_type

   DESCRIPTION:      Response for get all available apps sync command

-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                                      total_no_of_apps;
  uimdiag_mmgsdi_app_info_type get_all_available_app_info[1];
}uimdiag_mmgsdi_get_all_available_apps_rsp_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_session_open_info_type

   DESCRIPTION:    Response parameters for Session open ext
   session_id_high:The higher value resulting session ID
   session_id_low: The lower value resulting session ID
   channel_id:     Channel ID
   app_type:       Application Type
   aid_length:     Application ID length
   aid_ptr:        Pointer to Application ID
   label_len:      Length of the label
   label_ptr:      Application Label
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32        session_id_high; /**< Session ID returned to indicate
                                      which session has been selected. */
  uint32        session_id_low;
  uint32        session_type; /**< Session type corresponding to
                                   Session ID */
  uint32        channel_id; /**< Channel ID. */
  uint32        slot_id; /**< Slot ID corresponding to the
                              session (valid only if an
                              application is linked to the
                              session). */
  uint32        app_type;
  uint32        aid_length;
  uint8         aid_ptr[MMGSDI_MAX_AID_LEN];
  uint32        label_len;
  uint8         label_ptr[MMGSDI_MAX_AID_LEN]; /**< Application details
                                                    (application type, ID, and
                                                    label); valid only if an
                                                    application is linked to
                                                    the session. */
} uimdiag_mmgsdi_session_open_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_mmgsdi_rsp_session_open_ext_pyld_type

   DESCRIPTION:    Response parameters for Session open
   num_sessions:   Number of sessions opened
   session_info:   Holds session info for all opened sessions
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                                  num_sessions;
  uimdiag_mmgsdi_session_open_info_type  session_info[1];
}uimdiag_mmgsdi_rsp_session_open_ext_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:           uimdiag_mmgsdi_session_get_nvruim_data_sync_rsp_type

   DESCRIPTION:         Response for UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                  nvruim_data_req_enum;
  uint8                   nvruim_data_sync_rsp[UIMDIAG_MAX_NVRUIM_DATA_SYNC];
}uimdiag_mmgsdi_session_get_nvruim_data_sync_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:              uimdiag_mmgsdi_event_payload_type

   DESCRIPTION:            The structure for the event payload of MMGSDI
   uint8  evt:             Type of event occured
   uint8 event_payload:    Event payload (16 byte limitation due to DIAG)
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                evt;
  uint8                event_payload[EVENT_MAX_PAYLOAD];
}uimdiag_mmgsdi_event_payload_type;


/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_CMD

DESCRIPTION
  Command handler for session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_cmd (
  PACKED void *req_ptr,
  word        pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CLOSE_CMD

DESCRIPTION
  Command handler for session close subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_close_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD

DESCRIPTION
  Command handler for client id and evt reg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_reg_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD

DESCRIPTION
  Command handler for client id and evt dereg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_dereg_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD

DESCRIPTION
  Command handler that alklows the client to establish an application:
  SIM/USIM/RUIM/CSIM as the provisioning application on a session which
  it has opened. It may also call this Command API to change the current
  provisioning application

DEPENDENCIES
  None.

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_act_or_sw_prov_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD

DESCRIPTION
  This command function allows a client to query MMGSDI for session information
  by Session ID or Session Type. A client can get Session Type, Slot Number,
  and AID, if querying by Session ID; or Session ID, Slot Number and AID,
  if querying by Session Type.

DEPENDENCIES
  If a client queries by Session ID, a valid Session ID is required, which can
  be achieved by calling mmgsdi_session_open()

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD

DESCRIPTION
  This function will build a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the session ID provided.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_all_pin_status_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD

DESCRIPTION
  This function will be used to build a request to perform a PIN verification
  procedure against a card for PIN1, PIN2 or UPIN.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_verify_pin_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD

DESCRIPTION
  This function will build a request to MMGSDI to disable the PIN provided.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_pin_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD

DESCRIPTION
  This function will build a request to MMGSDI to enable the PIN provided.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_pin_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_MD

DESCRIPTION
  This function will build a request to MMGSDI to change the PIN provided.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_change_pin_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD

DESCRIPTION
  This function will build a request to MMGSDI to unblock the PIN provided.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_unblock_pin_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD

DESCRIPTION
  This function will build a request to MMGSDI to get file attr.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_file_attr_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD

DESCRIPTION
  This function will build a request to MMGSDI to read record.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_record_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD

DESCRIPTION
  This function will build a request to MMGSDI to read transparent file.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_transparent_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD

DESCRIPTION
  This function will build a request to MMGSDI to write record.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_record_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD

DESCRIPTION
  This function will build a request to MMGSDI to write to transparent file.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_transparent_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD

DESCRIPTION
  This function will build a request to MMGSDI to Register for Refresh.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_PROCEED_VOTING_CMD

DESCRIPTION
  This function will build a request to MMGSDI to Register for Refresh.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD

DESCRIPTION
  This function will build a request to Simulate OK to refresh to MMGSDI.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_refresh_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD

DESCRIPTION
  This function will build a request to Simulate OK to proceed with refresh to MMGSDI.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD

DESCRIPTION
  This function will build a request to to indicate to MMGSDI that the client
  has finished its refresh process

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_refresh_complete_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD

DESCRIPTION
  This function will build a request to MMGSDI to Deregister for Refresh.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_dereg_for_refresh_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD

DESCRIPTION
  This function will build a request to MMGSDI to activate a personalization
  feature by an external source.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_activate_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD

DESCRIPTION
  This function will build a request to MMGSDI to deactivate a personalization
  feature by an external source.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_deactivate_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_IND_CMD

DESCRIPTION
  This function will build a request to retrieve the information about
  personalization Indicators

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_ind_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD

DESCRIPTION
  This function will build a request to set the personalization data for a
  particular personalization feature

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_set_data_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD

DESCRIPTION
  This function will build a request to get the personalization data for a
  particular personalization feature

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_data_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD

DESCRIPTION
  This function will build a request to get the personalization key

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_key_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD

DESCRIPTION
  This function will build a request to MMGSDI to unblock a personalization
  feature by an external source.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_unblock_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD

DESCRIPTION
  This function will build a request to get the DCK number of Retries.

DEPENDENCIES
  Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_dck_retries_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD

DESCRIPTION
  This function will build a request to lock down some of the perso commands.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD

DESCRIPTION
  This function will build a request to allow the client to deactivate a application

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_deact_prov_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD

DESCRIPTION
  This function will build a request to MMGSDI for JCDMA get card info

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_jcdma_get_card_info_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD

DESCRIPTION
  This function will build a request to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_cap_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD

DESCRIPTION
  This function will build a request to extract the CPHS related information
  which is already populated in MMGSDI.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_cphs_info_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD

DESCRIPTION
  This function will build a request to perform a "Network" Initiated
  Authentication with the ISIM Application.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_isim_auth_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD

DESCRIPTION
  This function will build a request to get the information concerning
  the current directory.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_send_card_status_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD

DESCRIPTION
  This function will build a request to disable service

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_service_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD

DESCRIPTION
  This function will build a request to Enable service

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_service_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD

DESCRIPTION
  This function will build a request to check if a particular service is
  available.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_is_service_avail_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEEK_CMD

DESCRIPTION
  This function will build a request that performs GSM Seek command on card

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_seek_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEARCH_CMD

DESCRIPTION
  This function will build a request that performs Search command on UICC card.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_search_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD

DESCRIPTION
  Notifies the MMGSDI that the client has finished its initialization procedure
  for the application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_prov_app_init_complete_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD

DESCRIPTION
  compute IP  authentication for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD CHAP
  Authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_compute_ip_auth_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD

DESCRIPTION
  Used to initiate the calculation of a new SSD value
                    for authentication purposes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ssd_update_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CONFIRM_SSD_CMD

DESCRIPTION
  Performs a Confirm SSD command for authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_confirm_ssd_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD

DESCRIPTION
  Used to perform an OTASP base station challenge before
  updating its SSD to ensure that the update process is being initiated by a
  legitimate source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_bs_chal_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD

DESCRIPTION
  Gets the IS-683-C or IS-683-A PRL from either the EF Extended
  PRL (EF-EPRL) or the EF-PRL when the EF-EPRL does not contain a valid PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_prl_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
  Command handler to determine if service is available or not.

DEPENDENCIES
  None.

RETURN VALUE
# if value is 1 , then uimdiag is supported, if value is 2, GSDIDIAG is supported.
# Preference and by default behaviour is UIMDIAG support.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_check_diag_support_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI for session information by
Session ID or Session Type. A client can get Session Type, Slot Number,
Channel ID and AID using this request.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD

DESCRIPTION
  Command handler to get the size of the file to be cached for a read operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_file_size_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD

DESCRIPTION
  Command handler to read the cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD

DESCRIPTION
  Command handler to write data to a cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM that the subscription on the card is illegal and is to be blocked.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_illegal_subscription_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM when the modem is in or out of the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD

DESCRIPTION
  Command handler to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_GET_ATR_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP GET ATR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_get_atr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP SEND
  APDU

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_POWER_ON_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP POWER
  ON.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_on_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP POWER
  OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_off_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP CARD
  READER STATUS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_card_reader_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_RESET_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP RESET.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_reset_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_NAA_REFRESH_CMD

DESCRIPTION
  This function will build a request to MMGSDI to Trigger Refresh.

DEPENDENCIES
   Valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_naa_refresh_cmd (
  PACKED void *req_ptr,
  word         pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_CARD_PDOWN_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a POWER
  DOWN for the SIM/USIM/RUIM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pdown_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_CARD_PUP_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a POWER
  UP for the SIM/USIM/RUIM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pup_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a ACTIVATE
  ONCHIP for a SIM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_sim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a ACTIVATE
  ONCHIP for a USIM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_usim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SEND_APDU_CMD

DESCRIPTION
  Command handler to extract

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP
  DISCONNECT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_disconnect_cmd(
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_SAP_CONNECT_CMD

DESCRIPTION
  Command handler to extract all information in order to perform a SAP
  CONNECT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_connect_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
) ;

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_TABLE_CMD

DESCRIPTION
  Command handler for getting operator Name(s)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_operator_name_table_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC_CMD

DESCRIPTION
  Command handler for get all available apps sync command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION   UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD

DESCRIPTION
  Command handler to extract all information in order to perform is service
  available (sync command)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_is_service_available_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD

DESCRIPTION
  Command handler for session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_ext_cmd (
  PACKED void * req_ptr,
  word          pkt_len
);

/*===========================================================================
FUNCTION uimdiag_mmgsdi_get_all_available_apps_async_cmd

DESCRIPTION
  Command handler for get all available apps async

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_async_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI to get nvruim state information and part of
nvruim's global cache. This is a synchronous call.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
);

#endif /* UIMDIAG_MMGSDI_H */
