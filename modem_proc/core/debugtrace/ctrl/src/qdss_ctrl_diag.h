#ifndef QDSS_CTRL_DIAG_H
#define QDSS_CTRL_DIAG_H

/*=============================================================================

FILE:         qdss_ctrl_diag.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "diagpkt.h"
#include "diagcmd.h"

#define QDSS_CTRL_FILTER_ETM                0x02
#define QDSS_CTRL_FILTER_ETM_RPM            0x40
#define QDSS_CTRL_ETM_CONFIG 	            0x41
#define QDSS_ETM_CONFIG_TESTBUS				0x01
#define QDSS_ETM_CONFIG_DEFAULT_MODE        0x02
#define QDSS_CTRL_TEST_CMD                  0xFF


typedef PACK(struct)
{
  uint8 cmdCode;        /**< Diag Message ID */
  uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
  uint16 subsysCmdCode; /**< Subsystem command code */
} qdss_ctrl_diag_pkt_hdr;

/*-------------------------------------------------------------------------*/
/**
  @brief Response packet: Generic reponse with result.

  Common packet when only returning a result.
 */

typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
} qdss_ctrl_diag_pkt_req;


typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr; /**< Header */
  uint8 result;            /**< See QDSS_CMDRESP_... definitions */
} qdss_ctrl_diag_pkt_rsp;


typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint8  state;
} qdss_ctrl_filter_etm_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_filter_etm_rsp; 

int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                                 int req_len,
                                 qdss_ctrl_filter_etm_rsp *pRsp,
                                 int rsp_len);


typedef PACK(struct)
{
   qdss_ctrl_diag_pkt_hdr hdr;
   uint8  state;
} qdss_ctrl_filter_etm_rpm_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_filter_etm_rpm_rsp;

int qdss_ctrl_filter_etm_rpm_handler(qdss_ctrl_filter_etm_rpm_req *pReq,
                                     int req_len,
                                     qdss_ctrl_filter_etm_rpm_rsp *pRsp,
                                     int rsp_len);
/*
* ETM Test bus config req, rsp and handler declarations
*
*/
typedef PACK(struct)
{
   qdss_ctrl_diag_pkt_hdr hdr;
   uint8  etm_cmd_id;
   uint8  reserved[3];
   uint32 testbus_mode;
} qdss_ctrl_etm_testbus_req;

typedef qdss_ctrl_etm_testbus_req  qdss_ctrl_etm_mode_req;

typedef PACK(struct)
{
   qdss_ctrl_diag_pkt_hdr hdr;
   uint8  etm_cmd_id;
   uint8  reserved[3];
   uint32 default_etm_mode;
} qdss_ctrl_etm_default_req;

typedef PACK(struct)
{
   qdss_ctrl_diag_pkt_hdr hdr;
   uint8  etm_cmd_id;
   uint8  reserved[3];
   uint8 result;
} qdss_ctrl_etm_mode_rsp;

typedef qdss_ctrl_etm_mode_rsp qdss_ctrl_etm_testbus_rsp;
typedef qdss_ctrl_etm_mode_rsp qdss_ctrl_etm_default_rsp;

int qdss_ctrl_etm_testbus_handler(qdss_ctrl_etm_testbus_req *pReq,
                                     int req_len,
                                     qdss_ctrl_etm_testbus_rsp *pRsp,
                                     int rsp_len);
									 
int qdss_ctrl_etm_default_handler(qdss_ctrl_etm_default_req *pReq,
                                     int req_len,
                                     qdss_ctrl_etm_default_rsp *pRsp,
                                     int rsp_len);

typedef PACK(struct)
{
   qdss_ctrl_diag_pkt_hdr hdr;
   uint8  test_num;
   uint8  args[1];
} qdss_ctrl_test_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_test_rsp;

#define QDSS_TRACER_DATA_TEST 1

typedef PACK(struct)
{
   uint8 count;
} qdss_ctrl_tracer_data_test_args;

#define QDSS_TRACER_TEST    3

typedef PACK(struct)
{
   uint8 type;
   uint8 subtype;
   uint16 option;
   uint32 mask;
   uint32 count1;
   uint32 count2;
} qdss_ctrl_tracer_test_args;

int qdss_ctrl_test_handler(qdss_ctrl_test_req *pReq,
                                 int req_len,
                           qdss_ctrl_test_rsp *pRsp,
                                 int rsp_len);


void qdss_ctrl_diag_init(void);

#endif //QDSS_CTRL_DIAG_H

