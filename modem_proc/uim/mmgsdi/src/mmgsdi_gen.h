#ifndef MMGSDI_GEN_H
#define MMGSDI_GEN_H
/*===========================================================================


           M M G S D I   I C C   H E A D E R S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_gen.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/03/16   tkl     F3 log prints cleanup
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
09/26/13   av      Enable OnChip on bootup based on EFS item
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
03/29/12   shr     Added support for Send APDU extension API
12/21/11   shr     Legacy GSDI removal updates
10/13/11   shr     Adding support for full recovery
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
03/02/10   kk      Fixed get all available apps command handling
02/01/10   jk      EPRL Support
02/24/09   nb      Perso Command support in MMGSDI
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/20/08   sun     Alowed protocol to be sent as part of card reset
09/06/08   kk      Changed pkcs15 command handling
11/15/07   sun     Added support for NAA refresh
09/10/07   tml     Added MFLO Supports.
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
03/16/06   sun     Added Auto Refresh Function
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
11/03/05   sun     Fixed refresh to a refresh command
08/18/05   pv      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"
#include "mmgsdilib.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define MMGSDI_GEN_SW_LEN                        4
#define MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN      0xFF

/* One byte for master tasg and two bytes for length in the OnChip
   activation configuration data */
#define MMGSDI_ONCHIP_TLV_PAYLOAD                3

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
   FUNCTION:      MMGSDI_GEN_CARD_RESET_SYNC

   DESCRIPTION:
     This function shall send a reset command to UIM and wait for the response
     synchronously

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
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_card_reset_sync(
  mmgsdi_slot_id_enum_type slot,
  uim_protocol_type        protocol
);

/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PUP

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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
extern mmgsdi_return_enum_type mmgsdi_gen_process_card_pup(
  const mmgsdi_card_pup_req_type  *request_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PDOWN

   DESCRIPTION:
     This function will send down a power down command to the UIM
     module to power down the CARD.

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
extern mmgsdi_return_enum_type mmgsdi_gen_process_card_pdown(
  const mmgsdi_card_pdown_req_type *request_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_RESET

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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
mmgsdi_return_enum_type mmgsdi_gen_process_card_reset(
  const mmgsdi_card_reset_req_type  *req_ptr);

/*=============================================================================
  FUNCTION: MMGSDI_GEN_PROCESS_GET_ATR

  DESCRIPTION:
    Function will get the ATR (Answer to Reset)from the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_get_atr (
  mmgsdi_cmd_enum_type           cmd,
  const mmgsdi_get_atr_req_type* req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_GEN_GET_RSP_TABLE_FLUSH_ALL

DESCRIPTION
  Function will clear all all entries in the GET APDU Get Response
  Cache Table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Will Free Up any memory allocated by any of the Add Functions

===========================================================================*/
void  mmgsdi_gen_get_rsp_table_flush_all(
  mmgsdi_slot_id_enum_type  slot_id
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_PROCESS_SEND_APDU

  DESCRIPTION:
    This function will send an APDU to the UICC.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_send_apdu (
  mmgsdi_send_apdu_req_type * req_ptr,
  mmgsdi_protocol_enum_type   protocol
);

/*===========================================================================
FUNCTION:      MMGSDI_GEN_BUILD_CARD_RESET

DESCRIPTION:
  This function is used to build a card reset command

DEPENDENCIES:

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_build_card_reset(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               curr_req_type,
  uim_protocol_type                  protocol,
  mmgsdi_card_reset_cause_enum_type  reset_cause,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_GET_INFO

  DESCRIPTION:
    This function will handle session get info request.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_get_info(
  const mmgsdi_session_get_info_req_type       *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_PROV_APP_INIT_COMPLETE

  DESCRIPTION:
    This function will handle prov app init complete request.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_prov_app_init_complete(
  const mmgsdi_session_prov_app_init_complete_req_type *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION

  DESCRIPTION:
    This function will handle a manage subscription request which is triggered
    when a client wants to mark an application illegal based on network
    rejection or later mark it back as legal.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    Makes application connected with session as illegal or ready, sends illegal
    subscription event or legal subscription event to all sessions associated
    with that app

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_manage_illegal_subscription(
  const mmgsdi_session_manage_illegal_subscription_req_type  *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_CPHS_INFO

  DESCRIPTION:
    This function will handle get cphs information request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_cphs_info(
  const mmgsdi_session_get_cphs_info_req_type *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_AVAILABLE_APPS_REQ

  DESCRIPTION:
    This function will handle get available apps request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_available_apps_req(
  const mmgsdi_available_apps_req_type * req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_APP_CAPABILITIES

  DESCRIPTION:
    This function will handle get app capability request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_app_capabilities(
  const mmgsdi_session_get_app_capabilities_req_type *req_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_READ_PRL

  DESCRIPTION:
    This function will handle session read prl request.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_read_prl(
  const mmgsdi_session_read_prl_req_type       *req_ptr
);

/*===========================================================================

FUNCTION mmgsdi_gen_onchip_auto_activate

DESCRIPTION
    This function auto activates the OnChip USIM/SIM feature for the given
    slot. This feature is controlled by an EFS item. If the EFS item indicates
    that OnChip feature is to be auto-activated, the function queues the
    command to MMGSDI for activating it for SIM or USIM (which is also
    indicated in the EFS item). The configuration required for activating
    OnChip is read from an EFS item. If the OnChip auto-activation has been
    successfully kicked off, the function returns SUCCESS.

DEPENDENCIES
    None

RETURN VALUE
    MMGSDI_SUCCESS: If OnChip activation req has been successfully queued
    MMGSDI_ERROR:   Otherwise

SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_onchip_auto_activate(
  mmgsdi_slot_id_enum_type req_slot_id
);

#endif /* MMGSDI_GEN_H */
