/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC TRANSMIT MODULE HEADER FILE
***
***
*** DESCRIPTION
***
***  RLC uplink transmit module header file
***
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlctx.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCTX_H
#define INC_GRLCTX_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlci.h"
#include "grlcutil.h"
#include "grlcultypes.h"
#include "grlculfifo.h"
#include "grlcdiag.h"
#include "grlclog.h"
#include "gl1rlc.h"
#include "grlcultst.h"
#include "grlcsgm.h"
#include "grlculproc.h"


/* Transmit array has x (SNS) elements each holding a data structure
** representing an uplink data block. This array contains the Tranmsit
** Window (between V(A) and V(S) for ack mode) and the Attach Window (between
** V(S) and seg_ind where the next segmented block is to be attached to the array)
*/
///HV-extern grlc_ul_tx_array_t  (*p_tx_arr)[];
extern grlc_ul_tx_array_t  (*(p_tx_arr[NUM_GERAN_DATA_SPACES]))[];

/*-------------------------------------------------------------------------------
** This 16-bit counter is incremented by PL1 every block period during uplink
** transfer. This is used to find the period between a block being confirmed as 
** transmitted by PL1 and when a packet uplink acknack is received.
** If the lapsed period is less than the expression (MAX(BS_CV_MAX,1)-1) then
** a '0' or Negative acknowledgement in the bitmap is disregarded.
**
** Ac = block counter value when receiving packet uplink acknack
** Bc = block counter value after block is transmitted
** expr = MAX(BS_CV_MAX,1)-1
**
** Disregard NACK if Ac - Bc < expr or,
**                if Ac < Bc + expr
** Inversely, NACK if Ac >= Bc + expr
**
** So after a block is confirmed as transmitted by PL1 the current value of 
** grlc_ul_block_counter plus 'expr' is assigned to that block. Determination of
** NACK later on will only involve a simple (even modulo) comparison
**-------------------------------------------------------------------------------
*/
extern uint16  grlc_ul_block_counter[NUM_GERAN_DATA_SPACES];

/*===========================================================================
===
===  FUNCTION      GRLC_UL_ROTATE_PS_MCS_CHANGE()
===
===  DESCRIPTION
===
===    Performs PS rotation on a data block given the current and new MCS.
===    Store updated PS value
===    See 04.60 Table 9.3.2.1.1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    None
===
===  SIDE EFFECTS
===
============================================================================*/
void grlc_ul_rotate_ps_mcs_change
(
  uint8 old_mcs,
  uint8 new_mcs,
  uint8 old_ps,
  uint8 *new_ps
);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DEMOTE_MCS_MODIFY_LI()
===
===  DESCRIPTION
===
===     Performs demotion of MCS via macro DEMOTE_MCS_FOR_TX( ) and then 
===     modify LI and EXT bits for the block as follows for PTX blocks:
===
===     If commanded MCS is MCS-8, then after demotion it is MCS-6. Now there 
===     are 6 empty octets available in MCS-6 block. This means changes are 
===     required in this block as follows for PTX blocks:
===     
===
===     [1]. If (TX_ARR(ind1).blk.ext == RM_EXT_NO_OCTET_FOLLOWS) and
===             (TX_ARR(ind1).blk.num_ext_octets == 0) then,
===
===            TX_ARR(ind1).blk.ext = RM_EXT_OCTET_FOLLOWS.
===            TX_ARR(ind1).blk.num_ext_octets = 1
===
===            TX_ARR(ind1).blk.ext_octet[n - 2].egprs.li = 
===              MCS-8 block size minus number of TLLI/PFI octets, if any.
===            TX_ARR(ind1).blk.ext_octet[n - 2].egprs.ext = 
===              RM_EXT_OCTET_FOLLOWS.
===
===            TX_ARR(ind1).blk.ext_octet[n - 1].egprs.li = 
===              RM_FILLER_OCTET_LEN.
===            TX_ARR(ind1).blk.ext_octet[n - 1].egprs.ext = 
===              RM_EXT_NO_OCTET_FOLLOWS.
===
===     [2]. Else 
===            Covers case (TX_ARR(ind1).blk.ext == RM_EXT_OCTET_FOLLOWS) and
===               (TX_ARR(ind1).blk.num_ext_octets >= 1) 
===
===             If ( TX_ARR(ind1).blk.ext_octet[n - 1].egprs.li != 
===                  RM_FILLER_OCTET_LEN) then, 
===
===                 Get total length octets in block including TLLI/PFI if any.
===                
===                 If (total octets in block == block size ) then,
===
===                   TX_ARR(ind1).blk.num_ext_octets + 1; say n,                    
===
===                   TX_ARR(ind1).blk.ext_octet[n - 2].egprs.ext = 
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(ind1).blk.ext_octet[n - 1].egprs.li = 
===                     RM_FILLER_OCTET_LEN.
===                   TX_ARR(ind1).blk.ext_octet[n - 1].egprs.ext = 
===                     RM_EXT_NO_OCTET_FOLLOWS.
===
===                 Else 
===                   Covers case (total octets in block != block size),
===
===                   TX_ARR(ind1).blk.num_ext_octets + 2; say n, 
===    
===                   TX_ARR(ind1).blk.ext_octet[n - 3].egprs.ext = 
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(ind1).blk.ext_octet[n - 2].egprs.li = 
===                     length of last PDU
===                   TX_ARR(ind1).blk.ext_octet[n - 2].egprs.ext = 
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(ind1).blk.ext_octet[n - 1].egprs.li = 
===                     RM_FILLER_OCTET_LEN.
===                   TX_ARR(ind1).blk.ext_octet[n - 1].egprs.ext = 
===                     RM_EXT_NO_OCTET_FOLLOWS.
===
===             Else 
===                do nothing as it is a copy over.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    None
===
===  SIDE EFFECTS
===
============================================================================*/
void grlc_ul_demote_mcs_modify_li( gas_id_t gas_id, uint16 ind );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_START_TIMER_T3182()
===
===  DESCRIPTION
===
===    Start timer T3182
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    None
===
===  SIDE EFFECTS
===
============================================================================*/
void grlc_ul_start_timer_T3182 ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      IS_PDU_TRANSMITTED()
===
===  DESCRIPTION    
===
===    Check and return boolean value if all data blocks belonging to the 
===    specified PSN haven been acknowledged as transmnitted by PL1
===
===  PARAMETERS
===
===    psn: PDU sequence number to be checked
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    TRUE or FALSE
===
===  SIDE EFFECTS
===
============================================================================*/
boolean is_pdu_transmitted
(
  gas_id_t  gas_id,
  uint16    psn
);

/*===========================================================================
===
===  FUNCTION      SEND_BLOCK_TO_PL1_FIFO()
===
===  DESCRIPTION
===
===    This function sends a radio block to the RLC UL-PL1 FIFO if the FIFO
===    is not full. All the fields in the MAC header are updated prior to 
===    sending. After segmentation each data block has its 'dsm_ptr' variable
===    pointing at the payload. If it sent out for the 1st time (PTX) then a 
===    duplicate of the payload is made and stored in dup_dsm_ptr for translating
===    into straight memory in the FIFO. After translation the dup_dsm_ptr is
===    freed. When a (PACK or NACK) block is re-transmitted another dup is made
===    again.
===
===    The status of the block being transmitted is not updated until the block
===    is confirmed as transmitted by the DSP (in post_tx_actions())
===
===    In un-ack mode, no blocks are re-transmitted unless it has CV = 0. So for
===    block with CV=0 we make a dup as per usual. For all other blocks we transfer
===    dsm_ptr to dup_dsm_ptr.
===    
===  ALGO  
===
===    if (fifo full)
===      if (is_block_present_in_fifo(bsn,ack_state))
===        set flag to prepare and put block
===        b_replace = TRUE -- expect delete follow by put
===      else
===        say fifo full
===      enfif
===    else
===      set flag to prepare and put block
===    endif
===
===    if (flag)
===      prepare dsm ptr
===      put blk (b_replace)
===    endif
===
===  PARAMS
===
===    ack_stat: ack status of the block to send to FIFO. Held in
===              TX_ARR(ind).vb except when in un-ack mode
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    TRUE if a block is sent to PL1 FIFO, FALSE when the FIFO is full.
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean send_block_to_pl1_fifo
( 
  gas_id_t        gas_id,
  uint8           ind,
  tx_ack_state_t  ack_state
);

/*===========================================================================
===
===  FUNCTION      E_SEND_BLOCK_TO_PL1_FIFO()
===
===  DESCRIPTION
===
===    This function sends a radio block to the RLC UL-PL1 FIFO if the FIFO
===    is not full. All the fields in the RLC header are updated prior to 
===    sending. Each data block carries its payload in 'dsm_ptr' after being 
===    segmented. A dup is made and used up (dup_dsm_ptr) on every transmission
===    and retransmission. Therefore the dup_dsm_ptr is transient and it is freed
===    after a transmission where as the dsm_ptr remains until a block is acknowledged.
===    The status of the block being transmitted is not updated until the block
===    is confirmed as transmitted by the DSP (in post_tx_actions())
===
===    In un-ack mode, no blocks are re-transmitted unless it has CV = 0. So for
===    block with CV=0 we make a dup as per usual. For all other blocks we transfer
===    dsm_ptr to dup_dsm_ptr.
===    
===  ALGO  
===
===    if (fifo full)
===      if (is_block_present_in_fifo(bsn,ack_state))
===        set flag to prepare and put block
===        b_replace = TRUE -- expect delete follow by put
===      else
===        say fifo full
===      enfif
===    else
===      set flag to prepare and put block
===    endif
===
===    if (flag)
===      prepare dsm ptr
===      put blk (b_replace)
===    endif
===
===  PARAMS
===
===    ack_stat: ack status of the block to send to FIFO. Held in
===              TX_ARR(ind).vb except when in un-ack mode
===
===  DEPENDENCIES
===
===  
===  RETURN VALUE
===
===    TRUE if a block is sent to PL1 FIFO, FALSE when the FIFO is full.
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean e_send_block_to_pl1_fifo
( 
  gas_id_t        gas_id,
  uint16          ind1,
  uint16          ind2,
  tx_ack_state_t  ack_state
);


/*===========================================================================
===
===  FUNCTION      ATTACH_BLOCK_TO_TX_ARRAY
===
===  DESCRIPTION
===
===    This fn takes the current rlc uplink data block in internal format
===    and attaches it to the attach segment of the transmit window.
===
===    Advances the value of seg_ind and assigns the block ack type with
===    TXA_PTX.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    return value written to module var cv.
===    dsm_ptr and dup_dsm_ptr made NULL after they are copied to TX_ARR().
===  
===========================================================================*/
void attach_block_to_tx_array( gas_id_t gas_id, grlc_ul_data_block_t *bp );

/*===========================================================================
===
===  FUNCTION      E_ATTACH_BLOCK_POST_INIT
===
===  DESCRIPTION
===
===    This fn takes the current rlc uplink data block in internal format
===    and attaches it to the attach segment of the transmit window.It also
===    then initialises the data block ready for the next 
===
===    Advances the value of seg_ind and assigns the block ack type with
===    TXA_PTX.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    return value written to module var cv.
===    dsm_ptr and dup_dsm_ptr made NULL after they are copied to TX_ARR().
===  
===========================================================================*/
void e_attach_block_post_init(gas_id_t gas_id, grlc_ul_data_block_t *bp);

/*===========================================================================
===
===  FUNCTION      TX_BLKS_SHELL
===
===  DESCRIPTION
===
===    This fn select the appropriate function to send data blocks in GPRS or
===    EDGE mode, in ack or un-ack mode to the FIFO
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===    None
===========================================================================*/
void tx_blks_shell ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      POST_TX_ACTIONS()
===
===  DESCRIPTION
===    
===    After NPL1 confirms to PL1 of a block having been transmitted RLC 
===    needs to perform a number of activities:
===    1. Handle T3182 uplink await acknack message in both ack/unack modes
===    2. If in ack mode, set block count value
===    3. Update ack status depending on its last ack status.
===    4. Update pointer to the next block to send to the FIFO depending on
===       the ack status of the block sent
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===  
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void post_tx_actions
(
  gas_id_t                          gas_id,
  const grlc_ul_fifo_access_block_t *blk_ptr
);

/*===========================================================================
===
===  FUNCTION      ACK_MODE_XFER()
===
===  DESCRIPTION
===
===    This function performs ack mode xfer in both one phase and two 
===    phase access modes. This is in response to either a 
===    PH_DATA_READY_TO_SEND primitive requesting RLC to refill the FIFO
===    or there are more blocks in the transmit windows
===
===    Possible exit states:
===    1.done all tx.
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    ul struct
===  
===  RETURN VALUE
===
===    ack_xfer_res_t: see def
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void ack_mode_xfer ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_ACK_MODE_XFER()
===
===  DESCRIPTION
===
===    This function performs ack mode xfer in both one phase and two 
===    phase access modes for EGPRS. This is in response to either a 
===    PH_DATA_READY_TO_SEND primitive requesting RLC to refill the FIFO
===    or there are more blocks in the transmit windows
===
===    Possible exit states:
===    1.done all tx.
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    ul struct
===  
===  RETURN VALUE
===
===    ack_xfer_res_t: see def
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void e_ack_mode_xfer ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      UNACK_MODE_XFER()
===
===  DESCRIPTION
===
===    Perform un-acknowledged mode transfer.
===
===    One phase acces is not used for un-ack mode. Hence TLLI field is 
===    never required in the data blocks.
===    
===  PARAMS
===
===
===  DEPENDENCIES
===
===    ul struct
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void unack_mode_xfer ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_UNACK_MODE_XFER()
===
===  DESCRIPTION
===
===    Perform un-acknowledged mode transfer in EGPRS TBF.
===
===    One phase acces is not used for un-ack mode. Hence TLLI field is 
===    never required in the data blocks.
===    
===  PARAMS
===
===
===  DEPENDENCIES
===
===    ul struct
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===  
===========================================================================*/
void e_unack_mode_xfer ( gas_id_t gas_id );

#endif /* INC_GRLCTX_H */

/*** EOF: don't remove! ***/
