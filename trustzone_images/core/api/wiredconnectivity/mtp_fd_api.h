/*=============================================================================

                      High Speed USB MTP Function Driver Header File

=============================================================================*/
  
/**
@file mtp_fd_api.h

High-speed USB MTP function driver header file.

This file is for the function driver, mtp_fd.c, and it exposes APIs 
available to the client.
*/

/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/mtp_fd_api.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
when      who   what, where, why
--------  ---   ----------------------------------------------------------
06/23/10   sw   (Tech Pubs) Edited/added Doxygen comments and markup.
06/29/07   bt    Integrate MTP (IUSBDriver) feature into Main Line
    
=============================================================================*/


#ifndef _MTP_FD_API_H_
#define _MTP_FD_API_H_


/** @ingroup hsusb_qc_api 
    @{
*/

typedef void * mtpfd_handle_t;

/** MTP IOCTL commands.
*/
typedef enum
{
    MTPFD_ABORT_READ,             /**< -- Cancels an uncompleted read. */
    MTPFD_ABORT_WRITE,            /**< -- Cancels an uncompleted write. */
    MTPFD_GET_CONNECT_STATUS,     /**< -- Retrieves the state of the DTR
                                          signal. */
    MTPFD_SET_CONNECT_STATUS_CB,  /**< -- Registers the DTR-Toggle callback
                                          function. */
    MTPFD_CLEAR_CONNECT_STATUS_CB /**< -- Deregisters the DTR-Toggle callback 
								          function. */
} mtpfd_ioctl_command_type;


/** Types of supported pipes (same as in jusb_chp9.h).
*/
typedef enum
{
  MTPFD_PIPE_CONTROL = 0,  /**< -- Control pipe. */
  MTPFD_PIPE_ISOC    = 1,  /**< -- ISOC pipe. */
  MTPFD_PIPE_BULK    = 2,  /**< -- Bulk pipe. */
  MTPFD_PIPE_INTR    = 3   /**< -- Interrupt pipe. */
} mtpfd_pipe_type_t;


/* USB connection API *************/
typedef void (*mtpfd_mtp_app_cb)(boolean);
typedef void (*mtpfd_enabled_cb)(void);
typedef void (*mtpfd_disabled_cb)(void);
typedef int (*mtpfd_uninit_cb)(mtpfd_handle_t );

typedef void (*mtpfd_read_complete_cb)(mtpfd_pipe_type_t);
typedef void (*mtpfd_write_complete_cb)(void);
typedef void (*mtpfd_notfication_cb)(void);

typedef void (*mtpfd_read_error_cb)(void);
typedef void (*mtpfd_write_error_cb)(void);
typedef void (*mtpfd_notification_error_cb)(void);

typedef void (*mtpfd_suspend_cb)(void);
typedef void (*mtpfd_resume_cb)(void);


typedef void (*mtpfd_setupmsg_cb)(void *);
typedef void (*mtpfd_cnxn_status_cb)(void);

typedef void (*mtpfd_flush_in_pipe_cb)(void);

/*==========================================================
 *   Control class API
 *==========================================================*/

/** Initialized from the OEM driver layer.
*/
typedef struct
{
   mtpfd_mtp_app_cb              mtp_app_launch_h;
     /**< Callback function to start/close the Teleca MTPH background 
		  application. */

   /* General Handlers */
   mtpfd_enabled_cb             enabled_h;
     /**< Callback to be called when MTP is enabled. */
   mtpfd_disabled_cb            disabled_h;
     /**< Callback to be called when MTP is disabled. */
   mtpfd_uninit_cb              uninit_h;
     /**< Callback to be called when MTP is uninitialized. */
   mtpfd_suspend_cb             suspend_h;
     /**< Suspend notification. */
   mtpfd_resume_cb              resume_h;
     /**< Resume notification. */
   mtpfd_cnxn_status_cb         cnxn_state_chng_h;
     /**< Connection status notification. */

   /* Transfer Completion Handlers */
   mtpfd_read_complete_cb       rx_complete_h;
     /**< Rx callback transfer from OUT is complete. */
   mtpfd_write_complete_cb      tx_complete_h;
     /**< Tx callback transfer on IN is complete. */
   mtpfd_notfication_cb         txInt_complete_h;
     /**< Tx callback transfer on INTR is complete. */

   /* Transfer error occurence Handlers */
   mtpfd_read_error_cb          rx_error_h;
     /**< Rx error callback. */
   mtpfd_write_error_cb         tx_error_h;
     /**< Tx error callback. */
   mtpfd_notification_error_cb  txInt_error_h;
     /**< Callback to be called when an error occurs. */

   /* Flush transmit pipe (IN endpoints) handlers */
   mtpfd_flush_in_pipe_cb       tx_flush_h ;
     /**< Tx flush callback for Bulk IN. */
   mtpfd_flush_in_pipe_cb       txInt_flush_h ;
     /**< Tx flush callback for INTR. */
   mtpfd_setupmsg_cb            setup_msg_h;
     /**< Process non-standard SETUP request handlers. */

} mtpfd_callbacks_t;

/** MTP debug buffer size.
*/
#define MTP_DEBUG_COUNT 2048

extern char mtp_debug_buf[]; 

extern uint32 mtp_debug_count;

void mtp_debug_append(char x);

/* Defined by the MTPFD client: hsu_al_brewext.c */

/*==============================================================================
FUNCTION    brewext_app_init     
==============================================================================*/
/** 
Initializse the BREW extension application and registers callbacks.

@param *handle [IN] -- Handle to client context.
@param **cbs_h [IN] -- Application callbacks.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success, otherwise an error code.

*/
int brewext_app_init(void* handle, mtpfd_callbacks_t** cbs_h);

/*==========================================================
 *     Data plane API available to MTPFD client
 *==========================================================*/
 
/*==============================================================================
FUNCTION    mtpfd_ioctl     
==============================================================================*/
/** 
Sends an MTP FD IO control message.

@param handle         [IN] -- Handle to client context.
@param type           [IN] -- MTP pipe type.
@param command        [IN] -- IOCTL command.
@param *command_param [IN] -- IOCTL command parameter.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void mtpfd_ioctl(mtpfd_handle_t handle, mtpfd_pipe_type_t type, 
                  mtpfd_ioctl_command_type command,
                  void *command_param);

/*==============================================================================
FUNCTION    mtpfd_post_write     
==============================================================================*/
/** 
Posts a write request to the host.

@param handle  [IN] -- Handle to client context.
@param *buffer [IN] -- Buffer to write to.
@param size    [IN] -- Buffer size.
@param type    [IN] -- MTP pipe type.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success, otherwise an error code
*/
int mtpfd_post_write(mtpfd_handle_t handle, void *buffer,
                       uint16 size, mtpfd_pipe_type_t type);

/*==============================================================================
FUNCTION    mtpfd_post_read     
==============================================================================*/
/** 
Post a read request from the host.

@param handle      [IN] --  Handle to client context.
@param *buffer     [OUT] -- Buffer to read into.
@param buffer_size [IN] --  Buffer size to read into.
@param *size       [OUT] -- Number of bytes read into buffer.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success, otherwise an error code
*/
int mtpfd_post_read(mtpfd_handle_t handle, void *buffer, 
                             uint16 buffer_size,  uint16 *size);

/*==============================================================================
FUNCTION    mtpfd_control_write     
==============================================================================*/
/** 
Posts a control write request to the host.

@param handle  [IN] -- Handle to client context.
@param *buffer [IN] -- Buffer to write to.
@param size    [IN] -- Buffer size.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) on success, otherwise an error code
*/
int mtpfd_control_write(mtpfd_handle_t handle, void *buffer, 
                                                      uint16 size);

/*==============================================================================
FUNCTION    mtpfd_control_read     
==============================================================================*/
/** 
Posts a control read request from the host.

@param handle  [IN]  -- Handle to client context.
@param *buffer [OUT] -- Buffer to read into.
@param size    [IN]  -- Buffer size.

@dependencies
None.

@sideeffects
None.

@return
0 on success, otherwise an error code
*/
int mtpfd_control_read(mtpfd_handle_t handle, void *buffer, 
                                                      uint32 size);

/*==============================================================================
FUNCTION    mtpfd_flush_in_pipe     
==============================================================================*/
/** 
Flushes the IN pipe data.

@param handle  [IN] -- Handle to client context.
@param type    [IN] -- MTP pipe type.
@param FlushCB [IN] -- Callback to be invoked upon completion of the operation.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void mtpfd_flush_in_pipe(mtpfd_handle_t handle, mtpfd_pipe_type_t type, 
                            mtpfd_flush_in_pipe_cb FlushCB);

/** @} */ /* endgroup hsusb_qc_api */

#endif
