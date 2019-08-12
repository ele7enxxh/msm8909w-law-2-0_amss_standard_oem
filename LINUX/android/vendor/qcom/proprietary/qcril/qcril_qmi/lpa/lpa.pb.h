/******************************************************************************
#  @file    lpa.pb.c
#  @brief   Lpa protobuf file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef _PB_LPA_PB_H_
#define _PB_LPA_PB_H_
#include <pb.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _lpa_Error {
    lpa_Error_UIM_LPA_ERR_SUCCESS = 0,
    lpa_Error_UIM_LPA_ERR_GENERIC_FAILURE = 1,
    lpa_Error_UIM_LPA_ERR_NOT_SUPPORTED = 2,
    lpa_Error_UIM_LPA_ERR_INVALID_PARAMETER = 3
} lpa_Error;

typedef enum _lpa_MessageType {
    lpa_MessageType_UIM_LPA_MSG_UNKNOWN = 0,
    lpa_MessageType_UIM_LPA_MSG_REQUEST = 1,
    lpa_MessageType_UIM_LPA_MSG_RESPONSE = 2,
    lpa_MessageType_UIM_LPA_MSG_INDICATION = 3
} lpa_MessageType;

typedef enum _lpa_MessageId {
    lpa_MessageId_UNKNOWN_REQ = 0,
    lpa_MessageId_UIM_LPA_USER_REQUEST = 1,
    lpa_MessageId_UIM_LPA_HTTP_TXN_COMPLETED_REQUEST = 2,
    lpa_MessageId_UIM_LPA_HTTP_TXN_IND = 3,
    lpa_MessageId_UIM_LPA_ADD_PROFILE_PROGRESS_IND = 4
} lpa_MessageId;

typedef enum _lpa_UimLpaUserEventId {
    lpa_UimLpaUserEventId_UIM_LPA_UNKNOWN_EVENT_ID = 0,
    lpa_UimLpaUserEventId_UIM_LPA_ADD_PROFILE = 1,
    lpa_UimLpaUserEventId_UIM_LPA_ENABLE_PROFILE = 2,
    lpa_UimLpaUserEventId_UIM_LPA_DISABLE_PROFILE = 3,
    lpa_UimLpaUserEventId_UIM_LPA_DELETE_PROFILE = 4,
    lpa_UimLpaUserEventId_UIM_LPA_EUICC_MEMORY_RESET = 5,
    lpa_UimLpaUserEventId_UIM_LPA_GET_PROFILE = 6,
    lpa_UimLpaUserEventId_UIM_LPA_UPDATE_NICKNAME = 7,
    lpa_UimLpaUserEventId_UIM_LPA_GET_EID = 8
} lpa_UimLpaUserEventId;

typedef enum _lpa_UimLpaResult {
    lpa_UimLpaResult_UIM_LPA_RESULT_SUCCESS = 0,
    lpa_UimLpaResult_UIM_LPA_RESULT_FAILURE = 1,
    lpa_UimLpaResult_UIM_LPA_RESULT_CONFRM_CODE_MISSING = 2
} lpa_UimLpaResult;

typedef enum _lpa_UimLpaProfileInfo_ProfileState {
    lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_UNKNOWN = 0,
    lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_ACTIVE = 1,
    lpa_UimLpaProfileInfo_ProfileState_UIM_LPA_PROFILE_STATE_INACTIVE = 2
} lpa_UimLpaProfileInfo_ProfileState;

typedef enum _lpa_UimLpaProfileInfo_IconType {
    lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_UNKNOWN = 0,
    lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_JPEG = 1,
    lpa_UimLpaProfileInfo_IconType_UIM_LPA_ICON_TYPE_PNG = 2
} lpa_UimLpaProfileInfo_IconType;

typedef enum _lpa_UimLpaAddProfileProgressInd_Status {
    lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_NONE = 0,
    lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_ERROR = 1,
    lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS = 2,
    lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS = 3,
    lpa_UimLpaAddProfileProgressInd_Status_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE = 4
} lpa_UimLpaAddProfileProgressInd_Status;

typedef enum _lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause {
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE = 0,
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC = 1,
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM = 2,
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK = 3,
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY = 4
} lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause;

/* Struct definitions */
typedef struct _lpa_CustomHeader {
    pb_callback_t headerName;
    pb_callback_t headerValue;
} lpa_CustomHeader;

typedef struct _lpa_MessageTag {
    lpa_MessageType type;
    lpa_MessageId id;
    bool has_error;
    lpa_Error error;
    bool has_token;
    uint32_t token;
    pb_callback_t payload;
} lpa_MessageTag;

typedef struct _lpa_UimHttpTransactionCompleteResp {
    lpa_UimLpaResult result;
} lpa_UimHttpTransactionCompleteResp;

typedef struct _lpa_UimLpaAddProfileProgressInd {
    lpa_UimLpaAddProfileProgressInd_Status status;
    bool has_cause;
    lpa_UimLpaAddProfileProgressInd_AddProfileFailureCause cause;
    bool has_progress;
    int32_t progress;
} lpa_UimLpaAddProfileProgressInd;

typedef struct _lpa_UimLpaProfileInfo {
    lpa_UimLpaProfileInfo_ProfileState state;
    pb_callback_t iccid;
    pb_callback_t profileName;
    pb_callback_t nickname;
    pb_callback_t spName;
    bool has_iconType;
    lpa_UimLpaProfileInfo_IconType iconType;
    pb_callback_t icon;
} lpa_UimLpaProfileInfo;

typedef struct _lpa_UimLpaUserReq {
    lpa_UimLpaUserEventId event;
    pb_callback_t activationCode;
    pb_callback_t confirmationCode;
    pb_callback_t nickname;
    pb_callback_t iccid;
} lpa_UimLpaUserReq;

typedef struct _lpa_UimHttpTransactionCompleteReq {
    lpa_UimLpaResult result;
    uint32_t tokenId;
    pb_callback_t responsePayload;
    pb_callback_t customHeaders;
} lpa_UimHttpTransactionCompleteReq;

typedef struct _lpa_UimHttpTransactionInd {
    uint32_t tokenId;
    pb_callback_t payload;
    pb_callback_t contentType;
    pb_callback_t customHeaders;
    pb_callback_t url;
} lpa_UimHttpTransactionInd;

typedef struct _lpa_UimLpaUserResp {
    lpa_UimLpaUserEventId event;
    lpa_UimLpaResult result;
    pb_callback_t eid;
    pb_callback_t profiles;
} lpa_UimLpaUserResp;

/* Default values for struct fields */

/* Field tags (for use in manual encoding/decoding) */
#define lpa_CustomHeader_headerName_tag          1
#define lpa_CustomHeader_headerValue_tag         2
#define lpa_MessageTag_type_tag                  1
#define lpa_MessageTag_id_tag                    2
#define lpa_MessageTag_error_tag                 3
#define lpa_MessageTag_token_tag                 4
#define lpa_MessageTag_payload_tag               5
#define lpa_UimHttpTransactionCompleteResp_result_tag 1
#define lpa_UimLpaAddProfileProgressInd_status_tag 1
#define lpa_UimLpaAddProfileProgressInd_cause_tag 2
#define lpa_UimLpaAddProfileProgressInd_progress_tag 3
#define lpa_UimLpaProfileInfo_state_tag          1
#define lpa_UimLpaProfileInfo_iccid_tag          2
#define lpa_UimLpaProfileInfo_profileName_tag    3
#define lpa_UimLpaProfileInfo_nickname_tag       4
#define lpa_UimLpaProfileInfo_spName_tag         5
#define lpa_UimLpaProfileInfo_iconType_tag       6
#define lpa_UimLpaProfileInfo_icon_tag           7
#define lpa_UimLpaUserReq_event_tag              1
#define lpa_UimLpaUserReq_activationCode_tag     2
#define lpa_UimLpaUserReq_confirmationCode_tag   3
#define lpa_UimLpaUserReq_nickname_tag           4
#define lpa_UimLpaUserReq_iccid_tag              5
#define lpa_UimHttpTransactionCompleteReq_result_tag 1
#define lpa_UimHttpTransactionCompleteReq_tokenId_tag 2
#define lpa_UimHttpTransactionCompleteReq_responsePayload_tag 3
#define lpa_UimHttpTransactionCompleteReq_customHeaders_tag 4
#define lpa_UimHttpTransactionInd_tokenId_tag    1
#define lpa_UimHttpTransactionInd_payload_tag    2
#define lpa_UimHttpTransactionInd_contentType_tag 3
#define lpa_UimHttpTransactionInd_customHeaders_tag 4
#define lpa_UimHttpTransactionInd_url_tag        5
#define lpa_UimLpaUserResp_event_tag             1
#define lpa_UimLpaUserResp_result_tag            2
#define lpa_UimLpaUserResp_eid_tag               3
#define lpa_UimLpaUserResp_profiles_tag          4

/* Struct field encoding specification for nanopb */
extern const pb_field_t lpa_MessageTag_fields[6];
extern const pb_field_t lpa_UimLpaUserReq_fields[6];
extern const pb_field_t lpa_UimLpaProfileInfo_fields[8];
extern const pb_field_t lpa_UimLpaUserResp_fields[5];
extern const pb_field_t lpa_UimLpaAddProfileProgressInd_fields[4];
extern const pb_field_t lpa_CustomHeader_fields[3];
extern const pb_field_t lpa_UimHttpTransactionCompleteReq_fields[5];
extern const pb_field_t lpa_UimHttpTransactionCompleteResp_fields[2];
extern const pb_field_t lpa_UimHttpTransactionInd_fields[6];

/* Maximum encoded size of messages (where known) */
#define lpa_UimLpaAddProfileProgressInd_size     23
#define lpa_UimHttpTransactionCompleteResp_size  6

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
