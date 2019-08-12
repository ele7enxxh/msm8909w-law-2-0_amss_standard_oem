#ifndef MMGSDILIB_P_H
#define MMGSDILIB_P_H
/*===========================================================================


            M M G S D I   L I B R A R Y   D E F I N I T I O N S

                      A N D   F U N C T I O N S


  This file contains Library function that MMGSDI client within UIM can call

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2011 - 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software is
regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/inc/mmgsdilib_p.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/12/15   vdc     Add support for potential silent file change notification
08/14/14   tl      Add support for silent PIN verification
01/10/14   df      Remove unnceccary include
10/04/13   vv      Added NV support for ENS sub-features
09/23/13   df      Support for SIM busy
08/14/13   av      Optimize read transparent req by reading from cache directly
04/25/13   vv      Remove the legacy hotswap function
04/24/13   yt      Add NV_GPRS_ANITE_GCF_I to MMGSDI NV cache
04/17/13   tl      Replace Featurization with NV
02/20/13   at      Added NV_FTM_MODE_I while fetching the cached items
02/14/13   tl      Move QMI UIM to MMGSDI context from QMI modem context
01/03/12   abg     Added API to read nv item from mmgsdi cache
12/05/12   vv      Added support for parallel processing in MMGSDI
08/07/12   tl      Added prototype of mmgsdi_get_protocol
03/29/12   shr     Added support for non-prov. app selection using zero length
                   and partial AID, support for Send APDU extension API
03/01/12   bcho    Function prototype comment block updated
02/23/12   shr     Added support for Session Open with MF
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
12/07/11   ssr     extern mmgsdi_init_rtre_config function
10/13/11   shr     Adding support for full recovery
09/23/11   nb      Async TP enhancement
08/01/11   vs      Support to open session with specified select rsp
05/11/11   nb      Added prototype for mmgsdi_tp_download_complete()
03/22/11   nb      Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "mmgsdilib_v.h"
#include "uim_p.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/*----------------------------------------------------------------------------
STRUCTURE       MMGSDI_APP_SELECT_WITH_RSP_TYPE
-----------------------------------------------------------------------------*/
/** @brief Contains the information for an application, whether the channel
    on which the application is selected can be shared and the type of
    select response expected
*/
typedef struct {
  mmgsdi_data_type             app_id_data;       /**< Application identifier */
  boolean                      exclusive_channel; /**< Indicates whether other
                                                       session IDs can be
                                                       mapped to the same
                                                       channel */
  mmgsdi_select_rsp_enum_type  select_rsp_type;   /**< Type of select response */
} mmgsdi_app_select_with_rsp_type;

/*=============================================================================

                     FUNCTION DECLARATIONS

=============================================================================*/
/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OPEN_WITH_SELECT_RSP
===============================================================================*/
/**
  Opens a non-provisioning session to an application on the card and returns
  the type of select response requested.

  Application selection using full length AID, partial AID and empty AID (i.e.
  AID length of zero) are supported.
  When partial/empty AID is provided, first/only occurence of the AID on the
  card is selected.

  @param[in] client_id        Client ID of the caller
  @param[in] slot             Slot in which card is inserted
  @param[in] app_info         Application ID, channel exclusivity and
                              select response type
  @param[in] evt_cb_ptr       Application event callback. A client must provide
                              an event callback if it is to receive
                              session-related event notifications. If the
                              pointer is NULL, the client receives no event
                              notifications.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  The client must have a valid client ID.

  @limitations
  Function cannot be used with ICC cards.

  @sideeffects
  The event callback for the session of interest is called every time the MMGSDI
  notifies the client on this session about all session-related events.
*/
mmgsdi_return_enum_type  mmgsdi_session_open_with_select_rsp (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_app_select_with_rsp_type    app_info,
  mmgsdi_evt_callback_type           evt_cb_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);


/*===========================================================================
   FUNCTION:      MMGSDI_GET_RECOVERY_DATA_SYNC

   DESCRIPTION:
     Returns all information needed for recovering a slot which has been Reset.

     This function uses critical sections to protect accesses to
     MMGSDI global data. Hence, it is advisable to pet the dog
     before invoking the function.

   PARAMETERS:
   uim_recovery_data_type recovery_data_ptr:  Output.  Pointer to the MMGSDI
                                                       recovery data type.
                                                       The result is stored
                                                       in this pointer.

   uim_slot_type          slot:               Input.   Slot for which data
                                                       is needed.
   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Request was successfully handled.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_recovery_data_sync(
  uim_recovery_data_type *recovery_data_ptr,
  uim_slot_type           uim_slot_id
);


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_LINK_EST

   DESCRIPTION:
     Notifies MMGSDI of card detection and provides all information about
     the Cards.
     The API shall be called by UIM after cards have been detected during
     inital power-up, due to a hotswap or as a result of a card RESET command.

   PARAMETERS:
   uim_link_established_data_type link_est_data:  Input.  Info on the detected
                                                          cards.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_link_est(
  uim_link_established_data_type link_est_data
);


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_CARD_ERROR

   DESCRIPTION:
     Notifies MMGSDI of any Card Error and the cause.

   PARAMETERS:
   uim_status_type error_condition:  Input.  Card Error cause.

   uim_slot_type   slot:             Input.  Card slot on which error
                                             has occurred.

   boolean         temporary_error:  Input.  Temporary or permanent
                                             Card error

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_card_error(
  uim_status_type error_condition,
  uim_slot_type   slot,
  boolean         temporary_error
);


/*===========================================================================
FUNCTION   mmgsdi_init_rtre_config

DESCRIPTION
  Initializes the RTRE config with CM

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  This function is passsing the RTRE Mode / RTRE Configuration value to the CM and
  later CM will notify this RTRE configuration value to its clients.
===========================================================================*/
void mmgsdi_init_rtre_config(
  void
);


/*================================================================
FUNCTION:      MMGSDI_SESSION_OPEN_WITH_MF

DESCRIPTION:
  Opens a non-provisioning session and a corresponding logical channel
  on the card.

PARAMETERS:
  client_id:        Client ID of the caller
  slot:             Slot in which card is inserted
  evt_cb_ptr:       Application event callback. A client must provide
                    an event callback if it is to receive
                    session-related event notifications. If the
                    pointer is NULL, the client receives no event
                    notifications.
  response_cb_ptr:  Pointer to the response callback.
  client_ref:       User data returned upon completion of this
                    command.

DEPENDENCIES:
  The client must have a valid client ID.

LIMITATIONS:
  Function cannot be used with ICC cards.
  PIN operations are not supported on the opened Session.

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  The event callback for the session of interest is called every time the MMGSDI
  notifies the client on this session about all session-related events.
================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open_with_mf (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_evt_callback_type           evt_cb_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);


/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE_EXT

DESCRIPTION
  Clients invoke this API to read data from a cached file.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  read_data_ptr:               Buffer length and pointer. Used to store the
                               data to be read. The memory should be allocated
                               and provided by the caller.
  offset:                      Offset to start the read from

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Either a transparent file or 1 record from a linear fixed file can be cached.
  Always read the entire file/record.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_NOT_FOUND:          The requested file not found
    MMGSDI_NOT_INIT:           Cache not yet initialized

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache_ext (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type           * read_data_ptr,
  mmgsdi_offset_type           offset
);


/* ============================================================================
   FUNCTION       MMGSDI_GET_PROTOCOL

   DESCRIPTION
     This function will request protocol for the slot.
     The client calling this function expects an instant response.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     protocol_ptr:      Buffer to store the protocol information.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_and_evt_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_INIT:         Called before CARD_INSERTED on either slot

   SIDE EFFECTS:
     None
===============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_protocol (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_protocol_enum_type        * protocol_ptr
);

/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_SIM_BUSY

   DESCRIPTION:
     Called by UIM Drivers to inform MMGSDI that the SIM card has been busy
     for a long period while processing a command.  UIM drivers is currently
     unable to send/receive APDUs but expects normal operation to resume.
     MMGSDI must inform its clients so they can take appropiate action to
     cope until normal sim card operations resume.

   PARAMETERS:
     bool          sim_busy :  True  => sim is currently busy
                               False => normal operations resumed
     uim_slot_type slot     :  Affected slot
   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task should invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

       MMGSDI_SUCCESS:          The command structure was properly generated
                                and queued onto the MMGSDI Command Queue.
       MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                within appropriate ranges.
       MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                                because the max number of commands are already
                                queued.
       MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_sim_busy(
  boolean        sim_busy,
  uim_slot_type  slot_id
);

/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_LINK_EST_SLOT

   DESCRIPTION:
     Notifies MMGSDI of card detection and provides all information about
     the card on a slot.
     The API shall be called by UIM after a card has been detected during
     initial power-up or due to a hotswap or as a result of a card RESET
     command.

   PARAMETERS:
   link_est_data:  Input info of the detected card on a slot
   uim_slot_type:  slot_id

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
   MMGSDI_ERROR:             Generic error.


   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_link_est_slot(
  uim_slot_link_established_data_type link_est_data,
  uim_slot_type                       slot_id
);

/*============================================================================
   FUNCTION:      MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE

   DESCRIPTION:
     In some specific cards available in the field when BIP session or SMS PP
     download is triggered, there is a potential for certain files to be update
     without any indication to MMGSDI, so this function will notify MMGSDI when
     any BIP session or SMS PP download starts, so that MMGSDI can perform the
     appropriate action. This will potentially save on redundant SELECTs when
     accessing the EFs.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     slot:              Slot on which potential file change occurs. 

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only GSTK task should invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
   MMGSDI_ERROR:             Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_start_potential_silent_file_change(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot
);

/*================================================================
FUNCTION  MMGSDI_SET_QMI_UIM_SIG

DESCRIPTION:
  Sets the MMGSDI_QMI_CAT_SIG to begin processing of QMI CAT.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  mmgsdi_task must be finished initialization.

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void mmgsdi_set_qmi_uim_sig
(
  void
);

#endif /* MMGSDILIB_P_H */

