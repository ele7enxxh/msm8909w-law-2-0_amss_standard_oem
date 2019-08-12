#ifndef MMGSDICNF_H
#define MMGSDICNF_H
/*===========================================================================


           M M G S D I   U T I L I T Y   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its
licensors.  All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_cnf.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/03/16   tkl     F3 log prints cleanup
04/05/16   vdc     Send act resp and session changed evt at same time
09/25/15   vr      Added support for ERA-GLONASS
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
09/27/14   ar      Add support for multiple slots in BT-SAP
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
11/06/13   yt      Fixes for multi-SIM OnChip activation
08/07/13   tl      Add CDMA AKA Authenticate functionality
06/28/13   bcho    APIs added to return plmn info from SE13 table.
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/11/13   vv      Re-design handling of VPE response from the card
01/20/13   vv      Added support for updating EF-ACSGL record
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
03/01/12   bcho    Function prototype comment block updated
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Added support for Get Operator Name API
08/01/11   av      Slot-specific handling of JCDMA apis
05/17/11   yt      Added support for INCREASE request
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
11/27/10   nmb     Updated ISIM Auth to correct possible out of bounds read
04/07/10   nb      SAP Enhancements
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/04/08   tml     Added sw to the cnf response header
05/05/08   sun     Added Prototype.
03/14/08   nk      Added support for mmgsdi_cnf_build_rehabilitate_data
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     lint
04/13/06   jar     Added ONCHIP SIM Support
02/22/06   tml     Return client and session id in open and close cnf
01/04/06   tml     Updated enum name for SAP and Card powerup and down
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "uim_variation.h"
#include "uim.h"
#include "mmgsdi.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ISIM_AUTH_data_TYPE

   DESCRIPTION:
     This structure is used for ISIM authentication data

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_data_type            res;
  mmgsdi_data_type            ck;
  mmgsdi_data_type            ik;
} mmgsdi_isim_auth_data_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_MAP_REQ_TO_CNF_ENUM_TYPE

   DESCRIPTION:
     This function maps the request enum type to confirmation enum

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

=============================================================================*/
void mmgsdi_cnf_map_req_to_cnf_enum_type(
  mmgsdi_cmd_enum_type  req_type,
  mmgsdi_cnf_enum_type *cnf_type_ptr
);

/* =============================================================================
   FUNCTION:      mmgsdi_cnf_populate_cnf_rsp_header

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

=============================================================================*/
void mmgsdi_cnf_populate_cnf_rsp_header(
  mmgsdi_callback_cnf_type          * cnf_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_AND_QUEUE

   DESCRIPTION:
     This function build the task command for confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_and_queue(
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_request_header_type  * req_header_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const void                        * add_data_ptr,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_WRITE_DATA

   DESCRIPTION:
     This function build the write confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_write_data(
  mmgsdi_write_cnf_type                   * write_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_READ_DATA

   DESCRIPTION:
     This function build the read confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_read_data(
  mmgsdi_read_cnf_type                    * read_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_read_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_INCREASE_DATA

   DESCRIPTION:
     This function build the increase confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_increase_data(
  mmgsdi_increase_cnf_type                * increase_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * inc_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_FILE_ATTR_DATA

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_file_attr_data(
  mmgsdi_get_file_attr_cnf_type           * get_file_attr_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_request_header_type        * req_header_ptr,
  const mmgsdi_data_type                  * data_read_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ISIM_AUTH_DATA

   DESCRIPTION:
     This function builds the ISIM authentication confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_isim_auth_data(
  mmgsdi_isim_auth_cnf_type   *isim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_isim_auth_data_type  *auth_rsp_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the send APDU confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_send_apdu_data(
  mmgsdi_send_apdu_cnf_type               * apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * uim_rsp_data_ptr,
  mmgsdi_slot_id_enum_type                  slot_id);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the SAP Send APDU for Get Response confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_send_apdu_data(
  mmgsdi_sap_send_apdu_cnf_type           * sap_send_apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * apdu_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_get_atr_data(
  mmgsdi_sap_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_slot_id_enum_type            slot_id);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_DISCONNECT_DATA

   DESCRIPTION:
     This function builds the SAP Disconnection confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_disconnect_data(
  mmgsdi_sap_disconnect_cnf_type    * disconnect_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_disconnect_mode_enum_type    disconnect_mode);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PUP_DATA

   DESCRIPTION:
     This function builds the Card Power Up confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pup_data(
  mmgsdi_card_pup_cnf_type               * card_pup_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pup_options_enum_type        pup_options);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PDOWN_DATA

   DESCRIPTION:
     This function builds the Card Power Down confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pdown_data(
  mmgsdi_card_pdown_cnf_type             * card_pdown_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pdown_options_enum_type      pdown_options);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_DATA

   DESCRIPTION:
     This function builds the Status confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_status_data(
  mmgsdi_status_cnf_type            * status_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_data_type            * status_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ONCHIP_SIM_DATA

   DESCRIPTION:
     This function builds cnf for the ONCHIP SIM ACTIVATION REQUEST

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not

   SIDE EFFECTS:

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_onchip_sim_data(
  mmgsdi_act_onchip_sim_cnf_type         * act_onchip_sim_cnf,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_slot_id_enum_type                 slot_id);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_PIN_OPERATION_DATA

   DESCRIPTION:
     This function builds the PIN Operation confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_pin_operation_data(
  const mmgsdi_request_header_type  * req_ptr,
  mmgsdi_pin_operation_cnf_type     * pin_operation_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_client_req_extra_info_type * extra_info_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA

   DESCRIPTION:
     This function builds the PIN Status confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_all_pin_status_data(
  mmgsdi_get_all_pin_status_cnf_type     * get_all_pin_status_cnf_ptr,
  const mmgsdi_request_header_type       * req_header_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SRV_AVAILABLE_DATA

   DESCRIPTION:
     This function builds the Service Available confirmation data

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_srv_available_data(
  mmgsdi_srv_available_cnf_type           * srv_available_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  const mmgsdi_client_req_extra_info_type *extra_info_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_USIM_AUTH_DATA

   DESCRIPTION:
     This function builds the USIM authentication confirmation data

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Protocol is incorrect

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_usim_auth_data(
  mmgsdi_usim_auth_cnf_type   *usim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_data_type      *auth_rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA

   DESCRIPTION:
     This function builds the Compute IP authentication confirmation data

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Protocol is incorrect

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_compute_ip_auth_data(
  mmgsdi_compute_ip_auth_cnf_type   *compute_ip_auth_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *compute_ip_auth_rsp_data_ptr
);


#ifdef FEATURE_MMGSDI_3GPP2
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_CAVE_DATA

   DESCRIPTION:
     This function builds the RUN CAVE confirmation data

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Protocol is incorrect

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_cave_data(
  mmgsdi_run_cave_cnf_type          *run_cave_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *run_cave_rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CDMA_AKA_DATA

   DESCRIPTION:
     This function builds the CDMA AKA Authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_cdma_aka_data(
  mmgsdi_cdma_aka_cnf_type          *aka_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *aka_rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA

   DESCRIPTION:
     This function builds the Generate Key confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_generate_key_vpm_data(
  mmgsdi_generate_key_vpm_cnf_type  *generate_key_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *generate_key_rsp_data_ptr
);
#endif /* FEATURE_MMGSDI_3GPP2 */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_GET_ALL_PIN_STATUS_EVT

   DESCRIPTION:
     This function generates all the pin events as a result of the
     get_all_pin_status confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_generate_get_all_pin_status_evt(
  mmgsdi_session_id_type session_id
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_RSP_AND_EVT

   DESCRIPTION:
     This function based on the confirmation, generate the reponse and
     corresponding event as needed

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to prov app indices.
=============================================================================-*/
void mmgsdi_cnf_generate_rsp_and_evt(
  const mmgsdi_callback_cnf_type     *cnf_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_JCDMA_GET_CARD_INFO

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS


   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_jcdma_get_card_info(
  mmgsdi_slot_id_enum_type                  slot_id,
  mmgsdi_jcdma_get_card_info_cnf_type     * jcdma_get_card_info_cnf_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_atr_data(
  mmgsdi_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type         mmgsdi_status,
  mmgsdi_slot_id_enum_type        slot_id
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REFRESH_DATA

   DESCRIPTION:
     This function builds cnf for the REFRESH

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_refresh_data(
  mmgsdi_refresh_cnf_type                 * refresh_cnf_ptr,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REHABILITATE_DATA

   DESCRIPTION:
     This function build the rehabilitate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_rehabilitate_data(
  mmgsdi_rehabilitate_cnf_type                     * rehabilitate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type          * extra_param_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_BCAST_DATA

   DESCRIPTION:
     This function builds the BCAST confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_bcast_data(
  mmgsdi_bcast_cnf_type   *bcast_cnf_ptr,
  mmgsdi_return_enum_type  mmgsdi_status,
  const mmgsdi_data_type  *bcast_rsp_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEARCH_DATA

   DESCRIPTION:
     This function build the search confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS : The command processing was successful.
     MMGSDI_ERROR   : The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_search_data(
  mmgsdi_search_cnf_type                  *search_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  const mmgsdi_client_req_extra_info_type *extra_param_ptr,
  const mmgsdi_data_type                  *data_search_ptr
);

#ifdef FEATURE_MMGSDI_3GPP
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_GSM_ALGO_DATA

   DESCRIPTION:
     This function builds the RUN GSM algorithm confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_gsm_algo_data(
  mmgsdi_session_run_gsm_algo_cnf_type *run_gsm_algo_cnf_ptr,
  mmgsdi_return_enum_type               mmgsdi_status,
  const mmgsdi_data_type               *rsp_data_ptr
);
#endif /* FEATURE_MMGSDI_3GPP */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_BS_CHAL_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for BS Chal request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_bs_chal_req_data(
  mmgsdi_session_bs_chal_cnf_type                * bs_chal_cnf_ptr,
  mmgsdi_return_enum_type                          mmgsdi_status,
  const mmgsdi_data_type                         * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_MS_KEY_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP MS KEY request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_ms_key_req_data(
  mmgsdi_session_otasp_ms_key_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                               mmgsdi_status,
  const mmgsdi_data_type                              * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_KEY_GEN_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP KEY GEN request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_key_gen_req_data(
  mmgsdi_session_otasp_key_gen_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_SSPR_CONFIG_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP SSPR Config request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_sspr_config_req_data(
  mmgsdi_session_otasp_sspr_config_cnf_type            * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_OTAPA_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTAPA request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR :   The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_otapa_req_data(
  mmgsdi_session_otasp_otapa_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                              mmgsdi_status,
  const mmgsdi_data_type                             * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_BLOCK_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP block request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_block_req_data(
  mmgsdi_session_otasp_block_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                              mmgsdi_status,
  const mmgsdi_client_req_extra_info_type            * extra_param_ptr,
  const mmgsdi_data_type                             * rsp_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_EONS_DATA

   DESCRIPTION:
     This function builds the CNF data for get eons request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_eons_data(
  mmgsdi_session_get_operator_name_cnf_type *eons_cnf_ptr,
  mmgsdi_return_enum_type                    mmgsdi_status,
  const mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_session_id_type                     session_id
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SE13_PLMN_NAMES_DATA

   DESCRIPTION:
     This function builds the CNF data for get SE13 plmn names request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_se13_plmn_names_data(
  mmgsdi_get_se13_plmn_names_cnf_type       *get_se13_plmn_names_cnf_ptr,
  mmgsdi_return_enum_type                    mmgsdi_status,
  const mmgsdi_client_req_extra_info_type   *extra_get_se13_plmn_names_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_UPDATE_ACSGL_DATA

   DESCRIPTION:
     This function builds the cnf data for session update acsgl command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_session_update_acsgl_data(
  mmgsdi_session_update_acsgl_record_cnf_type       *session_update_acsgl_cnf_ptr,
  const mmgsdi_session_update_acsgl_extra_info_type *session_update_acsgl_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GSTK_LOC_ENV_DATA

   DESCRIPTION:
     This function builds the location env response confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_gstk_loc_env_data(
  mmgsdi_gstk_loc_env_rsp_received_cnf_type * gstk_loc_env_cnf_ptr,
  const mmgsdi_client_req_extra_info_type   * extra_param_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SE13_PLMN_INFO_BY_NAME_DATA

   DESCRIPTION:
     This function builds the CNF data for get SE13 plmn info by name request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_se13_plmn_info_by_name_data(
  mmgsdi_get_se13_plmn_info_by_name_cnf_type       *get_se13_plmn_info_by_name_cnf_ptr,
  const mmgsdi_get_se13_plmn_info_by_name_req_type *get_se13_plmn_info_by_name_req_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SET_BUILTIN_PLMN_LIST_DATA

   DESCRIPTION:
     This function builds the CNF data for set builtin plmn list request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_set_builtin_plmn_list_data(
  const mmgsdi_set_builtin_plmn_list_req_type      *set_builtin_plmn_list_req_ptr
);

#ifdef FEATURE_UIM_MULTI_PROFILE
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SIM_PROFILE_DATA

   DESCRIPTION:
     This function builds the CNF data for get sim profile command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_sim_profile_data(
  mmgsdi_get_sim_profile_cnf_type  *get_sim_profile_cnf_ptr,
  mmgsdi_return_enum_type           mmgsdi_status
);
#endif /* FEATURE_UIM_MULTI_PROFILE */
#endif /* MMGSDICNF_H */
