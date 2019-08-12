#ifndef LPA_LPD_H
#define LPA_LPD_H
/*===========================================================================


            L P A   L P D   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_lpd.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   ll      Peek into profile installation result  
06/21/16   av      Add SVN check for supporting phase1, phase1.1 and phase2
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"

#include "lpa.h"
#include "lpa_cmd.h"
#include "lpa_util.h"
#include "lpalib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define LPA_INVALID_REFERENCE_ID             0
#define LPA_EUICC_CHALLENGE_LEN              16
#define LPA_SVN_LEN                          3
#define LPA_INSTALLATION_RESULT_CODE_LEN     2

/* ----------------------------------------------------------------------------
   ENUM:      LPA_LPD_EUICC_PHASE_ENUM_TYPE

   DESCRIPTION:
     Indicates the eUICC phase
-------------------------------------------------------------------------------*/
typedef enum {
  LPA_LPD_PHASE1_EUICC,    /* Conforming up to published phase1 of SGP spec */
  LPA_LPD_PHASE1_1_EUICC,  /* Conforming up to phase1.1 of SGP spec */
  LPA_LPD_PHASE2_EUICC     /* Conforming up to phase2 of SGP spec */
} lpa_lpd_euicc_phase_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_LPD_CMD_ENUM_TYPE

   DESCRIPTION:
     Indicates the ES9+ command rseponses received from MMGSDI or HTTP.
-------------------------------------------------------------------------------*/
typedef enum {
  LPA_LPD_NONE_CMD,
  LPA_LPD_GET_EUICC_CHALLENGE_RESP_CMD,
  LPA_LPD_GET_EUICC_INFO_RESP_CMD,
  LPA_LPD_PREPARE_DOWNLOAD_RESP_CMD,
  LPA_LPD_INITIATE_AUTH_RESP_CMD,
  LPA_LPD_GET_BPP_RESP_CMD,
  LPA_LPD_LOAD_BPP_RESP_CMD,
  LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_CMD
} lpa_lpd_cmd_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_INITIATE_AUTH_RESPTYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  uint32                                  http_response;
  lpa_data_type                           transaction_id;
  lpa_data_type                           dp_signed1;
  lpa_data_type                           smdp_signature1;
  lpa_data_type                           cert_format_to_be_used;
  lpa_data_type                           curve_to_be_used;
  lpa_data_type                           smdp_certificate;
} lpa_lpd_initiate_auth_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_HANDLE_INSTALLATION_RESULT_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  uint32                                  http_response;
} lpa_lpd_handle_installation_result_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_GET_BPP_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  uint32                                  http_response;
  lpa_data_type                           profile_metadata;
  lpa_data_type                           bpp;
} lpa_lpd_get_bpp_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_GET_EUICC_CHALLENGE_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  uint8                                   euicc_challenge[LPA_EUICC_CHALLENGE_LEN];
} lpa_lpd_get_euicc_challenge_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_GET_EUICC_INFO_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  uint8                                   svn[LPA_SVN_LEN];
  lpa_data_type                           cert_info;
  lpa_data_type                           euicc_info;
} lpa_lpd_get_euicc_info_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_PREPARE_DOWNLOAD_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_data_type                           prepare_download;
} lpa_lpd_prepare_download_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_LOAD_BPP_RESP_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_data_type                           load_bpp;
} lpa_lpd_load_bpp_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_CMD_DATA_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_lpd_cmd_enum_type                             cmd;
  union {
    lpa_lpd_get_euicc_challenge_resp_type            get_euicc_challenge_resp;
    lpa_lpd_get_euicc_info_resp_type                 get_euicc_info_resp;
    lpa_lpd_initiate_auth_resp_type                  initiate_auth_resp;
    lpa_lpd_handle_installation_result_resp_type     handle_installation_result_resp;
    lpa_lpd_prepare_download_resp_type               prepare_download_resp;
    lpa_lpd_get_bpp_resp_type                        get_bpp_resp;
    lpa_lpd_load_bpp_resp_type                       load_bpp_resp;
  } data;
} lpa_lpd_cmd_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_RESPONSE_DATA_TYPE

   DESCRIPTION:
     Contains information for handling LPA ES9+ commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                                         link;
  lpa_result_enum_type                                status;
  lpa_slot_id_enum_type                               slot_id;
  lpa_lpd_cmd_data_type                               cmd_data;
} lpa_lpd_response_data_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      LPA_LPD_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
typedef void (*lpa_lpd_callback_type) (lpa_result_enum_type              status,
                                       const lpa_lpd_cmd_data_type      *resp_ptr,
                                       lpa_slot_id_enum_type             slot_id);

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_INITIATE_AUTH_REQ_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_slot_id_enum_type                  slot_id;
  uint8                                  euicc_challenge[LPA_EUICC_CHALLENGE_LEN];
  uint8                                  svn[LPA_SVN_LEN];
  lpa_data_type                          cert_info;
  lpa_data_type                          euicc_info;
  lpa_smdp_address_type                  smdp_address;
  lpa_lpd_callback_type                  lpd_callback;
} lpa_lpd_initiate_auth_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_GET_BPP_REQ_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_slot_id_enum_type           slot_id;
  lpa_data_type                   transaction_id_data;
  lpa_data_type                   prepare_download_data;
  lpa_lpd_callback_type           lpd_callback;
} lpa_lpd_get_bpp_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_LPD_HANDLE_INSTALLATION_RESULT_REQ_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef struct {
  lpa_slot_id_enum_type           slot_id;
  lpa_data_type                   profile_installation_result;
  lpa_lpd_callback_type           lpd_callback;
} lpa_lpd_handle_installation_result_req_type;

/* ----------------------------------------------------------------------------
   ARRAY:      LPA_LPD_PROFILE_INSTALLATION_RESULT_CODE_ARRAY_TYPE

   DESCRIPTION:

-------------------------------------------------------------------------------*/
typedef uint8 lpa_lpd_profile_installation_result_code_array_type[LPA_INSTALLATION_RESULT_CODE_LEN];

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION LPA_LPD_CMD_INIT

DESCRIPTION
  This function, called in lpa_main, is called to initialize the LPD command
  module.

DEPENDENCIES
  LPA task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_lpd_cmd_init (
  void
);

/*===========================================================================
   FUNCTION:      LPA_LPA_FREE_RESPONSE_DATA

   DESCRIPTION:
     This function frees the response pointer that is mallocd (in lpa_mmgsdi or
     in lpa_http) in order to send a response back to LPA LPD via its callback

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_lpd_free_response_data(
  lpa_lpd_cmd_data_type                *lpa_response_data_ptr
);

/*===========================================================================
   FUNCTION:      LPA_LPD_RESET

   DESCRIPTION:
     This function resets/frees any globals in lpa lpd.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_lpd_reset (
  lpa_slot_id_enum_type                lpa_slot
);

/*===========================================================================
FUNCTION LPA_LPD_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, handles all bootstrap command
  processing for each LPA LPD command. This function will also maintain
  the state machine and handle invalid states appropriately.

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_lpd_handle_cmd_sig (
  void
);

/*===========================================================================
FUNCTION LPA_LPD_CLEANUP

DESCRIPTION
  This function flushes the lpa lpd queue.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_lpd_cleanup (
  void
);

/*===========================================================================
FUNCTION LPA_LPD_INIT

DESCRIPTION
  This function, called in lpa_main, is called to initialize the LPA LPD
  module and globals.

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_lpd_init (
  void
);

/*===========================================================================
FUNCTION LPA_LPD_START_ADD_PROFILE_PROCEDURE

DESCRIPTION

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_lpd_start_add_profile_procedure (
  const lpa_add_profile_req_type   *add_profile_req_ptr,
  lpa_slot_id_enum_type      slot_id
);

/*===========================================================================
FUNCTION LPA_LPD_RETRIEVE_EUICC_PHASE

DESCRIPTION
  This function can be called to retrieve the phase conformance of the eUICC

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_lpd_retrieve_euicc_phase (
  lpa_slot_id_enum_type             slot_id,
  lpa_lpd_euicc_phase_enum_type    *phase_ptr
);

#endif /* LPA_LPD_H */

