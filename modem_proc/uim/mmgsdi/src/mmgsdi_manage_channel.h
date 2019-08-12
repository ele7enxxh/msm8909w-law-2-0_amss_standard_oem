#ifndef MMGSDI_MANAGE_CHANNEL_H
#define MMGSDI_MANAGE_CHANNEL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   M A N A G E   C H A N N E L   F U N C T I O N S


GENERAL DESCRIPTION

  This header file contains data types and prototypes related to converting
  Manage Channel streaming APDU to MMGSDI command
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_manage_channel.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/15   hh      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdi.h"
#include "mmgsdilib_common.h"

/*=============================================================================

                       DEFINES

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_APDU_CLIENT_DATA_TYPE

   DESCRIPTION:
      Client data for streaming APDU
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type               client_id;
  mmgsdi_slot_id_enum_type            slot_id;
  mmgsdi_channel_id_type              channel_id;
  uint8                               command;           /* APDU INS byte */
  mmgsdi_cmd_enum_type                orig_request_type; /* MMGSDI_SEND_APDU_REQ or MMGSDI_SEND_APDU_EXT_REQ */
  mmgsdi_apdu_params_type             apdu_params;
  mmgsdi_send_apdu_options_enum_type  apdu_option;
  mmgsdi_callback_type                response_cb;
  mmgsdi_client_data_type             client_ref;
} mmgsdi_apdu_client_data_type;


/*===========================================================================
                       F U N C T I O N S
============================================================================*/

/* ============================================================================
  FUNCTION:      MMGSDI_APDU_MANAGE_CHANNEL

  DESCRIPTION:
    This function translates the MANAGE CHANNEL APDU streaming to a high-level
    MMGSDI API call.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_apdu_manage_channel
(
  mmgsdi_client_id_type                    client_id,
  mmgsdi_slot_id_enum_type                 slot_id,
  mmgsdi_send_apdu_options_enum_type       apdu_option,
  mmgsdi_send_apdu_data_type               apdu_data,
  mmgsdi_client_data_type                  client_ref,
  mmgsdi_callback_type                     client_response_cb,
  mmgsdi_cmd_enum_type                     req_type,
  boolean                                 *reject_adpu_ptr
);

#endif /* MMGSDI_MANAGE_CHANNEL_H */
