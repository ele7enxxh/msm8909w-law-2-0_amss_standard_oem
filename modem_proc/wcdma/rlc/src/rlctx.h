#ifndef RLCTX_H
#define RLCTX_H
/*===========================================================================
               R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains data types and function prototypes associated with the
  RLC.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlctx.h_v   1.1   22 May 2001 14:51:12   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlctx.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
05/21/01   rc      Added more comments.
01/31/01   rc/ttl  Moved function prototypes rlci_ul_build_pdu_header(),
                   rlci_ul_build_pdu_data() and rlci_ul_insert_padding()
                   from rlculum.h since these functions are shared 
                   between Unacknowledged and Acknowledged mode.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "rlcul.h"



/*===========================================================================

FUNCTION RLCI_UL_BUILD_PDU_HEADER

DESCRIPTION
  Build PDU header. 
  Leave space for Sequence number and other header information (2 bytes for
  Acknowledged Mode and 1 byte for unacknowledged mode). If the SDU ended 
  in the previous PDU and there was no room for the length indicator in 
  that PDU, store the special length indicator(0x00 or 0x7FFB) to indicate 
  that in the current PDU. If padding is TRUE, then store padding length 
  indicator (0x7F or 0x7FFF) and return.
  For all other cases, While PDU size is greater than the SDU size, assign 
  length indicator values to the DSM item.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_build_pdu_header
(
  dsm_item_type         **packet_ptr,/* Address of Pointer to head of PDU  */
  rlci_ul_ctrl_blk_type *ctrl_blk,   /* Pointer to Uplink Control Block    */
  uint16                pdu_size     /* Size of the PDU                    */
);

/*===========================================================================

FUNCTION RLCI_UL_BUILD_PDU_DATA

DESCRIPTION
 Builds the data part of the PDU. Copies the data from the SDU(s) to the PDU.
 The sdu is not removed from the queue till all the bytes have been copied
 to build PDUs. The current cnt of the watermark is updated as the bytes 
 are copied into the PDU. When the SDU is completely copied into PDUs, the
 packet is removed from the queue. At this time, the current cnt already has
 the right value. 
 If there is more room in the PDU and the current SDU has been copied 
 completely, get the next SDU from the queue. If there are no more SDUs in 
 the queue, set PADDING = TRUE.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_build_pdu_data
(
  dsm_item_type         **packet_ptr,/* Address of Pointer to head of pkt  */
  rlci_ul_ctrl_blk_type *ctrl_blk,   /* Pointer to uplink control block    */
  uint16                pdu_size     /* Size of the PDU in bytes           */
);


/*===========================================================================

FUNCTION RLCI_UL_INSERT_PADDING

DESCRIPTION
  Inserts Padding at the end of a PDU, when there are no more SDUs 
  to be processed and there is room left in the PDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_insert_padding
(
  dsm_item_type         **packet_ptr,/* Address of pointer to head of pkt  */
  uint16                pdu_size     /* PDU size in bytes                  */
);

/*===========================================================================

FUNCTION WRITE_SPECIAL_HEADER

DESCRIPTION
  Build special header 
  Reserve  space for sequence number and extension bit, 
  If the special indication is turned on. This is turned on when the previous
  PDU was exactly filled w/ the last segment of the previous SDU and there 
  is no LI that indicates the end of the SDU in the previous PDU.
     
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint8 write_special_header
(
  rlci_ul_ctrl_blk_type *ctrl_blk,     /* Pointer to Uplink Control Block  */
  dsm_item_type         *item_ptr,     /* Pointer to DSM item store header */
  uint8                 **data_ptr     /* Address of data pointer          */
);

#endif
