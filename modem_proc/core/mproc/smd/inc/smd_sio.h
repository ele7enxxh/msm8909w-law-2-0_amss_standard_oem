#ifndef SMD_SIO_H
#define SMD_SIO_H
/*===========================================================================

                    Shared Memory Driver SIO Interface Header File

 Copyright (c) 2005-2008 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/smd/inc/smd_sio.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/09   hwu     Include sio.h (WM does not compile otherwise) 
11/26/08   jlk     removed smd internal prototypes
09/18/08   jlk     Moved or removed unecessary macros
02/28/07   ptm     Updated smd port ID and ioctl cmd translations.
01/09/07   bfc     Added SMD using DM
09/28/06   taw     Added macros for translating SIO ports and command to
                   SMD ports and commands.
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
01/26/06   bfc     Added and defined smd_sio_close_ext and 
                   smd_sio_flush_tx_ext
01/24/06   bfc     Added and defined smd_sio_contol_(open|close|transmit) 
11/23/05   ptm     Moved smd_sio_bridge_*() functions to smd_bridge.h and
                   added control APIs.
09/29/05   TMR     Added smd_sio_bridge_connect(), smd_sio_bridge_disconnect()
                   and smd_sio_bridge_init()
11/07/04   adm     SMD's SIO interface related code was leveraged from MSP1000
                   shared memory driver code. 
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"

/* this disables sio_wrapper from registering smd_sio */
#define FEATURE_SMD_WRAPPER_FREE


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      SMD_SIO_DEV_INIT

DESCRIPTION   Initializes the SMD SIO device.
  
DEPENDENCIES  Should be called just once.

ARGUMENTS     sio_register_device_driver_cb_type register_driver_fn 
               The registration function passed by SIO

RETURN VALUE  Always 1
   
SIDE EFFECTS  None
===========================================================================*/
uint32 smd_sio_driver_init
(
  sio_register_device_driver_cb_type register_driver_fn
);

/* these functions are called directly by sio.c, we should change that */
/*===========================================================================
FUNCTION      smd_sio_flush_tx_ext
                                                                             
DESCRIPTION   This function flushes the TX and then calls an extended
              callback.

ARGUMENTS     port_id - SIO port to flush

              flush_cb - extended callback to call when all of the queued data 
                         has been received by the other processor.
                  
              cb_data - data to use when calling the extended callback
DEPENDENCIES  User should not queue more data until after the registered
              callback is executed.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_flush_tx_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type flush_cb,
  void                  *cb_data
);

/*===========================================================================
FUNCTION      smd_sio_close_ext
                                                                             
DESCRIPTION   This function starts closing the port. It saves the client's
              extended callback function which will be called when the port 
              is closed.

ARGUMENTS     port_id - SIO port to close

              close_cb - callback to call when the stream is closed and all
                        of the queued data has been received by the other
                        processor.

              cb_data - data to pass the the close_cb when ready
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called when all pending data has been received
              by the other processor
===========================================================================*/
void smd_sio_close_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type close_cb,
  void                  *cb_data
);
#endif /* SMD_SIO_H */
