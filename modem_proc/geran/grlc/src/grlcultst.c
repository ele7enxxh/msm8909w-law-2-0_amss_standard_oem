/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK TEST MODULE
***
***
*** DESCRIPTION
***
***  This module implements functions required for diagnostic purposes as well
***  as functions to aid Host test and general debugging
***
*** EXTERNALIZED FUNCTIONS
***
***  uint32  grlc_ul_pack_queue_info (void)
***  void grlc_ul_show_block_sent(uint16 bsn, tx_ack_state_t ack_state)
***  void grlc_ul_show_paq (void)
***  void grlc_ul_show_new_alloc(mr_ul_alloc_cnf_t *old_ptr, mr_ul_alloc_cnf_t *ptr)
***  void grlc_ul_show_alloc_cnf_await_ass (mr_ul_alloc_cnf_t *ptr)
***  void grlc_ul_show_llc_pdu_type (gprs_grr_ul_mes_t *ptr)
***  void grlc_ul_get_llc_pdu_info (gprs_grr_ul_mes_t *ptr, uint32 *lu, uint32 *l2)
***  void grlc_ul_show_tx_vars(void)
***  void show_rbb (gas_id_t gas_id, rm_acknack_desc_t *desc_ptr, uint8 fai)
***  void grlc_ul_show_vars (gas_id_t gas_id)
***  uint16 rlc_ul_octets_rem (gas_id_t gas_id)
***  uint16 rlc_ul_get_timer_cnt (gas_id_t gas_id, uint8 bsn)
***  void rlc_ul_show_state_vars(rlc_ul_show_state_vars)
***  void rlc_ul_show_sv(gas_id_t gas_id)
***  uint8 rlc_get_data_blk_size (uint8 cs)
***  uint16 rlc_ul_get_bs_cv_max_wait_ms (gas_id_t gas_id)
***  uint16 rlc_ul_get_bs_cv_max (gas_id_t gas_id)
***  uint16 rlc_ul_get_block_period_ms (void)
***  void rlc_ul_set_bs_cv_max (gas_id_t gas_id, uint8 by)
***  void fill_ack_bit (const uint8 bsn, const uint8 ssn,uint8 *ptr)
***  void rlc_ul_scan_tx_array_for_non_null_dsm_ptrs (gas_id_t gas_id)
***  static void block_timer_cb (int4 interval)
***  void xlate_u2d (grlc_ul_pl1_data_t  *pub, pl1_radio_block_t   *pdrb)
***  void grlc_diag_show_ul (gas_id_t gas_id)
***  void show_group_status (gas_id_t gas_id)
***  const grlc_diag_ul_statistics_t* grlc_diag_get_ul_statistics (gas_id_t gas_id)
***  void grlc_diag_clr_ul_statistics (gas_id_t gas_id)
***  const grlc_diag_ul_abnormal_release_counts_t*
***     grlc_diag_get_ul_abnormal_release_counts (void)
***  void grlc_diag_clr_ul_abnormal_release_counts (gas_id_t gas_id)
***  const grlc_diag_ul_event_counts_t* grlc_diag_get_ul_event_counts (void)
***  void grlc_diag_clr_ul_event_counts (gas_id_t gas_id)
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcultst.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
*** 08/03/04    hv     Added EGPRS code
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "queue.h"

#include "gmutex.h"

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "grlcultst.h"
#include "grlcllc.h"
#include "grlctsk.h"


#ifdef GSM_GPRS_GRLU_CONFIRM_TX
static post_tx_blk_fifo_t  grlc_ul_post_tx_blk_fifo[NUM_GERAN_DATA_SPACES];
#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

/*-----------------------------------------------------------------------
**  Uplink statistical info
**-----------------------------------------------------------------------
*/
grlc_diag_ul_statistics_t                ul_diag_stats_st[NUM_GERAN_DATA_SPACES];
grlc_diag_ul_abnormal_release_counts_t   ul_diag_abn_rel_cnts_st[NUM_GERAN_DATA_SPACES];
grlc_diag_ul_event_counts_t              ul_diag_evt_cnts_st[NUM_GERAN_DATA_SPACES];

grlc_diag_ul_pdu_stat_t    ul_diag_pdu_stat[NUM_GERAN_DATA_SPACES];
grlc_diag_ul_block_stat_t  ul_diag_block_stat[NUM_GERAN_DATA_SPACES];
grlc_diag_ul_tx_stat_t     ul_diag_tx_stat[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GRLC_UL_PACK_QUEUE_INFO
===
===  DESCRIPTION
===
===    Return LLC PDU queue count, number of PDUs held in 'ul' struct and
===    number of octets as a packed 32 bit hex value
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    uint32
===
===  SIDE EFFECTS
===
===========================================================================*/
uint32  grlc_ul_pack_queue_info (gas_id_t gas_id)
{
  uint32  lu;

  /* Np Oc te ts    Np: number of PDUs, Octets: number of octets
  **  |  |  |  |
  **  |  |  |  |
  ** 24
  */
  lu  = 0L;
  lu  = (uint32)ul[gas_id].llc_octets & 0xffffff;
  lu |= ((uint32)ul[gas_id].llc_pdus & 0xff) << 24;

  return lu;
}

#ifdef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_BLOCK_SENT
===
===  DESCRIPTION
===
===     Show the block type, BSN, CV, payload length of the data block sent
===     to the UL FIFO
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_block_sent(gas_id_t gas_id, uint16 bsn, tx_ack_state_t ack_state)
{
  switch(ack_state)
  {
    case TXA_PTX:
#ifdef  GSM_GPRS_GRLU_SHOW_PTX
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_716, "ERLU Sent PTX %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_717, "GRLU Sent PTX %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
#endif
      break;

    case TXA_PACK:
#ifdef  GSM_GPRS_GRLU_SHOW_PACK
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_718, "ERLU Sent PAK %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_719, "GRLU Sent PAK %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
#endif
      break;

    case TXA_ACK:
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_720, "ERLU Sent ACK %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
      MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_721, "GRLU Sent ACK %d CV %d l %d",
                bsn,
                TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                TX_ARR(gas_id, bsn).blk.data_len);
      }
      break;

    case TXA_UNACK:
#ifdef  GSM_GPRS_GRLU_SHOW_UNACK
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_722, "ERLU Sent UNA %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_723, "GRLU Sent UNA %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
#endif
      break;

    case TXA_PENDING_PACK:
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_724, "ERLU Sent PPA %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_725, "GRLU Sent PPA %d CV %d l %d",
                bsn,
                TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                TX_ARR(gas_id, bsn).blk.data_len);
      }
      break;

    case TXA_NACK:
#ifdef  GSM_GPRS_GRLU_SHOW_NACK
      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_726, "ERLU Sent NAK %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.hdr.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
      else
      {
        MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_727, "GRLU Sent NAK %d CV %d l %d",
                  bsn,
                  TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR(gas_id, bsn).blk.data_len);
      }
#endif
      break;

    case TXA_INVALID:
    case TXA_INACTIVE:
    default:
      break;
  }
} /* grlc_ul_show_block_sent */

#endif /* GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO */

#ifdef GSM_GPRS_GRLU_CONFIRM_TX

/*===========================================================================
===
===  FUNCTION        GRLC_UL_INIT_POST_TX_SHOW_FIFO
===
===  DESCRIPTION
===
===     Clear the post-tx-block-fifo
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_init_post_tx_show_fifo (gas_id_t gas_id)
{
  GRLC_UL_TX_VARS_LOCK(gas_id);

  grlc_ul_post_tx_blk_fifo[gas_id].in_ind = 0;
  grlc_ul_post_tx_blk_fifo[gas_id].out_ind= 0;
  grlc_ul_post_tx_blk_fifo[gas_id].ctr= 0;

  GRLC_UL_TX_VARS_UNLOCK(gas_id);
}

/*===========================================================================
===
===  FUNCTION        GRLC_UL_QUEUE_BLK_FOR_SHOW
===
===  DESCRIPTION
===
===     This function inserts the BSN and ack-status of a data block into the
===     post-tx-blk-fifo. This function is called from post_tx_actions(), ie.
===     from PL1's ISR such that the action of formatting and calling MSG_X()
===     are deferred from the ISR and executed in RLC's task context
===
===     Pushes BSN and ack_state into the global variable
===     grlc_ul_post_tx_blk_fifo
===
===     No INT-LOCK used because it is designed to be called from the
===     post_tx_actions() which is called by the ISR callback
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_queue_blk_for_show
(
  gas_id_t        gas_id,
  uint16          bsn,
  tx_ack_state_t  ack_state
)
{

  GRLC_UL_TX_VARS_LOCK(gas_id);

  if (grlc_ul_post_tx_blk_fifo[gas_id].ctr >= POST_TX_SHOW_ITEM_MAX)
  {
    /* Counter overflow: show error and clear buffer
    */
    MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_1227, "GRLU post msg buf(%u) OVF", POST_TX_SHOW_ITEM_MAX);
    grlc_ul_init_post_tx_show_fifo(gas_id);
  }
  else
  {
    grlc_ul_post_tx_blk_fifo[gas_id].show_fifo[grlc_ul_post_tx_blk_fifo[gas_id].in_ind].bsn = bsn;
    grlc_ul_post_tx_blk_fifo[gas_id].show_fifo[grlc_ul_post_tx_blk_fifo[gas_id].in_ind].ack_state =
      ack_state;

    INC_MOD(grlc_ul_post_tx_blk_fifo[gas_id].in_ind, POST_TX_SHOW_ITEM_MAX);

    /* Already checked for OVF.
    */
    ++grlc_ul_post_tx_blk_fifo[gas_id].ctr;
  }

  GRLC_UL_TX_VARS_UNLOCK(gas_id);

} /* grlc_ul_queue_blk_for_show */

/*===========================================================================
===
===  FUNCTION        GRLC_UL_DEQUEUE_BLK_FOR_SHOW
===
===  DESCRIPTION
===
===     This function takes an item out of the post-tx-fifo and adjust the
===     FIFO accordingly
===
===     Lock interrupt briefly to decrement a single counter
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_dequeue_blk_for_show
(
  gas_id_t        gas_id,
  uint16          *bsn_ptr,
  tx_ack_state_t  *ack_state_ptr
)
{
  GRLC_UL_TX_VARS_LOCK(gas_id);

  if (grlc_ul_post_tx_blk_fifo[gas_id].ctr != 0)
  {
    *bsn_ptr = grlc_ul_post_tx_blk_fifo[gas_id].show_fifo[grlc_ul_post_tx_blk_fifo[gas_id].out_ind].bsn;
    *ack_state_ptr =
      grlc_ul_post_tx_blk_fifo[gas_id].show_fifo[grlc_ul_post_tx_blk_fifo[gas_id].out_ind].ack_state;

    --grlc_ul_post_tx_blk_fifo[gas_id].ctr;
    INC_MOD(grlc_ul_post_tx_blk_fifo[gas_id].out_ind, POST_TX_SHOW_ITEM_MAX);
  }
  else
  {
    /* Check underflow: show error message and clear buffer
    */
    MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_1228, "GRLU post msg buf(%u) UDF", POST_TX_SHOW_ITEM_MAX);
    grlc_ul_init_post_tx_show_fifo(gas_id);
  }

  GRLC_UL_TX_VARS_UNLOCK(gas_id);

} /* grlc_ul_dequeue_blk_for_show */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_CONFIRM_TX
===
===  DESCRIPTION
===
===     Show the BSN and the type of data block that has been confirmed as
===     transmitted by PL1 (following NPL1 callback)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_show_confirm_tx
(
  gas_id_t        gas_id,
  uint16          bsn,
  tx_ack_state_t  ack_state
)
{
  uint32  u = 0L, u2=0L, u3=0L;

  /* check for valid RLC UL state and valid tx_arr pointer
  ** This is required as the memory for tx_arr is freed during release ,
  ** but a previous post_ptx_actions callback which is deferred to print
  ** block tx info may end up with Null pointer.
  */
  if (p_tx_arr[gas_id]  == NULL   ||
      ul[gas_id].state  == US_NULL )
  {
    MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_728, "GRLU RLC UL state already NULL %d",ul[gas_id].state);
    return;
  }

  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    /* ERLU post yyy BSN L=zz CvMcCpSi
    **                         | | | |
    **                        24 | | |
    **                          16 8 0
    **                             8 0
    */
    u = (((uint32)TX_ARR(gas_id, bsn).blk.hdr.cv  & 0x0f) << 24 |
         ((uint32)TX_ARR(gas_id, bsn).blk.cs      & 0x0f) << 16 |
         ((uint32)TX_ARR(gas_id, bsn).blk.hdr.cps & 0x1f) <<  8 |
         ((uint32)TX_ARR(gas_id, bsn).blk.hdr.si  & 0x01)
        );

    /* ERLU post 0BsnBsLn.     0Bsn: BSN in hex, Bs=BS_CV_MAX Ln=block length
    **            | | | |
    **           24 | | |
    **             16 8 0
    */
    u2 = ((((uint32)bsn                      & 0xffff) << 16) |
          (((uint32)BS_CV_MAX(gas_id)        &    0xf) <<  8) |
          ((uint32)TX_ARR(gas_id, bsn).blk.data_len &   0xff)
         );

    /* ERLU   0TbcABsn. 0Tbc: current ul[gas_id].tbc, bp->abs_bsn
    **         | | | |
    **        24 | | |
    **          16 8 0
    */
    u3 = ((((uint32)ul[gas_id].tbc              & 0xffff) << 16) |
          ((uint32)TX_ARR(gas_id, bsn).blk.abs_bsn  & 0xffff)
         );

    switch(ack_state)
    {
      /* 'post' refers to post-transmission ack
      */
      case TXA_PTX:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1229, "ERLU post PTX 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_PACK:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1230, "ERLU post PAK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_ACK:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1231, "ERLU post ACK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_UNACK:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1232, "ERLU post UNA 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_PENDING_PACK:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1233, "ERLU post PPA 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_NACK:
        MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1234, "ERLU post NAK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",u2,u3,u);
        break;

      case TXA_INVALID:
      case TXA_INACTIVE:
        MSG_GRLC_HIGH_1_G (GRLC_F3_MSG_ENUM_1235, "ERLU post invalid ack_state %d",ack_state);
        break;

      default:
        break;
    }
  }
  else
  {
    /* GRLU post yyy BSN L=zz CvCsTsSi
    **                         | | | |
    **                        24 | | |
    **                          16   0
    */
    u = ((uint32)TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.cv << 24) |
        ((uint32)ul[gas_id].cur_res.cs                           << 16) |
        (((uint32)ul[gas_id].cur_res.num_timeslots & 0xf)        <<  8) |
        (uint32)TX_ARR(gas_id, bsn).blk.mac_hdr.type.ul_data.si;

    /* GRLU post 0BsnBsLn.     0Bsn: BSN in hex, Bs=BS_CV_MAX Ln=block length
    **            | | | |
    **           24 | | |
    **             16 8 0
    */
    u2 = ((((uint32)bsn                  & 0x00ff) << 16) |
          (((uint32)BS_CV_MAX(gas_id)       &    0xf) <<  8) |
          ((uint32)TX_ARR(gas_id, bsn).blk.data_len & 0xff)
         );

    /* GRLU   0TbcABsn. 0Tbc: current ul[gas_id].tbc, bp->abs_bsn
    **         | | | |
    **        24 | | |
    **          16 8 0
    */
    u3 = ((((uint32)ul[gas_id].tbc                  & 0xffff) << 16) |
          ((uint32)TX_ARR(gas_id, bsn).blk.abs_bsn  & 0xffff)
         );

    switch(ack_state)
    {
      /* 'post' refers to post-transmission ack
      */
      case TXA_PTX:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1236, "GRLU post PTX 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_PACK:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1237, "GRLU post PAK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_ACK:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1238, "GRLU post ACK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_UNACK:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1239, "GRLU post UNA 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_PENDING_PACK:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1240, "GRLU post PPA 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_NACK:
          MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_1241, "GRLU post NAK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",u2,u3,u);
        break;

      case TXA_INVALID:
      case TXA_INACTIVE:
        MSG_GRLC_HIGH_1_G (GRLC_F3_MSG_ENUM_1242, "GRLU post invalid ack_state %d",ack_state);
        break;

      default:
        break;
    }
  }
} /* grlc_ul_show_confirm_tx */

/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_POST_TX_BLKS
===
===  DESCRIPTION
===
===     This function shows all the data blocks queued up in the post-tx-blk-fifo
===
===  DEPENDENCIES
===
===  RETURN VALUE
[NUM_GERAN_DATA_SPACES]===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_post_tx_blks (gas_id_t gas_id)
{
  uint16         bsn = 0;
  tx_ack_state_t ack_state = TXA_INACTIVE;
  uint16         ctr = 0;   /* don't loop for ever if good people
                            ** violate our space
                            */

  while (grlc_ul_post_tx_blk_fifo[gas_id].ctr != 0 && ctr < POST_TX_SHOW_ITEM_MAX)
  {
    grlc_ul_dequeue_blk_for_show (gas_id, &bsn, &ack_state);
    GRLC_UL_SHOW_CONFIRM_TX(gas_id, bsn, ack_state);

    ctr++;
  }
}

#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_PAQ
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
/* Show contents of the Restore-FIFO
*/
void grlc_ul_show_paq (gas_id_t gas_id)
{
  MSG_GRLC_MED_3_G (GRLC_F3_MSG_ENUM_729, "GRLU show_paq NpOctets=%08x dup free=%u small free=%u",grlc_ul_pack_queue_info(gas_id),
            DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
            DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));
}
#endif

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_NEW_ALLOC
===
===  DESCRIPTION
===     Show the attributes of the new uplink assignment
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_new_alloc
(
  gas_id_t                gas_id,
  const mr_ul_alloc_cnf_t *old_ptr,
  const mr_ul_alloc_cnf_t *ptr
)
{
  uint32  lu0=0L, lu1=0L, lu2=0L;

  /* Pack num_timeslots, coding scheme, TFI, TBF-type (open/close), access-phase (1/2),
  ** access-mode (dyn/fixed), tlli-block coding scheme and usf-granularity into two
  ** 32-bit hex
  **
  ** Ts Cs Tf Tp    Ph Mo Tc Gr
  ** ff ff ff ff    ff ff ff ff
  **  24             24
  **     16             16
  **        8              8
  */
  lu0  = ((uint32)old_ptr->num_timeslots & 0xf) << 24;
  lu0 |= ((uint32)old_ptr->cs & 0xf) << 16;
  lu0 |= ((uint32)old_ptr->ul_tfi & 0x1f) << 8;
  lu0 |= ((uint32)old_ptr->tbf_type & 0xf);

  lu1  = ((uint32)ptr->num_timeslots & 0xf) << 24;
  lu1 |= ((uint32)ptr->cs & 0xf) << 16;
  lu1 |= ((uint32)ptr->ul_tfi & 0x1f) << 8;
  lu1 |= ((uint32)ptr->tbf_type & 0xf);

  lu2  = ((uint32)ptr->access_phase & 0xff) << 24;
  lu2 |= ((uint32)ptr->access_mode & 0xf) << 16;
  lu2 |= ((uint32)ptr->tlli_block_cs & 0xf) << 8;
  lu2 |= ((uint32)ptr->usf_granularity & 0xf);

  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_730, "ERLU TsMcTfTp=%08x NewTsMcTfTp=%08x PhMoTcGr=%08x",lu0,lu1,lu2);
    MSG_GRLC_EXT_MED_2_G (GRLC_F3_MSG_ENUM_731, "ERLU NewMCS=%u WS=%u",ptr->mcs, ptr->ws);
  }
  else
  {
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_732, "GRLU TsCsTfTp=%08x NewTsCsTfTp=%08x PhMoTcGr=%08x",lu0,lu1,lu2);
    MSG_GRLC_EXT_MED_1_G (GRLC_F3_MSG_ENUM_733, "GRLU NewCS=%u",ptr->cs);
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu0; (void)lu1; (void)lu2;

} /* grlc_ul_show_new_alloc */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_ALLOC_CNF_AWAIT_ASS
===
===  DESCRIPTION
===     Show the attributes of the uplink assignment during AWAIT_ASS state
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_alloc_cnf_await_ass (gas_id_t gas_id, const mr_ul_alloc_cnf_t *ptr)
{
  uint32  lu1=0L, lu2=0L;

  /* Pack num_timeslots, coding scheme, TFI, TBF-type (open/close), access-phase (1/2),
  ** access-mode (dyn/fixed), tlli-block coding scheme and usf-granularity into two
  ** 32-bit hex
  **
  ** Ts Cs Tf Tp    Ph Mo Tc Gr
  ** ff ff ff ff    ff ff ff ff
  **  24             24
  **     16             16
  **        8              8
  */
  lu1  = ((uint32)ptr->num_timeslots & 0xf) << 24;
  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
    lu1 |= ((uint32)ptr->mcs & 0xf) << 16;
  else
    lu1 |= ((uint32)ptr->cs & 0xf) << 16;
  lu1 |= ((uint32)ptr->ul_tfi & 0x1f) << 8;
  lu1 |= ((uint32)ptr->tbf_type & 0xf);

  lu2  = ((uint32)ptr->access_phase & 0xff) << 24;
  lu2 |= ((uint32)ptr->access_mode & 0xf) << 16;
  lu2 |= ((uint32)ptr->tlli_block_cs & 0xf) << 8;
  lu2 |= ((uint32)ptr->usf_granularity & 0xf);

  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_734, "ERLU AWAIT_ASS ALLOC_CNF TsMcTfTp=%08x PhMoTcGr=%08x WS=%u",
            lu1,
            lu2,
            ptr->ws);
  }
  else
  {
    MSG_GRLC_EXT_HIGH_2_G(GRLC_F3_MSG_ENUM_735, "GRLU AWAIT_ASS ALLOC_CNF TsCsTfTp=%08x PhMoTcGr=%08x",lu1, lu2);
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu1; (void)lu2;

} /* grlc_ul_show_alloc_cnf_await_ass */

#ifdef  GSM_GPRS_GRLU_LLC_PDU_TYPE
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_LLC_PDU
===
===  DESCRIPTION
===
===     Show the characteristics of LLC PDU received
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_llc_pdu_type (gas_id_t gas_id, gprs_grr_ul_mes_t *ptr)
{
  uint32  lu = 0L;
  uint32  l2 = 0L;

  /* Mo (rlc mode), Pt (peak-throughput class), Rp (radio priority), GCa (cause)
  ** GCa is Gas_id plus Cause, b3b2 = gas_id, b1b0 = Cause
  ** Mo Pt Rp GCa    tlli     type len_PcPi
  **  |  |  | ||cause         |  | | |
  **  |  |  | 4gas_id         |  | | |
  **  |  |  8                 |  | | PFI value
  **  |  16                   |  | PFC active or not
  **  24                      |  length of LLC PDU in hex 0fff 0000
  **                          llc_pdu_type MSByte f000 0000
  */

  lu  = ((uint32)ptr->rlc_mode & 0xf) << 24;
  lu |= ((uint32)ptr->peak_throughput & 0xf) << 16;
  lu |= ((uint32)ptr->radio_priority & 0xf) << 8;
  lu |= ((uint32)ptr->gas_id & 0xf) << 4;
  lu |= (uint32)ptr->cause & 0xf;

  l2  = ((uint32)ptr->llc_pdu_type & 0xf) << 28;
  l2 |= ((uint32)ptr->len << 16) & (0x0fff0000);
  l2 |= ((uint32)(((IS_PFC_NW_SUPPORTED(gas_id) ? 1 : 0) << 8) & (0x0000ff00)));
  l2 |= ((uint32)ptr->pfi & 0xff);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_736, "GRLU MoPtRpGC=%08x tlli=%08x Tylen_PcPi=%08x ",lu, ptr->tlli, l2);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu;  (void)l2;

} /* grlc_ul_show_llc_pdu_type */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_GET_LLC_PDU_INFO
===
===  DESCRIPTION
===
===     Returns the characteristics of LLC PDU received
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_get_llc_pdu_info (gprs_grr_ul_mes_t *ptr, uint32 *lu, uint32 *l2)
{
  /* Mo (rlc mode), Pt (peak-throughput class), Rp (radio priority), Ca (Cause : 4 bit)
  ** Mo Pt Rp - Ca      type len_PcPi
  ** |  |  |  | |       |    |   | |
  ** |  |  |  4 cause   |    |   | |
  ** |  |  8            |    |   | PFI value
  ** |  16              |    |   PFC active or not
  ** 24                 |    length of LLC PDU in hex 0fff 0000
  **                    llc_pdu_type MSByte f000 0000
  */

  *lu  = ((uint32)ptr->rlc_mode & 0xf) << 24;
  *lu |= ((uint32)ptr->peak_throughput & 0xf) << 16;
  *lu |= ((uint32)ptr->radio_priority & 0xf) << 8;
  *lu |= (uint32)ptr->cause & 0xf;

  *l2  = ((uint32)ptr->llc_pdu_type & 0xf) << 28;
  *l2 |= ((uint32)ptr->len << 16) & (0x0fff0000);
  *l2 |= ((uint32)(((IS_PFC_NW_SUPPORTED(ptr->gas_id) ? 1 : 0) << 8) & (0x0000ff00)));
  *l2 |= ((uint32)ptr->pfi & 0xff);

} /* grlc_ul_get_llc_pdu_info */

#endif /* GSM_GPRS_GRLU_LLC_PDU_TYPE */

#ifdef GSM_GPRS_GRLU_SHOW_TX_VARS
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_TX_VARS
===
===  DESCRIPTION
===
===     Show the 6 Transmit Variables packed into three 32-bit hex numbers
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_tx_vars(gas_id_t gas_id)
{
  /* Show tx vars in hex in 1 line or in decimal in 2 lines
  */
  uint32  u321=0L, u322=0L, u323=0L;

  /*    u321                 u322               u323
  **    ff  ff  ff  ff       ff  ff  ff  ff     ff  ff  ff  ff
  **    00  va  00  vs       00  vsf 00  seg    00  pvsf 0  nvsf
  */
  u321 = (uint32)((uint32)ul[gas_id].va << 16)       | (uint32)ul[gas_id].vs;
  u322 = (uint32)((uint32)ul[gas_id].vsf << 16)      | (uint32)ul[gas_id].seg_ind;
  u323 = (uint32)((uint32)ul[gas_id].pack_vsf << 16) | (uint32)ul[gas_id].nack_vsf;

  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_737, "VaVs %08x VfSg%08x PfNf %08x",u321,u322,u323);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u321; (void)u322; (void)u323;

} /* grlc_ul_show_tx_vars */

#endif /* GSM_GPRS_GRLU_SHOW_TX_VARS */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_FIFO_VARS
===
===  DESCRIPTION
===
===     Show Uplink FIFO variables
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_fifo_vars (gas_id_t gas_id)
{
  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_738, "GRLU Ptx ctr=%d in=%d out=%d",
          grlc_ul_fifo[gas_id].ptx_ctr,
          grlc_ul_fifo[gas_id].ptx_in_ind,
          grlc_ul_fifo[gas_id].ptx_out_ind);
  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_739, "GRLU Ptx read_ctr=%d read_out=%d validity=%d",
          grlc_ul_fifo[gas_id].ptx_read_ctr,
          grlc_ul_fifo[gas_id].ptx_read_out_ind,
          grlc_ul_fifo[gas_id].b_validity);

  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_740, "GRLU Pak ctr=%d in=%d out=%d",
          grlc_ul_fifo[gas_id].pack_ctr,
          grlc_ul_fifo[gas_id].pack_in_ind,
          grlc_ul_fifo[gas_id].pack_out_ind);
  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_741, "GRLU Pak read_ctr=%d read_out=%d dup free=%u",
          grlc_ul_fifo[gas_id].pack_read_ctr,
          grlc_ul_fifo[gas_id].pack_read_out_ind,
          DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL));

  MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_742, "GRLU Nak ctr=%d in=%d out=%d",
          grlc_ul_fifo[gas_id].nack_ctr,
          grlc_ul_fifo[gas_id].nack_in_ind,
          grlc_ul_fifo[gas_id].nack_out_ind);
  MSG_GRLC_EXT_MED_2_G (GRLC_F3_MSG_ENUM_743, "GRLU Nak read_ctr=%d read_out=%d",
          grlc_ul_fifo[gas_id].nack_read_ctr,
          grlc_ul_fifo[gas_id].nack_read_out_ind);
}

/*===========================================================================
===
===  FUNCTION      SHOW_RBB
===
===  DESCRIPTION
===
===     Format the RBB array into uint32 such that the bitmap can be displayed
===     as two 32-bit hex numbers
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_rbb
(
  gas_id_t                gas_id,
  const rm_acknack_desc_t *desc_ptr,
  uint8                   fai
)
{
  uint16 u;
  uint32 hu32=0L, lu32=0L, lu1=0L;

  for (u=0; u<4; u++)
  {
    /* RBB[4..7] into the lower 32 bits
    */
    lu32 |= (desc_ptr->rbb[u+4] << (8*((4-u)-1)) );

    /* RBB[0..3] into the higher 32 bits
    */
    hu32 |= (desc_ptr->rbb[u] << (8*((4-u)-1)) );
  }

  /* Pack SS(SSN), 00OOOO(ul[gas_id].llc_octets) into a 32 bit hex
  ** ff ff ff ff
  ** SS Oc te ts
  **  | ||
  **  | |16
  **  | 20
  **  24
  */
  lu1 = (((uint32)desc_ptr->ssn & 0xff)   << 24 |
         ((uint32)ul[gas_id].llc_octets & 0xffffff)
        );

  if (fai == RM_ACKNACK_FAI_TBF_COMPLETE)
  {
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_744, "GRLU FA=1 SnOctets=%08x 0x%08x %08x",lu1,hu32,lu32);
  }
  else
  {
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_745, "GRLU FA=0 SnOctets=%08x 0x%08x %08x",lu1,hu32,lu32);
    GRLC_UL_SHOW_TX_VARS(gas_id);
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu1;
} /* show_rbb */


/*===========================================================================
===
===  FUNCTION      E_SHOW_RBB
===
===  DESCRIPTION
===
===     Format and display the EGPRS acknack desc in the most compact form
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_show_rbb (gas_id_t gas_id, const egprs_ack_nack_des_t *desc_ptr, uint8 fai)
{
  /* Three 32-bit words representing the fields:
  **
  ** FBEC SSSN                           CRBB_len(decimal)  URBB_len(decimal)
  ** |||| ||_|_Starting-sequence-number
  ** |||| |_Starting colour code
  ** ||||_Compressed_present
  ** |||_End-of-window
  ** ||_Beginning-of-window
  ** |_Final-ack-ind
  **
  ** If compressed bitmap is present
  **
  ** crbb
  */

  uint32 l1=0L;
  NOTUSED(fai);

  /* Format Fai, bow, eow, comp_present, scc and ssn fields
  */
  l1  = ((uint32)desc_ptr->fai << 28);
  l1 |= ((uint32)desc_ptr->bow << 24);
  l1 |= ((uint32)desc_ptr->eow << 20);
  l1 |= ((uint32)desc_ptr->comp_present << 16);

  l1 |= ((uint32)desc_ptr->scc << 12);
  l1 |= ((uint32)desc_ptr->ssn & 0x0fff);

  MSG_GRLC_EXT_HIGH_3_G (GRLC_F3_MSG_ENUM_746, "ERLU FBECSSsn=%08x crbb_len=%u urbb_len=%u",
          l1,
          (desc_ptr->comp_present?desc_ptr->crbb_len:0),
          desc_ptr->urbb_len);

  if (desc_ptr->comp_present)
  {
    /* show crbb[4] of when compressed bitmap is present
    */
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_747, "ERLU crbb[2]=%08x [1]%08x [0]%08x",
            desc_ptr->crbb[2],
            desc_ptr->crbb[1],
            desc_ptr->crbb[0]);
    MSG_GRLC_EXT_MED_1_G (GRLC_F3_MSG_ENUM_748, "ERLU crbb[3]=%08x",desc_ptr->crbb[3]);
  }

  if (desc_ptr->urbb_len)
  {
    /* Show the first 3 words (96 bits)
    */
    MSG_GRLC_EXT_MED_3_G (GRLC_F3_MSG_ENUM_749, "ERLU urbb[2]=%08x [1]%08x [0]%08x",
            desc_ptr->urbb[2],
            desc_ptr->urbb[1],
            desc_ptr->urbb[0]);

    /* Show the next 3 words only if urbb_len allows it
    */
    if (desc_ptr->urbb_len > 32*3)
    {
      MSG_GRLC_EXT_MED_2_G (GRLC_F3_MSG_ENUM_750, "ERLU urbb[4]=%08x [3]%08x",desc_ptr->urbb[4], desc_ptr->urbb[3]);
    }
  }
} /* e_show_rbb */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_pdus
===
===  DESCRIPTION
===
===     List each of the uplink PDUs in the pending-ack queue, the temporary
===     queue and the main queue. The PDUs are NOT removed from the queues
===
===  DEPENDENCIES
===     When this function is called a soft recovery is about to be executed so
===     no state preservation is required
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_pdus ( gas_id_t gas_id )
{
  gprs_grr_ul_mes_t*  pdu_ptr;

  MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_751, "Temp Queue = %u items", q_cnt(&ULTQ(gas_id)));

  /* Read the top-most PDU - it is not removed from the queue
  */
  pdu_ptr = q_check( &ULTQ(gas_id) );
  while ( pdu_ptr != NULL )
  {
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_752, "psn=%u orig_len=%u ptr=0x%x",
        pdu_ptr->psn,
        pdu_ptr->orig_len,
        pdu_ptr->pdu_ptr);
    grlc_ul_show_llc_pdu_type(gas_id, pdu_ptr);

    /* read the next PDU - again it is not removed from the queue
    */
    pdu_ptr = q_next( &ULTQ(gas_id), &pdu_ptr->link );
  }

  MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_753, "Main Queue = %u items", q_cnt(&ULQ(gas_id)));

  /* Read the top-most PDU - it is not removed from the queue
  */
  pdu_ptr = q_check( &ULQ(gas_id) );
  while ( pdu_ptr != NULL )
  {
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_754, "psn=%u orig_len=%u ptr=0x%x",
        pdu_ptr->psn,
        pdu_ptr->orig_len,
        pdu_ptr->pdu_ptr);
    grlc_ul_show_llc_pdu_type(gas_id, pdu_ptr);

    /* read the next PDU - again it is not removed from the queue
    */
    pdu_ptr = q_next( &ULQ(gas_id), &pdu_ptr->link );
  }

} /* grlc_ul_show_pdus */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_vars
===
===  DESCRIPTION
===
===     Show the main variables in RLC UL in verbose format
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_vars ( gas_id_t gas_id )
{
  uint16  al;       /* actual length */
  uint16  qcnt;

  if (cur_pdu_ptr[gas_id] != NULL)
  {
    al = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr[gas_id]->pdu_ptr);
  }
  else
  {
    al = 9999;
  }
  qcnt = (uint16)q_cnt(&ULQ(gas_id));

  MSG_GRLC_ERROR_0_G (GRLC_F3_MSG_ENUM_755, "GRLU Dump Mode");
  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_756, "GRLU Diag CntA=%d CntB=%d CntC=%d",
            ul_diag_stats_st[gas_id].ul_gen_purpose_count_a,
            ul_diag_stats_st[gas_id].ul_gen_purpose_count_b,
            ul_diag_stats_st[gas_id].ul_gen_purpose_count_c);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_757, "GRLU  va=%d  vs=%d vsf=%d",
            ul[gas_id].va,
            ul[gas_id].vs,
            ul[gas_id].vsf);
  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_758, "GRLU seg=%d p_f=%d n_f=%d",
            ul[gas_id].seg_ind,
            ul[gas_id].pack_vsf,
            ul[gas_id].nack_vsf);
  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
    e_show_rbb(gas_id, &ul[gas_id].egprs_acknack_desc,
                ul[gas_id].egprs_acknack_desc.fai);
  else
    show_rbb(gas_id, &ul[gas_id].ul_acknack,ul[gas_id].ul_acknack.fai);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_759, "GRLU tbc=%u abs=%u si=%d", ul[gas_id].tbc, ul[gas_id].abs_bsn, ul[gas_id].si);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_760, "GRLU #p=%u #octs=%lu #grtd=%u",
            ul[gas_id].llc_pdus,
            ul[gas_id].llc_octets,
            ul[gas_id].blocks_granted);
  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_761, "GRLU cur_blk bsn=%d ofs=%d a_bsn=%d",
            cur_blk[gas_id].bsn,
            cur_blk[gas_id].offset,
            cur_blk[gas_id].abs_bsn);
  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_762, "GRLU cur_blk data_len=%d rem_cnt=%d cv=%d",
            cur_blk[gas_id].data_len,
            cur_blk[gas_id].rem_cnt,
            cur_blk[gas_id].mac_hdr.type.ul_data.cv);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_763, "GRLU b_access = %u 1st_psn_to_free %u fctrl=%u",
            ul[gas_id].b_access,
            ul[gas_id].first_psn_to_free,
            ul[gas_id].flow_control_suspend);

  if (cur_pdu_ptr[gas_id]  != NULL &&
      next_pdu_ptr[gas_id] != NULL
     )
  {
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_764, "GRLU curp=0x%x nxt=0x%x #q=%d",
              cur_pdu_ptr[gas_id],
              next_pdu_ptr[gas_id],
              qcnt);
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_765, "GRLU p=%u last_p=%u pgstate=%d",
              cur_pdu_ptr[gas_id]->psn,
              cur_pdu_ptr[gas_id]->pg_last_psn,
              cur_pdu_ptr[gas_id]->pg_state);
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_766, "GRLU cur#blks=%u next#blks=%u next_s=%d",
              cur_pdu_ptr[gas_id]->pg_num_blks,
              next_pdu_ptr[gas_id]->pg_num_blks,
              next_pdu_ptr[gas_id]->pg_state);
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_767, "GRLU len=%u act_len=%u rem=%u",
              cur_pdu_ptr[gas_id]->len,
              al,
              cur_pdu_ptr[gas_id]->rem_ind);
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_768, "GRLU ori_len=%u ptc=%d rprio=%d",
              cur_pdu_ptr[gas_id]->orig_len,
              cur_pdu_ptr[gas_id]->peak_throughput,
              cur_pdu_ptr[gas_id]->radio_priority);
    MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_769, "GRLU #blks=%u restore=%u #pdus=%u",
              cur_pdu_ptr[gas_id]->pg_num_blks_cur_pdu,
              cur_pdu_ptr[gas_id]->restore_cnt,
              cur_pdu_ptr[gas_id]->pg_pdus);
  }

  grlc_ul_show_pdus(gas_id);

  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_770, "GRLU PtxCtr=%d PtxRdCtr=%d NakCtr=%d",
            grlc_ul_fifo[gas_id].ptx_ctr,
            grlc_ul_fifo[gas_id].ptx_read_ctr,
            grlc_ul_fifo[gas_id].nack_ctr);
  MSG_GRLC_HIGH_3_G (GRLC_F3_MSG_ENUM_771, "GRLU NakRdCtr=%d PakCtr=%d PakRdCtr=%d",
            grlc_ul_fifo[gas_id].nack_read_ctr,
            grlc_ul_fifo[gas_id].pack_ctr,
            grlc_ul_fifo[gas_id].pack_read_ctr);
  MSG_GRLC_HIGH_2_G (GRLC_F3_MSG_ENUM_772, "GRLU validity=%d dup free = %u",
            grlc_ul_fifo[gas_id].b_validity,
            DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL));

  if (grlc_ul_fifo[gas_id].ptx_ctr != 0       ||
      grlc_ul_fifo[gas_id].ptx_read_ctr != 0  ||
      grlc_ul_fifo[gas_id].nack_ctr != 0      ||
      grlc_ul_fifo[gas_id].nack_read_ctr != 0 ||
      grlc_ul_fifo[gas_id].pack_ctr != 0      ||
      grlc_ul_fifo[gas_id].pack_read_ctr
     )
  {
    MSG_GRLC_HIGH_0_G (GRLC_F3_MSG_ENUM_773, "GRLU PL1_NO_GET");
  }

  MSG_GRLC_HIGH_2_G (GRLC_F3_MSG_ENUM_774, "GRLU dup free = %u tot = %u",
            DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
            DSM_POOL_ITEM_CNT(DSM_DUP_ITEM_POOL)
            );

  MSG_GRLC_HIGH_2_G (GRLC_F3_MSG_ENUM_775, "GRLU large free = %u tot = %u",
            DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
            DSM_POOL_ITEM_CNT(DSM_DS_LARGE_ITEM_POOL)
            );

  MSG_GRLC_HIGH_2_G (GRLC_F3_MSG_ENUM_776, "GRLU small free = %u tot = %u",
            DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
            DSM_POOL_ITEM_CNT(DSM_DS_SMALL_ITEM_POOL)
            );

} /* grlc_ul_show_vars */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_dsm_info
===
===  DESCRIPTION
===
===     Show DSM Memory info
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_dsm_info ( gas_id_t gas_id )
{
  uint32  u1 = 0L, u2=0L, u3=0L;

  u1 = (DSM_POOL_ITEM_CNT(DSM_DS_SMALL_ITEM_POOL) << 16) | DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL);
  u2 = (DSM_POOL_ITEM_CNT(DSM_DUP_ITEM_POOL)      << 16) | DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL);

  #if (DSM_MAJOR_VER_NO == 2)
    u3 = (dsmi_pool_mgmt_table[DSM_DS_SMALL_ITEM_POOL].stats_min_free_count <<16 ) |
        dsmi_pool_mgmt_table[DSM_DUP_ITEM_POOL].stats_min_free_count;
  #else
    u3 = (dsm_ds_small_item_pool.stats_min_free_count) << 16 | dsm_dup_item_pool.stats_min_free_count;
  #endif

  MSG_GRLC_LOW_3_G (GRLC_F3_MSG_ENUM_777, "SmCntFree=%08x ,DupCntFree=%08x , SmMinDupMin=%08x",u1,u2,u3);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u1; (void)u2; (void)u3;

}/* grlc_ul_show_dsm_info*/


#if  defined (DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST/PERLUTF */

#if  defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST/PERLUTF */

/*===========================================================================
===
===  FUNCTION      SHOW_GROUP_STATUS()
===
===  DESCRIPTION
===
===    Show PDU group status info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_group_status (gas_id_t gas_id)
{
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_STATUS
  if (cur_pdu_ptr[gas_id] != NULL)
  {
    MSG_GRLC_MED_3_G (GRLC_F3_MSG_ENUM_796, "GRLU pg psn=%u state=%d last_p%u",
            cur_pdu_ptr[gas_id]->psn,
            cur_pdu_ptr[gas_id]->pg_state,
            cur_pdu_ptr[gas_id]->pg_last_psn);
  }
#endif
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Uplink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_statistics_t* grlc_diag_get_ul_statistics (gas_id_t gas_id)
{
  static grlc_diag_ul_statistics_t  temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_stats_st[gas_id];

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Statistics
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_statistics (gas_id_t gas_id)
{
  (void)memset(&ul_diag_stats_st[gas_id], 0, sizeof(ul_diag_stats_st[gas_id]));
}


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Abnornal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_abnormal_release_counts_t* grlc_diag_get_ul_abnormal_release_counts
(
  gas_id_t gas_id
)
{
  static grlc_diag_ul_abnormal_release_counts_t   temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_abn_rel_cnts_st[gas_id];

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Abnormal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_abnormal_release_counts (gas_id_t gas_id)
{
  (void)memset(&ul_diag_abn_rel_cnts_st[gas_id], 0, sizeof(ul_diag_abn_rel_cnts_st[gas_id]));
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_event_counts_t* grlc_diag_get_ul_event_counts ( gas_id_t gas_id )
{
  static grlc_diag_ul_event_counts_t  temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_evt_cnts_st[gas_id];

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_event_counts (gas_id_t gas_id)
{
  (void)memset(&ul_diag_evt_cnts_st[gas_id], 0, sizeof(ul_diag_evt_cnts_st[gas_id]));
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_PDU_STAT_ADD_ENTRY
===
===  DESCRIPTION
===
===     Adds an uplink PDU to the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_diag_pdu_stat_add_entry(gprs_grr_ul_mes_t *ptr, const gas_id_t gas_id)
{
  grlc_diag_ul_pdu_t *diag_ptr;

  if (ptr == NULL)
  {
    return;
  }

  if (ul_diag_pdu_stat[gas_id].cnt == GRLC_DIAG_UL_PDU_BUF_MAX)
  {
    rlc_log_ul_statistics_ext(gas_id);
  }

  diag_ptr = &ul_diag_pdu_stat[gas_id].buf[ul_diag_pdu_stat[gas_id].index];
  diag_ptr->psn             = ptr->psn;
  diag_ptr->len             = ptr->len;
  diag_ptr->pdu_prio        = ptr->pdu_prio;
  diag_ptr->rlc_mode        = ptr->rlc_mode;
  diag_ptr->peak_throughput = ptr->peak_throughput;
  diag_ptr->radio_priority  = ptr->radio_priority;
  diag_ptr->cause           = ptr->cause;
  diag_ptr->pfc             = IS_PFC_NW_SUPPORTED(gas_id) ? 1 : 0;
  diag_ptr->pfi             = ptr->pfi;

  INC_MOD(ul_diag_pdu_stat[gas_id].index, GRLC_DIAG_UL_PDU_BUF_MAX);
  ul_diag_pdu_stat[gas_id].cnt++;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_PDU_STAT_REMOVE_ENTRY
===
===  DESCRIPTION
===
===     Removes the oldest uplink PDU from the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Uplink PDU if present, otherwise NULL
===
===  SIDE EFFECTS
===
===========================================================================*/
grlc_diag_ul_pdu_t* grlc_ul_diag_pdu_stat_remove_entry(const gas_id_t gas_id)
{
  grlc_diag_ul_pdu_t *diag_pdu = NULL;

  if (ul_diag_pdu_stat[gas_id].cnt > 0)
  {
    /* Remove the oldest PDU */
    uint8 remove_index = MOD_DIF(ul_diag_pdu_stat[gas_id].index, ul_diag_pdu_stat[gas_id].cnt, GRLC_DIAG_UL_PDU_BUF_MAX);
    diag_pdu = &ul_diag_pdu_stat[gas_id].buf[remove_index];
    DEC_MOD(ul_diag_pdu_stat[gas_id].cnt, GRLC_DIAG_UL_PDU_BUF_MAX);
  }

  return diag_pdu;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_PDU_STAT_GET_COUNT
===
===  DESCRIPTION
===
===     Returns number of Uplink PDUs present in the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Number of Uplink PDUs present
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 grlc_ul_diag_pdu_stat_get_count(const gas_id_t gas_id)
{
  return ul_diag_pdu_stat[gas_id].cnt;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_BLOCK_STAT_ADD_ENTRY
===
===  DESCRIPTION
===
===     Adds an Uplink Data Block to the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_diag_block_stat_add_entry(grlc_ul_data_block_t *ptr, const gas_id_t gas_id)
{
  grlc_diag_ul_block_t *diag_ptr;

  if (ptr == NULL)
  {
    return;
  }

  if (ul_diag_block_stat[gas_id].cnt == GRLC_DIAG_UL_BLOCK_BUF_MAX)
  {
    rlc_log_ul_statistics_ext(gas_id);
  }

  diag_ptr = &ul_diag_block_stat[gas_id].buf[ul_diag_block_stat[gas_id].index];
  diag_ptr->bsn       = ptr->bsn;
  diag_ptr->first_psn = ptr->first_psn;
  diag_ptr->last_psn  = ptr->last_psn;

  INC_MOD(ul_diag_block_stat[gas_id].index, GRLC_DIAG_UL_BLOCK_BUF_MAX);
  ul_diag_block_stat[gas_id].cnt++;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_BLOCK_STAT_REMOVE_ENTRY
===
===  DESCRIPTION
===
===     Removes the oldest Uplink Data Block from the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Uplink data block if present, otherwise NULL
===
===  SIDE EFFECTS
===
===========================================================================*/
grlc_diag_ul_block_t* grlc_ul_diag_block_stat_remove_entry(const gas_id_t gas_id)
{
  grlc_diag_ul_block_t *diag_blk = NULL;

  if (ul_diag_block_stat[gas_id].cnt > 0)
  {
    /* Remove the oldest data block */
    uint8 remove_index = MOD_DIF(ul_diag_block_stat[gas_id].index, ul_diag_block_stat[gas_id].cnt, GRLC_DIAG_UL_BLOCK_BUF_MAX);
    diag_blk = &ul_diag_block_stat[gas_id].buf[remove_index];
    DEC_MOD(ul_diag_block_stat[gas_id].cnt, GRLC_DIAG_UL_BLOCK_BUF_MAX);
  }

  return diag_blk;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_BLOCK_STAT_GET_COUNT
===
===  DESCRIPTION
===
===     Returns number of Uplink Data Blocks present in the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Number of Uplink Data blocks present
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 grlc_ul_diag_block_stat_get_count(const gas_id_t gas_id)
{
  return ul_diag_block_stat[gas_id].cnt;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_TX_STAT_ADD_ENTRY
===
===  DESCRIPTION
===
===     Adds Uplink Tx Block info to the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_diag_tx_stat_add_entry(grlc_ul_tx_array_t *ptr, const gas_id_t gas_id)
{
  grlc_diag_ul_tx_t *diag_ptr;

  if (ptr == NULL)
  {
    return;
  }

  if (ul_diag_tx_stat[gas_id].cnt == GRLC_DIAG_UL_TX_BUF_MAX)
  {
    rlc_log_ul_statistics_ext(gas_id);
  }

  diag_ptr = &ul_diag_tx_stat[gas_id].buf[ul_diag_tx_stat[gas_id].index];
  diag_ptr->ack_state = ptr->vb;
  diag_ptr->bsn       = ptr->blk.bsn;
  diag_ptr->len       = ptr->blk.data_len;
  diag_ptr->tbc       = ul[gas_id].tbc;
  diag_ptr->abs_bsn   = ptr->blk.abs_bsn;
  diag_ptr->cv        = ptr->blk.hdr.cv;
  diag_ptr->cs        = ptr->blk.cs;
  diag_ptr->cps       = ptr->blk.hdr.cps;
  diag_ptr->si        = ptr->blk.hdr.si;

  INC_MOD(ul_diag_tx_stat[gas_id].index, GRLC_DIAG_UL_TX_BUF_MAX);
  ul_diag_tx_stat[gas_id].cnt++;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_TX_STAT_REMOVE_ENTRY
===
===  DESCRIPTION
===
===     Removes the oldest Uplink Tx Block info from the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Uplink Tx block if present, otherwise NULL
===
===  SIDE EFFECTS
===
===========================================================================*/
grlc_diag_ul_tx_t* grlc_ul_diag_tx_stat_remove_entry(const gas_id_t gas_id)
{
  grlc_diag_ul_tx_t *diag_tx = NULL;

  if (ul_diag_tx_stat[gas_id].cnt > 0)
  {
    /* Remove the oldest Tx block */
    uint8 remove_index = MOD_DIF(ul_diag_tx_stat[gas_id].index, ul_diag_tx_stat[gas_id].cnt, GRLC_DIAG_UL_TX_BUF_MAX);
    diag_tx = &ul_diag_tx_stat[gas_id].buf[remove_index];
    DEC_MOD(ul_diag_tx_stat[gas_id].cnt, GRLC_DIAG_UL_TX_BUF_MAX);
  }

  return diag_tx;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_TX_STAT_GET_COUNT
===
===  DESCRIPTION
===
===     Returns number of Uplink Tx Blocks present in the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Number of Uplink Tx blocks present
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 grlc_ul_diag_tx_stat_get_count(const gas_id_t gas_id)
{
  return ul_diag_tx_stat[gas_id].cnt;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DIAG_CLEAR_STAT
===
===  DESCRIPTION
===
===     Clears the Uplink diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_diag_clear_stat(const gas_id_t gas_id)
{
  if (ul_diag_pdu_stat[gas_id].cnt != 0)
  {
    MSG_GERAN_ERROR_1_G("GRLC UL Statistics:: PDU counter (%d) reset to zero", ul_diag_pdu_stat[gas_id].cnt);
    ul_diag_pdu_stat[gas_id].cnt = 0;
  }

  if (ul_diag_block_stat[gas_id].cnt != 0)
  {
    MSG_GERAN_ERROR_1_G("GRLC UL Statistics:: Data Block counter (%d) reset to zero", ul_diag_block_stat[gas_id].cnt);
    ul_diag_block_stat[gas_id].cnt = 0;
  }

  if (ul_diag_tx_stat[gas_id].cnt != 0)
  {
    MSG_GERAN_ERROR_1_G("GRLC UL Statistics:: RX Block counter (%d) reset to zero", ul_diag_tx_stat[gas_id].cnt);
    ul_diag_tx_stat[gas_id].cnt = 0;
  }
}

/*** EOF: don't remove! ***/
