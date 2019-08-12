/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: time_service.c
** DESC: sensor language - slang - sensor1 time service support
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sli.h"
#include "literals.h"
#include "slnew_req.h"
#include "slmalloc.h"

#include "sensor1.h"
#include "sns_time_api_v02.h"

#include <stdio.h>      // sprintf

// ===========
// AMD Service
// ===========
void TimeSetReq( request *req_msg, const token *t_keyvalue)
{
    void *body = req_msg->payload;
    int32_t msg_id = req_msg->header.msg_id;

    char *fieldname = t_keyvalue->pSymbol;
    if ( fieldname == NULL) {
        RTErrorMsg( "TimeSetReq: pSymbol == NULL\n");
        return;
    }

    if ( msg_id == SNS_TIME_TIMESTAMP_REQ_V02) {
        sns_time_timestamp_req_msg_v02 *payload = body;
        if ( strcasecmp( fieldname, get_literal( REG_REPORT)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                payload->reg_report_valid = true;
                payload->reg_report       = t_keyvalue->data.i;
            }
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else {
        InvalidFieldName( fieldname);
    }
}

void TimeNewReq( request *req_msg, const array *fieldlist)
{
    if ( fieldlist) {
        int i;
        // expecting field_name == value
        if ( req_msg->header.service_number != SNS_TIME2_SVC_ID_V01) {
            RTErrorMsg( "invalid time timestamp message");
            return;
        }
        for ( i = 0; i <= fieldlist->high_water; ) {
            token *t_keyvalue  = ( token *)get_array_element( fieldlist, i++);
            if ( t_keyvalue == NULL) {
                break;
            }
            if ( t_keyvalue->subtype != KEYVALUE) {
                RTErrorMsg( "invalid key => value argument");
                break;
            }
            TimeSetReq( req_msg, t_keyvalue);
        }
    }
}


/**
 * @name  TimeGetTimestampResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void TimeGetTimestampResp( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_time_timestamp_resp_msg_v02 *pMsg =
        ( sns_time_timestamp_resp_msg_v02 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( SSC_TIMESTAMP)) == 0) {
        if ( pMsg->timestamp_dsps_valid) {
            t_value->data.i = pMsg->timestamp_dsps;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( APPS_TIMESTAMP)) == 0) {
        if ( pMsg->timestamp_apps_valid) {
            t_value->data.i = pMsg->timestamp_apps;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ERROR_CODE)) == 0) {
        if ( pMsg->error_code_valid) {
            t_value->data.i = pMsg->error_code;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SSC_ROLLOVER_COUNT)) == 0) {
        if ( pMsg->dsps_rollover_cnt_valid) {
            t_value->data.i = pMsg->dsps_rollover_cnt;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( APPS_BOOTTIME)) == 0) {
        if ( pMsg->timestamp_apps_boottime_valid) {
            t_value->data.i = pMsg->timestamp_apps_boottime;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  TimeGetTimestampInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void TimeGetTimestampInd( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_time_timestamp_ind_msg_v02 *pMsg =
        ( sns_time_timestamp_ind_msg_v02 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( SSC_TIMESTAMP)) == 0) {
        t_value->data.i = pMsg->timestamp_dsps;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( APPS_TIMESTAMP)) == 0) {
        t_value->data.i = pMsg->timestamp_apps;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSC_ROLLOVER_COUNT)) == 0) {
        t_value->data.i = pMsg->dsps_rollover_cnt;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( APPS_BOOTTIME)) == 0) {
        if ( pMsg->timestamp_apps_boottime_valid) {
            t_value->data.i = pMsg->timestamp_apps_boottime;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/*
 * @name TimeGetter
 * @brief       get field values from the p_respind message
 *
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] element
 */
void TimeGetter( const respind *p_respind, const token *t_field, token *t_value)
{

    SetTypes( t_value, VALUE, SCALAR, UNDEFINED);

    uint32_t service_number = p_respind->header.service_number;
    if ( service_number != SNS_TIME2_SVC_ID_V01) {
        FatalError( get_literal( BAD_SRVC_NUM));
    }

    uint32_t msg_id = p_respind->header.msg_id;
    uint16_t msg_size = p_respind->header.msg_size;
    sensor1_msg_type_e msg_type = p_respind->msg_type;

    if ( msg_type == SENSOR1_MSG_TYPE_RESP) {
        if ( msg_id == SNS_TIME_TIMESTAMP_RESP_V02) {
            TimeGetTimestampResp( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
    else if ( msg_type == SENSOR1_MSG_TYPE_IND) {
        if ( msg_id == SNS_TIME_TIMESTAMP_IND_V02) {
            TimeGetTimestampInd( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
}

/**
 * @name TimeInit
 */

void TimeInit( void)
{
    int svc_id = SNS_TIME2_SVC_ID_V01;
    service_toc[ svc_id].name = "time";
    service_toc[ svc_id].new_req_func = (new_req)&TimeNewReq;
    service_toc[ svc_id].set_req_func = (set_req)&TimeSetReq;
    service_toc[ svc_id].getter_func  = (get_func)&TimeGetter;

    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.service_number = SNS_TIME2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_id = SNS_TIME_CANCEL_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_VERSION].header.service_number = SNS_TIME2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_id = SNS_TIME_VERSION_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.service_number = SNS_TIME2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_id = SNS_TIME_TIMESTAMP_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_size = sizeof(sns_time_timestamp_req_msg_v02);
}



