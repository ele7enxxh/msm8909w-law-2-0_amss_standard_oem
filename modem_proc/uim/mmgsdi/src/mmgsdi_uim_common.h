#ifndef MMGSDI_UIM_COMMON_H
#define MMGSDI_UIM_COMMON_H
/*===========================================================================


           M M G S D I   U I M   C O M M O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_common.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/22/14   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"


/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called to send Compute IP Auth request to UIM.

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
mmgsdi_return_enum_type mmgsdi_uim_common_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_RUN_CAVE

   DESCRIPTION
     This function is called to send RUN CAVE ALGORITHM request to UIM.

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
mmgsdi_return_enum_type mmgsdi_uim_common_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_CDMA_AKA_AUTH

   DESCRIPTION
     This function is called to send CDMA AKA Authentication request to UIM.

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
mmgsdi_return_enum_type mmgsdi_uim_common_cdma_aka_auth(
  const mmgsdi_cdma_aka_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_GENERATE_KEY_VPM

   DESCRIPTION
     This function is called to send Generate Key VPM request to UIM.

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
mmgsdi_return_enum_type mmgsdi_uim_common_generate_key_vpm(
  const mmgsdi_generate_key_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                    protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_BLOCK

  DESCRIPTION:
    This function will format the OTASP block request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP Block
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_block(
  const mmgsdi_session_otasp_block_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_UICC_OTASP_OTAPA

  DESCRIPTION:
    This function will format the OTAPA request in the form required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending the store
                             ESN command to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_otapa (
  const mmgsdi_session_otasp_otapa_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_COMMIT

  DESCRIPTION:
    This function will format the OTASP Commit request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_commit (
  const mmgsdi_session_otasp_commit_req_type *req_ptr,
  mmgsdi_protocol_enum_type                   protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will format the OTASP SSPR Config request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP SSPR
                             Config CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_sspr_config (
  const mmgsdi_session_otasp_sspr_config_req_type *req_ptr,
  mmgsdi_protocol_enum_type                        protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_KEY_GEN

  DESCRIPTION:
    This function will format the OTASP KEY GEN request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP KEY
                             GEN CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_key_gen (
  const mmgsdi_session_otasp_key_gen_req_type *req_ptr,
  mmgsdi_protocol_enum_type                    protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_OTASP_MS_KEY

  DESCRIPTION:
    This function will format the OTASP MS KEY request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP MS KEY
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_otasp_ms_key (
  const mmgsdi_session_otasp_ms_key_req_type *req_ptr,
  mmgsdi_protocol_enum_type                   protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_BS_CHAL

  DESCRIPTION:
    This function will format the BS CHAL request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_bs_chal (
  const mmgsdi_session_bs_chal_req_type *req_ptr,
  mmgsdi_protocol_enum_type              protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_SSD_CONFIRM

  DESCRIPTION:
    This function will format the OTASP SSD Confrim request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending OTASP
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_ssd_confirm (
  const mmgsdi_session_ssd_confirm_req_type *req_ptr,
  mmgsdi_protocol_enum_type                  protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_SSD_UPDATE

  DESCRIPTION:
    This function will format the SSD Update request in the form
    required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending SSD Update
                             CMD to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_ssd_update (
  const mmgsdi_session_ssd_update_req_type *req_ptr,
  mmgsdi_protocol_enum_type                 protocol
);

/*============================================================================
  FUNCTION:      MMGSDI_UIM_COMMON_STORE_ESN

  DESCRIPTION:
    This function will format the store ESN request in the form required by UIM.

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The funtion was successful in sending the store
                             ESN command to UIM.
    MMGSDI_ERROR:
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_store_esn (
  mmgsdi_session_store_esn_req_type    *req_ptr,
  boolean                               sync_process,
  mmgsdi_session_store_esn_cnf_type   **cnf_pptr,
  mmgsdi_protocol_enum_type             protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_UNBLOCK_PIN
   DESCRIPTION:
     This function packages the UNBLOCK PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_unblock_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                   retry_type,
  mmgsdi_protocol_enum_type                    protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_VERIFY_PIN

   DESCRIPTION:
     This function packages the VERIFY PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_verify_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                   retry_type,
  mmgsdi_protocol_enum_type                    protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_ENABLE_PIN
   DESCRIPTION:
     This function packages the ENABLE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_enable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_DISABLE_PIN
   DESCRIPTION:
     This function packages the DISABLE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_disable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
);

/*============================================================================
   FUNCTION:      MMGSDI_UIM_COMMON_CHANGE_PIN
   DESCRIPTION:
     This function packages the CHANGE PIN  command and sends it to UIM
     for further processing

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_common_change_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr,
  mmgsdi_protocol_enum_type                         protocol
);

#endif /* MMGSDI_UIM_COMMON_H */
