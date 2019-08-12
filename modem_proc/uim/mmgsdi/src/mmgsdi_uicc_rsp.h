#ifndef MMGSDI_UICC_RSP_H
#define MMGSDI_UICC_RSP_H
/*===========================================================================


           M M G S D I   I C C   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uicc_rsp.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/07/16   kv      UIM PSM handling updates
09/25/15   vr      Added support for ERA-GLONASS
05/18/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
10/23/14   yt      Updates to session and app management
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/23/14   yt      Send STATUS before app deactivation
01/21/14   yt      Remove duplicate code
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
04/16/13   tl      Clean up of radio access features from MMGSDI code
02/07/13   yt      Support for activation/deactivation of EFs
09/03/12   bcho    Modify mmgsdi_uicc_rsp_search prototype for sync operation
07/30/12   yt      Parse SELECT AID response
12/23/11   shr     Removed featurization for ALWAYS ON features
05/17/11   yt      Added support for INCREASE request
04/27/10   shr     Added Hiddenkey support
12/09/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
10/12/09   kp      Moving Read-Write varibles to Heap
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
04/07/07   sp      Added parsing of life cycle byte
10/26/06   tml     Externalize decode_pin_header function
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Added support to handle a Status Command Response from the
                   Card
07/26/05   sst     Added MMGSDIBT support
07/13/05   tml     extern the parsing function
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"
#include "mmgsdilib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/*TODO - Can we remove this?*/
#define MMGSDI_UICC_MAX_PIN_NUM 27

#define MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED    0x05
#define MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED  0x04
#define MMGSDI_UICC_LIFE_CYCLE_BYTE_CHECK        0xF8

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UICC_PIN_MAP_TABLE_TYPE

   DESCRIPTION:
     This structure contains the mapping table to map the MMGSDI PIN ENUMS to
     the PINS sent by the card.
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_pin_enum_type   pin_enum;
  int32                  pin_val;
} mmgsdi_uicc_pin_map_table_type;

extern mmgsdi_uicc_pin_map_table_type mmgsdi_pin_map_table[MMGSDI_UICC_MAX_PIN_NUM];

/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_read (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_read_cnf_type              ** mmgsdi_cnf_pptr,
  boolean                              synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_write_cnf_type                ** mmgsdi_cnf_pptr,
  boolean                                 synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_INCREASE

   DESCRIPTION:
     This function will build the response to INCREASE request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_increase (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_increase_cnf_type          ** mmgsdi_cnf_pptr,
  boolean                              synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_PIN_HEADER_DATA

   DESCRIPTION:
     This function decodes the EF header information into mmgsdi file attribute
     structure

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_pin_header_data (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_FILE_HEADER

   DESCRIPTION:
     This function decodes the header information for MF, DF and EF

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC
     Currently, this function only supports decoding for EF

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_uicc_rsp_decode_file_header (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length,
  mmgsdi_file_attributes_type                 *gsdi_format_attr_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to the get file attributes request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type  * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type    ** attr_cnf_pptr,
  boolean                             synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This function will parse write access to any file using the UICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
);

/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_status_cnf_type               ** mmgsdi_cnf_pptr,
  boolean                                 synch
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Operation Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_HIDDENKEY_OPERATION

   DESCRIPTION:
     This function will build the response to the Hiddenkey Command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_hiddenkey_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_AUTH

   DESCRIPTION:
     This function will handle the response for a  USIM Authentication
     and build a response.

   DEPENDENCIES:


   LIMITATIONS:
     For ISIM , mmgsdi_session_main_response should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_auth(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_BCAST

   DESCRIPTION:
     This function will handle the response for a  USIM BCAST command
     and build a response.

   DEPENDENCIES:


   LIMITATIONS:
     For ISIM , mmgsdi_session_main_response should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_bcast(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_FREE_PIN_PROTECTION_INFO

   DESCRIPTION:
     This function populate the corresponding command (read/write/activate/
     deactivate/increase etc)'s key reference into mmgsdi file security struct

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_uicc_rsp_free_pin_protection_info(
  mmgsdi_file_attributes_type *file_attr_ptr);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PARSE_SELECT_RESPONSE_ADF

   DESCRIPTION:
     This function will parse the SELECT response for an ADF

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The SELECT response was successfully parsed.
     MMGSDI_ERROR:            The SELECT response was not parsed.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_select_response_adf(
  mmgsdi_int_app_info_type              * app_info_ptr,
  const uint8                           * rsp_data_ptr,
  uint16                                  rsp_data_len,
  mmgsdi_select_rsp_enum_type             select_rsp_type
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_SET_FILE_STATUS

   DESCRIPTION:
     This function will build the response to activate/deactivate request

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_invalidate/
     mmgsdi_icc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_set_file_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_cmd_enum_type                    req_type
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_MANAGE_CHANNEL_OP_SYNC

   DESCRIPTION:
     This function will build the response to the manage Channel sync Command

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_manage_channel_op_sync (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_session_deactivate_cnf_type   ** mmgsdi_cnf_pptr,
  mmgsdi_channel_id_type                 *out_channel_id_ptr,
  uim_mc_action_type                      mc_action
);

/*===========================================================================
FUNCTION MMGSDI_UICC_RSP_MANAGE_CHANNEL

DESCRIPTION
  Function to handle the MANAGE CHANNEL response received from UIM.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_manage_channel(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr
);

#ifdef FEATURE_UIM_MULTI_PROFILE
/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_UICC_RSP_SET_SIM_PROFILE

  DESCRIPTION:
    This function will build the response for set sim profile request

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of USIM

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_sim_profile(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_profile_id_enum_type             profile_id,
  uint8                                   apdu_index,
  uint8                                   num_apdu_count
);
#endif /* #ifdef FEATURE_UIM_MULTI_PROFILE */
#endif /* MMGSDI_UICC_RSP_H */
