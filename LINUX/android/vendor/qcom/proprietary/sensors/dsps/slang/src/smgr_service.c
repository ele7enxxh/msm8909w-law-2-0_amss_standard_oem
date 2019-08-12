/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: smgr_service.c
** DESC: sensor language - slang - smgr service support
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sli.h"
#include "diagnostics.h"
#include "literals.h"
#include "slcore.h"
#include "slmalloc.h"
#include "slnew_req.h"

#include "sensor1.h"
#include "sns_smgr_common_v01.h"
#include "sns_smgr_api_v01.h"

#include <stdio.h>      // sprintf

name2num    sensorname2num[] = {
    { "accel",        0},
    { "gyro",         10},
    { "mag",          20},
    { "pressure",     30},
    { "prox_light",   40},
    { "humidity",     50},
    { "rgb_cct",      60},
    { "sar",          70},
    { "hall",         80},
    { "heart_rate",   90},
    { "object_temp",  100},
};

static int smgr_req_item = 0;

void BadSensorName( const char *sensorname)
{
    char errbuf[ 64];
    snprintf( errbuf, sizeof(errbuf), "%s \'%s\'", get_literal( UNKNOWN_SENSOR), sensorname);
    RTErrorMsg( errbuf);
}

/**
 * @name        get_sensor_id
 * @brief       get sensor number from name
 * @param[i]    char *lower case sensor name
 * @return      UNKNOWN or int service_number
 */
int32_t get_sensor_id( const char *name)
{
    unsigned int i;

    for ( i = 0; i < sizeof( sensorname2num) / sizeof( name2num); i++) {
        if ( strcasecmp( sensorname2num[ i].name, name) == 0) {
            return sensorname2num[ i].num;
        }
    }
    BadSensorName( name);
    return UNKNOWN;
}

/**
 * @name        GetSensorId
 * @param[i]    t_keyvalue
 * @return      int32_t
 */
int32_t GetSensorId( const token *t_keyvalue)
{
    int32_t sensor_id = UNKNOWN;

    if ( t_keyvalue->datatype == INTEGER) {
        sensor_id = t_keyvalue->data.i;
    }
    else if ( t_keyvalue->datatype == POINTER) {
        sensor_id = get_sensor_id( t_keyvalue->data.p);
    }
    else {
        char *fieldname = t_keyvalue->pSymbol;
        ErrReasonForField( BAD_VALUE, fieldname);
    }
    return sensor_id;
}

uint8_t GetDataType( const token *t_keyvalue)
{
    uint8_t datatype = 0;

    if ( t_keyvalue->datatype == INTEGER) {
        datatype = t_keyvalue->data.i;
    }
    else if ( t_keyvalue->datatype == POINTER) {
        if ( strcasecmp( t_keyvalue->data.p, get_literal( PRIMARY)) == 0) {
            datatype = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( SECONDARY)) == 0) {
            datatype = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
        }
        else {
            char *fieldname = t_keyvalue->pSymbol;
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    return datatype;
}


/** === SETTERS === */
/** === SETTERS === */
/** === SETTERS === */

/**
 * @name        SmgrSetSnglInfo
 * @param[io]   req_msg
 * @param[i]    t_keyvalue
 */
void  SmgrSetSnglInfo( request *req_msg, const token *t_keyvalue)
{
    char *fieldname = t_keyvalue->pSymbol;
    sns_smgr_single_sensor_info_req_msg_v01 *pMsg = req_msg->payload;

    if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        int32_t sensor_id = GetSensorId( t_keyvalue);
        pMsg->SensorID = (uint8_t)sensor_id;
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name        SmgrSetReport
 * @param[io]   req_msg
 * @param[i]    t_keyvalue
 * @brief       requires fields to be presented using ascending
 *              items numbers. for example
 *              { "item" => 0, "sensor_id" => "accel", "item" => 1,
 *              "sensor_id" => "gyro"
 */

void  SmgrSetReport( request *req_msg, const token *t_keyvalue)
{
    char *fieldname = t_keyvalue->pSymbol;
    sns_smgr_periodic_report_req_msg_v01 *pMsg = req_msg->payload;

    if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        pMsg->ReportId = (uint8_t)t_keyvalue->data.i;
    }
    else if ( strcasecmp( fieldname, get_literal( ACTION)) == 0) {
        if ( strcasecmp( t_keyvalue->data.p, get_literal( ADD)) == 0) {
            pMsg->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( DELETE)) == 0) {
            pMsg->Action = SNS_SMGR_REPORT_ACTION_DELETE_V01;
            pMsg->Item_len = 0;
            pMsg->cal_sel_valid = 0;
            pMsg->SampleQuality_valid = 0;
            pMsg->notify_suspend_valid = 0;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( REPORTRATE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->ReportRate = (uint16_t)t_keyvalue->data.i;
        }
        else if ( t_keyvalue->datatype == FLOAT) {
            pMsg->ReportRate = (uint16_t)((int)t_keyvalue->data.d);
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        if ( t_keyvalue->datatype != INTEGER) {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
        int item_index = t_keyvalue->data.i;
        smgr_req_item  = item_index;

        if ( item_index < 0 || item_index >= SNS_SMGR_MAX_ITEMS_PER_REPORT_V01 ) {
            ErrReasonForField( BAD_VALUE, fieldname);
            return;
        }
        smgr_req_item = item_index;
        pMsg->Item_len = item_index + 1;
        pMsg->Item[ item_index].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
    }
    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        pMsg->Item_len = smgr_req_item + 1;
        pMsg->Item[ smgr_req_item].SensorId = (uint8_t)GetSensorId( t_keyvalue);
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        pMsg->Item_len = smgr_req_item + 1;
        pMsg->Item[ smgr_req_item].DataType = GetDataType( t_keyvalue);
    }
    else if ( strcasecmp( fieldname, get_literal( DECIMATION)) == 0) {
        pMsg->Item_len = smgr_req_item + 1;
        if ( strcasecmp( t_keyvalue->data.p, get_literal( SAMPLE)) == 0) {
            pMsg->Item[ smgr_req_item].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FILTER)) == 0) {
            pMsg->Item[ smgr_req_item].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( CALIBRATION)) == 0) {
        pMsg->cal_sel_valid = true;
        pMsg->cal_sel_len = smgr_req_item + 1;
        if ( strcasecmp( t_keyvalue->data.p, get_literal( RAW)) == 0) {
            pMsg->cal_sel[ smgr_req_item] = SNS_SMGR_CAL_SEL_RAW_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FACTORY)) == 0) {
            pMsg->cal_sel[ smgr_req_item] = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FULL)) == 0) {
            pMsg->cal_sel[ smgr_req_item] = SNS_SMGR_CAL_SEL_FULL_CAL_V01;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLEQUALITY)) == 0) {
        pMsg->SampleQuality_valid = true;
        pMsg->SampleQuality_len = smgr_req_item + 1;
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->SampleQuality[ smgr_req_item] = (uint16_t)t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else {
        InvalidFieldName( fieldname);
    }
}

/**
 * @name        SmgrSetSelfTest
 * @brief       sets fields in the
 *              sns_smgr_single_sensor_test_req_msg_v01
 * @param[io]   req_msg
 * @param[i]    t_keyvalue
 */
void SmgrSetSelfTest( request *req_msg, const token *t_keyvalue)
{
    char *fieldname = t_keyvalue->pSymbol;
    sns_smgr_single_sensor_test_req_msg_v01 *pMsg = req_msg->payload;

    if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        pMsg->SensorID = (uint8_t)GetSensorId( t_keyvalue);
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        pMsg->DataType = GetDataType( t_keyvalue);
    }
    else if ( strcasecmp( fieldname, get_literal( TESTTYPE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->TestType = ( sns_smgr_test_type_e_v01) t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SAVE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->SaveToRegistry_valid = true;
            pMsg->SaveToRegistry       = t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( APPLY)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->ApplyCalNow_valid = true;
            pMsg->ApplyCalNow       = t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else {
        InvalidFieldName( fieldname);
    }
}

/**
 * @name        SmgrSetBuffered
 * @brief       req_msg intialized with keyvalue
 *              requires setting items in ascending order
 *              because item_len gets set per item.
 * @param[io]   req_msg
 * @param[i]    t_keyvalue
 */
void SmgrSetBuffered( request *req_msg, const token *t_keyvalue)
{
    char *fieldname = t_keyvalue->pSymbol;
    sns_smgr_buffering_req_msg_v01 *pMsg = req_msg->payload;

    if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        pMsg->ReportId = (uint8_t)t_keyvalue->data.i;
    }
    else if ( strcasecmp( fieldname, get_literal( ACTION)) == 0) {
        if ( strcasecmp( t_keyvalue->data.p, get_literal( ADD)) == 0) {
            pMsg->Action = SNS_SMGR_BUFFERING_ACTION_ADD_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( DELETE)) == 0) {
            pMsg->Action = SNS_SMGR_BUFFERING_ACTION_DELETE_V01;
            pMsg->Item_len = 0;
            pMsg->notify_suspend_valid = 0;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( REPORTRATE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            uint32_t q16_value = (uint32_t)t_keyvalue->data.i * 65536;
            pMsg->ReportRate   = q16_value;
        }
        else if ( t_keyvalue->datatype == FLOAT) {
            uint32_t q16_value = (uint32_t)( t_keyvalue->data.d * 65536);
            pMsg->ReportRate = q16_value;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        if ( t_keyvalue->datatype != INTEGER
             || t_keyvalue->data.i < 0
             || t_keyvalue->data.i >= SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01 ) {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
        else {
            smgr_req_item = t_keyvalue->data.i;
            pMsg->Item[ smgr_req_item].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
            pMsg->Item_len = smgr_req_item + 1;
        }
    }

    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        pMsg->Item_len = smgr_req_item + 1;
        int32_t sensor_id = UNKNOWN;
        if ( t_keyvalue->datatype == INTEGER) {
            sensor_id = t_keyvalue->data.i;
        }
        else if ( t_keyvalue->datatype == POINTER) {
            sensor_id = get_sensor_id( t_keyvalue->data.p);
        }
        if ( sensor_id != UNKNOWN) {
            pMsg->Item[ smgr_req_item].SensorId = (uint8_t)sensor_id;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->Item[ smgr_req_item].DataType  = t_keyvalue->data.i;
        }
        else if ( t_keyvalue->datatype == POINTER) {
            if ( strcasecmp( t_keyvalue->data.p, get_literal( PRIMARY)) == 0) {
                pMsg->Item[ smgr_req_item].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
            }
            else if ( strcasecmp( t_keyvalue->data.p, get_literal( SECONDARY)) == 0) {
                pMsg->Item[ smgr_req_item].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
            }
            else {
                ErrReasonValueForFieldName( BAD_VALUE, t_keyvalue->data.p, fieldname);
                // stub WANT slnew_req return NULL
            }
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( DECIMATION)) == 0) {
        if ( strcasecmp( t_keyvalue->data.p, get_literal( SAMPLE)) == 0) {
            pMsg->Item[ smgr_req_item].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FILTER)) == 0) {
            pMsg->Item[ smgr_req_item].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;
        }
        else {
            ErrReasonValueForFieldName( BAD_VALUE, t_keyvalue->data.p, fieldname);
            // stub WANT slnew_req return NULL
        }
    }
    else if ( strcasecmp( fieldname, get_literal( CALIBRATION)) == 0) {
        if ( strcasecmp( t_keyvalue->data.p, get_literal( RAW)) == 0) {
            pMsg->Item[ smgr_req_item].Calibration = SNS_SMGR_CAL_SEL_RAW_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FACTORY)) == 0) {
            pMsg->Item[ smgr_req_item].Calibration = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
        }
        else if ( strcasecmp( t_keyvalue->data.p, get_literal( FULL)) == 0) {
            pMsg->Item[ smgr_req_item].Calibration = SNS_SMGR_CAL_SEL_FULL_CAL_V01;
        }
        else {
            ErrReasonValueForFieldName( BAD_VALUE, t_keyvalue->data.p, fieldname);
            // stub WANT slnew_req return NULL
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLERATE)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->Item[ smgr_req_item].SamplingRate = (uint16_t)t_keyvalue->data.i;
        }
        else if ( t_keyvalue->datatype == FLOAT) {
            pMsg->Item[ smgr_req_item].SamplingRate = (uint16_t)t_keyvalue->data.d;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLEQUALITY)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->Item[ smgr_req_item].SampleQuality = (uint16_t)t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
    else if ( strcasecmp( fieldname, get_literal( NOTIFY_SUSPEND)) == 0) {
        if ( t_keyvalue->datatype == INTEGER) {
            pMsg->notify_suspend_valid = 1;
            pMsg->notify_suspend.send_indications_during_suspend = t_keyvalue->data.i;
        }
        else {
            ErrReasonForField( BAD_VALUE, fieldname);
        }
    }
}


/**
 * @name        SmgrSetReq
 * @brief       SmgrSetReq is invoked once per field name.
 * @param[io]   req_msg
 * @param[i]     t_keyvalue
 */
void SmgrSetReq( request *req_msg, const token *t_keyvalue)
{
    void *body = req_msg->payload;
    int32_t msg_id = req_msg->header.msg_id;

    char *fieldname = t_keyvalue->pSymbol;
    if ( fieldname == NULL) {
        RTErrorMsg( "SmgrSetReq: pSymbol == NULL\n");
        return;
    }

    if ( msg_id == SNS_SMGR_REPORT_REQ_V01) {
        SmgrSetReport( req_msg, t_keyvalue);
    }
    else if ( msg_id == SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01) {
        SmgrSetSnglInfo( req_msg, t_keyvalue);
    }
    else if ( msg_id == SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01) {
        SmgrSetSelfTest( req_msg, t_keyvalue);
    }
    else if ( msg_id == SNS_SMGR_BUFFERING_REQ_V01) {
        SmgrSetBuffered( req_msg, t_keyvalue);
    }
//    else if ( msg_id == SNS_SMGR_BUFFERING_QUERY_REQ_V01) {
//        SmgrSetBufQuery( req_msg, t_keyvalue);
//    }
    else {
        InvalidFieldName( fieldname);
    }
}

/**
 * @name        SmgrNewReq
 * @brief       SmgrSetReq is invoked to initialize optional
 *              fields in a newly allocated req_msg
 * @param[io]   req_msg
 * @param[i]    fieldlist
 */
void SmgrNewReq( request *req_msg, const array *fieldlist)
{
    if ( fieldlist) {
        if ( req_msg->header.service_number != SNS_SMGR_SVC_ID_V01) {
            RTErrorMsg( "invalid req_msg for smgr");
            return;
        }

        /*
        ** populate periodic report payload with default values      !! stub new - only binary zero defaults !!
        ** ------------------------------------------------- */
        if ( req_msg->header.msg_id == SNS_SMGR_REPORT_REQ_V01) {
            sns_smgr_periodic_report_req_msg_v01 *pMsg = req_msg->payload;
            smgr_req_item = 0;                                                  // default item_index
            pMsg->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;                      // default action
        }

        // note: arrays populate the high_water element !! thus using <= high_water
        int i;
        for ( i = 0; i <= fieldlist->high_water; ) {
            token *t_keyvalue  = ( token *)get_array_element( fieldlist, i++);
            if ( t_keyvalue == NULL) {
                break;
            }
            if ( t_keyvalue->subtype != KEYVALUE) {
                RTErrorMsg( "invalid key => value argument");           // expecting field_name => value, ...
                break;
            }
            SmgrSetReq( req_msg, t_keyvalue);
        }
    }
}

/** === GETTERS === */
/** === GETTERS === */
/** === GETTERS === */

void SetTypes( token *t_value, uint16_t type, uint8_t subtype, uint8_t datatype)
{
    t_value->type = type;
    t_value->subtype = subtype;
    t_value->datatype = datatype;
}

/**
 * @name  SmgrGetReportResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetReportResp( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t reasonpair_index = 0;

    sns_smgr_periodic_report_resp_msg_v01 *pMsg =
        ( sns_smgr_periodic_report_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( REASONPAIR)) == 0) {
            reasonpair_index = t_field->data.i;
            if ( reasonpair_index >= ( int32_t)pMsg->ReasonPair_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                reasonpair_index = 0;
            }
        }
        else {
            RTErrorMsg( get_literal( BAD_FIELD));
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        t_value->data.i = pMsg->ReportId;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( ACKNAK)) == 0) {
        t_value->data.i = pMsg->AckNak;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( REASONPAIR)) == 0) {
        t_value->data.i = pMsg->ReasonPair_len;           // number of entries
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        t_value->data.i = pMsg->ReasonPair[ reasonpair_index].ItemNum;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( REASON)) == 0) {
        t_value->data.i = pMsg->ReasonPair[ reasonpair_index].Reason;
        t_value->datatype = INTEGER;
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  SmgrGetAllSensorInfoResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetAllSensorInfoResp( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t info_index = 0;

    sns_smgr_all_sensor_info_resp_msg_v01 *pMsg =
        ( sns_smgr_all_sensor_info_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( INFO)) == 0) {
            info_index = t_field->data.i;
            if ( info_index >= ( int32_t)pMsg->SensorInfo_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                info_index = 0;
            }
        }
        else {
            BadFieldName( fieldname);
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( INFO)) == 0) {
        t_value->data.i = pMsg->SensorInfo_len;     // return number of entries
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->SensorInfo[info_index].SensorID;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SHORTNAME)) == 0) {
        uint32_t len = pMsg->SensorInfo[info_index].SensorShortName_len;
        void *p_from = pMsg->SensorInfo[info_index].SensorShortName;
        void *p_to   = LatchLiteral( p_from, len);
//        SetTypes( t_value, VALUE, STRING, POINTER);
        t_value->data.p = p_to;
        t_value->subtype = STRING;
        t_value->datatype = POINTER;
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name    SmgrGetSnglSensorInfoResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetSnglSensorInfoResp( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t info_index = 0;
    static int32_t odr_index = 0;

    sns_smgr_single_sensor_info_resp_msg_v01 *pMsg =
        ( sns_smgr_single_sensor_info_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( INFO)) == 0) {
            info_index = t_field->data.i;
            if ( info_index >= ( int32_t)pMsg->SensorInfo.data_type_info_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                info_index = 0;
            }
        }
        else if ( strcmp( fieldname, get_literal( ODR_LIST)) == 0) {
            odr_index = t_field->data.i;
            if ( odr_index >= (int32_t)pMsg->supported_odr_list[ info_index].odrs_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                odr_index = 0;
            }
        }
        else {
            BadFieldName( fieldname);
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( INFO)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info_len;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info[info_index].SensorID;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info[info_index].DataType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SENSOR_NAME)) == 0) {
        uint32_t len = pMsg->SensorInfo.data_type_info[info_index].SensorName_len;
        void *p_from = pMsg->SensorInfo.data_type_info[info_index].SensorName;
        void *p_to   = LatchLiteral( p_from, len );
        t_value->data.p = p_to;
        t_value->subtype = STRING;
        t_value->datatype = POINTER;
    }
    else if ( strcasecmp( fieldname, get_literal( VENDOR)) == 0) {
        uint32_t len = pMsg->SensorInfo.data_type_info[info_index].VendorName_len;
        void *p_from = pMsg->SensorInfo.data_type_info[info_index].VendorName;
        void *p_to   = LatchLiteral( p_from, len );
        t_value->data.p = p_to;
        t_value->subtype = STRING;
        t_value->datatype = POINTER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_RATE)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info[info_index].MaxSampleRate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( IDLE_POWER)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info[info_index].IdlePower;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_POWER)) == 0) {
        t_value->data.i = pMsg->SensorInfo.data_type_info[info_index].MaxPower;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_RANGE)) == 0) {
        t_value->data.d = (double)pMsg->SensorInfo.data_type_info[info_index].MaxRange / 65536;
        t_value->datatype = FLOAT;
    }
    else if ( strcasecmp( fieldname, get_literal( RESOLUTION)) == 0) {
        t_value->data.d = (double)pMsg->SensorInfo.data_type_info[info_index].Resolution / 65536;
        t_value->datatype = FLOAT;
    }
    else if ( strcasecmp( fieldname, get_literal( MAX_REPORTS)) == 0) {
        if ( pMsg->num_buffered_reports_valid) {
            t_value->data.i = pMsg->num_buffered_reports[info_index];
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( OP_MODE)) == 0) {
        if ( pMsg->op_mode_valid) {
            t_value->data.i = pMsg->op_mode;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( SUID)) == 0) {
        if ( pMsg->suid_valid) {
            if ( info_index < (int32_t)pMsg->suid_len) {
                char lit_suid[ 20];
                uint8_t *pHex = ( uint8_t *)&pMsg->suid[ info_index];
                snprintf( lit_suid, sizeof( lit_suid), "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                         pHex[0], pHex[1], pHex[2], pHex[3],
                         pHex[4], pHex[5], pHex[6], pHex[7]);
                void *p_to   = LatchLiteral( lit_suid, 16);
                t_value->data.p = p_to;
                t_value->datatype = POINTER;
            }
            else {
                RTErrorMsg( get_literal( BAD_INDEX));
            }
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ODR_LIST)) == 0) {
        if ( pMsg->supported_odr_list_valid) {
            uint32_t odr_count = pMsg->supported_odr_list[ info_index].odrs_len;
            t_value->data.i = odr_count;
            t_value->datatype = INTEGER;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ODR)) == 0) {
        if ( pMsg->supported_odr_list_valid) {
            uint32_t odr = pMsg->supported_odr_list[ info_index].odrs[ odr_index];
            t_value->data.i = odr;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}


/**
 * @name  SmgrGetSelfTestResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value - gets field value
 */
void SmgrGetSelfTestResp( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t reasonpair_index = 0;

    sns_smgr_single_sensor_test_ind_msg_v01 *pMsg =
        ( sns_smgr_single_sensor_test_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->SensorID;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        t_value->data.i = pMsg->DataType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( TESTTYPE)) == 0) {
        t_value->data.i = pMsg->TestType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( TESTRESULT)) == 0) {
        t_value->data.i = pMsg->TestResult;
        t_value->datatype = INTEGER;
    }
    else {
        BadFieldName( fieldname);

    }
}

/**
 * @name  SmgrGetBufferedResp
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetBufferedResp( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t reasonpair_index = 0;

    sns_smgr_buffering_resp_msg_v01 *pMsg =
        ( sns_smgr_buffering_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( REASONPAIR)) == 0) {
            reasonpair_index = t_field->data.i;
            if ( reasonpair_index >= ( int32_t)pMsg->ReasonPair_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                reasonpair_index = 0;
            }
        }
        else {
            BadFieldName( fieldname);
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        if ( pMsg->ReportId_valid) {
            t_value->datatype = INTEGER;
            t_value->data.i = pMsg->ReportId;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ACKNAK)) == 0) {
        if ( pMsg->AckNak_valid) {
            t_value->datatype = INTEGER;
            t_value->data.i = pMsg->AckNak;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( REASONPAIR)) == 0) {
        if ( pMsg->ReasonPair_valid) {
            t_value->datatype = INTEGER;
            t_value->data.i = pMsg->ReasonPair_len;           // number of entries
        }
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        if ( pMsg->ReasonPair_valid) {
            t_value->datatype = INTEGER;
            t_value->data.i = pMsg->ReasonPair[ reasonpair_index].ItemNum;
        }
    }
    else if ( strcasecmp( fieldname, get_literal( REASON)) == 0) {
        if ( pMsg->ReasonPair_valid) {
            t_value->datatype = INTEGER;
            t_value->data.i = pMsg->ReasonPair[ reasonpair_index].Reason;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  SmgrGetReportInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetReportInd( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t item_index = 0;

    sns_smgr_periodic_report_ind_msg_v01 *pMsg =
        ( sns_smgr_periodic_report_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( ITEM)) == 0) {
            item_index = t_field->data.i;
            if ( item_index >= ( int32_t)pMsg->Item_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
                item_index = 0;
            }
        }
        else {
            RTErrorMsg( get_literal( BAD_FIELD));
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        t_value->data.i = pMsg->ReportId;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( STATUS)) == 0) {
        t_value->data.i = pMsg->status;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( CURRENTRATE)) == 0) {
        t_value->data.i = pMsg->CurrentRate;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        t_value->data.i = pMsg->Item_len;                           // number of items
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].SensorId;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].DataType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATAX)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].ItemData[0];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATAY)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].ItemData[1];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATAZ)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].ItemData[2];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].TimeStamp;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( FLAGS)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].ItemFlags;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( QUALITY)) == 0) {
        t_value->data.i = pMsg->Item[ item_index].ItemQuality;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLERATE)) == 0) {
        if ( pMsg->SamplingRate_valid
             && pMsg->SamplingRate_len >= (uint32_t)item_index) {
            t_value->data.i = pMsg->SamplingRate[ item_index];
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  SmgrGetBufferedInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetBufferedInd( const respind *p_respind, const token *t_field, token *t_value)
{
    static int32_t item_index = 0;
    static int32_t sample_index = 0;            // absolute:  FirstSampleIdx + relative sample index.

    sns_smgr_buffering_ind_msg_v01 *pMsg =
        ( sns_smgr_buffering_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( t_field->subtype == KEYVALUE) {
        if ( strcmp( fieldname, get_literal( ITEM)) == 0) {
            if ( t_field->data.i >= pMsg->Indices_len) {
                RTErrorMsg( get_literal( BAD_INDEX));
            }
            else {
                item_index = t_field->data.i;
            }
        }
        else if ( strcmp( fieldname, get_literal( SAMPLE)) == 0) {
            if ( t_field->data.i >= pMsg->Indices[ item_index].SampleCount) {
                RTErrorMsg( get_literal( BAD_INDEX));
            }
            else {
                sample_index = (int32_t)pMsg->Indices[ item_index].FirstSampleIdx + t_field->data.i;
            }
        }
        else {
            BadFieldName( fieldname);
        }
        return;
    }

    if ( strcasecmp( fieldname, get_literal( ITEM)) == 0) {
        t_value->data.i = pMsg->Indices_len;                        // number of indices
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLE)) == 0) {
        t_value->data.i = pMsg->Indices[ item_index].SampleCount;   // number of samples in item
        t_value->datatype = INTEGER;
    }

    else if ( strcasecmp( fieldname, get_literal( REPORTID)) == 0) {
        t_value->data.i = pMsg->ReportId;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( INDTYPE)) == 0) {
        if ( pMsg->IndType_valid) {
            t_value->data.i = pMsg->IndType;
            t_value->datatype = INTEGER;
        }
    }
    /*
    ** sns_smgr_buffering_sample_index_s_v01
    ** number of indices correspond to number of number of items.
    ** ------------------------------------------------------- */
    else if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->Indices[ item_index].SensorId;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        t_value->data.i = pMsg->Indices[ item_index].DataType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SAMPLERATE)) == 0) {
        t_value->data.i = pMsg->Indices[ item_index].SamplingRate;
        t_value->datatype = INTEGER;
    }
    /*
    ** sns_smgr_buffering_sample_s_v01
    ** ---------------------------- */
    else if ( strcasecmp( fieldname, get_literal( DATAX)) == 0) {
        t_value->data.i = pMsg->Samples[ sample_index].Data[0];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATAY)) == 0) {
        t_value->data.i = pMsg->Samples[ sample_index].Data[1];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATAZ)) == 0) {
        t_value->data.i = pMsg->Samples[ sample_index].Data[2];
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( SSCTS)) == 0) {
        uint32_t sscts = pMsg->Indices[ item_index].FirstSampleTimestamp;
        uint32_t l_idx = pMsg->Indices[ item_index].FirstSampleIdx;
        while ( l_idx <= (uint32_t)sample_index) {
            sscts += pMsg->Samples[ l_idx].TimeStampOffset;
            l_idx++;
        }
        t_value->data.i = sscts;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( FLAGS)) == 0) {
        t_value->data.i = pMsg->Samples[ sample_index].Flags;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( QUALITY)) == 0) {
        t_value->data.i = pMsg->Samples[ sample_index].Quality;
        t_value->datatype = INTEGER;
    }

    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  SmgrGetSelfTestInd
 * @brief   get value of fieldname from the p_respind message
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void SmgrGetSelfTestInd( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_smgr_single_sensor_test_ind_msg_v01 *pMsg =
        ( sns_smgr_single_sensor_test_ind_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( SENSORID)) == 0) {
        t_value->data.i = pMsg->SensorID;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( DATATYPE)) == 0) {
        t_value->data.i = pMsg->DataType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( TESTTYPE)) == 0) {
        t_value->data.i = pMsg->TestType;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( TESTRESULT)) == 0) {
        t_value->data.i = pMsg->TestResult;
        t_value->datatype = INTEGER;
    }
    else if ( strcasecmp( fieldname, get_literal( ERRORCODE)) == 0) {
        if ( pMsg->ErrorCode_valid) {
            t_value->data.i = pMsg->ErrorCode;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/*
** general form of getters are:
** get single value
**      my ( $count)  = slget( $all_resp, { info} );
** get multiple values
**      my ( $sensor_id, $sensor_name) = slget( $all_resp,  { info.index, sensor_id, sensor_name});
*/

/**
 * @name SmgrGetter
 * @brief       get field values from the p_respind message
 *
 * @param[ i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[ io] element
 */
void SmgrGetter( const respind *p_respind, const token *t_field, token *t_value)
{
//    printf( "SmgrGetter\n");

    SetTypes( t_value, VALUE, SCALAR, UNDEFINED);

    uint32_t service_number = p_respind->header.service_number;
    if ( service_number != SNS_SMGR_SVC_ID_V01) {
        FatalError( get_literal( BAD_SRVC_NUM));
    }

    uint32_t msg_id = p_respind->header.msg_id;
    uint16_t msg_size = p_respind->header.msg_size;
    sensor1_msg_type_e msg_type = p_respind->msg_type;

//    printf( "SmgrGetter \'%s\'\n", fieldname);

    if ( msg_type == SENSOR1_MSG_TYPE_RESP) {
        if ( msg_id == SNS_SMGR_VERSION_RESP_V01) {
            ;
        }
        else if ( msg_id == SNS_SMGR_REPORT_RESP_V01) {
            SmgrGetReportResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_ALL_SENSOR_INFO_RESP_V01) {
            SmgrGetAllSensorInfoResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01) {
            SmgrGetSnglSensorInfoResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01) {
            SmgrGetSelfTestResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_BUFFERING_RESP_V01) {
            SmgrGetBufferedResp( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_BUFFERING_QUERY_RESP_V01) {
            ;
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
    else if ( msg_type == SENSOR1_MSG_TYPE_IND) {
        if ( msg_id == SNS_SMGR_REPORT_IND_V01) {
            SmgrGetReportInd( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_BUFFERING_IND_V01) {
            SmgrGetBufferedInd( p_respind, t_field, t_value);
        }
        else if ( msg_id == SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01) {
            SmgrGetSelfTestInd( p_respind, t_field, t_value);
        }
        else {
            RTErrorMsg( get_literal( BAD_MSG_ID));
        }
    }
}

/**
 * @name SmgrInit
 */

void SmgrInit( void)
{
    int svc_id = SNS_SMGR_SVC_ID_V01;
    service_toc[ svc_id].name = "smgr";
    service_toc[ svc_id].new_req_func   = (new_req)&SmgrNewReq;
    service_toc[ svc_id].set_req_func   = (set_req)&SmgrSetReq;
    service_toc[ svc_id].getter_func    = (get_func)&SmgrGetter;

    service_toc[ svc_id].req_msg[ SMGR_CANCEL].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_CANCEL].header.msg_id = SNS_SMGR_CANCEL_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_CANCEL].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SMGR_VERSION].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_VERSION].header.msg_id = SNS_SMGR_VERSION_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_VERSION].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SMGR_REPORT].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_REPORT].header.msg_id = SNS_SMGR_REPORT_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_REPORT].header.msg_size = sizeof( sns_smgr_periodic_report_ind_msg_v01);

    service_toc[ svc_id].req_msg[ SMGR_ALL_INFO].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_ALL_INFO].header.msg_id = SNS_SMGR_ALL_SENSOR_INFO_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_ALL_INFO].header.msg_size = 0;

    service_toc[ svc_id].req_msg[ SMGR_SNGL_INFO].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_SNGL_INFO].header.msg_id = SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_SNGL_INFO].header.msg_size = sizeof( sns_smgr_single_sensor_info_req_msg_v01);

    service_toc[ svc_id].req_msg[ SMGR_SELFTEST].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_SELFTEST].header.msg_id = SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_SELFTEST].header.msg_size = sizeof( sns_smgr_single_sensor_test_req_msg_v01);

    service_toc[ svc_id].req_msg[ SMGR_BUFFERED].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_BUFFERED].header.msg_id = SNS_SMGR_BUFFERING_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_BUFFERED].header.msg_size = sizeof( sns_smgr_buffering_req_msg_v01);

    service_toc[ svc_id].req_msg[ SMGR_BUF_QUERY].header.service_number = svc_id;
    service_toc[ svc_id].req_msg[ SMGR_BUF_QUERY].header.msg_id = SNS_SMGR_BUFFERING_QUERY_REQ_V01;
    service_toc[ svc_id].req_msg[ SMGR_BUF_QUERY].header.msg_size = sizeof( sns_smgr_buffering_query_req_msg_v01);
}


