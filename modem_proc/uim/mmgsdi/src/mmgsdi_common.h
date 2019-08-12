#ifndef MMGSDI_COMMON_H
#define MMGSDI_COMMON_H
/*===========================================================================


           M M G S D I   C O M M O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_common.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/03/16   tkl     F3 log prints cleanup
01/21/14   yt      Remove duplicate code
10/02/13   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_GET_FILE_ATTR

   DESCRIPTION:
     This function process the GET_FILE_ATTR command for ICC or UICC.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     None


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_get_file_attr(
  mmgsdi_get_file_attr_req_type * req_ptr,
  mmgsdi_protocol_enum_type       protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_WRITE

  DESCRIPTION:
    This function will process the write command to any file

  DEPENDENCIES:


  LIMITATIONS:
    None

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
mmgsdi_return_enum_type  mmgsdi_common_write (
  mmgsdi_write_req_type     * req_ptr,
  mmgsdi_protocol_enum_type   protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_INCREASE

  DESCRIPTION:
    This function will provide increase access to any file 

  DEPENDENCIES:


  LIMITATIONS:
    None

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
mmgsdi_return_enum_type  mmgsdi_common_increase (
  mmgsdi_increase_req_type     * req_ptr,
  mmgsdi_protocol_enum_type      protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_ACTIVATE_ONCHIP

  DESCRIPTION:
    This function will be used to activate the ONCHIP USIM or SIM.  It will
    configure the USIM/SIM Cache.

  DEPENDENCIES:


  LIMITATIONS:
    USIM Authentication in USIM mode is not supported.

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
    None.

  CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to slot data ptr, channel info ptr, pin info ptr and app info ptr.
============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_activate_onchip(
  mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr,
  mmgsdi_onchip_mode_enum_type          onchip_mode
);

/*============================================================================
   FUNCTION:      MMGSDI_COMMON_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called for a Compute IP Authentication request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_COMMON_RUN_CAVE

   DESCRIPTION
     This function is called for a RUN CAVE ALGORITHM request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_COMMON_CDMA_AKA_AUTH

   DESCRIPTION
     This function is called for a CDMA AKA Authenticate request.

   DEPENDENCIES:
     None

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
mmgsdi_return_enum_type mmgsdi_common_cdma_aka_auth(
  const mmgsdi_cdma_aka_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_GENERATE_KEY_VPM

  DESCRIPTION:
    This function will allow GENERATE KEY / VPM command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_common_generate_key_vpm (
  const mmgsdi_generate_key_req_type   *req_ptr,
  mmgsdi_protocol_enum_type             protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_BLOCK

  DESCRIPTION:
    This function will allow OTASP Block Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_block (
  const mmgsdi_session_otasp_block_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_OTAPA

  DESCRIPTION:
    This function will allow OTAPA Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_otapa (
  const mmgsdi_session_otasp_otapa_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_COMMIT

  DESCRIPTION:
    This function will allow OTASP Commit Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_commit (
  const mmgsdi_session_otasp_commit_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will allow OTASP SSPR Config Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_sspr_config (
  const mmgsdi_session_otasp_sspr_config_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                            protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_KEY_GEN

  DESCRIPTION:
    This function will allow OTASP KEY GEN Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_key_gen (
  const mmgsdi_session_otasp_key_gen_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                        protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_MS_KEY

  DESCRIPTION:
    This function will allow OTASP MS Key Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_ms_key (
  const mmgsdi_session_otasp_ms_key_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_BS_CHAL

  DESCRIPTION:
    This function will allow BS CHAL Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_bs_chal (
  const mmgsdi_session_bs_chal_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_CONFIRM

  DESCRIPTION:
    This function will allow SSD Confirm Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_confirm (
  const mmgsdi_session_ssd_confirm_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_UPDATE

  DESCRIPTION:
    This function will allow SSD Update Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_update (
  const mmgsdi_session_ssd_update_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                     protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations

  DEPENDENCIES:

  LIMITATIONS:

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
mmgsdi_return_enum_type  mmgsdi_common_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr,
  mmgsdi_protocol_enum_type                 protocol
);

/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SEARCH

  DESCRIPTION:
    This function will provide search access to record-based file

  DEPENDENCIES:


  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support.
                              When the Search Type is MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_search (
  mmgsdi_search_req_type      * req_ptr,
  mmgsdi_protocol_enum_type     protocol
);

#endif /* MMGSDI_COMMON_H */
