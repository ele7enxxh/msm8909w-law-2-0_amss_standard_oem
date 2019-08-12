#ifndef SENSORRDIAG_H
#define SENSORRDIAG_H

/*=====================================================================
  @file sensorrdiag.h

  @brief
  This contains definitions for Remote Diag command implementation.

  @note
  Many of the data structures defined in this file were specifically
  duplicated from sns_diagproxy.h, for backward compatibility purposes.
  Some structure fields do not match the sensor1 API in size.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=====================================================================*/

/*=====================================================================
   INCLUDE FILES
=====================================================================*/
#include "comdef.h"
#include "diagpkt.h"
#include <stdint.h>

/*=====================================================================
   DEFINES
=====================================================================*/

/* Packet type 2/3 Sensor Header value defines */
#define SNS_DIAG_SENSOR1_OPEN       1
#define SNS_DIAG_SENSOR1_WRITE      2
#define SNS_DIAG_SENSOR1_CLOSE      3
#define SNS_DIAG_SENSOR1_WRITABLE   4

/*=====================================================================
   STRUCTURES
=====================================================================*/

/* Packet type values for the different messages */
typedef enum
{
  SNS_DIAG_SENSOR1_QMI_MSG = 2, // Message is QMI encoded
  SNS_DIAG_SENSOR1_MSG =3 // Raw byte stream  (i.e. sent decoded)
} sns_rdiag_pkt_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint8_t pkt_type; // Of type sns_rdiag_pkt_type
  uint8_t payload[1];
} sns_rdiag_handle_req;

/* Sensor Header for Packet Type 2/3 - Requests to Sensor1 API */
typedef uint8_t sns_rdiag_header_type;

/* Sensor1_Write request message body */
typedef PACK(struct)
{
  uint32_t cbdata;
  uint8_t srvc_num;  // Note: uint32_t in sensor1 API
  int16_t msg_id;    // Note: int32_t in sensor1 API
  int8_t msg_type;
  uint8_t txn_id;
  uint16_t msg_size;
  byte qmi_msg[1];
} sns_rdiag_write_msgbody;

/* Sensor1_Close request message body */
typedef PACK(struct)
{
  uint32_t cbdata;
} sns_rdiag_close_msgbody;

/* Sensor1_Writable request message body */
typedef PACK(struct)
{
  uint32_t cbdata;
  uint32_t srvc_num;
} sns_rdiag_writable_msgbody;

/* Sensor1 Request */
typedef PACK(struct)
{
  sns_rdiag_header_type cm_fn;
  PACK(union)
  {
    sns_rdiag_write_msgbody write_msgbody;
    sns_rdiag_close_msgbody close_msgbody;
    sns_rdiag_writable_msgbody writable_msgbody;
  };
  /* Don't add any items to the end of structure. Write msg_body
     has a variable length buffer and needs to be at the end
     of struct. */
} sns_rdiag_msg_body;

/* Delayed response DIAG header */
typedef PACK(struct)
{
  uint8_t command_code;
  uint8_t subsys_id;
  uint16_t subsys_cmd_code;
  uint32_t status;
  uint16_t delayed_rsp_id;
  uint16_t rsp_cnt;  /* 0: one response; 1: two responses */
} sns_rdiag_subsys_hdr_v2_type;

/* Packet Type 2/3 (Sensor1 Request) Immediate Response */
typedef PACK(struct)
{
  sns_rdiag_subsys_hdr_v2_type  diag_hdr;
  int32_t sensor1_api_ret_value;
  uint32_t cb_data;
  uint32_t context;
  uint16_t delayed_rsp_id;
  uint8_t txn_id;
} sns_rdiag_sensor1_immediate_res_msg;

/* Packet type 2/3 (Sensor1 Request) Delayed Response */
typedef PACK(struct)
{
  sns_rdiag_subsys_hdr_v2_type hdr;
  uint16_t delayed_rsp_id;
  uint8_t txn_id;
  uint32_t cb_data;
  uint32_t context;
  uint8_t srvc_num;   // Note: uint32_t in sensor1 API
  uint16_t msg_id;    // Note: uint32_t in sensor1 API
  uint8_t msg_type;
  uint16_t msg_size;
  uint8_t msg;
} sns_rdiag_sensor1_delayed_rsp_msg;

typedef struct libsensor_svc_accessor
{
  qmi_idl_service_object_type (*get_svc)(int32_t, int32_t, int32_t);
  int32_t maj_ver;
  int32_t min_ver;
  int32_t tool_ver;
} libsensor_svc_accessor;

#endif  // SENSORRDIAG_H
