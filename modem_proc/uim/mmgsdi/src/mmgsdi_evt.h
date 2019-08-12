#ifndef MMGSDI_EVT_H
#define MMGSDI_EVT_H
/*===========================================================================



                 M M G S D I   E V T   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the event handling functions for MMGSDI

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_evt.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   ar      Send FDN event source along with FDN event
01/07/16   kv      UIM PSM handling updates
11/23/15   vdc     Send profile switching evt when profile has switched in card
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   stv     To send temporary unlock status indication
11/12/14   vdc     Send card error evt if NO_ATR is received after slot power up
08/22/14   tl      Change client notification when app state is brought
                   down from SUB_READY TO PERSO_READY
04/09/14   tl      Change function prototype
11/05/13   tkl     Add IMSI check for TDS test SIM
10/16/13   df      Remove unused function
09/23/13   df      Support for SIM busy
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
10/21/12   av      Added support for new ECC event
10/12/12   abg     Do not send provisioning events to non-prov sessions
05/23/12   vv      Added support for the file update notification
12/21/11   shr     Legacy GSDI removal updates
09/23/11   nb      Async TP enhancement
10/19/10   kk      Added support for sending FDN event when EF FDN is updated
04/07/10   nb      SAP Enhancement
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
12/08/09   shr     Read EF-DIR and cache app data before sending
                   TP request to GSTK
07/13/09   tml     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ==========================================================================
   ENUM:      MMGSDI_EVT_SESSION_NOTIFY_ENUM_TYPE
===========================================================================*/
typedef enum{
  MMGSDI_EVT_NOTIFY_NONE           = 0,
  MMGSDI_EVT_NOTIFY_ALL_SESSIONS   = 1,
  MMGSDI_EVT_NOTIFY_SINGLE_SESSION = 2,
  MMGSDI_EVT_NOTIFY_CARD_SESSION   = 3,
  MMGSDI_MAX_EVT_NOTIFY_ENUM       = 0x7FFFFFFF
} mmgsdi_evt_session_notify_enum_type;

/* ==========================================================================
   STRUCTURE:      MMGSDI_EVT_SESSION_NOTIFY_TYPE

   DESCRIPTION:
     Structure to indicate what type of notification for event is to be made

     if MMGSDI_EVT_NOTIFY_ALL_SESSIONS or MMGSDI_EVT_NOTIFY_CARD_SESSION:
        slot_id is mandatory
     if MMGSDI_EVT_NOTIFY_SINGLE_SESSION:
        session_id is mandatory
===========================================================================*/
typedef struct{
  mmgsdi_evt_session_notify_enum_type  notify_type;
  mmgsdi_session_id_type               session_id;
  mmgsdi_slot_id_enum_type             slot_id;
} mmgsdi_evt_session_notify_type;

/*===========================================================================
         F U N C T I O N S   T O   B E    O B S O L E L E
============================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_EVT_BUILD_SESSION_CLOSE

   DESCRIPTION:
     This function populate session close event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_evt_build_session_close(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_client_id_type      session_id,
  mmgsdi_int_app_info_type  *app_info_ptr,
  mmgsdi_event_data_type    *evt_info_ptr);

/*===========================================================================
         E V E N T    N O T I F I C A T I O N   F U N C T I O N S
============================================================================*/
/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_FREE_DATA

  DESCRIPTION:
     This function free any memory allocation for the mmgsdi_evet_data_type
     data structure for different command

  DEPENDENCIES:
    None

  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_free_data(
  mmgsdi_event_data_type *event_info_ptr);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_CLIENT

  DESCRIPTION:
    This function sends notification event to registered clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_client(
  mmgsdi_event_data_type *event_info_ptr);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SPECIFIC_CLIENT_ID

  DESCRIPTION:
    This function sends notification event to the passed in client id

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_specific_client_id(
  mmgsdi_client_id_type   client_id,
  mmgsdi_event_data_type *event_info_ptr);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SESSION

  DESCRIPTION:
    This function sends notification event to registered clients to their specific
    sessions

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_session(
  mmgsdi_event_data_type         *event_info_ptr,
  mmgsdi_slot_id_enum_type        slot_id,
  const mmgsdi_int_app_info_type *app_info_ptr,
  boolean                         is_prov_evt);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_CARD_SESSION

  DESCRIPTION:
    This function sends notification event to registered clients to their
    specific card slot sessions

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_card_session(
  mmgsdi_event_data_type         *event_info_ptr,
  mmgsdi_session_type_enum_type   session_type);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_NOTIFY_SPECIFIC_SESSION_ID

  DESCRIPTION:
    This function sends notification event to the specific session id.  Used
    when the client registers late and mmgsdi tries to notify "missing event"
    to this client

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_notify_specific_session_id(
  mmgsdi_session_id_type    session_id,
  mmgsdi_event_data_type   *event_info_ptr);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_PIN

  DESCRIPTION:
    This function populates pin event for notification

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_evt_build_pin(
  mmgsdi_session_id_type      session_id,
  mmgsdi_pin_enum_type        pin,
  mmgsdi_pin_status_enum_type status,
  mmgsdi_event_data_type     *evt_info_ptr);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_AND_NOTIFY_CARD_EVT_FROM_STATE

  DESCRIPTION:
    This function builds the event to client who registered late based on
    the current state of the card.
    Multiple events may be sent out based on what is the currently state
    of MMGSDI

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_build_and_notify_card_evt_from_state(
  mmgsdi_slot_id_enum_type            slot,
  int32                               client_id_table_index);

/* ==========================================================================
  FUNCTION:      MMGSDI_EVT_BUILD_AND_NOTIFY_SESSION_EVT_FROM_STATE

  DESCRIPTION:
    This function builds the event to session who registered late based on
    the current state of the session.
    Multiple events may be sent out based on what is the currently state
    of MMGSDI

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
==========================================================================*/
void mmgsdi_evt_build_and_notify_session_evt_from_state(
  mmgsdi_session_id_type   session_id);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_CARD_INSERTED

  DESCRIPTION
    This function builds and sends the card inserted event to all relevant clients
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_card_inserted(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SUBSCRIPTION_READY

  DESCRIPTION
    This function builds the subscription ready event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_subscription_ready(
  mmgsdi_evt_session_notify_type    notify_type,
  const mmgsdi_int_app_info_type   *app_info_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SESSION_CHANGED

  DESCRIPTION
    This function builds the session changed event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_session_changed(
  mmgsdi_evt_session_notify_type  notify_type,
  boolean                         activate,
  const mmgsdi_int_app_info_type *app_info_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PIN

  DESCRIPTION
    This function builds the PIN1/PIN2/UPIN event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_pin(
  mmgsdi_evt_session_notify_type  notify_type,
  mmgsdi_pin_enum_type            pin_id,
  mmgsdi_pin_status_enum_type     pin_status,
  const mmgsdi_int_app_info_type *app_info_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PERSO

  DESCRIPTION
    This function builds and sends the Perso event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_perso(
  mmgsdi_evt_session_notify_type    notify_type,
  const mmgsdi_int_app_info_type   *app_info_ptr,
  mmgsdi_perso_status_enum_type     status,
  mmgsdi_perso_feature_enum_type    feature,
  uint32                            num_retries,
  uint32                            num_unblock_retries);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_FDN

  DESCRIPTION
    This function builds the FDN event with the current FDN status along with
    the updated record details, if any.
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    MMGSDI_SUCCESS: If the event can be built
    MMGSDI_ERROR:   If the event cannot be built

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_fdn(
  mmgsdi_evt_session_notify_type  notify_type,
  boolean                         is_enabled,
  const mmgsdi_int_app_info_type *app_info_ptr,
  mmgsdi_rec_num_type             rec_num,
  const mmgsdi_data_type         *rec_data_ptr,
  mmgsdi_fdn_evt_source_enum_type fdn_source);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_ILLEGAL_SESSION

  DESCRIPTION
    This function builds the illegal session event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_illegal_session(
  mmgsdi_evt_session_notify_type   notify_type,
  const mmgsdi_int_app_info_type  *app_info_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_LEGAL_SESSION

  DESCRIPTION
    This function builds the legal session event which is sent out when an
    application earlier marked as illegal is updated back as legal application.
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_legal_session(
  mmgsdi_evt_session_notify_type   notify_type,
  const mmgsdi_int_app_info_type  *app_info_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_CARD_ERROR

  DESCRIPTION
    This function builds and sends the card error event to all relevant clients
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_card_error(
  boolean                        notify_all,
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       slot,
  mmgsdi_card_err_info_enum_type prev_err_cause);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_SIM_BUSY

  DESCRIPTION
    This function builds and sends the sim busy event to all relevant clients
    Input parameter client_id is mandatory when notify_all is FALSE

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_sim_busy(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_FILE_UPDATE

  DESCRIPTION
    This function builds the file update event
    The event will be sent to specific session id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_file_update(
  mmgsdi_evt_session_notify_type       notify_type,
  mmgsdi_access_type                   access_type,
  const mmgsdi_write_extra_info_type * write_extra_ptr);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_ECC

  DESCRIPTION
    This function builds and sends out the ECC event.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_ecc(
  mmgsdi_evt_session_notify_type  notify_type);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_TEST_SIM_STATUS

  DESCRIPTION
    This function builds and sends out the Test SIM event.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_test_sim_status(
  boolean                  notify_all,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_TEMP_UNLOCK_EVT

  DESCRIPTION
    This function builds the temporary unlock event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_temp_unlock_evt(
  boolean                                notify_all,
  mmgsdi_perso_temp_unlock_evt_info_type temp_unlock_info,
  mmgsdi_client_id_type                  client_id);

#ifdef FEATURE_UIM_MULTI_PROFILE
/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PROFILE_DATA_EVT

  DESCRIPTION
    This function builds the profile data event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_profile_data_evt(
  boolean                  notify_all,
  mmgsdi_profile_info_type profile_info,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot_id);

/* ============================================================================
  FUNCTION MMGSDI_EVT_BUILD_AND_SEND_PROFILE_SWITCHING_EVT

  DESCRIPTION
    This function builds the profile switching event
    If notify_all is set to TRUE, the event will be broadcasted to all,
    else, the event will be sent to specific client id only

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_evt_build_and_send_profile_switching_evt(
  boolean                  notify_all,
  mmgsdi_profile_info_type profile_info,
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type slot_id);

#endif /* FEATURE_UIM_MULTI_PROFILE */

/*===========================================================================
FUNCTION MMGSDI_EVT_GET_SLOT_FROM_EVENT_DATA

DESCRIPTION
  Extract slot from event data based on the event enum

DEPENDENCIES
  None

RETURN VALUE
  slot id

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
mmgsdi_slot_id_enum_type mmgsdi_evt_get_slot_from_event_data (
  const mmgsdi_event_data_type *event_ptr);

#endif /* MMGSDI_EVT_H */
