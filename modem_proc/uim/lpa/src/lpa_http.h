#ifndef LPA_HTTP_H
#define LPA_HTTP_H
/*===========================================================================


            L P A   H T T P   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_http.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/16   ar      Introduce DS http post retry timer logic
10/19/16   ar      Fix to support bigger profile (> 65535) download
10/19/16   ar      Add support for configuring HTTP stack via LPA EFS item
05/16/16   av      Route HTTP request to QMI HTTP
05/15/16   ll      HTTP Chunking
05/15/16   av      LPA should wait for DS session open cb before sending post
04/11/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"
#include "time_types.h"
#include "ds_http_types.h"

#include "lpalib.h"
#include "lpa.h"
#include "lpa_lpd.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define LPA_HTTP_HEADER_NAME_MAX_LEN     32
#define LPA_HTTP_HEADER_VALUE_MAX_LEN    256
#define LPA_SESSION_ID_NULL              0xFFFFFFFF
#define LPA_MAX_PROFILE_SIZE             0x100000  /*1 MB*/

/* Enum used for capturing which stack to be used for HTTP traffic:
   modem or AP */
typedef enum {
  LPA_HTTP_USE_STACK_BASED_ON_CHIP_ID, /* Use CHIP_ID to determine which
                                          HTTP stack to use */
  LPA_HTTP_USE_MODEM_STACK,            /* Use HTTP stack on modem */
  LPA_HTTP_USE_AP_STACK                /* Use HTTP stack on AP */
} lpa_http_stack_usage_enum_type;

typedef enum {
  LPA_HTTP_RSP,
  LPA_HTTP_QMI_HTTP_RSP
} lpa_http_cmd_enum_type;


/* Custom headers that can be specified by the client */
typedef struct
{
  char name [LPA_HTTP_HEADER_NAME_MAX_LEN+1];
  char value[LPA_HTTP_HEADER_VALUE_MAX_LEN+1];
} lpa_http_response_header_type;

typedef struct {
   uint32                         session_id;
   uint32                         request_id;
   sint15                         error;
   uint16                         http_status;
   uint32                         content_size;
   uint8                        * content_ptr;
   uint32                         num_headers;
   lpa_http_response_header_type* header_info_ptr;
} lpa_http_response_data_type;

typedef struct {
   qmi_uim_http_service_registry_id_type service_id;
   qmi_uim_http_transaction_result_type  http_status;
   uint32                                content_size;
   uint8                               * content_ptr;
   uint16                                num_headers;
   lpa_http_response_header_type       * header_info_ptr;
} lpa_http_qmi_http_response_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_HTTP_CMD_TYPE

   DESCRIPTION:
     Contains information for handling LPA HTTP commands
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                            link;
  lpa_http_cmd_enum_type                 cmd_type;
  union {
    lpa_http_response_data_type          rsp_data;
    lpa_http_qmi_http_response_data_type qmi_http_rsp_data;
  } data;
} lpa_http_rsp_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION LPA_HTTP_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in lpa_main, is called to process HTTP event

DEPENDENCIES
  LPA task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_http_handle_cmd_sig (
  void
);

/*===========================================================================
   FUNCTION:      LPA_HTTP_INIT

   DESCRIPTION:
     LPA HTTP init function called during task init

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void lpa_http_init (
  void
);

/*===========================================================================
   FUNCTION:      LPA_HTTP_CLEANUP

   DESCRIPTION:
     LPA HTTP cleanup function called to clean up refrerence data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
lpa_result_enum_type lpa_http_cleanup(
  lpa_slot_id_enum_type             slot_id
);

/*===========================================================================
FUNCTION LPA_HTTP_BUILD_AND_SEND_INITIATE_AUTH

DESCRIPTION
  This function is used to send Initiate Auth request to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_initiate_auth (
  lpa_lpd_initiate_auth_req_type                *initiate_auth_req_ptr
);

/*===========================================================================
FUNCTION LPA_HTTP_BUILP_AND_SEND_GET_BPP

DESCRIPTION
  This function is used to send Get Bound Profile Package request to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_get_bpp (
  lpa_lpd_get_bpp_req_type                *get_bpp_req_ptr
);

/*===========================================================================
FUNCTION LPA_HTTP_BUILP_AND_SEND_HANDLE_INSTALLATION_RESULT

DESCRIPTION
  This function is used to send the Handle Installation Result to SMDP

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_http_build_and_send_handle_installation_result (
  lpa_lpd_handle_installation_result_req_type         *profile_installation_result_req_ptr
);

/*===========================================================================
FUNCTION LPA_TIMER_HANDLE_HTTP_RETRY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in lpa_task, is called to process http retry timer
  expiration signal for a given slot.

DEPENDENCIES
  lpa_task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_http_handle_http_retry_timer_expire_sig (
  lpa_slot_id_enum_type               slot_id
);
#endif /* LPA_HTTP_H */

