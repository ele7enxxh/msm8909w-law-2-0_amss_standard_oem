/******************************************************************************
#  @file    lpa_msg_meta.c
#  @brief   Local profile assistant meta information source file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/


#include "qcril_uim_lpa_msg_meta.h"

typedef struct
{
    lpa_MessageId     msg_id;
    lpa_MessageType   msg_type;
    const pb_field_t* msg_pb_fields;
    size_t            msg_req_size;
    qcril_evt_e_type  msg_event;
    const char*       msg_log_str;
} qcril_uim_lpa_msg_meta_type;

const qcril_uim_lpa_msg_meta_type qcril_uim_lpa_msg_meta_data[] =
{
    // Requests, Response and Indications
    {lpa_MessageId_UIM_LPA_USER_REQUEST, lpa_MessageType_UIM_LPA_MSG_REQUEST, lpa_UimLpaUserReq_fields, sizeof(lpa_UimLpaUserReq), QCRIL_EVT_UIM_LPA_USER_REQ, "UIM_LPA_USER_REQ"},
    {lpa_MessageId_UIM_LPA_USER_REQUEST, lpa_MessageType_UIM_LPA_MSG_RESPONSE, lpa_UimLpaUserResp_fields, sizeof(lpa_UimLpaUserResp), QCRIL_EVT_NONE, "UIM_LPA_USER_RESP"},
    {lpa_MessageId_UIM_LPA_HTTP_TXN_COMPLETED_REQUEST, lpa_MessageType_UIM_LPA_MSG_REQUEST, lpa_UimHttpTransactionCompleteReq_fields, sizeof(lpa_UimHttpTransactionCompleteReq), QCRIL_EVT_UIM_LPA_HTTP_REQ, "UIM_LPA_HTTP_REQ"},
    {lpa_MessageId_UIM_LPA_HTTP_TXN_COMPLETED_REQUEST, lpa_MessageType_UIM_LPA_MSG_RESPONSE, lpa_UimHttpTransactionCompleteResp_fields, sizeof(lpa_UimHttpTransactionCompleteResp), QCRIL_EVT_NONE, "UIM_LPA_HTTP_REQ"},
    {lpa_MessageId_UIM_LPA_HTTP_TXN_IND, lpa_MessageType_UIM_LPA_MSG_INDICATION, lpa_UimHttpTransactionInd_fields, sizeof(lpa_UimHttpTransactionInd), QCRIL_EVT_NONE, "UIM_LPA_HTTP_TXN_IND"},
    {lpa_MessageId_UIM_LPA_ADD_PROFILE_PROGRESS_IND, lpa_MessageType_UIM_LPA_MSG_INDICATION, lpa_UimLpaAddProfileProgressInd_fields, sizeof(lpa_UimLpaAddProfileProgressInd), QCRIL_EVT_NONE, "UIM_LPA_ADD_PROFILE_PROGRESS_IND"}
};

size_t qcril_uim_lpa_get_msg_size(lpa_MessageId msg_id, lpa_MessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_uim_lpa_msg_meta_data)/sizeof(qcril_uim_lpa_msg_meta_data[0]); i++)
    {
        if ( qcril_uim_lpa_msg_meta_data[i].msg_id == msg_id &&
             qcril_uim_lpa_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_uim_lpa_msg_meta_data[i].msg_req_size;
        }
    }
    return -1;
}

const pb_field_t* qcril_uim_lpa_get_msg_pb_fields(lpa_MessageId msg_id, lpa_MessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_uim_lpa_msg_meta_data)/sizeof(qcril_uim_lpa_msg_meta_data[0]); i++)
    {
        if ( qcril_uim_lpa_msg_meta_data[i].msg_id == msg_id &&
             qcril_uim_lpa_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_uim_lpa_msg_meta_data[i].msg_pb_fields;
        }
    }
    return NULL;
}

const char* qcril_uim_lpa_get_msg_log_str(lpa_MessageId msg_id, lpa_MessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_uim_lpa_msg_meta_data)/sizeof(qcril_uim_lpa_msg_meta_data[0]); i++)
    {
        if ( qcril_uim_lpa_msg_meta_data[i].msg_id == msg_id &&
             qcril_uim_lpa_msg_meta_data[i].msg_type == msg_type )
        {
            if (qcril_uim_lpa_msg_meta_data[i].msg_log_str)
            {
                return qcril_uim_lpa_msg_meta_data[i].msg_log_str;
            }
            else
            {
                break;
            }
        }
    }
    return "Unknown msg";
}

qcril_evt_e_type qcril_uim_lpa_get_msg_event(lpa_MessageId msg_id, lpa_MessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_uim_lpa_msg_meta_data)/sizeof(qcril_uim_lpa_msg_meta_data[0]); i++)
    {
        if (qcril_uim_lpa_msg_meta_data[i].msg_id == msg_id)
        {
            return qcril_uim_lpa_msg_meta_data[i].msg_event;
        }
    }
    QCRIL_LOG_ERROR("did not find the corresponding event for %s(msg id %d, type %d)", qcril_uim_lpa_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
    return QCRIL_EVT_NONE;
}

boolean qcril_uim_lpa_is_msg_valid(lpa_MessageId msg_id, lpa_MessageType msg_type)
{
    size_t i;
    for (i=0; i<sizeof(qcril_uim_lpa_msg_meta_data)/sizeof(qcril_uim_lpa_msg_meta_data[0]); i++)
    {
        if (qcril_uim_lpa_msg_meta_data[i].msg_id == msg_id &&
             qcril_uim_lpa_msg_meta_data[i].msg_type == msg_type)
        {
            return TRUE;
        }
    }
    QCRIL_LOG_ERROR("did not find a valid entry in the meta table for %s(msg id %d, type %d)", qcril_uim_lpa_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
    return FALSE;
}
