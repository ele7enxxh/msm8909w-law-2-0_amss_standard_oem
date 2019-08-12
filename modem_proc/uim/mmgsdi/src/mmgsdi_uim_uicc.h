#ifndef MMGSDI_UIM_UICC_H
#define MMGSDI_UIM_UICC_H
/*===========================================================================


           M M G S D I   U I M   U I C C   H E A D E R S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_uicc.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/07/16   kv      UIM PSM handling updates
12/25/15   kk      Send Card and session events based on SFS data in PSM mode
06/18/15   bcho    Close channel if session is activated on another slot
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
10/23/14   yt      Updates to session and app management
06/23/14   yt      Send STATUS before app deactivation
06/03/14   tl      Remove additional parameter variables
01/21/14   yt      Remove duplicate code
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
04/16/13   tl      Clean up of radio access features from MMGSDI code
02/07/13   yt      Support for activation/deactivation of EFs
09/03/12   bcho    Modifiy mmgsdi_uim_uicc_search_record for sync operation
12/23/11   shr     Removed featurization for ALWAYS ON features
08/01/11   vs      Added support for session open with select response
05/17/11   yt      Added support for INCREASE request
02/17/10   shr     Fixed Session activate/deactivate handling
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/02/09   js      Fixed session open issue for non-prov type sessions with
                   exclusive channel
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Added support for UICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
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

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_transparent (
  const mmgsdi_write_req_type             * req_ptr,
  boolean                                   sync_process,
  mmgsdi_write_cnf_type                   **cnf_pptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_record (
  const mmgsdi_write_req_type             * req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SELECT

   DESCRIPTION:
     This function will format the get file attribute request in the form
     required by the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of UICC. This function is also
     limited to the use of EFs.


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_select (
  mmgsdi_get_file_attr_req_type             *req_ptr,
  boolean                                    sync_process,
  mmgsdi_get_file_attr_cnf_type            **cnf_pptr,
  boolean                                    extra_param_req
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_transparent (
  const mmgsdi_read_req_type              * req_ptr,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_pptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_record (
  const mmgsdi_read_req_type              * req_ptr,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_pptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_INCREASE

   DESCRIPTION:
     This function package the Write Record command and send it to UIM
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_increase (
  const mmgsdi_increase_req_type             * req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_MANAGE_CHANNEL_REQ

   DESCRIPTION
     This function handles MANAGE CHANNEL request

   DEPENDENCIES:
     The function is applicable only for UICC cards

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_manage_channel_req(
  const mmgsdi_manage_channel_req_type  *req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_OPEN_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Open Channel
     function to build a UIM Manage Channel Request to open a new
     communication channel with the Card presently inserted.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_open_channel(
  const mmgsdi_request_header_type *  req_header_ptr,
  mmgsdi_aid_type                  *  aid_ptr,
  boolean                             exclusive_channel,
  boolean                             select_rsp_requested,
  mmgsdi_select_rsp_enum_type         select_rsp_type
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_CLOSE_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Close Channel
     function to build a UIM Manage Channel Request to close a new
     communication channel with the Card presently inserted.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_close_channel(
  const mmgsdi_request_header_type    * req_header_ptr,
  mmgsdi_client_req_extra_info_type   * extra_param_ptr,
  boolean                               sync_process,
  mmgsdi_session_deactivate_cnf_type ** cnf_pptr,
  const mmgsdi_aid_type               * app_data_ptr,
  mmgsdi_channel_id_type                channel_id
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SESSION_DEACTIVATE

   DESCRIPTION
     This function is called for Status command.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_session_deactivate(
  const mmgsdi_status_req_type        *  req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_ISIM_AUTH

   DESCRIPTION
     This function is called for ISIM Authentication request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_isim_auth(
  const mmgsdi_auth_req_type        *  req_ptr
);

/*===========================================================================
FUNCTION:  MMGSDI_UIM_UICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_UIM_UICC_STATUS

   DESCRIPTION
     This function builds the STATUS command and sends it to UIMDRV for
     further processing.

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_status(
  const mmgsdi_status_req_type        *  req_ptr,
  boolean                                sync_process,
  mmgsdi_status_cnf_type             **  cnf_pptr,
  const mmgsdi_aid_type               *  app_data_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_AUTH

   DESCRIPTION
     This function is called for USIM Authentication request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_auth(
  const mmgsdi_auth_req_type        *  req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_BCAST

   DESCRIPTION
     This function is called for USIM BCAST request.

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_bcast(
  const mmgsdi_bcast_req_type        *  req_ptr
);

/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for UICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_search_record (
  const mmgsdi_search_req_type             * req_ptr,
  boolean                                    sync_process,
  mmgsdi_search_cnf_type                  ** cnf_pptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SET_FILE_STATUS

   DESCRIPTION:
     This function will activate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of UICC.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_set_file_status (
  const mmgsdi_set_file_status_req_type * req_ptr,
  mmgsdi_cmd_enum_type                    req_type
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_MANAGE_CHANNEL_OP

   DESCRIPTION
     This function sends MANAGE_CHANNEL_F to UIMDRV

   DEPENDENCIES:
     The function is applicable only for UICC cards

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_manage_channel_op(
  const mmgsdi_request_header_type         *req_header_ptr,
  mmgsdi_channel_id_type                    op_close_channel,
  uim_mc_action_type                        mc_action,
  mmgsdi_client_req_extra_info_type        *extra_param_ptr,
  boolean                                   sync_process,
  mmgsdi_session_deactivate_cnf_type      **cnf_pptr,
  mmgsdi_channel_id_type                   *out_channel_id_ptr
);

#endif /* MMGSDI_UIM_UICC_H */
