/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: sam_common.c
** DESC: sensor language - slang - functions common to many services
** Revision History
** ================================================================ */
#include "sam_common.h"

#include "sensor1.h"
#include "sns_smgr_common_v01.h"
#include "sns_sam_qmd_v01.h"

#include "sns_sam_gravity_vector_v01.h"

#include <stdio.h>      // sprintf

/** aka set man tlv 1 instance_id
 *  for disable req, get report req, ???
 * @name CommonSetManT1InstanceId
 * @brief set mandatory instance_id into disable request message
 * @param[io] p_pody
 * @param[i] t_keyvalue
 */
void CommonSetManT1InstanceId( void *p_body, const token *t_keyvalue)
{
    char *fieldname = t_keyvalue->pSymbol;

    if ( strcmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        sns_sam_qmd_disable_req_msg_v01 *payload = p_body;
        payload->instance_id = (uint8_t)t_keyvalue->data.i;
    }
    else {
        InvalidFieldName( fieldname);
    }
}

/**
 * @name  CommonGetManT3InstanceId
 * @brief   get mandatory instance_id value from response
 *          packets including
 *          sns_sam_gravity_enable_resp_msg_v01
 *          sns_sam_game_rotation_vector_enable_resp_msg_v01
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void CommonGetManT3InstanceId( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_qmd_enable_resp_msg_v01 *pMsg =
        ( sns_sam_qmd_enable_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        t_value->data.i = pMsg->instance_id;
        t_value->datatype = INTEGER;
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name  CommonGetOptT3InstanceId
 * @brief   get optional instance_id value from response packets
 *          including sns_sam_gravity_enable_resp_msg_v01
 *          sns_sam_game_rotation_vector_enable_resp_msg_v01
 * @param[i] p_respind
 * @param[i] t_field - KEYVALUE or fieldname SYMBOL
 * @param[io] t_value   - gets field value
 */
void CommonGetOptT3InstanceId( const respind *p_respind, const token *t_field, token *t_value)
{
    sns_sam_gravity_enable_resp_msg_v01 *pMsg =
        ( sns_sam_gravity_enable_resp_msg_v01 *) p_respind->msg_ptr;

    char *fieldname = t_field->pSymbol;

    if ( strcasecmp( fieldname, get_literal( INSTANCE_ID)) == 0) {
        if ( pMsg->instance_id_valid) {
            t_value->data.i = pMsg->instance_id;
            t_value->datatype = INTEGER;
        }
    }
    else {
        BadFieldName( fieldname);
    }
}

/**
 * @name CommonGetAttributesResp
 * @param[i] p_respind
 * @param[i] t_field
 * @param[io] t_value
 */
void CommonGetAttributesResp( const respind *p_respind, const token *t_field, token *t_value)
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


