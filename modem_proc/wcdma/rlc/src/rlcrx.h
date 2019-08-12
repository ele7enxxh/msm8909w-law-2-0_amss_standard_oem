#ifndef RLCRX_H
#define RLCRX_H
/*===========================================================================
            D O W N L I N K   R E C E I V E  

           R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  processing RLC PDUs received on the Downlink in acknowledged mode.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2004, 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcrx.h_v   1.3   22 May 2001 14:32:56   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcrx.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/09   ssg     Updated Copyright Information
06/02/04   mv      Fixed lint errors and warnings.
05/21/01   rc      Added prototypes for rlci_dl_adjust_pdu_pointer() and 
                   rlci_dl_calc_hdr_len(). Removed prototypes for 
                   AM specific functions.
04/10/01   rc      Fixed warnings found while compiling for ARM.
04/03/01   rc      Removed enum type rlci_pdu_type_e_type.
02/28/01   rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "rlcdl.h"
#include "rlci.h"

/*===========================================================================

FUNCTION RLCI_DL_ADJUST_PDU_POINTER

DESCRIPTION
   This function adjusts the pointers in the original PDU to point to the next 
   LI or data. The Header portion (LIs) are copied into a buffer and the
   data pointer is moved by the size of the length indicator. The header
   portion is then copied back to the front of the PDU.
 
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_adjust_pdu_pointer
(
  dsm_item_type **packet_ptr,                /* Pointer to PDU Packet Chain*/
  uint16        li,                          /* Length Indicator vale.     */
  uint16        hdr_len                      /* Length of the Header (LIs) */
                                             /* in the PDU.                */
);                                            



/*===========================================================================

FUNCTION RLCI_DL_CALC_HDR_LEN

DESCRIPTION
  Calculates the length of the PDU Header (All the length indicators).
  
DEPENDENCIES
  None.

RETURN VALUE
  Length of the header.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 rlci_dl_calc_hdr_len
(
  dsm_item_type *item_ptr,             /* Downlink PDU                     */
  uint16         li_len,               /* Length of the length Indicator   */
                                       /* 1: 7-bit LI & 2 :15-bit Li       */
  uint8          ext                   /* Extension bit - 1 indicates that */
                                       /* the next byte of the PDU is a    */
                                       /* length Indicator                 */             
);                                     


#endif
