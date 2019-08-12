/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC to L1 Downlink FIFO Module
***
*** DESCRIPTION
***
***  Functions to implement the FIFO queue interface between RLC DL and L1
***
*** EXTERNALIZED FUNCTIONS
***
***
***  grlc_dl_fifo_one_time_init( gas_id_t gas_id )
***  grlc_dl_fifo_clear( gas_id_t gas_id )
***  grlc_dl_fifo_get_cnt( gas_id_t gas_id )
***  grlc_dl_fifo_get_block( gas_id_t gas_id )
***  grlc_dl_fifo_put_block( gas_id_t gas_id )
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  grlc_dl_fifo_one_time_init() uses dynamic memory allocation for the data area
***  and therfore should just be called once. Use grlc_dl_fifo_clear(gas_id) to clear
***  and re-initialise the DL FIFO.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcdlfifo.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/08/13    hv     CR531564
*** 08/15/11    hv     Widened mutex locks for multi-thread targets
*** 03/21/01    hv     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gmutex.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "msg.h"
#include "gl1rlccmn.h"
#include "err.h"
#include "grlcdl.h"
#include "rex.h"
#include "gprs_mem.h"
#include "grlci.h"
#include "grlcdli.h"
#include "grlcutil.h"
#include "grlcdlfifo.h"

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




/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*-----------------------------------
** RLC DL to L1 FIFO data structure
**-----------------------------------
*/
grlc_dl_pl1_fifo_t  grlc_dl_fifo[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*---------------------------------------------------------------
** Critical section to protect access to GRLC DL FIFO
**---------------------------------------------------------------
*/

rex_crit_sect_type grlc_dl_fifo_crit_sec[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_ONE_TIME_INIT
===
===  DESCRIPTION
===
===    Initialise the L1 to RLC Downlink FIFO structure by clearing the in
===    and out indices and item counter to zero. Also point the array of
===    pointers to dynamically allocated data block area.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    This should be called by rlc dl task once as it dynamically allocates
===    memory for the data blocks.
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
void grlc_dl_fifo_one_time_init ( gas_id_t gas_id )
{
  uint8  b;


  /* Initialise GRLC DL FIFO access control mutex */

  rex_init_crit_sect(&grlc_dl_fifo_crit_sec[gas_id]);


  GRLC_DL_FIFO_LOCK(gas_id);

  grlc_dl_fifo[gas_id].in_ind = 0;
  grlc_dl_fifo[gas_id].out_ind = 0;
  grlc_dl_fifo[gas_id].ctr = 0;

  for (b=0; b<GRLC_DL_PL1_FIFO_SIZE; b++)
  {
    grlc_dl_fifo[gas_id].pl1_pyld_ptrs[b]= NULL;
    grlc_dl_fifo[gas_id].pl1_blk_hdr[b].loop_dl_blk = 0;
  }

  for (b=0; b<GRLC_DL_PL1_FIFO_SIZE; b++)
  {
    grlc_dl_fifo[gas_id].arr_elm[b].b_block_put = FALSE;
    grlc_dl_fifo[gas_id].arr_elm[b].loop_dl_blk = 0;
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_CLEAR
===
===  DESCRIPTION
===
===    Read out all remaining items in FIFO. For each value returned, if the
===    DSM pointer is not NULL then free the DSM item.
===
===    Also re-init the DL FIFO indices and counter
===
===
===  PARAMETERS
===
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

void grlc_dl_fifo_clear ( gas_id_t gas_id )
{
  grlc_dl_fifo_op_result_t  res;
  pl1_radio_block_header_t  *hdr_ptr  = NULL;
  dsm_item_type             *pyld_ptr = NULL;

  e_pl1_radio_block_t pl1_blk;

  GRLC_DL_FIFO_LOCK(gas_id);

  if( dl[gas_id].tbf_mode == TBF_MODE_EGPRS )
  {
    do
    {
      res = e_grlc_dl_fifo_get_block (gas_id, &pl1_blk);
      if (res == DL_FIFO_GET_OK)
      {
        if( pl1_blk.payload_1_ptr )
        {
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_996, "GRLD fifo_clr in=%d out=%d ctr=%d",
                  grlc_dl_fifo[gas_id].in_ind,
                  grlc_dl_fifo[gas_id].out_ind,
                  grlc_dl_fifo[gas_id].ctr);
          GPRS_PDU_FREE(&pl1_blk.payload_1_ptr);
        }

        if( pl1_blk.mcs > RM_MCS_6 &&
            pl1_blk.payload_2_ptr
          )
        {
          MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_997, "GRLD fifo_clr in=%d out=%d ctr=%d",
                  grlc_dl_fifo[gas_id].in_ind,
                  grlc_dl_fifo[gas_id].out_ind,
                  grlc_dl_fifo[gas_id].ctr);
          GPRS_PDU_FREE(&pl1_blk.payload_2_ptr);
        }
      }
    } while (res != DL_FIFO_EMPTY);

    /* Check and show inconsistencies between indices and counter
    */
    if (grlc_dl_fifo[gas_id].ctr > 0 ||
        (grlc_dl_fifo[gas_id].ctr == 0 &&
         grlc_dl_fifo[gas_id].in_ind != grlc_dl_fifo[gas_id].out_ind
        )
       )
    {
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_998, "GRLD FIFO re-init (pre-re-init values) in=%u out=%u ctr=%u",
              grlc_dl_fifo[gas_id].in_ind,
              grlc_dl_fifo[gas_id].out_ind,
              grlc_dl_fifo[gas_id].ctr
             );
    }

    /* re-init DL FIFO by setting counter and in-index and out-index
    */
    grlc_dl_fifo[gas_id].in_ind = 0;
    grlc_dl_fifo[gas_id].out_ind = 0;
    grlc_dl_fifo[gas_id].ctr = 0;
  }
  else
  /* GPRS mode in EGPRS-enabled code
  */
  {
    do
    {
      res = grlc_dl_fifo_get_block (gas_id, &hdr_ptr, &pyld_ptr);
      if (pyld_ptr != NULL)
      {
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_999, "GRLD fifo_clr in=%d out=%d ctr=%d",
                grlc_dl_fifo[gas_id].in_ind,
                grlc_dl_fifo[gas_id].out_ind,
                grlc_dl_fifo[gas_id].ctr);
        GPRS_PDU_FREE(&pyld_ptr);
      }
    } while (res != DL_FIFO_EMPTY);

    /* Check and show inconsistencies between indices and counter
    */
    if (grlc_dl_fifo[gas_id].ctr > 0 ||
        (grlc_dl_fifo[gas_id].ctr == 0 &&
         grlc_dl_fifo[gas_id].in_ind != grlc_dl_fifo[gas_id].out_ind
        )
       )
    {
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_1000, "GRLD FIFO re-init (pre-re-init values) in=%u out=%u ctr=%u",
              grlc_dl_fifo[gas_id].in_ind,
              grlc_dl_fifo[gas_id].out_ind,
              grlc_dl_fifo[gas_id].ctr
             );
    }

    /* re-init DL FIFO by setting counter and in-index and out-index
    */
    grlc_dl_fifo[gas_id].in_ind = 0;
    grlc_dl_fifo[gas_id].out_ind = 0;
    grlc_dl_fifo[gas_id].ctr = 0;
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_GET_CNT
===
===  DESCRIPTION
===
===    return the number of items currently held in the fifo.
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte representing the number of items held in the fifo.
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_dl_fifo_get_cnt ( gas_id_t gas_id )
{
  uint8 b;

  /*-------------------------------*/

  GRLC_DL_FIFO_LOCK(gas_id);

  b = grlc_dl_fifo[gas_id].ctr;

  GRLC_DL_FIFO_UNLOCK(gas_id);

  return b;
}


/*===========================================================================
===
===  FUNCTION       GRLC_DL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===    This function inserts a PL1 Radio Block Header and a pointer to a
===    DSM item containing the PL1 Radio Block payload into the FIFO.
===
===    If sucessful, the in_index and ctr of the FIFO are adjusted.
===
===    After one or more blocks are copied to the fifo, L1 should send a
===    a PH_DATA_IND signal to RLC to indicate there are data block(s)
===    available in the fifo.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    DL_FIFO_PUT_OK, DL_FIFO_FULL
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_dl_fifo_op_result_t  grlc_dl_fifo_put_block
(
  gas_id_t                        gas_id,
  const pl1_radio_block_header_t  *pl1_blk_hdr_ptr_ptr,
  dsm_item_type                   *pl1_blk_pyld_ptr_ptr
)
{
  grlc_dl_fifo_op_result_t  res;

  GRLC_DL_FIFO_LOCK(gas_id);

  /* Only accept blocks when not in DS_NULL state
  */
  if ( DS_NULL == dl[gas_id].state )
  {
    /* Say Full here to reject the put
    */
    res = DL_FIFO_FULL;
  }
  else
  if( dl[gas_id].tbf_mode == TBF_MODE_EGPRS )
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_1001, "GRLD FIFO PL1 put GPRS block whilst in EGPRS mode ctr=%u in=%u out=%u",
                grlc_dl_fifo[gas_id].ctr,
                grlc_dl_fifo[gas_id].in_ind,
                grlc_dl_fifo[gas_id].out_ind
             );
    res = DL_FIFO_WRONG_TBF_MODE;
    /* PL1 should detect this condition and free the DSM item(s) to avoid leakage
    */
  }
  else
  {
    if (grlc_dl_fifo[gas_id].ctr >= GRLC_DL_PL1_FIFO_SIZE)
    {
      res = DL_FIFO_FULL;
    }
    else
    {

      grlc_dl_fifo[gas_id].pl1_blk_hdr[grlc_dl_fifo[gas_id].in_ind]   = *pl1_blk_hdr_ptr_ptr;
      grlc_dl_fifo[gas_id].pl1_pyld_ptrs[grlc_dl_fifo[gas_id].in_ind] = pl1_blk_pyld_ptr_ptr;

      /* Delink pointer */
      pl1_blk_pyld_ptr_ptr = NULL;

      INC_MOD(grlc_dl_fifo[gas_id].in_ind, GRLC_DL_PL1_FIFO_SIZE);
      grlc_dl_fifo[gas_id].ctr++;

      res = DL_FIFO_PUT_OK;

      TIMER_START_T3190(gas_id);
    }
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);

  MSG_GERAN_LOW_3_G("GRLD FIFO ctr=%u in=%u out=%u",
          grlc_dl_fifo[gas_id].ctr,
          grlc_dl_fifo[gas_id].in_ind,
          grlc_dl_fifo[gas_id].out_ind);
  return res;

} /* grlc_dl_fifo_put_block */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_GET_BLOCK
===
===  DESCRIPTION
===
===    Retrieve a downlink data block from the rlc dl fifo. If succesful,
===    the block is copied to the designated block pointer and the function
===    adjust the out index and counter and return DL_FIFO_GET_OK. If the
===    fifo is empty then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    grlc_dl_fifo_op_result_t type
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t  grlc_dl_fifo_get_block
(
  gas_id_t                  gas_id,
  pl1_radio_block_header_t  **pl1_blk_hdr_ptr_ptr,
  dsm_item_type             **pl1_blk_pyld_ptr_ptr
)
{
  grlc_dl_fifo_op_result_t  res;

  /*----------------------------------------------------------*/

  GRLC_DL_FIFO_LOCK(gas_id);

  if (grlc_dl_fifo[gas_id].ctr == 0)
  {
    res = DL_FIFO_EMPTY;
  }
  else
  {

    *pl1_blk_hdr_ptr_ptr  = &grlc_dl_fifo[gas_id].pl1_blk_hdr[grlc_dl_fifo[gas_id].out_ind];
    *pl1_blk_pyld_ptr_ptr = grlc_dl_fifo[gas_id].pl1_pyld_ptrs[grlc_dl_fifo[gas_id].out_ind];
    INC_MOD(grlc_dl_fifo[gas_id].out_ind, GRLC_DL_PL1_FIFO_SIZE);
    grlc_dl_fifo[gas_id].ctr--;

    res = DL_FIFO_GET_OK;
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);

  return res;
} /* grlc_dl_fifo_get_block */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FIFO_REF_BLK_HDR
===
===  DESCRIPTION
===
===    Reference the next downlink data block header in the rlc dl fifo
===    without removing the element from the FIFO.
===
===    If succesfull the designated block pointer is made to point to the
===    block and the function returns DL_FIFO_REF_OK. If the fifo is empty
===    then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    grlc_dl_fifo_op_result_t type
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t grlc_dl_fifo_ref_blk_hdr
(
  gas_id_t                  gas_id,
  pl1_radio_block_header_t  **pl1_blk_hdr_ptr_ptr
)
{
  grlc_dl_fifo_op_result_t  res;

  /*----------------------------------------------------------*/

  if (grlc_dl_fifo[gas_id].ctr == 0)
  {
    res = DL_FIFO_EMPTY;
  }
  else
  {
    /* retrieve the next pointer which points to the DSM item representing
    ** the Portable Layer 1 radio block
    */
    *pl1_blk_hdr_ptr_ptr = &grlc_dl_fifo[gas_id].pl1_blk_hdr[grlc_dl_fifo[gas_id].out_ind];

    res = DL_FIFO_REF_OK;
  }

  return res;
}

/*===========================================================================
===
===  FUNCTION       E_GRLC_DL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===   this function allows L1 to copy a downlink data block to the fifo.
===     After one or more blocks are copied to the fifo, L1 should send a
===
===
===  PARAMETERS
===
===    pl1_blk_ptr: points to PL1 structure which represents an element of
===                 the DL FIFO
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
grlc_dl_fifo_op_result_t  e_grlc_dl_fifo_put_block
(
  gas_id_t                  gas_id,
  const e_pl1_radio_block_t *pl1_blk_ptr
)
{
  grlc_dl_fifo_op_result_t  res;

  GRLC_DL_FIFO_LOCK(gas_id);

  /* Only accept blocks when not in DS_NULL state
  */
  if ( DS_NULL == dl[gas_id].state )
  {
    /* Say Full here to reject the put
    */
    res = DL_FIFO_FULL;
  }
  /* Guard against PL1 putting GPRS block into the FIFO whilst in EGPRS mode
  */
  else
  if( dl[gas_id].tbf_mode == TBF_MODE_GPRS )
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_1004, "ERLD FIFO PL1 put EGPRS block whilst in GPRS mode ctr=%u in=%u out=%u",
                grlc_dl_fifo[gas_id].ctr,
                grlc_dl_fifo[gas_id].in_ind,
                grlc_dl_fifo[gas_id].out_ind
             );
    res = DL_FIFO_WRONG_TBF_MODE;
    /* PL1 should detect this condition and free the DSM item(s) to avoid leakage
    */
  }
  else
  if (grlc_dl_fifo[gas_id].ctr >= GRLC_DL_PL1_FIFO_SIZE)
  {
    res = DL_FIFO_FULL;
  }
  else
  {
    /* copy pointer to DSM item representing Portable Layer 1 radio block
    */
    grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].in_ind] = *pl1_blk_ptr;

    /* Check block's status */
    if( !grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].in_ind].b_block_put )
    {
      grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].in_ind].b_block_put = TRUE;
    }
    else
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_1006, "ERLD FIFO put block with status UNREAD ctr=%d, in_ind=%d, out_ind=%d",
        grlc_dl_fifo[gas_id].ctr,grlc_dl_fifo[gas_id].in_ind,grlc_dl_fifo[gas_id].out_ind);
    }

#if 0
    /*-----------------------------------------------------------------------------
    ** Code to help debug PL1's DSM problem
    **-----------------------------------------------------------------------------
    */

    /* Validate block and store error info in the 2 LSBytes of dl_gen_purpose_count_b */
    if( pl1_blk_ptr->payload_1_ptr )
    {
      if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_1_ptr) ||
          (pl1_blk_ptr->payload_1_ptr->references != 1)   )
      {
        /* Clear flag before logging the info */
        dl_diag_stats_st[gas_id].dl_gen_purpose_count_b &= 0xffff0000;

        dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |=
          E_GET_RAW_DL_BSN1(&(pl1_blk_ptr->hdr_arr[0])) & 0xffff07ff;

        dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |=  (uint32)(pl1_blk_ptr->crc_1 << 12);

        if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_1_ptr) )
        {
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |= GRLC_INVAL_1ST_PLD_DSM_LEN;
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1007, "ERLD FIFO put block with 1st pld len=0");
        }

        if( pl1_blk_ptr->payload_1_ptr->references != 1)
        {
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |= GRLC_INVAL_1ST_DSM_REF_CNT;
          MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1008, "ERLD FIFO put block with 1st pld dsm ref_cnt=%d",
              pl1_blk_ptr->payload_1_ptr->references);
        }
      }
    }

    if( pl1_blk_ptr->mcs > RM_MCS_6 )
    {
      /* Validate block and store error info in the 2 MSBytes of dl_gen_purpose_count_a
      */
      if( pl1_blk_ptr->payload_2_ptr )
      {
        if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_2_ptr) ||
            (pl1_blk_ptr->payload_2_ptr->references != 1)   )
        {
          /* Clear flag before logging the info */
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_b  &= 0x0000ffff;
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_b  |= (uint32)
           ( ( MOD_ESNS(E_GET_RAW_DL_BSN1(&(pl1_blk_ptr->hdr_arr[0])) +
                           E_GET_RAW_DL_BSN2(&(pl1_blk_ptr->hdr_arr[0])) ) << 16 ));

          dl_diag_stats_st[gas_id].dl_gen_purpose_count_b  |= (uint32)(pl1_blk_ptr->crc_2 << 28);

          if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_2_ptr) )
          {
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |= GRLC_INVAL_2ND_PLD_DSM_LEN;
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1009, "ERLD FIFO put block with 2nd pld len=0");
          }

          if( pl1_blk_ptr->payload_2_ptr->references != 1)
          {
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_b |= GRLC_INVAL_2ND_DSM_REF_CNT;
            MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1010, "ERLD FIFO put block with 2nd pld dsm ref_cnt=%d",
              pl1_blk_ptr->payload_2_ptr->references);
          }
        }
      }
    }
#endif  /* debug code */

    /* Delink pointer */
    pl1_blk_ptr = NULL;

    INC_MOD(grlc_dl_fifo[gas_id].in_ind, GRLC_DL_PL1_FIFO_SIZE);
    grlc_dl_fifo[gas_id].ctr++;

    res = DL_FIFO_PUT_OK;

    TIMER_START_T3190(gas_id);
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);

  MSG_GERAN_LOW_3_G("ERLD FIFO ctr=%u in=%u out=%u",
          grlc_dl_fifo[gas_id].ctr,
          grlc_dl_fifo[gas_id].in_ind,
          grlc_dl_fifo[gas_id].out_ind);
  return res;
} /* e_grlc_dl_fifo_put_block */


/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_FIFO_GET_BLOCK
===
===  DESCRIPTION
===
===    Retrieve a downlink data block from the rlc dl fifo. If succesful,
===    the block is copied to the designated block pointer and the function
===    adjust the out index and counter and return DL_FIFO_GET_OK. If the
===    fifo is empty then DL_FIFO_EMPTY is returned.
===
===  PARAMETERS
===
===    pl1_blk_ptr: points to PL1 structure which represents an element of
===                 the DL FIFO
===
===  DEPENDENCIES
===
===    grlc_dl_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    grlc_dl_fifo_op_result_t type
===
===  SIDE EFFECTS
===
===    grlc_dl_fifo
===
===========================================================================*/
grlc_dl_fifo_op_result_t  e_grlc_dl_fifo_get_block
(
  gas_id_t            gas_id,
  e_pl1_radio_block_t *pl1_blk_ptr
)
{
  grlc_dl_fifo_op_result_t  res;

  /*----------------------------------------------------------*/

  GRLC_DL_FIFO_LOCK(gas_id);

  if (grlc_dl_fifo[gas_id].ctr == 0)
  {
    res = DL_FIFO_EMPTY;
  }
  else
  {
    /* retrieve the next pointer which points to the DSM item representing
    ** the Portable Layer 1 radio block
    */
    *pl1_blk_ptr = grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].out_ind];

    /* Check block's status */
    if( pl1_blk_ptr->b_block_put )
    {
      grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].out_ind].b_block_put = FALSE;
    }
    else
    {
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_1012, "ERLD FIFO get block with status READ ctr=%d, in_ind=%d, out_ind=%d",
        grlc_dl_fifo[gas_id].ctr,grlc_dl_fifo[gas_id].in_ind,grlc_dl_fifo[gas_id].out_ind);
    }

#if 0
    /*-----------------------------------------------------------------------------
    ** Code to help debug PL1's DSM problem
    **-----------------------------------------------------------------------------
    */
    /* Validate block and store error info in the 2 LSBytes of dl_gen_purpose_count_a */
    if( pl1_blk_ptr->payload_1_ptr )
    {
      if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_1_ptr) ||
          (pl1_blk_ptr->payload_1_ptr->references != 1)   )
      {
        /* Clear flag before logging the info */
        dl_diag_stats_st[gas_id].dl_gen_purpose_count_a &= 0xffff0000;

        dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |=
          E_GET_RAW_DL_BSN1(&(pl1_blk_ptr->hdr_arr[0])) & 0xffff07ff;

        dl_diag_stats_st[gas_id].dl_gen_purpose_count_a  |= (uint32)(pl1_blk_ptr->crc_1 << 12);

        if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_1_ptr) )
        {
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |= GRLC_INVAL_1ST_PLD_DSM_LEN;
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1013, "ERLD FIFO get block with 1st pld len=0");
        }

        if( pl1_blk_ptr->payload_1_ptr->references != 1)
        {
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |= GRLC_INVAL_1ST_DSM_REF_CNT;
          MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1014, "ERLD FIFO get block with 1st pld dsm ref_cnt=%d",
              pl1_blk_ptr->payload_1_ptr->references);
        }
      }
    }

    if( pl1_blk_ptr->mcs > RM_MCS_6 )
    {
      /* Validate block and store error info in the 2 MSBytes of dl_gen_purpose_count_a
      */
      if( pl1_blk_ptr->payload_2_ptr )
      {
        if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_2_ptr) ||
            (pl1_blk_ptr->payload_2_ptr->references != 1)   )
        {
          /* Clear flag before logging the info */
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_a  &= 0x0000ffff;
          dl_diag_stats_st[gas_id].dl_gen_purpose_count_a  |= (uint32)
           ( ( MOD_ESNS(E_GET_RAW_DL_BSN1(&(pl1_blk_ptr->hdr_arr[0])) +
                           E_GET_RAW_DL_BSN2(&(pl1_blk_ptr->hdr_arr[0])) ) << 16 ));

          dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |= (uint32)(pl1_blk_ptr->crc_2 << 28);

          if( !GPRS_PDU_LENGTH(pl1_blk_ptr->payload_2_ptr) )
          {
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |= GRLC_INVAL_2ND_PLD_DSM_LEN;
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1015, "ERLD FIFO get block with 2nd pld len=0");
          }

          if( pl1_blk_ptr->payload_2_ptr->references != 1)
          {
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_a |= GRLC_INVAL_2ND_DSM_REF_CNT;
            MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1016, "ERLD FIFO get block with 2nd pld dsm ref_cnt=%d",
              pl1_blk_ptr->payload_2_ptr->references);
          }
        }
      }
    }
#endif /* debug code */

    INC_MOD(grlc_dl_fifo[gas_id].out_ind, GRLC_DL_PL1_FIFO_SIZE);
    grlc_dl_fifo[gas_id].ctr--;

    res = DL_FIFO_GET_OK;
  }

  GRLC_DL_FIFO_UNLOCK(gas_id);

  return res;
} /* e_grlc_dl_fifo_get_block */

/*===========================================================================
===
===  FUNCTION      E_GRLC_IS_RLC_DL_REQUIRED_CONTEXT()
===
===  DESCRIPTION
===
===    This function is called by PL1 after a radio block is put into the
===    DL FIFO. It returns a boolean value which when TRUE requires PL1 to
===    call the grlc_send_ph_data_ind() to give RLC context to process and
===    flush the DL FIFO. This function ensures RLC DL only gains context
===    when it needs it in order to reduce context switches from PL1 to RLC DL.
===
===  PARAMETERS
===
===    crc_1/2: CRC status of each payload
===    fbi_e_1/2: FBI/E bits of each payload
===    esp: ES/P field
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   Return TRUE if any of the below conditions is TRUE:
===   0. first block indicator is not yet set
===   1. CRC of payload 1 is valid and either the FBI is set or Extension
===      indicates there is an octet following (0)
===   2. CRC of payload 2 is valid and either the FBI is set or Extension
===      indicates there is an octet following (0)
===   3. ES/P bits indicate polling (any values other than ESP_NO_POLL)
===   4. High water mark of DL FIFO is reached
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean e_grlc_is_rlc_dl_required_context
(
  gas_id_t  gas_id,
  uint8     crc_1,
  uint8     crc_2,
  uint8     fbi_e_1,
  uint8     fbi_e_2,
  uint8     esp
)
{
  /*---------------------------------------------------------------------
  ** Return TRUE if any of the below conditions is TRUE:
  ** 0. first block indicator is not yet set
  ** 1. CRC of payload 1 is valid and either the FBI is set or Extension
  **    indicates there is an octet following (0)
  ** 2. CRC of payload 2 is valid and either the FBI is set or Extension
  **    indicates there is an octet following (0)
  ** 3. ES/P bits indicate polling (any values other than ESP_NO_POLL)
  ** 4. High water mark of DL FIFO is reached
  **---------------------------------------------------------------------
  */
  return ((dl[gas_id].b_first_block_ind_sent == FALSE)
           ||
          (crc_1 && ( (fbi_e_1 & 0x2) ||
                      ((fbi_e_1 & 0x1) == 0)
                    )
          )
           ||
          (crc_2 && ( (fbi_e_2 & 0x2) ||
                      ((fbi_e_2 & 0x1) == 0)
                    )
          )
           ||
          (esp != ESP_NO_POLL)
           ||
          (grlc_dl_fifo[gas_id].ctr >= GRLC_DL_FIFO_HIGH_WATER)
         );
} /* e_grlc_is_rlc_dl_required_context */


/*===========================================================================
===
===  FUNCTION      GRLC_IS_RLC_DL_REQUIRED_CONTEXT()
===
===  DESCRIPTION
===
===    This function is called by PL1 after a radio block is put into the
===    DL FIFO. It returns a boolean value which when TRUE requires PL1 to
===    call the grlc_send_ph_data_ind() to give RLC context to process and
===    flush the DL FIFO. This function ensures RLC DL only gains context
===    when it needs it in order to reduce context switches from PL1 to RLC DL.
===
===    This function returns TRUE when one of the conditions below is TRUE:
===    0. first block indicator is not yet set
===    1. FBI bit of the DL radio block is set: this indicates the data block
===       is the last of the current PDU or that an acknowledgement is requested
===       by the NW.
===    2. Ext bit is clear: this indicates the presence of a PDU boundary which
===       means the data block may be the last of the current PDU.
===    3. S/P bit is set: RRBP field is valid -- which means the MS is polled
===       to return an acknowledgement bitmap.
===    4. If the number of radio blocks in the DL FIFO reaches either the
===       high watermark or full.
===
===  PARAMETERS
===
===    data_block_ptr: pointer to the RLC data block which includes the MAC
===                    header followed by RLC DL header and payload
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    boolean: TRUE when PL1 should call grlc_send_ph_data_ind()
===             FALSE when PL1 does not need to call grlc_ph_data_ind()
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_is_rlc_dl_required_context
(
  gas_id_t    gas_id,
  const uint8 *data_block_ptr
)
{
  /* Return TRUE if:
  ** 0. first block indicator is not yet set
  ** 1. FBI bit is set or
  ** 2. Ext bit is clear or
  ** 3. S/P bit i set (RRBP field is valid)
  ** 4. number of data blocks reaches the high water mark
  **
  ** Otherwise retrun FALSE
  */

  /*-------------------------------------------------------------------------
  ** Macro to extract the S/P field from the MAC header of an RLC data block
  ** (preceded with MAC header) and return TRUE if the S/P is valid (1)
  **-------------------------------------------------------------------------
  */
  #define IS_RAW_SP_ACTIVE(db_ptr)    ( ((db_ptr)[0] & 0x08) == 0x08 )

  /*-----------------------------------------------------------------------
  ** Macro to extract the FBI field from the RLC data block (with preceded
  ** MAC header) and return TRUE if the FBI bit is set
  **-----------------------------------------------------------------------
  */
  #define IS_RAW_FBI_ACTIVE(db_ptr)   ( ((db_ptr)[1] & 0x1) == 0x1 )

  /*-----------------------------------------------------------------------
  ** Macro to extract the Ext field from the RLC data block (with preceded
  ** MAC header) and return TRUE if the Ext bit is clear
  **-----------------------------------------------------------------------
  */
  #define IS_RAW_EXT_ACTIVE(db_ptr)   ( ((db_ptr)[2] & 0x1) == 0 )

  if ((dl[gas_id].b_first_block_ind_sent == FALSE) ||
      IS_RAW_FBI_ACTIVE(data_block_ptr) ||
      IS_RAW_EXT_ACTIVE(data_block_ptr) ||
      IS_RAW_SP_ACTIVE(data_block_ptr)  ||
      grlc_dl_fifo[gas_id].ctr >= GRLC_DL_FIFO_HIGH_WATER
     )
    return TRUE;

  return FALSE;
} /* grlc_is_rlc_dl_required_context */


#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      grlc_dl_free_excess_dl_blocks()
===
===  DESCRIPTION
===
===   The function read the DL FIFO and deletes the excess DL data blocks
===   as the limit DL_BLIND_DATA_BLKS is exceeded to prevent memory leak.
===
===  DEPENDENCIES
===
===    none
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void  grlc_dl_free_excess_dl_blocks( gas_id_t gas_id )
{
  while (grlc_dl_fifo_get_cnt(gas_id) > 0)
  {
    /* Free the DSM Items */
    if(dl[gas_id].psho_tbf_mode == TBF_MODE_EGPRS)
    {
      e_pl1_dl_blk[gas_id] = grlc_dl_fifo[gas_id].arr_elm[grlc_dl_fifo[gas_id].out_ind];

     /* Check block's status */
     if( e_pl1_dl_blk[gas_id].b_block_put )
     {
       e_pl1_dl_blk[gas_id].b_block_put = FALSE;
     }

     /* Free the DSM Item */
     if( e_pl1_dl_blk[gas_id].payload_1_ptr != NULL )
     {
       GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_1_ptr);
       e_pl1_dl_blk[gas_id].payload_1_ptr = NULL;
     }

     if( e_pl1_dl_blk[gas_id].payload_2_ptr != NULL )
     {
       GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_2_ptr);
       e_pl1_dl_blk[gas_id].payload_2_ptr = NULL;
     }
    }
    else /* TBF_MODE_GPRS */
    {
      pl1_blk_dsm_ptr[gas_id] = grlc_dl_fifo[gas_id].pl1_pyld_ptrs[grlc_dl_fifo[gas_id].out_ind];

      if( pl1_blk_dsm_ptr[gas_id] != NULL )
      {
        GPRS_PDU_FREE( &pl1_blk_dsm_ptr[gas_id]);
        pl1_blk_dsm_ptr[gas_id] = NULL;
      }
    }

    GRLC_DL_FIFO_LOCK(gas_id);

    INC_MOD(grlc_dl_fifo[gas_id].out_ind, GRLC_DL_PL1_FIFO_SIZE);
    grlc_dl_fifo[gas_id].ctr--;

    GRLC_DL_FIFO_UNLOCK(gas_id);
  }
} /* grlc_dl_free_excess_dl_blocks */

#endif /* FEATURE_GPRS_PS_HANDOVER */

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
/*===========================================================================
===
===  FUNCTION      SHOW_BSN_AND_FIFO_VARS()
===
===  DESCRIPTION
===
===    Show the BSN value of the downlink data block and the associated
===    internal DL FIFO variables
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_bsn_and_fifo_vars ( gas_id_t gas_id )
{
  uint32 u=0UL;

  /* Only for monitoring purposes hence do not require mutex lock for accessing FIFO */

  u =((uint32) (((uint32)grlc_dl_fifo[gas_id].ctr & 0xff) << 16)    |
      (uint32) (((uint32)grlc_dl_fifo[gas_id].in_ind & 0xff) << 8)  |
      (uint32) ((uint32)grlc_dl_fifo[gas_id].out_ind & 0xff)
     );
	
  MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_1017, "G/ERLD A/U BSN=%d FBI=%d FIFO 00CtInOu=%08x",dl[gas_id].bsn,dl[gas_id].fbi,u);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u;
} /* show_bsn_and_fifo_vars */

#endif /* GRLD_SHOW_BLOCK || GRLD_SHOW_BSN */

#ifdef  ERLD_SHOW_BLOCK
/*===========================================================================
===
===  FUNCTION      SHOW_DL_BLOCK()
===
===  DESCRIPTION
===
===    Show the values of the EDGE data block
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_dl_block
(
  gas_id_t              gas_id,
  e_pl1_radio_block_t   *radio_blk_ptr,
  grlc_dl_data_block_t  *data_blk_1_ptr,
  grlc_dl_data_block_t  *data_blk_2_ptr
)
{
  uint32 u1=0UL, u2=0UL, u3=0UL;

#if 0
  uint8 buf[100];
  uint8 *c_ptr;
  uint16 len=0,l;
  dsm_item_type *dsm_ptr;
#endif
  /*
  **   Crc1
  **   |   FBI1
  **   |   |   Ext1
  **   |   |   |   SPB
  **   |   |   |   |      Crc2
  **   |   |   |   |      |   FBI2
  **   |   |   |   |      |   |   Ext2
  **   |   |   |   |      |   |   |   esp
  **   f   f   f   f      f   f   f   f
  */
  u1 = (uint32)(radio_blk_ptr->crc_1 & 0x1) << 28 |
       (uint32)((radio_blk_ptr->fbi_e_1 >> 1) & 0x1) << 24 |
       (uint32)(radio_blk_ptr->fbi_e_1 & 0x1) << 20 |
       (uint32)(data_blk_1_ptr->spb & 0x3)<< 16 |

       (uint32)(radio_blk_ptr->crc_2 & 0x1) << 12 |
       (uint32)((radio_blk_ptr->fbi_e_2 >> 1) & 0x1) << 8 |
       (uint32)(radio_blk_ptr->fbi_e_2 & 0x1) << 4 |
       (uint32)(radio_blk_ptr->esp & 0x3);

  /*    mcs
  **   |___|      bsn1        bsn2
  **           |_______|   |_______|
  **   f   f   f   f   f   f   f   f
  */
  u2 = (uint32)radio_blk_ptr->mcs << 28 |
       (uint32)data_blk_1_ptr->bsn << 12 |
       (uint32)data_blk_2_ptr->bsn;

  u3 = ( (uint32) (((uint32)grlc_dl_fifo[gas_id].ctr & 0xff) << 16)    |
         (uint32) (((uint32)grlc_dl_fifo[gas_id].in_ind & 0xff) << 8)  |
         (uint32) ((uint32)grlc_dl_fifo[gas_id].out_ind & 0xff)
       );

  if( (dl[gas_id].state != DS_RX_UNACK_PENDING_RELEASE) &&
      (dl[gas_id].state != DS_RX_UNACK )                   )
  {
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1018, "ERLD A blk CFEsCFEe=%08x M0bs1bs2=%08x FIFO 00CtInOu=%08x",u1, u2, u3);
  }
  else
  {
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1019, "ERLD U blk CFEsCFEe=%08x M0bs1bs2=%08x FIFO 00CtInOu=%08x",u1, u2, u3);
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u1; (void)u2; (void)u3;

  /* Usefull to see RLC Block contents */
#if 0

    memset(buf,0,100);
    c_ptr=buf;
    len = 0;

    dsm_ptr=gprs_pdu_copy(data_blk_1_ptr->dsm_payload_ptr);
    len=(uint16)GPRS_PDU_LENGTH(dsm_ptr);
    (void)gprs_pdu_strip_head(&dsm_ptr, buf, len);
    /*MSG_HIGH("dsm_a_len=%u len=%u cs=%d",len,blk_ptr->len,blk_ptr->cs);*/

    for (l=0; l<((len/12)+1); l++)
    {
      u1  = 0L;
      u1 |= (uint32)(*c_ptr++) << 24;
      u1 |= (uint32)(*c_ptr++) << 16;
      u1 |= (uint32)(*c_ptr++) <<  8;
      u1 |= (uint32)(*c_ptr++);

      u2  = 0L;
      u2 |= (uint32)(*c_ptr++) << 24;
      u2 |= (uint32)(*c_ptr++) << 16;
      u2 |= (uint32)(*c_ptr++) <<  8;
      u2 |= (uint32)(*c_ptr++);

      u3  = 0L;
      u3 |= (uint32)(*c_ptr++) << 24;
      u3 |= (uint32)(*c_ptr++) << 16;
      u3 |= (uint32)(*c_ptr++) <<  8;
      u3 |= (uint32)(*c_ptr++);

      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_1022, "ERLD DATA BLK: %08x %08x %08x", u1, u2, u3);
    }
#endif

} /* show_dl_block */
#endif  /* ERLD_SHOW_BLOCK */


/*** EOF: don't remove! ***/

