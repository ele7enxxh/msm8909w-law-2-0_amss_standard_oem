#ifndef DS3GSIOLIB_EX_H
#define DS3GSIOLIB_EX_H
/*===========================================================================

                        D A T A   S E R V I C E S

                    E X T E N D E D     D 3 G S I O L I B 

                          H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures,
  enumerated constants, and function prototypes required for the
  Extended DS3GSIOLIB .
  
Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat.h_v   1.10   08 Nov 2002 12:46:24   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gsiolib_ex.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/03/16   skc    Adding support for ESC SEQUENCE for PS Data call.
05/14/10   vs      Initial Extended file

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds3gsiolib.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_REGISTER_CALLBACK_FUNC

DESCRIPTION
  This function registers a mode specific call back function table. 

  Caller should check the return value of this function to see if the
  registration succeeded.
 
DEPENDENCIES
  None
 
PARAMETERS 
  Function table to be registered
  Port Id
 
RETURN VALUE
  Returns TRUE if a func_tbl is not registered with SIOLIB already and
  if the passed in function pointer is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  If a mode-specific handler function table was previously registered, 
  it will be over-written.
===========================================================================*/
boolean  ds3g_siolib_ex_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr,        /* Handler function table */
  ds3g_siolib_port_e_type     port_id
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode-specific handler function table.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Function table to be de-registered
  2. Port Id
 
RETURN VALUE
  Returns TRUE if deregister happened successfully. 
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  Resets the callback function table pointer to null. 
===========================================================================*/
boolean  ds3g_siolib_ex_deregister_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr,        /* Handler function table */
  ds3g_siolib_port_e_type     port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CALL_COMING_UP

DESCRIPTION
  This function sets the call coming up boolean variable which determines
  if a data call is in the process of coming up.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Flag indicating that the call is coming up
  2. Port Id
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void  ds3g_siolib_ex_set_call_coming_up
(
  ds3g_siolib_port_e_type port_id,
  boolean                 busy
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IOCTL

DESCRIPTION
  This function allows control of open stream for the port on which data call
  is established.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.
 
PARAMETERS 
  1. SIO IOCTL command
  2. Argument to be passed along with the command.
  3. Port Id

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_ex_ioctl
(
  sio_ioctl_cmd_type       cmd,
  sio_ioctl_param_type    *param,
  ds3g_siolib_port_e_type  port_id  /* Port Id */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_CHANGE_MODE
DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the SIO RX watermark , and the rx_func will be called each time data is
  enqueued to the watermark.

  Depending on the new serial mode, callback functions are registered for
  serial events of interest, namely:
    Autodetect Mode - Packet detection callback
    Rawdata Mode    - Escape signal detection callback
    Packet Mode     - None

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.
 
PARAMETERS 
  1. New Serial Mode
  2. SIO Rx Watermark
  3. SIO Tx Watermark
  4. Function to be invoked everytime data bytes are rx'ed
  5. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_change_mode
(
    sio_stream_mode_type    stream_mode,                   /* New stream mode */
    dsm_watermark_type      *rx_wm_ptr,     /* Watermark for rx data from SIO */
    dsm_watermark_type      *tx_wm_ptr,       /* Watermark for tx data to SIO */
    sio_rx_func_ptr_type    rx_func_ptr, /* Called each time data bytes rx'ed */
    ds3g_siolib_port_e_type port_id                               /* Port Id */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  If there is no port currently open, the dtr_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_dtr_asserted
(
  boolean                  *dtr_status, /* Set to TRUE if DTR asserted, or 
                                          FALSE if not */
  ds3g_siolib_port_e_type  port_id     /* Port Id */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_RTS_ASSERTED

DESCRIPTION
  Checks the state of the Request To Send (RTS) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not RTS is asserted.

DEPENDENCIES
  If there is no port on which data call exists, the rts_status value 
  has no meaning.
 
PARAMETERS 
  1. Will indicate whether RTS was asserted or not
  2. Port Id
 
RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

==========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_rts_asserted
(
  boolean                 *rts_status, /* Set to TRUE if RTS asserted, 
                                          or FALSE if not */
  ds3g_siolib_port_e_type  port_id  /* Port Id */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.
 
PARAMETERS 
  1. DSM item containing data to transmit
  2. Re-arm Autodetect Mode
  3. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_setup_tx
(
  dsm_item_type            *tx_ptr,            /* DSM item containing 
                                                  data to transmit */
  boolean                  rearm_autodetect,   /* TRUE = Re-arm Autodetect 
                                                  Mode */
  ds3g_siolib_port_e_type  port_id             /* Port Id */
);

/*=========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CD_STATE

DESCRIPTION
  This function sets the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0 - CD is always ON
     &C1 - CD is ON when the remote carrier is ON
     &C2 - CD is always ON except for 1sec 'wink' when a data call is
           disconnected
  2) Remote carrier state
  3) End-of-call winking (only if &C=2)
  4) SMS message entry
     CD is ON during SMS message entry (overrides &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.
 
PARAMETERS 
  1. DCD event
  2. Port Id
 
RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event,      /* Event that occurred, if any */
  ds3g_siolib_port_e_type       port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_FLUSH_TX

DESCRIPTION
  This function is used to schedule the SIO TX queue to be flushed when
  changed to and from online-command and online-data modes.

  The flush_func_ptr_cb parameter is used to pass a callback function to
  SIO that is called when the flush operation has completed.  The callback
  should be used to notify the task which requested the flush operation
  that the TX queue is now empty.

DEPENDENCIES
  No action is taken if there is no port currently open.
 
PARAMETERS 
  1. Callback to be invoked when flush operation is complete.
  2. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_flush_tx
(
  void                     (*flush_func_ptr_cb)(void),
  ds3g_siolib_port_e_type  port_id /* Port Id */
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_SET_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl on a 
  specified port.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.

DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed
 
PARAMETERS 
  1. Flwo mask indicating the kind of flow being enabled/disabled
  2. Flag indicating whether flow is enabled or disabled
  2. Port Id

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void ds3g_siolib_ex_set_inbound_flow
( 
  uint32 in_mask,          /* input mask of calling function               */
  ds3g_flow_e_type flow,  /* specifies if flow is to be enabled or disabled */
  ds3g_siolib_port_e_type  port_id   /* DS3G_SIOLIB port id                */
);

/*=========================================================================

FUNCTION DS3G_SIOLIB_DETERMINE_PORT_TO_BE_ALLOCATED

DESCRIPTION
  This function returns the port that will be allocated.

DEPENDENCIES
 
PARAMETERS 
  Port Allocation Information
 
RETURN VALUE
  Port Id of the port to be allocated

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_determine_port_to_be_allocated
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_ALLOCATE_PORT

DESCRIPTION
  This function attempts to allocate an SIO port for an external data
  call.  It implements a simple semaphore flag for the port resource
  to ensure only one data call per SIO port.  A port must be
  unallocated and in AUTODETECT state for it to be available.  On
  successful allocation, a valid port identifier is returned.
  Otherwise, the NONE identifier is returned.  It is the calling
  routines responsibility to honor the resource semaphore.
                
DEPENDENCIES
  None
 
PARAMETERS 
  1. Port allocation information
  2. Client Id which is trying to allocate the port

RETURN VALUE
  DS3G_SIOLIB_PORTID_NONE on error condition, valid port identifier otherwise.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_ex_allocate_port
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr,
  ds_call_type_e_type                  call_type
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_RELEASE_PORT

DESCRIPTION
  This function releases a previously allocated SIO port, clearing the
  semaphore flag.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Port Id
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_release_port
(
  ds3g_siolib_port_e_type portid
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_CSD_PORT

DESCRIPTION
  This function returns the CSD port, if allocated. THe function will return
  DS3G_SIOLIB_PORT_NONE if CSD port is not allocated.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  CSD port, if allocated
  DS3G_SIOLIB_PORT_NONE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_ex_get_csd_port
(
  void
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_PSD_PORT

DESCRIPTION
  This function returns the PSD port, if allocated. THe function will return
  DS3G_SIOLIB_PORT_NONE if PSD port is not allocated.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  PSD port, if allocated
  DS3G_SIOLIB_PORT_NONE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_ex_get_psd_port
(
  void
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_CURRENT_CALL_TYPE

DESCRIPTION
  This function returns the current call type.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
 Current call type

SIDE EFFECTS
  None

===========================================================================*/
ds_call_type_e_type ds3g_siolib_ex_get_current_call_type
(
  void
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CURRENT_CALL_TYPE

DESCRIPTION
  This function sets the current call type.

DEPENDENCIES
  None
 
PARAMETERS 
  DS Call type
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_current_call_type
(
  ds_call_type_e_type call_type
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_CD_ASSERTED

DESCRIPTION
  Checks the state of the Carrier Detect (CD) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_cd_asserted
(
  boolean                 *cd_status,     /* Set to TRUE if DTR asserted, 
                                              or FALSE if not */
  ds3g_siolib_port_e_type  port_id
);


#endif /* DS3GSIOLIB_EX_H */
