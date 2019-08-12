/*===========================================================================
                U  P  L  I  N  K    T  R  A  N  S  M  I  T
          R A D I O   L I N K   C O N T R O L
                          
DESCRIPTION
  This file provides the functions to prepare RLC PDUs for
  transmission on the uplink.
    
EXTERNALIZED FUNCTIONS
  List any functions that are externalized in this file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001-03 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2004-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlctx.c_v   1.18   12 Jul 2002 19:10:24   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlctx.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
-------  ---     ----------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
02/01/12  grk     Feature cleanup
06/15/10  pj      Fixed lint warning:
                  copy 2-byte value byte-by-byte into data_ptr 
05/11/09  ssg     Updated Copyright Information
03/03/08  ssg     Added support to alternate e-bit interpretations under the
                  featue flag FEATURE_RLC_ALTERNATE_E_BIT.
05/07/08  pj      Assigned prev_byte pointer to data pointer to avoid NULL pointer access
03/24/08  pj      Added support for data path optimizations under 
                  FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
01/21/08  pj      Added null pointer check to avoid klocwork error.
01/18/07  ssg     Added fix to mark the SDU as used, when li = 00/0000/7ffb
                  needs to inserted in the PDU.
08/01/06  ttl     Remove code under RLC_FILL_NULL_PATTERN.
06/05/06  ssg     Added poll_delay check for octet less(LI=7FFB) condition
03/14/06  ttl     Lint fixes.
11/18/05  aw      Added the check to ignore the SDU with size 0 while
                  building PDUs.
11/07/05  aw      Replaced the FEATURE_HSDPA with the check for release 5 
                  indicator from L2.
05/31/05  mv      Changed the feature name from FEATURE_HSDPA to
                  FEATURE_RELEASE_5_RLC. This is because this RLC change
                  is backward incompatible with Release 99.
05/04/05  mv      Added the support to send special LI 7C on the UM mode 
                  uplink for Release 5 under FEATURE_HSDPA if the 
                  beginning of the PDU is the beginning of a SDU and there 
                  is no other special LI to indicate that.
06/02/04  mv      Fixed lint errors and warnings.
02/19/04  mv      Added the include file customer.h
01/20/04  mv      Replaced the feature FEATURE_RLC_ENHANCED_CB with
                  FEATURE_RLC_SDU_RCVD_ENHANCED_CB.
12/29/03  vsk     call watermark lowater callback with the watermarck callback data
10/16/03  mv      Changed the access to the AM transmission queue because it 
                  is no longer part of the structure rlci_ul_am_ctrl_type.
06/26/03  mv      Fixed the bug of not allocating new dsm item while building 
                  PDU header when there is no space in the dsm item for 
                  LI that indicates the presence of padding in the PDU but the 
                  PDU has to be filled with padding.
02/14/03  ttl     Fixed the bug of not setting the polling bit at the last
                  tx PDU.
10/30/02  ttl     Replaced % w/ &.
09/16/02  ttl     Added FEATURE_RLC_DSM_TRACING.
07/31/02  ttl     Fixed the bug of Poll SDU not being working correctly.
07/27/02  ttl     Optimize the PDU building code.
07/11/02  ttl     Fixed the WM problem due to the locally changing of current_cnt
                  of WM.
02/14/02  ttl     Changed the loop counts to be type uint16.
01/07/02  ttl     Added a check for DSM allocation.
11/14/01  ttl     Fixed LI problem which is not accumulated when building PDUs.
                  Removed debug codes.
10/22/01  sk      Implemented POLL_SDU feature
08/08/01  ttl     Changed padding from 0xff to 0x00.
06/28/01  rc      Changed MSG levels.
06/11/01  ttl     Fixed bug on SDU_PDU mapping.
06/08/01  ttl     Added Last PU in the buffer polling mechanism.
06/08/01  ttl     Added two messages for the SDU CNF cb function.
                  Changed TIN_SDU_ACK compiler option to FEATURE_AL1.
05/21/01  rc      Fixed a bug with 15-bit LIs where only one byte of the LI
                  was copied instead of two, when the LI spanned two
                  dsm items. Also, cleaned up code and added more comments.
05/02/01  ttl     Added SDU/PDU mapping infromation.
04/10/01  rc      Fixed warnings found while compiling for ARM.
02/28/01  ttl     Added support for Acknowledged mode.
02/01/01  rc      Changed code to update the LSB of the previous 
                  LI/Sequence number byte before the new LI is copied into 
                  the dsm item.
01/31/01  rc/ttl  Moved functions rlci_ul_build_pdu_header(),
                  rlci_ul_build_pdu_data() and rlci_ul_insert_padding()
                  from rlculum.c since these functions are shared 
                  between Unacknowledged and acknowledged mode.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "customer.h"

#define FEATURE_DSM_WM_CB

#include "rlctx.h"
#include "rlci.h"


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


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
)
{
  uint8  li_len = 0;                   /* Length of length indicator       */

  /*-------------------------------------------------------------------------
    For Unacknowledged Mode, leave the first byte in the item for RLC 
    Sequence Number. (Sequence number + Extension bit)
  -------------------------------------------------------------------------*/
  if (ctrl_blk->lc_mode == UE_MODE_UNACKNOWLEDGED)
  {
    li_len = ctrl_blk->ctrl.um_ctrl.li_length;
    **data_ptr = 0x00;
    item_ptr->used += 1;
  }
  /*-------------------------------------------------------------------------
    For Acknowledged Mode, leave 2 bytes in the item for the RLC Sequence 
    Number. (Sequence number + Polling bit + Header Extension bits)
  -------------------------------------------------------------------------*/
  else
  {
    li_len = ctrl_blk->ctrl.am_ctrl.ul_state.li_length;
    **data_ptr = 0x00;
    *(*data_ptr + 1) = 0x00;
    *data_ptr += 1;
    item_ptr->used += 2;
  }   

  /*-------------------------------------------------------------------------
    Send polling now if it is delayed due to special LI=0x0 or LI=FFFB 
  -------------------------------------------------------------------------*/
  if ((ctrl_blk->special_ind == TRUE || ctrl_blk->octet_less== TRUE)
    && ctrl_blk->ctrl.am_ctrl.ul_state.poll_delay)
  {
    ctrl_blk->ctrl.am_ctrl.ul_state.poll_next = TRUE;
    ctrl_blk->ctrl.am_ctrl.ul_state.poll_delay = FALSE;
    MSG(MSG_SSID_WCDMA_RLC, MSG_LEGACY_LOW, "Polling on Last_PU_Poll");
  }
  
  /*-------------------------------------------------------------------------
    Check whether the special indication flag is true. If it is, store the 
    special indication value (0x00 or 0x0000) in the dsm item.
    This special indication is turned on when the previous PDU was exactly
    filled w/ the last segment of the previous SDU and there is no LI that
    indicates the end of the SDU in the previous PDU.
  -------------------------------------------------------------------------*/
  if (ctrl_blk->special_ind == TRUE)
  {
    ctrl_blk->special_ind = FALSE;

    /*-----------------------------------------------------------------------
      Turn on the Extension bit(UM) or the last LSB of the Header Extension 
      Type.(by setting the LSB of the current byte to 1, since there is a 
      new LI and move the data pointer to the next location.)
    -----------------------------------------------------------------------*/
    **data_ptr |= 0x01;  
    *data_ptr += 1;

    /*-----------------------------------------------------------------------
      Filled in this special_ind LI (0x00 or 0x0000).
    -----------------------------------------------------------------------*/
    if (li_len == 1) /* li_len is 7 bits  */
    {
      **data_ptr =0x00;
    }
    else /* li_len is 15 bits */
    {
      **data_ptr = 0x00;
      *(++(*data_ptr)) = 0x00;
    }
    item_ptr->used += li_len;
  }

  /*-------------------------------------------------------------------------
    The last segment of the previous SDU was exactly one octet short of 
    filling the previous PDU. This is valid only for 15-bit LIs.
  -------------------------------------------------------------------------*/
  else if (ctrl_blk->octet_less == TRUE)
  {
    ctrl_blk->octet_less = FALSE;
    /*-----------------------------------------------------------------------
      Set the LSB of the byte to 1, since there is a new LI and move the 
      data pointer to the next location.
    -----------------------------------------------------------------------*/
    **data_ptr |= 0x01;
    *data_ptr += 1;
    /*-----------------------------------------------------------------------
      This type of LI is only applied to the LI w/ length of 15 bits.
    -----------------------------------------------------------------------*/
    **data_ptr = 0xFF;
    *(++(*data_ptr)) = 0xF6;

    item_ptr->used += li_len;
  } 

  return li_len;
}

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
)
{
  dsm_item_type *item_ptr = NULL;    /* Pointer to head of packet          */
  dsm_item_type *temp_ptr = NULL;    /* Temporary Pointer                  */
  uint16        data_size = 0;       /* Size of data to pad                */

  item_ptr = *packet_ptr;
  if (item_ptr == NULL)
  {
    ERR("Null item",0,0,0);
    return ;
  }
  /*-------------------------------------------------------------------------
    Get the amount of data that has already been filled into the
    PDU and subtract it from the PDU size.
  -------------------------------------------------------------------------*/
  pdu_size -= item_ptr->used;
  while(item_ptr->pkt_ptr != NULL)
  {
    item_ptr = item_ptr->pkt_ptr;
    pdu_size -= item_ptr->used;
  }

  while (pdu_size > 0)
  {
    /*-----------------------------------------------------------------------
      If there is not enough space in the dsm item, get a new dsm item.
    -----------------------------------------------------------------------*/
    if ((item_ptr->size - item_ptr->used) == 0)
    {
      if ((temp_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) == NULL)
      {
        dsm_free_packet(packet_ptr);
        return ;
      }
      item_ptr->pkt_ptr = temp_ptr;
      item_ptr = item_ptr->pkt_ptr;
    }
    data_size = MIN(item_ptr->size - item_ptr->used, pdu_size);
    item_ptr->used += data_size;
    pdu_size -= data_size;
  }
  return;

}/* rlci_ul_insert_padding() */
    

