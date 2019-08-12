#ifndef _HSU_AL_BREWEXT_H_
#define _HSU_AL_BREWEXT_H_
/*=============================================================================

                      High Speed USB Brew Extended Device Adaptation Layer

=============================================================================*/
/**
@file hsu_al_brewext.h

Implementation of the BREW&reg; &nbsp; extended adaptation layer.

Externalized functions:
- hsu_al_brewext_handle_mode_change()
- hsu_al_brewext_connect()
- hsu_al_brewext_disconnect()
- hsu_al_brewext_read_request()
- hsu_al_brewext_write_request()
- hsu_al_brewext_send_setup_msg_response()
- hsu_al_brewext_flush_tx()
- hsu_al_brewext_ioctl()
- hsu_al_brewext_stall_in_pipe()
- hsu_al_brewext_stall_out_pipe()
- hsu_al_brewext_register_rdm_callback()
- hsu_al_brewext_register_mtp_app_cb() 
- hsu_al_brewext_register_app_cbs()
- hsu_al_brewext_unregister_app_cbs()
- hsu_al_brewext_unregister_rdm_callback()
- hsu_al_brewext_set_setup_msg_data_buffer()
- hsu_al_brewext_update_hs_main_core_brew_ext_comp()
- hsu_al_brewext_switch_comp_to_brew_ext()
- hsu_al_brewext_switch_comp()
- hsu_al_brewext_get_cur_comp_details()

*/

/*=============================================================================      
Copyright (c) 2006-2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_brewext.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $  
     
when       who     what, where, why
--------  ---     ---------------------------------------------------------
06/24/10   lo     (Tech Pubs) Edited/added Doxygen comments and markup.
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line
   
=============================================================================*/
#include "hsu_conf_sel_types.h"
#include "mtp_fd_api.h"
#include "rdevmap.h"

/** @ingroup hsusb_qc_api
    @{
*/

/** Callback for BREW notifications.
*/
typedef void (*brewext_notify_cb_t)(void *po, uint32 rdm_event);

/** Callback for when the  BREW extension is enabled.
*/
typedef void (*brewext_enable_cb_t)(void);

/* connect status */
#define HSU_CONNECT_SUCCESS     0 /**< Connect status -- success. */
#define HSU_CONNECT_FAIL        1 /**< Connect status -- failure. */
#define HSU_CONNECT_WAIT        2 /**< Connect status -- wait. */

/** BREW extended device dynamic composition switching operation status.
*/
typedef enum
{
    BREW_RDM_DONE,        /**< &nbsp; */
    BREW_RDM_NOT_ALLOWED, /**< &nbsp; */   
    BREW_RDM_DEVMAP_BUSY, /**< &nbsp; */
    BREW_RDM_APP_BUSY     /**< &nbsp; */
} brewext_rdm_change_status_t;


/*=============================================================================
                     Brew Extended Device  API implementations
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_brewext_handle_mode_change
=============================================================================*/
/**
Called to handle an RDM mode change.\ This function is obsolete.

@param status     [IN] -- RDM change status.
@param appClosing [IN] -- Is application closing.

@dependencies
None.

@sideeffects
None.

@return
TRUE for success.
*/
boolean hsu_al_brewext_handle_mode_change(brewext_rdm_change_status_t status, boolean appClosing);

/*=============================================================================
FUNCTION    hsu_al_brewext_connect
=============================================================================*/
/**
Called to connect a BREW extension.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success.
*/
uint32 hsu_al_brewext_connect(void);

/*=============================================================================
FUNCTION    hsu_al_brewext_disconnect
=============================================================================*/
/**
Initiates a spoof disconnect (emulated USB disconnection) and re-enumrates the
device as an MTP BREW extended device.

@dependencies
None.

@sideeffects
None.

@return
One (1) for success.
*/
uint32 hsu_al_brewext_disconnect(void);

/*=============================================================================
              USB functions
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_brewext_read_request
=============================================================================*/
/**
Reads data to the buffer.

@param *rx_buffer     [OUT] -- Buffer to read to.
@param rx_buffer_size [IN] -- Buffer size.
@param *rx_data_size  [OUT] -- Actual data size that was read.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_read_request(void *rx_buffer,
                                       uint16 rx_buffer_size,
                                       uint16 *rx_data_size);

/*=============================================================================
FUNCTION    hsu_al_brewext_write_request
=============================================================================*/
/**
Write data to a pipe.

@param pipe_type [IN] -- Pipe to write to.
@param *response [IN] -- Buffer to write.
@param size      [IN] -- Size of data to write.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_write_request(mtpfd_pipe_type_t pipe_type, 
                                       void* response, 
                                       int size);

/*=============================================================================
FUNCTION    hsu_al_brewext_send_setup_msg_response
=============================================================================*/
/**
Sends setup message.

@param nSize     [IN] -- Size of the setup message.
@param *response [IN] -- Buffer to send.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_send_setup_msg_response(int nSize,
                                            void *response);

/*=============================================================================
FUNCTION    hsu_al_brewext_flush_tx
=============================================================================*/
/**
Flushes the Tx pipe.

@param type    [IN] -- Pipe type.
@param FlushCB [IN] -- Callback to call after completion.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_flush_tx(mtpfd_pipe_type_t type, 
                                 mtpfd_flush_in_pipe_cb FlushCB);

/*=============================================================================
FUNCTION    hsu_al_brewext_ioctl
=============================================================================*/
/**
IOCTL handler.

@param pipe           [IN] -- Pipe to call.
@param command        [IN] -- IOCTL command to execute.
@param *command_param [IN] -- Parameter to use while executing the IOCTL.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_ioctl(mtpfd_pipe_type_t pipe, 
                             mtpfd_ioctl_command_type command,
                             void *command_param);

/*=============================================================================
FUNCTION    hsu_al_brewext_stall_in_pipe
=============================================================================*/
/**
Stalls IN pipe.

@param pipe [IN] -- Pipe to stall.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_stall_in_pipe(mtpfd_pipe_type_t pipe);

/*=============================================================================
FUNCTION    hsu_al_brewext_stall_out_pipe
=============================================================================*/
/**
Stalls OUT pipe

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_stall_out_pipe(void);

/*=============================================================================
FUNCTION    hsu_al_brewext_register_rdm_callback
=============================================================================*/
/**
Registers RDM callback.

@param *pClientData [IN] -- RDM client data.
@param cb           [IN] -- Callback to call on completion.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The RDM callback was registered successfully.\n
FALSE -- The RDM callback was not registered successfully.
*/
boolean hsu_al_brewext_register_rdm_callback(void * pClientData,  
                                             brewext_notify_cb_t cb);

/*=============================================================================
FUNCTION    hsu_al_brewext_register_mtp_app_cb
=============================================================================*/
/**
Registers MTP application callback.

@param mtpAppCBFunc [IN] -- MTP callback function.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The MTP application callback was registered successfully.\n
FALSE -- The MTP application callback was not registered successfully.
*/
boolean hsu_al_brewext_register_mtp_app_cb(mtpfd_mtp_app_cb mtpAppCBFunc);

/*=============================================================================
FUNCTION    hsu_al_brewext_register_app_cbs
=============================================================================*/
/**
Registers application callbacks.

@param EnableCB_ISR   [IN] -- MTP enabled callback.
@param TxCB           [IN] -- MTP write complete callback.
@param RxCB           [IN] -- MTP read complete callback.
@param IntTxCB        [IN] -- MTP Tx complete callback.
@param TxErrCB        [IN] -- MTP Tx error callback.
@param RxErrCB        [IN] -- MTP Rx error callback.
@param IntTxErrCB     [IN] -- MTP interrupt Tx error callback.
@param SetupMsgCB_ISR [IN] -- MTP setup message callback.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success.
*/
int hsu_al_brewext_register_app_cbs (mtpfd_enabled_cb EnableCB_ISR, 
                                                 mtpfd_write_complete_cb TxCB,
                                                 mtpfd_read_complete_cb RxCB,
                                                 mtpfd_notfication_cb IntTxCB,
                                                 mtpfd_write_error_cb TxErrCB,  
                                                 mtpfd_read_error_cb RxErrCB,
                                                 mtpfd_notification_error_cb IntTxErrCB,
                                                 mtpfd_setupmsg_cb SetupMsgCB_ISR);


/*=============================================================================
FUNCTION    hsu_al_brewext_unregister_app_cbs
=============================================================================*/
/**
Unregisters application callbacks.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success.
*/
int hsu_al_brewext_unregister_app_cbs(void);

/*=============================================================================
FUNCTION    hsu_al_brewext_unregister_rdm_callback
=============================================================================*/
/**
Unregisters RDM callback.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success.
*/
int hsu_al_brewext_unregister_rdm_callback(void);

/*=============================================================================
FUNCTION    hsu_al_brewext_set_setup_msg_data_buffer
=============================================================================*/
/**
Sets setup message buffer.

@param *data [IN] -- Buffer for the setup message.
@param size  [IN] -- Size of the buffer.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_brewext_set_setup_msg_data_buffer(void *data, uint32 size);

/*=============================================================================
FUNCTION    hsu_al_brewext_update_hs_main_core_brew_ext_comp
=============================================================================*/
/**
Updates the configuration selector information of the HS-USB main core to use
a BREW extension composition.

@dependencies
None.

@sideeffects
None.

@return
FALSE on error; TRUE otherwise.
*/ 
boolean hsu_al_brewext_update_hs_main_core_brew_ext_comp( void );

/*=============================================================================
FUNCTION    hsu_al_brewext_switch_comp_to_brew_ext
=============================================================================*/
/**
Switches composition to BREW_EXT by calling hsu_conf_sel_ui_switch_comp.
See hsu_conf_sel_ui_switch_comp() for more details.

@param rdm_comp_cb_func_ptr [IN] -- Completion callback

@dependencies
Must be called after stack initialization. 

@sideeffects
None.

@return
TRUE -- Indicates the first step was successful.\n
FALSE -- Indicates failure.
*/
boolean hsu_al_brewext_switch_comp_to_brew_ext
(
  rdm_assign_port_cb_type           rdm_comp_cb_func_ptr
);

/*=============================================================================
FUNCTION    hsu_al_brewext_switch_comp
=============================================================================*/
/**
Switches composition by calling hsu_conf_sel_ui_switch_comp.
  
Same as hsu_conf_sel_ui_switch_comp, except that this function receives an
integer. See hsu_conf_sel_ui_switch_comp() for more details.

@param usb_comp_id          [IN] -- USB composition ID.
@param rdm_comp_cb_func_ptr [IN] -- Callback for USB composition switch completion.

@dependencies
Must be called after stack initialization. 

@sideeffects
None.

@return
TRUE -- Indicates the first step was successful.\n
FALSE -- Indicates failure.
*/
boolean hsu_al_brewext_switch_comp
(
  int usb_comp_id, 
  rdm_assign_port_cb_type rdm_comp_cb_func_ptr
);

/*=============================================================================
FUNCTION    hsu_al_brewext_get_cur_comp_details
=============================================================================*/
/**
Returns the composition ID and number of RDM devices for the primary core's
current composition.

@param *hsu_comp_id            [IN] -- USB composition ID.
@param *num_of_rdm_dev_in_comp [IN] -- Number of RDM devices in the composition.

@dependencies
None.

@sideeffects
None.

@return
FALSE on error, TRUE otherwise.
*/
boolean hsu_al_brewext_get_cur_comp_details
(
  int *hsu_comp_id, 
  int *num_of_rdm_dev_in_comp
);

/** @} */ /* endgroup hsusb_qc_api */

#endif
