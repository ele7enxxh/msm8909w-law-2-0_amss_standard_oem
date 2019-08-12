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


/**@file jusb_common.h

Holds the common definitions needed for the USB device stack.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jusb_common.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _JUSB_COMMON_H_
#define _JUSB_COMMON_H_

#include <jos.h>
#include <jutil.h>
#include "jslave_init.h"


/** @ingroup hsusb_jungo_api 
@{ */

/** Types of supported pipes (per the USB Specification, Chapter 9).
*/
typedef enum  {
    PIPE_CONTROL = 0,   /**< &nbsp; */
    PIPE_ISOC    = 1,   /**< &nbsp; */
    PIPE_BULK    = 2,   /**< &nbsp; */
    PIPE_INTR    = 3    /**< &nbsp; */
} pipe_type_t;

/** Used to return the status of a pipe. */
typedef enum {
    PIPE_UNKNOWN = 0,   /**< &nbsp; */
    PIPE_ENABLED,       /* I/O can be performed on the pipe */ /**< &nbsp; */
    PIPE_DISABLED,      /**< &nbsp; */
    PIPE_STALLLED,      /**< &nbsp; */
    PIPE_ABORTING       /**< &nbsp; */
} pipe_status_t;


#define DIRECTION_IN    0
#define DIRECTION_OUT   1

/** Supported speeds. */
typedef enum {
    SPEED_UNKNOWN       = 0,   /**< &nbsp; */
    SPEED_LOW           = 1,   /**< &nbsp; */
    SPEED_FULL          = 2,   /**< &nbsp; */
    SPEED_HIGH          = 3,   /**< &nbsp; */
/* HSU addition */
    SPEED_SUPER         = 4    /**< &nbsp; */
/* End of HSU addition */

} device_speed_t;

/** An array that holds the maximum packet sizes for each type (Bulk, Interrupt, 
    Isochronous, Control), and each speed (Low, Full, High, Super).
*/
extern const juint16_t MAX_PACKET_SIZES[4][4]; 

/** Standard callback context. */
typedef void *context_t;

/** Standard buffer (with a physical address for the DMA).
*/
typedef struct {
    void        *vaddr;         /**< Virtual address of the buffer. */
    void        *dma_addr;      /**< Address used for the DMA (physical). */
    juint32_t    buffer_size;   /**< Size of the buffer. */
    void        *cookie;        /**< Parameter that can be used to identify 
                                     DMA allocation. */
} buffer_t;

/** Possible request statuses. */
typedef enum {
    REQUEST_READY = 0,       /**< &nbsp; */
    REQUEST_INPROGRESS = 1,  /**< &nbsp; */
    REQUEST_CANCELLED = 2,   /**< &nbsp; */
    REQUEST_PENDING = 3,     /**< &nbsp; */
    REQUEST_ERROR = 4,       /**< &nbsp; */
    REQUEST_COMPLETED = 5    /**< &nbsp; */
} request_status_t;

/** Forward declaration for request_t.
*/
struct request_t;

typedef void (*callback_t)(struct request_t *);

/** Holds the request data for a data transfer (FD <--> DCD).
*/
typedef struct request_t  {
    juint_t      zero      : 1;    /**< Forces a zero length packet if the last 
								        frame equals the maximum packet size. */
    juint_t      direction : 1;    /**< For use by callbacks; set by
								        Core 0/1 -- In/Out. */
    juint_t      is_buffer_allocated : 1; /**< Indicates whether request_free 
										       is to free the buffer. */
    juint_t      ep0_zlp_reply : 1; /**< Set to 1 when a zero length data stage 
									     is required on EP0. */
    juint_t      padding : 4;       /**< Reserved bits for future use and 
									     alignment. */

    juint8_t ep0_req_tag;           /**< Verifies that the reply matches the 
									     right control request. */

    buffer_t    buffer;             /**< Buffer on which to perform the 
									     operation. */
    
    callback_t complete;            /**< Completion callback. */
    context_t   context;            /**< For use by the completion callback. */

    request_status_t status;        /**< Request status. */

    juint32_t    transfer_size;     /**< Number of bytes to transfer. */
    juint32_t    bytes_transferred; /**< Number of bytes transferred. */

    void        *caller_private;    /**< For use by the request initiator. */

    STAILQ_ENTRY(request_t) next_request;    /**< Queue of pending transfers. */
    LIST_ENTRY(request_t) next_request_in_list;
	                                /**< List of requests when the multiple 
									     transfers API is used. */
    juint32_t    timeout;           /**< Timeout for the request. */
    juint32_t    completed_req;     /**< Number of completed requests. */
    jbool_t      single_request;    /**< Request for single or multiple 
									     transfers */
    void        *dcd_private;       /**< For use by the DCD. */

    juint32_t    isoc_max_payload;  /**< Maximum payload for isochronous 
									     transfer. */

    juint16_t    test_mode;         /**< Electrical Test Mode transfer. */

/* HSU addition */
    juint8_t     req_id;            /**< Request Identification. */
/* End of HSU addition */

/* HSU addition: SPS. */
    /* to be set by FD when SPS mode is desired. */
    jbool_t  sps_mode;    /**< Uses SPS or legacy mode.\ In SPS mode, the buffer 
						       field should be ignored. */
    jbool_t  tbe;         /**< Total bytes enabled.\ TRUE for Single Transfer, 
						       FALSE for Infinite.\ In Infinite mode, the 
							   transfer_size and bytes_transferred are 
							   irrelevant. */
    juint8_t sps_pipe_id; /**< SPS pipe allocated for this request (0 to 30).\ 
						       0x1f means no pipe is allocated. */
/* End of HSU addition */
} request_t;

/** Pipe descriptor structure.
*/
typedef struct pipe_desc_t {
    juint16_t max_pkt_size_super; /**< Pipe's maximum packet size for a 
                                       super-speed connection.\ Normal 
                                       representation that is not in 2^(size). */
    juint16_t max_pkt_size_high;  /**< Pipe's maximum packet size for a 
								       high-speed connection.\ Normal 
									   representation that is not in 2^(size). */
    juint16_t max_pkt_size_full;  /**< Pipe's maximum packet size for a 
								       full-speed connection.\ Normal 
									   representation that is not in 2^(size). */
    pipe_type_t type;             /**< Request type supported by the pipe: 
								       control, isochronous, bulk, or interrupt. */
    unsigned direction    : 1;    /**< Pipe's data transfer direction: IN/OUT. */
    unsigned sync_type    : 2;    /**< For ISOC pipes. */
    unsigned usage_type   : 2;    /**< For ISOC pipes. */
    juint8_t address;             /**< Pipe's address. */
    unsigned transaction_per_microframe :2;
	                              /**< In high speed 1 to 3 transactions are 
								       allowed per microframe, zero indexed 
									   (i.e., 0 = 1 packet per microframe). */

	  /** Pipe rates union. */
	  union {
          juint8_t nak_rate;       /**< For bulk, the maximum NAK (not-ACK) rate 
								        per microframe. */
          juint8_t poll_interval;  /**< For an ISOC/interrupt, the polling 
								        frequency. */
      } rate_u; 

    struct {
      juint8_t  max_burst;          /**< For SuperSpeed pipes, Max burst size */
      juint8_t  max_streams;        /**< For SuperSpeed pipes, Max Streams = 2^max_streams */
      juint8_t  mult;               /**< For SuperSpeed isoc pipes, Max packets = max_burst x (mult+1) */
    } ss_attributes;

    juint32_t extra_descriptor_size; /**< The size (in bytes) of the
                                          extra_descriptor data. */
    juint8_t *extra_descriptor;    /**< Extra, class-specific, descriptor data 
								        for a GET_CONFIGURATION descriptor 
										request. */
    juint8_t poll_interval_high;   /**< For isochronous and interrupt pipes: 
									    the pipe's polling frequency. */
    
    void *fd_handle;               /**< Pipe handle for use by the function 
									    driver when sending requests to the USB 
                                        core.\ The pipe handle is set by the 
										USB core as part of the function 
										driver's registration. */
    void *dcd_handle;              /**< Pipe handle for use by the DCD. */

    juint16_t max_pkt_size;        /**< Packet size for the current speed. */
    pipe_status_t status;          /**< Pipe status. */
    
    STAILQ_HEAD(pipe_req_queue, request_t) req_queue; /**< Requests a queue. */
} pipe_desc_t; 

void core_uninit(void);

juint8_t core_get_controller_idx(juint8_t controller_number);
jresult_t core_enable_by_idx(juint8_t core_index);
void core_disable_by_idx(juint8_t core_index);

juint8_t core_get_curr_configuration(juint8_t core_number);

/* HSU Addition */
void core_clear_reconfig_flag(juint8_t core_index);
jbool_t jotg_is_enabled(void);
/* HSU Addition End */

/** @} */ /* end_group hsusb_jungo_api */

#endif
