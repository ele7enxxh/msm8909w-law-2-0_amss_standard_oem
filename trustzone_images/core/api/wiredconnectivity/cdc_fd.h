/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file cdc_fd.h

Contains basic definitions and implementation of the CDC FD (Communication Device 
Class Function Driver) that supports the Abstract Control Model (ACM), Ethernet 
Control Model (ECM), and Object Exchange Communication Model (OBEX).

The definitions in this file are are mostly internal to the CDC layer, with
some minor exceptions.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/cdc_fd.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/24/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _CDC_FD_H_
#define _CDC_FD_H_
#include <uw_device_cdc.h>

#include <uw_device_cdc_acm.h>
#include <uw_device_cdc_ecm.h>
#include <uw_device_cdc_obex.h>
#include <jusb_common.h>


/** @ingroup hsusb_jungo_api 
@{ */

/* CCI subclass codes */
#define USB_CDC_SUBCLASS_ACM            0x02
#define USB_CDC_SUBCLASS_ECM            0x06
#define USB_CDC_SUBCLASS_OBEX           0x0B

/* CCI protocol codes */
#define USB_CDC_CTRL_PROTO_NONE         0x00
#define USB_CDC_CTRL_PROTO_AT           0x01    /* Hayes compatible */

/* Functional descriptor subtypes */
#define USB_CDC_SUBTYPE_HEADER          0x00
#define USB_CDC_SUBTYPE_CALL_MGMT       0x01
#define USB_CDC_SUBTYPE_ACM             0x02
#define USB_CDC_SUBTYPE_UNION           0x06
#define USB_CDC_SUBTYPE_COUNTRY         0x07
#define USB_CDC_SUBTYPE_ECM             0x0F
#define USB_CDC_SUBTYPE_OBEX            0x15


#define USB_CDC_SUBTYPE_MDLM            0x12
#define USB_CDC_SUBTYPE_MDLM_DETAIL     0x13

#define USB_CDC_MDLM_DETAIL_GUID_DISCRIMINATOR           0x0
#define USB_CDC_MDLM_DETAIL_CONTROL_CAPS_VENDOR_SPECIFIC 0x1
#define USB_CDC_MDLM_DETAIL_DATA_CAPS_BYTE_PADDING      0x10
#define USB_CDC_MDLM_DETAIL_BI_DIR_NMEA                 0x20


/* Functional Descriptors 
*/
__PACKED_PRE__
struct cdc_header_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdCDC;
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_call_mgmt_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bmCapabilities;
    juint8_t bDataIface;
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_acm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bmCapabilities;
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_union_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bMasterIface;
    juint8_t bSlaveIface[1]; /* Should be able to handle more slave interfaces */
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_ecm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t iMACAddress;
    juint32_t bmEtherStats;
    juint16_t wMaxSegSize;
    juint16_t wNumMCFilters;
    juint8_t bNumPowerFilters;
} __PACKED_POST__;

__PACKED_PRE__
struct wmcdc_obex_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdVersion;
} __PACKED_POST__;


/** GUID representing the device vendor (QCOM).\ This will be incorporated 
  inside the functional descriptors of the various CDC FDs.
*/
#define QCOM_16_BYTES_GUID {0x98, 0xB0, 0x6A, 0x49, 0xB0, 0x9E, 0x48, 0x96,\
                            0x94, 0x46, 0xd9, 0x9a, 0x28, 0xca, 0x4e, 0x5d}

__PACKED_PRE__
struct cdc_mdlm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdVersion;
    juint8_t bGUID[16];
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_mdlm_detail_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bGUIDType;
    juint8_t bmCtrlCapabilities;
    juint8_t bmDataCapabilities;
} __PACKED_POST__;


/** Request codes defined by the CDC. 
*/
enum cdc_requests_t
{
    CDC_REQ_SEND_ENCAPSULATED_COMMAND = 0x00,      /**< &nbsp; */
    CDC_REQ_GET_ENCAPSULATED_RESPONSE = 0x01,      /**< &nbsp; */
    CDC_REQ_SET_COMM_FEATURE = 0x02,               /**< &nbsp; */
    CDC_REQ_GET_COMM_FEATURE = 0x03,               /**< &nbsp; */
    CDC_REQ_CLEAR_COMM_FEATURE = 0x04,             /**< &nbsp; */
    CDC_REQ_SET_AUX_LINE_STATE = 0x10,             /**< &nbsp; */
    CDC_REQ_SET_HOOK_STATE = 0x11,                 /**< &nbsp; */
    CDC_REQ_PULSE_SETUP = 0x12,                    /**< &nbsp; */
    CDC_REQ_SEND_PULSE = 0x013,                    /**< &nbsp; */
    CDC_REQ_SET_PULSE_TIME = 0x14,                 /**< &nbsp; */
    CDC_REQ_RING_AUX_JACK = 0x15,                  /**< &nbsp; */
    CDC_REQ_SET_LINE_CODING = 0x20,                /**< &nbsp; */
    CDC_REQ_GET_LINE_CODING = 0x21,                /**< &nbsp; */
    CDC_REQ_SET_CONTROL_LINE_STATE = 0x22,         /**< &nbsp; */
    CDC_REQ_SEND_BREAK = 0x23,                     /**< &nbsp; */
    CDC_REQ_SET_RINGER_PARMS = 0x30,               /**< &nbsp; */
    CDC_REQ_GET_RINGER_PARMS = 0x31,               /**< &nbsp; */
    CDC_REQ_SET_OPERATION_PARMS = 0x32,            /**< &nbsp; */
    CDC_REQ_GET_OPERATION_PARMS = 0x33,            /**< &nbsp; */
    CDC_REQ_SET_LINE_PARMS = 0x34,                 /**< &nbsp; */
    CDC_REQ_GET_LINE_PARMS = 0x35,                 /**< &nbsp; */
    CDC_REQ_DIAL_DIGITS = 0x36,                    /**< &nbsp; */
    CDC_REQ_SET_UNIT_PARAMETER = 0x37,             /**< &nbsp; */
    CDC_REQ_GET_UNIT_PARAMETER = 0x38,             /**< &nbsp; */
    CDC_REQ_CLEAR_UNIT_PARAMETER = 0x39,           /**< &nbsp; */
    CDC_REQ_GET_PROFILE = 0x3A,                    /**< &nbsp; */
    CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS = 0x40, /**< &nbsp; */
    CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER = 0x41, /**< &nbsp; */
    CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER = 0x42, /**< &nbsp; */
    CDC_REQ_SET_ETHERNET_PACKET_FILTER = 0x43,     /**< &nbsp; */
    CDC_REQ_GET_ETHERNET_STATISTIC = 0x44,         /**< &nbsp; */
    CDC_REQ_SET_ATM_DATA_FORMAT = 0x50,            /**< &nbsp; */
    CDC_REQ_GET_ATM_DEVICE_STATISTICS = 0x51,      /**< &nbsp; */
    CDC_REQ_SET_ATM_DEFAULT_VC = 0x52,             /**< &nbsp; */
    CDC_REQ_GET_ATM_VC_STATISTICS = 0x53           /**< &nbsp; */
};

/** Notification codes defined by the CDC. 
*/
enum cdc_notifications_t
{
    CDC_NTF_NETWORK_CONNECTION = 0x00,      /**< &nbsp; */
    CDC_NTF_RESPONSE_AVAILABLE = 0x01,      /**< &nbsp; */
    CDC_NTF_AUX_JACK_HOOK_STATE = 0x08,     /**< &nbsp; */
    CDC_NTF_RING_DETECT = 0x09,             /**< &nbsp; */
    CDC_NTF_SERIAL_STATE = 0x20,            /**< &nbsp; */
    CDC_NTF_CALL_STATE_CHANGE = 0x28,       /**< &nbsp; */
    CDC_NTF_LINE_STATE_CHANGE = 0x29,       /**< &nbsp; */
    CDC_NTF_CONNECTION_SPEED_CHANGE = 0x2A  /**< &nbsp; */
};

/** CDC pipe information. 
*/
typedef struct
{
    pipe_desc_t *desc;          /**< Pointer to the core's pipe structure. */
    juint16_t if_num;           /**< Interface number for this CDC instance. */
} cdc_pipe_t;

/** CDC context information. 
*/
typedef struct _cdc_ctx_t
{
    void *core_ctx;            /**< Core context. */
    cdc_appctx_t app_ctx;      /**< Application context. */
    fd_ops_t ops;              /**< Core callbacks. */
    fd_desc_t *desc;           /**< Descriptors for the USB Core. */
    cdc_pipe_t in_pipe;        /**< IN pipe parameters structure */
    cdc_pipe_t out_pipe;       /**< OUT pipe parameters structure. */
    cdc_pipe_t intr_pipe;      /**< INTERRUPT pipe parameters structure. */
    request_t *ctrl_read;      /**< Preallocated request for control reads. */
    request_t *ctrl_write;     /**< Preallocated request for control writes. */

    juint8_t app_enabled;       /**< Application is disabled/enabled. */

    juint16_t usb_max_packet_size; /**< Current maximum packet size, as
								        determined by the connection speed. */

    /** Stores the last setup packet while waiting for control data to arrive 
	    from the host.
	*/
    struct usb_ctrlrequest setup_packet;
    
    /** State and data for sending an encapsulated response.
	*/
    struct
    {
        enum
        {
            CDC_ENCAP_READY=0,
            CDC_ENCAP_RESP_SENDING,
            CDC_ENCAP_RESP_SENT
        } state;
        
        jbool_t notification_pending;
        jbool_t sending_zldp;
        
        void *buffer;
        juint32_t size;
        juint32_t bytes_sent;
    } encap_response;

    /** Relevant callbacks according to the model that is implemented.
	*/
    union
    {
        cdc_callbacks_t basic;
        acm_callbacks_t acm;
        ecm_callbacks_t ecm;
        obex_callbacks_t obex;
    } cbs;
    
    /** Relevant data according to the model that is implemented.
	*/
    union
    {
        cdc_init_info_t basic;
        acm_init_info_t acm;
        ecm_init_info_t ecm;
        obex_init_info_t obex;
    } info;
} cdc_ctx_t;

/* Internal APIs */

cdc_iostatus_t cdc_get_iostatus(request_t *req, jbool_t check_size);

typedef juint32_t (*cdc_get_func_headers_fn)(cdc_ctx_t *ctx, void *buffer, 
    jbool_t is_single_iface, juint8_t interface_number);

/** Information required to fill out a CDC descriptor.
*/
typedef struct
{
    if_control_msg_fn cc_handler;        /**< EP0 handler. */
    juint8_t cci_subclass;               /**< Control-if CDC subclass. */
    juint8_t cci_protocol;               /**< Control-if CDC protocol. */
    jbool_t cc_ntf_supported;            /**< Control-if notification pipe 
                                              support. */
    jbool_t dc_supported;                /**< Data-if support. */
    jbool_t dc_empty_alt_0;              /**< Provides an empty data-if alternate 
                                              setting. */
    cdc_get_func_headers_fn get_func_headers;  /**< Functional header filler 
                                                    function. */
    jbool_t is_single_iface;             /**< Indicates a single interface. */
    juint8_t interface_number;           /**< Interface number if the application
                                              is to set it by itself. */
} cdc_desc_init_t;

jresult_t cdc_alloc_descriptors(cdc_ctx_t *ctx, cdc_desc_init_t *init_info);

void cdc_free_descriptors(cdc_ctx_t *ctx);

request_t* cdc_create_notification(cdc_ctx_t *ctx, juint8_t bNotification,
    juint16_t wValue, juint16_t wIndex, juint16_t wLength, juint8_t *Data);

request_t* cdc_create_ep0_read(cdc_ctx_t *ctx, juint32_t size);

request_t* cdc_create_ep0_write(cdc_ctx_t *ctx, void *buffer, juint32_t size, 
    juint32_t max_size);

jresult_t cdc_send_status(cdc_ctx_t *ctx);

jresult_t cdc_handle_encapsulated_command(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req);

jresult_t cdc_handle_encapsulated_response(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req);

jresult_t cdc_enable(cdc_ctx_t *ctx);
void cdc_disable(cdc_ctx_t *ctx);
    
jresult_t cdc_init(cdc_ctx_t *ctx);

jresult_t cdc_uninit(context_t context);

/** @} */ /* end_group hsusb_jungo_api */

#endif /* _CDC_FD_H_ */
