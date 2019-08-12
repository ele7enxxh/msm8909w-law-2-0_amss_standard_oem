#ifndef MMGSDI_COMMON_RSP_H
#define MMGSDI_COMMON_RSP_H
/*===========================================================================


           M M G S D I   C O M M O N   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_common_rsp.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/03/16   tkl     F3 log prints cleanup
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
01/21/14   yt      Remove duplicate code
10/02/13   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"


/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_INCREASE

   DESCRIPTION:
     This function will build the response to INCREASE request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_rsp_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_increase (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_increase_cnf_type          ** mmgsdi_cnf_pptr,
  boolean                              synch,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

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

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_write_cnf_type                ** mmgsdi_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_ACTIVATE_ONCHIP

   DESCRIPTION:
     This function will build the response to ACTIVATE ONCHIP REQ

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

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_activate_onchip(
  mmgsdi_return_enum_type                     mmgsdi_status,
  const mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_compute_ip_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_RUN_CAVE

   DESCRIPTION:
     This function will build the response to a RUN CAVE command

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
mmgsdi_return_enum_type  mmgsdi_common_rsp_run_cave (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_CDMA_AKA_AUTH

   DESCRIPTION:
     This function will build the response to a CDMA AKA AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_cdma_aka_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_GENERATE_KEY_VPM

   DESCRIPTION:
     This function will build the response to a GENERATE KEY command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_generate_key_vpm (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_BLOCK

  DESCRIPTION:
    This function will build the response to OTASP Block Request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_block (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_OTAPA

  DESCRIPTION:
    This function will build the response to OTASP OTAPA Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_otapa (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_COMMIT

  DESCRIPTION:
    This function will build the response to OTASP COMMIT Request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_commit (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will build the response to OTASP SSPR Config Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_sspr_config (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_KEY_GEN

  DESCRIPTION:
    This function will build the response to OTASP Key Gen Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_key_gen (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_MS_KEY

  DESCRIPTION:
    This function will build the response to OTASP MS Key Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_ms_key (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_BS_CHAL

  DESCRIPTION:
    This function will build the response to BS CHAL Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_bs_chal (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_CONFIRM

  DESCRIPTION:
    This function will build the response to SSD Confirm Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_confirm (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_UPDATE

  DESCRIPTION:
    This function will build the response to SSD Update Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_update (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_STORE_ESN

  DESCRIPTION:
    This function will build the response to Store ESN request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_store_esn(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_session_store_esn_cnf_type    ** store_esn_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_SEARCH

   DESCRIPTION:
     This function will build the response to search request

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_search (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_search_cnf_type            ** mmgsdi_cnf_pptr,
  boolean                              synch,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_SRV_AVAILABLE

   DESCRIPTION:
     This function will build the response for the service available command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_srv_available(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

#endif /* MMGSDI_COMMON_RSP_H */
