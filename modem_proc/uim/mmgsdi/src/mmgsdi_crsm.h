#ifndef MMGSDI_CRSM_H
#define MMGSDI_CRSM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   C R S M   F U N C T I O N S


GENERAL DESCRIPTION

  This header file contains the prototypes of all Restricted SIM Access
  related functions and data types

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_crsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/14   yt      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdi.h"


/*=============================================================================

                       DEFINES

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_RESTRICTED_SIM_ACCESS_DATA_TYPE

   DESCRIPTION:
      Client data for mmgsdi_restricted_sim_access()
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type    client_id;
  mmgsdi_slot_id_enum_type slot_id;
  uint8                    command;
  mmgsdi_apdu_params_type  apdu_params;
  mmgsdi_callback_type     response_cb;
  mmgsdi_client_data_type  client_ref;
} mmgsdi_restricted_sim_access_data_type;


/*===========================================================================
                       F U N C T I O N S
============================================================================*/

/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_FIND_SESSION_FOR_ACCESS

  DESCRIPTION:
    This function finds the MMGSDI session that can be used for sending the
    command based on slot id and file path.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_find_session_for_access
(
  mmgsdi_slot_id_enum_type                 card_slot,
  mmgsdi_path_type                         file_id_and_path,
  uint8                                    command,
  mmgsdi_session_id_type                 * session_id_ptr,
  mmgsdi_app_enum_type                   * app_type_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_CALCULATE_FILE_ACCESS

  DESCRIPTION:
    This function calculates the file access parameter to be sent in the
    MMGSDI command.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_calculate_file_access
(
  mmgsdi_app_enum_type                     app_type,
  mmgsdi_path_type                         file_id_and_path,
  mmgsdi_access_type                     * file_access_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_COPY_RESTRICTED_SIM_ACCESS_CLIENT_DATA

  DESCRIPTION:
    Allocate heap memory and store the client data passed via
    mmgsdi_restricted_sim_access() API.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_copy_restricted_sim_access_client_data
(
  mmgsdi_client_id_type                     client_id,
  mmgsdi_slot_id_enum_type                  slot_id,
  uint8                                     command,
  mmgsdi_apdu_params_type                   apdu_params,
  mmgsdi_callback_type                      response_cb_ptr,
  mmgsdi_client_data_type                   client_ref,
  mmgsdi_restricted_sim_access_data_type ** crsm_client_data_pptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_CRSM_QUEUE_RESTRICTED_SIM_ACCESS_CMD

  DESCRIPTION:
    This function translates the AT+CRSM request to an MMGSDI request and
    calls the appropriate API.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_crsm_queue_restricted_sim_access_cmd
(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_access_type                       file_access,
  uint8                                    command,
  mmgsdi_apdu_params_type                  apdu_params,
  mmgsdi_data_type                         data,
  mmgsdi_restricted_sim_access_data_type * crsm_client_data_ptr
);

#endif /* MMGSDI_CRSM_H */
