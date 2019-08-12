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


/**@file jusb_core_fd.h

Contains the functions that are exported by the core for the function 
driver.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jusb_core_fd.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _JUSB_CORE_FD_H
#define _JUSB_CORE_FD_H

#include "jusb_common.h"
#include "jusb_chp9.h"


/** @ingroup hsusb_jungo_api 
@{ */

/*HSU addition*/
#include "hsu_vendor_specific.h"
/*End of HSU addition*/

#define MAX_PIPE_COUNT  16

/** Alternate interface descriptor.
*/
typedef struct {
    juint8_t      pipe_count; /**< Number of pipes in this alternate 
                                   interface. */

    pipe_desc_t  *pipes;      /**< Array of pipes for this alternate 
   									interface.	*/

    juint8_t     *extra_descriptor;
         /**< Pass extra, class-specific, and descriptor data in the 
              Configuration descriptor. */
    juint16_t    extra_descriptor_size;
         /**< Vendor/class specific descriptor size. */
} alt_interface_desc_t;

typedef jresult_t (*if_control_msg_fn)(void *context, void *buffer);

/** Interface descriptor structure.
*/
typedef struct {
    if_control_msg_fn control_msg;
         /**< Endpoint handler for this interface. */

    alt_interface_desc_t *alt_ifs;
         /**< Pointer to the alternate interface array */
    juint8_t     alt_if_num;    /**< Number of alternate interfaces. */
    juint8_t     alt_if_curr;   /**< Current alternate interface. */

    juint8_t     *extra_descriptor;
         /**< Pass extra, class-specific, and descriptor data in the 
              Config descriptor. */

    juint32_t    extra_descriptor_size;
         /**< Vendor/class specific descriptor size. */

#define UWD_UNDEFINED_INTERFACE 0xFF /**< Initial interface number value 
                                          (0xFF). */
    juint8_t     number;       /**< Actual interface number (set by the 
                                    core). */

    juint8_t     if_class;     /**< Function driver class-specific 
                                    identification. */
    juint8_t     if_subclass;  /**< Interface's subclass. */
    juint8_t     if_protocol;  /**< Interface's protocol. */

    juint8_t     if_string;    /**< Optional string descriptor index to 
                                    describe this interface. */
} interface_desc_t;

/** Configuration descriptor.
*/
typedef struct {
    device_speed_t speed;          /**< Supported speed. */
    device_speed_t current_speed;  /**< Current speed (filled in by the 
                                        core). */
    juint8_t     interface_count;  /**< Interfaces counter. */
    interface_desc_t *interfaces;  /**< Pointer to the interface 
                                        descriptor. */
    struct usb_interface_assoc_descriptor *iad;  /**< Pointer to the 
                                        interface association descriptor. */
    jbool_t fd_remote_wakeup_capable;  /**< FD supported remote-wakeup. */
    jbool_t fd_remote_wakeup_enabled;  /**< FD remote-wakeup feature is enabled. */

} fd_desc_t;

/** Functions exported by the function driver for the core.
 */
typedef struct {
    fd_desc_t *(*get_pipe_config)(context_t context);
         /**< Returns the pipe configuration the function driver requires. */

    jresult_t (*enable)(context_t context);
         /**< Enables the function driver. */
    void     (*disable)(context_t context);
         /**< Disables the function driver. */

    jresult_t (*suspend)(context_t context);
         /**< Notifies the function driver on suspend. */

    jresult_t (*resume)(context_t context);
         /**< Notifies the function driver on resume. */

    jresult_t (*uninit)(context_t context);
         /**< Uninitializes the function driver. */
} fd_ops_t;


/** Posts a data send request. This function is exported by the core for 
    the function driver.
*/
jresult_t core_send_data(void *ctx, pipe_desc_t *pipe , request_t *request);

/** Posts a data read request. This function is exported by the core for 
    the function driver.
*/
jresult_t core_read_data(void *ctx, pipe_desc_t *pipe, request_t *request);

/** Sets the STALL state on a pipe (0 = Off, 1 = On). This function is exported
    by the core for the function driver.
*/
void core_stall_pipe(void *ctx, pipe_desc_t *pipe,
    juint8_t flag);

/** Cancels all pending requests on a pipe. This function is exported by
    the core for the function driver.
*/
void core_abort_pipe(void *ctx, pipe_desc_t *pipe);

/** Gets the number of the next available interface. This function is exported
    by the core for the function driver.
*/
juint8_t core_get_available_iface(void *ctx);

/** Registers the function driver with the core. This function is exported by
    the core for the function driver.
*/
jresult_t core_register_fd(void *ctx, fd_ops_t *ops, context_t context);

/** Allocates a request object. This function is exported by
    the core for the function driver.
*/
request_t *core_request_alloc(juint32_t buffer_size, void *address);

/* HSU Temp Fix for Jungo CR B49754: Start */

/** Allocates a new core request. This function is exported by
    the core for the function driver.
*/
request_t *core_request_alloc_ex(juint32_t buffer_size, void *address, juint16_t align);
/* HSU Temp Fix for Jungo CR B49754: End */

/** Frees a request object. This function is exported by
    the core for the function driver.
*/
void core_request_free(request_t *request);

/** Adds a Microsoft&tm; &nbsp; OS string descriptor to be included in the
    response to a GET_MS_DESCRIPTOR request. This function is exported by
	the core for the function driver.
*/
void core_add_msos_string_descriptor(void *ctx, juint8_t vendor_code);

/* HSU addition */

/** Removes a Microsoft OS string descriptor. This function is exported by the
    core for the function driver.
*/
void core_release_msos_string_descriptor(void *ctx);
/* End of HSU addition */

/** Adds a string descriptor to be included in the response to a GET_DESCRIPTOR
    request. This function is exported by the core for the function driver.
*/
juint8_t core_add_string_descriptor(void *ctx, char *string,
                                    /* HSU addition */
                                    hsu_str_idx_desc_type str_desc); 
                                    /* End of HSU addition */

/** Removes a string descriptor. This function is exported by the core for the
    function driver.
*/
void core_release_string_descriptor(void *ctx, juint8_t index);

/** Changes a string descriptor. This function is exported by the core for the
    function driver.
*/
void core_change_string_descriptor(void *ctx, juint8_t id, char *string);

/** Handles the completion for IN transfers. This function is exported by the
    core for the function driver.
*/
void core_handle_tx_completion(void *ctx, pipe_desc_t *pipe);

juint8_t core_get_controller_index(void *ctx);
juint8_t core_get_controller_number(void *ctx);

/* Request Access macros  */
#define REQUEST_GET_BUFFER(request) (request->buffer)
#define REQUEST_SET_BUFFER(request, buf) (request->buffer = buf)
#define REQUEST_SET_CALLBACK(request, callback, context) \
        (request->complete = callback ; request->context = context)
#define REQUEST_GET_CONTEXT(request) (request->context)

/* Standard pipe descriptors */
#define DEFAULT_HIGHSPEED_BULK_IN_PIPE
#define DEFAULT_FULLSPEED_INTR_PIPE

/* HSU Addition Begin */
void *core_get_core_context(uint8 core_idx);
void core_set_fd_ops(void *core_ctx, fd_ops_t *fd_ops);
pipe_desc_t *core_get_pipe_desc(void *core_ctx, juint8_t direction, juint8_t ep_num);
boolean core_is_configured(void *core_ctx);
/* HSU Addition End */

/** @} */ /* end_group hsusb_jungo_api */

#endif
