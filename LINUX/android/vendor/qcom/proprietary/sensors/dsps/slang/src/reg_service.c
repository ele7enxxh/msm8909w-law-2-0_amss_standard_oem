/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: reg_service.c
** DESC: sensor language - slang - sensor1 registry service support
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sli.h"
#include "literals.h"
#include "slnew_req.h"

#include "sensor1.h"
#include "sns_smgr_common_v01.h"
#include "sns_reg_api_v02.h"

#include <stdio.h>      // sprintf

// ================
// REGISTRY Service
// ================

// stub where is retval to for returncode?
void RegSetter( request *req_msg, const token *t_keyvalue)
{
    void *body = req_msg->payload;
    int32_t msg_id = req_msg->header.msg_id;

    char *fieldname = t_keyvalue->pSymbol;
    if ( fieldname == NULL) {
        RTErrorMsg( "RegSetReq: pSymbol == NULL\n");
        return;
    }

    if ( msg_id == SNS_REG_SINGLE_READ_REQ_V02) {
        sns_reg_single_read_req_msg_v02 *payload = body;
        if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
            payload->item_id = (uint16_t) t_keyvalue->data.i;
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else if ( msg_id == SNS_REG_SINGLE_WRITE_REQ_V02) {
        sns_reg_single_write_req_msg_v02 *payload = body;
        if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
            payload->item_id = (uint16_t) t_keyvalue->data.i;
        }
        else if ( strcasecmp( fieldname, get_literal( LENGTH)) == 0) {
            payload->data_len = ( uint32_t)t_keyvalue->data.i;
        }
        else if ( strcasecmp( fieldname, get_literal( DATA)) == 0) {
            if ( payload->data_len == 0) {
                RTErrorMsg( get_literal( MISSING_LENGTH));
            }
            else if ( payload->data_len == 1) {
                payload->data[0] = t_keyvalue->data.i & 0xff;
            }
            else if ( payload->data_len == 2) {
                payload->data[0] = t_keyvalue->data.i & 0xff;
                payload->data[1] = ( t_keyvalue->data.i >> 8) & 0xff;
            }
            else if ( payload->data_len == 4) {
                payload->data[0] = t_keyvalue->data.i & 0xff;
                payload->data[1] = ( t_keyvalue->data.i >> 8) & 0xff;
                payload->data[2] = ( t_keyvalue->data.i >> 16) & 0xff;
                payload->data[3] = ( t_keyvalue->data.i >> 24) & 0xff;
            }
            else {
                RTErrorMsg( get_literal( BAD_VALUE));   // length == 8
            }
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else {
        RTErrorMsg( get_literal( BAD_MSG_ID));
    }
}

void RegNewReq( request *req_msg, const array *fieldlist)
{
    if ( fieldlist) {
        int i;
        // expecting field_name == value
        if ( req_msg->header.service_number != SNS_REG2_SVC_ID_V01) {
            RTErrorMsg( "invalid reg req message");
            return;
        }
        // note: arrays actually populate the high_water field !! thus using <= high_water
        for ( i = 0; i <= fieldlist->high_water; ) {
            token *t_keyvalue  = ( token *)get_array_element( fieldlist, i++);
            if ( t_keyvalue == NULL) {
                break;
            }
            if ( t_keyvalue->subtype != KEYVALUE) {
                RTErrorMsg( "invalid key => value argument");
                break;
            }
            RegSetter( req_msg, t_keyvalue);
        }
    }
}

/**
 * @name SmgrSingleReadResp
 * @brief get field values from the p_respind message
 * @param[ i] p_respind
 * @param[ i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[ io] value gets field value
 */
void SmgrSingleReadResp( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_reg_single_read_resp_msg_v02 *pMsg =
        ( sns_reg_single_read_resp_msg_v02 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        t_value->data.i = pMsg->item_id;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( LENGTH)) == 0) {
        t_value->data.i = pMsg->data_len;                   // length of opaque byte array
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATA)) == 0) {
        t_value->data.i = pMsg->item_id;
        t_value->datatype = INTEGER;
        if ( pMsg->data_len == 1) {
            t_value->data.i = pMsg->data[0];
        }
        else if ( pMsg->data_len == 2) {
            t_value->data.i = pMsg->data[0]
                              | ( pMsg->data[1] << 8);
        }
        else if ( pMsg->data_len == 4) {
            t_value->data.i = pMsg->data[0]
                              | ( pMsg->data[1] << 8)
                              | ( pMsg->data[2] << 16)
                              | ( pMsg->data[3] << 24);
        }
        else if ( pMsg->data_len == 8) {

        }
    }
    else {
        InvalidFieldName( fieldname);
    }
}

/**
 * @name SmgrGetter
 * @brief       get field values from the p_respind message
 * @param[ i] p_respind
 * @param[ i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[ io] element
 */
void RegGetter( const respind *p_respind, const token *t_field, token *t_value)
{
//    printf( "SmgrGetter\n");

    SetTypes( t_value, VALUE, SCALAR, UNDEFINED);

    uint32_t service_number = p_respind->header.service_number;
    if ( service_number != SNS_REG2_SVC_ID_V01) {
        FatalError( get_literal( BAD_SRVC_NUM));        // internal logicl error
    }

    uint32_t msg_id = p_respind->header.msg_id;
//    uint16_t msg_size = p_respind->header.msg_size;
    sensor1_msg_type_e msg_type = p_respind->msg_type;

    if ( msg_type == SENSOR1_MSG_TYPE_RESP) {
        if ( msg_id == SNS_REG_VERSION_RESP_V02) {
            ;
        }
        else if ( msg_id == SNS_REG_SINGLE_READ_RESP_V02) {
            SmgrSingleReadResp( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
}

/**
 * @name RegInit
 */

void RegInit( void)
{
    int svc_id = SNS_REG2_SVC_ID_V01;
    service_toc[ svc_id].name = "amd";
    service_toc[ svc_id].new_req_func = (new_req)&RegNewReq;
    service_toc[ svc_id].set_req_func = (set_req)&RegSetter;
    service_toc[ svc_id].getter_func  = (get_func)&RegGetter;

    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.service_number = SNS_REG2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_id = SNS_REG_CANCEL_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_VERSION].header.service_number = SNS_REG2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_id = SNS_REG_VERSION_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.service_number = SNS_REG2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_id = SNS_REG_SINGLE_READ_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_size = sizeof(sns_reg_single_read_req_msg_v02);

    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.service_number = SNS_REG2_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_id = SNS_REG_SINGLE_WRITE_REQ_V02;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_size = sizeof(sns_reg_single_write_req_msg_v02);
}


