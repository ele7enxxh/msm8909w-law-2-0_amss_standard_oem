#ifndef ONDEVICE_DIAG_H
#define ONDEVICE_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            ONDEVIVE DRIVER  I N I T              
                             
            Initialization of Ondevice driver

GENERAL DESCRIPTION

  This module contains the code used by the device manager (SIO) to initialize
  the OnDEvice driver.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  The only external function, ondevice_diag_driver_init, is called by the device 
  manager (SIO) to perform device driver initialization.  The name of this 
  function cannot change since SIO depends on it.

Copyright (c) 2009,2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/ondevice_diag.h#1 $

when         who       what, where, why
-----------  ------  ------------------------------------------------------ 
05/18/2010   as      Added support for selecting dynamic configuaration mask 
10/10/2009   as      Initial revision.

===========================================================================*/
#include "comdef.h"

#include "sio.h"

/*===========================================================================

FUNCTION       ondevice_diag_driver_init

DESCRIPTION    This is the UART device driver initialization function.  It's
               called by the device manager (SIO) to initialize the 
               UART driver.

DEPENDENCIES   None.

RETURN VALUE   1.

SIDE EFFECTS   None.
===========================================================================*/

uint32  ondevice_diag_driver_init( sio_register_device_driver_cb_type register_driver_cb_fn );

    

/*===========================================================================

FUNCTION ondevice_diag_sio_open

DESCRIPTION
  This function allocates stream as indicated in open_ptr.
    
DEPENDENCIES
  None
  
RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.  
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern sio_status_type ondevice_diag_sio_open
(
  sio_open_type     *open_ptr
);


/*===========================================================================

FUNCTION ondevice_diag_sio_close

DESCRIPTION
  This function deallocates given stream.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

extern void ondevice_diag_sio_close
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  sio_port_id_type   port_id,
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
);


/*===========================================================================

FUNCTION ondevice_diag_sio_transmit                   

DESCRIPTION
  This function transmits over SD (ONDEVICE) Card.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ondevice_diag_sio_transmit
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,             /* SIO Port ID                   */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
);


/*===========================================================================

FUNCTION ondevice_diag_sio_ioctl                        EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream
    
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/
void ondevice_diag_sio_ioctl
(
  sio_stream_id_type    stream_id,             /* Stream ID                */
  sio_port_id_type      port_id,               /* SIO Port ID              */
  sio_ioctl_cmd_type    cmd,                   /* Command                  */
  sio_ioctl_param_type *param                  /* Parameter                */
);

/*===========================================================================

FUNCTION ondevice_diag_sio_flush_tx                       EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of flush Tx, 
    
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/
void ondevice_diag_sio_flush_tx
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  sio_port_id_type   port_id,               /* Port ID                     */
  void             (*flush_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
);

/*===========================================================================

FUNCTION ondevice_diag_set_mask_config

DESCRIPTION 
  This function allows client to select dynamic mask configuration file. 
 
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS 
 
===========================================================================*/
void ondevice_diag_set_mask_config ( char * path);

#endif /* ONDEVICE_DIAG_H */

