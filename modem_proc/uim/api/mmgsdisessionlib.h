#ifndef MMGSDISESSIONLIB_H
#define MMGSDISESSIONLIB_H

/**
@file mmgsdisessionlib.h
@brief This library contains functions in the Multimode Generic SIM Driver
Interface (MMGSDI) with session enhancements.

This interface allows clients to request services, receive events, and use
provisioning information from multiple Universal Integrated Circuit Card (UICC)
applications on a smart card, including CDMA SIM and UMTS SIM provisioning
applications.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The mmgsdi_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the mmgsdi_mainpage.dox file or
      contact Tech Pubs.

      The above description for this file is part of the "session_library" group
      description in the mmgsdi_mainpage.dox file.
===========================================================================*/

/*================================================================================

  This file contains Library function that MMGSDI client can call to access

                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/mmgsdisessionlib.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
11/25/15   sp      Updated copyright
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
02/25/14   yt      API to support SELECT AID request with select mode
10/17/13   at      Support for MMGSDI Cancel APIs
08/07/13   tl      Add CDMA AKA Authenticate functionality
06/25/13   yt      New AUTH APIs with option to ignore app state
04/15/13   tl      Remove obsoleted featurization
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/07/13   yt      Support for activation/deactivation of EFs
01/20/13   vv      Added support for updating EF-ACSGL record
09/25/12   av      Define new API mmgsdi_get_file_attr_ext
09/11/12   bcho    Added prototype for Session Get Operator Name Extentsion API
08/16/12   at      Added new API mmgsdi_session_register_all_for_refresh
08/16/12   tl      Add support to close channel without app termination
08/10/12   av      Removed RPC support along with RPC style comments
05/23/12   vv      Added support for file update notification registration
03/01/12   kk      Added support for app state extraction in get_session_info
11/21/11   bcho    Description of Get Operator Name API converted into
                   doxygen format
11/02/11   bcho    Added support for Get Operator Name API
06/15/11   shr     Added support for Session Open extentsion API
05/17/11   yt      Support for INCREASE command
05/05/11   shr     Add mutex protection for global accesses in
                   GSDI/MMGSDI APIs
04/07/11   nmb     Support for ISIM GBA authentication
03/07/11   js      Support for exporting illegal subscription
09/23/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
08/13/10   js      Exported prototype of mmgsdi_session_get_info_sync() via RPC
08/04/10   js      Updated nvruim get data to support 3gpd control
07/03/10   js      Updated metacomment for get_nvruim_data_sync API
06/28/10   nmb     Updates for JCDMA CSIM support
06/15/10   js      Support for get_nvruim_data_sync API
05/20/10   shr     Updated RPC meta comments
03/29/10   shr     Updated RPC meta comments
03/19/10   shr     Switch/deactivate provisioning clean up and redesign
02/19/10   shr     Session get info API implementation
02/01/10   jk      EPRL Support
01/25/10   vs      Updates for header decomposition
12/23/09   shr     Fixed Perso Lock Down handling
12/14/09   shr     Fixed Perso handling and APIs
10/23/09   nb      CMI changes
10/15/09   ssr     Add support for OTASP request
08/13/09   kk      Added support for session based get app capabilities API
08/13/09   kk      Added support for session based GET CPHS INFO API
08/13/09   jk      Support for Illegal SIM, Get App Capabilities, OTASP APIs
05/12/09   yz      Initial Version


=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdilib_common.h"


/** @addtogroup session_library
@{ */

/*=============================================================================

                           DEFINES DECLARATIONS

=============================================================================*/

/*=============================================================================

                        ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*----------------------------------------------------------------------------
ENUM            MMGSDI_SESSION_INFO_ENUM_TYPE */
/** Indication of a query key.
*/
typedef enum {
    MMGSDI_SESSION_INFO_QUERY_BY_ID,   /**< Query by session ID. */
    MMGSDI_SESSION_INFO_QUERY_BY_TYPE, /**< Query by session type. */
    MMGSDI_MAX_SESSION_INFO_QUERY_ENUM = 0x7FFFFFFF /**< Maximum value of the
                                                         enumeration type. */
} mmgsdi_session_info_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_NVRUIM_DATA_REQ_ENUM_TYPE */
/** NVRUIM global data request.
*/
typedef enum {
  MMGSDI_NVRUIM_GET_ESN_USAGE_REQ                   = 0x00,
    /**< Get the ESN usage request. */
  MMGSDI_NVRUIM_GET_HRPD_CONTROL_REQ                = 0x01,
    /**< Get the HRPD control request. */
  MMGSDI_NVRUIM_GET_3GPD_EXT_SUPPORT_REQ            = 0x02,
    /**< Get the 3GPD extension support request. */
  MMGSDI_NVRUIM_GET_3GPD_CTRL_AND_ESN_USG_DATA_REQ  = 0x03,
    /**< Get the 3GPD control and ESN usage data request. */
  MMGSDI_NVRUIM_REQ_MAX_VALUE                       = 0x7FFFFFFF
    /**< Maximum value of the enumeration type. */
}mmgsdi_nvruim_data_req_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_NVRUIM_ESN_USAGE_ENUM_TYPE   */
/** ESN usage indicator value.
*/
typedef enum {
  MMGSDI_NVRUIM_USE_ESN = 0,           /**< Use the ESN as the ESN. */
  MMGSDI_NVRUIM_USE_RUIM_ID = 1,       /**< Use the RUIM ID as the ESN. */
  MMGSDI_NVRUIM_ZERO_ESN = 2,          /**< Use zero as the ESN. */
  MMGSDI_NVRUIM_USE_MEID = 3,          /**< Use the MEID as the MEID. */
  MMGSDI_NVRUIM_USE_SF_EUIMID = 4,     /**< Use the Short Form RUIM ID as the
                                            MEID. */
  MMGSDI_NVRUIM_USE_SF_EUIMID_ESN = 5, /**< Use the Short Form EUIMID for
                                            identification, and use the ESN for
                                            authentication. */
  MMGSDI_NVRUIM_USE_MEID_UIMID = 6     /**< Use the MEID for identification, and
                                            use the UIMID for authentication. */
} mmgsdi_nvruim_esn_usage_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_NVRUIM_AN_HRPD_SUPPORT_STATUS_ENUM_TYPE  */
/** Types of return values for an HRPD control check.
*/
typedef enum {
  MMGSDI_NVRUIM_AN_HRPD_SUPPORT     = 0, /**< HRPD is supported on the card. */
  MMGSDI_NVRUIM_AN_HRPD_NO_SUPPORT  = 1, /**< HRPD is not supported on the card. */
  MMGSDI_NVRUIM_AN_HRPD_USE_CAVE    = 2  /**< HRPD is not supported on the card,
                                              and CAVE authentication is used. */
} mmgsdi_nvruim_an_hrpd_support_status_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_NVRUIM_3GPD_SUPPORT_STATUS */
/** Types of return values for a 3GPD control check from 3GPD data service.
*/
typedef enum {
  MMGSDI_NVRUIM_3GPD_MIP_RUIM_SIP_RUIM = 0, /**< Card has both SIP and MIP
                                                 support. */
  MMGSDI_NVRUIM_3GPD_MIP_NV_SIP_NV     = 1, /**< Card does not have MIP and SIM
                                                 capability. */
  MMGSDI_NVRUIM_3GPD_MIP_RUIM_SIP_NV   = 2, /**< Card has MIP capablity only. */
  MMGSDI_NVRUIM_3GPD_MIP_NV_SIP_RUIM   = 3  /**< Card has SIP capabilty only. */
} mmgsdi_nvruim_3gpd_support_status;

/*=============================================================================
   ENUM  :   MMGSDI_CDMA_AKA_CMD_ENUM_TYPE
=============================================================================*/
/** Identifies the CDMA AKA commands
*/
typedef enum {
  MMGSDI_CDMA_AKA_AUTH_3G,
  MMGSDI_CDMA_AKA_AUTH_WLAN
} mmgsdi_cdma_aka_cmd_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SESSION_APP_LEGAL_STATUS_TYPE */
/** Types of status for a subscription as determined by the network
*/
typedef enum {
  MMGSDI_SESSION_APP_IS_LEGAL    = 0x00, /**< Mark APP state as READY */
  MMGSDI_SESSION_APP_IS_ILLEGAL  = 0x01, /**< Mark APP state as ILLEGAL */
} mmgsdi_session_app_legal_status_type;

/** @} */ /* end_addtogroup session_library */
/** @addtogroup session_library
@{ */

/*=============================================================================

                      ABSTRACT DATA TYPE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CDMA_AKA_RANDA_TYPE
-----------------------------------------------------------------------------*/
/** Random anonymity key type for a AKA Authentication.
*/
typedef uint8 mmgsdi_cdma_aka_randa_type[MMGSDI_CDMA_AKA_RANDA_LEN];

/*----------------------------------------------------------------------------
  STRUCTURE:   MMGSDI_NVRUIM_3GPD_CTRL_AND_ESN_USG_DATA_TYPE
  -----------------------------------------------------------------------------*/
/** @brief Defines NVRUIM 3GPD control and ESN usage data to be requested by
    clients.
*/
typedef struct {
  boolean                             mmgsdi_nvruim_3gpd_sip_svc;
    /**< Indicates where SIP service is enabled in the CDMA service table file. */
  boolean                             mmgsdi_nvruim_3gpd_mip_svc;
    /**< Indicates where MIP service is enabled in the CDMA service table file. */
  mmgsdi_nvruim_3gpd_support_status   mmgsdi_nvruim_3gpd_control_data;
    /**< Indicates where clients are to use UIM for reading 3GPD support data. */
  mmgsdi_nvruim_esn_usage_enum_type   esn_usage_type;
    /**< Indicates the current ESN usage type. */
}mmgsdi_nvruim_3gpd_ctrl_and_esn_usg_data_type;

/*----------------------------------------------------------------------------
STRUCTURE       MMGSDI_NON_PROV_APP_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Contains all the information for an application associated with a
    nonprovisioning session.
*/
typedef struct {
  mmgsdi_data_type       app_id_data;       /**< Application identifier. */
  boolean                exclusive_channel; /**< Indicates whether other session
                                                 IDs can be mapped to the same
                                                 channel. */
} mmgsdi_non_prov_app_info_type;

/*----------------------------------------------------------------------------
STRUCTURE       MMGSDI_SESSION_INFO_QUERY_TYPE
-----------------------------------------------------------------------------*/
/** @brief Contains the query key information that a client uses to query the
    MMGSDI for session information.
*/
typedef struct {
  mmgsdi_session_info_enum_type query_type ; /**< Indicates how the union
                                             session ID and session type are
                                             interpreted. */

  /** Used by the mmgsdi_session_info_query_type structure to pick a type for a
      query: session ID or session type. */
  union {
    mmgsdi_session_id_type        session_id;   /**< Session ID that a client
                                                     uses in the query. */
    mmgsdi_session_type_enum_type session_type; /**< Session type that a client
                                                     uses in the query. */
  } query_key;
} mmgsdi_session_info_query_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NVRUIM_DATA_TYPE
-------------------------------------------------------------------------------*/
/** @brief Used when returning the NVRUIM global data.
*/
typedef struct {
  mmgsdi_nvruim_data_req_enum_type                   req_type;
    /**< NVRUIM data request type. */

  /** Used by the mmgsdi_nvruim_data_type structure for the data of the NVRUIM
      request. */
  union
  {
    mmgsdi_nvruim_esn_usage_enum_type                esn_usage;
      /**< ESN data request. */
    mmgsdi_nvruim_an_hrpd_support_status_enum_type   an_hrpd_status;
      /**< HRPD data request. */
    boolean                                          nvruim_3gpd_ext_support;
      /**< 3GPD extentions data request. */
    mmgsdi_nvruim_3gpd_ctrl_and_esn_usg_data_type    nvruim_3gpd_ctrl_and_esn_usg;
      /**< 3GPD control and ESN usage data request. */
  } data;
} mmgsdi_nvruim_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_INFO_TYPE
-------------------------------------------------------------------------------*/
/** @brief Used when returning session-related information.
*/
typedef struct {
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_slot_id_enum_type       slot_id;    /**< Slot on which the request is to
                                                  be performed. */
  int32                          channel_id; /**< Logical channel used by the
                                                  application. */
  mmgsdi_aid_type                app_data;   /**< Application identifier. */
  mmgsdi_app_state_enum_type     app_state; /**< Application status */
} mmgsdi_session_get_info_type;

/** @} */ /* end_addtogroup session_library */
/** @addtogroup session_library
@{ */

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OPEN
===============================================================================*/
/**
  Opens a session to an application on the card.

  The session points to the GSM/WCDMA (GW) or 1X provisioning application, or to
  a particular application if an Application Identifier (AID) is specified instead.

  If a provisioning application is indicated and the provisioning application
  is not yet established, the session is automatically mapped to the
  provisioning application when it is established. Using this application also
  provides the client with all application-related events.

  If this is an ICC card, a session must still be opened, but all sessions are
  internally mapped to the default channel.

  If a 1X provisioning application is specified and the card does not contain a
  CSIM application or CDMA Directory File (DF), or if a GW provisioning
  application is specified but the card does not contain a USIM application or
  GSM DF, the session still succeeds, but no initialization completed event is
  sent for this application. Subsequent requests with this session ID may result
  in failures depending on the commands being sent.

  Upon successful completion of this command, the session has an associated
  logical channel and an associated UICC application ID. This session can be
  MUXED on the same channel as other sessions pointing to the same application.

  If the session is of type MMGSDI_NON_PROV_SESSION_SLOT_1 or
  MMGSDI_NON_PROV_SESSION_SLOT_2, the non_prov_app_data_ptr pointer must point
  to a valid application ID. Otherwise, the pointer must be NULL.

  @param[in] client_id              Client ID of the caller.
  @param[in] session_type           GW, 1X, or nonprovisioning application.
  @param[in] non_prov_app_data_ptr  Pointer to the application ID of a
                                    nonprovisioning application.
  @param[in] evt_cb_ptr       Application event callback. A client must provide
                              an event callback if it is to receive
                              session-related event notifications. If the
                              pointer is NULL, the client receives no event
                              notifications.
  @param[in] set_notify       Indicates whether the client is to call
                              mmgsdi_session_prov_app_init_complete() when
                              the client finishes initialization procedures
                              for the applications. This parameter has
                              no meaning if evt_cb_ptr is a NULL pointer.
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
  If the set_notify parameter is not set to FALSE for nonprovisioning sessions,
  the MMGSDI returns an error.

  @sideeffects
  The event callback for the session of interest is called every time the MMGSDI
  notifies the client on this session about all session-related events.
*/
mmgsdi_return_enum_type  mmgsdi_session_open (
  mmgsdi_client_id_type                 client_id,
  mmgsdi_session_type_enum_type         session_type,
  const mmgsdi_non_prov_app_info_type * non_prov_app_data_ptr,
  mmgsdi_evt_callback_type              evt_cb_ptr,
  boolean                               set_notify,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OPEN_EXT
===============================================================================*/
/**
  This function allows the client to open multiple sessions to
  applications on the card.

  The session can point to the GW/1X PROVISIONING Applications or to Card
  Sessions. Non-provisioning applications are not supported. If a provisioning
  application is indicated, and the provisioning application is not yet
  established, it will automatically be mapped to the provisioning application
  when it is established. Using this application also provides the client
  with all application-related events.

  If this is an ICC card, a session must still be opened, but all
  sessions will internally be mapped to the default channel.

  If a 1X Provisioning app is specified, and the card does not contain
  an CSIM App or CDMA DF, or if a GW Provisioning App is specified, but
  the card does not contain a USIM Application or GSM DF, the session
  will still succeed, but no init completed event will ever be sent for
  this application. Subsequent request with this Session ID may yield
  failures depending on the commands being sent.

  @param[in] client_id         Client ID of the caller.
  @param[in] session_type_mask Holds a mask of all sessions requested.
                               (GW prov. and/or 1X prov. and/or Card session)
                               session type will be determined by the bits set.
  @param[in] evt_cb_ptr        Event Callback.  If NULL, client will receive
               no session related events (not recommended).
  @param[in] set_notify_mask   Mask indicating whether clients should
                               call mmgsdi_session_prov_app_init_complete()
                               when they finish their initialization
                               procedure for the applications. This is only
                               applicable to the provisioning-session types.
                               No meaning if evt_cb_ptr is a NULL pointer.
  @param[in] response_cb_ptr   Command callback.
  @param[in] client_ref        User data.

  @dependencies
  The client must have a valid client ID.

  @limitations
  If the set_notify parameter is not set to FALSE for nonprovisioning sessions,
  the MMGSDI returns an error.

  @sideeffects
  The event callback for the sessions of interest is called every time the
  MMGSDI notifies the client on these session about all session-related events.
*/
mmgsdi_return_enum_type  mmgsdi_session_open_ext (
  mmgsdi_client_id_type                 client_id,
  uint32                                session_type_mask,
  mmgsdi_evt_callback_type              evt_cb_ptr,
  uint32                                set_notify_mask,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/* ============================================================================
FUNCTION:       MMGSDI_SESSION_CLOSE
=============================================================================== */
/**
  Closes the currently active session associated with the specified session ID.

  Upon successful completion of this function, the MMGSDI deallocates the
  session ID and breaks the link in its internal session table.

  If this function applies to the last session ID for the active UICC
  application, the MMGSDI cleans up and performs the application deactivation
  procedure. The logical channel associated with the session ID is released,
  with the exception of logical channel 0.

  @param[in] session_id       Session ID,
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
  The client must have a valid client ID, and a valid Session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_close (
  mmgsdi_session_id_type       session_id,
  mmgsdi_callback_type         response_cb_ptr,
  mmgsdi_client_data_type      client_ref
);

/* ============================================================================
FUNCTION        MMGSDI_SESSION_CLOSE_TERMINATE

DESCRIPTION
  This function is used to close the currently active session associated with
  the Session ID specified.

  Upon successful completion of this function, MMGSDI deallocates the
  Session ID and breaks the link in its internal sesssion table.

  If this close session applies to the last session ID for the active UICC
  application and the client allows for applications to terminate, MMGSDI
  will clean up as above and, if requested by the client for non-provisioning
  applications, perform the application deactivation procedure. Also, note
  that, the logical channel associated with the Session ID will be released
  with the exception of logical channel 0.

  When the terminate parameter is TRUE, this function will perform a normal
  session close where the application is terminated before the channel is
  closed. When this parameter is FALSE, this function will skip the
  application termination and perform only the channel close.

PARAMETERS
  mmgsdi_session_id_type   session_id:      Session ID
  boolean                  terminate:       TRUE: perform terminate SELECT
                                            FALSE: skip terminate SELECT
                                            Does not affect the following
                                            manage channel
  mmgsdi_callback_type     response_cb_ptr: Response callback
  mmgsdi_client_data_type  client_ref:      User Data returned upon
                                            completion of this cmd.

DEPENDENCIES
  The client must have a valid client ID, and the Session ID specified must
  be valid for deallocation.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
SIDE EFFECTS

=============================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_close_terminate (
  mmgsdi_session_id_type       session_id,
  boolean                      terminate,
  mmgsdi_callback_type         response_cb_ptr,
  mmgsdi_client_data_type      client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING
===============================================================================*/
/**
  Establishes a SIM, USIM, RUIM, or CSIM provisioning application on a session
  that the client has opened. This function can also be used to change the
  current provisioning application.

  Upon successful completion of this command for a UICC card, the opened session
  is associated with the USIM or CSIM AID specified by the slot number. The
  associated application is selected on a specific logical channel mapped to
  the session ID.

  Upon successful completion of this command for an ICC card, the opened session
  is associated with the GSM DF or CDMA DF specified by the slot number. The
  associated application is selected on the default logical channel mapped to
  the session ID.

  For any other clients that have opened a 1X or GSM/WCDMA (GW) provisioning
  application, their sessions are automatically mapped to the same logical
  channel.

  If this function is used to change the current 1X or GW provisioning
  application, the new application is opened on either the same or a different
  logical channel and is mapped to this session.

  All clients registered to receive session events for this provisioning
  application (1X or GW) receive MMGSDI_SESSION_CHANGED_EVT (activated = FALSE)
  as the first notification followed by MMGSDI_SESSION_CHANGED_EVT
  (activated = TRUE) as the next notification.

  @param[in] session_id       Session ID.
  @param[in] card_slot        Physical slot for multiple slot targets.
  @param[in] app_id_data      Application ID.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_activate_or_switch_provisioning (
  mmgsdi_session_id_type                session_id,
  mmgsdi_slot_id_enum_type              card_slot,
  mmgsdi_data_type                      app_id_data,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_DEACTIVATE_PROVISIONING
===============================================================================*/
/**
  Deactivates a SIM, USIM, RUIM, or CSIM application from being the
  provisioning application.

  For a UICC card, the existing session IDs for this provisioning application
  are no longer bound to any application on the card. MMGSDI performs the
  deactivation procedure for the provisioning application, and the logical
  channel is closed with the exception of channel 0.

  For an ICC card, the existing session IDs for this provisioning application
  are no longer bound to any application on the card.

  All clients registered to receive session events for this provisioning
  application (1X or GSM/WCDMA) receive MMGSDI_SESSION_CHANGED_EVT
  (activated = FALSE) as the notification.

  @param[in] session_id       Session ID.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_deactivate_provisioning (
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*=========================================================================
FUNCTION:       MMGSDI_SESSION_GET_INFO
=======================================================================*/
/**
  Gets session information by session ID or session type. A client can get the
  session type, slot number, channel ID, and AID.

  @param[in] session_info_query  Query type and key.
  @param[in] response_cb_ptr     Pointer to the response callback.
  @param[in] client_ref          User data returned upon completion of this
                                 command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_info (
  mmgsdi_session_info_query_type    session_info_query,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*=========================================================================
FUNCTION:       MMGSDI_SESSION_PROV_APP_INIT_COMPLETE
=======================================================================*/
/**
  Notifies the MMGSDI that the client has finished its initialization procedure
  for the application.

  @param[in] session_id       Session ID.
  @param[in] init_status      Indicates whether the initialization succeeded.
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
  A valid session ID is required.\n
  A client can call this function only if the client sets the set_notify flag
  to TRUE in mmgsdi_session_open().

  @limitations
  A client can call this function only when it opens a provisioning session.
*/
mmgsdi_return_enum_type  mmgsdi_session_prov_app_init_complete (
  mmgsdi_session_id_type            session_id,
  boolean                           init_status,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_GET_ALL_PIN_STATUS
===========================================================================*/
/**
  Gets the PIN status of all relevant PINs applicable to the session ID
  provided.

  @param[in] session_id       Session ID of the caller
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
  A valid session ID is required.

  @limitations
  The MMGSDI is responsible for getting the PIN status for provisioning
  applications only.\n
  Clients are independently responsible for getting the PIN status for
  nonprovisioning applications.
*/
mmgsdi_return_enum_type mmgsdi_session_get_all_pin_status (
  mmgsdi_session_id_type            session_id,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_VERIFY_PIN
===========================================================================*/
/**
  Performs a PIN verification procedure against the card for a UPIN, PIN1, or
  PIN2. The PIN provided must be applicable to the session ID provided.

  A failure to verify a PIN may result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

  @param[in] session_id       Session ID of the caller.
  @param[in] pin_id           PIN ID to be verified.
  @param[in] pin_data         Length and code of the PIN.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_verify_pin (
  mmgsdi_session_id_type            session_id,
  mmgsdi_pin_enum_type              pin_id,
  mmgsdi_data_type                  pin_data,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_DISABLE_PIN
===========================================================================*/
/**
  Disables a PIN and indicates whether it is to be replaced by the UPIN. The
  PIN provided must be applicable to the session ID provided.

  A failure to disable the PIN may result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

  @param[in] session_id       Session ID of the caller.
  @param[in] pin_id           PIN ID to be verified.
  @param[in] replace_pin      Replacement option.
  @param[in] pin_data         Length and code of the PIN.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_disable_pin (
  mmgsdi_session_id_type            session_id,
  mmgsdi_pin_enum_type              pin_id,
  mmgsdi_pin_replace_enum_type      replace_pin,
  mmgsdi_data_type                  pin_data,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_ENABLE_PIN
===========================================================================*/
/**
  Enables a PIN. The PIN provided must be applicable to the session ID
  provided.

  A failure to enable the PIN may result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

  @param[in] session_id       Session ID of the caller.
  @param[in] pin_id           PIN ID to be verified.
  @param[in] pin_data         Length and code of the PIN.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_enable_pin (
  mmgsdi_session_id_type             session_id,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_CHANGE_PIN
===================================================================*/
/**
  Changes the PIN (UPIN, PIN1, or PIN2). The PIN provided must be applicable
  to the session ID provided.\n

  A failure to change the PIN may result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

  @param[in] session_id        Session ID of the caller.
  @param[in] pin_id            PIN ID to be verified.
  @param[in] pin_data          Length and code of the current PIN.
  @param[in] new_pin_ptr_data  Length and code of the new PIN.
  @param[in] response_cb_ptr   Pointer to the response callback.
  @param[in] client_ref        User data returned upon completion of this
                               command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_change_pin(
  mmgsdi_session_id_type        session_id,
  mmgsdi_pin_enum_type          pin_id,
  mmgsdi_data_type              pin_data,
  mmgsdi_data_type              new_pin_ptr_data,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_UNBLOCK_PIN
===========================================================================*/
/**
  Unblocks the PIN. The PIN provided must be applicable to the session ID
  provided.

  A failure to unblock the PIN may result in a permanently blocked PIN ID
  if the maximum number of attempts to unblock are exhausted.

  @param[in] session_id        Session ID of the caller.
  @param[in] pin_id            PIN ID to be verified.
  @param[in] puk_data          Length and code of the PUK.
  @param[in] new_pin_ptr_data  Length and code of the new PIN.
  @param[in] response_cb_ptr   Pointer to the response callback.
  @param[in] client_ref        User data returned upon completion of this
                               command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_unblock_pin(
  mmgsdi_session_id_type             session_id,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   puk_data,
  mmgsdi_data_type                   new_pin_ptr_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ========================================================================
FUNCTION:       MMGSDI_SESSION_READ_RECORD
=========================================================================== */
/**
  Provides access (by file enumeration or path) to any record file in the SIM,
  USIM, RUIM, or CSIM.

  This function encapsulates the details required to read a file from the SIM,
  USIM, RUIM, or CSIM card. The client using this function can access only the
  files that require a record number.

  The function behaves as follows for these file types:
   - Transparent file  -- Returns an error.
   - Cyclic file       -- Accesses the record in the file provided in the
                          function call.
   - Linear fixed file -- Accesses the record in the file provided in the
                          function call.

  The function behaves as follows for these access types:
   - MMGSDI_EF_ENUM_ACCESS -- Returns the contents as required in the
                              function call.
   - MMGSDI_BY_PATH_ACCESS -- Returns the contents as required in the
                              function call. There is no check whether the
                              path is a record file. The caller is expected to
                              know whether the file to access is a record.
   - Others -- Returns an error.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File to read.
  @param[in] record_number    Record number, which must be equal to or
                              greater than 1.
  @param[in] request_length   Request length.
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
  A valid session ID is required.

  @limitations
  Returns an error when used to access a transparent file.
*/
mmgsdi_return_enum_type  mmgsdi_session_read_record (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ===========================================================================
FUNCTION:       MMGSDI_SESSION_READ_TRANSPARENT
========================================================================== */
/**
  Provides read access (by file enumeration or path) to any transparent file in
  the SIM, USIM, RUIM, or CSIM.

  This function encapsulates the details required to read a file from the SIM,
  USIM, RUIM, or CSIM card. The client using this function can access only
  the files that have a flat data structure.

  The function behaves as follows for these file types:
   - Transparent file  -- Returns the contents as required in the function call.
   - Cycle file        -- Returns an error.
   - Linear fixed file -- Returns an error.

  The function behaves as follows for these access types:
   - MMGSDI_EF_ENUM_ACCESS -- Returns the contents as required in the
                              function call.
   - MMGSDI_BY_PATH_ACCESS -- Returns the contents as required in the function
                              call. There is no check whether the path passed
                              in is a transparent file or not. The caller is
                              expected to know whether the file to access is
                              transparent.
   - Others -- Returns an error.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File to read.
  @param[in] offset           Offset from the start of the file.
  @param[in] req_len          Request length.
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
  A valid session ID is required.

  @limitations
  Returns an error when used to access a cyclic file or linear fixed file.
*/
mmgsdi_return_enum_type  mmgsdi_session_read_transparent (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_WRITE_RECORD
========================================================================== */
/**
  Provides write access (by file enumeration or path) to any record in a linear
  fixed file or cyclic file.

  This function encapsulates the details required to write a file to the SIM,
  USIM, RUIM, or CSIM card. The client using this function can access only the
  files that require a record number.

  The function behaves as follows for these file types:
   - Transparent file  -- Returns an error.
   - Cyclic file       -- Writes to the record in the file provided in the
                          function call.
   - Linear fixed file -- Writes to the record in the file provided in the
                          function call.

  The function behaves as follows for these access types:
   - MMGSDI_EF_ENUM_ACCESS -- Writes to the record in the file provided in the
                              function call.
   - MMGSDI_BY_PATH_ACCESS -- Writes to the record in the file provided in the
                              function call. There is no check whether the path
                              passed in is a record file or not. The caller is
                              expected to know whether the file to access is a
                              record.
   - Others -- Returns an error.

  The function behaves as follows for these record types:
   - MMGSDI_LINEAR_FIXED_FILE -- Uses ABSOLUTE access to the card.
   - MMGSDI_CYCLIC_FILE       -- Uses PREVIOUS access to the card,
   - MMGSDI_TRANSPARENT_FILE  -- Returns an error.
   - MMGSDI_MASTER_FILE       -- Returns an error.
   - MMGSDI_DEDICATED_FILE    -- Returns an error.
   - MMGSDI_MAX_FILE_STRUCTURE_ENUM -- Returns an error.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File to write.
  @param[in] record_type      Record type.
  @param[in] record_number    Record number, which must be equal to or
                              greater than 1.
  @param[in] write_data       Data to be written.
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
  A valid session ID is required.

  @limitations
  Returns an error when used to access a transparent file.
*/
mmgsdi_return_enum_type  mmgsdi_session_write_record (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_file_structure_enum_type    record_type,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_WRITE_TRANSPARENT
========================================================================== */
/**
  Provides write access (by file enumeration or path) to any transparent file.

  This function encapsulates the details required to write a file to the SIM,
  USIM, RUIM, or CSIM card. The client using this function can access only the
  files that have a flat data structure.

  The function behaves as follows for these file types:
   - Transparent file  -- Writes the data provided in the function call.
   - Cyclic file       -- Returns an error.
   - Linear fixed file -- Returns an error.

  The function behaves as follows for these access types:
   - MMGSDI_EF_ENUM_ACCESS -- Writes the data provided in the function call.
   - MMGSDI_BY_PATH_ACCESS -- Writes the data provided in the function call.
                              There is no check whether the path passed in
                              is a transparent file or not. The caller is
                              expected to know whether the file to access is
                              transparent.
   - Others -- Returns an error.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File to write.
  @param[in] offset           Offset from the start of the file.
  @param[in] write_data       Data to be written.
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
  A valid session ID is required.

  @limitations
  Returns an error when used to access a record-based file, such as a  cyclic
  file or a linear fixed file.

  @sideeffects
  MMGSDI cache is updated if the requested data is located in the cache.
*/
mmgsdi_return_enum_type  mmgsdi_session_write_transparent (
  mmgsdi_session_id_type           session_id,
  mmgsdi_access_type               file_access,
  mmgsdi_offset_type               offset,
  mmgsdi_write_data_type           write_data,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_client_data_type          client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_GET_FILE_ATTR
==========================================================================*/
/**
  Gets file attributes from any file in the SIM, USIM, RUIM, or CSIM card.

  The function behaves as follows for these file types:
   - Transparent file  -- Gets information on the characteristics of a
                          transparent file.
   - Cyclic file       -- Gets information on the characteristics of a
                          cyclic file.
   - Linear fixed file -- Gets information on the characteristics of a linear
                          fixed file.
   - Master file       -- Gets information on the master file.
   - Dedicated file    -- Gets information on the dedicated file.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_name        File name.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_file_attr (
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_name,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_GET_FILE_ATTR_EXT
==========================================================================*/
/**
  Gets file attributes from any file in the SIM, USIM, RUIM, or CSIM card.
  The way it is different from mmgsdi_session_get_file_attr() is that the
  mmgsdi_session_get_file_attr_ext() lets the caller specify whether they
  want to skip the EF-ARR check for the file. If client chooses to do so,
  it will receive ALWAYS as the security attribute for the file in the cnf.
  This API does not achieve anything different from
  mmgsdi_session_get_file_attr() for the UICC cards for which all the
  accessibility information is in the FCP (instead of a reference to EF-ARR).

  The skip_uicc_arr parameter is applicable only to UICC. For ICC, it is
  ignored by the implementation.

  The function behaves as follows for these file types:
   - Transparent file  -- Gets information on the characteristics of a
                          transparent file.
   - Cyclic file       -- Gets information on the characteristics of a
                          cyclic file.
   - Linear fixed file -- Gets information on the characteristics of a linear
                          fixed file.
   - Master file       -- Gets information on the master file.
   - Dedicated file    -- Gets information on the dedicated file.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_name        File name.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.
  @param[in] skip_uicc_arr    Skip reading EF-ARR for accessibility attributes
                              of the file.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_file_attr_ext (
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_name,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref,
  boolean                           skip_uicc_arr
);

/* ============================================================================
FUNCTION:       MMGSDI_SESSION_ISIM_AUTHENTICATE
========================================================================== */
/**
  Performs a Network Initiated Authentication with an ISIM application.

  This operation is valid and applicable only when an ISIM application is
  available, as specified in 3GPP TS 31.103. The function checks for the ISIM
  application before sending the command.

  @param[in] session_id       Session ID of the caller.
  @param[in] auth_req         Length and data of the authentication challenge.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_isim_authenticate (
  mmgsdi_session_id_type   session_id,
  mmgsdi_data_type         auth_req,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/* ============================================================================
FUNCTION:       MMGSDI_SESSION_UICC_AUTHENTICATE
========================================================================== */
/**
  Performs a Network Initiated Authentication with any UICC application.

  @param[in] session_id       Session ID of the caller.
  @param[in] auth_req         Length and data of the authentication challenge.
  @param[in] auth_context     Security context to use for authentication
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_uicc_authenticate (
  mmgsdi_session_id_type        session_id,
  mmgsdi_autn_context_enum_type auth_context,
  mmgsdi_data_type              auth_req,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_USIM_AUTHENTICATE
===========================================================================*/
/**
  Performs a Network Initiated Authentication with a USIM application.

  This operation is valid and applicable only when a USIM application is
  available. The function checks for the USIM application before sending
  the command.

  @param[in] session_id       Session ID of the caller.
  @param[in] auth_context     Context of the authentication challenge.
  @param[in] auth_req         Length and data of the authentication
                              challenge.
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
  If the USIM application is not the default application, the client must have
  a valid session open.
*/
mmgsdi_return_enum_type  mmgsdi_session_usim_authenticate (
  mmgsdi_session_id_type              session_id,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_COMPUTE_IP_AUTH
===========================================================================*/
/**
  Computes the IP authentication for 3GPD Simple IP CHAP, Mobile IP, or HRPD
  CHAP Authentication.

  @param[in] session_id       Session ID of the caller.
  @param[in] compute_ip_data  Data required for Compute IP Command.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_compute_ip_auth (
  mmgsdi_session_id_type             session_id,
  mmgsdi_compute_ip_data_type        compute_ip_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_RUN_CAVE_ALGO
===========================================================================*/
/**
  Performs a Cellular Authentication and Voice Encryption (CAVE) operation in
  the UIM.

  The Run CAVE command is used to generate authentication responses and to
  enable the calculation of ciphering keys.

  @param[in] session_id       Session ID of the caller.
  @param[in] rand_type        Type of random challenge, either 0000 0000 RAND
                              (global random challenge) or 0000 0001 RANDU
                              (unique random challenge).
  @param[in] rand_chal        Random challenge for the CAVE algorithm.
  @param[in] dig_len          Length of digits provided.
  @param[in] digits           Digits for the CAVE calculation.
  @param[in] process_control  Process control for the CAVE command, including
                              which command triggers saving a new SSD value
                              and whether to freeze internal registers.
  @param[in] esn              ESN for the CAVE calculation.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_run_cave_algo (
  mmgsdi_session_id_type             session_id,
  mmgsdi_cave_rand_type_type         rand_type,
  mmgsdi_cave_rand_chal_type         rand_chal,
  mmgsdi_cave_dig_len_type           dig_len,
  mmgsdi_cave_digits_type            digits,
  mmgsdi_cave_process_control_type   process_control,
  mmgsdi_cave_esn_type               esn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION        MMGSDI_SESSION_CDMA_AKA_AUTHENTICATE
DESCRIPTION
  This function will build a request to the MMGSDI Task to perform 3GPP2
  authentication and key agreement commands

PARAMETERS
  session_id:        Session ID of the caller
  aka_cmd:           Specifies the AKA command
  randa:             Random Anonymity Key
  autn:              Authentication Token
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_cdma_aka_authenticate(
  mmgsdi_session_id_type             session_id,
  mmgsdi_cdma_aka_cmd_enum_type      aka_cmd,
  mmgsdi_cdma_aka_randa_type         randa,
  mmgsdi_data_type                   autn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_GENERATE_KEY_VPM
===========================================================================*/
/**
  Performs a Generate Key command in the UIM.

  @param[in] session_id       Session ID of the caller.
  @param[in] first_octet      First octet of the VPM to be output.
  @param[in] last_octet       Last octet of the VPM to be output.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.\n
  MMGSDI_AUTH_ERROR_CAVE_NOT_RUN -- Command cannot be called unless the Run CAVE
                                    command has already been run with SAVE
                                    Registers turned on.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_generate_key_vpm (
  mmgsdi_session_id_type             session_id,
  mmgsdi_single_vpm_octet_type       first_octet,
  mmgsdi_single_vpm_octet_type       last_octet,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_RUN_GSM_ALGO
===========================================================================*/
/**
  Performs a Network Initiated Authentication with the SIM card. This operation
  is valid and applicable only when a SIM card is available.

  @param[in] session_id       Session ID of the caller.
  @param[in] rand             Length and data of the RAND.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_run_gsm_algo (
  mmgsdi_session_id_type              session_id,
  mmgsdi_data_type                    rand,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/* ============================================================================
FUNCTION:      MMGSDI_SESSION_SEND_CARD_STATUS
========================================================================== */
/**
  Returns information about the current directory or current application.

  For GSM 11.11:
  - This function returns information about the current directory only.
  - A current EF is not affected by the status function.
  - The function also provides an opportunity for a pro-active SIM to indicate
  that the SIM is to issue a SIM Application Toolkit command to the ME.
  - The status and return type are not required when using a GSM SIM card.

  For UMTS 31.102:
  - This function returns information concerning the current
  directory or the current application.
  - Depending on the application specification, the function can also be used
  to indicate to the application in the UICC that its session activation
  procedure has been successfully executed or that its termination procedure
  will be executed. These indications can be used to synchronize the
  applications in the terminal and UICC.
  - A status and return type is required when using a UICC Card with a USIM
    application.

  @param[in] session_id       Session ID of the caller.
  @param[in] me_app_status    Status of the application in the handset.
  @param[in] ret_data_struct  Structure in which the data is to be returned.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_send_card_status(
  mmgsdi_session_id_type           session_id,
  mmgsdi_status_me_app_enum_type   me_app_status,
  mmgsdi_status_ret_data_enum_type ret_data_struct,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_client_data_type          client_ref
);

/* ============================================================================
FUNCTION:      MMGSDI_SESSION_ENABLE_SERVICE
========================================================================== */
/**
  Enables an Access Control List, Barred Dialing Number, or Fixed Dialing
  Number service.

  @param[in] session_id       Session ID of the caller.
  @param[in] service          Service type.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.\n
  MMGSDI_NOT_SUPPORTED    -- Service is not ACL, BDN, or FDN.

  @dependencies
  A valid session ID is required.

  @limitations
  This function enables only an ACL, BDN, or FDN service.\n
  For all other services, the MMGSDI returns an error.
*/
mmgsdi_return_enum_type mmgsdi_session_enable_service (
  mmgsdi_session_id_type             session_id,
  mmgsdi_service_enum_type           service,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ============================================================================
FUNCTION:       MMGSDI_SESSION_DISABLE_SERVICE
========================================================================== */
/**
  Disables an Access Control List, Barred Dialing Number, or Fixed Dialing
  Number service.

  @param[in] session_id       Session ID of the caller.
  @param[in] service          Service type.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.\n
  MMGSDI_NOT_SUPPORTED    -- Service is not ACL, BDN, or FDN.

  @dependencies
  None.

  @limitations
  This function disables only an ACL, BDN, or FDN service.\n
  For all other services, the MMGSDI returns an error.
*/
mmgsdi_return_enum_type mmgsdi_session_disable_service (
  mmgsdi_session_id_type             session_id,
  mmgsdi_service_enum_type           service,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_IS_SERVICE_AVAILABLE
===========================================================================*/
/**
  Determines if a service is available, given the service type.

  @param[in] session_id       Session ID of the caller.
  @param[in] srvc_type        Service in which the client is interested.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_is_service_available (
  mmgsdi_session_id_type              session_id,
  mmgsdi_service_enum_type            srvc_type,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_JCDMA_GET_CARD_INFO
===========================================================================*/
/**
  Sends a request to the MMGSDI task for JCDMA card information.

  This function is called from the OEM layer only during JCDMA phone power up
  after a Subscription Ready event.

  @param[in] session_id       Session ID of the caller.
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
  A valid session ID is required.\n
  Before calling this function, PIN1 verification must be performed and a
  Subscription Ready event must be received.
*/
mmgsdi_return_enum_type mmgsdi_session_jcdma_get_card_info (
  mmgsdi_session_id_type             session_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_REGISTER_FOR_REFRESH
=============================================================================*/
/**
  Registers a client for File Change Notifications (FCNs) for specific files,
  or for voting to refresh an application.

  When an FCN request is received, the MMGSDI sends an MMGSDI_REFRESH_EVT event
  with mode = MMGSDI_REFRESH_FCN and with the file list that has changed.
  The MMGSDI filters out the file list based on what the client registered for.

  Some FCNs can bring the protocol stack down. If the client has registered for
  these files, it must also register for voting explicitly on the default
  application.

  A Refresh vote means to either take the call stack down or terminate an
  application.

  @param[in] session_id       Session ID of the caller.
  @param[in] refresh_files    Files for which the client is getting
                              notification.
  @param[in] vote_for_init    Vote to bring down the call stack or
                              terminate the application.
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
  A valid session ID is required.

  @limitations
  This function is limited to the client's session.\n
  If the client has registered for the default application, the appropriate
  client ID must be used.
*/
mmgsdi_return_enum_type mmgsdi_session_register_for_refresh (
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_DEREGISTER_FOR_REFRESH
=============================================================================*/
/**
  Deregisters a client for FCNs for specific files, voting for refresh, or both.

  @param[in] session_id       Session ID of the caller.
  @param[in] refresh_files    Files for which the client is not to get
                              notification.
  @param[in] vote_for_init    Deregister from voting, where FALSE means no
                              changes to the current registration, and TRUE
                              means deregister from voting.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_deregister_for_refresh (
   mmgsdi_session_id_type             session_id,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_REFRESH_COMPLETE
=============================================================================*/
/**
  Notifies the MMGSDI when the client has finished processing all the files that
  were refreshed.

  @param[in] session_id       Session ID of the caller.
  @param[in] pass_fail        Client successfully read all files that were
                              refreshed. This can also be applicable to
                              initialization cases, where the client was able
                              to perform its initialization properly.
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
  A valid session ID is required.\n
  A client must have registered for FCNs and have received a notification for
  an FCN before calling this function.
*/
mmgsdi_return_enum_type mmgsdi_session_refresh_complete (
   mmgsdi_session_id_type             session_id,
   boolean                            pass_fail,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref
);

/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OK_TO_REFRESH
=============================================================================*/
/**
  Approves the refresh procedures.

  The client must call this function if it gets a refresh event
  (REFRESH_STAGE_WAIT_FOR_OK_TO_INIT or REFRESH_STAGE_WAIT_FOR_OK_TO_FCN),
  or if the refresh procedures does not start. The client uses this function
  call to notify the MMGSDI that it can either terminate the application
  or continue with the file change.

  @param[in] session_id       Session ID of the caller.
  @param[in] ok_to_refresh    Client notifies the MMGSDI that it can
                              continue with refresh (TRUE) or not (FALSE). For
                              clients on the default application, this implies
                              bringing down the stack, and for others, it
                              implies terminating the application.
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
  A valid session ID is required.\n
  A client must have registered for FCNs and have received a notification for
  an FCN before calling this function.
*/
mmgsdi_return_enum_type mmgsdi_session_ok_to_refresh (
   mmgsdi_session_id_type         session_id,
   boolean                        ok_to_refresh,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_OMA_BCAST
===========================================================================*/
/**
  Enables the Generic SIM Toolkit task to send an Open Mobile Alliance
  Broadcast command as specified in 3GPP/ETSI for OMA BCAST Key Management
  System.

  Following are the command modes:
   - MMGSDI_BCAST_SPE_AUTDIT_MODE
   - MMGSDI_BCAST_SPE_SIG_MODE
   - MMGSDI_BCAST_REC_AUDIT_MODE

  @param[in] session_id       Session ID of the caller.
  @param[in] mode             Mode in which the command is to operate.
  @param[in] bcast_req        Input data.
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
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_oma_bcast(
  mmgsdi_session_id_type             session_id,
  mmgsdi_bcast_mode_enum_type        mode,
  mmgsdi_data_type                   bcast_req,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION:     MMGSDI_SESSION_SEEK
===========================================================================*/
/**
  Performs a GSM Seek command on the card.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File information.
  @param[in] seek_direction   Search direction.
  @param[in] seek_pattern     Search data.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.\n
  MMGSDI_NOT_SUPPORTED    -- Card is not an ICC card.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_seek(
  mmgsdi_session_id_type          session_id,
  mmgsdi_access_type              file_access,
  mmgsdi_seek_direction_enum_type seek_direction,
  mmgsdi_seek_data_type           seek_pattern,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_SEARCH
===========================================================================*/
/**
  Performs a Search command on a UICC card.

  @param[in] session_id         Session ID of the caller.
  @param[in] file_access        File information.
  @param[in] search_type        UICC search type: MMGSDI_UICC_ENHANCE_SEARCH
                                or MMGSDI_UICC_SIMPLE_SEARCH.
  @param[in] search_record_num  Record number from where the search starts in
                                the file. The valid value for the current
                                record is 0, and for a physical record number
                                it is in the range of 0 to 254.
  @param[in] search_direction   Search direction.
  @param[in] enhanced_search_offset_data  Offset from where the search starts
                                in a record. The valid range for offset data
                                is 0 to 255 for MMGSDI_SEARCH_BY_CHAR, or 0
                                to 254 for MMGSDI_SEARCH_BY_OFFSET.
  @param[in] search_pattern     Data to be searched.
  @param[in] response_cb_ptr    Pointer to the response callback.
  @param[in] client_ref         User data returned upon completion of this
                                command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.\n
  MMGSDI_NOT_SUPPORTED    -- Card is not an ICC card.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_search(
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_access,
  mmgsdi_search_enum_type           search_type,
  mmgsdi_rec_num_type               search_record_num,
  mmgsdi_search_direction_enum_type search_direction,
  mmgsdi_search_offset_data_type    enhanced_search_offset_data,
  mmgsdi_search_data_type           search_pattern,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
);

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_ACTIVATE
===========================================================================*/
/**
  Activates a personalization feature by an external source.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature          Identifier for the personalization feature.
  @param[in] key_ptr          Pointer to a structure with control key
                              information.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_activate
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature,
  const mmgsdi_perso_client_data_type * key_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_DEACTIVATE
===========================================================================*/
/**
  Deactivates a personalization feature by an external source.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature          Identifier for the personalization feature.
  @param[in] key_ptr          Pointer to the structure with control key
                              information.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_deactivate
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature,
  const mmgsdi_perso_client_data_type * key_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
 FUNCTION:      MMGSDI_SESSION_PERSO_GET_INDICATORS
================================================================*/
/**
  Retrieves the information about personalization indicators.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature_type     Information required for permanently disabled
                              or all personalization indicators.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_get_indicators
(
  mmgsdi_session_id_type              session_id,
  mmgsdi_perso_feature_ind_enum_type  feature_type,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_OTA_DEPERSO
===========================================================================*/
/**
  Performs OTA depersonalization for a Point-to-Point SMS message.

  The Wireless Messaging Services (WMS) task handling the OTA SMS messages calls this function when it
  determines that the SMS PP contains data relevant to OTA depersonalization.

  @important This function is not intended for SMS-PP Download OTA
  Depersonalization. The handling task, WMS, must use the GSTK API to send an
  SMS PP Download.

  @param[in] session_id        Session ID of the caller
  @param[in] feature           Identifier for the personalization feature.
  @param[in] deperso_data_ptr  Pointer to the depersonalization data
                               containing the control key.
  @param[in] response_cb_ptr   Pointer to the response callback.
  @param[in] client_ref        User data returned upon completion of this
                               command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_ota_deperso
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature,
  const mmgsdi_perso_client_data_type * deperso_data_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*===========================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_SET_DATA
===========================================================================*/
/**
  Sets the personalization data for a specific personalization feature.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature          Identifier for the personalization feature.
  @param[in] perso_codes_ptr  Pointer to the structure that contains the
                              personalization feature and data to be set.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_set_data
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature,
  const mmgsdi_perso_client_data_type * perso_codes_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_GET_DATA
================================================================*/
/**
  Gets the personalization data for a specific personalization feature.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature_ind      Identifiers used for each personalization
                              feature.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_get_data
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature_ind,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_PERM_DISABLE
================================================================*/
/**
  Permanently disables a personalization feature. The feature and control key
  are required.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature_ind      Identifiers used for each personalization
                              feature.
  @param[in] key_ptr          Pointer to the structure with control key
                              information.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_perm_disable
(
  mmgsdi_session_id_type                session_id,
  mmgsdi_perso_feature_enum_type        feature_ind,
  const mmgsdi_perso_client_data_type * key_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_GET_DCK_RETRIES
================================================================*/
/**
  Gets the number of depersonalization attempts remaining.

  Every time a depersonalization attempt is made and fails, the number of
  attempts remaining is decremented. Therefore, this function can be used to
  check the number of retries remaining. The function returns the number of
  retries for all features, regardless of the feature indicator state.

  @param[in] session_id        Session ID of the caller.
  @param[in] dck_retries_type  Depersonalisation retries or unblock retries.
  @param[in] response_cb_ptr   Pointer to the response callback.
  @param[in] client_ref        User data returned upon completion of this
                               command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID for a provisioning application is required.
*/
 mmgsdi_return_enum_type mmgsdi_session_perso_get_dck_retries
(
  mmgsdi_session_id_type                 session_id,
  mmgsdi_perso_dck_retries_enum_type     dck_retries_type,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_UNBLOCK
================================================================*/
/**
 Unblocks a personalization feature.

  @param[in] session_id   Session ID of the caller
  @param[in] feature_ind  Identifiers used for each personalization feature.
  @param[in] key_ptr      Pointer to the structure with control key
                          information.
  @param[in] response_cb  Response callback data type.
  @param[in] client_ref   User data returned upon completion of this
                          command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_unblock
(
  mmgsdi_session_id_type               session_id,
  mmgsdi_perso_feature_enum_type       feature_ind,
  const mmgsdi_perso_client_data_type* key_ptr,
  mmgsdi_callback_type                 response_cb,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_GET_KEY
================================================================*/
/**
  Gets the personalization feature indicator key.

  @param[in] session_id       Session ID of the caller.
  @param[in] feature_ind      Identifiers used for each personalization
                              feature.
  @param[in] key_type         Indicates whether a key or unblock key is
                              being requested.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_get_key(
  mmgsdi_session_id_type               session_id,
  mmgsdi_perso_feature_enum_type       feature_ind,
  mmgsdi_perso_get_key_enum_type       key_type,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
 FUNCTION:      MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION
================================================================*/
/**
  Indicates that a subscription on the card is illegal or legal.
  This is a synchronous function from a client perspective.

  @param[in] session_id   Session ID of the illegal subscription (provisioning
                          session).
  @param[in] legal_status Whether the request is to mark the app ILLEGAL or
                          LEGAL

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n

  @dependencies
  A valid session ID for a provisioning application is required.
*/
  mmgsdi_return_enum_type mmgsdi_session_manage_illegal_subscription(
  mmgsdi_session_id_type                session_id,
  mmgsdi_session_app_legal_status_type  legal_status
);

/*================================================================
 FUNCTION:      MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC
================================================================*/
/**
  Indicates that a subscription on the card is illegal and is to be blocked.\
  This is a synchronous function.

  @param[in] session_id  Session ID of the illegal subscription (provisioning
                         session).

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_illegal_subscription_sync(
  mmgsdi_session_id_type              session_id
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_SSD_UPDATE
================================================================*/
/**
  Performs an Update Shared Secret Data command. This command initiates the
  calculation of a new SSD value for authentication purposes.

  @param[in] session_id       Session ID of the caller.
  @param[in] randssd          56-bit random number from the base station for
                              SSD generation.
  @param[in] process_control  Process control bits.
  @param[in] esn              ESN of the ME.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_ssd_update(
  mmgsdi_session_id_type               session_id,
  qword                                randssd,
  byte                                 process_control,
  dword                                esn,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_CONFIRM_SSD
================================================================*/
/**
  Performs a Confirm SSD command.

  The authentication signature calculated by the network is compared to that
  calculated by the RUIM. If the results match, they both had the same SSD input
  and the RUIM is authenticated.

  @param[in] session_id       Session ID of the caller.
  @param[in] authbs           Authentication signature from the base station.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_confirm_ssd(
  mmgsdi_session_id_type               session_id,
  dword                                authbs,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_BS_CHAL
================================================================*/
/**
  Performs an OTASP base station challenge.

  In response to an SSD Update command, the ME initiates a challenge before
  updating its SSD to ensure that the update process is being initiated by a
  legitimate source.

  @param[in] session_id       Session ID of the caller.
  @param[in] randseed         Random number generator seed.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_bs_chal(
  mmgsdi_session_id_type               session_id,
  dword                                randseed,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_MS_KEY_REQ
================================================================*/
/**
  Performs an OTASP MS Key Request command. The network sends the MS Key
  Request message to the ME.

  @param[in] session_id       Session ID of the caller.
  @param[in] randseed         Seed used to generate a true random number; sent
                              to the RUIM by the ME.
  @param[in] a_key_p_rev      A_KEY_P_REV variable sent by the base station,
                              indicating the Protocol Revision and
                              type of A-key supported by the base station.
  @param[in] param_p          Parameter modulus P is sent to the RUIM from the
                              network. The RUIM uses this with parameter
                              generator G to calculate the A-key for the SSD
                update.
  @param[in] param_g          Parameter generator G is sent to the RUIM from
                              the network. The RUIM uses this with parameter
                              modulus P to calculate the A-key for the SSD
                              update.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_ms_key_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      randseed,
  byte                                  a_key_p_rev,
  mmgsdi_data_type                      param_p,
  mmgsdi_data_type                      param_g,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_KEY_GEN_REQ
================================================================*/
/**
  Performs an OTASP Key Generation request. The network sends the Key
  Generation Request command to the ME.

  @param[in] session_id       Session ID of the caller.
  @param[in] bs_result        Base station result data calculated with the P
                              and G parameters and sent to the RUIM.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_key_gen_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      bs_result,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ
================================================================*/
/**
  Performs an OTASP System Selection for Preferred Roaming Configuration
  request.

  @param[in] session_id       Session ID of the caller.
  @param[in] block_id         Block ID of the PRL parameter block.
  @param[in] req_offset       Requested offset in the PRL.
  @param[in] req_max_size     Requested maximum number of octets in the response.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_sspr_config_req(
  mmgsdi_session_id_type                session_id,
  byte                                  block_id,
  word                                  req_offset,
  byte                                  req_max_size,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_BLOCK_REQ

================================================================*/
/**
  Performs an OTASP Block request.

  This function is for a Configuration request, Download request, SSPR Download
  request, or Validate request. These types of requests use block parameters.

  @param[in] session_id           Session ID of the caller.
  @param[in] otasp_block_op_type  Type of parameter block to use for a
                                  Configuration, Download, SSPR download, or
                                  Validate operation.
  @param[in] block_id             Block ID for the request.
  @param[in] block_data           Block data (for Download, SSPR Download, or
                                  Validate only).
  @param[in] response_cb_ptr      Pointer to the response callback.
  @param[in] client_ref           User data returned upon completion of this
                                  command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_block_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_otasp_block_op_enum_type       otasp_block_op_type,
  byte                                  block_id,
  mmgsdi_data_type                      block_data,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_COMMIT_REQ
================================================================*/
/**
  Performs an OTASP Commit request.

  This function requests that the RUIM move the new SSD (and a new A-key, if
  applicable) from its temporary memory to semi-permanent memory.

  @param[in] session_id       Session ID of the caller.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_commit_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_OTAPA_REQ
================================================================*/
/**
  Performs an OTASP OTAPA request.

  @param[in] session_id       Session ID of the caller.
  @param[in] start_stop       OTAPA session start-stop indicator (1=start,
                              0=stop).
  @param[in] randseed         Random seed. A randomly-generated 32-bit value
                              that the ME sends to the RUIM.
  @param[in] esn              ESN or pseudo-ESN of the mobile equipment. If
                              the mobile station supports only the ME
                              Identifier, the pseudo-ESN is used as the ESN.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_otasp_otapa_req(
  mmgsdi_session_id_type                session_id,
  byte                                  start_stop,
  dword                                 randseed,
  dword                                 esn,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_CAPABILITIES
================================================================*/
/**
  Extracts all the FDN, BDN, ACL, and IMSI status provisioning application
  capabilities, and the phone book-related capabilities.

  @param[in] session_id       Session ID of the caller.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_get_app_capabilities(
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_CPHS_INFO
================================================================*/
/**
  Gets information related to the Common PCN Handset Specification. This
  information pertains to GSM handsets.

  @param[in] session_id       Session ID of the caller.
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
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_get_cphs_info(
  mmgsdi_session_id_type               session_id,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_PERSO_LOCK_DOWN_AND_ENABLE_LIMITED_ACCESS
================================================================*/
/**
  Locks down some of the personalization commands.

  Locked-down commands return an error when executed. This can be used to avoid
  retrieving personalization keys from the device.

  @param[in] session_id       Session ID of the caller
  @param[in] locked_command   Identifiers used for each personalization
                              command.
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
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_perso_lock_down_and_enable_limited_access
(
  mmgsdi_session_id_type               session_id,
  mmgsdi_perso_lock_down_enum_type     locked_command,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_READ_PRL
================================================================*/
/**
  Gets the IS-683-C or IS-683-A PRL from either the EF Extended PRL (EF-EPRL)
  or the EF-PRL when the EF-EPRL does not contain a valid PRL.

  @param[in] session_id       Session ID of the caller.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion
                              of this command.

  @return
  MMGSDI_SUCCESS           -- Command structure was properly generated and sent
                              to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS  -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL    -- Command was not queued to the MMGSDI task because
                              the maximum number of commands are already queued.

  @dependencies
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_read_prl (
  mmgsdi_session_id_type               session_id,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC
================================================================*/
/**
  Gets NVRUIM state information and part of NVRUIM's global cache.
  This is a synchronous call.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

  @param[in] session_id        Session ID of the caller.
  @param[out] nvruim_data_ptr  NVRUIM data of global cache.

  @return
  MMGSDI_SUCCESS           -- Successfully retrieved NVRUIM data.\n
  MMGSDI_INCORRECT_PARAMS  -- Parameters are not within the appropriate ranges.\n

  @dependencies
  A valid 1X session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_get_nvruim_data_sync(
  mmgsdi_session_id_type               session_id,
  mmgsdi_nvruim_data_type             *nvruim_data_ptr
);

/*===============================================================
FUNCTION:        MMGSDI_SESSION_GET_INFO_SYNC
================================================================*/
/**
  Gets session information: session ID, session type, slot number, and channel
  ID. This is a synchronous call.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

  @parm[in] session_info_query    Query type and key.
  @parm[out] session_get_info_ptr Session information.

  @return
  MMGSDI_SUCCESS          -- Successfully retrieved the session information.\n
  MMGSDI_ERROR            -- No application is linked to the Session ID/Session
                             type.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_info_sync (
  mmgsdi_session_info_query_type    session_info_query,
  mmgsdi_session_get_info_type    * session_get_info_ptr
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_INCREASE
========================================================================== */
/**
  Provides increase access (by file enumeration or path) to the last-selected
  record in a cyclic file.

  This function encapsulates the details required to increment contents of
  a file in the SIM, USIM, RUIM, or CSIM card. The client using this function
  can access only the files that are cyclic.

  The function behaves as follows for these file types:
   - Transparent file  -- Returns an error.
   - Cyclic file       -- Increments the last-selected record in the file.
   - Linear fixed file -- Returns an error.

  The function behaves as follows for these access types:
   - MMGSDI_EF_ENUM_ACCESS -- Increments the record in the file provided in the
                              function call.
   - MMGSDI_BY_PATH_ACCESS -- Increments the record in the file provided in the
                              function call. There is no check whether the path
                              passed in is a record file or not. The caller is
                              expected to know whether the file to access is a
                              record.
   - Others -- Returns an error.

  The function behaves as follows for these record types:
   - MMGSDI_LINEAR_FIXED_FILE -- Returns an error.
   - MMGSDI_CYCLIC_FILE       -- Uses PREVIOUS access to the card,
   - MMGSDI_TRANSPARENT_FILE  -- Returns an error.
   - MMGSDI_MASTER_FILE       -- Returns an error.
   - MMGSDI_DEDICATED_FILE    -- Returns an error.
   - MMGSDI_MAX_FILE_STRUCTURE_ENUM -- Returns an error.

  @param[in] session_id       Session ID of the caller.
  @param[in] file_access      File to increment.
  @param[in] increase_data    Value of increment.
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
  A valid session ID is required.

  @limitations
  Returns an error when used to access a transparent or linear fixed file.
*/
mmgsdi_return_enum_type  mmgsdi_session_increase (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_increase_data_type          increase_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_GET_OPERATOR_NAME_EXT
========================================================================== */
/**
  This function is used to get operator name according to PLMN Id.
  Upon successful completion of this function, MMGSDI sends a confimation to
  client with PLMN name list and service provider name. Client can set param
  ignore_display_condition to bypass display condition provided in EF-SPN for
  RPLMN name and SPN.
  If any PLMN id of PLMN Id list matches with registered PLMN Id, MMGSDI sends
  registered PLMN name in confirmation.

  @param[in] session_id               Session ID
  @param[in] plmn_id_list             list of PLMN id
  @param[in] ignore_display_condition ignore display condition enum type
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID, and the Session ID specified must
  be valid.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_operator_name_ext (
  mmgsdi_session_id_type                 session_id,
  mmgsdi_plmn_id_list_type               plmn_id_list,
  mmgsdi_eons_ignore_disp_cond_enum_type ignore_display_condition,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_GET_OPERATOR_NAME
========================================================================== */
/**
  This function is used to get operator name according to PLMN Id.
  Upon successful completion of this function, MMGSDI sends a confimation to
  client with PLMN name list and service provider name
  If any PLMN id of PLMN Id list matches with registered PLMN Id, MMGSDI sends
  registered PLMN name in confirmation.

  @param[in] session_id       Session ID
  @param[in] plmn_id_list     list of PLMN id
  @param[in] response_cb_ptr  Response callback
  @param[in] client_ref        User Data returned upon
                              completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID, and the Session ID specified must
  be valid.
*/
mmgsdi_return_enum_type  mmgsdi_session_get_operator_name (
  mmgsdi_session_id_type             session_id,
  mmgsdi_plmn_id_list_type           plmn_id_list,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE
========================================================================== */
/**
  This function allows the client to register or unregister for file update
  notifications for specific files.
  If the API is called a second time by a client, the file list is updated,
  meaning the previous list is overwritten.
  If the API is called by a client with an empty file list the client
  is unregistered.

  @param[in] session_id             Session ID of the caller.
  @param[in] file_watch_list_ptr    Files for which the client is getting
                                    notification.
  @param[in] response_cb_ptr        Pointer to the response callback.
  @param[in] client_ref             User data returned upon completion of this
                                    command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.

  @limitations
  This function is limited to the client's session.\n
  If the client has registered for the default application, the appropriate
  client ID must be used.
*/
mmgsdi_return_enum_type mmgsdi_session_register_for_file_update (
  mmgsdi_session_id_type                session_id,
  const mmgsdi_file_update_list_type  * file_watch_list_ptr,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_REGISTER_ALL_FOR_REFRESH
========================================================================== */
/**
  This function allows the client to register or unregister for file update
  notifications for any file in a particular DF depending upon the session
  id.
  If the API is called a second time by a client, previous parameters are
  updated only if there is any change in the current request.

  @param[in] session_id             Session ID of the caller.
  @param[in] is_registration        Indicates if it is registration or
                                    de-registration.
  @param[in] vote_for_init          Vote to bring down the call stack or
                                    terminate the application.
  @param[in] response_cb_ptr        Pointer to the response callback.
  @param[in] client_ref             User data returned upon completion of this
                                    command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
  @limitations
  This function is limited to the client's session.\n
  If the client has registered for the default application, the appropriate
  client ID must be used.
*/
mmgsdi_return_enum_type mmgsdi_session_register_all_for_refresh (
  mmgsdi_session_id_type                session_id,
  boolean                               is_registration,
  boolean                               vote_for_init,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_UPDATE_ACSGL_RECORD
========================================================================== */
/**
  This function allows the client to update (add or delete) a CSG ID associated
  with a PLMN ID in the EF-ACSGL record.

  @param[in] session_id             Session ID of the caller.
  @param[in] update_type            Type of update of CSG ID requested
  @param[in] plmn_csg_id            CSG ID to be updated and associated PLMN ID
  @param[in] hbn_name               Home Node(b) name corresponding to the CSG ID
  @param[in] response_cb_ptr        Pointer to the response callback.
  @param[in] client_ref             User data returned upon completion of this
                                    command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.

  @limitations
  This function is limited to the client's session.\n
  If the client has registered for the default application, the appropriate
  client ID must be used.
*/
mmgsdi_return_enum_type mmgsdi_session_update_acsgl_record(
  mmgsdi_session_id_type               session_id,
  mmgsdi_update_acsgl_enum_type        update_type,
  mmgsdi_plmn_csg_id_type              plmn_csg_id,
  mmgsdi_data_type                     hnb_name,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
);

/* ===========================================================================
FUNCTION        MMGSDI_SESSION_ACTIVATE_FILE
========================================================================== */
/**
DESCRIPTION
  This function provides a way to activate (rehabilitate) a file in the
  SIM/USIM/RUIM/CSIM.

PARAMETERS
  @param[in] session_id:           Session ID of the caller.
  @param[in] file_access:          File to activate.
  @param[in] response_cb_ptr:      Response callback.
  @param[in] client_ref:           User Data returned upon completion of this cmd.

  @return
  MMGSDI_SUCCESS:          The command structure was properly generated
                           and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already
                           queued.

  @dependencies
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
*/
mmgsdi_return_enum_type  mmgsdi_session_activate_file (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/* ===========================================================================
FUNCTION        MMGSDI_SESSION_DEACTIVATE_FILE
========================================================================== */
/**
DESCRIPTION
  This function provides a way to deactivate (invalidate) a file in the
  SIM/USIM/RUIM/CSIM.

PARAMETERS
  @param[in] session_id:           Session ID of the caller.
  @param[in] file_access:          File to deactivate.
  @param[in] response_cb_ptr:      Response callback.
  @param[in] client_ref:           User Data returned upon completion of this cmd.

  @return
  MMGSDI_SUCCESS:          The command structure was properly generated
                           and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already
                           queued.

  @dependencies
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
*/
mmgsdi_return_enum_type  mmgsdi_session_deactivate_file (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_USIM_AUTHENTICATE_EXT
===========================================================================*/
/**
  Performs a Network Initiated Authentication with a USIM application.

  This operation is valid and applicable only when a USIM application is
  available. The caller of this function can specify whether to perform
  a check on application state before sending the command to card.

  @param[in] session_id          Session ID of the caller.
  @param[in] auth_context        Context of the authentication challenge.
  @param[in] auth_req            Length and data of the authentication
                                 challenge.
  @param[in] illegal_or_perso_ok Flag to indicate if this command can be
                                 executed if app state is ILLEGAL or
                                 PERSO failed.
  @param[in] response_cb_ptr     Pointer to the response callback.
  @param[in] client_ref          User data returned upon completion of this
                                 command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  If the USIM application is not the default application, the client must have
  a valid session open.
*/
mmgsdi_return_enum_type  mmgsdi_session_usim_authenticate_ext (
  mmgsdi_session_id_type              session_id,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  boolean                             illegal_or_perso_ok,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/*===========================================================================
FUNCTION:       MMGSDI_SESSION_RUN_GSM_ALGO_EXT
===========================================================================*/
/**
  Performs a Network Initiated Authentication with the SIM card. This operation
  is valid and applicable only when a SIM card is available. The caller of
  this function can specify whether to perform a check on application state
  before sending the command to card.

  @param[in] session_id          Session ID of the caller.
  @param[in] rand                Length and data of the RAND.
  @param[in] illegal_or_perso_ok Flag to indicate if this command
                                 can be executed if app state is ILLEGAL or
                                 PERSO failed
  @param[in] response_cb_ptr     Pointer to the response callback.
  @param[in] client_ref          User data returned upon completion of this
                                 command.

  @return
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

  @dependencies
  A valid session ID is required.
*/
mmgsdi_return_enum_type mmgsdi_session_run_gsm_algo_ext (
  mmgsdi_session_id_type              session_id,
  mmgsdi_data_type                    rand,
  boolean                             illegal_or_perso_ok,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/*=============================================================================
 FUNCTION:      MMGSDI_SESSION_CANCEL_REQUESTS
=============================================================================*/
/**
  This API is used to cancel all the pending requests related to the particular
  session ID.

  @param[in] session_id  Session ID of the requests the need to be cancelled.

  @return
  MMGSDI_SUCCESS          -- Command was properly processed.\n
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.\n

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_cancel_requests(
  mmgsdi_session_id_type              session_id
);

/* ===========================================================================
FUNCTION        MMGSDI_SESSION_RESELECT_AID
=============================================================================*/
/**
DESCRIPTION
  This function provides a way to SELECT an AID on an already-opened channel
  using the given selection mode.

PARAMETERS
  session_id:           Session ID of the caller.
  select_mode:          Mode for SELECT command.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
*/
mmgsdi_return_enum_type  mmgsdi_session_reselect_aid (
  mmgsdi_session_id_type         session_id,
  mmgsdi_select_mode_enum_type   select_mode,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);

/* ==========================================================================
FUNCTION        MMGSDI_SESSION_STORE_ESN

DESCRIPTION
  This function builds request to store ESN into the card.

PARAMETERS
  session_id:           Session ID of the caller.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_store_esn (
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
);

/** @} */ /* end_addtogroup session_library */

#endif /* MMGSDISESSIONLIB_H */
