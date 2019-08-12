#ifndef DS_MUX_EXT_H
#define DS_MUX_EXT_H
/*===========================================================================

                    D A T A   S E R V I C E S    M U X   

                                E X T E R N A L 

DESCRIPTION 
  This is the external interface that is exposed out to modules outside the
  modem.


  This module mostly forwards the function calls with some translation to 
  the mux_hdlr module.  

Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_mux_ext.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/10   jl      Moved ds_mux_task to AU API file. 
08/14/09   spn     Created the module. 
===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/


#include "sio.h"


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================

FUNCTION      DS_MUX_EXT_SIO_INIT

DESCRIPTION   This function called from sio, to initialize mux driver
              calls uint32 sio_register_driver(sio_device_info_type *pHead) 
              to register all mux ports with sio

              Also the trigger to initialize the mux client

PARAMETERS    Function call back that MUX DRV has to call to register
              all the devices associated with this driver

DEPENDENCIES   None

RETURN VALUE   non-zero value for successful initialization

SIDE EFFECTS   None

===========================================================================*/
uint32 ds_mux_ext_sio_init
( 
  sio_register_device_driver_cb_type register_driver_cb_fn
);
/*===========================================================================

FUNCTION      DS_MUX_TASK

DESCRIPTION   This function is called from tmc to initialize mux task


PARAMETERS     dword, currently un used

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds_mux_task(dword dummy);
#endif /* DS_MUX_EXT_H */
