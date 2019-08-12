#ifndef _DSDVRLPI_RX_H
#define _DSDVRLPI_RX_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   I N T E R N A L   H E A D E R  F I L E

DESCRIPTION
  This file contains internal variable and function declarations specific to 
  the Radio Link Protocol implementation that supports EVDV.

EXTERNALIZED FUNCTIONS
  dsdvrlpi_rx_idle_frame()
    This function processes idle frames.
  
  dsdvrlpi_rx_new_data()
    This function processes new data frames.

   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsdvrlpi_rx.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/04   atp     After lint.
05/07/04   atp     Initial version. First cut at supporting EVDV (IS707-B).

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "dsrlpi.h"
#include "queue.h"



/*===========================================================================
                      EXTERNAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION DSDVRLPI_RX_IDLE_FRAME

DESCRIPTION
  This function processes idle frames. Specific to DV RLP implementation.

DEPENDENCIES
  None

RETURN VALUE
  Always returns DONE.

SIDE EFFECTS
  None
===========================================================================*/
dsrlpi_fr_det_enum_type dsdvrlpi_rx_idle_frame
( 
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block      */
  dsrlpi_curr_fr_info_type     *curr_info_ptr /* frame info ptr            */
); 

/*===========================================================================

FUNCTION DSDVRLPI_RX_NEW_DATA

DESCRIPTION
  This function processes new data frames. Specific to DV RLP implementation.

DEPENDENCIES
  CRC checks done by frame determination.
  Input pointers are non-NULL.
  The dsm_item embedded within contains only the RLP frame's data payload,
  and does not contain any header info or CRC info.

RETURN VALUE
  Always returns DONE.

SIDE EFFECTS
  None.
===========================================================================*/
dsrlpi_fr_det_enum_type dsdvrlpi_rx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block       */
  dsm_item_type            **fr_item_pkt_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr    /* frame info ptr             */
);

#endif  /* _DSDVRLPI_RX_H */

