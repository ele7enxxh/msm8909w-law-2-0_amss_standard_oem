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


/**@file uw_device_cdc_acm.h

Contains the declarations of the Communication Device Class/Abstract Control Model (CDC/ACM) 
function.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_device_cdc_acm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/23/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com) 
**************************************************************************/ 


#ifndef _UW_DEVICE_CDC_ACM_H_
#define _UW_DEVICE_CDC_ACM_H_

#include <uw_device_cdc.h>
#include <compiler.h>


/** @ingroup hsusb_jungo_api 
@{ */

/*
 * Host to Device - Requests
 */

/** ACM communication states.
*/
typedef enum
{
    ACM_FS_ABSTRACT_STATE       = 0x01,     /**< &nbsp; */
    ACM_FS_COUNTRY_STATE        = 0x02      /**< &nbsp; */ 
} acm_comm_fs_t;

/** ACM abstract state.
*/
enum acm_abstract_state_t
{
    ACM_COMM_DATA_MULTIPLEXING  = 0x0001,   /**< &nbsp; */
    ACM_COMM_IDLE_SETTING       = 0x0002    /**< &nbsp; */
};

/** ACM communication feature setting.
*/
union acm_comm_feature_setting_t
{
    juint16_t abstract_state;   /**< Abstract state. */
    juint16_t country_code;    /**< Country code as defined in ISO3166. */
};

typedef jresult_t (*acm_set_comm_feature_cb)(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx);

typedef jresult_t (*acm_get_comm_feature_cb)(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx);

typedef jresult_t (*acm_clear_comm_feature_cb)(acm_comm_fs_t selector,
    cdc_appctx_t ctx);

/** ACM LC (Low Complexity) bits enumeration.
*/
enum acm_lc_stop_bits_t
{
    ACM_LC_1_STOP_BITS,         /**< &nbsp; */
    ACM_LC_1_5_STOP_BITS,       /**< &nbsp; */
    ACM_LC_2_STOP_BITS          /**< &nbsp; */
};

/** ACM parity.
*/
enum acm_lc_parity_t
{
    ACM_LC_PARITY_NONE,         /**< &nbsp; */
    ACM_LC_PARITY_ODD,          /**< &nbsp; */
    ACM_LC_PARITY_EVEN,         /**< &nbsp; */
    ACM_LC_PARITY_MARK,         /**< &nbsp; */
    ACM_LC_PARITY_SPACE         /**< &nbsp; */
};

/** ACM Data bits enumeration.
*/
enum acm_lc_data_bits_t
{
    ACM_LC_DATA_5_BITS = 5,     /**< &nbsp; */
    ACM_LC_DATA_6_BITS = 6,     /**< &nbsp; */
    ACM_LC_DATA_7_BITS = 7,     /**< &nbsp; */
    ACM_LC_DATA_8_BITS = 8,     /**< &nbsp; */
    ACM_LC_DATA_16_BITS = 16    /**< &nbsp; */
};

/* ACM encoding. */
__PACKED_PRE__
struct _acm_line_coding_t
{
    juint32_t dte_rate;         /*  BAUD rate. */
    juint8_t char_format;       /*  Use acm_lc_stop_bits_t. */
    juint8_t parity_type;       /*  Use acm_lc_parity_t. */
    juint8_t data_bits;         /*  Use acm_lc_data_bits_t. */
} __PACKED_POST__;

/** ACM encoding.
*/
typedef struct _acm_line_coding_t acm_line_coding_t;

typedef jresult_t (*acm_set_line_coding_cb)(acm_line_coding_t *coding,
    cdc_appctx_t ctx);

typedef jresult_t (*acm_get_line_coding_cb)(acm_line_coding_t *coding,
    cdc_appctx_t ctx);

/** ACM control line state.
*/
enum acm_control_line_t
{
    ACM_CONTROL_SIG_DTR = 0x0001,    /**< &nbsp; */
    ACM_CONTROL_SIG_RTS = 0x0002     /**< &nbsp; */
};

typedef jresult_t (*acm_set_control_line_state_cb)(juint16_t ctrl_sig_bitmap,
    cdc_appctx_t ctx);

#define ACM_BREAK_START 0xFFFF;
#define ACM_BREAK_STOP 0x0000;

typedef jresult_t (*acm_send_break_cb)(juint16_t break_duration, cdc_appctx_t ctx);

/*
 * Device to Host - Notifications
 */

/** Serial state notifications.
*/
enum acm_serial_state_t
{
    ACM_SERIAL_DCD              = 0x0001,    /**< &nbsp; */
    ACM_SERIAL_DSR              = 0x0002,    /**< &nbsp; */
    ACM_SERIAL_BREAK            = 0x0004,    /**< &nbsp; */
    ACM_SERIAL_RING_SIG         = 0x0008,    /**< &nbsp; */
    ACM_SERIAL_FRAMING_ERROR    = 0x0010,    /**< &nbsp; */
    ACM_SERIAL_PARITY_ERROR     = 0x0020,    /**< &nbsp; */
    ACM_SERIAL_OVERRUN_ERROR    = 0x0040     /**< &nbsp; */
};

jresult_t acm_notify_serial_state(cdc_handle_t handle, juint16_t serial_state);

jresult_t acm_notify_network_connection(cdc_handle_t handle, jbool_t link_up);

/*
 * Initialization
 */

/** ACM callbacks to the application.
*/
typedef struct
{
    cdc_callbacks_t             basic;     /**< Basic CDC handler. */
        
    acm_set_comm_feature_cb             set_comm_feature_h;
         /**< ACM host-to-device request handler. */

    acm_get_comm_feature_cb             get_comm_feature_h;
         /**< ACM host-to-device request handler. */

    acm_clear_comm_feature_cb           clear_comm_feature_h;
         /**< ACM host-to-device request handler. */

    acm_set_line_coding_cb              set_line_coding_h;
         /**< ACM host-to-device request handler. */

    acm_get_line_coding_cb              get_line_coding_h; 
         /**< ACM host-to-device request handler. */

    acm_set_control_line_state_cb       set_control_line_state_h;
         /**< ACM host-to-device request handler. */

    acm_send_break_cb                   send_break_h;
         /**< ACM host-to-device request handler. */

    cdc_io_complete_cb          get_comm_feature_complete_h;
         /**< ACM request completion handler. */

    cdc_io_complete_cb          get_line_coding_complete_h;
         /**< ACM request completion handler. */

    cdc_io_complete_cb          notify_net_conn_complete_h;
         /**< ACM notification completion handler. */

    cdc_io_complete_cb          notify_serial_state_complete_h;
         /**< ACM notification completion handler. */
} acm_callbacks_t;

/** ACM capabities.
*/
enum acm_enum_capabilities_t
{
    ACM_CAP_COMM_FEATURE        = 0x01,   /**< -- ACM functional header. */
    ACM_CAP_LINE_CODING         = 0x02,   /**< -- ACM functional header. */
    ACM_CAP_SEND_BREAK          = 0x04,   /**< -- ACM functional header. */
    ACM_CAP_NETWORK_CONNECTION  = 0x08,   /**< -- ACM functional header. */

    /* Call Management functional header */
    ACM_CAP_CALL_MGMT           = 0x10,   /**< -- Call management is handled in
                                                  the device. */
    ACM_CAP_CALL_MGMT_OVER_DC   = 0x20    /**< -- Call management (in CC only,
                                                  or is DC capable). */
};

/** ACM initialization information.
*/
typedef struct
{
    cdc_init_info_t basic;     /**< Information relevant for all CDC
							        modules. */
    juint32_t capabilities;    /**< ACM capabilities */
} acm_init_info_t;

jresult_t acm_init(cdc_handle_t handle, acm_callbacks_t *cbs,
    acm_init_info_t *info, cdc_appctx_t *ctx);

/** @} */ /* end_group hsusb_jungo_api */

#endif
