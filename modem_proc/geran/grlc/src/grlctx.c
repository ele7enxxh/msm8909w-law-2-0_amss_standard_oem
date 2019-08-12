/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK TRANSMIT MODULE
***
***
*** DESCRIPTION
***
***  This module handles the transmission of data blocks in both acknowledged
***  and unacknowledged modes
***
***
*** EXTERNALIZED FUNCTIONS
***
*** boolean is_pdu_transmitted()
*** boolean send_block_to_pl1_fifo( gas_id_t gas_id )
*** boolean e_send_block_to_pl1_fifo( gas_id_t gas_id )
*** void attach_block_to_tx_array( gas_id_t gas_id, grlc_ul_data_block_t *bp)
*** void e_attach_block_post_init( gas_id_t gas_id, grlc_ul_data_block_t *bp)
*** void post_tx_actions( gas_id_t gas_id )
*** void ack_mode_xfer ( gas_id_t gas_id )
*** void e_ack_mode_xfer ( gas_id_t gas_id )
*** void unack_mode_xfer ( gas_id_t gas_id )
*** void e_unack_mode_xfer ( gas_id_t gas_id )
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlctx.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 25/11/03    hv     Created
*** 22/04/04    hv     Added EGPRS tx array
*** 09/19/12    hv     Added dual data space functionality
*** 23/10/12    hv     Debug code for CR 413204
*** 12/04/13    hv     CR467504
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
*** 20/08/13    hv     CR531564
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlctx.h"
#include "grlcsgm.h"
#include "grlculproc.h"
#include "grlculfifo.h"
#include "grlcultypes.h"
#include "grlcpaq.h"
#include "grlcdli.h"
#include "grlci.h"
#include "geran_msgs.h"
#include "geran_tasks.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/* RLC UL private data types and definitions are defined in gRLCulTypes.h
*/

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* Transmit array has x (SNS) elements each holding a data structure
** representing an uplink data block. This array contains the Tranmsit
** Window (between V(A) and V(S) for ack mode) and the Attach Window (between
** V(S) and seg_ind where the next segmented block is to be attached to the array)
*/

/* Pointer to transmit array - used for both GPRS and EGPRS modes
** p_tx_arr is a two dimensional arrays of pointers to transmit element structures.
*/
///HV-grlc_ul_tx_array_t  (*p_tx_arr[NUM_GERAN_DATA_SPACES])[];
grlc_ul_tx_array_t  (*(p_tx_arr[NUM_GERAN_DATA_SPACES]))[];

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
uint16  grlc_ul_block_counter[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      INDICATE_N3104_EXPIRY()
===
===  DESCRIPTION
===
===    Indicate N3104 expiry to MAC
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
============================================================================*/
static void indicate_n3104_expiry (gas_id_t gas_id)
{
  /* pointer to buffer holding message intended for MAC
  */
  rlc_mac_msg_t   *rlc_msg_ptr;

  /*-------------------------------------------------------*/

  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    rlc_msg_ptr->msg.ul_n3104_expiry_ind.sig = RM_UL_N3104_EXPIRY_IND;
    rlc_msg_ptr->msg.ul_n3104_expiry_ind.gas_id = gas_id;
    mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
  }
} /* indicate_n3104_expiry */

/*===========================================================================
===
===  FUNCTION      IS_ANY_NACKED_BLKS()
===
===  DESCRIPTION
===
===   Return the TRUE if a nacked block exists in the tx window otherwise
===   return FALSE
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean is_any_nacked_blks ( gas_id_t gas_id )
{
  byte b, ind;
  byte tx_win_size;

  /* quick return for the normal case
  */
  if (ul[gas_id].va == ul[gas_id].vs)
    return FALSE;

  tx_win_size = (byte)GET_TX_WINDOW_SIZE(gas_id);

  for (b = 0; b < tx_win_size; b++)
  {
    ind = MOD_SNS(ul[gas_id].va+b);

    /* return on the 1st Nacked block found
    */
    if (TX_ARR(gas_id, ind).vb == TXA_NACK)
      return TRUE;
  }

  return FALSE;
}

/*===========================================================================
===
===  FUNCTION      SEEK_BLOCK_TYPE()
===
===  DESCRIPTION
===
===   Seek the given 'block-type' in the e-tx-array range from 'start' to 'end-1'.
===   Return the BSN if the found block or E_GRLC_INVALID_BSN(0xffff)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
static uint16 seek_block_type
(
  gas_id_t        gas_id,
  uint16          start,
  uint16          end,
  tx_ack_state_t  block_type
)
{
  register uint16  u, ind;

  /* quick return for the normal case
  */
  if (start == end)
    return E_GRLC_INVALID_BSN;

  for (u = 0; u < MOD_DIF_ESNS(end, start); u++)
  {
    ind = MOD_ESNS(start+u);

    /* return on the 1st block found
    */
    if (TX_ARR(gas_id, ind).vb == block_type)
      return ind;
  }

  return E_GRLC_INVALID_BSN;
} /* seek_block_type */

/*===========================================================================
===
===  FUNCTION      e_find_2nd_payload()
===
===  DESCRIPTION
===
===   Seek the given 'block-type' and MCS in the e-tx-array range from 'start' to 'end-1'
===   Return the BSN if the found block or E_GRLC_INVALID_BSN(0xffff)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
static uint16 e_find_2nd_payload
(
  gas_id_t        gas_id,
  uint16          start,
  uint16          end,
  uint8           bsn1_mcs,
  tx_ack_state_t  block_type
)
{
  register uint16  u, ind, range;
  /* Two variables to satisfy KW */
  uint8            bsn2_mcs;
  uint8            init_mcs;

  /* Forced to use intermediate temp var to show Klocwork that cmd_mcs was already ranged checked.
  ** Clocwork throws a warning if ul[gas_id].cur_res.cs is used directly
  */
  uint8 cmd_mcs = ul[gas_id].cur_res.cs;

  /* quick return for the normal case
  */
  if (start == end)
    return E_GRLC_INVALID_BSN;

  range = MOD_DIF_ESNS(end, start);

  for (u = 0; u < range; u++)
  {
    ind = MOD_ESNS(start+u);

    init_mcs = TX_ARR(gas_id, ind).blk.init_mcs;

    if ((init_mcs <= RM_MCS_9) && (cmd_mcs <= RM_MCS_6_9 ))
    {
      bsn2_mcs = GET_MCS_FOR_RETX( init_mcs, cmd_mcs, ul[gas_id].cur_res.resegment);
    }
    else
    {
      MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_358, "GRLU init_mcs %u or cur_res.cs %u is oor",init_mcs, cmd_mcs);
      return E_GRLC_INVALID_BSN;
    }

    /* Return on the 1st block found that matches the block type and the MCS
    ** of the first payload
    */
    if ( (TX_ARR(gas_id, ind).vb == block_type) &&
         (bsn2_mcs == bsn1_mcs)
       )
    {
      return ind;
    }
    else if( TX_ARR(gas_id, ind).vb == block_type)
      return E_GRLC_INVALID_BSN;
  }
#if 0
  if( block_type == TXA_PACK )
  {
    range = MOD_DIF_ESNS(start, ul[gas_id].va);

    /* Wrap around, starting from V(A), to search
    ** for any NACK blocks that have become PACK blocks before you return.
    ** Note that "start" now becomes "end" in the MOD_DIF_ESNS
    */
    for (u = 0; u < range; u++)
    {
      ind = MOD_ESNS(ul[gas_id].va + u);

      bsn2_mcs = GET_MCS_FOR_RETX( TX_ARR(gas_id, ind).blk.init_mcs, cmd_mcs, ul[gas_id].cur_res.resegment);

      /* Return on the 1st block found that matches the block type and the MCS of
      ** the first payload
      */
      if ( (TX_ARR(gas_id, ind).vb == block_type) &&
           (bsn2_mcs == bsn1_mcs)
         )
      {
        return ind;
      }
      else if( TX_ARR(gas_id, ind).vb == block_type)
        return E_GRLC_INVALID_BSN;
    }
  }
#endif
  return E_GRLC_INVALID_BSN;
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

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
void grlc_ul_start_timer_T3182 ( gas_id_t gas_id )
{
  (void)rex_set_timer(&(ul[gas_id].timer_t3182.rex_timer), RM_TMR_T3182_DUR);
  ul[gas_id].timer_t3182.state = TMR_ACTIVE;
} /* grlc_ul_start_timer_T3182 */


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
)
{
  if( old_mcs == RM_MCS_9 && new_mcs == RM_MCS_6 )
  {
    if( old_ps == GRLC_PS_1 || old_ps == GRLC_PS_3 )
      *new_ps = GRLC_PS_1;
    else if( old_ps == GRLC_PS_2 )
      *new_ps = GRLC_PS_2;
    else
      *new_ps = GRLC_PS_1;
  }
  else if( old_mcs == RM_MCS_6 && new_mcs == RM_MCS_9 )
  {
    if( old_ps == GRLC_PS_1 )
      *new_ps = GRLC_PS_3;
    else if( old_ps == GRLC_PS_2 )
      *new_ps = GRLC_PS_2;
    else
      *new_ps = GRLC_PS_1;
  }
  else if( old_mcs == RM_MCS_5 && new_mcs == RM_MCS_7 )
  {
    *new_ps = GRLC_PS_2;
  }
  else
  {
    *new_ps = GRLC_PS_1;
  }
} /* grlc_ul_rotate_ps_mcs_change */

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
===     [1]. If (TX_ARR(gas_id, ind1).blk.ext == RM_EXT_NO_OCTET_FOLLOWS) and
===             (TX_ARR(gas_id, ind1).blk.num_ext_octets == 0) then,
===
===            TX_ARR(gas_id, ind1).blk.ext = RM_EXT_OCTET_FOLLOWS.
===            TX_ARR(gas_id, ind1).blk.num_ext_octets = 1
===
===            TX_ARR(gas_id, ind1).blk.ext_octet[n - 2].egprs.li =
===              MCS-8 block size minus number of TLLI/PFI octets, if any.
===            TX_ARR(gas_id, ind1).blk.ext_octet[n - 2].egprs.ext =
===              RM_EXT_OCTET_FOLLOWS.
===
===            TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.li =
===              RM_FILLER_OCTET_LEN.
===            TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.ext =
===              RM_EXT_NO_OCTET_FOLLOWS.
===
===     [2]. Else
===            Covers case (TX_ARR(gas_id, ind1).blk.ext == RM_EXT_OCTET_FOLLOWS) and
===               (TX_ARR(gas_id, ind1).blk.num_ext_octets >= 1)
===
===             If ( TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.li !=
===                  RM_FILLER_OCTET_LEN) then,
===
===                 Get total length octets in block including TLLI/PFI if any.
===
===                 If (total octets in block == block size ) then,
===
===                   TX_ARR(gas_id, ind1).blk.num_ext_octets + 1; say n,
===
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 2].egprs.ext =
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.li =
===                     RM_FILLER_OCTET_LEN.
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.ext =
===                     RM_EXT_NO_OCTET_FOLLOWS.
===
===                 Else
===                   Covers case (total octets in block != block size),
===
===                   TX_ARR(gas_id, ind1).blk.num_ext_octets + 2; say n,
===
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 3].egprs.ext =
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 2].egprs.li =
===                     length of last PDU
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 2].egprs.ext =
===                     RM_EXT_OCTET_FOLLOWS.
===
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.li =
===                     RM_FILLER_OCTET_LEN.
===                   TX_ARR(gas_id, ind1).blk.ext_octet[n - 1].egprs.ext =
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

void grlc_ul_demote_mcs_modify_li( gas_id_t gas_id, uint16 ind )
{
  uint8 ext_oct_cnt = 0;
  uint8 pre_demotion_mcs = 0;

  /* Store pre-demotion mcs. */
  pre_demotion_mcs = TX_ARR(gas_id,  ind ).blk.cs;

  /* TX_ARR(gas_id, tx_arr_index ).blk.cs will contain demoted MCS after call to macro
  ** below.
  */
  DEMOTE_MCS_FOR_TX( TX_ARR(gas_id,  ind ).blk.cs );

  /* Modify Length Indicator(s) as approriate. */
  if ( ( TX_ARR(gas_id, ind).vb     == TXA_PTX  ) &&
       ( TX_ARR(gas_id, ind).blk.cs == RM_MCS_6 ) &&
       ( pre_demotion_mcs   == RM_MCS_8 )
     )
  {
    /* Precise fit and no ext octets in MCS-8 block. */
    if ( ( TX_ARR(gas_id, ind).blk.ext == RM_EXT_NO_OCTET_FOLLOWS ) &&
         ( TX_ARR(gas_id, ind).blk.num_ext_octets == 0            )
       )
    {
      /* 2 new ext octets required. */
      TX_ARR(gas_id, ind).blk.num_ext_octets = ( TX_ARR(gas_id, ind).blk.num_ext_octets + 2 );
      TX_ARR(gas_id, ind).blk.ext = RM_EXT_OCTET_FOLLOWS;
      ext_oct_cnt = TX_ARR(gas_id, ind).blk.num_ext_octets;

      /* First ext octet. */
      TX_ARR(gas_id, ind).blk.ext_octet[0].egprs.li  = TX_ARR(gas_id, ind).blk.data_len;
      TX_ARR(gas_id, ind).blk.ext_octet[0].egprs.ext = RM_EXT_OCTET_FOLLOWS;

      /* Second ext octet. */
      TX_ARR(gas_id, ind).blk.ext_octet[1].egprs.li  = RM_FILLER_OCTET_LEN;
      TX_ARR(gas_id, ind).blk.ext_octet[1].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    }
    /* One or more ext octet in MCS-8. */
    else
    {
      ext_oct_cnt = TX_ARR(gas_id, ind).blk.num_ext_octets;

      /* Last ext octet LI != 127. */
      if ( TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 1].egprs.li !=
           RM_FILLER_OCTET_LEN )
      {
        uint8 total_blk_len = 0;

        /* Add ext oct count to total length of the block. */
        total_blk_len = TX_ARR(gas_id, ind).blk.data_len +
                        TX_ARR(gas_id, ind).blk.num_ext_octets;

        /* Add length of TLLI and PFI octets, if present. */
        if ( TX_ARR(gas_id, ind).blk.ti == RM_TLLI_PRESENT )
        {
          total_blk_len = total_blk_len + RM_TLLI_LEN;

          if( TX_ARR(gas_id, ind).blk.hdr.pi == RM_PFI_PRESENT )
          {
            total_blk_len = total_blk_len + RM_PFI_E_LEN;
          }
        }

        /* If it is a precise fit for MCS-8, then add a filler length
        ** octet as last LI.
        */
        if ( total_blk_len == E_GRLC_DATA_BLOCK_LEN[ RM_MCS_8 ] )
        {
          /* Add 1 to ext octet cnt */
          TX_ARR(gas_id, ind).blk.num_ext_octets = (TX_ARR(gas_id, ind).blk.num_ext_octets + 1);
          ext_oct_cnt = TX_ARR(gas_id, ind).blk.num_ext_octets;

          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 2].egprs.ext =
            RM_EXT_OCTET_FOLLOWS;

          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 1].egprs.li =
            RM_FILLER_OCTET_LEN;
          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 1].egprs.ext =
            RM_EXT_NO_OCTET_FOLLOWS;
        }
        /* Not a precise fit. */
        else
        {
          /* Add 2 to ext octet cnt */
          TX_ARR(gas_id, ind).blk.num_ext_octets = (TX_ARR(gas_id, ind).blk.num_ext_octets + 2);
          ext_oct_cnt = TX_ARR(gas_id, ind).blk.num_ext_octets;

          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 3].egprs.ext =
            RM_EXT_OCTET_FOLLOWS;

          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 2].egprs.li =
            (E_GRLC_DATA_BLOCK_LEN[ RM_MCS_8 ] - total_blk_len );
          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 2].egprs.ext =
            RM_EXT_OCTET_FOLLOWS;

          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 1].egprs.li =
            RM_FILLER_OCTET_LEN;
          TX_ARR(gas_id, ind).blk.ext_octet[ext_oct_cnt - 1].egprs.ext =
            RM_EXT_NO_OCTET_FOLLOWS;
        }
      }
      /* Else, i.e. last ext octet LI = 127. Do nothing in this case. Straight
      ** copy over of the block from MCS8 to MCS6.
      */
    }
  }
} /* grlc_ul_demote_mcs_modify_li */



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
)
{
  uint16  bsn;  /* last block transmitted */

  bsn = ul[gas_id].vs;

  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_359, "GRLU pdu-txed psn=%u vs=%u seg_ind=%u",
           psn,
           ul[gas_id].vs,
           ul[gas_id].seg_ind
          );

  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    DEC_MOD(bsn, ESNS);
    /* Ensure BSN is in a sensible range
    */
    if (IS_GE_MODULO_COMP(ul[gas_id].seg_ind, bsn, ESNS) &&
        IS_GE_MODULO_COMP(bsn, ul[gas_id].va, ESNS)
       )
    {
      if (IS_GT_MODULO_COMP(TX_ARR(gas_id, bsn).blk.last_psn, psn, 32768))
      {
        MSG_GRLC_EXT_MED_2_G(GRLC_F3_MSG_ENUM_360, "GRLU pdu-txed TRUE bsn=%u last_psn=%u",
                bsn,
                TX_ARR(gas_id, bsn).blk.last_psn
               );

        return TRUE;
      }
      else if (TX_ARR(gas_id, bsn).blk.last_psn == psn)
      {
        /* check for last block
        */
        if (TX_ARR(gas_id, bsn).blk.hdr.cv == 0)
        {
          MSG_GRLC_EXT_MED_0_G(GRLC_F3_MSG_ENUM_361, "GRLU pdu-txed TRUE cv=0");
          return TRUE;
        }

        /* IN EGPRS - pdu can be transmitted...
        ** 1. when it fits the rlc block with L1
        ** 2. when its a precise fit and next bsn has LI=0
        */
        if ((IS_GT_MODULO_COMP(ul[gas_id].seg_ind, ul[gas_id].vs, ESNS) &&
             IS_GT_MODULO_COMP(TX_ARR(gas_id, (bsn+1)%ESNS).blk.first_psn, psn, 32768)
            )
           )
        {
          if(IS_E_DATA_BLOCK_PRECISE_FIT(gas_id, (bsn+1) % ESNS))
          {
            MSG_GRLC_EXT_MED_0_G(GRLC_F3_MSG_ENUM_362, "GRLU pdu-txed FALSE - will fit in next bsn with LI=0");
            return FALSE;
          }
          else
          {
            MSG_GRLC_EXT_MED_1_G(GRLC_F3_MSG_ENUM_363, "GRLU pdu-txed TRUE psn=%d fits with L1",psn);
            return TRUE;
          }
        }
      }
    }
  }
  else if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS)
  {
    DEC_MOD(bsn, SNS);
    if (IS_GE_MODULO_COMP(ul[gas_id].seg_ind, bsn, SNS) &&
        IS_GE_MODULO_COMP(bsn, ul[gas_id].va, SNS)
       )
    {
      if (IS_GT_MODULO_COMP(TX_ARR(gas_id, bsn).blk.last_psn, psn, 32768))
      {
        MSG_GRLC_EXT_MED_2_G(GRLC_F3_MSG_ENUM_364, "GRLU pdu-txed TRUE bsn=%u last_psn=%u",
                bsn,
                TX_ARR(gas_id, bsn).blk.last_psn
               );
        return TRUE;
      }
      else if (TX_ARR(gas_id, bsn).blk.last_psn == psn)
      {
        /* check for last block
        */
        if (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0 ||
            (IS_GT_MODULO_COMP(ul[gas_id].seg_ind, ul[gas_id].vs, SNS) &&
             IS_GT_MODULO_COMP(TX_ARR(gas_id, (bsn+1)%SNS).blk.first_psn, psn, 32768)
            )
           )
        {
          MSG_GRLC_EXT_MED_0_G(GRLC_F3_MSG_ENUM_365, "GRLU pdu-txed TRUE cv=0 or good fit");
          return TRUE;
        }
      }
    }
  }
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_366, "GRLU pdu-txed bad tbf_mode");
  }

  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_367, "GRLU pdu-txed FALSE ",
          psn,
          ul[gas_id].vs,
          ul[gas_id].seg_ind
         );

  return FALSE;

} /* is_pdu_transmitted */

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
===              TX_ARR(gas_id, ind).vb except when in un-ack mode
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
)
{
  grlc_ul_fifo_op_result_t  put_res;
  uint16                    u;

  /*---------------------------------------------------------------*/

  if (grlc_ul_fifo_is_area_full(gas_id, ack_state))
  {
    return FALSE;
  }
  GRLC_UL_SHOW_BLOCK_SENT(gas_id, ind, ack_state);

  /*====================================================================
  ** Refresh the fields in MAC header before sending block to the FIFO
  **====================================================================
  */
  TX_ARR(gas_id, ind).blk.mac_hdr.type.ul_data.retry = ul[gas_id].cur_res.retry;

  /*
  **stall indicator should not be set of a PTX block but for PACK blocks only.
  */
  if(ack_state != TXA_PTX)
  {
    TX_ARR(gas_id, ind).blk.mac_hdr.type.ul_data.si = ul[gas_id].si;
  }

  /*====================================================================
  ** Refresh the TFI field in RLC header before sending block to the FIFO
  **====================================================================
  */
  TX_ARR(gas_id, ind).blk.tfi = ul[gas_id].cur_res.ul_tfi;

  /*=====================================================================
  ** Create/Duplicate DSM items to send a block to the FIFO in ACK mode
  **=====================================================================
  */
  if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
  {
    /*--------------------------------------------------------------------------
    ** If the block is to be transmitted for the 1st time, make a 'dup' version
    ** of the data and store it in dup_dsm_ptr and transmit the dup_dsm_ptr.
    ** However, dup_dsm_ptr could be prepared and then not put succesfully. So
    ** we only dup when it is NULL
    **--------------------------------------------------------------------------
    */
    if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
    {
      TX_ARR(gas_id, ind).blk.dup_dsm_ptr =
        gprs_pdu_duplicate(TX_ARR(gas_id, ind).blk.dsm_ptr, 0, TX_ARR(gas_id, ind).blk.data_len);
      if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
      {
        MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_368, "GRLU A ##Dup Exhaustion %d vb=%d ofs=%u",
                    ind,
                    TX_ARR(gas_id, ind).vb,
                    TX_ARR(gas_id, ind).blk.offset);
        u = (uint16)GPRS_PDU_LENGTH(TX_ARR(gas_id, ind).blk.dsm_ptr);
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_369, "dsm_ptr=%x a_len=%u dup=%x",
                  TX_ARR(gas_id, ind).blk.dsm_ptr,
                  u,
                  TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
        grlc_ul_show_dsm_info(gas_id);
        grlc_ul_show_vars(gas_id);

        /* Failed to create dup'ed packet. Set flag to release at the appro fn level
        */
        ul[gas_id].b_error_recovery_required = TRUE;
        ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
        return FALSE;
      }
    }
  }
  else
  /*======================================================================
  ** Create duplicate DSM item to send block to the FIFO in UN-ACK mode
  **======================================================================
  */
  {
    /* if block has CV = 0 then we must save a copy in case we need to
    ** re-send
    */
    if (TX_ARR(gas_id, ind).blk.mac_hdr.type.ul_data.cv == 0)
    {
      /* Save block with CV = 0 in dup_dsm_ptr if it's not been saved already
      */

      /* If not NULL then free item and flag error
      */
      if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr != NULL)
      {
        MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_370, "GRLU U ##dup not NULL bsn=%d",ind);
        GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
      }

      if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
      {
        TX_ARR(gas_id, ind).blk.dup_dsm_ptr =
          gprs_pdu_duplicate(TX_ARR(gas_id, ind).blk.dsm_ptr, 0, TX_ARR(gas_id, ind).blk.data_len);
        ul[gas_id].b_cv_0 = TRUE;

        if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
        {
          MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_371, "GRLU U ##Dup Exhaustion %d vb=%d ofs=%u",
                    ind,
                    TX_ARR(gas_id, ind).vb,
                    TX_ARR(gas_id, ind).blk.offset);
          u = (uint16)GPRS_PDU_LENGTH(TX_ARR(gas_id, ind).blk.dsm_ptr);
          MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_372, "dsm_ptr=%x a_len=%u dup=%x",
                    TX_ARR(gas_id, ind).blk.dsm_ptr,
                    u,
                    TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
          grlc_ul_show_dsm_info(gas_id);
          grlc_ul_show_vars(gas_id);

          /* Failed to create dup'ed packet. Set flag to release at the appro fn level
          */
          ul[gas_id].b_error_recovery_required = TRUE;
          ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
          return FALSE;
        }
      }
    } /* end block with CV = 0 */
    else
    {
      /* Block has CV != 0. dsm_ptr holds the payload and dup_dsm_ptr is NULL.
      ** If this is the 1st transmit attempt then transfer dsm_ptr into dup_dsm_ptr
      ** and make dsm_ptr NULL. However, the put may not be succesful and on the
      ** subsequent attempts, check if the payload is already in dup_dsm_ptr
      */
      if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
      {
        TX_ARR(gas_id, ind).blk.dup_dsm_ptr = TX_ARR(gas_id, ind).blk.dsm_ptr;
        TX_ARR(gas_id, ind).blk.dsm_ptr = NULL;
      }
    } /* end block CV not 0 */
  } /* un-ack mode */

  /*======================================================================
  ** Put the block into the FIFO
  **======================================================================
  */
  put_res = grlc_ul_fifo_put_block(gas_id, &(TX_ARR(gas_id, ind).blk), ack_state);
  if (put_res != UL_FIFO_PUT_OK)
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_373, "GRLU send_b bad put result %d bsn=%d",put_res, ind);

    /* When this happens dsm_ptr will have been prepared but not consumed.
    ** This will be detected the next attempt
    */
    return FALSE;
  }

  /*=====================================================================
  ** Inform PL1 to start polling the FIFO for data blocks by sending the
  ** PH_DATA_REQ primitive on the very first data block of a TBF
  **=====================================================================
  */
  if (ul[gas_id].b_first_fifo_load)
  {
    pl1_send_ph_data_req(gas_id);
    grlc_ul_fifo_enable_pl1_access(gas_id);

    ul[gas_id].b_first_fifo_load = FALSE;

    if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      /* trigger block timers servicing
      */
      b_block_timers_service_active[gas_id] = TRUE;
    }
    else
    {
      /* ensure timer service is not active in un-ack mode
      */
      b_block_timers_service_active[gas_id] = FALSE;
    }
  }

  return TRUE;

} /* send_block_to_pl1_fifo */

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
===              TX_ARR(gas_id, ind).vb except when in un-ack mode
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
)
{
  grlc_ul_fifo_op_result_t  put_res;
  uint16                    u,ind;
  uint8                     payload = 0;

  /*---------------------------------------------------------------*/

  if (grlc_ul_fifo_is_area_full(gas_id, ack_state))
  {
    return FALSE;
  }

  /* Deal with the first payload */
  ind = ind1;

  while( payload < 2 )
  {
#ifdef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO
    grlc_ul_show_block_sent(gas_id, ind,TX_ARR(gas_id, ind).vb);
#endif
    /*====================================================================
    ** Refresh the fields in MAC header before sending block to the FIFO
    **====================================================================
    */
    TX_ARR(gas_id, ind).blk.hdr.retry = ul[gas_id].cur_res.retry;

    /*
    ** stall indicator should not be set of a PTX block but for PACK blocks only.
    */
    if(ack_state != TXA_PTX)
    {
      TX_ARR(gas_id, ind).blk.hdr.si = ul[gas_id].si;
    }

    /*====================================================================
    ** Refresh the TFI field in RLC header before sending block to the FIFO
    **====================================================================
    */
    TX_ARR(gas_id, ind).blk.hdr.tfi = ul[gas_id].cur_res.ul_tfi;

    /*=====================================================================
    ** Create/Duplicate DSM items to send a block to the FIFO in ACK mode
    **=====================================================================
    */
    if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      /*--------------------------------------------------------------------------
      ** If the block is to be transmitted for the 1st time, make a 'dup' version
      ** of the data and store it in dup_dsm_ptr and transmit the dup_dsm_ptr.
      ** However, dup_dsm_ptr could be prepared and then not put succesfully. So
      ** we only dup when it is NULL
      **--------------------------------------------------------------------------
      */
      if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
      {
        TX_ARR(gas_id, ind).blk.dup_dsm_ptr =
          gprs_pdu_duplicate(TX_ARR(gas_id, ind).blk.dsm_ptr, 0, TX_ARR(gas_id, ind).blk.data_len);
        if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
        {
          MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_374, "GRLU A Potential Dup Exhaustion %d vb=%d ofs=%u",
                    ind,
                    TX_ARR(gas_id, ind).vb,
                    TX_ARR(gas_id, ind).blk.offset
                   );
          u = (uint16)GPRS_PDU_LENGTH( TX_ARR(gas_id, ind).blk.dsm_ptr );
          MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_375, "dsm_ptr=%x a_len=%u dup=%x",
                   TX_ARR(gas_id, ind).blk.dsm_ptr,
                   u,
                   TX_ARR(gas_id, ind).blk.dup_dsm_ptr
                  );

          /* Dump other vars
          */
          grlc_ul_show_vars(gas_id);

#if 1 /* debug */
          /* Show details of 10 elements of the e_tx array around the BSN in question
          */
          u = MOD_ESNS (ind-5);
          for ( u = MOD_ESNS (ind-5);
                u < MOD_ESNS (ind+5);
                u++
              )
          {
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_376, "BSN=%u vb=%u data_len=%u",
                      u,
                      TX_ARR(gas_id, u).vb,
                      TX_ARR(gas_id, u).blk.data_len
                    );
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_377, " dsm_ptr=%p dup=%p rem_cnt=%u",
                      TX_ARR(gas_id, u).blk.dsm_ptr,
                      TX_ARR(gas_id, u).blk.dup_dsm_ptr,
                      TX_ARR(gas_id, u).blk.rem_cnt
                    );
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_378, " mcs=%u ptx_miss_cnt=%u nack_miss_cnt=%u",
                      TX_ARR(gas_id, u).blk.cs,
                      TX_ARR(gas_id, u).blk.ptx_miss_cnt,
                      TX_ARR(gas_id, u).blk.nack_miss_cnt
                    );
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_379, " 1st_psn=%u last_psn=%u rlc_mode=%u",
                      TX_ARR(gas_id, u).blk.first_psn,
                      TX_ARR(gas_id, u).blk.last_psn,
                      ul[gas_id].rlc_mode
                    );
          }
#endif /* debug */

          /* Failed to create dup'ed packet. Set flag to release at the appro fn level
          */
          ul[gas_id].b_error_recovery_required = TRUE;
          ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
          return FALSE;
        }
      }
    }
    else
    /*======================================================================
    ** Create duplicate DSM item to send block to the FIFO in UN-ACK mode
    **======================================================================
    */
    {
      /* if block has CV = 0 then we must save a copy in case we need to
      ** re-send
      */
      if (TX_ARR(gas_id, ind).blk.hdr.cv == 0)
      {
        /* Save block with CV = 0 in dup_dsm_ptr if it's not been saved already
        */

        /* If not NULL then free item and flag error
        */
        if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr != NULL)
        {
          MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_380, "GRLU U dup not NULL bsn=%d",ind);
          GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
        }

        if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
        {
          TX_ARR(gas_id, ind).blk.dup_dsm_ptr =
            gprs_pdu_duplicate(TX_ARR(gas_id, ind).blk.dsm_ptr, 0, TX_ARR(gas_id, ind).blk.data_len);
          ul[gas_id].b_cv_0 = TRUE;

          if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
          {
            MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_381, "GRLU U Potential Dup Exhaustion %d vb=%d ofs=%u",
                      ind,
                      TX_ARR(gas_id, ind).vb,
                      TX_ARR(gas_id, ind).blk.offset);
            u = (uint16)GPRS_PDU_LENGTH(TX_ARR(gas_id, ind).blk.dsm_ptr);
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_382, "dsm_ptr=%x a_len=%u dup=%x",
                      TX_ARR(gas_id, ind).blk.dsm_ptr,
                      u,
                      TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_383, "dup free = %u dp=%x gas_id",
                      DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
                      TX_ARR(gas_id, ind).blk.dsm_ptr,gas_id);
            grlc_ul_show_vars(gas_id);

            /* Failed to create dup'ed packet. Set flag to release at the appro fn level
            */
            ul[gas_id].b_error_recovery_required = TRUE;
            ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
            return FALSE;
          }
        }
      } /* end block with CV = 0 */
      else
      {
        /* Block has CV != 0. dsm_ptr holds the payload and dup_dsm_ptr is NULL.
        ** If this is the 1st transmit attempt then transfer dsm_ptr into dup_dsm_ptr
        ** and make dsm_ptr NULL. However, the put may not be succesful and on the
        ** subsequent attempts, check if the payload is already in dup_dsm_ptr
        */
        if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr == NULL)
        {
          TX_ARR(gas_id, ind).blk.dup_dsm_ptr = TX_ARR(gas_id, ind).blk.dsm_ptr;
          TX_ARR(gas_id, ind).blk.dsm_ptr = NULL;
        }
      } /* end block CV not 0 */
    } /* un-ack mode */

    if( (TX_ARR(gas_id, ind).blk.cs == RM_MCS_7  ||
         TX_ARR(gas_id, ind).blk.cs == RM_MCS_8  ||
         TX_ARR(gas_id, ind).blk.cs == RM_MCS_9 )&&
         (ind2 != 0xffff)
      )
    {
      ind = ind2; /* deal with the 2nd payload */
      payload++;
    }
    else
      break; /* Single payload block. Exit loop */

  } /* end while() */

  /*======================================================================
  ** Put the block into the FIFO
  **======================================================================
  */
  if( payload )
    put_res = e_grlc_ul_fifo_put_block(gas_id, &(TX_ARR(gas_id, ind1).blk),
                                       &(TX_ARR(gas_id, ind2).blk),
                                       ack_state);
  else
    put_res = e_grlc_ul_fifo_put_block(gas_id, &(TX_ARR(gas_id, ind1).blk), NULL, ack_state);

  if (put_res != UL_FIFO_PUT_OK)
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_384, "GRLU send_b bad put result %d bsn=%d",put_res, ind);

    /* When this happens dsm_ptr will have been prepared but not consumed.
    ** This will be detected the next attempt
    */
    return FALSE;
  }

  /*=====================================================================
  ** Inform PL1 to start polling the FIFO for data blocks by sending the
  ** PH_DATA_REQ primitive on the very first data block of a TBF
  **=====================================================================
  */
  if (ul[gas_id].b_first_fifo_load)
  {
    pl1_send_ph_data_req(gas_id);
    grlc_ul_fifo_enable_pl1_access(gas_id);

    ul[gas_id].b_first_fifo_load = FALSE;

    if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      /* trigger block timers servicing
      */
      b_block_timers_service_active[gas_id] = TRUE;
    }
    else
    {
      /* ensure timer service is not active in un-ack mode
      */
      b_block_timers_service_active[gas_id] = FALSE;
    }
  }

  return TRUE;

}

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
===    dsm_ptr and dup_dsm_ptr made NULL after they are copied to TX_ARR(gas_id, ).
===
===========================================================================*/
void attach_block_to_tx_array(gas_id_t gas_id, grlc_ul_data_block_t *bp)
{
  /* Check and warn before we over-write an item
  */
  if (TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr != NULL)
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_385, "GRLU ##atta_blk[%d] dsm not NULL 0x%x",
              ul[gas_id].seg_ind,
              TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr
              );
    GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr);
  }

  /* Check and warn before we over-write an item
  */
  if (TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr != NULL)
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_386, "GRLU ##atta_blk[%d] dup not NULL 0x%x",
              ul[gas_id].seg_ind,
              TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr
              );
    GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr);
  }

  /* copy of the data block in internal format to attach window
  */
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk = *bp;

  /* update the cv as block is now FULLY segmented */
  ul[gas_id].cv = (bp)->mac_hdr.type.ul_data.cv;

  /* Once copied the dsm_ptr and dup_dsm_ptr must be cleared
  */
  bp->dsm_ptr = NULL;
  bp->dup_dsm_ptr = NULL;

  /* Use TXA_PTX for ack mode and TXA_UNACK for un-ack mode
  */
  if (cur_pdu_ptr[gas_id]->rlc_mode == GRLC_MODE_ACK)
    TX_ARR(gas_id, ul[gas_id].seg_ind).vb = TXA_PTX;
  else
    TX_ARR(gas_id, ul[gas_id].seg_ind).vb = TXA_UNACK;

  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.ptx_miss_cnt = 0;

  /* Mark block as part of the current PSN
  */
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.last_psn = cur_pdu_ptr[gas_id]->psn;
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.bsn = ul[gas_id].seg_ind;

  grlc_ul_diag_block_stat_add_entry(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk, gas_id);

#ifdef  GSM_GPRS_SHOW_FIRST_LAST_PSNS
  MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_387, "GRLU atta bsn=%d 1st_psn=%u last_psn=%u",
    TX_ARR(gas_id, ul[gas_id].seg_ind).blk.bsn,
    TX_ARR(gas_id, ul[gas_id].seg_ind).blk.first_psn,
    TX_ARR(gas_id, ul[gas_id].seg_ind).blk.last_psn);
#endif

  /* advance the index with wrapround
  */
  INC_MOD(ul[gas_id].seg_ind,SNS);
} /* attach_block_to_tx_array */

/*===========================================================================
===
===  FUNCTION      E_ATTACH_BLOCK_POST_INIT
===
===  DESCRIPTION
===
===    This fn takes the current rlc uplink data block in internal format
===    and attaches it to the attach segment of the transmit window.It also
===    then initialises the e_cur_blk structure ready for the next usage
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
===    dsm_ptr and dup_dsm_ptr made NULL after they are copied to TX_ARR(gas_id, ).
===
===========================================================================*/
void e_attach_block_post_init(gas_id_t gas_id, grlc_ul_data_block_t *bp)
{
  /* Check and warn before we over-write an item
  */
  if (TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr != NULL )
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_388, "GRLU ##atta_blk[%d] dsm not NULL 0x%x",
              ul[gas_id].seg_ind,
              TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr
              );
    GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dsm_ptr);
  }

  /* Check and warn before we over-write an item
  */
  if (TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr != NULL )
  {
    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_389, "GRLU ##atta_blk[%d] dup not NULL 0x%x",
              ul[gas_id].seg_ind,
              TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr
              );
    GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk.dup_dsm_ptr);
  }

  /* copy of the data block in internal format to attach window
  */
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk = *bp;

  /* update the cv as block is now FULLY segmented */
  ul[gas_id].cv = bp->hdr.cv;

  /* Once copied the dsm_ptr and dup_dsm_ptr must be cleared
  */
  bp->dsm_ptr = NULL;
  bp->dup_dsm_ptr = NULL;

  /* Use TXA_PTX for ack mode and TXA_UNACK for un-ack mode
  */
  if (cur_pdu_ptr[gas_id]->rlc_mode == GRLC_MODE_ACK)
    TX_ARR(gas_id, ul[gas_id].seg_ind).vb = TXA_PTX;
  else
    TX_ARR(gas_id, ul[gas_id].seg_ind).vb = TXA_UNACK;

  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.ptx_miss_cnt  = 0;
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.nack_miss_cnt = 0;

  /* Mark block as part of the current PSN
  */
  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.last_psn = cur_pdu_ptr[gas_id]->psn;

  TX_ARR(gas_id, ul[gas_id].seg_ind).blk.bsn = ul[gas_id].seg_ind;

  grlc_ul_diag_block_stat_add_entry(&TX_ARR(gas_id, ul[gas_id].seg_ind).blk, gas_id);

#ifdef  GSM_GPRS_SHOW_FIRST_LAST_PSNS
  MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_390, "GRLU atta bsn=%d 1st_psn=%u last_psn=%u", TX_ARR(gas_id, ul[gas_id].seg_ind).blk.bsn,
                                                      TX_ARR(gas_id, ul[gas_id].seg_ind).blk.first_psn,
                                                      TX_ARR(gas_id, ul[gas_id].seg_ind).blk.last_psn);
#endif

  /* advance the index with wrapround
  */
  INC_MOD(ul[gas_id].seg_ind, ESNS);

  /* --------------------------------------------------------
  ** Post init the data block for the next usage.
  ** --------------------------------------------------------
  */
  E_NEW_DATA_BLOCK(gas_id, bp);

} /* e_attach_block_post_init */

/*===========================================================================
===
===  FUNCTION           E_TX_BLKS(gas_id)
===
===  DESCRIPTION
===
===    Transmit the radio blocks in the Transmit Array in the order of
===    nacked blocks (TXA_NACK), followed by unsent blocks
===    (TXA_PTX), followed by pending ack blocks (TXA_PACK) as per 9.1.3 with
===    respect to the preemp_tx flag
===
===    The number of blocks to send include all ack types: pending_tx,
===    pending_ack and nacked. The blocks to send are in the transmit window
===    (between V(A) and V(S), and the attach window (between V(S) and
===    seg_ind).
===
===    After a block is sent the resource (blocks_granted) is reduced only
===    after it is confirmed as transmitted by PL1/NPL1, ie. after a GET-access
===    is made.
===
===    After sending a NACK block to the FIFO only nack_vsf is updated.
===    After sending a PTX block to the FIFO only vsf is updated.
===    After sending a PACK block to the FIFO only pack_vsf is updated.
===
===    The variable V(A), and V(S) are adjusted as the blocks are sent.
===
===    The SI indicator and timer T3182 are set and reset as the transmit
===    window becomes stalled or un-stalled.
===
===  DESIGN
===
===    if NACK block(s) exist
===       repeat send NACK block(s) to FIFO
===       adjust ack status
===    end NACK
===
===    if (stall)
===       set flag to deal with PACK blocks
===    else (not stall)
===       if PTX block(s) exist
===          send PTX block(s)
===          adjust ack status
===       endif
===
===       if num_blocks still > 0
===          set flag to deal with sending PACK blocks
===       endif
===    endifelse
===
===    if flag to deal with sending PACK blocks is TRUE
===       repeat sending PACK blocks
===    endif
===
===
===
===  <-------------------- SNS-blocks transmit workspace ----------------------->
===  |..X...................X...X........X.......................................|
===     ^                   ^   ^        ^
===     VA                  VS  VSF      seg_ind
===     <--nack_vsf range-->
===     <--pack_vsf range-->
===                         <-vsf range->
===
===
===  PARAMETERS
===
===    num_blks: specifies the maximum number of pending-tx blocks to send which
===              is the space available in the FIFO
===
===  DEPENDENCIES
===
===    TX_ARR, vs, va, seg_ind etc..
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul[gas_id].si,
===    TX_ARR(gas_id, )
===
===========================================================================*/
static void e_tx_blks ( gas_id_t gas_id, uint8  num_blks )
{
    /* loop count and array index */
  uint16       ind1, num_blks_to_search;
  uint16       ind2 = E_GRLC_INVALID_BSN;
  boolean      found_pack_blck = FALSE;

  /* flag to indicate repeat sending of pending tx blocks
  */
  boolean     b_repeat_sending_p_ack;

  /*------------------------------------------------------------------*/

  /* exit when there is no block to transmit
  */
  GRLC_UL_SHOW_TX_VARS(gas_id);

  if (num_blks == 0)
  {
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_391, "ERLU e_tx num_blks = 0 va=%d vs=%d vsf=%d",ul[gas_id].va,ul[gas_id].vs,ul[gas_id].vsf);
    return;
  }

  /* sending pending_ack blocks are required in both non-stall and stall
  ** conditions. To save code space a flag is set when this action is
  ** required. The flag is tested and if TRUE sending pending_ack is
  ** executed in one place at the end of the function. First we need to
  ** clear this flag.
  */
  b_repeat_sending_p_ack = FALSE;

  /* if RLC gets context and hangs on to it for longer than, say 1 frame, NPL1
  ** could potentially accesses the FIFO again whilst RLC is working on it.
  ** So we must lock out NPL1 access to this. Provided this function should take
  ** no longer than 1 frame to produce a maximum of 4 data blocks to send to the
  ** FIFO, it is safe to do so.
  **
  ** This INTLOCK() could be refined to lock just NPL1 ISRs
  */

  /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can take
  ** a snapshot and use it throughout processing of NACK blocks without reading again
  */
  ul[gas_id].inst_vs = ul[gas_id].vs;

  /*====================================================
  ** Search TX Array and send NACKED blocks to UL FIFO
  **====================================================
  */
  if (ul[gas_id].va != ul[gas_id].inst_vs)
  {
    /*----------------------------------------------------------------------
    ** Scan and send NACKED blocks first.
    ** If the last nack block sent is within the tx window (between
    ** V(A) and V(S)) then we want to resume from that point on. Otherwise,
    ** resume the search from V(A).
    **----------------------------------------------------------------------
    */
    if ( IS_GE_MOD(ul[gas_id].nack_vsf, ul[gas_id].inst_vs, ul[gas_id].cur_res.ws) ||
         IS_GT_MOD(ul[gas_id].va,ul[gas_id].nack_vsf, ul[gas_id].cur_res.ws)
       )
    {
      ul[gas_id].nack_vsf = ul[gas_id].va;
      UPD_UL_NACK_VSF(gas_id);
    }

    /* Window size is the distance between V(A) and V(S).
    ** Work out the remaining number of blocks to search for a NACK block which
    ** is between nack_vsf and V(S)
    */
    ind1 = ul[gas_id].nack_vsf;
    num_blks_to_search = MOD_DIF_ESNS(ul[gas_id].inst_vs, ul[gas_id].nack_vsf);

    while (num_blks_to_search && num_blks)
    {
      if (TX_ARR(gas_id, ind1).vb == TXA_NACK)
      {
        if( grlc_ul_fifo_get_nack_cnt(gas_id) == RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE )
        {
          /* No enough space in the PL1 FIFO to send any NACK blocks.
          ** Exit this function and try again next time around.
          */
          MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_392, "GRLU PL1 FIFO full for NACK blks");
          return;
        }

        /* Initialise 2nd payload index to invalid block. */
        ind2 = E_GRLC_INVALID_BSN;

        /* If the MCS used for the initial transmission is undefined
        ** then set initial MCS to commanded MCS
        */
        if( TX_ARR(gas_id, ind1).blk.init_mcs == RM_MCS_UNDEF )
        {
          SET_MCS_FOR_TX( TX_ARR(gas_id, ind1).blk.init_mcs, ul[gas_id].cur_res.cs );
          MSG_GRLC_HIGH_1_G( GRLC_F3_MSG_ENUM_393, "ERLU init_msc of NACK block bsn %d is UNDEF!!",ind1);
        }

        /* if the commanded MSC has changed since the block was last transmitted,
        ** the new MSC for retxing the block needs to be re-evaluated
        */
        TX_ARR(gas_id, ind1).blk.cs = GET_MCS_FOR_RETX( TX_ARR(gas_id, ind1).blk.init_mcs,
                                                ul[gas_id].cur_res.cs,
                                                ul[gas_id].cur_res.resegment);

        if( IS_SPLIT_REQUIRED(TX_ARR(gas_id, ind1).blk.init_mcs,
                              TX_ARR(gas_id, ind1).blk.cs,
                              ul[gas_id].cur_res.resegment
                             )
          )
        {
          if (num_blks < 2 || ((RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE - grlc_ul_fifo_get_nack_cnt(gas_id)) < 2))
          {
          /* No enough space in the PL1 FIFO to send the two blocks created after
          ** the split. Exit this function and try again next time around.
          */
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_394, "GRLU PL1 FIFO full for split NACK bsn=%u",ind1);
            return;
          }
          /* one less block to send as split means 2 blocks
          */
          num_blks--;
        }

        TX_ARR(gas_id, ind1).blk.mcs_demoted_ps = NO_MCS_DEMOTION;

        if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ind1).blk.cs) )
        {
          /* If the new MCS is 7/8 or 9 then a dual payload needs to be formed from two
          ** NACK blocks or a NAK and a PACK block if there isn't a 2nd NACK block in
          ** the tx array. Search the NACK blocks to find a pairing to form the dual
          ** payload.
          */
          ind2 = e_find_2nd_payload( gas_id, (uint16)MOD_ESNS(ind1+1), ul[gas_id].inst_vs,
                                     TX_ARR(gas_id, ind1).blk.cs, TXA_NACK );

          if( ind2 == E_GRLC_INVALID_BSN )
          {
#if 0
            /* Can not  pair NACK/PTX due to the fifo implementation. If there are queued
            ** PTXs and we send a NACK/PTX pair to the NACK FIFO, the NACK/PTX block will
            ** be transmitted first violating the PTX transmission order. Same applies
            ** for NACK/PACK pairing.
            */

            /* If no NACK found, check if we can pair a NACK and a PTX in the same block.
            ** Note that only the 1st available PTX is checked and it is paired with the
            ** NACK block only if they both have the same MCS, otherwise, the MCS will
            ** be downgraded and the NACK block will be send in a single payload block.
            */
            if( ul[gas_id].vsf != ul[gas_id].seg_ind )
            {
              /* Check if vsf is inside V(S) and seg_ind and work out the offset
              ** relative to V(S)
              */

              /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions()
              ** we can take a snapshot and use it throughout processing of PTX blocks
              ** without reading again
              */
              ul[gas_id].inst_vs = ul[gas_id].vs;

              if (IS_GT_MOD(ul[gas_id].seg_ind, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
              {
                if (IS_GT_MOD(ul[gas_id].inst_vs, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
                {
                  /* vsf behind vs so take it to vs value
                  */
                  ul[gas_id].vsf = ul[gas_id].inst_vs;
                  UPD_UL_VSF(gas_id);
                }

                /* index to first PTX block
                */
                if( TX_ARR(gas_id, ind1).blk.cs == TX_ARR(gas_id, ul[gas_id].vsf).blk.cs )
                  ind2 = ul[gas_id].vsf;
              }
              else
              {
                MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_395, "GRLU ptx vsf(%d) >= seg_ind(%d)",ul[gas_id].vsf, ul[gas_id].seg_ind);
              }
            }

            if( ind2 == E_GRLC_INVALID_BSN )
            {
              /* No PTX block found to form a dual payload.
              ** If no more PTX blocks are available for transmission then
              ** a NACK and a PACK block can be paired on a dual payload block
              ** without violating the precedence order.
              */

              if( ul[gas_id].segment_state == SEGMENT_DONE )
              {
                /* If the last pending_ack block is within the tx window (between V(A)
                ** and V(S)) then we want to resume from that point on. Otherwise start
                ** searching from V(A)
                */
                if( !( IS_GT_MOD(ul[gas_id].inst_vs, ul[gas_id].pack_vsf, ul[gas_id].cur_res.ws) &&
                       IS_GE_MOD(ul[gas_id].pack_vsf, ul[gas_id].va, ul[gas_id].cur_res.ws)         )
                   )
                {
                  /* pack_vsf outside V(A) and V(S) so reset it to V(A)
                  */
                  ul[gas_id].pack_vsf = ul[gas_id].va;
                }

                /* No more LLC pdus in the input queue and no more segments expected.
                ** Try to find a PACK block to pair the PTX in the dual payload block
                ** but only if:
                ** - Pre-emptive transmission is on OR
                ** - Pre-emptive transmission is off and the 2nd block is the block
                **   with CV=0
                */
                ind2 = e_find_2nd_payload ( gas_id, ul[gas_id].pack_vsf,
                                            ul[gas_id].inst_vs,
                                            TX_ARR(gas_id, ind1).blk.cs,
                                            TXA_PACK );

                if( (ind2 == E_GRLC_INVALID_BSN) ||
                    ( !ul[gas_id].pre_emptive_tx && ul[gas_id].e_bsn_of_cv_0_block != ind2 )
                  )
                {
                  /* No valid PACK found to pair the PTX block. Downgrade MCS and send
                  ** PTX block in a single payload block
                  */
                  grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
                }
              }

              if( ind2 == E_GRLC_INVALID_BSN )
              {
                /* No pair was found after searching NACK/PTX/PACK blocks
                ** Downgrade the MCS to send block with single payload
                */
                grlc_ul_demote_mcs_modify_li( gas_id, ind1);
              }
            }
#else
            /* No pair was found after searching NACK blocks
            ** Downgrade the MCS to send block with single payload
            */
            grlc_ul_demote_mcs_modify_li( gas_id, ind1 );

            /* no % needed , storing old_ps as MCS is demoted , and will be useful
            ** if mcs is promoted back to higher MCS before this block can be txed.
            */
            TX_ARR(gas_id, ind1).blk.mcs_demoted_ps = TX_ARR(gas_id, ind1).blk.old_ps+1;
#endif /* if 0 */
          }
        } /* Formating dual payload */

        /* Set the Puncturing Scheme (PS) for the data block. Note that if there is no
        ** change in MCS the rotation of the PS value is done in the post processing
        ** function
        */
        if( TX_ARR(gas_id, ind1).blk.old_mcs != TX_ARR(gas_id, ind1).blk.cs )
        {
          /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new PS value
          */
          ROTATE_PS_MCS_CHANGE(TX_ARR(gas_id, ind1).blk.old_mcs, TX_ARR(gas_id, ind1).blk.cs,
                               TX_ARR(gas_id, ind1).blk.old_ps, &(TX_ARR(gas_id, ind1).blk.ps)
                              );
        }

        if( ind2 != E_GRLC_INVALID_BSN )
        {
          /* Set BSN2 number which is relevent to BSN1 */
          TX_ARR(gas_id, ind1).blk.hdr.bsn2 = MOD_DIF_ESNS( ind2 , ind1);

          /* If 2nd payload is a NACK, e_find_2nd_payload() has returned with
          ** the 2nd payload that is compatible to the MCS of the 1st payload.
          ** Set the derived MCS for the 2nd payload and rotate PS.
          */
          if( TX_ARR(gas_id, ind2).vb == TXA_NACK )
          {
            TX_ARR(gas_id, ind2).blk.cs = TX_ARR(gas_id, ind1).blk.cs;

            if( TX_ARR(gas_id, ind2).blk.old_mcs != TX_ARR(gas_id, ind2).blk.cs )
            {
              /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new PS
              ** value
              */
              ROTATE_PS_MCS_CHANGE( TX_ARR(gas_id, ind2).blk.old_mcs, TX_ARR(gas_id, ind2).blk.cs,
                                    TX_ARR(gas_id, ind2).blk.old_ps, &(TX_ARR(gas_id, ind2).blk.ps)
                                  );
            }
          }
        }

        /* Set Coding and Puncturing Scheme (CPS) for the data block */
        GET_CPS( gas_id, ind1, ind2, TX_ARR(gas_id, ind1).blk.hdr.cps );

        /* Set Resent Block Bit for every block that is re-txed */
        TX_ARR(gas_id, ind1).blk.hdr.rsb = RM_RSB_RE_TX;

        /* send block to FIFO and update vars if FIFO is not full
        */
        if (e_send_block_to_pl1_fifo(gas_id, ind1, ind2, TXA_NACK))
        {
          /* reduce the number of blocks to send within this call
          */
          --num_blks;

          TX_ARR(gas_id, ind1).vb = TXA_PENDING_PACK;

          /* Update old MCS/PS values and rotate PS to be ready for next re-tx
          ** and we will be sending nack as pending pack if needed.
          */
          TX_ARR(gas_id, ind1).blk.old_mcs = TX_ARR(gas_id, ind1).blk.cs;
          TX_ARR(gas_id, ind1).blk.old_ps = TX_ARR(gas_id, ind1).blk.ps;
          ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ind1).blk.cs,
            TX_ARR(gas_id, ind1).blk.old_ps, TX_ARR(gas_id, ind1).blk.ps );

          if( ind2 != E_GRLC_INVALID_BSN )
          {

            /* Update PTX or NACK vsf indexes according
            ** to the 2nd block status
            */
            if( TX_ARR(gas_id, ind2).vb == TXA_NACK )
            {
              ul[gas_id].nack_vsf = ind2;

              /* Update old MCS/PS values and rotate PS to be ready for next re-tx
              ** and we will be sending nack as pending pack if needed.
              */
              TX_ARR(gas_id, ind2).blk.old_mcs = TX_ARR(gas_id, ind2).blk.cs;
              TX_ARR(gas_id, ind2).blk.old_ps = TX_ARR(gas_id, ind2).blk.ps;
              ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ind2).blk.cs,
                TX_ARR(gas_id, ind2).blk.old_ps, TX_ARR(gas_id, ind2).blk.ps );

              /* If pack_vsf is ahead of the oldest NACK block just been sent,
              ** set pack_vsf equal to the oldest NACK that is just txed.
              ** The NACK block is now P_ACK and that is where pack_vsf should point at
              */
              if (IS_GT_MOD(ul[gas_id].pack_vsf, ind2, ul[gas_id].cur_res.ws))
              {
                ul[gas_id].pack_vsf = ind2;
              }
            }
            else if( TX_ARR(gas_id, ind2).vb == TXA_PTX )
            {
              ul[gas_id].vsf = ind2 + 1;

              /* 2nd payload is a PTX, that means no NACKs were found to
              ** form the dual payload. Reset nack_vsf and leave the "sending NACKs" loop
              */
              ul[gas_id].nack_vsf = ul[gas_id].va;
              break;
            }
            else if( TX_ARR(gas_id, ind2).vb == TXA_PACK )
            {
              ul[gas_id].pack_vsf = ind2 + 1;

              /* 2nd payload is a PTX, that means no NACKs were found to
              ** form the dual payload. Reset nack_vsf and leave the "sending NACKs" loop
              */
              ul[gas_id].nack_vsf = ul[gas_id].va;
              break;
            }

            TX_ARR(gas_id, ind2).vb = TXA_PENDING_PACK;
          }
          else
          {
            if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ind1).blk.cs) )
            {
              /* There isn't a 2nd NACK or a PTX block to form a dual payload block.
              ** No point continuing this loop.
              ** Since no NACK blocks were found, bring nack_vsf to V(A)
              ** so that next time around a NACK search will start from V(A)
              */
              ul[gas_id].nack_vsf = ul[gas_id].va;

              break;
            }

            /* Update nack_vsf.
            */
            ul[gas_id].nack_vsf = ind1;

            /* If pack_vsf is ahead of the oldest NACK block just been sent,
            ** set pack_vsf equal to the oldest NACK that is just txed.
            ** The NACK block is now P_ACK and that is where pack_vsf should point at
            */
            if (IS_GT_MOD(ul[gas_id].pack_vsf, ind1, ul[gas_id].cur_res.ws))
            {
              ul[gas_id].pack_vsf = ind1;
            }
          }

          INC_MOD(ul[gas_id].nack_vsf, ESNS);
          if ( IS_GE_MOD(ul[gas_id].nack_vsf, ul[gas_id].inst_vs, ul[gas_id].cur_res.ws) ||
               IS_GT_MOD(ul[gas_id].va,ul[gas_id].nack_vsf, ul[gas_id].cur_res.ws)
             )
          {
            /* Reset nack_vsf to V(A) if NOT (ul[gas_id].va <= nack_vsf < ul[gas_id].inst_vs) */
            ul[gas_id].nack_vsf = ul[gas_id].va;
          }

          UPD_UL_NACK_VSF(gas_id);
        }
        else
        {
          MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_396, "GRLU Info tx NAK FIFO full ind1=%d num_blks=%d",ind1, num_blks);

          return;
        }

        if( ind2 != E_GRLC_INVALID_BSN )
        {
          /* Decrement number of blocks to search by ind2-ind1 blocks.
          */
          num_blks_to_search = ( num_blks_to_search > MOD_DIF_ESNS( ind2, ind1) ) ?
            (num_blks_to_search - MOD_DIF_ESNS( ind2, ind1 ) ) : 0;
        }
        else
        {
          if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ind1).blk.cs) )
          {
            /* There isn't a 2nd NACK or a PACK block to form a dual payload block.
            ** No point continuing this loop. Set num_blks_to_search to 0.
            */
            num_blks_to_search = 0;
          }
          else
          {
            num_blks_to_search--;
          }
        }
      } /* block is NACK */
      else
      {
        num_blks_to_search--;

        INC_MOD(ul[gas_id].nack_vsf, ESNS);

        if ( IS_GE_MOD(ul[gas_id].nack_vsf, ul[gas_id].inst_vs, ul[gas_id].cur_res.ws) ||
             IS_GT_MOD(ul[gas_id].va,ul[gas_id].nack_vsf, ul[gas_id].cur_res.ws)
           )
        {
          /* Reset nack_vsf to V(A) if NOT (ul[gas_id].va <= nack_vsf < ul[gas_id].inst_vs) */
          ul[gas_id].nack_vsf = ul[gas_id].va;
        }
      }

      /* Index to next nack block.
      ** ind can not go beyond V(S)
      */
      ind1 = ul[gas_id].nack_vsf;

    } /* send loop */
  } /* processing NACK blocks */

  MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_397, "ERLU e_tx petx = %u",ul[gas_id].pre_emptive_tx);

  /*------------------------------------
  ** Deal with PTX and PACK blocks next
  **------------------------------------
  */

  if (IS_E_TX_WINDOW_FULL(gas_id))
  /*----------------------------------------------------------------------
  ** Deal with the tx window stall case. The NACKED blocks had been dealt
  ** with so deal with the PACK blocks next since the PTX blocks are not
  ** available as the window has stalled.
  **----------------------------------------------------------------------
  */
  {
    /* set this flag to deal with PACK blocks at the end section only if
    ** pre-emptive transmission is on.
    */
    if ( (num_blks > 0) && (ul[gas_id].pre_emptive_tx) )
      b_repeat_sending_p_ack = TRUE;

#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_398, "ERLU e_tx ptx/pack w full va=%u vs=%u repeat_sending_p_ack=%u",
            ul[gas_id].va,
            ul[gas_id].vs,
            b_repeat_sending_p_ack);
#endif

    if (ul[gas_id].si == RM_TX_NOT_STALLED)
    {
      /* Don't start T3182 timer here since the block will sit in the FIFO.
      ** post_tx_actions() will start T3182 if SI is set and T3182
      ** is not active
      */
      ul[gas_id].si = RM_TX_STALLED;
#ifdef  GSM_GPRS_GRLU_SHOW_SI
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_399, "ERLU e_tx SI=1 va=%d vs=%d seg=%d",ul[gas_id].va,ul[gas_id].vs,ul[gas_id].seg_ind);
#endif
    }
  } /* end window stall */
  else
  /*---------------------------------------------------------------
  ** Deal with non-stall condition by checking and sending the
  ** PTX blocks followed by checking and sending of PACK blocks
  **---------------------------------------------------------------
  */
  {
#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_400, "ERLU e_tx ptx/pack w ! full va=%u vs=%u p_vsf=%u",
            ul[gas_id].va,
            ul[gas_id].vs,
            ul[gas_id].pack_vsf);
#endif

    /* post_tx_actions() invoked by PL1 after a block is transmitted from the DSP
    ** buffer alters V(S). Therefore all operations relating to V(S) must be
    ** protected from INTerrupts
    */

    /*=================================================
    ** Search TX Array and send PTX blocks to UL FIFO
    **=================================================
    */
    if (num_blks > 0 &&
        ul[gas_id].vsf != ul[gas_id].seg_ind
       )
    {
      /* Check if vsf is inside V(S) and seg_ind and work out the offset
      ** relative to V(S)
      */

      /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we
      ** can take a snapshot and use it throughout processing of PTX blocks without
      ** reading again
      */
      ul[gas_id].inst_vs = ul[gas_id].vs;

      if (IS_GT_MOD(ul[gas_id].seg_ind, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
      {
        if (IS_GT_MOD(ul[gas_id].inst_vs, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
        {
          /* vsf behind vs so take it to vs value
          */
          ul[gas_id].vsf = ul[gas_id].inst_vs;
          UPD_UL_VSF(gas_id);
        }
      }
      else
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_401, "GRLU ptx vsf(%d) >= seg_ind(%d)",ul[gas_id].vsf,ul[gas_id].seg_ind);
      }

      /* index to first block to search
      */
      ind1 = ul[gas_id].vsf;

      /* Send loop. Terminate when num_blks runs out or the vsf reaches the
      ** seg_ind where segmentation stops
      */
      while ( num_blks &&
              IS_GT_MOD(ul[gas_id].seg_ind, ind1, ul[gas_id].cur_res.ws)
            )
      {
        if (TX_ARR(gas_id, ind1).vb == TXA_PTX)
        {
          if( grlc_ul_fifo_get_ptx_cnt(gas_id) == RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE )
          {
            /* No enough space in the PL1 FIFO to send any PTX blocks.
            ** Exit this function and try again next time around.
            */
            MSG_GRLC_EXT_MED_0_G(GRLC_F3_MSG_ENUM_402, "GRLU PL1 FIFO full for PTX blks");
            return;
          }

          /* Initialise 2nd payload index to invalid block. */
          ind2 = E_GRLC_INVALID_BSN;

          /* Assign PS 1 to the PTX data block before is sent to PL1 FIFO */
          TX_ARR(gas_id, ind1).blk.ps = GRLC_PS_1;

          /* The MCS for the initial transmission will be set to the block's
          ** MCS after the block has been confirmed as sent by L1 in post_tx_actions()
          */
          TX_ARR(gas_id, ind1).blk.init_mcs = RM_MCS_UNDEF;

          /* Demote the MCS in case the 2nd block falls outside Stall window
          ** Adding additional checking attach space - when empty demote the MCS.
          */
          if( IS_FORMING_DUAL_PAYLOAD_REQUIRED( TX_ARR(gas_id, ind1).blk.cs ) )
          {
            /* If the new MCS is 7/8 or 9 then a dual payload needs to be formed from
            ** two PTX blocks.
            */
            if( ( IS_GT_MOD(ul[gas_id].seg_ind, MOD_ESNS(ind1+1), ul[gas_id].cur_res.ws)) ||
                ( !E_GET_ATTACH_WINDOW_SPACE(gas_id) )
              )
            {
              if( TX_ARR(gas_id, MOD_ESNS(ind1 + 1)).blk.cs == TX_ARR(gas_id, ind1).blk.cs &&
                  TX_ARR(gas_id, MOD_ESNS(ind1 + 1)).vb == TX_ARR(gas_id, ind1).vb
                )
              {
                ind2 = MOD_ESNS(ind1 + 1);

                /* Assign PS 1 to the PTX data block before is sent to PL1 FIFO */
                TX_ARR(gas_id, ind2).blk.ps = GRLC_PS_1;

                /* The MCS for the initial transmission will be set to the block's
                ** MCS after the block has been confirmed as sent by L1 in
                ** post_tx_actions()
                */
                TX_ARR(gas_id, ind2).blk.init_mcs = RM_MCS_UNDEF;
              }
              else
              {
                /* Commanded MCS changed. Dowmgrade the MCS to send a single payload
                ** block
                */
                grlc_ul_demote_mcs_modify_li( gas_id, ind1);
              }
            }
            else
            {

              if( ul[gas_id].segment_state == SEGMENT_DONE )
              {
#if 0  /* Pairing PTX with PACK is not valid because of limited bsn2 offset range */

                /* If the last pending_ack block is within the tx window (between V(A)
                ** and V(S)) then we want to resume from that point on. Otherwise start
                ** searching from V(A)
                */
                if( !( IS_GT_MOD(ul[gas_id].inst_vs, ul[gas_id].pack_vsf, ul[gas_id].cur_res.ws) &&
                       IS_GE_MOD(ul[gas_id].pack_vsf, ul[gas_id].va, ul[gas_id].cur_res.ws)         )
                   )
                {
                  /* pack_vsf outside V(A) and V(S) so reset it to V(A)
                  */
                  ul[gas_id].pack_vsf = ul[gas_id].va;
                }

                /* No more LLC pdus in the input queue and no more segments expected.
                ** Try to find a PACK block to pair the PTX in the dual payload block
                ** but only if:
                ** - Pre-emptive transmission is on OR
                ** - Pre-emptive transmission is off and the 2nd block is the block
                **   with CV=0
                */
                ind2 = e_find_2nd_payload ( gas_id, ul[gas_id].pack_vsf, ul[gas_id].inst_vs,
                                            TX_ARR(gas_id, ind1).blk.cs, TXA_PACK );

                if( (ind2 == E_GRLC_INVALID_BSN) ||
                    (!ul[gas_id].pre_emptive_tx && ul[gas_id].e_bsn_of_cv_0_block != ind2)
                  )
                {
                  /* No valid PACK found to pair the PTX block. Downgrade MCS and send
                  ** PTX block in a single payload block
                  */
                  grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
                }
#else
                grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
#endif
              }
              else
              {
                /* Segmentation has not finished. Form dual payload next time around.
                */
                return;
              }
            }
          } /* Formating dual payload */

          /* Mark the bsn with CV = 0 */
          if (TX_ARR(gas_id, ind1).blk.hdr.cv == 0)
          {
            MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_403, "GRLU tx CV=0 bsn=%d",ind1);
            ul[gas_id].e_bsn_of_cv_0_block = ind1;
            ul[gas_id].re_tx_cv_0_cnt = 0;
          }
          else
          {
            ul[gas_id].e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;
          }

          if( (ind2 != E_GRLC_INVALID_BSN) )
          {
            /* Mark the bsn with CV = 0 */
            if (TX_ARR(gas_id, ind2).blk.hdr.cv == 0)
            {
              MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_404, "GRLU tx ind2 CV=0 bsn=%d",ind2);
              ul[gas_id].e_bsn_of_cv_0_block = ind2;
              ul[gas_id].re_tx_cv_0_cnt = 0;
            }
            else
            {
              ul[gas_id].e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;
            }

            /* Set BSN2 number which is relevent to BSN1 */
            TX_ARR(gas_id, ind1).blk.hdr.bsn2 = MOD_DIF_ESNS( ind2 , ind1);

            /* If 2nd payload is a PACK, e_find_2nd_payload() has returned with
            ** the 2nd payload that is compatible to the MCS of the 1st payload.
            ** Set the derived MCS for the 2nd payload and rotate PS.
            */
            if( TX_ARR(gas_id, ind2).vb == TXA_PACK )
            {
              TX_ARR(gas_id, ind2).blk.cs = TX_ARR(gas_id, ind1).blk.cs;

              if( TX_ARR(gas_id, ind2).blk.old_mcs != TX_ARR(gas_id, ind2).blk.cs )
              {
                /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the
                ** new PS value
                */
                ROTATE_PS_MCS_CHANGE( TX_ARR(gas_id, ind2).blk.old_mcs, TX_ARR(gas_id, ind2).blk.cs,
                                      TX_ARR(gas_id, ind2).blk.old_ps, &(TX_ARR(gas_id, ind2).blk.ps)
                                    );
              }

              /* Set Resent Block Bit for every block that is re-txed */
              /* TX_ARR(gas_id, ind1).blk.hdr.rsb = RM_RSB_RE_TX; */
            }
          }

          /* Set Coding and Puncturing Scheme (CPS) for the data block */
          GET_CPS( gas_id, ind1, ind2, TX_ARR(gas_id, ind1).blk.hdr.cps );

          /* send block to FIFO with current status as PTX
          */
          if (e_send_block_to_pl1_fifo(gas_id, ind1,ind2, TXA_PTX))
          {
            /* one less block to send
            */
            num_blks--;

            /* increment VSF. No need to check for wrap-round or overflow
            ** because 'num_blks_to search' puts a limit at seg_ind
            */
            if( ind2 != E_GRLC_INVALID_BSN )
            {
              if( TX_ARR(gas_id, ind2).vb == TXA_PTX )
              {
                ul[gas_id].vsf = ind2;
                INC_MOD(ul[gas_id].vsf, ESNS);
                UPD_UL_VSF(gas_id);
              }
              else if( TX_ARR(gas_id, ind2).vb == TXA_PACK )
              {
                ul[gas_id].pack_vsf = ind2 + 1;

                /* 2nd payload is a PACK, that means no PTXs were found to
                ** form the dual payload. Increment ul_vsf and leave the
                ** "sending PTXs" loop.
                */
                INC_MOD(ul[gas_id].vsf, ESNS);
                UPD_UL_VSF(gas_id);
                break;
              }
            }
            else
            {
              if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ind1).blk.cs) )
              {
                /* There isn't a 2nd PTX or a PACK block to form a dual payload block.
                ** No point continuing this loop.
                */
                INC_MOD(ul[gas_id].vsf, ESNS);
                UPD_UL_VSF(gas_id);

                break;
              }

              ul[gas_id].vsf = ind1;
              INC_MOD(ul[gas_id].vsf, ESNS);
              UPD_UL_VSF(gas_id);
            }
          }
          else
          {

#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
            MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_405, "GRLU Info PTX FIFO full ind1=%d ind2=%d",ind1,ind2);
#endif
            return;
          }
        } /* if finding PTX block */
        else
        {
          INC_MOD(ul[gas_id].vsf, ESNS);
          UPD_UL_VSF(gas_id);
        }

        /* Get next PTX block */
        ind1 = ul[gas_id].vsf;

      } /* loop sending PTX */
    } /* processing PTX blocks */

    /*--------------------------------------------------------------------------
    ** If the number of blocks to send specified has not been exhausted after
    ** sending NACK and PTX blocks then consider sending PACK blocks by setting
    ** the flag below, but only if:
    ** - num_blks is positive AND
    ** - Pre-emptive transmission is on AND
    ** - Either window is full (stalled) OR segmentation is completed (TBF-ends)
    **--------------------------------------------------------------------------
    */
    if ( num_blks > 0 &&
         ul[gas_id].pre_emptive_tx &&
         ((! E_GET_ATTACH_WINDOW_SPACE(gas_id)) || ul[gas_id].segment_state == SEGMENT_DONE)
       )
    {
      b_repeat_sending_p_ack = TRUE;
#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_406, "ERLU e_tx ena-PACKs p=%u vs=%d in=%d",
              cur_pdu_ptr[gas_id]->psn,
              ul[gas_id].vs,
              ul[gas_id].seg_ind);
#endif

      /* TX Window stall , set the stall indicator
      */
      if (! E_GET_ATTACH_WINDOW_SPACE(gas_id))
      {
        if (ul[gas_id].si == RM_TX_NOT_STALLED)
        {
          /* Don't start T3182 timer here since the block will sit in the FIFO.
          ** post_tx_actions() will start T3182 if SI is set and T3182
          ** is not active
          */
          ul[gas_id].si = RM_TX_STALLED;
#ifdef  GSM_GPRS_GRLU_SHOW_SI
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_407, "ERLU tx SI=1 va=%d vs=%d seg=%d",ul[gas_id].va,ul[gas_id].vs,ul[gas_id].seg_ind);
#endif
        }
      }
    }
#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
    else
    {
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_408, "ERLU e_tx num_blks=%u WINDOW_FULL=%u segment_state=%u",
                num_blks,
                IS_E_TX_WINDOW_FULL(gas_id),
                ul[gas_id].segment_state
             );
    }
#endif
  } /* non-stall window */

  /* In cases when there is ABORT midway from layer 1 during PUAN processing,
  ** pack_vsf = va after PUAN processing may not mack with the pack_vsf block
  ** in the FIFO , which means PACK FIFO has stale data and needs to cleared.
  ** GCF TC Anite 53.1.1.2. CR: 125334
  ** As this is a corner case , we have not reverted the Puncturing schemes
  ** of the pack blocks being flushed.
  ** The solution is only implemented for EDGE PACK blocks.
  */
  if( (grlc_ul_fifo[gas_id].pack_ctr > grlc_ul_fifo[gas_id].pack_read_ctr) && ul[gas_id].b_puan)
  {
    grlc_ul_fifo_disable_pl1_access(gas_id);

    /* Clear PACK FIFO */
    grlc_ul_fifo[gas_id].pack_in_ind = 0;
    grlc_ul_fifo[gas_id].pack_out_ind = 0;
    grlc_ul_fifo[gas_id].pack_ctr = 0;
    grlc_ul_fifo[gas_id].pack_read_out_ind = 0;
    grlc_ul_fifo[gas_id].pack_read_ctr = 0;

    MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_409, "ERLU Pack FIFO Cleared");

    grlc_ul_fifo_enable_pl1_access(gas_id);
  }
  ul[gas_id].b_puan = FALSE;

  /*===========================================================================
  ** Search TX Array and send PACK blocks to UL FIFO if there are any.
  **
  ** pack_vsf marks the block beyond where the previous PACK block was sent.
  ** This is the start of the search for the next PACK block to send to the
  ** FIFO. When pack_vsf reaches V(S) it wraps back to V(A) to seek the oldest
  ** un-ack block to send.
  **===========================================================================
  */

  /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can
  ** take a snapshot and use it throughout processing of PACK blocks without reading
  ** again
  */
  ul[gas_id].inst_vs = ul[gas_id].vs;

  if (b_repeat_sending_p_ack)
  {
    /* If the last pending_ack block is within the tx window (between V(A) and
    ** V(S)) then we want to resume from that point on. Otherwise start sending
    ** pending_ack from V(A)
    */
    if( !( IS_GE_MOD(ul[gas_id].inst_vs, ul[gas_id].pack_vsf, ul[gas_id].cur_res.ws) &&
           IS_GE_MOD(ul[gas_id].pack_vsf, ul[gas_id].va, ul[gas_id].cur_res.ws)         )
       )
    {
      /* pack_vsf outside V(A) and V(S) so reset it to V(A)
      */
      ul[gas_id].pack_vsf = ul[gas_id].va;
    }

    /* In MS Class 12, there might be PTX blks in the FIFO that have not been processed
    ** (post txed) leaving the vs behind vsf. This can cause multiple PACK transmission
    ** on the same BSN in the PACK FIFO which will confuse the peer entity.
    ** For example PTX blks with BSN 0,1,2,3 and 4 with CV 0, has been send to the PTX
    ** FIFO but only BSN 0 has been processed by PL1. Since there are no more PTX blks
    ** to send, PACK blks of BSN0 will fill up the PACK FIFO. To prevent this, PACK blks
    ** will be produced only if pack_vsf is behind vs or if vs is equal to vsf.
    ** Note that this same check is added every time we update pack_vsf in the while
    ** loop below.
    */

    if( ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs != ul[gas_id].vsf )
    {
      return;
    }
    else if (ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs == ul[gas_id].vsf)
    {
      ul[gas_id].pack_vsf = ul[gas_id].va;
    }

    num_blks_to_search = MOD_DIF_ESNS(ul[gas_id].inst_vs, ul[gas_id].va);

    /* ! while (num_blks && IS_TX_WINDOW_FULL())
    */
    while (num_blks &&
           ul[gas_id].va != ul[gas_id].inst_vs
          )
    {
      /* Find index of the start of the tx window
      */
      ind1 = ul[gas_id].pack_vsf;

      /*-------------------------------------------------------------------------
      ** Only send PACKs that have been pending ack before this function is
      ** called and not the re-sent NACKED blocks. PTX blocks should not be sent
      ** but because PTX blocks will eventually be sent and become PACK blocks
      ** we also send PTX blocks as PACK .
      **-------------------------------------------------------------------------
      */
      if (TX_ARR(gas_id, ind1).vb == TXA_PACK ||
          TX_ARR(gas_id, ind1).vb == TXA_PENDING_PACK
        )
      {
        if( grlc_ul_fifo_get_pack_cnt(gas_id) == RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE )
        {
          /* No enough space in the PL1 FIFO to send any PACK blocks.
          ** Exit this function and try again next time around.
          */
          MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_410, "GRLU PL1 FIFO full for PACK blks");
          return;
        }

        /* Initialise 2nd payload index to invalid block. */
        ind2 = E_GRLC_INVALID_BSN;

        if (TX_ARR(gas_id, ind1).vb == TXA_PENDING_PACK)
        {
          if (++TX_ARR(gas_id, ind1).blk.nack_miss_cnt > MAX_MISS_CNT)
          {
#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_411, "ERLU tx NACK %d not acked from PL1",ind1);
#endif
            TX_ARR(gas_id, ind1).blk.nack_miss_cnt = 0;
          }
        }

        /* Set flag to indicate that at lease one PACK block has been found and
        ** the PACK loop should continue indefinitely until num_blks = 0 ,
        ** which may not get zero , and this would run in infinite loop.
        */
        found_pack_blck = TRUE;


        /* If the MCS used for the initial transmission is undefined
        ** then set initial MCS to commanded MCS
        */
        if( TX_ARR(gas_id, ind1).blk.init_mcs == RM_MCS_UNDEF )
        {
          SET_MCS_FOR_TX( TX_ARR(gas_id, ind1).blk.init_mcs, ul[gas_id].cur_res.cs );
          MSG_GRLC_HIGH_1_G( GRLC_F3_MSG_ENUM_412, "ERLU init_msc of PACK block bsn %d is UNDEF!!",ind1);
        }

        /* if the commanded MSC has changed since the block was last transmitted,
        ** the new MSC for retxing the block needs to be re-evaluated
        */
        TX_ARR(gas_id, ind1).blk.cs = GET_MCS_FOR_RETX( TX_ARR(gas_id, ind1).blk.init_mcs,
                                                 ul[gas_id].cur_res.cs,
                                                 ul[gas_id].cur_res.resegment);

        if( IS_SPLIT_REQUIRED(TX_ARR(gas_id, ind1).blk.init_mcs,
                              TX_ARR(gas_id, ind1).blk.cs,
                              ul[gas_id].cur_res.resegment
                             )
          )
        {
          if(num_blks < 2 || ((RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE - grlc_ul_fifo_get_pack_cnt(gas_id)) < 2))
          {
            /* No enough space in the PL1 FIFO to send the two blocks created after
            ** the split. Exit this function and try again next time around.
            */
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_413, "GRLU PL1 FIFO full for split PACK bsn=%u",ind1);
            return;
          }
          /* one less block to send as split means 2 blocks*/
          num_blks--;
        }

        if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ind1).blk.cs) )
        {
          /* If the new MCS is 7/8 or 9 then a dual payload needs to be formed
          ** from two PACK blocks. Search the PACK blocks to find a pairing to
          ** form the dual payload.
          */
          ind2 = e_find_2nd_payload ( gas_id, (uint16)MOD_ESNS(ind1+1), ul[gas_id].inst_vs,
                                      TX_ARR(gas_id, ind1).blk.cs, TXA_PACK );

          /* as pending pack block is a nack block which would eventually become
          ** a pack block.
          */
          if (ind2 == E_GRLC_INVALID_BSN)
          {
            ind2 = e_find_2nd_payload ( gas_id, (uint16)MOD_ESNS(ind1+1), ul[gas_id].inst_vs,
                                        TX_ARR(gas_id, ind1).blk.cs, TXA_PENDING_PACK );
          }

          if( ind2 == E_GRLC_INVALID_BSN )
          {
            /* No PACK block found to form a dual payload.
            ** Downgrade the MCS to send a block with single payload
            */
            grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
          }
        } /* Formating dual payload */

        if( TX_ARR(gas_id, ind1).blk.old_mcs != TX_ARR(gas_id, ind1).blk.cs )
        {
          /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new
          ** PS value
          */
          ROTATE_PS_MCS_CHANGE( TX_ARR(gas_id, ind1).blk.old_mcs, TX_ARR(gas_id, ind1).blk.cs,
                                TX_ARR(gas_id, ind1).blk.old_ps, &(TX_ARR(gas_id, ind1).blk.ps)
                                );
        }

        if( ind2 != E_GRLC_INVALID_BSN )
        {
          if (TX_ARR(gas_id, ind2).vb == TXA_PENDING_PACK)
          {
            if (++TX_ARR(gas_id, ind2).blk.nack_miss_cnt > MAX_MISS_CNT)
            {
#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_414, "ERLU tx NACK %d not acked from PL1",ind2);
#endif
              TX_ARR(gas_id, ind2).blk.nack_miss_cnt = 0;
            }
          }
        }

        if( ind2 != E_GRLC_INVALID_BSN )
        {
          /* Set BSN2 number which is relevent to BSN1 */
          TX_ARR(gas_id, ind1).blk.hdr.bsn2 = MOD_DIF_ESNS( ind2 , ind1);

          /* e_find_2nd_payload() has returned with the 2nd payload that matches
          ** the MCS of the 1st payload. Set the derived MCS for the 2nd payload
          */
          TX_ARR(gas_id, ind2).blk.cs = TX_ARR(gas_id, ind1).blk.cs;

          if( TX_ARR(gas_id, ind2).blk.old_mcs != TX_ARR(gas_id, ind2).blk.cs )
          {
            /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new
            ** PS value
            */
            ROTATE_PS_MCS_CHANGE( TX_ARR(gas_id, ind2).blk.old_mcs, TX_ARR(gas_id, ind2).blk.cs,
                                  TX_ARR(gas_id, ind2).blk.old_ps, &(TX_ARR(gas_id, ind2).blk.ps)
                                );
          }
        }

        /* Set Resent Block Bit for every block that is re-txed */
        TX_ARR(gas_id, ind1).blk.hdr.rsb = RM_RSB_RE_TX;

        /* Set Coding and Puncturing Scheme (CPS) for the data block */
        GET_CPS( gas_id, ind1, ind2, TX_ARR(gas_id, ind1).blk.hdr.cps );

        if (e_send_block_to_pl1_fifo(gas_id, ind1, ind2, TXA_PACK))
        {
          /* one less block to send
          */
          num_blks--;

          /* Update old MCS/PS values and rotate PS to be ready for next re-tx */
          TX_ARR(gas_id, ind1).blk.old_mcs = TX_ARR(gas_id, ind1).blk.cs;
          TX_ARR(gas_id, ind1).blk.old_ps = TX_ARR(gas_id, ind1).blk.ps;
          ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ind1).blk.cs,
            TX_ARR(gas_id, ind1).blk.old_ps, TX_ARR(gas_id, ind1).blk.ps );

          /* update the bsn of the next pending_ack block
          */
          if( ind2 != E_GRLC_INVALID_BSN )
          {
            ul[gas_id].pack_vsf = ind2;

            /* 2nd payload. Update old MCS/PS values and rotate PS to be
            ** ready for next re-tx
            */
            TX_ARR(gas_id, ind2).blk.old_mcs = TX_ARR(gas_id, ind2).blk.cs;
            TX_ARR(gas_id, ind2).blk.old_ps = TX_ARR(gas_id, ind2).blk.ps;
            ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ind2).blk.cs,
              TX_ARR(gas_id, ind2).blk.old_ps, TX_ARR(gas_id, ind2).blk.ps );
          }
          else
          {
            ul[gas_id].pack_vsf = ind1;
          }

          INC_MOD(ul[gas_id].pack_vsf, ESNS);

          if( ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs != ul[gas_id].vsf )
          {
            return;
          }
          else
          {
            if (IS_GE_MOD(ul[gas_id].pack_vsf, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
            {
              ul[gas_id].pack_vsf = ul[gas_id].va;
            }
            UPD_UL_PACK_VSF(gas_id);
          }
        }
        else
        {

#ifdef GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
          MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_415, "ERLU e_tx PAK FIFO full ind1=%d ind2=%d",ind1,ind2);
#endif
          /* exit loop and drop out the sending PACK blokcs section
          */
          return;
        }
      }
      else
      {
        if(TX_ARR(gas_id, ind1).vb == TXA_PTX)
        {
          /* when post_tx_actions() misses acknowleging PTX blocks and cause
          ** the BSN not to be continuous with VS then make these PTX blocks
          ** in PACK range PACK blocks. Otherwise they don't get re-transmitted
          ** and get stuck in PTX state. Also update VS in the process
          */
          if (++TX_ARR(gas_id, ind1).blk.ptx_miss_cnt > MAX_MISS_CNT)
          {
#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_416, "ERLU tx PTX %d not acked from PL1",ind1);
#endif
            TX_ARR(gas_id, ind1).blk.ptx_miss_cnt = 0;

#if 0  /* Forcing TXA_PTX blocks to TXA_PTX
   ** have some undesirable effect on vsf pointers
   */
            TX_ARR(gas_id, ind1).vb = TXA_PACK;
            if (IS_GE_MOD(ind1,ul[gas_id].vs,ul[gas_id].cur_res.ws))
            {
              ul[gas_id].vs = ind1;
              E_INC_VS(gas_id);
            }
#endif /* Forcing TXA_PTX blocks to TXA_PTX */
          }
        }

        INC_MOD(ul[gas_id].pack_vsf, ESNS);

        if( ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs != ul[gas_id].vsf )
        {
          return;
        }
        else
        {
          if (IS_GE_MOD(ul[gas_id].pack_vsf, ul[gas_id].vsf, ul[gas_id].cur_res.ws))
          {
            ul[gas_id].pack_vsf = ul[gas_id].va;
          }
          UPD_UL_PACK_VSF(gas_id);
        }
      }

      num_blks_to_search--;

      /* Break the PACK loop if no PACK blocks were found in the tx window */
      if( !num_blks_to_search && !found_pack_blck )
        break;

    } /* while */
  } /* repeat_sending_p_ack */
  else
  {
    /* If the data block is the last one in the TBF it shall be
    ** transmitted even if its state is PENDING_ACK and pre-emptive
    ** transmission is off. Check if V(A)<=bsn<V(S) before accessing the tx array.
    ** we can produce pack blocks until PACK FIFO is full.
    */
    while( num_blks                                                &&
           ul[gas_id].e_bsn_of_cv_0_block != E_GRLC_INVALID_BSN            &&
           IS_GE_MOD( ul[gas_id].e_bsn_of_cv_0_block,ul[gas_id].va,ul[gas_id].cur_res.ws)  &&
           IS_GT_MOD( ul[gas_id].vs, ul[gas_id].e_bsn_of_cv_0_block,ul[gas_id].cur_res.ws) &&
           ( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).vb == TXA_PACK         ||
             TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).vb == TXA_PENDING_PACK
           )
         )
    {
      if( grlc_ul_fifo_get_pack_cnt(gas_id) == RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE )
      {
        /* No enough space in the PL1 FIFO to send any PACK blocks.
        ** Exit this function and try again next time around.
        */
        MSG_GRLC_EXT_MED_0_G(GRLC_F3_MSG_ENUM_417, "GRLU PL1 FIFO full for PACK blks");
        return;
      }

      if( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_mcs !=
            TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs
        )
      {
        /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new
        ** PS value
        */
        ROTATE_PS_MCS_CHANGE ( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_mcs,
                               TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs,
                               TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_ps,
                               &(TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.ps)
                             );
      }

      if( IS_SPLIT_REQUIRED(TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.init_mcs,
                            TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs,
                            ul[gas_id].cur_res.resegment
                           )
        )
      {
        /* you want to produce split block as per num_blks as by producing by pack_cnt you will
           produce in access unnessarily */
        if( num_blks < 2 || ((RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE - grlc_ul_fifo_get_pack_cnt(gas_id)) < 2))
        {
          /* No enough space in the PL1 FIFO to send the two blocks created after
          ** the split. Exit this function and try again next time around.
          */
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_418, "GRLU PL1 FIFO full for split PACK with CV=0 bsn=%u",
                  ul[gas_id].e_bsn_of_cv_0_block,
                  0,
                  0);
          return;
        }
        /* one less block to send as split means 2 blocks
        */
        num_blks--;
      }

      /* In case of dual payload MCS, the MCS will be downgraded to a single
      ** payload MCS
      */
      if( IS_FORMING_DUAL_PAYLOAD_REQUIRED(TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs) )
      {
        grlc_ul_demote_mcs_modify_li( gas_id, ul[gas_id].e_bsn_of_cv_0_block );

        /* if MCS is demoted than get the correct CPS */
        if( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_mcs !=
            TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs
          )
        {
          /* MCS has changed. Use TS 04.60 table 9.3.2.1.1 to determine the new
          ** PS value
          */
          ROTATE_PS_MCS_CHANGE ( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_mcs,
                                 TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs,
                                 TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_ps,
                                 &(TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.ps)
                               );
        }
      }

      /* Set Coding and Puncturing Scheme (CPS) for the data block */
      GET_CPS( gas_id,
               ul[gas_id].e_bsn_of_cv_0_block,
               ind2,
               TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.hdr.cps
             );

      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_419, "ELRU e_tx petx=%u sending CV_0 bsn =%u num_blks=%u",
              ul[gas_id].pre_emptive_tx,
              ul[gas_id].e_bsn_of_cv_0_block,
              num_blks);

      /* Omit checking for return value because if FIFO is full we will drop back
      ** in here again as this is the ONLY data block to send
      */
      (void)e_send_block_to_pl1_fifo(gas_id, ul[gas_id].e_bsn_of_cv_0_block, ind2, TXA_PACK);

      /* one less block to send
      */
      num_blks--;

      /* Update old MCS/PS values and rotate PS to be ready for next re-tx */
      TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_mcs = TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs;
      TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_ps = TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.ps;
      ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.cs,
                               TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.old_ps,
                               TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.ps );
    }
  } /* repeat_sending_p_ack = FALSE */
} /* e_tx_blks() */

/*===========================================================================
===
===  FUNCTION           E_TX_BLKS_UNACK_MODE()
===
===  DESCRIPTION
===
===    This function searches the TX Array and sends a data block to the FIFO.
===    The transmit window in un-ack mode starts from V(S) and ends just before
===    at in_idex.
===
===    After a block is transmitted its ack status is set to TXA_PACK
===
===    ! to add: detect sending 64 blocks without receiving a paknak and
===    start T3182 (9.3.3.2 para 3)
===
===    In unack mode, only V(S) and In_index are used. V(A) is not used.
===
===    The Un-ack Transmit Window starts from V(S) and runs to before seg_ind.
===
===     <----------- SNS-block transmit workspace --------------->
=== ..............................X......................X............
===                               ^                  ^
===                               V(S)                   seg_ind
===                               <-- unack tx window -->
===
===  PARAMETERS
===
===    num_blks: specifies the maximum number of pending-tx blocks to send.
===
===  DEPENDENCIES
===
===    TX_ARR, vs, seg_ind etc..
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void e_tx_blks_unack_mode (gas_id_t gas_id, uint8 num_blks)
{
  /* loop count and common array index
  */
  uint16  b, ind1, max_to_send;
  uint16  ind2 = E_GRLC_INVALID_BSN;
  uint16  psn;

  /*----------------------------------------------------------------*/

  GRLC_UL_SHOW_TX_VARS(gas_id);

  /* exit if there is no block to transmit
  */
  if (num_blks == 0)
  {
#ifdef  GSM_GPRS_GRLU_SHOW_NO_TX_UNACK
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_420, "ERLU U blks=0 PL1space=%u seg=%u vs=%u",
            GET_PL1_FREE_SPACE(gas_id),
            ul[gas_id].seg_ind,
            ul[gas_id].vs);
#endif
    return;
  }

  /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can take
  ** a snapshot and use it throughout processing of UNACK blocks without reading again
  */
  ul[gas_id].inst_vs = ul[gas_id].vs;

  if (IS_GT_MOD(ul[gas_id].seg_ind, ul[gas_id].pack_vsf, ul[gas_id].cur_res.ws))
  {
    if (IS_GT_MOD(ul[gas_id].inst_vs, ul[gas_id].pack_vsf, ul[gas_id].cur_res.ws))
    {
      /* vsf behind vs so take it to vs value
      */
      ul[gas_id].pack_vsf = ul[gas_id].inst_vs;
      UPD_UL_VSF(gas_id);
    }
  }
  else if( IS_GT_MOD(ul[gas_id].pack_vsf, ul[gas_id].seg_ind, ul[gas_id].cur_res.ws) )
  {
    MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_421, "ERLU U pack vsf(%d) >= seg_ind(%d)",ul[gas_id].pack_vsf,ul[gas_id].seg_ind);
  }

  ind1 = ul[gas_id].pack_vsf;
  max_to_send = MIN(num_blks, MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].pack_vsf));

  b = 0;
  while ( (ind1 != ul[gas_id].seg_ind) && (b < max_to_send) )
  {
    /* Initialise 2nd payload index to invalid block. */
    ind2 = E_GRLC_INVALID_BSN;

    /* Assign PS 1 to the PTX data block before is sent to PL1 FIFO */
    TX_ARR(gas_id, ind1).blk.ps = GRLC_PS_1;

    /* The MCS for the initial transmission will be set to the block's
    ** MCS after the block has been confirmed as sent by L1 in post_tx_actions()
    */
    TX_ARR(gas_id, ind1).blk.init_mcs = RM_MCS_UNDEF;

    /* Demote the MCS in case the 2nd block falls outside Stall window
    ** Adding additional checking attach space - when empty demote the MCS.
    */
    if( IS_FORMING_DUAL_PAYLOAD_REQUIRED( TX_ARR(gas_id, ind1).blk.cs ) )
    {
      /* If the new MCS is 7/8 or 9 then a dual payload needs to be formed from two
      ** PTX blocks.
      */
      if( ( IS_GT_MOD(ul[gas_id].seg_ind, MOD_ESNS(ind1+1), ul[gas_id].cur_res.ws)) ||
          ( !E_GET_UNACK_WINDOW_SPACE(gas_id) )
        )
      {
        if( TX_ARR(gas_id, MOD_ESNS(ind1 + 1)).blk.cs == TX_ARR(gas_id, ind1).blk.cs &&
            TX_ARR(gas_id, MOD_ESNS(ind1 + 1)).vb == TX_ARR(gas_id, ind1).vb
          )
        {
          ind2 = MOD_ESNS(ind1 + 1);

          /* Assign PS 1 to the PTX data block before is sent to PL1 FIFO */
          TX_ARR(gas_id, ind2).blk.ps = GRLC_PS_1;

          /* The MCS for the initial transmission will be set to the block's
          ** MCS after the block has been confirmed as sent by L1 in post_tx_actions()
          */
          TX_ARR(gas_id, ind2).blk.init_mcs = RM_MCS_UNDEF;
        }
        else
        {
          /* Commanded MCS changed. Dowmgrade the MCS to send a single payload block */
          grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
        }
      }
      else
      {
        if( ul[gas_id].segment_state == SEGMENT_DONE )
        {
          grlc_ul_demote_mcs_modify_li( gas_id, ind1 );
        }
        else
        {
          /* Segmentation has not finished. Form dual payload next time around.
          */
          return;
        }
      }
    } /* Formating dual payload */

    if( (ind2 != E_GRLC_INVALID_BSN) )
    {
      /* Set BSN2 number which is relevent to BSN1 */
      TX_ARR(gas_id, ind1).blk.hdr.bsn2 = MOD_DIF_ESNS( ind2 , ind1);
    }

    /* Set Coding and Puncturing Scheme (CPS) for the data block */
    GET_CPS( gas_id, ind1, ind2, TX_ARR(gas_id, ind1).blk.hdr.cps );

    /* Update CV, MAC header (SI..) and send block to FIFO.
    ** Over-ride ack status with TXA_UNACK to ensure block goes to PACK area
    */
    if (e_send_block_to_pl1_fifo(gas_id, ind1, ind2, TXA_UNACK))
    {
      /* Update the BSN of the block with CV = 0.
      ** During un-ack release this block may need to be re-sent
      */
      if (TX_ARR(gas_id, ind1).blk.hdr.cv == 0)
      {
        MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_422, "ERLU U tx CV=0 bsn=%d",ind1);

        /* Before marking the BSN value of the block with CV 0, try to free the
        ** dsm_ptr of the previous BSN with CV 0. Since in un-ack mode, we free
        ** all PDUs behind the current PDU
        */
        if (ul[gas_id].e_bsn_of_cv_0_block != E_GRLC_INVALID_BSN)
        {
          if (TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.dsm_ptr != NULL &&
              ind1 != ul[gas_id].e_bsn_of_cv_0_block
             )
          {
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_423, "ERLU free prev CV_0 bsn (%u)",
                    ul[gas_id].e_bsn_of_cv_0_block,
                    0,
                    0
                   );
            GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.dsm_ptr);
          }
        }
        MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_424, "ERLU new ind1 e_bsn_of_cv_0_block = %u", ul[gas_id].e_bsn_of_cv_0_block);
        ul[gas_id].e_bsn_of_cv_0_block = ind1;
        ul[gas_id].re_tx_cv_0_cnt = 0;
      }
      else
      {
        ul[gas_id].e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;
      }

      if( (ind2 != E_GRLC_INVALID_BSN) )
      {
        /* Update pack_vsf to point to the 2nd block */
        ul[gas_id].pack_vsf = ind2;

        /* Mark the bsn with CV = 0 */
        if (TX_ARR(gas_id, ind2).blk.hdr.cv == 0)
        {
          MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_425, "GRLU tx CV=0 bsn=%d",ind2);

          /* Before marking the BSN value of the block with CV 0, try to free the
          ** dsm_ptr of the previous BSN with CV 0. Since in un-ack mode, we free
          ** all PDUs behind the current PDU
          */
          if (ul[gas_id].e_bsn_of_cv_0_block != E_GRLC_INVALID_BSN)
          {
            if (TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.dsm_ptr != NULL &&
                ind2 != ul[gas_id].e_bsn_of_cv_0_block
               )
            {
              MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_426, "ERLU free prev CV_0 bsn (%u)",
                      ul[gas_id].e_bsn_of_cv_0_block,
                      0,
                      0
                     );
              GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].e_bsn_of_cv_0_block).blk.dsm_ptr);
            }
          }

          ul[gas_id].e_bsn_of_cv_0_block = ind2;
          ul[gas_id].re_tx_cv_0_cnt = 0;
          MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_427, "ERLU new ind2 e_bsn_of_cv_0_block = %u", ul[gas_id].e_bsn_of_cv_0_block);
        }
        else
        {
          ul[gas_id].e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;
        }
      }

      INC_MOD(ul[gas_id].pack_vsf,ESNS);
      UPD_UL_PACK_VSF(gas_id);

      --num_blks;
    }
    else
    {
#ifdef  GSM_GPRS_GRLU_SHOW_E_TX_PTX_PACK_BLKS
      MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_428, "ERLU Info UNACK PACK FIFO full ind1=%d ind2=%d",ind1,ind2);
#endif
        break;
    }

    b++;

    /* Get next PTX block */
    ind1 = ul[gas_id].pack_vsf;

  } /* loop sending un-ack blocks */

  /*-------------------------------------------------------------------------
  ** Free all PDUs upto 1 block before VS when VS is not 0.
  ** Get the data that has been ack'ed as sent by PL1 and seek the last PSN.
  ** This is one position before VS
  **-------------------------------------------------------------------------
  */
  if (ul[gas_id].vs != 0)
  {
    ind1 = (ul[gas_id].vs + ESNS - 1)% ESNS;
    psn = TX_ARR(gas_id, ind1).blk.last_psn;

    /* UNACK mode, no need to wait for ACKs, free PDUs up to the
    ** PDU before the current one in case of reprocessing.
    */
    psn = (psn + 32768 - 1) % 32768;

    if (IS_GT_MODULO_COMP(psn,ul[gas_id].first_psn_to_free,32768))
    {
      /* Successful transmission of PDU , set the flag to TRUE
      ** to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn(gas_id, psn,TRUE);
      ul[gas_id].first_psn_to_free = (psn+1)%32768;
      MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_429, "ERLU U post free %u NpOctets=%08x 1st_to_free=%u",
              psn,
              grlc_ul_pack_queue_info(gas_id),
              ul[gas_id].first_psn_to_free);
    }
  }
} /* e_tx_blks_unack_mode */

/*===========================================================================
===
===  FUNCTION           tx_blks()
===
===  DESCRIPTION
===
===    Transmit the radio blocks in the Transmit Array in the order of
===    unacked blocks (TXA_NACK), followed by unsent blocks
===    (TXA_PTX), followed by pending ack blocks (TXA_PACK) as per 9.1.3
===
===    The number of blocks to send include all ack types: pending_tx,
===    pending_ack and nacked. The blocks to send are in the transmit window
===    (between V(A) and V(S), and the attach window (between V(S) and
===    seg_ind).
===
===    After a block is sent the resource (blocks_granted) is not reduced only
===    after it is confirmed as transmitted by PL1/NPL1, ie. after a GET-access
===    is made.
===
===    After sending a NACK block to the FIFO only nack_vsf is updated.
===    After sending a PTX block to the FIFO only vsf is updated.
===    After sending a PACK block to the FIFO only pack_vsf is updated.
===
===    The variable V(A), and V(S) are adjusted as the blocks are sent.
===
===    The SI indicator and timer T3182 are set and reset as the transmit
===    window becomes stalled or un-stalled.
===
===  DESIGN
===
===    if NACK block(s) exist
===       repeat send NACK block(s) to FIFO
===       adjust ack status
===    end NACK
===
===    if (stall)
===       set flag to deal with PACK blocks
===    else (not stall)
===       if PTX block(s) exist
===          send PTX block(s)
===          adjust ack status
===       endif
===
===       if num_blocks still > 0
===          set flag to deal with sending PACK blocks
===       endif
===    endifelse
===
===    if flag to deal with sending PACK blocks is TRUE
===       repeat sending PACK blocks
===    endif
===
===
===
===  <-------------------- SNS-blocks transmit workspace ----------------------->
===  |..X...................X...X........X.......................................|
===     ^                   ^   ^        ^
===     VA                  VS  VSF      seg_ind
===     <--nack_vsf range-->
===     <--pack_vsf range-->
===                         <-vsf range->
===
===
===  PARAMETERS
===
===    num_blks: specifies the maximum number of pending-tx blocks to send which
===              is the space available in the FIFO
===
===  DEPENDENCIES
===
===    TX_ARR, vs, va, seg_ind etc..
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul[gas_id].si,
===    TX_ARR(gas_id, )
===
===========================================================================*/
static void tx_blks ( gas_id_t gas_id, uint8  num_blks )
{
  /* loop count and array index */
  uint8       ind, ofs;

  uint16      num_blks_to_search;

  /* flag to indicate repeat sending of pending tx blocks
  */
  boolean     b_repeat_sending_p_ack;

  /* Set when a return is to be forced from half way throu code segment,
  ** such that an INTFREE() will only be activiated once
  */
  boolean     b_ret = FALSE;

  /*------------------------------------------------------------------*/

  /* exit when there is no block to transmit
  */
  GRLC_UL_SHOW_TX_VARS(gas_id);

  if (num_blks == 0)
  {
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_430, "GRLU tx num_blks = 0 va=%d vs=%d vsf=%d",ul[gas_id].va, ul[gas_id].vs, ul[gas_id].vsf);
    return;
  }

  /* sending pending_ack blocks are required in both non-stall and stall
  ** conditions. To save code space a flag is set when this action is
  ** required. The flag is tested and if TRUE sending pending_ack is
  ** executed in one place at the end of the function. First we need to
  ** clear this flag.
  */
  b_repeat_sending_p_ack = FALSE;

  /* if RLC gets context and hangs on to it for longer than, say 1 frame, NPL1
  ** could potentially accesses the FIFO again whilst RLC is working on it.
  ** So we must lock out NPL1 access to this. Provided this function should take
  ** no longer than 1 frame to produce a maximum of 4 data blocks to send to the
  ** FIFO, it is safe to do so.
  **
  ** This INTLOCK() could be refined to lock just NPL1 ISRs
  */

  /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can take
  ** a snapshot and use it throughout processing of NACK blocks without reading again
  */
  ul[gas_id].inst_vs = ul[gas_id].vs;

  /*====================================================
  ** Search TX Array and send NACKED blocks to UL FIFO
  **====================================================
  */
  if (ul[gas_id].va != ul[gas_id].inst_vs)
  {
    /*----------------------------------------------------------------------
    ** Scan and send NACKED blocks first.
    ** If the last nack block sent is within the tx window (between
    ** V(A) and V(S)) then we want to resume from that point on. Otherwise,
    ** resume the search from V(A).
    **----------------------------------------------------------------------
    */
    if (IS_GE_MODULO_COMP(ul[gas_id].nack_vsf, ul[gas_id].inst_vs, SNS) ||
        IS_GT_MODULO_COMP(ul[gas_id].va,ul[gas_id].nack_vsf, SNS)
       )
    {
      ul[gas_id].nack_vsf = ul[gas_id].va;
      UPD_UL_NACK_VSF(gas_id);
      ofs = 0;
    }
    else
    {
      /* V(A) <= nack_vsf < V(S), ie. in range
      */
      ofs = (uint8)MOD_DIF(ul[gas_id].nack_vsf, ul[gas_id].va, SNS);
    }

    /* Window size is the distance between V(A) and V(S).
    ** Work out the remaining number of blocks to search for a NACK block which
    ** is between nack_vsf and V(S)
    */
    ind = MOD_SNS(ul[gas_id].va+ofs);
    num_blks_to_search = (uint8)MOD_DIF(ul[gas_id].inst_vs, ul[gas_id].nack_vsf, SNS);

    while (num_blks_to_search && num_blks)
    {
      if (TX_ARR(gas_id, ind).vb == TXA_NACK)
      {
        /* send block to FIFO and update vars if FIFO is not full
        */
        if (send_block_to_pl1_fifo(gas_id, ind, TXA_NACK))
        {
          TX_ARR(gas_id, ind).vb = TXA_PENDING_PACK;

          /* reduce the number of blocks to send within this call
          */
          --num_blks;

          /* Update nack_vsf.
          ** nack_vsf won't go beyond V(S) as the loop terninates before that
          */
          ul[gas_id].nack_vsf = ind;
          INC_MOD(ul[gas_id].nack_vsf, SNS);
          UPD_UL_NACK_VSF(gas_id);

          if (IS_GT_MODULO_COMP(ul[gas_id].nack_vsf, ul[gas_id].pack_vsf, SNS))
          {
            ul[gas_id].pack_vsf = ul[gas_id].nack_vsf;
            UPD_UL_PACK_VSF(gas_id);
          }
        }
        else
        {
          if (ul[gas_id].b_error_recovery_required)
            return;

          /*MSG_HIGH("GRLU Info tx NAK FIFO full ind=%d num_blks=%d",ind,num_blks,0);*/

          /* Exit loop and drop out at the bottom and execute INTFREE before returning
          */
          b_ret = TRUE;
          num_blks = 0;
          break;
        }
      } /* block is NACK */

      ofs++;
      num_blks_to_search--;

      /* Index to next nack block relative to V(A) modulo SNS.
      ** ind can not go beyond V(S)
      */
      ind = MOD_SNS(ul[gas_id].va+ofs);
    } /* send loop */
  } /* processing NACK blocks */

  if (!b_ret)
  {
    /*------------------------------------
    ** Deal with PTX and PACK blocks next
    **------------------------------------
    */

    if (IS_TX_WINDOW_FULL(gas_id))
    /*----------------------------------------------------------------------
    ** Deal with the tx window stall case. The NACKED blocks had been dealt
    ** with so deal with the PACK blocks next since the PTX blocks are not
    ** available as the window has stalled.
    **----------------------------------------------------------------------
    */
    {
      /* set this flag to deal with PACK blocks at the end section
      */
      if (num_blks > 0)
        b_repeat_sending_p_ack = TRUE;

      if (ul[gas_id].si == RM_TX_NOT_STALLED)
      {
        /* Don't start T3182 timer here since the block will sit in the FIFO.
        ** post_tx_actions() will start T3182 if SI is set and T3182
        ** is not active
        */
        ul[gas_id].si = RM_TX_STALLED;
#ifdef  GSM_GPRS_GRLU_SHOW_SI
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_431, "GRLU tx SI=1 va=%d vs=%d seg=%d",ul[gas_id].va,ul[gas_id].vs,ul[gas_id].seg_ind);
#endif
      }
    } /* end window stall */
    else
    /*---------------------------------------------------------------
    ** Deal with non-stall condition by checking and sending and the
    ** PTX blocks followed by checking and sending of PACK blocks
    **---------------------------------------------------------------
    */
    {
      /* post_tx_actions() invoked by PL1 after a block is transmitted from the DSP
      ** buffer alters V(S). Therefore all operations relating to V(S) must be
      ** protected from INTerrupts
      */

      /*=================================================
      ** Search TX Array and send PTX blocks to UL FIFO
      **=================================================
      */
      if (num_blks > 0 &&
          ul[gas_id].vsf != ul[gas_id].seg_ind
         )
      {
        /* Preset offset value to 0
        */
        ofs= 0;

        /* Check if vsf is inside V(S) and seg_ind and work out the offset
        ** relative to V(S)
        */

        /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can
        ** take a snapshot and use it throughout processing of PTX blocks without reading
        ** again
        */
        ul[gas_id].inst_vs = ul[gas_id].vs;

        if (IS_GT_MODULO_COMP(ul[gas_id].seg_ind, ul[gas_id].vsf, SNS))
        {
          if (IS_GE_MODULO_COMP(ul[gas_id].vsf, ul[gas_id].inst_vs, SNS))
          {
            ofs = (uint8)MOD_DIF(ul[gas_id].vsf, ul[gas_id].inst_vs, SNS);
          }
          else
          {
            /* vsf behind vs so take it to vs value
            */
            ul[gas_id].vsf = ul[gas_id].inst_vs;
            UPD_UL_VSF(gas_id);
          }
        }
        else
        {
          MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_432, "GRLU ptx vsf(%d) >= seg_ind(%d)",ul[gas_id].vsf,ul[gas_id].seg_ind);
        }

        /* index to first block to search
        */
        ind = MOD_SNS(ul[gas_id].inst_vs + ofs);

        /* Send loop. Terminate when num_blks runs out or the vsf reaches the
        ** seg_ind where segmentation stops
        */
        while ( num_blks &&
                IS_GT_MODULO_COMP(ul[gas_id].seg_ind, ind, SNS)
              )
        {
          if (TX_ARR(gas_id, ind).vb == TXA_PTX)
          {
            /* send block to FIFO with current status as PTX
            */
            if (send_block_to_pl1_fifo(gas_id, ind, TXA_PTX))
            {
              /* increment VSF. No need to check for wrap-round or overflow
              ** because 'num_blks_to search' puts a limit at seg_ind
              */
              ul[gas_id].vsf = ind;
              INC_MOD(ul[gas_id].vsf, SNS);
              UPD_UL_VSF(gas_id);

              /* one less block to send
              */
              num_blks--;
            }
            else
            {
              if (ul[gas_id].b_error_recovery_required)
                return;

              /*MSG_HIGH("GRLU Info PTX FIFO full ind %d",ind,0,0);*/
              /* to exit loop and drop out of the sending PTX blocks section and
              ** execute an INTFREE() before returning
              */
              b_ret = TRUE;
              break;
            }
          } /* if finding PTX block */

          /* get the index mod SNS
          */
          ofs++;
          ind = MOD_SNS(ul[gas_id].inst_vs + ofs);

        } /* loop sending PTX */
      } /* processing PTX blocks */

      /*--------------------------------------------------------------------------
      ** If the number of blocks to send specified has not been exhausted after
      ** sending NACK and PTX blocks then consider sending PACK blocks by setting
      ** the flag below, but only if:
      ** - Open-ended and the current PDU is the last of this TBF
      ** - Close-ended and the current PDU is the last of this TBF
      **--------------------------------------------------------------------------
      */
      if ((num_blks > 0) &&
          /* TX window stalls
          */
          ((! GET_ATTACH_WINDOW_SPACE(gas_id)) ||

          /* No more PTX blocks to send in Dynamic Open-ended TBF or
          ** Dynamic Close-ended TBF
          */
           ((ul[gas_id].segment_state == SEGMENT_DONE) &&
            (IS_DYNAMIC_OPEN(gas_id) || IS_DYNAMIC_CLOSE(gas_id))
           )
          )
         )
      {
        b_repeat_sending_p_ack = TRUE;
        MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_433, "GRLU tx ena-PACKs p=%u vs=%d in=%d",cur_pdu_ptr[gas_id]->psn,ul[gas_id].vs,ul[gas_id].seg_ind);

        /* TX Window stall , set the stall indicator
        */
        if (! GET_ATTACH_WINDOW_SPACE(gas_id))
        {
          if (ul[gas_id].si == RM_TX_NOT_STALLED)
          {
            /* Don't start T3182 timer here since the block will sit in the FIFO.
            ** post_tx_actions() will start T3182 if SI is set and T3182
            ** is not active
            */
            ul[gas_id].si = RM_TX_STALLED;
#ifdef  GSM_GPRS_GRLU_SHOW_SI
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_434, "GRLU tx SI=1 va=%d vs=%d seg=%d",ul[gas_id].va,ul[gas_id].vs,ul[gas_id].seg_ind);
#endif
          }
        }
      }
    } /* non-stall window */

    if (!b_ret)
    {
      /*===========================================================================
      ** Search TX Array and send PACK blocks to UL FIFO if there are any.
      **
      ** pack_vsf marks the block beyond where the previous PACK block was sent.
      ** This is the start of the search for the next PACK block to send to the
      ** FIFO. When pack_vsf reaches V(S) it wraps back to V(A) to seek the oldest
      ** un-ack block to send.
      **===========================================================================
      */

      /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can
      ** take a snapshot and use it throughout processing of PACK blocks without reading
      ** again
      */
      ul[gas_id].inst_vs = ul[gas_id].vs;

      if (b_repeat_sending_p_ack)
      {
        /* If the last pending_ack block is within the tx window (between V(A) and
        ** V(S)) then we want to resume from that point on. Otherwise start sending
        ** pending_ack from V(A)
        */
        if (IS_GT_MODULO_COMP(ul[gas_id].inst_vs, ul[gas_id].pack_vsf, SNS) &&
            IS_GE_MODULO_COMP(ul[gas_id].pack_vsf, ul[gas_id].va, SNS)
           )
        {
          /* Find where that block should be relative to V(A)
          */
          ofs = (uint8)MOD_DIF(ul[gas_id].pack_vsf, ul[gas_id].va, SNS);
        }
        else
        {
          /* pack_vsf outside V(A) and V(S) so reset it to V(A)
          */
          ul[gas_id].pack_vsf = ul[gas_id].va;

          ofs = 0;
        }

        /* In MS Class 12, there might be PTX blks in the FIFO that have not been processed
        ** (post txed) leaving the vs behind vsf. This can cause multiple PACK transmission
        ** on the same BSN in the PACK FIFO which will confuse the peer entity.
        ** For example PTX blks with BSN 0,1,2,3 and 4 with CV 0, has been send to the PTX
        ** FIFO but only BSN 0 has been processed by PL1. Since there are no more PTX blks
        ** to send, PACK blks of BSN0 will fill up the PACK FIFO. To prevent this, PACK blks
        ** will be produced only if pack_vsf is behind vs or if vs is equal to vsf.
        ** Note that this same check is added every time we update pack_vsf in the while
        ** loop below.
        */
        if( ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs != ul[gas_id].vsf )
        {
          return;
        }
        else if (ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs == ul[gas_id].vsf)
        {
          ul[gas_id].pack_vsf = ul[gas_id].va;

          ofs = 0;
        }

        num_blks_to_search = GET_TX_WINDOW_SIZE(gas_id);

        /* ! while (num_blks && IS_TX_WINDOW_FULL())
        */
        while (num_blks &&
               num_blks_to_search &&
               !b_ret &&
               ul[gas_id].va != ul[gas_id].inst_vs
              )
        {
          /* Find index of the start of the tx window
          */
          ind = MOD_SNS(ul[gas_id].va+ofs);

          /*-------------------------------------------------------------------------
          ** Only send PACKs that have been pending ack before this function is
          ** called and not the re-sent NACKED blocks. PTX blocks should not be sent
          ** but because PTX blocks will eventually be sent and become PACK blocks
          ** we also send PTX blocks.
          **-------------------------------------------------------------------------
          */
          if (TX_ARR(gas_id, ind).vb == TXA_PACK)
          {
            if (send_block_to_pl1_fifo(gas_id, ind, TXA_PACK))
            {
              /* update the bsn of the next pending_ack block
              */
              ul[gas_id].pack_vsf = ind;
              INC_MOD(ul[gas_id].pack_vsf, SNS);

              if( ul[gas_id].pack_vsf == ul[gas_id].inst_vs && ul[gas_id].inst_vs != ul[gas_id].vsf )
              {
                return;
              }
              else
              {
                if (IS_GT_MODULO_COMP(ul[gas_id].pack_vsf, ul[gas_id].vsf,SNS))
                {
                  ul[gas_id].pack_vsf = ul[gas_id].va;
                }
                UPD_UL_PACK_VSF(gas_id);
              }

              /* one less block to send
              */
              num_blks--;
            }
            else
            {
              if (ul[gas_id].b_error_recovery_required)
                return;

              /*MSG_HIGH("GRLU Info PAK FIFO full ind %d",ind,0,0);*/

              /* exit loop and drop out the sending PACK blokcs section and
              ** execute an INTFREE() before returning
              */
              b_ret = TRUE;
              break;
            }
          }
          else
          {
            if (TX_ARR(gas_id, ind).vb == TXA_PENDING_PACK)
            {
#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_435, "GRLU P_PACK %d becomes PACK (PL1 mis-ack)",ind);
#endif
              TX_ARR(gas_id, ind).vb = TXA_PACK;
            }
            else if(TX_ARR(gas_id, ind).vb == TXA_PTX)
            {
              /* when post_tx_actions() misses acknowleging PTX blocks and cause
              ** the BSN not to be continuous with VS then make these PTX blocks
              ** in PACK range PACK blocks. Otherwise they don't get re-transmitted
              ** and get stuck in PTX state. Also update VS in the process
              */
              if (++(TX_ARR(gas_id, ind).blk.ptx_miss_cnt) > MAX_MISS_CNT)
              {
#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
                MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_436, "GRLU tx PTX %d becomes PACK (PL1 mis-ack)",ind);
#endif
                TX_ARR(gas_id, ind).vb = TXA_PACK;
                if (IS_GE_MODULO_COMP(ind,ul[gas_id].vs,SNS))
                {
                  TX_ARR(gas_id, ind).blk.ptx_miss_cnt = 0;

                  ul[gas_id].vs = ind;
                  INC_VS(gas_id);
                }
              }
            }
          }

          /* if exhausted then wrap round again
          */
          ++ofs;

          if (IS_GE_MODULO_COMP((ul[gas_id].va+ofs),ul[gas_id].vsf,SNS))
          {
            ofs = 0;
          }

          /* Searching for PACK blocks wraps around but only for a maximum
          ** length of the tx window size between V(A) and V(S).
          */
          --num_blks_to_search;

        } /* while */
      } /* repeat_sending_p_ack */
    } /* b_ret == FALSE */
  } /* b_ret == FALSE */
} /* tx_blks() */


/*===========================================================================
===
===  FUNCTION           TX_BLKS_UNACK_MODE()
===
===  DESCRIPTION
===
===    This function searches the TX Array and sends a data block to the FIFO.
===    The transmit window in un-ack mode starts from V(S) and ends just before
===    at in_idex.
===
===    After a block is transmitted its ack status is set to TXA_PACK
===
===    ! to add: detect sending 64 blocks without receiving a paknak and
===    start T3182 (9.3.3.2 para 3)
===
===    In unack mode, only V(S) and In_index are used. V(A) is not used.
===
===    The Un-ack Transmit Window starts from V(S) and runs to before seg_ind.
===
===     <----------- SNS-block transmit workspace --------------->
=== ..............................X......................X............
===                               ^                  ^
===                               V(S)                   seg_ind
===                               <-- unack tx window -->
===
===  PARAMETERS
===
===    num_blks: specifies the maximum number of pending-tx blocks to send.
===
===  DEPENDENCIES
===
===    TX_ARR, vs, seg_ind etc..
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void tx_blks_unack_mode (gas_id_t gas_id, uint8 num_blks)
{
  /* loop count and common array index
  */
  uint8   b, ind, max_to_send, ofs;
  uint16  psn;

  /*----------------------------------------------------------------*/

  GRLC_UL_SHOW_TX_VARS(gas_id);

  /* exit if there is no block to transmit
  */
  if (num_blks == 0)
  {
#ifdef  GSM_GPRS_GRLU_SHOW_NO_TX_UNACK
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_437, "GRLU U blks=0 PL1space=%d seg=%d vs=%d",
            GET_PL1_FREE_SPACE(gas_id),
            ul[gas_id].seg_ind,
            ul[gas_id].vs);
#endif
    return;
  }

  /* As the value of VS (ul[gas_id].vs) is refreshed by the ISR in post_tx_actions() we can take
  ** a snapshot and use it throughout processing of UNACK blocks without reading again
  */
  ul[gas_id].inst_vs = ul[gas_id].vs;

  if (IS_GT_MODULO_COMP(ul[gas_id].seg_ind, ul[gas_id].pack_vsf, SNS))
  {
    if (IS_GE_MODULO_COMP(ul[gas_id].pack_vsf, ul[gas_id].inst_vs, SNS))
    {
      /* pack_vsf is inside V(S) and seg_ind
      */
      ofs = (uint8)MOD_DIF(ul[gas_id].pack_vsf, ul[gas_id].inst_vs, SNS);
    }
    else
    {
      /* pack_vsf is behind V(S).
      */
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_438, "GRLU tx_u pack_vsf < V(S)");
      ul[gas_id].pack_vsf = ul[gas_id].inst_vs;
      UPD_UL_PACK_VSF(gas_id);

      ofs = 0;
    }
  }
  else
  {
    /* pack_vsf = seg_ind
    ** no blocks to send
    */
    ofs = (uint8)MOD_DIF(ul[gas_id].pack_vsf, ul[gas_id].inst_vs, SNS);
  }

  ind = MOD_SNS(ul[gas_id].inst_vs + ofs);

  max_to_send = (uint8)MIN(num_blks, MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].pack_vsf, SNS));
  if (max_to_send > SNS)
    max_to_send -= SNS;

  b = 0;
  while ( (ind != ul[gas_id].seg_ind) && (b < max_to_send) )
  {
    /* Update CV, MAC header (SI..) and send block to FIFO.
    ** Over-ride ack status with TXA_UNACK to ensure block goes to PACK area
    */
    if (send_block_to_pl1_fifo(gas_id, ind, TXA_UNACK))
    {
      /* Update the BSN of the block with CV = 0.
      ** During un-ack release this block may need to be re-sent
      */
      if (TX_ARR(gas_id, ind).blk.mac_hdr.type.ul_data.cv == 0)
      {
        MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_439, "GRLU U tx CV=0 bsn=%d",ind);

        /* Before marking the BSN value of the block with CV 0, try to free the
        ** dsm_ptr of the previous BSN with CV 0. Since in un-ack mode, we free
        ** all PDUs behind the current PDU
        */
        if (ul[gas_id].bsn_of_cv_0_block != GRLC_INVALID_BSN)
        {
          if (TX_ARR(gas_id, ul[gas_id].bsn_of_cv_0_block).blk.dsm_ptr != NULL &&
              ind != ul[gas_id].bsn_of_cv_0_block
             )
          {
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_440, "GRLU free prev CV_0 bsn (%u)",
                    ul[gas_id].bsn_of_cv_0_block,
                    0,
                    0
                   );
            GPRS_PDU_FREE(&TX_ARR(gas_id, ul[gas_id].bsn_of_cv_0_block).blk.dsm_ptr);
          }
        }

        ul[gas_id].bsn_of_cv_0_block = ind;
        ul[gas_id].re_tx_cv_0_cnt = 0;
        MSG_GRLC_EXT_HIGH_1_G(GRLC_F3_MSG_ENUM_441, "GRLU new bsn_of_cv_0_block = %u",ul[gas_id].bsn_of_cv_0_block);
      }
      else
      {
        ul[gas_id].bsn_of_cv_0_block = GRLC_INVALID_BSN;
      }

      INC_MOD(ul[gas_id].pack_vsf,SNS);
      UPD_UL_PACK_VSF(gas_id);

      --num_blks;
    }
    else
    {
      if (ul[gas_id].b_error_recovery_required)
        return;

      break;
    }

    b++;

    ofs++;
    ind = MOD_SNS(ul[gas_id].inst_vs + ofs);
  } /* loop sending un-ack blocks */

  /*-------------------------------------------------------------------------
  ** Free all PDUs upto 1 block before VS when VS is not 0.
  ** Get the data that has been ack'ed as sent by PL1 and seek the last PSN.
  ** This is one position before VS
  **-------------------------------------------------------------------------
  */
  if (ul[gas_id].vs != 0)
  {
    ind = (ul[gas_id].vs + SNS - 1)%SNS;
    psn = TX_ARR(gas_id, ind).blk.last_psn;

    /* Step back 1 PDU
    */
    psn = (psn + 32768 - 1) % 32768;

    if (IS_GT_MODULO_COMP(psn,ul[gas_id].first_psn_to_free,32768))
    {
      /* Successful transmission of PDU , set the flag to TRUE
      ** to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn(gas_id, psn,TRUE);
      ul[gas_id].first_psn_to_free = (psn+1)%32768;
      MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_442, "GRLU U post free %u NpOctets=%08x 1st_to_free=%u",
              psn,
              grlc_ul_pack_queue_info(gas_id),
              ul[gas_id].first_psn_to_free);
    }
  }
} /* tx_blks_unack_mode */

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
void tx_blks_shell ( gas_id_t gas_id )
{
  ul[gas_id].free_space = GET_PL1_FREE_SPACE(gas_id);
  ul[gas_id].max_blks = MAX_BLOCKS_TO_SEND;
  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS)
  {
    if(ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      tx_blks(gas_id, (uint8)MIN3(ul[gas_id].free_space,
                          ul[gas_id].max_blks,
                          GET_ATTACH_WINDOW_SIZE(gas_id)
                         )
             );
    }
    else
    {
      tx_blks_unack_mode( gas_id,
                          (uint8)
                          MIN3(ul[gas_id].free_space,
                               RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE,
                               MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].vs, SNS)
                              )
                        );
    }
  }
  else
  {
    if(ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      e_tx_blks(gas_id, (uint8)MIN(ul[gas_id].free_space,ul[gas_id].max_blks));
    }
    else
    {
      e_tx_blks_unack_mode( gas_id,
                           (uint8)
                            MIN3(ul[gas_id].free_space,
                                 RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE,
                                 MOD_DIF_ESNS(ul[gas_id].seg_ind,ul[gas_id].vs)
                                )
                          );
    }
  }
} /* tx_blks_shell */

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
)
{
  uint16          bsn;
  uint16          ind;
  uint8           loop_cnt;
  uint8 buf[6];   /* EGPRS Header is 6 bytes Max.. */

#if 0
  /*#if (! (defined(DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined(PERLUTF)) )*/
  /*---------------------------------------------------------------------
  ** CR 112411
  ** Attempt to trap entity writing to RLC t3182's TCB
  **    timer_t3188.tcb = 0x6666DA68
  **    instead of
  **    timer_t3188.tcb = 0x0166DA68 (gprs_rlc_ul_tcb)
  **---------------------------------------------------------------------
  */
  uint32  tcb = 0L;

  /* Get MSByte
  */
  tcb = (uint32)ul[gas_id].timer_t3182.rex_timer.tcb >> 24;

  /* 6250, 7200 builds place gprs_rlc_ul_tcb at    0x01xxxxxx
  ** 6275, 6280/40 builds place gprs_rlc_ul_tcb at 0x02xxxxxx
  ** So any MSByte that is not 01 NOR 02 is bogus
  */
  if (tcb != 0x01 &&
      tcb != 0x02
     )
  {
    ERR_GERAN_FATAL_1_G("GRLU post_tx bad timer_tcb MSB=%u",tcb);
  }
#endif

  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    /*---------------------------------------------------------------*/

    if( (blk_ptr->blk_ptr->elm.egprs.mcs > RM_MCS_6) &&
        (blk_ptr->blk_ptr->elm.egprs.mcs <= RM_MCS_9)   )
    {
      /* PL1 block sent a dual payload RLC block */
      loop_cnt = 2;
    }
    else
    {
      /* PL1 block sent a single payload RLC block */
      loop_cnt = 1;
    }

    /* get BSN of data block just transmmitted given pointer to PL1 UL
    ** radio block which includes PL1 block header. BSN is in RLC header
    */
    bsn = E_GET_RAW_BSN(blk_ptr->blk_ptr->elm.egprs.hdr);

    while( loop_cnt )
    {
      /*--------------------------------
      ** Handle NACK block transmitted
      **--------------------------------
      */
      /* This function is called by GL1. Hence, mutex lock is needed.
       * However, protection is done only for NACK blocks to avoid RLC updating
       * these type of blocks to ACK after geting PUAN at the same time */
      GRLC_UL_TX_VARS_LOCK(gas_id);
      if (TX_ARR(gas_id, bsn).vb == TXA_NACK ||
          TX_ARR(gas_id, bsn).vb == TXA_PENDING_PACK
         )
      {
        grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
        GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);

        /* Status becomes PACK
        */
        TX_ARR(gas_id, bsn).vb = TXA_PACK;
        ul[gas_id].nack_cnt++;
        GRLC_UL_TX_VARS_UNLOCK(gas_id);
      }
      else
      /*--------------------------------
      ** Handle PTX block transmitted
      **--------------------------------
      */
      {
        GRLC_UL_TX_VARS_UNLOCK(gas_id);
        if (TX_ARR(gas_id, bsn).vb == TXA_PTX)
        {
          grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
          GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
  
          /* Restart T3182 for every CV = 0 block
          ** T3182 is now only started in TBF suspend state, hence if running
          ** and another CV=0 block is sent restart it.
          */
          if (IS_EXT_UTBF_ACTIVE(gas_id) &&
              TX_ARR(gas_id, bsn).blk.hdr.cv == 0 &&
              ul[gas_id].timer_t3182.state == TMR_ACTIVE &&
              (seek_block_type(gas_id, ul[gas_id].va, ul[gas_id].vs, TXA_NACK)==E_GRLC_INVALID_BSN)
             )
          {
  
            TIMER_START_T3182(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_T3182
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_443, "ERLU EXT_UTBF T3182 started CV=0 or stalled bsn=%u e_bsn_of_cv_0=%u",
                    bsn,
                    ul[gas_id].e_bsn_of_cv_0_block,
                    0);
#endif
          }
  
          /* If the (main) T3182 timer is NOT active then check for the Guard Timer
          ** When the Guard Timer State is GUARD_OFF/ON/CV_ZERO and
          ** Ext UTBF mode.is active and
          ** When PTX block has CV = 0
          ** then re/start the extended uplink tbf guard timer
          */
          if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
          {
            if (((ul[gas_id].guard_timer_state == GUARD_OFF) ||
                 (ul[gas_id].guard_timer_state == GUARD_ON)  ||
                 (ul[gas_id].guard_timer_state == CV_ZERO)
                ) &&
                (IS_EXT_UTBF_ACTIVE(gas_id)) &&
                (TX_ARR(gas_id, bsn).blk.hdr.cv == 0)
               )
            {
              ul[gas_id].guard_timer_state = CV_ZERO;
              TIMER_START_AWAIT_UL_ACKNACK(gas_id);
  
              /* Mark VA value when CV-zero is sent */
              ul[gas_id].va_old = ul[gas_id].va;
              MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_444, "ERLU XTimer started CV=0 bsn=%u e_bsn_of_cv_0=%u",
                      bsn,
                      ul[gas_id].e_bsn_of_cv_0_block);
            }
          }
  
          /* If a new block arrives either in CV_ZERO state OR GUARD_ON state and CV is no longer zero 
          ** then stop the Guard Timer
          */
          if (((ul[gas_id].guard_timer_state == CV_ZERO) ||
               (ul[gas_id].guard_timer_state == GUARD_ON)
              ) &&
              (IS_EXT_UTBF_ACTIVE(gas_id)) &&
              (TX_ARR(gas_id, bsn).blk.hdr.cv != 0)
             )
          {
            ul[gas_id].guard_timer_state = GUARD_OFF;
            TIMER_STOP_AWAIT_UL_ACKNACK(gas_id);
            MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_445, "ERLU XTimer stopped CV not 0 bsn=%u e_bsn_of_cv_0=%u",
                    bsn,
                    ul[gas_id].e_bsn_of_cv_0_block);
          }
  
          /* Status becomes PACK
          */
          TX_ARR(gas_id, bsn).vb = TXA_PACK;
  
          /* if BSN of block sent co-incides with V(S) then increment V(S)
          */
          if (bsn == ul[gas_id].vs)
          {
            /* No need to check for overflow. Note that V(S) should not go
            ** beyond seg_ind
            */
            E_INC_VS(gas_id);
          }
          else
          {
            MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_1023, "ERLU post_tx bsn(%u) != vs(%u) vb[vs]=%u",
                      bsn,
                      ul[gas_id].vs,
                      TX_ARR(gas_id, ul[gas_id].vs).vb);
  
            if (IS_GT_MOD(bsn,ul[gas_id].vs,ul[gas_id].cur_res.ws))
            {
              /* Make blocks from VS to bsn inclusive PACK blocks to account for PL1 not
              ** acknowledging certain PTX blocks
              */
              ind = ul[gas_id].vs;
  
              for (; ind != bsn;)
              {
                MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1024, "ERLU post_tx ind(%d) becomes PACK bsn(%d) (PL1 mis-ack)",
                        ind,
                        bsn,
                        0);
                TX_ARR(gas_id, ind).vb = TXA_PACK;
  
                /* In addition to forcing to PACK we must set the timers for these blocks
                ** in case they will be NACK'ed by the NW
                */
  
                /* Assign the global value of grlc_ul_block_counter plus the limit
                ** determined by BS_CV_MAX to the block just transmitted. The check for
                ** NACK later will only involve comparing this count value to the current
                ** global counter value
                */
                E_SET_BLOCK_COUNT_LIMIT(gas_id, ind);
  
                /* Initiliaze to the values as they were segmented               */
                TX_ARR(gas_id, ind).blk.old_mcs = TX_ARR(gas_id, ind).blk.cs;
                TX_ARR(gas_id, ind).blk.init_mcs = TX_ARR(gas_id, ind).blk.cs;
                TX_ARR(gas_id, ind).blk.old_ps = TX_ARR(gas_id, ind).blk.ps;
                ROTATE_PS_NO_MCS_CHANGE( TX_ARR(gas_id, ind).blk.cs,
                                         TX_ARR(gas_id, ind).blk.old_ps,
                                         TX_ARR(gas_id, ind).blk.ps );
  
                INC_MOD(ind, ESNS);
              }
  
              ul[gas_id].vs = bsn;
              E_INC_VS(gas_id);
            }
          }
          /* Another PTX block sent
          */
          ul[gas_id].ptx_cnt++;
  
          /*===========================================================================
          ** Detect the conditions for sending PRR type 2 in ACK mode
          ** 1. realloc state is RALS_AWAIT_TX_COMPLETE, AND
          ** 2. last PSN has is or has gone past the designated reallocation PDU
          ** If detected set the realloc state to RALS_CURRENT_PDU_TX_COMPLETED
          **===========================================================================
          */
          if (ul[gas_id].realloc_state == RALS_AWAIT_TX_COMPLETE)
          {
            if (TX_ARR(gas_id, bsn).blk.hdr.cv == 0)
            {
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1025, "ERLU post bsn=%u RA_AT_TX_COMPLETE aborted cv=0",bsn);
#endif
              ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
            }
            else if (is_pdu_transmitted(gas_id, ul[gas_id].realloc_psn))
            {
              ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1026, "ERLU post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
              /* Set internal signal to trigger sending PRR type 2 in task time
              */
              if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
#ifndef PERLUTF
              {
                RLC_UL_SET_ITN_SIG(gas_id);
              }
#else
              #error code not present
#endif
            }
          }
  
          /*----------------------------------------------------------------------------
          ** If in one-phase access and Contention has not been resolved then increment
          ** N3104 on every data block transmitted. Note that N3104 expiry is checked
          ** outside this function (outside PL1'ISR) in ack_mode_xfer(). Note that this
          ** is done on all types of blocks (PTX,PACK and NACK).
          ** For EDGE HEADER TYPE 1 - MCS 7 - MCS 8 MCS 9 - RADIO BLOCK consists of 2 BSN.
          ** N3104 should be incremented per radio block and not per BSN.
          **----------------------------------------------------------------------------
          */
          if ( (IS_TLLI_REQUIRED(gas_id)) && (loop_cnt == 1) )
          {
            INC_N3104(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
            MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_446, "ERLU A post TLLI IncN3104=%u max=%u vsf=%u",
                    ul[gas_id].n3104,
                    N3104_MAX(gas_id),
                    ul[gas_id].vsf);
#endif
          }
  
          TX_ARR(gas_id, bsn).blk.old_mcs = TX_ARR(gas_id, bsn).blk.cs;
          TX_ARR(gas_id, bsn).blk.init_mcs = TX_ARR(gas_id, bsn).blk.cs;
  
          TX_ARR(gas_id, bsn).blk.old_ps = TX_ARR(gas_id, bsn).blk.ps;
          ROTATE_PS_NO_MCS_CHANGE(  TX_ARR(gas_id, bsn).blk.cs,
                                    TX_ARR(gas_id, bsn).blk.old_ps,
                                    TX_ARR(gas_id, bsn).blk.ps );
        }
        else
        /*--------------------------------
        ** Handle PACK block transmitted
        **--------------------------------
        */
        if (TX_ARR(gas_id, bsn).vb == TXA_PACK)
        {
          grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
#ifdef  GSM_GPRS_GRLU_CONFIRM_PACK_TX
          GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
#endif
          ul[gas_id].pack_cnt++;
        }
        else
        /*---------------------------------
        ** Handle UNACK block transmitted
        **---------------------------------
        */
        if (TX_ARR(gas_id, bsn).vb == TXA_UNACK)
        {
          grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
          GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
  
          /* Status becomes PACK
          */
          TX_ARR(gas_id, bsn).vb = TXA_PACK;
  
          /* if BSN of block sent co-incides with V(S) then increment V(S)
          */
          if (bsn == ul[gas_id].vs)
          {
            /* No need to check for overflow. Note that V(S) should not go
            ** beyond seg_ind
            */
            E_INC_VS(gas_id);
          }
  
          if (cur_pdu_ptr[gas_id] != NULL &&
              cur_pdu_ptr[gas_id]->realloc_action == RA_AT_TX_COMPLETE_CHANGE_MODE &&
              /* last block (CV=0) of PDU was sent
              */
              TX_ARR(gas_id, bsn).blk.hdr.cv == 0 &&
              /* only first cv = 0 block */
               ul[gas_id].re_tx_cv_0_cnt == 0
             )
          {
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1027, "ERLU post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
            /* Set internal signal to trigger sending PRR type 2 in task time
            */
            if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
            {
              RLC_UL_SET_ITN_SIG(gas_id);
            }
          }
          else
  
          /*===========================================================================
          ** Detect the conditions for sending PRR type 2 in UNACK mode
          ** 1. realloc state is RALS_AWAIT_TX_COMPLETE, AND
          ** 2. last PSN has is or has gone past the designated reallocation PDU
          ** If detected set the realloc state to RALS_CURRENT_PDU_TX_COMPLETED
          **===========================================================================
          */
          if (ul[gas_id].realloc_state == RALS_AWAIT_TX_COMPLETE)
          {
            if (TX_ARR(gas_id, bsn).blk.hdr.cv == 0)
            {
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_447, "ERLU U post bsn=%u RA_AT_TX_COMPLETE aborted cv=0",bsn);
#endif
              ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
            }
            else if (is_pdu_transmitted(gas_id, ul[gas_id].realloc_psn))
            {
              ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
              MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_448, "ERLU U post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
              /* Set internal signal to trigger sending PRR type 2 in task time
              */
              if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
#ifndef PERLUTF
              {
                RLC_UL_SET_ITN_SIG(gas_id);
              }
#else
              #error code not present
#endif
            }
          }
          ul[gas_id].pack_cnt++;
        } /* txa_unack */
      }

      /* Trigger re-allocation request
      */
      ul[gas_id].b_first_blk_sent = TRUE;

      /* Sort out T3182 and block counter for the block just transmitted,
      ** in both ack and unack modes
      */
      if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
      {
        /*---------------------------------------------------------------------
        ** Start T3182 timer under condition (1) or (2) if T3182 is NOT active
        ** 1. block with CV = 0 is accessed and no nacked blocks in V(B)
        **---------------------------------------------------------------------
        */
        if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
        {
          if ( IS_TX_COMPLETE(gas_id)                                           &&
               (seek_block_type(gas_id, ul[gas_id].va, ul[gas_id].vs, TXA_NACK)==E_GRLC_INVALID_BSN)
               &&
               ( (!IS_EXT_UTBF_ACTIVE(gas_id) ) || (ul[gas_id].suspend_state == GRLC_SUSPEND) )
             )
          {
            TIMER_START_T3182(gas_id);
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1028, "ERLU post T3182 start CV=0 NoNack va=%u vs=%u seg=%u",
                    ul[gas_id].va,
                    ul[gas_id].vs,
                    ul[gas_id].seg_ind);
#endif
          }
        }

        /* Assign the global value of grlc_ul_block_counter plus the limit determined
        ** by BS_CV_MAX to the block just transmitted. The check for NACK later will
        ** only involve comparing this count value to the current global counter value
        */
        E_SET_BLOCK_COUNT_LIMIT(gas_id, bsn);

        /* MSG_GERAN_HIGH_3_G("ERLU post bsn=%u global=%u blk_cnt[]=%u",bsn,
        ** grlc_ul_block_counter,TX_ARR(gas_id, (bsn)).blk_cnt);
        */
      }
      else
      /* GRLC_MODE_UNACK
      */
      {
        /* In unack mode, T3182 is re-started whenever the block with CV = 0
        ** is sent OR when the tx window reaches WS
        */
        if ( ( ( TX_ARR(gas_id, bsn).blk.hdr.cv == 0 )
               &&
               ( !IS_EXT_UTBF_ACTIVE(gas_id)       )
             )
             ||
             ( ( GET_E_TX_WINDOW_SIZE(gas_id) >= ul[gas_id].cur_res.ws ) &&
               ( ul[gas_id].timer_t3182.state != TMR_ACTIVE      )
             )
           )
        {
          if( ul[gas_id].state != US_TEST_MODE )
          {
            TIMER_START_T3182(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_T3182
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_449, "ERLU U T3182 started CV=0 or stalled bsn=%u e_bsn_of_cv_0=%u",
                      bsn,
                      ul[gas_id].e_bsn_of_cv_0_block,
                      0);
#endif
          }
        }
      }

      /* Check Window Full condition (Stalled) again now that VS might have changed
      */
      if ( (ul[gas_id].rlc_mode == GRLC_MODE_ACK) && IS_E_TX_WINDOW_FULL(gas_id) )
      {
        if (ul[gas_id].si != RM_TX_STALLED)
        {
          ul[gas_id].si = RM_TX_STALLED;
        }

        if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
        {
          TIMER_START_T3182(gas_id);
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1029, "ERLU post WinFull start T3182 va=%u vs=%d SI=%u",
                  ul[gas_id].va,
                  ul[gas_id].vs,
                  ul[gas_id].si);
#endif
        }
      }

      /* Process 2nd Payload */
      loop_cnt--;

      if( loop_cnt )
      {
        /* Get the BSN of the 2nd payload */
        bsn = MOD_ESNS( bsn  + E_GET_RAW_BSN2(blk_ptr->blk_ptr->elm.egprs.hdr) );
      }
    } /* Dual payload loop */

    /* Log EGPRS RLC POST Tx Blocks
    */
    rlc_log_ul_header(gas_id, (uint8)(GET_HEADER_TYPE(blk_ptr->blk_ptr->elm.egprs.mcs) + 1),
                       blk_ptr->blk_ptr->elm.egprs.hdr
                     );

  } /* EGPRS */
  else
  { /* GPRS */
    /*---------------------------------------------------------------*/

    /* get BSN of data block just transmmitted given pointer to PL1 UL
    ** radio block which includes PL1 block header. BSN is in RLC header
    */
    bsn = GET_RAW_BSN(blk_ptr->blk_ptr->elm.gprs.block);

    /*--------------------------------
    ** Handle NACK block transmitted
    **--------------------------------
    */
    /* This function is called by GL1. Hence, mutex lock is needed.
     * However, protection is done only for NACK blocks to avoid RLC updating
     * these type of blocks to ACK after geting PUAN at the same time */
    GRLC_UL_TX_VARS_LOCK(gas_id);
    if (TX_ARR(gas_id, bsn).vb == TXA_NACK ||
        TX_ARR(gas_id, bsn).vb == TXA_PENDING_PACK
       )
    {
      grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
      GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);

      /* Status becomes PACK
      */
      TX_ARR(gas_id, bsn).vb = TXA_PACK;
      ul[gas_id].nack_cnt++;
      GRLC_UL_TX_VARS_UNLOCK(gas_id);
    }
    else
    {
      GRLC_UL_TX_VARS_UNLOCK(gas_id);
      /*--------------------------------
      ** Handle PTX block transmitted
      **--------------------------------
      */
      if (TX_ARR(gas_id, bsn).vb == TXA_PTX)
      {
        grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
        GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
  
        /* Restart T3182 for every CV = 0 block
        ** T3182 is now only started in TBF suspend state, hence if running
        ** and another CV=0 block is sent restart it.
        */
        if ( (IS_EXT_UTBF_ACTIVE(gas_id)) &&
             (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0) &&
             (ul[gas_id].timer_t3182.state == TMR_ACTIVE) &&
             ( !is_any_nacked_blks(gas_id) )
           )
        {
          TIMER_START_T3182(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_T3182
          MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_450, "GRLU EXT_TBF:T3182 start bsn cv 0 = %d",bsn);
#endif
        }
  
        /* If the (main) T3182 timer is NOT active then check for the Extra Guard Timer
        ** (don't want main T3182 timer going off and the extra Guard Tinmer as well)
        ** Ext UTBF mode is active, and
        ** When PTX block has CV = 0, and
        ** then re/start the extra timer
        */
        if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
        {
          if (((ul[gas_id].guard_timer_state == GUARD_OFF) ||
               (ul[gas_id].guard_timer_state == CV_ZERO)
              ) &&
              (IS_EXT_UTBF_ACTIVE(gas_id)) &&
              (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0)
             )
          {
            ul[gas_id].guard_timer_state = CV_ZERO;
  
            /* Mark VA value when CV-zero is sent */
            ul[gas_id].va_old = ul[gas_id].va;
            TIMER_START_AWAIT_UL_ACKNACK(gas_id);
            MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_451, "GRLU XTimer started CV=0 bsn=%u bsn_of_cv_0=%u",
                    bsn,
                    ul[gas_id].bsn_of_cv_0_block);
          }
        }
  
        /* If a new block arrives either in CV_ZERO state OR GUARD_ON state and CV is no longer zero 
        ** then stop the Guard Timer
        */
        if (((ul[gas_id].guard_timer_state == CV_ZERO) ||
             (ul[gas_id].guard_timer_state == GUARD_ON)
            ) &&
            (IS_EXT_UTBF_ACTIVE(gas_id)) &&
            (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv != 0)
           )
        {
          ul[gas_id].guard_timer_state = GUARD_OFF;
          TIMER_STOP_AWAIT_UL_ACKNACK(gas_id);
          MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_452, "GRLU XTimer stopped CV not 0 bsn=%u bsn_of_cv_0=%u",
                  bsn,
                  ul[gas_id].bsn_of_cv_0_block);
        }
  
        /* Status becomes PACK
        */
        TX_ARR(gas_id, bsn).vb = TXA_PACK;
  
        /* if BSN of block sent co-incides with V(S) then increment V(S)
        */
        if (bsn == ul[gas_id].vs)
        {
          /* No need to check for overflow. Note that V(S) should not go
          ** beyond seg_ind
          */
          INC_VS(gas_id);
        }
        else
        {
          MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_1030, "GRLU post_tx bsn(%u) != vs(%u) vb[vs]=%u",bsn,ul[gas_id].vs,TX_ARR(gas_id, ul[gas_id].vs).vb);
  
          if (IS_GT_MODULO_COMP(bsn,ul[gas_id].vs,SNS))
          {
            /* Make blocks from VS to bsn inclusive PACK blocks to account for PL1 not
            ** acknowledging certain PTX blocks
            */
            ind = ul[gas_id].vs;
  
            for (; ind != bsn;)
            {
              MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_1031, "GRLU post_tx ind(%d) becomes PACK bsn(%d) (PL1 mis-ack)",ind,bsn);
              TX_ARR(gas_id, ind).vb = TXA_PACK;
  
              /* In addition to forcing to PACK we must set the timers for these blocks
              ** in case they will be NACK'ed by the NW
              */
  
              /* Assign the global value of grlc_ul_block_counter plus the limit determined
              ** by BS_CV_MAX to the block just transmitted. The check for NACK later will
              ** only involve comparing this count value to the current global counter
              ** value
              */
              SET_BLOCK_COUNT_LIMIT(gas_id, ind);
  
              INC_MOD(ind, SNS);
            }
  
            ul[gas_id].vs = bsn;
            INC_VS(gas_id);
          }
        }
        /* Another PTX block sent
        */
        ul[gas_id].ptx_cnt++;
  
        /*===========================================================================
        ** Detect the conditions for sending PRR type 2 in ACK mode
        ** 1. realloc state is RALS_AWAIT_TX_COMPLETE, AND
        ** 2. last PSN has is or has gone past the designated reallocation PDU
        ** If detected set the realloc state to RALS_CURRENT_PDU_TX_COMPLETED
        **===========================================================================
        */
        if (ul[gas_id].realloc_state == RALS_AWAIT_TX_COMPLETE)
        {
          if (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0)
          {
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1032, "GRLU post bsn=%u RA_AT_TX_COMPLETE aborted cv=0",bsn);
#endif
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
          }
          else if (is_pdu_transmitted(gas_id, ul[gas_id].realloc_psn))
          {
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1033, "GRLU post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
            /* Set internal signal to trigger sending PRR type 2 in task time
            */
            if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
#ifndef PERLUTF
            {
              RLC_UL_SET_ITN_SIG(gas_id);
            }
#else
            #error code not present
#endif
          }
        }
        /*----------------------------------------------------------------------------
        ** If in one-phase access and Contention has not been resolved then increment
        ** N3104 on every data block transmitted. Note that N3104 expiry is checked
        ** outside this function (outside PL1'ISR) in ack_mode_xfer(). Note that this
        ** is done on all types of blocks (PTX,PACK and NACK)
        **----------------------------------------------------------------------------
        */
        if (IS_TLLI_REQUIRED(gas_id))
        {
          INC_N3104(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
          MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_453, "GRLU A post TLLI IncN3104=%u max=%d vsf=%u",ul[gas_id].n3104, N3104_MAX(gas_id), ul[gas_id].vsf);
#endif
        }
      }
      else
      /*--------------------------------
      ** Handle PACK block transmitted
      **--------------------------------
      */
      if (TX_ARR(gas_id, bsn).vb == TXA_PACK)
      {
        grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
#ifdef  GSM_GPRS_GRLU_CONFIRM_PACK_TX
        GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
#endif
        ul[gas_id].pack_cnt++;
      }
      else
      /*---------------------------------
      ** Handle UNACK block transmitted
      **---------------------------------
      */
      if (TX_ARR(gas_id, bsn).vb == TXA_UNACK)
      {
        grlc_ul_diag_tx_stat_add_entry(&TX_ARR(gas_id, bsn), gas_id);
        GRLC_UL_QUEUE_BLK_FOR_SHOW(gas_id, bsn, TX_ARR(gas_id, bsn).vb);
  
        /* Status becomes PACK
        */
        TX_ARR(gas_id, bsn).vb = TXA_PACK;
  
        /* if BSN of block sent co-incides with V(S) then increment V(S)
        */
        if (bsn == ul[gas_id].vs)
        {
          /* No need to check for overflow. Note that V(S) should not go
          ** beyond seg_ind
          */
          INC_VS(gas_id);
        }
  
        if (cur_pdu_ptr[gas_id] != NULL &&
            cur_pdu_ptr[gas_id]->realloc_action == RA_AT_TX_COMPLETE_CHANGE_MODE &&
            /* last block (CV=0) of PDU was sent
             */
            TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0   &&
            /* only first cv = 0 block */
            ul[gas_id].re_tx_cv_0_cnt == 0
           )
        {
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
          MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_1034, "GRLU post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
          /* Set internal signal to trigger sending PRR type 2 in task time
          */
          if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
          {
            RLC_UL_SET_ITN_SIG(gas_id);
          }
        }
        else
  
        /*===========================================================================
        ** Detect the conditions for sending PRR type 2 in UNACK mode
        ** 1. realloc state is RALS_AWAIT_TX_COMPLETE, AND
        ** 2. last PSN has is or has gone past the designated reallocation PDU
        ** If detected set the realloc state to RALS_CURRENT_PDU_TX_COMPLETED
        **===========================================================================
        */
        if (ul[gas_id].realloc_state == RALS_AWAIT_TX_COMPLETE)
        {
          if (TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0)
          {
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_454, "GRLU U post bsn=%u RA_AT_TX_COMPLETE aborted cv=0",bsn);
#endif
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
          }
          else if (is_pdu_transmitted(gas_id, ul[gas_id].realloc_psn))
          {
            ul[gas_id].realloc_state = RALS_CURRENT_PDU_TX_COMPLETED;
#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
            MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_455, "GRLU U post bsn=%u RA_AT_TX_COMPLETE",bsn);
#endif
            /* Set internal signal to trigger sending PRR type 2 in task time
            */
            if (!RLC_UL_IS_ITN_SIG_SET(gas_id))
#ifndef PERLUTF
            {
              RLC_UL_SET_ITN_SIG(gas_id);
            }
#else
            #error code not present
#endif
          }
        }
  
        /*----------------------------------------------------------------------------
        ** We don't expect one-phase access in unack mode but just in case.
        **
        ** If in one-phase access and Contention has not been resolved then increment
        ** N3104 on every data block transmitted. Note that N3104 expiry is checked
        ** outside this function (outside PL1'ISR) in ack_mode_xfer(). Note that this
        ** is done on all types of blocks (PTX,PACK and NACK)
        **----------------------------------------------------------------------------
        */
        if (IS_TLLI_REQUIRED(gas_id))
        {
          INC_N3104(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
          MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_456, "GRLU U post IncN3104=%d max=%d vsf=%d",ul[gas_id].n3104, N3104_MAX(gas_id), ul[gas_id].vsf);
#endif
        }
      } /* tx_unack */
    }

    /* Trigger re-allocation request
    */
    ul[gas_id].b_first_blk_sent = TRUE;

    /* Sort out T3182 and block counter for the block just transmitted,
    ** in both ack and unack modes
    */
    if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      /*---------------------------------------------------------------------
      ** Start T3182 timer under condition (1) or (2) if T3182 is NOT active
      ** 1. if tx_blks() has set the stall indicator and T3182
      ** 2. block with CV = 0 is accessed and no nacked blocks in V(B)
      **---------------------------------------------------------------------
      */
      if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
      {
        if ( ( IS_TX_COMPLETE(gas_id) ) &&
             ( !is_any_nacked_blks(gas_id) )
             &&
             ( (!IS_EXT_UTBF_ACTIVE(gas_id)) || (ul[gas_id].suspend_state == GRLC_SUSPEND) )
           )
        {
          TIMER_START_T3182(gas_id);
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1035, "GRLU post T3182 start CV=0 NoNack va=%d vs=%d seg=%d",
                  ul[gas_id].va,
                  ul[gas_id].vs,
                  ul[gas_id].seg_ind);
#endif
        }
      }

      /* Assign the global value of grlc_ul_block_counter plus the limit determined
      ** by BS_CV_MAX to the block just transmitted. The check for NACK later will
      ** only involve comparing this count value to the current global counter value
      */
      SET_BLOCK_COUNT_LIMIT(gas_id, bsn)
    }
    else
    /* GRLC_MODE_UNACK
    */
    {
      /* In unack mode, T3182 is re-started whenever the block with CV = 0
      ** is sent OR when the tx window reaches WS
      */
      if ( ( ( TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv == 0 )
             &&
             ( !IS_EXT_UTBF_ACTIVE(gas_id)                          )
           )
           ||
           ( ( GET_TX_WINDOW_SIZE(gas_id) >= RM_RLC_WINDOW_SIZE     )  &&
             ( ul[gas_id].timer_t3182.state != TMR_ACTIVE             )
           )
         )
      {
        if( ul[gas_id].state != US_TEST_MODE )
        {
          TIMER_START_T3182(gas_id);
#ifdef  GSM_GPRS_GRLU_SHOW_T3182
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_457, "GRLU U T3182 started CV=0 or stalled bsn=%u bsn_of_cv_0=%u",
                      bsn,
                      ul[gas_id].bsn_of_cv_0_block,
                      0);
#endif
        }
      }
    }

    /* Check Window Full condition (Stalled) again now that VS might have changed
    */
    if ( (ul[gas_id].rlc_mode == GRLC_MODE_ACK) && IS_TX_WINDOW_FULL(gas_id) )
    {
      if (ul[gas_id].si != RM_TX_STALLED)
      {
        ul[gas_id].si = RM_TX_STALLED;
      }

      if (ul[gas_id].timer_t3182.state != TMR_ACTIVE)
      {
        TIMER_START_T3182(gas_id);
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1036, "GRLU post WinFull start T3182 va=%u vs=%u SI=%u", ul[gas_id].va, ul[gas_id].vs, ul[gas_id].si);
#endif
      }
    }

    /* Log GPRS RLC Post Tx Blocks
    */
    memscpy(buf, sizeof(buf), blk_ptr->blk_ptr->elm.gprs.block, (RM_MAC_HDR_LEN + RM_RLC_HDR_LEN));
    buf[3] = 0x03 & blk_ptr->blk_ptr->elm.gprs.cs;
    rlc_log_ul_header(gas_id, (uint8)GPRS_HEADER_TYPE, buf);

  } /* GPRS */

#if defined(DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (PERLUTF)
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST/PERLUTF */

} /* post_tx_actions */


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
void ack_mode_xfer ( gas_id_t gas_id )
{
  /*-------------------------------------------------------------------*/

  if (ul[gas_id].state != US_ACK_XFER)
  {
    MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_458, "GRLU A_X s=%d return",ul[gas_id].state);
    return;
  }

  /*========================================================================
  ** One phase access acknowledged mode transfer with contention resolution
  ** Reallocation is delayed until after contention has been resolved.
  **========================================================================
  */
  if (IS_TLLI_REQUIRED(gas_id))
  {
    if (MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].vsf, SNS) < MAX_BLOCKS_TO_SEGMENT)
    {
      /*-----------------------------------------------------------------
      ** If segment state is not complete then segment a few more blocks
      **-----------------------------------------------------------------
      */
      ul[gas_id].segment_state = segment_pdu(gas_id);
      segment_loop(gas_id, FALSE);
    }

    TX_BLKS(gas_id);

    /*------------------------------------------------------------------------
    ** Check if N3104 has expired. If so inform MAC and clear UL data
    ** structures and goto AWAIT_ASS as MAC shall attempt access upto 4 times
    ** using the previous channel request info from RLC
    **------------------------------------------------------------------------
    */
    if (IS_N3104_EXPIRED(gas_id))
    {
      /*------------------------------------------------------------------------
      ** Before we execute N3104 expiry check to see if a Contention Resolution
      ** message already exists on the queue. If one is NOT found then perform
      ** N3104 Expiry, otherwise do nothing so that the Contention Res Message
      ** is processed on our next context
      **------------------------------------------------------------------------
      */
      MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_459, "GRLU A Expy N3104=%d N3104_MAX=%d",ul[gas_id].n3104, N3104_MAX(gas_id));

      /* Inform MAC so it can retry 4 times
      */
      indicate_n3104_expiry(gas_id);

      /* Set up some vars and enter AWAIT_ASS state
      */
      handle_contention_resolution_failure(gas_id);
    }

    /* Check if PRR should be sent during contention resolution. */
    check_to_send_prr(gas_id);

  } /* one phase access mode with contention un-resolved */

  else
  /*================================================================
  ** Two-phase access acknowledge mode transfer or One-phase access
  ** mode transfer (contention resolved) with re-allocation
  **================================================================
  */
  {
    /*---------------------------------------------------------------------
    ** Check segment status and segment some more blocks in current PDU.
    ** Only segment if it is not done and that seg_ind is, say 5, blocks
    ** away from vsf.
    **---------------------------------------------------------------------
    */
    if (MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].vsf, SNS) < MAX_BLOCKS_TO_SEGMENT)
    {
      /* Do this before segmentation starts in case the PDU in the group is a
      ** short PDU
      */
      if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
      {
        MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_460, "A_X calls group_pdus");
        group_pdus(gas_id);
      }

      /* Segment as many blocks as there are free space in L1 FIFO.
      ** Even though the free blocks may be used for re-tx of NACKED
      ** in which case the segmented blocks won't get sent right now
      */
      ul[gas_id].segment_state = segment_pdu(gas_id);

      /*--------------------------------------------------------------------------
      ** Call segment_pdu() in a loop when the length of the current PDU is shorter
      ** than the capacity of a block. When this happens the 'cur_blk' is partial
      ** and un-attached and the next PDU needs to be loaded and segmented
      ** to make up enough blocks to transmit with. However, we should only do
      ** this when vs == vsf, ie. no more blocks to send to FIFO
      **--------------------------------------------------------------------------
      */
      segment_loop(gas_id, FALSE);
    }

    TX_BLKS(gas_id);
  } /* 2-phase access or 1-phase access mode with contention resolved */

  detect_error_and_recover(gas_id);

} /* ack_mode_xfer() */

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
void e_ack_mode_xfer ( gas_id_t gas_id )
{
  /*-------------------------------------------------------------------*/

  if (ul[gas_id].state != US_ACK_XFER)
  {
    MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_461, "ERLU A_X s=%d return",ul[gas_id].state);
    return;
  }

  /*========================================================================
  ** One phase access acknowledged mode transfer with contention resolution
  ** Reallocation is delayed until after contention has been resolved.
  **========================================================================
  */
  if (IS_TLLI_REQUIRED(gas_id))
  {
    if (MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].vsf) < MAX_BLOCKS_TO_SEGMENT)
    {
      /*-----------------------------------------------------------------
      ** If segment state is not complete then segment a few more blocks
      **-----------------------------------------------------------------
      */
      ul[gas_id].segment_state = e_segment_pdu(gas_id);
      segment_loop(gas_id, FALSE);
    }

    TX_BLKS(gas_id);

    /*------------------------------------------------------------------------
    ** Check if N3104 has expired. If so inform MAC and clear UL data
    ** structures and goto AWAIT_ASS as MAC shall attempt access upto 4 times
    ** using the previous channel request info from RLC
    **------------------------------------------------------------------------
    */
    if (IS_N3104_EXPIRED(gas_id))
    {
      /*------------------------------------------------------------------------
      ** Before we execute N3104 expiry check to see if a Contention Resolution
      ** message already exists on the queue. If one is NOT found then perform
      ** N3104 Expiry, otherwise do nothing so that the Contention Res Message
      ** is processed on our next context
      **------------------------------------------------------------------------
      */
      MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_462, "GRLU A Expy N3104=%d N3104_MAX=%d",ul[gas_id].n3104, N3104_MAX(gas_id));

      /* Inform MAC so it can retry 4 times
      */
      indicate_n3104_expiry(gas_id);

      /* Set up some vars and enter AWAIT_ASS state
      */
      handle_contention_resolution_failure(gas_id);
    }

    /* Check if PRR should be sent during contention resolution. */
    check_to_send_prr(gas_id);

  } /* one phase access mode with contention un-resolved */

  else
  /*================================================================
  ** Two-phase access acknowledge mode transfer or One-phase access
  ** mode transfer (contention resolved) with re-allocation
  **================================================================
  */
  {
    /*---------------------------------------------------------------------
    ** Check segment status and segment some more blocks in current PDU.
    ** Only segment if it is not done and that seg_ind is, say 5, blocks
    ** away from vsf.
    **---------------------------------------------------------------------
    */
    if (MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].vsf) < MAX_BLOCKS_TO_SEGMENT)
    {
      /* Do this before segmentation starts in case the PDU in the group is a
      ** short PDU
      */
      if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
      {
        MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_463, "E_A_X calls group_pdus");
        group_pdus(gas_id);
      }

      /* Segment as many blocks as there are free space in L1 FIFO.
      ** Even though the free blocks may be used for re-tx of NACKED
      ** in which case the segmented blocks won't get sent right now
      */
      ul[gas_id].segment_state = e_segment_pdu(gas_id);

      /*--------------------------------------------------------------------------
      ** Call segment_pdu() in a loop when the length of the current PDU is shorter
      ** than the capacity of a block. When this happens the 'cur_blk' is partial
      ** and un-attached and the next PDU needs to be loaded and segmented
      ** to make up enough blocks to transmit with. However, we should only do
      ** this when vs == vsf, ie. no more blocks to send to FIFO
      **--------------------------------------------------------------------------
      */
      segment_loop(gas_id, FALSE);
    }
  } /* 2-phase access or 1-phase access mode with contention resolved */

  TX_BLKS(gas_id);

  detect_error_and_recover(gas_id);

} /* e_ack_mode_xfer() */


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
void unack_mode_xfer ( gas_id_t gas_id )
{
  segment_status_t    seg_res;

  /*-------------------------------------------------*/

  if (ul[gas_id].state != US_UNACK_XFER)
  {
    MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_464, "GRLU U_X s=%d return",ul[gas_id].state);
  }

  /*----------------------------------------------------------------------
  ** Segment if segmented blocks are less than MAX_BLOCKS_TO_SEGMENT and
  ** segment is not done
  **----------------------------------------------------------------------
  */
  if (MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].pack_vsf, SNS) < MAX_BLOCKS_TO_SEGMENT)
  {
    /* Do this before segmentation starts in case the PDU in the group is a
    ** short PDU
    */
    if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
    {
      MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_465, "U_X calls group_pdus");
      group_pdus(gas_id);
    }

    seg_res = segment_pdu(gas_id);

    if (seg_res == SEGMENT_DONE)
      ul[gas_id].segment_state = SEGMENT_DONE;

    /*--------------------------------------------------------------------------
    ** Call segment_pdu() again when the length of the 1st PDU is shorter
    ** than the capacity of a block. When this happens the 'cur_blk' is partial
    ** and un-attached and the next PDU needs to be loaded and segmented
    ** to make up enough blocks to transmit with. However, we only do this when
    ** VS == VSF, ie. no more blocks to send to FIFO
    **--------------------------------------------------------------------------
    */
    segment_loop(gas_id, FALSE);
  }

  TX_BLKS(gas_id);

  detect_error_and_recover(gas_id);

} /* unack_mode_xfer */



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
void e_unack_mode_xfer ( gas_id_t gas_id )
{
  segment_status_t    seg_res;

  /*-------------------------------------------------*/

  if (ul[gas_id].state != US_UNACK_XFER)
  {
    MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_466, "ERLU U_X s=%d return",ul[gas_id].state);
  }

  /*----------------------------------------------------------------------
  ** Segment if segmented blocks are less than MAX_BLOCKS_TO_SEGMENT and
  ** segment is not done
  **----------------------------------------------------------------------
  */
  if (MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].pack_vsf) < MAX_BLOCKS_TO_SEGMENT)
  {
    /* Do this before segmentation starts in case the PDU in the group is a
    ** short PDU
    */
    if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
    {
      MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_467, "E_U_X calls group_pdus");
      group_pdus(gas_id);
    }

    seg_res = e_segment_pdu(gas_id);

    if (seg_res == SEGMENT_DONE)
      ul[gas_id].segment_state = SEGMENT_DONE;

    /*--------------------------------------------------------------------------
    ** Call segment_pdu() again when the length of the 1st PDU is shorter
    ** than the capacity of a block. When this happens the 'cur_blk' is partial
    ** and un-attached and the next PDU needs to be loaded and segmented
    ** to make up enough blocks to transmit with. However, we only do this when
    ** VS == VSF, ie. no more blocks to send to FIFO
    **--------------------------------------------------------------------------
    */
    segment_loop(gas_id, FALSE);
  }

  TX_BLKS(gas_id);

  detect_error_and_recover(gas_id);

} /* e_unack_mode_xfer */




/*** EOF: don't remove! ***/
