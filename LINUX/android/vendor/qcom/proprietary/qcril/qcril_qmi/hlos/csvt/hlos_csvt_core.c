/***************************************************************************************************
    @file
    hlos_csvt_core.c

    @brief
    Implements functions supported in hlos_csvt_core.h.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#include "hlos_csvt_core.h"
#include "hlos_core.h"
#include "core_handler.h"
#include "cri_csvt_core.h"
#include "cri_core.h"
#include "qcril_qmi_voice.h"
#include "qcril_qmi_ims_misc.h"
#include "qcril_qmi_ims_socket.h"
#include "qcril_qmi_npb_utils.h"

#define HLOS_CSVT_EXTENDED_CODE_MAX_LEN (10)







/***************************************************************************************************
    @function
    hlos_csvt_convert_csvt_call_state_to_ims_call_state

    @brief
    Converts the CRI CSVT call state to HLOS IMS call state.

    @param[in]
        csvt_call_state
           CRI CSVT call state that is being used for the conversion

    @param[out]
        none

    @retval
    HLOS IMS call state
***************************************************************************************************/
static ims_CallState hlos_csvt_convert_csvt_call_state_to_ims_call_state(
    csvt_event_type_enum_v01 csvt_call_state
);


/***************************************************************************************************
    @function
    hlos_csvt_create_ims_calllist

    @brief
    Create ims call list

    @param[in]
        none

    @param[out]
        none

    @retval
    HLOS IMS call list
***************************************************************************************************/
static ims_CallList *hlos_csvt_create_ims_calllist(
        cri_csvt_utils_hlos_call_object_type *cri_csvt_utils_hlos_ongoing_call_objects,
        int number_of_csvt_calls);

/***************************************************************************************************
    @function
    hlos_csvt_free_ims_calllist

    @brief
    Frees IMS Call List object

    @param[in]
        Pointer to IMS Call List object

    @param[out]
        none

    @retval
        none
***************************************************************************************************/
static void hlos_csvt_free_ims_calllist(ims_CallList *call_list);



/***************************************************************************************************
    @function
    hlos_csvt_dial_response_handler

    @brief
    Handles CRI dial response.

    @param[in]
        cri_core_context
           context comprising of HLOS token id and subscription id
        cri_core_error
           error corresponding to the dial request
        hlos_cb_data
           HLOS callback data that was sent as part of dial request
        cri_resp_data
           CRI dial response data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void hlos_csvt_dial_response_handler(cri_core_context_type context,
                                            cri_core_error_type cri_core_error,
                                            void *hlos_cb_data,
                                            void *cri_resp_data);





/***************************************************************************************************
    @function
    hlos_csvt_answer_response_handler

    @brief
    Handles CRI answer response.

    @param[in]
        cri_core_context
           context comprising of HLOS token id and subscription id
        cri_core_error
           error corresponding to the answer request
        hlos_cb_data
           HLOS callback data that was sent as part of answer request
        cri_resp_data
           CRI answer response data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void hlos_csvt_answer_response_handler(cri_core_context_type context,
                                              cri_core_error_type cri_core_error,
                                              void *hlos_cb_data,
                                              void *cri_resp_data);





/***************************************************************************************************
    @function
    hlos_csvt_hangup_response_handler

    @brief
    Handles CRI hangup response.

    @param[in]
        cri_core_context
           context comprising of HLOS token id and subscription id
        cri_core_error
           error corresponding to the hangup request
        hlos_cb_data
           HLOS callback data that was sent as part of hangup request
        cri_resp_data
           CRI hangup response data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void hlos_csvt_hangup_response_handler(cri_core_context_type context,
                                              cri_core_error_type cri_core_error,
                                              void *hlos_cb_data,
                                              void *cri_resp_data);






/***************************************************************************************************
    @function
    hlos_csvt_unsol_ind_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_unsol_ind_handler(CRI_UNUSED(unsigned long message_id),
                                 CRI_UNUSED(void *ind_data),
                                 CRI_UNUSED(int ind_data_len))
{
    ims_CallList *call_list = NULL;
    int number_of_csvt_calls = 0;
    cri_csvt_utils_hlos_call_object_type *cri_csvt_utils_hlos_ongoing_call_objects = NULL;

    UTIL_LOG_MSG("entry");

    number_of_csvt_calls = cri_csvt_utils_retrieve_number_of_ongoing_csvt_calls(FALSE);
    cri_csvt_utils_hlos_ongoing_call_objects =
            cri_csvt_utils_retrieve_hlos_ongoing_call_objects(FALSE);

    call_list = hlos_csvt_create_ims_calllist(cri_csvt_utils_hlos_ongoing_call_objects,
            number_of_csvt_calls);

    if(NULL == call_list)
    {
        UTIL_LOG_MSG("no hlos call objects to be reported");
    }

    qcril_qmi_ims_socket_send_cri(0,
            ims_MsgType_UNSOL_RESPONSE,
            ims_MsgId_UNSOL_RESPONSE_CALL_STATE_CHANGED,
            ims_Error_E_SUCCESS,
            call_list,
            sizeof(ims_CallList));

    hlos_csvt_free_ims_calllist(call_list);

    util_memory_free((void**) &cri_csvt_utils_hlos_ongoing_call_objects);

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_request_handler

    @implementation detail
    None.
***************************************************************************************************/
int hlos_csvt_request_handler(ims_MsgId request_id,
                              uint32_t token_id,
                              void *data,
                              size_t data_len)
{
    int ret_code;
    ims_Dial *ims_dial_req_ptr;
    ims_Hangup *ims_hangup_req_ptr;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;

    UTIL_LOG_MSG("entry");

    ret_code = FALSE;
    ims_dial_req_ptr = NULL;
    ims_hangup_req_ptr = NULL;
    hlos_core_hlos_request_data = NULL;

    switch(request_id)
    {
        case ims_MsgId_REQUEST_DIAL:
            if(data && data_len)
            {
                ims_dial_req_ptr = (ims_Dial*) data;
                if(TRUE == ims_dial_req_ptr->callDetails.has_callDomain &&
                   ims_CallDomain_CALL_DOMAIN_CS == ims_dial_req_ptr->callDetails.callDomain &&
                   TRUE == ims_dial_req_ptr->callDetails.has_callType &&
                   ims_CallType_CALL_TYPE_VT == ims_dial_req_ptr->callDetails.callType)
                {
                    ret_code = TRUE;
                }
            }
            break;

        case ims_MsgId_REQUEST_HANGUP:
            if(data && data_len)
            {
                ims_hangup_req_ptr = (ims_Hangup*) data;
                if(TRUE == cri_csvt_utils_is_hlos_call_id_belongs_to_csvt_call(
                               ims_hangup_req_ptr->conn_index
                           )
                   )
                {
                    ret_code = TRUE;
                }
            }
            break;

        case ims_MsgId_REQUEST_ANSWER:
        case ims_MsgId_REQUEST_GET_CURRENT_CALLS:
        case ims_MsgId_REQUEST_LAST_CALL_FAIL_CAUSE:
            ret_code = cri_csvt_utils_is_csvt_calls_present();
            break;

        default:
            break;
    }

    if(ret_code)
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)
                                            util_memory_alloc(sizeof(*hlos_core_hlos_request_data));
        if(hlos_core_hlos_request_data)
        {
            hlos_core_hlos_request_data->event_id = (unsigned long) request_id;
            hlos_core_hlos_request_data->token_id =
                                            qcril_qmi_ims_convert_ims_token_to_ril_token(token_id);
            hlos_core_hlos_request_data->data = data;
            hlos_core_hlos_request_data->data_len = data_len;
            core_handler_add_event(CORE_HANDLER_HLOS_REQUEST,
                                   hlos_core_hlos_request_data);
        }
    }

    UTIL_LOG_MSG("request id %d, token id %d, data %p, data len %d - is csvt req %d",
                 request_id,
                 token_id,
                 data,
                 data_len,
                 ret_code);
    return ret_code;
}

/***************************************************************************************************
    @function
    hlos_csvt_convert_csvt_call_state_to_ims_call_state

    @implementation detail
    None.
***************************************************************************************************/
ims_CallState hlos_csvt_convert_csvt_call_state_to_ims_call_state(
    csvt_event_type_enum_v01 csvt_call_state
)
{
    ims_CallState ims_call_state;

    ims_call_state = ims_CallState_CALL_DIALING;

    switch(csvt_call_state)
    {
        case CSVT_EVENT_TYPE_CONFIRM_V01:
            ims_call_state = ims_CallState_CALL_DIALING;
            break;

        case CSVT_EVENT_TYPE_PROGRESS_V01:
            ims_call_state = ims_CallState_CALL_ALERTING;
            break;

        case CSVT_EVENT_TYPE_CONNECT_V01:
            ims_call_state = ims_CallState_CALL_ACTIVE;
            break;

        case CSVT_EVENT_TYPE_INCOMING_V01:
            ims_call_state = ims_CallState_CALL_INCOMING;
            break;

        case CSVT_EVENT_TYPE_END_V01:
            ims_call_state = ims_CallState_CALL_END;
            break;

        default:
            ims_call_state = ims_CallState_CALL_DIALING;
            break;
    }

    UTIL_LOG_MSG("ims call state %d",
                 ims_call_state);
    return ims_call_state;
}

/***************************************************************************************************
    @function
    hlos_csvt_dial_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_dial_request_handler(void *event_data)
{
    ims_Dial *event_msg_ptr;
    cri_core_error_type ret_val;
    cri_core_context_type cri_core_context;
    csvt_originate_call_req_msg_v01 csvt_originate_call_req_msg;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;

    UTIL_LOG_MSG("entry");

    event_msg_ptr = NULL;
    ret_val = CRI_ERR_INTERNAL_V01;
    cri_core_context = NIL;
    memset(&csvt_originate_call_req_msg,
           NIL,
           sizeof(csvt_originate_call_req_msg));
    hlos_core_hlos_request_data = NULL;

    if(event_data)
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
        event_msg_ptr = (ims_Dial *) hlos_core_hlos_request_data->data;
        if(event_msg_ptr)
        {
            cri_core_context =
                cri_core_generate_context_using_subscription_id__hlos_token_id(
                    NIL,
                    hlos_core_get_token_id_value(hlos_core_hlos_request_data->token_id)
                );
            strlcpy(csvt_originate_call_req_msg.dial_string,
                    event_msg_ptr->address.arg,
                    sizeof(csvt_originate_call_req_msg.dial_string));
            ret_val = cri_csvt_core_dial_request_handler(cri_core_context,
                                                         (void *)&csvt_originate_call_req_msg,
                                                         event_data,
                                                         hlos_csvt_dial_response_handler);

            if(CRI_ERR_NONE_V01 != ret_val)
            {
                hlos_core_send_response(IMS_PIPE,
                                        ret_val,
                                        event_data,
                                        NULL,
                                        NIL);
                qcril_qmi_npb_release(ims_Dial_fields, event_msg_ptr);
                util_memory_free((void**) &event_msg_ptr);
                util_memory_free((void**) &event_data);
            }
        }
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_dial_response_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_dial_response_handler(cri_core_context_type context,
                                     cri_core_error_type cri_core_error,
                                     void *hlos_cb_data,
                                     CRI_UNUSED(void *cri_resp_data))
{
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    cri_core_subscription_id_type subscription_id;
    cri_core_hlos_token_id_type hlos_token_id;

    UTIL_LOG_MSG("entry");

    hlos_core_hlos_request_data = NULL;
    subscription_id = NIL;
    hlos_token_id = NIL;

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
                                                                  &subscription_id,
                                                                  &hlos_token_id);
    if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) hlos_cb_data;
        hlos_core_send_response(IMS_PIPE,
                                cri_core_error,
                                hlos_cb_data,
                                NULL,
                                NIL);
        qcril_qmi_npb_release(ims_Dial_fields, hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data);
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_answer_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_answer_request_handler(void *event_data)
{
    ims_Answer *event_msg_ptr;
    cri_core_error_type ret_val;
    cri_core_context_type cri_core_context;
    int hlos_call_id;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;

    UTIL_LOG_MSG("entry");

    event_msg_ptr = NULL;
    ret_val = CRI_ERR_INTERNAL_V01;
    cri_core_context = NIL;
    hlos_call_id = NIL;
    hlos_core_hlos_request_data = NULL;


    if(event_data)
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
        event_msg_ptr = (ims_Answer *) hlos_core_hlos_request_data->data;
        if(event_msg_ptr)
        {
            cri_core_context =
                cri_core_generate_context_using_subscription_id__hlos_token_id(
                    NIL,
                    hlos_core_get_token_id_value(hlos_core_hlos_request_data->token_id)
                );

            hlos_call_id = cri_csvt_utils_find_hlos_call_id_in_csvt_call_state(
                                CSVT_EVENT_TYPE_INCOMING_V01
                           );
            if(NIL != hlos_call_id)
            {
                ret_val = cri_csvt_core_answer_request_handler(cri_core_context,
                                                               hlos_call_id,
                                                               TRUE,
                                                               NIL,
                                                               event_data,
                                                               hlos_csvt_answer_response_handler);
            }

            if(CRI_ERR_NONE_V01 != ret_val)
            {
                hlos_core_send_response(IMS_PIPE,
                                        ret_val,
                                        event_data,
                                        NULL,
                                        NIL);
                qcril_qmi_npb_release(ims_Answer_fields, event_msg_ptr);
                util_memory_free((void**) &event_msg_ptr);
                util_memory_free((void**) &event_data);
            }
        }
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_answer_response_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_answer_response_handler(cri_core_context_type context,
                                       cri_core_error_type cri_core_error,
                                       void *hlos_cb_data,
                                       CRI_UNUSED(void *cri_resp_data))
{
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    cri_core_subscription_id_type subscription_id;
    cri_core_hlos_token_id_type hlos_token_id;

    UTIL_LOG_MSG("entry");

    hlos_core_hlos_request_data = NULL;
    subscription_id = NIL;
    hlos_token_id = NIL;

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
                                                                  &subscription_id,
                                                                  &hlos_token_id);
    if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) hlos_cb_data;
        hlos_core_send_response(IMS_PIPE,
                                cri_core_error,
                                hlos_cb_data,
                                NULL,
                                NIL);
        qcril_qmi_npb_release(ims_Answer_fields, hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data);
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_hangup_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_hangup_request_handler(void *event_data)
{
    ims_Hangup *event_msg_ptr;
    cri_core_error_type ret_val;
    cri_core_context_type cri_core_context;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    ims_CallFailCause fail_cause;

    UTIL_LOG_MSG("entry");

    event_msg_ptr = NULL;
    ret_val = CRI_ERR_INTERNAL_V01;
    cri_core_context = NIL;
    hlos_core_hlos_request_data = NULL;
    fail_cause = ims_CallFailCause_CALL_FAIL_NORMAL;

    if(event_data)
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
        event_msg_ptr = (ims_Hangup *) hlos_core_hlos_request_data->data;
        if(event_msg_ptr)
        {
            cri_core_context =
                cri_core_generate_context_using_subscription_id__hlos_token_id(
                    NIL,
                    hlos_core_get_token_id_value(hlos_core_hlos_request_data->token_id)
                );

            if(TRUE == cri_csvt_utils_is_csvt_call_with_hlos_call_id_in_csvt_call_state(
                            event_msg_ptr->conn_index,
                            CSVT_EVENT_TYPE_INCOMING_V01
                       )
               )
            {
                if (event_msg_ptr->has_failCauseResponse &&
                    event_msg_ptr->failCauseResponse.has_failcause)
                {
                    fail_cause = event_msg_ptr->failCauseResponse.failcause;
                }
                ret_val = cri_csvt_core_answer_request_handler(cri_core_context,
                                                               event_msg_ptr->conn_index,
                                                               FALSE,
                                                               fail_cause,
                                                               event_data,
                                                               hlos_csvt_hangup_response_handler);
            }
            else
            {
                ret_val = cri_csvt_core_end_request_handler(cri_core_context,
                                                            event_msg_ptr->conn_index,
                                                            event_data,
                                                            hlos_csvt_hangup_response_handler);
            }

            if(CRI_ERR_NONE_V01 != ret_val)
            {
                hlos_core_send_response(IMS_PIPE,
                                        ret_val,
                                        event_data,
                                        NULL,
                                        NIL);
                qcril_qmi_npb_release(ims_Hangup_fields, event_msg_ptr);
                util_memory_free((void**) &event_msg_ptr);
                util_memory_free((void**) &event_data);
            }
        }
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_hangup_response_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_hangup_response_handler(cri_core_context_type context,
                                       cri_core_error_type cri_core_error,
                                       void *hlos_cb_data,
                                       CRI_UNUSED(void *cri_resp_data))
{
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    cri_core_subscription_id_type subscription_id;
    cri_core_hlos_token_id_type hlos_token_id;

    UTIL_LOG_MSG("entry");

    hlos_core_hlos_request_data = NULL;
    subscription_id = NIL;
    hlos_token_id = NIL;

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
                                                                  &subscription_id,
                                                                  &hlos_token_id);
    if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) hlos_cb_data;
        hlos_core_send_response(IMS_PIPE,
                                cri_core_error,
                                hlos_cb_data,
                                NULL,
                                NIL);
        qcril_qmi_npb_release(ims_Hangup_fields, hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data->data);
        util_memory_free((void**) &hlos_core_hlos_request_data);
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    hlos_csvt_get_current_calls_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_get_current_calls_request_handler(void *event_data)
{
    cri_core_error_type ret_val;
    ims_CallList *call_list = NULL;
    int number_of_csvt_calls = 0;
    cri_csvt_utils_hlos_call_object_type *cri_csvt_utils_hlos_ongoing_call_objects = NULL;

    UTIL_LOG_MSG("entry");

    ret_val = CRI_ERR_NONE_V01;

    if(event_data)
    {
        number_of_csvt_calls = cri_csvt_utils_retrieve_number_of_ongoing_csvt_calls(TRUE);
        cri_csvt_utils_hlos_ongoing_call_objects =
                cri_csvt_utils_retrieve_hlos_ongoing_call_objects(TRUE);

        call_list = hlos_csvt_create_ims_calllist(cri_csvt_utils_hlos_ongoing_call_objects,
                number_of_csvt_calls);

        if(NULL == call_list)
        {
            UTIL_LOG_MSG("no hlos call objects to be reported");
        }

        hlos_core_send_response(IMS_PIPE,
                                ret_val,
                                event_data,
                                call_list,
                                sizeof (ims_CallList));

        util_memory_free((void**) &event_data);
    }

    util_memory_free((void**) &cri_csvt_utils_hlos_ongoing_call_objects);
    hlos_csvt_free_ims_calllist(call_list);

    UTIL_LOG_MSG("exit");
}


/***************************************************************************************************
    @function
    hlos_csvt_create_ims_calllist

    @implementation detail
    None.
***************************************************************************************************/
ims_CallList *hlos_csvt_create_ims_calllist(
        cri_csvt_utils_hlos_call_object_type *cri_csvt_utils_hlos_ongoing_call_objects,
        int number_of_csvt_calls
)
{
    ims_CallList       *call_list    = NULL;
    ims_CallList_Call *call          = NULL;
    boolean is_failed            = FALSE;
    char    *error_info_data     = NULL;
    int     iter_resp_object     = 0;
    const uint8 MAX_EXTRAS_PER_CALL = 5;
    char** call_details_extras;

    UTIL_LOG_MSG("entry");

    do
    {
        UTIL_LOG_MSG("number_of_csvt_calls = %d, cri_csvt_utils_hlos_ongoing_call_objects = %p",
                number_of_csvt_calls, cri_csvt_utils_hlos_ongoing_call_objects);
        if (cri_csvt_utils_hlos_ongoing_call_objects == NULL || number_of_csvt_calls == 0)
        {
            break;
        }

        call_list = qcril_malloc(sizeof (ims_CallList));
        if (call_list == NULL)
        {
            is_failed = TRUE;
            break;
        }
        call_list->callAttributes.arg = qcril_malloc(sizeof (ims_CallList_Call*) *
                                                      (number_of_csvt_calls+1));
        if (call_list->callAttributes.arg == NULL)
        {
            is_failed = TRUE;
            break;
        }

        for ( iter_resp_object = 0;
              iter_resp_object < number_of_csvt_calls;
              iter_resp_object++ )
        {
            call = qcril_malloc(sizeof(ims_CallList_Call));
            call_details_extras = qcril_malloc(sizeof(char*) *
                                               number_of_csvt_calls *
                                               (MAX_EXTRAS_PER_CALL+1));
            if (call == NULL || call_details_extras == NULL)
            {
                is_failed = TRUE;
                break;
            }

            ((ims_CallList_Call**)call_list->callAttributes.arg)[iter_resp_object] = call;
            call->has_state = TRUE;
            call->state     =
                hlos_csvt_convert_csvt_call_state_to_ims_call_state(
                 cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].csvt_call_state
                );
            call->has_index = TRUE;
            call->index     =
                cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].hlos_call_id;
            call->has_isMpty = TRUE;
            call->isMpty    = FALSE;
            call->has_isVoice = TRUE;
            call->isVoice   = FALSE;
            call->name.arg  = NULL;
            call->number.arg   =
             cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].remote_party_number;
            call->has_numberPresentation = TRUE;
            call->numberPresentation = QCRIL_QMI_VOICE_RIL_PI_ALLOWED;
            call->has_isMT  = TRUE;
            call->isMT      =
                cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].is_mt;
            call->has_toa   = TRUE;
            if ( cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].remote_party_number[0]
                    == QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX )
            {
              call->toa = QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL;
            }
            else
            {
              call->toa = QCRIL_QMI_VOICE_SS_TA_UNKNOWN;
            }

            call->has_callDetails = TRUE;
            call->callDetails.has_callType = TRUE;
            call->callDetails.callType = ims_CallType_CALL_TYPE_VT;
            call->callDetails.has_callDomain = TRUE;
            call->callDetails.callDomain = ims_CallDomain_CALL_DOMAIN_CS;

            if (call->state == ims_CallState_CALL_END)
            {
                error_info_data = util_memory_alloc(sizeof(char) * HLOS_CSVT_EXTENDED_CODE_MAX_LEN);
                if (error_info_data == NULL)
                {
                    is_failed = TRUE;
                    break;
                }
                call->failCause.has_failcause = TRUE;
                call->failCause.failcause = ims_CallFailCause_CALL_FAIL_MISC;
                snprintf(error_info_data, HLOS_CSVT_EXTENDED_CODE_MAX_LEN,
                         "%d",
                         cri_csvt_utils_hlos_ongoing_call_objects[iter_resp_object].call_fail_cause);
                qcril_binary_data_type bin_data;
                bin_data.len =  strlen(error_info_data);
                bin_data.data =  (uint8_t*) error_info_data;
                call->failCause.errorinfo.arg = (void*)&bin_data;

            }
        }

        if (is_failed)
        {
            break;
        }

        for ( iter_resp_object = 0;
              iter_resp_object < number_of_csvt_calls;
              iter_resp_object++ )
        {
            UTIL_LOG_MSG("hlos call object id %d, call id %d, state %d, is mt %d, number %s",
                         (iter_resp_object + 1),
                         ((ims_CallList_Call**)call_list->callAttributes.arg)[iter_resp_object]->index,
                         ((ims_CallList_Call**)call_list->callAttributes.arg)[iter_resp_object]->state,
                         ((ims_CallList_Call**)call_list->callAttributes.arg)[iter_resp_object]->isMT,
                         ((ims_CallList_Call**)call_list->callAttributes.arg)[iter_resp_object]->number);
        }
    } while (FALSE);

    if (is_failed)
    {
        UTIL_LOG_MSG("failed to create call list");
        hlos_csvt_free_ims_calllist(call_list);
        call_list = NULL;
    }

    UTIL_LOG_MSG("exit");

    return call_list;
}

/***************************************************************************************************
    @function
    hlos_csvt_free_ims_calllist

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_free_ims_calllist(ims_CallList *call_list)
{
    if (call_list)
    {
        if ( call_list->callAttributes.arg )
        {
          util_memory_free((void**) &call_list->callAttributes.arg);
        }
        util_memory_free((void**) &call_list);
    }
}

/***************************************************************************************************
    @function
    hlos_csvt_last_call_failure_cause_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void hlos_csvt_last_call_failure_cause_request_handler(void *event_data)
{
    cri_core_error_type ret_val;
    char error_info_data[HLOS_CSVT_EXTENDED_CODE_MAX_LEN + 1];
    uint32_t csvt_last_call_failure_cause;
    void *last_call_failure_cause_payload;
    size_t last_call_failure_cause_payload_len;

    UTIL_LOG_MSG("entry");

    ret_val = CRI_ERR_INTERNAL_V01;
    memset(error_info_data,
           NIL,
           sizeof(error_info_data));
    csvt_last_call_failure_cause = NIL;
    last_call_failure_cause_payload = NULL;
    last_call_failure_cause_payload_len = NIL;

    if(event_data)
    {
        csvt_last_call_failure_cause = cri_csvt_utils_retrieve_last_call_failure_cause();
        if(NIL != csvt_last_call_failure_cause)
        {
            ims_CallFailCauseResponse last_call_failure_cause;
            memset(&last_call_failure_cause, 0, sizeof(last_call_failure_cause));
            last_call_failure_cause.has_failcause = TRUE;
            last_call_failure_cause.failcause =  ims_CallFailCause_CALL_FAIL_MISC;

            snprintf(error_info_data, sizeof(error_info_data),
                     "%d",
                     csvt_last_call_failure_cause);
            qcril_binary_data_type bin_data;
            bin_data.len =  strlen(error_info_data);
            bin_data.data =  (uint8_t*) error_info_data;
            last_call_failure_cause.errorinfo.arg = (void*)&bin_data;

            last_call_failure_cause_payload = &last_call_failure_cause;
            last_call_failure_cause_payload_len = sizeof(last_call_failure_cause);

            ret_val = CRI_ERR_NONE_V01;
        }
        hlos_core_send_response(IMS_PIPE,
                                ret_val,
                                event_data,
                                last_call_failure_cause_payload,
                                last_call_failure_cause_payload_len);

        util_memory_free((void**) &event_data);
    }

    UTIL_LOG_MSG("exit");
}

