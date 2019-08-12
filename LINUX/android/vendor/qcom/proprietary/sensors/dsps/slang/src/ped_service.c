/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: ped_service.c
** DESC: sensor language - slang - sam pedometer service support
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sam_common.h"

#include "sensor1.h"
#include "sns_smgr_common_v01.h"
#include "sns_sam_ped_v01.h"

#include <stdio.h>      // sprintf

void PedSetReq( request *req_msg, const token *t_keyvalue)
{
    void *body = req_msg->payload;
    int32_t msg_id = req_msg->header.msg_id;

    char *fieldname = t_keyvalue->pSymbol;
    if ( fieldname == NULL) {
        RTErrorMsg( "PedSetReq: pSymbol == NULL\n");
        return;
    }

    if ( msg_id == SNS_SAM_PED_ENABLE_REQ_V01) {
        sns_sam_ped_enable_req_msg_v01 *payload = body;
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
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else if ( strcasecmp( fieldname, get_literal( SAMPLERATE)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                payload->sample_rate_valid = 1;
                uint32_t value = (int)( t_keyvalue->data.i * 65536);                     // convert to Q16
                payload->sample_rate = value;
            }
            else if ( t_keyvalue->datatype == FLOAT) {
                payload->sample_rate_valid = 1;
                uint32_t value = (int)( t_keyvalue->data.d * 65536);                     // convert to Q16
                payload->sample_rate = value;
            }
            else {
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else if ( strcasecmp( fieldname, get_literal( STEP_COUNT_THRESHOLD)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                payload->step_count_threshold_valid = 1;
                payload->step_count_threshold = ( uint32_t)t_keyvalue->data.i;
            }
            else {
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else if ( strcasecmp( fieldname, get_literal( DUTY_CYCLE_ON_PERCENT)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                payload->duty_cycle_on_percent_valid = 1;
                payload->duty_cycle_on_percent = ( uint8_t)t_keyvalue->data.i;
            }
            else {
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else if ( strcmp( fieldname, get_literal( NOTIFY_SUSPEND)) == 0) {
            payload->notify_suspend_valid = 1;
            payload->notify_suspend.proc_type = 0;
            payload->notify_suspend.send_indications_during_suspend = t_keyvalue->data.i;
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else if ( msg_id == SNS_SAM_PED_DISABLE_REQ_V01) {
        CommonSetManT1InstanceId( body, t_keyvalue);
    }
    else if ( msg_id == SNS_SAM_PED_GET_REPORT_REQ_V01) {
        CommonSetManT1InstanceId( body, t_keyvalue);
    }
    else if ( msg_id == SNS_SAM_PED_RESET_REQ_V01) {
        CommonSetManT1InstanceId( body, t_keyvalue);
    }
    else  if ( msg_id == SNS_SAM_PED_BATCH_REQ_V01) {      // stub note sameas grav
        sns_sam_ped_batch_req_msg_v01 *payload = body;
        if ( strcmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
            payload->instance_id = (uint8_t)t_keyvalue->data.i;
        }
        else if ( strcasecmp( fieldname, get_literal( BATCH_PERIOD)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                uint32_t value = (int)( t_keyvalue->data.i * 65536);        // convert to Q16
                payload->batch_period = value;
            }
            else if ( t_keyvalue->datatype == FLOAT) {
                uint32_t value = (int)( t_keyvalue->data.d * 65536);        // convert to Q16
                payload->batch_period = value;
            }
            else {
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else  if ( msg_id == SNS_SAM_PED_UPDATE_BATCH_PERIOD_REQ_V01) {     // stub note sameas grav
        sns_sam_ped_update_batch_period_req_msg_v01 *payload = body;
        if ( strcmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
            payload->instance_id = (uint8_t)t_keyvalue->data.i;
        }
        else if ( strcasecmp( fieldname, get_literal( BATCH_PERIOD)) == 0) {
            if ( t_keyvalue->datatype == INTEGER) {
                uint32_t value = (int)( t_keyvalue->data.i * 65536);                     // convert to Q16
                payload->active_batch_period = value;
            }
            else if ( t_keyvalue->datatype == FLOAT) {
                uint32_t value = (int)( t_keyvalue->data.d * 65536);                     // convert to Q16
                payload->active_batch_period = value;
            }
            else {
                ErrReasonForField( BAD_VALUE, fieldname);
            }
        }
        else {
            InvalidFieldName( fieldname);
        }
    }
    else {
        // TODO ErrReasonForField( BAD_MESSAGE, fieldname);
    }
}

// ServiceNewReq really GENERIC...
void PedNewReq( request *req_msg, const array *fieldlist)
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
            PedSetReq( req_msg, t_keyvalue);
        }
    }
}

void PedGetEnableResp( const respind *p_respind, const token *t_field, token *t_value)
{
    CommonGetOptT3InstanceId( p_respind, t_field, t_value);
}

void PedGetDisableResp( const respind *p_respind, const token *t_field, token *t_value)
{
    CommonGetOptT3InstanceId( p_respind, t_field, t_value);
}

void PedGetResetResp( const respind *p_respind, const token *t_field, token *t_value)
{
    CommonGetOptT3InstanceId( p_respind, t_field, t_value);
}

/**
 * @name  PedGetReportData
 * @brief   get value of fieldname from the
 *          sns_sam_ped_result_s_v01 structure
 * @param[i] p_result
 * @param[i] fieldname
 * @param[io] t_value   - gets field value
 * @returns 0 == successful, <> 0 == invalid field name
 */
int PedGetReportData( const sns_sam_ped_report_data_s_v01 *p_report, const char *fieldname, token *t_value)
{
    int rc = 0;     // successful

    if ( strcasecmp( fieldname, get_literal( STEP_EVENT)) == 0) {
        t_value->datatype = INTEGER;
        t_value->data.i = p_report->step_event;
    }
    else if ( strcasecmp( fieldname, get_literal( STEP_CONFIDENCE)) == 0) {
        t_value->datatype = INTEGER;
        t_value->data.i = p_report->step_confidence;
    }
    else if ( strcasecmp( fieldname, get_literal( STEP_COUNT)) == 0) {
        t_value->datatype = INTEGER;
        t_value->data.i = p_report->step_count;
    }
    else if ( strcasecmp( fieldname, get_literal( STEP_COUNT_ERROR)) == 0) {
        t_value->datatype = INTEGER;
        t_value->data.i = p_report->step_count_error;
    }
    else if ( strcasecmp( fieldname, get_literal( STEP_RATE)) == 0) {
        t_value->datatype = FLOAT;
        t_value->data.d = p_report->step_rate;
    }
    else {
        rc = 1;       // invalid field name
    }

    return rc;
}

int PedGetBatchItem( const sns_sam_ped_batch_item_s_v01 *p_item, const char *fieldname, token *t_value)
{
    int rc = 0;     // successful

    if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        t_value->data.i = p_item->timestamp;
        t_value->datatype = INTEGER;
    }
    else if ( PedGetReportData( &p_item->report, fieldname, t_value) ) {
        int rc = 1;     // failed
    }

    return rc;
}

/**
 * @name  PedGetReportResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void PedGetReportResp( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_ped_get_report_resp_msg_v01 *pMsg =
        ( sns_sam_ped_get_report_resp_msg_v01 *) p_respind->msg_ptr;


    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        if ( pMsg->instance_id_valid) {
            t_value->data.i = pMsg->instance_id;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        if ( pMsg->timestamp_valid) {
            t_value->data.i = pMsg->timestamp;
            t_value->datatype = INTEGER;
        }
    }
    else {
        if ( pMsg->report_data_valid) {
            sns_sam_ped_report_data_s_v01 *p_report = &pMsg->report_data;
            if ( PedGetReportData( p_report, fieldname, t_value)) {
                BadFieldName( fieldname);
            }
        }
    }
}

/**
 * @name  PedGetBatchResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void PedGetBatchResp( const respind *p_respind, const token *t_field, token *t_value)           // sameas GRAV
{
    sns_sam_ped_batch_resp_msg_v01 *pMsg =
        ( sns_sam_ped_batch_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        if ( pMsg->instance_id_valid) {
            t_value->data.i = pMsg->instance_id;
            t_value->datatype = INTEGER;
        }
    }
    if ( strcasecmp( fieldname, get_literal( MAX_BATCH_SIZE)) == 0) {
        if ( pMsg->max_batch_size_valid) {
            t_value->data.i = pMsg->max_batch_size;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        if ( pMsg->timestamp_valid) {
            t_value->data.i = pMsg->timestamp;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  PedGetUpdateBatchPeriodResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void PedGetUpdateBatchPeriodResp( const respind *p_respind, const token *t_field, token *t_value)   // sameas GRAV
{
    sns_sam_ped_update_batch_period_resp_msg_v01 *pMsg =
        ( sns_sam_ped_update_batch_period_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        if ( pMsg->instance_id_valid) {
            t_value->data.i = pMsg->instance_id;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        if ( pMsg->timestamp_valid) {
            t_value->data.i = pMsg->timestamp;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  PedGetReportInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void PedGetReportInd( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_ped_report_ind_msg_v01 *pMsg =
        ( sns_sam_ped_report_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        t_value->data.i = pMsg->instance_id;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        t_value->data.i = pMsg->timestamp;
        t_value->datatype = INTEGER;
    }
    else {
        sns_sam_ped_report_data_s_v01 *p_report = &pMsg->report_data;
        if ( PedGetReportData( p_report, fieldname, t_value)) {
            BadFieldName( fieldname);
        }
    }
}

/**
 * @name  PedGetBatchInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void PedGetBatchInd( const respind *p_respind, const token *t_field, token *t_value)
{
    static uint32_t item_index = 0;

    sns_sam_ped_batch_ind_msg_v01 *pMsg =
        ( sns_sam_ped_batch_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
            if ( t_field->data.i >= pMsg->items_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
            }
            else {
                item_index = t_field->data.i;
            }
//          printf( "PedGetBatchInd set index %d, ask %ld\n", report_index, t_field->data.i); // debugging
        }
        else {
            BadFieldName( fieldname);
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        t_value->data.i = pMsg->instance_id;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( IND_TYPE)) == 0) {
        if ( pMsg->ind_type_valid) {
            t_value->data.i = pMsg->ind_type;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        t_value->data.i = pMsg->items_len;
        t_value->datatype = INTEGER;
    }
    else {
        sns_sam_ped_batch_item_s_v01 *p_item = &pMsg->items[ item_index];
//      printf( "PedGetBatchInd get index %d, %p, %s\n", report_index, p_result, fieldname);   // debugging
        if ( PedGetBatchItem( p_item, fieldname, t_value)) {
            BadFieldName( fieldname);
        }
    }
}

/*
 * @name PedGetter
 * @brief       get field values from the p_respind message
 *
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] element
 */
void PedGetter( const respind *p_respind, const token *t_field, token *t_value)
{

    SetTypes( t_value, VALUE, SCALAR, UNDEFINED);

    uint32_t service_number = p_respind->header.service_number;
    if ( !( service_number == SNS_SAM_PED_SVC_ID_V01)) {
        FatalError( get_literal( BAD_SRVC_NUM));
    }

    uint32_t msg_id = p_respind->header.msg_id;
    uint16_t msg_size = p_respind->header.msg_size;
    sensor1_msg_type_e msg_type = p_respind->msg_type;

    if ( msg_type == SENSOR1_MSG_TYPE_RESP) {
        if ( msg_id == SNS_SAM_PED_ENABLE_RESP_V01) {
            PedGetEnableResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_DISABLE_RESP_V01) {
            PedGetDisableResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_GET_REPORT_RESP_V01) {
            PedGetReportResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_RESET_RESP_V01) {
            PedGetResetResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_BATCH_RESP_V01) {
            PedGetBatchResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_UPDATE_BATCH_PERIOD_RESP_V01) {
            PedGetUpdateBatchPeriodResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_GET_ATTRIBUTES_RESP_V01) {     // GENERIC
            CommonGetAttributesResp( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
    else if ( msg_type == SENSOR1_MSG_TYPE_IND) {
        if ( msg_id == SNS_SAM_PED_REPORT_IND_V01) {
            PedGetReportInd( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SAM_PED_BATCH_IND_V01) {
            PedGetBatchInd( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
}

/**
 * @name PedInit
 */
void PedInit( void)
{
    int svc_id = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].name = "ped";
    service_toc[ svc_id].new_req_func = (new_req)&PedNewReq;
    service_toc[ svc_id].set_req_func = (set_req)&PedSetReq;
    service_toc[ svc_id].getter_func  = (get_func)&PedGetter;

    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_id = SNS_SAM_PED_CANCEL_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_VERSION].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_id = SNS_SAM_PED_VERSION_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_id = SNS_SAM_PED_ENABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ENABLE].header.msg_size = sizeof(sns_sam_ped_enable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_id = SNS_SAM_PED_DISABLE_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_DISABLE].header.msg_size = sizeof(sns_sam_ped_disable_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_id = SNS_SAM_PED_GET_REPORT_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_GET_REPORT].header.msg_size = sizeof(sns_sam_ped_get_report_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_ALGO_SPECIFIC].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_SPECIFIC].header.msg_id = SNS_SAM_PED_RESET_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_SPECIFIC].header.msg_size = sizeof( sns_sam_ped_reset_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_BATCH].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_BATCH].header.msg_id = SNS_SAM_PED_BATCH_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_BATCH].header.msg_size = sizeof(sns_sam_ped_batch_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_FLUSH].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_FLUSH].header.msg_id = SNS_SAM_PED_UPDATE_BATCH_PERIOD_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_FLUSH].header.msg_size = sizeof(sns_sam_ped_update_batch_period_req_msg_v01);

    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.service_number = SNS_SAM_PED_SVC_ID_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_id = SNS_SAM_PED_GET_ATTRIBUTES_REQ_V01;
    service_toc[ svc_id].req_msg[ SAM_ALGO_ATTRIB].header.msg_size = 0;
}


