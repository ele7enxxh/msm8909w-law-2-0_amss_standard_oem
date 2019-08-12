#ifndef HDRRLPSORT_H
#define HDRRLPSORT_H

/*===========================================================================

                  H D R   R L P   S O R T I N G   L A Y E R
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the HDR RLP Sorting Layer.
  
Copyright (c) 2003, 2004, 2005 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlpsort.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/07   da      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "errno.h"

#ifdef FEATURE_HDR_QOS

#include "dsm.h"
#include "dsrlp_v.h"
#include "queue.h"
#include "errno.h"
#include "ds707_qosdefs.h"

#ifndef FEATURE_HDR
#error "FEATURE_HDR_QOS defined without FEATURE_HDR being defined"
#endif /* FEATURE_HDR */

/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/


/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRRLPSORT_BUFFER_PACKET

DESCRIPTION
  Called from hdrmrlp when a packet is received from PCP. This function
  stores the packets for a particular slot till the End of Slot
  notification is received from PCP, which triggers the processing
  of the stored packets.
  
DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
void hdrrlpsort_buffer_packet
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr,       /* RLP-specific ctl block   */
  dsrlpi_pkt_info_type        *pkt_info_ptr    /* Packet and RLP hdr       */
);

/*===========================================================================

FUNCTION HDRRLPSORT_SORT_PACKETS_IN_SLOT

DESCRIPTION
  This function sorts the rlp packets received in the last slot based on
  SAR sequence number.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpsort_sort_packets_in_slot
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr   /* RLP-specific ctl block         */
);

#endif /* FEATURE_HDR_QOS */

#endif /* HDRRLPSORT_H */
