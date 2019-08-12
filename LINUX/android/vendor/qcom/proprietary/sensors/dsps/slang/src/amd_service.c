/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: amd_service.c
** DESC: sensor language - slang - QMD service support
**       amd, rmd - motion detect
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sam_common.h"

#include "sensor1.h"
#include "sns_smgr_common_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_rmd_v01.h"

#include <stdio.h>      // sprintf

// ===========
// AMD Service
// ===========
void QmdSetReq( request *req_msg, const token *t_keyvalue)
{
    void *body = req_msg->payload;
    int32_t msg_id = req_msg->header.msg_id;

    char *fieldname = t_keyvalue->pSymbol;
    if ( fieldname == NULL) {
        RTErrorMsg( "QmdSetReq: pSymbol == NULL\n");
        return;
    }

    if ( msg_id == SNS_SAM_AMD_ENABLE_REQ_V01) {
        sns_sam_qmd_enable_req_msg_v01 *payload = body;
        if ( strcasecmp( fieldname, get_literal( REPORT_PERIOD)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                uint32_t value = (int)( t_keyvalue->data.i * 65536);                     // convert to Q16
                payload->report_period = value;
            }
            else if ( t_keyvalue->datatype == FLOAT) {
                uint32_t value = (int)( t_keyvalue->data.d * 65536);                     // convert to Q16
                payload->report_period = value;
            }
            else {
                InvalidFieldName( fieldname);
            }
        }
        else if ( strcmp( fieldname, get_literal( NOTIFY_SUSPEND)) == 0) {
            sns_sam_qmd_enable_req_msg_v01 *payload = body;
            payload->notify_suspend_valid = 1;
            payload->notify_suspend.proc_type = 0;
            payload->notify_suspend.send_indications_during_suspend = t_keyvalue->data.i;
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else if ( msg_id == SNS_SAM_AMD_DISABLE_REQ_V01) {
        sns_sam_qmd_disable_req_msg_v01 *payload = body;
        if ( strcmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
            payload->instance_id = (uint8_t)t_keyvalue->data.i;
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else if ( msg_id == SNS_SAM_AMD_GET_REPORT_REQ_V01) {
        sns_sam_qmd_get_report_req_msg_v01 *payload = body;
        if ( strcmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
            payload->instance_id = (uint8_t)t_keyvalue->data.i;
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
}

void QmdNewReq( request *req_msg, const array *fieldlist)
{
    if ( fieldlist) {
        int i;
        for ( i = 0; i <= fieldlist->high_water; ) {
            token *t_keyvalue  = ( token *)get_array_element( fieldlist, i++);
            if ( t_keyvalue == NULL) {
                break;
            }
            if ( t_keyvalue->subtype != KEYVALUE) {
                RTErrorMsg( "invalid key => value argument");
                break;
            }
            QmdSetReq( req_msg, t_keyvalue);
        }
    }
}

void QmdGetEnableResp( const respind *p_respind, const token *t_field, token *t_value)
{
    CommonGetManT3InstanceId( p_respind, t_field, t_value);
}

void QmdGetDisableResp( const respind *p_respind, const token *t_field, token *t_value)
{
    CommonGetManT3InstanceId( p_respind, t_field, t_value);
}


/**
 * @name  QmdGetReportResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void QmdGetReportResp( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_qmd_get_report_resp_msg_v01 *pMsg =
        ( sns_sam_qmd_get_report_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        t_value->data.i = pMsg->instance_id;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        t_value->data.i = pMsg->timestamp;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( STATE)) == 0) {
        t_value->data.i = pMsg->state;
        t_value->datatype = INTEGER;
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name QmdGetAttributesResp
 * @param[i] p_respind
 * @param[i] t_field
 * @param[io] t_value
 */
void QmdGetAttributesResp( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_get_algo_attrib_resp_msg_v01 *pMsg =
        ( sns_sam_get_algo_attrib_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( ALGORITHM_REVISION)) == 0) {
        t_value->data.i = pMsg->algorithm_revision;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( PROC_TYPE)) == 0) {
        t_value->data.i = pMsg->proc_type;      // int32_t
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( REPORTING_MODES)) == 0) {
        t_value->data.i = pMsg->supported_reporting_modes;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MIN_REPORT_RATE)) == 0) {
        t_value->data.i = pMsg->min_report_rate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_REPORT_RATE)) == 0) {
        t_value->data.i = pMsg->max_report_rate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MIN_SAMPLE_RATE)) == 0) {
        t_value->data.i = pMsg->min_sample_rate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_SAMPLE_RATE)) == 0) {
        t_value->data.i = pMsg->max_sample_rate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_BATCH_SIZE)) == 0) {
        t_value->data.i = pMsg->max_batch_size;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( POWER)) == 0) {
        t_value->data.i = pMsg->power;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SUID)) == 0) {
        if ( pMsg->sensorUID_valid) {
            char lit_suid[ 20];
            uint8_t *pHex = ( uint8_t *)&pMsg->sensorUID;
            snprintf( lit_suid, sizeof( lit_suid), "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                     pHex[0], pHex[1], pHex[2], pHex[3],
                     pHex[4], pHex[5], pHex[6], pHex[7]);
            void *p_to   = LatchLiteral( lit_suid, 16);
            t_value->data.p = p_to;
            t_value->datatype = POINTER;

        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  QmdGetReportInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void QmdGetReportInd( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_qmd_report_ind_msg_v01 *pMsg =
        ( sns_sam_qmd_report_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        t_value->data.i = pMsg->instance_id;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        t_value->data.i = pMsg->timestamp;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( STATE)) == 0) {
        t_value->data.i = pMsg->state;
        t_value->datatype = INTEGER;
    }
    else {
        BadFieldName( fieldname);
    }
}

/*
 * @name QmdGetter
 * @brief       get field values from the p_respind message
 *
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] element
 */
void QmdGetter( const respind *p_respind, const token *t_field, token *t_value)
{
    SetTypes( t_value, VALUE, SCALAR, UNDEFINED);

    uint32_t service_number = p_respind->header.service_number;
    if ( !( service_number == SNS_SAM_AMD_SVC_ID_V01 || service_number == SNS_SAM_RMD_SVC_ID_V01)) {
        FatalError( get_literal( BAD_SRVC_NUM));
    }

    uint32_t msg_id = p_respind->header.msg_id;
    uint16_t msg_size = p_respind->header.msg_size;
    sensor1_msg_type_e msg_type = p_respind->msg_type;

    if ( msg_type == SENSOR1_MSG_TYPE_RESP) {
        if ( msg_id == SNS_SAM_AMD_ENABLE_RESP_V01) {
            QmdGetEnableResp( p_respind, t_field, t_value);         // GENERIC w/mandatory_instance_id
        }
        else if ( msg_id == SNS_SAM_AMD_DISABLE_RESP_V01) {
            QmdGetDisableResp( p_respind, t_field, t_value);         // GENERIC w/mandatory_instance_id
        }
        else if ( msg_id == SNS_SAM_AMD_GET_REPORT_RESP_V01) {
            QmdGetReportResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_AMD_GET_ATTRIBUTES_RESP_V01) {
            CommonGetAttributesResp( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
    else if ( msg_type == SENSOR1_MSG_TYPE_IND) {
        if ( msg_id == SNS_SAM_AMD_REPORT_IND_V01) {
            QmdGetReportInd( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
}

/**
 * @name QmdInit
 */

void QmdInit( void)
{
    int svc_id = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].name = "amd";
    service_toc[ svc_id].new_req_func = (new_req)&QmdNewReq;
    service_toc[ svc_id].set_req_func = (set_req)&QmdSetReq;
    service_toc[ svc_id].getter_func  = (get_func)&QmdGetter;

    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_id = SNS_SAM_AMD_CANCEL_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_VERSION].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_id = SNS_SAM_AMD_VERSION_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_id = SNS_SAM_AMD_ENABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_size = sizeof(sns_sam_qmd_enable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_id = SNS_SAM_AMD_DISABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_size = sizeof(sns_sam_qmd_disable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_id = SNS_SAM_AMD_GET_REPORT_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_size = sizeof(sns_sam_qmd_get_report_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.service_number = SNS_SAM_AMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_id = SNS_SAM_AMD_GET_ATTRIBUTES_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_size = 0;

    /*
    ** RMD
    **  */

    svc_id = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].name = "rmd";
    service_toc[ svc_id].new_req_func = (new_req)&QmdNewReq;
    service_toc[ svc_id].set_req_func = (set_req)&QmdSetReq;
    service_toc[ svc_id].getter_func  = (get_func)&QmdGetter;

    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_id = SNS_SAM_RMD_CANCEL_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_VERSION].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_id = SNS_SAM_RMD_VERSION_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_id = SNS_SAM_RMD_ENABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_size = sizeof(sns_sam_qmd_enable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_id = SNS_SAM_RMD_DISABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_size = sizeof(sns_sam_qmd_disable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_id = SNS_SAM_RMD_GET_REPORT_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_size = sizeof(sns_sam_qmd_get_report_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.service_number = SNS_SAM_RMD_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_id = SNS_SAM_RMD_GET_ATTRIBUTES_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_size = 0;
}


