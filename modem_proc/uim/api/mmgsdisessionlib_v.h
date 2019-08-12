#ifndef MMGSDISESSIONLIB_V_H
#define MMGSDISESSIONLIB_V_H
/*================================================================================

  This file contains Library function that MMGSDI client can call to access

                        COPYRIGHT INFORMATION
Copyright (c) 2009 - 2011 Qualcomm Technologies, Incorporated and its licensors.  All
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/mmgsdisessionlib_v.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/05/15   ks      Rel12 polling enhancement
12/21/11   shr     Added support for get file enum from path Sync. API,
                   Is service available sync. API
11/22/11   shr     Updated temporary definitions related to GSDI removal
11/11/11   kk      Added definitions related to GSDI removal temporarily
10/13/11   nb      Added Prototype for new refresh APIs
05/17/11   shr     Updates for Thread safety
04/22/11   vs      Added API for client controlled subscription publication
02/21/11   kk/ms   Added sync call for get app capabilities
08/13/10   js      Moved prototype for  mmgsdi_session_get_info_sync()
                   to mmgsdisessionlib.h
08/04/10   shr     Added support for session based UIM power control API
02/19/10   shr     Session get info API implementation
02/15/10   kk      Added get app info sync api
01/12/10   nb      Update for Rel 7 Enhanced Network Selection
10/23/09   nb      Initial Version


=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

/*=============================================================================

                           DEFINES DECLARATIONS

=============================================================================*/

/* Call masks indicating if PS or NON-PS call is ongoing */
#define MMGSDI_MASK_PS_CALL_ONGOING      0x1
#define MMGSDI_MASK_NON_PS_CALL_ONGOING  0x2

/*=============================================================================

                        ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                           STRUCTURE DECLARATIONS

=============================================================================*/


/*=============================================================================

                     CONFIRMATION STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                         EVENT STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE_FILE_SIZE

DESCRIPTION
  Clients invoke this API to get the size of the file to be cached for read.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  file_size_ptr:               The pointer to the buffer to store the file
                               size. The memory should be allocated and
                               provided by the caller.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:     The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache_file_size (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_len_type            * file_size_ptr
);


/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE

DESCRIPTION
  Clients invoke this API to read data from a cached file.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  read_data:                   Buffer length and pointer. Used to store the
                               data to be read. The memory should be allocated
                               and provided by the caller.

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
    MMGSDI_CMD_QUEUE_FULL:     The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type             read_data
);


/*===========================================================================
FUNCTION        MMGSDI_SESSION_WRITE_CACHE

DESCRIPTION
  Clients invoke this API to write data to a cached file.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                 Session ID of the caller
  file_name:                  The enum used to access the SIM/USIM/RUIM/CSIM
                              Elementary Files.
  write_data:                 Buffer length and pointer. Used to store the
                              data to be written.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Either a transparent file or 1 record from a linear fixed file can be cached.
  Always write the entire file/record.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:     The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_write_cache (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type             write_data
);


/*=============================================================================
FUNCTION        MMGSDI_SESSION_REGISTER_FOR_REFRESH_WITH_STEERING

DESCRIPTION
  This function allows the client to register for file change notifications for
  specific files and/or vote for refresh (i.e. vote for either taking
  the call stack down or terminating application that the client is
  interested in). When the FCN request is received, then MMGSDI will send
  an event MMGSDI_REFRESH_EVT with mode = MMGSDI_REFRESH_FCN with the file
  list that has changed. MMGSDI will filter out the file list based on what
  the client registered for.
  Some FCNs on default could potentially result in bringing the protocol
  stack down - if the client is interested in these files, then they should
  also register for voting explicitly on default app.

  For mode Steering of Roaming, MMGSDI will send an event MGSDI_REFRESH_EVT
  with mode = MMGSDI_REFRESH_STEERING_OF_ROAMING along with the PLMNwAcT
  list in the event payload. MMGSDI will send the event only to clients
  registered for refresh mode steering of roaming. The client needs to handle
  refresh STEERING OF ROAMING events in the mmgsdi event callback function.

PARAMETERS
  session_id         : Session ID of the caller
  refresh_files      : Files that the Client is interested in getting
                            notification for
  vote_for_init     : Vote to ok bringing the call stack down/terminating
                            the application
  response_cb_ptr: Callback to this command
  client_ref          : User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client is.
  If the client is interested in default app, then the appropriate client
  id should be used.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS: The command structure was properly generated
                                   and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                                   within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL: The command was not Queued to the MMGSDI Task
                                               because the max number of commands are already
                                              queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_register_for_refresh_with_steering (
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_INFO_SYNC

DESCRIPTION:
  This API is used by modem layers to extract the app info associated to a
  particular session id, if that session type is active

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id     : Session Id of the caller
  app_type_ptr : App type stored here for calling function to use
                          MMGSDI_APP_NONE: No app selected
                          MMGSDI_APP_SIM: 2G SIM mode selected
                          MMGSDI_APP_RUIM: 2G RUIM mode selected
                          MMGSDI_APP_USIM: 3G USIM mode app selected
                          MMGSDI_APP_CSIM: 3G CSIM mode app selected
                          MMGSDI_APP_UNKNOWN: Unknown app selected

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a GW provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS: Request handled successfully, app_type_ptr has the app info
    MMGSDI_INCORRECT_PARAMS: Session id is invalid
    MMGSDI_ERROR: Request handling failed

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_app_info_sync (
  mmgsdi_session_id_type               session_id,
  mmgsdi_app_enum_type                *app_type_ptr
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_TOGGLE_UIM_POWER_CONTROL_SYNC

DESCRIPTION:
  This API is used by modem layers to inform UIM when the modem is in and out
  of traffic channel. when Modem is in traffic channel, power control is turned
  ON else turned OFF.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id : Session on which traffic channel is enabled
  in_TC      : Boolean - in traffic channel or out of Traffic channel
  mode       : Mode on which traffic channel established GW or 1X
  uim_call_mask : Mask indicating if PS or non PS call is ongoing

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_toggle_uim_power_control_sync (
  mmgsdi_session_id_type      session_id,
  boolean                     in_TC,
  mmgsdi_in_tc_mode_enum_type mode,
  uint16                      uim_call_mask
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC
================================================================*/
/**
Extracts all the FDN, BDN, ACL, and IMSI status provisioning application
capabilities, and phone book-related capabilities.
This is a synchronous function.

This API uses critical sections to protect accesses to
MMGSDI global data. Hence, it is advisable to pet the dog
before invoking the API.

@param session_id           [IN] Session Id of the caller
@param app_capabilities_ptr [OUT] app capabilities

@retval MMGSDI_SUCCESS Command structure was properly generated and sent to the
                       MMGSDI command queue.
@retval MMGSDI_INCORRECT_PARAMS Parameters are not within the appropriate ranges.

@dependencies A valid Session ID for a provisioning application is required.

@sideeffects Undetermined.
*/
mmgsdi_return_enum_type mmgsdi_session_get_app_capabilities_sync(
  mmgsdi_session_id_type                    session_id,
  mmgsdi_session_app_capabilities_cnf_type *app_capabilities_ptr
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_SUBSCRIPTION_OK
================================================================*/
/**
  Allows clients to inform the card services layer that it can proceed to publish
  the subscription to all clients allowing the modem to move from limited service
  to full service for that provisioning application. This API should be invoked
  only after the application state is perso ready which indicates to the client
  that the modem has completed its part of the initialization successfully

@param session_id           [IN] Session Id of the caller
@param ok_for_subscription  [IN] OK/not OK indication to publish subscription
@param response_cb_ptr      [IN] Response callback function pointer
@param client_ref           [IN] Client data stored and returned in callback

@retval MMGSDI_SUCCESS Command structure was properly generated and sent to the
                       MMGSDI command queue.
@retval MMGSDI_INCORRECT_PARAMS Parameters are not within the appropriate range

@dependencies
A valid Session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_subscription_ok(
  mmgsdi_session_id_type        session_id,
  boolean                       ok_for_subscription,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
);

/*=============================================================================
FUNCTION:   MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING
=============================================================================*/
/**
  Registers a client for FCNs for specific files, or for voting to refresh an 
  application, or sending approval for proceeding with refresh.\n

  When an FCN request is received, the MMGSDI sends an MMGSDI_REFRESH_EVT event 
  with mode = MMGSDI_REFRESH_FCN and with the file list that has changed. 
  The MMGSDI filters out the file list based on what the client registered for.\n

  Some FCNs may result in bringing the protocol stack down. If the client has 
  registered for these files, it should also register for voting explicitly 
  on the default application.\n

  Some refresh scenario may result in a card powerdown. If a client is interested 
  in providing approval before MMGSDI can proceed with refresh and do a card 
  power down later, they do it using proceed_with_pdown flag.\n
  
  A Refresh vote means to either take the call stack down or terminate an 
  application. 

  @param session_id      [IN] -- Session ID of the caller.
  @param refresh_files   [IN] -- Files for which the client is getting 
                                 notification.
  @param vote_for_init   [IN] -- Vote to bring down the call stack or 
                                 terminate the application.
  @proceed_with_pdown    [IN] -- Approval before power down. 
  @param response_cb_ptr [IN] -- Pointer to the response callback.
  @param client_ref      [IN] -- User data returned upon completion of this
                                 command.

  @return 
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

  @dependencies A valid session ID is required.

  @limitations This function is limited to the client's session.\n
               If the client has registered for the default application, 
               the appropriate client ID should be used.

  @sideeffects None.
*/
mmgsdi_return_enum_type mmgsdi_session_register_for_refresh_ok_to_proceed_voting (
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  boolean                            proceed_with_refresh,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH
=============================================================================*/
/**
  Approves to proceed with refresh.\n

  The client must call this function if it gets a refresh event 
  (REFRESH_STAGE_WAIT_FOR_OK_TO_PROCEED), or else MMGSDI will not proceed with refresh.
 
  @param session_id      [IN] -- Session ID of the caller.
  @param ok_to_proceed   [IN] -- Client notifies the MMGSDI that it is OK to 
                                 continue with refresh (TRUE) or not (FALSE).
  @param response_cb_ptr [IN] -- Pointer to the response callback.
  @param client_ref      [IN] -- User data returned upon completion of this 
                                 command.

  @return 
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

  @dependencies A valid session ID is required. Also, the client must have 
                registered for FCNs and have received a notification for an 
                FCN before calling this function.

  @sideeffects None.
*/
mmgsdi_return_enum_type mmgsdi_session_ok_to_proceed_with_refresh (
   mmgsdi_session_id_type         session_id,
   boolean                        ok_to_proceed,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref
);

/*=====================================================================
FUNCTION        MMGSDI_SESSION_IS_SERVICE_AVAILABLE_SYNC

DESCRIPTION
  This function determines if a service is available or not for a given
  service type.

  This function uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the function.
  
PARAMETERS
  session_id:             Session ID of the caller.
  srvc_type:              Service that the client is interested in.
  srv_available_ptr:      Pointer to a boolean type, this pointer holds the
                          information if service is available or not.

DEPENDENCIES
  A valid Session ID for a provisioning application is required.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The request was successfully handled.
    MMGSDI_ERROR:            The parameters supplied to the API are not
                             within appropriate ranges.


SIDE EFFECTS
  None.
=====================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_is_service_available_sync (
  mmgsdi_session_id_type                session_id,
  mmgsdi_service_enum_type              srvc_type,
  boolean                               *srvc_available_ptr
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_FILE_ENUM_FROM_PATH_SYNC
================================================================*/
/**
  This function, on providing the complete path, returns the MMGSDI file
  enum.

  This function uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the function.

@param session_id           [IN]  Session Id of the caller
@param path_type            [IN]  Path information for which MMGSDI file enum
                                  to be determined
@param mmgsdi_file_enum_ptr [OUT] Pointer to MMGSDI file enum type, result is
                                  stored in this pointer

@retval MMGSDI_SUCCESS  The request was successfully handled
@retval MMGSDI_ERROR    The parameters supplied to the API are not
                         within appropriate ranges

@dependencies
A valid Session ID for a provisioning application is required.
=====================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_file_enum_from_path_sync (
  mmgsdi_session_id_type                session_id,
  mmgsdi_path_type                      path_type,
  mmgsdi_file_enum_type                *mmgsdi_file_enum_ptr
);

#endif /* MMGSDISESSIONLIB_V_H */
