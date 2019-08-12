/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           qmi_wms.c --

  The Qualcomm MSM Interface Wireless Message Service source file.


  -------------

    Copyright (c) 2007-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/qmi_wms.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================
EXTERNALIZED FUNCTIONS

  qmi_wms_init
    Register the WMS service with QMUX

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

  #include "mmcp_variation.h"
  #include "comdef.h"
  #include "customer.h"

  #include <stdio.h>

  #include "wms.h"
  #include "msg.h"
  #include "err.h"
  #include "amssassert.h"

  #include "ps_in.h"
  #include "modem_mem.h"


  /* QMI framework headers*/
  #include "qmi_svc_utils.h"
  #include "qmi_framework.h"
  #include "ds_qmi_fw_common.h"

  /* QMI task headers */
  #include "qmi_wms_task_svc.h"

  /* QMI_WMS service header */
  #include "qmi_wms.h"

  /* QMI_WMS service-specific framework header */
  //#include "qmi_wms_svc.h"

#ifdef FEATURE_DOMAIN_SELECTION
  #include "cm_msgr_msg.h"
  #include "fs_public.h"
#endif

  #include "qmi_si.h"
  #include "qmi_idl_lib.h"
  #include "wireless_messaging_service_v01.h"
  #include "wireless_messaging_service_impl_v01.h"
  #include "stringl.h"

  #include "wmsutils.h"
  #include "common_v01.h"
  #include "dsm_pool.h"
  #include "dsm_item.h"
  #include "msg_diag_service.h"


/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
  #define WMSI_MAX_CLIDS    (10)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for WMS
---------------------------------------------------------------------------*/
  #define WMSI_BASE_VER_MAJOR    (1)
  #define WMSI_BASE_VER_MINOR    (10)

  #define WMSI_ADDENDUM_VER_MAJOR  (0)
  #define WMSI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  WMS Service defined values
---------------------------------------------------------------------------*/
  #define WMSI_INVALID_CLIENT_ID         (WMS_DUMMY_CLIENT_ID)
  #define WMSI_MAX_LEN                   (WMS_MAX_LEN)
  #define WMSI_MAX_ETWS_LEN              (WMS_ETWS_MAX_MSG_LEN)
  #define WMSI_MESSAGE_LIST_MAX          (WMS_MESSAGE_LIST_MAX)
  #define WMSI_MESSAGE_CLASS_MAX         (WMS_MESSAGE_CLASS_MAX)
  #define WMSI_ADDRESS_MAX               (WMS_ADDRESS_MAX)
  #define WMSI_ADDRESS_TYPE_MAX          (4)
  #define WMSI_BC_SMS_TABLE_SIZE         (50)
  #define WMSI_LINK_TIMER_MAX            (5)
  #define WMSI_DEST_ADDR_LEN             (12)
  #define WMSI_SMSC_ADDRESS_MAX          (22)
  #define WMSI_MT_SMSC_ADDRESS_MAX       (11)
  #define WMSI_ALPHA_ID_MAX              (255)
  #define WMSI_DBG_RETRY_TIMER_MAX       (0xFFFFFFFF)
  #define WMSI_ETWS_NOTIFICATION_TYPE_MAX (3)
  #define WMSI_MAX_MOBILE_CODE           (999)
  #define WMSI_MWI_TUPLE_MAX             (3)

/*---------------------------------------------------------------------------
  For internal conversion to 4-bit DTMF encoding
---------------------------------------------------------------------------*/
  #define WMSI_SMSC_DTMF_MAX        12
  #define WMSI_SMSC_DTMF_END_MARKER 15

  /*---------------------------------------------------------------------------
	Message-internal TLV type field values
  ---------------------------------------------------------------------------*/
  #define WMSI_PRM_TYPE_SEND_ACK_IND_USER_DATA                   (0x11)  
  #define WMSI_PRM_TYPE_ASYNC_SEND_IND_USER_DATA                (0x17)
  #define WMSI_PRM_TYPE_ASYNC_MEM_STORE_IND_USER_DATA            (0x17)
  #define WMSI_PRM_TYPE_TRANSPORT_MWI_ADDR                       (0x02)  
  #define WMSI_PRM_TYPE_TRANSPORT_MWI_DETAILS                    (0x10) 
  
/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
  #define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define QMI_WMSI_MAX_VAL_NUM_ITEM 0xFFFFFFFF /*  Max value of a numeric AT parm     */

#define QMI_WMSI_MEM_FREEIF(p)      if (p) { qmi_wmsi_mem_free((void*)p); (p) = 0; }

/*---------------------------------------------------------------------------
  MWI definitions
---------------------------------------------------------------------------*/
#define WMSI_MASK_EF_MWIS_VOICEMAIL     0x01
#define WMSI_MASK_EF_MWIS_FAX           0x02
#define WMSI_MASK_EF_MWIS_EMAIL         0x04
#define WMSI_MASK_EF_MWIS_OTHER         0x08
#define WMSI_MASK_EF_MWIS_VIDEOMAIL     0x10

#define WMSI_EF_MWIS_VOICEMAIL_POS      1
#define WMSI_EF_MWIS_FAX_POS            2
#define WMSI_EF_MWIS_EMAIL_POS          3
#define WMSI_EF_MWIS_OTHER_POS          4
#define WMSI_EF_MWIS_VIDEOMAIL_POS      5

#define WMS_MAX_RETRY_PERIOD            240       //referenced from "wmsi.h"

/*---------------------------------------------------------------------------
	Macro used in event handlers (common)
---------------------------------------------------------------------------*/
#define ENCODE_RESPONSE_TLV(pdu_type, message_type)  { result = (QMI_ERR_NONE == errval) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE; \
                                                       ((pdu_type*)response_pdu)->resp.result = (qmi_result_type_v01)result; \
                                                       ((pdu_type*)response_pdu)->resp.error  = (qmi_error_type_v01)errval; \
                                                       qmi_wms_idl_message_encode( qmi_wmsi_global.svc_obj, \
                                                                                   (uint16_t)QMI_IDL_RESPONSE, \
                                                                                   (uint16_t)message_type, \
                                                                                   (void *) response_pdu, \
                                                                                   sizeof( pdu_type ), \
                                                                                   &response); }

#define ENCODE_INDICATION_TLV(pdu_type, message_type)  { qmi_wms_idl_message_encode( qmi_wmsi_global.svc_obj, \
                                                                                     (uint16_t)QMI_IDL_INDICATION, \
                                                                                     (uint16_t)message_type, \
                                                                                     (void *) ind_pdu, \
                                                                                     sizeof( pdu_type ), \
                                                                                     &ind); }

#define DECODE_REQUEST_TLV(pdu_type, message_type, pdu_name)    qmi_wms_idl_message_decode( qmi_wmsi_global.svc_obj, \
                                                                                            QMI_IDL_REQUEST, \
                                                                                            message_type, \
                                                                                            sdu_in, \
                                                                                            (void*)pdu_name, \
                                                                                            sizeof(pdu_type) ); 


/*===========================================================================

                                DATA TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI WMS message mode enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MSG_MODE_CDMA  = 0x0000,
  WMSI_MSG_MODE_GW    = 0x0001,
  WMSI_MSG_MODE_MAX,
  WMS_MSG_MODE_MAX32  = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_mode_e_type;

/*---------------------------------------------------------------------------
  QMI WMS tpdu type enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MSG_TPDU_TYPE_DELIVER = 0x0000,
  WMSI_MSG_TPDU_TYPE_SUBMIT  = 0x0001,
  WMSI_MSG_TPDU_TYPE_MAX,
  WMSI_MSG_TPDU_TYPE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_tpdu_type_e_type;

/*---------------------------------------------------------------------------
  QMI WMS message write mode enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MSG_WRITE_MODE_INSERT  = 0x0000,
  WMSI_MSG_WRITE_MODE_REPLACE = 0x0001,
  WMSI_MSG_WRITE_MODE_MAX,
  WMSI_MSG_WRITE_MODE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_write_mode_e_type;

/*---------------------------------------------------------------------------
  QMI WMS message format enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MSG_FORMAT_CDMA  = 0x0000,
  WMSI_MSG_FORMAT_GW_PP = 0x0006,
  WMSI_MSG_FORMAT_GW_BC = 0x0007,
  WMSI_MSG_FORMAT_MWI   = 0x0008,
  WMSI_MSG_FORMAT_MAX,
  WMS_MSG_FORMAT_MAX32  = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_format_e_type;

/*---------------------------------------------------------------------------
  QMI WMS memory storage enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MEM_STORE_UIM    = 0x0000,
  WMSI_MEM_STORE_NV     = 0x0001,
  WMSI_MEM_STORE_NONE   = 0x00FF,
  WMSI_MEM_STORE_MAX,
  WMSI_MEM_STORE_MAX32  = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_memory_storage_e_type;

/*---------------------------------------------------------------------------
  QMI WMS tag type enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TAG_MT_READ     = 0x0000,
  WMSI_TAG_MT_NOT_READ = 0x0001,
  WMSI_TAG_MO_SENT     = 0x0002,
  WMSI_TAG_MO_NOT_SENT = 0x0003,
  WMSI_TAG_MAX,
  WMSI_TAG_MAX32       = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_tag_e_type;

/*---------------------------------------------------------------------------
  QMI WMS message class enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MSG_CLASS_0     = 0x0000,
  WMSI_MSG_CLASS_1     = 0x0001,
  WMSI_MSG_CLASS_2     = 0x0002,
  WMSI_MSG_CLASS_3     = 0x0003,
  WMSI_MSG_CLASS_NONE  = 0x0004,
  WMSI_MSG_CLASS_CDMA  = 0x0005,
  WMSI_MSG_CLASS_MAX,
  WMSI_MSG_CLASS_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_class_e_type;

/*---------------------------------------------------------------------------
  QMI WMS route type enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_ROUTE_TYPE_PP    = 0x0000,
  WMSI_ROUTE_TYPE_MAX,
  WMSI_ROUTE_TYPE_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_route_type_e_type;

/*---------------------------------------------------------------------------
  QMI WMS route value enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_ROUTE_DISCARD          = 0x0000,
  WMSI_ROUTE_STORE_AND_NOTIFY = 0x0001,
  WMSI_ROUTE_TRANSFER_ONLY    = 0x0002,
  WMSI_ROUTE_TRANSFER_AND_ACK = 0x0003,
  WMSI_ROUTE_UNKNOWN          = 0x00FF,
  WMSI_ROUTE_MAX,
  WMSI_ROUTE_MAX32            = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_route_e_type;

/*---------------------------------------------------------------------------
  QMI WMS report MT message enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_REPORT_MT_MSG_DISABLE = 0x0000,
  WMSI_REPORT_MT_MSG_ENABLE  = 0x0001,
  WMSI_REPORT_MT_MSG_MAX,
  WMSI_REPORT_MT_MSG_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_report_mt_msg_e_type;

/*---------------------------------------------------------------------------
  QMI WMS report Call Control information enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_REPORT_CALL_CTRL_INFO_DISABLE = 0x0000,
  WMSI_REPORT_CALL_CTRL_INFO_ENABLE  = 0x0001,
  WMSI_REPORT_CALL_CTRL_INFO_MAX,
  WMSI_REPORT_CALL_CTRL_INFO_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_report_call_ctrl_info_e_type;

/*---------------------------------------------------------------------------
  QMI WMS report MWI message enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_REPORT_MWI_MSG_DISABLE = 0x0000,
  WMSI_REPORT_MWI_MSG_ENABLE  = 0x0001,
  WMSI_REPORT_MWI_MSG_MAX,
  WMSI_REPORT_MWI_MSG_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_report_mwi_msg_e_type;

/*---------------------------------------------------------------------------
  QMI WMS report status event enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_REPORT_STATUS_EV_MT_MSG      = 0x0001,
  WMSI_REPORT_STATUS_EV_SUBMIT_RPT  = 0x0002,
  WMSI_REPORT_STATUS_EV_MAX,
  WMSI_REPORT_STATUS_EV_MAX32       = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_report_status_event_mask_e_type;

/*---------------------------------------------------------------------------
  QMI WMS ack failure cause enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_ACK_FAILURE_NO_NETWORK_RESPONSE   = 0x0000,
  WMSI_ACK_FAILURE_NETWORK_RELEASED_LINK = 0x0001,
  WMSI_ACK_FAILURE_ACK_NOT_SENT          = 0x0002,
  WMSI_ACK_FAILURE_MAX,
  WMSI_ACK_FAILURE_MAX32     = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_ack_failure_cause_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Memory Available enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MEMORY_NOT_AVAILABLE   = 0x0000,
  WMSI_MEMORY_AVAILABLE       = 0x0001,
  WMSI_MEMORY_AVAILABLE_MAX,
  WMSI_MEMORY_AVAILABLE_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_mem_avail_e_type;

/*---------------------------------------------------------------------------
  QMI WMS BC preference enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_BC_PREF_DISABLE = 0x0000,
  WMSI_BC_PREF_ENABLE  = 0x0001,
  WMSI_BC_PREF_MAX,
  WMSI_BC_PREF_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_bc_pref_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Error Class enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_ERROR_CLASS_TEMP  = 0x0000,
  WMSI_ERROR_CLASS_PERM  = 0x0001,
  WMSI_ERROR_CLASS_MAX,
  WMSI_ERROR_CLASS_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_error_class_e_type;

/*---------------------------------------------------------------------------
  QMI WMS MO SMS Control Status enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MO_CTRL_STATUS_TYPE_DISALLOWED  = 0x0000,
  WMSI_MO_CTRL_STATUS_TYPE_NO_MOD      = 0x0001,
  WMSI_MO_CTRL_STATUS_TYPE_MOD         = 0x0002,
  WMSI_MO_CTRL_STATUS_TYPE_MAX,
  WMSI_MO_CTRL_STATUS_TYPE_MAX32       = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_mo_ctrl_status_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Primary Client enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_PRIMARY_CLIENT_UNSET = 0x0000,
  WMSI_PRIMARY_CLIENT_SET   = 0x0001,
  WMSI_PRIMARY_CLIENT_MAX,
  WMSI_PRIMARY_CLIENT_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_primary_client_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Message Waiting enum (GW)
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MWI_VOICEMAIL = 0x0000,
  WMSI_MWI_FAX       = 0x0001,
  WMSI_MWI_EMAIL     = 0x0002,
  WMSI_MWI_OTHER     = 0x0003,
  WMSI_MWI_VIDEOMAIL = 0x0004,
  WMSI_MWI_MAX,
  WMSI_MWI_MAX32     = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_mwi_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Transport Message Waiting enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TRANSPORT_MWI_VOICE      = 0x0000,
  WMSI_TRANSPORT_MWI_VIDEO      = 0x0001,
  WMSI_TRANSPORT_MWI_FAX        = 0x0002,
  WMSI_TRANSPORT_MWI_PAGER      = 0x0003,
  WMSI_TRANSPORT_MWI_MULTIMEDIA = 0x0004,
  WMSI_TRANSPORT_MWI_TEXT       = 0x0005,
  WMSI_TRANSPORT_MWI_MAX,
  WMSI_TRANSPORT_MWI_MAX32      = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_transport_mwi_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Message Waiting enum (GW)
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MWI_UNKNOWN_PRIORITY = 0x0000,
  WMSI_MWI_LOW_PRIORITY     = 0x0001,
  WMSI_MWI_NORMAL_PRIORITY  = 0x0002,
  WMSI_MWI_URGENT_PRIORITY  = 0x0003,
  WMSI_MWI_PRIORITY_MAX,
  WMSI_MWI_PRIORITY_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_mwi_priority_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Indication Register enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_INDICATION_DISABLE = 0x0000,
  WMSI_INDICATION_ENABLE  = 0x0001,
  WMSI_INDICATION_MAX,
  WMSI_INDICATION_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
}qmi_wmsi_indication_register_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Transport Type enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TRANSPORT_TYPE_IMS   = 0x0000,
  WMSI_TRANSPORT_TYPE_MAX,
  WMSI_TRANSPORT_TYPE_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_transport_type_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Transport NW Reg enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TRANSPORT_NW_REG_NO_SERVICE   = 0x0000,
  WMSI_TRANSPORT_NW_REG_IN_PROGRESS  = 0x0001,
  WMSI_TRANSPORT_NW_REG_FAILED       = 0x0002,
  WMSI_TRANSPORT_NW_REG_LIM_SERVICE  = 0x0003,
  WMSI_TRANSPORT_NW_REG_FULL_SERVICE = 0x0004,
  WMSI_TRANSPORT_NW_REG_MAX,
  WMSI_TRANSPORT_NW_REG_MAX32        = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_transport_nw_reg_status_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Subscription enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_SUBSCRIPTION_PRIMARY   = 0x0000,
  WMSI_SUBSCRIPTION_SECONDARY = 0x0001,
  WMSI_SUBSCRIPTION_TERTIARY  = 0x0002,
  WMSI_SUBSCRIPTION_MAX,
  WMSI_SUBSCRIPTION_MAX32     = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_subscription_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Call Status enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_CALL_STATUS_INCOMING     = 0x0000,
  WMSI_CALL_STATUS_CONNECTED    = 0x0001,
  WMSI_CALL_STATUS_ABORTED      = 0x0002,
  WMSI_CALL_STATUS_DISCONNECTED = 0x0003,
  WMSI_CALL_STATUS_CONNECTING   = 0x0004,
  WMSI_CALL_STATUS_MAX,
  WMSI_CALL_STATUS_MAX32        = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_call_status_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Message Delivery Failure Type enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MESSAGE_DELIVERY_FAILURE_TEMP  = 0x0000,
  WMSI_MESSAGE_DELIVERY_FAILURE_PERM  = 0x0001,
  WMSI_MESSAGE_DELIVERY_FAILURE_MAX,
  WMSI_MESSAGE_DELIVERY_FAILURE_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_delivery_failure_type_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Message Delivery Failure Cause enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MESSAGE_BLOCKED_DUE_TO_CALL_CONTROL = 0x0000,
  WMSI_MESSAGE_DELIVERY_FAILURE_CAUSE_MAX,
  WMSI_MESSAGE_DELIVERY_FAILURE_CAUSE_MAX32  = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_message_delivery_failure_cause_e_type;

/*---------------------------------------------------------------------------
  QMI WMS LTE Domain Pref enum
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_LTE_DOMAIN_PREF_NONE  = 0x0000,
  WMSI_LTE_DOMAIN_PREF_IMS   = 0x0001,
  WMSI_LTE_DOMAIN_PREF_MAX,
  WMSI_LTE_DOMAIN_PREF_MAX32 = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_lte_domain_pref_e_type;

/*-------------------------------------------------------------------------
  Types of sms message format mode.
  Refer to 3GPP TS27.005 3.2.3 Message Format +CMGF
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_WMS_TEXT_MODE,
  QMI_WMS_PDU_MODE
} qmi_wmsi_fmt_mode_e_type;

/*---------------------------------------------------------------------------
     Specifies dial character validation result
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_WMSI_DIAL_EMPTY,          /*  Empty                    */
  QMI_WMSI_DIAL_DIGIT,          /*  Digits                   */
  QMI_WMSI_DIAL_ASCII,          /*  ASCII chars              */
  QMI_WMSI_DIAL_ASCII_ABC,      /*  ASCII A, B, or C         */
  QMI_WMSI_DIAL_ASCII_D,        /*  ASCII D                  */
  QMI_WMSI_DIAL_ASCII_E,        /*  ASCII E                  */
  QMI_WMSI_DIAL_ALLOWED,        /*  OTHER ALLOWED CHARACTERS */
  QMI_WMSI_DIAL_SEMICOLON,      /*  SEMICOLON                */
  QMI_WMSI_DIAL_STAR,           /*  ASTERIX                  */
  QMI_WMSI_DIAL_POUND,          /*  POUND                    */
  QMI_WMSI_DIAL_PLUS,           /*  PLUS                     */
  QMI_WMSI_DIAL_STRIP,          /*  Stripped chars           */
  QMI_WMSI_DIAL_IGNORE,         /*  Insignificant chars      */
  QMI_WMSI_DIAL_ERROR,          /*  has illegal chars        */
  QMI_WMSI_DIAL_MAX             /*  Internal use only        */
} qmi_wmsi_dial_val_e_type;

typedef enum
{
  WMSI_ATOI_OK,              /*  conversion complete             */
  WMSI_ATOI_NO_ARG,          /*  nothing to convert              */
  WMSI_ATOI_OUT_OF_RANGE     /*  character or value out of range */
} qmi_wmsi_atoi_enum_type;


/*---------------------------------------------------------------------------
  QMI WMS Message Waiting definition
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_MWI_STATE_UNINIT  = 0x0000,
  WMSI_MWI_STATE_INIT    = 0x0001,
  WMSI_MWI_STATE_ERROR   = 0x0002,
  WMSI_MWI_STATE_MAX,
  WMSI_MWI_STATE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_mwi_state_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Transport Layer definition
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TRANSPORT_LAYER_STATE_UNINIT  = 0x0000,
  WMSI_TRANSPORT_LAYER_STATE_INIT    = 0x0001,
  WMSI_TRANSPORT_LAYER_STATE_MAX,
  WMSI_TRANSPORT_LAYER_STATE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_transport_layer_state_e_type;

/*---------------------------------------------------------------------------
  QMI WMS Transport NW Reg Status definition
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_TRANSPORT_NW_REG_STATE_UNINIT  = 0x0000,
  WMSI_TRANSPORT_NW_REG_STATE_INIT    = 0x0001,
  WMSI_TRANSPORT_NW_REG_STATE_MAX,
  WMSI_TRANSPORT_NW_REG_STATE_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_transport_nw_reg_state_e_type;

/*---------------------------------------------------------------------------
  QMI WMS GW template definition
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                        inited;
  qmi_wmsi_memory_storage_e_type mem_store;
  uint32                         index;
  wms_gw_template_s_type         mo_template;
} qmi_wmsi_gw_template_type;

typedef struct
{
  qmi_wmsi_mwi_e_type type;                 /* Type of Message Waiting Indication */
  boolean             active;               /* Whether the indication is active or not */
  uint32              number_of_messages;   /* Number of messages waiting */
} qmi_wmsi_mwi_info_type;

typedef struct
{
  boolean                      refresh_due_to_fcn;
  qmi_wmsi_mwi_state_e_type    mwi_state;
  uint8                        num_mwi;
  qmi_wmsi_mwi_info_type       mwi[WMSI_MWI_MAX];
} qmi_wmsi_mwi_cache_type;

typedef struct
{
  qmi_wmsi_transport_layer_state_e_type state;
  boolean                               registered_ind;
  qmi_wmsi_transport_type_e_type        transport_type;
  qmi_wmsi_message_mode_e_type          transport_cap;
  sys_modem_as_id_e_type                as_id;
} qmi_wmsi_transport_layer_type;

typedef struct
{
  qmi_wmsi_transport_nw_reg_state_e_type  state;
  qmi_wmsi_transport_nw_reg_status_e_type nw_reg_status;
  sys_modem_as_id_e_type                  as_id;
} qmi_wmsi_transport_nw_reg_status_type;

typedef struct qmi_wmsi_report_event_info_s
{
  uint32 mask;
  boolean is_etws;

  /* new MT message params */
  wms_memory_store_e_type storage_type;
  uint32                  storage_index;
  boolean                 ack_indicator;
  uint32                  transaction_id;
  wms_route_e_type        route;
  uint8                   format;
  uint16                  len;
  uint8                   data[WMSI_MAX_LEN];

  /* SMSC Address */
  boolean smsc_address_incl;
  uint8   smsc_address[WMSI_MT_SMSC_ADDRESS_MAX + 1]; // 1st byte is the length

  /* ETWS params */
  uint8                   notification_type;
  uint16                  etws_len;
  uint8                   etws_data[WMSI_MAX_ETWS_LEN];
  boolean                 plmn_info_present;
  uint16                  mcc;
  uint16                  mnc;
  boolean                 sms_on_ims;

  /* Call control information */
  qmi_wmsi_mo_ctrl_status_e_type mo_ctrl_status;
  uint8                          alpha_id_len;
  uint8                          alpha_id[WMSI_ALPHA_ID_MAX];

} qmi_wmsi_report_event_info_s_type;

/*---------------------------------------------------------------------------
  WMS Command enum type - not equal to the actual command values!
  mapping is in qmi_wms_cmd_callbacks table

  DO NOT REORDER THIS ENUM!
---------------------------------------------------------------------------*/
typedef enum
{
  WMSI_CMD_MIN                          =  0,
  WMSI_CMD_RESET                        =  WMSI_CMD_MIN,
  WMSI_CMD_SET_EVENT_REPORT,
  WMSI_CMD_RAW_SEND,
  WMSI_CMD_RAW_WRITE,
  WMSI_CMD_RAW_READ,
  WMSI_CMD_MODIFY_TAG,
  WMSI_CMD_DELETE,
  WMSI_CMD_GET_MESSAGE_MODE,
  WMSI_CMD_LIST_MESSAGES,
  WMSI_CMD_SET_ROUTES,
  WMSI_CMD_GET_ROUTES,
  WMSI_CMD_GET_SMSC_ADDR,
  WMSI_CMD_SET_SMSC_ADDR,
  WMSI_CMD_GET_STORE_MAX_SIZE,
  WMSI_CMD_SEND_ACK,
  WMSI_SET_RETRY_PERIOD,
  WMSI_SET_RETRY_INTERVAL,
  WMSI_SET_DC_DISCONNECT_TIMER,
  WMSI_CMD_SET_MEMORY_STATUS,
  WMSI_CMD_SET_BROADCAST_ACTIVATION,
  WMSI_CMD_SET_BROADCAST_CONFIG,
  WMSI_CMD_GET_BROADCAST_CONFIG,
  WMSI_CMD_GET_DOMAIN_PREF,
  WMSI_CMD_SET_DOMAIN_PREF,
  WMSI_CMD_SEND_FROM_MEM_STORE,
  WMSI_CMD_GET_MESSAGE_WAITING,
  WMSI_CMD_SET_PRIMARY_CLIENT,
  WMSI_CMD_INDICATION_REGISTER,
  WMSI_CMD_GET_TRANSPORT_LAYER_INFO,
  WMSI_CMD_GET_TRANSPORT_NW_REG_INFO,
  WMSI_CMD_BIND_SUBSCRIPTION,
  WMSI_CMD_GET_INDICATION_REGISTER,
  WMSI_CMD_GET_SMS_PARAMETERS,
  WMSI_CMD_SET_SMS_PARAMETERS,
  WMSI_CMD_GET_DOMAIN_PREF_CONFIG,
  WMSI_CMD_SET_DOMAIN_PREF_CONFIG,
  WMSI_CMD_GET_RETRY_PERIOD,
  WMSI_CMD_GET_RETRY_INTERVAL,
  WMSI_CMD_GET_DC_DISCONNECT_TIMER,
  WMSI_CMD_GET_MEMORY_STATUS,
  WMSI_CMD_GET_PRIMARY_CLIENT,
  WMSI_CMD_GET_SUBSCRIPTION_BINDING,
  WMSI_CMD_ASYNC_RAW_SEND,
  WMSI_CMD_ASYNC_SEND_ACK,
  WMSI_CMD_ASYNC_SEND_FROM_MEM_STORE,
  WMSI_CMD_GET_SERVICE_READY_STATUS,
  WMSI_CMD_SET_MESSAGE_WAITING,
  WMSI_CMD_MAX,
  WMSI_CMD_WIDTH                        = 0xFFFF
} qmi_wmsi_cmd_e_type;

typedef enum
{
  WMSI_CMD_VAL_MIN                       = 0x0000,
  WMSI_CMD_VAL_RESET                     = 0x0000,
  WMSI_CMD_VAL_SET_EVENT_REPORT          = 0x0001,
  WMSI_CMD_VAL_RAW_SEND                  = 0x0020,
  WMSI_CMD_VAL_RAW_WRITE                 = 0x0021,
  WMSI_CMD_VAL_RAW_READ                  = 0x0022,
  WMSI_CMD_VAL_MODIFY_TAG                = 0x0023,
  WMSI_CMD_VAL_DELETE                    = 0x0024,
  WMSI_CMD_VAL_GET_MESSAGE_PROTOCOL      = 0x0030,
  WMSI_CMD_VAL_LIST_MESSAGES             = 0x0031,
  WMSI_CMD_VAL_SET_ROUTES                = 0x0032,
  WMSI_CMD_VAL_GET_ROUTES                = 0x0033,
  WMSI_CMD_VAL_GET_SMSC_ADDR             = 0x0034,
  WMSI_CMD_VAL_SET_SMSC_ADDR             = 0x0035,
  WMSI_CMD_VAL_GET_STORE_MAX_SIZE        = 0x0036,
  WMSI_CMD_VAL_SEND_ACK                  = 0x0037,
  WMSI_CMD_VAL_SET_RETRY_PERIOD          = 0x0038,
  WMSI_CMD_VAL_SET_RETRY_INTERVAL        = 0x0039,
  WMSI_CMD_VAL_SET_DC_DISCONNECT_TIMER   = 0x003A,
  WMSI_CMD_VAL_SET_MEMORY_STATUS         = 0x003B,
  WMSI_CMD_VAL_SET_BROADCAST_ACTIVATION  = 0x003C,
  WMSI_CMD_VAL_SET_BROADCAST_CONFIG      = 0x003D,
  WMSI_CMD_VAL_GET_BROADCAST_CONFIG      = 0x003E,
  WMSI_CMD_VAL_MEMORY_FULL_IND           = 0x003F,
  WMSI_CMD_VAL_GET_DOMAIN_PREF           = 0x0040,
  WMSI_CMD_VAL_SET_DOMAIN_PREF           = 0x0041,
  WMSI_CMD_VAL_SEND_FROM_MEM_STORE       = 0x0042,
  WMSI_CMD_VAL_GET_MESSAGE_WAITING       = 0x0043,
  WMSI_CMD_VAL_MESSAGE_WAITING_IND       = 0x0044,
  WMSI_CMD_VAL_SET_PRIMARY_CLIENT        = 0x0045,
  WMSI_CMD_VAL_SMSC_ADDR_IND             = 0x0046,
  WMSI_CMD_VAL_INDICATION_REGISTER       = 0x0047,
  WMSI_CMD_VAL_GET_TRANSPORT_LAYER_INFO  = 0x0048,
  WMSI_CMD_VAL_TRANSPORT_LAYER_INFO_IND  = 0x0049,
  WMSI_CMD_VAL_GET_TRANSPORT_NW_REG_INFO = 0x004A,
  WMSI_CMD_VAL_TRANSPORT_NW_REG_INFO_IND = 0x004B,
  WMSI_CMD_VAL_BIND_SUBSCRIPTION         = 0x004C,
  WMSI_CMD_VAL_GET_INDICATION_REGISTER   = 0x004D,
  WMSI_CMD_VAL_GET_SMS_PARAMETERS        = 0x004E,
  WMSI_CMD_VAL_SET_SMS_PARAMETERS        = 0x004F,
  WMSI_CMD_VAL_CALL_STATUS_IND           = 0x0050,
  WMSI_CMD_VAL_GET_DOMAIN_PREF_CONFIG    = 0x0051,
  WMSI_CMD_VAL_SET_DOMAIN_PREF_CONFIG    = 0x0052,
  WMSI_CMD_VAL_GET_RETRY_PERIOD          = 0x0053,
  WMSI_CMD_VAL_GET_RETRY_INTERVAL        = 0x0054,
  WMSI_CMD_VAL_GET_DC_DISCONNECT_TIMER   = 0x0055,
  WMSI_CMD_VAL_GET_MEMORY_STATUS         = 0x0056,
  WMSI_CMD_VAL_GET_PRIMARY_CLIENT        = 0x0057,
  WMSI_CMD_VAL_GET_SUBSCRIPTION_BINDING  = 0x0058,
  WMSI_CMD_VAL_ASYNC_RAW_SEND            = 0x0059,
  WMSI_CMD_VAL_ASYNC_RAW_SEND_IND        = 0x0059,
  WMSI_CMD_VAL_ASYNC_SEND_ACK            = 0x005A,
  WMSI_CMD_VAL_ASYNC_SEND_ACK_IND        = 0x005A,
  WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE = 0x005B,
  WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE_IND = 0x005B,
  WMSI_CMD_VAL_GET_SERVICE_READY_STATUS  = 0x005C,
  WMSI_CMD_VAL_SERVICE_READY_IND         = 0x005D,
  WMSI_CMD_VAL_BROADCAST_CONFIG_IND      = 0x005E,
  WMSI_CMD_VAL_SET_MESSAGE_WAITING       = 0x005F,
  WMSI_CMD_VAL_TRANSPORT_LAYER_MWI_IND   = 0x0060,
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555.
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  WMSI_CMD_VAL_MAX,
  WMSI_CMD_VAL_WIDTH                    = 0xFFFF
} qmi_wmsi_cmd_val_e_type;

/*---------------------------------------------------------------------------
  QMI Task types - used to identify what type of task buffers are being
  processed.
---------------------------------------------------------------------------*/
typedef enum
{
  /* framework generated tasks */
  WMS_TASK_CMD_ALLOC_CLID = 1,
  WMS_TASK_CMD_DEALLOC_CLID,
  WMS_TASK_CMD_INIT,
  WMS_TASK_CMD_CMD_HDLR,

  /* subsystem generated tasks */
  WMS_TASK_CMD_CMD_STATUS,
  WMS_TASK_CMD_MSG_EVENT,
  WMS_TASK_CMD_CFG_EVENT,
  WMS_TASK_CMD_DC_EVENT,
  WMS_TASK_CMD_BC_MM_EVENT,
  WMS_TASK_CMD_DC_DISCONNECT,
  WMS_TASK_CMD_TRANSPORT_STATUS,
  WMS_TASK_CMD_DBG_EVENT,

  WMS_TASK_CMD_MAX,
  WMS_TASK_CMD_WIDTH  = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wms_task_cmd_e_type;

typedef int (* qmi_wmsi_compare_func)
(
  qmi_cmd_buf_type * cmd_buf_p,
  void *             compare_val
);


/*---------------------------------------------------------------------------
  QMI WMS client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_wmsi_client_state_s
{
  qmi_common_client_state_type common; // must be first since we alias
  int16 instance;
  int16 service_id;

  struct                             /* client event ind. config */
  {
    boolean  report_mt_message;
    boolean  report_call_control_info;
    boolean  report_mwi_message;
  } report_status;

  struct
  {
    qmi_wmsi_subscription_e_type subscription_type;
    boolean                      primary_client_state[WMSI_SUBSCRIPTION_MAX];
  } subscription_status;

  struct
  {
    boolean reg_transport_layer_events;
    boolean reg_transport_nw_reg_events;
    boolean reg_call_status_events;
    boolean reg_service_ready_events;
    boolean reg_broadcast_config_events;
    boolean reg_transport_layer_mwi_events;
    boolean reg_sim_ready_events;
  } indication_reg;

} qmi_wmsi_client_state_type;


typedef uint32 qmi_wms_tid_type;

typedef struct qmi_wms_cmd_node_s
{
  q_link_type                   link;  // must be first
  qmi_cmd_buf_type             *cmd_buf;
  uint16                        cmd_type;
  boolean                       async_cmd;
  qmi_wms_tid_type              tid;
  qmi_wmsi_client_state_type   *wms_cl_sp;

  /* Cache used to store request info for later processing */
  union
  {
    struct
    {
      wms_memory_store_e_type   storage_type;
      wms_message_tag_e_type    tag_type;
    } get_list;
    struct
    {
      boolean pending;
    } get_routes;
    struct
    {
      wms_message_tag_e_type    new_tag_type;
    } modify_tag;
    struct
    {
      wms_memory_store_e_type   storage_type;
    } get_store_max_size;
    struct
    {
      wms_client_message_s_type client_msg;
      uint32                    wms_msgid;
      boolean                   sms_on_ims;
      boolean                   retry_message;
      uint32                    retry_message_id;
      wms_mo_ctrl_status_e_type mo_ctrl_status;
      uint8                     alpha_id_len;
      uint8                     alpha_id[WMSI_ALPHA_ID_MAX];
      boolean                   got_user_data_tlv;
      uint32                    user_data;
    } raw_send;
    struct
    {
      boolean pending;
      qmi_wmsi_bc_pref_e_type bc_pref;
      qmi_wmsi_message_mode_e_type mode;
    } get_bc_config;
    struct
    {
      boolean pending;
      qmi_wmsi_message_mode_e_type mode;
      uint8 num_entries;
      struct
      {
        boolean selected;
        union
        {
          wms_bc_service_id_s_type       bc_srv_id;
          wms_gw_cb_srv_range_s_type     gw_cb_range;
        } u;
      } bc_table[WMSI_BC_SMS_TABLE_SIZE];
    } set_bc_config;
    struct
    {
      boolean pending;
    } get_domain_pref;
    struct
    {
      boolean enabled;
    } primary_client;
    struct
    {
      boolean                   got_user_data_tlv;
      uint32                    user_data;
    } send_ack;
    struct
    {
      boolean  pending;
    } get_message_waiting;
  } cache;
} qmi_wms_cmd_node_type;

typedef struct qmi_wms_cmd_hdlr_s
{
  qmi_framework_msg_hdr_type  hdr;
  dsm_item_type              *sdu;
} qmi_wms_cmd_hdlr_type;

typedef struct qmi_wms_cmd_status_s
{
  wms_cmd_id_e_type         cmd;
  const void *              user_data;
  wms_cmd_err_e_type        cmd_err;
} qmi_wms_cmd_status_type;

typedef struct qmi_wms_msg_event_s
{
  wms_msg_event_e_type      event;
  wms_msg_event_info_s_type event_info;
} qmi_wms_msg_event_type;

typedef struct qmi_wms_cfg_event_s
{
  wms_cfg_event_e_type      event;
  wms_cfg_event_info_s_type event_info;
} qmi_wms_cfg_event_type;

typedef struct qmi_wms_dc_event_s
{
  wms_dc_event_e_type       event;
} qmi_wms_dc_event_type;

typedef struct qmi_wms_bc_mm_event_s
{
  wms_bc_mm_event_e_type event;
  wms_bc_mm_event_info_s_type event_info;
} qmi_wms_bc_mm_event_type;



typedef struct qmi_wms_transport_status_s
{
  wms_msg_transport_status_s_type status_info;
} qmi_wms_transport_status_type;

typedef struct qmi_wms_dbg_event_s
{
  wms_dbg_event_e_type event;
  wms_dbg_event_info_s_type event_info;
} qmi_wms_dbg_event_type;

typedef struct qmi_wms_task_buffer_s
{
  q_link_type                  link;  // must be first

  qmi_wms_task_cmd_e_type task;
  union task_data_u
  {
    qmi_framework_common_msg_hdr_type fw_msg_hdr;
    uint16                            num_instances;
    qmi_wms_cmd_hdlr_type             cmd_hdlr;
    qmi_wms_cmd_status_type           cmd_status;
    qmi_wms_msg_event_type            msg_event;
    qmi_wms_cfg_event_type            cfg_event;
    qmi_wms_dc_event_type             dc_event;
    qmi_wms_bc_mm_event_type          bc_mm_event;
    qmi_wms_transport_status_type     transport_status;
    qmi_wms_dbg_event_type            dbg_event;
  } data;
} qmi_wms_task_buffer_s_type;


/*---------------------------------------------------------------------------
  QMI WMS state definition
---------------------------------------------------------------------------*/
typedef struct qmi_wmsi_state_s
{
  qmi_common_svc_state_type   common; // must be first since we alias
  qmi_wmsi_client_state_type *client[WMSI_MAX_CLIDS];
  uint16                      num_instances;
} qmi_wmsi_state_type;

/*---------------------------------------------------------------------------
 QMI WMS Service Ready Status definition
---------------------------------------------------------------------------*/
typedef enum {
  WMSI_SERVICE_READY_STATUS_NONE  = 0x0000,
  WMSI_SERVICE_READY_STATUS_3GPP  = 0x0001,
  WMSI_SERVICE_READY_STATUS_3GPP2 = 0x0002,
  WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2 = 0x0003,
  WMSI_SERVICE_READY_STATUS_MAX,
  WMSI_SERVICE_READY_STATUS_MAX32   = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_service_ready_status_e_type;

typedef struct
{
  boolean                              registered_ind;
  qmi_wmsi_service_ready_status_e_type status;
} qmi_wmsi_service_ready_status_type;

/*---------------------------------------------------------------------------
 QMI WMS SIM Ready Status definition
---------------------------------------------------------------------------*/
/* use the enum to match the mask value. */
typedef enum {
  WMSI_SIM_READY_STATUS_NONE           = 0x0000,
  WMSI_SIM_READY_STATUS_3GPP           = 0x0001,
  WMSI_SIM_READY_STATUS_3GPP2          = 0x0002,
  WMSI_SIM_READY_STATUS_3GPP_AND_3GPP2 = 0x0003,
  WMSI_SIM_READY_STATUS_MAX,
  WMSI_SIM_READY_STATUS_MAX32          = 0x7FFFFFFF /* forces enum values to 32 bits */
} qmi_wmsi_sim_ready_status_e_type;

typedef struct
{
  boolean                              sim_ready_reg_ind;
  qmi_wmsi_sim_ready_status_e_type     sim_ready_status;
} qmi_wmsi_sim_ready_status_type;

/*---------------------------------------------------------------------------
  QMI_WMS Global state information - initialized in qmi_wms_init()
  Global to all QMI instances
---------------------------------------------------------------------------*/
static struct
{
  boolean                      inited;

  q_type                       cmd_pend_q;
  q_type                       task_pend_q;

  wms_client_id_type           wms_cid;
  boolean                      wms_cid_activated;
  qmi_wmsi_message_mode_e_type message_mode;
  qmi_wmsi_gw_template_type    gw_template[WMSI_SUBSCRIPTION_MAX];
  qmi_wmsi_mwi_cache_type      mwi[WMSI_SUBSCRIPTION_MAX];
  qmi_wmsi_transport_layer_type transport_layer;
  qmi_wmsi_transport_nw_reg_status_type transport_nw_reg_status;
  qmi_wmsi_service_ready_status_type service_ready_status[WMSI_SUBSCRIPTION_MAX];
  qmi_wmsi_sim_ready_status_type        sim_ready_status[WMSI_SUBSCRIPTION_MAX];

  /* cache DBG info so it does not have to be re-queried every time */
  uint32                       dbg_retry_period;
  uint32                       dbg_retry_interval;

  wms_client_ch_pref_e_type    default_ch_pref_from_nv;  // cache channel preference from NV 
  wms_client_ch_pref_e_type    last_used_ch_pref;        // last used channel preference
  boolean                      last_used_follow_on_dc;  

  wms_dc_so_e_type             default_service_option;  // cache service option from NV 
  wms_dc_so_e_type             last_used_so;            // last used service option
  qmi_idl_service_object_type  svc_obj;
} qmi_wmsi_global = {FALSE,};

/*
*/
static qmi_wms_tid_type qmi_wmsi_get_next_tid
(
  void
)
{
  static  qmi_wms_tid_type     tid = 1;

  MSG_HIGH_1("qmi_wmsi_get_next_tid() returns tid = %d", tid);

  return  tid++;

} /* end of qmi_wmsi_get_next_tid() */

boolean qmi_wms_check_if_valid_user_data_ptr
(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
   if (NULL == user_data_ptr) 
   {
      MSG_ERROR_0("NULL user_data_ptr");
      return FALSE;
   }

   return TRUE;
}
boolean qmi_wms_check_null_ptr(void* sp , void* cmd_buf_p ,dsm_item_type ** sdu_in)
{
   if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == sdu_in))
   {
     MSG_ERROR_3( "NULL pointer sp = 0x%x, cmd_buf_p = 0x%x, sdu_in = 0x%x",
                 sp,
                 cmd_buf_p,
                 sdu_in );
     return FALSE;
   }

   return TRUE;
}


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers forward declarations
---------------------------------------------------------------------------*/
#ifndef REMOVE_QMI_WMS_RESET_V01
    static dsm_item_type* qmi_wmsi_reset(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_EVENT_REPORT_V01
    static dsm_item_type* qmi_wmsi_set_event_report(void*, void *, void *, dsm_item_type **);
#endif
#if !defined(REMOVE_QMI_WMS_RAW_SEND_V01) || !defined(REMOVE_QMI_WMS_ASYNC_RAW_SEND_V01)
    static dsm_item_type* qmi_wmsi_raw_send(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_RAW_WRITE_V01
    static dsm_item_type* qmi_wmsi_raw_write(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_RAW_READ_V01
    static dsm_item_type* qmi_wmsi_raw_read(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_MODIFY_TAG_V01
    static dsm_item_type* qmi_wmsi_modify_tag(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_DELETE_V01
    static dsm_item_type* qmi_wmsi_delete(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_MESSAGE_PROTOCOL_V01
    static dsm_item_type* qmi_wmsi_get_message_protocol(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_LIST_MESSAGES_V01
    static dsm_item_type* qmi_wmsi_list_messages(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_ROUTES_V01
    static dsm_item_type* qmi_wmsi_set_routes(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_ROUTES_V01
    static dsm_item_type* qmi_wmsi_get_routes(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_SMSC_ADDRESS_V01
    static dsm_item_type* qmi_wmsi_get_smsc_addr(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_SMSC_ADDRESS_V01
    static dsm_item_type* qmi_wmsi_set_smsc_addr(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_STORE_MAX_SIZE_V01
    static dsm_item_type* qmi_wmsi_get_store_max_size(void *, void *, void *, dsm_item_type **);
#endif
#if !defined(REMOVE_QMI_WMS_SEND_ACK_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_ACK_V01)
    static dsm_item_type* qmi_wmsi_send_ack(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_RETRY_PERIOD_V01
    static dsm_item_type* qmi_wmsi_set_retry_period(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_RETRY_INTERVAL_V01
    static dsm_item_type* qmi_wmsi_set_retry_interval(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_DC_DISCONNECT_TIMER_V01
    static dsm_item_type* qmi_wmsi_set_dc_disconnect_timer(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_MEMORY_STATUS_V01
    static dsm_item_type* qmi_wmsi_set_memory_status(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_BROADCAST_ACTIVATION_V01
    static dsm_item_type* qmi_wmsi_set_broadcast_activation(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_BROADCAST_CONFIG_V01
    static dsm_item_type* qmi_wmsi_set_broadcast_config(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_BROADCAST_CONFIG_V01
    static dsm_item_type* qmi_wmsi_get_broadcast_config(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_V01
    static dsm_item_type* qmi_wmsi_get_domain_pref(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_V01
    static dsm_item_type* qmi_wmsi_set_domain_pref(void *, void *, void *, dsm_item_type **);
#endif
#if !defined(REMOVE_QMI_WMS_SEND_FROM_MEM_STORE_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_V01)
    static dsm_item_type* qmi_wmsi_send_from_mem_store(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_MESSAGE_WAITING_V01
    static dsm_item_type* qmi_wmsi_get_message_waiting(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_PRIMARY_CLIENT_V01
    static dsm_item_type* qmi_wmsi_set_primary_client(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_INDICATION_REGISTER_V01
    static dsm_item_type* qmi_wmsi_indication_register(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_LAYER_INFO_V01
    static dsm_item_type* qmi_wmsi_get_transport_layer_info(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_NW_REG_INFO_V01
    static dsm_item_type* qmi_wmsi_get_transport_nw_reg_info(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_BIND_SUBSCRIPTION_V01
    static dsm_item_type* qmi_wmsi_bind_subscription(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_INDICATION_REGISTER_V01
    static dsm_item_type* qmi_wmsi_get_indication_register(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_SMS_PARAMETERS_V01
    static dsm_item_type* qmi_wmsi_get_sms_parameters(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_SMS_PARAMETERS_V01
    static dsm_item_type* qmi_wmsi_set_sms_parameters(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_CONFIG_V01
    static dsm_item_type* qmi_wmsi_get_domain_pref_config(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_CONFIG_V01
    static dsm_item_type* qmi_wmsi_set_domain_pref_config(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_RETRY_PERIOD_V01
    static dsm_item_type* qmi_wmsi_get_retry_period(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_RETRY_INTERVAL_V01
    static dsm_item_type* qmi_wmsi_get_retry_interval(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_DC_DISCONNECT_TIMER_V01
    static dsm_item_type* qmi_wmsi_get_dc_disconnect_timer(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_MEMORY_STATUS_V01
    static dsm_item_type* qmi_wmsi_get_memory_status(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_PRIMARY_CLIENT_V01
    static dsm_item_type* qmi_wmsi_get_primary_client(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_SUBSCRIPTION_BINDING_V01
    static dsm_item_type* qmi_wmsi_get_subscription_binding(void*, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_GET_SERVICE_READY_STATUS_V01
    static dsm_item_type* qmi_wmsi_get_service_ready_status(void *, void *, void *, dsm_item_type **);
#endif
#ifndef REMOVE_QMI_WMS_SET_MESSAGE_WAITING_V01
    static dsm_item_type* qmi_wmsi_set_message_waiting(void *, void *, void *, dsm_item_type **);
#endif

/*---------------------------------------------------------------------------
  QMI service command response generating functions
---------------------------------------------------------------------------*/
qmi_error_e_type qmi_wmsi_send_resp(wms_raw_send_resp_msg_v01 *, wms_msg_event_info_s_type *,  qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_async_send_ind(wms_async_raw_send_ind_msg_v01 *, wms_msg_event_info_s_type  *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_mem_send_resp(wms_send_from_mem_store_resp_msg_v01 *, wms_msg_event_info_s_type *, qmi_wms_cmd_node_type  * );
qmi_error_e_type qmi_wmsi_async_mem_send_ind(wms_async_send_from_mem_store_ind_msg_v01 *, wms_msg_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_write_resp(wms_raw_write_resp_msg_v01 *, wms_msg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_read_resp(wms_raw_read_resp_msg_v01 *, wms_msg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_list_resp(wms_list_messages_resp_msg_v01 *, wms_cfg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_routes_resp(wms_get_routes_resp_msg_v01 *, wms_cfg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_smsc_addr_resp(wms_get_smsc_address_resp_msg_v01 *, wms_msg_event_info_s_type *, uint8);
qmi_error_e_type qmi_wmsi_async_ack_ind(wms_async_send_ack_ind_msg_v01 *, wms_msg_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_ack_resp(wms_send_ack_resp_msg_v01 *, wms_msg_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_get_store_max_size_resp(wms_get_store_max_size_resp_msg_v01 *, wms_cfg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_set_bc_config_processing(qmi_cmd_buf_type *, wms_bc_mm_event_info_s_type *, qmi_wms_cmd_node_type *, boolean *);
qmi_error_e_type qmi_wmsi_get_bc_config_resp(wms_get_broadcast_config_resp_msg_v01 *, wms_bc_mm_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_get_domain_pref_config_resp(wms_get_domain_pref_config_resp_msg_v01  *, wms_cfg_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_get_domain_pref_resp(wms_get_domain_pref_resp_msg_v01 *, wms_cfg_event_info_s_type *, qmi_wms_cmd_node_type *);
qmi_error_e_type qmi_wmsi_get_sms_parameters_resp(wms_get_sms_parameters_resp_msg_v01 *, wms_msg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_retry_period_resp(wms_get_retry_period_resp_msg_v01 *, wms_dbg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_retry_interval_resp(wms_get_retry_interval_resp_msg_v01 *, wms_dbg_event_info_s_type *);
qmi_error_e_type qmi_wmsi_get_message_waiting_resp(wms_get_message_waiting_resp_msg_v01 *, uint8);

/*---------------------------------------------------------------------------
  "compare" functions to search for pending requests that are queued
---------------------------------------------------------------------------*/
static int qmi_wmsi_compare_cmd_buf(qmi_wms_cmd_node_type *, void *);
static int qmi_wmsi_compare_get_list(qmi_wms_cmd_node_type *, wms_cfg_event_info_s_type *);
static int qmi_wmsi_compare_get_routes(qmi_wms_cmd_node_type *, wms_cfg_event_info_s_type *);
static int qmi_wmsi_compare_get_store_max_size(qmi_wms_cmd_node_type *, wms_cfg_event_info_s_type *);
static int qmi_wmsi_compare_get_domain_pref(qmi_wms_cmd_node_type *, wms_cfg_event_info_s_type *);
static int qmi_wmsi_compare_get_message_waiting(qmi_wms_cmd_node_type *, wms_cfg_event_info_s_type *);
static int qmi_wmsi_compare_get_dbg_retry_period(qmi_wms_cmd_node_type *, wms_dbg_event_info_s_type *);
static int qmi_wmsi_compare_get_dbg_retry_interval(qmi_wms_cmd_node_type *, wms_dbg_event_info_s_type *);
static int qmi_wmsi_compare_tid(qmi_wms_cmd_node_type *, qmi_wms_tid_type *);

/*---------------------------------------------------------------------------
  callback functions to post commands to QMI_WMS to be processed
---------------------------------------------------------------------------*/
boolean qmi_wms_fw_alloc_clid_cb(qmi_framework_common_msg_hdr_type *);
void qmi_wms_fw_dealloc_clid_cb(qmi_framework_common_msg_hdr_type *);
void qmi_wms_fw_init_cb(uint16);
void qmi_wms_fw_cmd_hdlr_cb(qmi_framework_msg_hdr_type *, dsm_item_type **);
void qmi_wms_cmd_status_cb(wms_cmd_id_e_type, const void *, wms_cmd_err_e_type);
void qmi_wms_msg_evt_cb(wms_msg_event_e_type, wms_msg_event_info_s_type *, boolean *);
void qmi_wms_cfg_evt_cb(wms_cfg_event_e_type, wms_cfg_event_info_s_type *);
void qmi_wms_dc_evt_cb(wms_dc_event_e_type, wms_dc_event_info_s_type *);
void qmi_wms_bc_mm_evt_cb(wms_bc_mm_event_e_type, wms_bc_mm_event_info_s_type *);
void qmi_wms_transport_status_cb(wms_msg_transport_status_s_type *);
void qmi_wms_dbg_evt_cb(wms_dbg_event_e_type, wms_dbg_event_info_s_type *);

/*---------------------------------------------------------------------------
  process functions to handle QMI_WMS tasks that have been queued
---------------------------------------------------------------------------*/
/* functions to set, listen, and process task signals */
qmi_wms_task_buffer_s_type * qmi_wms_get_cmd_buf(void);
boolean qmi_wms_free_cmd_buf(void *);
boolean qmi_wms_put_cmd_buf(qmi_wms_task_buffer_s_type *);
boolean qmi_wms_sig_hdlr(qmi_wms_sig_enum_type, void *);
void qmi_wms_process_task_cmd(void);
/* functions used to process framework tasks */
void qmi_wms_process_alloc_clid(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_dealloc_clid(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_init_cback(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_cmd_hdlr(qmi_wms_task_buffer_s_type *);
/* functions used to process WMS subsystem tasks */
void qmi_wms_process_cmd_status(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_msg_evt(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_cfg_evt(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_dc_evt(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_bc_mm_evt(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_transport_status(qmi_wms_task_buffer_s_type *);
void qmi_wms_process_dbg_evt(qmi_wms_task_buffer_s_type *);

/*---------------------------------------------------------------------------
  functions to generate indications
---------------------------------------------------------------------------*/
void qmi_wmsi_generate_mt_message_ind(wms_msg_event_e_type, wms_msg_event_info_s_type *, qmi_wmsi_subscription_e_type, boolean);
void qmi_wmsi_generate_smsc_addr_ind(wms_gw_template_s_type *, uint8);
void qmi_wmsi_generate_mem_full_ind(wms_cfg_event_info_s_type *, qmi_wmsi_subscription_e_type);
void qmi_wmsi_generate_msg_wait_ind(qmi_wmsi_subscription_e_type);
void qmi_wmsi_generate_transport_layer_info_ind(void);
void qmi_wmsi_generate_transport_nw_reg_info_ind(void);
void qmi_wmsi_generate_call_status_ind(wms_dc_event_e_type);
void qmi_wmsi_generate_ready_ind(qmi_wmsi_subscription_e_type, boolean);
void qmi_wmsi_generate_broadcast_config_ind(wms_bc_mm_event_info_s_type *);
void qmi_wmsi_generate_call_control_ind(wms_msg_event_info_s_type *, qmi_wmsi_subscription_e_type);
void qmi_wmsi_generate_transport_layer_mwi_ind(wms_msg_event_info_s_type *, qmi_wmsi_subscription_e_type);
static void qmi_wmsi_message_waiting_ind(void * , uint8);

/*---------------------------------------------------------------------------
  functions to perform QMI_WMS specific 'service' utilities
---------------------------------------------------------------------------*/
void* qmi_wmsi_get_client_sp_by_cmd_buf(qmi_cmd_buf_type*);
void* qmi_wmsi_get_client_sp_by_clid(uint8);
static boolean qmi_wmsi_send_response(qmi_cmd_buf_type *, dsm_item_type *);
static boolean qmi_wmsi_send_ind(void *, uint16, dsm_item_type *);

/*---------------------------------------------------------------------------
  other misc utility functions
---------------------------------------------------------------------------*/
void qmi_wmsi_process_mt_msg(wms_msg_event_e_type, wms_msg_event_info_s_type *, qmi_wmsi_subscription_e_type);
uint32 qmi_wmsi_retrieve_msgid(wms_msg_event_info_s_type *);
void qmi_wmsi_read_params_from_template(wms_msg_event_info_s_type *, uint8);
boolean qmi_wmsi_bin_to_addr(byte *, byte, wms_address_s_type *);
boolean qmi_wmsi_addr_to_bin(wms_address_s_type *, byte *);
boolean qmi_wmsi_addr_to_str(wms_address_s_type *, byte *, uint8 *, uint8 *);
void qmi_wmsi_init_mwi_cache(void);
boolean qmi_wmsi_compare_mwi_cache_to_sim(byte *, uint8);
void qmi_wmsi_update_mwi_cache_from_sim(byte *, uint8);
boolean qmi_wmsi_check_transport_layer_cache(wms_msg_transport_reg_info_s_type *);
void qmi_wmsi_update_transport_layer_cache(wms_msg_transport_reg_info_s_type *);
boolean qmi_wmsi_update_channel_pref_and_service_option(wms_client_ch_pref_e_type, wms_dc_so_e_type);
boolean qmi_wmsi_update_follow_on_dc_option(boolean follow_on_dc);
void* qmi_wmsi_convert_tpt_mwi_to_opt_tlv(wms_msg_event_info_s_type *, int *);
boolean qmi_wmsi_check_and_update_sim_status(qmi_wmsi_subscription_e_type, wms_cfg_sys_e_type, boolean);

/*---------------------------------------------------------------------------
QMI service command handler dispatch table definition
---------------------------------------------------------------------------*/
#define WMS_HDLR(a, b)  QMI_SVC_HDLR(a, (qmi_svc_hdlr_ftype) b)

static qmi_svc_cmd_hdlr_type  qmi_wmsi_cmd_callbacks[WMSI_CMD_MAX] =
{
#ifndef REMOVE_QMI_WMS_RESET_V01
  WMS_HDLR( WMSI_CMD_VAL_RESET,
            qmi_wmsi_reset),
#endif
#ifndef REMOVE_QMI_WMS_SET_EVENT_REPORT_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_EVENT_REPORT,
            qmi_wmsi_set_event_report),
#endif
#ifndef REMOVE_QMI_WMS_RAW_SEND_V01
  WMS_HDLR( WMSI_CMD_VAL_RAW_SEND,
            qmi_wmsi_raw_send),
#endif
#ifndef REMOVE_QMI_WMS_RAW_WRITE_V01
  WMS_HDLR( WMSI_CMD_VAL_RAW_WRITE,
            qmi_wmsi_raw_write),
#endif
#ifndef REMOVE_QMI_WMS_RAW_READ_V01
  WMS_HDLR( WMSI_CMD_VAL_RAW_READ,
            qmi_wmsi_raw_read),
#endif
#ifndef REMOVE_QMI_WMS_MODIFY_TAG_V01
  WMS_HDLR( WMSI_CMD_VAL_MODIFY_TAG,
            qmi_wmsi_modify_tag),
#endif
#ifndef REMOVE_QMI_WMS_DELETE_V01
  WMS_HDLR( WMSI_CMD_VAL_DELETE,
            qmi_wmsi_delete),
#endif
#ifndef REMOVE_QMI_WMS_GET_MESSAGE_PROTOCOL_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_MESSAGE_PROTOCOL,
            qmi_wmsi_get_message_protocol),
#endif
#ifndef REMOVE_QMI_WMS_LIST_MESSAGES_V01
  WMS_HDLR( WMSI_CMD_VAL_LIST_MESSAGES,
            qmi_wmsi_list_messages),
#endif
#ifndef REMOVE_QMI_WMS_SET_ROUTES_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_ROUTES,
            qmi_wmsi_set_routes),
#endif
#ifndef REMOVE_QMI_WMS_GET_ROUTES_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_ROUTES,
            qmi_wmsi_get_routes),
#endif
#ifndef REMOVE_QMI_WMS_GET_SMSC_ADDRESS_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_SMSC_ADDR,
            qmi_wmsi_get_smsc_addr),
#endif
#ifndef REMOVE_QMI_WMS_SET_SMSC_ADDRESS_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_SMSC_ADDR,
            qmi_wmsi_set_smsc_addr),
#endif
#ifndef REMOVE_QMI_WMS_GET_STORE_MAX_SIZE_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_STORE_MAX_SIZE,
            qmi_wmsi_get_store_max_size),
#endif
#ifndef REMOVE_QMI_WMS_SEND_ACK_V01
  WMS_HDLR( WMSI_CMD_VAL_SEND_ACK,
            qmi_wmsi_send_ack),
#endif
#ifndef REMOVE_QMI_WMS_SET_RETRY_PERIOD_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_RETRY_PERIOD,
            qmi_wmsi_set_retry_period),
#endif
#ifndef REMOVE_QMI_WMS_SET_RETRY_INTERVAL_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_RETRY_INTERVAL,
            qmi_wmsi_set_retry_interval),
#endif
#ifndef REMOVE_QMI_WMS_SET_DC_DISCONNECT_TIMER_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_DC_DISCONNECT_TIMER,
            qmi_wmsi_set_dc_disconnect_timer),
#endif
#ifndef REMOVE_QMI_WMS_SET_MEMORY_STATUS_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_MEMORY_STATUS,
            qmi_wmsi_set_memory_status),
#endif
#ifndef REMOVE_QMI_WMS_SET_BROADCAST_ACTIVATION_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_BROADCAST_ACTIVATION,
            qmi_wmsi_set_broadcast_activation),
#endif
#ifndef REMOVE_QMI_WMS_SET_BROADCAST_CONFIG_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_BROADCAST_CONFIG,
            qmi_wmsi_set_broadcast_config),
#endif
#ifndef REMOVE_QMI_WMS_GET_BROADCAST_CONFIG_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_BROADCAST_CONFIG,
            qmi_wmsi_get_broadcast_config),
#endif
#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_DOMAIN_PREF,
            qmi_wmsi_get_domain_pref),
#endif
#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_DOMAIN_PREF,
            qmi_wmsi_set_domain_pref),
#endif
#ifndef REMOVE_QMI_WMS_SEND_FROM_MEM_STORE_V01
  WMS_HDLR( WMSI_CMD_VAL_SEND_FROM_MEM_STORE,
            qmi_wmsi_send_from_mem_store),
#endif
#ifndef REMOVE_QMI_WMS_GET_MESSAGE_WAITING_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_MESSAGE_WAITING,
            qmi_wmsi_get_message_waiting),
#endif
#ifndef REMOVE_QMI_WMS_SET_PRIMARY_CLIENT_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_PRIMARY_CLIENT,
            qmi_wmsi_set_primary_client),
#endif
#ifndef REMOVE_QMI_WMS_INDICATION_REGISTER_V01
  WMS_HDLR( WMSI_CMD_VAL_INDICATION_REGISTER,
            qmi_wmsi_indication_register),
#endif
#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_LAYER_INFO_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_TRANSPORT_LAYER_INFO,
            qmi_wmsi_get_transport_layer_info),
#endif
#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_NW_REG_INFO_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_TRANSPORT_NW_REG_INFO,
            qmi_wmsi_get_transport_nw_reg_info),
#endif
#ifndef REMOVE_QMI_WMS_BIND_SUBSCRIPTION_V01
  WMS_HDLR( WMSI_CMD_VAL_BIND_SUBSCRIPTION,
            qmi_wmsi_bind_subscription),
#endif
#ifndef REMOVE_QMI_WMS_GET_INDICATION_REGISTER_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_INDICATION_REGISTER,
            qmi_wmsi_get_indication_register),
#endif
#ifndef REMOVE_QMI_WMS_GET_SMS_PARAMETERS_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_SMS_PARAMETERS,
            qmi_wmsi_get_sms_parameters),
#endif
#ifndef REMOVE_QMI_WMS_SET_SMS_PARAMETERS_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_SMS_PARAMETERS,
            qmi_wmsi_set_sms_parameters),
#endif
#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_CONFIG_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_DOMAIN_PREF_CONFIG,
            qmi_wmsi_get_domain_pref_config),
#endif
#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_CONFIG_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_DOMAIN_PREF_CONFIG,
            qmi_wmsi_set_domain_pref_config),
#endif
#ifndef REMOVE_QMI_WMS_GET_RETRY_PERIOD_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_RETRY_PERIOD,
            qmi_wmsi_get_retry_period),
#endif
#ifndef REMOVE_QMI_WMS_GET_RETRY_INTERVAL_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_RETRY_INTERVAL,
            qmi_wmsi_get_retry_interval),
#endif
#ifndef REMOVE_QMI_WMS_GET_DC_DISCONNECT_TIMER_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_DC_DISCONNECT_TIMER,
            qmi_wmsi_get_dc_disconnect_timer),
#endif
#ifndef REMOVE_QMI_WMS_GET_MEMORY_STATUS_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_MEMORY_STATUS,
            qmi_wmsi_get_memory_status),
#endif
#ifndef REMOVE_QMI_WMS_GET_PRIMARY_CLIENT_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_PRIMARY_CLIENT,
            qmi_wmsi_get_primary_client),
#endif
#ifndef REMOVE_QMI_WMS_GET_SUBSCRIPTION_BINDING_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_SUBSCRIPTION_BINDING,
            qmi_wmsi_get_subscription_binding),
#endif
#ifndef REMOVE_QMI_WMS_ASYNC_RAW_SEND_V01
  WMS_HDLR( WMSI_CMD_VAL_ASYNC_RAW_SEND,
            qmi_wmsi_raw_send),
#endif
#ifndef REMOVE_QMI_WMS_ASYNC_SEND_ACK_V01
  WMS_HDLR( WMSI_CMD_VAL_ASYNC_SEND_ACK,
            qmi_wmsi_send_ack),
#endif
#ifndef REMOVE_QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_V01
  WMS_HDLR( WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE,
            qmi_wmsi_send_from_mem_store),
#endif
#ifndef REMOVE_QMI_WMS_GET_SERVICE_READY_STATUS_V01
  WMS_HDLR( WMSI_CMD_VAL_GET_SERVICE_READY_STATUS,
            qmi_wmsi_get_service_ready_status),
#endif
#ifndef REMOVE_QMI_WMS_SET_MESSAGE_WAITING_V01
  WMS_HDLR( WMSI_CMD_VAL_SET_MESSAGE_WAITING,
            qmi_wmsi_set_message_waiting),
#endif
};

/*---------------------------------------------------------------------------
  Each QMI link (usb, BT etc) has its own WMS state
---------------------------------------------------------------------------*/
static qmi_wmsi_state_type  qmi_wms_state;

/*---------------------------------------------------------------------------
  QMI_WMS instance configuration
---------------------------------------------------------------------------*/
static ds_qmi_fw_svc_cfg_type qmi_wmsi_cfg;



/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI_WMS memory allocation/deallocation functions
---------------------------------------------------------------------------*/
static void * qmi_wmsi_mem_alloc(size_t);
static void qmi_wmsi_mem_free(void *);

/*---------------------------------------------------------------------------
  QMI_WMS command caching functions
---------------------------------------------------------------------------*/
static qmi_wms_cmd_node_type * qmi_wmsi_get_cmd_node(void);
static void qmi_wmsi_free_cmd_node(qmi_wms_cmd_node_type *);
static void qmi_wmsi_put_pending_cmd_buf(qmi_wms_cmd_node_type *);
static qmi_wms_cmd_node_type * qmi_wmsi_get_pending_cmd_buf(q_compare_func_type, void *, boolean);
static int qmi_wmsi_cmd_buf_belongs_to_client(qmi_wms_cmd_node_type *, void *);
static qmi_wmsi_state_type * qmi_wmsi_get_wms_sp_by_cmd_buf(void *);
static void qmi_wmsi_reset_client(void *);

static void qmi_wmsi_event_report_ind(void *, qmi_wmsi_report_event_info_s_type *);

/* WMS <==> QMI enum mapping functions */
qmi_error_e_type qmi_wmsi_from_wms_status
(
  wms_status_e_type wms_err
);

wms_message_tag_e_type qmi_wmsi_check_and_update_wms_tag
(
  wms_message_tag_e_type wms_tag
);

qmi_wmsi_tag_e_type qmi_wmsi_from_wms_tag
(
  wms_message_tag_e_type wms_tag
);

wms_message_mode_e_type qmi_wmsi_to_wms_message_mode
(
  qmi_wmsi_message_mode_e_type qmi_msg_mode
);

wms_format_e_type qmi_wmsi_to_wms_format
(
  qmi_wmsi_message_format_e_type qmi_format
);

qmi_wmsi_message_format_e_type qmi_wmsi_from_wms_format
(
  wms_format_e_type wms_format
);

wms_memory_store_e_type qmi_wmsi_to_wms_storage
(
  qmi_wmsi_memory_storage_e_type qmi_storage_type,
  qmi_wmsi_message_mode_e_type   qmi_message_mode
);

qmi_wmsi_memory_storage_e_type qmi_wmsi_from_wms_storage
(
  wms_memory_store_e_type wms_storage_type
);

qmi_wmsi_message_mode_e_type qmi_wmsi_message_mode_from_wms_storage
(
  wms_memory_store_e_type wms_storage_type
);

wms_message_tag_e_type qmi_wmsi_to_wms_tag
(
  qmi_wmsi_tag_e_type qmi_tag
);

wms_message_class_e_type qmi_wmsi_to_wms_message_class
(
  qmi_wmsi_message_class_e_type qmi_class
);

qmi_wmsi_message_class_e_type qmi_wmsi_from_wms_message_class
(
  wms_message_class_e_type wms_class
);

wms_route_e_type qmi_wmsi_to_wms_route
(
  qmi_wmsi_route_e_type qmi_route
);

qmi_wmsi_route_e_type qmi_wmsi_from_wms_route
(
  wms_route_e_type wms_route
);

qmi_error_e_type qmi_wmsi_from_wms_error
(
  wms_cmd_err_e_type wms_err
);

qmi_wmsi_mwi_state_e_type qmi_wmsi_from_wms_state
(
  wms_mwi_state_e_type wms_state
);

qmi_error_e_type qmi_wmsi_from_wms_report_error
(
  wms_report_status_e_type wms_report_err
);

uint16 qmi_wmsi_from_wms_cause_error
(
  wms_cdma_tl_status_e_type wms_causecode
);

qmi_wmsi_error_class_e_type qmi_wmsi_from_wms_error_class
(
  wms_error_class_e_type wms_errorclass
);

qmi_wmsi_mo_ctrl_status_e_type qmi_wmsi_from_wms_mo_ctrl_status
(
  wms_mo_ctrl_status_e_type mo_ctrl_type
);

wms_bc_pref_e_type qmi_wmsi_to_wms_bc_pref
(
  qmi_wmsi_bc_pref_e_type bc_pref,
  boolean activate_all
);

qmi_wmsi_bc_pref_e_type qmi_wms_to_wmsi_bc_pref
(
  wms_bc_pref_e_type bc_pref
);

qmi_wmsi_mwi_e_type qmi_wms_to_wmsi_mwi
(
  wms_gw_msg_waiting_kind_e_type wms_mwi
);

qmi_wmsi_mwi_e_type qmi_wms_transport_mwi_to_wmsi_mwi
(
  wms_msg_transport_mwi_kind_e_type wms_mwi
);

qmi_wmsi_transport_mwi_e_type qmi_wms_transport_mwi_to_wmsi_transport_mwi
(
  wms_msg_transport_mwi_kind_e_type wms_mwi
);

qmi_wmsi_mwi_priority_e_type qmi_wms_transport_mwi_priority_to_wmsi_mwi_priority
(
  wms_msg_transport_mwi_priority_e_type priority
);

qmi_wmsi_subscription_e_type qmi_wms_get_subscription_from_asid
(
  sys_modem_as_id_e_type as_id
);

sys_modem_as_id_e_type qmi_wms_get_asid_from_subscription
(
  qmi_wmsi_subscription_e_type qmi_wms_subscription
);

qmi_wmsi_transport_type_e_type qmi_wms_to_wmsi_transport_type
(
  wms_msg_transport_type_e_type wms_transport_type
);

qmi_wmsi_message_mode_e_type qmi_wms_to_wmsi_transport_cap
(
  uint32 wms_transport_cap
);

qmi_wmsi_transport_nw_reg_status_e_type qmi_wms_to_wmsi_transport_nw_reg
(
  wms_msg_transport_nw_reg_status_e_type wms_nw_reg_status
);

qmi_wmsi_call_status_e_type qmi_wms_to_wmsi_call_status
(
  wms_dc_event_e_type wms_call_status
);

byte * qmi_wmsi_util_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
);

qmi_wmsi_dial_val_e_type qmi_wmsi_validate_dial_char
(
  const byte * c_ptr                 /*  Input character   */
);

boolean qmi_wmsi_str_to_addr
(
  wms_address_s_type           *addr,   /* Pointer to address structure to
                                           be filled                             */
  byte                         * da,    /* Pointer to destination address        */
  const byte                   * toda,  /* Pointer to type of address            */
  qmi_wmsi_fmt_mode_e_type     sms_mode, /* SMS mode Enum                        */
  qmi_wmsi_message_mode_e_type message_mode /* Message mode */
);

qmi_wmsi_atoi_enum_type qmi_wmsi_util_atoi
(
  uint32       *val_arg_ptr,    /*  value returned  */
  const byte   *s,             /*  points to string to eval  */
  unsigned int r              /*  radix */
);

void qmi_wmsi_deep_copy
(
  void** dest,
  void* src,
  uint32 size
);

boolean qmi_wmsi_mobile_is_ims_registered(sys_modem_as_id_e_type);

/*===========================================================================

                             IDL LIBRARY ENCODE / DECODE FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WMSI_CHECK_FOR_VALID_REQ()

  DESCRIPTION
    Checks if the request is allowed to have no TLV at all.

  PARAMETERS
    message_id: Message ID of the request.

  RETURN VALUE
    QMI_IDL_LIB_NO_ERR      : if allowed.
    QMI_IDL_LIB_MISSING_TLV : if not allowed.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32_t qmi_wmsi_check_for_valid_req(uint16_t message_id)
{
  int32_t retval = QMI_IDL_LIB_NO_ERR;
  
  switch (message_id)
  {
    case QMI_WMS_RESET_REQ_V01:
    case QMI_WMS_SET_EVENT_REPORT_REQ_V01:
    case QMI_WMS_GET_SUPPORTED_MSGS_REQ_V01:
    case QMI_WMS_GET_MESSAGE_PROTOCOL_REQ_V01:
    case QMI_WMS_GET_ROUTES_REQ_V01:
    case QMI_WMS_GET_SMSC_ADDRESS_REQ_V01:
    case QMI_WMS_GET_MESSAGE_WAITING_REQ_V01:
    case QMI_WMS_INDICATION_REGISTER_REQ_V01:
    case QMI_WMS_GET_TRANSPORT_LAYER_INFO_REQ_V01:
    case QMI_WMS_GET_TRANSPORT_NW_REG_INFO_REQ_V01:
    case QMI_WMS_GET_INDICATION_REGISTER_REQ_V01:
    case QMI_WMS_GET_DOMAIN_PREF_CONFIG_REQ_V01:
    case QMI_WMS_SET_DOMAIN_PREF_CONFIG_REQ_V01:
    case QMI_WMS_GET_RETRY_PERIOD_REQ_V01:
    case QMI_WMS_GET_RETRY_INTERVAL_REQ_V01:
    case QMI_WMS_GET_DC_DISCONNECT_TIMER_REQ_V01:
    case QMI_WMS_GET_MEMORY_STATUS_REQ_V01:
    case QMI_WMS_GET_PRIMARY_CLIENT_REQ_V01:
    case QMI_WMS_GET_SUBSCRIPTION_BINDING_REQ_V01: 
    case QMI_WMS_GET_SERVICE_READY_STATUS_REQ_V01:
      // No Error to be returned. These requests don't require any Mandatory TLV.
      break;

     default:
      // Return an error for requests that require mandatory TLV but no TLV is present.
      retval = QMI_IDL_LIB_MISSING_TLV;
      break;

  }

  return retval;
} /* qmi_wmsi_check_for_valid_req() */

/*===========================================================================
  FUNCTION QMI_WMS_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI WMS message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    qmi_idl_type_of_message_type type
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to C structure to contain decoded message
    p_req_size  Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, 
    otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_wms_idl_message_decode
 ( 
   qmi_idl_service_object_type svc, 
   qmi_idl_type_of_message_type type,
   uint16_t message_id, 
   dsm_item_type ** msg_ptr, 
   void *p_req, 
   uint32_t p_req_size
 )
 {
   qmi_error_e_type   errval = QMI_ERR_NONE;
   int32_t            retval = QMI_IDL_LIB_NO_ERR;
   uint32             size;
   uint8              *buf = NULL;
 
   // Check if there is at least one TLV in the request once the message type (2 bytes) and message length (2 bytes) 
   //  bytes have been removed. 
   if (msg_ptr != NULL && *msg_ptr != NULL) 
   {
     size = dsm_length_packet( *msg_ptr );
     MSG_HIGH_1("qmi_wms_idl_message_decode dsm_pullup size %d", size);

     if (size > 0)
     {
       buf = (uint8 *)modem_mem_alloc((size_t)size, MODEM_MEM_CLIENT_QMI);
     
       if (NULL != buf)
       {
         if (size == dsm_pullup( msg_ptr, buf, size ))
         {
           retval= qmi_idl_message_decode( svc,
                                           type,                // IDL message type
                                           message_id,          // QMI WMS cmd type
                                           (void*)buf,          // pointer to incoming byte stream
                                           (uint32_t)size,      // pointer to no of bytes to be read
                                           p_req,               // pointer to c structure
                                           p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                          );
         }
         else
         {
           retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
         }
     
         modem_mem_free( (uint8*)buf, MODEM_MEM_CLIENT_QMI );
       }
       else
       {
         retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
         MSG_ERROR_0("qmi_wms_idl_message_decode no memory for buffer");
       }
     }
     else
     {
       retval= qmi_idl_message_decode( svc,
                                       type,                   // IDL message type
                                       message_id,             // QMI WMS cmd type
                                       (*msg_ptr)->data_ptr,   // pointer to incoming byte stream
                                       (*msg_ptr)->used,       // pointer to no of bytes to be read
                                       p_req,                  // pointer to c structure
                                       p_req_size              // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                       );
     }
   }
   else 
   {
     // No TLV is there in the Input request.
     if (NULL != msg_ptr && NULL == *msg_ptr)
     {
       // Call check function to determine the validity of this request based on message ID.
       retval = qmi_wmsi_check_for_valid_req(message_id);
     }
   }
   
     switch( retval )
     {  
       case QMI_IDL_LIB_NO_ERR:
         errval = QMI_ERR_NONE;
         break;
 
       case QMI_IDL_LIB_ARRAY_TOO_BIG:
         errval = QMI_ERR_ARG_TOO_LONG;
         break;
 
       case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
         errval = QMI_ERR_NOT_SUPPORTED;
         break;
 
       case QMI_IDL_LIB_TLV_DUPLICATED : 
       case QMI_IDL_LIB_UNKNOWN_MANDATORY_TLV:
         errval = QMI_ERR_MALFORMED_MSG;
         break;

       case QMI_IDL_LIB_LENGTH_INCONSISTENCY: 
         errval = QMI_ERR_INVALID_ARG;
         break;

       case QMI_IDL_LIB_MISSING_TLV:
         errval = QMI_ERR_MISSING_ARG;
         break;	   	
   
       case QMI_IDL_LIB_EXTENDED_ERR:
       case QMI_IDL_LIB_BUFFER_TOO_SMALL:
       case QMI_IDL_LIB_PARAMETER_ERROR:
       case QMI_IDL_LIB_UNRECOGNIZED_SERVICE_VERSION:
       case QMI_IDL_LIB_INCOMPATIBLE_SERVICE_VERSION:
       default:
         errval = QMI_ERR_INTERNAL;
         break;
     }
 
     if ( QMI_IDL_LIB_NO_ERR != retval )
     {
       MSG_ERROR_2( "qmi_wms_idl_message_decode failed %d %d", retval, errval );
     }
 
   return  (qmi_error_type_v01)errval;
 } /* qmi_wms_idl_message_decode() */ 

/*======================================================================================================================
  FUNCTION QMI_WMS_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI WMS message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
========================================================================================================================*/
boolean qmi_wms_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void                         *p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type                **p_response
)
{
  qmi_error_type_v01  eval        = QMI_IDL_LIB_NO_ERR;
  int32_t             eval2       = QMI_IDL_LIB_NO_ERR;
  uint32_t            rsp_buf_len = 0;
  uint8 *             rsp_buf     = NULL;
  uint32_t            size_buf    = 0;
  boolean             retval      = FALSE;

  *p_response = NULL;
 
  /*--------------------------------------------------------------------------------
    Accessor function for getting the maximum message length of a response
    message. Message length includes c struct size, size of type and size of length.
  ----------------------------------------------------------------------------------*/
  eval = qmi_idl_get_max_message_len( svc, type, message_id, &rsp_buf_len );
  
  if (QMI_IDL_LIB_NO_ERR == eval)
  {
    rsp_buf = (uint8 *)modem_mem_alloc(rsp_buf_len, MODEM_MEM_CLIENT_QMI);
	
    if (NULL != rsp_buf)
    {
      memset(rsp_buf, 0x00, rsp_buf_len);      
      eval2 = qmi_idl_message_encode( svc,
                                      type,         // IDL message type
                                      message_id,   // QMI WMS cmd type
                                      p_src,        // pointer to c structure
                                      p_src_size,   // size of c structure which is to be encoded
                                      rsp_buf,      // buffer to contain encoded message data
                                      rsp_buf_len,  // size of buffer, should be maximum size of the encoded c structure
                                      &size_buf     // pointer to store length of the encoded message
                                    );

      if (QMI_IDL_LIB_NO_ERR == eval2)
      {
        if ( size_buf != dsm_pushdown_packed( p_response, &rsp_buf[0], size_buf, DSM_DS_SMALL_ITEM_POOL) )
        {
          MSG_ERROR_1("Pushdown Failed for qmi_wms_idl_message_encode %d", size_buf);
          dsm_free_packet(p_response);
        }
        else
        {
          retval = TRUE;
        }
      }
      else
      {
        MSG_ERROR_2( "qmi_idl_message_encode failed eval2 = %d,  src_size = %d", eval2, p_src_size );
      }
    }
    else
    {
      MSG_ERROR_1( "Not enough memory to allocate for QMI_WMS_IDL_MESSAGE_ENCODE cmd_type = %d", message_id );
    }
  }
  else
  {
    MSG_ERROR_1( "qmi_idl_get_max_message_len failed %d", eval );
  }
  
  if (NULL != rsp_buf) { modem_mem_free((uint8*) rsp_buf, MODEM_MEM_CLIENT_QMI); }

  return retval;
} /* qmi_wms_idl_message_encode() */

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_WMS_INIT

  DESCRIPTION
    Register the WMS service with QMUX

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_init(
 void
)
{
  qmi_framework_err_e_type      errval = QMI_FRAMEWORK_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&qmi_wms_state, 0, sizeof(qmi_wmsi_state_type));
  memset(&qmi_wmsi_cfg, 0, sizeof(qmi_wmsi_cfg));

  /* Init the task pending queue - this MUST be done before registering with
     QMUX because the callbacks below require this queue to process task
     requests */
  memset(&qmi_wmsi_global, 0, sizeof(qmi_wmsi_global));

  memset (&qmi_wmsi_global.task_pend_q, 0, sizeof(q_type));
  (void) q_init (&qmi_wmsi_global.task_pend_q);

  /*-----------------------------------------------------------------------
    Register service with QMUX.
  -----------------------------------------------------------------------*/
  qmi_wmsi_cfg.fw_cfg.base_version.major     = WMSI_BASE_VER_MAJOR;
  qmi_wmsi_cfg.fw_cfg.base_version.minor     = WMSI_BASE_VER_MINOR;

  qmi_wmsi_cfg.fw_cfg.addendum_version.major = WMSI_ADDENDUM_VER_MAJOR;
  qmi_wmsi_cfg.fw_cfg.addendum_version.minor = WMSI_ADDENDUM_VER_MINOR;

  qmi_wmsi_cfg.fw_cfg.cbs.alloc_clid         = qmi_wms_fw_alloc_clid_cb;
  qmi_wmsi_cfg.fw_cfg.cbs.dealloc_clid       = qmi_wms_fw_dealloc_clid_cb;
  qmi_wmsi_cfg.fw_cfg.cbs.init_cback         = qmi_wms_fw_init_cb;
  qmi_wmsi_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_wms_fw_cmd_hdlr_cb;

  qmi_wmsi_cfg.cmd_hdlr_array                = qmi_wmsi_cmd_callbacks;
  qmi_wmsi_cfg.cmd_num_entries               = WMSI_CMD_MAX;

  errval = qmi_framework_reg_service(QMUX_SERVICE_WMS, &qmi_wmsi_cfg.fw_cfg);

  qmi_wmsi_global.svc_obj = wms_get_service_object_v01();
  (void) qmi_si_register_object (qmi_wmsi_global.svc_obj,
                                 0, /* Service Instance */
                                 wms_get_service_impl_v01());

  if (QMI_FRAMEWORK_ERR_NONE != errval)
  {
    MSG_ERROR_1("qmi_wms_init() qmi_framework_reg_service failed %d", errval);
  }

  return ;
} /* qmi_wms_init() */


/*=========================================================================

                      INTERNAL TASK FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_WMS_GET_CMD_BUF()

  DESCRIPTION
    Allocates memory for a new QMI_WMS task command buffer.

  PARAMETERS
    None

  RETURN VALUE
    Pointer to allocated memory (NULL if failed)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_wms_task_buffer_s_type * qmi_wms_get_cmd_buf(
  void
)
{
  qmi_wms_task_buffer_s_type * ptr = NULL;

  ptr = (qmi_wms_task_buffer_s_type *) qmi_wmsi_mem_alloc(sizeof(qmi_wms_task_buffer_s_type));

  return ptr;
} /* qmi_wms_get_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_WMS_FREE_CMD_BUF()

  DESCRIPTION
    Frees memory from a QMI_WMS task command buffer.

  PARAMETERS
    Pointer to allocated memory (NULL if failed)

  RETURN VALUE
    TRUE if memory freed
    FALSE if memory not freed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wms_free_cmd_buf(
  void * ptr
)
{
  boolean retval = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ptr)
  {
    MSG_ERROR_0("qmi_wms_free_cmd_buf(): passed NULL ptr");
  }
  else
  {
    qmi_wmsi_mem_free(ptr);
    retval = TRUE;
  }

  return retval;
}/* qmi_wms_free_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_WMS_PUT_CMD_BUF()

  DESCRIPTION
    Takes given task info and queues it to be processed.  Task is stored in the
    global QMI_WMS task queue and then controlling task is signaled to be
    processed.

  PARAMETERS
    task_ptr : ptr passed containing QMI_WMS task info

  RETURN VALUE
    TRUE if task queued
    FALSE if task not queued

  DEPENDENCIES
    task_pend_q must be initialized

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wms_put_cmd_buf(
  qmi_wms_task_buffer_s_type * task_ptr
)
{
  boolean ret_val = TRUE;

  if (QMI_WMS_TASK_STOP_SIG_IS_RECEIVED)
  {
    //QMI_WMS task has shut down due to task_stop_sig rcvd. No commands allowed now
    ret_val = FALSE;
  }
  else if (NULL != task_ptr)
  {
    /* Add the message to the QMI WMS queue */
    (void) q_link(&qmi_wmsi_global.task_pend_q, &task_ptr->link);
    q_put(&qmi_wmsi_global.task_pend_q, &task_ptr->link);

    /* Enable the qmi_wms task sig to process in qmi_wms context*/
    qmi_wms_set_signal(QMI_WMS_SERVICE_WMS_SIGNAL);
  }

  return ret_val;
} /* qmi_wms_put_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_WMS_SIG_HDLR()

  DESCRIPTION
    Task signal handler to process QMI_WMS tasks.

  PARAMETERS
    sig           : signal sent from calling task
    user_data_ptr : user_data ptr passed containing QMI_WMS task info

  RETURN VALUE
    TRUE if signal should be cleared from the set signal mask
    FALSE if further signal processing is needed and hence signal should not be
      cleared.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wms_sig_hdlr(
  qmi_wms_sig_enum_type     sig,
  void *                user_data_ptr
)
{
  if (qmi_wmsi_global.task_pend_q.cnt)
  {
    qmi_wms_process_task_cmd();
  }

  /* This signal is set asyncronously however the framework handles
     concurrency issues so no need to worry about it here */
  if (0 == qmi_wmsi_global.task_pend_q.cnt)
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_wms_sig_hdlr() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_TASK_CMD()

  DESCRIPTION
    Processes task command buffers stored in the global QMI_WMS task queue.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_task_cmd(
  void
)
{
  qmi_wms_task_buffer_s_type * task_buffer_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  task_buffer_ptr = (qmi_wms_task_buffer_s_type*) q_get(&qmi_wmsi_global.task_pend_q);

  if (NULL == task_buffer_ptr)
  {
    MSG_ERROR_0("No pending tasks in task_pend_q");
    return;
  }

  MSG_HIGH_1("Processing QMI_WMS task %d", task_buffer_ptr->task);

  switch (task_buffer_ptr->task)
  {
    case WMS_TASK_CMD_ALLOC_CLID:
      qmi_wms_process_alloc_clid(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_DEALLOC_CLID:
      qmi_wms_process_dealloc_clid(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_INIT:
      qmi_wms_process_init_cback(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_CMD_HDLR:
      qmi_wms_process_cmd_hdlr(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_CMD_STATUS:
      qmi_wms_process_cmd_status(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_MSG_EVENT:
      qmi_wms_process_msg_evt(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_CFG_EVENT:
      qmi_wms_process_cfg_evt(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_DC_EVENT:
      qmi_wms_process_dc_evt(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_BC_MM_EVENT:
      qmi_wms_process_bc_mm_evt(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_TRANSPORT_STATUS:
      qmi_wms_process_transport_status(task_buffer_ptr);
      break;

    case WMS_TASK_CMD_DBG_EVENT:
      qmi_wms_process_dbg_evt(task_buffer_ptr);
      break;

    default:
      MSG_ERROR_1("task type (%d) invalid", task_buffer_ptr->task);
    return;
  }

  qmi_wms_free_cmd_buf(task_buffer_ptr);

} /* qmi_wms_process_task_cmd() */

/*===========================================================================
  FUNCTION QMI_WMS_FW_ALLOC_CLID_CB()

  DESCRIPTION
    Callback to allocate the client from Framework

  PARAMETERS
    hdr : QMI framework header to allocate a client ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean qmi_wms_fw_alloc_clid_cb(
  qmi_framework_common_msg_hdr_type *hdr
)
{
  qmi_wms_task_buffer_s_type *ptr     = NULL;
  boolean                     ret_val = FALSE;

  if (hdr)
  {
    ptr = qmi_wms_get_cmd_buf();

    if (ptr)
    {
      ptr->task = WMS_TASK_CMD_ALLOC_CLID;
      (void)memscpy(&ptr->data.fw_msg_hdr,
                    sizeof(qmi_framework_common_msg_hdr_type),
                    hdr,
                    sizeof(qmi_framework_common_msg_hdr_type));

      ret_val = qmi_wms_put_cmd_buf(ptr);
    }
  }
  else
  {
    MSG_ERROR_0("qmi_wms_fw_alloc_clid_cb() hdr is null");
  }

  return ret_val;
} /* qmi_wms_fw_alloc_clid_cb() */

/*===========================================================================
  FUNCTION qmi_wmsi_alloc_cl_sp()

  DESCRIPTION
    Allocate the client data buffer

  PARAMETERS
    clid: uint8

  RETURN VALUE
    ptr to malloc'ed client pointer in case of success. NULL otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
static qmi_wmsi_client_state_type* qmi_wmsi_alloc_cl_sp(
   uint8 clid
)
{
  qmi_wmsi_client_state_type *cl_sp = NULL;

  if (QMI_SVC_CLID_UNUSED != clid && WMSI_MAX_CLIDS >= clid)
  {
    cl_sp = (qmi_wmsi_client_state_type *) qmi_wmsi_mem_alloc(sizeof(qmi_wmsi_client_state_type));

    if (cl_sp)
    {
      qmi_wms_state.client[clid - 1] = cl_sp;
    }
  }
  else
  {
    MSG_ERROR_0("qmi_wmsi_alloc_cl_sp() invalid clid");
  }

  return cl_sp;
} /* qmi_wmsi_alloc_cl_sp() */

/*===========================================================================
  FUNCTION qmi_wms_process_alloc_clid()

  DESCRIPTION
    Process allocate clid from Framework

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_alloc_clid(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  qmi_framework_common_msg_hdr_type *hdr   = NULL;
  qmi_wmsi_client_state_type        *cl_sp = NULL;
  qmi_result_e_type                 result = QMI_RESULT_FAILURE;
  wms_client_err_e_type         wms_result = WMS_CLIENT_ERR_NONE;
  qmi_error_e_type                  errval = QMI_ERR_NONE; 

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  hdr = &user_data_ptr->data.fw_msg_hdr;

  if ( (QMI_SVC_CLID_UNUSED != hdr->client_id)  && (WMSI_MAX_CLIDS >= hdr->client_id) )
  {
    cl_sp = qmi_wmsi_alloc_cl_sp(hdr->client_id);

    if (cl_sp)
    {
      ds_qmi_fw_common_cl_init(&qmi_wms_state, &cl_sp->common);

      /*
       * initialize client specific data here
       */
      cl_sp->common.clid       = hdr->client_id;
      cl_sp->service_id        = hdr->service;
      cl_sp->instance          = (uint16)hdr->qmi_instance;

      /*------------------------------------------------------------------------
        Clear event reporting
      ------------------------------------------------------------------------*/
      memset((void *)&cl_sp->report_status, 0, sizeof(cl_sp->report_status));

      /*------------------------------------------------------------------------
        Reset the subscription to primary
      ------------------------------------------------------------------------*/
      memset((void *)&cl_sp->subscription_status, 0, sizeof(cl_sp->subscription_status));
      cl_sp->subscription_status.subscription_type = WMSI_SUBSCRIPTION_PRIMARY;

      /*------------------------------------------------------------------------
        Reset the indication registration info
      ------------------------------------------------------------------------*/
      memset((void *)&cl_sp->indication_reg, 0, sizeof(cl_sp->indication_reg));

      /*------------------------------------------------------------------------
        Activate the WMS service client if it is not already active
      ------------------------------------------------------------------------*/
      wms_result = WMS_CLIENT_ERR_NONE;
      if (FALSE == qmi_wmsi_global.wms_cid_activated)
      {
        wms_result = wms_client_activate(qmi_wmsi_global.wms_cid);
      }

      if (wms_result == WMS_CLIENT_ERR_NONE)
      {
        if (FALSE == qmi_wmsi_global.wms_cid_activated)
        {
          qmi_wmsi_global.wms_cid_activated = TRUE;
        }
        result = QMI_RESULT_SUCCESS;
      }
      else
      {
        errval = QMI_ERR_INVALID_CLIENT_ID;
      }
    }
    else
    {
      errval = QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    errval = QMI_ERR_INVALID_CLIENT_ID;
  }

  MSG_HIGH_2("qmi_wms_process_alloc_clid: errval %d, client id %d", errval, hdr->client_id);

  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result_ex(result, hdr, errval);

#if (!defined( FEATURE_DUAL_SIM ) && !defined( FEATURE_TRIPLE_SIM ))
  if (cl_sp)
  {
    if (WMSI_MWI_STATE_INIT == qmi_wmsi_global.mwi[cl_sp->subscription_status.subscription_type].mwi_state)
    {
      // Send MWI indication to client.
      qmi_wmsi_message_waiting_ind(cl_sp, (uint8)cl_sp->subscription_status.subscription_type);
    }
  }
#endif /* !FEATURE_DUAL_SIM && !FEATURE_TRIPLE_SIM */
} /* qmi_wms_process_alloc_clid() */

/*===========================================================================
  FUNCTION qmi_wms_fw_dealloc_clid_cb()

  DESCRIPTION
    Callback to deallocate the client from Framework

  PARAMETERS
    hdr : QMI framework header to deallocate a client ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_fw_dealloc_clid_cb(
  qmi_framework_common_msg_hdr_type *hdr
)
{
  qmi_wms_task_buffer_s_type *ptr = NULL;

  if (hdr)
  {
    ptr = qmi_wms_get_cmd_buf();

    if (ptr)
    {
      ptr->task = WMS_TASK_CMD_DEALLOC_CLID;
      (void)memscpy(&ptr->data.fw_msg_hdr,
                    sizeof(qmi_framework_common_msg_hdr_type),
                    hdr,
                    sizeof(qmi_framework_common_msg_hdr_type));

      qmi_wms_put_cmd_buf(ptr);
    }
  }
  else
  {
    MSG_ERROR_0("qmi_wms_fw_dealloc_clid_cb() hdr is null");
  }
} /* qmi_wms_fw_dealloc_clid_cb() */
/*===========================================================================
  FUNCTION qmi_wmsi_dealloc_cl_sp()

  DESCRIPTION
    Free the client data buffer for the given clid.

  PARAMETERS
    clid: uint8 type

  RETURN VALUE
    None


  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void qmi_wmsi_dealloc_cl_sp(
 uint8 clid 
)
{
  qmi_wmsi_client_state_type *cl_sp = NULL;

  cl_sp = qmi_wms_state.client[clid - 1];
  q_destroy(&cl_sp->common.x_free_q);
  q_destroy(&cl_sp->common.cmd_free_q);
  qmi_wmsi_mem_free( cl_sp );
  qmi_wms_state.client[clid - 1] = NULL;

} /* qmi_wmsi_dealloc_cl_sp() */


/*===========================================================================
  FUNCTION qmi_wms_process_dealloc_clid()

  DESCRIPTION
    Process deallocate clid from Framework

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_process_dealloc_clid(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  qmi_framework_common_msg_hdr_type *hdr = NULL;
  qmi_wmsi_client_state_type      *cl_sp = NULL;
  qmi_result_e_type               result = QMI_RESULT_FAILURE;
  qmi_error_e_type                errval = QMI_ERR_NONE;

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  hdr = &user_data_ptr->data.fw_msg_hdr;
  if ( (QMI_SVC_CLID_UNUSED != hdr->client_id)  && (WMSI_MAX_CLIDS >= hdr->client_id) )
  {
    cl_sp = qmi_wms_state.client[hdr->client_id - 1]; // client id values start from 1

    if ( cl_sp )
    {
      if ( cl_sp->common.clid == hdr->client_id )
      {
        qmi_wmsi_reset_client( cl_sp );
        qmi_wmsi_dealloc_cl_sp( hdr->client_id ); 	 

        result = QMI_RESULT_SUCCESS;
      }
      else
      {
        errval = QMI_ERR_INVALID_CLIENT_ID;
      }
    }
    else
    {
      errval = QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    errval = QMI_ERR_INVALID_CLIENT_ID;
  }

  MSG_HIGH_2( "qmi_wms_process_dealloc_clid,: errval %d, client id %d", errval, hdr->client_id);
  
  /* Notifying the clid deallocation back to client via QMI Framework*/
  qmi_framework_svc_send_dealloc_clid_result_ex( result, hdr, errval );

} /* qmi_wms_process_dealloc_clid() */

/*===========================================================================
  FUNCTION qmi_wms_fw_init_cb()

  DESCRIPTION
    Callback for Service initialization from Framework

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_fw_init_cb(
  uint16 num_instances
)
{
  qmi_wms_task_buffer_s_type *ptr = NULL;


  /* Enable the signal to add it
     to the qmi_wms task wait_sig mask */
  qmi_wms_enable_sig(QMI_WMS_SERVICE_WMS_SIGNAL);

  ptr = qmi_wms_get_cmd_buf();

  if (ptr)
  {
    ptr->task = WMS_TASK_CMD_INIT;
    (void)memscpy(&ptr->data.num_instances,
                  sizeof(uint16),
                  &num_instances,
                  sizeof(uint16));

    qmi_wms_put_cmd_buf(ptr);
  }
} /* qmi_wms_fw_init_cb() */

/*===========================================================================
  FUNCTION qmi_wms_process_init_cback()

  DESCRIPTION
    Process init from Framework

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_process_init_cback(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  uint16 *num_instances = NULL;
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  num_instances = &user_data_ptr->data.num_instances;

  memset(&qmi_wms_state, 0, sizeof(qmi_wms_state));

  qmi_wms_state.num_instances = *num_instances;

  if (FALSE == qmi_wmsi_global.inited)
  {
    /*-----------------------------------------------------------------------
      Initialize the global WMS service parameters
    -----------------------------------------------------------------------*/
    qmi_wmsi_global.wms_cid            = WMSI_INVALID_CLIENT_ID;
    qmi_wmsi_global.wms_cid_activated  = FALSE;
    for (i = 0; i < WMSI_SUBSCRIPTION_MAX; i++)
    {
      qmi_wmsi_global.gw_template[i].inited = FALSE;
      qmi_wmsi_global.gw_template[i].mem_store = WMSI_MEM_STORE_MAX;
      qmi_wmsi_global.gw_template[i].index = 0;
      /* SMSC address and type */
      memset(&qmi_wmsi_global.gw_template[i].mo_template, 0,
             sizeof(qmi_wmsi_global.gw_template[i].mo_template));
    }

    /* If it is a multimode build, there is no global message mode. */
    qmi_wmsi_global.message_mode = WMSI_MSG_MODE_MAX;
#if defined (FEATURE_CDSMS) && !defined (FEATURE_GWSMS)
    /* This is a CDMA-only build */
    qmi_wmsi_global.message_mode = WMSI_MSG_MODE_CDMA;
#elif defined (FEATURE_GWSMS) && !defined (FEATURE_CDSMS)
    /* This is a GW-only build */
    qmi_wmsi_global.message_mode = WMSI_MSG_MODE_GW;
#endif

    qmi_wmsi_init_mwi_cache();
    memset(&qmi_wmsi_global.transport_layer, 0, sizeof(qmi_wmsi_global.transport_layer));
    qmi_wmsi_global.transport_layer.state = WMSI_TRANSPORT_LAYER_STATE_UNINIT;
    memset(&qmi_wmsi_global.transport_nw_reg_status, 0, sizeof(qmi_wmsi_global.transport_nw_reg_status));
    qmi_wmsi_global.transport_nw_reg_status.state = WMSI_TRANSPORT_NW_REG_STATE_UNINIT;

    for (i = 0; i < WMSI_SUBSCRIPTION_MAX; i++)
    {
      qmi_wmsi_global.service_ready_status[i].status        = WMSI_SERVICE_READY_STATUS_NONE;
      qmi_wmsi_global.sim_ready_status[i].sim_ready_status  = WMSI_SIM_READY_STATUS_NONE;
    }

  /*-------------------------------------------------------------------------
      Init the QMI_WMS command pending queue
  -------------------------------------------------------------------------*/
    memset (&qmi_wmsi_global.cmd_pend_q, 0, sizeof(q_type));
    (void) q_init (&qmi_wmsi_global.cmd_pend_q);

    qmi_wmsi_global.dbg_retry_period   = WMSI_DBG_RETRY_TIMER_MAX;
    qmi_wmsi_global.dbg_retry_interval = WMSI_DBG_RETRY_TIMER_MAX;

    qmi_wmsi_global.inited = TRUE;
  }

  /*-------------------------------------------------------------------------
    Init the WMS client for the service if it has not yet happened
  -------------------------------------------------------------------------*/
  if (WMSI_INVALID_CLIENT_ID == qmi_wmsi_global.wms_cid)
  {
    /*  This will initialize our client ID properly */
    if (WMS_CLIENT_ERR_NONE != wms_client_init (WMS_CLIENT_TYPE_QMI, &qmi_wmsi_global.wms_cid))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_init");
      return;
    }

    /*-----------------------------------------------------------------------
      Register for necessary events with the WMS service
    -----------------------------------------------------------------------*/
    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_msg_cb (qmi_wmsi_global.wms_cid, qmi_wms_msg_evt_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_msg_cb");
    }

    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_cfg_cb (qmi_wmsi_global.wms_cid, qmi_wms_cfg_evt_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_cfg_cb");
    }

    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_detail_cb (qmi_wmsi_global.wms_cid, TRUE))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_detail_cb");
    }

    if (WMS_CLIENT_ERR_NONE != wms_client_reg_dc_cb(qmi_wmsi_global.wms_cid, qmi_wms_dc_evt_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_dc_cb");
    }

    if (WMS_CLIENT_ERR_NONE != wms_client_reg_bc_mm_cb(qmi_wmsi_global.wms_cid, qmi_wms_bc_mm_evt_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_bc_mm_cb");
    }

    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_transport_status_cb(qmi_wmsi_global.wms_cid, qmi_wms_transport_status_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_transport_status_cb");
    }

    if (WMS_CLIENT_ERR_NONE !=
        wms_client_reg_dbg_cb(qmi_wmsi_global.wms_cid, qmi_wms_dbg_evt_cb))
    {
      MSG_ERROR_0("QMI_WMS: Error in wms_client_reg_dbg_cb");
    }
  }
} /* qmi_wms_process_init_cback() */

/*===========================================================================
  FUNCTION qmi_wms_fw_cmd_hdlr_cb()

  DESCRIPTION
    Callback REQ message from Framework

  PARAMETERS
    hdr : QMI framework header for Req message
    sdu : QMI framework data for Req message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_fw_cmd_hdlr_cb(
  qmi_framework_msg_hdr_type *hdr,
  dsm_item_type **sdu
)
{
  qmi_wms_task_buffer_s_type *ptr = NULL;

  ptr = qmi_wms_get_cmd_buf();

  if (ptr)
  {
    ptr->task = WMS_TASK_CMD_CMD_HDLR;
    ptr->data.cmd_hdlr.hdr.common_hdr.service        = hdr->common_hdr.service;
    ptr->data.cmd_hdlr.hdr.common_hdr.client_id      = hdr->common_hdr.client_id;
    ptr->data.cmd_hdlr.hdr.common_hdr.qmi_instance   = hdr->common_hdr.qmi_instance;
    ptr->data.cmd_hdlr.hdr.common_hdr.transaction_id = hdr->common_hdr.transaction_id;
    ptr->data.cmd_hdlr.hdr.msg_ctl_flag              = hdr->msg_ctl_flag;
    ptr->data.cmd_hdlr.hdr.msg_len                   = hdr->msg_len;
    ptr->data.cmd_hdlr.sdu                           = *sdu;

    qmi_wms_put_cmd_buf(ptr);
  }
} /* qmi_wms_fw_cmd_hdlr_cb() */

/*===========================================================================
  FUNCTION qmi_wms_process_cmd_hdlr()

  DESCRIPTION
    Process REQ message from Framework

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void qmi_wms_process_cmd_hdlr(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  qmi_wms_cmd_hdlr_type * ptr = NULL;
  qmi_wmsi_client_state_type *cl_sp = NULL;

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  ptr = &user_data_ptr->data.cmd_hdlr;

  if (ptr->sdu)
  {
    if (QMI_SVC_CLID_UNUSED != ptr->hdr.common_hdr.client_id &&
        WMSI_MAX_CLIDS >= ptr->hdr.common_hdr.client_id)
    {
      MSG_HIGH_3( "qmi_wms_process_cmd_hdlr(): clid %d tx_id %d ctl_flag %d",
                  ptr->hdr.common_hdr.client_id,
                  ptr->hdr.common_hdr.transaction_id,
                  ptr->hdr.msg_ctl_flag );

      cl_sp = qmi_wms_state.client[ptr->hdr.common_hdr.client_id - 1];

      if (cl_sp)
      {
        ds_qmi_fw_recv(&qmi_wmsi_cfg, &qmi_wms_state, &ptr->hdr, &cl_sp->common, ptr->sdu);
      }
      else
      {
        MSG_ERROR_0("qmi_wms_process_cmd_hdlr(): cl_sp is NULL");
      }
    }
    else
    {
      MSG_ERROR_1("qmi_wms_process_cmd_hdlr(): client_id is invalid %d", ptr->hdr.common_hdr.client_id);
    }
  }
  else
  {
    MSG_ERROR_0("qmi_wms_process_cmd_hdlr(): sdu is NULL");
  }

} /* qmi_wms_process_cmd_hdlr() */

/*===========================================================================
  FUNCTION QMI_WMS_CMD_CB()

  DESCRIPTION
    Callback function called by WMS when QMI invokes message commands and WMS
    has an intermediate command status

  PARAMETERS
    cmd       : command that has issued the return value
    user_data : user data pointer passed when the command was called
    cmd_err   : error value returned from the command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_cmd_status_cb(
  wms_cmd_id_e_type cmd,
  const void *user_data,
  wms_cmd_err_e_type cmd_err
)
{
  qmi_wms_task_buffer_s_type * cmd_status_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (WMS_CMD_MAX <= cmd || WMS_CMD_ERR_MAX <= cmd_err)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    post a new DS command to process
  -------------------------------------------------------------------------*/
  switch (cmd)
  {
    /* CFG commands */
    case WMS_CMD_CFG_SET_ROUTES:
    case WMS_CMD_CFG_GET_ROUTES:
    case WMS_CMD_CFG_GET_MESSAGE_LIST:
    case WMS_CMD_CFG_GET_MEMORY_STATUS:
    case WMS_CMD_MSG_READ_TEMPLATE:
    case WMS_CMD_MSG_WRITE_TEMPLATE:
    case WMS_CMD_CFG_SET_LINK_CONTROL:
    case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
    case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
    case WMS_CMD_CFG_SET_LTE_DOMAIN_PREF:
    case WMS_CMD_CFG_SET_MEMORY_FULL:
    case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
    case WMS_CMD_CFG_SET_MESSAGE_WAITING:
    case WMS_CMD_CFG_GET_MESSAGE_WAITING:

    /* DBG Commands */
    case WMS_CMD_DBG_GET_RETRY_INTERVAL:
    case WMS_CMD_DBG_SET_RETRY_INTERVAL:
    case WMS_CMD_DBG_GET_RETRY_PERIOD:
    case WMS_CMD_DBG_SET_RETRY_PERIOD:

    /* DC Commands */
    case WMS_CMD_DC_CONNECT:
    case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
    case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
    case WMS_CMD_DC_DISCONNECT:

      /* MSG commands */
    case WMS_CMD_MSG_SEND:
    case WMS_CMD_MSG_READ:
    case WMS_CMD_MSG_WRITE:

    case WMS_CMD_MSG_ACK:

    case WMS_CMD_MSG_DELETE:
    case WMS_CMD_MSG_DELETE_ALL:
    case WMS_CMD_MSG_MODIFY_TAG:

    /* BC SMS commands */
    case WMS_CMD_BC_MM_SET_PREF:
    case WMS_CMD_BC_MM_ADD_SRV:
    case WMS_CMD_BC_MM_GET_PREF:
    case WMS_CMD_BC_MM_GET_TABLE:
    case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
    case WMS_CMD_BC_MM_DELETE_SRV:
      cmd_status_ptr = qmi_wms_get_cmd_buf();

      if (NULL != cmd_status_ptr)
      {
        cmd_status_ptr->task = WMS_TASK_CMD_CMD_STATUS;
        cmd_status_ptr->data.cmd_status.cmd       = cmd;
        cmd_status_ptr->data.cmd_status.user_data = user_data;
        cmd_status_ptr->data.cmd_status.cmd_err   = cmd_err;

        qmi_wms_put_cmd_buf(cmd_status_ptr);
      }
      break;

    default:
      MSG_HIGH_1("Unexpected command received (%d)", cmd);
  }

  return;
} /* qmi_wms_cmd_status_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_CMD_STATUS()

  DESCRIPTION
    Process command status updates from the WMS service

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_cmd_status(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  dsm_item_type               *response                = NULL;
  qmi_cmd_buf_type            *cmd_buf_p               = NULL;
  qmi_error_e_type             errval                  = QMI_ERR_NONE;
  qmi_result_e_type            result;
  qmi_wmsi_client_state_type  *wms_cl_sp               = NULL;
  qmi_wms_cmd_node_type       *cmd_node                = NULL;
  qmi_wms_cmd_status_type     *cmd_status_ptr          = NULL;
  qmi_wms_tid_type             tid;
  uint16                       cmd_type;
  wms_cmd_id_e_type            cmd;
  boolean                      cmd_buf_must_be_freed   = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  cmd_status_ptr = &user_data_ptr->data.cmd_status;
  cmd = cmd_status_ptr->cmd;

  // tid is passed as user_data when sending the followign commands to WMS
  if (WMS_CMD_MSG_SEND == cmd || WMS_CMD_MSG_ACK == cmd || WMS_CMD_CFG_SET_LINK_CONTROL == cmd)
  {
    tid = (qmi_wms_tid_type) cmd_status_ptr->user_data;
    cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_tid,
                                             &tid, TRUE);
    if (NULL == cmd_node)
    {
      return;
    }
    
      MSG_HIGH_1("Found the cmd_node with tid %d", tid);
      wms_cl_sp = cmd_node->wms_cl_sp;
      cmd_type = cmd_node->cmd_type;
      cmd_buf_p = cmd_node->cmd_buf;
    }
  else // cmd_buf_p is passed as user_data for other WMS commands.
  {
    cmd_buf_p = (qmi_cmd_buf_type *) cmd_status_ptr->user_data;
    if (NULL == cmd_buf_p)
    {
      MSG_ERROR_0("NULL cmd_buf_p");
      return;
    }
    wms_cl_sp = (qmi_wmsi_client_state_type *)qmi_wmsi_get_client_sp_by_cmd_buf(cmd_buf_p);

    if (NULL != wms_cl_sp)
    {
      cmd_type = cmd_buf_p->cmd_type;
    }
  }

  if (NULL == wms_cl_sp)
  {
    MSG_ERROR_0("Null wms_cl_sp from cmd_buf pointer");
    if (NULL != cmd_node)
    {
      qmi_wmsi_free_cmd_node(cmd_node);
      cmd_node = NULL;
    }
    return;
  }

  if (WMS_CMD_ERR_NONE == cmd_status_ptr->cmd_err)
  {
    MSG_HIGH_2( "Processing success command (%d) status, error (%d)",
                cmd,
                cmd_status_ptr->cmd_err );
    switch (cmd)
    {
      /*------------------------------------------------------------------------
        ignore these commands for now when successful
        these commands use events that are still to come to generate responses
      ------------------------------------------------------------------------*/
      case WMS_CMD_MSG_SEND:
      case WMS_CMD_MSG_WRITE:
      case WMS_CMD_MSG_READ:
      case WMS_CMD_MSG_READ_TEMPLATE:
      case WMS_CMD_MSG_ACK:
        return;

      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_GET_MESSAGE_LIST:
      case WMS_CMD_CFG_GET_MEMORY_STATUS:
      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_MESSAGE_WAITING:
        return;

      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
        return;

      case WMS_CMD_BC_MM_GET_PREF:
      case WMS_CMD_BC_MM_GET_TABLE:
      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
        return;

      case WMS_CMD_BC_MM_DELETE_SRV:
        cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                cmd_buf_p, TRUE);
        if (NULL != cmd_node && 0 == cmd_node->cache.set_bc_config.num_entries)
        {
          // Send the response now
          break;
        }
        else
        {
          // Wait for cmd status for WMS_CMD_BC_MM_ADD_SRV
          return;
        }

      /*------------------------------------------------------------------------
        return generic response TLV's for these successful commands
      ------------------------------------------------------------------------*/
      case WMS_CMD_MSG_WRITE_TEMPLATE:
      case WMS_CMD_MSG_DELETE:
      case WMS_CMD_MSG_DELETE_ALL:
      case WMS_CMD_MSG_MODIFY_TAG:
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
      case WMS_CMD_BC_MM_SET_PREF:
      case WMS_CMD_BC_MM_ADD_SRV:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_MEMORY_FULL:
      case WMS_CMD_CFG_SET_MESSAGE_WAITING:
        break;

      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
        /* if setting primary client successful, update client state value */

        cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                  cmd_buf_p, FALSE);
        if (cmd_node)
        {
          MSG_HIGH_3( "Client %d: updating sub_type %d to %d",
                      wms_cl_sp->common.clid,
                      wms_cl_sp->subscription_status.subscription_type,
                      cmd_node->cache.primary_client.enabled );

          wms_cl_sp->subscription_status.primary_client_state[(wms_cl_sp->subscription_status.subscription_type)] = cmd_node->cache.primary_client.enabled;
          qmi_wmsi_free_cmd_node(cmd_node);
          cmd_node = NULL;
        }
        break;

      default:
        MSG_HIGH_0("Unknown command status found");
        errval = QMI_ERR_UNKNOWN;
        break;
    }
  }
  else
  {
    MSG_HIGH_2( "Processing failed command status (%d) error (%d)",
                cmd,
                cmd_status_ptr->cmd_err );
    switch (cmd)
    {
      /*------------------------------------------------------------------------
        return generic response TLV's for these successful commands
      ------------------------------------------------------------------------*/
      case WMS_CMD_MSG_SEND:
      case WMS_CMD_MSG_WRITE:
      case WMS_CMD_MSG_READ:
      case WMS_CMD_MSG_READ_TEMPLATE:
      case WMS_CMD_MSG_WRITE_TEMPLATE:

      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_GET_MESSAGE_LIST:
      case WMS_CMD_CFG_GET_MEMORY_STATUS:
      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_MSG_DELETE:
      case WMS_CMD_MSG_DELETE_ALL:
      case WMS_CMD_MSG_MODIFY_TAG:
      case WMS_CMD_MSG_ACK:
      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
      case WMS_CMD_BC_MM_SET_PREF:
      case WMS_CMD_BC_MM_ADD_SRV:
      case WMS_CMD_BC_MM_GET_PREF:
      case WMS_CMD_BC_MM_GET_TABLE:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_MEMORY_FULL:
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
      case WMS_CMD_BC_MM_DELETE_SRV:
      case WMS_CMD_CFG_SET_MESSAGE_WAITING:
      case WMS_CMD_CFG_GET_MESSAGE_WAITING:
        errval = qmi_wmsi_from_wms_error(cmd_status_ptr->cmd_err);
        break;

      default:
        MSG_HIGH_0("Unknown command status found");
        errval = QMI_ERR_UNKNOWN;
        break;
    }
  }

  if ((WMSI_CMD_VAL_ASYNC_RAW_SEND == cmd_type) ||
      (WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE == cmd_type) ||
      (WMSI_CMD_VAL_ASYNC_SEND_ACK == cmd_type))
  {
    // Send one-shot indication
    PACK(struct)
    {
       uint16 ecode;
    } v_result;

    PACK(struct)
    {
       uint32 user_data;
    } v_user_data;

    v_result.ecode = (uint16) errval;

    if (!qmi_svc_put_param_tlv(&response,
                               QMI_TYPE_REQUIRED_PARAMETERS,
                               sizeof(v_result),
                               &v_result))
    {
      goto clean_up;
    }

    // Add user_data TLV if needed
    if ((WMSI_CMD_VAL_ASYNC_SEND_ACK == cmd_type) &&
        (NULL != cmd_node) &&
        (cmd_node->cache.send_ack.got_user_data_tlv))
    {

      v_user_data.user_data = cmd_node->cache.send_ack.user_data;
      if (!qmi_svc_put_param_tlv(&response,
                                 WMSI_PRM_TYPE_SEND_ACK_IND_USER_DATA,
                                 sizeof(v_user_data),
                                 &v_user_data))
      {
        goto clean_up;
      }
    }
    else if ((WMSI_CMD_VAL_ASYNC_RAW_SEND == cmd_type ||
              WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE == cmd_type) &&
              (NULL != cmd_node) &&
              cmd_node->cache.raw_send.got_user_data_tlv)
    {

      v_user_data.user_data = cmd_node->cache.raw_send.user_data;
      if (!qmi_svc_put_param_tlv(&response,
                                 (WMSI_CMD_VAL_ASYNC_RAW_SEND == cmd_type)?
                                 WMSI_PRM_TYPE_ASYNC_SEND_IND_USER_DATA :
                                 WMSI_PRM_TYPE_ASYNC_MEM_STORE_IND_USER_DATA,
                                 sizeof(v_user_data),
                                 &v_user_data))
      {
        goto clean_up;
      }
    }


    if (!qmi_wmsi_send_ind(wms_cl_sp,
                           cmd_type, // indication and cmd are identical
                           response))
    {
        MSG_HIGH_1("Unable to send indication %d", cmd_type);
        response = NULL;
        goto clean_up;
    }
  }
  else
  {
    // Send response
    result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;
    if (!qmi_svc_put_result_tlv(&response, result, errval))
    {
      goto clean_up;
    }
    MSG_HIGH_2( "Send response for sync cmd_type = %d, cmd_buf_p = 0x%x",
                cmd_type,
                cmd_buf_p );
	
    cmd_buf_must_be_freed = FALSE;  // cmd_buf gets freed in qmi_wmsi_send_response, no need to free it in cleanup.
	
    if (!qmi_wmsi_send_response((void *)cmd_buf_p, response))
    {
       MSG_HIGH_0("Unable to send WMS CMD ERR response");
       response = NULL;
       goto clean_up;
    }
  }

  if (NULL == cmd_node && NULL != cmd_buf_p)
  {
    cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                             cmd_buf_p, FALSE);
  }

  if (NULL != cmd_node)
  {
    q_delete(&qmi_wmsi_global.cmd_pend_q, &cmd_node->link);
    qmi_wmsi_free_cmd_node(cmd_node);
    cmd_node = NULL;
  }

  return;

clean_up:
  if (NULL != response)
  {
    dsm_free_packet(&response);
  }

  /* On Error, cleanup the cmd_node */
  if (NULL != cmd_buf_p)
  {
    cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                            cmd_buf_p, FALSE);
  }

  if (NULL != cmd_node)
  {
    q_delete(&qmi_wmsi_global.cmd_pend_q, &cmd_node->link);
    qmi_wmsi_free_cmd_node(cmd_node);
    cmd_node = NULL;
  }

  if ((NULL != cmd_buf_p) && (cmd_buf_must_be_freed))
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
  }
} /* qmi_wms_process_cmd_status */


/*===========================================================================
  FUNCTION QMI_WMS_MSG_EVT_CB()

  DESCRIPTION
    Callback function called by WMS when QMI invokes message commands and WMS
    has an event to return

  PARAMETERS
    msg_event      : event
    msg_event_info : event info struct
    shared         : if this event is shared by all WMS clients

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_msg_evt_cb(
  wms_msg_event_e_type event,
  wms_msg_event_info_s_type *info_ptr,
  boolean *shared
)
{
  qmi_wms_task_buffer_s_type * msg_evt_ptr = NULL;
  wms_client_type_e_type       client_id   = WMS_CLIENT_TYPE_INTERNAL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check for NULL wms message event info pointer */
  if (NULL == info_ptr)
  {
    MSG_ERROR_0("Null info_ptr");
    return;
  }
  /* check for valid wms message event */
  if (WMS_MSG_EVENT_MAX <= event)
  {
    return;
  }

  switch(event)
  {
    case WMS_MSG_EVENT_SEND:
    case WMS_MSG_EVENT_WRITE:
    case WMS_MSG_EVENT_READ:
    case WMS_MSG_EVENT_READ_TEMPLATE:
    case WMS_MSG_EVENT_WRITE_TEMPLATE:		
    case WMS_MSG_EVENT_SMSC_ADDR_CHANGE:		
      client_id = info_ptr->status_info.client_id;
      break;

    case WMS_MSG_EVENT_ACK_REPORT:
      client_id = info_ptr->ack_report_info.client_id;
      break;

    default:
      break;
  }

  if ((WMS_CLIENT_TYPE_QMI != client_id) && (WMS_CLIENT_TYPE_INTERNAL != client_id))     
  {
    /* This event is for another client. Ignore it. */
    MSG_ERROR_2("Ignore this event %d, client ID mismatch %d", event, client_id);
    return;
  }

  MSG_HIGH_1("Message cb found event (%d)", event);
  switch (event)
  {
    /*-------------------------------------------------------------------------
      Post a cmd to QMI_WMS to handle this in QMI_WMS context
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_SEND:
    case WMS_MSG_EVENT_WRITE:
    case WMS_MSG_EVENT_READ:

    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
    case WMS_MSG_EVENT_SUBMIT_REPORT:
    case WMS_MSG_EVENT_ACK_REPORT:
    case WMS_MSG_EVENT_STATUS_REPORT:
    case WMS_MSG_EVENT_TRANSPORT_REG:
    case WMS_MSG_EVENT_ETWS_NOTIFICATION:
    case WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION:

    case WMS_MSG_EVENT_READ_TEMPLATE:
    case WMS_MSG_EVENT_SMSC_ADDR_CHANGE:
      msg_evt_ptr = qmi_wms_get_cmd_buf();
      if (NULL != msg_evt_ptr)
      {
        msg_evt_ptr->task = WMS_TASK_CMD_MSG_EVENT;
        msg_evt_ptr->data.msg_event.event = event;
        (void)memscpy(&msg_evt_ptr->data.msg_event.event_info,
                      sizeof(wms_msg_event_info_s_type),
                      info_ptr,
                      sizeof(wms_msg_event_info_s_type));

        if (WMS_MSG_EVENT_SEND == event)
        {
          if (info_ptr->status_info.alpha_id.len > 0 && NULL != info_ptr->status_info.alpha_id.data)
          {
            qmi_wmsi_deep_copy((void**)&msg_evt_ptr->data.msg_event.event_info.status_info.alpha_id.data,
                               (void*)info_ptr->status_info.alpha_id.data,
                               info_ptr->status_info.alpha_id.len);
          }
          else
          {
            msg_evt_ptr->data.msg_event.event_info.status_info.alpha_id.data = NULL;
            msg_evt_ptr->data.msg_event.event_info.status_info.alpha_id.len = 0;
          }
        }

        /* Deep copy summary and details information. */
        if (WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION == event)
        {
          MSG_HIGH_2("summary count = %d, details count = %d", 
                      info_ptr->transport_mwi_info.summary_tuple_count, 
                      info_ptr->transport_mwi_info.details_tuple_count);

          if (info_ptr->transport_mwi_info.summary_tuple_count > 0 && NULL != info_ptr->transport_mwi_info.mwi_msg_summary)
          {
            qmi_wmsi_deep_copy((void**)&msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.mwi_msg_summary,
                               (void*)info_ptr->transport_mwi_info.mwi_msg_summary,
                               ((info_ptr->transport_mwi_info.summary_tuple_count)*sizeof(wms_msg_mwi_summary_s_type)));
          }
          else
          {
            msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.mwi_msg_summary = NULL;
            msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.summary_tuple_count = 0;
          }

          if (info_ptr->transport_mwi_info.details_tuple_count > 0 && NULL != info_ptr->transport_mwi_info.mwi_msg_details)
          {
            qmi_wmsi_deep_copy((void**)&msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.mwi_msg_details,
                               (void*)info_ptr->transport_mwi_info.mwi_msg_details,
                               ((info_ptr->transport_mwi_info.details_tuple_count)*sizeof( wms_msg_mwi_detail_s_type)));
          }
          else
          {
            msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.mwi_msg_details = NULL;
            msg_evt_ptr->data.msg_event.event_info.transport_mwi_info.details_tuple_count = 0;
          }
        }

        if (WMS_MSG_EVENT_SMSC_ADDR_CHANGE == event)
        {
          qmi_wmsi_subscription_e_type sub_index = qmi_wms_get_subscription_from_asid( info_ptr->status_info.as_id );
          if (WMSI_SUBSCRIPTION_MAX == sub_index)
          {
            MSG_ERROR_0( "Unable to map as_id in WMS_MSG_EVENT_SMSC_ADDR_CHANGE to a subscription" );
            return;
          }

          if (qmi_wmsi_global.gw_template[sub_index].mo_template.alpha_id.len > 0 && 
              NULL != qmi_wmsi_global.gw_template[sub_index].mo_template.alpha_id.data)
          {
            QMI_WMSI_MEM_FREEIF(qmi_wmsi_global.gw_template[sub_index].mo_template.alpha_id.data);
            qmi_wmsi_global.gw_template[sub_index].mo_template.alpha_id.len = 0;
          }
          if (info_ptr->status_info.message.u.gw_template.alpha_id.len > 0 && 
              NULL != info_ptr->status_info.message.u.gw_template.alpha_id.data)
          {
            qmi_wmsi_deep_copy( (void**)&msg_evt_ptr->data.msg_event.event_info.status_info.message.u.gw_template.alpha_id.data,
                                (void*)info_ptr->status_info.message.u.gw_template.alpha_id.data,
                                info_ptr->status_info.message.u.gw_template.alpha_id.len );
          }
          else
          {
            msg_evt_ptr->data.msg_event.event_info.status_info.message.u.gw_template.alpha_id.data = NULL;
            msg_evt_ptr->data.msg_event.event_info.status_info.message.u.gw_template.alpha_id.len = 0;
          }
        }

        qmi_wms_put_cmd_buf(msg_evt_ptr);
      }
      else
      {
        MSG_ERROR_0("Mem alloc failed");
      }
      break;

    /*-------------------------------------------------------------------------
      These events have already been handled by the command status callback,
      ignore them
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_DELETE:
    case WMS_MSG_EVENT_DELETE_ALL:
    case WMS_MSG_EVENT_MODIFY_TAG:
      break;

    default:
      MSG_HIGH_0("Unexpected event received in qmi_wms_msg_cb");
  }

  return;
} /* qmi_wms_msg_evt_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_MSG_EVT()

  DESCRIPTION
    Process message events from the WMS service

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_msg_evt(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  dsm_item_type               *response              = NULL;
  dsm_item_type               *ind                   = NULL;
  qmi_wmsi_state_type         *wms_sp                = NULL;
  qmi_cmd_buf_type            *cmd_buf_p             = NULL;
  qmi_error_e_type             errval                = QMI_ERR_NONE;
  qmi_result_e_type            result;
  qmi_wms_cmd_node_type       *cmd_node              = NULL;
  sys_modem_as_id_e_type       as_id                 = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type  *wms_cl_sp             = NULL;
  qmi_wmsi_subscription_e_type subscription;
  qmi_wms_msg_event_type      *msg_evt_ptr           = NULL;
  wms_msg_event_info_s_type   *event_info            = NULL;
  qmi_wms_tid_type             tid;
  boolean                      async_cmd             = FALSE;
  boolean                      cmd_buf_must_be_freed = TRUE;
  void                        *response_pdu          = NULL;
  void                        *ind_pdu               = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wms_sp = &qmi_wms_state;

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  msg_evt_ptr = &user_data_ptr->data.msg_event;
  event_info = &msg_evt_ptr->event_info;

  MSG_HIGH_1("Processing msg event (%d)", msg_evt_ptr->event);
  switch (msg_evt_ptr->event)
  {
    /*-------------------------------------------------------------------------
      sends are ignored because a MO message update will follow
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_SEND:
      tid = (qmi_wms_tid_type) event_info->status_info.user_data;
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_tid,
                                               &tid, TRUE);
      if (NULL != cmd_node)
      {
        cmd_node->cache.raw_send.wms_msgid = qmi_wmsi_retrieve_msgid(event_info);
        cmd_node->cache.raw_send.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        if (event_info->status_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
        {
          cmd_node->cache.raw_send.mo_ctrl_status = event_info->status_info.mo_ctrl_status;

          if (NULL != event_info->status_info.alpha_id.data)
          {
            cmd_node->cache.raw_send.alpha_id_len = event_info->status_info.alpha_id.len;
            (void)memscpy(cmd_node->cache.raw_send.alpha_id,
                          WMSI_ALPHA_ID_MAX,
                          event_info->status_info.alpha_id.data,
                          event_info->status_info.alpha_id.len);

            QMI_WMSI_MEM_FREEIF(event_info->status_info.alpha_id.data);
          }
        }
      }

      /*Ignoring send event until WMS_MSG_EVENT_SUBMIT_REPORT received */
      return;

    /*-------------------------------------------------------------------------
      generate the raw write response packet with the index written data
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_WRITE:
      cmd_buf_p = (qmi_cmd_buf_type *) event_info->status_info.user_data;
      if (wms_sp == qmi_wmsi_get_wms_sp_by_cmd_buf(cmd_buf_p))
      {
        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_raw_write_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        }
        errval = qmi_wmsi_write_resp((wms_raw_write_resp_msg_v01*)response_pdu, event_info);
        ENCODE_RESPONSE_TLV(wms_raw_write_resp_msg_v01, QMI_WMS_RAW_WRITE_RESP_V01);
      }
      else
      {
        /* not a QMI event, ignore it*/
        return;
      }	  
      break;

    /*-------------------------------------------------------------------------
      generate the raw read response packet with the message data read
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_READ:
      cmd_buf_p = (qmi_cmd_buf_type *) event_info->status_info.user_data;
      if (wms_sp == qmi_wmsi_get_wms_sp_by_cmd_buf(cmd_buf_p))
      {
        if (WMSI_CMD_VAL_MODIFY_TAG == cmd_buf_p->cmd_type)
        {
          cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                  cmd_buf_p, FALSE);
          if (cmd_node)
          {
            /*-------------------------------------------------------------------
              CB from read portion of modify tag: prevent changeover from MO
              to MT tag (msgs are encoded differently)
            -------------------------------------------------------------------*/
            if (cmd_node->cache.modify_tag.new_tag_type ==
                event_info->status_info.message.msg_hdr.tag)
            {
              /*-----------------------------------------------------------------
                No tag change - skip wms_modify_tag step
              -----------------------------------------------------------------*/
              qmi_wmsi_free_cmd_node(cmd_node);
              cmd_node = NULL;

              response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_modify_tag_resp_msg_v01) );
              if (NULL == response_pdu)
              {
                return;
              }
              ENCODE_RESPONSE_TLV(wms_modify_tag_resp_msg_v01, QMI_WMS_MODIFY_TAG_RESP_V01);              
              goto skip_reset; 
            }
            else if ((WMS_IS_MO(cmd_node->cache.modify_tag.new_tag_type) &&
                      WMS_IS_MO(event_info->status_info.message.msg_hdr.tag))
                     ||
                     (WMS_IS_MT(cmd_node->cache.modify_tag.new_tag_type) &&
                      WMS_IS_MT(event_info->status_info.message.msg_hdr.tag)))
            {
              /*-----------------------------------------------------------------
                Queue wms_modify_tag command - only send QMI resp from here if
                we get an error from adding the cmd to the WMS queue
              -----------------------------------------------------------------*/
              wms_cl_sp = (qmi_wmsi_client_state_type *)qmi_wmsi_get_client_sp_by_cmd_buf(cmd_buf_p);
              if (NULL != wms_cl_sp)
              {
                as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
              }

              if (SYS_MODEM_AS_ID_NONE == as_id)
              {
                errval = QMI_ERR_INTERNAL;
              }
              else
              {
                errval = (qmi_error_e_type)
                         qmi_wmsi_from_wms_status(wms_msg_ms_modify_tag(qmi_wmsi_global.wms_cid,
                                                                        as_id,
                                                                        (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                        event_info->status_info.user_data,
                                                                        event_info->status_info.message.msg_hdr.mem_store,
                                                                        event_info->status_info.message.msg_hdr.index,
                                                                        cmd_node->cache.modify_tag.new_tag_type));
              }

              qmi_wmsi_free_cmd_node(cmd_node);
              cmd_node = NULL;              

              if (QMI_ERR_NONE == errval)
              {
                return;
              }
              else
              {
                response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_modify_tag_resp_msg_v01) );
                if (NULL == response_pdu)
                {
                  return;
                }
                ENCODE_RESPONSE_TLV(wms_modify_tag_resp_msg_v01, QMI_WMS_MODIFY_TAG_RESP_V01);
                goto skip_reset; 
             }
            }
            else
            {
              qmi_wmsi_free_cmd_node(cmd_node);
              cmd_node = NULL;
              errval = QMI_ERR_INVALID_ARG;

              response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_modify_tag_resp_msg_v01) );
               if (NULL == response_pdu)
               {
                 return;            
               }
              ENCODE_RESPONSE_TLV(wms_modify_tag_resp_msg_v01, QMI_WMS_MODIFY_TAG_RESP_V01);
            }
          }
        }
        else
        {
          /*-------------------------------------------------------------------
            Read CB from normal (not modify_tag) QMI command
          -------------------------------------------------------------------*/
          response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_raw_read_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          }                
          errval       = qmi_wmsi_read_resp((wms_raw_read_resp_msg_v01*)response_pdu, event_info);
          ENCODE_RESPONSE_TLV(wms_raw_read_resp_msg_v01, QMI_WMS_RAW_RESP_MSG_V01);
        }
      }
      else
      {
        /* not a QMI event, ignore it*/
        return;
      }
      break;

    /*-------------------------------------------------------------------------
      MO message update
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_SUBMIT_REPORT:
      if (WMS_CLIENT_TYPE_QMI == event_info->submit_report_info.client_id)
      {
        tid = (qmi_wms_tid_type) event_info->submit_report_info.user_data;
        cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_tid,
                                                 &tid, FALSE);  // dequeue

        if (NULL != cmd_node && NULL != cmd_node->wms_cl_sp)
        {
          wms_cl_sp = cmd_node->wms_cl_sp;
          cmd_buf_p = cmd_node->cmd_buf;

          if (cmd_node->async_cmd)
          {
            if (WMSI_CMD_VAL_ASYNC_RAW_SEND == cmd_node->cmd_type)
            {
              ind_pdu = qmi_wmsi_mem_alloc( sizeof(wms_async_raw_send_ind_msg_v01) ); 
              if (NULL == ind_pdu)
              {
                return;
              }
              errval  = qmi_wmsi_async_send_ind((wms_async_raw_send_ind_msg_v01*)ind_pdu, event_info, cmd_node);
              ((wms_async_raw_send_ind_msg_v01*)ind_pdu)->send_status =  (qmi_error_type_v01)errval;
              ENCODE_INDICATION_TLV(wms_async_raw_send_ind_msg_v01, QMI_WMS_ASYNC_RAW_SEND_IND_V01);              
            }
            else if (WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE == cmd_node->cmd_type)
            {
              ind_pdu =  qmi_wmsi_mem_alloc( sizeof(wms_async_send_from_mem_store_ind_msg_v01) );
              if (NULL == ind_pdu)
              {
                return;
              }              
              errval	= qmi_wmsi_async_mem_send_ind((wms_async_send_from_mem_store_ind_msg_v01*)ind_pdu, event_info, cmd_node);
              ((wms_async_send_from_mem_store_ind_msg_v01*)ind_pdu)->send_status =	(qmi_error_type_v01)errval;
              ENCODE_INDICATION_TLV(wms_async_send_from_mem_store_ind_msg_v01, QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_IND_V01);
            }
          }
          else
          {
            if (WMSI_CMD_VAL_RAW_SEND == cmd_node->cmd_type)
            {

              response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_raw_send_resp_msg_v01) );
              if (NULL == response_pdu)
              {
                return;
              } 
              errval       = qmi_wmsi_send_resp((wms_raw_send_resp_msg_v01*)response_pdu, event_info, cmd_node);
              ENCODE_RESPONSE_TLV(wms_raw_send_resp_msg_v01, QMI_WMS_RAW_SEND_RESP_V01);
            }
            else if (WMSI_CMD_VAL_SEND_FROM_MEM_STORE == cmd_node->cmd_type)
            {
              response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_send_from_mem_store_resp_msg_v01) );
              if (NULL == response_pdu)
              {
                return;
              } 
              errval       = qmi_wmsi_mem_send_resp((wms_send_from_mem_store_resp_msg_v01*)response_pdu, event_info, cmd_node);
              ENCODE_RESPONSE_TLV(wms_send_from_mem_store_resp_msg_v01, QMI_WMS_SEND_FROM_MEM_STORE_RESP_V01);
            }
          }  
          goto skip_reset;           
        }
        else
        {
          MSG_HIGH_0("NULL command node pointer");
        }
      }
      /* MO SMS sent by other WMS clients. Report call control info to App */
      else
      {
        subscription = qmi_wms_get_subscription_from_asid(event_info->submit_report_info.as_id);
        if (WMSI_SUBSCRIPTION_MAX == subscription)
        {
          return;
        }

        qmi_wmsi_generate_call_control_ind(event_info, subscription);
      }

      break;

    /*-------------------------------------------------------------------------
      MT indicaitons are special because they are sent to all clients
      registered to recieve them, handle this seperately
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
    case WMS_MSG_EVENT_STATUS_REPORT:
      subscription = qmi_wms_get_subscription_from_asid(event_info->mt_message_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      qmi_wmsi_process_mt_msg(msg_evt_ptr->event, event_info, subscription);
      return;

    /*-------------------------------------------------------------------------
      message templates are used to retrieve the SMSC address
    -------------------------------------------------------------------------*/
    case WMS_MSG_EVENT_READ_TEMPLATE:
      subscription = qmi_wms_get_subscription_from_asid(event_info->status_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      cmd_buf_p = (qmi_cmd_buf_type *) event_info->status_info.user_data;

      if (wms_sp == qmi_wmsi_get_wms_sp_by_cmd_buf(cmd_buf_p))
      {
        if (WMSI_CMD_VAL_GET_SMSC_ADDR == cmd_buf_p->cmd_type)
        {
          response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_smsc_address_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          }           
          errval = qmi_wmsi_get_smsc_addr_resp((wms_get_smsc_address_resp_msg_v01*)response_pdu, event_info,
                                               (uint8) subscription);
          ENCODE_RESPONSE_TLV(wms_get_smsc_address_resp_msg_v01, QMI_WMS_GET_SMSC_ADDRESS_RESP_V01);
        }
        else if (WMSI_CMD_VAL_GET_SMS_PARAMETERS == cmd_buf_p->cmd_type)
        {
          response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_sms_parameters_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          } 
          errval       = qmi_wmsi_get_sms_parameters_resp((wms_get_sms_parameters_resp_msg_v01*)response_pdu, event_info);
          ENCODE_RESPONSE_TLV(wms_get_sms_parameters_resp_msg_v01, QMI_WMS_GET_SMS_PARAMETERS_RESP_V01);
        }
      }
      else
      {
        /* not a QMI event, ignore it*/
        return;
      }
      break;

      /*-------------------------------------------------------------------------
        message templates writes are used to trigger SMSC address IND's
        -------------------------------------------------------------------------*/
     case WMS_MSG_EVENT_SMSC_ADDR_CHANGE:
      subscription = qmi_wms_get_subscription_from_asid(event_info->status_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      /* read the template to update the SMSC address */
      qmi_wmsi_read_params_from_template(event_info, (uint8) subscription);
      return;

    case WMS_MSG_EVENT_ACK_REPORT:
      tid = (qmi_wms_tid_type) event_info->ack_report_info.user_data;
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_tid,
                                               &tid, FALSE);  // dequeue
      if (NULL != cmd_node && NULL != cmd_node->wms_cl_sp)
      {
        wms_cl_sp = cmd_node->wms_cl_sp;
        cmd_buf_p = cmd_node->cmd_buf;

        if (cmd_node->async_cmd)
        {
          ind_pdu =  qmi_wmsi_mem_alloc( sizeof(wms_async_send_ack_ind_msg_v01) );
          if (NULL == ind_pdu)
          {
            return;
          } 
    errval  = qmi_wmsi_async_ack_ind((wms_async_send_ack_ind_msg_v01*)ind_pdu, event_info, cmd_node);
         ((wms_async_send_ack_ind_msg_v01*)ind_pdu)->ack_status =	(qmi_error_type_v01)errval;
          ENCODE_INDICATION_TLV(wms_async_send_ack_ind_msg_v01, QMI_WMS_ASYNC_SEND_ACK_IND_V01);          
        }
        else
        {
           response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_send_ack_resp_msg_v01) );
           if (NULL == response_pdu)
           {
             return;
           } 
          errval = qmi_wmsi_ack_resp((wms_send_ack_resp_msg_v01*)response_pdu, event_info, cmd_node);
          ENCODE_RESPONSE_TLV(wms_send_ack_resp_msg_v01, QMI_WMS_SEND_ACK_RESP_V01);          
        }
        goto skip_reset;
      }
      else
      {
        /* not a QMI event, ignore it*/
        return;
      }
      break;

    case WMS_MSG_EVENT_TRANSPORT_REG:
      if (qmi_wmsi_check_transport_layer_cache(&event_info->transport_reg_info))
      {
        qmi_wmsi_update_transport_layer_cache(&event_info->transport_reg_info);

        qmi_wmsi_generate_transport_layer_info_ind();
      }
      break;

    case WMS_MSG_EVENT_ETWS_NOTIFICATION:
      subscription = qmi_wms_get_subscription_from_asid(event_info->etws_msg_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      qmi_wmsi_process_mt_msg(msg_evt_ptr->event, event_info, subscription);
      break;

    case WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION:
      subscription = qmi_wms_get_subscription_from_asid(event_info->transport_mwi_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      qmi_wmsi_generate_transport_layer_mwi_ind(event_info, subscription);
      break;

    default:
      /* Unknown message event found */
      goto clean_up;
  }

  if ((NULL == cmd_buf_p) || (wms_sp != qmi_wmsi_get_wms_sp_by_cmd_buf(cmd_buf_p)))
  {
    /* not a QMI user-generated event so do not send a response */
    goto clean_up;
  }

skip_reset:
  if (NULL != cmd_node)
  {
     async_cmd = cmd_node->async_cmd;
  }

  if (async_cmd)
  {
    MSG_HIGH_0("Send indication");

    if (NULL == ind || !qmi_wmsi_send_ind(wms_cl_sp,
                                          cmd_node->cmd_type,  // one shot indication and request has the same cmd type
                                          ind))
    {
      MSG_HIGH_0("Unable to send ind");
      ind = NULL;
      goto clean_up;
    }
  }
  else
  {
    cmd_buf_must_be_freed = FALSE;  // cmd_buf gets freed in qmi_wmsi_send_response, no need to free it in cleanup.

    if (NULL == response || FALSE == qmi_wmsi_send_response((void *)cmd_buf_p,
                                                             response))
    {
      MSG_HIGH_0("Unable to send response");
      response = NULL;
      goto clean_up;
    }
  }

  /* Free the command node */
  if (NULL != cmd_node)
  {
    qmi_wmsi_free_cmd_node(cmd_node);
    cmd_node = NULL;
  }

  QMI_WMSI_MEM_FREEIF(response_pdu);
  QMI_WMSI_MEM_FREEIF(ind_pdu);
  return;

  clean_up:

  QMI_WMSI_MEM_FREEIF(response_pdu);
  QMI_WMSI_MEM_FREEIF(ind_pdu);
  
  if (NULL != response)
  {
    dsm_free_packet(&response);
  }
  if (NULL != ind)
  {
    dsm_free_packet(&ind);
  }

  if (NULL != cmd_node)
  {
    qmi_wmsi_free_cmd_node(cmd_node);
  }

  if ((NULL != cmd_buf_p) && (cmd_buf_must_be_freed))
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
  }
} /* qmi_wms_process_msg_evt() */

/*===========================================================================
  FUNCTION QMI_WMS_CFG_EVT_CB()

  DESCRIPTION
    Callback function called by WMS when QMI invokes configuration commands

  PARAMETERS
    msg_event      : event
    msg_event_info : event info struct

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_cfg_evt_cb(
  wms_cfg_event_e_type event,
  wms_cfg_event_info_s_type *info_ptr
)
{
  qmi_wms_task_buffer_s_type * cfg_evt_ptr;
  wms_client_type_e_type       client_id = WMS_CLIENT_TYPE_INTERNAL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check for NULL wms message event info pointer */
  if (NULL == info_ptr || WMS_CFG_EVENT_MAX <= event)
  {
    MSG_ERROR_1("Null info_ptr or Invalid event (%d)", event);
    return;
  }

  switch(event)
  {
    case WMS_CFG_EVENT_ROUTES:
      client_id = info_ptr->routes.client_id;
      break;

    case WMS_CFG_EVENT_MESSAGE_LIST:
      client_id = info_ptr->message_list.client_id;
      break;

    case WMS_CFG_EVENT_MEMORY_STATUS:
      client_id = info_ptr->memory_status.client_id;
      break;

    default:
      break;	  
  }

  if ((WMS_CLIENT_TYPE_QMI != client_id) && (WMS_CLIENT_TYPE_INTERNAL != client_id))
  {
    /* This event is for another client. Ignore it. */
    return;
  }

  MSG_HIGH_1("Config cb found event (%d)", event);
  switch (event)
  {
    case WMS_CFG_EVENT_ROUTES:
    case WMS_CFG_EVENT_MESSAGE_LIST:
    case WMS_CFG_EVENT_MEMORY_STATUS:
    case WMS_CFG_EVENT_MS_GWL_DOMAIN_PREF:
    case WMS_CFG_EVENT_MS_READY:
    case WMS_CFG_EVENT_MS_MEMORY_FULL:
    case WMS_CFG_EVENT_PRIMARY_CLIENT_SET:
    case WMS_CFG_EVENT_MSG_WAITING:
    case WMS_CFG_EVENT_MS_NOT_READY:
    case WMS_CFG_EVENT_MS_SIM_INIT_START:
    case WMS_CFG_EVENT_MS_SIM_INIT_END:
    case WMS_CFG_EVENT_MS_REFRESH_START:
    case WMS_CFG_EVENT_MS_REFRESH_END:
    case WMS_CFG_EVENT_MS_SERV_AVAIL:
    case WMS_CFG_EVENT_MS_SERV_NOT_AVAIL:
      /*-----------------------------------------------------------------------
        Post a cmd to QMI_WMS to handle this in QMI_WMS context
      -----------------------------------------------------------------------*/
      cfg_evt_ptr = qmi_wms_get_cmd_buf();

      if (NULL != cfg_evt_ptr)
      {
        cfg_evt_ptr->task = WMS_TASK_CMD_CFG_EVENT;
        cfg_evt_ptr->data.cfg_event.event = event;
        (void)memscpy(&cfg_evt_ptr->data.cfg_event.event_info,
                      sizeof(wms_cfg_event_info_s_type),
                      info_ptr,
                      sizeof(wms_cfg_event_info_s_type));

        qmi_wms_put_cmd_buf(cfg_evt_ptr);
      }
      break;

    default:
      break;
  }

  return;
} /* qmi_wms_cfg_evt_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_CFG_EVT()

  DESCRIPTION
    Process configuration events from the WMS service.

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_cfg_evt(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  dsm_item_type               *response              = NULL;
  qmi_cmd_buf_type            *cmd_buf_p             = NULL;
  qmi_error_e_type             errval                = QMI_ERR_NONE;
  qmi_result_e_type            result;
  qmi_wms_cmd_node_type       *cmd_node              = NULL;
  qmi_wmsi_subscription_e_type subscription;
  qmi_wms_cfg_event_type      *cfg_evt_ptr           = NULL;
  wms_cfg_event_info_s_type   *event_info            = NULL;
  boolean                      cmd_buf_must_be_freed = TRUE;
  void                        *response_pdu          = NULL;


  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  cfg_evt_ptr = &user_data_ptr->data.cfg_event;
  event_info = &cfg_evt_ptr->event_info;

  MSG_HIGH_1("Processing cfg event (%d)", cfg_evt_ptr->event);
  switch (cfg_evt_ptr->event)
  {
    case WMS_CFG_EVENT_MESSAGE_LIST:
      {
        subscription = qmi_wms_get_subscription_from_asid(event_info->message_list.as_id);
        if (WMSI_SUBSCRIPTION_MAX == subscription)
        {
          return;
        }
  
        cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_list,
                                                (void *) event_info, FALSE);
        if (! cmd_node)
        {
          return;
        }
        cmd_buf_p = cmd_node->cmd_buf;

        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_list_messages_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        errval = qmi_wmsi_get_list_resp((wms_list_messages_resp_msg_v01*)response_pdu, event_info);
        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
        ENCODE_RESPONSE_TLV(wms_list_messages_resp_msg_v01, QMI_WMS_LIST_MESSAGES_RESP_V01);
      }
      break;

    case WMS_CFG_EVENT_ROUTES:
      { 
        cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_routes,
                                                 (void *) event_info, FALSE);
        if (! cmd_node)
        {
          return;
        }
        cmd_buf_p = cmd_node->cmd_buf;

        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_routes_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        errval = qmi_wmsi_get_routes_resp((wms_get_routes_resp_msg_v01*)response_pdu, event_info);
        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;

        ENCODE_RESPONSE_TLV(wms_get_routes_resp_msg_v01, QMI_WMS_GET_ROUTES_RESP_V01);
      }
      break;

    case WMS_CFG_EVENT_MEMORY_STATUS:
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_store_max_size,
                                               (void *) event_info, FALSE);
      if (!cmd_node)
      {
        return;
      }

      {
        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_store_max_size_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        cmd_buf_p    = cmd_node->cmd_buf;
        errval       = qmi_wmsi_get_store_max_size_resp((wms_get_store_max_size_resp_msg_v01*)response_pdu, event_info);

        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
        ENCODE_RESPONSE_TLV(wms_get_store_max_size_resp_msg_v01, QMI_WMS_GET_STORE_MAX_SIZE_RESP_V01);
      }
      break;

    case WMS_CFG_EVENT_MS_GWL_DOMAIN_PREF:
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_domain_pref,
                                              (void *) event_info, FALSE);
      if (!cmd_node)
      {
        return;
      }

      {
        cmd_buf_p = cmd_node->cmd_buf;

        if (WMSI_CMD_VAL_GET_DOMAIN_PREF_CONFIG == cmd_buf_p->cmd_type)
        {
          response_pdu =  qmi_wmsi_mem_alloc( sizeof(wms_get_domain_pref_config_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          } 
          errval       = qmi_wmsi_get_domain_pref_config_resp((wms_get_domain_pref_config_resp_msg_v01*)response_pdu, event_info, cmd_node);
 
          ENCODE_RESPONSE_TLV(wms_get_domain_pref_config_resp_msg_v01, QMI_WMS_GET_DOMAIN_PREF_CONFIG_RESP_V01);
        }
        else
        {
          response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_domain_pref_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          } 
          errval       = qmi_wmsi_get_domain_pref_resp((wms_get_domain_pref_resp_msg_v01*)response_pdu, event_info, cmd_node);

          ENCODE_RESPONSE_TLV(wms_get_domain_pref_resp_msg_v01, QMI_WMS_GET_DOMAIN_PREF_RESP_V01);
        }
        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
      }
      break;

    case WMS_CFG_EVENT_MS_MEMORY_FULL:
      subscription = qmi_wms_get_subscription_from_asid(event_info->ms_memory_full.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }

      qmi_wmsi_generate_mem_full_ind(event_info, subscription);
      return;

    case WMS_CFG_EVENT_MS_READY:
      {
        boolean status_changed = TRUE;
        uint8 sub;

        sub = qmi_wms_get_subscription_from_asid(event_info->ms_ready.as_id);

        if (WMSI_SUBSCRIPTION_MAX == sub)
        {
          return;
        }

        MSG_HIGH_3( "Got WMS_CFG_EVENT_MS_READY for as_id = %d with system mode = %d, Current Ready Status = %d",
                    sub,
                    event_info->ms_ready.sys_type,
                    qmi_wmsi_global.service_ready_status[sub].status );
        if (WMS_CFG_SYS_GW == event_info->ms_ready.sys_type)
        {
          if (WMSI_SERVICE_READY_STATUS_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2;
          }
          else if (WMSI_SERVICE_READY_STATUS_NONE == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP;
          }
          /* No change if already 3GPP or 3GPP_and_3GPP2 ready */
          else
          {
            status_changed = FALSE;
          }
        }

        if (WMS_CFG_SYS_CDMA == event_info->ms_ready.sys_type)
        {
          if (WMSI_SERVICE_READY_STATUS_3GPP == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2;
          }
          else if (WMSI_SERVICE_READY_STATUS_NONE == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP2;
          }
          /* No change if already 3GPP2 or 3GPP_and_3GPP2 ready */
          else
          {
            status_changed = FALSE;
          }
        }

        if (status_changed)
        {
          MSG_HIGH_2( "Updated Ready Status = %d for as_id = %d",
                      qmi_wmsi_global.service_ready_status[sub].status,
                      sub );
          qmi_wmsi_generate_ready_ind(sub, FALSE);
        }

        // Read Channel Preference.
        {
          wms_client_ch_pref_e_type channel_pref;
		
          if (WMS_CLIENT_ERR_NONE != wms_client_get_cdma_channel_pref(qmi_wmsi_global.wms_cid,
                                                                      &channel_pref))
          {
            MSG_HIGH_0("Unable to get the default channel preference");
            return;
          }

          qmi_wmsi_global.default_ch_pref_from_nv = channel_pref;
          qmi_wmsi_global.last_used_ch_pref 	  = channel_pref;
        }

        // Read Service Option Preference.
        {
          wms_dc_so_e_type service_opt;
		
          if (WMS_CLIENT_ERR_NONE != wms_client_get_cdma_service_option_pref(qmi_wmsi_global.wms_cid,
                                                                             &service_opt))
          {
            MSG_HIGH_0("Unable to get the default service option preference");
            return;
          }

          qmi_wmsi_global.default_service_option = service_opt;
          qmi_wmsi_global.last_used_so	         = service_opt;
        }
      }

      return;

    case WMS_CFG_EVENT_MSG_WAITING:
      subscription = qmi_wms_get_subscription_from_asid(event_info->msg_wait_info.as_id);
      if (WMSI_SUBSCRIPTION_MAX == subscription)
      {
        return;
      }
      qmi_wmsi_global.mwi[subscription].mwi_state = qmi_wmsi_from_wms_state(event_info->msg_wait_info.state);

      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_message_waiting,
                                              (void *) event_info, FALSE);
      if (WMSI_MWI_STATE_ERROR == qmi_wmsi_global.mwi[subscription].mwi_state)
      {
        MSG_ERROR_0("QMI_WMS: Error in MWI READ");
        errval = QMI_ERR_INTERNAL;
      }
      else
      {
        /* Got Success response(MWI cache is in INIT state) Check if the file has changed. */
        if (qmi_wmsi_compare_mwi_cache_to_sim(event_info->msg_wait_info.sim_mwi_data, (uint8)subscription))
        {
          /* Update the MWI cache */

          MSG_HIGH_0("Updating the QMI_WMS MWI cache");
          qmi_wmsi_update_mwi_cache_from_sim(event_info->msg_wait_info.sim_mwi_data, (uint8)subscription);
          if (!cmd_node)
          {
             /* Generate the Message Waiting indication only if there is no request from client
                otherwise send the Response */
            qmi_wmsi_generate_msg_wait_ind((qmi_wmsi_subscription_e_type) subscription);
            return;
          }
        }
      }

      if (cmd_node)
      {
        /* Send the Response to the client if it was a client's request regardless of whether
           cache content is same or different than EF_MWI content */
        cmd_buf_p = cmd_node->cmd_buf;
        if (QMI_ERR_INTERNAL != errval)
        {
          response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_message_waiting_resp_msg_v01) );
          if (NULL == response_pdu)
          {
            return;
          } 
          //Fill in the response with MWI data only when there was no error
          errval = qmi_wmsi_get_message_waiting_resp((wms_get_message_waiting_resp_msg_v01*)response_pdu, (uint8)subscription);         
          ENCODE_RESPONSE_TLV(wms_get_message_waiting_resp_msg_v01, QMI_WMS_GET_MESSAGE_WAITING_RESP_V01);
         }
        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
      }
      else
      {
        return;
      }
      break;

    case WMS_CFG_EVENT_MS_NOT_READY:
      {
        boolean status_changed = TRUE;
        uint8 sub;

        sub = qmi_wms_get_subscription_from_asid(event_info->ms_not_ready.as_id);

        if (WMSI_SUBSCRIPTION_MAX == sub)
        {
          return;
        }

        MSG_HIGH_3( "Got WMS_CFG_EVENT_MS_NOT_READY for as_id = %d with system mode = %d, Current Ready Status = %d",
                    sub,
                    event_info->ms_not_ready.sys_type,
                    qmi_wmsi_global.service_ready_status[sub].status );
        if (WMS_CFG_SYS_GW == event_info->ms_not_ready.sys_type)
        {
          if (WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP2;
          }
          else if (WMSI_SERVICE_READY_STATUS_3GPP == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_NONE;
          }
          /* Current ready status is 3GPP2 or NONE. No need to update the status. */
          else
          {
            status_changed = FALSE;
          }
        }

        if (WMS_CFG_SYS_CDMA == event_info->ms_not_ready.sys_type)
        {
          if (WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP;
          }
          else if (WMSI_SERVICE_READY_STATUS_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
          {
            qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_NONE;
          }
          /* Current ready status is 3GPP or NONE. No need to update the status. */
          else
          {
            status_changed = FALSE;
          }
        }

        if (status_changed)
        {
          MSG_HIGH_2( "Updated Ready Status = %d for as_id = %d",
                      qmi_wmsi_global.service_ready_status[sub].status,
                      sub );
          qmi_wmsi_generate_ready_ind(sub, FALSE);
        }
      }

      return;

    case WMS_CFG_EVENT_MS_SIM_INIT_START:
      {
        uint8 sub;
        sub = qmi_wms_get_subscription_from_asid(event_info->begin_sim_init.as_id);

        MSG_HIGH_3( "is_inited = %d, as_id = %d, sys_type = %d",
                    event_info->begin_sim_init.is_inited,
                    sub,
                    event_info->begin_sim_init.sys_type );
      }
      return;

    case WMS_CFG_EVENT_MS_SIM_INIT_END:
      {
        uint8 sub;
        boolean status_changed;

        sub = qmi_wms_get_subscription_from_asid(event_info->end_sim_init.as_id);

        if (WMSI_SUBSCRIPTION_MAX == sub)
        {
          return;
        }

        MSG_HIGH_4( "Got WMS_CFG_EVENT_MS_SIM_INIT_END for as_id = %d with system mode = %d, is_inited = %d, "
                    "SIM Current Ready Status = %d",
                     sub,
                     event_info->end_sim_init.sys_type,
                     event_info->end_sim_init.is_inited,
                     qmi_wmsi_global.sim_ready_status[sub].sim_ready_status );

        status_changed = qmi_wmsi_check_and_update_sim_status(sub, event_info->end_sim_init.sys_type, 
                                                              event_info->end_sim_init.is_inited);

        if (status_changed)
        {
          boolean update_ready_status = TRUE;

          MSG_HIGH_2( "Updated SIM Ready Status = %d for as_id = %d",
                      qmi_wmsi_global.sim_ready_status[sub].sim_ready_status,
                      sub );

          if (FALSE == event_info->end_sim_init.is_inited) 
          {
            if (WMS_CFG_SYS_GW == event_info->end_sim_init.sys_type)
            {
              if (WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
              {
                qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP2;
              }
              else if (WMSI_SERVICE_READY_STATUS_3GPP == qmi_wmsi_global.service_ready_status[sub].status)
              {
                qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_NONE;
              }
              /* Current ready status is 3GPP2 or NONE. No need to update the status. */
              else
              {
                update_ready_status = FALSE;
              }
            }

            if (WMS_CFG_SYS_CDMA == event_info->end_sim_init.sys_type)
            {
              if (WMSI_SERVICE_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
              {
                qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_3GPP;
              }
              else if (WMSI_SERVICE_READY_STATUS_3GPP2 == qmi_wmsi_global.service_ready_status[sub].status)
              {
                qmi_wmsi_global.service_ready_status[sub].status = WMSI_SERVICE_READY_STATUS_NONE;
              }
              /* Current ready status is 3GPP or NONE. No need to update the status. */
              else
              {
                update_ready_status = FALSE;
              }
            }

            /* This will update both service ready status and sim ready status for registered clients. */
            if (update_ready_status)
            {
              MSG_HIGH_2( "Updated Ready Status = %d for as_id = %d",
                           qmi_wmsi_global.service_ready_status[sub].status,
                           sub );
              qmi_wmsi_generate_ready_ind(sub, FALSE);
            }
            else
            {
              /* Update the SIM ready status for registered clients. */
              qmi_wmsi_generate_ready_ind(sub, TRUE);
            }
          }
          else
          {
            /* Update the SIM ready status for registered clients. */
            qmi_wmsi_generate_ready_ind(sub, TRUE);
          }
        }
        else
        {
          MSG_HIGH_0("No change in the SIM Ready Status");
        }
      }

      return;

    case WMS_CFG_EVENT_MS_REFRESH_START:
      {
        uint8 sub;
        boolean status_changed;

        sub = qmi_wms_get_subscription_from_asid(event_info->begin_refresh.as_id);

        if (WMSI_SUBSCRIPTION_MAX == sub)
        {
          return;
        }

        MSG_HIGH_3( "Got WMS_CFG_EVENT_MS_REFRESH_START for as_id = %d with system mode = %d, SIM Current Ready Status = %d",
                    sub,
                    event_info->begin_refresh.sys_type,
                    qmi_wmsi_global.sim_ready_status[sub].sim_ready_status );

        status_changed = qmi_wmsi_check_and_update_sim_status(sub, event_info->begin_refresh.sys_type, FALSE);
        
        if (status_changed)
        {
          MSG_HIGH_2( "Updated SIM Ready Status = %d for as_id = %d",
                      qmi_wmsi_global.sim_ready_status[sub].sim_ready_status,
                      sub );

          qmi_wmsi_generate_ready_ind(sub, TRUE);
        }
        else
        {
          MSG_HIGH_0("No change in the SIM Ready Status");
        }
      }
      return;

    case WMS_CFG_EVENT_MS_REFRESH_END:
      {
        uint8 sub;
        sub = qmi_wms_get_subscription_from_asid(event_info->end_refresh.as_id);

        MSG_HIGH_3( "Refresh end success = %d, as_id = %d, sys_type = %d",
                    event_info->end_refresh.is_success,
                    sub,
                    event_info->end_refresh.sys_type );
      }
      return;

    case WMS_CFG_EVENT_MS_SERV_AVAIL:
    case WMS_CFG_EVENT_MS_SERV_NOT_AVAIL:
      return;

    default:
      MSG_HIGH_0("Unknown cfg event found");
      goto clean_up;
  }

  if (QMI_ERR_NONE != errval)
  {
    MSG_HIGH_1("Processing cfg event failed (error %d) resetting response", errval);
    dsm_free_packet(&response);
    response = NULL;
  }
  
  cmd_buf_must_be_freed = FALSE;  // cmd_buf gets freed in qmi_wmsi_send_response, no need to free it in clean up.
  
  if (NULL == response || FALSE == qmi_wmsi_send_response((void *)cmd_buf_p,
                                                           response))
  {
    MSG_HIGH_0("Unable to send WMS CFG EVT response");
    response = NULL;
    goto clean_up;
  }

 QMI_WMSI_MEM_FREEIF(response_pdu);
 return;

clean_up:
	
  QMI_WMSI_MEM_FREEIF(response_pdu);

  if (NULL != response)
  {
    dsm_free_packet(&response);
  }

  if ((NULL != cmd_buf_p) && (cmd_buf_must_be_freed))
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
  }
} /* qmi_wms_process_cfg_evt() */

/*===========================================================================
  FUNCTION QMI_WMS_DC_EVT_CB()

  DESCRIPTION
    Process DC callbacks from the WMS service.

  PARAMETERS
    dc_event      : event
    wms_dc_event_info_s_type    *info_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_dc_evt_cb(
  wms_dc_event_e_type        event,
  wms_dc_event_info_s_type * info_ptr
)
{
  qmi_wms_task_buffer_s_type * dc_evt_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (event)
  {
    case WMS_DC_EVENT_CONNECTING:
    case WMS_DC_EVENT_CONNECTED:
    case WMS_DC_EVENT_ABORTED:
    case WMS_DC_EVENT_DISCONNECTED:
      /*-----------------------------------------------------------------------
        Post a cmd to QMI_WMS to handle this in QMI_WMS context
      -----------------------------------------------------------------------*/
      dc_evt_ptr = qmi_wms_get_cmd_buf();

      if (NULL != dc_evt_ptr)
      {
        dc_evt_ptr->task = WMS_TASK_CMD_DC_EVENT;
        dc_evt_ptr->data.dc_event.event = event;

        qmi_wms_put_cmd_buf(dc_evt_ptr);
      }
    break;
  default:
    MSG_HIGH_1("Unhandled dc event (%d)", event);
    break;
  }
} /* qmi_wms_dc_evt_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_DC_EVT()

  DESCRIPTION
    Process DC events from the WMS service.

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_dc_evt(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  qmi_wms_dc_event_type * dc_evt_ptr = NULL;

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  dc_evt_ptr = &user_data_ptr->data.dc_event;

  /* Generate the Call Status Indication */
  qmi_wmsi_generate_call_status_ind(dc_evt_ptr->event);

} /* qmi_wms_process_dc_evt */

/*===========================================================================
  FUNCTION QMI_WMS_BC_MM_EVT_CB()

  DESCRIPTION
    Process Broadcast callbacks from the WMS service.

  PARAMETERS
    wms_bc_mm_event_e_type      : event
    wms_bc_mm_event_info_s_type    *info_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_bc_mm_evt_cb(
  wms_bc_mm_event_e_type         event,
  wms_bc_mm_event_info_s_type    *info_ptr
)
{
  wms_bc_mm_service_info_s_type * bc_mm_table_ptr = NULL;
  wms_bc_mm_srv_id_s_type * bc_srv_ids_ptr = NULL;
  qmi_wms_task_buffer_s_type *    bc_mm_evt_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (event)
  {
    case WMS_BC_MM_EVENT_TABLE:
    case WMS_BC_MM_EVENT_SCPT_ADD_SRVS:
      /* Handle this event separately.  WMS passes a pointer to the BC MM table,
         and QMI WMS needs to allocate memory for the table and update the
         pointer so that it points to QMI WMS memory. */

      /* Allocate memory for the BC MM table */
      if (info_ptr->u.bc_mm_table.size > 0)
      {
        bc_mm_table_ptr = (wms_bc_mm_service_info_s_type *) qmi_wmsi_mem_alloc(info_ptr->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));

        if (NULL == bc_mm_table_ptr)
        {
          return;
        }
      }
      else
      {
        bc_mm_table_ptr = NULL;
      }

      bc_mm_evt_ptr = qmi_wms_get_cmd_buf();

      if (NULL != bc_mm_evt_ptr)
      {
        bc_mm_evt_ptr->task                                           = WMS_TASK_CMD_BC_MM_EVENT;
        bc_mm_evt_ptr->data.bc_mm_event.event                         = event;
        bc_mm_evt_ptr->data.bc_mm_event.event_info.event              = info_ptr->event;
        bc_mm_evt_ptr->data.bc_mm_event.event_info.message_mode       = info_ptr->message_mode;
        bc_mm_evt_ptr->data.bc_mm_event.event_info.user_data          = info_ptr->user_data;
        bc_mm_evt_ptr->data.bc_mm_event.event_info.u.bc_mm_table.size = info_ptr->u.bc_mm_table.size;
        bc_mm_evt_ptr->data.bc_mm_event.event_info.as_id              = info_ptr->as_id;

        if (NULL != bc_mm_table_ptr)
        {
          /* Copy the table to the new location */
          (void)memscpy(bc_mm_table_ptr,
                        info_ptr->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type),
                        info_ptr->u.bc_mm_table.entries,
                        info_ptr->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));
        }

        /* Update the pointer to point to QMI WMS memory */
        bc_mm_evt_ptr->data.bc_mm_event.event_info.u.bc_mm_table.entries =bc_mm_table_ptr;

        qmi_wms_put_cmd_buf(bc_mm_evt_ptr);
      }
      break;

    case WMS_BC_MM_EVENT_SRV_IDS:
    case WMS_BC_MM_EVENT_SCPT_DELETE_SRVS:
    case WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS:
    /* Handle this event separately.  WMS passes a pointer to the BC srv ids.
       QMI WMS needs to allocate memory and update the pointer so that it points
       to QMI WMS memory. */

    /* Allocate memory for the BC srv ids */
    if (info_ptr->u.bc_mm_srv_ids.size > 0)
    {
      bc_srv_ids_ptr = (wms_bc_mm_srv_id_s_type *) qmi_wmsi_mem_alloc(info_ptr->u.bc_mm_srv_ids.size * sizeof(wms_bc_mm_srv_id_s_type));

      if (NULL == bc_srv_ids_ptr)
      {
        return;
      }
    }
    else
    {
      bc_srv_ids_ptr = NULL;
    }

    bc_mm_evt_ptr = qmi_wms_get_cmd_buf();

    if (NULL != bc_mm_evt_ptr)
    {
      bc_mm_evt_ptr->task                                             = WMS_TASK_CMD_BC_MM_EVENT;
      bc_mm_evt_ptr->data.bc_mm_event.event                           = event;
      bc_mm_evt_ptr->data.bc_mm_event.event_info.event                = info_ptr->event;
      bc_mm_evt_ptr->data.bc_mm_event.event_info.message_mode         = info_ptr->message_mode;
      bc_mm_evt_ptr->data.bc_mm_event.event_info.user_data            = info_ptr->user_data;
      bc_mm_evt_ptr->data.bc_mm_event.event_info.u.bc_mm_srv_ids.size = info_ptr->u.bc_mm_srv_ids.size;
      bc_mm_evt_ptr->data.bc_mm_event.event_info.as_id                = info_ptr->as_id;

      if (NULL != bc_srv_ids_ptr)
      {
        /* Copy the table to the new location */
        (void)memscpy(bc_srv_ids_ptr,
                      info_ptr->u.bc_mm_srv_ids.size * sizeof(wms_bc_mm_srv_id_s_type),
                      info_ptr->u.bc_mm_srv_ids.entries,
                      info_ptr->u.bc_mm_srv_ids.size * sizeof(wms_bc_mm_srv_id_s_type));
      }

      /* Update the pointer to point to QMI WMS memory */
      bc_mm_evt_ptr->data.bc_mm_event.event_info.u.bc_mm_srv_ids.entries = bc_srv_ids_ptr;

      qmi_wms_put_cmd_buf(bc_mm_evt_ptr);
    }
    break;

    case WMS_BC_MM_EVENT_PREF:
      bc_mm_evt_ptr = qmi_wms_get_cmd_buf();

      if (NULL != bc_mm_evt_ptr)
      {
        bc_mm_evt_ptr->task = WMS_TASK_CMD_BC_MM_EVENT;
        bc_mm_evt_ptr->data.bc_mm_event.event = event;
        (void)memscpy(&bc_mm_evt_ptr->data.bc_mm_event.event_info,
                      sizeof(wms_bc_mm_event_info_s_type),
                      info_ptr,
                      sizeof(wms_bc_mm_event_info_s_type));

        qmi_wms_put_cmd_buf(bc_mm_evt_ptr);
      }
      break;

    case WMS_BC_MM_EVENT_ADD_SRVS:
    case WMS_BC_MM_EVENT_DELETE_SRVS:
      /*-------------------------------------------------------------------------
        These events have already been handled by the command status callback,
        ignore them
      -------------------------------------------------------------------------*/
      break;

    default:
      MSG_HIGH_1("Unexpected bc mm event (%d)", event);
      break;
  }
} /* qmi_wms_bc_mm_evt_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_BC_MM_EVT()

  DESCRIPTION
    Process Broadcast events from the WMS service

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_bc_mm_evt(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  dsm_item_type                *response              = NULL;
  void                         *response_pdu          = NULL;
  qmi_cmd_buf_type             *cmd_buf_p             = NULL;
  qmi_error_e_type              errval                = QMI_ERR_NONE;
  qmi_result_e_type             result;
  qmi_wms_cmd_node_type        *cmd_node              = NULL;
  wms_bc_mm_event_info_s_type  *event_info            = NULL;
  qmi_wms_bc_mm_event_type     *bc_mm_evt_ptr         = NULL;
  boolean                       cmd_buf_must_be_freed = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  bc_mm_evt_ptr = &user_data_ptr->data.bc_mm_event;
  event_info    = &bc_mm_evt_ptr->event_info;
  cmd_buf_p     =  bc_mm_evt_ptr->event_info.user_data;

  MSG_HIGH_1("Processing bc mm event (%d)", bc_mm_evt_ptr->event);
  switch (bc_mm_evt_ptr->event)
  {
    case WMS_BC_MM_EVENT_PREF:
      /*-------------------------------------------------------------------------
        Update the cache with the bc preference.  Response will be sent when
        WMS_BC_MM_EVENT_TABLE is received.
      -------------------------------------------------------------------------*/
      cmd_node = qmi_wmsi_get_pending_cmd_buf( (q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                cmd_buf_p, TRUE );
      if (! cmd_node)
      {
        return;
      }

      cmd_node->cache.get_bc_config.bc_pref = qmi_wms_to_wmsi_bc_pref(event_info->u.bc_pref);
      // Ignoring WMS_BC_MM_EVENT_PREF until WMS_BC_MM_EVENT_TABLE received
      return;

    case WMS_BC_MM_EVENT_TABLE:
      cmd_node = qmi_wmsi_get_pending_cmd_buf( (q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                cmd_buf_p, FALSE );
      if (! cmd_node)
      {
        return;
      }
      
      {    
        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_broadcast_config_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        errval       = qmi_wmsi_get_bc_config_resp((wms_get_broadcast_config_resp_msg_v01*)response_pdu, event_info, cmd_node);
        ENCODE_RESPONSE_TLV(wms_get_broadcast_config_resp_msg_v01, QMI_WMS_GET_BROADCAST_CONFIG_RESP_V01);

        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;

        /* QMI WMS allocated memory for the BC SMS table.  Deallocate that memory now. */
        if (NULL != event_info->u.bc_mm_table.entries)
        {
          qmi_wmsi_mem_free(event_info->u.bc_mm_table.entries);
        }
      }
      break;

    case WMS_BC_MM_EVENT_SRV_IDS:
      {
        boolean response_pending = FALSE;
        cmd_node = qmi_wmsi_get_pending_cmd_buf( (q_compare_func_type)qmi_wmsi_compare_cmd_buf,
                                                  cmd_buf_p, TRUE );
        if (! cmd_node)
        {
          return;
        }
        
        errval = qmi_wmsi_set_bc_config_processing(cmd_buf_p, event_info, cmd_node, &response_pending);

      /* QMI WMS allocated memory for the BC SRV IDs table.  Deallocate that memory now. */
        if (NULL != event_info->u.bc_mm_srv_ids.entries)
        {
          qmi_wmsi_mem_free(event_info->u.bc_mm_srv_ids.entries);
        }

        if (! response_pending)  //Send the response now if there is no response pending from WMS
        {
          response_pdu =  qmi_wmsi_mem_alloc( sizeof(wms_set_broadcast_config_resp_msg_v01) );
          if (NULL == response_pdu)
          { 
            return;     
          }
          q_delete(&qmi_wmsi_global.cmd_pend_q, &cmd_node->link);
          ENCODE_RESPONSE_TLV(wms_set_broadcast_config_resp_msg_v01, QMI_WMS_SET_BROADCAST_CONFIG_RESP_V01);
          qmi_wmsi_free_cmd_node(cmd_node);
          cmd_node = NULL;
          break;
        }
        else
        {
          return;
        }

      }

   case WMS_BC_MM_EVENT_SCPT_ADD_SRVS:
    qmi_wmsi_generate_broadcast_config_ind(event_info);
    /* QMI WMS allocated memory for the BC SMS table.  Deallocate that memory now. */
    if (NULL != event_info->u.bc_mm_table.entries)
    {
      qmi_wmsi_mem_free(event_info->u.bc_mm_table.entries);
    }
    return;

  case WMS_BC_MM_EVENT_SCPT_DELETE_SRVS:
  case WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS:
    qmi_wmsi_generate_broadcast_config_ind(event_info);
    /* QMI WMS allocated memory for the BC SRV IDs table.  Deallocate that memory now. */
    if (NULL != event_info->u.bc_mm_srv_ids.entries)
    {
      qmi_wmsi_mem_free(event_info->u.bc_mm_srv_ids.entries);
    }
    return;

    default:
      goto clean_up;
  }

  if (QMI_ERR_NONE != errval)
  {
    MSG_HIGH_1("Processing bc mm event failed (error %d) resetting response", errval);
    dsm_free_packet(&response);
    response = NULL;
  }

  cmd_buf_must_be_freed = FALSE;  // cmd_buf gets freed in qmi_wmsi_send_response, no need to free it in cleanup.

  if (NULL == response || FALSE == qmi_wmsi_send_response((void *)cmd_buf_p,
                                                           response))
  {
    MSG_HIGH_0("Unable to send WMS BC MM EVT response");
    response = NULL;
    goto clean_up;
  }

  QMI_WMSI_MEM_FREEIF(response_pdu);
  return;

clean_up:
  QMI_WMSI_MEM_FREEIF(response_pdu);

  if (NULL != response)
  {
    dsm_free_packet(&response);
  }

  if ((NULL != cmd_buf_p) && (cmd_buf_must_be_freed))
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
  }
} /* qmi_wms_process_bc_mm_evt */



/*===========================================================================
  FUNCTION QMI_WMS_TRANSPORT_STATUS_CB()

  DESCRIPTION
    Process Transport Status callback from the WMS service.

  PARAMETERS
    wms_msg_transport_status_s_type * status_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_transport_status_cb(
  wms_msg_transport_status_s_type * status_ptr
)
{
  qmi_wms_task_buffer_s_type * status_info_ptr = NULL;

  status_info_ptr = qmi_wms_get_cmd_buf();

  if (NULL != status_info_ptr)
  {
    status_info_ptr->task = WMS_TASK_CMD_TRANSPORT_STATUS;
    (void)memscpy(&status_info_ptr->data.transport_status.status_info,
                  sizeof(wms_msg_transport_status_s_type),
                  status_ptr,
                  sizeof(wms_msg_transport_status_s_type));

    qmi_wms_put_cmd_buf(status_info_ptr);
  }
} /* qmi_wms_transport_status_cb */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_TRANSPORT_STATUS()

  DESCRIPTION
    Process Transport Status CB from the WMS service.

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_transport_status(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  wms_msg_transport_status_s_type * status_info = NULL;
  qmi_wms_transport_status_type *   status_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    return;
  }

  status_info_ptr = &(user_data_ptr->data.transport_status);

  if (NULL == status_info_ptr)
  {
    MSG_ERROR_0("NULL status_info_ptr");
    return;
  }

  status_info = &(status_info_ptr->status_info);

  MSG_HIGH_0("Processing transport status");

  /* Compare the Transport NW Reg status to the cache */
  if ((WMSI_TRANSPORT_NW_REG_STATE_UNINIT == qmi_wmsi_global.transport_nw_reg_status.state)||
      (qmi_wmsi_global.transport_nw_reg_status.nw_reg_status !=
       qmi_wms_to_wmsi_transport_nw_reg( status_info->transport_status )) || 
      (qmi_wmsi_global.transport_nw_reg_status.as_id != status_info->as_id)
     )
  {
    /* Update the Transport NW Reg cache */
    qmi_wmsi_global.transport_nw_reg_status.state = WMSI_TRANSPORT_NW_REG_STATE_INIT;
    qmi_wmsi_global.transport_nw_reg_status.nw_reg_status =
      qmi_wms_to_wmsi_transport_nw_reg(status_info->transport_status);
    qmi_wmsi_global.transport_nw_reg_status.as_id = status_info->as_id;
    
    /* Generate the Transport NW Reg Info Indication */
    qmi_wmsi_generate_transport_nw_reg_info_ind();
  }

} /* qmi_wms_process_transport_status */

/*===========================================================================
  FUNCTION QMI_WMS_DBG_EVT_CB()

  DESCRIPTION
    Callback function called by WMS when QMI invokes debug commands

  PARAMETERS
    event      : event
    info_ptr   : event info struct

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_dbg_evt_cb(
  wms_dbg_event_e_type event,
  wms_dbg_event_info_s_type *info_ptr
)
{
  qmi_wms_task_buffer_s_type * dbg_evt_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check for NULL wms message event info pointer */
  if (NULL == info_ptr || WMS_DBG_EVENT_MAX <= event)
  {
    MSG_ERROR_1("Null info_ptr or Invalid event (%d)", event);
    return;
  }

  MSG_HIGH_1("Dbg cb found event (%d)", event);
  switch (event)
  {
    case WMS_DBG_EVENT_RETRY_INTERVAL:
    case WMS_DBG_EVENT_RETRY_PERIOD:
      dbg_evt_ptr = qmi_wms_get_cmd_buf();
      if (NULL != dbg_evt_ptr)
      {
        dbg_evt_ptr->task = WMS_TASK_CMD_DBG_EVENT;
        dbg_evt_ptr->data.dbg_event.event = event;
        (void)memscpy(&dbg_evt_ptr->data.dbg_event.event_info,
                      sizeof(wms_dbg_event_info_s_type),
                      info_ptr,
                      sizeof(wms_dbg_event_info_s_type));
        qmi_wms_put_cmd_buf(dbg_evt_ptr);
      }
      break;

    default:
      break;
  }

  return;
} /* qmi_wms_dbg_evt_cb() */

/*===========================================================================
  FUNCTION QMI_WMS_PROCESS_DBG_EVT()

  DESCRIPTION
    Process debug events from the WMS service.

  PARAMETERS
    user_data_ptr : user_data ptr passed to QMI_WMS task posting command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wms_process_dbg_evt(
  qmi_wms_task_buffer_s_type * user_data_ptr
)
{
  dsm_item_type             *response              = NULL;
  void                      *response_pdu          = NULL;
  qmi_cmd_buf_type          *cmd_buf_p             = NULL;
  qmi_error_e_type           errval                = QMI_ERR_NONE;
  qmi_result_e_type          result;
  qmi_wms_cmd_node_type     *cmd_node              = NULL;
  wms_dbg_event_info_s_type *info_ptr              = NULL;
  qmi_wms_dbg_event_type    *dbg_evt_ptr           = NULL;
  boolean                    cmd_buf_must_be_freed = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_if_valid_user_data_ptr(user_data_ptr))
  {
    MSG_ERROR_0("NULL user_data_ptr");
    return;
  }

  dbg_evt_ptr = &user_data_ptr->data.dbg_event;
  info_ptr = &dbg_evt_ptr->event_info;

  MSG_HIGH_1("Processing dbg event (%d)", dbg_evt_ptr->event);
  switch (dbg_evt_ptr->event)
  {
    case WMS_DBG_EVENT_RETRY_PERIOD:
      qmi_wmsi_global.dbg_retry_period = info_ptr->retry_period;
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_dbg_retry_period,
                                               info_ptr, FALSE);
      if (! cmd_node)
      {
        return;
      }

      {
        cmd_buf_p = cmd_node->cmd_buf;

        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_retry_period_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        errval       = qmi_wmsi_get_retry_period_resp((wms_get_retry_period_resp_msg_v01*)response_pdu, info_ptr);
        ENCODE_RESPONSE_TLV(wms_get_retry_period_resp_msg_v01, QMI_WMS_GET_RETRY_PERIOD_RESP_V01);
        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
      }
      break;

    case WMS_DBG_EVENT_RETRY_INTERVAL:
      qmi_wmsi_global.dbg_retry_interval = info_ptr->retry_interval;
      cmd_node = qmi_wmsi_get_pending_cmd_buf((q_compare_func_type)qmi_wmsi_compare_get_dbg_retry_interval,
                                               info_ptr, FALSE);
      if (! cmd_node)
      {
        return;
      }

      {  
        response_pdu = qmi_wmsi_mem_alloc( sizeof(wms_get_retry_interval_resp_msg_v01) );
        if (NULL == response_pdu)
        {
          return;
        } 
        cmd_buf_p    = cmd_node->cmd_buf;
        errval       = qmi_wmsi_get_retry_interval_resp((wms_get_retry_interval_resp_msg_v01*)response_pdu, info_ptr);

        ENCODE_RESPONSE_TLV(wms_get_retry_interval_resp_msg_v01, QMI_WMS_GET_RETRY_INTERVAL_RESP_V01);

        qmi_wmsi_free_cmd_node(cmd_node);
        cmd_node = NULL;
      }
      break;

    default:
      goto clean_up;
  }

  if (QMI_ERR_NONE != errval)
  {
    MSG_HIGH_1("Processing dbg event failed (error %d) resetting response", errval);
    dsm_free_packet(&response);
    response = NULL;
  }

  cmd_buf_must_be_freed = FALSE;  // cmd_buf gets freed in qmi_wmsi_send_response, no need to free it in cleanup.

  if (NULL == response || FALSE == qmi_wmsi_send_response((void *)cmd_buf_p,
                                                           response))
  {
    MSG_HIGH_0("Unable to send WMS DBG EVT response");
    response = NULL;
    goto clean_up;
  }
  
  QMI_WMSI_MEM_FREEIF(response_pdu);
  return;

clean_up:
  QMI_WMSI_MEM_FREEIF(response_pdu);

  if (NULL != response)
  {
    dsm_free_packet(&response);
  }

  if ((NULL != cmd_buf_p) && (cmd_buf_must_be_freed))
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
  }
} /* qmi_wms_process_dbg_evt() */

/*=========================================================================

                      INTERNAL HELPER FUNCTIONS

===========================================================================*/
static void * qmi_wmsi_mem_alloc(
  size_t size
)
{
  void * ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ptr = modem_mem_alloc(size, MODEM_MEM_CLIENT_QMI_WMS_CRIT);

  if (NULL == ptr)
  {
    MSG_ERROR_1("qmi_wmsi_mem_alloc failed, size %d", size);
  }
  else
  {
    //MSG_HIGH_2( "qmi_wmsi_mem_alloc @0x%x, size %d", ptr, size );
    memset(ptr, 0, size);
  }

  return ptr;
} /* qmi_wmsi_mem_alloc() */

static void qmi_wmsi_mem_free(
  void * ptr
)
{
  if (NULL == ptr)
  {
    MSG_ERROR_0("qmi_wmsi_mem_free passed NULL ptr");
  }
  else
  {
    //MSG_HIGH_1( "qmi_wmsi_mem_free @0x%x", ptr );
    modem_mem_free(ptr, MODEM_MEM_CLIENT_QMI_WMS_CRIT);
    ptr = NULL;
  }
}/* qmi_wmsi_mem_free() */

static qmi_wms_cmd_node_type * qmi_wmsi_get_pending_cmd_buf(
  q_compare_func_type compare_func,
  void * compare_val,
  boolean no_delete
)
{
  qmi_wms_cmd_node_type *       cmd_node_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == compare_val)
  {
    MSG_HIGH_0("Null compare value in qmi_wmsi_get_pending_cmd_buf");
    return NULL;
  }

  /*-------------------------------------------------------------------------

  -------------------------------------------------------------------------*/
    cmd_node_ptr = (qmi_wms_cmd_node_type *)
              q_linear_search(&qmi_wmsi_global.cmd_pend_q, compare_func, compare_val);

    if (NULL != cmd_node_ptr)
    {
      MSG_HIGH_1("Pending response found cmd_node_ptr  0x%x", cmd_node_ptr);
      if (!no_delete)
      {
        q_delete(&qmi_wmsi_global.cmd_pend_q, &cmd_node_ptr->link);
      }
      return cmd_node_ptr;
    }

  return NULL;
} /* qmi_wmsi_get_pending_cmd_buf() */

static int qmi_wmsi_compare_cmd_buf(
  qmi_wms_cmd_node_type * cmd_node,
  void *  cmd_buf
)
{
  int ret = (int) FALSE;

  if ((NULL != cmd_buf) && (NULL != cmd_node))
  {
    if (cmd_buf == cmd_node->cmd_buf)
    {
      ret = (int) TRUE;
    }
  }

  return ret;
} /* qmi_wmsi_compare_cmd_buf() */

/*===========================================================================
FUNCTION qmi_wmsi_validate_cmd_buf_p() 

DESCRIPTION
Function to validate returned cmd_buf_p is still allocated in client.

PARAMETERS
cmd_buf_p: pointer to queue element for comparison

RETURN VALUE
Pointer to the client state if cmdbuf valid. otherwise NULL
===========================================================================*/
qmi_wmsi_client_state_type *qmi_wmsi_validate_cmd_buf_p( 
  qmi_cmd_buf_type *cmd_buf_p
)
{
  int                  cl_idx;
  int                  cmd_idx;
  qmi_wmsi_state_type *svc_state_ptr;

  svc_state_ptr = &qmi_wms_state;

  if (NULL == cmd_buf_p)
  {
    MSG_ERROR_0("qmi_wmsi_validate_cmd_buf_p: NULL cmd_buf_p");
    return NULL;
  }

  for ( cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++ )
  {
    if ( NULL != svc_state_ptr->client[cl_idx] )
    {
      for ( cmd_idx = 0; cmd_idx < MAX_PENDING_COMMANDS; cmd_idx++ )
      {
        if ( cmd_buf_p == &(svc_state_ptr->client[cl_idx]->common.cmd_buf[cmd_idx]) )
        {
          if ( NULL != svc_state_ptr->client[cl_idx]->common.cmd_buf[cmd_idx].x_p )
          {
            return svc_state_ptr->client[cl_idx];
          }
          else
          {
            MSG_ERROR_2("Client %d cmd_buf %d has null transaction", cl_idx, cmd_idx);
            return NULL;
          }
        }
      }
    }
  }

  MSG_HIGH_0("Buffer does not match existing client transaction");
  return NULL;
}  /* qmi_wmsi_validate_cmd_buf_p() */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_CLIENT_SP_BY_CMD_BUF()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void* qmi_wmsi_get_client_sp_by_cmd_buf(
   qmi_cmd_buf_type* cmd_buf_p
)
{
  qmi_common_client_state_type *cl_sp   = NULL;
  void                         *ret_val = NULL;
  int                           i=0;

  if (!qmi_wmsi_validate_cmd_buf_p( cmd_buf_p ))
  {
    return NULL;
  }

  if (cmd_buf_p->x_p)
  {
    if (cmd_buf_p->x_p->cl_sp)
    {
      cl_sp = cmd_buf_p->x_p->cl_sp;

      for (i = 0; i < WMSI_MAX_CLIDS; i++)
      {
        if (qmi_wms_state.client[i])
        {
          if (qmi_wms_state.client[i]->common.clid == cl_sp->clid)
          {
            ret_val = (void*)qmi_wms_state.client[i];
            break;
          }
        }
      }
    }
    else
    {
      MSG_ERROR_1("qmi_wmsi_get_client_sp_by_cmd_buf(): cl_sp %d is null", cmd_buf_p->x_p->cl_sp);
    }
  }
  else
  {
    MSG_ERROR_1("qmi_wmsi_get_client_sp_by_cmd_buf(): x_p %d is null", cmd_buf_p->x_p);
  }

  return ret_val;
} /* qmi_wmsi_get_client_sp_by_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_WMSI_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    cmd_buf_p:     QMI command to send response to
    msg_ptr:       Response to QMI request

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_wmsi_send_response(
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr
)
{
  qmi_framework_common_msg_hdr_type hdr;
  qmi_wmsi_client_state_type *      cl_sp  = NULL;
  boolean                           status = FALSE;

  cl_sp = (qmi_wmsi_client_state_type*)qmi_wmsi_get_client_sp_by_cmd_buf(cmd_buf_p);

  if (cl_sp)
  {
    memset(&hdr, 0, sizeof(hdr));

    hdr.service      = (qmux_service_e_type)cl_sp->service_id;
    hdr.client_id    = cl_sp->common.clid;
    hdr.qmi_instance = cl_sp->instance;
    hdr.transaction_id = cmd_buf_p->x_p->x_id;
 
    status = ds_qmi_fw_send_response(&hdr, cmd_buf_p, msg_ptr);
	
    MSG_HIGH_2( "qmi_wmsi_send_response(): sending resp - clnt %d status %d",
                hdr.client_id,
                status );
  }
  else
  {
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    dsm_free_packet(&msg_ptr);
  }

  return status;
} /* qmi_wmsi_send_response() */

/*===========================================================================
  FUNCTION QMI_WMSI_SEND_IND()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the WMS service
    indication to client.

  PARAMETERS
    cl_sp    : client state pointer
    cmd_type : type of indication
    response      : dsm item

  RETURN VALUE
    TRUE if response sent to Framework sucessfully
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_wmsi_send_ind(
  void          * cl_sp_ptr,
  uint16          cmd_type,
  dsm_item_type * ind
)
{
  qmi_framework_common_msg_hdr_type hdr;
  boolean                           status = FALSE;
  qmi_wmsi_client_state_type       *cl_sp = NULL;

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  if (cl_sp)
  {
    /*-----------------------------------------------------------------------
      Fill the QMI Framework message header before sending Indication to
      Framework.
    -----------------------------------------------------------------------*/
    hdr.client_id      = cl_sp->common.clid;
    hdr.qmi_instance   = cl_sp->instance;
    hdr.service        = (qmux_service_e_type)cl_sp->service_id;
    hdr.transaction_id = 0;

    status = ds_qmi_fw_send_ind(&hdr, cmd_type, ind);

    MSG_HIGH_3( "qmi_wmsi_send_ind(): cmd_type %d clnt %d status %d",
                cmd_type,
                hdr.client_id,
                status );
  }
  else
  {
    dsm_free_packet(&ind);
  }

  return status;
} /* qmi_wmsi_send_ind() */

#ifndef REMOVE_QMI_WMS_RESET_V01
/*===========================================================================
  FUNCTION QMI_WMSI_RESET()

  DESCRIPTION
    Reset the issuing WMS client's state

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_reset(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type * response = NULL;
  boolean         retval;

  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  if (NULL != sp && NULL != cmd_buf_p && NULL != cl_sp && NULL != sdu_in)
  {
    qmi_wmsi_reset_client(cl_sp);

    /*-------------------------------------------------------------------------
      Build the response from back to front
      No error possible in reset, so result is success
    -------------------------------------------------------------------------*/
    retval = qmi_svc_put_result_tlv(&response,
                                    QMI_RESULT_SUCCESS,
                                    QMI_ERR_NONE);
  }
  else
  {
    retval = qmi_svc_put_result_tlv(&response,
                                    QMI_RESULT_FAILURE,
                                    QMI_ERR_INTERNAL);
  }

  CHECK_RETVAL();
  return response;

} /* qmi_wmsi_reset() */
#endif /* REMOVE_QMI_WMS_RESET_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client

  PARAMETERS
    cl_sp :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_reset_client(
  void * cl_sp_in
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  qmi_wms_cmd_node_type * cmd_node_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp_in)
  {
    MSG_ERROR_0("NULL cl_sp_in");
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_in;

  /*-------------------------------------------------------------------------
    Clear event reporting
  -------------------------------------------------------------------------*/
  cl_sp->report_status.report_mt_message = FALSE;
  cl_sp->report_status.report_mwi_message = FALSE;

  /*-------------------------------------------------------------------------
    Reset the subscription to primary
  -------------------------------------------------------------------------*/
  memset((void *)&cl_sp->subscription_status, 0, sizeof(cl_sp->subscription_status));
  cl_sp->subscription_status.subscription_type = WMSI_SUBSCRIPTION_PRIMARY;

  /*-------------------------------------------------------------------------
    Reset the indication registration info
  -------------------------------------------------------------------------*/
  cl_sp->indication_reg.reg_transport_layer_events = FALSE;
  cl_sp->indication_reg.reg_transport_nw_reg_events = FALSE;
  cl_sp->indication_reg.reg_call_status_events = FALSE;

  /*-------------------------------------------------------------------------
    Delete all items from the pending queue
  -------------------------------------------------------------------------*/
    cmd_node_ptr = NULL;
    do
    {
      if (cmd_node_ptr)
      {
        q_delete(&qmi_wmsi_global.cmd_pend_q, &cmd_node_ptr->link);
        qmi_wmsi_free_cmd_node(cmd_node_ptr);
      }
    cmd_node_ptr = (qmi_wms_cmd_node_type *)q_linear_search(&qmi_wmsi_global.cmd_pend_q,
                                                            (q_compare_func_type)qmi_wmsi_cmd_buf_belongs_to_client,
                                                            cl_sp);
    } while (cmd_node_ptr);

  ds_qmi_fw_common_reset_client(cl_sp_in);

} /* qmi_wmsi_reset_client() */


/*===========================================================================
  FUNCTION QMI_WMSI_CMD_BUF_BELONGS_TO_CLIENT()

  DESCRIPTION
    Determines if the given command buffer belongs to the given client

  PARAMETERS
    cl_sp :  client state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int qmi_wmsi_cmd_buf_belongs_to_client(
  qmi_wms_cmd_node_type *       cmd_node,
  void * cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int ret = (int)FALSE;
  if ((NULL != cl_sp) && (NULL != cmd_node) &&
      (NULL != cmd_node->cmd_buf) && (NULL != cmd_node->cmd_buf->x_p))
  {
    ret = (int)((cmd_node->cmd_buf->x_p->cl_sp == cl_sp) ? TRUE : FALSE);
  }

  return ret;
} /* wmsi_cmd_buf_belongs_to_client */


/*===========================================================================
  FUNCTION QMI_WMSI_GET_QMI_WMS_SP_BY_CMD_BUF()

  DESCRIPTION
    to get the wireless message service state ptr from a command buff ptr

  PARAMETERS
    cmd_buf_p_in : QMI command buff ptr

  RETURN VALUE
    wireless emssage service state ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_wmsi_state_type * qmi_wmsi_get_wms_sp_by_cmd_buf(
  void * cmd_buf_p_in
)
{
  qmi_cmd_buf_type * cmd_buf_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_buf_p = (qmi_cmd_buf_type *)cmd_buf_p_in;

  if (!qmi_wmsi_validate_cmd_buf_p( cmd_buf_p ))
  {
    return NULL;
  }

  if (NULL == cmd_buf_p->x_p)
  {
    return NULL;
  }

  return cmd_buf_p->x_p->svc_sp;
} /* qmi_wmsi_get_wms_sp_by_cmd_buf() */

static qmi_wms_cmd_node_type *qmi_wmsi_get_cmd_node(
  void
)
{
  qmi_wms_cmd_node_type * node_ptr = NULL;

  node_ptr = (qmi_wms_cmd_node_type *)
                           qmi_wmsi_mem_alloc(sizeof(qmi_wms_cmd_node_type));
  MSG_HIGH_1("malloc cmd_node @ 0x%x", node_ptr);

  return node_ptr;
} /* qmi_wmsi_get_cmd_node() */

static void qmi_wmsi_free_cmd_node(
  qmi_wms_cmd_node_type *node_ptr
)
{
  MSG_HIGH_1("free cmd_node @ 0x%x", node_ptr);
  qmi_wmsi_mem_free(node_ptr);
} /* qmi_wmsi_free_cmd_node() */

static void qmi_wmsi_put_pending_cmd_buf(
  qmi_wms_cmd_node_type * cmd_node_ptr
)
{
  (void) q_link(&qmi_wmsi_global.cmd_pend_q, &cmd_node_ptr->link);
  q_put(&qmi_wmsi_global.cmd_pend_q, &cmd_node_ptr->link);
} /* qmi_wmsi_put_pending_cmd_buf() */

void qmi_wmsi_read_params_from_template(
  wms_msg_event_info_s_type *event_info,
  uint8 sub_index
)
{
  wms_client_message_s_type *cli_msg = NULL;

  if (NULL == event_info)
  {
    return;
  }

  cli_msg = &event_info->status_info.message;

  /* Update the default MO template index */
  if ( (WMS_MESSAGE_MODE_GW == cli_msg->msg_hdr.message_mode) && 
       (cli_msg->u.gw_template.mask & WMS_GW_TEMPLATE_MASK_SC_ADDR) )
  {
    MSG_HIGH_2( "Processing template from [%d-%d]", 
                event_info->status_info.message.msg_hdr.mem_store,
                event_info->status_info.message.msg_hdr.index );

    qmi_wmsi_global.gw_template[sub_index].index         = cli_msg->msg_hdr.index;		  
    qmi_wmsi_global.gw_template[sub_index].inited        = TRUE;
    qmi_wmsi_global.gw_template[sub_index].mem_store     = qmi_wmsi_from_wms_storage(cli_msg->msg_hdr.mem_store);

    qmi_wmsi_generate_smsc_addr_ind(&cli_msg->u.gw_template, sub_index);

    (void)memscpy(&qmi_wmsi_global.gw_template[sub_index].mo_template,
                  sizeof(cli_msg->u.gw_template),
                  &cli_msg->u.gw_template,
                  sizeof(cli_msg->u.gw_template));

  }
  else
  {
    QMI_WMSI_MEM_FREEIF(cli_msg->u.gw_template.alpha_id.data);
  }
  return;
} /* qmi_wmsi_read_params_from_template */

boolean qmi_wmsi_bin_to_addr(
  byte               *str,
  byte               len,
  wms_address_s_type *addr
)
{
  boolean retval = FALSE;
  uint8 c, i;

  if ((NULL != str) && (NULL != addr))
  {
    addr->digit_mode = WMS_DIGIT_MODE_4_BIT;

    /* Number type & plan are included in first byte, rest is digits */
    addr->number_type = (wms_number_type_e_type)((*str & 0x70)>>4);
    addr->number_plan = (wms_number_plan_e_type)(*str & 0x0F);

    /* Adjust len value after skipping TON/NPI byte and begin parsing
       4-bit DTMF encoded dial string from second byte. */
    len--;
    str++;

    /* Need to div by 2 since len is number of bytes @ 2 digits/byte */
    if ((WMSI_ADDRESS_MAX / 2) < len)
    {
      return FALSE;
    }

    i = 0;
    
    while ( (WMSI_ADDRESS_MAX > i) && (len > 0) )
    {
      /* First digit */
      c = *str & 0x0F;
      if (WMSI_SMSC_DTMF_MAX < c)
      {
        return FALSE;
      }

      addr->digits[i++] = c;

      /* Second digit */
      c = (*str & 0xF0) >> 4;
      if (WMSI_SMSC_DTMF_MAX < c)
      {
        if (WMSI_SMSC_DTMF_END_MARKER == c)
        {
          break;
        }
        else
        {
          return FALSE;
        }
      }

      addr->digits[i++] = c;
      len--;
      str++;
    }

    addr->number_of_digits = i;
    retval = TRUE;
  }

  return retval;
} /* qmi_wmsi_bin_to_addr */


boolean qmi_wmsi_addr_to_bin(
  wms_address_s_type *addr,
  uint8              *bin
)
{
  boolean retval = FALSE;
  uint8 *len, i;

  if ((NULL != addr) && (NULL != bin))
  {
    if (0 < addr->number_of_digits && WMSI_ADDRESS_MAX >= addr->number_of_digits)
    {
      /* Initialize first byte (length) to 1 (for the type/plan) */
      len = bin++;
      *len = 1;

      /* Formulate the number type & plan */
      *bin = 0x80 | ((addr->number_type & 0x7) << 4);
      *bin |= addr->number_plan & 0xF;

      /* Pack the digits */
      for(i = 0, bin++; i < addr->number_of_digits; i++, bin++, (*len)++)
      {
        *bin = addr->digits[i] & 0xF;
        if (WMSI_SMSC_DTMF_MAX < *bin)
        {
          return FALSE;
        }

        if (addr->number_of_digits == ++i)
        {
          *bin |= 0xF0;
        }
        else if (i < WMSI_ADDRESS_MAX)
        {
          *bin |= (addr->digits[i] & 0xF) << 4;
          if (WMSI_SMSC_DTMF_MAX < (*bin >> 4))
          {
            return FALSE;
          }
        }
      }
      retval = TRUE;
    }
  }

  return retval;
} /* qmi_wmsi_addr_to_bin */

boolean qmi_wmsi_addr_to_str(
  wms_address_s_type *addr,
  uint8              *res_ptr,
  uint8              *type_of_addr,
  uint8              *addr_len
)
{
  boolean retval = FALSE;
  byte addr_ira[WMSI_ADDRESS_MAX+1], *addr_ira_ptr;
  uint8 type_i = 0;
  uint8 addr_ira_len = 0;

  byte bcd_idx = 0;
  uint8 temp = 0;

  if ((NULL != addr) && (NULL != res_ptr) && (NULL != type_of_addr) &&
      (NULL != addr_len))
  {
    memset(addr_ira, 0, sizeof(addr_ira));
    addr_ira_ptr = addr_ira;
    *type_of_addr = 0;

    temp = (uint8)((uint32)addr->number_type & 0x07);
    type_i = (uint8)((type_i | temp) << 4);
    temp = (uint8)((uint32)addr->number_plan & 0x0F);
    type_i = (uint8)(type_i | temp  | 0x80);

    if (type_i == 145)
    {
      *addr_ira_ptr++ = '+';
    }

    /* prepend 0's to the TYPE string to ensure it will be three digits */
    if (100 > type_i)
    {
      *type_of_addr++ = '0';
    }
    if (10 > type_i)
    {
      *type_of_addr++ = '0';
    }

    /* convert the TYPE from hex to ASCII */
    (void)qmi_wmsi_util_itoa(type_i, (byte *) type_of_addr, 10);

    if (WMSI_ADDRESS_MAX >= addr->number_of_digits)
    {
      /* convert the digits from BCD to ASCII*/
      while (bcd_idx < addr->number_of_digits)
      {
        switch (addr->digits[bcd_idx])
        {
          case 12:
            *addr_ira_ptr++ = 'A';
            break;
          case 13:
            *addr_ira_ptr++ = 'B';
            break;
          case 14:
            *addr_ira_ptr++ = 'C';
            break;
          case 10:
            *addr_ira_ptr++ = '*';
            break;
          case 11:
            *addr_ira_ptr++ = '#';
            break;
          default:
            addr_ira_ptr =
            (byte *)qmi_wmsi_util_itoa((uint32) addr->digits[bcd_idx],
                                       (byte *) addr_ira_ptr,
                                       10);
            break;
        }/* switch */
        bcd_idx++;
      }
      *addr_ira_ptr = '\0';
      addr_ira_len = strlen((char *) addr_ira);
      *addr_len = addr_ira_len;
      addr_ira_len = MIN(addr_ira_len, WMS_ADDRESS_DIGIT_MAX_V01);
      (void)memscpy((void *) res_ptr, addr_ira_len, (void *) addr_ira, addr_ira_len);

      retval = TRUE;
    }
    else
    {
      MSG_HIGH_2( "address digits longer than max(%d/%d)",
                  addr->number_of_digits,
                  WMSI_ADDRESS_MAX );
    }
  }

  return retval;
} /* qmi_wmsi_addr_to_str */

/*===========================================================================
  FUNCTION QMI_WMSI_INIT_MWI_CACHE()

  DESCRIPTION
  Initialize the MWI cache.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_init_mwi_cache(
  void
)
{
  uint32 i;

  for (i = 0; i < WMSI_SUBSCRIPTION_MAX; i ++)
  {
    qmi_wmsi_global.mwi[i].refresh_due_to_fcn = FALSE;
    qmi_wmsi_global.mwi[i].mwi_state = WMSI_MWI_STATE_UNINIT;
    qmi_wmsi_global.mwi[i].num_mwi = 5;

    /* Initialize voicemail */
    qmi_wmsi_global.mwi[i].mwi[0].type = WMSI_MWI_VOICEMAIL;
    qmi_wmsi_global.mwi[i].mwi[0].active = FALSE;
    qmi_wmsi_global.mwi[i].mwi[0].number_of_messages = 0;

    /* Initialize fax */
    qmi_wmsi_global.mwi[i].mwi[1].type = WMSI_MWI_FAX;
    qmi_wmsi_global.mwi[i].mwi[1].active = FALSE;
    qmi_wmsi_global.mwi[i].mwi[1].number_of_messages = 0;

    /* Initialize e-mail */
    qmi_wmsi_global.mwi[i].mwi[2].type = WMSI_MWI_EMAIL;
    qmi_wmsi_global.mwi[i].mwi[2].active = FALSE;
    qmi_wmsi_global.mwi[i].mwi[2].number_of_messages = 0;

    /* Initialize other */
    qmi_wmsi_global.mwi[i].mwi[3].type = WMSI_MWI_OTHER;
    qmi_wmsi_global.mwi[i].mwi[3].active = FALSE;
    qmi_wmsi_global.mwi[i].mwi[3].number_of_messages = 0;

    /* Initialize videomail */
    qmi_wmsi_global.mwi[i].mwi[4].type = WMSI_MWI_VIDEOMAIL;
    qmi_wmsi_global.mwi[i].mwi[4].active = FALSE;
    qmi_wmsi_global.mwi[i].mwi[4].number_of_messages = 0;
  }

} /* qmi_wmsi_init_mwi_cache */

/*===========================================================================
  FUNCTION QMI_WMSI_COMPARE_MWI_CACHE_TO_SIM()

  DESCRIPTION
  Compare the MWI cache to the SIM, to see if it needs to be updated.

  PARAMETERS
    sim_mwi_info : EF-MWIS info

  RETURN VALUE
    TRUE if SIM is different than MWI cache
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wmsi_compare_mwi_cache_to_sim(
  byte * sim_mwi_info,
  uint8  sub
)
{

  if (WMSI_MWI_STATE_UNINIT == qmi_wmsi_global.mwi[sub].mwi_state)
  {
    return TRUE;
  }

  if ((sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VOICEMAIL)
      && ((FALSE == qmi_wmsi_global.mwi[sub].mwi[0].active)
          || (qmi_wmsi_global.mwi[sub].mwi[0].number_of_messages !=
              sim_mwi_info[WMSI_EF_MWIS_VOICEMAIL_POS])))
  {
    return TRUE;
  }
  else if (!(sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VOICEMAIL)
           && (TRUE == qmi_wmsi_global.mwi[sub].mwi[0].active))
  {
    return TRUE;
  }

  if ((sim_mwi_info[0] & WMSI_MASK_EF_MWIS_FAX)
      && ((FALSE == qmi_wmsi_global.mwi[sub].mwi[1].active)
          || (qmi_wmsi_global.mwi[sub].mwi[1].number_of_messages !=
              sim_mwi_info[WMSI_EF_MWIS_FAX_POS])))
  {
    return TRUE;
  }
  else if (!(sim_mwi_info[0] & WMSI_MASK_EF_MWIS_FAX)
           && (TRUE == qmi_wmsi_global.mwi[sub].mwi[1].active))
  {
    return TRUE;
  }

  if ((sim_mwi_info[0] & WMSI_MASK_EF_MWIS_EMAIL)
      && ((FALSE == qmi_wmsi_global.mwi[sub].mwi[2].active)
          || (qmi_wmsi_global.mwi[sub].mwi[2].number_of_messages !=
              sim_mwi_info[WMSI_EF_MWIS_EMAIL_POS])))
  {
    return TRUE;
  }
  else if (!(sim_mwi_info[0] & WMSI_MASK_EF_MWIS_EMAIL)
           && (TRUE == qmi_wmsi_global.mwi[sub].mwi[2].active))
  {
    return TRUE;
  }

  if ((sim_mwi_info[0] & WMSI_MASK_EF_MWIS_OTHER)
      && ((FALSE == qmi_wmsi_global.mwi[sub].mwi[3].active)
          || (qmi_wmsi_global.mwi[sub].mwi[3].number_of_messages !=
              sim_mwi_info[WMSI_EF_MWIS_OTHER_POS])))
  {
    return TRUE;
  }
  else if (!(sim_mwi_info[0] & WMSI_MASK_EF_MWIS_OTHER)
           && (TRUE == qmi_wmsi_global.mwi[sub].mwi[3].active))
  {
    return TRUE;
  }

  if ((sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VIDEOMAIL)
      && ((FALSE == qmi_wmsi_global.mwi[sub].mwi[4].active)
          || (qmi_wmsi_global.mwi[sub].mwi[4].number_of_messages !=
              sim_mwi_info[WMSI_EF_MWIS_VIDEOMAIL_POS])))
  {
    return TRUE;
  }
  else if (!(sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VIDEOMAIL)
           && (TRUE == qmi_wmsi_global.mwi[sub].mwi[4].active))
  {
    return TRUE;
  }

  return FALSE;

} /* qmi_wmsi_compare_mwi_cache_to_sim */

/*===========================================================================
  FUNCTION QMI_WMSI_UPDATE_MWI_CACHE_FROM_SIM()

  DESCRIPTION
  Update the MWI cache from the SIM.

  PARAMETERS
    sim_mwi_info : EF-MWIS info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_update_mwi_cache_from_sim(
  byte * sim_mwi_info,
  uint8  sub
)
{
  /* Update the Voicemail info */
  if (sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VOICEMAIL)
  {
    qmi_wmsi_global.mwi[sub].mwi[0].active = TRUE;
    qmi_wmsi_global.mwi[sub].mwi[0].number_of_messages = sim_mwi_info[WMSI_EF_MWIS_VOICEMAIL_POS];
  }
  else
  {
    qmi_wmsi_global.mwi[sub].mwi[0].active = FALSE;
    qmi_wmsi_global.mwi[sub].mwi[0].number_of_messages = 0;
  }

  /* Update the Fax info */
  if (sim_mwi_info[0] & WMSI_MASK_EF_MWIS_FAX)
  {
    qmi_wmsi_global.mwi[sub].mwi[1].active = TRUE;
    qmi_wmsi_global.mwi[sub].mwi[1].number_of_messages = sim_mwi_info[WMSI_EF_MWIS_FAX_POS];
  }
  else
  {
    qmi_wmsi_global.mwi[sub].mwi[1].active = FALSE;
    qmi_wmsi_global.mwi[sub].mwi[1].number_of_messages = 0;
  }

  /* Update the E-mail info */
  if (sim_mwi_info[0] & WMSI_MASK_EF_MWIS_EMAIL)
  {
    qmi_wmsi_global.mwi[sub].mwi[2].active = TRUE;
    qmi_wmsi_global.mwi[sub].mwi[2].number_of_messages = sim_mwi_info[WMSI_EF_MWIS_EMAIL_POS];
  }
  else
  {
    qmi_wmsi_global.mwi[sub].mwi[2].active = FALSE;
    qmi_wmsi_global.mwi[sub].mwi[2].number_of_messages = 0;
  }

  /* Update the Other info */
  if (sim_mwi_info[0] & WMSI_MASK_EF_MWIS_OTHER)
  {
    qmi_wmsi_global.mwi[sub].mwi[3].active = TRUE;
    qmi_wmsi_global.mwi[sub].mwi[3].number_of_messages = sim_mwi_info[WMSI_EF_MWIS_OTHER_POS];
  }
  else
  {
    qmi_wmsi_global.mwi[sub].mwi[3].active = FALSE;
    qmi_wmsi_global.mwi[sub].mwi[3].number_of_messages = 0;
  }


  /* Update the Videomail info */
  if (sim_mwi_info[0] & WMSI_MASK_EF_MWIS_VIDEOMAIL)
  {
    qmi_wmsi_global.mwi[sub].mwi[4].active = TRUE;
    qmi_wmsi_global.mwi[sub].mwi[4].number_of_messages = sim_mwi_info[WMSI_EF_MWIS_VIDEOMAIL_POS];
  }
  else
  {
    qmi_wmsi_global.mwi[sub].mwi[4].active = FALSE;
    qmi_wmsi_global.mwi[sub].mwi[4].number_of_messages = 0;
  }

} /* qmi_wmsi_update_mwi_cache_from_sim */

/*===========================================================================
  FUNCTION QMI_WMSI_COMPARE_MWI_CACHE_TO_MT_SMS()

  DESCRIPTION
  Compare the MWI cache to the MT SMS, to see if it needs to be updated.

  PARAMETERS
    num_mwi : Number of MWI items in the MT SMS
    mt_sms_mwi_info: MWI items in the MT SMS

  RETURN VALUE
    TRUE if MT SMS is different than MWI cache
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wmsi_compare_mwi_cache_to_mt_sms(
  uint32 num_mwi,
  wms_gw_mwi_info_s_type * mt_sms_mwi_info,
  uint8 sub
)
{
  uint32 i, j;

  if (WMSI_MWI_STATE_UNINIT == qmi_wmsi_global.mwi[sub].mwi_state)
  {
    return TRUE;
  }

  for (i = 0; (i < num_mwi) && (i < WMSI_MWI_MAX); i++)
  {
    for (j = 0; j < WMSI_MWI_MAX; j++)
    {
      if (qmi_wms_to_wmsi_mwi(mt_sms_mwi_info[i].type) == qmi_wmsi_global.mwi[sub].mwi[j].type)
      {
        if ((qmi_wmsi_global.mwi[sub].mwi[j].active != mt_sms_mwi_info[i].is_active) ||
            (qmi_wmsi_global.mwi[sub].mwi[j].number_of_messages != mt_sms_mwi_info[i].count))
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;

} /* qmi_wmsi_compare_mwi_cache_to_mt_sms */

/*===========================================================================
  FUNCTION QMI_WMSI_UPDATE_MWI_CACHE_FROM_MT_SMS()

  DESCRIPTION
  Update the MWI cache from the MT SMS.

  PARAMETERS
    num_mwi : Number of MWI items in the MT SMS
    mt_sms_mwi_info: MWI items in the MT SMS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_update_mwi_cache_from_mt_sms(
  uint32 num_mwi,
  wms_gw_mwi_info_s_type * mt_sms_mwi_info,
  uint8 sub
)
{
  uint32 i, j;

  qmi_wmsi_global.mwi[sub].mwi_state = WMSI_MWI_STATE_INIT;

  for (i = 0; (i < num_mwi) && (i < WMSI_MWI_MAX); i++)
  {
    for (j = 0; j < WMSI_MWI_MAX; j++)
    {
      if (qmi_wms_to_wmsi_mwi(mt_sms_mwi_info[i].type) == qmi_wmsi_global.mwi[sub].mwi[j].type)
      {
        qmi_wmsi_global.mwi[sub].mwi[j].active = mt_sms_mwi_info[i].is_active;
        qmi_wmsi_global.mwi[sub].mwi[j].number_of_messages = mt_sms_mwi_info[i].count;
      }
    }
  }

} /* qmi_wmsi_update_mwi_cache_from_mt_sms */

/*===========================================================================
  FUNCTION QMI_WMSI_PROCESS_MT_MSG()

  DESCRIPTION
  Process an MT SMS message.

  PARAMETERS
    event_info_ptr : MT SMS message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_process_mt_msg(
  wms_msg_event_e_type        event,
  wms_msg_event_info_s_type * event_info_ptr,
  qmi_wmsi_subscription_e_type subscription
)
{
  wms_gw_mwi_info_s_type       mwi[WMSI_MWI_MAX];
  int                          num_mwi;

  /* If the message contains Message Waiting information, QMI WMS sends a
     Message Waiting indication. */
  if ((WMS_MSG_EVENT_RECEIVED_MESSAGE == event) &&
      (WMS_MESSAGE_MODE_GW == event_info_ptr->mt_message_info.message.msg_hdr.message_mode) &&
      (TRUE == wms_cfg_check_mwi_contents(&event_info_ptr->mt_message_info.message.u.gw_message.raw_ts_data,
                                          mwi,
                                          WMSI_MWI_MAX,
                                          &num_mwi)))
  {
    if (num_mwi < 0)
    {
      MSG_ERROR_0("MT msg contains MWI info, but num_mwi < 0");
    }
    else
    {
      /* Check if the MT SMS differs from the MWI cache */
      if (qmi_wmsi_compare_mwi_cache_to_mt_sms((uint32) num_mwi, mwi, (uint8) subscription))
      {
        /* Update the cache */
        qmi_wmsi_update_mwi_cache_from_mt_sms((uint32) num_mwi, mwi, (uint8) subscription);

        /* Generate the MWI indication */
        qmi_wmsi_generate_msg_wait_ind(subscription);
      }
      qmi_wmsi_generate_mt_message_ind(event, event_info_ptr, subscription, TRUE);
    }
  }
  else
  {
    /* It is a regular MT SMS. */
    qmi_wmsi_generate_mt_message_ind(event, event_info_ptr, subscription, FALSE);
  }

} /* qmi_wmsi_process_mt_msg */



/*===========================================================================
  FUNCTION QMI_WMSI_CHECK_TRANSPORT_LAYER_CACHE()

  DESCRIPTION
  Compare the transport layer cache to the event info received from WMS, to see if it
  needs to be updated.

  PARAMETERS
    wms_transport_reg_info : WMS Transport Layer info

  RETURN VALUE
    TRUE if the Transport Layer cache needs to be updated
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wmsi_check_transport_layer_cache(
  wms_msg_transport_reg_info_s_type * wms_transport_reg_info
)
{

  qmi_wmsi_transport_type_e_type        transport_type;
  qmi_wmsi_message_mode_e_type          transport_cap;

  if (NULL == wms_transport_reg_info)
  {
    return FALSE;
  }

  transport_type = qmi_wms_to_wmsi_transport_type(wms_transport_reg_info->transport_type);
  transport_cap = qmi_wms_to_wmsi_transport_cap(wms_transport_reg_info->transport_cap);

  // Check if the transport type and cap from WMS are valid
  if ((WMSI_TRANSPORT_TYPE_MAX <= transport_type) || WMSI_MSG_MODE_MAX <= transport_cap)
  {
    return FALSE;
  }

  if (WMSI_TRANSPORT_LAYER_STATE_UNINIT == qmi_wmsi_global.transport_layer.state)
  {
    /* The cache is uninitialized */
    return TRUE;
  }

  if (qmi_wmsi_global.transport_layer.as_id !=  wms_transport_reg_info->as_id)
  {
    return TRUE;
  }

  if (qmi_wmsi_global.transport_layer.registered_ind != wms_transport_reg_info->is_registered)
  {
    return TRUE;
  }

  if (qmi_wmsi_global.transport_layer.transport_type != transport_type)
  {
    return TRUE;
  }

  if (qmi_wmsi_global.transport_layer.transport_cap != transport_cap)
  {
    return TRUE;
  }

  return FALSE;

} /* qmi_wmsi_check_transport_layer_cache */

/*===========================================================================
  FUNCTION QMI_WMSI_UPDATE_TRANSPORT_LAYER_CACHE()

  DESCRIPTION
  Update the transport layer cache from the WMS event info

  PARAMETERS
    wms_transport_reg_info : WMS Transport Layer info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_update_transport_layer_cache(
  wms_msg_transport_reg_info_s_type * wms_transport_reg_info
)
{
  memset(&qmi_wmsi_global.transport_layer, 0, sizeof(qmi_wmsi_global.transport_layer));

  qmi_wmsi_global.transport_layer.state          = WMSI_TRANSPORT_LAYER_STATE_INIT;
  qmi_wmsi_global.transport_layer.registered_ind = wms_transport_reg_info->is_registered;
  qmi_wmsi_global.transport_layer.as_id          = wms_transport_reg_info->as_id;

  qmi_wmsi_global.transport_layer.transport_type =
    qmi_wms_to_wmsi_transport_type(wms_transport_reg_info->transport_type);

  qmi_wmsi_global.transport_layer.transport_cap  =
    qmi_wms_to_wmsi_transport_cap(wms_transport_reg_info->transport_cap);
} /* qmi_wmsi_update_transport_layer_cache */

/*===========================================================================
  FUNCTION QMI_WMSI_UPDATE_CHANNEL_PREF_AND_SERVICE_OPTION()

  DESCRIPTION
    Update the client's channel preference and service option.

  PARAMETERS
    Channel preference and service option

  RETURN VALUE
    Boolean to indicate if update was successful or not

  DEPENDENCIES
    None

  SIDE EFFECTS
    Changes the client's preference for channel and service option
===========================================================================*/
boolean qmi_wmsi_update_channel_pref_and_service_option(
  wms_client_ch_pref_e_type ch_pref,
  wms_dc_so_e_type so
)
{
  if (qmi_wmsi_global.last_used_ch_pref != ch_pref)
  {
    if (WMS_CLIENT_ERR_NONE == wms_client_set_cdma_channel_pref(qmi_wmsi_global.wms_cid,
                                                                ch_pref))
    {
      qmi_wmsi_global.last_used_ch_pref = ch_pref;
    }
    else
    {
      MSG_HIGH_0("Channel preference update failed");
      return FALSE;
    }
  }

  if ((WMS_CLIENT_CH_PREF_ACH_ONLY != ch_pref) && (qmi_wmsi_global.last_used_so != so))
  {
    if (WMS_CLIENT_ERR_NONE == wms_client_set_cdma_service_option_pref(qmi_wmsi_global.wms_cid,
                                                                       so))
    {
      qmi_wmsi_global.last_used_so = so;
    }
    else
    {
      MSG_HIGH_0("Service option preference update failed");
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_WMSI_UPDATE_FOLLOW_ON_DC_OPTION()

  DESCRIPTION
    Update the client's follow on DC option.

  PARAMETERS
    follow on DC option
 
  RETURN VALUE
    Boolean to indicate if update was successful or not

  DEPENDENCIES
    None

  SIDE EFFECTS
    Changes the client's preference for channel and service option
===========================================================================*/
boolean qmi_wmsi_update_follow_on_dc_option(
  boolean follow_on_dc 
)
{
  if (qmi_wmsi_global.last_used_follow_on_dc != follow_on_dc)
  {
    if (WMS_CLIENT_ERR_NONE == wms_client_set_cdma_service_follow_on_dc_pref( qmi_wmsi_global.wms_cid, 
                                                                              follow_on_dc) )
    {
      qmi_wmsi_global.last_used_follow_on_dc = follow_on_dc;
    }
    else
    {
      MSG_HIGH_0("Follow on DC update failed");
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_WMSI_CHECK_AND_UPDATE_SIM_STATUS()

  DESCRIPTION
    Update the global SIM ready status.

  PARAMETERS
    Subscription id
    Sys mode
    SIM ready option
 
  RETURN VALUE
    Boolean to indicate if update was successful or not

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wmsi_check_and_update_sim_status(
   qmi_wmsi_subscription_e_type sub, 
   wms_cfg_sys_e_type sys_type,
   boolean sim_is_ready 
)
{
  boolean is_updated = TRUE;

  switch (sim_is_ready)
  {   
     case 0:

          if (WMS_CFG_SYS_GW == sys_type)
          {
            if (WMSI_SIM_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP2;
            }
            else if (WMSI_SIM_READY_STATUS_3GPP == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_NONE;
            }
            /* Current SIM ready status is 3GPP2 or NONE. No need to update the status. */
            else
            {
              is_updated = FALSE;
            }
          }

          if (WMS_CFG_SYS_CDMA == sys_type)
          {
            if (WMSI_SIM_READY_STATUS_3GPP_AND_3GPP2 == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP;
            }
            else if (WMSI_SIM_READY_STATUS_3GPP2 == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_NONE;
            }
            /* Current SIM ready status is 3GPP or NONE. No need to update the status. */
            else
            {
              is_updated = FALSE;
            }
          }

          break;
    
     case 1:

          if (WMS_CFG_SYS_GW == sys_type)
          {
            if (WMSI_SIM_READY_STATUS_3GPP2 == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP_AND_3GPP2;
            }
            else if (WMSI_SIM_READY_STATUS_NONE == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP;
            }
            /* No change if already 3GPP or 3GPP_and_3GPP2 ready */
            else
            {
              is_updated = FALSE;
            }
          }

          if (WMS_CFG_SYS_CDMA == sys_type)
          {
            if (WMSI_SIM_READY_STATUS_3GPP == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP_AND_3GPP2;
            }
            else if (WMSI_SIM_READY_STATUS_NONE == qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
            {
              qmi_wmsi_global.sim_ready_status[sub].sim_ready_status = WMSI_SIM_READY_STATUS_3GPP2;
            }
            /* No change if already 3GPP2 or 3GPP_and_3GPP2 SIM ready */
            else
            {
              is_updated = FALSE;
            }
          }

          break;

     default:
          break;
  }

  return is_updated;
}

/*===========================================================================

                        QMI WMS MESSAGE FUNCTIONS

===========================================================================*/
void qmi_wmsi_generate_mt_message_ind(
  wms_msg_event_e_type         event,
  wms_msg_event_info_s_type *  event_info_ptr,
  qmi_wmsi_subscription_e_type subscription,
  boolean                      mt_msg_is_mwi
)
{
  uint8                              cl_idx;
  qmi_wmsi_report_event_info_s_type *wmsi_event_ptr = NULL;
  qmi_error_e_type                   errval         = QMI_ERR_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wmsi_event_ptr = (qmi_wmsi_report_event_info_s_type*)qmi_wmsi_mem_alloc(sizeof(qmi_wmsi_report_event_info_s_type));
  if (NULL == wmsi_event_ptr)
  {
    return;
  }

  MSG_HIGH_0("Generating MT message indication");

  wmsi_event_ptr->mask           |= WMSI_REPORT_STATUS_EV_MT_MSG;

  if ((WMS_MSG_EVENT_RECEIVED_MESSAGE == event) ||
      (WMS_MSG_EVENT_STATUS_REPORT == event))
  {
    wmsi_event_ptr->is_etws = FALSE;
    wmsi_event_ptr->smsc_address_incl = FALSE;
    wmsi_event_ptr->storage_type   =
    event_info_ptr->mt_message_info.message.msg_hdr.mem_store;
    wmsi_event_ptr->storage_index  =
    event_info_ptr->mt_message_info.message.msg_hdr.index;
    wmsi_event_ptr->transaction_id = event_info_ptr->mt_message_info.transaction_id;
    wmsi_event_ptr->route          = event_info_ptr->mt_message_info.route;
    wmsi_event_ptr->ack_indicator  =
    (WMS_DUMMY_TRANSACTION_ID != wmsi_event_ptr->transaction_id) ? FALSE : TRUE;
    wmsi_event_ptr->sms_on_ims = (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == event_info_ptr->mt_message_info.transport_type) ? 1: 0;

    if (WMS_MESSAGE_MODE_CDMA ==
        event_info_ptr->mt_message_info.message.msg_hdr.message_mode)
    {
      uint16 msg_len = 0;
      errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(wms_ts_cdma_cl2OTA(&(event_info_ptr->mt_message_info.message),
                                                                              wmsi_event_ptr->data,
                                                                              WMSI_MAX_LEN,
                                                                              &msg_len));
      if (QMI_ERR_NONE != errval)
      {
        MSG_HIGH_1("qmi_wmsi_generate_mt_message_ind: wms_ts_cdma_OTA2cl error: %d", errval);
        QMI_WMSI_MEM_FREEIF(wmsi_event_ptr);
        return;
      }
      wmsi_event_ptr->format = qmi_wmsi_from_wms_format(event_info_ptr->mt_message_info.message.u.cdma_message.raw_ts.format);
      wmsi_event_ptr->len    = msg_len;
    }
    else if (WMS_MESSAGE_MODE_GW ==
             event_info_ptr->mt_message_info.message.msg_hdr.message_mode)
    {
      wmsi_event_ptr->format = qmi_wmsi_from_wms_format(event_info_ptr->mt_message_info.message.u.gw_message.raw_ts_data.format);
      wmsi_event_ptr->len = (uint16)
                       event_info_ptr->mt_message_info.message.u.gw_message.raw_ts_data.len;
      (void)memscpy(wmsi_event_ptr->data,
                    wmsi_event_ptr->len,
                    event_info_ptr->mt_message_info.message.u.gw_message.raw_ts_data.data,
                    wmsi_event_ptr->len);

      /* For GW PP messages, include the SMSC address */
      if (WMSI_MSG_FORMAT_GW_PP == wmsi_event_ptr->format)
      {
        if (qmi_wmsi_addr_to_bin(&event_info_ptr->mt_message_info.message.u.gw_message.sc_address,
                                 wmsi_event_ptr->smsc_address))
        {
          wmsi_event_ptr->smsc_address_incl = TRUE;
        }
        else
        {
          MSG_ERROR_0("qmi_wmsi_generate_mt_message_ind: Unable to convert SMSC address to binary");
        }
      }
    }
  }
  else if (WMS_MSG_EVENT_ETWS_NOTIFICATION == event)
  {
    wmsi_event_ptr->is_etws = TRUE;
    wmsi_event_ptr->notification_type = (uint8) event_info_ptr->etws_msg_info.type;
    wmsi_event_ptr->plmn_info_present = event_info_ptr->etws_msg_info.plmn_info_present;

    wmsi_event_ptr->mcc = event_info_ptr->etws_msg_info.plmn_info.mcc[0] * 100 +
                     event_info_ptr->etws_msg_info.plmn_info.mcc[1] * 10 +
                     event_info_ptr->etws_msg_info.plmn_info.mcc[2];

    if (2 == event_info_ptr->etws_msg_info.plmn_info.num_mnc_digits)
    {
      wmsi_event_ptr->mnc = event_info_ptr->etws_msg_info.plmn_info.mnc[0] * 10 +
                       event_info_ptr->etws_msg_info.plmn_info.mnc[1];
    }
    else
    {
      wmsi_event_ptr->mnc = event_info_ptr->etws_msg_info.plmn_info.mnc[0] * 100 +
                       event_info_ptr->etws_msg_info.plmn_info.mnc[1] * 10 +
                       event_info_ptr->etws_msg_info.plmn_info.mnc[2];
    }

    wmsi_event_ptr->etws_len = (uint16) event_info_ptr->etws_msg_info.data_len;
    (void)memscpy(wmsi_event_ptr->etws_data,
                  wmsi_event_ptr->etws_len,
                  event_info_ptr->etws_msg_info.data,
                  wmsi_event_ptr->etws_len);
  }

  /*-------------------------------------------------------------------------
    Report incomming MT message to all clients that have registered for it
  -------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
        (cl_sp->subscription_status.subscription_type == subscription))
    {
      /* Send MWI PDU if the client requested for it */
      if (mt_msg_is_mwi && cl_sp->report_status.report_mwi_message)
      {
        qmi_wmsi_event_report_ind(cl_sp, wmsi_event_ptr);
      }

      /* Report the regular MT SMS to registered clients */
      if (!mt_msg_is_mwi && cl_sp->report_status.report_mt_message)
      {
        qmi_wmsi_event_report_ind(cl_sp, wmsi_event_ptr);
      }
    }
  }

  QMI_WMSI_MEM_FREEIF(wmsi_event_ptr);
} /* qmi_wmsi_generate_mt_message_ind() */

void qmi_wmsi_generate_call_control_ind(
  wms_msg_event_info_s_type * event_info_ptr,
  qmi_wmsi_subscription_e_type subscription
)
{
  uint8                             cl_idx;
  qmi_wmsi_report_event_info_s_type *wmsi_event_ptr = NULL;

  wmsi_event_ptr = (qmi_wmsi_report_event_info_s_type*)qmi_wmsi_mem_alloc(sizeof(qmi_wmsi_report_event_info_s_type));

  if (NULL == wmsi_event_ptr)
  {
    return;
  } 

  wmsi_event_ptr->mask           |=  WMSI_REPORT_STATUS_EV_SUBMIT_RPT;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_2( "Generating MO SMS Call Control indication for WMS client %d, message mode %d",
              event_info_ptr->submit_report_info.client_id,
              event_info_ptr->submit_report_info.message_mode );

  if ((WMS_MESSAGE_MODE_GW == event_info_ptr->submit_report_info.message_mode) &&
      (WMS_CLIENT_TYPE_QMI != event_info_ptr->submit_report_info.client_id))
  {
    wmsi_event_ptr->mo_ctrl_status = qmi_wmsi_from_wms_mo_ctrl_status(event_info_ptr->submit_report_info.mo_ctrl_status);
    wmsi_event_ptr->alpha_id_len = event_info_ptr->submit_report_info.alpha_id.len;

    (void)memscpy(wmsi_event_ptr->alpha_id,
                  event_info_ptr->submit_report_info.alpha_id.len,
                  event_info_ptr->submit_report_info.alpha_id.data,
                  event_info_ptr->submit_report_info.alpha_id.len);
  }
  else
  {
    /* Received non-QMI event for CDMA mode. ignore it as call control indication is not sent in this case*/
    QMI_WMSI_MEM_FREEIF(wmsi_event_ptr);
    return;
  }

  /*--------------------------------------------------------------------------------------
    Report MO SMS Call control status to all clients that have registered for it
  ----------------------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
        (cl_sp->subscription_status.subscription_type == subscription))
    {
      /* Send Call Control Info if the client requested for it */
      if (cl_sp->report_status.report_call_control_info)
      {
        qmi_wmsi_event_report_ind(cl_sp, wmsi_event_ptr);
      }
    }
  }

  QMI_WMSI_MEM_FREEIF(wmsi_event_ptr);
} /* qmi_wmsi_generate_call_control_ind() */

#ifndef REMOVE_QMI_WMS_SET_EVENT_REPORT_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_EVENT_REPORT()

  DESCRIPTION
    To set state for event reporting

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_set_event_report(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *               response  = NULL;
  qmi_wmsi_client_state_type *  wms_cl_sp = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_set_event_report_req_msg_v01     *set_event_report_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_HIGH_0("NULL pointer(s) in qmi_wmsi_set_event_report()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp                = (qmi_wmsi_client_state_type *)cl_sp;
  set_event_report_req_pdu = ( wms_set_event_report_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_event_report_req_msg_v01) );

  if (NULL == set_event_report_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_event_report_req_msg_v01, QMI_WMS_SET_EVENT_REPORT_REQ_V01, set_event_report_req_pdu);  

  /*-------------------------------------------------------------------------
    If ALL optional TLVs are absent return error or else carry on and
    process the request.
  -------------------------------------------------------------------------*/
  if (!(set_event_report_req_pdu->report_mt_message_valid) &&  !(set_event_report_req_pdu->report_call_control_info_valid) && !(set_event_report_req_pdu->report_mwi_message_valid))
  {
    errval = QMI_ERR_MISSING_ARG;
  }
  else
  {
    /*-------------------------------------------------------------------------
      change MT message reporting if tlv included in request
    -------------------------------------------------------------------------*/
    if (set_event_report_req_pdu->report_mt_message_valid)
    {
      switch (set_event_report_req_pdu->report_mt_message)
      {
        case WMSI_REPORT_MT_MSG_DISABLE:
          wms_cl_sp->report_status.report_mt_message = FALSE;
          break;

        case WMSI_REPORT_MT_MSG_ENABLE:
          wms_cl_sp->report_status.report_mt_message = TRUE;
          break;

        default:
          errval = QMI_ERR_INVALID_ARG;
      }
    }

    /*-------------------------------------------------------------------------
      change Call Control Information reporting if tlv included in request
    -------------------------------------------------------------------------*/
    if (set_event_report_req_pdu->report_call_control_info_valid)
    {
      switch (set_event_report_req_pdu->report_call_control_info)
      {
        case WMSI_REPORT_CALL_CTRL_INFO_DISABLE:
          wms_cl_sp->report_status.report_call_control_info = FALSE;
          break;

        case WMSI_REPORT_CALL_CTRL_INFO_ENABLE:
          wms_cl_sp->report_status.report_call_control_info = TRUE;
          break;

        default:
          errval = QMI_ERR_INVALID_ARG;
      }
    }

    /*-------------------------------------------------------------------------
      change MWI message reporting if tlv included in request
    -------------------------------------------------------------------------*/
    if (set_event_report_req_pdu->report_mwi_message_valid)
    {
      switch (set_event_report_req_pdu->report_mwi_message)
      {
        case WMSI_REPORT_MWI_MSG_DISABLE:
          wms_cl_sp->report_status.report_mwi_message = FALSE;
          break;

        case WMSI_REPORT_MWI_MSG_ENABLE:
          wms_cl_sp->report_status.report_mwi_message = TRUE;
          break;

        default:
          errval = QMI_ERR_INVALID_ARG;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  QMI_WMSI_MEM_FREEIF(set_event_report_req_pdu);
  return response;

} /* qmi_wmsi_set_event_report() */
#endif /* REMOVE_QMI_WMS_SET_EVENT_REPORT_V01 */


/*===========================================================================
  FUNCTION QMI_WMSI_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in set_event_report becomes true
    Sends an indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    event_ptr     : indication event info pointger

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_event_report_ind(
  void * cl_sp_ptr,
  qmi_wmsi_report_event_info_s_type * event_info_ptr
)
{
  dsm_item_type *  ind = NULL;
  qmi_wmsi_client_state_type * cl_sp = NULL;

  wms_event_report_ind_msg_v01  *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == cl_sp_ptr) || (NULL == event_info_ptr))
  {
    MSG_ERROR_0("Event report indication passed NULL ptr, return");
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_2( "Event report indication for client %d, mask %x",
              cl_sp->common.clid,
              event_info_ptr->mask );

  
  ind_pdu = ( wms_event_report_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_event_report_ind_msg_v01) );
  if (NULL == ind_pdu)
  {
    return;
  }   
  /*-------------------------------------------------------------------------
    ind was called for a new incomming MT message event
    Build the indication from back to front
  -------------------------------------------------------------------------*/
  if ((event_info_ptr->mask & WMSI_REPORT_STATUS_EV_MT_MSG))
  {
    if (FALSE == event_info_ptr->is_etws)
    {
      if (WMS_ROUTE_STORE_AND_NOTIFY == event_info_ptr->route)
      {
        ind_pdu->message_mode       = (wms_message_mode_enum_v01)qmi_wmsi_message_mode_from_wms_storage(event_info_ptr->storage_type);
        ind_pdu->message_mode_valid = TRUE;
		
        if (WMSI_MSG_MODE_MAX <= ind_pdu->message_mode)
        {
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }

        if (WMSI_MEM_STORE_MAX <= (qmi_wmsi_memory_storage_e_type)ind_pdu->mt_message.storage_type)
        {
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }
        ind_pdu->mt_message.storage_type = (wms_storage_type_enum_v01)qmi_wmsi_from_wms_storage(event_info_ptr->storage_type);
        ind_pdu->mt_message_valid        = TRUE;

        ind_pdu->mt_message.storage_index  = event_info_ptr->storage_index;
        if (WMSI_MESSAGE_LIST_MAX <= ind_pdu->mt_message.storage_index)
        {
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }
      }
      else if ((WMS_ROUTE_TRANSFER_ONLY == event_info_ptr->route) ||
               (WMS_ROUTE_TRANSFER_AND_ACK == event_info_ptr->route))
      {
        if (event_info_ptr->smsc_address_incl)
        {
          /* The first byte has the length of the SMSC address */
          ind_pdu->mt_message_smsc_address.data_len = event_info_ptr->smsc_address[0];
          if (WMSI_MT_SMSC_ADDRESS_MAX < ind_pdu->mt_message_smsc_address.data_len)
          {
            MSG_HIGH_0("The length of the SMSC address is too long");
            QMI_WMSI_MEM_FREEIF(ind_pdu);
            return;
          }
          ind_pdu->mt_message_smsc_address_valid = TRUE;

          (void)memscpy(ind_pdu->mt_message_smsc_address.data,
                        ind_pdu->mt_message_smsc_address.data_len,
                        &event_info_ptr->smsc_address[1],
                        ind_pdu->mt_message_smsc_address.data_len);
        }

        ind_pdu->transfer_route_mt_message.ack_indicator  = (event_info_ptr->ack_indicator)? 1 : 0;
        ind_pdu->transfer_route_mt_message.transaction_id = event_info_ptr->transaction_id;

        ind_pdu->transfer_route_mt_message_valid = TRUE;

        ind_pdu->transfer_route_mt_message.format = event_info_ptr->format;
        if (WMSI_MSG_FORMAT_MAX <= ind_pdu->transfer_route_mt_message.format)
        {
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }

        ind_pdu->transfer_route_mt_message.data_len = event_info_ptr->len;
        if (WMSI_MAX_LEN < ind_pdu->transfer_route_mt_message.data_len)
        {
          MSG_HIGH_0("The length the raw data is too long");
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }

        (void)memscpy(ind_pdu->transfer_route_mt_message.data, ind_pdu->transfer_route_mt_message.data_len, event_info_ptr->data, ind_pdu->transfer_route_mt_message.data_len);

      }

      ind_pdu->sms_on_ims_valid = TRUE;
      ind_pdu->sms_on_ims       = event_info_ptr->sms_on_ims;
    }
    else /* ETWS */
    {
      if (event_info_ptr->plmn_info_present)
      {
        /* The mobile country code value should be between 0 and 999 */
        ind_pdu->etws_plmn_info_valid               = TRUE;
        ind_pdu->etws_plmn_info.mobile_country_code = event_info_ptr->mcc;

        if (WMSI_MAX_MOBILE_CODE < ind_pdu->etws_plmn_info.mobile_country_code)
        {
          MSG_HIGH_0("The mobile country code value should be between 0 and 999");
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }

        /* The mobile network code value should be between 0 and 999 */
        ind_pdu->etws_plmn_info.mobile_network_code = event_info_ptr->mnc;
        if (WMSI_MAX_MOBILE_CODE < ind_pdu->etws_plmn_info.mobile_network_code)
        {
          MSG_HIGH_0("The mobile network code value should be between 0 and 999");
          QMI_WMSI_MEM_FREEIF(ind_pdu);
          return;
        }
      }

      ind_pdu->etws_message_valid             = TRUE;
      ind_pdu->etws_message.notification_type = event_info_ptr->notification_type;
	  
      if (WMSI_ETWS_NOTIFICATION_TYPE_MAX <= ind_pdu->etws_message.notification_type)
      {
        QMI_WMSI_MEM_FREEIF(ind_pdu);
        return;
      }

      ind_pdu->etws_message.data_len = event_info_ptr->etws_len;
      if (WMSI_MAX_ETWS_LEN < ind_pdu->etws_message.data_len)
      {
        MSG_HIGH_0("Length of ETWS is too long");
        QMI_WMSI_MEM_FREEIF(ind_pdu);
        return;
      }

      (void)memscpy(ind_pdu->etws_message.data, ind_pdu->etws_message.data_len, event_info_ptr->etws_data, ind_pdu->etws_message.data_len);
    }
  }

  if (event_info_ptr->mask & WMSI_REPORT_STATUS_EV_SUBMIT_RPT)
  {
    ind_pdu->call_control_info_valid           = TRUE;
    ind_pdu->call_control_info.mo_control_type = (wms_mo_control_info_enum_v01)event_info_ptr->mo_ctrl_status;

    if (WMSI_MO_CTRL_STATUS_TYPE_MAX <= ind_pdu->call_control_info.mo_control_type)
    {
      QMI_WMSI_MEM_FREEIF(ind_pdu);
      return;
    }

    ind_pdu->call_control_info.alpha_id_len = MIN(WMSI_ALPHA_ID_MAX, event_info_ptr->alpha_id_len);
    (void)memscpy(ind_pdu->call_control_info.alpha_id,
                  event_info_ptr->alpha_id_len,
                  event_info_ptr->alpha_id,
                  event_info_ptr->alpha_id_len);
  }

  ENCODE_INDICATION_TLV(wms_event_report_ind_msg_v01, QMI_WMS_EVENT_REPORT_IND_V01);
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_SET_EVENT_REPORT, ind);
  }
} /* qmi_wmsi_event_report_ind() */

#if !defined(REMOVE_QMI_WMS_RAW_SEND_V01) || !defined(REMOVE_QMI_WMS_ASYNC_RAW_SEND_V01)
/*===========================================================================
  FUNCTION QMI_WMSI_RAW_SEND()

  DESCRIPTION
    Sends a given message in raw format over the network using the WMS service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_raw_send(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;


  boolean                       force_on_dc     = FALSE;
  qmi_wms_cmd_node_type        *cmd_node        = NULL;
  wms_client_message_s_type    *client_msg_ptr  = NULL;
  wms_cfg_link_control_mode_e_type link_enable_mode = WMS_LINK_CONTROL_ENABLED_ONE;
#ifdef FEATURE_GWSMS
  uint8 *data_ptr;
#endif /* FEATURE_GWSMS */
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  uint8 sub;
  boolean                  retry_message = FALSE;
  boolean                  sms_on_ims = FALSE;
  boolean                  async_send = FALSE;
  wms_message_number_type  retry_message_id = WMS_DUMMY_MESSAGE_NUMBER;
  qmi_wms_tid_type         tid;
  wms_client_ch_pref_e_type ch_pref;
  wms_dc_so_e_type so;
  boolean follow_on_dc;     
  
  wms_async_raw_send_req_msg_v01     *raw_send_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == sdu_in))
  {
    MSG_ERROR_3( "NULL pointer sp = 0x%x, cmd_buf_p = 0x%x, sdu_in = 0x%x",
                 sp,
                 cmd_buf_p,
                 sdu_in );
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  raw_send_req_pdu = ( wms_async_raw_send_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_async_raw_send_req_msg_v01) );

  if (NULL == raw_send_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  if (WMSI_CMD_VAL_ASYNC_RAW_SEND == ((qmi_cmd_buf_type *)cmd_buf_p)->cmd_type)
  {
    async_send = TRUE;
    errval     = DECODE_REQUEST_TLV(wms_async_raw_send_req_msg_v01, QMI_WMS_ASYNC_RAW_SEND_REQ_V01, raw_send_req_pdu);

    if (QMI_ERR_NONE != errval)
    {
      goto send_result;
    }
  }

  else
  { // Need to distinguish between send and async_send because of corresponding TLVs out of order in IDL structure.
    wms_raw_send_req_msg_v01     *sync_raw_send_req_pdu = NULL;
    sync_raw_send_req_pdu = ( wms_raw_send_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_raw_send_req_msg_v01) );
  
    if (NULL == sync_raw_send_req_pdu)
    {
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }
    
    errval = DECODE_REQUEST_TLV(wms_raw_send_req_msg_v01, QMI_WMS_RAW_SEND_REQ_V01, sync_raw_send_req_pdu); 
    if (QMI_ERR_NONE != errval)
    {
      goto send_result;
    }
    
    raw_send_req_pdu->raw_message_data       = sync_raw_send_req_pdu->raw_message_data;
    raw_send_req_pdu->force_on_dc_valid      = sync_raw_send_req_pdu->force_on_dc_valid;
    raw_send_req_pdu->force_on_dc            = sync_raw_send_req_pdu->force_on_dc;
    raw_send_req_pdu->follow_on_dc_valid     = sync_raw_send_req_pdu->follow_on_dc_valid;
    raw_send_req_pdu->follow_on_dc           = sync_raw_send_req_pdu->follow_on_dc;
    raw_send_req_pdu->link_timer_valid       = sync_raw_send_req_pdu->link_timer_valid;
    raw_send_req_pdu->link_timer             = sync_raw_send_req_pdu->link_timer;  
    raw_send_req_pdu->sms_on_ims_valid       = sync_raw_send_req_pdu->sms_on_ims_valid;
    raw_send_req_pdu->sms_on_ims             = sync_raw_send_req_pdu->sms_on_ims;
    raw_send_req_pdu->retry_message_valid    = sync_raw_send_req_pdu->retry_message_valid;
    raw_send_req_pdu->retry_message          = sync_raw_send_req_pdu->retry_message;
    raw_send_req_pdu->retry_message_id_valid = sync_raw_send_req_pdu->retry_message_id_valid;
    raw_send_req_pdu->retry_message_id       = sync_raw_send_req_pdu->retry_message_id;
    raw_send_req_pdu->link_enable_mode_valid = sync_raw_send_req_pdu->link_enable_mode_valid;
    raw_send_req_pdu->link_enable_mode       = sync_raw_send_req_pdu->link_enable_mode;

    QMI_WMSI_MEM_FREEIF(sync_raw_send_req_pdu);
  }

  if (WMS_FORMAT_MAX == qmi_wmsi_to_wms_format((qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* The total length of the TLV should be 3 + the length of the raw data
     (3 = 1 byte format field + 2 byte len field). */
  if ((0 == raw_send_req_pdu->raw_message_data.raw_message_len) || (WMSI_MAX_LEN < raw_send_req_pdu->raw_message_data.raw_message_len))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (raw_send_req_pdu->force_on_dc_valid)
  {
    if (WMS_DC_SO_14 < raw_send_req_pdu->force_on_dc.so)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    if (WMSI_MSG_FORMAT_GW_PP == (qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
    }
  }

  if (raw_send_req_pdu->follow_on_dc_valid)
  {
    if (WMSI_MSG_FORMAT_GW_PP == (qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
    }
    else
    {
      MSG_HIGH_1("follow_on_dc = %d", raw_send_req_pdu->follow_on_dc);
    }
  }

  if (raw_send_req_pdu->link_timer_valid)
  {
    if (WMSI_LINK_TIMER_MAX < raw_send_req_pdu->link_timer)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    if (WMSI_MSG_FORMAT_CDMA == (qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
    }
  }

  if (raw_send_req_pdu->sms_on_ims_valid)
  {
    sms_on_ims = (1 == raw_send_req_pdu->sms_on_ims) ? TRUE : FALSE;

    if (sms_on_ims && raw_send_req_pdu->force_on_dc_valid && raw_send_req_pdu->force_on_dc.force_on_dc)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }
  else if (qmi_wmsi_mobile_is_ims_registered(as_id))
  {
    sms_on_ims = TRUE;
  }

  if (raw_send_req_pdu->link_enable_mode_valid)
  {
    if (!raw_send_req_pdu->link_timer_valid)
    {
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }

    link_enable_mode = (1 == raw_send_req_pdu->link_enable_mode) ? WMS_LINK_CONTROL_ENABLE_ALL : WMS_LINK_CONTROL_ENABLED_ONE;
  }

  client_msg_ptr = (wms_client_message_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_message_s_type));
  if (NULL == client_msg_ptr)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  /*-----------------------------------------------------------------------
    perform the requested function
    begin by fill in the client message struct info for the message
  -----------------------------------------------------------------------*/
  switch ((qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
  {
#ifdef FEATURE_GWSMS
    case WMSI_MSG_FORMAT_GW_PP:
      /* fill in message header values */
      client_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      client_msg_ptr->msg_hdr.tag          = WMS_TAG_MO_NOT_SENT;
      client_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
      client_msg_ptr->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

      /* message format */
      client_msg_ptr->u.gw_message.raw_ts_data.format =
        qmi_wmsi_to_wms_format((qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format);

      /* message length is the length of the PDU minus the length of the SMSC
         address length (guaranteed 1st byte of PDU) minus the actual length of
         the SMSC address */
      if (raw_send_req_pdu->raw_message_data.raw_message_len > (uint32)(raw_send_req_pdu->raw_message_data.raw_message[0] + 1))
      {
        client_msg_ptr->u.gw_message.raw_ts_data.len = raw_send_req_pdu->raw_message_data.raw_message_len - 1 -
                                                         raw_send_req_pdu->raw_message_data.raw_message[0];
        /* T_PDU type to SUBMIT for MO */
        client_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
      }
      else
      {
        errval = QMI_ERR_MALFORMED_MSG;
        goto send_result;
      }

      /* error checking */
      if ((WMS_FORMAT_MAX <= client_msg_ptr->u.gw_message.raw_ts_data.format) ||
          (WMSI_MAX_LEN   <= client_msg_ptr->u.gw_message.raw_ts_data.len))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      if (0 != raw_send_req_pdu->raw_message_data.raw_message[0])
      {
        if ((raw_send_req_pdu->raw_message_data.raw_message_len <= raw_send_req_pdu->raw_message_data.raw_message[0]) ||
            (2                >  raw_send_req_pdu->raw_message_data.raw_message[0]))
        {
          errval = QMI_ERR_INVALID_ARG;
          goto send_result;
        }

        data_ptr = (uint8 *) raw_send_req_pdu->raw_message_data.raw_message;
        data_ptr++;
        if (! qmi_wmsi_bin_to_addr(data_ptr,
                                   raw_send_req_pdu->raw_message_data.raw_message[0],
                                   &(client_msg_ptr->u.gw_message.sc_address)))
        {
          errval = QMI_ERR_SMSC_ADDR;
          goto send_result;
        }
      }
      else
      {
        MSG_HIGH_0("SMSC address not given, using default");
        sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
        (void)memscpy(&(client_msg_ptr->u.gw_message.sc_address),
                      sizeof(qmi_wmsi_global.gw_template[sub].mo_template.sc_addr),
                      &qmi_wmsi_global.gw_template[sub].mo_template.sc_addr,
                      sizeof(qmi_wmsi_global.gw_template[sub].mo_template.sc_addr));
      }

      data_ptr = (uint8 *) raw_send_req_pdu->raw_message_data.raw_message;
      data_ptr += (raw_send_req_pdu->raw_message_data.raw_message[0] + 1);
      (void)memscpy(client_msg_ptr->u.gw_message.raw_ts_data.data,
                    client_msg_ptr->u.gw_message.raw_ts_data.len,
                    data_ptr,
                    client_msg_ptr->u.gw_message.raw_ts_data.len);

      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMSI_MSG_FORMAT_CDMA:
      /* fill in message header values */
      client_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      client_msg_ptr->msg_hdr.tag          = WMS_TAG_MO_NOT_SENT;
      client_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
      client_msg_ptr->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

      /* message format */
      client_msg_ptr->u.cdma_message.raw_ts.format =
        qmi_wmsi_to_wms_format((qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format);

      /* message length */
      client_msg_ptr->u.cdma_message.raw_ts.len = raw_send_req_pdu->raw_message_data.raw_message_len;

      /* error checking */
      if ((WMS_FORMAT_MAX <= client_msg_ptr->u.cdma_message.raw_ts.format) ||
          (WMSI_MAX_LEN   <= client_msg_ptr->u.cdma_message.raw_ts.len))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      /* copy the message data buffer */
      (void)memscpy(client_msg_ptr->u.cdma_message.raw_ts.data,
                    raw_send_req_pdu->raw_message_data.raw_message_len,
                    raw_send_req_pdu->raw_message_data.raw_message,
                    raw_send_req_pdu->raw_message_data.raw_message_len);

      errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
        wms_ts_cdma_OTA2cl(raw_send_req_pdu->raw_message_data.raw_message, raw_send_req_pdu->raw_message_data.raw_message_len, client_msg_ptr));

      if (QMI_ERR_NONE != errval)
      {
        MSG_HIGH_1("qmi_wmsi_raw_send : wms_ts_cdma_OTA2cl returned error[%d]", errval);
        goto send_result;
      }

      break;
#endif /* FEATURE_CDSMS */

    default:
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    tid = qmi_wmsi_get_next_tid();
    if (raw_send_req_pdu->retry_message_valid)
    {
      retry_message = (1 == raw_send_req_pdu->retry_message) ? TRUE : FALSE;

      if (raw_send_req_pdu->retry_message_id_valid)
      {
        retry_message_id = raw_send_req_pdu->retry_message_id;
      }
    }


    if (WMSI_MSG_FORMAT_CDMA == (qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
    {
      boolean ret_val = FALSE;
      
      if (raw_send_req_pdu->force_on_dc_valid)
      {
        force_on_dc = (raw_send_req_pdu->force_on_dc.force_on_dc != 0) ? TRUE : FALSE;
        MSG_HIGH_1("force_on_dc is set to %d", force_on_dc);

        if (force_on_dc)
        {
          ch_pref = WMS_CLIENT_CH_PREF_TCH_ONLY;
          so = raw_send_req_pdu->force_on_dc.so;
        }
        /* Do not care about the channel on which message is sent */
        else
        {
          ch_pref = WMS_CLIENT_CH_PREF_ACH_OR_TCH;
          so = qmi_wmsi_global.default_service_option;
        }
      }
      /* Channel preference should be the same as NV 4228 and 4229 settings */
      else
      {
        ch_pref = qmi_wmsi_global.default_ch_pref_from_nv;
        so = qmi_wmsi_global.default_service_option;
      }

      ret_val = qmi_wmsi_update_channel_pref_and_service_option(ch_pref, so);
      
      if (FALSE == ret_val)
      {
        errval = QMI_ERR_INTERNAL;
        goto send_result;
      }

      if ( raw_send_req_pdu->follow_on_dc_valid) 
      {
        follow_on_dc = (raw_send_req_pdu->follow_on_dc != 0) ? TRUE : FALSE;
        ret_val = qmi_wmsi_update_follow_on_dc_option( follow_on_dc );

        if (FALSE == ret_val)
        {
          errval = QMI_ERR_INTERNAL;
          goto send_result;
        }
      }
    }

    if (raw_send_req_pdu->link_timer_valid)
    {
      errval = (qmi_error_e_type) qmi_wmsi_from_wms_status
                (wms_cfg_ms_set_link_control(qmi_wmsi_global.wms_cid,
                                             as_id,
                                             (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                             (void *) tid,
                                             link_enable_mode,
                                             raw_send_req_pdu->link_timer));
    }

    if (sms_on_ims)
    {
      if (retry_message)
      {
        errval = (qmi_error_e_type)
                   qmi_wmsi_from_wms_status(wms_msg_ms_resend_ext(qmi_wmsi_global.wms_cid,
                                                                  as_id,
                                                                  (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                  (void *) tid,
                                                                  WMS_SEND_MODE_CLIENT_MESSAGE,
                                                                  client_msg_ptr,
                                                                  retry_message_id,
                                                                  WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS));
       }
       else
       {
         errval = (qmi_error_e_type)
                    qmi_wmsi_from_wms_status(wms_msg_ms_send_ext(qmi_wmsi_global.wms_cid,
                                                                 as_id,
                                                                 (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                 (void *) tid,
                                                                 WMS_SEND_MODE_CLIENT_MESSAGE,
                                                                 client_msg_ptr,
                                                                 WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS));


      }
    }
    else
    {
      if (retry_message)
      {
        errval = (qmi_error_e_type)
                   qmi_wmsi_from_wms_status(wms_msg_ms_resend_ext(qmi_wmsi_global.wms_cid,
                                                                  as_id,
                                                                  (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                  (void *) tid,
                                                                  WMS_SEND_MODE_CLIENT_MESSAGE,
                                                                  client_msg_ptr,
                                                                  retry_message_id,
                                                                  WMS_MSG_TRANSPORT_TYPE_CS));
      }
      else
      {
        errval = (qmi_error_e_type)
                   qmi_wmsi_from_wms_status(wms_msg_ms_send_ext(qmi_wmsi_global.wms_cid,
                                                                as_id,
                                                                (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                (void *) tid,
                                                                WMS_SEND_MODE_CLIENT_MESSAGE,
                                                                client_msg_ptr,
                                                                WMS_MSG_TRANSPORT_TYPE_CS));
      }
    }
  }

  if (WMSI_MSG_FORMAT_CDMA == (qmi_wmsi_message_format_e_type)raw_send_req_pdu->raw_message_data.format)
  {
    boolean ret_val = FALSE;

    // Revert the channel preference and service option preference to default NV preferences. 
    ret_val = qmi_wmsi_update_channel_pref_and_service_option(qmi_wmsi_global.default_ch_pref_from_nv,
                                                              qmi_wmsi_global.default_service_option);
    
    if (FALSE == ret_val)
    {
      MSG_HIGH_0( "Unable to revert the client channel or service option preference" );
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }	 
  }

  if (QMI_ERR_NONE == errval)
  {
    /* cache client_msg */
    cmd_node = qmi_wmsi_get_cmd_node();
    if (NULL == cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }
    cmd_node->cmd_buf = (qmi_cmd_buf_type *) cmd_buf_p;
    (void)memscpy(&cmd_node->cache.raw_send.client_msg,
                  sizeof(cmd_node->cache.raw_send.client_msg),
                  client_msg_ptr,
                  sizeof(wms_client_message_s_type));

    cmd_node->cache.raw_send.sms_on_ims = sms_on_ims;
    cmd_node->cache.raw_send.retry_message = retry_message;
    cmd_node->cache.raw_send.retry_message_id = retry_message_id;
    cmd_node->tid = tid;
    cmd_node->wms_cl_sp = wms_cl_sp;
    cmd_node->cmd_type = WMSI_CMD_VAL_RAW_SEND;
    cmd_node->async_cmd = FALSE;
    cmd_node->cache.raw_send.got_user_data_tlv = raw_send_req_pdu->user_data_valid;
    if (async_send)
    {
      cmd_node->cache.raw_send.user_data = raw_send_req_pdu->user_data;
      cmd_node->async_cmd = TRUE;
      cmd_node->cmd_type = WMSI_CMD_VAL_ASYNC_RAW_SEND;
      cmd_node->cmd_buf = NULL;
    }
    MSG_LOW_1("Pushing pending cmd_node %p for raw_send", cmd_node);
    qmi_wmsi_put_pending_cmd_buf(cmd_node);

     if (!async_send)
    {
      QMI_WMSI_MEM_FREEIF(raw_send_req_pdu);
      QMI_WMSI_MEM_FREEIF(client_msg_ptr);
      return QMI_SVC_RESPONSE_PENDING;
    }
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(raw_send_req_pdu);
  QMI_WMSI_MEM_FREEIF(client_msg_ptr);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_raw_send() */
#endif /* !defined(REMOVE_QMI_WMS_RAW_SEND_V01) || !defined(REMOVE_QMI_WMS_ASYNC_RAW_SEND_V01) */

/*===========================================================================
  FUNCTION QMI_WMSI_RETRIEVE_MSGID()

  DESCRIPTION
    Creates the response message for a raw send WMS command result event.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qmi_wmsi_retrieve_msgid(
  wms_msg_event_info_s_type * event_info_ptr
)
{
  wms_client_ts_data_s_type  *wms_ts_data_ptr = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  uint32             msgid = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return 0;
  }

  wms_ts_data_ptr = (wms_client_ts_data_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_ts_data_s_type));
  if (NULL == wms_ts_data_ptr)
  {
    return 0;
  }
  
  if (WMS_MESSAGE_MODE_CDMA == event_info_ptr->status_info.message.msg_hdr.message_mode)
  {
    errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
      wms_ts_decode(&event_info_ptr->status_info.message.u.cdma_message.raw_ts,
                    wms_ts_data_ptr));
    if (QMI_ERR_NONE == errval)
    {
      msgid = wms_ts_data_ptr->u.cdma.message_id.id_number;
    }
  }
  else if (WMS_MESSAGE_MODE_GW == event_info_ptr->status_info.message.msg_hdr.message_mode)
  {
    errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
      wms_ts_decode(&event_info_ptr->status_info.message.u.gw_message.raw_ts_data,
                    wms_ts_data_ptr));

    if (QMI_ERR_NONE == errval)
    {
      msgid = wms_ts_data_ptr->u.gw_pp.u.submit.message_reference;
    }
  }

  QMI_WMSI_MEM_FREEIF(wms_ts_data_ptr);

  return msgid;
} /* qmi_wmsi_retrieve_msgid */

/*===========================================================================
  FUNCTION QMI_WMSI_ASYNC_SEND_IND()

  DESCRIPTION
    Creates the response message for a raw send WMS command result event.

  PARAMETERS
    raw_send_ind_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr         : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_async_send_ind(
  wms_async_raw_send_ind_msg_v01 *raw_send_ind_pdu,
  wms_msg_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type * cmd_node
)
{
  qmi_error_e_type   errval =  QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
     return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* check the event report to see if send was successful, if not get error */
  if (WMS_RPT_OK != event_info_ptr->submit_report_info.report_status)
  {
    if (WMS_RPT_CDMA_TL_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      raw_send_ind_pdu->error_class = (wms_error_class_send_enum_v01)
      qmi_wmsi_from_wms_error_class(event_info_ptr->submit_report_info.cause_info.error_class);

      raw_send_ind_pdu->error_class_valid = TRUE;

      raw_send_ind_pdu->cause_code =
      qmi_wmsi_from_wms_cause_error(event_info_ptr->submit_report_info.cause_info.tl_status);

      raw_send_ind_pdu->cause_code_valid = TRUE;
    }
    else if (WMS_RPT_RP_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      raw_send_ind_pdu->gw_cause_info.rp_cause = (uint16)event_info_ptr->submit_report_info.cause_info.cause_value;
      raw_send_ind_pdu->gw_cause_info.tp_cause = (uint8)event_info_ptr->submit_report_info.u.error.tp_cause;

      raw_send_ind_pdu->gw_cause_info_valid = TRUE;
    }
    else
    {
      errval = qmi_wmsi_from_wms_report_error(event_info_ptr->submit_report_info.report_status);

      if (QMI_ERR_MESSAGE_DELIVERY_FAILURE == errval)
      {
        /* Message Delivery Failure Cause TLV */
        if ((WMS_MESSAGE_MODE_GW == event_info_ptr->submit_report_info.message_mode) &&
            (WMS_RPT_MO_CONTROL_DISALLOW == event_info_ptr->submit_report_info.report_status))
        {
          raw_send_ind_pdu->message_delivery_failure_cause       = (wms_message_delivery_failure_cause_enum_v01 )WMSI_MESSAGE_BLOCKED_DUE_TO_CALL_CONTROL;
          raw_send_ind_pdu->message_delivery_failure_cause_valid = TRUE;

        }

        /* Message Delivery Failure Type TLV */
        raw_send_ind_pdu->message_delivery_failure_type =(wms_message_delivery_failure_type_enum_v01)
                                                         ( event_info_ptr->submit_report_info.delivery_failure_permanent ? 
                                                           WMSI_MESSAGE_DELIVERY_FAILURE_PERM : 
                                                           WMSI_MESSAGE_DELIVERY_FAILURE_TEMP );
        raw_send_ind_pdu->message_delivery_failure_type_valid = TRUE;
      }
    }
  }

  if (WMS_MO_CTRL_STATUS_MOD == cmd_node->cache.raw_send.mo_ctrl_status)
  {
    raw_send_ind_pdu->call_control_modified_info.alpha_id_len = cmd_node->cache.raw_send.alpha_id_len;

    (void)memscpy(raw_send_ind_pdu->call_control_modified_info.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len,
                  cmd_node->cache.raw_send.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len);
	
    raw_send_ind_pdu->call_control_modified_info_valid = TRUE;
  }

  raw_send_ind_pdu->message_id       = (uint16) cmd_node->cache.raw_send.wms_msgid;
  raw_send_ind_pdu->message_id_valid = TRUE;

  if (cmd_node->cache.raw_send.got_user_data_tlv)
  {
    raw_send_ind_pdu->user_data       = cmd_node->cache.raw_send.user_data;
    raw_send_ind_pdu->user_data_valid = TRUE;
  }

  return errval;

} /* qmi_wmsi_async_send_ind */


/*===========================================================================
  FUNCTION QMI_WMSI_ASYNC_MEM_SEND_IND()

  DESCRIPTION
    Creates the response message for a raw send WMS command result event.

  PARAMETERS
    raw_send_ind_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_async_mem_send_ind(
  wms_async_send_from_mem_store_ind_msg_v01 *mem_send_ind_pdu,
  wms_msg_event_info_s_type                 *event_info_ptr,
  qmi_wms_cmd_node_type                     *cmd_node
)
{
  qmi_error_e_type                  errval =  QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
     return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* check the event report to see if send was successful, if not get error */
  if (WMS_RPT_OK != event_info_ptr->submit_report_info.report_status)
  {
    if (WMS_RPT_CDMA_TL_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      mem_send_ind_pdu->error_class = (wms_error_class_send_enum_v01)
      qmi_wmsi_from_wms_error_class(event_info_ptr->submit_report_info.cause_info.error_class);

      mem_send_ind_pdu->error_class_valid = TRUE;

      mem_send_ind_pdu->cause_code =
      qmi_wmsi_from_wms_cause_error(event_info_ptr->submit_report_info.cause_info.tl_status);

      mem_send_ind_pdu->cause_code_valid = TRUE;
    }
    else if (WMS_RPT_RP_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;
	  
      mem_send_ind_pdu->gw_cause_info.rp_cause = (uint16)event_info_ptr->submit_report_info.cause_info.cause_value;
      mem_send_ind_pdu->gw_cause_info.tp_cause = (uint8)event_info_ptr->submit_report_info.u.error.tp_cause;

      mem_send_ind_pdu->gw_cause_info_valid = TRUE;
    }
    else
    {
      errval = qmi_wmsi_from_wms_report_error(event_info_ptr->submit_report_info.report_status);

      if (QMI_ERR_MESSAGE_DELIVERY_FAILURE == errval)
      {
        /* Message Delivery Failure Cause TLV */
        if ((WMS_MESSAGE_MODE_GW == event_info_ptr->submit_report_info.message_mode) &&
            (WMS_RPT_MO_CONTROL_DISALLOW == event_info_ptr->submit_report_info.report_status))
        {
          mem_send_ind_pdu->message_delivery_failure_cause       = (wms_message_delivery_failure_cause_enum_v01)WMSI_MESSAGE_BLOCKED_DUE_TO_CALL_CONTROL;
          mem_send_ind_pdu->message_delivery_failure_cause_valid = TRUE;

        }

        /* Message Delivery Failure Type TLV */
        mem_send_ind_pdu->message_delivery_failure_type = (wms_message_delivery_failure_type_enum_v01)
            ( event_info_ptr->submit_report_info.delivery_failure_permanent ?
              WMSI_MESSAGE_DELIVERY_FAILURE_PERM : WMSI_MESSAGE_DELIVERY_FAILURE_TEMP);
        mem_send_ind_pdu->message_delivery_failure_type_valid = TRUE;
      }
    }
  }

  if (WMS_MO_CTRL_STATUS_MOD == cmd_node->cache.raw_send.mo_ctrl_status)
  {
    mem_send_ind_pdu->call_control_modified_info.alpha_id_len = cmd_node->cache.raw_send.alpha_id_len;
	
    (void)memscpy(mem_send_ind_pdu->call_control_modified_info.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len,
                  cmd_node->cache.raw_send.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len);
    mem_send_ind_pdu->call_control_modified_info_valid = TRUE;
  }

  mem_send_ind_pdu->message_id       = (uint16) cmd_node->cache.raw_send.wms_msgid;
  mem_send_ind_pdu->message_id_valid = TRUE;

  if (cmd_node->cache.raw_send.got_user_data_tlv)
  {
    mem_send_ind_pdu->user_data       = cmd_node->cache.raw_send.user_data;
    mem_send_ind_pdu->user_data_valid = TRUE;
  }

  return errval;

} /* qmi_wmsi_async_mem_send_ind */

/*===========================================================================
  FUNCTION QMI_WMSI_MEM_SEND_RESP()

  DESCRIPTION
    Creates the response message for a raw send WMS command result event.

  PARAMETERS
    mem_send_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_mem_send_resp(
  wms_send_from_mem_store_resp_msg_v01 *mem_send_resp_pdu,
  wms_msg_event_info_s_type *event_info_ptr,
  qmi_wms_cmd_node_type     *cmd_node
)
  {
  qmi_error_e_type                  errval =  QMI_ERR_NONE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
     return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* check the event report to see if send was successful, if not get error */
  if (WMS_RPT_OK != event_info_ptr->submit_report_info.report_status)
  {
    if (WMS_RPT_CDMA_TL_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      mem_send_resp_pdu->error_class = (wms_error_class_send_enum_v01)
      qmi_wmsi_from_wms_error_class(event_info_ptr->submit_report_info.cause_info.error_class);

      mem_send_resp_pdu->error_class_valid = TRUE;

      mem_send_resp_pdu->cause_code =
        qmi_wmsi_from_wms_cause_error(event_info_ptr->submit_report_info.cause_info.tl_status);

      mem_send_resp_pdu->cause_code_valid = TRUE;
    }
    else if (WMS_RPT_RP_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      mem_send_resp_pdu->gw_cause_info.rp_cause = (uint16)event_info_ptr->submit_report_info.cause_info.cause_value;
      mem_send_resp_pdu->gw_cause_info.tp_cause = (uint8)event_info_ptr->submit_report_info.u.error.tp_cause;

      mem_send_resp_pdu->gw_cause_info_valid = TRUE;
    }
    else
    {
      errval = qmi_wmsi_from_wms_report_error(event_info_ptr->submit_report_info.report_status);

      if (QMI_ERR_MESSAGE_DELIVERY_FAILURE == errval)
      {
         /* Message Delivery Failure Type TLV */
        mem_send_resp_pdu->message_delivery_failure_type = (wms_message_delivery_failure_type_enum_v01)
            ( event_info_ptr->submit_report_info.delivery_failure_permanent ?
              WMSI_MESSAGE_DELIVERY_FAILURE_PERM : WMSI_MESSAGE_DELIVERY_FAILURE_TEMP);
        mem_send_resp_pdu->message_delivery_failure_type_valid = TRUE;
      }
    }
  }

  mem_send_resp_pdu->message_id       = (uint16) cmd_node->cache.raw_send.wms_msgid;
  mem_send_resp_pdu->message_id_valid = TRUE;
  
  return errval;

} /* qmi_wmsi_mem_send_resp */

/*===========================================================================
  FUNCTION QMI_WMSI_SEND_RESP()

  DESCRIPTION
    Creates the response message for a raw send WMS command result event.

  PARAMETERS
    raw_send_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_send_resp(
  wms_raw_send_resp_msg_v01 *raw_send_resp_pdu,
  wms_msg_event_info_s_type *event_info_ptr,
  qmi_wms_cmd_node_type     *cmd_node
)
{
  qmi_error_e_type                  errval =  QMI_ERR_NONE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
     return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* check the event report to see if send was successful, if not get error */
  if (WMS_RPT_OK != event_info_ptr->submit_report_info.report_status)
  {
    if (WMS_RPT_CDMA_TL_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      raw_send_resp_pdu->error_class       = (wms_error_class_send_enum_v01)
                                              qmi_wmsi_from_wms_error_class(event_info_ptr->submit_report_info.cause_info.error_class);
      raw_send_resp_pdu->error_class_valid = TRUE;

      raw_send_resp_pdu->cause_code        = qmi_wmsi_from_wms_cause_error(event_info_ptr->submit_report_info.cause_info.tl_status);
      raw_send_resp_pdu->cause_code_valid  = TRUE;
    }
    else if (WMS_RPT_RP_ERROR == event_info_ptr->submit_report_info.report_status)
    {
      errval = QMI_ERR_CAUSE_CODE;

      raw_send_resp_pdu->gw_cause_info.rp_cause = (uint16)event_info_ptr->submit_report_info.cause_info.cause_value;
      raw_send_resp_pdu->gw_cause_info.tp_cause = (uint8)event_info_ptr->submit_report_info.u.error.tp_cause;

      raw_send_resp_pdu->gw_cause_info_valid    = TRUE;
    }
    else
    {
      errval = qmi_wmsi_from_wms_report_error(event_info_ptr->submit_report_info.report_status);

      if (QMI_ERR_MESSAGE_DELIVERY_FAILURE == errval)
      {
        /* Message Delivery Failure Cause TLV */
        if ((WMS_MESSAGE_MODE_GW == event_info_ptr->submit_report_info.message_mode) &&
            (WMS_RPT_MO_CONTROL_DISALLOW == event_info_ptr->submit_report_info.report_status))
        {
          raw_send_resp_pdu->message_delivery_failure_cause       = (wms_message_delivery_failure_cause_enum_v01)WMSI_MESSAGE_BLOCKED_DUE_TO_CALL_CONTROL;
          raw_send_resp_pdu->message_delivery_failure_cause_valid = TRUE;

        }

        /* Message Delivery Failure Type TLV */
        raw_send_resp_pdu->message_delivery_failure_type = (wms_message_delivery_failure_type_enum_v01)
                                                            (event_info_ptr->submit_report_info.delivery_failure_permanent ?
                                                             WMSI_MESSAGE_DELIVERY_FAILURE_PERM :
                                                             WMSI_MESSAGE_DELIVERY_FAILURE_TEMP);
        raw_send_resp_pdu->message_delivery_failure_type_valid = TRUE;
      }
    }
  }

  if (WMS_MO_CTRL_STATUS_MOD == cmd_node->cache.raw_send.mo_ctrl_status)
  {
    raw_send_resp_pdu->call_control_modified_info.alpha_id_len = cmd_node->cache.raw_send.alpha_id_len;

    (void)memscpy(raw_send_resp_pdu->call_control_modified_info.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len,
                  cmd_node->cache.raw_send.alpha_id,
                  cmd_node->cache.raw_send.alpha_id_len);

    raw_send_resp_pdu->call_control_modified_info_valid = TRUE;
  }

  raw_send_resp_pdu->message_id  = (uint16) cmd_node->cache.raw_send.wms_msgid;

  return errval;

} /* qmi_wmsi_send_resp */

#ifndef REMOVE_QMI_WMS_RAW_WRITE_V01
/*===========================================================================
  FUNCTION QMI_WMSI_RAW_WRITE()

  DESCRIPTION
    Writes a given message in raw format to the device memory using the WMS
    service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_raw_write(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_client_message_s_type *client_msg_ptr = NULL;
  wms_message_tag_e_type  tag_type;
#ifdef FEATURE_GWSMS
  uint8 *data_ptr;
#endif /* FEATURE_GWSMS */
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  uint8 sub;

  wms_raw_write_req_msg_v01     *raw_write_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  raw_write_req_pdu = ( wms_raw_write_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_raw_write_req_msg_v01) );

  if (NULL == raw_write_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_raw_write_req_msg_v01, QMI_WMS_RAW_WRITE_REQ_V01, raw_write_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (WMS_FORMAT_MAX == qmi_wmsi_to_wms_format((qmi_wmsi_message_format_e_type)raw_write_req_pdu->raw_message_write_data.format))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* The total length of the TLV should be 4 + the length of the raw data
     (4 = 1 byte storage_type + 1 byte format field + 2 byte len field) */
  if ((0 == raw_write_req_pdu->raw_message_write_data.raw_message_len) ||
     (WMSI_MAX_LEN < raw_write_req_pdu->raw_message_write_data.raw_message_len))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

   /* message tag type */
  /* NOTE: tags must be treated differently than other WMS enums because there
           is no WMS_TAG_MAX included for error checking */
  if (WMSI_TAG_MAX > raw_write_req_pdu->tag_type)
  {
    tag_type = qmi_wmsi_to_wms_tag((qmi_wmsi_tag_e_type)raw_write_req_pdu->tag_type);
  }
  else
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  client_msg_ptr = (wms_client_message_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_message_s_type));
  if (NULL == client_msg_ptr)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    begin by fill in the client message struct info for the message
  -----------------------------------------------------------------------*/
  switch ((qmi_wmsi_message_format_e_type)raw_write_req_pdu->raw_message_write_data.format)
  {
#ifdef FEATURE_GWSMS
    case WMSI_MSG_FORMAT_GW_PP:
      /* fill in message header values */
      client_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      client_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
      client_msg_ptr->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

      /* message format */
      client_msg_ptr->u.gw_message.raw_ts_data.format =
      qmi_wmsi_to_wms_format(
                           (qmi_wmsi_message_format_e_type)raw_write_req_pdu->raw_message_write_data.format);
      /* message length is the length of the PDU minus the length of the SMSC
         address length (guaranteed 1st byte of PDU) minus the actual length of
         the SMSC address */
      if (raw_write_req_pdu->raw_message_write_data.raw_message_len > (uint32)(raw_write_req_pdu->raw_message_write_data.raw_message[0] + 1))
      {
        client_msg_ptr->u.gw_message.raw_ts_data.len = raw_write_req_pdu->raw_message_write_data.raw_message_len - 1 -
                                                  raw_write_req_pdu->raw_message_write_data.raw_message[0];
      }
      else
      {
        errval = QMI_ERR_MALFORMED_MSG;
        goto send_result;
      }
      /* set the write storage type */
      client_msg_ptr->msg_hdr.mem_store =
                        qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                                (raw_write_req_pdu->raw_message_write_data.storage_type),
                                                WMSI_MSG_MODE_GW);

      if ((WMS_FORMAT_MAX       <= client_msg_ptr->u.gw_message.raw_ts_data.format)||
          (WMSI_MAX_LEN         <= client_msg_ptr->u.gw_message.raw_ts_data.len) ||
          (WMS_MEMORY_STORE_MAX <= client_msg_ptr->msg_hdr.mem_store))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      if (0 != raw_write_req_pdu->raw_message_write_data.raw_message[0])
      {
        if ((12 <= raw_write_req_pdu->raw_message_write_data.raw_message[0]) ||
            (2  >  raw_write_req_pdu->raw_message_write_data.raw_message[0]))
        {
          errval = QMI_ERR_INVALID_ARG;
          goto send_result;
        }

        data_ptr = (uint8 *) raw_write_req_pdu->raw_message_write_data.raw_message;
        data_ptr++;
        if (! qmi_wmsi_bin_to_addr(data_ptr,
                                   raw_write_req_pdu->raw_message_write_data.raw_message[0],
                                   &(client_msg_ptr->u.gw_message.sc_address)))
        {
          errval = QMI_ERR_SMSC_ADDR;
          goto send_result;
        }
      }
      else
      {
        MSG_HIGH_0("SMSC address not given, using default");
        sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
        (void)memscpy(&(client_msg_ptr->u.gw_message.sc_address),
                      sizeof(qmi_wmsi_global.gw_template[sub].mo_template.sc_addr),
                      &qmi_wmsi_global.gw_template[sub].mo_template.sc_addr,
                      sizeof(qmi_wmsi_global.gw_template[sub].mo_template.sc_addr));
      }

      data_ptr = (uint8 *) raw_write_req_pdu->raw_message_write_data.raw_message;
      data_ptr += (raw_write_req_pdu->raw_message_write_data.raw_message[0] + 1);

      /* The first two bits of the TPDU are the TPDU type */
      if (WMSI_MSG_TPDU_TYPE_DELIVER == (data_ptr[0] & 0x03))
      {
        client_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_DELIVER;
        if (raw_write_req_pdu->tag_type_valid)
        {
          if (WMS_IS_MT(tag_type))
          {
            client_msg_ptr->msg_hdr.tag = tag_type;
          }
          else
          {
            MSG_HIGH_0("Invalid tag type for MT message");
            errval = QMI_ERR_INVALID_ARG;
            goto send_result;
          }
        }
        else
        {
          client_msg_ptr->msg_hdr.tag = WMS_TAG_MT_NOT_READ;
        }
      }
      else if (WMSI_MSG_TPDU_TYPE_SUBMIT == (data_ptr[0] & 0x03))
      {
        client_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
        if (raw_write_req_pdu->tag_type_valid)
        {
          if (WMS_IS_MO(tag_type))
          {
            client_msg_ptr->msg_hdr.tag = tag_type;
          }
          else
          {
            MSG_HIGH_0("Invalid tag type for MO message");
            errval = QMI_ERR_INVALID_ARG;
            goto send_result;
          }
        }
        else
        {
          client_msg_ptr->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
        }
      }

      (void)memscpy(client_msg_ptr->u.gw_message.raw_ts_data.data,
                    client_msg_ptr->u.gw_message.raw_ts_data.len,
                    data_ptr,
                    client_msg_ptr->u.gw_message.raw_ts_data.len);
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMSI_MSG_FORMAT_CDMA:
      /* fill in message header values */
      client_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      client_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
      client_msg_ptr->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

      /* message format */
      client_msg_ptr->u.cdma_message.raw_ts.format =
      qmi_wmsi_to_wms_format((qmi_wmsi_message_format_e_type)raw_write_req_pdu->raw_message_write_data.format);
      /* message length */
      client_msg_ptr->u.cdma_message.raw_ts.len = raw_write_req_pdu->raw_message_write_data.raw_message_len;
      /* set the write storage type */
      client_msg_ptr->msg_hdr.mem_store =
                        qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                                (raw_write_req_pdu->raw_message_write_data.storage_type),
                                                WMSI_MSG_MODE_CDMA);

      if ((WMS_FORMAT_MAX       <= client_msg_ptr->u.cdma_message.raw_ts.format) ||
          (WMSI_MAX_LEN         <= client_msg_ptr->u.cdma_message.raw_ts.len) ||
          (WMS_MEMORY_STORE_MAX <= client_msg_ptr->msg_hdr.mem_store))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      /* copy the message data buffer */
      (void)memscpy(client_msg_ptr->u.cdma_message.raw_ts.data,
                    raw_write_req_pdu->raw_message_write_data.raw_message_len,
                    raw_write_req_pdu->raw_message_write_data.raw_message,
                    raw_write_req_pdu->raw_message_write_data.raw_message_len);

      errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
        wms_ts_cdma_OTA2cl(raw_write_req_pdu->raw_message_write_data.raw_message, raw_write_req_pdu->raw_message_write_data.raw_message_len, client_msg_ptr));

      if (QMI_ERR_NONE != errval)
      {
        MSG_HIGH_1("qmi_wmsi_raw_write : wms_ts_cdma_OTA2cl returned error [ %d ]", errval);
        goto send_result;
      }

      if (client_msg_ptr->u.cdma_message.is_mo)
      {
        if (raw_write_req_pdu->tag_type_valid)
        {
          if (WMS_IS_MO(tag_type))
          {
            client_msg_ptr->msg_hdr.tag = tag_type;
          }
          else
          {
            MSG_HIGH_0("Invalid tag type for MO message");
            errval = QMI_ERR_INVALID_ARG;
            goto send_result;
          }
        }
        else
        {
          client_msg_ptr->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
        }
      }
      else
      {
        if (raw_write_req_pdu->tag_type_valid)
        {
          if (WMS_IS_MT(tag_type))
          {
            client_msg_ptr->msg_hdr.tag = tag_type;
          }
          else
          {
            MSG_HIGH_0("Invalid tag type for MT message");
            errval = QMI_ERR_INVALID_ARG;
            goto send_result;
          }
        }
        else
        {
          client_msg_ptr->msg_hdr.tag = WMS_TAG_MT_NOT_READ;
        }
      }

      break;
#endif /* FEATURE_CDSMS */

    default:
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_write(qmi_wmsi_global.wms_cid,
                                                       as_id,
                                                       (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                       (void *) cmd_buf_p,
                                                       WMS_WRITE_MODE_INSERT,
                                                       client_msg_ptr
                                                       ));

  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(raw_write_req_pdu);
    QMI_WMSI_MEM_FREEIF(client_msg_ptr);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  
  QMI_WMSI_MEM_FREEIF(raw_write_req_pdu);
  QMI_WMSI_MEM_FREEIF(client_msg_ptr);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_raw_write() */
#endif /* REMOVE_QMI_WMS_RAW_WRITE_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_WRITE_RESP()

  DESCRIPTION
    Creates the response message for a raw write WMS command result event.

  PARAMETERS
    raw_write_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_write_resp(
  wms_raw_write_resp_msg_v01 * raw_write_resp_pdu,
  wms_msg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  raw_write_resp_pdu->storage_index = event_info_ptr->status_info.message.msg_hdr.index;

  return errval;
} /* qmi_wmsi_write_resp */

#ifndef REMOVE_QMI_WMS_RAW_READ_V01
/*===========================================================================
  FUNCTION QMI_WMSI_RAW_READ()

  DESCRIPTION
    Reads a message in raw format using the WMS service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_raw_read(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  uint32                  storage_index = 0;
  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  
  wms_raw_read_req_msg_v01     *raw_read_req_pdu = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p,sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  raw_read_req_pdu = ( wms_raw_read_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_raw_read_req_msg_v01) );

  if (NULL == raw_read_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_raw_read_req_msg_v01, QMI_WMS_RAW_READ_REQ_V01, raw_read_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (raw_read_req_pdu->message_mode_valid)
  {
    /* We got the optional TLV spcifying the message mode. */
    if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) raw_read_req_pdu->message_mode)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    switch ((qmi_wmsi_message_mode_e_type) raw_read_req_pdu->message_mode)
    {
#ifdef FEATURE_GWSMS
      case WMSI_MSG_MODE_GW:
        message_mode = WMSI_MSG_MODE_GW;
        break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case WMSI_MSG_MODE_CDMA:
        message_mode = WMSI_MSG_MODE_CDMA;
        break;
#endif /* FEATURE_CDSMS */

      default:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
        goto send_result;
    }
  }
  else if (WMSI_MSG_MODE_MAX != qmi_wmsi_global.message_mode)
  {
    /* This is a single-mode build.  Use the global message mode. */
    message_mode = qmi_wmsi_global.message_mode;
  }
  else
  {
    /* This is multimode build, and the message_mode TLV was not included. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                         (raw_read_req_pdu->message_memory_storage_identification.storage_type),
                                         message_mode);
  /* set the read storage index */
  storage_index = raw_read_req_pdu->message_memory_storage_identification.storage_index;

  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (WMSI_MESSAGE_LIST_MAX <= storage_index)
  {
    errval = QMI_ERR_INVALID_INDEX;
    goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_read(qmi_wmsi_global.wms_cid,
                                                      as_id,
                                                      (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                      (void *) cmd_buf_p,
                                                      storage_type,
                                                      storage_index
                                                      ));
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(raw_read_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(raw_read_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_raw_read() */
#endif /* REMOVE_QMI_WMS_RAW_READ_V01 */
/*===========================================================================
  FUNCTION QMI_WMSI_READ_RESP()

  DESCRIPTION
    Creates the response message for a raw read WMS command result event.

  PARAMETERS
    raw_read_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_read_resp(
  wms_raw_read_resp_msg_v01 * raw_read_resp_pdu,
  wms_msg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;
  wms_client_message_s_type * evt_msg = NULL;

#ifdef FEATURE_GWSMS
  uint8 * data_ptr;
  uint8   data_offset = 0;
#endif /* FEATURE_GWSMS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  evt_msg = &(event_info_ptr->status_info.message);

  switch (evt_msg->msg_hdr.message_mode)
  {
#ifdef FEATURE_GWSMS
    case WMS_MESSAGE_MODE_GW:
      raw_read_resp_pdu->raw_message_data.tag_type = (wms_message_tag_type_enum_v01)qmi_wmsi_from_wms_tag(evt_msg->msg_hdr.tag);
      raw_read_resp_pdu->raw_message_data.format   = (wms_message_format_enum_v01)qmi_wmsi_from_wms_format(
                                      evt_msg->u.gw_message.raw_ts_data.format);
      raw_read_resp_pdu->raw_message_data.data_len      = (uint16) evt_msg->u.gw_message.raw_ts_data.len;

      /*-----------------------------------------------------------------------
        Check that all of the read message parameters are valid
      -----------------------------------------------------------------------*/
      if (WMS_TPDU_STATUS_REPORT == evt_msg->u.gw_message.raw_ts_data.tpdu_type)
      {
        return QMI_ERR_TPDU_TYPE;
      }

      if ((WMSI_TAG_MAX        <= raw_read_resp_pdu->raw_message_data.tag_type) &&
          (WMSI_MSG_FORMAT_MAX <= raw_read_resp_pdu->raw_message_data.format) &&
          (WMSI_MAX_LEN        <= raw_read_resp_pdu->raw_message_data.data_len))
      {
        return QMI_ERR_INVALID_ARG;
      }

      data_ptr = raw_read_resp_pdu->raw_message_data.data;
      if (!qmi_wmsi_addr_to_bin(&evt_msg->u.gw_message.sc_address, data_ptr))
      {
        return QMI_ERR_SMSC_ADDR;
      }

      data_offset = raw_read_resp_pdu->raw_message_data.data[0] + 1;
      if (WMSI_MAX_LEN >= (raw_read_resp_pdu->raw_message_data.data_len + data_offset))
      {
        data_ptr = raw_read_resp_pdu->raw_message_data.data + data_offset;
        (void)memscpy(data_ptr,
                      raw_read_resp_pdu->raw_message_data.data_len,
                      evt_msg->u.gw_message.raw_ts_data.data,
                      raw_read_resp_pdu->raw_message_data.data_len);
        raw_read_resp_pdu->raw_message_data.data_len += data_offset;
      }

      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMS_MESSAGE_MODE_CDMA:
    {
      uint16 msg_len = 0;
      errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
        wms_ts_cdma_cl2OTA(evt_msg, raw_read_resp_pdu->raw_message_data.data, WMSI_MAX_LEN, &msg_len));
      if (QMI_ERR_NONE != errval)
      {
        MSG_HIGH_1("qmi_wmsi_raw_read : wms_ts_cdma_OTA2cl returned error [ %d ]", errval);
        return errval;
      }

      raw_read_resp_pdu->raw_message_data.tag_type = (wms_message_tag_type_enum_v01)qmi_wmsi_from_wms_tag(evt_msg->msg_hdr.tag);
      raw_read_resp_pdu->raw_message_data.format   = (wms_message_format_enum_v01)qmi_wmsi_from_wms_format(
                                                 evt_msg->u.cdma_message.raw_ts.format);
      raw_read_resp_pdu->raw_message_data.data_len      = msg_len; //(uint16) evt_msg->u.cdma_message.raw_ts.len;

      /*-----------------------------------------------------------------------
        Check that all of the read message parameters are valid
      -----------------------------------------------------------------------*/
      if ((WMSI_TAG_MAX        <= raw_read_resp_pdu->raw_message_data.tag_type) &&
          (WMSI_MSG_FORMAT_MAX <= raw_read_resp_pdu->raw_message_data.format) &&
          (WMSI_MAX_LEN        <= raw_read_resp_pdu->raw_message_data.data_len))
      {
        return QMI_ERR_INVALID_ARG;
      }

     //memcpy(v_out_req_raw.data,
     //        evt_msg->u.cdma_message.raw_ts.data,
     //        v_out_req_raw.len);
    }
      break;

#endif /* FEATURE_CDSMS */

    default:
      return QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }

  return errval;
} /* qmi_wmsi_read_resp */

#ifndef REMOVE_QMI_WMS_MODIFY_TAG_V01
/*===========================================================================
  FUNCTION QMI_WMSI_MODIFY_TAG()

  DESCRIPTION
    Modifies the message status tag of a message in device memory storage to
    the given value using the WMS service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_modify_tag(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  uint32                  storage_index = 0;
  wms_message_tag_e_type  tag_type = WMS_TAG_NONE;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_modify_tag_req_msg_v01     *modify_tag_req_pdu = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  modify_tag_req_pdu = ( wms_modify_tag_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_modify_tag_req_msg_v01) );

  if (NULL == modify_tag_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  errval = DECODE_REQUEST_TLV(wms_modify_tag_req_msg_v01, QMI_WMS_MODIFY_TAG_REQ_V01, modify_tag_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (modify_tag_req_pdu->message_mode_valid)
  {
    /* We got the optional TLV spcifying the message mode. */
    if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) modify_tag_req_pdu->message_mode)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    switch ((qmi_wmsi_message_mode_e_type) modify_tag_req_pdu->message_mode)
    {
#ifdef FEATURE_GWSMS
      case WMSI_MSG_MODE_GW:
        message_mode = WMSI_MSG_MODE_GW;
        break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case WMSI_MSG_MODE_CDMA:
        message_mode = WMSI_MSG_MODE_CDMA;
        break;
#endif /* FEATURE_CDSMS */

      default:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
        goto send_result;
    }
  }
  else if (WMSI_MSG_MODE_MAX != qmi_wmsi_global.message_mode)
  {
    /* This is a single-mode build.  Use the global message mode. */
    message_mode = qmi_wmsi_global.message_mode;
  }
  else
  {
    /* This is multimode build, and the message_mode TLV was not included. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                          (modify_tag_req_pdu->wms_message_tag.storage_type),
                                          message_mode);
  /* set the write storage index */
  storage_index = modify_tag_req_pdu->wms_message_tag.storage_index;

  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (WMSI_MESSAGE_LIST_MAX <= storage_index)
  {
    errval = QMI_ERR_INVALID_INDEX;
    goto send_result;
  }

  /* message tag type */
  /* NOTE: tags must be treated differently than other WMS enums because there
           is no WMS_TAG_MAX included for error checking */
  if (WMSI_TAG_MAX > modify_tag_req_pdu->wms_message_tag.tag_type)
  {
    tag_type = qmi_wmsi_to_wms_tag((qmi_wmsi_tag_e_type)modify_tag_req_pdu->wms_message_tag.tag_type);
  }
  else
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    /*---------------------------------------------------------------------
      Don't allow tag modification between MO and MT types (they are
      encoded differently).  Read the current tag and decide whether to
      process modification in qmi_wms_process_msg_evt().
    ---------------------------------------------------------------------*/
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_read(qmi_wmsi_global.wms_cid,
                                                      as_id,
                                                      (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                      (void *) cmd_buf_p,
                                                      storage_type,
                                                      storage_index
                                                      ));
  }

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                       = (void *) cmd_buf_p;
    cmd_node->cache.modify_tag.new_tag_type = tag_type;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    QMI_WMSI_MEM_FREEIF(modify_tag_req_pdu);

    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(modify_tag_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_modify_tag() */
#endif /* REMOVE_QMI_WMS_MODIFY_TAG_V01 */

#ifndef REMOVE_QMI_WMS_DELETE_V01
/*===========================================================================
  FUNCTION QMI_WMSI_DELETE()

  DESCRIPTION
    Deletes messages from the device memeory storage, matching the specified
    search parameters, using the WMS service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_delete(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  uint32                  storage_index = 0;
  wms_message_tag_e_type  tag_type = WMS_TAG_NONE;
  boolean                 delete_all = TRUE;
  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  
  wms_delete_req_msg_v01     *delete_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  delete_req_pdu = ( wms_delete_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_delete_req_msg_v01) );

  if (NULL == delete_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_delete_req_msg_v01, QMI_WMS_DELETE_REQ_V01, delete_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if ((delete_req_pdu->index_valid) && (delete_req_pdu->tag_type_valid))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (delete_req_pdu->message_mode_valid)
  {
    /* We got the optional TLV spcifying the message mode. */
    if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) delete_req_pdu->message_mode)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    switch ((qmi_wmsi_message_mode_e_type) delete_req_pdu->message_mode)
    {
#ifdef FEATURE_GWSMS
      case WMSI_MSG_MODE_GW:
        message_mode = WMSI_MSG_MODE_GW;
        break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case WMSI_MSG_MODE_CDMA:
        message_mode = WMSI_MSG_MODE_CDMA;
        break;
#endif /* FEATURE_CDSMS */

      default:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
        goto send_result;
    }
  }
  else if (WMSI_MSG_MODE_MAX != qmi_wmsi_global.message_mode)
  {
    /* This is a single-mode build.  Use the global message mode. */
    message_mode = qmi_wmsi_global.message_mode;
  }
  else
  {
    /* This is multimode build, and the message_mode TLV was not included. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                         (delete_req_pdu->storage_type),
                                         message_mode);
  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (delete_req_pdu->index_valid)
  {
    /* set the write storage index */
    storage_index = delete_req_pdu->index;
    if (WMSI_MESSAGE_LIST_MAX <= storage_index)
    {
      errval = QMI_ERR_INVALID_INDEX;
      goto send_result;
    }
    delete_all = FALSE;
  }

  if (delete_req_pdu->tag_type_valid)
  {
    /* message tag type */
    /* NOTE: tags must be treated differently than other WMS enums because there
             is no WMS_TAG_MAX included for error checking */
    if (WMSI_TAG_MAX > delete_req_pdu->tag_type)
    {
      tag_type = qmi_wmsi_to_wms_tag((qmi_wmsi_tag_e_type)delete_req_pdu->tag_type);
    }
    else
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    if (delete_all)
    {
      errval = (qmi_error_e_type)
               qmi_wmsi_from_wms_status(wms_msg_ms_delete_all(qmi_wmsi_global.wms_cid,
                                                              as_id,
                                                              (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                              (void *) cmd_buf_p,
                                                              storage_type,
                                                              tag_type
                                                              ));
    }
    else
    {
      errval = (qmi_error_e_type)
               qmi_wmsi_from_wms_status(wms_msg_ms_delete(qmi_wmsi_global.wms_cid,
                                                          as_id,
                                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                          (void *) cmd_buf_p,
                                                          storage_type,
                                                          storage_index
                                                          ));
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(delete_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(delete_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_delete() */
#endif /* REMOVE_QMI_WMS_DELETE_V01 */

#ifndef REMOVE_QMI_WMS_GET_MESSAGE_PROTOCOL_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_MESSAGE_PROTOCOL()

  DESCRIPTION
    Gets the current message protocol that the QMI_WMS service is running.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_message_protocol(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;

  wms_get_message_protocol_resp_msg_v01 get_message_protocol_resp_pdu;
  void  *response_pdu = NULL;
  boolean retval;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -----------------------*/
  memset(&get_message_protocol_resp_pdu, 0, sizeof(wms_get_message_protocol_resp_msg_v01));
  response_pdu = &get_message_protocol_resp_pdu;
  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == sdu_in) || (NULL == cl_sp))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_message_protocol");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_MSG_MODE_MAX == qmi_wmsi_global.message_mode)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }
  else
  {
    get_message_protocol_resp_pdu.message_protocol = (wms_message_protocol_enum_v01)qmi_wmsi_global.message_mode;
    ENCODE_RESPONSE_TLV(wms_get_message_protocol_resp_msg_v01, QMI_WMS_GET_MESSAGE_PROTOCOL_RESP_V01); 
    return response;
  }
  
send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_get_message_protocol() */
#endif /* REMOVE_QMI_WMS_GET_MESSAGE_PROTOCOL_V01 */

#ifndef REMOVE_QMI_WMS_LIST_MESSAGES_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_LIST()

  DESCRIPTION
    Gets a list of message indexes and corresponding message tags from the
    device memory storage, matching specified search parameters, using the WMS
    service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_list_messages(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  wms_message_tag_e_type  tag_type = WMS_TAG_NONE;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  
  wms_list_messages_req_msg_v01 *list_messages_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  list_messages_req_pdu = ( wms_list_messages_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_list_messages_req_msg_v01) );

  if (NULL == list_messages_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_list_messages_req_msg_v01, QMI_WMS_LIST_MESSAGES_REQ_V01, list_messages_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (list_messages_req_pdu->message_mode_valid)
  {
    /* We got the optional TLV spcifying the message mode. */
    if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) list_messages_req_pdu->message_mode)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    switch ((qmi_wmsi_message_mode_e_type) list_messages_req_pdu->message_mode)
    {
#ifdef FEATURE_GWSMS
      case WMSI_MSG_MODE_GW:
        message_mode = WMSI_MSG_MODE_GW;
        break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case WMSI_MSG_MODE_CDMA:
        message_mode = WMSI_MSG_MODE_CDMA;
        break;
#endif /* FEATURE_CDSMS */

      default:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
        goto send_result;
    }
  }
  else if (WMSI_MSG_MODE_MAX != qmi_wmsi_global.message_mode)
  {
    /* This is a single-mode build.  Use the global message mode. */
    message_mode = qmi_wmsi_global.message_mode;
  }
  else
  {
    /* This is multimode build, and the message_mode TLV was not included. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                         (list_messages_req_pdu->storage_type),
                                         message_mode);
  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (list_messages_req_pdu->tag_type_valid)
  {
    /* message tag type */
    /* NOTE: tags must be treated differently than other WMS enums because there
             is no WMS_TAG_MAX included for error checking */
    if (WMSI_TAG_MAX > list_messages_req_pdu->tag_type)
    {
      tag_type = qmi_wmsi_to_wms_tag((qmi_wmsi_tag_e_type)list_messages_req_pdu->tag_type);
    }
    else
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status (wms_cfg_ms_get_message_list(qmi_wmsi_global.wms_cid,
                                                                   as_id,
                                                                   (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                   (void *) cmd_buf_p,
                                                                   storage_type,
                                                                   tag_type));
  }

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                       = (void *) cmd_buf_p;
    cmd_node->cache.get_list.storage_type   = storage_type;
    cmd_node->cache.get_list.tag_type       = tag_type;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    QMI_WMSI_MEM_FREEIF(list_messages_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(list_messages_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_list_messages() */
#endif /* REMOVE_QMI_WMS_LIST_MESSAGES_V01 */

static int qmi_wmsi_compare_get_list(
  qmi_wms_cmd_node_type * cmd_node,
  wms_cfg_event_info_s_type * evt_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != evt_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_LIST_MESSAGES == cmd_buf_p->cmd_type))
    {
      if ((cmd_node->cache.get_list.storage_type == evt_ptr->message_list.mem_store) &&
          (cmd_node->cache.get_list.tag_type     == evt_ptr->message_list.tag))
      {
        ret = (int) TRUE;
      }
    }
  }

  return ret;
}

static int qmi_wmsi_compare_tid(
  qmi_wms_cmd_node_type * cmd_node,
  qmi_wms_tid_type *  tid
)
{
  int ret = (int) FALSE;

  if (NULL != cmd_node && NULL != tid)
  {
    if (*tid == cmd_node->tid)
    {
      ret = (int) TRUE;
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION QMI_WMSI_GET_LIST_RESP()

  DESCRIPTION
    Creates the response message for a get list WMS command result event.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_list_resp(
  wms_list_messages_resp_msg_v01 * get_list_resp_pdu,
  wms_cfg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  uint32 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Copy and check the message list count
  -----------------------------------------------------------------------*/
  get_list_resp_pdu->message_tuple_len = event_info_ptr->message_list.len;
  if (WMSI_MESSAGE_LIST_MAX <= get_list_resp_pdu->message_tuple_len)
  {
    errval = QMI_ERR_INVALID_ARG;
  }

  /*-----------------------------------------------------------------------
    Copy and check each of the message list values
  -----------------------------------------------------------------------*/
  for (i = 0; (QMI_ERR_NONE == errval) && (i < get_list_resp_pdu->message_tuple_len); i++)
  {
    get_list_resp_pdu->message_tuple[i].tag_type         = (wms_message_tag_type_enum_v01)qmi_wmsi_from_wms_tag(event_info_ptr->message_list.tags[i]);
    get_list_resp_pdu->message_tuple[i].message_index    = event_info_ptr->message_list.indices[i];

    if ((WMSI_TAG_MAX <= get_list_resp_pdu->message_tuple[i].tag_type) ||
        (WMSI_MESSAGE_LIST_MAX <= get_list_resp_pdu->message_tuple[i].message_index))
    {
      errval = QMI_ERR_INVALID_ARG;
    }
  }

  return errval;
} /* qmi_wmsi_get_list_resp */

#ifndef REMOVE_QMI_WMS_SET_ROUTES_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_ROUTES()

  DESCRIPTION
    Sets the routes used by the WMS service to route incomming messages to the
    specified values.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_set_routes(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_routes_s_type        routes;
  wms_message_class_e_type msg_class;
  wms_memory_store_e_type  storage_type;
  wms_route_e_type         route;
  uint32                   i;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;

  qmi_wmsi_client_state_type  *wms_cl_sp          = (qmi_wmsi_client_state_type *)cl_sp;
  wms_set_routes_req_msg_v01  *set_routes_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  for (i = 0; i < WMS_MESSAGE_CLASS_MAX; i++)
  {
    routes.pp_routes[i].route = WMS_ROUTE_NO_CHANGE;
    routes.pp_routes[i].mem_store = WMS_MEMORY_STORE_NONE;
  }

  routes.transfer_status_report = FALSE;

  msg_class    = WMS_MESSAGE_CLASS_MAX;
  storage_type = WMS_MEMORY_STORE_MAX;
  route        = WMS_ROUTE_MAX;
  i = 0;

  set_routes_req_pdu = ( wms_set_routes_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_routes_req_msg_v01) );

  if (NULL == set_routes_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_routes_req_msg_v01, QMI_WMS_SET_ROUTES_REQ_V01, set_routes_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
  -----------------------------------------------------------------------*/
  if ((0 == set_routes_req_pdu->route_list_tuple_len) || (WMSI_MESSAGE_CLASS_MAX < set_routes_req_pdu->route_list_tuple_len))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (set_routes_req_pdu->transfer_ind_valid)
  {
    routes.transfer_status_report = TRUE;
  }

  for (i = 0; (i < set_routes_req_pdu->route_list_tuple_len) && (QMI_ERR_NONE == errval); i++)
  {
    msg_class = qmi_wmsi_to_wms_message_class((qmi_wmsi_message_class_e_type)
                                              set_routes_req_pdu->route_list_tuple[i].message_class);
    switch (msg_class)
    {
      case WMS_MESSAGE_CLASS_0:
      case WMS_MESSAGE_CLASS_1:
      case WMS_MESSAGE_CLASS_2:
      case WMS_MESSAGE_CLASS_3:
      case WMS_MESSAGE_CLASS_NONE:
        storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                               (set_routes_req_pdu->route_list_tuple[i].route_storage),
                                               WMSI_MSG_MODE_GW);
        break;

      case WMS_MESSAGE_CLASS_CDMA:
        storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                               (set_routes_req_pdu->route_list_tuple[i].route_storage),
                                               WMSI_MSG_MODE_CDMA);
        break;

      default:
        storage_type = WMS_MEMORY_STORE_MAX;
        break;
    }
    route = qmi_wmsi_to_wms_route((qmi_wmsi_route_e_type)
                                  set_routes_req_pdu->route_list_tuple[i].receipt_action);
    if ((WMS_MESSAGE_CLASS_MAX <= msg_class) ||
        (WMS_MEMORY_STORE_MAX  <= storage_type) ||
        (WMS_ROUTE_MAX         <= route))
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
    switch (set_routes_req_pdu->route_list_tuple[i].message_type)
    {
      /* set Point-to-Point routing */
      case WMSI_ROUTE_TYPE_PP:
        routes.pp_routes[msg_class].route     = route;
        routes.pp_routes[msg_class].mem_store = storage_type;
        break;

      default:
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status (wms_cfg_ms_set_routes(qmi_wmsi_global.wms_cid,
                                                             as_id,
                                                             (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                             (void *) cmd_buf_p,
                                                             &routes));
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_routes_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_routes_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_set_routes() */
#endif /* REMOVE_QMI_WMS_SET_ROUTES_V01 */

#ifndef REMOVE_QMI_WMS_GET_ROUTES_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_ROUTES()

  DESCRIPTION
    Gets the routes used by the WMS service to route incomming messages on the
    device.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_routes(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status (wms_cfg_ms_get_routes(qmi_wmsi_global.wms_cid,
                                                           as_id,
                                                           (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                           (void *) cmd_buf_p));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }
    cmd_node->cmd_buf                  = (void *) cmd_buf_p;
    /* cache the message request for later retrieval */
    cmd_node->cache.get_routes.pending = TRUE;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);

    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_routes() */
#endif /* REMOVE_QMI_WMS_GET_ROUTES_V01 */

static int qmi_wmsi_compare_get_routes(
  qmi_wms_cmd_node_type * cmd_node,
  wms_cfg_event_info_s_type * evt_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != evt_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_GET_ROUTES == cmd_buf_p->cmd_type))
    {
      if (TRUE == cmd_node->cache.get_routes.pending)
      {
        ret = (int) TRUE;
      }
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION QMI_WMSI_GET_ROUTES_RESP()

  DESCRIPTION
    Creates the response message for a get routes WMS command result event.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_routes_resp(
  wms_get_routes_resp_msg_v01 * get_routes_resp_pdu,
  wms_cfg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  int i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  get_routes_resp_pdu->transfer_ind = (event_info_ptr->routes.transfer_status_report) ? 1 : 0;
  get_routes_resp_pdu->transfer_ind_valid = TRUE;

  /*-----------------------------------------------------------------------
    Zero the route list count, the final length will be determined by the
    number of active routes (routes valid for the current messaging protocol)
  -----------------------------------------------------------------------*/
  get_routes_resp_pdu->route_list_len = 0;

  /*-----------------------------------------------------------------------
    Copy and check each of the message route values
  -----------------------------------------------------------------------*/
  for (i = WMSI_MSG_CLASS_0; (QMI_ERR_NONE == errval) && (WMSI_MSG_CLASS_CDMA >= i); i++)
  {
    get_routes_resp_pdu->route_list[i].route_type       = (wms_message_type_enum_v01)WMSI_ROUTE_TYPE_PP;
    get_routes_resp_pdu->route_list[i].route_class      = (wms_message_class_enum_v01)qmi_wmsi_from_wms_message_class(
                                                   (wms_message_class_e_type)i);
    get_routes_resp_pdu->route_list[i].route_memory     = (wms_storage_type_enum_v01)qmi_wmsi_from_wms_storage(
                                 event_info_ptr->routes.pp_routes[i].mem_store);
    get_routes_resp_pdu->route_list[i].route_value      = (wms_receipt_action_enum_v01)qmi_wmsi_from_wms_route(
                                     event_info_ptr->routes.pp_routes[i].route);

    if ((WMSI_MSG_CLASS_MAX <= get_routes_resp_pdu->route_list[i].route_class) ||
        (WMSI_MEM_STORE_MAX <= (get_routes_resp_pdu->route_list[i].route_memory)))
    {
      errval = QMI_ERR_INVALID_ARG;
    }

    get_routes_resp_pdu->route_list_len += 1;
  }

  return errval;
} /* qmi_wmsi_get_routes_resp */

#ifndef REMOVE_QMI_WMS_GET_SMSC_ADDRESS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_SMSC_ADDR()

  DESCRIPTION
    Reads a message in raw format using the WMS service.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_smsc_addr(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
#ifdef FEATURE_GWSMS
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_NONE;
  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  uint8                   storage_index = 0;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  uint8  sub = 0;

  wms_get_smsc_address_req_msg_v01 	*get_smsc_req_pdu = NULL;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
  }

#ifdef FEATURE_GWSMS
  if (QMI_ERR_NONE == errval && NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
  }
  else
  {
    get_smsc_req_pdu = ( wms_get_smsc_address_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_get_smsc_address_req_msg_v01) );

    if (NULL == get_smsc_req_pdu)
    {
      errval = QMI_ERR_INTERNAL;
    }

    if (QMI_ERR_NONE == errval)
    {
      errval = DECODE_REQUEST_TLV(wms_get_smsc_address_req_msg_v01, QMI_WMS_GET_SMSC_ADDRESS_REQ_V01, get_smsc_req_pdu);
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    if (FALSE == qmi_wmsi_global.gw_template[sub].inited)
    {
      errval = QMI_ERR_DEVICE_NOT_READY;
    }
    else
    {
      if (WMSI_MEM_STORE_MAX == qmi_wmsi_global.gw_template[sub].mem_store)
      {
        errval = QMI_ERR_NOT_PROVISIONED;
      }
    }
  }

  /* Read the SMSC address based on the following rules:
       1. If client provides valid record index, fetch SMSC address from EF-SMSP at
          the index specified by client.
       2. If client does not specify index:
          a. Read SMSC address from index 0 (if no valid record exists) or NV.
          b. Else read SMSC address from the most recent valid record.
  */

  if (QMI_ERR_NONE == errval)
  {
    if (get_smsc_req_pdu->index_valid)
    {
      if (WMSI_MESSAGE_LIST_MAX <= get_smsc_req_pdu->index)
      {
        errval = QMI_ERR_INVALID_INDEX;
      }
      else
      {
        storage_index = get_smsc_req_pdu->index;
        storage_type = WMS_MEMORY_STORE_SIM;
      }
    }
    else
    {
      storage_index = qmi_wmsi_global.gw_template[sub].index;
      storage_type = qmi_wmsi_to_wms_storage(qmi_wmsi_global.gw_template[sub].mem_store, WMSI_MSG_MODE_GW);
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_read_template(qmi_wmsi_global.wms_cid,
                                                               as_id,
                                                               (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                               (void *) cmd_buf_p,
                                                               storage_type,
                                                               storage_index));
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(get_smsc_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }
#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* FEATURE_GWSMS */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(get_smsc_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_smsc_addr() */
#endif /* REMOVE_QMI_WMS_GET_SMSC_ADDRESS_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_SMSC_ADDR_RESP()

  DESCRIPTION
    Creates the response message for a read SMSC address WMS command event.

  PARAMETERS
    get_smsc_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_smsc_addr_resp(
  wms_get_smsc_address_resp_msg_v01 * get_smsc_resp_pdu,
  wms_msg_event_info_s_type * event_info_ptr,
  uint8 sub_index
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;
  uint8 type_temp[WMSI_ADDRESS_TYPE_MAX];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  if (WMS_MESSAGE_MODE_GW == event_info_ptr->status_info.message.msg_hdr.message_mode)
  {
    if (event_info_ptr->status_info.message.u.gw_template.mask & WMS_GW_TEMPLATE_MASK_SC_ADDR)
    {
      if (!qmi_wmsi_addr_to_str(&qmi_wmsi_global.gw_template[sub_index].mo_template.sc_addr,
                                (uint8 *) get_smsc_resp_pdu->smsc_address.smsc_address_digits,
                                (uint8 *) type_temp,
                                (uint8 *) &(get_smsc_resp_pdu->smsc_address.smsc_address_digits_len)))
      {
        /* conversion from binary to string failed, return internal error */
        errval = QMI_ERR_INTERNAL;
      }
      else
      {
        /* copy only the string of the type to the output variable but not the \0 */
        (void)memscpy((void *) get_smsc_resp_pdu->smsc_address.smsc_address_type,
                      (WMSI_ADDRESS_TYPE_MAX - 1),
                      (void *) type_temp,
                      (WMSI_ADDRESS_TYPE_MAX - 1));
      }
    }
    else
    {
      errval = QMI_ERR_NOT_PROVISIONED;
    }
  }
  else
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }

  return errval;
} /* qmi_wmsi_get_smsc_addr_resp */

#ifndef REMOVE_QMI_WMS_SET_SMSC_ADDRESS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_SMSC_ADDR()

  DESCRIPTION
    Sets the routes used by the WMS service to route incomming messages to the
    specified values.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_set_smsc_addr(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

#ifdef FEATURE_GWSMS
  wms_address_s_type        dummy_addr;
  wms_write_mode_e_type     write_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  uint8 sub;

  wms_set_smsc_address_req_msg_v01     *set_smsc_req_pdu = NULL;
#endif /* FEATURE_GWSMS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

#ifdef FEATURE_GWSMS
  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_smsc_req_pdu = ( wms_set_smsc_address_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_smsc_address_req_msg_v01) );

  if (NULL == set_smsc_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_smsc_address_req_msg_v01, QMI_WMS_SET_SMSC_ADDRESS_REQ_V01, set_smsc_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (set_smsc_req_pdu->index_valid && (WMSI_MESSAGE_LIST_MAX <= set_smsc_req_pdu->index) )
  {
    errval = QMI_ERR_INVALID_INDEX;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
  -----------------------------------------------------------------------*/
  memset(&dummy_addr, 0, sizeof(dummy_addr));
  /* if SMSC Type has been supplied then that buffer will be set to the
     v_in_opt1 value, otherwise it will still be set to 0 and ignored in the
     qmi_wmsi_str_to_addr call */
  if (!qmi_wmsi_str_to_addr(&dummy_addr,
                            (uint8 *) set_smsc_req_pdu->smsc_address_digits,
                            (uint8 *) set_smsc_req_pdu->smsc_address_type,
                            QMI_WMS_TEXT_MODE,
                            WMSI_MSG_MODE_GW))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {

    wms_client_message_s_type  *template_msg_ptr = NULL;

    template_msg_ptr = (wms_client_message_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_message_s_type));

    if (NULL == template_msg_ptr)
    {
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }

    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;

    /* Initialize the template to the stored template to preserve the current settings. */
    (void)memscpy(&(template_msg_ptr->u.gw_template),
                  sizeof(wms_gw_template_s_type),
                  &qmi_wmsi_global.gw_template[sub].mo_template,
                  sizeof(wms_gw_template_s_type));

    template_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
    template_msg_ptr->msg_hdr.tag          = WMS_TAG_MO_TEMPLATE;
    template_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_SIM;
    /* do not copy requested template into cache until after processed by
       WMS service (WMS_MSG_EVENT_WRITE_TEMPLATE) */
    template_msg_ptr->u.gw_template.mask |= (WMS_GW_TEMPLATE_MASK_SC_ADDR);
    (void)memscpy(&(template_msg_ptr->u.gw_template.sc_addr),
                  sizeof(dummy_addr),
                  &dummy_addr,
                  sizeof(dummy_addr));

    /* Write the SMSC address based on the following rules:
       1. If client provides valid record index, write SMSC address to the
          index specified by client.
       2. If client does not specify index:
          a. Write SMSC address to index 0 if no valid record exists.
          b. Else write SMSC address to the most recent valid record.
    */
    if (set_smsc_req_pdu->index_valid)
    {
      template_msg_ptr->msg_hdr.index = set_smsc_req_pdu->index;
      write_mode = WMS_WRITE_MODE_REPLACE;
    }
    else
    {
      if (WMSI_MEM_STORE_MAX == qmi_wmsi_global.gw_template[sub].mem_store)
      {
        write_mode = WMS_WRITE_MODE_INSERT;
        template_msg_ptr->msg_hdr.index = 0;
      }
      else
      {
        write_mode = WMS_WRITE_MODE_REPLACE;
        template_msg_ptr->msg_hdr.index = qmi_wmsi_global.gw_template[sub].index;
      }
    }

    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_write_template(qmi_wmsi_global.wms_cid,
                                                                as_id,
                                                                (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                (void *) cmd_buf_p,
                                                                write_mode,
                                                                template_msg_ptr));
    QMI_WMSI_MEM_FREEIF(template_msg_ptr);
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_smsc_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* FEATURE_GWSMS */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_smsc_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_set_smsc_addr() */
#endif /* REMOVE_QMI_WMS_SET_SMSC_ADDRESS_V01 */

#ifndef REMOVE_QMI_WMS_GET_STORE_MAX_SIZE_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_STORE_MAX_SIZE()

  DESCRIPTION
    Retrieves the maximum number of messages that can fit in the given
    memory store.

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_store_max_size(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_wms_cmd_node_type * cmd_node = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_get_store_max_size_req_msg_v01     *get_store_max_size_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  get_store_max_size_req_pdu = ( wms_get_store_max_size_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_get_store_max_size_req_msg_v01) );

  if (NULL == get_store_max_size_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_get_store_max_size_req_msg_v01, QMI_WMS_GET_STORE_MAX_SIZE_REQ_V01, get_store_max_size_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (get_store_max_size_req_pdu->message_mode_valid)
  {
    /* We got the optional TLV spcifying the message mode. */
    if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) get_store_max_size_req_pdu->message_mode)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    switch ((qmi_wmsi_message_mode_e_type) get_store_max_size_req_pdu->message_mode)
    {
#ifdef FEATURE_GWSMS
      case WMSI_MSG_MODE_GW:
        message_mode = WMSI_MSG_MODE_GW;
        break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case WMSI_MSG_MODE_CDMA:
        message_mode = WMSI_MSG_MODE_CDMA;
        break;
#endif /* FEATURE_CDSMS */

      default:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
        goto send_result;
    }
  }
  else if (WMSI_MSG_MODE_MAX != qmi_wmsi_global.message_mode)
  {
    /* This is a single-mode build.  Use the global message mode. */
    message_mode = qmi_wmsi_global.message_mode;
  }
  else
  {
    /* This is multimode build, and the message_mode TLV was not included. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                         (get_store_max_size_req_pdu->storage_type),
                                         message_mode);

  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status(wms_cfg_ms_get_memory_status(qmi_wmsi_global.wms_cid,
                                                                 as_id,
                                                                 (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                 (void *) cmd_buf_p,
                                                                 storage_type,
                                                                 WMS_TAG_NONE));

  if (QMI_ERR_NONE == errval)
  {
    /* Cache the storage type for later retrieval */
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                  = (void *) cmd_buf_p;
    cmd_node->cache.get_store_max_size.storage_type = storage_type;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    QMI_WMSI_MEM_FREEIF(get_store_max_size_req_pdu);

    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(get_store_max_size_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_get_store_max_size() */
#endif /* REMOVE_QMI_WMS_GET_STORE_MAX_SIZE_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_STORE_MAX_SIZE_RESP()

  DESCRIPTION
    Creates the response message for a memory status WMS command result
    event, which is the data source for the QMI get store max size request.

  PARAMETERS
    get_store_max_size_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_store_max_size_resp(
  wms_get_store_max_size_resp_msg_v01 *get_store_max_size_resp_pdu,
  wms_cfg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  get_store_max_size_resp_pdu->free_slots       = event_info_ptr->memory_status.free_slots;
  get_store_max_size_resp_pdu->free_slots_valid = TRUE;

  get_store_max_size_resp_pdu->mem_store_max_size = event_info_ptr->memory_status.max_slots;

  return errval;
} /* qmi_wmsi_get_store_max_size_resp */

static int qmi_wmsi_compare_get_store_max_size(
  qmi_wms_cmd_node_type * cmd_node,
  wms_cfg_event_info_s_type * evt_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != evt_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_GET_STORE_MAX_SIZE == cmd_buf_p->cmd_type))
    {
      /* QMI_WMS_GET_STORE_MAX_SIZE calls wms_cfg_ms_get_memory_status() with a specific storage type
         (UIM/NV) and tag type as WMS_TAG_NONE (free slots). The event received needs to be validated
         with the corresponding QMI request. */
      if ((cmd_node->cache.get_store_max_size.storage_type == evt_ptr->memory_status.mem_store) &&
          (WMS_TAG_NONE == evt_ptr->memory_status.tag))
      {
        ret = (int) TRUE;
      }
    }
  }

  return ret;
}

#if !defined(REMOVE_QMI_WMS_SEND_ACK_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_ACK_V01)
/*===========================================================================
  FUNCTION QMI_WMSI_SEND_ACK()

  DESCRIPTION
    Send SMS ACK

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_send_ack(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            async_send_ack = FALSE;
  wms_ack_info_s_type          *ack_info_ptr   = NULL;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  qmi_wms_tid_type   tid;
  boolean            sms_on_ims = FALSE;

  wms_async_send_ack_req_msg_v01     *send_ack_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  send_ack_req_pdu = ( wms_async_send_ack_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_async_send_ack_req_msg_v01) );

  if (NULL == send_ack_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_async_send_ack_req_msg_v01, QMI_WMS_ASYNC_SEND_ACK_REQ_V01, send_ack_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (WMSI_CMD_VAL_ASYNC_SEND_ACK == ((qmi_cmd_buf_type *)cmd_buf_p)->cmd_type)
  {
    async_send_ack = TRUE;
  }

  ack_info_ptr =(wms_ack_info_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_ack_info_s_type));

  if (NULL == ack_info_ptr)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  ack_info_ptr->transaction_id = send_ack_req_pdu->ack_information.transaction_id;
  if (WMS_MESSAGE_MODE_MAX <= (wms_message_mode_e_type)send_ack_req_pdu->ack_information.message_protocol)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  ack_info_ptr->message_mode = (wms_message_mode_e_type)send_ack_req_pdu->ack_information.message_protocol;

  if (WMS_MESSAGE_MODE_CDMA == ack_info_ptr->message_mode)
  {
    /* used to hold 'success' state as well*/
    ack_info_ptr->u.cdma.error_class = (1 == send_ack_req_pdu->ack_information.success) ? WMS_ERROR_NONE : WMS_ERROR_TEMP;
    if (send_ack_req_pdu->wms_3gpp2_failure_information_valid)
    {
      ack_info_ptr->u.cdma.error_class = (wms_error_class_e_type)send_ack_req_pdu->wms_3gpp2_failure_information.error_class;
      ack_info_ptr->u.cdma.tl_status = (wms_cdma_tl_status_e_type)send_ack_req_pdu->wms_3gpp2_failure_information.tl_status;
    }
  }
  else if (WMS_MESSAGE_MODE_GW == ack_info_ptr->message_mode)
  {
    ack_info_ptr->u.gw.success = (1 == send_ack_req_pdu->ack_information.success) ? TRUE : FALSE;
    if (send_ack_req_pdu->wms_3gpp_failure_information_valid)
    {
      ack_info_ptr->u.gw.rp_cause = (wms_rp_cause_e_type)send_ack_req_pdu->wms_3gpp_failure_information.rp_cause;
      ack_info_ptr->u.gw.u.error.tp_cause = (wms_tp_cause_e_type)send_ack_req_pdu->wms_3gpp_failure_information.tp_cause;
    }
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (send_ack_req_pdu->sms_on_ims_valid)
  {
    sms_on_ims = (1 == send_ack_req_pdu->sms_on_ims) ? TRUE : FALSE;
  }
  else if (qmi_wmsi_mobile_is_ims_registered(as_id))
  {
     sms_on_ims = TRUE;
  }

  tid = qmi_wmsi_get_next_tid();
  if (sms_on_ims)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_ack_ext(qmi_wmsi_global.wms_cid,
                                                     as_id,
                                                     (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                     (void *) tid,
                                                     ack_info_ptr,
                                                     WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS));
  }
  else
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_ack_ext(qmi_wmsi_global.wms_cid,
                                                   as_id,
                                                   (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                   (void *) tid,
                                                   ack_info_ptr,
                                                   WMS_MSG_TRANSPORT_TYPE_CS));
  }

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (NULL == cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }
    cmd_node->cmd_buf = (qmi_cmd_buf_type *) cmd_buf_p;
    cmd_node->tid = tid;
    cmd_node->wms_cl_sp = wms_cl_sp;
    cmd_node->cmd_type = WMSI_CMD_VAL_SEND_ACK;
    cmd_node->async_cmd = FALSE;
    cmd_node->cache.send_ack.got_user_data_tlv = send_ack_req_pdu->user_data_valid;
    if (async_send_ack)
    {
      cmd_node->cache.send_ack.user_data = send_ack_req_pdu->user_data;
      cmd_node->async_cmd = TRUE;
      cmd_node->cmd_type = WMSI_CMD_VAL_ASYNC_SEND_ACK;
      cmd_node->cmd_buf = NULL;
    }

    qmi_wmsi_put_pending_cmd_buf(cmd_node);

    if (!async_send_ack)
    {
      QMI_WMSI_MEM_FREEIF(send_ack_req_pdu);
      QMI_WMSI_MEM_FREEIF(ack_info_ptr);
      return QMI_SVC_RESPONSE_PENDING;
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(send_ack_req_pdu);
  QMI_WMSI_MEM_FREEIF(ack_info_ptr);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_send_sms_ack() */
#endif /* !defined(REMOVE_QMI_WMS_SEND_ACK_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_ACK_V01) */

/*===========================================================================
  FUNCTION QMI_WMSI_ASYNC_ACK_IND()

  DESCRIPTION
    Creates the indication message for a async send ack command.

  PARAMETERS
    send_ack_ind_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_async_ack_ind(
  wms_async_send_ack_ind_msg_v01 *send_ack_ind_pdu,
  wms_msg_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type*  cmd_node
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
     return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  if (WMS_RPT_OK != event_info_ptr->ack_report_info.error_status)
  {
    errval = QMI_ERR_ACK_NOT_SENT;

    if (WMS_MESSAGE_MODE_GW == event_info_ptr->ack_report_info.message_mode)
    {
      send_ack_ind_pdu->failure_cause       = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_ACK_NOT_SENT;
      send_ack_ind_pdu->failure_cause_valid = TRUE;

      if (WMS_RPT_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT == event_info_ptr->ack_report_info.error_status)
      {
        send_ack_ind_pdu->failure_cause = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_NETWORK_RELEASED_LINK;
      }
      else if (WMS_RPT_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT ==
               event_info_ptr->ack_report_info.error_status)
      {
        send_ack_ind_pdu->failure_cause = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_NO_NETWORK_RESPONSE;
      }
    }
  }

  if (cmd_node->cache.send_ack.got_user_data_tlv)
  {
    send_ack_ind_pdu->user_data       = cmd_node->cache.send_ack.user_data;
    send_ack_ind_pdu->user_data_valid = TRUE;
  }

  return errval;
} /* qmi_wmsi_ack_resp */


/*===========================================================================
  FUNCTION QMI_WMSI_ACK_RESP()

  DESCRIPTION
    Creates the response message for a send ack command.

  PARAMETERS
    send_ack_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_ack_resp(
  wms_send_ack_resp_msg_v01 * send_ack_resp_pdu,
  wms_msg_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type     *cmd_node
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr || NULL == cmd_node)
  {
     MSG_ERROR_2( "NULL pointer: event_info_ptr = 0x%x, cmd_node = 0x%x",
                  event_info_ptr,
                  cmd_node );
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  if (WMS_RPT_OK != event_info_ptr->ack_report_info.error_status)
  {
    errval = QMI_ERR_ACK_NOT_SENT;

    if (WMS_MESSAGE_MODE_GW == event_info_ptr->ack_report_info.message_mode)
    {
      send_ack_resp_pdu->failure_cause           = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_ACK_NOT_SENT;
      send_ack_resp_pdu->failure_cause_valid = TRUE;

      if (WMS_RPT_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT == event_info_ptr->ack_report_info.error_status)
      {
        send_ack_resp_pdu->failure_cause = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_NETWORK_RELEASED_LINK;
      }
      else if (WMS_RPT_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT ==
               event_info_ptr->ack_report_info.error_status)
      {
        send_ack_resp_pdu->failure_cause = (wms_ack_failure_cause_enum_v01)WMSI_ACK_FAILURE_NO_NETWORK_RESPONSE;
      }

    }
  }

  return errval;
} /* qmi_wmsi_ack_resp */

#ifndef REMOVE_QMI_WMS_SET_RETRY_PERIOD_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_RETRY_PERIOD()

  DESCRIPTION
    Creates the response message for SMS Send ACK request.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_retry_period(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_set_retry_period_req_msg_v01   *set_retry_period_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_set_retry_period");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_retry_period_req_pdu = ( wms_set_retry_period_req_msg_v01 *) qmi_wmsi_mem_alloc ( sizeof(wms_set_retry_period_req_msg_v01) );

  if (NULL == set_retry_period_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_retry_period_req_msg_v01, QMI_WMS_SET_RETRY_PERIOD_REQ_V01, set_retry_period_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (WMS_MAX_RETRY_PERIOD < set_retry_period_req_pdu->retry_period)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status (wms_dbg_set_retry_period(qmi_wmsi_global.wms_cid,
                                                              (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                              (void *) cmd_buf_p,
                                                              set_retry_period_req_pdu->retry_period));

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_retry_period_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_retry_period_req_pdu);
  CHECK_RETVAL();

  return response;
}
#endif /* REMOVE_QMI_WMS_SET_RETRY_PERIOD_V01 */

#ifndef REMOVE_QMI_WMS_SET_RETRY_INTERVAL_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_RETRY_INTERVAL()

  DESCRIPTION
    Creates the response message for SMS Send ACK request.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_retry_interval(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_set_retry_interval_req_msg_v01	 *set_retry_interval_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_set_retry_interval");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  set_retry_interval_req_pdu = ( wms_set_retry_interval_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_retry_interval_req_msg_v01) );

  if (NULL == set_retry_interval_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_retry_interval_req_msg_v01, QMI_WMS_SET_RETRY_INTERVAL_REQ_V01, set_retry_interval_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  errval = (qmi_error_e_type)
    qmi_wmsi_from_wms_status (wms_dbg_set_retry_interval(qmi_wmsi_global.wms_cid,
                                                         (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                         (void *) cmd_buf_p,
                                                         set_retry_interval_req_pdu->retry_interval));
  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_retry_interval_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_retry_interval_req_pdu);
  CHECK_RETVAL();

  return response;
}
#endif /* REMOVE_QMI_WMS_SET_RETRY_INTERVAL_V01 */

#ifndef REMOVE_QMI_WMS_SET_DC_DISCONNECT_TIMER_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_DC_DISCONNECT_TIMER()

  DESCRIPTION
    Creates the response message for SMS Send ACK request.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_dc_disconnect_timer(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_set_dc_disconnect_timer_req_msg_v01   *set_dc_timer_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_set_dc_disconnect_timer");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_dc_timer_req_pdu = ( wms_set_dc_disconnect_timer_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_dc_disconnect_timer_req_msg_v01) );

  if (NULL == set_dc_timer_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_dc_disconnect_timer_req_msg_v01, QMI_WMS_SET_DC_DISCONNECT_TIMER_REQ_V01, set_dc_timer_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (0 == set_dc_timer_req_pdu->dc_auto_disconn_timer)
  {
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status (
      wms_dc_disable_auto_disconnect(qmi_wmsi_global.wms_cid,
                                    (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                    (void *) cmd_buf_p));
  }
  else
  {
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status (
      wms_dc_enable_auto_disconnect(qmi_wmsi_global.wms_cid,
                                    (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                    (void *) cmd_buf_p,
                                    set_dc_timer_req_pdu->dc_auto_disconn_timer));
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_dc_timer_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_dc_timer_req_pdu);
  CHECK_RETVAL();

  return response;
}
#endif /* REMOVE_QMI_WMS_SET_DC_DISCONNECT_TIMER_V01 */

#ifndef REMOVE_QMI_WMS_SET_MEMORY_STATUS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_MEMORY_STATUS()

  DESCRIPTION
    Sets client's memory status

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_memory_status(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_set_memory_status_req_msg_v01   *set_memory_status_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_memory_status_req_pdu = ( wms_set_memory_status_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_memory_status_req_msg_v01) );

  if (NULL == set_memory_status_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_memory_status_req_msg_v01, QMI_WMS_SET_MEMORY_STATUS_REQ_V01, set_memory_status_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (WMSI_MEMORY_AVAILABLE_MAX <= set_memory_status_req_pdu->memory_available)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_cfg_ms_set_memory_full(qmi_wmsi_global.wms_cid,
                                       as_id,
                                       (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                       (void *) cmd_buf_p,
                                       !set_memory_status_req_pdu->memory_available));

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_memory_status_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_memory_status_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_memory_status */
#endif /* REMOVE_QMI_WMS_SET_MEMORY_STATUS_V01 */

#ifndef REMOVE_QMI_WMS_SET_BROADCAST_ACTIVATION_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_BROADCAST_ACTIVATION()

  DESCRIPTION
    Sets Broadcast Activation.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_broadcast_activation(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            activate_all = FALSE;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type * wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_set_broadcast_activation_req_msg_v01   *set_bc_activation_req_pdu = NULL;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_bc_activation_req_pdu = ( wms_set_broadcast_activation_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_broadcast_activation_req_msg_v01) );

  if (NULL == set_bc_activation_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_broadcast_activation_req_msg_v01, QMI_WMS_SET_BROADCAST_ACTIVATION_REQ_V01, set_bc_activation_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if ((WMS_MESSAGE_MODE_MAX <= set_bc_activation_req_pdu->broadcast_activation_info.message_mode) || (WMSI_BC_PREF_MAX <= set_bc_activation_req_pdu->broadcast_activation_info.bc_activate))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (set_bc_activation_req_pdu->activate_all_valid)
  {
    activate_all = (0 == set_bc_activation_req_pdu->activate_all)? FALSE : TRUE;
  }

  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_bc_ms_set_pref(qmi_wmsi_global.wms_cid,
                               as_id,
                               (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                               (void *) cmd_buf_p,
                               (wms_message_mode_e_type)set_bc_activation_req_pdu->broadcast_activation_info.message_mode,
                               qmi_wmsi_to_wms_bc_pref((qmi_wmsi_bc_pref_e_type)set_bc_activation_req_pdu->broadcast_activation_info.bc_activate,
                                                        activate_all)));

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_bc_activation_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_bc_activation_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_broadcast_activation */
#endif /* REMOVE_QMI_WMS_SET_BROADCAST_ACTIVATION_V01 */

#ifndef REMOVE_QMI_WMS_SET_BROADCAST_CONFIG_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_BROADCAST_CONFIG()

  DESCRIPTION
    Sets Broadcast Configuration.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_broadcast_config(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  uint32 i;

  qmi_wmsi_message_mode_e_type message_mode;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type * wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_set_broadcast_config_req_msg_v01   *set_bc_config_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {    
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_bc_config_req_pdu = ( wms_set_broadcast_config_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_broadcast_config_req_msg_v01) );

  if (NULL == set_bc_config_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_broadcast_config_req_msg_v01, QMI_WMS_SET_BROADCAST_CONFIG_REQ_V01, set_bc_config_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (((WMS_MESSAGE_MODE_GW == (wms_message_mode_e_type) set_bc_config_req_pdu->message_mode) &&
       !set_bc_config_req_pdu->wms_3gpp_broadcast_config_info_valid) ||
      ((WMS_MESSAGE_MODE_CDMA == (wms_message_mode_e_type) set_bc_config_req_pdu->message_mode) &&
       !set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info_valid))
  {
    /* The mode is GW, but we didn't get the GW BC Config info;
       or, the mode is CDMA, but we didn't get the CDMA BC Config info. */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (((WMS_MESSAGE_MODE_GW == (wms_message_mode_e_type) set_bc_config_req_pdu->message_mode) &&
       set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info_valid) ||
      ((WMS_MESSAGE_MODE_CDMA == (wms_message_mode_e_type) set_bc_config_req_pdu->message_mode) &&
       set_bc_config_req_pdu->wms_3gpp_broadcast_config_info_valid))
  {
    /* The mode is GW, but we got the CDMA BC Config info; or the mode is CDMA,
       but we got the GW BC Config info. */
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) set_bc_config_req_pdu->message_mode)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  switch ((qmi_wmsi_message_mode_e_type) set_bc_config_req_pdu->message_mode)
  {
#ifdef FEATURE_GWSMS
    case WMSI_MSG_MODE_GW:
      message_mode = WMSI_MSG_MODE_GW;
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMSI_MSG_MODE_CDMA:
      message_mode = WMSI_MSG_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS */

    default:
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
  }

  if (set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info_valid && WMSI_BC_SMS_TABLE_SIZE < set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info_len)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  
  if (set_bc_config_req_pdu->wms_3gpp_broadcast_config_info_valid && WMSI_BC_SMS_TABLE_SIZE < set_bc_config_req_pdu->wms_3gpp_broadcast_config_info_len)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Get the current BC ids.  Note we cannot call wms_bc_mm_delete_all_services(),
     because it causes side effects (as lower layers believe that BC SMS has
     been disabled).  Instead, get the current BC ids, and delete them. */
  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_bc_ms_get_all_service_ids(qmi_wmsi_global.wms_cid,
                                          as_id,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p,
                                          qmi_wmsi_to_wms_message_mode(message_mode)));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                  = (void *) cmd_buf_p;
    cmd_node->cache.set_bc_config.pending = TRUE;
    cmd_node->cache.set_bc_config.mode = message_mode;

    cmd_node->cache.set_bc_config.num_entries = ( (WMSI_MSG_MODE_GW == message_mode) ? (uint8)set_bc_config_req_pdu->wms_3gpp_broadcast_config_info_len 
		                                                                           : (uint8)set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info_len);

    for (i = 0; i < cmd_node->cache.set_bc_config.num_entries; i++)
    {
      
      if (WMSI_MSG_MODE_GW == message_mode)
      {
        cmd_node->cache.set_bc_config.bc_table[i].selected = set_bc_config_req_pdu->wms_3gpp_broadcast_config_info[i].selected;
        cmd_node->cache.set_bc_config.bc_table[i].u.gw_cb_range.from =
          set_bc_config_req_pdu->wms_3gpp_broadcast_config_info[i].from_service_id;
        cmd_node->cache.set_bc_config.bc_table[i].u.gw_cb_range.to =
          set_bc_config_req_pdu->wms_3gpp_broadcast_config_info[i].to_service_id;
      }
      else if (WMSI_MSG_MODE_CDMA == message_mode)
      {
        cmd_node->cache.set_bc_config.bc_table[i].selected = set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info[i].selected;
        cmd_node->cache.set_bc_config.bc_table[i].u.bc_srv_id.service =
          (wms_service_e_type) set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info[i].service_category;
        cmd_node->cache.set_bc_config.bc_table[i].u.bc_srv_id.language =
          (wms_language_e_type) set_bc_config_req_pdu->wms_3gpp2_broadcast_config_info[i].language;
      }
    }
    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    QMI_WMSI_MEM_FREEIF(set_bc_config_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_bc_config_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_broadcast_config */
#endif /* REMOVE_QMI_WMS_SET_BROADCAST_CONFIG_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_SET_BC_CONFIG_PROCESSING()

  DESCRIPTION
    Finishes the processing for setting the broadcast configuration.

  PARAMETERS
    response       : dsm_item ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_set_bc_config_processing(
  qmi_cmd_buf_type * cmd_buf_p,
  wms_bc_mm_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type * cmd_node_ptr,
  boolean * response_pending
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;
  uint32 i;
  wms_bc_mm_service_info_s_type * bc_mm_table_ptr = NULL;
  wms_bc_mm_srv_id_type         * srv_ids_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  MSG_HIGH_1("In qmi_wmsi_set_bc_config_processing, as_id %d", event_info_ptr->as_id);

  if (event_info_ptr->u.bc_mm_srv_ids.size > 0 || cmd_node_ptr->cache.set_bc_config.num_entries > 0)
  {
    *response_pending = TRUE; //Set the boolean to true indicating that response is pending from WMS
  }

  if (event_info_ptr->u.bc_mm_srv_ids.size > 0)
  {
    /* Allocate memory for the srv ids array */
    srv_ids_ptr = (wms_bc_mm_srv_id_type *) qmi_wmsi_mem_alloc(event_info_ptr->u.bc_mm_srv_ids.size * sizeof(wms_bc_mm_srv_id_type));
    if (NULL == srv_ids_ptr)
    {
      return QMI_ERR_NO_MEMORY;
    }

    for (i = 0; i < event_info_ptr->u.bc_mm_srv_ids.size; i++)
    {
      srv_ids_ptr[i].bc_srv_id = event_info_ptr->u.bc_mm_srv_ids.entries[i].bc_srv_id;
      srv_ids_ptr[i].gw_cb_range = event_info_ptr->u.bc_mm_srv_ids.entries[i].gw_cb_range;
    }

    /* Delete the existing BC Config table. */
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
             (wms_bc_ms_delete_services(qmi_wmsi_global.wms_cid,
                                        event_info_ptr->as_id,
                                        (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                        (void *) cmd_buf_p,
                                        qmi_wmsi_to_wms_message_mode(cmd_node_ptr->cache.set_bc_config.mode),
                                        event_info_ptr->u.bc_mm_srv_ids.size,
                                        srv_ids_ptr));

    if (NULL != srv_ids_ptr)
    {
       qmi_wmsi_mem_free(srv_ids_ptr);
    }
  }

  if (cmd_node_ptr->cache.set_bc_config.num_entries > 0)
  {
    /* Allocate memory for the BC MM table */
    bc_mm_table_ptr = qmi_wmsi_mem_alloc(cmd_node_ptr->cache.set_bc_config.num_entries * sizeof(wms_bc_mm_service_info_s_type));

    if (NULL == bc_mm_table_ptr)
    {
      return QMI_ERR_NO_MEMORY;
    }

    for (i = 0; i < cmd_node_ptr->cache.set_bc_config.num_entries; i++)
    {
      /* Default values not passed in as input */
      bc_mm_table_ptr[i].priority = WMS_PRIORITY_NORMAL;
      bc_mm_table_ptr[i].alert = WMS_BC_ALERT_DEFAULT;
      bc_mm_table_ptr[i].max_messages = 20;
      bc_mm_table_ptr[i].label[0]    = 0;
      bc_mm_table_ptr[i].label_encoding = WMS_ENCODING_GSM_7_BIT_DEFAULT;
      bc_mm_table_ptr[i].selected = cmd_node_ptr->cache.set_bc_config.bc_table[i].selected;

      if (WMSI_MSG_MODE_GW == cmd_node_ptr->cache.set_bc_config.mode)
      {
        bc_mm_table_ptr[i].srv_id.gw_cb_range.from =
        cmd_node_ptr->cache.set_bc_config.bc_table[i].u.gw_cb_range.from;
        bc_mm_table_ptr[i].srv_id.gw_cb_range.to =
        cmd_node_ptr->cache.set_bc_config.bc_table[i].u.gw_cb_range.to;
      }
      else if (WMSI_MSG_MODE_CDMA == cmd_node_ptr->cache.set_bc_config.mode)
      {
        bc_mm_table_ptr[i].srv_id.bc_srv_id.service =
        cmd_node_ptr->cache.set_bc_config.bc_table[i].u.bc_srv_id.service;
        bc_mm_table_ptr[i].srv_id.bc_srv_id.language =
        cmd_node_ptr->cache.set_bc_config.bc_table[i].u.bc_srv_id.language;
      }
    }

    /* Add the new services to the BC Config table */
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
             (wms_bc_ms_add_services(qmi_wmsi_global.wms_cid,
                                     event_info_ptr->as_id,
                                     (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                     (void *) cmd_buf_p,
                                     qmi_wmsi_to_wms_message_mode(cmd_node_ptr->cache.set_bc_config.mode),
                                     cmd_node_ptr->cache.set_bc_config.num_entries,
                                     bc_mm_table_ptr));

    if (NULL != bc_mm_table_ptr)
    {
      qmi_wmsi_mem_free(bc_mm_table_ptr);
    }
  }
  return errval;
} /* qmi_wmsi_set_bc_config_processing */

#ifndef REMOVE_QMI_WMS_GET_BROADCAST_CONFIG_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_BROADCAST_CONFIG()

  DESCRIPTION
    Gets Broadcast Configuration.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_broadcast_config(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_wms_cmd_node_type * cmd_node = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  qmi_wmsi_message_mode_e_type message_mode;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type * wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  wms_get_broadcast_config_req_msg_v01   *get_bc_config_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  get_bc_config_req_pdu = ( wms_get_broadcast_config_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_get_broadcast_config_req_msg_v01) );

  if (NULL == get_bc_config_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_get_broadcast_config_req_msg_v01, QMI_WMS_GET_BROADCAST_CONFIG_REQ_V01, get_bc_config_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) get_bc_config_req_pdu->message_mode)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  switch ((qmi_wmsi_message_mode_e_type) get_bc_config_req_pdu->message_mode)
  {
#ifdef FEATURE_GWSMS
    case WMSI_MSG_MODE_GW:
      message_mode = WMSI_MSG_MODE_GW;
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMSI_MSG_MODE_CDMA:
      message_mode = WMSI_MSG_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS */

    default:
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status(wms_bc_ms_get_pref(qmi_wmsi_global.wms_cid,
                                                       as_id,
                                                       (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                       (void *) cmd_buf_p,
                                                       qmi_wmsi_to_wms_message_mode(message_mode)));

  if (QMI_ERR_NONE != errval)
  {
    /* If there was an error, let the command callback handle it. */
    QMI_WMSI_MEM_FREEIF(get_bc_config_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status(wms_bc_ms_get_table(qmi_wmsi_global.wms_cid,
                                                        as_id,
                                                        (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                        (void *) cmd_buf_p,
                                                        qmi_wmsi_to_wms_message_mode(message_mode)));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                  = (void *) cmd_buf_p;
    cmd_node->cache.get_bc_config.pending = TRUE;
    cmd_node->cache.get_bc_config.mode = message_mode;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    QMI_WMSI_MEM_FREEIF(get_bc_config_req_pdu);

    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(get_bc_config_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_get_broadcast_config() */
#endif /* REMOVE_QMI_WMS_GET_BROADCAST_CONFIG_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_BC_CONFIG_RESP()

  DESCRIPTION
    Creates the response message for a get BC config WMS command result event.

  PARAMETERS
    get_bc_config_resp_pdu        : pdu ptr to begin writing the response message to
    event_info_ptr                : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_bc_config_resp(
  wms_get_broadcast_config_resp_msg_v01 * get_bc_config_resp_pdu,
  wms_bc_mm_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type * cmd_node_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;
  uint32 i;
  uint16 num_instances;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr || NULL == cmd_node_ptr)
  {
    MSG_ERROR_2("Null ptr: event_info %d, cmd_node_ptr %d", event_info_ptr, cmd_node_ptr);
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  if (WMS_BC_PREF_MAX <= cmd_node_ptr->cache.get_bc_config.bc_pref)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  num_instances = MIN(event_info_ptr->u.bc_mm_table.size, WMSI_BC_SMS_TABLE_SIZE);

  if (WMSI_MSG_MODE_CDMA == cmd_node_ptr->cache.get_bc_config.mode)
  {
    get_bc_config_resp_pdu->wms_3gpp2_broadcast_info_valid                               = TRUE;
    get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.activated_ind                       = cmd_node_ptr->cache.get_bc_config.bc_pref;
    get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info_len = num_instances;

    for (i = 0; i < num_instances; i++)
    {
      get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].service_category = (wms_service_category_enum_v01)
      (event_info_ptr->u.bc_mm_table.entries[i].srv_id.bc_srv_id.service);
      get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].language = (wms_language_enum_v01)
      (event_info_ptr->u.bc_mm_table.entries[i].srv_id.bc_srv_id.language);
      if (FALSE == event_info_ptr->u.bc_mm_table.entries[i].selected)
      {
        get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].selected = 0;
      }
      else
      {
        get_bc_config_resp_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].selected = 1;
      }
    }
  }
  else if (WMSI_MSG_MODE_GW == cmd_node_ptr->cache.get_bc_config.mode)
  {
    get_bc_config_resp_pdu->wms_3gpp_broadcast_info_valid                               = TRUE;
    get_bc_config_resp_pdu->wms_3gpp_broadcast_info.activated_ind                       = cmd_node_ptr->cache.get_bc_config.bc_pref;
    get_bc_config_resp_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info_len  = num_instances;

    for (i = 0; i < num_instances; i++)
    {
      get_bc_config_resp_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info[i].from_service_id =
      event_info_ptr->u.bc_mm_table.entries[i].srv_id.gw_cb_range.from;
      get_bc_config_resp_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info[i].to_service_id =
      event_info_ptr->u.bc_mm_table.entries[i].srv_id.gw_cb_range.to;
      if (FALSE == event_info_ptr->u.bc_mm_table.entries[i].selected)
      {
        get_bc_config_resp_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info[i].selected = 0;
      }
      else
      {
        get_bc_config_resp_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info[i].selected = 1;
      }
    }
  }

  return errval;
} /* qmi_wmsi_get_bc_config_resp */

/*===========================================================================
  FUNCTION QMI_WMSI_MEMORY_FULL_IND()

  DESCRIPTION
    Sends the memory full indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    event_ptr     : indication event info pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_memory_full_ind(
  void * cl_sp_ptr,
  wms_cfg_event_info_s_type * event_info_ptr
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;

  wms_memory_full_ind_msg_v01  *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == cl_sp_ptr) || (NULL == event_info_ptr))
  {
    MSG_ERROR_0("Memory full indication passed NULL ptr, ignoring");
    return;
  }

  ind_pdu = ( wms_memory_full_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_memory_full_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_1("Memory full indication for client ID %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  if (WMSI_MEM_STORE_MAX <= (qmi_wmsi_memory_storage_e_type)ind_pdu->memory_full_info.storage_type)
  {
    QMI_WMSI_MEM_FREEIF(ind_pdu);
    return;
  }
  ind_pdu->memory_full_info.storage_type = (wms_storage_type_enum_v01)qmi_wmsi_from_wms_storage(event_info_ptr->ms_memory_full.mem_store);


  ind_pdu->memory_full_info.message_mode = (wms_message_mode_enum_v01)qmi_wmsi_message_mode_from_wms_storage(event_info_ptr->ms_memory_full.mem_store);
  if (WMSI_MSG_MODE_MAX <= ind_pdu->memory_full_info.message_mode)
  {
    QMI_WMSI_MEM_FREEIF(ind_pdu);
    return;
  }

  ENCODE_INDICATION_TLV(wms_memory_full_ind_msg_v01, QMI_WMS_MEMORY_FULL_IND_V01);
  
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_MEMORY_FULL_IND, ind);
  }
} /* qmi_wmsi_memory_full_ind() */

void qmi_wmsi_generate_mem_full_ind(
  wms_cfg_event_info_s_type * event_info_ptr,
  qmi_wmsi_subscription_e_type subscription
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == event_info_ptr)
  {
    return;
  }

  if (WMSI_SUBSCRIPTION_MAX <= subscription)
  {
    MSG_ERROR_0("Invalid subscription type");
    return;
  }

  MSG_HIGH_0("Generating memory full indication");

  /*-------------------------------------------------------------------------
    Report memory full indication to all clients
  -------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid))
    {
      if (subscription == cl_sp->subscription_status.subscription_type)
      {
        qmi_wmsi_memory_full_ind(cl_sp, event_info_ptr);
      }
    }
  }
} /* qmi_wmsi_generate_mem_full_ind */

#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_DOMAIN_PREF()

  DESCRIPTION
    Gets the GW Domain Preference.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_domain_pref(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
#ifdef FEATURE_GWSMS
  qmi_wms_cmd_node_type * cmd_node = NULL;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

#ifdef FEATURE_GWSMS
  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status(wms_cfg_ms_get_gw_domain_pref(qmi_wmsi_global.wms_cid,
                                                                  as_id,
                                                                  (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                  (void *) cmd_buf_p));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf  = (void *) cmd_buf_p;
    /* cache the message request for later retrieval */
    cmd_node->cache.get_domain_pref.pending = TRUE;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);

    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* FEATURE_GWSMS */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_get_domain_pref() */
#endif /* REMOVE_QMI_WMS_GET_DOMAIN_PREF_V01 */

static int qmi_wmsi_compare_get_domain_pref(
  qmi_wms_cmd_node_type * cmd_node,
  wms_cfg_event_info_s_type * evt_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != evt_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if ((cmd_buf_p && (WMSI_CMD_VAL_GET_DOMAIN_PREF == cmd_buf_p->cmd_type)) ||
        (cmd_buf_p && (WMSI_CMD_VAL_GET_DOMAIN_PREF_CONFIG == cmd_buf_p->cmd_type)))
    {
      if (TRUE == cmd_node->cache.get_domain_pref.pending)
      {
        ret = (int) TRUE;
      }
    }
  }

  return ret;
}

/*===========================================================================
  FUNCTION QMI_WMSI_GET_DOMAIN_PREF_CONFIG_RESP()

  DESCRIPTION
    Creates the response message for GW Domain Pref request

  PARAMETERS
    get_domain_pref_config_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr                        : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_domain_pref_config_resp(
  wms_get_domain_pref_config_resp_msg_v01  *get_domain_pref_config_resp_pdu,
  wms_cfg_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type * cmd_node
)
{
  qmi_error_e_type   errval    = QMI_ERR_NONE;
  qmi_cmd_buf_type  *cmd_buf_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (NULL == event_info_ptr) || 
       (NULL == cmd_node)       || 
       (NULL == cmd_node->cmd_buf) )
  {
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  cmd_buf_p = cmd_node->cmd_buf;

  if ( ( WMS_GW_DOMAIN_PREF_MAX <= event_info_ptr->ms_gwl_domain_pref.gw.domain_pref ) ||
       ( SYS_SMS_DOMAIN_PREF_MAX <= event_info_ptr->ms_gwl_domain_pref.lte_domain_pref ) )
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  get_domain_pref_config_resp_pdu->gw_domain_pref       = (wms_domain_pref_enum_v01)event_info_ptr->ms_gwl_domain_pref.gw.domain_pref;
  get_domain_pref_config_resp_pdu->gw_domain_pref_valid = TRUE;

#ifdef FEATURE_DOMAIN_SELECTION
  get_domain_pref_config_resp_pdu->lte_domain_pref       = (uint8)event_info_ptr->ms_gwl_domain_pref.lte_domain_pref;
  get_domain_pref_config_resp_pdu->lte_domain_pref_valid = TRUE;
#endif /* FEATURE_DOMAIN_SELECTION */

  return errval;
} /* qmi_wmsi_get_domain_pref__config_resp */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_DOMAIN_PREF_RESP()

  DESCRIPTION
    Creates the response message for GW Domain Pref request

  PARAMETERS
    get_domain_pref_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr                 : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_domain_pref_resp(
  wms_get_domain_pref_resp_msg_v01  *get_domain_pref_resp_pdu,
  wms_cfg_event_info_s_type * event_info_ptr,
  qmi_wms_cmd_node_type * cmd_node
)
  {
  qmi_error_e_type   errval    = QMI_ERR_NONE;
  qmi_cmd_buf_type  *cmd_buf_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ((NULL == event_info_ptr) || 
      (NULL == cmd_node)       || 
      (NULL == cmd_node->cmd_buf))
  {
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  cmd_buf_p = cmd_node->cmd_buf;

  if (WMS_GW_DOMAIN_PREF_MAX <= event_info_ptr->ms_gwl_domain_pref.gw.domain_pref)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  get_domain_pref_resp_pdu->domain_pref = (wms_domain_pref_enum_v01)event_info_ptr->ms_gwl_domain_pref.gw.domain_pref;  
  
  return errval;
} /* qmi_wmsi_get_domain_pref_resp */

#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_DOMAIN_PREF()

  DESCRIPTION
    Sets the GW Domain Pref.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_domain_pref(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval   = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

#ifdef FEATURE_GWSMS
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  
  wms_set_domain_pref_req_msg_v01  *set_domain_pref_req_pdu = NULL;
#endif /* FEATURE_GWSMS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

#ifdef FEATURE_GWSMS
  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_domain_pref_req_pdu = ( wms_set_domain_pref_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_domain_pref_req_msg_v01) );

  if (NULL == set_domain_pref_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_domain_pref_req_msg_v01, QMI_WMS_SET_DOMAIN_PREF_REQ_V01, set_domain_pref_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (WMS_GW_DOMAIN_PREF_MAX <= set_domain_pref_req_pdu->domain_pref)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Set the GW Domain Preference */
  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_cfg_ms_set_gw_domain_pref(qmi_wmsi_global.wms_cid,
                                          as_id,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p,
                                          (wms_gw_domain_pref_e_type)set_domain_pref_req_pdu->domain_pref));

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(set_domain_pref_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* FEATURE_GWSMS */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_domain_pref_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_domain_pref */
#endif /* REMOVE_QMI_WMS_SET_DOMAIN_PREF_V01 */

#if !defined(REMOVE_QMI_WMS_SEND_FROM_MEM_STORE_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_V01)
/*===========================================================================
  FUNCTION QMI_WMSI_SEND_FROM_MEM_STORE()

  DESCRIPTION
    Sends a message from a memory store

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_send_from_mem_store(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;


  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            sms_on_ims = FALSE;

  wms_memory_store_e_type storage_type = WMS_MEMORY_STORE_MAX;
  uint32                  storage_index = 0;
  qmi_wmsi_message_mode_e_type message_mode;
  wms_client_message_s_type     *client_msg_ptr = NULL;
  qmi_wms_cmd_node_type *       cmd_node = NULL;
  boolean            async_mem_send = FALSE;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  qmi_wms_tid_type   tid;

  
  wms_async_send_from_mem_store_req_msg_v01     *mem_send_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE == ((qmi_cmd_buf_type *)cmd_buf_p)->cmd_type)
  {
    async_mem_send = TRUE;
  }
  mem_send_req_pdu = ( wms_async_send_from_mem_store_req_msg_v01 *) qmi_wmsi_mem_alloc ( sizeof(wms_async_send_from_mem_store_req_msg_v01) );

  if (NULL == mem_send_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
    }

  errval = DECODE_REQUEST_TLV(wms_async_send_from_mem_store_req_msg_v01, QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_REQ_V01, mem_send_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (WMSI_MSG_MODE_MAX <= (qmi_wmsi_message_mode_e_type) mem_send_req_pdu->message_memory_storage_info.message_mode)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  switch ((qmi_wmsi_message_mode_e_type) mem_send_req_pdu->message_memory_storage_info.message_mode)
  {
#ifdef FEATURE_GWSMS
    case WMSI_MSG_MODE_GW:
      message_mode = WMSI_MSG_MODE_GW;
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case WMSI_MSG_MODE_CDMA:
      message_mode = WMSI_MSG_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS */

    default:
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
  }

  /* message storage type */
  storage_type = qmi_wmsi_to_wms_storage((qmi_wmsi_memory_storage_e_type)(uint8)
                                         (mem_send_req_pdu->message_memory_storage_info.storage_type),
                                         message_mode);
  /* set the read storage index */
  storage_index = mem_send_req_pdu->message_memory_storage_info.storage_index;

  if (WMS_MEMORY_STORE_MAX <= storage_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (WMSI_MESSAGE_LIST_MAX <= storage_index)
  {
    errval = QMI_ERR_INVALID_INDEX;
    goto send_result;
  }

  client_msg_ptr = (wms_client_message_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_message_s_type));
  if (NULL == client_msg_ptr)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  client_msg_ptr->msg_hdr.message_mode = qmi_wmsi_to_wms_message_mode(message_mode);
  client_msg_ptr->msg_hdr.tag          = WMS_TAG_NONE;
  client_msg_ptr->msg_hdr.mem_store    = storage_type;
  client_msg_ptr->msg_hdr.index        = storage_index;

  if (QMI_ERR_NONE == errval)
  {
    tid = qmi_wmsi_get_next_tid();

    if (mem_send_req_pdu->sms_on_ims_valid)
    {
      sms_on_ims = (1 == mem_send_req_pdu->sms_on_ims) ? TRUE : FALSE;
    }
    else if (qmi_wmsi_mobile_is_ims_registered(as_id))
    {
      sms_on_ims = TRUE;
    }

    if (sms_on_ims)
    {

       errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_send_ext(qmi_wmsi_global.wms_cid,
                                                      as_id,
                                                      (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                      (void *) tid,
                                                      WMS_SEND_MODE_MEMORY_STORE,
                                                      client_msg_ptr,
                                                      WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS));
    }
    else
    {
       errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_send_ext(qmi_wmsi_global.wms_cid,
                                                      as_id,
                                                      (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                      (void *) tid,
                                                      WMS_SEND_MODE_MEMORY_STORE,
                                                      client_msg_ptr,
                                                      WMS_MSG_TRANSPORT_TYPE_CS));
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    /* cache client_msg */
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf = (qmi_cmd_buf_type *) cmd_buf_p;
    (void)memscpy(&cmd_node->cache.raw_send.client_msg,
                  sizeof(cmd_node->cache.raw_send.client_msg),
                  client_msg_ptr,
                  sizeof(wms_client_message_s_type));

    cmd_node->tid = tid;
    cmd_node->wms_cl_sp = wms_cl_sp;
    cmd_node->cmd_type = WMSI_CMD_VAL_SEND_FROM_MEM_STORE;
    cmd_node->async_cmd = FALSE;
    cmd_node->cache.raw_send.got_user_data_tlv = mem_send_req_pdu->user_data_valid;
    if (async_mem_send)
    {
      cmd_node->cache.raw_send.user_data = mem_send_req_pdu->user_data;
      cmd_node->async_cmd = TRUE;
      cmd_node->cmd_type = WMSI_CMD_VAL_ASYNC_SEND_FROM_MEM_STORE;
      cmd_node->cmd_buf = NULL;
    }

    qmi_wmsi_put_pending_cmd_buf(cmd_node);
    if (!async_mem_send)
    {
      QMI_WMSI_MEM_FREEIF(mem_send_req_pdu);
      QMI_WMSI_MEM_FREEIF(client_msg_ptr);
      return QMI_SVC_RESPONSE_PENDING;
    }
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  
  QMI_WMSI_MEM_FREEIF(mem_send_req_pdu);
  QMI_WMSI_MEM_FREEIF(client_msg_ptr);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_send_from_mem_store */
#endif /* !defined(REMOVE_QMI_WMS_SEND_FROM_MEM_STORE_V01) || !defined(REMOVE_QMI_WMS_ASYNC_SEND_FROM_MEM_STORE_V01) */

qmi_error_e_type qmi_wmsi_get_message_waiting_resp(
  wms_get_message_waiting_resp_msg_v01 * get_msg_waiting_resp_pdu,
  uint8 sub
)
{

uint32  i;
qmi_error_e_type errval = QMI_ERR_NONE;

/*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the indication from the MWI cache */
  get_msg_waiting_resp_pdu->message_waiting_info_len = (uint8)qmi_wmsi_global.mwi[sub].num_mwi;

  for (i =0; (i < get_msg_waiting_resp_pdu->message_waiting_info_len) && (i < WMSI_MWI_MAX); i++)
  {
    get_msg_waiting_resp_pdu->message_waiting_info[i].message_type   = (wmi_wms_message_type_enum_v01)qmi_wmsi_global.mwi[sub].mwi[i].type;
    get_msg_waiting_resp_pdu->message_waiting_info[i].active_ind     = qmi_wmsi_global.mwi[sub].mwi[i].active;
    get_msg_waiting_resp_pdu->message_waiting_info[i].message_count  = (uint8)qmi_wmsi_global.mwi[sub].mwi[i].number_of_messages;
  }

  return errval;
}

static int qmi_wmsi_compare_get_message_waiting(
  qmi_wms_cmd_node_type * cmd_node,
  wms_cfg_event_info_s_type * evt_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != evt_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_GET_MESSAGE_WAITING == cmd_buf_p->cmd_type))
    {
      if (TRUE == cmd_node->cache.get_message_waiting.pending)
      {
        ret = (int) TRUE;
      }
    }
  }
  return ret;
}

#ifndef REMOVE_QMI_WMS_GET_MESSAGE_WAITING_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_MESSAGE_WAITING()

  DESCRIPTION
    Gets the Message Waiting information.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_message_waiting(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  qmi_wms_cmd_node_type * cmd_node = NULL;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  uint8 sub;

  void              *response_pdu = NULL;
  wms_get_message_waiting_resp_msg_v01 get_message_waiting_resp_pdu;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  response_pdu = &get_message_waiting_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_message_waiting()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  sub = (uint8) wms_cl_sp->subscription_status.subscription_type;

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_MWI_STATE_UNINIT == qmi_wmsi_global.mwi[sub].mwi_state || WMSI_MWI_STATE_ERROR == qmi_wmsi_global.mwi[sub].mwi_state)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_cfg_ms_get_message_waiting(qmi_wmsi_global.wms_cid,
                                                         as_id,
                                                         (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                         (void *) cmd_buf_p) );

    if (QMI_ERR_NONE == errval)
    {
      cmd_node = qmi_wmsi_get_cmd_node();
      if (!cmd_node)
      {
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }
      /* cache the message request for later retrieval */
      cmd_node->cmd_buf                  = (void *) cmd_buf_p;
      cmd_node->cache.get_message_waiting.pending = TRUE;
      cmd_node->wms_cl_sp = wms_cl_sp;
      cmd_node->cmd_type = WMSI_CMD_VAL_GET_MESSAGE_WAITING;
      qmi_wmsi_put_pending_cmd_buf(cmd_node);
      return QMI_SVC_RESPONSE_PENDING;
    }
    else
    {
      MSG_ERROR_0("QMI WMS: Msg Waiting info requested, but couldn't post the command to WMS");
      goto send_result;
    }
  }

  errval = qmi_wmsi_get_message_waiting_resp(&get_message_waiting_resp_pdu, sub);
  send_result:
  ENCODE_RESPONSE_TLV(wms_get_message_waiting_resp_msg_v01, QMI_WMS_GET_MESSAGE_WAITING_RESP_V01); 
  return response;
} /* qmi_wmsi_get_message_waiting() */
#endif /* REMOVE_QMI_WMS_GET_MESSAGE_WAITING_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_MESSAGE_WAITING_IND()

  DESCRIPTION
    Sends the message waiting indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    sub           : subscription that has the message waiting

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_message_waiting_ind(
  void *   cl_sp_ptr,
  uint8    sub
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;

  wms_message_waiting_ind_msg_v01  *ind_pdu = NULL;

  uint32 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp_ptr)
  {
    MSG_ERROR_0("Message Waiting indication passed NULL ptr, ignoring");
    return;
  }

  ind_pdu = ( wms_message_waiting_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_message_waiting_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_1("Message Waiting indication for client %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the indication from the MWI cache */
  ind_pdu->message_waiting_info_len = (uint8)qmi_wmsi_global.mwi[sub].num_mwi;

  for (i =0; (i < ind_pdu->message_waiting_info_len) && (i < WMSI_MWI_MAX); i++)
  {
    ind_pdu->message_waiting_info[i].message_type   = (wmi_wms_message_type_enum_v01)qmi_wmsi_global.mwi[sub].mwi[i].type;
    ind_pdu->message_waiting_info[i].active_ind     = qmi_wmsi_global.mwi[sub].mwi[i].active;
    ind_pdu->message_waiting_info[i].message_count  = (uint8)qmi_wmsi_global.mwi[sub].mwi[i].number_of_messages;
  }

  ENCODE_INDICATION_TLV(wms_message_waiting_ind_msg_v01, QMI_WMS_MESSAGE_WAITING_IND_V01);  
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_MESSAGE_WAITING_IND, ind);
  }
} /* qmi_wmsi_message_waiting_ind() */

void qmi_wmsi_generate_msg_wait_ind(
  qmi_wmsi_subscription_e_type subscription
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating message waiting indication");

  /*-------------------------------------------------------------------------
    Report Message Waiting indication to all clients
  -------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid))
    {
      if (subscription == cl_sp->subscription_status.subscription_type)
      {
        qmi_wmsi_message_waiting_ind(cl_sp, (uint8) subscription);
      }
    }
  }
} /* qmi_wmsi_generate_msg_wait_ind */

#ifndef REMOVE_QMI_WMS_SET_PRIMARY_CLIENT_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_PRIMARY_CLIENT()

  DESCRIPTION
    Set a client as the primary client of QMI WMS.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_primary_client(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  qmi_wms_cmd_node_type * cmd_node = NULL;

    
  wms_set_primary_client_req_msg_v01  *set_primary_client_req_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_primary_client_req_pdu = ( wms_set_primary_client_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_primary_client_req_msg_v01) );

  if (NULL == set_primary_client_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_primary_client_req_msg_v01, QMI_WMS_SET_PRIMARY_CLIENT_REQ_V01, set_primary_client_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/
  if (WMSI_PRIMARY_CLIENT_MAX <= set_primary_client_req_pdu->primary_client)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_cfg_ms_set_primary_client(qmi_wmsi_global.wms_cid,
                                          as_id,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p,
                                          set_primary_client_req_pdu->primary_client,
                                          set_primary_client_req_pdu->primary_client));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                       = (void *) cmd_buf_p;
    cmd_node->cache.primary_client.enabled  = set_primary_client_req_pdu->primary_client;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);

    QMI_WMSI_MEM_FREEIF(set_primary_client_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_primary_client_req_pdu);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_primary_client */
#endif /* REMOVE_QMI_WMS_SET_PRIMARY_CLIENT_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_SMSC_ADDR_IND()

  DESCRIPTION
    Sends the SMSC address changed indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    gw_template   : indication event info pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_smsc_addr_ind(
  void *   cl_sp_ptr,
  wms_gw_template_s_type * gw_template
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;

  uint8 type_temp[WMSI_ADDRESS_TYPE_MAX];
  wms_smsc_address_ind_msg_v01 *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == cl_sp_ptr) || (NULL == gw_template))
  {
    MSG_ERROR_0("SMSC address indication passed NULL ptr, return");
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  ind_pdu = ( wms_smsc_address_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_smsc_address_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  MSG_HIGH_1("SMSC address indication for client %d", cl_sp->common.clid);

  if (!qmi_wmsi_addr_to_str(&gw_template->sc_addr,
                            (uint8 *) ind_pdu->smsc_address.smsc_address_digits,
                            (uint8 *) type_temp,
                            (uint8 *) &(ind_pdu->smsc_address.smsc_address_digits_len)))
  {
    /* conversion from binary to string failed, return internal error */
    MSG_HIGH_0("qmi_wmsi_addr_to_str failed for SMSC addr IND");
  }
  else
  {
    /* copy only the string of the type to the output variable but not the \0 */
    (void)memscpy((void *) ind_pdu->smsc_address.smsc_address_type,
                  (WMSI_ADDRESS_TYPE_MAX - 1),
                  (void *) type_temp,
                  (WMSI_ADDRESS_TYPE_MAX - 1));

    ENCODE_INDICATION_TLV(wms_smsc_address_ind_msg_v01, QMI_WMS_SMSC_ADDRESS_IND_V01);
  }

  QMI_WMSI_MEM_FREEIF(ind_pdu);
  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_SMSC_ADDR_IND, ind);
  }
} /* qmi_wmsi_smsc_addr_ind() */


/*===========================================================================
  FUNCTION QMI_WMSI_GENERATE_SMSC_ADDR_IND()

  DESCRIPTION
    Sends the SMSC address changed indication to the client

  PARAMETERS
    gw_template   : pointer to GW template struct
    sub_index     : index of the subscription that the template is for

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_generate_smsc_addr_ind(
  wms_gw_template_s_type * gw_template,
  uint8 sub_index
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating SMSC address changed indication");

  /* make sure the number_of_digits is valid (less than the digits[] max size)*/
  if (WMSI_ADDRESS_MAX <= gw_template->sc_addr.number_of_digits)
  {
    MSG_ERROR_2( "SMSC address number_of_digits (%d) greater than max (%d)",
                 gw_template->sc_addr.number_of_digits,
                 WMSI_ADDRESS_MAX );
    return;
  }

  /*-------------------------------------------------------------------------
    Report SMSC address changed indication to all clients
  -------------------------------------------------------------------------*/
  MSG_HIGH_0("SMSC address changed - send IND");
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
         sub_index == (uint8) (cl_sp->subscription_status.subscription_type))
    {
      qmi_wmsi_smsc_addr_ind(cl_sp, gw_template);
    }
  }
} /* qmi_wmsi_generate_smsc_addr_ind */

#ifndef REMOVE_QMI_WMS_INDICATION_REGISTER_V01
/*===========================================================================
  FUNCTION QMI_WMSI_INDICATION_REGISTER()

  DESCRIPTION
    Sets the registration state for different QMI WMS indications for the
    requesting control point.

  PARAMETERS
    sp      : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_indication_register(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *               response  = NULL;
  qmi_wmsi_client_state_type *  wms_cl_sp = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
   
  wms_indication_register_req_msg_v01  *set_indication_reg_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_indication_register()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_indication_reg_req_pdu = ( wms_indication_register_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_indication_register_req_msg_v01) );

  if (NULL == set_indication_reg_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_indication_register_req_msg_v01, QMI_WMS_INDICATION_REGISTER_REQ_V01, set_indication_reg_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (set_indication_reg_req_pdu->reg_transport_layer_info_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_transport_layer_info_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_transport_nw_reg_info_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_transport_nw_reg_info_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_call_status_info_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_call_status_info_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_service_ready_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_service_ready_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_broadcast_config_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_broadcast_config_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_transport_layer_mwi_info_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_transport_layer_mwi_info_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if (set_indication_reg_req_pdu->reg_sim_ready_info_events_valid)
  {
    if (WMSI_INDICATION_MAX <= set_indication_reg_req_pdu->reg_sim_ready_info_events)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }
  if (set_indication_reg_req_pdu->reg_transport_layer_info_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_transport_layer_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_layer_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_transport_layer_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_layer_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_transport_nw_reg_info_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_transport_nw_reg_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_nw_reg_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_transport_nw_reg_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_nw_reg_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_call_status_info_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_call_status_info_events)
    {
      wms_cl_sp->indication_reg.reg_call_status_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_call_status_info_events)
    {
      wms_cl_sp->indication_reg.reg_call_status_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_service_ready_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_service_ready_events)
    {
      wms_cl_sp->indication_reg.reg_service_ready_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_service_ready_events)
    {
      wms_cl_sp->indication_reg.reg_service_ready_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_broadcast_config_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_broadcast_config_events)
    {
      wms_cl_sp->indication_reg.reg_broadcast_config_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_broadcast_config_events)
    {
      wms_cl_sp->indication_reg.reg_broadcast_config_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_transport_layer_mwi_info_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_transport_layer_mwi_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_layer_mwi_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_transport_layer_mwi_info_events)
    {
      wms_cl_sp->indication_reg.reg_transport_layer_mwi_events = TRUE;
    }
  }

  if (set_indication_reg_req_pdu->reg_sim_ready_info_events_valid)
  {
    if (WMSI_INDICATION_DISABLE == set_indication_reg_req_pdu->reg_sim_ready_info_events)
    {
      wms_cl_sp->indication_reg.reg_sim_ready_events = FALSE;
    }
    else if (WMSI_INDICATION_ENABLE == set_indication_reg_req_pdu->reg_sim_ready_info_events)
    {
      wms_cl_sp->indication_reg.reg_sim_ready_events = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_indication_reg_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_indication_register() */
#endif /* REMOVE_QMI_WMS_INDICATION_REGISTER_V01 */

#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_LAYER_INFO_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_TRANSPORT_LAYER_INFO()

  DESCRIPTION
    Gets the transport layer information.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_transport_layer_info(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *             response  = NULL;
  qmi_error_e_type            errval    = QMI_ERR_NONE;
  qmi_result_e_type           result;
  qmi_wmsi_client_state_type *wms_cl_sp = NULL;
  uint8                       sub;

  wms_get_transport_layer_resp_msg_v01     transport_layer_info_resp_pdu;
  void                                    *response_pdu = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&transport_layer_info_resp_pdu, 0, sizeof(wms_get_transport_layer_resp_msg_v01));
  response_pdu = &transport_layer_info_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_transport_layer_info");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (NULL != wms_cl_sp)
  {
    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
  }
  else
  {
    MSG_ERROR_0("NULL client pointer");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  if (WMSI_TRANSPORT_LAYER_STATE_UNINIT == qmi_wmsi_global.transport_layer.state)
  {
    MSG_ERROR_0("QMI WMS: Transport Layer info requested, but Transport Layer info is unavailable");
    errval = QMI_ERR_DEVICE_NOT_READY;
    goto send_result;
  }

  if (qmi_wms_get_subscription_from_asid( qmi_wmsi_global.transport_layer.as_id ) != sub)
  {
    MSG_ERROR_0("QMI WMS: Transport Layer info requested, but Transport Layer info is unavailable on this sub");
    errval = QMI_ERR_DEVICE_NOT_READY;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the response from the Transport Layer cache */
  if (qmi_wmsi_global.transport_layer.registered_ind)
  {
    transport_layer_info_resp_pdu.transport_layer_info_valid          = TRUE;
    transport_layer_info_resp_pdu.transport_layer_info.transport_type = (uint8)qmi_wmsi_global.transport_layer.transport_type;
    transport_layer_info_resp_pdu.transport_layer_info.transport_cap  = (uint8)qmi_wmsi_global.transport_layer.transport_cap;
  }

  transport_layer_info_resp_pdu.registered_ind       = qmi_wmsi_global.transport_layer.registered_ind;
  transport_layer_info_resp_pdu.registered_ind_valid = TRUE;


send_result:

  ENCODE_RESPONSE_TLV(wms_get_transport_layer_resp_msg_v01, QMI_WMS_GET_TRANSPORT_LAYER_INFO_RESP_V01);  

  return response;
} /* qmi_wmsi_get_transport_layer_info() */
#endif /* REMOVE_QMI_WMS_GET_TRANSPORT_LAYER_INFO_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_TRANSPORT_LAYER_INFO_IND()

  DESCRIPTION
    Sends the Transport Layer Info indication to the client

  PARAMETERS
    cl_sp         : client state pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_transport_layer_info_ind(
  void * cl_sp_ptr
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;
  wms_transport_layer_info_ind_msg_v01  *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp_ptr)
  {
    MSG_ERROR_0("Transport Layer Info indication passed NULL ptr, return");
    return;
  }

  ind_pdu = ( wms_transport_layer_info_ind_msg_v01 *) qmi_wmsi_mem_alloc ( sizeof(wms_transport_layer_info_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_1("Transport Layer Info indication for client ID %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the indication from the Transport Layer cache */
  if (qmi_wmsi_global.transport_layer.registered_ind)
  {
    ind_pdu->transport_layer_info_valid          = TRUE;
    ind_pdu->transport_layer_info.transport_type = (uint8)qmi_wmsi_global.transport_layer.transport_type;
    ind_pdu->transport_layer_info.transport_cap  = (uint8)qmi_wmsi_global.transport_layer.transport_cap;
  }

  ind_pdu->registered_ind = qmi_wmsi_global.transport_layer.registered_ind;
  
  ENCODE_INDICATION_TLV(wms_transport_layer_info_ind_msg_v01, QMI_WMS_TRANSPORT_LAYER_INFO_IND_V01);
  
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_TRANSPORT_LAYER_INFO_IND, ind);
  }
} /* qmi_wmsi_transport_layer_info_ind() */

void qmi_wmsi_generate_transport_layer_info_ind(
  void
)
{
  uint8 cl_idx;
  qmi_wmsi_subscription_e_type subscription = 
    qmi_wms_get_subscription_from_asid(qmi_wmsi_global.transport_layer.as_id);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating Transport Layer info indication");

  /*---------------------------------------------------------------------------------
    Report Transport Layer Info indication to all clients that have registered for it
  ----------------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
        (cl_sp->subscription_status.subscription_type == subscription))
    {
      if (cl_sp->indication_reg.reg_transport_layer_events)
      {
        qmi_wmsi_transport_layer_info_ind(cl_sp);
      }
    }
  }
} /* qmi_wmsi_generate_transport_layer_info_ind */

#ifndef REMOVE_QMI_WMS_GET_TRANSPORT_NW_REG_INFO_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_TRANSPORT_NW_REG_INFO()

  DESCRIPTION
    Gets the transport nw reg information.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_transport_nw_reg_info(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *             response  = NULL;
  qmi_error_e_type            errval    = QMI_ERR_NONE;
  qmi_result_e_type           result;
  void                       *response_pdu = NULL;
  qmi_wmsi_client_state_type *wms_cl_sp = NULL;
  uint8                       sub;

  wms_get_transport_nw_reg_resp_msg_v01 get_transport_nw_reg_resp_pdu;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&get_transport_nw_reg_resp_pdu, 0 , sizeof(wms_get_transport_nw_reg_resp_msg_v01));
  response_pdu = &get_transport_nw_reg_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_transport_nw_reg_info()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (NULL != wms_cl_sp)
  {
    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
  }
  else
  {
    MSG_ERROR_0("NULL client pointer");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_TRANSPORT_NW_REG_STATE_UNINIT == qmi_wmsi_global.transport_nw_reg_status.state)
  {
    MSG_ERROR_0("QMI WMS: Transport NW Reg info requested, but Transport NW Reg info is unavailable");
    errval = QMI_ERR_DEVICE_NOT_READY;
    goto send_result;
  }

  if (qmi_wms_get_subscription_from_asid( qmi_wmsi_global.transport_nw_reg_status.as_id ) != sub)
  {
    MSG_ERROR_0("QMI WMS: Transport NW Reg info requested, but Transport NW Reg info is unavailable on this sub");
    errval = QMI_ERR_DEVICE_NOT_READY;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the response from the Transport NW Reg Info cache */
  get_transport_nw_reg_resp_pdu.transport_nw_reg_status_valid = TRUE;
  get_transport_nw_reg_resp_pdu.transport_nw_reg_status       = (uint8)qmi_wmsi_global.transport_nw_reg_status.nw_reg_status;

send_result:

  
  ENCODE_RESPONSE_TLV(wms_get_transport_nw_reg_resp_msg_v01, QMI_WMS_GET_TRANSPORT_NW_REG_INFO_RESP_V01);  

  return response;
} /* qmi_wmsi_get_transport_nw_reg_info() */
#endif /* REMOVE_QMI_WMS_GET_TRANSPORT_NW_REG_INFO_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_TRANSPORT_NW_REG_INFO_IND()

  DESCRIPTION
    Sends the Transport NW Reg Info indication to the client

  PARAMETERS
    cl_sp         : client state pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_transport_nw_reg_info_ind(
  void *   cl_sp_ptr
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;

  wms_transport_nw_reg_info_ind_msg_v01  *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp_ptr)
  {
    MSG_ERROR_0("Transport NW Reg Info indication passed NULL ptr, return");
    return;
  }

  ind_pdu = ( wms_transport_nw_reg_info_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_transport_nw_reg_info_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_1("Transport NW Reg Info indication for client ID %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  /* Build the indication from the Transport NW Reg Info cache */
  ind_pdu->transport_nw_reg_status = (uint8)qmi_wmsi_global.transport_nw_reg_status.nw_reg_status;
  ENCODE_INDICATION_TLV(wms_transport_nw_reg_info_ind_msg_v01, QMI_WMS_TRANSPORT_NW_REG_INFO_IND_V01);

  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_TRANSPORT_NW_REG_INFO_IND, ind);
  }
} /* qmi_wmsi_transport_nw_reg_info_ind() */

void qmi_wmsi_generate_transport_nw_reg_info_ind(
  void
)
{
  uint8 cl_idx;
  qmi_wmsi_subscription_e_type subscription = 
    qmi_wms_get_subscription_from_asid(qmi_wmsi_global.transport_nw_reg_status.as_id);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating Transport NW Reg info indication");

  /*-------------------------------------------------------------------------------
    Report Transport NW Reg indication to all clients that have registered for it
  ---------------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
        (cl_sp->subscription_status.subscription_type == subscription))
    {
      if (cl_sp->indication_reg.reg_transport_nw_reg_events)
      {
        qmi_wmsi_transport_nw_reg_info_ind(cl_sp);
      }
    }
  }
} /* qmi_wmsi_generate_transport_nw_reg_info_ind */

#ifndef REMOVE_QMI_WMS_BIND_SUBSCRIPTION_V01
/*===========================================================================
  FUNCTION QMI_WMSI_BIND_SUBSCRIPTION()

  DESCRIPTION
    Bind the current control point to a specific subscription

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_bind_subscription(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *               response = NULL;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_bind_subscription_req_msg_v01     *bind_sub_req_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_bind_subscription");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  bind_sub_req_pdu = ( wms_bind_subscription_req_msg_v01 *) qmi_wmsi_mem_alloc ( sizeof(wms_bind_subscription_req_msg_v01) );

  if (NULL == bind_sub_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_bind_subscription_req_msg_v01, QMI_WMS_BIND_SUBSCRIPTION_REQ_V01, bind_sub_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (WMSI_SUBSCRIPTION_MAX <= bind_sub_req_pdu->subs_type)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    wms_cl_sp->subscription_status.subscription_type =
      (qmi_wmsi_subscription_e_type) bind_sub_req_pdu->subs_type;

    if (WMSI_MWI_STATE_INIT == qmi_wmsi_global.mwi[wms_cl_sp->subscription_status.subscription_type].mwi_state)
    {
      // Send MWI indication to client.
      MSG_HIGH("QMI_WMS sends missed MWI indication", 0, 0, 0);
      qmi_wmsi_message_waiting_ind(wms_cl_sp, (uint8)wms_cl_sp->subscription_status.subscription_type);
    }
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  QMI_WMSI_MEM_FREEIF(bind_sub_req_pdu);

  return response;

} /* qmi_wmsi_bind_subscription() */
#endif /* REMOVE_QMI_WMS_BIND_SUBSCRIPTION_V01 */

#ifndef REMOVE_QMI_WMS_GET_INDICATION_REGISTER_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_INDICATION_REGISTER()

  DESCRIPTION
    Gets the registration state for different QMI WMS indications for the
    requesting control point.

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_indication_register(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  qmi_wmsi_client_state_type *  wms_cl_sp = NULL;


  wms_get_indication_register_resp_msg_v01 get_ind_reg_resp_pdu;
  void *response_pdu = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  response_pdu = (void *)&get_ind_reg_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_indication_register");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  get_ind_reg_resp_pdu.reg_sim_ready_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_sim_ready_events       = wms_cl_sp->indication_reg.reg_sim_ready_events;

  get_ind_reg_resp_pdu.reg_transport_layer_mwi_info_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_transport_layer_mwi_info_events       = wms_cl_sp->indication_reg.reg_transport_layer_mwi_events;

  get_ind_reg_resp_pdu.reg_broadcast_config_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_broadcast_config_events       = wms_cl_sp->indication_reg.reg_broadcast_config_events;

  get_ind_reg_resp_pdu.reg_service_ready_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_service_ready_events       = wms_cl_sp->indication_reg.reg_service_ready_events;


  get_ind_reg_resp_pdu.reg_call_status_info_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_call_status_info_events       = wms_cl_sp->indication_reg.reg_call_status_events;

  get_ind_reg_resp_pdu.reg_transport_nw_reg_info_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_transport_nw_reg_info_events       = wms_cl_sp->indication_reg.reg_transport_nw_reg_events;

  get_ind_reg_resp_pdu.reg_transport_layer_info_events_valid = TRUE;
  get_ind_reg_resp_pdu.reg_transport_layer_info_events       = wms_cl_sp->indication_reg.reg_transport_layer_events;

send_result:
  ENCODE_RESPONSE_TLV(wms_get_indication_register_resp_msg_v01, QMI_WMS_GET_INDICATION_REGISTER_RESP_V01);

  return response;

} /* qmi_wmsi_get_indication_register() */
#endif /* REMOVE_QMI_WMS_GET_INDICATION_REGISTER_V01 */

#ifndef REMOVE_QMI_WMS_GET_SMS_PARAMETERS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_SMS_PARAMETERS()

  DESCRIPTION
    Read the SMS parameters from EF-SMSP

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_sms_parameters(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  uint8  sub;

  
  wms_get_sms_parameters_req_msg_v01 *get_sms_param_req_pdu = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  get_sms_param_req_pdu = ( wms_get_sms_parameters_req_msg_v01 *) qmi_wmsi_mem_alloc ( sizeof(wms_get_sms_parameters_req_msg_v01) );

  if (NULL == get_sms_param_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_get_sms_parameters_req_msg_v01, QMI_WMS_GET_SMS_PARAMETERS_REQ_V01, get_sms_param_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (WMSI_MSG_MODE_GW != (qmi_wmsi_message_mode_e_type) get_sms_param_req_pdu->message_mode)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (QMI_ERR_NONE == errval)
  {
    if (FALSE == qmi_wmsi_global.gw_template[sub].inited)
    {
      errval = QMI_ERR_DEVICE_NOT_READY;
      goto send_result;
    }
    else
    {
      if (WMSI_MEM_STORE_MAX == qmi_wmsi_global.gw_template[sub].mem_store)
      {
        errval = QMI_ERR_NOT_PROVISIONED;
        goto send_result;
      }
    }
  }

  if (QMI_ERR_NONE == errval)
  {
    errval = (qmi_error_e_type)
             qmi_wmsi_from_wms_status(wms_msg_ms_read_template(qmi_wmsi_global.wms_cid,
                                                               as_id,
                                                               (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                               (void *) cmd_buf_p,
                                                               qmi_wmsi_to_wms_storage(qmi_wmsi_global.gw_template[sub].mem_store,
                                                                                       (qmi_wmsi_message_mode_e_type) get_sms_param_req_pdu->message_mode),
                                                               qmi_wmsi_global.gw_template[sub].index));
  }

  if (QMI_ERR_NONE == errval)
  {
    QMI_WMSI_MEM_FREEIF(get_sms_param_req_pdu);
    return QMI_SVC_RESPONSE_PENDING;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(get_sms_param_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_sms_parameters() */
#endif /* REMOVE_QMI_WMS_GET_SMS_PARAMETERS_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_SMS_PARAMETERS_RESP()

  DESCRIPTION
    Creates the response message for a read SMS parameters WMS command event.

  PARAMETERS
    get_sms_params_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_sms_parameters_resp(
  wms_get_sms_parameters_resp_msg_v01 *get_sms_params_resp_pdu,
  wms_msg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  if (WMS_MESSAGE_MODE_GW == event_info_ptr->status_info.message.msg_hdr.message_mode)
  {
    if (event_info_ptr->status_info.message.u.gw_template.mask & WMS_GW_TEMPLATE_MASK_VALIDITY)
    {
      get_sms_params_resp_pdu->validity_valid = TRUE;
      get_sms_params_resp_pdu->validity       = wms_ts_encode_relative_time(&event_info_ptr->status_info.message.u.gw_template.relative_validity);
    }

    if (event_info_ptr->status_info.message.u.gw_template.mask & WMS_GW_TEMPLATE_MASK_DCS)
    {
      get_sms_params_resp_pdu->dcs_valid = TRUE;
      (void)wms_ts_encode_dcs(&event_info_ptr->status_info.message.u.gw_template.dcs,
                              &get_sms_params_resp_pdu->dcs);
    }

    if (event_info_ptr->status_info.message.u.gw_template.mask & WMS_GW_TEMPLATE_MASK_PID)
    {
      get_sms_params_resp_pdu->pid_valid = TRUE;
      get_sms_params_resp_pdu->pid       = (wms_pid_enum_v01)event_info_ptr->status_info.message.u.gw_template.pid;
    }

    if (event_info_ptr->status_info.message.u.gw_template.mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR)
    {
      get_sms_params_resp_pdu->dest_addr_valid = TRUE;
      get_sms_params_resp_pdu->dest_addr_len = wms_ts_encode_address(&event_info_ptr->status_info.message.u.gw_template.dest_addr,
                                                       get_sms_params_resp_pdu->dest_addr);

      if (0 == get_sms_params_resp_pdu->dest_addr_len)
      {
        MSG_HIGH_0("Unable to encode destination address, resetting all TLVs other than result code TLV");
        memset(get_sms_params_resp_pdu, 0 , sizeof(wms_get_sms_parameters_resp_msg_v01));
        errval = QMI_ERR_INTERNAL;
      }
    }
  }
  else
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }

  return errval;
} /* qmi_wmsi_get_sms_parameters_resp */

#ifndef REMOVE_QMI_WMS_SET_SMS_PARAMETERS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_SMS_PARAMETERS()

  DESCRIPTION
    Writes SMS parameters to EF-SMSP.

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_set_sms_parameters(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type * response = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

  wms_address_s_type           dest_addr;
  wms_gw_dcs_s_type            dcs;
  wms_timestamp_s_type         relative_validity;
  wms_write_mode_e_type        write_mode;
  sys_modem_as_id_e_type       as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type * wms_cl_sp =  (qmi_wmsi_client_state_type *)cl_sp;
  uint8                        sub;

  wms_set_sms_parameters_req_msg_v01	 *set_sms_params_req_pdu = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  set_sms_params_req_pdu = ( wms_set_sms_parameters_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_sms_parameters_req_msg_v01) );

  if (NULL == set_sms_params_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_sms_parameters_req_msg_v01, QMI_WMS_SET_SMS_PARAMETERS_REQ_V01, set_sms_params_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

  if (WMSI_MSG_MODE_GW != (qmi_wmsi_message_mode_e_type) set_sms_params_req_pdu->message_mode)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if (set_sms_params_req_pdu->dest_addr_valid || set_sms_params_req_pdu->pid_valid || set_sms_params_req_pdu->dcs_valid || set_sms_params_req_pdu->validity_valid)
  {
    if (set_sms_params_req_pdu->dest_addr_valid)
    {
      memset(&dest_addr, 0, sizeof(dest_addr));

      /* Spec TS 23.040 Section 9.1.2.5 and 9.2.3.8 specify that the maximum length of the
         full address field (Address Length, Type of Address and Address Value) is 12 octets.
         The length of the destination address should be between 3 to 12 octets (20 semi-octets). */
      if ((set_sms_params_req_pdu->dest_addr_len < 2) || (WMSI_DEST_ADDR_LEN < set_sms_params_req_pdu->dest_addr_len))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      /* The number of digits cannot be greater than 20 and address should be decoded correctly */
      if ((WMS_GW_ADDRESS_MAX < set_sms_params_req_pdu->dest_addr[0])||
         (!wms_ts_decode_address(set_sms_params_req_pdu->dest_addr,&dest_addr)))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
    }

    if (set_sms_params_req_pdu->dcs_valid)
    {
      memset(&dcs, 0, sizeof(dcs));
      if (!wms_ts_decode_dcs(&(set_sms_params_req_pdu->dcs),&dcs))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
    }

    if (set_sms_params_req_pdu->validity_valid)
    {
      memset(&relative_validity, 0, sizeof(relative_validity));

      wms_ts_decode_relative_time(set_sms_params_req_pdu->validity,&relative_validity);
    }

    if (QMI_ERR_NONE == errval)
    {

      wms_client_message_s_type   *template_msg_ptr = NULL;

      template_msg_ptr = (wms_client_message_s_type*)qmi_wmsi_mem_alloc(sizeof(wms_client_message_s_type));

      if (NULL == template_msg_ptr)
      {
        errval = QMI_ERR_INTERNAL;
        goto send_result;
      }

      sub = (uint8) wms_cl_sp->subscription_status.subscription_type;

      /* Initialize the template to the stored template to preserve the current settings. */
      (void)memscpy(&(template_msg_ptr->u.gw_template),
                    sizeof(wms_gw_template_s_type),
                    &qmi_wmsi_global.gw_template[sub].mo_template,
                    sizeof(wms_gw_template_s_type));

      template_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      template_msg_ptr->msg_hdr.tag          = WMS_TAG_MO_TEMPLATE;
      template_msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_SIM;

      /* do not copy requested template into cache until after processed by
         WMS service (WMS_MSG_EVENT_WRITE_TEMPLATE) */

      if (set_sms_params_req_pdu->dest_addr_valid)
      {
        template_msg_ptr->u.gw_template.mask |= (WMS_GW_TEMPLATE_MASK_DEST_ADDR);
        (void)memscpy(&(template_msg_ptr->u.gw_template.dest_addr),
                      sizeof(dest_addr),
                      &dest_addr,
                      sizeof(dest_addr));
      }

      if (set_sms_params_req_pdu->pid_valid)
      {
        template_msg_ptr->u.gw_template.mask |= (WMS_GW_TEMPLATE_MASK_PID);
        template_msg_ptr->u.gw_template.pid   = (wms_pid_e_type) set_sms_params_req_pdu->pid;
      }

      if (set_sms_params_req_pdu->dcs_valid)
      {
        template_msg_ptr->u.gw_template.mask |= (WMS_GW_TEMPLATE_MASK_DCS);
        (void)memscpy(&(template_msg_ptr->u.gw_template.dcs),
                      sizeof(dcs),
                      &dcs,
                      sizeof(dcs));
      }

      if (set_sms_params_req_pdu->validity_valid)
      {
        template_msg_ptr->u.gw_template.mask |= (WMS_GW_TEMPLATE_MASK_VALIDITY);
        (void)memscpy(&(template_msg_ptr->u.gw_template.relative_validity),
                      sizeof(relative_validity),
                      &relative_validity,
                      sizeof(relative_validity));
      }

      if (WMSI_MEM_STORE_MAX == qmi_wmsi_global.gw_template[sub].mem_store)
      {
        write_mode = WMS_WRITE_MODE_INSERT;
        template_msg_ptr->msg_hdr.index = 0;
      }
      else
      {
        write_mode = WMS_WRITE_MODE_REPLACE;
        template_msg_ptr->msg_hdr.index = qmi_wmsi_global.gw_template[sub].index;
      }

      errval = (qmi_error_e_type)
               qmi_wmsi_from_wms_status(wms_msg_ms_write_template(qmi_wmsi_global.wms_cid,
                                                                  as_id,
                                                                  (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                  (void *) cmd_buf_p,
                                                                  write_mode,
                                                                  template_msg_ptr));
     QMI_WMSI_MEM_FREEIF(template_msg_ptr);
    }

    if (QMI_ERR_NONE == errval)
    {
      QMI_WMSI_MEM_FREEIF(set_sms_params_req_pdu);
      return QMI_SVC_RESPONSE_PENDING;
    }
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  QMI_WMSI_MEM_FREEIF(set_sms_params_req_pdu);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_set_sms_parameters() */
#endif /* REMOVE_QMI_WMS_SET_SMS_PARAMETERS_V01 */
/*===========================================================================
  FUNCTION QMI_WMSI_CALL_STATUS_IND()

  DESCRIPTION
    Sends the Call Status indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    event_ptr     : indication event info pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_call_status_ind(
  void * cl_sp_ptr,
  qmi_wmsi_call_status_e_type call_status
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;

  wms_call_status_ind_msg_v01  *ind_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp_ptr)
  {
    MSG_ERROR_0("Call Status indication passed NULL ptr, return");
    return;
  }


  ind_pdu = ( wms_call_status_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_call_status_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  cl_sp = (qmi_wmsi_client_state_type *) cl_sp_ptr;

  MSG_HIGH_1("Call Status indication for client ID %d", cl_sp->common.clid);
  ind_pdu->call_status = (uint8) call_status;
  ENCODE_INDICATION_TLV(wms_call_status_ind_msg_v01, QMI_WMS_CALL_STATUS_IND_V01);
  
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_CALL_STATUS_IND, ind);
  }
} /* qmi_wmsi_call_status_ind() */


/*===========================================================================
  FUNCTION QMI_WMSI_GENERATE_CALL_STATUS_IND()

  DESCRIPTION
    Sends the Call Status indication to the client

  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    event_ptr     : indication event info pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_generate_call_status_ind(
  wms_dc_event_e_type event
)
{
  uint8                         cl_idx;
  qmi_wmsi_call_status_e_type   qmi_call_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating Call Status indication");

  qmi_call_status = qmi_wms_to_wmsi_call_status(event);

  if (WMSI_CALL_STATUS_MAX == qmi_call_status)
  {
    /* Drop unsupported call states */
    return;
  }

  /*-------------------------------------------------------------------------
    Report Call Status indication to all clients that have registered for it
  -------------------------------------------------------------------------*/
  MSG_HIGH_0("Call Status changed - send IND");
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) && (QMI_SVC_CLID_UNUSED != cl_sp->common.clid))
    {
      if (cl_sp->indication_reg.reg_call_status_events)
      {
        qmi_wmsi_call_status_ind(cl_sp, qmi_call_status);
      }
    }
  }
} /* qmi_wmsi_generate_call_status_ind */

#ifndef REMOVE_QMI_WMS_GET_DOMAIN_PREF_CONFIG_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_DOMAIN_PREF_CONFIG()

  DESCRIPTION
    Gets the Domain Preference Configuration.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_domain_pref_config(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

#ifdef FEATURE_GWSMS
  qmi_wms_cmd_node_type * cmd_node = NULL;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp =  (qmi_wmsi_client_state_type *)cl_sp;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

#ifdef FEATURE_GWSMS
  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = (qmi_error_e_type)
           qmi_wmsi_from_wms_status(wms_cfg_ms_get_gw_domain_pref(qmi_wmsi_global.wms_cid,
                                                                  as_id,
                                                                  (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                                                  (void *) cmd_buf_p));

  if (QMI_ERR_NONE == errval)
  {
    cmd_node = qmi_wmsi_get_cmd_node();
    if (!cmd_node)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    cmd_node->cmd_buf                  = (void *) cmd_buf_p;
    /* cache the message request for later retrieval */
    cmd_node->cache.get_domain_pref.pending = TRUE;
    cmd_node->cmd_type = WMSI_CMD_VAL_GET_DOMAIN_PREF_CONFIG;
    qmi_wmsi_put_pending_cmd_buf(cmd_node);

    return QMI_SVC_RESPONSE_PENDING;
  }
#endif

send_result:
#if !defined (FEATURE_DOMAIN_SELECTION) && !defined (FEATURE_GWSMS)
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_domain_pref_config() */
#endif /* REMOVE_QMI_WMS_GET_DOMAIN_PREF_CONFIG_V01 */

#ifndef REMOVE_QMI_WMS_SET_DOMAIN_PREF_CONFIG_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_DOMAIN_PREF_CONFIG()

  DESCRIPTION
    Sets the Domain Preference Configuration.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_wmsi_set_domain_pref_config(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;

#ifdef FEATURE_GWSMS
  sys_modem_as_id_e_type        as_id     = SYS_MODEM_AS_ID_NONE;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
#endif

  wms_set_domain_pref_config_req_msg_v01	*set_domain_pref_config_req_pdu = NULL;
  wms_set_domain_pref_config_resp_msg_v01        set_domain_pref_config_resp_pdu;

  void *response_pdu = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&set_domain_pref_config_resp_pdu, 0, sizeof(wms_set_domain_pref_config_resp_msg_v01));
  response_pdu = (void*) &set_domain_pref_config_resp_pdu;
  
  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

#ifdef FEATURE_GWSMS
  if ( NULL != wms_cl_sp ) 
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
    if ( SYS_MODEM_AS_ID_NONE == as_id )  
    {
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }
  }
#endif
  set_domain_pref_config_req_pdu = ( wms_set_domain_pref_config_req_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_set_domain_pref_config_req_msg_v01) );

  if (NULL == set_domain_pref_config_req_pdu)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  errval = DECODE_REQUEST_TLV(wms_set_domain_pref_config_req_msg_v01, QMI_WMS_SET_DOMAIN_PREF_CONFIG_REQ_V01, set_domain_pref_config_req_pdu);

  if (QMI_ERR_NONE != errval)
  {
    goto send_result;
  }

/*-----------------------------------------------------------------------
      validate the input parameters
  -----------------------------------------------------------------------*/

#ifdef FEATURE_DOMAIN_SELECTION
  if (set_domain_pref_config_req_pdu->lte_domain_pref_valid)
  {
    if ((uint8)WMSI_LTE_DOMAIN_PREF_MAX <= set_domain_pref_config_req_pdu->lte_domain_pref)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }
#endif /* FEATURE_DOMAIN_SELECTION */

#ifdef FEATURE_GWSMS
  if (set_domain_pref_config_req_pdu->gw_domain_pref_valid)
  {
    if ((uint8)WMS_GW_DOMAIN_PREF_MAX <= set_domain_pref_config_req_pdu->gw_domain_pref)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }
#endif /* FEATURE_GWSMS */

/*-----------------------------------------------------------------------
    perform the requested function
-----------------------------------------------------------------------*/

#ifdef FEATURE_DOMAIN_SELECTION
  if (set_domain_pref_config_req_pdu->lte_domain_pref_valid)
  {
    /* Set the LTE Domain Preference */
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
             (wms_cfg_set_lte_domain_pref(qmi_wmsi_global.wms_cid,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p,
                                          set_domain_pref_config_req_pdu->lte_domain_pref));

    set_domain_pref_config_resp_pdu.lte_domain_pref_outcome         = (qmi_error_type_v01)errval;
    set_domain_pref_config_resp_pdu.lte_domain_pref_outcome_valid   = TRUE;

    /* Fail the command if either of the input TLVs cannot be processed successfully. */
    if (QMI_ERR_NONE != errval)
    {
      goto send_result;
    }
  }
#endif /* FEATURE_DOMAIN_SELECTION */

#ifdef FEATURE_GWSMS
  if (set_domain_pref_config_req_pdu->gw_domain_pref_valid)
  {
    /* Set the GW Domain Preference */
    errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
             (wms_cfg_ms_set_gw_domain_pref(qmi_wmsi_global.wms_cid,
                                            as_id,
                                            (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                            (void *) cmd_buf_p,
                                            (wms_gw_domain_pref_e_type)set_domain_pref_config_req_pdu->gw_domain_pref));

    set_domain_pref_config_resp_pdu.gw_domain_pref_outcome         = (qmi_error_type_v01)errval;
    set_domain_pref_config_resp_pdu.gw_domain_pref_outcome_valid   = TRUE;
  }
#endif /* FEATURE_GWSMS */

send_result:
#if !defined (FEATURE_DOMAIN_SELECTION) && !defined (FEATURE_GWSMS)
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif

  ENCODE_RESPONSE_TLV(wms_set_domain_pref_config_resp_msg_v01, QMI_WMS_SET_DOMAIN_PREF_CONFIG_RESP_V01);  
  QMI_WMSI_MEM_FREEIF(set_domain_pref_config_req_pdu);
  return response;
} /* qmi_wmsi_set_domain_pref_config */
#endif /* REMOVE_QMI_WMS_SET_DOMAIN_PREF_CONFIG_V01 */

#ifndef REMOVE_QMI_WMS_GET_RETRY_PERIOD_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_RETRY_PERIOD()

  DESCRIPTION
    Gets the message time peroid used for MO SMS attempts.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_retry_period(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_wms_cmd_node_type * cmd_node = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  void              *response_pdu = NULL;

  wms_get_retry_period_resp_msg_v01 get_retry_period_resp_pdu;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response_pdu = &get_retry_period_resp_pdu;
  
  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_DBG_RETRY_TIMER_MAX != qmi_wmsi_global.dbg_retry_period)
  {
    /* DBG retry period already read from WMS, return cached value */
    /*-------------------------------------------------------------------------
      Build the response from back to front
    -------------------------------------------------------------------------*/
    get_retry_period_resp_pdu.retry_period       = qmi_wmsi_global.dbg_retry_period;
    get_retry_period_resp_pdu.retry_period_valid = TRUE;	
  }
  else
  {
    errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
               wms_dbg_get_retry_period(qmi_wmsi_global.wms_cid,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p));

    if (QMI_ERR_NONE == errval)
    {
      cmd_node = qmi_wmsi_get_cmd_node();
      if (!cmd_node)
      {
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }

      cmd_node->cmd_buf                  = (void *) cmd_buf_p;
      qmi_wmsi_put_pending_cmd_buf(cmd_node);

      return QMI_SVC_RESPONSE_PENDING;
    }
  }

  send_result:

  ENCODE_RESPONSE_TLV(wms_get_retry_period_resp_msg_v01, QMI_WMS_GET_RETRY_PERIOD_RESP_V01);  
  return response;

} /* qmi_wmsi_get_retry_period() */
#endif /* REMOVE_QMI_WMS_GET_RETRY_PERIOD_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_RETRY_PEROID_RESP()

  DESCRIPTION
    Creates the response message for a get retry period result event.

  PARAMETERS
    get_retry_period_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_retry_period_resp(
  wms_get_retry_period_resp_msg_v01 *get_retry_period_resp_pdu,
  wms_dbg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr)
  {
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  get_retry_period_resp_pdu->retry_period       = event_info_ptr->retry_period;
  get_retry_period_resp_pdu->retry_period_valid = TRUE;
  return errval;
} /* qmi_wmsi_get_retry_period_resp */

static int qmi_wmsi_compare_get_dbg_retry_period(
  qmi_wms_cmd_node_type * cmd_node,
  wms_dbg_event_info_s_type * info_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != info_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_GET_RETRY_PERIOD == cmd_buf_p->cmd_type))
    {
      ret = (int) TRUE;
    }
  }
  return ret;
} /* qmi_wmsi_compare_get_dbg_retry_period */

#ifndef REMOVE_QMI_WMS_GET_RETRY_INTERVAL_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_RETRY_INETRVAL()

  DESCRIPTION
    Gets the retry interval used for MO SMS attempts.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_retry_interval(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_wms_cmd_node_type * cmd_node = NULL;

  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  void              *response_pdu = NULL;

  wms_get_retry_interval_resp_msg_v01  get_retry_interval_resp_pdu;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  response_pdu = &get_retry_interval_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }


  if (WMSI_DBG_RETRY_TIMER_MAX != qmi_wmsi_global.dbg_retry_interval)
  {
    /* DBG retry interval already read from WMS, return cached value */
    /*-------------------------------------------------------------------------
      Build the response from back to front
    -------------------------------------------------------------------------*/
    get_retry_interval_resp_pdu.retry_interval       = qmi_wmsi_global.dbg_retry_interval;
    get_retry_interval_resp_pdu.retry_interval_valid = TRUE;
  }
  else
  {
    errval = (qmi_error_e_type) qmi_wmsi_from_wms_status(
               wms_dbg_get_retry_interval(qmi_wmsi_global.wms_cid,
                                          (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                          (void *) cmd_buf_p));

    if (QMI_ERR_NONE == errval)
    {
      cmd_node = qmi_wmsi_get_cmd_node();
      if (!cmd_node)
      {
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }

      cmd_node->cmd_buf                  = (void *) cmd_buf_p;
      qmi_wmsi_put_pending_cmd_buf(cmd_node);

      return QMI_SVC_RESPONSE_PENDING;
    }
  }

  send_result:
  ENCODE_RESPONSE_TLV(wms_get_retry_interval_resp_msg_v01, QMI_WMS_GET_RETRY_INTERVAL_RESP_V01);  

  return response;

} /* qmi_wmsi_get_retry_interval() */
#endif /* REMOVE_QMI_WMS_GET_RETRY_INTERVAL_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_GET_RETRY_INTERVAL_RESP()

  DESCRIPTION
    Creates the response message for a get retry interval result event.

  PARAMETERS
    get_retry_interval_resp_pdu       : pdu ptr to begin writing the response message to
    event_info_ptr : event data from the WMS service

  RETURN VALUE
    qmi error from generating the response message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_wmsi_get_retry_interval_resp(
  wms_get_retry_interval_resp_msg_v01 *get_retry_interval_resp_pdu,
  wms_dbg_event_info_s_type * event_info_ptr
)
{
  qmi_error_e_type   errval = QMI_ERR_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == event_info_ptr)
  {
    return QMI_ERR_INTERNAL;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  get_retry_interval_resp_pdu->retry_interval       = event_info_ptr->retry_interval;
  get_retry_interval_resp_pdu->retry_interval_valid = TRUE;

  return errval;
} /* qmi_wmsi_get_retry_interval_resp */

static int qmi_wmsi_compare_get_dbg_retry_interval(
  qmi_wms_cmd_node_type * cmd_node,
  wms_dbg_event_info_s_type * info_ptr
)
{
  int ret = (int) FALSE;
  if ((NULL != cmd_node) && (NULL != info_ptr))
  {
    qmi_cmd_buf_type * cmd_buf_p = cmd_node->cmd_buf;

    if (cmd_buf_p && (WMSI_CMD_VAL_GET_RETRY_INTERVAL == cmd_buf_p->cmd_type))
    {
      ret = (int) TRUE;
    }
  }
  return ret;
} /* qmi_wmsi_compare_get_dbg_retry_interval */

#ifndef REMOVE_QMI_WMS_GET_DC_DISCONNECT_TIMER_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_DC_DISCONNECT_TIMER()

  DESCRIPTION
    Gets the max timer value used to control when the DC (dedicated channel) is
    torn down after sending an MO SMS.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_dc_disconnect_timer(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL ;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

#if 0 // TODO: not supported in WMS service, return error until it is added
  struct
  {
    uint32   disconnect_timer;
  } v_out_opt1;
#endif /* 0 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
  }

#if 0 // TODO: not supported in WMS service, return error until it is added
  memset(&v_out_opt1, 0, sizeof(v_out_opt1));

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  v_out_opt1.disconnect_timer = 0;
  if (FALSE == qmi_svc_put_param_tlv(&response,
                                     WMSI_PRM_TYPE_GET_DC_DISCONNECT_TIMER,
                                     sizeof(v_out_opt1),
                                     &v_out_opt1))
  {
    errval = QMI_ERR_NO_MEMORY;
  }

#else /* 0 */
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* 0 */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_dc_disconnect_timer() */
#endif /* REMOVE_QMI_WMS_GET_DC_DISCONNECT_TIMER_V01 */

#ifndef REMOVE_QMI_WMS_GET_MEMORY_STATUS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_MEMORY_STATUS()

  DESCRIPTION
    Gets the status of the device memory.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_memory_status(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;

#if 0 // TODO: not supported in WMS service, return error until it is added
#ifdef _WIN32
#pragma pack(push, 1) // Save previous, and turn on 1 byte alignment
#endif

  PACK(struct)
  {
    uint8  memory_status;
  } v_out_opt1;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#endif /* 0 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
  }

#if 0 // TODO: not supported in WMS service, return error until it is added
  memset(&v_out_opt1, 0, sizeof(v_out_opt1));

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  v_out_opt1.memory_status = 0;
  if (FALSE == qmi_svc_put_param_tlv(&response,
                                     WMSI_PRM_TYPE_GET_MEMORY_STATUS,
                                     sizeof(v_out_opt1),
                                     &v_out_opt1))
  {
    errval = QMI_ERR_NO_MEMORY;
  }
#else /* 0 */
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
#endif /* 0 */

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_wmsi_get_memory_status() */
#endif

#ifndef REMOVE_QMI_WMS_GET_PRIMARY_CLIENT_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_PRIMARY_CLIENT()

  DESCRIPTION
    Gets whether the client is currently set as the primary client.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_primary_client(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  void              *response_pdu = NULL;

  wms_get_primary_client_resp_msg_v01 get_primary_client_resp_pdu;

  qmi_wmsi_client_state_type *  wms_cl_sp = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  response_pdu = &get_primary_client_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_primary_client()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (WMSI_SUBSCRIPTION_MAX <= wms_cl_sp->subscription_status.subscription_type)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  get_primary_client_resp_pdu.primary_client_valid = TRUE;
  get_primary_client_resp_pdu.primary_client       = wms_cl_sp->subscription_status.primary_client_state[(wms_cl_sp->subscription_status.subscription_type)];

send_result:

  ENCODE_RESPONSE_TLV(wms_get_primary_client_resp_msg_v01, QMI_WMS_GET_PRIMARY_CLIENT_RESP_V01); 

  return response;

} /* qmi_wmsi_get_primary_client() */
#endif /* REMOVE_QMI_WMS_GET_PRIMARY_CLIENT_V01 */

#ifndef REMOVE_QMI_WMS_GET_SUBSCRIPTION_BINDING_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_SUBSCRIPTION_BINDING()

  DESCRIPTION
    Gets the subscription the client is currently bound to.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_subscription_binding(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  void		    *response_pdu = NULL;
  wms_get_subscription_binding_resp_msg_v01 get_subs_binding_resp_pdu;
  qmi_wmsi_client_state_type *  wms_cl_sp = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response_pdu = &get_subs_binding_resp_pdu;

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_subscription_binding()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (WMSI_SUBSCRIPTION_MAX <= wms_cl_sp->subscription_status.subscription_type)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  get_subs_binding_resp_pdu.subs_type_valid = TRUE;
  get_subs_binding_resp_pdu.subs_type       = (uint8)wms_cl_sp->subscription_status.subscription_type;


send_result:
  ENCODE_RESPONSE_TLV(wms_get_subscription_binding_resp_msg_v01, QMI_WMS_GET_SUBSCRIPTION_BINDING_RESP_V01);  

  return response;

} /* qmi_wmsi_get_subscription_binding() */
#endif /* REMOVE_QMI_WMS_GET_SUBSCRIPTION_BINDING_V01 */

#ifndef REMOVE_QMI_WMS_GET_SERVICE_READY_STATUS_V01
/*===========================================================================
  FUNCTION QMI_WMSI_GET_SERVICE_READY_STATUS()

  DESCRIPTION
    Gets the service ready status.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_get_service_ready_status(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  qmi_wmsi_client_state_type * wms_cl_sp = NULL;
  uint8 sub;

  void              *response_pdu = NULL;

  wms_get_service_ready_status_resp_msg_v01 get_service_ready_resp_pdu;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response_pdu = (void *)&get_service_ready_resp_pdu; 

  if ((NULL == sp) || (NULL == cmd_buf_p) || (NULL == cl_sp) || (NULL == sdu_in))
  {
    MSG_ERROR_0("NULL pointer(s) in qmi_wmsi_get_service_ready_status()");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;

  if (NULL != wms_cl_sp)
  {
    sub = (uint8) wms_cl_sp->subscription_status.subscription_type;
  }
  else
  {
    MSG_ERROR_0("NULL client pointer");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (WMSI_SUBSCRIPTION_MAX <= sub)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  if (wms_cl_sp->indication_reg.reg_sim_ready_events)
  {
    get_service_ready_resp_pdu.sim_ready_status       = (uint64)qmi_wmsi_global.sim_ready_status[sub].sim_ready_status;
    get_service_ready_resp_pdu.sim_ready_status_valid = TRUE;

    if (WMSI_SIM_READY_STATUS_MAX <= (uint32)qmi_wmsi_global.sim_ready_status[sub].sim_ready_status)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    get_service_ready_resp_pdu.sim_ready_registered_ind_valid = TRUE;
    get_service_ready_resp_pdu.sim_ready_registered_ind       = (uint8)wms_cl_sp->indication_reg.reg_sim_ready_events;
  }

  get_service_ready_resp_pdu.ready_status_valid = TRUE;
  get_service_ready_resp_pdu.ready_status       = (uint32)qmi_wmsi_global.service_ready_status[sub].status;
  
  if (WMSI_SERVICE_READY_STATUS_MAX <= get_service_ready_resp_pdu.ready_status )
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  get_service_ready_resp_pdu.registered_ind_valid = TRUE;
  get_service_ready_resp_pdu.registered_ind       = (uint8)wms_cl_sp->indication_reg.reg_service_ready_events;

send_result:

  ENCODE_RESPONSE_TLV(wms_get_service_ready_status_resp_msg_v01, QMI_WMS_GET_SERVICE_READY_STATUS_RESP_V01); 


  return response;
} /* qmi_wmsi_get_service_ready_status() */
#endif /* REMOVE_QMI_WMS_GET_SERVICE_READY_STATUS_V01 */

/*===========================================================================
  FUNCTION QMI_WMSI_SERVICE_READY_IND()

  DESCRIPTION
    Indicates if SMS service is ready

  PARAMETERS
    cl_sp         : client state pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_service_ready_ind(
  void * sp,
  qmi_wmsi_subscription_e_type subscription,
  boolean notify_sim_status
)
{
  dsm_item_type *  ind = NULL;
  qmi_wmsi_client_state_type * cl_sp = (qmi_wmsi_client_state_type *)sp;

  wms_service_ready_ind_msg_v01  *ind_pdu = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == cl_sp)
  {
    MSG_HIGH_1("NULL pointer sp = 0x%x", sp);
    return;
  }

  MSG_HIGH_1("Ready indication for client ID %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/

  ind_pdu = ( wms_service_ready_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_service_ready_ind_msg_v01) );
  if (NULL == ind_pdu)
  {
    return;
  }

  if (notify_sim_status)
  {
    ind_pdu->sim_ready_status       = (uint64)qmi_wmsi_global.sim_ready_status[subscription].sim_ready_status;
    ind_pdu->sim_ready_status_valid = TRUE;   
    if (WMSI_SIM_READY_STATUS_MAX <= (uint32)qmi_wmsi_global.sim_ready_status[subscription].sim_ready_status)
    {
      QMI_WMSI_MEM_FREEIF(ind_pdu);
      return;
    }
  }
  ind_pdu->ready_status = (uint32)qmi_wmsi_global.service_ready_status[subscription].status;
  if (WMSI_SERVICE_READY_STATUS_MAX <= ind_pdu->ready_status)
  {
    QMI_WMSI_MEM_FREEIF(ind_pdu);
    return;
  }

  ENCODE_INDICATION_TLV(wms_service_ready_ind_msg_v01, QMI_WMS_SERVICE_READY_IND_V01);
  QMI_WMSI_MEM_FREEIF(ind_pdu);

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_SERVICE_READY_IND, ind);
  }
} /* qmi_wmsi_service_ready_ind() */

void qmi_wmsi_generate_ready_ind(
  qmi_wmsi_subscription_e_type subscription,
  boolean                      report_sim_ready        
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (WMSI_SUBSCRIPTION_MAX <= subscription)
  {
    MSG_ERROR_0("Invalid subscription type");
    return;
  }

  MSG_HIGH_2("Generating Ready indication for as_id = %d, report_sim_ready = %d", 
             subscription, report_sim_ready);

  /*--------------------------------------------------------------------------
    Report Service Ready indication to all clients that have registered for it
  ----------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];
    boolean cl_reg_ready_evt = FALSE;

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid) &&
        (subscription == cl_sp->subscription_status.subscription_type))
    {
      if ( report_sim_ready )
      {
        cl_reg_ready_evt = (cl_sp->indication_reg.reg_sim_ready_events) ? 1 : 0;
      }
      else
      {
        cl_reg_ready_evt = (cl_sp->indication_reg.reg_service_ready_events) ? 1 : 0;

        /* Notify SIM ready status to registered clients (in cases where device is put
           to LPM mode and UIM card is not powered down) */
        report_sim_ready = (cl_sp->indication_reg.reg_sim_ready_events) ? 1 : 0;
      }

      if ( cl_reg_ready_evt )
      {
        qmi_wmsi_service_ready_ind(cl_sp, (uint8)subscription, report_sim_ready);
      }
    }
  }
} /* qmi_wmsi_generate_ready_ind */

/*===========================================================================
  FUNCTION QMI_WMSI_BROADCAST_CONFIG_IND()

  DESCRIPTION
    Indicates when broadcast configuration has been changed.

  PARAMETERS
    cl_sp         : client state pointer

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_broadcast_config_ind(
  void * sp,
  wms_bc_mm_event_info_s_type * evt_ptr
)
{
  dsm_item_type *  ind = NULL;
  qmi_wmsi_client_state_type * cl_sp = NULL;
  uint32 i;
  uint16       num_instances = 0;

  wms_broadcast_config_ind_msg_v01  *ind_pdu = NULL;

  cl_sp = (qmi_wmsi_client_state_type *) sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cl_sp || NULL == evt_ptr)
  {
    MSG_ERROR_0("qmi_wmsi_broadcast_config_ind() receives NULL pointer");
    return;
  }

  ind_pdu = ( wms_broadcast_config_ind_msg_v01 *) qmi_wmsi_mem_alloc( sizeof(wms_broadcast_config_ind_msg_v01) );

  if (NULL == ind_pdu)
  {
    return;
  }

  MSG_HIGH_2( "Received event %d, send broadcast config indication for client ID %d",
              evt_ptr->event,
              cl_sp->common.clid );

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  ind_pdu->message_mode = (uint8) evt_ptr->message_mode;

  if (WMS_BC_MM_EVENT_SCPT_ADD_SRVS == evt_ptr->event)
  {
    num_instances = MIN(evt_ptr->u.bc_mm_table.size, WMSI_BC_SMS_TABLE_SIZE);
  }
  else if (WMS_BC_MM_EVENT_SCPT_DELETE_SRVS == evt_ptr->event ||
           WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS == evt_ptr->event)
  {
    num_instances = MIN(evt_ptr->u.bc_mm_srv_ids.size, WMSI_BC_SMS_TABLE_SIZE);
  }

  if (WMSI_MSG_MODE_CDMA == evt_ptr->message_mode)
  {
    ind_pdu->wms_3gpp2_broadcast_info.activated_ind                       = TRUE;
    ind_pdu->wms_3gpp2_broadcast_info_valid                               = TRUE;
    ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info_len = num_instances;
  }
  else
  {
    ind_pdu->wms_3gpp_broadcast_info.activated_ind                      = TRUE;
    ind_pdu->wms_3gpp_broadcast_info_valid 		                = TRUE;	  
    ind_pdu->wms_3gpp_broadcast_info.wms_3gpp_broadcast_config_info_len = num_instances;
  }

  if (WMSI_MSG_MODE_CDMA == evt_ptr->message_mode)
  {
    if (WMS_BC_MM_EVENT_SCPT_ADD_SRVS == evt_ptr->event)
    {
      for (i = 0; i < num_instances; i++)
      {
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].service_category =
          (uint16) evt_ptr->u.bc_mm_table.entries[i].srv_id.bc_srv_id.service;
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].language =
          (uint16) evt_ptr->u.bc_mm_table.entries[i].srv_id.bc_srv_id.language;
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].selected =  
           (evt_ptr->u.bc_mm_table.entries[i].selected) ? 1 : 0;
      }
    }
    else if (WMS_BC_MM_EVENT_SCPT_DELETE_SRVS == evt_ptr->event ||
             WMS_BC_MM_EVENT_SCPT_DELETE_ALL_SRVS == evt_ptr->event)
    {
      for (i = 0; i < num_instances; i++)
      {
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].service_category =
          (uint16) evt_ptr->u.bc_mm_srv_ids.entries[i].bc_srv_id.service;
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].language =
          (uint16) evt_ptr->u.bc_mm_srv_ids.entries[i].bc_srv_id.language;
        ind_pdu->wms_3gpp2_broadcast_info.wms_3gpp2_broadcast_config_info[i].selected = 
           (evt_ptr->u.bc_mm_srv_ids.entries[i].selected)? 1 : 0;
    }
    }
  }

   ENCODE_INDICATION_TLV(wms_broadcast_config_ind_msg_v01, QMI_WMS_BROADCAST_CONFIG_IND_V01);
   QMI_WMSI_MEM_FREEIF(ind_pdu);


  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_BROADCAST_CONFIG_IND, ind);
  }
} /* qmi_wmsi_broadcast_config_ind() */

void qmi_wmsi_generate_broadcast_config_ind(
  wms_bc_mm_event_info_s_type * evt_ptr
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating Broadcast Config indication");

  /*-----------------------------------------------------------------------------
    Report Broadcast Config indication to all clients that have registered for it
  -------------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid))
    {
      if (cl_sp->indication_reg.reg_broadcast_config_events)
      {
        qmi_wmsi_broadcast_config_ind(cl_sp, evt_ptr);
      }
    }
  }
} /* qmi_wmsi_generate_broadcast_config_ind */

#ifndef REMOVE_QMI_WMS_SET_MESSAGE_WAITING_V01
/*===========================================================================
  FUNCTION QMI_WMSI_SET_MESSAGE_WAITING()

  DESCRIPTION
    Sets the Message Waiting information.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : ptr to command buffer
    cl_sp         : service provided client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    dsm_item_type * : ptr to dsm item

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_wmsi_set_message_waiting(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;
  qmi_result_e_type  result;
  boolean            retval;
  uint32             i;
  uint8              type;
  uint16             len;
  void *             value;
  qmi_wmsi_client_state_type *  wms_cl_sp = (qmi_wmsi_client_state_type *)cl_sp;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  wms_gw_mwi_info_s_type mwi_info[WMSI_MWI_MAX];

#ifdef _WIN32
#pragma pack(push, 1) // Save previous, and turn on 1 byte alignment
#endif

  PACK (struct)
  {
    uint8 num_instances;
    PACK (struct)
    {
      uint8 msg_type;
      uint8 active_ind;
      uint8 msg_count;
    } mwi[WMSI_MWI_MAX];
  } v_in_req;

#ifdef _WIN32
#pragma pack (pop) // Revert alignment to what it was previously
#endif

  boolean            got_message_waiting_info_tlv = FALSE;
  uint16             mwi_tlv_len = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!qmi_wms_check_null_ptr(sp,cmd_buf_p, sdu_in))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (NULL != wms_cl_sp)
  {
    as_id = qmi_wms_get_asid_from_subscription(wms_cl_sp->subscription_status.subscription_type);
  }

  if (SYS_MODEM_AS_ID_NONE == as_id)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  memset (&v_in_req, 0, sizeof(v_in_req));

  while (*sdu_in)
  {
    if (!qmi_svc_get_tl(sdu_in, &type, &len))
    {
      continue;
    }

    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if (sizeof(v_in_req) >= len)
        {
          got_message_waiting_info_tlv = TRUE;
          mwi_tlv_len = len;
          value = &v_in_req;
        }
        else
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        break;

      default:
        break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if (len != dsm_pullup(sdu_in, value, len))
    {
      MSG_HIGH_0("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if (!got_message_waiting_info_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    perform the requested function
    validate the input parameters
  -----------------------------------------------------------------------*/

  /* Validate length of mandatory TLV */
  if (mwi_tlv_len != sizeof(v_in_req.num_instances) +
      v_in_req.num_instances * sizeof(v_in_req.mwi[0]))
  {
    MSG_HIGH_0("Invalid length in the mandatory TLV");
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  if ((v_in_req.num_instances == 0) || (WMSI_MWI_MAX < v_in_req.num_instances))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  for (i = 0; i < v_in_req.num_instances; i++)
  {
    if (WMSI_MWI_MAX <= v_in_req.mwi[i].msg_type)
    {
       errval = QMI_ERR_INVALID_ARG;
       goto send_result;
    }

    mwi_info[i].type       = v_in_req.mwi[i].msg_type;
    mwi_info[i].count      = v_in_req.mwi[i].msg_count;
    mwi_info[i].is_active  = v_in_req.mwi[i].active_ind;
  }

  /* Set the Message Waiting Information */
  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_cfg_ms_set_message_waiting(qmi_wmsi_global.wms_cid,
                                           as_id,
                                           (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                           (void *) cmd_buf_p,
                                           mwi_info,
                                           v_in_req.num_instances));

  if (QMI_ERR_NONE == errval)
  {
    return QMI_SVC_RESPONSE_PENDING;
  }

  send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_wmsi_set_message_waiting() */
#endif /* REMOVE_QMI_WMS_SET_MESSAGE_WAITING_V01 */

void * qmi_wmsi_convert_tpt_mwi_to_opt_tlv
(
   wms_msg_event_info_s_type * evt_ptr,
   int *size
)
{
  int buffer_size = 0;
  uint16 num_instances = 0;
  uint8 *temp_ptr = NULL;
  int pos = 0;
  int i = 0;

  num_instances = (uint16)evt_ptr->transport_mwi_info.details_tuple_count;
  if ((num_instances <= 0) || (num_instances > WMSI_MWI_TUPLE_MAX))
  {
    MSG_ERROR_1("Invalid number %d of detailed instances in IMS MWI", num_instances);
    return NULL;
  }

  /* calculate buffer size (this should match with the QMI WMS spec definition of the TLV)
     2 - number of instances
     4 - size of message type for each instance
     4 - size of priority field for each instance
     1 - to address length for each instance
     1 - from address length for each instance
     1 - timsetamp length for each instance
     1 - subject length for each instance
     1 - message id length for each instance
  */

  buffer_size += 2; /* Number of instances. */
  for (i = 0; i < num_instances; i++)
  {
    int temp;
    temp = (int)evt_ptr->transport_mwi_info.mwi_msg_details[i].message_type;
    if (WMS_TRANSPORT_MWI_MAX < temp)
    {
      MSG_ERROR_1("Invalid MWI type %d passed in MWI notification", temp);
      return NULL;
    }
    else
    {
      buffer_size += 4;
    }

    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].to_addr_len;
    if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < temp)
    {
      MSG_ERROR_1("Invalid destination address of length %d passed in MWI notification", temp);
      return NULL;
    }
    else
    {
      /* calculate buffer size for holding the length field and the actual address. */
      buffer_size += temp + 1;
    }

    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].from_addr_len;
    if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < temp)
    {
      MSG_ERROR_1("Invalid source address of length %d passed in MWI notification", temp);
      return NULL;
    }
    else
    {
      /* calculate buffer size for holding the length field and the actual address. */
      buffer_size += temp + 1;
    }

    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].len;
    if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < temp)
    {
      MSG_ERROR_1("Subject line of length %d passed in MWI notification exceeds max length", temp);
      return NULL;
    }
    else
    {
      /* calculate buffer size for holding the length field and the actual subject line. */
      buffer_size += temp + 1;
    }

    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].timestamp_len;
    if (WMS_TRANSPORT_MWI_DATE_LENGTH_MAX < temp)
    {
      MSG_ERROR_1("Timestamp of length %d passed in MWI notification exceeds max length", temp);
      return NULL;
    }
    else
    {
      /* calculate buffer size for holding the length field and the actual timestamp. */
      buffer_size += temp + 1;
    }

    temp = (int)evt_ptr->transport_mwi_info.mwi_msg_details[i].priority;
    if (WMS_TRANSPORT_MWI_MAX < temp)
    {
      MSG_ERROR_1("Invalid priority %d passed in MWI notification", temp);
      return NULL;
    }
    else
    {
      buffer_size += 4;
    }

    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].identifier_len;
    if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < temp)
    {
      MSG_ERROR_1("Message Identifier length %d passed in MWI notification exceeds max length", temp);
      return NULL;
    }
    else
    {
      /* calculate buffer size for holding the length field and the actual message id. */
      buffer_size += temp + 1;
    }
  }

  /* Allocate memory for the buffer. */
  MSG_HIGH_1("Buffer size = %d", buffer_size);
  temp_ptr = (void *)qmi_wmsi_mem_alloc(buffer_size);

  if (NULL == temp_ptr)
  {
    return NULL;
  }

  /* Pack the data. */

  /* Copy the number of instances. */
  (void)memscpy(&temp_ptr[pos],
                2,
                &num_instances,
                2);
  pos += 2;

  for (i = 0; i < num_instances && pos < buffer_size; i++)
  {
    int temp = 0;
    uint32 type = 0;

    /* Copy the message type. */
    type = (uint32)qmi_wms_transport_mwi_to_wmsi_transport_mwi
                  (evt_ptr->transport_mwi_info.mwi_msg_details[i].message_type);

    (void)memscpy(&temp_ptr[pos],
                  4,
                  &type,
                  4);

    pos += 4;
    
    /* Packing "to address" length and "to address". */
    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].to_addr_len;
    temp_ptr[pos] = temp;
    pos++;
    (void)memscpy(&temp_ptr[pos],
                  temp,
                  (void *)evt_ptr->transport_mwi_info.mwi_msg_details[i].receiver_addr,
                  temp);

    pos += temp;

    /* Packing "from address" length and "from address". */
    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].from_addr_len;
    temp_ptr[pos] = temp;
    pos++;
    (void)memscpy(&temp_ptr[pos],
                  temp,
                  (void *)evt_ptr->transport_mwi_info.mwi_msg_details[i].sender_addr,
                  temp);

    pos += temp;
   
    /* Packing "subject line" length and "subject line". */
    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].len;
    temp_ptr[pos] = temp;
    pos++;
    (void)memscpy(&temp_ptr[pos],
                  temp,
                  (void *)evt_ptr->transport_mwi_info.mwi_msg_details[i].subject,
                  temp);

    pos += temp;

    /* Packing "timestamp" length and "timestamp". */
    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].timestamp_len;
    temp_ptr[pos] = temp;
    pos++;
    (void)memscpy(&temp_ptr[pos],
                  temp,
                  (void *)evt_ptr->transport_mwi_info.mwi_msg_details[i].timestamp,
                  temp);

    pos += temp;

    /* Packing "to address" length and "to address". */
    type = (uint32)qmi_wms_transport_mwi_priority_to_wmsi_mwi_priority
                   (evt_ptr->transport_mwi_info.mwi_msg_details[i].priority);
    (void)memscpy(&temp_ptr[pos],
                  4,
                  &type,
                  4);

    pos += 4;

    /* Packing "message id" length and "message id". */
    temp = evt_ptr->transport_mwi_info.mwi_msg_details[i].identifier_len;
    temp_ptr[pos] = temp;
    pos++;
    (void)memscpy(&temp_ptr[pos],
                  temp,
                  (void *)evt_ptr->transport_mwi_info.mwi_msg_details[i].message_id,
                  temp);

    pos += temp;
  }

  *size = buffer_size;
  return temp_ptr;
}

/*===========================================================================
  FUNCTION QMI_WMSI_TRANSPORT_LAYER_MWI_IND()

  DESCRIPTION
    Sends the transpot layer message waiting indication to the client

  PARAMETERS
    cl_sp         : client state pointer
    sub           : subscription that has the message waiting

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wmsi_transport_layer_mwi_ind(
  void * sp,
  wms_msg_event_info_s_type * evt_ptr,
  qmi_wmsi_subscription_e_type sub
)
{
  qmi_wmsi_client_state_type * cl_sp = NULL;
  dsm_item_type *  ind = NULL;
  int i;
  int num_mwi = 0;
  wms_gw_mwi_info_s_type mwi_info[WMSI_MWI_MAX];
  qmi_error_e_type   errval = QMI_ERR_NONE;
  int buf_size = 0;

#ifdef _WIN32
#pragma pack(push, 1) // Save previous, and turn on 1 byte alignment
#endif

  typedef PACK(struct)
  {
    uint8 num_instances;
    PACK(struct)
    {
      uint32 msg_type;
      uint16 new_msg;
      uint16 old_msg;
      uint16 new_urgent;
      uint16 old_urgent;
    } mwi_summary[WMS_TRANSPORT_MWI_MAX];
  } v_summary;

  typedef PACK(struct)
  {
    uint8 ue_addr[WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX];
  } v_ue_addr;

#ifdef _WIN32
#pragma pack (pop) // Revert alignment to what it was previously
#endif

  v_summary v_out_req;
  v_ue_addr v_out_req1;
  uint8 *v_out_opt = NULL;

  cl_sp = (qmi_wmsi_client_state_type *) sp;

  if (NULL == cl_sp || NULL == evt_ptr)
  {
    MSG_ERROR_0("qmi_wmsi_transport_layer_mwi_ind() receives NULL pointer");
    return;
  }

  MSG_HIGH_1("Transport layer MWI indication for client %d", cl_sp->common.clid);

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  memset(&v_out_req,  0, sizeof(v_out_req));
  memset(&v_out_req1, 0, sizeof(v_out_req1));

  if (evt_ptr->transport_mwi_info.summary_tuple_count <= 0)
  {
    MSG_ERROR_1("Missing mandatory summary TLV n = %d", evt_ptr->transport_mwi_info.summary_tuple_count);
    return;
  }

  MSG_HIGH_1("Number of detail tuples = %d",evt_ptr->transport_mwi_info.details_tuple_count);

  if (evt_ptr->transport_mwi_info.details_tuple_count > 0)
  {
    v_out_opt = qmi_wmsi_convert_tpt_mwi_to_opt_tlv(evt_ptr, &buf_size);

    if (NULL != v_out_opt)
    {
      if (FALSE == qmi_svc_put_param_tlv(&ind,
                                         WMSI_PRM_TYPE_TRANSPORT_MWI_DETAILS,
                                         buf_size,
                                         v_out_opt))
      {
        MSG_HIGH_0("Unable to generate Transport Layer Message Waiting indication");
        dsm_free_packet(&ind);
        return;
      }
    }
  }
  
  /* packing the UE address receieved from transport (IMS). */
  if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < evt_ptr->transport_mwi_info.address_len)
  {
    MSG_ERROR_1("Invalid address passed in MWI notification", evt_ptr->transport_mwi_info.address_len);
    return;
  }

  (void)memscpy((void *)v_out_req1.ue_addr,
                evt_ptr->transport_mwi_info.address_len,
                (void *)evt_ptr->transport_mwi_info.msg_account_addr,
                evt_ptr->transport_mwi_info.address_len);

  if (FALSE == qmi_svc_put_param_tlv(&ind,
                                     WMSI_PRM_TYPE_TRANSPORT_MWI_ADDR,
                                     evt_ptr->transport_mwi_info.address_len + 1,
                                     &v_out_req1))
  {
    MSG_HIGH_0("Unable to generate Transport Layer Message Waiting indication");
    dsm_free_packet(&ind);
    return;
  }

  /* packing the summary information for each MWI receieved from transport (IMS). */
  v_out_req.num_instances = MIN(evt_ptr->transport_mwi_info.summary_tuple_count,WMS_TRANSPORT_MWI_MAX);

  for (i = 0; (i < v_out_req.num_instances) && (i < WMS_TRANSPORT_MWI_MAX); i++)
  {
    v_out_req.mwi_summary[i].msg_type   = (uint32)qmi_wms_transport_mwi_to_wmsi_transport_mwi(evt_ptr->transport_mwi_info.mwi_msg_summary[i].msg_type);
    v_out_req.mwi_summary[i].new_msg    = evt_ptr->transport_mwi_info.mwi_msg_summary[i].new_msg;
    v_out_req.mwi_summary[i].old_msg    = evt_ptr->transport_mwi_info.mwi_msg_summary[i].old_msg;
    v_out_req.mwi_summary[i].new_urgent = evt_ptr->transport_mwi_info.mwi_msg_summary[i].new_urgent;
    v_out_req.mwi_summary[i].old_urgent = evt_ptr->transport_mwi_info.mwi_msg_summary[i].old_urgent;
  }

  if (FALSE == qmi_svc_put_param_tlv(&ind,
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     (sizeof(v_out_req.num_instances) +
                                     sizeof(v_out_req.mwi_summary[0]) *
                                     v_out_req.num_instances),
                                     &v_out_req))
  {
    MSG_HIGH_0("Unable to generate Transport Layer Message Waiting indication");
    dsm_free_packet(&ind);
    return;
  }

  /* Build the MWI data to write to EF-MWIS */
  for (i = 0; i < v_out_req.num_instances; i++)
  {
    uint8 type = (wms_gw_msg_waiting_kind_e_type)qmi_wms_transport_mwi_to_wmsi_mwi(evt_ptr->transport_mwi_info.mwi_msg_summary[i].msg_type);
    if (WMSI_MWI_MAX != type)
    {
      mwi_info[i].type       = type;
      mwi_info[i].count      = evt_ptr->transport_mwi_info.mwi_msg_summary[i].new_msg +
                               evt_ptr->transport_mwi_info.mwi_msg_summary[i].old_msg;
      mwi_info[i].is_active  = TRUE;
      ++num_mwi;
    }
  }

  /* Update EF-MWIS with partial information for supported message waiting types */
  errval = (qmi_error_e_type)qmi_wmsi_from_wms_status
           (wms_cfg_ms_set_message_waiting(qmi_wmsi_global.wms_cid,
                                           qmi_wms_get_asid_from_subscription(sub),
                                           (wms_cmd_cb_type) qmi_wms_cmd_status_cb,
                                           NULL,
                                           mwi_info,
                                           num_mwi));
  
  if (QMI_ERR_NONE != errval)
  {
    MSG_ERROR_0("Update to EF-MWIS failed");
    return;
  }

  if (NULL != ind)
  {
    (void)qmi_wmsi_send_ind(cl_sp, WMSI_CMD_VAL_TRANSPORT_LAYER_MWI_IND, ind);
  }

  QMI_WMSI_MEM_FREEIF(v_out_opt);
} /* qmi_wmsi_transport_layer_mwi_ind() */

void qmi_wmsi_generate_transport_layer_mwi_ind(
  wms_msg_event_info_s_type * evt_ptr,
  qmi_wmsi_subscription_e_type sub
)
{
  uint8 cl_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("Generating Transport MWI indication");

  /*--------------------------------------------------------------------------
    Report Transport MWI indication to all clients that have registered for it
  ----------------------------------------------------------------------------*/
  for (cl_idx = 0; cl_idx < WMSI_MAX_CLIDS; cl_idx++)
  {
    qmi_wmsi_client_state_type * cl_sp = qmi_wms_state.client[cl_idx];

    if ((NULL != cl_sp) &&
        (QMI_SVC_CLID_UNUSED != cl_sp->common.clid))
    {
      if ((cl_sp->indication_reg.reg_transport_layer_mwi_events) &&
          (sub == cl_sp->subscription_status.subscription_type))
      {
        qmi_wmsi_transport_layer_mwi_ind(cl_sp, evt_ptr, sub);
      }
    }
  }

  
  if (NULL != evt_ptr)
  {
    QMI_WMSI_MEM_FREEIF(evt_ptr->transport_mwi_info.mwi_msg_summary);
    QMI_WMSI_MEM_FREEIF(evt_ptr->transport_mwi_info.mwi_msg_details);
  }
} /* qmi_wmsi_generate_transport_layer_mwi_ind */

qmi_error_e_type qmi_wmsi_from_wms_status(
  wms_status_e_type wms_err
)
{
  qmi_error_e_type qmi_err;

  switch (wms_err)
  {
    case WMS_OK_S:
      qmi_err = QMI_ERR_NONE;
      break;

    case WMS_UNSUPPORTED_S:
      qmi_err = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      break;

    case WMS_INVALID_FORMAT_S:
    case WMS_INVALID_PARM_SIZE_S:
    case WMS_INVALID_USER_DATA_SIZE_S:
    case WMS_INVALID_PARM_VALUE_S:
    case WMS_MISSING_PARM_S:
      qmi_err = QMI_ERR_ENCODING;
      break;

    case WMS_GENERAL_ERROR_S:
      qmi_err = QMI_ERR_GENERAL;
      break;

    default:
      qmi_err = QMI_ERR_UNKNOWN;
      break;
  }

  return qmi_err;
}

wms_message_tag_e_type qmi_wmsi_check_and_update_wms_tag
(
  wms_message_tag_e_type wms_tag
)
{
   wms_message_tag_e_type new_tag = WMS_TAG_NONE;

   /*
     Spec TS 31.102, section 4.2 decodes the SMS tag as follows:
    
     b8	  b7	b6	b5	b4	b3	b2	b1
      |	   |    |       |       |									
      |	   |    |       |       |	X	X	0	free space
      |	   |    |       |       |	X	X	1	used space
      |	   |    |       |       |	0	0	1	message received by UE from network; message read
      |	   |    |       |       |	0	1	1	message received by UE from network; message to be read
      |	   |    |       |       |	1	1	1	UE originating message; message to be sent
      -    -    -       -       -       -       -       -       RFU (see TS 31.101 [3])
    
    
      b8  b7	b6	b5	b4	b3	b2	b1
										
      |	   |    |       X	X	1	0	1	UE originating message; message sent to the network
      |	   |    |	0	0	1	0	1	Status report not requested
      |	   |    |	0	1	1	0	1	Status report requested but not (yet) received
      |	   |    |	1	0	1	0	1	Status report requested, received but not stored in EF-SMSR
      |	   |    |	1	1	1	0	1	Status report requested, received and stored in EF-SMSR
      -    -    -       -       -       -       -       -       RFU (see TS 31.101 [3]) 

   */
   if (wms_tag & 0x01)
   {
      switch (wms_tag & 0x07)
      {
        case 1: 
          new_tag = WMS_TAG_MT_READ;
          break;
        
        case 3: 
          new_tag = WMS_TAG_MT_NOT_READ;
          break;
                
        case 7: 
          new_tag = WMS_TAG_MO_NOT_SENT;
          break;

        case 5: 
           switch (wms_tag & 0x1F)
           {
             case 5: 
               new_tag = WMS_TAG_MO_SENT;
               break;

             case 13:
               new_tag = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
               break;

             case 22: 
               new_tag = WMS_TAG_MO_SENT_ST_NOT_STORED;
               break;

             case 29: 
               new_tag = WMS_TAG_MO_SENT_ST_STORED;
               break;

             default:
               break;
           }

        default:
          break;
      }
   }

   return new_tag;

}

qmi_wmsi_tag_e_type qmi_wmsi_from_wms_tag(
  wms_message_tag_e_type wms_tag
)
{
  qmi_wmsi_tag_e_type qmi_tag;
  wms_message_tag_e_type tag;

  if ((WMS_TAG_MO_TEMPLATE != wms_tag) && (WMS_TAG_STATUS_RPT != wms_tag))
  {
    tag = qmi_wmsi_check_and_update_wms_tag(wms_tag);
  }
  else
  {
    tag = wms_tag;
  }

  switch (tag)
  {
    case WMS_TAG_MT_READ:
    case WMS_TAG_STATUS_RPT:
      qmi_tag = WMSI_TAG_MT_READ;
      break;

    case WMS_TAG_MT_NOT_READ:
      qmi_tag = WMSI_TAG_MT_NOT_READ;
      break;

    case WMS_TAG_MO_SENT:
#ifdef FEATURE_GWSMS
    case WMS_TAG_MO_SENT_ST_NOT_RECEIVED:
    case WMS_TAG_MO_SENT_ST_NOT_STORED:
    case WMS_TAG_MO_SENT_ST_STORED:
#endif
      qmi_tag = WMSI_TAG_MO_SENT;
      break;

    case WMS_TAG_MO_NOT_SENT:
    case WMS_TAG_MO_TEMPLATE:
      qmi_tag = WMSI_TAG_MO_NOT_SENT;
      break;

    default:
      qmi_tag = WMSI_TAG_MAX;
      break;
  }

  return qmi_tag;
}

wms_message_mode_e_type qmi_wmsi_to_wms_message_mode(
  qmi_wmsi_message_mode_e_type qmi_msg_mode
)
{
  wms_message_mode_e_type wms_msg_mode;

  switch (qmi_msg_mode)
  {
    case WMSI_MSG_MODE_CDMA:
      wms_msg_mode = WMS_MESSAGE_MODE_CDMA;
      break;

    case WMSI_MSG_MODE_GW:
      wms_msg_mode = WMS_MESSAGE_MODE_GW;
      break;

    default:
      wms_msg_mode = WMS_MESSAGE_MODE_MAX;
      break;
  }

  return wms_msg_mode;
}

wms_format_e_type qmi_wmsi_to_wms_format(
  qmi_wmsi_message_format_e_type qmi_format
)
{
  wms_format_e_type wms_format;

  switch (qmi_format)
  {
    case WMSI_MSG_FORMAT_CDMA:
      wms_format = WMS_FORMAT_CDMA;
      break;

    case WMSI_MSG_FORMAT_GW_PP:
    wms_format = WMS_FORMAT_GW_PP;
    break;

    default:
      wms_format = WMS_FORMAT_MAX;
      break;
  }

  return wms_format;
}

qmi_wmsi_message_format_e_type qmi_wmsi_from_wms_format(
  wms_format_e_type wms_format
)
{
  qmi_wmsi_message_format_e_type qmi_format;

  switch (wms_format)
  {
    case WMS_FORMAT_CDMA:
      qmi_format = WMSI_MSG_FORMAT_CDMA;
      break;

    case WMS_FORMAT_GW_PP:
      qmi_format = WMSI_MSG_FORMAT_GW_PP;
      break;

    case WMS_FORMAT_GW_CB:
      qmi_format = WMSI_MSG_FORMAT_GW_BC;
      break;

    case WMS_FORMAT_MWI:
      qmi_format = WMSI_MSG_FORMAT_MWI;
      break;

    default:
      qmi_format = WMSI_MSG_FORMAT_MAX;
      break;
  }

  return qmi_format;
}

wms_memory_store_e_type qmi_wmsi_to_wms_storage(
  qmi_wmsi_memory_storage_e_type qmi_storage_type,
  qmi_wmsi_message_mode_e_type   qmi_message_mode
)
{
  wms_memory_store_e_type wms_storage_type = WMS_MEMORY_STORE_MAX;
  wms_message_mode_e_type wms_message_mode;

  wms_message_mode = qmi_wmsi_to_wms_message_mode(qmi_message_mode);
  switch (qmi_storage_type)
  {
    case WMSI_MEM_STORE_NV:
      if (WMS_MESSAGE_MODE_CDMA == wms_message_mode)
      {
        wms_storage_type = WMS_MEMORY_STORE_NV_CDMA;
      }
      if (WMS_MESSAGE_MODE_GW == wms_message_mode)
      {
        wms_storage_type = WMS_MEMORY_STORE_NV_GW;
      }
      break;

    case WMSI_MEM_STORE_UIM:
#if defined (FEATURE_CDSMS_RUIM)
      if (WMS_MESSAGE_MODE_CDMA == wms_message_mode)
      {
        wms_storage_type = WMS_MEMORY_STORE_RUIM;
      }
#endif /* defined (FEATURE_CDSMS_RUIM) */
      if (WMS_MESSAGE_MODE_GW == wms_message_mode)
      {
        wms_storage_type = WMS_MEMORY_STORE_SIM;
      }
      break;

    case WMSI_MEM_STORE_NONE:
      wms_storage_type = WMS_MEMORY_STORE_NONE;
      break;

    default:
      wms_storage_type = WMS_MEMORY_STORE_MAX;
      break;
  }

  return wms_storage_type;
}

qmi_wmsi_memory_storage_e_type qmi_wmsi_from_wms_storage(
  wms_memory_store_e_type wms_storage_type
)
{
  qmi_wmsi_memory_storage_e_type qmi_storage_type;

  switch (wms_storage_type)
  {
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_CSIM:
#if defined (FEATURE_CDSMS_RUIM)
    case WMS_MEMORY_STORE_RUIM:
#endif /* defined (FEATURE_CDSMS_RUIM) */
      qmi_storage_type = WMSI_MEM_STORE_UIM;
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
    case WMS_MEMORY_STORE_NV_GW:
      qmi_storage_type = WMSI_MEM_STORE_NV;
      break;

    case WMS_MEMORY_STORE_NONE:
      qmi_storage_type = WMSI_MEM_STORE_NONE;
      break;

    default:
      qmi_storage_type = WMSI_MEM_STORE_MAX;
      break;
  }

  return qmi_storage_type;
}


/*===========================================================================
FUNCTION: qmi_wmsi_from_wms_state()                               (INTERNAL)

DESCRIPTION:
  This function maps the mwi_state from WMS to QMI_WMS .

RETURN VALUE
  qmi_wmsi_mwi_state_e_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

qmi_wmsi_mwi_state_e_type qmi_wmsi_from_wms_state(
  wms_mwi_state_e_type wms_state
)
{

  qmi_wmsi_mwi_state_e_type qmi_wmsi_state;

  switch (wms_state)
  {
    case WMS_MWI_STATE_INIT:
      qmi_wmsi_state = WMSI_MWI_STATE_INIT;
      break;

    case WMS_MWI_STATE_ERROR:
      qmi_wmsi_state = WMSI_MWI_STATE_ERROR;
      break;

    default:
      qmi_wmsi_state = WMSI_MWI_STATE_MAX;

  }

  return qmi_wmsi_state;

}

qmi_wmsi_message_mode_e_type qmi_wmsi_message_mode_from_wms_storage(
  wms_memory_store_e_type wms_storage_type
)
{
  qmi_wmsi_message_mode_e_type wmsi_msg_mode;

  switch (wms_storage_type)
  {
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_NV_GW:
      wmsi_msg_mode = WMSI_MSG_MODE_GW;
      break;

    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_NV_CDMA:
      wmsi_msg_mode = WMSI_MSG_MODE_CDMA;
      break;

    default:
      wmsi_msg_mode = WMSI_MSG_MODE_MAX;
      break;
  }

  return wmsi_msg_mode;
}

wms_message_tag_e_type qmi_wmsi_to_wms_tag(
  qmi_wmsi_tag_e_type qmi_tag
)
{
  wms_message_tag_e_type wms_tag;

  switch (qmi_tag)
  {
    case WMSI_TAG_MT_READ:
      wms_tag = WMS_TAG_MT_READ;
      break;

    case WMSI_TAG_MT_NOT_READ:
      wms_tag = WMS_TAG_MT_NOT_READ;
      break;

    case WMSI_TAG_MO_SENT:
      wms_tag = WMS_TAG_MO_SENT;
      break;

    case WMSI_TAG_MO_NOT_SENT:
      wms_tag = WMS_TAG_MO_NOT_SENT;
      break;

    default:
      wms_tag = WMS_TAG_NONE;
      break;
  }

  return wms_tag;
}

wms_message_class_e_type qmi_wmsi_to_wms_message_class(
  qmi_wmsi_message_class_e_type qmi_class
)
{
  wms_message_class_e_type wms_class;

  switch (qmi_class)
  {
    case WMSI_MSG_CLASS_0:
      wms_class = WMS_MESSAGE_CLASS_0;
      break;

    case WMSI_MSG_CLASS_1:
      wms_class = WMS_MESSAGE_CLASS_1;
      break;

    case WMSI_MSG_CLASS_2:
      wms_class = WMS_MESSAGE_CLASS_2;
      break;

    case WMSI_MSG_CLASS_3:
      wms_class = WMS_MESSAGE_CLASS_3;
      break;

    case WMSI_MSG_CLASS_NONE:
      wms_class = WMS_MESSAGE_CLASS_NONE;
      break;

    case WMSI_MSG_CLASS_CDMA:
      wms_class = WMS_MESSAGE_CLASS_CDMA;
      break;

    default:
      wms_class = WMS_MESSAGE_CLASS_MAX;
      break;
  }

  return wms_class;
}

qmi_wmsi_message_class_e_type qmi_wmsi_from_wms_message_class(
  wms_message_class_e_type wms_class
)
{
  qmi_wmsi_message_class_e_type qmi_class;

  switch (wms_class)
  {
    case WMS_MESSAGE_CLASS_0:
      qmi_class = WMSI_MSG_CLASS_0;
      break;

    case WMS_MESSAGE_CLASS_1:
      qmi_class = WMSI_MSG_CLASS_1;
      break;

    case WMS_MESSAGE_CLASS_2:
      qmi_class = WMSI_MSG_CLASS_2;
      break;

    case WMS_MESSAGE_CLASS_3:
      qmi_class = WMSI_MSG_CLASS_3;
      break;

    case WMS_MESSAGE_CLASS_NONE:
      qmi_class = WMSI_MSG_CLASS_NONE;
      break;

    case WMS_MESSAGE_CLASS_CDMA:
      qmi_class = WMSI_MSG_CLASS_CDMA;
      break;

    default:
      qmi_class = WMSI_MSG_CLASS_MAX;
      break;
  }

  return qmi_class;
}

wms_route_e_type qmi_wmsi_to_wms_route(
  qmi_wmsi_route_e_type qmi_route
)
{
  wms_route_e_type wms_route;

  switch (qmi_route)
  {
    case WMSI_ROUTE_DISCARD:
      wms_route = WMS_ROUTE_DISCARD;
      break;

    case WMSI_ROUTE_STORE_AND_NOTIFY:
      wms_route = WMS_ROUTE_STORE_AND_NOTIFY;
      break;

    case WMSI_ROUTE_TRANSFER_ONLY:
      wms_route = WMS_ROUTE_TRANSFER_ONLY;
      break;

    case WMSI_ROUTE_TRANSFER_AND_ACK:
      wms_route = WMS_ROUTE_TRANSFER_AND_ACK;
      break;

    default:
      wms_route = WMS_ROUTE_MAX;
      break;
  }

  return wms_route;
}

qmi_wmsi_route_e_type qmi_wmsi_from_wms_route(
  wms_route_e_type wms_route
)
{
  qmi_wmsi_route_e_type qmi_route;

  switch (wms_route)
  {
    case WMS_ROUTE_DISCARD:
      qmi_route = WMSI_ROUTE_DISCARD;
      break;

    case WMS_ROUTE_STORE_AND_NOTIFY:
      qmi_route = WMSI_ROUTE_STORE_AND_NOTIFY;
      break;

    case WMS_ROUTE_TRANSFER_ONLY:
      qmi_route = WMSI_ROUTE_TRANSFER_ONLY;
      break;

    case WMS_ROUTE_TRANSFER_AND_ACK:
      qmi_route = WMSI_ROUTE_TRANSFER_AND_ACK;
      break;

    default:
      qmi_route = WMSI_ROUTE_UNKNOWN;
      break;
  }

  return qmi_route;
}


qmi_error_e_type qmi_wmsi_from_wms_error(
  wms_cmd_err_e_type wms_err
)
{
  qmi_error_e_type qmi_err;

  switch (wms_err)
  {
    case WMS_CMD_ERR_NONE:
      qmi_err = QMI_ERR_NONE;
      break;

    case WMS_CMD_ERR_NO_RESOURCE:
      qmi_err = QMI_ERR_NO_MEMORY;
      break;

    case WMS_CMD_ERR_UNSUPPORTED:
      qmi_err = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      break;

    case WMS_CMD_ERR_BUSY:
    case WMS_CMD_ERR_MSG_LPM:
      qmi_err = QMI_ERR_DEVICE_NOT_READY;
      break;

    case WMS_CMD_ERR_NO_NETWORK:
      qmi_err = QMI_ERR_NETWORK_NOT_READY;
      break;


    case WMS_CMD_ERR_MSG_INDEX:
      qmi_err = QMI_ERR_INVALID_INDEX;
      break;

    case WMS_CMD_ERR_MSG_EMPTY_MESSAGE:
      qmi_err = QMI_ERR_NO_ENTRY;
      break;

    case WMS_CMD_ERR_MSG_MEMORY_FULL:
    case WMS_CMD_ERR_MSG_RAM_FULL:
    case WMS_CMD_ERR_MSG_SIM_FULL:
    case WMS_CMD_ERR_MSG_NV_FULL:
      qmi_err = QMI_ERR_DEVICE_STORAGE_FULL;
      break;

    case WMS_CMD_ERR_MSG_ENCODE:
    case WMS_CMD_ERR_MSG_DECODE:
    case WMS_CMD_ERR_MSG_TPDU_TYPE:
    case WMS_CMD_ERR_MSG_NO_MO_MSG:
    case WMS_CMD_ERR_MSG_NO_SC_ADDRESS:
      qmi_err = QMI_ERR_ENCODING;
      break;

    case WMS_CMD_ERR_MSG_TAG:
    case WMS_CMD_ERR_MSG_MEMORY_STORE:
    case WMS_CMD_ERR_NULL_PTR:
    case WMS_CMD_ERR_MSG_FORMAT:
    case WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED:
      qmi_err = QMI_ERR_INVALID_ARG;
      break;

    case WMS_CMD_ERR_MSG_BLOCKED:
      qmi_err = QMI_ERR_MSG_BLOCKED;
      break;

    case WMS_CMD_ERR_TRANSPORT_NOT_READY:
      qmi_err = QMI_ERR_INVALID_OPERATION;
      break;

    case WMS_CMD_ERR_MSG_TRANSACTION_ID:
      qmi_err = QMI_ERR_ACK_NOT_SENT;
      break;

    case WMS_CMD_ERR_DBG_BAD_PARAM:
      qmi_err = QMI_ERR_INVALID_ARG;
      break;

    case WMS_CMD_ERR_BC_DEACTIVATE_NOT_ALLOWED:
      qmi_err = QMI_ERR_INVALID_OPERATION;
      break;

    default:
      qmi_err = QMI_ERR_UNKNOWN;
  }

  return qmi_err;
}

qmi_error_e_type qmi_wmsi_from_wms_report_error(
  wms_report_status_e_type wms_report_err
)
{
  qmi_error_e_type qmi_err;

  switch (wms_report_err)
  {
    case WMS_RPT_OK:
      qmi_err = QMI_ERR_NONE;
      break;

    case WMS_RPT_ACCESS_TOO_LARGE:
    case WMS_RPT_DC_TOO_LARGE:
      qmi_err = QMI_ERR_MESSAGE_NOT_SENT;
      break;

    case WMS_RPT_NETWORK_NOT_READY:
    case WMS_RPT_RADIO_UNAVAILABLE :
      qmi_err = QMI_ERR_NETWORK_NOT_READY;
      break;

    case WMS_RPT_PHONE_NOT_READY:
      qmi_err = QMI_ERR_DEVICE_NOT_READY;
      break;

    case WMS_RPT_GENERAL_ERROR:
      qmi_err = QMI_ERR_GENERAL;
      break;

    default:
      qmi_err = QMI_ERR_MESSAGE_DELIVERY_FAILURE;
  }

  return qmi_err;
}

uint16 qmi_wmsi_from_wms_cause_error(
  wms_cdma_tl_status_e_type wms_causecode
)
{
  uint16 qmi_causecode;

  switch (wms_causecode)
  {
    default:
      qmi_causecode = (uint16) wms_causecode;
  }

  return qmi_causecode;
}

qmi_wmsi_error_class_e_type qmi_wmsi_from_wms_error_class(
  wms_error_class_e_type wms_errorclass
)
{
  qmi_wmsi_error_class_e_type qmi_errorclass;

  switch (wms_errorclass)
  {
    case WMS_ERROR_TEMP:
      qmi_errorclass = WMSI_ERROR_CLASS_TEMP;
      break;

    case WMS_ERROR_PERM:
      qmi_errorclass = WMSI_ERROR_CLASS_PERM;
      break;

    default:
      qmi_errorclass = WMSI_ERROR_CLASS_MAX;
  }

  return qmi_errorclass;
}

qmi_wmsi_mo_ctrl_status_e_type qmi_wmsi_from_wms_mo_ctrl_status(
  wms_mo_ctrl_status_e_type mo_ctrl_type
)
{
  qmi_wmsi_mo_ctrl_status_e_type mo_ctrl_status;

  switch (mo_ctrl_type)
  {
    case WMS_MO_CTRL_STATUS_NO_MOD:
      mo_ctrl_status = WMSI_MO_CTRL_STATUS_TYPE_NO_MOD;
      break;

    case WMS_MO_CTRL_STATUS_MOD:
      mo_ctrl_status = WMSI_MO_CTRL_STATUS_TYPE_MOD;
      break;

    case WMS_MO_CTRL_STATUS_DISALLOWED:
      mo_ctrl_status = WMSI_MO_CTRL_STATUS_TYPE_DISALLOWED;
      break;

    default:
      mo_ctrl_status = WMSI_MO_CTRL_STATUS_TYPE_MAX;
  }

  return mo_ctrl_status;
}

wms_bc_pref_e_type qmi_wmsi_to_wms_bc_pref(
  qmi_wmsi_bc_pref_e_type qmi_bc_pref,
  boolean activate_all
)
{

  wms_bc_pref_e_type wms_bc_pref;

  switch (qmi_bc_pref)
  {
    case WMSI_BC_PREF_DISABLE:
      wms_bc_pref = WMS_BC_PREF_DEACTIVATE;
      break;

    case WMSI_BC_PREF_ENABLE:
      if (activate_all)
      {
        wms_bc_pref = WMS_BC_PREF_ACTIVATE_ALL;
      }
      else
      {
        wms_bc_pref = WMS_BC_PREF_ACTIVATE_TABLE;
      }
      break;

    default:
      wms_bc_pref = WMS_BC_PREF_MAX;
      break;
  }

  return wms_bc_pref;
}

qmi_wmsi_bc_pref_e_type qmi_wms_to_wmsi_bc_pref(
  wms_bc_pref_e_type wms_bc_pref
)
{
  qmi_wmsi_bc_pref_e_type qmi_bc_pref;

  switch (wms_bc_pref)
  {
    case WMS_BC_PREF_DEACTIVATE:
      qmi_bc_pref = WMSI_BC_PREF_DISABLE;
      break;

    case WMS_BC_PREF_ACTIVATE_TABLE:
    case WMS_BC_PREF_ACTIVATE_ALL:
      qmi_bc_pref = WMSI_BC_PREF_ENABLE;
      break;

    default:
      qmi_bc_pref = WMSI_BC_PREF_MAX;
      break;
  }

  return qmi_bc_pref;
}

qmi_wmsi_mwi_e_type qmi_wms_to_wmsi_mwi(
  wms_gw_msg_waiting_kind_e_type wms_mwi
)
{
  qmi_wmsi_mwi_e_type qmi_mwi;

  switch (wms_mwi)
  {
    case WMS_GW_MSG_WAITING_VOICEMAIL:
      qmi_mwi = WMSI_MWI_VOICEMAIL;
      break;

    case WMS_GW_MSG_WAITING_FAX:
      qmi_mwi = WMSI_MWI_FAX;
      break;

    case WMS_GW_MSG_WAITING_EMAIL:
      qmi_mwi = WMSI_MWI_EMAIL;
      break;

    case WMS_GW_MSG_WAITING_OTHER:
      qmi_mwi = WMSI_MWI_OTHER;
      break;

    case WMS_GW_MSG_WAITING_VIDEOMAIL:
      qmi_mwi = WMSI_MWI_VIDEOMAIL;
      break;

    default:
      qmi_mwi = WMSI_MWI_MAX;
      break;
  }

  return qmi_mwi;
}

qmi_wmsi_mwi_priority_e_type qmi_wms_transport_mwi_priority_to_wmsi_mwi_priority
(
  wms_msg_transport_mwi_priority_e_type priority
)
{
  qmi_wmsi_mwi_priority_e_type qmi_priority;

  switch (priority)
  {
    case WMS_TRANSPORT_MWI_MSG_PRIORITY_UNKNOWN:
      qmi_priority = WMSI_MWI_UNKNOWN_PRIORITY;
      break;

    case WMS_TRANSPORT_MWI_MSG_PRIORITY_LOW:
      qmi_priority = WMSI_MWI_LOW_PRIORITY;
      break;

    case WMS_TRANSPORT_MWI_MSG_PRIORITY_NORMAL:
      qmi_priority = WMSI_MWI_NORMAL_PRIORITY;
      break;

    case WMS_TRANSPORT_MWI_MSG_PRIORITY_URGENT:
      qmi_priority = WMSI_MWI_URGENT_PRIORITY;
      break;

    default:
      qmi_priority = WMSI_MWI_PRIORITY_MAX;
      break;
  }

  return qmi_priority;
}

qmi_wmsi_transport_mwi_e_type qmi_wms_transport_mwi_to_wmsi_transport_mwi(
  wms_msg_transport_mwi_kind_e_type wms_mwi
)
{
  qmi_wmsi_transport_mwi_e_type qmi_mwi;

  switch (wms_mwi)
  {
    case WMS_TRANSPORT_MWI_MSG_VOICE:
      qmi_mwi = WMSI_TRANSPORT_MWI_VOICE;
      break;

    case WMS_TRANSPORT_MWI_MSG_VIDEO:
      qmi_mwi = WMSI_TRANSPORT_MWI_VIDEO;
      break;

    case WMS_TRANSPORT_MWI_MSG_FAX:
      qmi_mwi = WMSI_TRANSPORT_MWI_FAX;
      break;

    case WMS_TRANSPORT_MWI_MSG_PAGER:
      qmi_mwi = WMSI_TRANSPORT_MWI_PAGER;
      break;
    
    case WMS_TRANSPORT_MWI_MSG_MULTIMEDIA:
      qmi_mwi = WMSI_TRANSPORT_MWI_MULTIMEDIA;
      break;

    case WMS_TRANSPORT_MWI_MSG_TEXT:
      qmi_mwi = WMSI_TRANSPORT_MWI_TEXT;
      break;

    default:
      qmi_mwi = WMSI_TRANSPORT_MWI_MAX;
      break;
  }

  return qmi_mwi;
}

qmi_wmsi_mwi_e_type qmi_wms_transport_mwi_to_wmsi_mwi(
  wms_msg_transport_mwi_kind_e_type wms_mwi
)
{
  qmi_wmsi_mwi_e_type qmi_mwi;

  switch (wms_mwi)
  {
    case WMS_TRANSPORT_MWI_MSG_VOICE:
      qmi_mwi = WMSI_MWI_VOICEMAIL;
      break;

    case WMS_TRANSPORT_MWI_MSG_FAX:
      qmi_mwi = WMSI_MWI_FAX;
      break;

    case WMS_TRANSPORT_MWI_MSG_VIDEO:
      qmi_mwi = WMSI_MWI_VIDEOMAIL;
      break;

    default:
      qmi_mwi = WMSI_MWI_MAX;
      break;
  }

  return qmi_mwi;
}

qmi_wmsi_subscription_e_type qmi_wms_get_subscription_from_asid(
  sys_modem_as_id_e_type as_id
)
{
  qmi_wmsi_subscription_e_type qmi_wms_subscription;

#ifdef FEATURE_DUAL_SIM
  switch (as_id)
  {
    case SYS_MODEM_AS_ID_1:
      qmi_wms_subscription = WMSI_SUBSCRIPTION_PRIMARY;
      break;

    case SYS_MODEM_AS_ID_2:
      qmi_wms_subscription = WMSI_SUBSCRIPTION_SECONDARY;
      break;

    case SYS_MODEM_AS_ID_3:
      qmi_wms_subscription = WMSI_SUBSCRIPTION_TERTIARY;
      break;

    default:
      qmi_wms_subscription = WMSI_SUBSCRIPTION_MAX;
      break;
  }
#else
  qmi_wms_subscription = WMSI_SUBSCRIPTION_PRIMARY;
#endif

  return qmi_wms_subscription;
}

sys_modem_as_id_e_type qmi_wms_get_asid_from_subscription(
  qmi_wmsi_subscription_e_type qmi_wms_subscription
)
{
  sys_modem_as_id_e_type as_id;

  switch (qmi_wms_subscription)
  {
    case WMSI_SUBSCRIPTION_PRIMARY:
      as_id = SYS_MODEM_AS_ID_1;
      break;

    case WMSI_SUBSCRIPTION_SECONDARY:
      as_id = SYS_MODEM_AS_ID_2;
      break;

    case WMSI_SUBSCRIPTION_TERTIARY:
      as_id = SYS_MODEM_AS_ID_3;
      break;

    default:
      as_id = SYS_MODEM_AS_ID_NONE;
      break;
  }

  return as_id;
}

qmi_wmsi_transport_type_e_type qmi_wms_to_wmsi_transport_type(
  wms_msg_transport_type_e_type wms_transport_type
)
{
  qmi_wmsi_transport_type_e_type qmi_transport_type;

  switch (wms_transport_type)
  {
    case WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS:
      qmi_transport_type = WMSI_TRANSPORT_TYPE_IMS;
      break;

    default:
      qmi_transport_type = WMSI_TRANSPORT_TYPE_MAX;
      break;
  }

  return qmi_transport_type;
}

qmi_wmsi_message_mode_e_type qmi_wms_to_wmsi_transport_cap(
  uint32 wms_transport_cap
)
{
  qmi_wmsi_message_mode_e_type qmi_transport_cap;

  switch (wms_transport_cap)
  {
    case WMS_MSG_TRANSPORT_CAP_3GPP2_1X:
    case WMS_MSG_TRANSPORT_CAP_3GPP2_EVDO:
    case WMS_MSG_TRANSPORT_CAP_3GPP2_IMS:
      qmi_transport_cap = WMSI_MSG_MODE_CDMA;
      break;

    case WMS_MSG_TRANSPORT_CAP_3GPP_CS:
    case WMS_MSG_TRANSPORT_CAP_3GPP_PS:
    case WMS_MSG_TRANSPORT_CAP_3GPP_IMS:
    case WMS_MSG_TRANSPORT_CAP_3GPP_LTE:
      qmi_transport_cap = WMSI_MSG_MODE_GW;
      break;

    default:
      qmi_transport_cap = WMSI_MSG_MODE_MAX;
      break;
  }

  return qmi_transport_cap;
}

qmi_wmsi_transport_nw_reg_status_e_type qmi_wms_to_wmsi_transport_nw_reg(
  wms_msg_transport_nw_reg_status_e_type wms_nw_reg_status
)
{
  qmi_wmsi_transport_nw_reg_status_e_type qmi_nw_reg_status;

  switch (wms_nw_reg_status)
  {
    case WMS_MSG_TRANSPORT_NW_REG_STATUS_NO_SRV:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_NO_SERVICE;
      break;

    case WMS_MSG_TRANSPORT_NW_REG_STATUS_IN_PROGRESS:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_IN_PROGRESS;
      break;

    case WMS_MSG_TRANSPORT_NW_REG_STATUS_FAILED:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_FAILED;
      break;

    case WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_LIM_SERVICE;
      break;

    case WMS_MSG_TRANSPORT_NW_REG_STATUS_FULL_SRV:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_FULL_SERVICE;
      break;

    default:
      qmi_nw_reg_status = WMSI_TRANSPORT_NW_REG_MAX;
      break;
  }

  return qmi_nw_reg_status;
}

qmi_wmsi_call_status_e_type qmi_wms_to_wmsi_call_status(
  wms_dc_event_e_type wms_call_status
)
{
  qmi_wmsi_call_status_e_type qmi_call_status;

  switch (wms_call_status)
  {
    case WMS_DC_EVENT_INCOMING:
      qmi_call_status = WMSI_CALL_STATUS_INCOMING;
      break;

    case WMS_DC_EVENT_CONNECTED:
      qmi_call_status = WMSI_CALL_STATUS_CONNECTED;
      break;

    case WMS_DC_EVENT_ABORTED:
      qmi_call_status = WMSI_CALL_STATUS_ABORTED;
      break;

    case WMS_DC_EVENT_DISCONNECTED:
      qmi_call_status = WMSI_CALL_STATUS_DISCONNECTED;
      break;

    case WMS_DC_EVENT_CONNECTING:
      qmi_call_status = WMSI_CALL_STATUS_CONNECTING;
      break;

    default:
      qmi_call_status = WMSI_CALL_STATUS_MAX;
      break;
  }

  return qmi_call_status;
}

/*===========================================================================

FUNCTION    QMI_WMSI_STR_TO_ADDR

DESCRIPTION
  Checks to see if <toda> field is specified by the user.
  If not, parses the <da> string for address length, type of number and
  numbering plan.
  If <toda> filed is specified, <toda> is split into type of number
  and numbering plan.

DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_wmsi_str_to_addr(
 wms_address_s_type      *addr,   /* Pointer to address structure to
                                     be filled                             */
 byte                    * da,    /* Pointer to destination address        */
 const byte              * toda,  /* Pointer to type of address            */
 qmi_wmsi_fmt_mode_e_type sms_mode, /* SMS mode Enum                         */
 qmi_wmsi_message_mode_e_type message_mode /* Message mode */
)
{
  uint8 loop=0, toa=0, toda_type;
  byte * s;
  byte c;
  uint32 type_of_address;
  boolean result = TRUE;

  /*check to see if optional <da> and <toda> are supplied by the user*/
  /* new <da>: no type of address */
  if ((NULL == toda) || (*toda == '\0'))
  {
    /* if <da> begins with '+' type-of-address is 145 */
    if (*da == '+')
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
      da++;
    }
    else /* default type-of-address is 129 */
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_UNKNOWN;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    }
  }
  /* We got a type of address */
  else
  {
    if (QMI_WMS_TEXT_MODE == sms_mode)
    {
      /* It is a decimal number if we are in text mode */
      toda_type = 10;
    }
    else
    {
      /* It is a hex number if we are in pdu mode */
      toda_type = 16;
    }

    if (WMSI_ATOI_OK != qmi_wmsi_util_atoi(&type_of_address, toda, toda_type))
    {
      /* Bad type of address */
      return FALSE;
    }

    /* Spec 3GPP TS 23.0.40, Section 9.2.1.5 specifies that Type-of-Address field is one octet long. */
    if (type_of_address > 255)
    {
      /* Over flow */
      return FALSE;
    }

    toa = (uint8)type_of_address;
    addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
    addr->number_type = (wms_number_type_e_type)((toa & 0x70)>>4);
    addr->number_plan = (wms_number_plan_e_type)(toa & 0x0F);

    /* Ignore the '+' */
    if (*da == '+')
    {
      da++;
      /* Needs to be International */
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
    }
  }

  s = da;
  /* Initializing the number mode*/
  addr->number_mode = WMS_NUMBER_MODE_DATA_NETWORK_MAX32;

  while((c = *s++) != '\0')
  {
    qmi_wmsi_dial_val_e_type vresult = qmi_wmsi_validate_dial_char(&c);
    if (QMI_WMSI_DIAL_DIGIT == vresult)
    {
      c -= '0';
    }
    else if (QMI_WMSI_DIAL_PLUS == vresult)
    {
      return FALSE;
    }
    else
    {
      if (QMI_WMS_TEXT_MODE == sms_mode)
      {
        switch (vresult)
        {
          case QMI_WMSI_DIAL_ASCII_ABC:
            c -= 53;  /* 'A' - 12 */
            break;
          case QMI_WMSI_DIAL_STAR:
            c = 10;
            break;
          case QMI_WMSI_DIAL_POUND:
            c = 11;
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }
      else /* QMI_WMS_PDU_MODE */
      {
        /* Mapping in PDU mode: */
        /*   'A' --> '*'        */
        /*   'B' --> '#'        */
        /*   'C' --> 'A'        */
        /*   'D' --> 'B'        */
        /*   'E' --> 'C'        */
        switch (vresult)
        {
          case QMI_WMSI_DIAL_ASCII_ABC:
          case QMI_WMSI_DIAL_ASCII_D:
          case QMI_WMSI_DIAL_ASCII_E:
            c -= 55;  /* 'A' - 10 */
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }/* else */
    }

    if (WMSI_MSG_MODE_GW == message_mode)
    {
      addr->digits[loop] = c;
    }

    loop++;

    /* Spec TS 23.040 Section 9.1.2.5 and 9.2.3.8 specify that the maximum length of the
       full address field (Address Length, Type of Address and Address Value) is 12 octets.
       The length of the destination address should be between 3 to 12 octets (20 semi-octets). */
    if (loop > WMS_GW_ADDRESS_MAX)
    {
      return FALSE;
    }
  }/* while "" */

  if (WMSI_MSG_MODE_CDMA == message_mode)
  {
    da[loop] = '\0';
    if (wms_ts_ascii2dtmf((const char *) da,(uint8 *) &addr->digits[0])== 0)
    {
      MSG_ERROR_0("Unsupported Address character");
      return FALSE;
    }
  }

  addr->number_of_digits = loop;
  if (!addr->number_of_digits)
  {
    MSG_HIGH_0("No Number available");
    return FALSE;
  }

  return result;
}/* qmi_wmsi_str_to_addr */


/*===========================================================================

FUNCTION QMI_WMSI_UTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFFFFFF and a radix of 2,
  which returns 33 bytes (32 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * qmi_wmsi_util_itoa(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
)
{
  /* The worst case is the value 0xFFFFFFFF and a radix of 2,
     which returns 33 bytes (32 one's and the null terminator). */
  byte buf[33], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      MSG_HIGH_0("OVERFLOW");
      break;
    }

    buf[--n] = (byte) ((c > 9) ? c + 'A' - 10 : c + '0');

  } while ((v /= r) > 0);

  while ((*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  qmi_wmsi_util_itoa */



/*===========================================================================

FUNCTION QMI_WMSI_VALIDATE_DIAL_CHAR

DESCRIPTION
  This function examines the passed dial string charater and returns a
  validation result.  It is up to the caller to act upon the result as
  approppriate within its context.

  Refer to 3GPP TS 27.007 section 6.2 for details on the valid dialing
  digits.  The supported set are summarized as follows:
    0 - 9
    A - C
    + * ; #
    D - E             (ETSI SMS PDU mode)
    W , $ T P ! @ D   (accepted but flagged for strip)

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating validation result.

SIDE EFFECTS
  None

===========================================================================*/
qmi_wmsi_dial_val_e_type qmi_wmsi_validate_dial_char(
  const byte * c_ptr                 /*  Input character   */
)
{
  qmi_wmsi_dial_val_e_type result =  QMI_WMSI_DIAL_EMPTY;

  /* Characters that are ignored in a dial string */
  const byte d_strip        [] = "W,$TP!@D";

  /* Test for empty input */
  if (*c_ptr != '\0')
  {
    byte c = UPCASE (*c_ptr);

    /* Catagorize character */
    if (c >= '0' && c <= '9')
    {
      result = QMI_WMSI_DIAL_DIGIT;
    }
    else if (c >= 'A' && c <= 'C')
    {
      result = QMI_WMSI_DIAL_ASCII_ABC;
    }
    else if (c == 'D')
    {
      result = QMI_WMSI_DIAL_ASCII_D;
    }
    else if (c == 'E')
    {
      result = QMI_WMSI_DIAL_ASCII_E;
    }
    else if (c == '*')
    {
      result = QMI_WMSI_DIAL_STAR;
    }
    else if (c == '+')
    {
      result = QMI_WMSI_DIAL_PLUS;
    }
    else if (c == '#')
    {
      result = QMI_WMSI_DIAL_POUND;
    }
    else if (c == ';')
    {
      result = QMI_WMSI_DIAL_SEMICOLON;
    }
    else if (0 != strchr ((char*)d_strip, (char)c))
    {
      result = QMI_WMSI_DIAL_STRIP;
    }
    else
    {
      result = QMI_WMSI_DIAL_IGNORE;
    }
  }
  return result;
} /* qmi_wmsi_validate_dial_char () */


/*===========================================================================

FUNCTION QMI_WMSI_UTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns
    ATOI_OK           : for a correct conversion,
    ATOI_NO_ARG       : 1 if no argument was found,
    ATOI_OUT_OF_RANGE : if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/

qmi_wmsi_atoi_enum_type qmi_wmsi_util_atoi(
  uint32 *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
)
{
  qmi_wmsi_atoi_enum_type err_ret = WMSI_ATOI_NO_ARG;
  byte c;
  uint32 val, val_lim, dig_lim;

  val = 0;
  val_lim = (uint32) ((unsigned int)QMI_WMSI_MAX_VAL_NUM_ITEM / r);
  dig_lim = (uint32) ((unsigned int)QMI_WMSI_MAX_VAL_NUM_ITEM % r);

  while ((c = *s++) != '\0')
  {
    if (c != ' ')
    {
      c = (byte) UPCASE (c);
      if (c >= '0' && c <= '9')
      {
        c -= '0';
      }
      else if (c >= 'A')
      {
        c -= 'A' - 10;
      }
      else
      {
        err_ret = WMSI_ATOI_OUT_OF_RANGE;  /*  char code too small */
        break;
      }

      if (c >= r || val > val_lim
          || (val == val_lim && c > dig_lim))
      {
        err_ret = WMSI_ATOI_OUT_OF_RANGE;  /*  char code too large */
        break;
      }
      else
      {
        err_ret = WMSI_ATOI_OK;            /*  arg found: OK so far*/
        val = (uint32) (val * r + c);
      }
    }
    *val_arg_ptr =  val;
  }

  return err_ret;

} /*  qmi_wmsi_util_atoi */

/*===========================================================================
  FUNCTION QMI_WMSI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wmsi_deep_copy(
  void** dest,
  void* src,
  uint32 size
)
{
  if (NULL != dest)
  {
    *dest = NULL;
    if (NULL != src && size > 0)
    {
      *dest = qmi_wmsi_mem_alloc(size);
      if (NULL != *dest)
      {
        (void)memscpy(*dest, size, src, size);
      }
    }
  }
} /* qmi_wmsi_deep_copy */



/*===========================================================================
  FUNCTION QMI_WMSI_MOBILE_IS_IMS_REGISTERED()

  DESCRIPTION
    Indicates whether the mobile is IMS registered or not on a particular subscription.

  PARAMETERS
    sys_modem_as_id_e_type.

  RETURN VALUE
    TRUE if mobile is IMS registered on client's subscription.
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_wmsi_mobile_is_ims_registered(
  sys_modem_as_id_e_type client_as_id
)
{
  if ((client_as_id == qmi_wmsi_global.transport_layer.as_id) &&
      (WMSI_TRANSPORT_LAYER_STATE_INIT == qmi_wmsi_global.transport_layer.state) &&
      (TRUE == qmi_wmsi_global.transport_layer.registered_ind) &&
      (WMSI_TRANSPORT_TYPE_IMS == qmi_wmsi_global.transport_layer.transport_type))
  {
    return TRUE;
  }

  return FALSE;

} /* qmi_wmsi_mobile_is_ims_registered */
