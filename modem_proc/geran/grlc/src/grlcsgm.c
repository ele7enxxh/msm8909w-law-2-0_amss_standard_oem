/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC SEGMENT MODULE
***
***
*** DESCRIPTION
***
***  This module contains data and functions relating to segmentation of LL PDU
***  into RLC data blocks
***
***
*** EXTERNALIZED FUNCTIONS
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcsgm.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 25/11/03    hv     Created
*** 22/04/04    hv     Added e_calc_cv() and place holders for other EGPRS fns
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gprs_pdu.h"
#include "grlcultypes.h"
#include "grlctx.h"
#include "grlcgrp.h"
#include "grlcpaq.h"
#include "grlculproc.h"
#include "grlctsk.h"

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

/* Current rlc uplink data block being worked on. This may already contain a
** number of short LLC pdu(s)
*/
grlc_ul_data_block_t   cur_blk[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
** Table of EGPRS Coding and Puncturing Sheme (CPS) values for header type 1.
** This table is used in macro GET_CPS_HDR_1() to return the CPS value.
**------------------------ ---------------------------------------------------
*/
const uint8 E_GRLC_HDR1_CPS[] =
{
  MCS_9_P1_MCS_9_P1,
  MCS_9_P1_MCS_9_P2,
  MCS_9_P1_MCS_9_P3,
  MCS_9_P2_MCS_9_P1,
  MCS_9_P2_MCS_9_P2,
  MCS_9_P2_MCS_9_P3,
  MCS_9_P3_MCS_9_P1,
  MCS_9_P3_MCS_9_P2,
  MCS_9_P3_MCS_9_P3,
  MCS_8_P1_MCS_8_P1,
  MCS_8_P1_MCS_8_P2,
  MCS_8_P1_MCS_8_P3,
  MCS_8_P2_MCS_8_P1,
  MCS_8_P2_MCS_8_P2,
  MCS_8_P2_MCS_8_P3,
  MCS_8_P3_MCS_8_P1,
  MCS_8_P3_MCS_8_P2,
  MCS_8_P3_MCS_8_P3,
  MCS_7_P1_MCS_7_P1,
  MCS_7_P1_MCS_7_P2,
  MCS_7_P1_MCS_7_P3,
  MCS_7_P2_MCS_7_P1,
  MCS_7_P2_MCS_7_P2,
  MCS_7_P2_MCS_7_P3,
  MCS_7_P3_MCS_7_P1,
  MCS_7_P3_MCS_7_P2,
  MCS_7_P3_MCS_7_P3
};

/*----------------------------------------------------------------------------
** Table of EGPRS Coding and Puncturing Sheme (CPS) values for header type 2.
** This table is used in macro GET_CPS_HDR_2() to return the CPS value.
**------------------------ ---------------------------------------------------
*/
const uint8 E_GRLC_HDR2_CPS[] =
{
  MCS_6_P1,
  MCS_6_P2,
  MCS_6_P1_PAD_MSC_8_RETX,
  MCS_6_P2_PAD_MCS_8_RETX,
  MCS_5_P1,
  MCS_5_P2
};

/*----------------------------------------------------------------------------
** Table of EGPRS Coding and Puncturing Sheme (CPS) values for header type 3.
** This table is used in macro GET_CPS_HDR_3() to return the CPS value.
**------------------------ ---------------------------------------------------
*/
const uint8 E_GRLC_HDR3_CPS[] =
{
  MCS_4_P1,
  MCS_4_P2,
  MCS_4_P3,
  MCS_3_P1,
  MCS_3_P2,
  MCS_3_P3,
  MCS_3_P1_PAD_MCS_8_RETX,
  MCS_3_P2_PAD_MCS_8_RETX,
  MCS_3_P3_PAD_MCS_8_RETX,
  MCS_2_P1,
  MCS_2_P2,
  MCS_1_P1,
  MCS_1_P2
};

/*-----------------------------------------------------------------------------
** Table to select MCS for retransmissions with resegmentation (Resegment=1).
** TS 04.60 Table 8.1.1.1
** The row contains the MCS for initial transmission and the columns comntain
** the commanded MCS (which include MCS_6_9 and MCS-5_7). Note that MCS-6-9
** MCS-5-7 have been moved to the start of the table due to their values:
** #define RM_MCS_5_7   9
** #define RM_MCS_6_9   10
**-----------------------------------------------------------------------------
*/
const uint8 E_GRLC_MCS_RESEGMENT_SET[9][11] =
{
  /* Init/Commanded
  **  MCS      MCS-6-9   MCS-5-7     MCS-9     MCS-8     MCS-7     MCS-6     MCS-5     MCS-4     MCS-3     MCS-2     MCS-1
  */
  /* MCS-9 */  { RM_MCS_6, RM_MCS_3, RM_MCS_9, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3 },
  /* MCS-8 */  { RM_MCS_6, RM_MCS_3, RM_MCS_8, RM_MCS_8, RM_MCS_6, RM_MCS_6, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3 },
  /* MCS-7 */  { RM_MCS_5, RM_MCS_5, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_5, RM_MCS_5, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2 },
  /* MCS-6 */  { RM_MCS_9, RM_MCS_3, RM_MCS_9, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3 },
  /* MCS-5 */  { RM_MCS_5, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_5, RM_MCS_5, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2 },
  /* MCS-4 */  { RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_1, RM_MCS_1, RM_MCS_1 },
  /* MCS-3 */  { RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3 },
  /* MCS-2 */  { RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2 },
  /* MCS-1 */  { RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1 },
};

/*-----------------------------------------------------------------------------
** Table to select MCS for retransmissions without resegmentation (Resegment=0).
** TS 04.60 Table 8.1.1.2
** The row contains the MCS for initial transmission and the columns comntain
** the commanded MCS (which include MCS_6_9 and MCS-5_7). Note that MCS-6-9
** MCS-5-7 have been moved to the start of the table due to their values:
** #define RM_MCS_5_7   9
** #define RM_MCS_6_9   10
**-----------------------------------------------------------------------------
*/
const uint8 E_GRLC_MCS_RESEGMENT_CLR[9][11] =
{
  /* Init/Commanded
  ** MCS       MCS-6-9   MCS-5-7     MCS-9     MCS-8     MCS-7     MCS-6     MCS-5     MCS-4     MCS-3     MCS-2     MCS-1
  */
  /* MCS-9 */  { RM_MCS_6, RM_MCS_6, RM_MCS_9, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6 },
  /* MCS-8 */  { RM_MCS_6, RM_MCS_6, RM_MCS_8, RM_MCS_8, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6 },
  /* MCS-7 */  { RM_MCS_5, RM_MCS_5, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5 },
  /* MCS-6 */  { RM_MCS_9, RM_MCS_6, RM_MCS_9, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6, RM_MCS_6 },
  /* MCS-5 */  { RM_MCS_5, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_7, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5, RM_MCS_5 },
  /* MCS-4 */  { RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4, RM_MCS_4 },
  /* MCS-3 */  { RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3, RM_MCS_3 },
  /* MCS-2 */  { RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2, RM_MCS_2 },
  /* MCS-1 */  { RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1, RM_MCS_1 },
};


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FREE_BLOCKS()
===
===  DESCRIPTION
===
===    Free 'num_blks' from the TX Array starting from 'bsn' and mark deleted
===    inactive.
===
===  DEPENDENCIES
===
===  PARAMETERS
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
static void grlc_ul_free_blocks (gas_id_t gas_id, uint16 bsn, uint16 num_blks)
{
  uint16 u, ind;

  for (u=0; u<num_blks; u++)
  {
    ind=MOD_SNS(u+bsn);

    /* Free the dup as well if required
    */
    if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr != NULL)
    {
      GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
    }

    if (TX_ARR(gas_id, ind).blk.dsm_ptr != NULL)
    {
      GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dsm_ptr);
      /* change the ack status to prevent the block being freed again
      */
      GRLC_UL_TX_VARS_LOCK(gas_id);
      TX_ARR(gas_id, ind).vb = TXA_INACTIVE;
      GRLC_UL_TX_VARS_UNLOCK(gas_id);
    }
  } /* end for */
} /* grlc_ul_free_blocks */

/*===========================================================================
===
===  FUNCTION      E_GRLC_UL_FREE_BLOCKS()
===
===  DESCRIPTION
===
===    Free 'num_blks' from the TX Array starting from 'bsn' and mark deleted
===    inactive.
===
===  DEPENDENCIES
===
===  PARAMETERS
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
static void e_grlc_ul_free_blocks (gas_id_t gas_id, uint16 bsn, uint16 num_blks)
{
  uint16 u, ind;

  for (u=0; u<num_blks; u++)
  {
    ind=MOD_ESNS(u+bsn);

    /* Free the dup as well if required
    */
    if (TX_ARR(gas_id, ind).blk.dup_dsm_ptr != NULL)
    {
      GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dup_dsm_ptr);
    }

    if (TX_ARR(gas_id, ind).blk.dsm_ptr != NULL)
    {
      GPRS_PDU_FREE(&TX_ARR(gas_id, ind).blk.dsm_ptr);
      /* change the ack status to prevent the block being freed again
      */
      GRLC_UL_TX_VARS_LOCK(gas_id);
      TX_ARR(gas_id, ind).vb = TXA_INACTIVE;
      GRLC_UL_TX_VARS_UNLOCK(gas_id);
    }
  }
} /* e_grlc_ul_free_blocks */

/*===========================================================================
===
===  FUNCTION      CALC_CV
===
===  DESCRIPTION
===
===    Calculate the Countdown Value variable as per GSM 04.60 v 6.9.0
===    Rel 97 9.3.1.
===
===
===  PARAMETERS
===
===    total_blocks_to_tx is the TBC.
===
===    Note that when TBF is forced to release due to lack of resource the
===    number of blocks to transmit should be that of the remained resource
===    and not the length of the PDU in blocks, ie ul[gas_id].blocks_rem instead of
===    ul[gas_id].blocks_req
===
===
===  DEPENDENCIES
===
===    bs_cv_max variable based on the BS_CV_MAX system info
===    NTS number of timeslots assigned to the uplink TBF (1..4 class 12+)
===       cur_res.num_timeslots
===    TBC number of rlc data blocks to be transmitted
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    return value of cv AND NOT UPDATE  ul[gas_id].cv atthis moment.
===
===========================================================================*/
static uint8 calc_cv
(
  gas_id_t      gas_id,
  const uint16  total_blocks_to_tx,
  const uint16  abs_bsn
)
{
  uint16 top, cv;

  /*---------------------------------------------------------------*/

  if (ul[gas_id].cur_res.num_timeslots == 0)
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_236, "GRLU calc_cv num_timeslots = 0 Forced to 1");
    ul[gas_id].cur_res.num_timeslots = 1;
  }

  /* Implement rounding function using modulo operation:
  ** CV = (top/bot) + ( (top % bot) == 0 ? 0 : 1 )
  ** bottom part is ul[gas_id].cur_res.num_timeslots
  */
  top = (total_blocks_to_tx - abs_bsn) - 1;

  if (ul[gas_id].cur_res.num_timeslots == 1)
  {
    cv = top;
  }
  else  /* num_timeslots > 1 */
  {
    cv = ( (top/ul[gas_id].cur_res.num_timeslots) +
           ( (top % ul[gas_id].cur_res.num_timeslots) == 0 ? 0 : 1 )
         );
  }

  /* if CV is below BS_CV_MAX then store the value for assigning to
  ** data blocks
  */
  if (cv <= BS_CV_MAX(gas_id))
  {
    /*----------------------------------------------
    ** Check for closing the group, if:
    ** 1. current group is PGS_OPEN, and
    ** 2. EXT_UTBF is NOT active, and
    ** 3. neither ptx_cv or pak_cv has counted down
    **----------------------------------------------
    */
    if (cur_pdu_ptr[gas_id]->pg_state == PGS_OPEN &&
        !IS_EXT_UTBF_ACTIVE(gas_id)             &&
        (ul[gas_id].ptx_cv != BS_CV_CONST ||
         ul[gas_id].pak_cv != BS_CV_CONST
        )
       )
    {
      cur_pdu_ptr[gas_id]->pg_state = PGS_CLOSE;
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_237, "GRLU calc_cv close psn %u last_p=%u",
              cur_pdu_ptr[gas_id]->psn,
              cur_pdu_ptr[gas_id]->pg_last_psn,
              0);
    }
  }
  else
  {
    /* if CV value is above BS_CV_MAX then clamp and store CV value
    */
    cv = (uint8)BS_CV_CONST;
  }

  return ((uint8)cv);
} /* calc_cv */

/*===========================================================================
===
===  FUNCTION      E_CALC_CV
===
===  DESCRIPTION
===
===    Calculate the Countdown Value variable as per EGPRS RLC LLD
===
===  PARAMETERS
===
===    - total_blocks_to_tx is the TBC, absolute bsn value and the MCS to format
===    - the block pointer to pass in the block MCS and the absolute BSN number
===
===  DEPENDENCIES
===
===    BS_CV_CONST based on the BS_CV_MAX system info
===    ul[gas_id].cur_res.num_timeslots (NTS)
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    return value of cv AND NOT UPDATE  ul[gas_id].cv atthis moment.
===
===
===========================================================================*/
static uint8 e_calc_cv
(
  gas_id_t                    gas_id,
  const uint16                total_blocks_to_tx,
  const grlc_ul_data_block_t  *bp
)
{
  uint16 top, bot, cv;

  /*---------------------------------------------------------------*/

  if (ul[gas_id].cur_res.num_timeslots == 0)
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_238, "ERLU e_calc_cv num_timeslots = 0 Forced to 1");
    ul[gas_id].cur_res.num_timeslots = 1;
  }

  /* CV = (top/bot) + ( (top % bot) == 0 ? 0 : 1 );
  */
  top = (total_blocks_to_tx - (bp->abs_bsn)) - 1;
  bot = ul[gas_id].cur_res.num_timeslots * GET_K(bp->cs);

  if (bot == 1)
  {
    cv = top;
  }
  else  /* num_timeslots > 1 */
  {
    cv = ( (top/bot) + ((top%bot) == 0 ? 0 : 1) );
  }

  #if 0
  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_239, "ERLU e_calc CV=%u tot=%u abs=%u", cv,total_blocks_to_tx,bp->abs_bsn);
  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_240, "ERLU e_calc CV top=%u bot=%u seg_ind=%u", top,bot,ul[gas_id].seg_ind);
  #endif

  /* if CV is below BS_CV_MAX then store the value for assigning to
  ** data blocks
  */
  if (cv <= BS_CV_MAX(gas_id))
  {
    #if 0
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_241, "ERLU e_calc CV started %u tot=%u abs=%u", cv,total_blocks_to_tx,
            bp->abs_bsn);
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_242, "ERLU e_calc CV top=%u bot=%u K=%u", top,bot,GET_K(bp->cs));
    MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_243, "ERLU e_calc CV ts=%u",ul[gas_id].cur_res.num_timeslots);
    #endif

    /*------------------------------------------------------------
    ** Check for closing the group, if:
    ** 1. current group is PGS_OPEN, and
    ** 2. EXT_UTBF is NOT active, and
    ** 3. neither ptx_cv or pak_cv has counted down
    **------------------------------------------------------------
    */
    if (cur_pdu_ptr[gas_id]->pg_state == PGS_OPEN &&
        !IS_EXT_UTBF_ACTIVE(gas_id)             &&
        (ul[gas_id].ptx_cv != BS_CV_CONST ||
         ul[gas_id].pak_cv != BS_CV_CONST
        )
       )
    {
      cur_pdu_ptr[gas_id]->pg_state = PGS_CLOSE;
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_244, "ERLU e_calc_cv close psn %u ptx_cv=%u pak_cv=%u",
        cur_pdu_ptr[gas_id]->psn,
        ul[gas_id].ptx_cv,
        ul[gas_id].pak_cv);
    }
  }
  else
  {
    /* if CV value is above BS_CV_MAX then clamp and store CV value
    */
    cv = (uint8)BS_CV_CONST;
  }

  return ((uint8)cv);
} /* e_calc_cv */


/*===========================================================================
===
===  FUNCTION      SEGMENT_PDU
===
===  DESCRIPTION
===
===    Segment the current PDU to produce a number of uplink data blocks
===    for transmission. The TLLI field will be included if IS_TLLI_REQUIRED()
===    is tested TRUE.
===
===    Padding of a partially-filled data block (where rem_cnt > 0) is done
===    elsewhere for efficientcy reasons.
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===    inputs: cur_pdu -- PDU being processed, pointed to by 'bp'
===            IS_TLLI_REQUIRED() determines if TLLI is required.
===
===    output: cur_blk
===
===  RETURN VALUE
===
===    segment_status_t
===
===  SIDE EFFECTS
===
===    xxx.
===
=== ! to send off block (for padding and transmission) if the CV is 0.
===
===========================================================================*/
segment_status_t segment_pdu( gas_id_t gas_id )
{
  /* pointer to current data block. Points to the uplink data block
  ** containing a result of the segmentation process.
  */
  grlc_ul_data_block_t	*bp;

  /* remained pdu length. Number of data octets remained to be segmented
  ** in the current pdu
  */
  uint16 rpl;

  /* data length: of data remained in the current data block
  */
  uint16 dl;

  /* number of data blocks produced within this segmentation. This is
  ** required to count the number of blocks in one phase access mode.
  */
  uint16 num_segs = 0;

  dsm_item_type *temp_dsm_ptr = NULL;

  uint16 blks_to_segment;
  uint8 cs;

  /*---------------------------------------------------------------------*/

  cs = (uint8)(IS_TLLI_REQUIRED(gas_id) ? ul[gas_id].tlli_cs : ul[gas_id].cur_res.cs);
  bp = &cur_blk[gas_id];

  if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
  {
    blks_to_segment =
      (uint16)MIN3( (MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].vsf, SNS) > RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE ?
                     0 :
                     RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE
                    ),
                    GET_ATTACH_WINDOW_SPACE(gas_id),
                    (uint16)(RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE -
                             grlc_ul_fifo[gas_id].ptx_ctr
                            )
                  );
  }
  else
  {
    blks_to_segment =
      (uint16)MIN3( (MOD_DIF(ul[gas_id].seg_ind, ul[gas_id].pack_vsf, SNS) > RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE ?
                     0 :
                     RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE
                    ),
                    GET_UNACK_TX_WINDOW_SPACE(gas_id),
                    (uint16)(RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE -
                             grlc_ul_fifo[gas_id].pack_ctr
                            )
                  );
  }
  /*-----------------------------------------------------------------------
  ** function structure
  **
  **
  **   1. Handle whole blocks but not the last whole block as this may
  **      incur 2 blocks
  **      eg. 42 octets, block size = 20. Do two blocks, drop into (3) to
  **      process the remaining 2 octets.
  **      eg. 60 octets, block size = 20. Do two blocks, drop into (2) to
  **      process the remaining 20 octets. If only 19 octets are processed
  **      then drop into (3) to process the last octet
  **
  **   while (pdu len > block len)
  **     process block
  **     if done
  **       exit
  **     else if reached limit
  **       return reached limit
  **     endif
  **   end while
  **
  **   2. Handle pdu len = block len. May incur 2 blocks
  **
  **   if (pdu len == block len)
  **     process block
  **   endif
  **
  **   3. Handle pdu len < block len
  **
  **   if (pdu len < block len)
  **     process block
  **     if block is partial
  **        check and send off block or hang on to block
  **     endif
  **   endif
  **
  **-----------------------------------------------------------------------
  */
  if (blks_to_segment == 0)
  {
#ifdef  GSM_GPRS_GRLU_SHOW_SEGMENT
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_245, "GRLU sgm 0 p=%u pl=%u last_p=%u",
            cur_pdu_ptr[gas_id]->psn,
            cur_pdu_ptr[gas_id]->len,
            cur_pdu_ptr[gas_id]->pg_last_psn);
#endif
    return ul[gas_id].segment_state;
  }

#ifdef  GSM_GPRS_GRLU_SHOW_SEGMENT
  if (cur_pdu_ptr[gas_id] != NULL)
  {
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_246, "GRLU SGM p=%u l=%u rem=%u",
            cur_pdu_ptr[gas_id]->psn,
            cur_pdu_ptr[gas_id]->len,
            cur_pdu_ptr[gas_id]->rem_ind);
  }
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_247, "GRLU ##SGM cur=NULL");
    return ul[gas_id].segment_state;
  }
#else
  if (cur_pdu_ptr[gas_id] == NULL)
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_248, "GRLU ##SGM cur=NULL");
    return ul[gas_id].segment_state;
  }
#endif

  /* work out the remaining data octets in the current pdu.
  ** It's the length of the complete pdu less the octets taken out so far.
  */
  rpl = cur_pdu_ptr[gas_id]->len - cur_pdu_ptr[gas_id]->rem_ind;

  /* to add case where rem_ind = len, ie. nothing more to segment
  */
  if (rpl == 0)
  {
    MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_249, "GRLU Info SGM rpl=0 #=%d pl=%d r=%d",
              blks_to_segment,
              cur_pdu_ptr[gas_id]->len,
              cur_pdu_ptr[gas_id]->rem_ind);
    return SEGMENT_DONE;
  }

  /* the last time we call NEW_DATA_BLOCK() rem_cnt is made up to date. But the
  ** contention resolution status may have changed since which would effect rem_cnt
  ** as the TLLI may/may not be required. So we re-affirm its value here
  */
  if (bp->num_ext_octets == 0)
  {
    if (IS_TLLI_REQUIRED(gas_id))
    {
      if( IS_PFI_REQUIRED(gas_id) )
      {
        bp->rem_cnt = (uint8)(GRLC_DATA_BLOCK_LEN[cs] -
                        (RM_RLC_HDR_LEN + RM_TLLI_LEN + RM_PFI_E_LEN));
      }
      else
      {
        bp->rem_cnt = (uint8)(GRLC_DATA_BLOCK_LEN[cs] -
                        (RM_RLC_HDR_LEN + RM_TLLI_LEN));
      }
    }
    else
    {
      bp->rem_cnt = (uint8)(GRLC_DATA_BLOCK_LEN[cs] - RM_RLC_HDR_LEN);
    }
  }

  /* remaining capacity to store data in the current data block
  */
  dl = bp->rem_cnt;

  /* copy coding scheme
  */
  bp->cs = cs;

  /*-------------------------------------------------------------------------
  ** Loop to segment until the remaining octets in the current pdu is less
  ** than or equal to the remaining octets in the current data block. During
  ** the loop, if a data block is full then it is attached to the tx buffer
  ** and cur_blk is re-initialised for re-use.
  **-------------------------------------------------------------------------
  */
  while (rpl > dl)
  {
    /* The data block can be new or partially filled as indicated by
    ** num_ext_octets
    **
    ** 1. new data block: data from pdu can occupy the whole block, no ext octet
    **    is required.
    ** 2. existing data block (ext octets != 0):
    */
    if (bp->num_ext_octets == 0)
    {
      /* (1) new data block: no ext octet required (as per B.7a)
      */
      INC_ABS_BSN(gas_id);
      /* we do not update ul[gas_id].cv for this block , as its not fully segmented
      ** we update the value when its attached to tx_arr.
      ** This is done to avoid doing a reprocess if cv=0 here and enter
      ** infinite segmentaion loop
      */
      PUT_UL_MAC_HDR(bp,calc_cv(gas_id, ul[gas_id].tbc, bp->abs_bsn),ul[gas_id].si,ul[gas_id].cur_res.retry);
      PUT_UL_RLC_HDR_OCTET_1(bp,RLC_SPARE,ul[gas_id].cur_res.ul_tfi);
      PUT_UL_RLC_HDR_OCTET_2(bp,(uint8)ul[gas_id].seg_ind,RM_EXT_NO_OCTET_FOLLOWS);
      bp->first_psn = cur_pdu_ptr[gas_id]->psn;
      bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
    }
    else
    {
      /* (2) existing data block. Only exists with at least 1 ext octet.
      ** Hence the boundary has been defined and a new ext octet is
      ** not required (as per B.7b), it simply occupies the
      ** remaining space of this data block
      **
      ** use existing MAC and RLC hdrs.
      ** set the more and ext fields of the last ext octet to
      ** more pdu (this one) and no ext octet follows.
      **
      ** num_ext_octets indexes the next octet hence to address the
      ** previous ext octet '-1' is used.
      */
      bp->ext_octet[bp->num_ext_octets-1].gprs.more = RM_MORE_PDU_FOLLOWS;
      bp->ext_octet[bp->num_ext_octets-1].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    }

    /*-----------------------------------------------------------------
    ** To convert to DSM operation the data stripped off a PDU (in DSM
    ** format) is stripped into another DSM item, pointed to by
    ** by bp->dsm_ptr. This replaces the bp->data[] array.
    **---------------------------------------------------------------------
    */

    /* Strip head of PDU into a temp dsm item
    ** the tertiary ensures the number of octets is limited to what is
    ** remained in the pdu.
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr),
                                           (uint16)(dl > rpl ? rpl : dl)
                                         );
    if (temp_dsm_ptr == NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_250, "GRLU ##sgm segment_head returns NULL");
      grlc_ul_show_dsm_info(gas_id);
      grlc_ul_show_vars(gas_id);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul[gas_id].b_error_recovery_required = TRUE;
      ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
      return SEGMENT_REACHED_NUM_BLOCKS;
    }

    /* Transfer the temp dsm item into the 'dsm_ptr' of the current data block.
    ** ie. the new block in appended to the existing block
    */
    gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

    /* temp should be NULL after call to augment_tail. If not flag error and free it
    */
    if (temp_dsm_ptr != NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_251, "GRLU ##sgm non NULL after augment_tail");
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }

    /* pdu length is now shorter
    */
    rpl -= dl;

    /* move remaining data index in the current pdu along
    */
    cur_pdu_ptr[gas_id]->rem_ind += dl;

    /* update length of current data block
    */
    bp->data_len += (uint8)dl;

    /* adjust count of remaining space in data block. Expect
    */
    bp->rem_cnt -= (uint8)dl;

    /* Pass on PDU priority info */
    bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

    /* commit data block to the tx array.
    */
    attach_block_to_tx_array(gas_id, bp);

    /* blank the 'current' data block now that it's been attached to
    ** the tx array
    */
    NEW_DATA_BLOCK(gas_id, bp);

    /* load new value of rem_cnt which is the maximum data octets according
    ** to the coding scheme less the length of TLLI block and the PFI/E octet
    ** if they need to be included in the block. There should be no extension
    ** octet for a new data block.
    */
    dl = bp->rem_cnt;

    /* one more data block
    */
    num_segs++;

    /* exit if number of blocks segmented has reached the
    ** maximum specified
    */
    if (rpl == 0)
      return SEGMENT_DONE;
    else if (num_segs >= blks_to_segment)
      return SEGMENT_REACHED_NUM_BLOCKS;

  };   /* while loop */

  /*---------------------------------------------------------------------------
  ** Checks the case where rpl = dl.
  **
  ** This last fragment may segment into 2 blocks due to the extension octet.
  **
  ** Three cases:
  **
  ** û1. CV = 0 && new data block (no extension octet)
  **   Last segment of pdu and last block of TBF
  **   E = RM_EXT_NO_OCTET_FOLLOWS
  **   All remaining octets into data block. No ext octet
  **
  ** ! 2. CV = 0 && old data block (ext octet > 0)
  **   Not a valid scenario because the segment associated with the extension
  **   octet would have been the final block of the TBF (for CV = 0) and the
  **   remaining octets would have been padded and sent off.
  **
  ** 3. CV != 0 && new data block (see B.2)
  **   Last segment of pdu but not the end of TBF hence we must add an
  **   extension octet to separate this final segment from the next pdu as a
  **   special case where LI is set to 0:
  **
  **   Add headers, ext octet, append rpl-1 octets, set LI to 0 (special case),
  **   exit if reached the number of blocks or drop onto the next part to place
  **   the last octet in a new block.
  **
  **   E = RM_EXT_OCTET_FOLLOWS
  **   E.M = RM_MORE_NO_PDU_FOLLOWS, E.E = RM_EXT_NO_OCTET_FOLLOWS
  **   E.LI = 0 (but in actual fact carries rpl-1 octets)
  **   Block is sent off and rpl = 1 where the next section will handle this
  **   final data octet.
  **
  ** 4. CV != 0 && old data block
  **   Last and only segment of pdu but not the end of a TBF hence we must add
  **   an extension octet to separate this final segment from the next pdu.
  **   The length of the extension octet will be set to 0 as in case (3):
  **
  **   Change existing extension octet as follows:
  **   E.M = RM_MORE_PDU_FOLLOWS, E.E = RM_EXT_OCTET_FOLLOWS
  **   New extension octet
  **   E.M = RM_MORE_NO_PDU_FOLLOWS, E.E = RM_EXT_NO_OCTET_FOLLOWS
  **   E.LI = 0 (but in actual fact carries rpl-1 octets)
  **
  **   exit if reached the number of blocks or drop onto the next part to
  **   place the last octet in a new block.
  **---------------------------------------------------------------------------
  */
  if (rpl == dl)
  {
    /* next CV will be 0
    */
    if ((ul[gas_id].tbc - ul[gas_id].abs_bsn) == 1)
    {
      if (bp->num_ext_octets == 0)
      {
        /* case (1)
        */
        INC_ABS_BSN(gas_id);
        /* we do not update ul[gas_id].cv for this block , as its not fully segmented
        ** we update the value when its attached to tx_arr.
        ** This is done to avoid doing a reprocess if cv=0 here and enter
        ** infinite segmentaion loop
        */
        PUT_UL_MAC_HDR(bp,calc_cv(gas_id, ul[gas_id].tbc, bp->abs_bsn),ul[gas_id].si,ul[gas_id].cur_res.retry);
        PUT_UL_RLC_HDR_OCTET_1(bp,RLC_SPARE,ul[gas_id].cur_res.ul_tfi);
        PUT_UL_RLC_HDR_OCTET_2(bp,(uint8)ul[gas_id].seg_ind,RM_EXT_NO_OCTET_FOLLOWS);
        bp->first_psn = cur_pdu_ptr[gas_id]->psn;
        bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
        bp->ext = RM_EXT_NO_OCTET_FOLLOWS;

        /* Strip head of PDU into a temp dsm item
        */
        temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr), rpl );
        if (temp_dsm_ptr == NULL)
        {
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_252, "GRLU ##sgm segment_head returns NULL");
          grlc_ul_show_dsm_info(gas_id);
          grlc_ul_show_vars(gas_id);

          /* Failed to create dup'ed packet. Set flag to release at the appro fn level
          */
          ul[gas_id].b_error_recovery_required = TRUE;
          ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
          return SEGMENT_REACHED_NUM_BLOCKS;
        }

        /* Transfer the temp dsm item into dsm_ptr
        */
        gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

        /* temp should be NULL after call to augment_tail. If not flag error and free it
        */
        if (temp_dsm_ptr != NULL)
        {
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_253, "GRLU ##sgm non NULL after augment_tail");
          GPRS_PDU_FREE(&temp_dsm_ptr);
        }

        /* adjust the offset to the remaining data octets in the current pdu
        */
        cur_pdu_ptr[gas_id]->rem_ind += rpl;

        /* update length of current data block
        */
        bp->data_len += (uint8)rpl;

        /* Adjust remaining data space in the current block.
        */
        bp->rem_cnt -= (uint8)rpl;

        /* Pass on PDU priority info */
        bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

        attach_block_to_tx_array(gas_id, bp);

        /* blank the 'current' data block now that it's been attached
        */
        NEW_DATA_BLOCK(gas_id, bp);

        return SEGMENT_DONE;
      }
      else
      {
        /* Case (2). Not a valid scenario.
        **
        ** Current block (cv=1) is partial (and not yet attached) and the
        ** remaining space fits the remaining octets perfectly. Ideally, we
        ** should fill the block and send off this block with CV=0. However, TBC
        ** was over-estimated by 1 block. So, we apply case 3 instead to expend the
        ** extra block, ie. we send dl-1 octets plus extension octet with LI=0 for
        ** the current block (CV=1). The next time round the remaining 1 octet will
        ** be sent as a last block (cv=0)
        */
        MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_254, "GRLU SGM Case 2");

        bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_PDU_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_OCTET_FOLLOWS;

        bp->num_ext_octets++;

        /* fill new ext octet
        */
        bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_NO_PDU_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.li = 0; /* actually carries rpl-1 octets */

        /* adjust the remaining data space since adding an extension octet will
        ** reduce the data (payload) area
        */
        bp->rem_cnt--;

        /* Strip head of PDU into a temp dsm item
        */
        temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr),
                                                (uint16)(rpl-1)
                                             );
        if (temp_dsm_ptr == NULL)
        {
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_255, "GRLU ##sgm segment_head returns NULL");
          grlc_ul_show_dsm_info(gas_id);
          grlc_ul_show_vars(gas_id);

          /* Failed to create dup'ed packet. Set flag to release at the appro fn level
          */
          ul[gas_id].b_error_recovery_required = TRUE;
          ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
          return SEGMENT_REACHED_NUM_BLOCKS;
        }

        /* Transfer the temp dsm item into dsm_ptr
        */
        gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

        /* temp should be NULL after call to augment_tail. If not flag error and free it
        */
        if (temp_dsm_ptr != NULL)
        {
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_256, "GRLU ##sgm non NULL after augment_tail");
          GPRS_PDU_FREE(&temp_dsm_ptr);
        }

        /* adjust the offset to the remaining data octets in the current pdu
        */
        cur_pdu_ptr[gas_id]->rem_ind += (rpl-1);

        /* update length of current data block
        */
        bp->data_len += (uint8)(rpl-1);

        /* adjust remaining data space in the current block
        */
        bp->rem_cnt -= (uint8)(rpl-1);

        /* one more data block
        */
        num_segs++;

        /* update rpl
        */
        rpl = 1;

        /* Pass on PDU priority info */
        bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

        attach_block_to_tx_array(gas_id, bp);

        /* blank the 'current' data block now that it's been attached
        */
        NEW_DATA_BLOCK(gas_id, bp);

        if (num_segs >= blks_to_segment)
          return SEGMENT_REACHED_NUM_BLOCKS;
      }
    } /* CV = 0 and rpl = dl */

    else /* CV != 0 */
    {
      if (bp->num_ext_octets == 0)
      {
        /* case (3)
        ** rpl = dl, CV != 0, new data block
        ** TBF continues, but
        */
        INC_ABS_BSN(gas_id);
        /* we do not update ul[gas_id].cv for this block , as its not fully segmented
        ** we update the value when its attached to tx_arr.
        ** This is done to avoid doing a reprocess if cv=0 here and enter
        ** infinite segmentaion loop
        */
        PUT_UL_MAC_HDR(bp,calc_cv(gas_id, ul[gas_id].tbc, bp->abs_bsn),ul[gas_id].si,ul[gas_id].cur_res.retry);
        PUT_UL_RLC_HDR_OCTET_1(bp,RLC_SPARE,ul[gas_id].cur_res.ul_tfi);
        PUT_UL_RLC_HDR_OCTET_2(bp,(uint8)ul[gas_id].seg_ind,RM_EXT_NO_OCTET_FOLLOWS);
        bp->first_psn = cur_pdu_ptr[gas_id]->psn;
        bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
        bp->ext = RM_EXT_OCTET_FOLLOWS;

        bp->ext_octet[0].gprs.more = RM_MORE_NO_PDU_FOLLOWS;
        bp->ext_octet[0].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
        bp->ext_octet[0].gprs.li = 0; /* actually carries rpl-1 */

        /* one extension octet
        */
        bp->num_ext_octets = 1;
      }
      else
      {
        /* case (4)
        ** Change existing ext octet
        */
        bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_PDU_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_OCTET_FOLLOWS;

        bp->num_ext_octets++;

        /* fill new ext octet
        */
        bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_NO_PDU_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
        bp->ext_octet[bp->num_ext_octets - 1].gprs.li = 0; /* actually carries rpl -1 */
      }

      /* common part of (3) and (4)
      */

      /* adjust the remaining data space since adding an extension octet will
      ** reduce the data (payload) area
      */
      bp->rem_cnt--;

      /* Strip head of PDU into a temp dsm item
      */
      temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr),
                                              (uint16)(rpl-1)
                                           );
      if (temp_dsm_ptr == NULL)
      {
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_257, "GRLU ##sgm segment_head returns NULL");
        grlc_ul_show_dsm_info(gas_id);
        grlc_ul_show_vars(gas_id);

        /* Failed to create dup'ed packet. Set flag to release at the appro fn level
        */
        ul[gas_id].b_error_recovery_required = TRUE;
        ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
        return SEGMENT_REACHED_NUM_BLOCKS;
      }

      /* Transfer the temp dsm item into dsm_ptr
      */
      gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

      /* temp should be NULL after call to augment_tail. If not flag error and free it
      */
      if (temp_dsm_ptr != NULL)
      {
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_258, "GRLU ##sgm non NULL after augment_tail");
        GPRS_PDU_FREE(&temp_dsm_ptr);
      }

      /* adjust the offset to the remaining data octets in the current pdu
      */
      cur_pdu_ptr[gas_id]->rem_ind += (rpl-1);

      /* update length of current data block
      */
      bp->data_len += (uint8)(rpl-1);

      /* adjust remaining data space in the current block
      */
      bp->rem_cnt -= (uint8)(rpl-1);

      /* one more data block
      */
      num_segs++;

      /* update rpl
      */
      rpl = 1;

      /* Pass on PDU priority info */
      bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

      attach_block_to_tx_array(gas_id, bp);

      /* blank the 'current' data block now that it's been attached
      */
      NEW_DATA_BLOCK(gas_id, bp);

      if (num_segs >= blks_to_segment)
        return SEGMENT_REACHED_NUM_BLOCKS;

      /* if number of segmented blocks has not reached the limit then drop
      ** onto the next section to place the final octet in the next block
      */

    } /* CV != 0 */
  } /* rpl = dl */

  if (rpl > 0)
  {
    /*--------------------------------------------------------------------------
    ** rpl < dl[gas_id].
    **
    ** At this point the data remained in the current pdu is less than the data
    ** remained in the current data block. The data block may be empty or
    ** partially filled (num_ext_octets > 0). In either case we need to add an
    ** extension octet for this last segment.
    **
    ** Two cases:
    **
    ** 1. new block (no ext octet(s)):
    **    Partially fill data block (simple case like B.2b)
    **    E = RM_EXT_OCTET_FOLLOWS
    **    E.M = RM_MORE_NO_PDU_FOLLOWS, E.E = RM_EXT_NO_OCTET_FOLLOWS
    **    E.LI = number of octets. The remaining octets are filler octets
    **
    ** 2. ext octet exists:
    **    Partially fill remaining area (B.1)
    **    Change current extension octet as follows:
    **    E.M = RM_MORE_PDU_FOLLOWS, E.E = RM_EXT_OCTET_FOLLOWS then
    **
    **    The new ext octet:
    **
    **    E.LI = number of octets,
    **    E.M = RM_MORE_NO_PDU_FOLLOWS, E.E = RM_EXT_NO_OCTET_FOLLOWS
    **--------------------------------------------------------------------------
    */
    if (bp->num_ext_octets == 0)
    {
      /* case (1). Partially fill new data block
      */

      /* add MAC header and RLC fixed header
      */
      INC_ABS_BSN(gas_id);
      /* we do not update ul[gas_id].cv for this block , as its not fully segmented
      ** we update the value when its attached to tx_arr.
      ** This is done to avoid doing a reprocess if cv=0 here and enter
      ** infinite segmentaion loop
      */
      PUT_UL_MAC_HDR(bp,calc_cv(gas_id, ul[gas_id].tbc, bp->abs_bsn),ul[gas_id].si,ul[gas_id].cur_res.retry);
      PUT_UL_RLC_HDR_OCTET_1(bp,RLC_SPARE,ul[gas_id].cur_res.ul_tfi);
      PUT_UL_RLC_HDR_OCTET_2(bp,(uint8)ul[gas_id].seg_ind,RM_EXT_OCTET_FOLLOWS);
      bp->first_psn = cur_pdu_ptr[gas_id]->psn;
      bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;

      /* add the first ext octet
      */
      bp->ext = RM_EXT_OCTET_FOLLOWS;

      /* assume no more pdu follows and no extension octet follows
      */
      bp->ext_octet[0].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
      bp->ext_octet[0].gprs.more = RM_MORE_NO_PDU_FOLLOWS;
      bp->ext_octet[0].gprs.li = (uint8)rpl;

      /* one extension octet
      */
      bp->num_ext_octets = 1;
    }
    else
    {
      /* case (2).
      **
      ** Partially fill data block where an extension octet(s) already exist(s).
      **
      ** Change current E.M, E.E as follows:
      **  E.M = 1 RM_MORE_PDU_FOLLOWS, E.E = RM_EXT_OCTET_FOLLOWS
      **
      ** The new ext octet:
      **
      **  E.LI = number of octets,
      **  E.M = RM_MORE_NO_PDU_FOLLOWS, E.E = RM_EXT_NO_OCTET_FOLLOWS
      */

      /* change current extension octet
      ** (num_ext_octets - 1) indexes the current extension octet
      */
      bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_OCTET_FOLLOWS;
      bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_PDU_FOLLOWS;

      /* 1 more extension octet
      */
      bp->num_ext_octets++;

      /* add new extension octet.
      ** E.LI = number of octets
      ** E.M = RM_MORE_NO_PDU_FOLLOWS
      ** E.E = RM_EXT_NO_OCTET_FOLLOWS
      */
      bp->ext_octet[bp->num_ext_octets - 1].gprs.li = (uint8)rpl;
      bp->ext_octet[bp->num_ext_octets - 1].gprs.more = RM_MORE_NO_PDU_FOLLOWS;
      bp->ext_octet[bp->num_ext_octets - 1].gprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    }

    /* adjust the remaining data space since adding an extension octet will
    ** reduce the data (payload) area
    */
    bp->rem_cnt--;

    /* now transfer the data (payload) into the data block
    */

    /* Strip head of PDU into a temp dsm item
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr), rpl );
    if (temp_dsm_ptr == NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_259, "GRLU ##sgm segment_head returns NULL");
      grlc_ul_show_dsm_info(gas_id);
      grlc_ul_show_vars(gas_id);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul[gas_id].b_error_recovery_required = TRUE;
      ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
      return SEGMENT_REACHED_NUM_BLOCKS;
    }

    /* Transfer the temp dsm item into dsm_ptr
    */
    gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

    /* temp should be NULL after call to augment_tail. If not flag error and free it
    */
    if (temp_dsm_ptr != NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_260, "GRLU ##sgm non NULL after augment_tail");
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }

    /* adjust the offset to the remaining data octets in the current pdu
    */
    cur_pdu_ptr[gas_id]->rem_ind += rpl;

    /* update length of current data block
    */
    bp->data_len += (uint8)rpl;

    /* adjust remaining data space in the current block
    */
    bp->rem_cnt -= (uint8)rpl;

    /* Pass on PDU priority info */
    bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;


      /* We need to handle the scenario where the current PDU fills the
      ** block partially and the remaining octets in the block are filled
      ** with octets from the next PDU which has a different PFI from the
      ** current PDU. In this case we use the PFI of the latest PDU in the
      ** block. We might need to check the radio priority and use the PFI
      ** of the PDU with the highest priority to inform the network that
      ** a high priority PDU (or just a segment of it) is included in the
      ** block (TBC).
      */

      /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
      ** PFI send on the last PRR. If so, replace current block PFI with
      ** the new PFI.
      */
      if( IS_NEXT_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
      {
        bp->pfi = ul[gas_id].pfi;
      }


    /* data block full
    */
    if (bp->rem_cnt == 0)
    {
      attach_block_to_tx_array(gas_id, bp);

      /* blank the 'current' data block now that it's been attached
      */
      NEW_DATA_BLOCK(gas_id, bp);


      /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
      ** PFI send on the last PRR. If so, replace current block PFI with
      ** the new PFI. Note we need to do this for the previous block and the
      ** newly initialised block too.
      */
      if( IS_NEXT_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
      {
        bp->pfi = ul[gas_id].pfi;
      }

      /* expect the remaining index to indicate that all octets in the pdus
      ** are taken out.
      */

      /* current pdu done.
      ** Next is to wait for all blocks in this pdu to be transmitted
      ** then it should be freed. Then do Reallocation and to update
      ** the current pdu and next pdu pointers
      */
      return SEGMENT_DONE;

    } /* last data block full */

    else /* rem_cnt > 0. Partial block */
    {
      /* if reallocation at tx-complete mode was on then do NOT hang
      ** on to the partial block but send it and clear the flag
      */
      if (ul[gas_id].b_to_send_last_block) /* && ul[gas_id].cv == 0 */
      {
        MSG_GRLC_LOW_2_G(GRLC_F3_MSG_ENUM_261, "SGM s_last=TRUE atta bsn=%d CV=%d",
                bp->bsn,
                cur_blk[gas_id].mac_hdr.type.ul_data.cv
                );
        attach_block_to_tx_array(gas_id, bp);

        /* blank the 'current' data block now that it's been attached
        */
        NEW_DATA_BLOCK(gas_id, bp);


        /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
        ** PFI send on the last PRR. If so, replace current block PFI with
        ** the new PFI. Note we need to do this for the previous block and the
        ** newly initialised block too.
        */
        if( IS_NEXT_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
        {
          bp->pfi = ul[gas_id].pfi;
        }

        return SEGMENT_DONE;

      } /* partial data block with no next pdu */

      else
      {
        /* PDU runs out of octets, last data block is partial. Attach it only
        ** if the current PDU is the last. Otherwise hang on
        */
        if (cur_pdu_ptr[gas_id]->psn == cur_pdu_ptr[gas_id]->pg_last_psn)
        {
          attach_block_to_tx_array(gas_id, bp);
          NEW_DATA_BLOCK(gas_id, bp);
        }
        else
        {
          MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_262, "GRLU ##sgm cv=%d psn(%u) != last_p(%u)",
                  ul[gas_id].cv,
                  cur_pdu_ptr[gas_id]->psn,
                  cur_pdu_ptr[gas_id]->pg_last_psn);
        }

        /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
        ** PFI send on the last PRR. If so, replace current block PFI with
        ** the new PFI. Note we need to do this for the previous block and the
        ** newly initialised block too.
        */
        if( IS_NEXT_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
        {
          bp->pfi = ul[gas_id].pfi;
        }

        return SEGMENT_DONE;
      } /* partial data block with new pdu in the pipeline */
    } /* rpl < dl */
  } /* rpl > 0 */

  return SEGMENT_REACHED_NUM_BLOCKS;

} /* segment_pdu */

/*===========================================================================
===
===  FUNCTION      E_SEGMENT_PDU
===
===  DESCRIPTION
===
===    Segment the current PDU to produce a number of uplink data blocks
===    for transmission. The TLLI field will be included if IS_TLLI_REQUIRED()
===    is tested TRUE.
===
===    Padding of a partially-filled data block (where rem_cnt > 0) is done
===    elsewhere for efficientcy reasons.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    inputs: cur_pdu -- PDU being processed, pointed to by 'bp'
===            IS_TLLI_REQUIRED() determines if TLLI is required.
===
===    output: cur_blk
===
===  RETURN VALUE
===
===    segment_status_t
===
===  SIDE EFFECTS
===
===========================================================================*/
segment_status_t  e_segment_pdu( gas_id_t gas_id )
{
  /* Use this block pointer (bp) is easier than using cur_blk
  */
  grlc_ul_data_block_t      *bp;

  /* Remained pdu length. Number of data octets remained to be segmented
  ** in the current pdu
  */
  uint16 rpl;

  /* data length: of data remained in the current data block
  */
  uint16 dl;

  dsm_item_type *temp_dsm_ptr = NULL;
  /* number of data blocks produced within this segmentation. This is
  ** required to count the number of blocks in one phase access mode.
  */
  uint16 num_segs = 0;
  uint16 blks_to_segment;

  /*---------------------------------------------------------------------*/

  bp = &cur_blk[gas_id];

  if (ul[gas_id].rlc_mode == GRLC_MODE_ACK)
  {
    blks_to_segment =
    (uint16)MIN3( (MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].vsf) >
                    RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE ? 0 :
                    RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE
                  ), E_GET_ATTACH_WINDOW_SPACE(gas_id),
                  (uint16)( RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE - grlc_ul_fifo[gas_id].ptx_ctr
                          ) * /* account for dual payload */
                          (ul[gas_id].cur_res.cs > RM_MCS_6 ? 2 : 1)
                );
  }
  else
  {
    blks_to_segment =
    (uint16)MIN3( (MOD_DIF_ESNS(ul[gas_id].seg_ind, ul[gas_id].pack_vsf) >
                    RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE ? 0 :
                    RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE
                  ), E_GET_UNACK_WINDOW_SPACE(gas_id),
                  (uint16)( RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE - grlc_ul_fifo[gas_id].pack_ctr
                          ) * /* account for dual payload */
                          (ul[gas_id].cur_res.cs > RM_MCS_6 ? 2 : 1)
                );
  }
  /*-----------------------------------------------------------------------
  ** Function structure
  **
  **   1. Handle pdu len > block len.
  **
  **   while (pdu len > block len)
  **     process block
  **     if done
  **       exit
  **     else if reached limit
  **       return reached limit
  **     endif
  **   end while
  **
  **   2. Handle pdu len = block len. May incur 2 blocks
  **
  **   if (pdu len == block len)
  **     process block
  **   endif
  **
  **   3. Handle pdu len < block len
  **
  **   if (pdu len < block len)
  **     process block
  **     if block is partial
  **        check and send off block or hang on to block
  **     endif
  **   endif
  **
  **-----------------------------------------------------------------------
  */
  if (blks_to_segment == 0)
  {
#ifdef  GSM_GPRS_GRLU_SHOW_SEGMENT
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_263, "ERLU esgm 0 p=%u pl=%u last_p=%u",
            cur_pdu_ptr[gas_id]->psn,
            cur_pdu_ptr[gas_id]->len,
            cur_pdu_ptr[gas_id]->pg_last_psn);
#endif
    return ul[gas_id].segment_state;
  }

#ifdef  GSM_GPRS_GRLU_SHOW_SEGMENT
  if (cur_pdu_ptr[gas_id] != NULL)
  {
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_264, "ERLU ESGM p=%u l=%u rem=%u",
            cur_pdu_ptr[gas_id]->psn,
            cur_pdu_ptr[gas_id]->len,
            cur_pdu_ptr[gas_id]->rem_ind);
  }
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_265, "ERLU ##eSGM cur=NULL");
    return ul[gas_id].segment_state;
  }
#else
  if (cur_pdu_ptr[gas_id] == NULL)
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_266, "ERLU ##eSGM cur=NULL");
    return ul[gas_id].segment_state;
  }
#endif

  /* work out the remaining data octets in the current pdu.
  ** It's the length of the complete pdu less the octets taken out so far.
  */
  rpl = cur_pdu_ptr[gas_id]->len - cur_pdu_ptr[gas_id]->rem_ind;

  /* to add case where rem_ind = len, ie. nothing more to segment
  */
  if (rpl == 0)
  {
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_267, "ERLU Info eSGM rpl=0 #=%d pl=%d r=%d",
              blks_to_segment,
              cur_pdu_ptr[gas_id]->len,
              cur_pdu_ptr[gas_id]->rem_ind);
    return SEGMENT_DONE;
  }

  /* remaining capacity to store data in the current data block
  */
  dl = bp->rem_cnt;

  /*-------------------------------------------------------------------------
  ** Loop to segment until the remaining octets in the current pdu is less
  ** than or equal to the remaining octets in the current data block. During
  ** the loop, if a data block is full then it is attached to the tx buffer
  ** and cur_blk is re-initialised for re-use.
  **-------------------------------------------------------------------------
  */
  while (rpl > dl)
  {
    /* The data block can be new or partially filled as indicated by
    ** num_ext_octets
    **
    ** 1. new data block: data from pdu occupy the whole block,
    ** 2. existing data block
    */
    if( (bp->num_ext_octets == 0)  ||
        ((bp->num_ext_octets == 1) && (bp->ext_octet[0].egprs.li == 0)))
    {
      /* (1) new data block.
      */
      INC_ABS_BSN(gas_id);

      /* Fill the header fields. Note that:
      ** - BSN2 is an offset of BSN1 and will be filled during Tx.
      ** - SPB is only used during re-tx and should be set only during re-tx.
      ** - PI has been set above when the decision was made if PFI shoud be included
      **   in the payload.
      ** - CPS to be set before txing the block, to account for dual payload.
      */
      /* we do not update ul[gas_id].cv for this block , as its not fully segmented
      ** we update the value when its attached to tx_arr.
      ** This is done to avoid doing a reprocess if cv=0 here and enter
      ** infinite segmentaion loop
      */
      bp->hdr.cv    = e_calc_cv( gas_id, ul[gas_id].tbc, bp );
      bp->hdr.retry = ul[gas_id].cur_res.retry;
      bp->hdr.si    = ul[gas_id].si;
      bp->hdr.tfi   = ul[gas_id].cur_res.ul_tfi;
      bp->hdr.bsn1  = ul[gas_id].seg_ind;

      /* Block is about to be txed for the first time */
      bp->hdr.rsb   = RM_RSB_1ST_TX;

      if( bp->num_ext_octets == 0 )
        bp->ext     = RM_EXT_NO_OCTET_FOLLOWS;
      else
        bp->ext     = RM_EXT_OCTET_FOLLOWS;

      /* Mark the first PSN of the block and the offset point
      ** to be used for re-segmentation.
      */
      bp->first_psn = cur_pdu_ptr[gas_id]->psn;
      bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
    }
    else
    {
      /* (2) existing data block. Only exists with at least 1 ext octet.
      ** Hence the boundary has been defined and a new ext octet is
      ** not required, it simply occupies the remaining space of this data block.
      **
      ** Use existing MAC and RLC hdrs. Set the ext field of the last
      ** ext octet to no ext octet follows.
      **
      ** num_ext_octets indexes the next octet, hence to address the
      ** previous ext octet '-1' is used.
      */
      bp->ext_octet[bp->num_ext_octets-1].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    }

    /*-----------------------------------------------------------------
    ** To convert to DSM operation the data stripped off a PDU (in DSM
    ** format) is stripped into another DSM item, pointed to by
    ** by bp->dsm_ptr. This replaces the bp->data[] array.
    **---------------------------------------------------------------------
    */

    /* Strip head of PDU into a temp dsm item  the tertiary ensures
    ** the number of octets is limited to what is remained in the pdu.
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr),
                                           (uint16)(dl > rpl ? rpl : dl)
                                         );
    if (temp_dsm_ptr == NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_268, "ERLU ##esgm segment_head returns NULL");
      grlc_ul_show_dsm_info(gas_id);
      grlc_ul_show_vars(gas_id);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul[gas_id].b_error_recovery_required = TRUE;
      ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
      return SEGMENT_REACHED_NUM_BLOCKS;
    }

    /* Transfer the temp dsm item into the 'dsm_ptr' of the current data block.
    ** ie. the new block in appended to the existing block
    */
    gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

    /* temp should be NULL after call to augment_tail. If not flag error and free it
    */
    if (temp_dsm_ptr != NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_269, "ERLU ##esgm non NULL after augment_tail");
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }

    /* pdu length is now shorter
    */
    rpl -= dl;

    /* move remaining data index in the current pdu along
    */
    cur_pdu_ptr[gas_id]->rem_ind += dl;

    /* update length of current data block
    */
    bp->data_len += (uint8)dl;

    /* adjust count of remaining space in data block. Expect
    */
    bp->rem_cnt -= (uint8)dl;

    /* Pass on PDU priority info */
    bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

    /* Commit data block to the tx array and initialise new block.
    ** Use the original MCS and not the bp->cs to avoid using MSC-1
    ** when ul[gas_id].cur_res.tlli_block_cs != RM_TLLI_USE_MCS_1.
    */
    e_attach_block_post_init(gas_id, bp);

    /* load new value of rem_cnt which is the maximum data octets according
    ** to the coding scheme less the fixed header and perhaps the length of
    ** TLLI block, if one exists. There should be no extension octet for a
    ** new data block
    */
    dl = bp->rem_cnt;

    /* one more data block
    */
    num_segs++;

    /* exit if number of blocks segmented has reached the
    ** maximum specified
    */
    if (rpl == 0)
      return SEGMENT_DONE;
    else if (num_segs >= blks_to_segment)
      return SEGMENT_REACHED_NUM_BLOCKS;
  };   /* while loop */

  /*---------------------------------------------------------------------------
  ** Checks the case where rpl = dl[gas_id].
  **
  ** This last fragment may segment into 2 blocks due to the extension octet.
  **
  ** Three cases:
  **
  ** 1. CV = 0 && new data block (no extension octet)
  **   Last segment of pdu and last block of TBF
  **   E = RM_EXT_NO_OCTET_FOLLOWS
  **   All remaining octets into data block. No ext octet
  **
  ** 2. CV = 0 && old data block (ext octet > 0)
  **   Not a valid scenario because the segment associated with the extension
  **   octet would have been the final block of the TBF (for CV = 0) and the
  **   remaining octets would have been padded and sent off.
  **
  ** 3. CV != 0 && new data block
  **   Last segment of pdu but not the end of TBF hence we must add an
  **   extension octet to the next data block to indicate LI = 0
  **
  ** 4. CV != 0 && old data block
  **   Last and only segment of pdu but not the end of a TBF hence add an
  **   extension octet to the next data block to indicate LI = 0
  **
  **   exit if reached the number of blocks or drop onto the next part to
  **   place the last octet in a new block.
  **---------------------------------------------------------------------------
  */
  if (rpl == dl)
  {
    if( (bp->num_ext_octets == 0)  ||
        ((bp->num_ext_octets == 1) && (bp->ext_octet[0].egprs.li == 0)))
    {
      /* case (1)
      */
      INC_ABS_BSN(gas_id);

      /* Fill the header fields. Note that:
      ** - BSN2 is an offset of BSN1 and will be filled during Tx.
      ** - SPB is only used during re-tx and should be set only during re-tx.
      ** - PI has been set above when the decision was made if PFI shoud be
      **   included in the payload.
      ** - CPS to be set before txing the block, to account for dual payload.
      */
      /* we do not update ul[gas_id].cv for this block , as its not fully segmented
      ** we update the value when its attached to tx_arr.
      ** This is done to avoid doing a reprocess if cv=0 here and enter
      ** infinite segmentaion loop
      */
      bp->hdr.cv    = e_calc_cv( gas_id, ul[gas_id].tbc, bp );
      bp->hdr.retry = ul[gas_id].cur_res.retry;
      bp->hdr.si    = ul[gas_id].si;
      bp->hdr.tfi   = ul[gas_id].cur_res.ul_tfi;
      bp->hdr.bsn1  = ul[gas_id].seg_ind;
      /* Block is about to be txed for the first time */
      bp->hdr.rsb   = RM_RSB_1ST_TX;

      if( bp->num_ext_octets == 0 )
        bp->ext     = RM_EXT_NO_OCTET_FOLLOWS;
      else
        bp->ext     = RM_EXT_OCTET_FOLLOWS;

      bp->first_psn = cur_pdu_ptr[gas_id]->psn;
      bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
    }

    /* common part for (1) and (2)
    */

    /* Strip head of PDU into a temp dsm item
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr), rpl );
    if (temp_dsm_ptr == NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_270, "ERLU ##esgm segment_head returns NULL");
      grlc_ul_show_dsm_info(gas_id);
      grlc_ul_show_vars(gas_id);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul[gas_id].b_error_recovery_required = TRUE;
      ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
      return SEGMENT_REACHED_NUM_BLOCKS;
    }

    /* Transfer the temp dsm item into dsm_ptr
    */
    gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

    /* temp should be NULL after call to augment_tail. If not flag error and free it
    */
    if (temp_dsm_ptr != NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_271, "ERLU ##esgm non NULL after augment_tail");
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }

    /* adjust the offset to the remaining data octets in the current pdu
    */
    cur_pdu_ptr[gas_id]->rem_ind += rpl;

    /* update length of current data block
    */
    bp->data_len += (uint8)rpl;

    /* Adjust remaining data space in the current block.
    */
    bp->rem_cnt -= (uint8)rpl;

    /* pdu length is now shorter
    */
    rpl -= dl;

    /* Pass on PDU priority info */
    bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

    MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_272, "ERLU eSGM precise-fit bsn=%u abs_bsn=%u",bp->hdr.bsn1, ul[gas_id].abs_bsn);

    /* Attach current block to tx array and initialise next block */
    e_attach_block_post_init(gas_id, bp);

    /* Indicate that LLC PDU fitted precisely in to the previous UL block */
    bp->ext              = RM_EXT_OCTET_FOLLOWS;;
    bp->ext_octet[0].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    bp->ext_octet[0].egprs.li  = 0;
    bp->num_ext_octets   = 1;

    /* LLC pdu fitted precisely in current RLC block.
    ** Next block has to set LI = 0 in the first ext octet
    ** to indicate the exception to the peer. In this case, decrement
    ** block's length by 1 to account for the extension octet.
    */
    bp->rem_cnt--;

    if (num_segs >= blks_to_segment)
      return SEGMENT_REACHED_NUM_BLOCKS;
    else
      return SEGMENT_DONE;
      /* since rlp = dl, there is no more data to segment in this LLC pdu */

  } /* rpl = dl */

  if (rpl > 0)
  {
    /*--------------------------------------------------------------------------
    ** rpl < dl[gas_id].
    **
    ** At this point the data remained in the current pdu is less than the data
    ** remained in the current data block. The data block may be empty or
    ** partially filled (num_ext_octets > 0). In either case we need to add an
    ** extension octet for this last segment.
    **
    ** Two cases:
    **
    ** 1. new block (no ext octet(s)):
    **    Partially fill data block
    **    E = RM_EXT_OCTET_FOLLOWS
    **    E.E = RM_EXT_NO_OCTET_FOLLOWS
    **    E.LI = number of octets.
    **
    ** 2. ext octet exists:
    **    Partially fill remaining area
    **    Change current extension octet as follows:
    **    E.E = RM_EXT_OCTET_FOLLOWS then
    **
    **    The new ext octet:
    **
    **    E.LI = number of octets,
    **    E.E = RM_EXT_NO_OCTET_FOLLOWS
    **
    **    For both cases if we have to attach a partially filled data block
    **    Add extension octet with E.LI = 127
    **    to indicate that the remaining octets are filler octets.
    **--------------------------------------------------------------------------
    */
    if( (bp->num_ext_octets == 0)  ||
        ((bp->num_ext_octets == 1) && (bp->ext_octet[0].egprs.li == 0)))
    {
      /* case (1)
      */
      INC_ABS_BSN(gas_id);

      /* Fill the header fields. Note that:
      ** - BSN2 is an offset of BSN1 and will be filled during Tx.
      ** - SPB is only used during re-tx and should be set only during re-tx.
      ** - PI has been set above when the decision was made if PFI shoud be
      **   included in the payload.
      ** - CPS to be set before txing the block, to account for dual payload.
      */
      /* we do not update ul[gas_id].cv for this block , as its not fully segmented
      ** we update the value when its attached to tx_arr.
      ** This is done to avoid doing a reprocess if cv=0 here and enter
      ** infinite segmentaion loop
      */
      bp->hdr.cv    = e_calc_cv( gas_id, ul[gas_id].tbc, bp );
      bp->hdr.retry = ul[gas_id].cur_res.retry;
      bp->hdr.si    = ul[gas_id].si;
      bp->hdr.tfi   = ul[gas_id].cur_res.ul_tfi;
      bp->hdr.bsn1  = ul[gas_id].seg_ind;
      /* Block is about to be txed for the first time */
      bp->hdr.rsb   = RM_RSB_1ST_TX;

      bp->first_psn = cur_pdu_ptr[gas_id]->psn;
      bp->offset = cur_pdu_ptr[gas_id]->rem_ind + cur_pdu_ptr[gas_id]->pdu_ofs;
    }

    if( bp->num_ext_octets == 0 )
    {
      bp->ext                     = RM_EXT_OCTET_FOLLOWS;
      bp->ext_octet[0].egprs.ext  = RM_EXT_NO_OCTET_FOLLOWS;
      bp->ext_octet[0].egprs.li   = (uint8)rpl;
      bp->num_ext_octets          = 1;
    }
    else
    {
      /* case (2).
      **
      ** Partially fill data block where an extension octet(s) already exist(s).
      */
      bp->ext     = RM_EXT_OCTET_FOLLOWS;

      /* change current extension octet
      ** (num_ext_octets - 1) indexes the current extension octet
      */
      bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_OCTET_FOLLOWS;

      /* 1 more extension octet
      */
      bp->num_ext_octets++;

      /* add new extension octet.
      ** E.LI = number of octets
      ** E.E = RM_EXT_NO_OCTET_FOLLOWS
      */
      bp->ext_octet[bp->num_ext_octets - 1].egprs.li = (uint8)rpl;
      bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;
    }

    /* adjust the remaining data space since adding an extension octet will
    ** reduce the data (payload) area
    */
    bp->rem_cnt--;

    /* now transfer the data (payload) into the data block
    */

    /* Strip head of PDU into a temp dsm item
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr[gas_id]->dup_pdu_ptr), rpl );
    if (temp_dsm_ptr == NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_273, "ERLU ##esgm segment_head returns NULL");
      grlc_ul_show_dsm_info(gas_id);
      grlc_ul_show_vars(gas_id);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul[gas_id].b_error_recovery_required = TRUE;
      ul_diag_stats_st[gas_id].ul_gen_purpose_count_a++;
      return SEGMENT_REACHED_NUM_BLOCKS;
    }

    /* Transfer the temp dsm item into dsm_ptr
    */
    gprs_pdu_augment_tail (&(bp->dsm_ptr), &temp_dsm_ptr);

    /* temp should be NULL after call to augment_tail. If not flag error and free it
    */
    if (temp_dsm_ptr != NULL)
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_274, "ERLU ##esgm non NULL after augment_tail");
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }

    /* adjust the offset to the remaining data octets in the current pdu
    */
    cur_pdu_ptr[gas_id]->rem_ind += rpl;

    /* update length of current data block
    */
    bp->data_len += (uint8)rpl;

    /* adjust remaining data space in the current block
    */
    bp->rem_cnt -= (uint8)rpl;

    /* Pass on PDU priority info */
    bp->pdu_prio |= cur_pdu_ptr[gas_id]->pdu_prio;

    /* data block full
    */
    if (bp->rem_cnt == 0)
    {


      /* We need to handle the scenario where the current PDU fills the
      ** block partially and the remaining octets in the block are filled
      ** with octets from the next PDU which has a different PFI from the
      ** current PDU. In this case we use the PFI of the latest PDU in the
      ** block. We might need to check the radio priority and use the PFI
      ** of the PDU with the highest priority to inform the network that
      ** a high priority PDU (or just a segment of it) is included in the
      ** block (TBC).
      */

      /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
      ** PFI send on the last PRR. If so, replace current block PFI with
      ** the new PFI.
      */
      if( IS_NEXT_EGPRS_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
      {
        bp->pfi = ul[gas_id].pfi;
      }

      e_attach_block_post_init(gas_id, bp);


      /* Check if the next PDU to be segmented (next_pdu_ptr) conveys the
      ** PFI send on the last PRR. If so, replace current block PFI with
      ** the new PFI. Note we need to do this for the previous block and the
      ** newly initialised block too.
      */
      if( IS_NEXT_EGPRS_PDU_PFI_DIFF_TO_CURRENT(gas_id, bp) )
      {
        bp->pfi = ul[gas_id].pfi;
      }

      /* expect the remaining index to indicate that all octets in the pdus
      ** are taken out.
      */

      /* current pdu done.
      ** Next is to wait for all blocks in this pdu to be transmitted
      ** then it should be freed. Then do Reallocation and to update
      ** the current pdu and next pdu pointers
      */
      return SEGMENT_DONE;

    } /* last data block full */

    else /* rem_cnt > 0. Partial block */
    {
      /* if reallocation at tx-complete mode was on then do NOT hang
      ** on to the partial block but send it and clear the flag
      */
      if (ul[gas_id].b_to_send_last_block) /* && ul[gas_id].cv == 0 */
      {
        MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_275, "ESGM s_last=TRUE atta bsn=%d CV=%d", bp->bsn, cur_blk[gas_id].hdr.cv);

        /* change current extension octet
        ** (num_ext_octets - 1) indexes the current extension octet
        */
        bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_OCTET_FOLLOWS;

        /* 1 more extension octet to indicate filling octets
        */
        bp->num_ext_octets++;

        /* add new extension octet.
        ** E.LI = 127. Refer to subclause 10.4.14a.
        ** E.E = RM_EXT_NO_OCTET_FOLLOWS
        */
        bp->ext_octet[bp->num_ext_octets - 1].egprs.li  = RM_FILLER_OCTET_LEN;
        bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;

        e_attach_block_post_init(gas_id, bp);

        return SEGMENT_DONE;

      } /* partial data block with no next pdu */
      else
      {
        /* PDU runs out of octets, last data block is partial. Attach it only
        ** if the current PDU is the last. Otherwise hang on
        */
        if (cur_pdu_ptr[gas_id]->psn == cur_pdu_ptr[gas_id]->pg_last_psn)
        {
          /* change current extension octet
          ** (num_ext_octets - 1) indexes the current extension octet
          */
          bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_OCTET_FOLLOWS;

          /* 1 more extension octet to indicate filling octets
          */
          bp->num_ext_octets++;

          /* add new extension octet.
          ** E.LI = 127. Refer to subclause 10.4.14a.
          ** E.E = RM_EXT_NO_OCTET_FOLLOWS
          */
          bp->ext_octet[bp->num_ext_octets - 1].egprs.li  = RM_FILLER_OCTET_LEN;
          bp->ext_octet[bp->num_ext_octets - 1].egprs.ext = RM_EXT_NO_OCTET_FOLLOWS;

          e_attach_block_post_init(gas_id, bp);
        }
        else
        {
          MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_276, "ERLU ##esgm cv=%d psn(%u) != last_p(%u)",
                  ul[gas_id].cv,
                  cur_pdu_ptr[gas_id]->psn,
                  cur_pdu_ptr[gas_id]->pg_last_psn);
        }

        return SEGMENT_DONE;
      } /* partial data block with new pdu in the pipeline */
    } /* rpl < dl */
  } /* rpl > 0 */

  return SEGMENT_REACHED_NUM_BLOCKS;
} /* e_segment_pdu */

/*===========================================================================
===
===  FUNCTION      SEGMENT_LOOP()
===
===  DESCRIPTION
===
===    Call segment_pdu() again when the length of the 1st PDU is shorter
===    than the capacity of a block. When this happens the 'cur_blk' is partial
===    and un-attached and the next PDU needs to be loaded and segmented
===    to make up enough blocks to transmit with. However, we should only do
===    this when vs == vsf, ie. no more blocks to send to FIFO
===    Common Function for GPRS and EGPRS.
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
void segment_loop
(
  gas_id_t  gas_id,
  boolean   b_reprocessing
)
{
  while (ul[gas_id].segment_state == SEGMENT_DONE &&
         cur_pdu_ptr[gas_id] != NULL &&
         IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,cur_pdu_ptr[gas_id]->psn,32768)
        )
  {
    ul[gas_id].segment_state = SEGMENT_NULL;

    /* To transfer grouping info to the next PDU
    */
    ungroup_pdu(gas_id, cur_pdu_ptr[gas_id]);

    if (!grlc_ul_paq_does_psn_exist(gas_id, cur_pdu_ptr[gas_id]->psn))
    {
      MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_277, "GRLU ##SGML %u being cleared not in PAQ",cur_pdu_ptr[gas_id]->psn);
    }

    /* a copy of cur_pdu_ptr is already held in PAQ so just clear it
    */
    cur_pdu_ptr[gas_id] = NULL;

    /* Load the next PDU
    */
    (void)load_cur_pdu(gas_id);
    if (cur_pdu_ptr[gas_id] != NULL)
    {
      if (b_reprocessing)
      {
        group_pdus_for_reprocessing(gas_id);
      }
      else
      {
        group_pdus(gas_id);
      }

      /* We have a new PDU here is is part of the PDU group. We need to load
      ** the octets_req, blocks_rem, num_blocks etc..
      **
      ** group_pdus() was called above so we can use cur_pdu_ptr[gas_id]->pg_last_psn etc..
      */
      ul[gas_id].octets_req = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr[gas_id]->dup_pdu_ptr);

      get_num_blks_req(gas_id, ul[gas_id].octets_req,RM_TLLI_NOT_PRESENT,
                       (uint16)(IS_DYNAMIC_OPEN(gas_id) &&
                                IS_GT_MODULO_COMP( cur_pdu_ptr[gas_id]->pg_last_psn,
                                                   cur_pdu_ptr[gas_id]->psn,
                                                   32768
                                                 )
                                )
                       );

      ul[gas_id].blocks_req = ul[gas_id].num_blks;

      /* Assign ourselves the res
      */
      ul[gas_id].octets_rem = ul[gas_id].octets_req;
      ul[gas_id].blocks_rem = ul[gas_id].blocks_req;
      ul[gas_id].blocks_granted = ul[gas_id].blocks_rem;

      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_278, "GRLU SGML psn=%u last_psn=%u seg=%d",
              cur_pdu_ptr[gas_id]->psn,
              cur_pdu_ptr[gas_id]->pg_last_psn,
              ul[gas_id].seg_ind);

      if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        ul[gas_id].segment_state = e_segment_pdu(gas_id);
      }
      else
      {
        ul[gas_id].segment_state = segment_pdu(gas_id);
      }
    }
  } /* end while */
} /* segment_loop */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_UNSEGMENT()
===
===  DESCRIPTION
===
===    Free the segmented blocks from BSN to segment index, free the
===    segmented blocks and adjusted the segment index and segment state
===
===  DEPENDENCIES
===
===  PARAMETERS
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
void grlc_ul_unsegment (gas_id_t gas_id, uint16 bsn)
{
  uint16 num_resegment;

  /*----------------------------------------------------------------------
  ** Reclaim the storage for blocks in the Pending-tx area as well as the
  ** segmented area
  **----------------------------------------------------------------------
  */
  num_resegment = (uint8)MOD_DIF(ul[gas_id].seg_ind, bsn, SNS);
  if (num_resegment)
  {
    /* Re-init segment state
    */
    if (ul[gas_id].segment_state == SEGMENT_DONE)
      ul[gas_id].segment_state = SEGMENT_NULL;

    grlc_ul_free_blocks(gas_id, bsn, num_resegment);
    ul[gas_id].seg_ind = bsn;
    UPD_UL_NSEG(gas_id);
  }
} /* grlc_ul_unsegment */

/*===========================================================================
===
===  FUNCTION      E_GRLC_UL_UNSEGMENT()
===
===  DESCRIPTION
===
===    Free the segmented blocks from BSN to segment index, free the
===    segmented blocks and adjusted the segment index and segment state
===
===  DEPENDENCIES
===
===  PARAMETERS
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
void e_grlc_ul_unsegment (gas_id_t gas_id, uint16 bsn)
{
  uint16 num_resegment;

  /*----------------------------------------------------------------------
  ** Reclaim the storage for blocks in the Pending-tx area as well as the
  ** segmented area
  **----------------------------------------------------------------------
  */
  num_resegment = MOD_DIF_ESNS(ul[gas_id].seg_ind, bsn);
  if (num_resegment)
  {
    /* Re-init segment state
    */
    if (ul[gas_id].segment_state == SEGMENT_DONE)
      ul[gas_id].segment_state = SEGMENT_NULL;

    e_grlc_ul_free_blocks(gas_id, bsn, num_resegment);
    ul[gas_id].seg_ind = bsn;
    UPD_UL_NSEG(gas_id);
  }
} /* e_grlc_ul_unsegment */

/*===========================================================================
===
===  FUNCTION      RECALC_CV_RELATED_VALUES()
===
===  DESCRIPTION
===
===     Recalculate the total number of blocks etc..during reallocation.
===     These variables effect the current CV value.
===
===
===  PARAMS
===    repro_cause
===
===  DEPENDENCIES
===
===    Requires ul[gas_id].octets_rem
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter ul[gas_id].blocks_rem, ul[gas_id].abs_bsn, ul[gas_id].tbc, ul[gas_id].cv
===
===========================================================================*/
void recalc_cv_related_values
(
  gas_id_t                gas_id,
  grlc_reprocess_cause_t  repro_cause
)
{
  /*------------------------------------------------------------*/

  /* Reset CV as MS just came out of CRes or ChCS where the last block with CV=0
  ** has been discarded. If CV is active then group_pdus() will halt later on !
  */
  ul[gas_id].cv = BS_CV_CONST;

  pg_clr_group_info(cur_pdu_ptr[gas_id]);
  pg_clr_tbc_info(gas_id, &pg[gas_id].tbci);

  if (repro_cause == GRLC_REPROCESS_OTHER)
    group_pdus_for_reprocessing(gas_id);
  else
    group_pdus(gas_id);

  if ( ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    e_pg_calc_xfer_value(gas_id);
  }
  else
  {
    pg_calc_xfer_value(gas_id);
  }

  ul[gas_id].blocks_rem = ul[gas_id].tbc;
  ul[gas_id].abs_bsn = 0;

  /* Reset CV again
  */
  ul[gas_id].cv = BS_CV_CONST;

} /* recalc_cv_related_values */


/*** EOF: don't remove! ***/
