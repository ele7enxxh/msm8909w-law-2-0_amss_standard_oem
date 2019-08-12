#ifndef HDRBCRSD_H
#define HDRBCRSD_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     H D R   

GENERAL DESCRIPTION
   Reed Solomon Decoder for N = 16(k=12,13,14), 32(K=26,26)
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcrsd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/04   kss     Initial port.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"                        
#include "dsm.h"


/*===========================================================================

FUNCTION HDRBCRSD_DECODE

DESCRIPTION
  Decodes and corrects (if possible) one row of an ECB block. Note that
  NULL pointers are taken to be missing packets or packets with failed CRC's.
  If error correction is possible, the reconstructed data is placed in a
  parity packet (no new packets are allocated). Thus, the pointers in the 
  packet array parameter may be reordered by this function.  (The caller 
  must take this into account.)
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrbcrsd_decode
(
  dsm_item_type* packets[], 
    /* Eventually this should be dsm_item pointers.  This is an array
       of N pointers to good data packets.  A NULL pointer implies a
       bad or missing packet. */

  uint8 outer_code
    /* Outer code index defined in the standard. */
);

#endif /* HDRBCRSD_H */
