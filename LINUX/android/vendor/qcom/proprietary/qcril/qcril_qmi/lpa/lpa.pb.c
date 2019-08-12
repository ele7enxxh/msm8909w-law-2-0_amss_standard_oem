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

#include "lpa.pb.h"



const pb_field_t lpa_MessageTag_fields[6] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_MessageTag, type, type, 0),
    PB_FIELD2(  2, ENUM    , REQUIRED, STATIC  , OTHER, lpa_MessageTag, id, type, 0),
    PB_FIELD2(  3, ENUM    , OPTIONAL, STATIC  , OTHER, lpa_MessageTag, error, id, 0),
    PB_FIELD2(  4, FIXED32 , OPTIONAL, STATIC  , OTHER, lpa_MessageTag, token, error, 0),
    PB_FIELD2(  5, BYTES   , OPTIONAL, CALLBACK, OTHER, lpa_MessageTag, payload, token, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimLpaUserReq_fields[6] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimLpaUserReq, event, event, 0),
    PB_FIELD2(  2, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaUserReq, activationCode, event, 0),
    PB_FIELD2(  3, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaUserReq, confirmationCode, activationCode, 0),
    PB_FIELD2(  4, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaUserReq, nickname, confirmationCode, 0),
    PB_FIELD2(  5, BYTES   , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaUserReq, iccid, nickname, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimLpaProfileInfo_fields[8] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimLpaProfileInfo, state, state, 0),
    PB_FIELD2(  2, BYTES   , REQUIRED, CALLBACK, OTHER, lpa_UimLpaProfileInfo, iccid, state, 0),
    PB_FIELD2(  3, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaProfileInfo, profileName, iccid, 0),
    PB_FIELD2(  4, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaProfileInfo, nickname, profileName, 0),
    PB_FIELD2(  5, STRING  , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaProfileInfo, spName, nickname, 0),
    PB_FIELD2(  6, ENUM    , OPTIONAL, STATIC  , OTHER, lpa_UimLpaProfileInfo, iconType, spName, 0),
    PB_FIELD2(  7, BYTES   , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaProfileInfo, icon, iconType, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimLpaUserResp_fields[5] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimLpaUserResp, event, event, 0),
    PB_FIELD2(  2, ENUM    , REQUIRED, STATIC  , OTHER, lpa_UimLpaUserResp, result, event, 0),
    PB_FIELD2(  3, BYTES   , OPTIONAL, CALLBACK, OTHER, lpa_UimLpaUserResp, eid, result, 0),
    PB_FIELD2(  4, MESSAGE , REPEATED, CALLBACK, OTHER, lpa_UimLpaUserResp, profiles, eid, &lpa_UimLpaProfileInfo_fields),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimLpaAddProfileProgressInd_fields[4] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimLpaAddProfileProgressInd, status, status, 0),
    PB_FIELD2(  2, ENUM    , OPTIONAL, STATIC  , OTHER, lpa_UimLpaAddProfileProgressInd, cause, status, 0),
    PB_FIELD2(  3, INT32   , OPTIONAL, STATIC  , OTHER, lpa_UimLpaAddProfileProgressInd, progress, cause, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_CustomHeader_fields[3] = {
    PB_FIELD2(  1, STRING  , REQUIRED, CALLBACK, FIRST, lpa_CustomHeader, headerName, headerName, 0),
    PB_FIELD2(  2, STRING  , REQUIRED, CALLBACK, OTHER, lpa_CustomHeader, headerValue, headerName, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimHttpTransactionCompleteReq_fields[5] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimHttpTransactionCompleteReq, result, result, 0),
    PB_FIELD2(  2, FIXED32 , REQUIRED, STATIC  , OTHER, lpa_UimHttpTransactionCompleteReq, tokenId, result, 0),
    PB_FIELD2(  3, BYTES   , REQUIRED, CALLBACK, OTHER, lpa_UimHttpTransactionCompleteReq, responsePayload, tokenId, 0),
    PB_FIELD2(  4, MESSAGE , REPEATED, CALLBACK, OTHER, lpa_UimHttpTransactionCompleteReq, customHeaders, responsePayload, &lpa_CustomHeader_fields),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimHttpTransactionCompleteResp_fields[2] = {
    PB_FIELD2(  1, ENUM    , REQUIRED, STATIC  , FIRST, lpa_UimHttpTransactionCompleteResp, result, result, 0),
    PB_LAST_FIELD
};

const pb_field_t lpa_UimHttpTransactionInd_fields[6] = {
    PB_FIELD2(  1, FIXED32 , REQUIRED, STATIC  , FIRST, lpa_UimHttpTransactionInd, tokenId, tokenId, 0),
    PB_FIELD2(  2, BYTES   , REQUIRED, CALLBACK, OTHER, lpa_UimHttpTransactionInd, payload, tokenId, 0),
    PB_FIELD2(  3, STRING  , REQUIRED, CALLBACK, OTHER, lpa_UimHttpTransactionInd, contentType, payload, 0),
    PB_FIELD2(  4, MESSAGE , REPEATED, CALLBACK, OTHER, lpa_UimHttpTransactionInd, customHeaders, contentType, &lpa_CustomHeader_fields),
    PB_FIELD2(  5, STRING  , REQUIRED, CALLBACK, OTHER, lpa_UimHttpTransactionInd, url, customHeaders, 0),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 *
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
STATIC_ASSERT((pb_membersize(lpa_UimLpaUserResp, profiles) < 65536 && pb_membersize(lpa_UimHttpTransactionCompleteReq, customHeaders) < 65536 && pb_membersize(lpa_UimHttpTransactionInd, customHeaders) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_lpa_MessageTag_lpa_UimLpaUserReq_lpa_UimLpaProfileInfo_lpa_UimLpaUserResp_lpa_UimLpaAddProfileProgressInd_lpa_CustomHeader_lpa_UimHttpTransactionCompleteReq_lpa_UimHttpTransactionCompleteResp_lpa_UimHttpTransactionInd)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 *
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
STATIC_ASSERT((pb_membersize(lpa_UimLpaUserResp, profiles) < 256 && pb_membersize(lpa_UimHttpTransactionCompleteReq, customHeaders) < 256 && pb_membersize(lpa_UimHttpTransactionInd, customHeaders) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_lpa_MessageTag_lpa_UimLpaUserReq_lpa_UimLpaProfileInfo_lpa_UimLpaUserResp_lpa_UimLpaAddProfileProgressInd_lpa_CustomHeader_lpa_UimHttpTransactionCompleteReq_lpa_UimHttpTransactionCompleteResp_lpa_UimHttpTransactionInd)
#endif


