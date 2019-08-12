#ifndef DS707_ASYNC_WMK_H
#define DS707_ASYNC_WMK_H
/*===========================================================================
                         D S 7 0 7 _ A S Y N C _ W M K

DESCRIPTION
  This file contains functions related to watermarks used in IS707 circuit
  switched data calls (Async/Fax).

EXTERNALIZED FUNCTIONS

ds707_async_wmk_init
  This function initializes watermarks.

ds707_async_wmk_setup_um_wmk
  This function configures the Um watermark. 

ds707_async_wmk_setup_sio_wmk
  This function configures the sio watermark.

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_wmk.h_v   1.4   02 Oct 2002 13:23:00   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_async_wmk.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
-----------------------------------------------------------------------------  
02/11/11    ack    Global Variable Cleanup
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
08/15/02    atp    Added support for mode-specific SIO wmks. 
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707_ASYNC
#include "dsm.h"
#include "queue.h"

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_UM_WMK

DESCRIPTION   This function initializes the PPP to RLP tx watermark
              to operate in 707 async mode. This is the same wm used in
              pkt mode. On the RLP rx side there's no wm, there's only a
              queue.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_um_wmk
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_SIO_WMK

DESCRIPTION   This function initializes the DS to SIO tx watermark
              to operate in 707 async mode. This is not the same wm used in
              pkt mode. On the SIO rx side there is no wm, instead the
              sio rx func is registered with SIO.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_sio_wmk
(
  void
);

/*===========================================================================
FUNCTION      DS707_GET_ASYNC_TO_SIO_TX_WMK

DESCRIPTION   Accessor function to ds707_wmk_async_to_sio_tx_wmk

DEPENDENCIES  None

RETURN VALUE  Returns a pointer to ds707_wmk_async_to_sio_tx_wmk

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type* ds707_get_async_to_sio_tx_wmk
(
  void
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DS707_ASYNC_WMK_H        */
