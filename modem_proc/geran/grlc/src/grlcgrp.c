/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PDU-GROUPING MODULE
***
***
*** DESCRIPTION
***
***  This module implements PDU grouping functionality
***
***
*** EXTERNALIZED FUNCTIONS
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcgrp.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/25/03    hv     Created
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
#include "grlcul.h"
#include "queue.h"
#include "grlcultypes.h"
#include "grlcsgm.h"
#include "grlculpdu.h"
#include "grlcgrp.h"
#include "grlctsk.h"
#include "geran_multi_sim.h"

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

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

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

/* Data structure to manage grouping of PDUs
*/
grlc_ul_pdu_group_t   pg[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GET_BLOCKS_PAYLOAD_SIZE()
===
===  DESCRIPTION
===
===    This function returns an RLC block's payload size given the coding
===    scheme, TLLI coding scheme and TLLI indicator.
===
===  PARAMS
===
===    Coding scheme and TLLI indicator
===
===  DEPENDENCIES
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
static uint8 get_blocks_payload_size
(
  gas_id_t gas_id,
  uint8 cs,
  uint8 tlli_cs,
  uint8 ti
)
{
  uint8 block_len;

  if(ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    if (ti == RM_TLLI_PRESENT)
    {
      /* Note tlli_cs is used as tlli_mcs in EGPRS mode
      */
      block_len = (uint8)( E_GRLC_DATA_BLOCK_LEN[tlli_cs] - RM_TLLI_LEN );


      if( IS_PFC_NW_SUPPORTED(gas_id) )
      {
        /* PFI length is 7 bits and 1 bit for the E bit */
        block_len -=  RM_PFI_E_LEN;
      }
    }
    else
    {
      block_len = (uint8) E_GRLC_DATA_BLOCK_LEN[cs];
    }
  }
  else
  {
    if (ti == RM_TLLI_PRESENT)
    {
      block_len = (uint8)( (GRLC_DATA_BLOCK_LEN[tlli_cs] - RM_RLC_HDR_LEN) - RM_TLLI_LEN );


      if( IS_PFC_NW_SUPPORTED(gas_id) )
      {
        /* PFI length is 7 bits and 1 bit for the E bit */
        block_len -=  RM_PFI_E_LEN;
      }
    }
    else
    {
      block_len = (uint8)( GRLC_DATA_BLOCK_LEN[cs] - RM_RLC_HDR_LEN );
    }
  }

  return( block_len );
} /* get_blocks_payload_size */

/*===========================================================================
===
===  FUNCTION      PG_CALC_INITIAL_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of octets and calculates the number of
===   blocks required to hold these octets -- given the coding scheme and TLLI
===   status. The results are saved back into the pg.tbci struct comprising
===   the number of octets in the partial (last) block and whether an extension
===   octet exists.
===
===   This function is based on get_num_blks_ex()
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   cs,ti:  coding scheme and TLLI indicator to determine the block size
===   tbci_ptr: pointer to TBC-info structure
===
===   OUTPUT
===
===   tbci_ptr: returned altered structure
===
===  DEPENDENCIES
===
===   RLC_DATA_BLOCK_SIZE[]
===   RM_RLC_HDR_LEN
===   RM_TLLI_LEN
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
static void pg_calc_initial_tbc
(
  gas_id_t            gas_id,
  grlc_ul_tbc_info_t  *tbci_ptr,
  gprs_grr_ul_mes_t*  pdu_ptr,
  uint8               cs,
  uint8               tlli_cs,
  uint8               ti,
  uint16              b_next_pdu_avail
)
{
  uint16  blk_size;

  /*-----------------------------------------------------------------*/

  /* (payload) block size given coding scheme, and tlli indicator (no MAC header)
  */
  blk_size = get_blocks_payload_size(pdu_ptr->gas_id, cs, tlli_cs, ti);

  /* whole number of blocks (was orig_len)
  */
  tbci_ptr->num_blks = pdu_ptr->len / blk_size;

  /* Number of octets in the partial block (remained) -- avoid using the % operator
  ** (was orig_len)
  */
  tbci_ptr->partial_octets = pdu_ptr->len - (uint16)(tbci_ptr->num_blks * blk_size);

  /* Note that when the final block is partial, an extension octet exists
  ** to mark the end of this pdu. If the TBF ends with this block then the
  ** remaining octets are padded.
  **
  ** When the final block fits perfectly then an extension octet is not required,
  ** and the TBF is assumed to end on that block.
  */

  if (tbci_ptr->partial_octets == 0)
  {
    if (b_next_pdu_avail)
    {
      /* Perfect fit. If this is the last block then the CV value will terminate this
      ** TBF and there is no need for an extension octet. However, if there TBF continues
      ** an extension octet is required as a PDU boundary.
      */
      tbci_ptr->num_blks++;

      /* One data octet pushed over
      */
      tbci_ptr->partial_octets = 1;

      /* Block size less 1 extension octet less 1 data octet (pushed over)
      */
      tbci_ptr->pad_octets = blk_size - 2;

      /* Assume TBF ends with this block and no extension octet required.
      ** If another pdu is later concatenated, an extension octet required may
      ** incur an extra block
      */
      tbci_ptr->b_ext_octet = TRUE;
    }
    else
    {
      /* Next PDU is not available hence the octets fit perfectly into this block.
      ** This block will have its CV = 0
      */
      tbci_ptr->pad_octets = blk_size;
      tbci_ptr->b_ext_octet = FALSE;
    }
  }
  else
  {
    /* If partial block exists then that is an extra block
    */
    tbci_ptr->num_blks++;

    /* Value of free octets is the block size less the partial octets and
    ** an extension
    */
    tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;

    tbci_ptr->b_ext_octet = TRUE;
  }

  /* Store num_blks away as this value is the total block count if this TBF ends
  ** with this PDU
  */
  pdu_ptr->pg_num_blks_cur_pdu = tbci_ptr->num_blks;

} /* pg_calc_initial_tbc */

/*===========================================================================
===
===  FUNCTION      E_PG_CALC_INITIAL_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of octets and calculates the number of
===   blocks required to hold these octets -- given the coding scheme and TLLI
===   status. The results are saved back into the pg.tbci struct comprising
===   the number of octets in the partial (last) block and whether an extension
===   octet exists.
===
===   This function is based on e_get_num_blks_ex()
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   mcs,ti:  coding scheme and TLLI indicator to determine the block size
===   tbci_ptr: pointer to TBC-info structure
===
===   OUTPUT
===
===   tbci_ptr: returned altered structure
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
static void e_pg_calc_initial_tbc
(
  gas_id_t            gas_id,
  grlc_ul_tbc_info_t  *tbci_ptr,
  gprs_grr_ul_mes_t*  pdu_ptr,
  uint8               mcs,
  uint8               tlli_cs,
  uint8               ti,
  uint16              b_next_pdu_avail
)
{
  uint16  blk_size;
  uint16  octets;

  /*-----------------------------------------------------------------*/

  /* (payload) block size given coding scheme, and tlli indicator (no MAC header)
  */
  blk_size = get_blocks_payload_size(pdu_ptr->gas_id, mcs, tlli_cs, ti);

  octets = pdu_ptr->len;

  tbci_ptr->pad_octets = blk_size;

  /* If we enter with a boundary on the previous block. This happens during
  ** re-processing when the grouping process is re-started on a PDU boundary
  */
  if (ul[pdu_ptr->gas_id].b_li_0_exists)
  {
    /* One less to make room for the initial LI=0
    */
    tbci_ptr->pad_octets--;

    /*----------------------------------------------------------------------------------
    ** Exact fit for the first PDU. ie. We enter with a boundary on the previous block
    ** and the first PDU fits this current block exactly. So, the LI is carried forward
    ** into the next block
    **----------------------------------------------------------------------------------
    */
    if (octets == tbci_ptr->pad_octets)
    {
      MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_52, "ELRU e_pg_calc_init octets = pad_octets(%u)",octets);

      tbci_ptr->num_blks++;
      tbci_ptr->partial_octets = 0;

      if (b_next_pdu_avail)
      {
        /* Perfect fit. Prepare for the next PDU
        */
        tbci_ptr->pad_octets = blk_size  - 1;
        tbci_ptr->b_ext_octet = TRUE;
      }
      else
      {
        /* Next PDU is not available hence the octets fits perfectly into this block.
        ** This block will have its CV = 0
        */
        tbci_ptr->pad_octets  = blk_size;
        tbci_ptr->b_ext_octet = FALSE;
      }
    }
    else
    /*--------------------------------
    ** octets go into multiple blocks
    **--------------------------------
    */
    if (octets > tbci_ptr->pad_octets)
    {
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_53, "ELRU e_pg_calc_init octets(%u) > pad_octets(%u)",
              octets,
              tbci_ptr->pad_octets,
              0);

      /* Take care of the 1st block
      */
      octets -= tbci_ptr->pad_octets;
      tbci_ptr->num_blks++;

      /* For subsequent blocks the pad_octets goes back to the nornmal block size
      */
      tbci_ptr->pad_octets = blk_size;

      /* whole number of blocks
      */
      tbci_ptr->num_blks += (octets / blk_size);

      /* Number of octets in the partial block (remained) -- avoid using the % operator
      ** (was orig_len)
      */
      tbci_ptr->partial_octets = octets - (uint16)((tbci_ptr->num_blks-1) * blk_size);

      if (tbci_ptr->partial_octets == 0)
      {
        if (b_next_pdu_avail)
        {
          /* Perfect fit. Prepare for the next PDU
          */
          tbci_ptr->pad_octets = blk_size  - 1;
          tbci_ptr->b_ext_octet = TRUE;
        }
        else
        {
          /* Next PDU is not available hence the octets fits perfectly into this block.
          ** This block will have its CV = 0
          */
          tbci_ptr->pad_octets  = blk_size;
          tbci_ptr->b_ext_octet = FALSE;
        }
      }
      else
      {
        /* If partial block exists then that is an extra block
        */
        tbci_ptr->num_blks++;

        /* Value of free octets is the block size less the partial octets and
        ** an extension
        */
        tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;
      }
    }
    else
    /*---------------------
    ** octets < pad_octets
    **---------------------
    */
    {
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_54, "ELRU e_pg_calc_init octets(%u) < pad_octets(%u)",
              octets,
              tbci_ptr->pad_octets,
              0);

      /* .. And num_blks has not been accounted for. And pad_octets is one less
      ** than blk_size
      */
      tbci_ptr->num_blks++;
      tbci_ptr->partial_octets = octets;

      /* Value of free octets is the block size less the partial octets and
      ** an extension
      */
      tbci_ptr->pad_octets = (tbci_ptr->pad_octets - octets) - 1;
    }

    /* Clear flag after processing
    */
    ul[pdu_ptr->gas_id].b_li_0_exists = FALSE;
    MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_55, "ERLU e_pg_calc_init clr b_li_o_exists");

  } /* end PDU boundary on previous data block */
  else
  {
    /* whole number of blocks
    */
    tbci_ptr->num_blks = octets / blk_size;

    /* Number of octets in the partial block (remained) -- avoid using the % operator
    ** (was orig_len)
    */
    tbci_ptr->partial_octets = octets - (uint16)(tbci_ptr->num_blks * blk_size);

    /*==================================================================
    ** Prepare vars for the next grouping
    **==================================================================
    */

    /* Note that when the final block is partial, an extension octet exists
    ** to mark the end of this pdu. If the TBF ends with this block then the
    ** remaining octets are padded.
    **
    ** When the final block fits perfectly then an extension octet is not required,
    ** and the TBF is assumed to end on that block.
    */
    if (tbci_ptr->partial_octets == 0)
    {
      if (b_next_pdu_avail)
      {
        /* Perfect fit. TBF continues and an extension octet is needed in the next
        ** block with LI=0 to indicate a PDU boundary.
        */
        tbci_ptr->pad_octets     = blk_size  - 1;

        /* Assume TBF ends with this block and no extension octet required.
        ** If another pdu is later concatenated, an extension octet required may
        ** incur an extra block with LI=0 to indicate a PDU boundary
        */
        tbci_ptr->b_ext_octet = TRUE;
      }
      else
      {
        /* Next PDU is not available hence the octets fits perfectly into this block.
        ** This block will have its CV = 0
        */
        tbci_ptr->pad_octets  = blk_size;
        tbci_ptr->b_ext_octet = FALSE;
      }
    }
    else
    {
      /* If partial block exists then that is an extra block
      */
      tbci_ptr->num_blks++;

      /* Value of free octets is the block size less the partial octets and
      ** an extension
      */
      tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;
    }
  }

  /* Store num_blks away as this value is the total block count if this TBF ends
  ** with this PDU
  */
  pdu_ptr->pg_num_blks_cur_pdu = tbci_ptr->num_blks;

} /* e_pg_calc_initial_tbc */


/*===========================================================================
===
===  FUNCTION      PG_ADJUST_TBC()
===
===  DESCRIPTION
===
===   This function adds the given number of octets (from a new PDU) to the
===   tbci_ptr struct and recalcs the num_blks field to update the TBC value
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   cs,ti:  coding scheme and TLLI indicator to determine the block size
===   tbci_ptr: pointer to TBC-info struct
===
===   OUTPUT
===
===   tbci_ptr: Altered struct
===
===  DEPENDENCIES
===
===   Must call pg_calc_initial_tbc() first for this function to work properly
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
static void pg_adjust_tbc
(
  gas_id_t            gas_id,
  grlc_ul_tbc_info_t  *tbci_ptr,
  gprs_grr_ul_mes_t   *pdu_ptr,
  uint8               cs,
  uint8               tlli_cs,
  uint8               ti
)
{
  uint16  blk_size;
  uint16  octets;

  /*----------------------------------------------------------------*/

  /* (was orig_len)
  */
  octets = pdu_ptr->len;

  /* Size of data block according to coding scheme, less the RLC header
  ** and whether TLLI is present
  */
  blk_size = get_blocks_payload_size(pdu_ptr->gas_id, cs, tlli_cs, ti);


  /*------------------------------------------------------------------------
  ** This is to cover the case where the previous block was the last block
  ** of a PDU and, say, 20 octets fit into the last block. If the TBF ends
  ** there is no need for a PDU boundary. However, if we are concatenating
  ** another PDU then we need to add an extension octet which pushes the
  ** previous PDU onto a new block; with a single final data octet. When
  ** this happens the partial octets is 0 and the extension octet is FALSE.
  **------------------------------------------------------------------------
  */
  if (tbci_ptr->partial_octets == 0)
  {
    if (tbci_ptr->b_ext_octet == FALSE)
    {
      /* Perfect fit and no extension octet. The last block will now take
      ** 1 data octet less to make room for the extension with LI = 0.
      */
      tbci_ptr->num_blks++;

      /* The final data octet is now in the next block
      */
      tbci_ptr->partial_octets = 1;

      /* now we need to add an extension octet to mark the end of this pdu
      */
      tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;

      tbci_ptr->b_ext_octet = TRUE;

      /* we have next_pdu_blks and pad_octets left and ready to add the new octets
      ** ...
      */
    }
    /* Extension octet flag is already set then no need to add
    */
  }

  /* At this point we have tbci.num_blks and the last block is partial and we
  ** are ready to add the new octets
  */

  /*---------------------------------
  ** Add new octets to partial block
  **---------------------------------
  */
  if (octets > tbci_ptr->pad_octets)
  {
    /* no won't fit. Some of the new octets will occupy the remaining space
    ** WITHOUT needing an extension octet and new pdu continues onto the
    ** next block
    */
    octets -= tbci_ptr->pad_octets;

    tbci_ptr->partial_octets = 0;
    tbci_ptr->pad_octets = blk_size;
    tbci_ptr->b_ext_octet = FALSE;

    /* here we have next_pdu_blks 'whole' blocks and ready to add the remaining
    ** new octets
    */
    tbci_ptr->num_blks += (octets / blk_size);

    /* new partial_octets
    */
    tbci_ptr->partial_octets = octets % blk_size;

    if (tbci_ptr->partial_octets == 0)
    {
      tbci_ptr->pad_octets = blk_size;

      /* perfect fit but we need to indicate PDU boundary so we need to add a Ext octet
      ** and thereby forcing another block
      */
      tbci_ptr->num_blks++;

      /* block size less 1 data octet less 1 ext octet
      */
      tbci_ptr->pad_octets = blk_size - 2;
      tbci_ptr->partial_octets = 1;
      tbci_ptr->b_ext_octet = TRUE;
    }
    else
    {
      /* add partial block
      */
      tbci_ptr->num_blks++;

      /*
      */
      tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;

      /* say we got extension octet already
      */
      tbci_ptr->b_ext_octet = TRUE;
    }
  }
  else if (octets == tbci_ptr->pad_octets)
  {
    /* new pdu could fit in remaining space but because the pdu ends here
    ** we need  to add an extenstion octet which unfortunately pushes the final
    ** octet into another. So, add extensio with LI = pad_octets - 1,
    ** partial_octets now hold 1 final data octet, pad_octets now holds the size
    ** of the block less 1 data octet and 1 extension octet.
    */
    tbci_ptr->num_blks++;

    /* in the new block the remaining space is block size less 1 data octet
    ** less one extension octet
    */
    tbci_ptr->pad_octets = blk_size - 2;
    tbci_ptr->partial_octets = 1;
    tbci_ptr->b_ext_octet = TRUE;

    /* done. To update vars .. */
  }
  else
  {
    /* pad_octets > octets
    */
    tbci_ptr->partial_octets += octets;
    tbci_ptr->pad_octets = (tbci_ptr->pad_octets - octets) - 1;
    tbci_ptr->b_ext_octet = TRUE;
  }

  ul[gas_id].tbc = pg[gas_id].tbci.num_blks;
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_ADJ
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_56, "GRLU pgadj tbc=%u new=%u abs=%u",
    ul[gas_id].tbc,pg[gas_id].tbci.num_blks,ul[gas_id].abs_bsn);
#endif

  /* If the TBF ends with this PDU then total block count is in the 'num_blks'
  */
  pdu_ptr->pg_num_blks_cur_pdu = tbci_ptr->num_blks;

} /* pg_adjust_tbc */

/*===========================================================================
===
===  FUNCTION      E_PG_ADJUST_TBC()
===
===  DESCRIPTION
===
===   This function adds the given number of octets (from a new PDU) to the
===   tbci_ptr struct and recalcs the num_blks field to update the TBC value
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   mcs,ti:  coding scheme and TLLI indicator to determine the block size
===   tbci_ptr: pointer to TBC-info struct
===
===   OUTPUT
===
===   tbci_ptr: Altered struct
===
===  DEPENDENCIES
===
===   Must call e_pg_calc_initial_tbc() first for this function to work properly
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
static void e_pg_adjust_tbc
(
  gas_id_t            gas_id,
  grlc_ul_tbc_info_t  *tbci_ptr,
  gprs_grr_ul_mes_t   *pdu_ptr,
  uint8               mcs,
  uint8               tlli_cs,
  uint8               ti
)
{
  uint16  blk_size;
  uint16  octets;

  /*----------------------------------------------------------------*/

  /* (was orig_len)
  */
  octets = pdu_ptr->len;

  /* Size of data block according to coding scheme, less the RLC header
  ** and whether TLLI is present
  */
  blk_size = get_blocks_payload_size(pdu_ptr->gas_id, mcs, tlli_cs, ti);

  /*------------------------------------------------------------------------
  ** This is to cover the case where the previous block was the last block
  ** of a PDU and, say, 20 octets fit into the last block. If the TBF ends
  ** there is no need for a PDU boundary. However, if we are concatenating
  ** another PDU then we need to add an extension octet with LI=0 into the new block.
  ** When this happens the partial octets is 0 and the extension octet is FALSE.
  **------------------------------------------------------------------------
  */
  if (tbci_ptr->partial_octets == 0)
  {
    if (tbci_ptr->b_ext_octet == FALSE)
    {
      /* we need to add an extension octet with LI=0 into the new block
      */
      tbci_ptr->pad_octets = blk_size - 1;

      tbci_ptr->b_ext_octet = TRUE;

      /* we have next_pdu_blks and pad_octets left and ready to add the new octets
      ** ...
      */
    }
    /* Extension octet flag is already set then no need to add
    */
  }

  /* At this point we have tbci.num_blks and the last block is partial and we
  ** are ready to add the new octets
  */

  /*---------------------------------
  ** Add new octets to partial block
  **---------------------------------
  */
  if (octets > tbci_ptr->pad_octets)
  {
    /* no won't fit. Some of the new octets will occupy the remaining space
    ** WITHOUT needing an extension octet and new pdu continues onto the
    ** next block
    */
    octets -= tbci_ptr->pad_octets;

    if( (tbci_ptr->partial_octets == 0) && (tbci_ptr->b_ext_octet == TRUE) )
    {
      /* LLC pdu fitted precisely into previous block. This block has not been counted.
      */
      tbci_ptr->num_blks++;
    }

    tbci_ptr->partial_octets = 0;
    tbci_ptr->pad_octets = blk_size;
    tbci_ptr->b_ext_octet = FALSE;

    /* here we have next_pdu_blks 'whole' blocks and ready to add the remaining
    ** new octets
    */
    tbci_ptr->num_blks += (octets / blk_size);

    /* new partial_octets
    */
    tbci_ptr->partial_octets = octets % blk_size;

    if (tbci_ptr->partial_octets == 0)
    {
      /* block size less 1 ext octet to indicate LI=0 in the next block
      */
      tbci_ptr->pad_octets = blk_size - 1;

      tbci_ptr->b_ext_octet = TRUE;
    }
    else
    {
      /* add partial block
      */
      tbci_ptr->num_blks++;

      tbci_ptr->pad_octets = (blk_size - tbci_ptr->partial_octets) - 1;

      /* say we got extension octet already
      */
      tbci_ptr->b_ext_octet = TRUE;
    }
  }
  else if (octets == tbci_ptr->pad_octets)
  {

    if( (tbci_ptr->partial_octets == 0) && (tbci_ptr->b_ext_octet == TRUE) )
    {
      /* LLC pdu fitted precisely into previous block. This block has not been counted.
      */
      tbci_ptr->num_blks++;
    }

    /* new pdu fits in remaining space therefore we need to add
    ** an extenstion octet to the next block indicating LI=0
    */
    tbci_ptr->pad_octets = blk_size - 1;
    tbci_ptr->partial_octets = 0;
    tbci_ptr->b_ext_octet = TRUE;

    /* done. To update vars .. */
  }
  else
  {
    /* pad_octets > octets
    */

    if( (tbci_ptr->partial_octets == 0) && (tbci_ptr->b_ext_octet == TRUE) )
    {
      /* LLC pdu fitted precisely into previous block. This block has not been counted.
      */
      tbci_ptr->num_blks++;
    }

    tbci_ptr->partial_octets += octets;
    tbci_ptr->pad_octets = (tbci_ptr->pad_octets - octets) - 1;
    tbci_ptr->b_ext_octet = TRUE;
  }

  ul[gas_id].tbc = pg[gas_id].tbci.num_blks;
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_ADJ
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_57, "GRLU pgadj tbc=%u new=%u abs=%u",
    ul[gas_id].tbc,pg[gas_id].tbci.num_blks,ul[gas_id].abs_bsn);
#endif

  /* If the TBF ends with this PDU then total block count is in the 'num_blks'
  */
  pdu_ptr->pg_num_blks_cur_pdu = tbci_ptr->num_blks;

} /* e_pg_adjust_tbc */

/*===========================================================================
===
===  FUNCTION      GRLC_COMPARE_PSN
===
===  DESCRIPTION
===
===    Compare function to seek a PDU in the queue with a given PSN value.
===
===  DEPENDENCIES
===
===    grlc_ul_llc_q OR ULQ
===
===  RETURN VALUE
===
===    Return 1 if the value matches and 0 otherwise
===
===  SIDE EFFECTS
===
===========================================================================*/
static uint16 grlc_compare_psn
(
  const gprs_grr_ul_mes_t*    item_ptr,
  const uint16*               psn_ptr
)
{
  return (uint16)( item_ptr->psn == *psn_ptr ? 1 : 0 );
}

/*===========================================================================
===
===  FUNCTION      CHECK_TX_CV_AND_REPROCESS()
===
===  DESCRIPTION
===
===    The table below is applicable for PDUs with same RLC MODE.
===
===    This function checks the conditions for re-processing. The conditions are
===    listed as states of the segmentation CV and transmit CV, and the actions
===    for these conditions are also listed in the table below.
===
===               tx CV   segmentation CV                      action
===          not active        not active               group PDU (1)
===          not active            active   reprocess, then group (2)
===              active        not active                 invalid (3)
===              active            active                no group (4)
===
===    The table below applies to EXT_UTBF mode
===    ---+----------------+----------+--------------+---------+-------------------------
===    #  |  seg_cv active | seg_cv=0 | tx_cv active | tx_cv=0 | action/comment
===    ---+----------------+----------+--------------+---------+-------------------------
===    1  |       0        |    [0]   |      0       |    0    | (1) group
===    2  |       0        |     0    |      0       |    1x   |  x
===    3  |       0        |     0    |      1x      |    0    |  x
===    4  |       0        |     0    |      1x      |    1x   |  x
===    5  |       0        |     1x   |      0       |    0    |  x
===    6  |       0        |     1x   |      0       |    1x   |  x
===    7  |       0        |     1x   |      1x      |    0    |  x
===    8  |       0        |     1x   |      1x      |    1x   |  x
===    9  |       1        |    [0]   |      0       |    0    | (1) group
===    10 |       1        |     0    |      0       |    1x   |  x
===    11 |       1        |    [0]   |      1       |    0    | (1) group
===    12 |       1        |     0    |      1       |    1x   |  x
===    13 |       1        |     1    |      0       |   [0]   | (2) reprocess then group or A*
===    14 |       1        |     1    |      0       |    1x   |  x
===    15 |       1        |     1    |      1       |   [0]   | (2) reprocess then group
===    16 |       1        |     1    |      1       |    1    | (3) clear grouping then
===       |                |          |              |         |     group
===    ---+----------------+----------+--------------+---------+-------------------------
===    A* - ptx cv = 0 and UL FIFO FULL.
===
===     x means not valid
===     [] means pre-dominant item in the row
===
===     #1: seg_cv not active and tx_cv correspondingly not active hence (1) group as
===         normal
===     #2..#8: invalid since if seg_cv is not active then nothing else will be active
===     #9 and #11: seg_cv active but not 0 so cv hence
===     #10 and #12: invalid since tx_cv is 0 whilst seg_cv is not
===     #13: seg_cv 0 and tx_cv is not 0, ie PL1 has not read block with cv = 0
===          hence (2) reprocess, then group or ptx cv = 0 and UL FIFO is full.
===     #14: invalid tx_cv 0 whilst tx_cv is not active at the same time
===     #15: seg_cv 0 and tx_cv not 0, ie. PL1 has not read block with CV = 0
===          hence (2) reprocess, then group
===     #16: tx_cv = 0 has read tx_cv = 0 so in order to discard the partial octets in
===          the block with cv=0 we clear the grouping info and start the new group with
===          the next PDU being the first PDU in the new group (3)
===
===     if (seg_cv != 0)
===        action (1)
===     else  -- seg_cv = 0
===        if (tx_cv != 0 or ptx_cv=0 & UL_FIFO_FULL)
===           action (2)
===        else -- tx_cv = 0
===           action (3)
===        endif
===     endif
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    TRUE if grouping of the next PDU is allowed
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean check_tx_cv_and_reprocess (gas_id_t gas_id)
{

  /* when rlc mode is different,the table above is void,
     as the current PDU group needs to be closed and no
     re-processing required.
  */
  if (((next_pdu_ptr[gas_id] != NULL) &&
       (cur_pdu_ptr[gas_id] != NULL)  &&
        next_pdu_ptr[gas_id]->rlc_mode != cur_pdu_ptr[gas_id]->rlc_mode
      ) ||
      ul[gas_id].b_forced_release
     )
  {
    /* Leave group_last_psn where it was.
    ** Leave the new (different PDU) for later.
    */
    if(cur_pdu_ptr[gas_id] != NULL)
    {
    
      /* Say it's now close (with group_last_psn as the closing PDU)
      */
      cur_pdu_ptr[gas_id]->pg_state = PGS_CLOSE;

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
      if ( next_pdu_ptr[gas_id] != NULL)
      {
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_58, "GRLU pg close %u new grp %u tbc=%u",
                  cur_pdu_ptr[gas_id]->pg_last_psn,
                  next_pdu_ptr[gas_id]->psn,
                  ul[gas_id].tbc);
      }
#endif
    }

    return FALSE;
  }
  else if (IS_EXT_UTBF_ACTIVE(gas_id))
  {
    if (IS_CV_NOT_ZERO(gas_id))
    {
      /* Action (1): group PDU as normal. Reduced message level to LOW
      */
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_59, "GRLU ch_grp (1) cv=%u ptx_cv=%u pak_cv=%u",
              ul[gas_id].cv,
              ul[gas_id].ptx_cv,
              ul[gas_id].pak_cv
             );
      return TRUE;
    }
    else /* ul[gas_id].cv is zero */
    {
      /* For case when the PTX/PACK FIFO is full due to slow USF
      ** and PTX CV = 0 , reprocessing is required to upscale the 
      ** mcs from previously lower mcs to higher MCS and also
      ** account for correct tbc by doing reprocess under
      ** ptx_cv/pak_cv = 0 updated latest in ul_fifo READ_MODE.
      ** Reprocess makes sense as FIFO is full and can be update 
      ** with correct tbc and fifo blocks.
      */
      if ( IS_TX_CV_NOT_ZERO(gas_id) ||
           ((IS_TX_CV_ZERO(gas_id)) &&
            (
              (grlc_ul_fifo_is_area_full(gas_id,TXA_PTX)) ||
              ((ul[gas_id].rlc_mode == GRLC_MODE_UNACK) && (grlc_ul_fifo_is_area_full(gas_id,TXA_UNACK)))
             )
           )
         )
      {
        /* Action (2): reprocess then group PDU
        */
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_60, "GRLU ch_grp (2) cv=%u ptx_cv=%u pak_cv=%u",
                ul[gas_id].cv,
                ul[gas_id].ptx_cv,
                ul[gas_id].pak_cv
               );

        /* Freeze FIFO access for reprocessing
        */
        grlc_ul_fifo_disable_pl1_access(gas_id);

        /* Reprocess by un-segmenting the segmented blocks which include the blocks with
        ** CV countdown
        */
        reprocess(gas_id, GRLC_REPROCESS_OTHER);

        /* Re-enable PL1 access to the FIFO
        */
        pl1_send_ph_data_req(gas_id);
        grlc_ul_fifo_enable_pl1_access(gas_id);

        /* The reprocessing will have taken care of the grouping so no further grouping
        ** is required
        */
        return FALSE;
      }
      else
      {
        /* Action (3): clear current group to discard partial octets in block with
        ** CV = 0, then create new PDU group for the next PDU
        */
        MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_61, "GRLU ch_grp (3) cv=%u ptx_cv=%u pak_cv=%u",
                ul[gas_id].cv,
                ul[gas_id].ptx_cv,
                ul[gas_id].pak_cv
               );

        pg_clr_tbc_info(gas_id, &pg[gas_id].tbci);
        ul[gas_id].abs_bsn=0;
        ul[gas_id].b_ext_utbf_init_group = TRUE;

        if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
        {
          E_NEW_DATA_BLOCK(gas_id, &cur_blk[gas_id]);
        }
        else
        {
          NEW_DATA_BLOCK(gas_id, &cur_blk[gas_id]);
        }

        return TRUE;
      }
    }
  }
  else if (IS_TX_CV_INACTIVE(gas_id))
  {
    if (IS_CV_INACTIVE(gas_id))
    {
      /* segment CV and transmit CV are both inactive hence group PDU(1)
      */
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_62, "GRLU ch_grp (1) both CV NOT active ptx_cv=%u pak_cv=%u cv=%u grp",
            ul[gas_id].ptx_cv,
            ul[gas_id].pak_cv,
            ul[gas_id].cv);
      return TRUE;
    }
    else
    {
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_63, "GRLU ch_grp (2) rep, grp ptx_cv=%u pak_cv=%u cv=%u grp",
            ul[gas_id].ptx_cv,
            ul[gas_id].pak_cv,
            ul[gas_id].cv);

      /*--------------------------
      ** Reprocess then group (2)
      **--------------------------
      */

      /* Freeze FIFO access for reprocessing
      */
      grlc_ul_fifo_disable_pl1_access(gas_id);

      /* Reprocess by un-segmenting the segmented blocks which include the blocks with
      ** CV countdown
      */
      //reprocess(gas_id, GRLC_REPROCESS_OTHER);

      /* Re-enable PL1 access to the FIFO
      */
      pl1_send_ph_data_req(gas_id);
      grlc_ul_fifo_enable_pl1_access(gas_id);

      /* The reprocessing will have taken care of the grouping so no further grouping
      ** is required
      */
      return FALSE;
    }
  }
  else
  {
    MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_64, "GRLU ch_grp (4) no rep, no grp ptx_cv=%u pak_cv=%u cv=%u !grp",
          ul[gas_id].ptx_cv,
          ul[gas_id].pak_cv,
          ul[gas_id].cv);
    /* tx CV active so segmentation CV must also be active: no group (4)
    */
    return FALSE;
  }
} /* check_tx_cv_and_reprocess */


/*===========================================================================
===
===  FUNCTION       SEEK_NEXT_PDU_IN_GROUP()
===
===  DESCRIPTION
===
===    Traverse the LLC PDU queue to find a PDU with the given PSN
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    Return pointer to PDU if found otherwise return NULL
===
===  SIDE EFFECTS
===
===    xxx.
===
============================================================================*/
static gprs_grr_ul_mes_t* seek_next_pdu_in_group
(
  const gprs_grr_ul_mes_t*  pdu_ptr  /* head PDU of group */
)
{
  /* Points to result of the search
  */
  gprs_grr_ul_mes_t*    sought_pdu_ptr;
  uint16                next_psn;

  /*--------------------------------------------------------*/

  if (pdu_ptr == NULL)
    return NULL;

  /* Pre-load result
  */
  sought_pdu_ptr = NULL;

  if (IS_GT_MODULO_COMP(cur_pdu_ptr[pdu_ptr->gas_id]->psn,pdu_ptr->pg_last_psn,32768))
  {
    /* If pg_last_psn is behind psn then it has not been initialised.
    ** So use psn instead
    */
    /* use the gas_id of the sought PDU to index "cur_pdu_ptr"
    */
    next_psn = cur_pdu_ptr[pdu_ptr->gas_id]->psn;
  }
  else
  {
    next_psn = pdu_ptr->pg_last_psn;
  }
  INC_MOD(next_psn,32768);

  /* Search the Cell Update PDU
  */
  sought_pdu_ptr = (gprs_grr_ul_mes_t*)
            q_linear_search(&ULQ(pdu_ptr->gas_id),
                            (q_compare_func_type)grlc_compare_psn,
                            &next_psn
                           );
  return sought_pdu_ptr;

} /* seek_next_pdu_in_group */

/*===========================================================================
===
===  FUNCTION      GROUP_OR_CLOSE_PDU()
===
===  DESCRIPTION
===
===    Compare the 'cur_pdu_ptr' and the given PDU 'next_ptr'. If radio priority
===    and peak-throughput class are the same then accumulate the pdu count
===    and octet count of the current PDU group and store in cur_pdu_ptr
===
===    Set the PDU group state to PGS_CLOSE or leave it at PGS_OPEN
===
===    TBC is not altered
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean group_or_close_pdu
(
  gas_id_t	                gas_id,
  const gprs_grr_ul_mes_t*  next_ptr
)
{
  /* Enhancement following 42.3.3.2.2 being downgraded
  **
  ** We group PDU when the current PDU and the next PDU are of the same RLC mode.
  ** (to add checking for PFI when it is supported)
  ** Assume that CV not started is assumed to be checked elsewhere
  */
  if (next_ptr->rlc_mode == cur_pdu_ptr[gas_id]->rlc_mode &&
      ul[gas_id].b_forced_release == FALSE
     )
  {
    /* Accumulate. (was orig_len)
    */
    cur_pdu_ptr[gas_id]->pg_pdus++;
    cur_pdu_ptr[gas_id]->pg_octets += next_ptr->len;

    /* Mark the last PSN of this open group, for now
    */
    cur_pdu_ptr[gas_id]->pg_last_psn = next_ptr->psn;

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_65, "GRLU pg add %u open tbc=%u num_blks=%u",
              cur_pdu_ptr[gas_id]->pg_last_psn,
              ul[gas_id].tbc,
              cur_pdu_ptr[gas_id]->pg_num_blks);
#endif

    return TRUE;
  }

  else
  /* Different group detected
  */
  {
    /* Leave group_last_psn where it was.
    ** Leave the new (different PDU) for later.
    */

    /* Say it's now close (with group_last_psn as the closing PDU)
    */
    cur_pdu_ptr[gas_id]->pg_state = PGS_CLOSE;

    /* Clear the running pg.tbci struct ready for the next PDU group
    */
    pg_clr_tbc_info(gas_id, &pg[gas_id].tbci);

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_66, "GRLU pg close %u new grp %u tbc=%u",
              cur_pdu_ptr[gas_id]->pg_last_psn,
              next_ptr->psn,
              ul[gas_id].tbc);
#endif

    return FALSE;
  }
} /* group_or_close_pdu */

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS_ON_QUEUE()
===
===  DESCRIPTION
===
===    If there are any PDUs on the queue then attempt to group them.
===    Also add any new PDUs to update TBC if TBC is not zero.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    cur_pdu_ptr
===
===  RETURN VALUE
===
===  SIDE EFFECTS
==
===    cur_pdu_ptr
===
===========================================================================*/
static void group_pdus_on_queue (gas_id_t gas_id)
{
  gprs_grr_ul_mes_t*    next_ptr;

  /*-----------------------------------------------------------------*/

  if (ul[gas_id].llc_pdus > 0)
  {
    /* Get ptr to head of queue
    */
    next_ptr = q_check(&ULQ(gas_id));

    if (next_ptr != NULL)
    {
      /* Advance to the PDU following the 'pg_last_psn'
      */
      next_ptr = seek_next_pdu_in_group(cur_pdu_ptr[gas_id]);
    }

    do
    {
      if (next_ptr != NULL)
      {
        /* Group PDU to cur_pdu_ptr and set cur_pdu_ptr[gas_id]->pg_state
        */
        if (check_tx_cv_and_reprocess(gas_id) && cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
        {
          if (group_or_close_pdu(gas_id, next_ptr))
          {
            if( ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS )
              pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                            (uint8)IS_TLLI_REQUIRED(gas_id));
            else
              e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                              (uint8)IS_TLLI_REQUIRED(gas_id));

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_67, "GRLU pgxf ONQ add psn %u len = %u blks=%u",
                    next_ptr->psn,
                    next_ptr->len,
                    pg[gas_id].tbci.num_blks);
#endif
          }
        } /* done grouping PDU */

        /* Seek next PDU only if the last PDUs were a match
        */
        if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
        {
          next_ptr = q_next(&ULQ(gas_id), &next_ptr->link);
        }
      }
    } while (next_ptr != NULL && cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE);
  }
} /* group_pdus_on_queue */

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS_ON_QUEUE_FOR_REPROCESSING()
===
===  DESCRIPTION
===
===    If there are any PDUs on the queue then attempt to group them.
===    Also add any new PDUs to update TBC if TBC is not zero.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    cur_pdu_ptr
===
===  RETURN VALUE
===
===  SIDE EFFECTS
==
===    cur_pdu_ptr
===
===========================================================================*/
static void group_pdus_on_queue_for_reprocessing (gas_id_t gas_id)
{
  gprs_grr_ul_mes_t*    next_ptr;

  /*-----------------------------------------------------------------*/

  if (ul[gas_id].llc_pdus > 0)
  {
    /* Get ptr to head of queue
    */
    next_ptr = q_check(&ULQ(gas_id));

    if (next_ptr != NULL)
    {
      /* Advance to the PDU following the 'pg_last_psn'
      */
      next_ptr = seek_next_pdu_in_group(cur_pdu_ptr[gas_id]);
    }

    do
    {
      if (next_ptr != NULL)
      {
        /* Group PDU to cur_pdu_ptr and set cur_pdu_ptr[gas_id]->pg_state
        */
        if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
        {
          if (group_or_close_pdu(gas_id, next_ptr))
          {
            if( ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS )
              pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                            (uint8)IS_TLLI_REQUIRED(gas_id));
            else
              e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                            (uint8)IS_TLLI_REQUIRED(gas_id));

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
            MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_68, "GRLU pgxf ONQ add psn %u len = %u blks=%u",
                    next_ptr->psn,
                    next_ptr->len,
                    pg[gas_id].tbci.num_blks);
#endif
          }
        } /* done checking PDU for grouping */

        /* Seek next PDU only if the last PDUs were a match
        */
        if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
        {
          next_ptr = q_next(&ULQ(gas_id), &next_ptr->link);
        }
      }
    } while (next_ptr != NULL && cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE);
  }
} /* group_pdus_on_queue_for_reprocessing */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GET_NUM_BLOCKS()
===
===  DESCRIPTION
===
===   Return the number of data blocks required to hold the specified number
===   of octets given the coding scheme, TLLI indicator. Partial block is
===   counted as one whole block and extension octet is accounted for.
===
===  DEPENDENCIES
===
===   RLC_DATA_BLOCK_SIZE[]
===   RM_TLLI_LEN
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
uint16 get_num_blks
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   cs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
)
{
  uint16  blk_size;
  uint16  num_blks;

  /*-----------------------------------------------------------------*/

  /* Data block size given coding scheme and TLLI indicator
  */
  blk_size = get_blocks_payload_size(gas_id, cs, tlli_cs, ti);

  if (blk_size == 0)
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_69, "GRLU blk_size=0, cs=%d tlli_cs=%d ti=%d",cs, tlli_cs, ti);

    /* Avoid blk_size = 0 !
    */
    blk_size = 1;
  }

  /* Number of whole blocks
  */
  num_blks = octets / blk_size;

  /* If there is no partial octets then the last block is perfectly filled.
  ** This means if there is another PDU whose extension (to show PDU boundary)
  ** octet will push the last data octet into another block
  */
  if (octets == (num_blks * blk_size))
  {
    /* Octets fit into the last block perfectly
    */
    if (b_next_pdu_avail)
    {
      /* But there is another PDU whose extension (to show PDU boundary) octet
      ** will push the last data octet into another block
      */
      num_blks++;
    }
    /* else TBF will end with block's CV = 0
    */
  }
  else
  {
    /* Partial means another block is required
    */
    num_blks++;
  }

  return num_blks;
}

/*===========================================================================
===
===  FUNCTION      E_GET_NUM_BLOCKS()
===
===  DESCRIPTION
===
===   Return the number of data blocks required to hold the specified number
===   of octets given the MCS, TLLI indicator. Partial block is
===   counted as one whole block and extension octet is accounted for.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    Number of blocks
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
uint16 e_get_num_blks
(
  gas_id_t gas_id,
  uint16  octets,
  uint8   mcs,
  uint8   tlli_cs,
  uint8   ti
)
{
  uint16  blk_size;
  uint16  num_blks;

  /*-----------------------------------------------------------------*/

  /* Data block size given coding scheme and TLLI indicator
  */
  blk_size = get_blocks_payload_size(gas_id, mcs, tlli_cs, ti);

  if (blk_size == 0)
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_70, "GRLU blk_size=0, mcs=%d tlli_cs=%d ti=%d",mcs, tlli_cs, ti);

    /* Avoid blk_size = 0 !
    */
    blk_size = 1;
  }

  /* Number of whole blocks
  */
  num_blks = octets / blk_size;

  /* If there is no partial octets then the last block is perfectly filled.
  ** ALthough we need to indicate LI=0 in the next block, we can not count
  ** the extension octet as a block yet.
  */
  if (octets != (num_blks * blk_size))
  {
    /* Partial means another block is required
    */
    num_blks++;
  }

  return num_blks;
}

/*===========================================================================
===
===  FUNCTION      GET_NUM_BLKS_REQ()
===
===  DESCRIPTION
===
===   Calculate the number of data blocks given coding scheme, tlli indicator,
===   and the number of octets for GPRS and EGPRS..
===
===   Return Extra params such as the number of partial octets, the pad octets
===   and the b_ext_octet flag to allow precise calculation of the extra blocks
===   required when reallocating.
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   b_next_pdu_avail: when TRUE indicates there is a next PDU to follow
===
===   OUTPUT
===
===   ul[gas_id].num_blks: total number of blocks required to hold the specified
===     octets given the block size
===
===   ul[gas_id].b_ext_octet: where a perfect fit is in force this flag indicates
===     whether the an extension octet is already present and accounted for
===     in the final data block.
===
===   ul[gas_id].partial_octets: number of (data) octets in the (last) partial block
===
===   ul[gas_id].pad_octets: number of octets remained free in the partial block
===     This value may or may not already account for any extension octet.
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
void get_num_blks_req
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   ti,
  uint16  b_next_pdu_avail
)
{
  if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    e_get_num_blks_ex(gas_id, octets, ul[gas_id].cur_res.cs,ul[gas_id].tlli_cs,ti,b_next_pdu_avail);
  }
  else
  {
    get_num_blks_ex(gas_id, octets,ul[gas_id].cur_res.cs,ul[gas_id].tlli_cs,ti,b_next_pdu_avail);
  }

  return;
} /* get_num_blks_req */

/*===========================================================================
===
===  FUNCTION      GET_NUM_BLKS_EX()
===
===  DESCRIPTION
===
===   Calculate the number of data blocks given coding scheme, tlli indicator,
===   and the number of octets as per get_num_blks().
===
===   Return Extra params such as the number of partial octets, the pad octets
===   and the b_ext_octet flag to allow precise calculation of the extra blocks
===   required when reallocating.
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   cs,ti:  coding scheme and TLLI indicator to determine the block size
===   b_next_pdu_avail: when TRUE indicates there is a next PDU to follow
===
===   OUTPUT
===
===   ul[gas_id].num_blks: total number of blocks required to hold the specified
===     octets given the block size
===
===   ul[gas_id].b_ext_octet: where a perfect fit is in force this flag indicates
===     whether the an extension octet is already present and accounted for
===     in the final data block.
===
===   ul[gas_id].partial_octets: number of (data) octets in the (last) partial block
===
===   ul[gas_id].pad_octets: number of octets remained free in the partial block
===     This value may or may not already account for any extension octet.
===
===  DEPENDENCIES
===
===   RLC_DATA_BLOCK_SIZE[]
===   RM_RLC_HDR_LEN
===   RM_TLLI_LEN
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void get_num_blks_ex
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   cs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
)
{
  uint16  blk_size;

  /*-----------------------------------------------------------------*/

  /* (payload) block size given coding scheme, and tlli indicator (no MAC header)
  */
  blk_size = get_blocks_payload_size(gas_id, cs, tlli_cs, ti);

  if (blk_size == 0)
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_71, "GRLU blk_size=0, cs=%d tlli_cs=%d ti=%d",cs, tlli_cs, ti);

    /* Avoid blk_size = 0 !
    */
    blk_size = 1;
  }

  /* whole number of blocks
  */
  ul[gas_id].num_blks = octets / blk_size;

  /* Number of octets in the partial block (remained) -- avoid using the % operator
  */
  ul[gas_id].partial_octets = octets - (uint16)(ul[gas_id].num_blks * blk_size);

  /* Note that when the final block is partial, an extension octet exists
  ** to mark the end of this pdu. If the TBF ends with this block then the
  ** remaining octets are padded.
  **
  ** When the final block fits perfectly then an extension octet is not required,
  ** and the TBF is assumed to end on that block.
  */

  if (ul[gas_id].partial_octets == 0)
  {
    if (b_next_pdu_avail)
    {
      /* Perfect fit but assume another PDU follows so the extension octet will push
      ** the last octet into a new block
      */
      ul[gas_id].num_blks++;

      /* One data octet pushed over
      */
      ul[gas_id].partial_octets = 1;

      /* block size less 1 extension octet less 1 data octet
      */
      ul[gas_id].pad_octets = blk_size - 2;

      /* Assume TBF ends with this block and no extension octet required.
      ** If another pdu is later concatenated, an extension octet required may
      ** incur an extra block
      */
      ul[gas_id].b_ext_octet = TRUE;
    }
    else
    {
      /* PDU fits into remaining data block perfectly and there is no following PDU
      */
      ul[gas_id].pad_octets = blk_size;
      ul[gas_id].b_ext_octet = FALSE;
    }
  }
  else
  {
    /* if partial block exists then add 1 to the whole number of blocks
    */
    ul[gas_id].num_blks++;

    /* value of free octets is the block size less the partial octets and
    ** an extension
    */
    ul[gas_id].pad_octets = (blk_size - ul[gas_id].partial_octets) - 1;

    ul[gas_id].b_ext_octet = TRUE;
  }

} /* get_num_blks_ex */

/*===========================================================================
===
===  FUNCTION      E_GET_NUM_BLKS_EX()
===
===  DESCRIPTION
===
===   Calculate the number of data blocks given coding scheme, tlli indicator,
===   and the number of octets as per get_num_blks().
===
===   Return Extra params such as the number of partial octets, the pad octets
===   and the b_ext_octet flag to allow precise calculation of the extra blocks
===   required when reallocating.
===
===  PARAMETERS
===
===   INPUT
===
===   octets: number of octets to store in the blocks
===   mcs,ti:  coding scheme and TLLI indicator to determine the block size
===   b_next_pdu_avail: when TRUE indicates there is a next PDU to follow
===
===   OUTPUT
===
===   ul[gas_id].num_blks: total number of blocks required to hold the specified
===     octets given the block size
===
===   ul[gas_id].b_ext_octet: where a perfect fit is in force this flag indicates
===     whether the an extension octet is already present and accounted for
===     in the final data block.
===
===   ul[gas_id].partial_octets: number of (data) octets in the (last) partial block
===
===   ul[gas_id].pad_octets: number of octets remained free in the partial block
===     This value may or may not already account for any extension octet.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void e_get_num_blks_ex
(
  gas_id_t  gas_id,
  uint16  octets,
  uint8   mcs,
  uint8   tlli_cs,
  uint8   ti,
  uint16  b_next_pdu_avail
)
{
  uint16  blk_size;

  /*-----------------------------------------------------------------*/

  /* (payload) block size given coding scheme, and tlli indicator
  */
  blk_size = get_blocks_payload_size(gas_id, mcs, tlli_cs, ti);

  if (blk_size == 0)
  {
    MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_72, "GRLU blk_size=0, mcs=%d tlli_cs=%d ti=%d",mcs, tlli_cs, ti);

    /* Avoid blk_size = 0 !
    */
    blk_size = 1;
  }

  /* whole number of blocks
  */
  ul[gas_id].num_blks = octets / blk_size;

  /* Number of octets in the partial block (remained) -- avoid using the % operator
  */
  ul[gas_id].partial_octets = octets - (uint16)(ul[gas_id].num_blks * blk_size);

  /* Note that when the final block is partial, an extension octet exists
  ** to mark the end of this pdu. If the TBF ends with this block then the
  ** remaining octets are padded.
  **
  ** When the final block fits perfectly then an extension octet is not required,
  ** and the TBF is assumed to end on that block.
  */

  if (ul[gas_id].partial_octets == 0)
  {
    if (b_next_pdu_avail)
    {
      /* Perfect fit but assume another PDU follows so an extension octet
      ** needs to be added in the next block to indicate LI=0
      */

      /* block size less 1 extension octet
      */
      ul[gas_id].pad_octets = blk_size - 1;

      /* Assume TBF ends with this block and no extension octet required.
      ** If another pdu is later concatenated, we need to remeber that
      ** the extension octet has been acounted for.
      */
      ul[gas_id].b_ext_octet = TRUE;
    }
    else
    {
      /* PDU fits into remaining data block perfectly and there is no following PDU
      */
      ul[gas_id].pad_octets = blk_size;
      ul[gas_id].b_ext_octet = FALSE;
    }
  }
  else
  {
    /* if partial block exists then add 1 to the whole number of blocks
    */
    ul[gas_id].num_blks++;

    /* value of free octets is the block size less the partial octets and
    ** an extension
    */
    ul[gas_id].pad_octets = (blk_size - ul[gas_id].partial_octets) - 1;

    ul[gas_id].b_ext_octet = TRUE;
  }

} /* e_get_num_blks_ex */

/*===========================================================================
===
===  FUNCTION      PG_CLR_TBC_INFO()
===
===  DESCRIPTION
===
===   This function clears the PDU group info struct to 0. This structure holds
===   the current partial octets, pad octets, extention octet etc.. of the
===   current PDU group. If a new group starts this structure should be cleared
===
===  PARAMETERS
===
===   tbci_ptr: pointer to 'total-block-count-info' struct
===
===   OUTPUT
===
===   tbci_ptr: returns the altered content
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
void pg_clr_tbc_info (gas_id_t gas_id, grlc_ul_tbc_info_t *tbci_ptr)
{
  /* passed in value already correctly gas-indexed
  */
  tbci_ptr->octets = 0;
  tbci_ptr->num_blks = 0;
  tbci_ptr->partial_octets = 0;
  tbci_ptr->pad_octets = 0;
  tbci_ptr->b_ext_octet = FALSE;

  /* Ensure no reallocation on value is not properly initialised
  */
  ul[gas_id].ptx_cv = BS_CV_CONST;
  ul[gas_id].pak_cv = BS_CV_CONST;
  MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_73, "ul.ptx_cv = BS_CV_CONST in pg_clr_tbc_info()");
}

/*===========================================================================
===
===  FUNCTION      PG_CLR_GROUP_INFO()
===
===  DESCRIPTION
===
===   Inititialise PDU-group information structure
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
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
===========================================================================*/
void pg_clr_group_info (gprs_grr_ul_mes_t*  pdu_ptr)
{
  /* Reinstate INIT state
  */
  pdu_ptr->pg_state = PGS_INIT;

  pdu_ptr->pg_octets_rem = 0;
  pdu_ptr->pg_last_psn = pdu_ptr->psn;
  pdu_ptr->pg_num_blks = 0;
  pdu_ptr->pg_pdus = 0;
}

/*===========================================================================
===
===  FUNCTION      PG_XFER_TO_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of PDUs in the PDU group and calculates
===   the number of blocks required to hold these octets and store the value in
===   tbci.num_blks field. This value is to load into TBC
===
===   This function should only be called when ul[gas_id].tbc is 0. The pg_octets field
===   not actually used. Instead the current PDU is first added to the TBC-info
===   structure of the PDU group. The next PDU in the queue is added one by one
===   to this structure.
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
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
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void pg_calc_xfer_value (gas_id_t gas_id)
{
  gprs_grr_ul_mes_t*  pdu_ptr;
  uint8               ti;

  /*------------------------------------------------------------*/

  /* Zero everything
  */
  pg_clr_tbc_info(gas_id, &pg[gas_id].tbci);

  ti = IS_TLLI_REQUIRED(gas_id);

  /* Add cur pdu octets to TBC info struct. The mode is Dynamic Open but is
  ** there a next PDU !
  */
  pg_calc_initial_tbc(gas_id, &pg[gas_id].tbci,
                      cur_pdu_ptr[gas_id],
                      ul[gas_id].cur_res.cs,
                      ul[gas_id].tlli_cs,
                      ti,
          (uint16)IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,cur_pdu_ptr[gas_id]->psn,32768));
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_74, "GRLU pgxf 1st psn %u len = %u blks=%u",
          cur_pdu_ptr[gas_id]->psn,
          cur_pdu_ptr[gas_id]->len,
          pg[gas_id].tbci.num_blks);
#endif

  /* Assess the next_pdu
  */
  if (IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,cur_pdu_ptr[gas_id]->psn,32768))
  {
    if (next_pdu_ptr[gas_id] == NULL)
      dequeue_pdu(gas_id, &next_pdu_ptr[gas_id]);

    if (next_pdu_ptr[gas_id] != NULL)
    {
      /* Next_pdu is loaded so add its length to TBC-info
      */
      pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_pdu_ptr[gas_id], ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs, ti);
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_75, "GRLU pgxf add psn %u len = %u blks=%u",
              next_pdu_ptr[gas_id]->psn,
              next_pdu_ptr[gas_id]->len,
              pg[gas_id].tbci.num_blks);
#endif
    } /* next_pdu is loaded */
  } /* group covers more than current PDU */

  /* Does PDU group cover any blocks in the queue
  */
  pdu_ptr = q_check(&ULQ(gas_id));

  if (pdu_ptr != NULL)
  {
    do
    {
      if (IS_GE_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,pdu_ptr->psn,32768))
      {
        pg_adjust_tbc(gas_id, &pg[gas_id].tbci, pdu_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs, ti);
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_76, "GRLU pgxf ONQ add psn %u len=%u blks=%u",
                pdu_ptr->psn,
                pdu_ptr->len,
                pg[gas_id].tbci.num_blks);
#endif
      }

      pdu_ptr = q_next(&ULQ(gas_id), &pdu_ptr->link);

    } while (pdu_ptr != NULL &&
             IS_GE_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,pdu_ptr->psn,32768)
            );
  }

  /* Update TBC value
  */
  ul[gas_id].tbc = pg[gas_id].tbci.num_blks;
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_77, "GRLU pgxf tbc=%u new=%u abs=%u",ul[gas_id].tbc,pg[gas_id].tbci.num_blks,ul[gas_id].abs_bsn);
#endif

} /* pg_calc_xfer_value */

/*===========================================================================
===
===  FUNCTION      E_PG_XFER_TO_TBC()
===
===  DESCRIPTION
===
===   This function takes the number of PDUs in the PDU group and calculates
===   the number of blocks required to hold these octets and store the value in
===   tbci.num_blks field. This value is to load into TBC
===
===   This function should only be called when ul[gas_id].tbc is 0. The pg_octets field
===   not actually used. Instead the current PDU is first added to the TBC-info
===   structure of the PDU group. The next PDU in the queue is added one by one
===   to this structure.
===
===  PARAMETERS
===
===   INPUT
===
===
===   OUTPUT
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
===    also returns the number of remanining octets in the last data block
===
===========================================================================*/
void e_pg_calc_xfer_value (gas_id_t gas_id)
{
  gprs_grr_ul_mes_t*  pdu_ptr;
  uint8               ti;

  /*------------------------------------------------------------*/

  /* Zero everything
  */
  pg_clr_tbc_info(gas_id, &pg[gas_id].tbci);

  ti = IS_TLLI_REQUIRED(gas_id);

  /* Add cur pdu octets to TBC info struct. The mode is Dynamic Open but is
  ** there a next PDU !
  */
  e_pg_calc_initial_tbc(gas_id, &pg[gas_id].tbci, cur_pdu_ptr[gas_id], ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs, ti,
    (uint16)IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,cur_pdu_ptr[gas_id]->psn,32768));

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_78, "GRLU pgxf 1st psn %u len = %u blks=%u",
          cur_pdu_ptr[gas_id]->psn,
          cur_pdu_ptr[gas_id]->len,
          pg[gas_id].tbci.num_blks);
#endif

  /* Assess the next_pdu
  */
  if (IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn, cur_pdu_ptr[gas_id]->psn, 32768))
  {
    if (next_pdu_ptr[gas_id] == NULL)
      dequeue_pdu(gas_id, &next_pdu_ptr[gas_id]);

    if (next_pdu_ptr[gas_id] != NULL)
    {
      /* Next_pdu is loaded so add its length to TBC-info
      */
      e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_pdu_ptr[gas_id], ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs, ti);
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_79, "GRLU pgxf add psn %u len = %u blks=%u",
              next_pdu_ptr[gas_id]->psn,
              next_pdu_ptr[gas_id]->len,
              pg[gas_id].tbci.num_blks);
#endif
    } /* next_pdu is loaded */
  } /* group covers more than current PDU */

  /* Does PDU group cover any blocks in the queue
  */
  pdu_ptr = q_check(&ULQ(gas_id));

  if (pdu_ptr != NULL)
  {
    do
    {
      if (IS_GE_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn, pdu_ptr->psn, 32768))
      {
        e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, pdu_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs, ti);
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_80, "GRLU pgxf ONQ add psn %u len=%u blks=%u",
                pdu_ptr->psn,
                pdu_ptr->len,
                pg[gas_id].tbci.num_blks);
#endif
      }

      pdu_ptr = q_next(&ULQ(gas_id), &pdu_ptr->link);

    } while (pdu_ptr != NULL &&
             IS_GE_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn, pdu_ptr->psn, 32768)
            );
  }

  /* Update TBC value
  */
  ul[gas_id].tbc = pg[gas_id].tbci.num_blks;
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_81, "ERLU pgxf tbc=%u new=%u abs=%u",ul[gas_id].tbc,pg[gas_id].tbci.num_blks,ul[gas_id].abs_bsn);
#endif
} /* e_pg_calc_xfer_value */

/*===========================================================================
===
===  FUNCTION      GROUP_PDU_INIT()
===
===  DESCRIPTION
===
===    PDU is being inserted in the queue for the first time so put a PSN
===    value on the PDU and set its grouping state to PGS_INIT.
===
===    This function should be called in enqueue_pdu()
===
===  PARAMS
===
===    in_pdu_ptr:  pointer to incoming PDU
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul[gas_id].llc_group_pdus, ul[gas_id].llc_group_octets
===
===========================================================================*/
void group_pdu_init (gas_id_t gas_id, gprs_grr_ul_mes_t* pdu_ptr)
{
  INC_MOD(pg[pdu_ptr->gas_id].psn,32768);
  pdu_ptr->psn = pg[pdu_ptr->gas_id].psn;
  pdu_ptr->pg_last_psn = pg[pdu_ptr->gas_id].psn;
  pdu_ptr->pg_state = PGS_INIT;

  pdu_ptr->realloc_action = RA_NULL;
}

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS()
===
===  DESCRIPTION
===
===    Search the PDU queue to find a next PDU, if one exists, and compare
===    its radio priority and peak-throughput class with the current PDU group.
===    If they match, then increment the number of PDUs in the group, the total
===    group octets count and update the group status. If the next PDU type
===    does not match the current type then terminate the current PDU group.
===
===  PARAMS
===
===    in_pdu_ptr:  pointer to incoming PDU
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul[gas_id].llc_group_pdus, ul[gas_id].llc_group_octets
===
===========================================================================*/
void group_pdus (gas_id_t gas_id)
{
  /* Points to result of the search
  */
  gprs_grr_ul_mes_t*    next_ptr;
  boolean               b_check = TRUE; /* ensure ..onq() is called unless
                                        ** .. reprocessing happens
                                        */

  /*-----------------------------------------------------------------*/

  if (cur_pdu_ptr[gas_id] == NULL)
  {
    MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_82, "GRLU pg cur pdu NULL (tbc=%u)",ul[gas_id].tbc);
    return;
  }

  switch (cur_pdu_ptr[gas_id]->pg_state)
  {
    case PGS_INIT:
      /*---------------------------------------------
      ** Make cur_pdu_ptr the first PDU in the group
      **---------------------------------------------
      */
      /* Presume the group terminates here
      */
      cur_pdu_ptr[gas_id]->pg_last_psn = cur_pdu_ptr[gas_id]->psn;

      /* Init counters. (was orig_len)
      */
      cur_pdu_ptr[gas_id]->pg_pdus = 1;
      cur_pdu_ptr[gas_id]->pg_octets = cur_pdu_ptr[gas_id]->len;

      /* The state is now PGS_OPEN
      */
      cur_pdu_ptr[gas_id]->pg_state = PGS_OPEN;

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_83, "GRLU pg INIT psn %u tbc=%u last_p=%u",
                cur_pdu_ptr[gas_id]->psn,
                ul[gas_id].tbc,
                cur_pdu_ptr[gas_id]->pg_last_psn);
#endif

      /* .. Drop through deliberately.
      ** .. No break statement
      */
      /*lint -fallthrough */

    case PGS_OPEN:
      /*-------------------------------------------------------------
      ** Now assess the next_pdu_ptr and then the queue itself.
      ** Only group a PDU if its PSN is greater than the pg_last_psn
      **-------------------------------------------------------------
      */
      if (next_pdu_ptr[gas_id] == NULL)
        dequeue_pdu(gas_id, &next_pdu_ptr[gas_id]);

      if (next_pdu_ptr[gas_id] != NULL)
      {
        if (IS_GT_MODULO_COMP(next_pdu_ptr[gas_id]->psn,cur_pdu_ptr[gas_id]->pg_last_psn,32768))
        {
          /* Group the 'next_pdu_ptr'
          */
          next_ptr = next_pdu_ptr[gas_id];

          /* If reprocessing takes place then this is set
          */
          b_check = check_tx_cv_and_reprocess(gas_id);

          if (b_check && cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
          {
            if (group_or_close_pdu(gas_id, next_ptr))
            {
              /* Group only if check_tx_cv_and_reprocess() had not performed PDU grouping
              */
              if (ul[gas_id].b_ext_utbf_init_group)
              {
                if( ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS )
                  pg_calc_initial_tbc
                    (gas_id,
                      &pg[gas_id].tbci, next_ptr,
                      ul[gas_id].cur_res.cs,
                      ul[gas_id].tlli_cs,
                      (uint8)IS_TLLI_REQUIRED(gas_id),
                      (uint16)IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,
                                                cur_pdu_ptr[gas_id]->psn,
                                                32768
                                               )
                    );
                else
                {
                  e_pg_calc_initial_tbc
                    (gas_id, 
                      &pg[gas_id].tbci,
                      next_ptr,
                      ul[gas_id].cur_res.cs,
                      ul[gas_id].tlli_cs,
                      (uint8)IS_TLLI_REQUIRED(gas_id),
                      (uint16)IS_GT_MODULO_COMP(cur_pdu_ptr[gas_id]->pg_last_psn,
                                                cur_pdu_ptr[gas_id]->psn,
                                                32768
                                               )
                    );
                }

                /* Update TBC value
                */
                ul[gas_id].tbc = pg[gas_id].tbci.num_blks;
                MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_84, "GRLU pgxf tbc=%u new=%u abs=%u",
                        ul[gas_id].tbc,
                        pg[gas_id].tbci.num_blks,
                        ul[gas_id].abs_bsn);

                ul[gas_id].b_ext_utbf_init_group = FALSE;
              }
              else
              {
                if( ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS )
                  pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                                (uint8)IS_TLLI_REQUIRED(gas_id));
                else
                  e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                                (uint8)IS_TLLI_REQUIRED(gas_id));
              }

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
              MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_85, "GRLU pgxf add psn %u len = %u blks=%u",
                      next_ptr->psn,
                      next_ptr->len,
                      pg[gas_id].tbci.num_blks);
#endif
            }
          } /* done checking PDU for grouping */
          else
          {
            MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_86, "GRLU pg check_tx_cv ret FALSE psn=%u tbc=%u abs=%u",
                    cur_pdu_ptr[gas_id]->psn,
                    ul[gas_id].tbc,
                    ul[gas_id].abs_bsn);
          }

          /* Check out QoS of current PDU against the next PDU and initiate reallocation
          ** activities if required
          */
          check_to_send_prr(gas_id);

        } /* next PDU follows current PDU */
      } /* next PDU is available */

      /* Attempt to group any PDUs on the queue.
      ** Also add PDUs to TBC if TBC is not zero, ie. in transfer
      */

      /* Avoid grouping on queue if reprocessing had taken place
      */
      if (b_check)
      {
        MSG_GRLC_LOW_1_G(GRLC_F3_MSG_ENUM_87, "group_pdus calls group_pdus_onq b_check=%u",b_check);
        group_pdus_on_queue(gas_id);
      }
      break;


    case PGS_CLOSE:

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_88, "GRLU PGS close last_psn %u tbc=%u num_blks=%u",
                cur_pdu_ptr[gas_id]->pg_last_psn,
                ul[gas_id].tbc,
                cur_pdu_ptr[gas_id]->pg_num_blks);
#endif
      break;

    default:

      MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_89, "GRLU PGS bad state %d",cur_pdu_ptr[gas_id]->pg_state);
      break;

  } /* switch pg_state */

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_90, "GRLU gp end tbc=%u psn=%u last_psn=%u",
          ul[gas_id].tbc,
          cur_pdu_ptr[gas_id]->psn,
          cur_pdu_ptr[gas_id]->pg_last_psn);
#endif
  show_group_status(gas_id);

} /* group_pdus() */

/*===========================================================================
===
===  FUNCTION      UNGROUP_PDU()
===
===  DESCRIPTION
===
===    This function is used when the current PDU runs out and the next PDU is
===    picked up. This function transfer the PDU Grouping status and counters
===    to the next PDU if one exists. It also subtracts the PDU count and
===    octets count of the finished (current PDU) from the record.
===
===  PARAMS
===
===    pdu_ptr: points to the PDU to be taken from the PDU group.
===             Normally this will be 'cur_pdu_ptr'
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void ungroup_pdu (gas_id_t gas_id, gprs_grr_ul_mes_t*  pdu_ptr)
{
  /* Point to the PDU following the one being ungrouped, if any
  */
  gprs_grr_ul_mes_t*  next_ptr;

  /*------------------------------------------------------------------*/

  if (cur_pdu_ptr[gas_id] == NULL)
  {
    MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_91, "GRLU unpg pdu NULL");
    return;
  }

  /* Take away the current PDU from our grouping record
  */
  if (pdu_ptr->pg_pdus > 0)
  {
    pdu_ptr->pg_pdus--;
  }
  else
  {
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING
    MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_92, "GRLU PGS ungp neg pg_pdus=%u",pdu_ptr->pg_pdus);
#endif
  }

  /* Take away the octets of the current PDU from our grouping record
  */
  if (pdu_ptr->pg_octets >= pdu_ptr->len) /* was orig */
  {
    pdu_ptr->pg_octets -= pdu_ptr->len; /* was orig */
  }
  else
  {
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING
    MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_93, "GRLU PGS ungp neg pg_octs=%u len=%u",pdu_ptr->pg_octets, pdu_ptr->len);
#endif
  }

  /* If the PDU group is close OR this PDU is the last PDU in the group then
  ** don't transfer the group info. The next pdu will have the PGS_INIT status
  */
  if (cur_pdu_ptr[gas_id]->pg_state == PGS_CLOSE &&
      cur_pdu_ptr[gas_id]->pg_last_psn == cur_pdu_ptr[gas_id]->psn
     )
  {
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING
    MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_94, "GRLU ungp pg_close psn=%u",cur_pdu_ptr[gas_id]->psn);
#endif
  }
  else
  {
    /*--------------------------------------------------------------------
    ** Find the next PDU in the chain, if any, which can be the next PDU,
    ** or on the queue and transfer the group info onto it
    **--------------------------------------------------------------------
    */
    if (next_pdu_ptr[gas_id] == NULL)
    {
      /* We need to find the next PDU in the chain on the queue now
      */
      next_ptr = q_check(&ULQ(gas_id));

      /* Assume pg_last_psn > psn now
      */

      /* Transfer group info to the next PDU
      */
      if (next_ptr != NULL)
      {
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING
        MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_95, "GRLU ungp xfer psn(%u) to psn(%u)",pdu_ptr->psn, next_ptr->psn);
#endif
        next_ptr->pg_state = pdu_ptr->pg_state;
        next_ptr->pg_pdus = pdu_ptr->pg_pdus;
        next_ptr->pg_octets = pdu_ptr->pg_octets;
        next_ptr->pg_octets_rem = pdu_ptr->pg_octets_rem;
        next_ptr->pg_last_psn = pdu_ptr->pg_last_psn;
      }
    }
    else
    {
      /* Transfer group onto the next_pdu_ptr
      */
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_UNGROUPING
      MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_96, "GRLU ungp xfer psn(%u) to psn(%u)",pdu_ptr->psn, next_pdu_ptr[gas_id]->psn);
#endif
      next_pdu_ptr[gas_id]->pg_state = pdu_ptr->pg_state;
      next_pdu_ptr[gas_id]->pg_pdus = pdu_ptr->pg_pdus;
      next_pdu_ptr[gas_id]->pg_octets = pdu_ptr->pg_octets;
      next_pdu_ptr[gas_id]->pg_octets_rem = pdu_ptr->pg_octets_rem;
      next_pdu_ptr[gas_id]->pg_last_psn = pdu_ptr->pg_last_psn;
    }
  }
} /* ungroup_pdu() */

/*===========================================================================
===
===  FUNCTION      GROUP_PDUS_FOR_REPROCESSING()
===
===  DESCRIPTION
===
===    This function performs grouping of PDUs without considering reallocation
===    actions
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul[gas_id].llc_group_pdus, ul[gas_id].llc_group_octets
===
===========================================================================*/
void group_pdus_for_reprocessing ( gas_id_t gas_id )
{
  /* Points to result of the search
  */
  gprs_grr_ul_mes_t*    next_ptr;

  /*-----------------------------------------------------------------*/

  if (cur_pdu_ptr[gas_id] == NULL)
  {
    MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_97, "GRLU pg cur pdu NULL (tbc=%u)",ul[gas_id].tbc);
    return;
  }

  switch (cur_pdu_ptr[gas_id]->pg_state)
  {
    case PGS_INIT:
      /*---------------------------------------------
      ** Make cur_pdu_ptr the first PDU in the group
      **---------------------------------------------
      */
      /* Presume the group terminates here
      */
      cur_pdu_ptr[gas_id]->pg_last_psn = cur_pdu_ptr[gas_id]->psn;

      /* Init counters. (was orig_len)
      */
      cur_pdu_ptr[gas_id]->pg_pdus = 1;
      cur_pdu_ptr[gas_id]->pg_octets = cur_pdu_ptr[gas_id]->len;

      /* The state is now PGS_OPEN
      */
      cur_pdu_ptr[gas_id]->pg_state = PGS_OPEN;

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_98, "GRLU pg INIT psn %u tbc=%u last_p=%u",
                cur_pdu_ptr[gas_id]->psn,
                ul[gas_id].tbc,
                cur_pdu_ptr[gas_id]->pg_last_psn);
#endif
      /* .. Drop through deliberately.
      ** .. No break statement
      */
      /*lint -fallthrough */

    case PGS_OPEN:
      /*-------------------------------------------------------------
      ** Now assess the next_pdu_ptr and then the queue itself.
      ** Only group a PDU if its PSN is greater than the pg_last_psn
      **-------------------------------------------------------------
      */
      if (next_pdu_ptr[gas_id] == NULL)
        dequeue_pdu(gas_id, &next_pdu_ptr[gas_id]);

      if (next_pdu_ptr[gas_id] != NULL)
      {
        if (IS_GT_MODULO_COMP(next_pdu_ptr[gas_id]->psn,cur_pdu_ptr[gas_id]->pg_last_psn,32768))
        {
          /* Group the 'next_pdu_ptr'
          */
          next_ptr = next_pdu_ptr[gas_id];
          if (cur_pdu_ptr[gas_id]->pg_state != PGS_CLOSE)
          {
            if (group_or_close_pdu(gas_id, next_ptr))
            {
              /* Group only if check_tx_cv_and_reprocess() had not performed PDU grouping
              */
              if( ul[gas_id].cur_res.tbf_mode == TBF_MODE_GPRS )
                pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                                (uint8)IS_TLLI_REQUIRED(gas_id));
              else
                e_pg_adjust_tbc(gas_id, &pg[gas_id].tbci, next_ptr, ul[gas_id].cur_res.cs, ul[gas_id].tlli_cs,
                                (uint8)IS_TLLI_REQUIRED(gas_id));

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_XFER
              MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_99, "GRLU pgxf add psn %u len = %u blks=%u",
                      next_ptr->psn,
                      next_ptr->len,
                      pg[gas_id].tbci.num_blks);
#endif
            } /* done grouping PDU */
          }

          /* Check out QoS of current PDU against the next PDU and initiate
          ** reallocation activities if required
          */
          check_to_send_prr(gas_id);
        } /* next PDU follows current PDU */
      } /* next PDU is available */

      /* No atttempt to group any PDUs on the queue.
      */
      group_pdus_on_queue_for_reprocessing(gas_id);
      break;


    case PGS_CLOSE:

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_100, "GRLU PGS close last_psn %u tbc=%u num_blks=%u",
                cur_pdu_ptr[gas_id]->pg_last_psn,
                ul[gas_id].tbc,
                cur_pdu_ptr[gas_id]->pg_num_blks);
#endif
      break;

    default:

      MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_101, "GRLU PGS bad state %d",cur_pdu_ptr[gas_id]->pg_state);
      break;

  } /* switch pg_state */

#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_GENERAL
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_102, "GRLU gp end tbc=%u psn=%u last_psn=%u",
          ul[gas_id].tbc,
          cur_pdu_ptr[gas_id]->psn,
          cur_pdu_ptr[gas_id]->pg_last_psn);
#endif
  show_group_status(gas_id);

} /* group_pdus_for_reprocessing() */



/*** EOF: don't remove! ***/
