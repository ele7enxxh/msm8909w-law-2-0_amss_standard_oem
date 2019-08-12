#ifndef _HSU_AL_SER_H_
#define _HSU_AL_SER_H_

/*=============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
H E A D E R   F I L E

==============================================================================*/
/**
@file hsu_al_ser.h

Contains the API for the serial functions' adaptation layer.

  Externalized functions:
  - hsu_al_ser_init
  - hsu_al_ser_open
  - hsu_al_ser_close
  - hsu_al_ser_transmit
  - hsu_al_ser_flush_tx
  - hsu_al_ser_ioctl
  - hsu_al_ser_handle_set_configuration
  - hsu_al_ser_handle_obex_uninit
  - hsu_al_ser_handle_acm_uninit

Initialization and sequencing requirements:\n
hsu_al_ser_init must be called prior to performing any other operations
*/
/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_ser.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
05/08/06 esh  First revision of file

=============================================================================*/

/*=============================================================================

INCLUDE FILES FOR MODULE

=============================================================================*/
#include <sio.h>
#include "dsm.h"

#include "uw_device_cdc.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Constants and Macros
=============================================================================*/
/*=============================================================================
Typedefs
=============================================================================*/

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_ser_init
=============================================================================*/
/**
Initializes the data structures for later usage.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Operation was successful.\n
FALSE -- Operation was not successful.
*/
boolean hsu_al_ser_init
(
  void
);

/*=============================================================================
FUNCTION    hsu_al_ser_open
=============================================================================*/
/**
Opens a stream across a serial interface on the USB.

@param *open_ptr [IN] -- Stream configuration.

@dependencies
None.

@sideeffects
None.

@return
SIO_DONE_S -- SIO stream as specified in open_ptr has been opened
              successfully.\n
SIO_BADP_S -- Bad parameter; function was not successful.
*/
sio_status_type hsu_al_ser_open
( 
  sio_open_type *open_ptr
);

/*=============================================================================
FUNCTION    hsu_al_ser_close
=============================================================================*/
/**
Performs all the bookkeeping necessary to close a stream.

An optional Close Callback can be passed to this function if the application
wants to be notified when this layer has done all the bookkeeping needed for
the stream closing.

@param stream_id     [IN] -- Stream ID.
@param port          [IN] -- Port ID.
@param close_done_cb [IN] -- Function to call when the application layer has
                             completed the bookkeeping required to close the
						     stream.

@dependencies
None

@sideeffects
close_done_cb will be called when this layer wants to signal to the caller
that it has done all the bookkeeping needed for the stream closing.

@return
None.
*/
void hsu_al_ser_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port,
  sio_vv_func_ptr_type close_done_cb
);

/*=============================================================================
FUNCTION    hsu_al_ser_transmit
=============================================================================*/
/**
Transmits a packet or, if the transmitter is busy, schedules the transmission
of a packet. Note that following this invocation, the caller is no longer the
owner of the packet and may no longer use it.

@param stream_id -- [IN] Stream ID.
@param port_id   -- [IN] SIO port ID.
@param *tx_ptr   -- [IN] Packet to be transmitted.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ser_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type      port_id,
  dsm_item_type *tx_ptr
);

/*=============================================================================
FUNCTION    hsu_al_ser_flush_tx
=============================================================================*/
/**
Signals the caller when the last packet to transfer has successfully reached
its destination, but only if the caller supplied a callback to be invoked when
this occurs.

@param stream_id        [IN] -- Stream ID.
@param port_id          [IN] -- SIO port ID.
@param flush_tx_done_cb [IN] -- Function to call when the application layer has
                                successfully transmitted all pending data.

@dependencies
None.

@sideeffects
flush_tx_done_cb is invoked when the last packet to transfer has reached its
destination.

@return
None.
*/
void hsu_al_ser_flush_tx
( 
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_vv_func_ptr_type flush_tx_done_cb
);

/*=============================================================================
FUNCTION    hsu_al_ser_ioctl
=============================================================================*/
/**
Allows the passing of an I/O control to the serial I/F, represented by 
port ID or stream ID.

@param stream_id        [IN] -- Stream ID.
@param port_id          [IN] -- SIO port ID.
@param ioctl_cmd        [IN] -- SIO I/O control command.
@param *ioctl_param_ptr [IN/OUT] -- SIO I/O control command parametrs.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ser_ioctl
(
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
);

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE UNIT TESTS (ut)
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_ser_ut_check_open_params
=============================================================================*/
/**
Checks if all parameters have been updated/set correctly after a sio_open
command. This is part of the open unit test. If an error is encountered, the
function returns FALSE and a description string.

@param *open_ptr            [IN] -- Pointer to the stream configuration.
@param *returned_error_str [OUT] -- Error description string (if applicable).
@param error_str_size       [IN] -- Size of the error string.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Parameters have been updated/set correctly.\n
FALSE -- Error, plus returned_error_str as an output parameter.
*/
boolean hsu_al_ser_ut_check_open_params
(
  const sio_open_type *open_ptr,
  char* returned_error_str,
  uint8 error_str_size
);

/*=============================================================================
FUNCTION    hsu_al_ser_ut_dont_simulate_hsu_over_uart
=============================================================================*/
/**
Causes any call to hsu_al_ser_open to bypass the HS-USB over UART emulation
(which is on by default if any of the serial I/Fs are mapped over the UART,
which is currently on the case with DIAG).

This function allows unit tests to call hsu_al_ser_base_* without UART
emulation. 

@dependencies
None.

@sideeffects
No HSU over UART simulation is possible.

@return
None.
*/
void hsu_al_ser_ut_dont_simulate_hsu_over_uart
(
  void
);

/*=============================================================================
FUNCTION    hsu_al_ser_hsu_over_uart_is_simulated
=============================================================================*/
/**
Checks whether a CDC application context has UART emulation.

@param *ctx [IN] -- CDC context.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- UART emulation enabled.\n
FALSE - UART emulation not enabled.
*/
boolean hsu_al_ser_hsu_over_uart_is_simulated
(
  const void* ctx
);


/*=============================================================================
FUNCTION    hsu_al_ser_ut_set_test_mask
=============================================================================*/
/**
Sets a test mask for the specified serial interface, represented by port ID
(used in unit tests).

As the code for the serial application layer is executed, this mask is checked
into commonly used code segments, and if a match is
made, the CDC FD will stub to intercept messages

@param port_id      [IN] -- SIO port ID.
@param ut_test_mask [IN] -- The test mask that is set.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ser_ut_set_test_mask
(
  sio_port_id_type port_id,
  uint32  ut_test_mask
);

/*=============================================================================
FUNCTION    hsu_al_ser_handle_set_configuration
=============================================================================*/
/**
Called when the enabled callback is received.

This function sets the FD handle according to the current configuration FD for
a specified core_index.

@param core_index -- [IN] Core index.

@dependencies
None.

@sideeffects
None.

@return
None. 
*/
void hsu_al_ser_handle_set_configuration(uint8 core_index);

/*=============================================================================
FUNCTION    hsu_al_ser_handle_obex_uninit
=============================================================================*/
/**
Called when OBEX CDC FD notifies that it is being de-initialized.

@param ctx -- [IN] CDC context.

@dependencies
None.

@sideeffects
None.

@return
None. 
*/
void hsu_al_ser_handle_obex_uninit(cdc_appctx_t ctx);

/*=============================================================================
FUNCTION    hsu_al_ser_handle_acm_uninit
=============================================================================*/
/**
Called when ACM CDC FD notifies that it is being de-initialized.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ser_handle_acm_uninit(void);

/*=============================================================================
FUNCTION    hsu_al_ser_pool_many_cb
=============================================================================*/
/**
Invokes the appropriate actions when the serial memory pool arrives at
DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

@param pool_id [IN] -- ID of the memory pool.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ser_pool_many_cb(dsm_mempool_id_enum_type pool_id);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_AL_SER_H_ */
