#ifndef MMGSDI_UICC_H
#define MMGSDI_UICC_H
/*===========================================================================


           M M G S D I   U I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uicc.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
08/11/14   bcho    Optimize srv req by using cached srv table in client context
01/21/14   yt      Remove duplicate code
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
04/16/13   tl      Clean up of radio access features from MMGSDI code
02/07/13   yt      Support for activation/deactivation of EFs
09/05/12   av      Handling of VPE response from the card
03/01/12   bcho    Function prototype comment block updated
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
05/17/11   yt      Added support for INCREASE request
06/10/10   shr     If PIN status is already cached, do not send PIN commands
                   to the card
04/27/10   shr     Added Hiddenkey support
12/18/09   nb      Moved Dual Slot Changes
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
07/08/09   shr     Fixed Lint warnings
05/04/09   mib     Fixed typo in mmgsdi_uicc_activate_onchip_usim name
03/26/09   mib     Integration of support for ONCHIP USIM
09/06/08   kk      Changed pkcs15 command handling
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     BDN support
11/03/05   tml     Fixed header
02/24/05   sk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/

/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_READ

  DESCRIPTION:
    This function will provide read access to any file using the UICC
    protocol

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_read should be used

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

===========================================================================--*/
mmgsdi_return_enum_type  mmgsdi_uicc_read (
  mmgsdi_read_req_type      * req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_UICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to ETSI TS 102.221

    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    The information is provided asynchronously in the response cnf.

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_status (
  const mmgsdi_status_req_type      * req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_UICC_PIN_STATUS

  DESCRIPTION:
    This function will get the pin status either from the global data if the
    pin is already cached or from the card

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_pin_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_HKEY_OPERATION

  DESCRIPTION:
    This function will handle the Hidden Key requests

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_hiddenkey_operation (
  const mmgsdi_pin_operation_req_type * req_ptr
);

/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_TOTAL_USIM_AIDS

  DESCRIPTION:
    This function will get the total number of aids available. The command
    is not sent down to the card.

  DEPENDENCIES:

  LIMITATIONS:
    ONLY applicable to UICC

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

===========================================================================-*/
mmgsdi_return_enum_type  mmgsdi_uicc_get_total_usim_aids(
  int32 *count_ptr
);

/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_AVAILABLE_AIDS

  DESCRIPTION:
    This function will read the EF-DIR to find out if there are any UICC
    Applications in the card. It will however return only the number of
    USIM applications back to the caller and also store the information for
    all the apps in the global mmgsdi_data_slot1. If EF-DIR is not found
    on the card, ef_dir_not_found_ptr is set to TRUE.

  DEPENDENCIES:

  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_read should be used

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_get_available_aids(
  mmgsdi_slot_id_enum_type     slot_id,
  mmgsdi_slot_data_type       *slot_data_ptr,
  int32                       *aid_count_ptr,
  boolean                     *fail_reason_card_error_ptr,
  boolean                     *ef_dir_not_found_ptr);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_IS_SERVICE_AVAILABLE

  DESCRIPTION:
    This function will find out if the service is enabled in UST

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_is_service_available (
  const mmgsdi_srv_available_req_type   *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_AUTH

  DESCRIPTION:
    This function will allow USIM Authentication command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_auth (
  const mmgsdi_auth_req_type   *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_BCAST

  DESCRIPTION:
    This function will allow USIM BCAST command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_bcast (
  const mmgsdi_bcast_req_type   *req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_UICC_CHK_SRV_AVAILABLE

  DESCRIPTION:
    This function checks the SRV AVAILABLE BIT in USIM

  DEPENDENCIES:


  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_chk_srv_available(
  mmgsdi_client_id_type     client_id,
  mmgsdi_session_id_type    session_id,
  mmgsdi_chk_srv_enum_type  check_type,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_available_ptr);

/* ============================================================================
  FUNCTION:      MMGSDI_UICC_CHK_SRV_ENABLED

  DESCRIPTION:
    This function checks the SRV ENABLED BIT in EST

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_chk_srv_enabled(
  mmgsdi_client_id_type     client_id,
  mmgsdi_session_id_type    session_id,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_enabled_ptr);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities for fdn, bdn, acl and phonebook
    pbr decoding procedure for UICC

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_get_capabilities (
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_app_enum_type      app_type
);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_PROCESS_ENABLE_OR_DISABLE_SRV

  DESCRIPTION:
    This function will enable/disable a service by doing an update to the EST EF

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_process_enable_or_disable_srv (
  mmgsdi_request_header_type   request_header,
  mmgsdi_service_enum_type     service,
  mmgsdi_cmd_enum_type         cmd_type
);

/*===========================================================================
FUNCTION MMGSDI_UICC_GSTK_LOC_ENV_RSP_RECEIVED

DESCRIPTION
  This function, called in mmgsdi_process_command.
  If card has responded to VPE with an IMSI switch, an internal
  refresh will be triggered by MMGSDI if MMGSDI has a stale value
  of IMSI in its cache.

DEPENDENCIES
  None

LIMITATIONS
  This command is only supported for USIM

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_gstk_loc_env_rsp_received (
  const mmgsdi_gstk_loc_env_rsp_received_req_type *gstk_loc_env_rsp_received_req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_UICC_SET_FILE_STATUS

  DESCRIPTION:
    This function will set the status of the EF to activated or deactivated

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using
    this function

  LIMITATIONS:
    This function is limited to the use of UICC.

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_set_file_status (
  const mmgsdi_set_file_status_req_type  *req_ptr,
  mmgsdi_cmd_enum_type                    req_type
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_CLEAR_ONCHIP_USIM_AUTHENTICATE

   DESCRIPTION:
     This function clears the authenticate responses for onchip

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     None

   SIDE EFFECTS:

============================================================================*/
void mmgsdi_uicc_clear_onchip_usim_authenticate(
  void
);

#endif /* MMGSDI_UICC_H */
