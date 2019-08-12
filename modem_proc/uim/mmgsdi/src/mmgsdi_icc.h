#ifndef MMGSDI_ICC_H
#define MMGSDI_ICC_H
/*===========================================================================


           M M G S D I   I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008, 2011 - 2015 QUALCOMM Technologies, Inc (QTI) and 
its licensors.  All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_icc.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/09/16   ar      Send FDN event source along with FDN event
11/16/15   nr      Updating fdn status in FCN handling based on ADN activation status
08/11/14   bcho    Optimize srv req by using cached srv table in client context
01/21/14   yt      Remove duplicate code
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
03/01/12   bcho    Function prototype comment block updated
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
05/20/11   yt      Added support for INCREASE request
05/17/11   shr     Updates for Thread safety
09/06/08   kk      Changed pkcs15 command handling
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     BDN, Rehabilitate and invalidate supports
04/13/06   jar     Added ONCHIP SIM SUPPORT
11/03/05   tml     Fixed header
08/30/05   jar     Added support to send Status Commands to the Card
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


/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/* =============================================================================
   FUNCTION:      MMGSDI_ICC_READ

   DESCRIPTION:
     This function will provide read access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_read (
  const mmgsdi_read_req_type      * req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_ICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to GSM 11.11 or IS820C

    TThis function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used
    to give an opportunity for a pro active SIM to indicate that the
    SIM wants to issue a SIM Application Toolkit command to the ME.

    The information is provided asynchronously in the response cnf.
    Which will contain information containing:  file ID, total memory space
    available, CHV enabled/disabled indicator, CHV status and other
    GSM/CDMA specific data (identical to SELECT).

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
mmgsdi_return_enum_type  mmgsdi_icc_status (
  mmgsdi_status_req_type      * req_ptr
);

/* =============================================================================
  FUNCTION:      MMGSDI_ICC_SET_FILE_STATUS

  DESCRIPTION:
    This function will rehabilitate or invalidate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_set_file_status (
  const mmgsdi_set_file_status_req_type   *req_ptr,
  mmgsdi_cmd_enum_type                     req_type
);

/* ============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_STATUS

  DESCRIPTION:
    This function will get the PIn Status from the cache or send the command to
    the card if the PIN Status in not available
  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

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
mmgsdi_return_enum_type  mmgsdi_icc_pin_status (
  const mmgsdi_get_all_pin_status_req_type   * req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_ICC_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function checks to see if the service is available in SST/CST.

  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_is_service_available should be used.

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
mmgsdi_return_enum_type  mmgsdi_icc_is_service_available (
  const mmgsdi_srv_available_req_type      * req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_ICC_CHK_SRV_AVAILABLE

  DESCRIPTION:
    This function checks the SRV AVAILABLE BIT in GSM/RUIM

  DEPENDENCIES:


  LIMITATIONS:
    There is no corresponding EST bit in SIM/RUIM

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_chk_srv_available(
  mmgsdi_session_id_type    session_id,
  mmgsdi_chk_srv_enum_type  check_type,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_available_ptr);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_SIM_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities on fdn, bdn, acl and imsi
    rehabilitation procedure for SIM

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
                              When the Search Type is not MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_sim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_RUIM_CAPABILITIES

  DESCRIPTION:
    This function will provide capabilities on fdn, bdn, acl and imsi
    rehabilitation procedure for RUIM

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When FEATURE_MMGSDI_3GPP is not defined

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_ruim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_RUN_GSM_ALGO

  DESCRIPTION:
    This function will allow RUN GSM algorithm command to be sent to UIM

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type   *req_ptr
);

/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_ENABLE_SERVICE

  DESCRIPTION:
    This function will enable service by performing a rehabiliate or
    invalidate of ADN/BDN EF.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_enable_service (
  const mmgsdi_session_enable_service_req_type *req_ptr
);

/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_DISABLE_SERVICE

  DESCRIPTION:
    This function will disable service by performing a rehabiliate or
    invalidate of ADN/BDN EF.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_disable_service (
  const mmgsdi_session_disable_service_req_type *req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_INIT_REHAB_FILE

   DESCRIPTION:
     This function checks if passed in file is invalidated.  If so, it will
     rehabilitate the file.

   DEPENDENCIES:
     Should only be called if FDN is enabled and should only be called for
     initialization processing.

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     Input file is re-habilitated conditionally

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_init_rehab_file(
  boolean                    skip_get_file_attr,
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_enum_type       app_type,
  mmgsdi_file_enum_type      sim_filename,
  mmgsdi_slot_id_enum_type   slot
);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_IS_DCS1800_DF_PRESENT

  DESCRIPTION:
    This function checks if the DCS 1800 DF is present on the card.
    The DCS 1800 NV must be checked before calling this funciton.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC protocol

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_is_dcs1800_df_present (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_IS_DISABLE_CHV1_ALLOWED

  DESCRIPTION:
    This function will check if disable CHV1 is allowed or not in the Service
    Table

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC

  RETURN VALUE:
    TRUE/FALSE

  SIDE EFFECTS:
     None

===========================================================================*/
boolean  mmgsdi_icc_is_disable_chv1_allowed (
  mmgsdi_session_id_type  session_id
);

/*===========================================================================
  FUNCTION:      MMGSDI_ICC_UPDATE_FDN_STATUS

  DESCRIPTION:
    This function will update capabilities on fdn procedure for ICC
    based on ADN activation status.

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_update_fdn_status (
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_fdn_evt_source_enum_type fdn_source
);

#endif /* MMGSDI_ICC_H */
