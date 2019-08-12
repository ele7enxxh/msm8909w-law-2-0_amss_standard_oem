/******************************************************************************
#  @file    qcril_uim_lpa_socket.cc
#  @brief   Local profile assistant socket file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#define __STDC_FORMAT_MACROS 1
#include <pthread.h>
#include "qcril_uim_lpa_socket.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <pb_decode.h>

#ifdef QMI_RIL_UTF
#include "unistd.h"
#include "netinet/ip.h"
extern "C" uint32 qcril_get_time_milliseconds();
#endif

#if defined(FEATURE_TARGET_GLIBC_x86)  || defined(QMI_RIL_UTF)
    extern "C" size_t strlcat(char *, const char *, size_t);
    extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

extern "C" {
    #include "qcril_log.h"
    #include "qcrili.h"
    #include "qcril_uim_lpa_packing.h"
    #include "qcril_uim_lpa_msg_meta.h"
    #include "qcril_reqlist.h"
}
#include "qcril_qmi_singleton_agent.h"

#define QCRIL_UIM_LPA_SOCKET_PATH "/dev/socket/qmux_radio/uim_lpa_socket"

class qcril_uim_lpa_socket_agent : public qcril_qmi_singleton_agent<qcril_uim_lpa_socket_agent>
{
public:
            qcril_uim_lpa_socket_agent();
    boolean is_socket_server_started() { return (listen_sid != 0); }
    boolean is_socket_connected()  { return (conn_sid >= 0); }
    void    close_socket_connection();
   boolean  send_message(bool has_token, RIL_Token token,
                             lpa_MessageType type,
                             lpa_MessageId message_id,
                             bool has_error,
                             lpa_Error error,
                             const void* msg, int msg_len);


private:
    void    thread_func();
    int     get_message_size();
    void    recv_thread_handler();
    boolean init_socket_listenfd();
    boolean process_incoming_message();

private:
    int              token;
    int              inited;
    int              listen_sid;
    int              conn_sid;
    pthread_t        thread_id;
    static const int MSG_OFFSET = 4;

    int              recvd_byte_num;
    uint8_t          recv_buffer[QCRIL_UIM_LPA_SOCKET_MAX_BUF_SIZE];
    uint8_t          send_buffer[QCRIL_UIM_LPA_SOCKET_MAX_BUF_SIZE];
};

//===========================================================================
// qcril_uim_lpa_socket_agent::qcril_uim_lpa_socket_agent
//===========================================================================
qcril_uim_lpa_socket_agent::qcril_uim_lpa_socket_agent() :
   qcril_qmi_singleton_agent<qcril_uim_lpa_socket_agent>(QMI_RIL_UIM_LPA_SOCKET_THREAD_NAME),
   token(0),
   listen_sid(0),
   conn_sid(0),
   recvd_byte_num(0)
{} // qcril_uim_lpa_socket_agent::qcril_uim_lpa_socket_agent

//===========================================================================
// qcril_uim_lpa_socket_agent::thread_func
//===========================================================================
void qcril_uim_lpa_socket_agent::thread_func()
{
    QCRIL_LOG_FUNC_ENTRY();

    if (listen_sid == 0)
    {
        if (!init_socket_listenfd())
        {
            recv_thread_handler();
        }
        else
        {
            listen_sid = 0;
        }
    }
    else
    {
        QCRIL_LOG_DEBUG("socket already initialized.");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_uim_lpa_socket_agent::thread_func

//===========================================================================
// qcril_uim_lpa_socket_agent::send_message
//===========================================================================
boolean qcril_uim_lpa_socket_agent::send_message(bool has_token, RIL_Token token,
                                                 lpa_MessageType type,
                                                 lpa_MessageId message_id,
                                                 bool has_error,
                                                 lpa_Error error,
                                                 const void* msg,
                                                 QCRIL_UNUSED(int msg_len))
{
    boolean ret = 0;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("type: %d, message_id: %d, error: %d", type, message_id, error);

    if (is_socket_connected())
    {
        size_t msg_size = 0;
        if(has_token)
        {
            msg_size = qcril_uim_lpa_pack_msg_and_tag((void*) msg, TRUE,
                                                       qcril_uim_lpa_free_and_convert_ril_token_to_uim_token(token),
                                                       type, message_id, has_error, error, send_buffer+4, sizeof(send_buffer)-4);
        }
        else
        {
            msg_size = qcril_uim_lpa_pack_msg_and_tag((void*) msg, FALSE, 0,
                                                       type, message_id, has_error, error, send_buffer+4, sizeof(send_buffer)-4);
        }
        QCRIL_LOG_INFO("msg_size: %d",msg_size);

        unsigned char *tmp = (unsigned char*) send_buffer;
        tmp[0] = (unsigned char)((msg_size) >> 24);
        tmp[1] = (unsigned char)(((msg_size) >> 16 ) & 0xff);
        tmp[2] = (unsigned char)(((msg_size) >> 8 ) & 0xff);
        tmp[3] = (unsigned char)((msg_size) & 0xff);

        qcril_qmi_print_hex(send_buffer, msg_size+4);
        QCRIL_LOG_INFO("Sending response");
        send(conn_sid, send_buffer, msg_size+4, 0);
    }
    else
    {
        QCRIL_LOG_INFO("socket is not connected");
        ret = 1;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
    return ret;
} // qcril_uim_lpa_socket_agent::send_message

//===========================================================================
// qcril_uim_lpa_socket_agent::get_message_size
//===========================================================================
int qcril_uim_lpa_socket_agent::get_message_size()
{
    int size = -1;
    if (recvd_byte_num >= MSG_OFFSET)
    {
        size = 0;
        // message size will be in 4 bytes(MSG_OFFSET) big-endian format
        for (int i=0; i<MSG_OFFSET; i++)
        {
            size = (size << 8) + recv_buffer[i];
        }
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(size);
    return size;
} // qcril_uim_lpa_socket_agent::get_message_size

//===========================================================================
// qcril_uim_lpa_socket_agent::process_incoming_message
//===========================================================================
boolean qcril_uim_lpa_socket_agent::process_incoming_message()
{
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO("received %d bytes", recvd_byte_num);
    qcril_qmi_print_hex(recv_buffer, recvd_byte_num);

    boolean err = FALSE;
    int size = get_message_size();

    do
    {
        if ((size < 0) || (size > INT_MAX - MSG_OFFSET))
        {
            err = TRUE;
            break;
        }

        if (recvd_byte_num < size+MSG_OFFSET )
        {
            err = TRUE;
            break;
        }

        size_t unpacked_msg_size = 0;

        lpa_MessageTag *msg_tag_ptr = NULL;
        void *msg_data_ptr = NULL;

        qcril_evt_e_type uim_req;
        QCRIL_LOG_INFO("unpacking Message tag");
        msg_tag_ptr = qcril_uim_lpa_unpack_msg_tag(recv_buffer+MSG_OFFSET, size);

        if(msg_tag_ptr)
        {

             if(msg_tag_ptr->has_error)
             {
               QCRIL_LOG_INFO( "msg: %s, type: %d, message_id: %d, error: %d",
                               qcril_uim_lpa_get_msg_log_str(msg_tag_ptr->id, msg_tag_ptr->type),
                               msg_tag_ptr->type,
                               msg_tag_ptr->id,
                               msg_tag_ptr->error);
             }
             else
             {
               QCRIL_LOG_INFO( "msg: %s, type: %d, message_id: %d",
                               qcril_uim_lpa_get_msg_log_str(msg_tag_ptr->id, msg_tag_ptr->type),
                               msg_tag_ptr->type,
                               msg_tag_ptr->id);
             }


            qcril_uim_lpa_parse_packed_msg(msg_tag_ptr->type, msg_tag_ptr->id, msg_tag_ptr->payload, size, &msg_data_ptr, &unpacked_msg_size, &uim_req);

            if(msg_tag_ptr->has_token)
            {
                qcril_event_queue( qmi_ril_get_process_instance_id(),
                                   QCRIL_DEFAULT_MODEM_ID,
                                   QCRIL_DATA_NOT_ON_STACK,
                                   (qcril_evt_e_type) uim_req,
                                   msg_data_ptr,
                                   unpacked_msg_size,
                                   qcril_uim_lpa_convert_uim_token_to_ril_token(msg_tag_ptr->token) );
            }
            else
            {
                qcril_event_queue( qmi_ril_get_process_instance_id(),
                                   QCRIL_DEFAULT_MODEM_ID,
                                   QCRIL_DATA_NOT_ON_STACK,
                                   (qcril_evt_e_type) uim_req,
                                   msg_data_ptr,
                                   unpacked_msg_size,
                                   qcril_uim_lpa_convert_uim_token_to_ril_token(token) );
            }
            QCRIL_LOG_INFO("Freeing msg_tag_ptr after queing the request");
            qcril_qmi_npb_release(lpa_MessageTag_fields, msg_tag_ptr);
            qcril_free(msg_tag_ptr);
        }
    } while (FALSE);
    QCRIL_LOG_FUNC_RETURN();
    return err;
} // qcril_uim_lpa_socket_agent::process_incoming_message

//===========================================================================
// qcril_uim_lpa_socket_agent::init_socket_listenfd
//===========================================================================
boolean qcril_uim_lpa_socket_agent::init_socket_listenfd()
{
    QCRIL_LOG_FUNC_ENTRY();

    struct sockaddr_un servaddr;
    /* creating a socket */
    if ((listen_sid = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
       QCRIL_LOG_ERROR("Error in socket");
       return 1;
    }

    /* configuring server address structure */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family      = AF_UNIX;
    snprintf(servaddr.sun_path, sizeof(servaddr.sun_path), "%s%d", QCRIL_UIM_LPA_SOCKET_PATH, qmi_ril_get_process_instance_id());
    unlink(servaddr.sun_path);

    QCRIL_LOG_INFO("bind to socket path %s", servaddr.sun_path);
    /* binding our socket to the service port */
    if (bind(listen_sid, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
       QCRIL_LOG_ERROR("Error in bind");
       return 1;
    }

    QCRIL_LOG_INFO("listen ...");
    /* convert our socket to a listening socket */
    if (listen(listen_sid, 0) < 0) //just one client for the server
    {
       QCRIL_LOG_ERROR("Error in listen");
       return 1;
    }
    QCRIL_LOG_INFO("listen socket init finished");

    return 0;
} // qcril_uim_lpa_socket_agent::init_socket_listenfd

//===========================================================================
// qcril_uim_lpa_socket_agent::recv_thread_handler
//===========================================================================
void qcril_uim_lpa_socket_agent::recv_thread_handler()
{
    QCRIL_LOG_FUNC_ENTRY();
    struct sockaddr_un cliaddr;
    socklen_t          clilen = sizeof(cliaddr);
    int newly_recvd_byte_num;
    int msg_size;

    do
    {
        if ((conn_sid = accept(listen_sid, (struct sockaddr*) &cliaddr, &clilen)) < 0)
        {
            // TODO: ERROR handling
            QCRIL_LOG_ERROR("accept failed. conn_sid: %d", conn_sid);
        }
        else
        {
            QCRIL_LOG_INFO("client connected with conn_sid %d", conn_sid);
            recvd_byte_num = 0;

            while (TRUE)
            {
                newly_recvd_byte_num = recv( conn_sid,
                                             recv_buffer + recvd_byte_num,
                                             sizeof(recv_buffer) - recvd_byte_num,
                                             0 );


                if (newly_recvd_byte_num <= 0)
                {
                    QCRIL_LOG_ERROR("recv failed. recv_byte_num: %d. closing the socket.", recvd_byte_num);
                    close(conn_sid);
                    break;
                }
                else
                {
                    recvd_byte_num += newly_recvd_byte_num;

                    while (!process_incoming_message())
                    {
                        msg_size = get_message_size();
                        recvd_byte_num = recvd_byte_num - msg_size - MSG_OFFSET;
                        if (recvd_byte_num > 0)
                        {
                            memcpy(recv_buffer, recv_buffer + msg_size + MSG_OFFSET, recvd_byte_num);
                        }
                    }
                }
            }
        }
    } while (TRUE);
    QCRIL_LOG_FUNC_RETURN();
} // qcril_uim_lpa_socket_agent::recv_thread_func

extern "C" {

//===========================================================================
// qcril_uim_lpa_socket_init
//===========================================================================
void qcril_uim_lpa_socket_init()
{
    QCRIL_LOG_FUNC_ENTRY();
    char *threadName = (char *)qcril_malloc(QCRIL_UIM_LPA_SOCKET_MAX_THREAD_NAME_SIZE);

    if(threadName)
    {
      snprintf(threadName, QCRIL_UIM_LPA_SOCKET_MAX_THREAD_NAME_SIZE, "%s%d", QMI_RIL_UIM_LPA_SOCKET_THREAD_NAME, qmi_ril_get_process_instance_id());
      QCRIL_LOG_INFO("..Create UIM thread on SUB%d with name %s", qmi_ril_get_process_instance_id(), threadName);
      qcril_uim_lpa_socket_agent::get_instance()->init(threadName);
      qcril_free(threadName);
    }
    else
    {
      QCRIL_LOG_INFO("..Failed to allocate Memory");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_uim_lpa_socket_init

//===========================================================================
// qcril_uim_lpa_socket_send
void qcril_uim_lpa_socket_send(bool has_token, RIL_Token token,
                                  lpa_MessageType type,
                                  lpa_MessageId message_id,
                                  bool has_error,
                                  lpa_Error error,
                                  const void* msg, int msg_len)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , token );
    QCRIL_LOG_INFO("socket send message");
    qcril_uim_lpa_socket_agent::get_instance()->send_message(has_token, token, type,
                                                                message_id, has_error,
                                                                error, msg, msg_len);
    QCRIL_LOG_FUNC_RETURN();
} // qcril_uim_lpa_socket_send

//===========================================================================
// qcril_uim_lpa_socket_send_empty_payload_unsol_resp
//===========================================================================
void qcril_uim_lpa_socket_send_empty_payload_unsol_resp(lpa_MessageId msg_id)
{
    qcril_uim_lpa_socket_send(FALSE, 0,
                              lpa_MessageType_UIM_LPA_MSG_INDICATION,
                              msg_id, TRUE,
                              lpa_Error_UIM_LPA_ERR_SUCCESS, NULL, 0);
} // qcril_uim_lpa_socket_send_empty_payload_unsol_resp

} // end of extern "C"


