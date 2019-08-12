/******************************************************************************
  @file    qcril_uim_remote_server_packing.c
  @brief   qcril  - uim remote server message packing

  DESCRIPTION
    Handles uim remote server related message packing/unpacking functions.

  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <pb_decode.h>
#include <pb_encode.h>
#include "qcril_uim_remote_server_packing.h"
#include "qcril_log.h"
#include "qcril_uim_remote_server_msg_meta.h"

#define UIM_REMOTE_SERVER_MAX_TAG_SIZE 7

//===========================================================================
// qcril_uim_remote_server_pack_payload
//===========================================================================
pb_bytes_array_t* qcril_uim_remote_server_pack_payload
(
   const void* data,
   uint16_t data_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    pb_bytes_array_t* payload_data = NULL;
    uint16_t payload_len = 0;

    QCRIL_LOG_INFO("copy message payload to pb_bytes_array_t(encoded)");
    payload_len = sizeof(pb_bytes_array_t) + data_len;
    payload_data = (pb_bytes_array_t *) qcril_malloc(payload_len);

    if(payload_data == NULL)
    {
        QCRIL_LOG_ERROR("qcril malloc failed");
        return NULL;
    }

    memset(payload_data, 0, payload_len);
    payload_data->size = (size_t) data_len;
    memcpy(payload_data->bytes, data, data_len);

    return payload_data;
}// qcril_uim_remote_server_pack_payload

//===========================================================================
// qcril_uim_remote_server_pack_msg_tag
//===========================================================================
size_t qcril_uim_remote_server_pack_msg_tag(const void *msg, int msg_len,
                                        uint32_t token,
                                        MsgType type,
                                        MsgId message_id,
                                        Error error,
                                        uint8_t *buf, size_t buf_size)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
        return 0;
    }

    size_t packed_size = 0;
    if(qcril_uim_remote_server_is_msg_valid(message_id, type))
    {
        QCRIL_LOG_ERROR("encoding the MsgHeader");
        MsgHeader msg_tag;
        memset(&msg_tag, 0x00, sizeof(msg_tag));

        msg_tag.token = token;
        msg_tag.type = type;
        msg_tag.id = message_id;
        msg_tag.error = error;
        msg_tag.payload = qcril_uim_remote_server_pack_payload(msg, msg_len);
        pb_ostream_t ostream;

        QCRIL_LOG_INFO("Filled in the tag, encoding the tag");
        ostream = pb_ostream_from_buffer(buf, buf_size);
        if (pb_encode(&ostream, MsgHeader_fields, &msg_tag))
        {
            packed_size = ostream.bytes_written;
            QCRIL_LOG_INFO("encoded the tag");
        }
        else
        {
            QCRIL_LOG_ERROR("pb_encode failed");
        }
    }
    else
    {
        QCRIL_LOG_ERROR("Message id and type doesnt co-exist. Cannot encode the tag");
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) packed_size);
    return packed_size;
} // qcril_uim_remote_server_pack_msg_tag

//===========================================================================
// qcril_uim_remote_server_unpack_msg_tag
//===========================================================================
MsgHeader* qcril_uim_remote_server_unpack_msg_tag(const uint8_t *buf, size_t buf_size)
{
    QCRIL_LOG_FUNC_ENTRY();

    MsgHeader* msg_tag_ptr = NULL;

    if (NULL == buf)
    {
        QCRIL_LOG_ERROR("buf is NULL");
    }
    else
    {
        msg_tag_ptr = qcril_malloc(sizeof(*msg_tag_ptr));
        if (msg_tag_ptr)
        {
            pb_istream_t istream;
            istream = pb_istream_from_buffer((uint8_t *)buf, buf_size);
            if(!pb_decode(&istream, MsgHeader_fields, msg_tag_ptr))
            {
                QCRIL_LOG_ERROR("pb_decode failed");
                qcril_free(msg_tag_ptr);
                return NULL;
            }
        }
        else
        {
            QCRIL_LOG_ERROR("malloc failed");
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return msg_tag_ptr;
} // qcril_uim_remote_server_unpack_msg_tag
