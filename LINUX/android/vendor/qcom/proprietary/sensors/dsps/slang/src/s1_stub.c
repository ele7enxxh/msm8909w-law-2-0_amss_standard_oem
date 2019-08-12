/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: s1_stub.c
** DESC: sensor language - slang - stub code for use in cygwin versions
** Revision History
** ================================================================ */
#include "literals.h"
#include "sensor1.h"
#include "sns_common_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_reg_api_v02.h"


#include <stdio.h>
#include <stdlib.h>     // MALLOC
#include <string.h>     // NULL

#define HANDLE  ((sensor1_handle_s *)0xcafefeed)

sensor1_notify_data_cb_t    saved_data_cbf;
intptr_t                    saved_cb_data;

sensor1_error_e
sensor1_init( void )
{
    return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_open( sensor1_handle_s **hndl,
              sensor1_notify_data_cb_t data_cbf,
              intptr_t cb_data )
{
    *hndl = HANDLE;
     saved_data_cbf = data_cbf;
     saved_cb_data  = cb_data;
     return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_close( sensor1_handle_s *hndl )
{
    if ( hndl != HANDLE) {
        printf( "sensor1_close: bad handle 0x%x\n", hndl);
        return SENSOR1_EINVALID_CLIENT;
    }
    return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_write( sensor1_handle_s     *hndl,
               sensor1_msg_header_s *msg_hdr,
               void                 *msg_ptr )
{
    if ( hndl != HANDLE) {
        printf( "sensor1_write: bad handle 0x%x\n", hndl);
        return SENSOR1_EINVALID_CLIENT;
    }

    if ( msg_hdr == NULL) {
        printf( "sensor1_write: msg_hdr is NULL\n");
    }

    if ( msg_hdr->service_number == SNS_SMGR_SVC_ID_V01
         && msg_hdr->msg_id == SNS_SMGR_ALL_SENSOR_INFO_REQ_V01) {

        size_t msg_size = sizeof( sns_smgr_all_sensor_info_resp_msg_v01);
        void *pMem = malloc( msg_size);
        if ( pMem == NULL) {
            FatalError( OOM);
        }
        memset( pMem, 0, msg_size);

        sns_smgr_all_sensor_info_resp_msg_v01 *pResp = pMem;
        pResp->Resp.sns_result_t = 0;
        pResp->Resp.sns_err_t = 0;

        pResp->SensorInfo_len = 3;
        pResp->SensorInfo[0].SensorID = SNS_SMGR_ID_ACCEL_V01;
        pResp->SensorInfo[0].SensorShortName_len = 5;
        strcpy( (char *)&pResp->SensorInfo[0].SensorShortName, "ACCEL");

        pResp->SensorInfo[1].SensorID = SNS_SMGR_ID_GYRO_V01;
        pResp->SensorInfo[1].SensorShortName_len = 4;
        strcpy( (char *)&pResp->SensorInfo[1].SensorShortName, "GYRO");

        pResp->SensorInfo[2].SensorID = SNS_SMGR_ID_MAG_V01;
        pResp->SensorInfo[2].SensorShortName_len = 3;
        strcpy( (char *)&pResp->SensorInfo[2].SensorShortName, "MAG");

        sensor1_msg_header_s hdr;
        hdr.service_number = msg_hdr->service_number;
        hdr.msg_id         = msg_hdr->msg_id;
        hdr.msg_size       = msg_size;
        hdr.txn_id         = msg_hdr->txn_id;

        // dispatch enable response message
        (saved_data_cbf)( saved_cb_data, &hdr, SENSOR1_MSG_TYPE_RESP, pMem);        // 1 == QMI_IDL_RESPONSE
    }

    else if ( msg_hdr->service_number == SNS_SMGR_SVC_ID_V01
         && msg_hdr->msg_id == SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01) {

        size_t msg_size = sizeof( sns_smgr_single_sensor_info_resp_msg_v01);
        void *pMem = malloc( msg_size);
        if ( pMem == NULL) {
            FatalError( OOM);
        }
        memset( pMem, 0, msg_size);

        sns_smgr_single_sensor_info_resp_msg_v01 *pResp = pMem;
        pResp->Resp.sns_result_t = 0;
        pResp->Resp.sns_err_t = 0;

        pResp->SensorInfo.data_type_info_len = 1;
        pResp->SensorInfo.data_type_info[0].SensorID = 0x01;
        pResp->SensorInfo.data_type_info[0].DataType = 0x02;
        pResp->SensorInfo.data_type_info[0].SensorName_len = 11;
        strcpy( (char *)pResp->SensorInfo.data_type_info[0].SensorName, "sensor_name");
        pResp->SensorInfo.data_type_info[0].VendorName_len = 11;
        strcpy( (char *)pResp->SensorInfo.data_type_info[0].VendorName, "vendor_name");
        pResp->SensorInfo.data_type_info[0].MaxSampleRate = 200;

        pResp->suid_valid = 1;
        pResp->suid_len = 1;
        pResp->suid[0] = 0x0123456789abcdef;

        sensor1_msg_header_s hdr;
        hdr.service_number = msg_hdr->service_number;
        hdr.msg_id         = msg_hdr->msg_id;
        hdr.msg_size       = msg_size;
        hdr.txn_id         = msg_hdr->txn_id;

        // dispatch enable response message
        (saved_data_cbf)( saved_cb_data, &hdr, SENSOR1_MSG_TYPE_RESP, pMem);        // 1 == QMI_IDL_RESPONSE
    }

    else if ( msg_hdr->service_number == SNS_REG2_SVC_ID_V01
         && msg_hdr->msg_id == SNS_REG_SINGLE_READ_REQ_V02) {

        size_t msg_size = sizeof( sns_reg_single_read_resp_msg_v02);
        void *pMem = malloc( msg_size);
        if ( pMem == NULL) {
            FatalError( OOM);
        }
        memset( pMem, 0, msg_size);

        sns_reg_single_read_resp_msg_v02 *pResp = pMem;
        pResp->resp.sns_result_t = 0;
        pResp->resp.sns_err_t = 0;

        pResp->item_id = 100;
        pResp->data_len = 4;
        pResp->data[0] = 1;     // 1 + 2 * 256 + 3 * 65536 + 4 * 16777216
        pResp->data[1] = 2;
        pResp->data[2] = 3;
        pResp->data[3] = 4;

        sensor1_msg_header_s hdr;
        hdr.service_number = msg_hdr->service_number;
        hdr.msg_id         = msg_hdr->msg_id;
        hdr.msg_size       = msg_size;
        hdr.txn_id         = msg_hdr->txn_id;

        (saved_data_cbf)( saved_cb_data, &hdr, SENSOR1_MSG_TYPE_RESP, pMem);        // 1 == QMI_IDL_RESPONSE
    }

    else if ( msg_hdr->service_number == SNS_SMGR_SVC_ID_V01
         && msg_hdr->msg_id == SNS_SMGR_REPORT_REQ_V01) {

        /** send sns_smgr_report_response */
        size_t msg_size = sizeof( sns_smgr_periodic_report_resp_msg_v01);
        void *pMem = malloc( msg_size);
        if ( pMem == NULL) {
            FatalError( OOM);
        }
        memset( pMem, 0, msg_size);

        sns_smgr_periodic_report_resp_msg_v01 *pResp = pMem;

        sensor1_msg_header_s hdr;
        hdr.service_number = msg_hdr->service_number;
        hdr.msg_id         = msg_hdr->msg_id;
        hdr.msg_size       = msg_size;
        hdr.txn_id         = msg_hdr->txn_id;

        (saved_data_cbf)( saved_cb_data, &hdr, SENSOR1_MSG_TYPE_RESP, pMem);        // 1 == QMI_IDL_RESPONSE

        /** send 3 sns_smgr_report_indications */

        int i = 0;
        int32_t d_value = 0;
        for ( ; i < 3; i++) {
            size_t msg_size = sizeof( sns_smgr_periodic_report_ind_msg_v01);

            void *pMem = malloc( msg_size);
            if ( pMem == NULL) {
                FatalError( OOM);
            }
            memset( pMem, 0, msg_size);

            sns_smgr_periodic_report_ind_msg_v01 *pResp = pMem;
            pResp->ReportId = 1;
            pResp->status = 2;
            pResp->CurrentRate = 3;
            pResp->Item_len = 1;
            pResp->Item[0].SensorId = 5;
            pResp->Item[0].ItemData[0] = d_value++;
            pResp->Item[0].ItemData[1] = d_value++;
            pResp->Item[0].ItemData[2] = d_value++;
            pResp->Item[0].TimeStamp   = d_value++;

            sensor1_msg_header_s hdr;
            hdr.service_number = msg_hdr->service_number;
            hdr.msg_id         = SNS_SMGR_REPORT_IND_V01;
            hdr.msg_size       = msg_size;
            hdr.txn_id         = 0;

            (saved_data_cbf)( saved_cb_data, &hdr, SENSOR1_MSG_TYPE_IND, pMem);        // 1 == QMI_IDL_INDICATION
        }
    }

    return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_alloc_msg_buf(sensor1_handle_s *hndl,
                      uint16_t          size,
                      void            **buffer )
{
    if ( hndl != HANDLE) {
        printf( "sensor1_write: bad handle 0x%x\n", hndl);
        return SENSOR1_EINVALID_CLIENT;
    }

    *buffer = malloc(size);
    return SENSOR1_SUCCESS;
}

sensor1_error_e
sensor1_free_msg_buf(sensor1_handle_s *hndl,
                     void             *msg_buf )
{
    if ( hndl != HANDLE) {
        printf( "sensor1_write: bad handle 0x%x\n", hndl);
        return SENSOR1_EINVALID_CLIENT;
    }

    free( msg_buf);
    return SENSOR1_SUCCESS;
}


