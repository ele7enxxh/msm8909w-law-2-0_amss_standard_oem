/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC-UL to PL1 FIFO Module
***
***
*** DESCRIPTION
***
***  This module implements the buffer interface between RLC UL and PL1.
***
***  Internally the FIFO is divided into 3 areas each holding blocks of
***  a particular acknowledge status: Nack(NACK), Pending-transmit
***  (PTX) and Pending-ack(PACK). This is also the order in which the data
***  blocks are retrieved. Each area contains 4 data blocks making a total of
***  12 data blocks.
***
***  PL1 uses the grlc_ul_fifo_access_block() to retrieve one data block using
***  the 3 access modes: READ_MODE, GET_MODE and ABORT_MODE.
***
***  READ_MODE is used to obtain a pointer to a data block without deleting it
***  from the FIFO. The pointer returned is passed to the DSP which copies the
***  data to its transmit regsters.When the DSP confirms a block has been
***  transmitted PL1 uses GET_MODE to delete the block from the FIFO. At the end
***  of a block period, if there are outstanding read blocks (un-transmitted) PL1
***  then makes an ABORT_MODE access to reset the read pointers in the 3 areas
***  of the FIFO.
***
***  In Fixed Allocation, since blocks copied to the DSP registers are always
***  transmitted, PL1 can use GET_MODE to obtain the data blocks and deletes
***  them from the FIFO at the same time. Without going through the intermediate
***  READ_MODE access(es)
***
***  TRANSMIT PRECEDENCE
***
***  RLC is required to send data blocks in the order according to their ack
***  status. Starting with Nack, followed by Pending-transmit and Pending-ack.
***  When a Nack block is sent to the FIFO by RLC, it must be transmitted before
***  the block(s) in the existing DSP buffer. Everytime PL1 makes a read access
***  it checks compares the ack status of the new block with those in the buffers,
***  and if required re-order accordingly.
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  grlc_ul_fifo_one_time_init() must be called on startup.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlculfifo.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/21/01    hv     Created
*** 07/16/01    hv     Added refresh of Stall Indicator value prior to adding
***                    data block to FIFO.
*** 07/26/01    hv     Removed SI from param. Add grlc_ul_fifo_resync() for L1.
*** 09/18/01    hv     Added test code.
*** 11/08/01    hv     Made buffer static. Split FIFO into 3 different areas
***                    according to acknowledgement status. Added params to
***                    allow PL1 to re-order blocks according to precidence.
*** 08/03/04    hv     Added EGPRS code
*** 08/15/11    hv     Widened mutex locks for multi-thread targets
*** 26/10/12    hv     CR409255 - corruption(s) of RLC uplink FIFO
*** 08/24/12    hv     CR 378752 - correct wrong puncturing scheme
*** 08/24/12    hv     CR 390241 - correct wrong punctruing scheme part 2
*** 20/08/13    hv     CR531564
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
#include "grlcutil.h"
#include "grlculfifo.h"
#include "grlcx.h"
#include "grlctx.h"
#include "grlcmac.h"
#include "grlcultypes.h"
#include "grlcultst.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* Turn this on to dump content of data blocks just copied to the FIFO. Only
** the 1st 48 octets are dumped to QXDM.
*/
#undef  GSM_GPRS_GRLU_DUMP_BLOCK


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

/*------------------------------------------------------------------------------
** Dedicated uplink FIFO between RLC and PL1. This is owned and managed by GRLC
**------------------------------------------------------------------------------
*/
grlc_ul_pl1_fifo_t   grlc_ul_fifo[NUM_GERAN_DATA_SPACES];


/*---------------------------------------------------------------
** Critical section to protect access to GRLC UL FIFO
**---------------------------------------------------------------
*/

rex_crit_sect_type grlc_ul_fifo_crit_sec[NUM_GERAN_DATA_SPACES];


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

#ifndef GSM_GPRS_GRLU_DUMP_BLOCK
#define GRLC_UL_FIFO_DUMP_BLOCK(ptr)
#else
#define GRLC_UL_FIFO_DUMP_BLOCK(ptr)    grlc_ul_fifo_dump_block(ptr)
#endif

#ifdef GSM_GPRS_GRLU_DUMP_BLOCK

/* Dump 48 bytes of the RLC/MAC block including the MAC header, RLC header, extension
** octets if any and payload. Only the 1st 48 bytes is dumped so it covers CS-3 but
** not CS-4
*/
void grlc_ul_fifo_dump_block(const uint8 *ptr)
{
  uint16  u;
  uint32 u1,u2,u3;

  for (u1=0L, u=0; u<4; u++)
  {
    u1 <<= 8;
    u1 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u2=0L, u=0; u<4; u++)
  {
    u2 <<= 8;
    u2 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u3=0L, u=0; u<4; u++)
  {
    u3 <<= 8;
    u3 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  /* Show the 1st group of 12 bytes 0-3, 4-7, 8-11
  */
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1195, "Dump %08x %08x %08x",u1,u2,u3);

  for (u1=0L, u=0; u<4; u++)
  {
    u1 <<= 8;
    u1 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u2=0L, u=0; u<4; u++)
  {
    u2 <<= 8;
    u2 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u3=0L, u=0; u<4; u++)
  {
    u3 <<= 8;
    u3 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  /* Show the 2nd group of 12 bytes 12-15, 16-19, 20-23
  */
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1196, "Dump %08x %08x %08x",u1,u2,u3);

  for (u1=0L, u=0; u<4; u++)
  {
    u1 <<= 8;
    u1 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u2=0L, u=0; u<4; u++)
  {
    u2 <<= 8;
    u2 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u3=0L, u=0; u<4; u++)
  {
    u3 <<= 8;
    u3 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  /* Show the 3rd group 12 bytes 24-27, 28-31, 32-35
  */
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1197, "Dump %08x %08x %08x",u1,u2,u3);

  for (u1=0L, u=0; u<4; u++)
  {
    u1 <<= 8;
    u1 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u2=0L, u=0; u<4; u++)
  {
    u2 <<= 8;
    u2 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  for (u3=0L, u=0; u<4; u++)
  {
    u3 <<= 8;
    u3 |= (uint32)(((uint8)(*ptr++)) & 0xff);
  }
  /* Show the 4th group 12 bytes 36-39, 40-43, 44-47
  */
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_1198, "Dump %08x %08x %08x",u1,u2,u3);

} /* grlc_ul_fifo_dump_block */

#endif /* GSM_GPRS_GRLU_DUMP_BLOCK */

/*****************************************************************************
***
***     External Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION         GRLC_UL_FIFO_ONE_TIME_INIT
===
===  DESCRIPTION
===
===    Initialise the L1 to RLC uplink FIFO structure by clearing the in
===    and out indices and item counter to zero.
===
===    The buffers are now statically allocated which removes the need for
===    this function. However, it is kept for the time being.
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
void grlc_ul_fifo_one_time_init (gas_id_t gas_id)
{


  /* Initialise critical section protecting GRLC UL FIFO access */

  rex_init_crit_sect(&grlc_ul_fifo_crit_sec[gas_id]);


  grlc_ul_fifo_re_init(gas_id);
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_RE_INIT
===
===  DESCRIPTION
===
===    Re-initialise the rlc ul fifo to empty state leaving the array of
===    pointers to data blocks unchanged.
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===    This should be called by rlc ul task.
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
void grlc_ul_fifo_re_init (gas_id_t gas_id)
{
  GRLC_UL_FIFO_LOCK(gas_id);

  grlc_ul_fifo[gas_id].b_validity = FALSE;

  grlc_ul_fifo[gas_id].nack_in_ind = 0;
  grlc_ul_fifo[gas_id].nack_out_ind = 0;
  grlc_ul_fifo[gas_id].nack_ctr = 0;
  grlc_ul_fifo[gas_id].nack_read_out_ind = 0;
  grlc_ul_fifo[gas_id].nack_read_ctr = 0;

  grlc_ul_fifo[gas_id].ptx_in_ind = 0;
  grlc_ul_fifo[gas_id].ptx_out_ind = 0;
  grlc_ul_fifo[gas_id].ptx_ctr = 0;
  grlc_ul_fifo[gas_id].ptx_read_out_ind = 0;
  grlc_ul_fifo[gas_id].ptx_read_ctr = 0;

  grlc_ul_fifo[gas_id].pack_in_ind = 0;
  grlc_ul_fifo[gas_id].pack_out_ind = 0;
  grlc_ul_fifo[gas_id].pack_ctr = 0;
  grlc_ul_fifo[gas_id].pack_read_out_ind = 0;
  grlc_ul_fifo[gas_id].pack_read_ctr = 0;

  GRLC_UL_FIFO_UNLOCK(gas_id);

} /* grlc_ul_fifo_re_init */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in all 3 areas of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_cnt (gas_id_t gas_id)
{
  return(uint8)(grlc_ul_fifo[gas_id].nack_ctr +
                grlc_ul_fifo[gas_id].ptx_ctr +
                grlc_ul_fifo[gas_id].pack_ctr
               );
}

/*===========================================================================
===
===  FUNCTION      UL_FIFO_GET_NACK_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the Nack area of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_nack_cnt(gas_id_t gas_id)
{
  return (uint8)grlc_ul_fifo[gas_id].nack_ctr;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_PTX_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the pending-transmit
===   area of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_ptx_cnt(gas_id_t gas_id)
{
  return (uint8)grlc_ul_fifo[gas_id].ptx_ctr;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_GET_PACK_CNT
===
===  DESCRIPTION
===
===   Return the number of items currently held in the pending-ack area
===   of the FIFO
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    byte
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 grlc_ul_fifo_get_pack_cnt(gas_id_t gas_id)
{
  return (uint8)grlc_ul_fifo[gas_id].pack_ctr;
}

/*===========================================================================
===
===  FUNCTION       GRLC_UL_FIFO_IS_AREA_FULL()
===
===  DESCRIPTION
===
===   Return TRUE if the given area in the RLC/PL1 UL FIFO is full.
===   Return FALSE otherwise.
===
===  PARAMETERS
===
===   ack_state: ack status of the block before it is sent off to the FIFO.
===        The ack state is of tx_ack_state_t but only 4 values are used:
===        TXA_NACK, TXA_PTX, TXA_PACK and TXA_UNACK.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===   boolean
===
===  SIDE EFFECTS
===
===   none
===
===========================================================================*/
boolean grlc_ul_fifo_is_area_full
(
  gas_id_t              gas_id,
  const tx_ack_state_t  ack_state
)
{
  if (ack_state == TXA_NACK ||
      ack_state == TXA_PENDING_PACK
     )
  {
    if (grlc_ul_fifo[gas_id].nack_ctr >= RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE)
      return TRUE;
  }
  else if (ack_state == TXA_PTX)
  {
    if (grlc_ul_fifo[gas_id].ptx_ctr >= RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE)
      return TRUE;
  }
  else if (ack_state == TXA_PACK || ack_state == TXA_UNACK)
  {
    if (grlc_ul_fifo[gas_id].pack_ctr >= RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE)
      return TRUE;
  }
  else
  {
    MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1199, "GRLU UL_FIFO bad ack status %d", ack_state);
  }

  return FALSE;

} /* grlc_ul_fifo_is_area_full */


/*===========================================================================
===
===  FUNCTION       GRLC_UL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===   This function inserts a data block into the FIFO in area given by the
===   ack status, if that area of the FIFO is not full.
===
====  Blocks with TXA_UNACK are put in the area Pending-ack area.
===
===   This function also invokes xlate_int_ul_data() to convert the payload
===   part of the data block in DSM item into straight buffer.
===
===  PARAMETERS
===
===   *bp: pointer to grlc_ul_data_block_t which holds the coding scheme and
===        an array of octets representing the RLC/MAC block including the
===        MAC header
===
===   ack_state: ack status of the block before it is sent off to the FIFO.
===        The ack state is of tx_ack_state_t but only 4 values are used:
===        TXA_NACK, TXA_PTX, TXA_PA and TXA_UNACK.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    grlc_ul_fifo_op_result_t
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_ul_fifo_op_result_t  grlc_ul_fifo_put_block
(
  gas_id_t				      gas_id,
  grlc_ul_data_block_t  *bp,
  tx_ack_state_t        ack_state
)
{
  grlc_ul_fifo_op_result_t   res;

  /*----------------------------------------------------------*/

  /* there is no need to check for FIFO being full since this function
  ** is only called from send_block_to_pl1_fifo() which checked the count already.
  ** However, leave this check in and throw an error
  */

  /* say FULL if all 3 areas are full. This stops the higher precedence area
  ** from being blocked by a lower precedence area
  */
  if (grlc_ul_fifo_is_area_full(gas_id, ack_state))
  {
    MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1200, "GRLU FIFO_PUT Area %d full",ack_state);
    res = UL_FIFO_FULL;
  }
  else
  {
    /*=================================
    ** Nack block goes to Nack area
    **=================================
    */
    if (ack_state == TXA_NACK || ack_state == TXA_PENDING_PACK)
    {
      GRLC_UL_FIFO_LOCK(gas_id);
      xlate_int_ul_data(bp,
        (byte *)&grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.gprs.block[0]);
      grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.gprs.cs = bp->cs;
      grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].pdu_prio = bp->pdu_prio;
      GRLC_UL_FIFO_DUMP_BLOCK(
        grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.gprs.block);
      INC_MOD(grlc_ul_fifo[gas_id].nack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

      grlc_ul_fifo[gas_id].nack_ctr++;
      GRLC_UL_FIFO_UNLOCK(gas_id);

      res = UL_FIFO_PUT_OK;
    }
    else
    /*=================================
    ** PTX block goes to PTX area
    **=================================
    */
    if (ack_state == TXA_PTX)
    {
      /* pending transmit block goes to pending transmit area
      */
      GRLC_UL_FIFO_LOCK(gas_id);
      xlate_int_ul_data(bp,
        (byte *)&grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].elm.gprs.block[0]);
      grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].elm.gprs.cs = bp->cs;
      grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].pdu_prio    = bp->pdu_prio;
      GRLC_UL_FIFO_DUMP_BLOCK(
        grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].elm.gprs.block);
      INC_MOD(grlc_ul_fifo[gas_id].ptx_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

      grlc_ul_fifo[gas_id].ptx_ctr++;
      GRLC_UL_FIFO_UNLOCK(gas_id);

      res = UL_FIFO_PUT_OK;
    }
    else
    /*=================================
    ** PACK block goes to PAK area
    **=================================
    */
    if (ack_state == TXA_PACK ||
        ack_state == TXA_UNACK
       )
    {
      /* Pending-ack block goes to Pending-ack area.
      **
      ** Un-ack block also goes to Pending-ack area. This is chosen so that
      ** during un-ack operation PL1 will only ever see the TXA_PACK ack state
      ** and no swapping of blocks or aborting of operation is required.
      */
      GRLC_UL_FIFO_LOCK(gas_id);
      xlate_int_ul_data(bp,
        (byte *)&grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.gprs.block[0]);
      grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.gprs.cs = bp->cs;
      grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].pdu_prio    = bp->pdu_prio;
      GRLC_UL_FIFO_DUMP_BLOCK(
        grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.gprs.block);

      INC_MOD(grlc_ul_fifo[gas_id].pack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

      grlc_ul_fifo[gas_id].pack_ctr++;
      GRLC_UL_FIFO_UNLOCK(gas_id);

      res = UL_FIFO_PUT_OK;
    }
    else
    {
      /* Unrecognise area
      */
      res = UL_FIFO_PUT_ERROR;
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1201, "GRLC UL FIFO PUT bad status %d",ack_state);
    }
  }

  return res;

} /* grlc_ul_fifo_put_block */

/*===========================================================================
===
===  FUNCTION       E_GRLC_UL_FIFO_PUT_BLOCK()
===
===  DESCRIPTION
===
===   This function inserts a data block into the FIFO in area given by the
===   ack status, if that area of the FIFO is not full.
===
====  Blocks with TXA_UNACK are put in the area Pending-ack area.
===
===   This function also invokes xlate_int_ul_data() to convert the payload
===   part of the data block in DSM item into straight buffer.
===
===  PARAMETERS
===
===   *bp1/2: pointers to e_grlc_ul_data_block_t which hold the MCS and
===           an array of octets representing the RLC data block
===
===   ack_state: ack status of the block before it is sent off to the FIFO.
===        The ack state is of tx_ack_state_t but only 4 values are used:
===        TXA_NACK, TXA_PTX, TXA_PA and TXA_UNACK.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    grlc_ul_fifo_op_result_t
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
grlc_ul_fifo_op_result_t  e_grlc_ul_fifo_put_block
(
  gas_id_t				      gas_id,
  grlc_ul_data_block_t  *bp1,
  grlc_ul_data_block_t  *bp2,
  tx_ack_state_t        ack_state
)
{
  grlc_ul_fifo_op_result_t   res;
  uint8                      temp_fifo_ind1;

  /*----------------------------------------------------------*/

  /* there is no need to check for FIFO being full since this function
  ** is only called from send_block_to_pl1_fifo() which checked the count already.
  ** However, leave this check in and throw an error
  */

  /* say FULL if all 3 areas are full. This stops the higher precedence area
  ** from being blocked by a lower precedence area
  */
  if (grlc_ul_fifo_is_area_full(gas_id, ack_state))
  {
    MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1202, "GRLU FIFO_PUT Area %d full",ack_state);
    res = UL_FIFO_FULL;
  }
  else
  {
    /*=================================
    ** Nack block goes to Nack area
    **=================================
    */
    if (ack_state == TXA_NACK || ack_state == TXA_PENDING_PACK)
    {
      if( IS_SPLIT_REQUIRED(bp1->init_mcs, bp1->cs, ul[gas_id].cur_res.resegment) )
      {
        GRLC_UL_FIFO_LOCK(gas_id);
        /* If split is required, then two blocks will be send to FIFO in one go.
        ** Note, the NACK FIFO space has been cheacked at e_tx_blocks().
        ** This point is reached only if grlc_ul_fifo_get_nack_cnt >= 2.
        */
        temp_fifo_ind1 = grlc_ul_fifo[gas_id].nack_in_ind;
        INC_MOD(grlc_ul_fifo[gas_id].nack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        e_xlate_int_ul_data(bp1, bp2,
          &grlc_ul_fifo[gas_id].nack_blk_arr[temp_fifo_ind1].elm.egprs,
          &grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.egprs );

        grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].pdu_prio = bp1->pdu_prio;
        INC_MOD(grlc_ul_fifo[gas_id].nack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        grlc_ul_fifo[gas_id].nack_ctr += 2;
        GRLC_UL_FIFO_UNLOCK(gas_id);

        res = UL_FIFO_PUT_OK;
      }
      else
      {
        GRLC_UL_FIFO_LOCK(gas_id);
        e_xlate_int_ul_data(bp1, bp2,
          &grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.egprs, NULL);

        grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].pdu_prio =
          bp1->pdu_prio || ( (bp2 != NULL) ? bp2->pdu_prio : 0 );

        /* GRLC_UL_FIFO_DUMP_BLOCK(
        **    grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_in_ind].elm.egprs);
        */

        INC_MOD(grlc_ul_fifo[gas_id].nack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        grlc_ul_fifo[gas_id].nack_ctr++;
        GRLC_UL_FIFO_UNLOCK(gas_id);

        res = UL_FIFO_PUT_OK;
      }
    }
    else
    /*=================================
    ** PTX block goes to PTX area
    **=================================
    */
    if (ack_state == TXA_PTX)
    {
      /* pending transmit block goes to pending transmit area
      */
      GRLC_UL_FIFO_LOCK(gas_id);
      e_xlate_int_ul_data(bp1, bp2,
        &grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].elm.egprs, NULL );

      grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].pdu_prio =
       bp1->pdu_prio || ( (bp2 != NULL) ? bp2->pdu_prio : 0 );

      /* GRLC_UL_FIFO_DUMP_BLOCK(
      **    grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_in_ind].elm.egprs);
      */

      INC_MOD(grlc_ul_fifo[gas_id].ptx_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

      grlc_ul_fifo[gas_id].ptx_ctr++;
      GRLC_UL_FIFO_UNLOCK(gas_id);

      res = UL_FIFO_PUT_OK;
    }
    else
    /*=================================
    ** PACK block goes to PAK area
    **=================================
    */
    if (ack_state == TXA_PACK ||
        ack_state == TXA_UNACK
       )
    {
      /* Pending-ack block goes to Pending-ack area.
      **
      ** Un-ack block also goes to Pending-ack area. This is chosen so that
      ** during un-ack operation PL1 will only ever see the TXA_PACK ack state
      ** and no swapping of blocks or aborting of operation is required.
      */

      if( ( ack_state   == TXA_PACK  &&
            IS_SPLIT_REQUIRED(bp1->init_mcs, bp1->cs, ul[gas_id].cur_res.resegment) ))
      {
        /* If split is required, then two blocks will be send to FIFO in one go.
        ** Note, the PACK FIFO space has been cheacked at e_tx_blocks().
        ** This point is reached only if grlc_ul_fifo_get_pack_cnt >= 2.
        */
        GRLC_UL_FIFO_LOCK(gas_id);
        temp_fifo_ind1 = grlc_ul_fifo[gas_id].pack_in_ind;
        INC_MOD(grlc_ul_fifo[gas_id].pack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        e_xlate_int_ul_data(bp1, bp2,
          &grlc_ul_fifo[gas_id].pack_blk_arr[temp_fifo_ind1].elm.egprs,
          &grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.egprs );

        grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].pdu_prio = bp1->pdu_prio;

        /* GRLC_UL_FIFO_DUMP_BLOCK(
        **    grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.egprs);
        */

        INC_MOD(grlc_ul_fifo[gas_id].pack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        grlc_ul_fifo[gas_id].pack_ctr += 2;
        GRLC_UL_FIFO_UNLOCK(gas_id);

        res = UL_FIFO_PUT_OK;
      }
      else
      {
        GRLC_UL_FIFO_LOCK(gas_id);
        e_xlate_int_ul_data(bp1, bp2,
          &grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.egprs, NULL );

        grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].pdu_prio =
          bp1->pdu_prio || ( (bp2 != NULL) ? bp2->pdu_prio : 0 );

        /* GRLC_UL_FIFO_DUMP_BLOCK(
        **    grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_in_ind].elm.egprs);
        */

        INC_MOD(grlc_ul_fifo[gas_id].pack_in_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        grlc_ul_fifo[gas_id].pack_ctr++;
        GRLC_UL_FIFO_UNLOCK(gas_id);

        res = UL_FIFO_PUT_OK;
      }
    }
    else
    {
      /* Unrecognise area
      */
      res = UL_FIFO_PUT_ERROR;
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1203, "GRLC UL FIFO PUT bad status %d",ack_state);
    }
  }

  return res;

} /* e_grlc_ul_fifo_put_block */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_ACCESS_BLOCK()
===
===  DESCRIPTION
===
===    This function provides the PL1 access to the RLC-PL1 UL FIFO in three
===    modes: READ_MODE, GET_MODE and ABORT_MODE.
===
===    Retrieve a pointer to a 'straight' buffer representing an RLC/MAC block from
===    the RLC-PL1 FIFO.
===
===    There are 3 areas in the FIFO each holding a maximum of 4 blocks of a
===    particular acknowledgement status: The NACK (Negatively Ack'ed), the PTX
===    (Pending-transmit) and the PACK (Pending-ack). This is also the order in
===    which the blocks are retrieved.
===
===    In Dynamic Allocation mode, PL1 uses the READ_MODE to obtain a pointer to a
===    data block for copying into the DSP, without taking the block out of the
===    FIFO. After the DSP confirms the block has been transmitted PL1 uses
===    GET_MODE to remove the block from the FIFO. For every READ_MODE access
===    PL1 must store the associated 'ack state'. This is used later to perform
===    the GET_MODE access (GET_WITH_PREVIOUS_READ) for that block.
===
===    ! If no USF occur on the next block period PL1 can perform an ABORT_MODE
===    access to !
===
===
===    PL1 may perform more READ_MODE accesses than GET_MODE accesses. The latter
===    is limited by the USF received. To synchronise the number of 'read' and
===    'get', PL1 uses the ABORT_MODE access on every block period.
===
===    Following a GET_MODE access, post_tx_actions() is called so RLC can update
===    its transmit variables, clear out DSM resources etc..
===
===  ACCESS PROTECTION
===
===    When RLC is given context it prepares and loads the FIFO with data blocks
===    under its task time. When RLC puts in a data block the 'in_ind' and 'ctr'
===    variables change. When PL1 accesses the FIFO which always occur under
===    interrupts, the 'out_ind' and 'ctr' variables change. Therefore accesses
===    to the 'ctr' variable of each area are protected with interrupt
===    disable/enable.
===
===    During Contention Resolution period RLC needs to delete the whole FIFO and
===    loads it with TLLI-free data blocks. RLC clears the 'Validity' flag to
===    FALSE (under interrupt disable/enable) at the beginning and sets it at the end.
===    If PL1 accesses the FIFO under interrupt during this period, UL_FIFO_BUSY is
===    returned.
===
===  PARAMETERS
===
===    access_mode: READ_MODE or GET_MODE or ABORT_MODE to be specified by PL1.
===
===    get_type: If access_mode is GET_MODE then PL1 uses:
===          - GET_WITH_PREVIOUS_READ: when in Dynamic Allocation mode
===
===    blk_ptr: pointer to struct representing data block returned to PL1.
===
===    ack_state: Only a subset of tx_ack_state_t is used: TXA_NACK, TXA_PTX
===          or TXA_PACK. This value is returned to PL1 in a READ_MODE access.
===          It identifies which area the block comes from. PL1 must store this
===          value away and passes back when PL1 next performs a GET_MODE access
===          for that data block.
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===    post_tx_actions() function must be defined.
===
===  RETURN VALUE
===
===    grlc_ul_fifo_op_result_t
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===    when access mode is GET_MODE signal_tlli_block() is called.
===
===========================================================================*/
grlc_ul_fifo_op_result_t  grlc_ul_fifo_access_block
(
  gas_id_t						          gas_id,
  grlc_ul_fifo_access_block_t   *ff_ptr
)
{
  grlc_ul_fifo_op_result_t   res;

  /*----------------------------------------------------------*/

  /*------------------------------------------------------------------------
  ** All access modes are subject to checking of the Validity flag.
  */
  if (!grlc_ul_fifo[gas_id].b_validity)
  {
    static uint32 ul_fifo_busy_cnt[NUM_GERAN_DATA_SPACES];

    ul_fifo_busy_cnt[gas_id] = 0;
    MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_1204, "GRLU FIFO busy access_mode=[%d]",ff_ptr->access_mode,++ul_fifo_busy_cnt[gas_id]);
     return UL_FIFO_BUSY;
  }

  /*==============================================================
  ** Read Access Mode
  **==============================================================
  */
  if (ff_ptr->access_mode == READ_MODE)
  {
    /* Say fifo empty if the blocks have all been read out (when respective
    ** read out counters are the same as the counters of blocks in each area)
    */
    if ((grlc_ul_fifo[gas_id].nack_ctr == grlc_ul_fifo[gas_id].nack_read_ctr) &&
        (grlc_ul_fifo[gas_id].ptx_ctr == grlc_ul_fifo[gas_id].ptx_read_ctr) &&
        (grlc_ul_fifo[gas_id].pack_ctr == grlc_ul_fifo[gas_id].pack_read_ctr)
       )
    {
      res = UL_FIFO_EMPTY;
    }
    else
    /*---------------------------------------------------------------------
    ** Find a block in one of the 3 areas starting with the NACK area
    ** followed by the PTX and PAK area. When found, return the address of
    ** that block. The read out index and read counter are adjusted
    **---------------------------------------------------------------------
    */
    {
      /*---------------------------------
      ** READ_MODE: Check the NACK area
      **---------------------------------
      */
      if (grlc_ul_fifo[gas_id].nack_ctr > grlc_ul_fifo[gas_id].nack_read_ctr)
      {
        /* Return the pointer to the next block indexed by the current
        ** Read Out index
        */
        ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_read_out_ind];

        /* Update the Read Out index
        */
        INC_MOD(grlc_ul_fifo[gas_id].nack_read_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

        /* Update the read counter
        */
        grlc_ul_fifo[gas_id].nack_read_ctr++;

        /* Return the block ack state to PL1 so it can use it to request the
        ** GET_MODE access later on
        */
        ff_ptr->ack_state = TXA_NACK;

        ff_ptr->pdu_prio  = ff_ptr->blk_ptr->pdu_prio;

        /* Say access is OK
        */
        res = UL_FIFO_ACCESS_OK;
      }
      else
      /*--------------------------------
      ** READ_MODE: Check the PTX area
      **--------------------------------
      */
      if (grlc_ul_fifo[gas_id].ptx_ctr > grlc_ul_fifo[gas_id].ptx_read_ctr)
      {
        ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_read_out_ind];
        INC_MOD(grlc_ul_fifo[gas_id].ptx_read_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);
        grlc_ul_fifo[gas_id].ptx_read_ctr++;
        ff_ptr->ack_state = TXA_PTX;
        ff_ptr->pdu_prio  = ff_ptr->blk_ptr->pdu_prio;
        res = UL_FIFO_ACCESS_OK;
        if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
            ul[gas_id].ptx_cv = GET_RAW_CV(ff_ptr->blk_ptr->elm.egprs.hdr);
        else
            ul[gas_id].ptx_cv = GET_RAW_CV(ff_ptr->blk_ptr->elm.gprs.block);

        if (ul[gas_id].ptx_cv != BS_CV_CONST)
        {
          MSG_GRLC_EXT_MED_2_G(GRLC_F3_MSG_ENUM_1205, "GRLU ptx_rd ptx_cv=%u pak_cv=%u",ul[gas_id].ptx_cv,ul[gas_id].pak_cv);
        }
      }
      else
      /*---------------------------------
      ** READ_MODE: Check the PACK area
      **---------------------------------
      */
      if (grlc_ul_fifo[gas_id].pack_ctr > grlc_ul_fifo[gas_id].pack_read_ctr)
      {
        ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_read_out_ind];
        INC_MOD(grlc_ul_fifo[gas_id].pack_read_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);
        grlc_ul_fifo[gas_id].pack_read_ctr++;
        ff_ptr->ack_state = TXA_PACK;
        ff_ptr->pdu_prio  = ff_ptr->blk_ptr->pdu_prio;
        res = UL_FIFO_ACCESS_OK;
        if (ul[gas_id].rlc_mode == GRLC_MODE_UNACK)
        {
          if (ul[gas_id].cur_res.tbf_mode == TBF_MODE_EGPRS)
              ul[gas_id].pak_cv = GET_RAW_CV(ff_ptr->blk_ptr->elm.egprs.hdr);
          else
              ul[gas_id].pak_cv = GET_RAW_CV(ff_ptr->blk_ptr->elm.gprs.block);

          if (ul[gas_id].pak_cv != BS_CV_CONST)
          {
            MSG_GRLC_EXT_MED_2_G(GRLC_F3_MSG_ENUM_1206, "GRLU pak_rd pak_cv=%u ptx_cv=%u",ul[gas_id].pak_cv, ul[gas_id].ptx_cv);
          }
        }
      }
      else
      /*------------------------------------------------------------------
      ** READ_MODE: No block to read even though the check above went OK
      **------------------------------------------------------------------
      */
      {
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1207, "GRLC Read-acc no blocks");
        res = UL_FIFO_READ_ERROR;
      }
    } /* end READ_MODE not empty */
  } /* end READ_MODE */

  else

  /*==============================================================
  ** Get Access Mode
  **==============================================================
  */
  if (ff_ptr->access_mode == GET_MODE)
  {
    if (grlc_ul_fifo[gas_id].nack_ctr == 0 &&
        grlc_ul_fifo[gas_id].ptx_ctr == 0 &&
        grlc_ul_fifo[gas_id].pack_ctr == 0
       )
    {
      res = UL_FIFO_EMPTY;
    }
    else
    {
      /*------------------------------------
       ** GET_MODE (GET_WITH_PREVIOUS_READ)
       **------------------------------------
      */

      /*
      ** Use ack_state to determine the area
      */
      switch (ff_ptr->ack_state)
      {
        /*-------------------------------------
        ** NACK area (GET_WITH_PREVIOUS_READ)
        **-------------------------------------
        */
        case TXA_NACK:
        case TXA_PENDING_PACK:
          /* If a NAC block exists then return it otherwise say error
          */
          GRLC_UL_FIFO_LOCK(gas_id);
          if (grlc_ul_fifo[gas_id].nack_ctr > 0 &&
              grlc_ul_fifo[gas_id].nack_read_ctr > 0
             )
          {
            /* Return pointer to block.
            ** Adjust the Out index.
            ** Adjust the block counter.
            ** Invoke RLC hook function.
            */
            ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_out_ind];
            INC_MOD(grlc_ul_fifo[gas_id].nack_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

            /* Adjust NACK read counter
            ** ! no check for 0 here
            */
            if (grlc_ul_fifo[gas_id].nack_read_ctr > 0)
            {
              grlc_ul_fifo[gas_id].nack_read_ctr--;
            }
            else
            {
              MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1208, "GRLU dec nack_read_ctr at 0");
            }

            --grlc_ul_fifo[gas_id].nack_ctr;

            res = UL_FIFO_ACCESS_OK;
          }
          else
          {
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1209, "GRLU Get-acc: prev-rd no nack block");
            res = UL_FIFO_EMPTY;
          }
          GRLC_UL_FIFO_UNLOCK(gas_id);

          /* excute post-tx-actions outside the mutex lock
          */
          if (UL_FIFO_ACCESS_OK == res)
          {
            post_tx_actions(gas_id, ff_ptr);
          }

          break;


        /*------------------------------------
        ** PTX area (GET_WITH_PREVIOUS_READ)
        **------------------------------------
        */
        case TXA_PTX:
          GRLC_UL_FIFO_LOCK(gas_id);
          if (grlc_ul_fifo[gas_id].ptx_ctr > 0 &&
              grlc_ul_fifo[gas_id].ptx_read_ctr > 0
             )
          {
            ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_out_ind];
            INC_MOD(grlc_ul_fifo[gas_id].ptx_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);

            if (grlc_ul_fifo[gas_id].ptx_read_ctr > 0)
            {
              grlc_ul_fifo[gas_id].ptx_read_ctr--;
            }
            else
            {
              MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1210, "GRLU dec ptx_read_ctr at 0");
            }

            --grlc_ul_fifo[gas_id].ptx_ctr;

            res = UL_FIFO_ACCESS_OK;
          }
          else
          {
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1211, "GRLU Get-acc: prev-rd no ptx block");
            res = UL_FIFO_EMPTY;
          }
          GRLC_UL_FIFO_UNLOCK(gas_id);

          /* excute post-tx-actions outside the mutex lock
          */
          if (UL_FIFO_ACCESS_OK == res)
          {
            post_tx_actions(gas_id, ff_ptr);
          }
          break;

        /*-------------------------------------
        ** PACK area (GET_WITH_PREVIOUS_READ)
        **-------------------------------------
        */
        case TXA_PACK:
          GRLC_UL_FIFO_LOCK(gas_id);
          if (grlc_ul_fifo[gas_id].pack_ctr > 0 &&
              grlc_ul_fifo[gas_id].pack_read_ctr > 0
             )
          {
            ff_ptr->blk_ptr = &grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_out_ind];
            INC_MOD(grlc_ul_fifo[gas_id].pack_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);


            if (grlc_ul_fifo[gas_id].pack_read_ctr > 0)
            {
              grlc_ul_fifo[gas_id].pack_read_ctr--;
            }
            else
            {
              MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1212, "GRLU pack_read_ctr at 0");
            }

            --grlc_ul_fifo[gas_id].pack_ctr;

            res = UL_FIFO_ACCESS_OK;
          }
          else
          {
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_1213, "GRLU Get-acc: prev-rd no pak block");
            res = UL_FIFO_EMPTY;
          }
          GRLC_UL_FIFO_UNLOCK(gas_id);

          /* excute post-tx-actions outside the mutex lock
          */
          if (UL_FIFO_ACCESS_OK == res)
          {
            post_tx_actions(gas_id, ff_ptr);
          }
          break;

        default:
          res = UL_FIFO_GET_ERROR;
          MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_1214, "GRLC UL FIFO bad ack state %d",ff_ptr->ack_state);
          grlc_ul_show_fifo_vars(gas_id);
          break;
      } /* ack_state switch */
    } /* GET_MODE access not empty */
  } /* GET_MODE */

  else

  /*==============================================================
  ** Abort Access Mode
  **==============================================================
  */
  if (ff_ptr->access_mode == ABORT_MODE)
  {
    GRLC_UL_FIFO_LOCK(gas_id);
    /*------------------------------------------------
    ** Reset all read blocks by:
    ** 1. clearing all read counters
    ** 2. aligning read out indexes with out indexes
    **------------------------------------------------
    */
    grlc_ul_fifo[gas_id].nack_read_ctr = 0;
    grlc_ul_fifo[gas_id].nack_read_out_ind = grlc_ul_fifo[gas_id].nack_out_ind;

    grlc_ul_fifo[gas_id].ptx_read_ctr = 0;
    grlc_ul_fifo[gas_id].ptx_read_out_ind = grlc_ul_fifo[gas_id].ptx_out_ind;

    grlc_ul_fifo[gas_id].pack_read_ctr = 0;
    grlc_ul_fifo[gas_id].pack_read_out_ind = grlc_ul_fifo[gas_id].pack_out_ind;

    GRLC_UL_FIFO_UNLOCK(gas_id);

    res = UL_FIFO_ACCESS_OK;
  }
  else
  /*-----------------------------
  ** Access mode out of range
  **-----------------------------
  */
  {
    res = UL_FIFO_ACCESS_ERROR;
  }

  return res;

} /* grlc_ul_fifo_access_block */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_ENABLE_PL1_ACCESS()
===
===  DESCRIPTION
===
===    Function for RLC to enable PL1 access to the RLC/PL1 UL FIFO.
===    RLC calls this function at the start of a TBF or after it finishes the
===    processing of the RLC-PL1 UL FIFO
===
===  PARAMETERS
===
===    none
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_enable_pl1_access (gas_id_t gas_id)
{
  if (!grlc_ul_fifo[gas_id].b_validity)
  {
    GRLC_UL_FIFO_LOCK(gas_id);
    grlc_ul_fifo[gas_id].b_validity = TRUE;
    GRLC_UL_FIFO_UNLOCK(gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DISABLE_PL1_ACCESS()
===
===  DESCRIPTION
===
===    Function for RLC to disable PL1 access to the RLC/PL1 UL FIFO whilst
===    RLC re-processes the FIFO following receipt of a Packet Uplink Acknack
===    message
===
===  PARAMETERS
===
===    none
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_disable_pl1_access (gas_id_t gas_id)
{
  if (grlc_ul_fifo[gas_id].b_validity)
  {
    GRLC_UL_FIFO_LOCK(gas_id);
    grlc_ul_fifo[gas_id].b_validity = FALSE;
    GRLC_UL_FIFO_UNLOCK(gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_NACK_PACK_BLOCKS
===
===  DESCRIPTION
===
===    Upon receipt of a Packet Uplink Acknack message, some data blocks intended
===    for re-transmission in the PACK and NACK areas may be marked as received.
===    For those data blocks already been read by PL1 we will leave. But the
===    un-read blocks must be deleted. This function performs those functions as
===    well as winding the transmit variables (pack_vsf and nack_vsf) back to their
===    correct values.
===
===    Actions:
===
===    1. Find the BSN value of the oldest block in the un-read part of the NACK
===       area.
===    2. Delete the un-read blocks.
===    3. Rewind the 'next-to-send' (nack_vsf) pointer to the BSN value found.
===    4. Do 1-3 for the PACK area.
===
===    eg. the NACK area contains BSN3,BSN4 and BSN5 and BSN3 has already been read.
===    BSN4 is the oldest un-read block. After processing the area should only hold
===    BSN3 and the transmit variable 'nack_vsf' should point to BSN4.
===
===    Note that PL1 should retain its read/get records throughout
===
===  PARAMETERS
===
===    nack_vsf_ptr: points to ul[gas_id].nack_vsf. Updated to the BSN value of the oldest
===                  purged NACK block
===    pack_vsf_ptr: points to ul[gas_id].pack_vsf. Updated to the BSN value of the oldest
===                  purged PACK block
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    grlc_ul_fifo
===
===========================================================================*/
void grlc_ul_fifo_delete_unread_nack_pack_blocks
(
  gas_id_t gas_id,
  uint16 *nack_vsf_ptr,
  uint16 *pack_vsf_ptr
)
{
  uint16  bsn;
  uint8   temp_nack_read_out_ind;
  uint8   temp_pack_read_out_ind;
  uint8   loop_ctr;
  uint8   ps=GRLC_PS_1;
  uint8   cps=0;

/*----------------------------------------------------------------------------
  ** If there are un-read PACKs, take them back by rewinding the pack_vsf index
  ** to the BSN value of the oldest block and deleting all the un-read PACKs
  **----------------------------------------------------------------------------
  */
  if (grlc_ul_fifo[gas_id].pack_ctr > grlc_ul_fifo[gas_id].pack_read_ctr)
  {
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
    {
      /* Upon receipt of a Packet Uplink Acknack message, new NACK blocks might
      ** be created. If these blocks have corresponding PACK blocks waiting
      ** to be txed in the PACK FIFO, the PACK block need to be deleted and the
      ** PS value decremented so the NACK block will be txed with the correct PS.
      */
      loop_ctr = grlc_ul_fifo[gas_id].pack_ctr - grlc_ul_fifo[gas_id].pack_read_ctr;

      /* Index the last block for reverse scan
      */
      temp_pack_read_out_ind =
         (loop_ctr + grlc_ul_fifo[gas_id].pack_read_out_ind - 1 ) % RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE;

      while( loop_ctr )
      {
        bsn = E_GET_RAW_BSN(
              grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.hdr);

        /* Restore PS value of BSN in TX array
        */
        ps = grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.ps1;
        TX_ARR(gas_id,bsn).blk.ps = ps;

        /* Restore PS_OLD value of the same BSN
        */
        DEC_MOD_PS(grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.mcs,ps);
        TX_ARR(gas_id,bsn).blk.old_ps = ps;

        TX_ARR(gas_id,bsn).blk.old_mcs = grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.mcs;

        /* Restore PS value of the 2nd PACK blk from the PACK FIFO*/
        if(grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.mcs > RM_MCS_6 )
        {
          bsn = MOD_ESNS( bsn + E_GET_RAW_BSN2(
                          grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.hdr)
                        );

          /* Restore PS value of BSN in TX array
          */
          ps = grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.ps2;
          TX_ARR(gas_id,bsn).blk.ps = ps;

          /* Restore PS_OLD value of the same BSN
          */
          DEC_MOD_PS(grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.mcs,ps);
          TX_ARR(gas_id,bsn).blk.old_ps = ps;

          TX_ARR(gas_id,bsn).blk.old_mcs = grlc_ul_fifo[gas_id].pack_blk_arr[temp_pack_read_out_ind].elm.egprs.mcs;
        }

        DEC_MOD(temp_pack_read_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);
        loop_ctr--;
      } /* end of while loop */
    } /* end of TBF_MODE_EGPRS */

    /* pack_read_out index contains the oldest un-read PACK
    */
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
    {
      bsn = E_GET_RAW_BSN(
            grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_read_out_ind].elm.egprs.hdr);

      if (IS_GT_MOD(*pack_vsf_ptr, bsn, ul[gas_id].cur_res.ws))
      {
#ifdef  GSM_GPRS_GRLU_SHOW_FIFO_UNPUT
        MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_1215, "GRLU spit PACK %d pack_vsf was %d",bsn, *pack_vsf_ptr);
#endif
        *pack_vsf_ptr = bsn;
      }
    }
    else
    {
      bsn = GET_RAW_BSN(
            grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_read_out_ind].elm.gprs.block);

      if (IS_GT_MODULO_COMP(*pack_vsf_ptr, bsn, SNS))
      {
#ifdef  GSM_GPRS_GRLU_SHOW_FIFO_UNPUT
        MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_1216, "GRLU spit PACK %d pack_vsf was %d",bsn, *pack_vsf_ptr);
#endif
        *pack_vsf_ptr = bsn;
      }
    }

    /* Delete the un-read PACKs
    */
    grlc_ul_fifo[gas_id].pack_ctr = grlc_ul_fifo[gas_id].pack_read_ctr;
    grlc_ul_fifo[gas_id].pack_in_ind = grlc_ul_fifo[gas_id].pack_read_out_ind;
  }

  loop_ctr = 0;

  /*----------------------------------------------------------------------------
  ** If there are un-read NACKs, take them back by rewinding the pack_vsf index
  ** to the BSN value of the oldest block and deleting all the un-read NACKs
  **----------------------------------------------------------------------------
  */

  if( grlc_ul_fifo[gas_id].nack_ctr > grlc_ul_fifo[gas_id].nack_read_ctr )
  {
    /* nack_read_out index contains the oldest un-read NACK
    */
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
      bsn = E_GET_RAW_BSN(
              grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_read_out_ind].elm.egprs.hdr);
    else
      bsn = GET_RAW_BSN(
              grlc_ul_fifo[gas_id].nack_blk_arr[grlc_ul_fifo[gas_id].nack_read_out_ind].elm.gprs.block);

#ifdef  GSM_GPRS_GRLU_SHOW_FIFO_UNPUT
    MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_1217, "ERLU spit NACK %d nack_vsf was %d",bsn, *nack_vsf_ptr);
#endif

    /* Set NACK vsf to the oldest bsn in the FIFO */
    *nack_vsf_ptr = bsn;
  }

  loop_ctr = grlc_ul_fifo[gas_id].nack_ctr - grlc_ul_fifo[gas_id].nack_read_ctr;

  /* Index the last block for reverse scan
  */
  temp_nack_read_out_ind =
    (grlc_ul_fifo[gas_id].nack_read_out_ind + loop_ctr - 1 ) % RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE;

  while( loop_ctr )
  {
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
    {
      bsn = E_GET_RAW_BSN(
        grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.hdr);

      GRLC_UL_TX_VARS_LOCK(gas_id);
      if( TX_ARR(gas_id,bsn).vb == TXA_PENDING_PACK )
      {
        TX_ARR(gas_id,bsn).vb = TXA_NACK;
      }
      else if( TX_ARR(gas_id,bsn).vb != TXA_NACK )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1218, "ERLU BSN %u vb=%u not TXA_PENDING_PACK",bsn, TX_ARR(gas_id,bsn).vb);
      }
      GRLC_UL_TX_VARS_UNLOCK(gas_id);

      /* Restore PS value of BSN in TX array
      */
      ps = grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.ps1;

      /* PS revertion needs to be done for first part of Block BSN only.
      */
      if(TX_ARR(gas_id,bsn).blk.mcs_demoted_ps)
      {
        /* get back the PS of the original mcs block.
        **
        */
        if(grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs == RM_MCS_5)
        {
          TX_ARR(gas_id,bsn).blk.cs = RM_MCS_7;
        }
        else if(grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs == RM_MCS_6)
        {
          /* if the block is padded , old mcs mcs 8 , cps will be 2 or 3
          */
          cps = (uint8)E_GET_RAW_CPS_HDR2(grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.hdr);
          if(cps == 2 || cps == 3)
          {
            TX_ARR(gas_id,bsn).blk.cs = RM_MCS_8;
          }
          else
          {
            TX_ARR(gas_id,bsn).blk.cs = RM_MCS_9;
          }
        }
        /* -1 not needed as enum for PS_1 is 1 , 0 is a NO_MCS_DEMOTION
        ** new_ps is old_ps+1 , and mcs_demoted_ps is already incremented.
        ** means assiging enum PS_1 = 1 , as assignig ps as PS2.
        */
        ps = TX_ARR(gas_id,bsn).blk.mcs_demoted_ps % 3;
        TX_ARR(gas_id,bsn).blk.mcs_demoted_ps = NO_MCS_DEMOTION;
      }

      TX_ARR(gas_id,bsn).blk.ps = ps;

      /* Restore PS_OLD value of the same BSN
      */
      DEC_MOD_PS(grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs,ps);
      TX_ARR(gas_id,bsn).blk.old_ps = ps;

      TX_ARR(gas_id,bsn).blk.old_mcs = grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs;

      if( grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs > RM_MCS_6 )
      {
        bsn = MOD_ESNS( bsn + E_GET_RAW_BSN2(
                        grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.hdr)
                      );

        GRLC_UL_TX_VARS_LOCK(gas_id);
        if( TX_ARR(gas_id,bsn).vb == TXA_PENDING_PACK )
        {
          TX_ARR(gas_id,bsn).vb = TXA_NACK;
        }
        else if( TX_ARR(gas_id,bsn).vb != TXA_NACK )
        {
          MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1219, "ERLU 2nd BSN %d vb=%u not TXA_PENDING_PACK", bsn, TX_ARR(gas_id, bsn).vb);
        }
        GRLC_UL_TX_VARS_UNLOCK(gas_id);

        /* Restore PS value of BSN in TX array
        */
        ps = grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.ps2;
        TX_ARR(gas_id,bsn).blk.ps = ps;

        /* Restore PS_OLD value of the same BSN
        */
        DEC_MOD_PS(grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs,ps);
        TX_ARR(gas_id,bsn).blk.old_ps = ps;

        TX_ARR(gas_id,bsn).blk.old_mcs = grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.egprs.mcs;
      }
    }
    else
    {
      bsn = GET_RAW_BSN(
        grlc_ul_fifo[gas_id].nack_blk_arr[temp_nack_read_out_ind].elm.gprs.block);

      GRLC_UL_TX_VARS_LOCK(gas_id);
      if( TX_ARR(gas_id,bsn).vb == TXA_PENDING_PACK )
      {
        TX_ARR(gas_id,bsn).vb = TXA_NACK;
      }
      else if( TX_ARR(gas_id,bsn).vb != TXA_NACK )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1220, "GRLU BSN %d vn=%u not TXA_PENDING_PACK", bsn, TX_ARR(gas_id, bsn).vb);
      }
      GRLC_UL_TX_VARS_UNLOCK(gas_id);
    }

    DEC_MOD(temp_nack_read_out_ind, RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE);
    loop_ctr--;
    grlc_ul_fifo[gas_id].nack_ctr--;
  }

  if( grlc_ul_fifo[gas_id].nack_ctr != grlc_ul_fifo[gas_id].nack_read_ctr )
  {
    MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1221, "ERLU NACK force nack_ctr %d to nack_read_ctr %d",
      grlc_ul_fifo[gas_id].nack_ctr,grlc_ul_fifo[gas_id].nack_read_ctr);

    grlc_ul_fifo[gas_id].nack_ctr = grlc_ul_fifo[gas_id].nack_read_ctr;
  }

  grlc_ul_fifo[gas_id].nack_in_ind = grlc_ul_fifo[gas_id].nack_read_out_ind;
  grlc_ul_fifo[gas_id].nack_ctr = grlc_ul_fifo[gas_id].nack_read_ctr;

} /* grlc_ul_fifo_delete_unread_nack_pack_blocks */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_PTX_BLOCKS()
===
===  DESCRIPTION
===
===    If the PTX contains any un-read blocks then delete them and return the
===    BSN of the oldest un-read block in bsn_ptr and return TRUE. Otherwise
===    return FALSE
===
===  PARAMETERS
===
===    bsn_ptr: points to the variable holding the result
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    TRUE if any un-read PTX blocks are deleted. FALSE otherwise
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_ul_fifo_delete_unread_ptx_blocks
(
  gas_id_t  gas_id,
  uint16    *bsn_ptr
)
{
  boolean b_ret = FALSE;

  if (grlc_ul_fifo[gas_id].ptx_ctr > grlc_ul_fifo[gas_id].ptx_read_ctr)
  {
    /* ctr greater than read_ctr indicates there are unread blocks. So, return
    ** the BSN of the block to be read out next which contains the 1st un-read
    ** block
    */
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
      *bsn_ptr = E_GET_RAW_BSN(
                grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_read_out_ind].elm.egprs.hdr);
    else
      *bsn_ptr = GET_RAW_BSN(
                grlc_ul_fifo[gas_id].ptx_blk_arr[grlc_ul_fifo[gas_id].ptx_read_out_ind].elm.gprs.block);

    #if 0
    /* Check if BSN is out of range, ie. it is OLD and before VS or even VA (CR300566)
    */
    if ( ( (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS) &&
           IS_GT_MOD((ul[gas_id].vs), (*bsn_ptr), (ul[gas_id].cur_res.ws))
         )
           ||
         ( (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_GPRS) &&
           IS_GT_MODULO_COMP((ul[gas_id].vs), (*bsn_ptr), (SNS))
         )
       )
    {
      /* BSN is old and before VS or VA
      */
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_1222, "GRLC UL FIFO bsn(%u) OOR before VS(%u)/VA(%u)",*bsn_ptr, ul[gas_id].vs, ul[gas_id].va);
      grlc_ul_show_fifo_vars(gas_id);

      /* Take it back to VS */
      *bsn_ptr = ul[gas_id].vs;

      /* This happens because ptx_out_ind was wrong (before VS) so fix  ptx_out_ind too.
      ** Assuming ptx_ctr and ptx_in_ind are OK, restore ptx_out_ind to the difference
      ** between ptx_in_ind and ptx_ctr
      */
      grlc_ul_fifo[gas_id].ptx_out_ind = grlc_ul_fifo[gas_id].ptx_in_ind +
                                  RLC_PL1_UL_INDIVIDUAL_FIFO_SIZE -
                                  grlc_ul_fifo[gas_id].ptx_ctr;
      MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1223, "GRLC UL FIFO corrected bsn(vs=%u) ptx_out_ind=%u = ptx_in-ptx_ctr",*bsn_ptr, grlc_ul_fifo[gas_id].ptx_out_ind);
    } /* end BSN out of range */
    #endif

    /* Delete the unread block by reducing the ctr to the read-counter
    */
    grlc_ul_fifo[gas_id].ptx_ctr = grlc_ul_fifo[gas_id].ptx_read_ctr;

    /* and bring back the ptx_in_ind
    */
    grlc_ul_fifo[gas_id].ptx_in_ind = grlc_ul_fifo[gas_id].ptx_read_out_ind;

    b_ret = TRUE;
  }

  return b_ret;

} /* grlc_ul_fifo_delete_unread_ptx_blocks */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_FIFO_DELETE_UNREAD_PACK_BLOCKS()
===
===  DESCRIPTION
===
===    If the PACK contains any un-read blocks then delete them and return the
===    BSN of the oldest un-read block in bsn_ptr and return TRUE. Otherwise
===    return FALSE
===
===  PARAMETERS
===
===    bsn_ptr: points to the variable holding the result
===
===  DEPENDENCIES
===
===    grlc_ul_fifo must be initialised and ready.
===
===  RETURN VALUE
===
===    TRUE if any un-read PACK blocks are deleted. FALSE otherwise
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean grlc_ul_fifo_delete_unread_pack_blocks
(
  gas_id_t gas_id,
  uint16 *bsn_ptr
)
{
  boolean b_ret = FALSE;

  if (grlc_ul_fifo[gas_id].pack_ctr > grlc_ul_fifo[gas_id].pack_read_ctr)
  {
    /* ctr greater than read_ctr indicates there are unread blocks. So, return
    ** the BSN of the block to be read out next which contains the 1st un-read
    ** block
    */
    if (grlc_ul_fifo[gas_id].tbf_mode == TBF_MODE_EGPRS)
      *bsn_ptr = E_GET_RAW_BSN(
            grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_read_out_ind].elm.egprs.hdr);
    else
      *bsn_ptr = GET_RAW_BSN(
            grlc_ul_fifo[gas_id].pack_blk_arr[grlc_ul_fifo[gas_id].pack_read_out_ind].elm.gprs.block);

    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_1224, "ERLU del-unread-pak bsn=%u pak_ctr=%u pak_read_ctr=%u",
		*bsn_ptr,
		grlc_ul_fifo[gas_id].pack_ctr,
		grlc_ul_fifo[gas_id].pack_read_ctr);

    /* Delete the unread block by reducing the ctr to the read-counter
    */
    grlc_ul_fifo[gas_id].pack_ctr = grlc_ul_fifo[gas_id].pack_read_ctr;

    /* And wind back the pack_in_ind
    */
    grlc_ul_fifo[gas_id].pack_in_ind = grlc_ul_fifo[gas_id].pack_read_out_ind;

    b_ret = TRUE;
  }

  return b_ret;

} /* grlc_ul_fifo_delete_unread_pack_blocks */


/*** EOF: don't remove! ***/
