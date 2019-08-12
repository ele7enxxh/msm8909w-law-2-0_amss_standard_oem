#ifndef LPALIB_H
#define LPALIB_H
/*===========================================================================


            L P A   L I B R A R Y   D E F I N I T I O N S

                      A N D   F U N C T I O N S


  This file contains library functions that can be triggered as a result
  of trigger from user application

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software is
regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/inc/lpalib.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/22/16   ll      Peek into profile installation result 
05/15/16   ll      ASN1 decoder enhancement
04/11/16   av      Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

#define LPA_EID_LEN                          16
#define LPA_ICCID_LEN                        10
#define LPA_ISDP_AID_LEN                     16
#define LPA_ISDP_AID_LEN_MIN                  5
#define LPA_NUM_PROFILES_MAX                  8
#define LPA_NAME_LEN_MAX                     64
#define LPA_SPN_LEN_MAX                      32
#define LPA_ICON_LEN_MAX                   1024

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   ENUM:      LAP_RESULT_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the result of LPA request message
-----------------------------------------------------------------------------*/
typedef enum {
  LPA_SUCCESS,
  LPA_GENERIC_ERROR,
  LPA_ERROR_INCORRECT_PARAMS,
  LPA_ERROR_COMMAND_TIMEOUT,
  LPA_ERROR_HEAP_EXHAUSTED,
  LPA_ERROR_NETWORK,
  LPA_ERROR_NETWORK_SSL,
  LPA_ERROR_NETWORK_DNS,
  LPA_ERROR_NETWORK_SOCKET,
  LPA_ERROR_SMDP_RESPONSE,
  LPA_ERROR_SIM_NOT_READY,
  LPA_ERROR_PROFILE_NOT_IN_DISABLED_STATE,
  LPA_ERROR_PROFILE_NOT_IN_ENABLED_STATE,
  LPA_ERROR_PROFILE_NOT_FOUND,
  LPA_ERROR_REFERENCE_DATA_NOT_FOUND,
  LPA_ERROR_OPERATION_NOT_ALLOWED,
  LPA_ERROR_CONFIRMATION_CODE_REQUIRED,
  LPA_ERROR_CMD_QUEUE_FULL,
  LPA_ERROR_SELECT_ISDR_FAILED,
  LPA_ERROR_NOT_SUPPORTED,
  LPA_ERROR_ICCID_EXISTS_ON_EUICC,
  LPA_ERROR_INSUFFICIENT_MEMORY_ON_EUICC,
  LPA_ERROR_PROFILE_INSTALLATION_INTERRUPTED_ON_EUICC,
  LPA_ERROR_PROFILE_INSTALLATION_UNKNOWN_ERR
} lpa_result_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_SLOT_ID_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the LPA slot IDs
-----------------------------------------------------------------------------*/
typedef enum {
  LPA_SLOT_1       = 0x00,
  LPA_SLOT_2,
  LPA_NUM_SLOTS
} lpa_slot_id_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_EVENT_ENUM_TYPE

   DESCRIPTION: The events from the LPA
-------------------------------------------------------------------------------*/
typedef enum {
  LPA_PROFILE_DOWNLOAD_PROGRESS_EVENT
} lpa_event_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_PROFILE_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the state of a LPA profile on the eUICC
-----------------------------------------------------------------------------*/
typedef enum {
  LPA_PROFILE_STATE_DISABLED,
  LPA_PROFILE_STATE_ENABLED
} lpa_profile_state_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_ICON_TYPE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the type of icon wrt the profile info
-----------------------------------------------------------------------------*/
typedef enum {
  LPA_ICON_TYPE_JPEG,
  LPA_ICON_TYPE_PNG
} lpa_icon_type_enum_type;

 /* ----------------------------------------------------------------------------
   ENUM:      LPA_PROFILE_CLASS_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the class of profile
-----------------------------------------------------------------------------*/
typedef enum {
  LPA_PROFILE_CLASS_TEST,
  LPA_PROFILE_CLASS_PROVISIONING,
  LPA_PROFILE_CLASS_OPERATIONAL
} lpa_profile_class_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      LPA_MESSAGE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the message types LPA supports
-------------------------------------------------------------------------------*/
typedef enum {
  LPA_GET_PROFILES_INFO_MSG,
  LPA_ENABLE_PROFILE_MSG,
  LPA_DISABLE_PROFILE_MSG,
  LPA_DELETE_PROFILE_MSG,
  LPA_CARD_MEMORY_RESET_MSG,
  LPA_GET_EID_MSG,
  LPA_ADD_PROFILE_MSG,
  LPA_ADD_OR_UPDATE_PROFILE_NICKNAME_MSG
} lpa_message_enum_type;

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
typedef uint8 lpa_eid_type[LPA_EID_LEN];

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ISDP_AID_TYPE

   DESCRIPTION:
     This structure contains the ISDP-AID.
     length:    Number of ISDP-AID bytes (max length can be 16 bytes per SGP.22)
     data:      ISDP-AID
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8 data_len;
  uint8 data[LPA_ISDP_AID_LEN];
} lpa_isdp_aid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ICCID_TYPE

   DESCRIPTION:
     This structure contains the ICCID.
     length:    Number of ICCID bytes (max length can be 10 bytes per SGP.22)
     data:      ICCID
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8 data_len;
  uint8 data[LPA_ICCID_LEN];
} lpa_iccid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_DATA_TYPE

   DESCRIPTION:
     This structure is generic structure to hold a stream of bytes.
     data_len:  Lenght of data
     data_ptr:  Data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint32  data_len;
  uint8*  data_ptr;
} lpa_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_NAME_TYPE

   DESCRIPTION:
     This structure is generic structure to hold profile name/nickname.
     name_len:  Lenght of name
     name:      Name
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8   name_len;
  uint8   name[LPA_NAME_LEN_MAX];
} lpa_name_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_SPN_TYPE

   DESCRIPTION:
     This structure is generic structure to hold service provider name.
     name_len:  Lenght of name
     name:      Name
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8   name_len;
  uint8   name[LPA_SPN_LEN_MAX];
} lpa_spn_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_PROFILE_INFO_TYPE

   DESCRIPTION:
     This structure contains information about a profile. The item is valid if
     it has a non-default (non-zero in most cases) value.
-------------------------------------------------------------------------------*/
typedef struct {
  lpa_iccid_type               iccid;
  lpa_isdp_aid_type            isdp_aid;
  lpa_profile_state_enum_type  state;
  lpa_name_type                nickname;
  lpa_spn_type                 spn;
  lpa_name_type                name;
  lpa_icon_type_enum_type      icon_type;
  lpa_data_type                icon;
  lpa_profile_class_enum_type  profile_class;
} lpa_profile_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_GET_PROFILES_INFO_RESP_TYPE

   DESCRIPTION:
     This structure is for the LPA response corresponding to the get profiles
     info request.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                           num_profiles;
  lpa_profile_info_type           profile_info[LPA_NUM_PROFILES_MAX];
} lpa_get_profiles_info_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_GET_EID_RESP_TYPE

   DESCRIPTION:
     This structure is for the LPA response corresponding to the get EID
     request
-------------------------------------------------------------------------------*/
typedef struct {
  lpa_eid_type               eid;
} lpa_get_eid_resp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_RESPONSE_DATA_TYPE

   DESCRIPTION:
     This structure is for the various response messages in LPA
-------------------------------------------------------------------------------*/
typedef struct {
  lpa_message_enum_type  msg_type;
  union {
    lpa_get_profiles_info_resp_type                   get_profiles_info_resp;
    lpa_get_eid_resp_type                             get_eid_resp;
  } message;
} lpa_response_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_PROFILE_DOWNLOAD_PROGRESS_EVENT_TYPE

   DESCRIPTION:
-------------------------------------------------------------------------------*/
typedef struct {
  lpa_slot_id_enum_type                              slot_id;
  uint8                                              percent_downloaded;
  boolean                                            installation_complete;
} lpa_profile_download_progress_event_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_EVENT_DATA_TYPE

   DESCRIPTION:
     This structure is for the various event messages from LPA.
-------------------------------------------------------------------------------*/
typedef struct {
  lpa_event_enum_type                               evt_type;
  union{
    lpa_profile_download_progress_event_type        profile_download_progress;
  } evt_data;
} lpa_event_data_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      LPA_CALLBACK_TYPE

   This is client callback funtion type. The parameter to the callback function
   contains the results of the procedures performed at the client's request.
-----------------------------------------------------------------------------*/
typedef void (*lpa_callback_type) (lpa_result_enum_type          status,
                                   const lpa_response_data_type *response_ptr,
                                   void                         *user_data_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      LPA_EVT_CALLBACK_TYPE

   Event callback function associated to a client for receiving LPA related
   events.
-------------------------------------------------------------------------------*/
typedef void (*lpa_event_callback_type) (lpa_result_enum_type       status,
                                         const lpa_event_data_type *event);

/*=============================================================================

                     FUNCTION DECLARATIONS

=============================================================================*/
/*=============================================================================
FUNCTION        LPA_GET_PROFILES_INFO

DESCRIPTION
  This function allows the user to get complete information about all the
  profiles on the card.

PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_get_profiles_info (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_ENABLE_PROFILE

DESCRIPTION
  This function allows the user to enable a profile identified by the ICCID
  on the card.
PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_enable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_DISABLE_PROFILE

DESCRIPTION
  This function allows the user to disable a profile identified by the ICCID
  on the card.
PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_disable_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_DELETE_PROFILE

DESCRIPTION
  This function allows the user to delete a profile identified by the ICCID
  on the card.

PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the profile on the card
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_delete_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_CARD_MEMORY_RESET

DESCRIPTION
  This function deletes all the installed profiles, regardless of their state,
  from the card.
PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_card_memory_reset (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_GET_EID

DESCRIPTION
  This function allows the user to retrieve the EID of the card.

PARAMETERS
  slot_id               :   Slot
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_get_eid (
  lpa_slot_id_enum_type                 slot_id,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_ADD_PROFILE

DESCRIPTION
  This function allows the user to download a profile from the SMDP server
  to the card. If the activation code suggests the confirmation code is
  needed, and no confirmation code is passed in to this API, the API call
  returns error.

PARAMETERS
  slot_id                 :   Slot
  activation_code         :   Activation code that is sent to the server to
                              download a profile. Max length is 255 characters
                              per SGP.22.
  confirmation_code       :   Confirmation code, if mandated by the
                              activation_code.
  response_cb_ptr         :   Command response call back
  event_cb_ptr            :   Event callback called every n time, with the
                              profile download progress
  user_data_ptr           :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                          The command structure was properly
                                          generated and queued onto the LPA
                                          command queue.
    LPA_ERROR_INCORRECT_PARAMS:                 The parameters supplied to the API
                                          are not within appropriate ranges.
    LPA_ERROR_CONFIRMATION_CODE_REQUIRED: Confirmation code is required but
                                          not provided by the caller.
    LPA_GENERIC_ERROR:                    The command was not queued to the LPA
                                          command queue.
===============================================================================*/
lpa_result_enum_type  lpa_add_profile (
  lpa_slot_id_enum_type                 slot_id,
  lpa_data_type                         activation_code,
  lpa_data_type                         confirmation_code,
  lpa_callback_type                     response_cb_ptr,
  lpa_event_callback_type               event_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_ADD_OR_UPDATE_PROFILE_NICKNAME

DESCRIPTION
  This function allows the user to add or update the nickname of a profile.

PARAMETERS
  slot_id               :   Slot
  iccid                 :   ICCID of the target profile
  nickname              :   Nickname of the target profile. Max length can be
                            64 characters per SGP.22
  response_cb_ptr       :   Command response call back
  user_data_ptr         :   User data

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:           The command structure was properly generated and
                           queued onto the LPA command queue.
    LPA_ERROR_INCORRECT_PARAMS:  The parameters supplied to the API are not within
                           appropriate ranges.
    LPA_GENERIC_ERROR:     The command was not queued to the LPA command
                           queue.
===============================================================================*/
lpa_result_enum_type  lpa_add_or_update_profile_nickname (
  lpa_slot_id_enum_type                 slot_id,
  lpa_iccid_type                        iccid,
  lpa_name_type                         nickname,
  lpa_callback_type                     response_cb_ptr,
  void                                 *user_data_ptr
);

/*=============================================================================
FUNCTION        LPA_IS_LPA_SUPPORTED

DESCRIPTION
  Indicates if LPD and LUI are supported or not. This info is needed to be
  sent to the card in the terminal capability.

PARAMETERS
  lpd_supported_ptr     :   Boolean to indicate if lpd is supported
  lui_supported_ptr     :   Boolean to indicate if lui is supported

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                The request was processed successfully
    LPA_ERROR_INCORRECT_PARAMS: The parameters supplied to the API are not within
                                appropriate ranges.
    LPA_GENERIC_ERROR:          Some error while processing the request
===============================================================================*/
lpa_result_enum_type lpa_is_lpa_supported(
  boolean                            *lpd_supported_ptr,
  boolean                            *lui_supported_ptr
);

/*=============================================================================
FUNCTION        LPA_SET_QMI_UIM_HTTP_SIG

DESCRIPTION
  This function sets the passed signal to the LPA task

PARAMETERS
  None

RETURN VALUE
  lpa_result_enum_type
    LPA_SUCCESS:                The request was processed successfully
    LPA_ERROR_INCORRECT_PARAMS: The parameters supplied to the API are not within
                                appropriate ranges.
===============================================================================*/
lpa_result_enum_type lpa_set_qmi_uim_http_cmd_q_sig();

#endif /* LPALIB_H */

