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


/**@file fd_storage.h

Holds definitions required by the Mass Storage Function Driver (MS FD) layer.

The definitions in this file are mostly internal to the MS FD layer, with some 
minor exceptions.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/fd_storage.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/24/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _FD_STORAGE_H_
#define _FD_STORAGE_H_

#include <jusb_common.h>
#include <jusb_core_fd.h>


/** @ingroup hsusb_jungo_api 
@{ */

/** Size of the SCSI buffer.
*/
#define SCSI_BUFFER_SIZE (512 * 64) 

/** Size of the SCSI command.
*/
#define MAX_COMMAND_SIZE        16

/** Maximum size of the command's data stage (not read/write).
*/
#define SCSI_MAX_REQUEST_SIZE   200

/** Number of transfer buffers.
*/
#define SCSI_TRANSFER_BUFFERS   2
/*HSU addition*/ 
/** Number of lookahead buffers. We currently support only two.
*/
#define SCSI_LOOKAHEAD_BUFFERS   2
/* End of HSU addition */
/** Mass storage command status code.
*/
#define USB_STATUS_PASS         0

/** Mass storage command status code.
*/
#define USB_STATUS_FAIL         1

/** Mass storage command status code.
*/
#define USB_STATUS_PHASE_ERROR  2

/** Mass storage function driver states.
*/
typedef enum {
    STATE_DISABLED = 0,  /**< &nbsp; */
    STATE_ENABLED  = 1   /**< &nbsp; */
} mass_states_t;

/** Main mass storage function driver structure.
*/
typedef struct {
    void *core_ctx;              /**< Core context. */

    pipe_desc_t *in_pipe;        /**< Pointer to the IN pipe. */
    pipe_desc_t *out_pipe;       /**< Pointer to the OUT pipe. */
    
    mass_states_t state;         /**< Current state. */

    mass_states_t prev_state;    /**< State before suspend. */

    juint8_t     total_luns;     /**< Total number of logical unit numbers, 
                                      as reported by scsi_init. */
    
    juint32_t scsi_buffer_size;  /**< Double buffer for data I/O. */

    buffer_t io_buffer[SCSI_TRANSFER_BUFFERS]; /**< Buffers used by the
											        function driver. */

    request_t *io_request[SCSI_TRANSFER_BUFFERS]; /**< Pointer to the data 
                                                       transfer request
													   structure.*/
/*HSU addition*/ 
    buffer_t la_io_buffer[SCSI_LOOKAHEAD_BUFFERS]; /**< Buffers used for read lookahead. */

    request_t *la_io_request[SCSI_LOOKAHEAD_BUFFERS]; /**< Pointer to the lookahead data 
                                                       transfer request
													   structure.*/
/* End of HSU addition */

    request_t *cmd_request;      /**< Pointer to the SCSI request transfer. */

    request_t *ep0_request;      /**< Pointer for the EP0 control transfer
								      request. */

    jbool_t current_cmd_status;  /**< TRUE if a command is currently being 
                                      processed.\ A CBW is accepted only when 
                                      there is no other command. */

    void *scsi_device;           /**< Pointer to the SCSI device. */

    fd_desc_t *fd_desc;          /**< Pointer to the configuration descriptor
								      for this function driver. */
} fd_storage_t;


/*=================
 * USB Functions
 * ===============*/

/*===== Function name:  send_csw */
/**
  Sends a CSW request to the host.\ All SCSI commands must finish processing by 
  calling this function.

  @param *fd     [IN] -- Pointer to the MS function driver context. 
  @param tag     [IN] -- Tag received in the matching CBW.
  @param residue [IN] -- Amount of data (in bytes) that was not transferred in 
                         the data stage.
  @param status  [IN] -- Command status.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void send_csw(fd_storage_t *fd, juint32_t tag, juint32_t residue, juint8_t status);


/*===== Function name:  post_write */
/**
  Sends data to the host.

  @param *fd      [IN] -- Pointer to the function driver context. 
  @param *req     [IN] -- Pointer to the request on which to perform the write.
  @param size     [IN] -- Transfer size, in bytes.
  @param callback [IN] -- Completion callback.
  @param context  [IN] -- Context for the completion callback.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise an error code.

  @scope
  Global.
*/
jresult_t post_write(fd_storage_t *fd, request_t *req, juint32_t size, 
    callback_t callback, context_t context);


/*===== Function name:  post_read */
/**
  Sends a read request to the host.

  @param *fd      [IN] -- Pointer to the function driver context.
  @param *req     [IN] -- Pointer to the request on which to perform the write.
  @param size     [IN] -- Transfer size, in bytes.
  @param callback [IN] -- Completion callback.
  @param context  [IN] -- Context for the completion callback.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise an error code.

  @scope
  Global.
*/
jresult_t post_read(fd_storage_t *fd, request_t *req, juint32_t size,
    callback_t callback, context_t context);


/*===== Function name:  stop_io */
/**
  Cancels the current USB I/O request.

  @param *fd [IN] -- Pointer to the function driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void stop_io(fd_storage_t *fd);


/*===== Function name:  fatal_processing_error */
/**
  Handles a fatal data processing error, including data pipes stalling.

  @param *fd [IN] -- Pointer to the function driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void fatal_processing_error(fd_storage_t *fd);


/*===== Function name:  abort_out_pipe */
/**
  Cancels pending read requests.
  
  @param *fd [IN] -- Pointer to the function driver context.
 *
  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void abort_out_pipe(fd_storage_t *fd);


/*===== Function name:  stall_in_pipe */
/**
  Stalls the IN data pipe.
  
  @param *fd [IN] -- Pointer to the function driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void stall_in_pipe(fd_storage_t *fd);


/*===== Function name:  stall_out_pipe */
/**
  Stalls the OUT data pipe.
  
  @param *fd [IN] -- Pointer to the function driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void stall_out_pipe(fd_storage_t *fd);


/*===== Function name:  clear_cmd_in_progress */
/**
  Returns the command's current status.\ This function is used to check whether 
  a command is still in progress.

  @param *fd [IN] -- Pointer to the function driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Local.
*/
static __INLINE__ jbool_t command_in_progress(fd_storage_t *fd)
{
    return fd->current_cmd_status;
}


/*==================
 * SCSI Functions
 * ================*/

/*===== Function name:  do_scsi_command */
/**
  Handles a SCSI command received from the host.

  @param *scsi_dev [IN] -- Pointer to the SCSI device context.
  @param *command  [IN] -- SCSI command buffer.
  @param size      [IN] -- Command size, in bytes.
  @param data_size [IN] -- Size of data (in bytes) for the command's data stage.
  @param tag       [IN] -- Tag for matching the CSW to the original CBW tag.
  @param lun       [IN] -- Logical unit number. 
  @param direction [IN] -- Transfer direction: 0 = OUT, 1 = IN.
  @param *buffer   [IN] -- Pointer to the buffer to be used for USB data transfers.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void do_scsi_command(void *scsi_dev, juint8_t *command, juint32_t size,
    juint32_t data_size, juint32_t tag, juint8_t lun,
    jbool_t direction, juint8_t *buffer);


/*===== Function name:  scsi_init */
/**
  Initializes the SCSI layer.

  @param *fd         [IN] -- Pointer to the function driver context.
  @param *luns      [OUT] -- Pointer to the number of logical unit numbers.
  @param **scsi_dev [OUT] -- Pointer to the SCSI device context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t scsi_init(fd_storage_t *fd, juint8_t *luns, void **scsi_dev);


/*===== Function name:  scsi_uninit */
/**
  De-initializes the SCSI layer.
 
  @param *scsi_dev [IN] -- Pointer to the SCSI device context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void scsi_uninit(void *scsi_dev);


/*===== Function name:  scsi_enable */
/**
  Enables all logical unit numbers on the specified SCSI device.

  @param *scsi_dev  [IN] -- Pointer to the SCSI device context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t scsi_enable(void *scsi_dev);


/*===== Function name:  scsi_disable */
/**
  Disables all logical unit numbers on the specified SCSI device.

  @param *scsi_dev  [IN] -- Pointer to the SCSI device context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void scsi_disable(void *scsi_dev);

/** @} */ /* end_group hsusb_jungo_api */

#endif
