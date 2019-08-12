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


/**@file uw_device_cdc.h

Contains the declarations of the Communication Device Class (CDC) device.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_device_cdc.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
**************************************************************************/ 

#ifndef _UW_DEVICE_CDC_H_
#define _UW_DEVICE_CDC_H_

#include <jtypes.h>


/** @ingroup hsusb_jungo_api 
@{ */

typedef void * cdc_handle_t;
typedef void * cdc_appctx_t;

/** CDC initialization flags. */
enum cdc_init_flags_t
{
    CDC_INIT_BUFFER_DMABLE         = 0x00000001L,  /**< &nbsp; */
    CDC_INIT_SINGLE_INTERFACE      = 0x00000002L,  /**< &nbsp; */
    CDC_INIT_VENDOR_SPECIFIC_CLASS = 0x00000004L,  /**< &nbsp; */
    CDC_INIT_NO_CLASS_DESC         = 0x00000008L,  /**< &nbsp; */
    /* HSU addition: API to disable alt. settings */
    CDC_INIT_NO_ALT_SETTINGS       = 0x00000010L,  /**< &nbsp; */
    /* HSU addition: API to disable AID when multi-interface */
    CDC_INIT_NO_IAD                = 0x00000020L,  /**< &nbsp; */
    /* HSU addition: API to identify NMEA instance of OBEX */
    CDC_INIT_BI_DIR_NMEA           = 0x00000040L   /**< &nbsp; */
};

/* USB connection API */
typedef jresult_t (*cdc_enabled_cb)(cdc_appctx_t ctx);
typedef jresult_t (*cdc_disabled_cb)(cdc_appctx_t ctx);
typedef jresult_t (*cdc_uninit_cb)(cdc_appctx_t ctx);

/* Temp fix for B46104: added resume */
typedef jresult_t (*cdc_resume_cb)(cdc_appctx_t ctx);
/* end temp fix */

/*========================
 *     Data plane API
 *=======================*/
jresult_t cdc_write_multiple(cdc_handle_t handle, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout);
jresult_t cdc_write(cdc_handle_t handle, void *buffer, juint32_t size);
jresult_t cdc_read(cdc_handle_t handle, void *buffer, juint32_t size);
jresult_t cdc_read_multiple(cdc_handle_t handle, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout);
jresult_t cdc_abort_read(cdc_handle_t handle);
jresult_t cdc_abort_write(cdc_handle_t handle);
jresult_t cdc_handle_tx_completion(cdc_handle_t handle);
/* HSU addition: SPS */
jresult_t cdc_sps_read_write_infinite(cdc_handle_t handle, juint8_t read_pipe_id, juint8_t write_pipe_id);

/** CDC IO status. */
typedef enum {
    CDC_TRANSFER_ERROR = -1,    /**< &nbsp; */
    CDC_TRANSFER_OK = 0,        /**< &nbsp; */
    CDC_TRANSFER_CANCELLED = 1  /**< &nbsp; */
} cdc_iostatus_t;

typedef void (*cdc_read_complete_cb)(cdc_iostatus_t status, void *buffer,
    juint32_t size, jbool_t transfer_complete, cdc_appctx_t ctx);

typedef void (*cdc_read_multi_complete_cb)(cdc_iostatus_t *status, void **buffers, 
    juint32_t *bytes_transferred, juint32_t req_num, cdc_appctx_t ctx);

typedef void (*cdc_write_complete_cb)(cdc_iostatus_t status, void *buffer,
    cdc_appctx_t ctx);

typedef void (*cdc_write_multi_complete_cb)(cdc_iostatus_t *status, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, cdc_appctx_t ctx);

/*========================
 *   Control class API
 *=======================*/

/** I/O completion callback for the device to host transfers and notifications. */
typedef void (*cdc_io_complete_cb)(cdc_iostatus_t status, cdc_appctx_t ctx);

/** I/O completion callback for the cdc_send_encap_response() function. */
typedef void (*cdc_response_complete_cb)(cdc_iostatus_t status,
    juint32_t bytes_sent, cdc_appctx_t ctx);

/** Host-to-device requests. */
typedef jresult_t (*cdc_encap_command_cb)(void *command, juint16_t size,
    cdc_appctx_t ctx);

/** Device-to-host notifications.\ No queueing is supported - a second call will 
return JEBUSY. */
jresult_t cdc_send_encap_response(cdc_handle_t handle, void *response,
    juint32_t size);


/** CDC callbacks structure.
*/
typedef struct
{
    /* General Handlers */
    cdc_enabled_cb              enabled_h;  /**< Callback for when the CDC is 
                                                 enabled. */
    cdc_disabled_cb             disabled_h; /**< Callback for when the CDC is 
                                                 disabled. */
    cdc_uninit_cb               uninit_h;   /**< Callback for when the CDC is 
                                                 deinitialized. */
    
    cdc_encap_command_cb        encap_command_h;
	                                      /**< Host-to-device request handler. */

    cdc_response_complete_cb    encap_response_complete_h;
	                                      /**< Encapsulated Completion Handler. */

    /* Transfer Completion Handlers */
    cdc_read_complete_cb        read_complete_h;
         /**< Callback for when the CDC read is complete. */

    cdc_read_multi_complete_cb  read_multi_complete_h;
         /**< Callback for when the CDC multi-buffer read is complete. */
    cdc_write_complete_cb       write_complete_h;
         /**< Callback for when the CDC write is complete. */
    cdc_write_multi_complete_cb write_multi_complete_h;
         /**< Callback for when the CDC multi-buffer write is complete. */

/* Temp fix for B46104: added resume */
    cdc_resume_cb               resumed_h;
         /**< Callback for when the CDC is resumed. */
/* end temp fix */

} cdc_callbacks_t;

/** CDC initalization information.
*/
typedef struct
{
    juint32_t flags;             /**< CDC initialization flags. */
    juint16_t max_segment_size;  /**< Maximum transfer unit over a data plane. */
    jbool_t zlp_terminator;      /**< Indicates if a zero length packet is to be 
                                      sent to mark the end of the transfer. */
    juint8_t interface_number;
    char *comm_string;           /**< Communication interface string descriptor. */
    char *data_string;           /**< Data interface string descriptor. */

    juint8_t cci_subclass;       /**< CDC subclass: control-if. */
    juint8_t cci_protocol;       /**< CDC protocol: control-if. */
} cdc_init_info_t;

/** @} */ /* end_group hsusb_jungo_api */

#endif
