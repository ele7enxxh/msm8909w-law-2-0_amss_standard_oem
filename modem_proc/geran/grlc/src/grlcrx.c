/*****************************************************************************
***
*** TITLE
***
***  GPRS Radio Link Control Receive Module
***
*** DESCRIPTION
***
***  This module contains all the receive processing functions
***
***
*** EXTERNALIZED FUNCTIONS
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcrx.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/08/13    hv     CR531564
*** 22/07/13    hv     CR515909
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
*** 11/09/06    hv     Created
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
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_msgs.h"
#include "gl1rlccmn.h"

#ifdef DEBUG_GSM_EGPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST */

#include "grlcdl.h"
#include "rex.h"
#include "gprs_mem.h"
#include "gmacrlc.h"
#include "grlcmac.h"
#include "grlcx.h"
#include "grlcutil.h"
#include "grlctimer.h"
#include "grlcdlfifo.h"
#include "gbitutil.h"
#include "grlcdiag.h"
#include "grlclog.h"
#include "grlcdli.h"
#include "grlct4encode.h"
#include "grlcl1.h"
#include "grlctsk.h"
#include "grlcsgm.h" /* definition of ul cur_dlk needed for TEST_MODE */
#include "grlcdltst.h"
#include "grlcasm.h"
#include "gmutex.h"
#include "geran_eng_mode_info.h"
#include "geran_tasks.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/

/*-----------------------------------------------------------------------
** Pointer to an arrray of structures, each representing a downlink
** GPRS/EGPRS data block
** p_rx_arr now becomes an array of (2) pointers to an array of structure
**-----------------------------------------------------------------------
*/
grlc_dl_data_block_t  (*(p_rx_arr[NUM_GERAN_DATA_SPACES]))[];

/*-------------------------------------------
** current downlink data block for gprs and egprs.
** Temporary downlink data block workspace.
**-------------------------------------------
*/
grlc_dl_data_block_t     cur_dl_blk[NUM_GERAN_DATA_SPACES];

/* GPRS MAC header octet
*/
mac_hdr_t                cur_mac_hdr[NUM_GERAN_DATA_SPACES];

/* EGPRS RLC Header
*/
e_grlc_dl_header_t       e_cur_hdr[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_GPRS_PS_HANDOVER
/*-----------------------------------------------
** EGPRS temporary downlink data block for PSHO
**-----------------------------------------------
*/
dl_radio_block_psho_t dl_psho_blind_arr[NUM_GERAN_DATA_SPACES][DL_BLIND_DATA_BLKS];

#endif /* FEATURE_GPRS_PS_HANDOVER */

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*-----------------------------------------------------------------------
** Pointer to the DSM item representing the portable layer 1 radio block
** This is used to retrieve a block from the PL1/RLC-DL FIFO
**-----------------------------------------------------------------------
*/
pl1_radio_block_header_t *pl1_blk_hdr_ptr[NUM_GERAN_DATA_SPACES];
dsm_item_type            *pl1_blk_dsm_ptr[NUM_GERAN_DATA_SPACES];

/*-----------------------------------------------------------------------
** Pointer to a EGPRS portable layer 1 radio block.
** This is used to retrieve a block from the PL1/RLC-DL FIFO
**-----------------------------------------------------------------------
*/
e_pl1_radio_block_t   e_pl1_dl_blk[NUM_GERAN_DATA_SPACES];

/********************************************************************
**
**    Forward Declarations
**
*********************************************************************
*/

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      E_CREATE_DL_FILLER_BLOCK
===
===  DESCRIPTION
===
===    Create a blank filler downlink data block. The payload contains
===    filler octets with the full data block length as per the current
===    coding scheme less the 2 octets RLC header. The block has no
===    extension octet and its FBI field is set to NOT_FINAL.
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
static void e_create_dl_filler_block
(
  gas_id_t                gas_id,
  grlc_dl_data_block_t    *blk_ptr,
  uint16                  bsn
)
{
  /* temp buffer to create filled block. No longer used after DSM item is
  ** created
  */
  uint8   buf[E_GRLC_DATA_BLOCK_MAX_LEN];

  /* the grlc_dl_e_assemble() function needs the BSN and FBI fields. The FBI should
  ** have a NOT_FINAL value because it is a filled block, hence it is
  ** behind the current block which means it can not be final.
  ** The EXT field should have a NO_OCTET_FOLLOWS value since there is no
  ** extension octet. The data block length is the full length according to
  ** the coding scheme less the RLC header.
  */
  blk_ptr->bsn = bsn;
  blk_ptr->fbi = RM_FBI_NOT_FINAL;
  blk_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;

  blk_ptr->num_ext_octets = 0;
  blk_ptr->len = (uint8)E_GRLC_DATA_BLOCK_LEN[dl[gas_id].cs];

  /* fills temp buffer with filler octet
  */
  (void)memset(buf, RM_DL_FILLER_OCTET, blk_ptr->len);

  /* convert buffer to DSM item
  */
  blk_ptr->dsm_payload_ptr = grlc_pdu_construct(buf, (uint16)blk_ptr->len);

} /* e_create_dl_filler_block */

/*===========================================================================
===
===  FUNCTION      E_VALIDATE_DL_PL1_BLOCK()
===
===  DESCRIPTION
===
===   Valitade PL1 block read from RLC<=PL1 DL FIFO.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
static void e_validate_dl_pl1_block( gas_id_t gas_id, boolean ack_mode )
{
  if( ack_mode )
  {
    if( ((e_pl1_dl_blk[gas_id].crc_1 == 0) && (e_pl1_dl_blk[gas_id].payload_1_ptr != NULL)) ||
        ((e_pl1_dl_blk[gas_id].crc_1 == 1) && (e_pl1_dl_blk[gas_id].payload_1_ptr == NULL))    )
    {
      /* 1st payload CRC status does not match payload pointer status.
      ** This is invalid in RLC ACK mode.
      */
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_137, "ERLD Rcved Invalid 1st payload with CRC=%d bsn1=%03x [%d]",
                e_pl1_dl_blk[gas_id].crc_1,
                E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk[gas_id].hdr_arr[0])),
                e_pl1_dl_blk[gas_id].frame_number);

      if( e_pl1_dl_blk[gas_id].payload_1_ptr != NULL )
      {
        GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_1_ptr);
        e_pl1_dl_blk[gas_id].payload_1_ptr = NULL;
      }
    }
/* Disabling this to remove excessive F3s. payload_1_ptr will always be NULL in case of CRC failure.
* Lower layers have enough logging from where we can come to know if CRC is failed. */
#if 0
    else if( (e_pl1_dl_blk[gas_id].crc_1 == 0) && (e_pl1_dl_blk[gas_id].payload_1_ptr == NULL) )
    {
      MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_138, "ERLD Rcved 1st payload with CRC=0 & dsm_ptr=NULL bsn1=%03x [%d]",
                E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk[gas_id].hdr_arr[0])),
                e_pl1_dl_blk[gas_id].frame_number,
                0);
    }
#endif

    if( ((e_pl1_dl_blk[gas_id].crc_2 == 0) && (e_pl1_dl_blk[gas_id].payload_2_ptr != NULL)) ||
        ((e_pl1_dl_blk[gas_id].crc_2 == 1) && (e_pl1_dl_blk[gas_id].payload_2_ptr == NULL))   )
    {
      /* 2nd payload CRC status does not match payload pointer status.
      ** This is invalid in RLC ACK mode.
      */
      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_139, "ERLD Rcved Invalid 2nd payload with CRC=%d & dsm_ptr=0x bsn2=%03x [%d]",
                     e_pl1_dl_blk[gas_id].crc_2,
                     MOD_ESNS(E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk[gas_id].hdr_arr[0])) +
                         E_GET_RAW_DL_BSN2(&(e_pl1_dl_blk[gas_id].hdr_arr[0])) ),
                     e_pl1_dl_blk[gas_id].frame_number
               );

      if( e_pl1_dl_blk[gas_id].payload_2_ptr != NULL )
      {
        GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_2_ptr );
        e_pl1_dl_blk[gas_id].payload_2_ptr = NULL;
      }
    }
/* Disabling this to remove excessive F3s. payload_2_ptr will always be NULL in case of CRC failure.
 * Lower layers have enough logging from where we can come to know if CRC is failed. */
#if 0
    else if( (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6) &&
             (e_pl1_dl_blk[gas_id].crc_2 == 0) &&
             (e_pl1_dl_blk[gas_id].payload_2_ptr == NULL)
           )
    {
      MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_140, "ERLD Rcved 2nd payload with CRC=0 & dsm_ptr= NULL bsn2=%03x [%d]",
                MOD_ESNS(E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk[gas_id].hdr_arr[0])) +
                         E_GET_RAW_DL_BSN2(&(e_pl1_dl_blk[gas_id].hdr_arr[0]))),
                e_pl1_dl_blk[gas_id].frame_number, 0
              );
    }
#endif

    if( (e_pl1_dl_blk[gas_id].mcs <= RM_MCS_6) && (e_pl1_dl_blk[gas_id].payload_2_ptr!= NULL) )
    {
      /* Single payload MCS with valid 2nd payload pointer! Release 2nd payload */
      GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_2_ptr );
      e_pl1_dl_blk[gas_id].payload_2_ptr = NULL;

      MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_141, "ERLD Rcved MCS=%d with valid 2nd payload [%d]",
                  e_pl1_dl_blk[gas_id].mcs,
                  e_pl1_dl_blk[gas_id].frame_number,
                  0);
    }
  }
  else  /* RLC UNACK mode */
  {
    if( (e_pl1_dl_blk[gas_id].crc_1 == 1) && (e_pl1_dl_blk[gas_id].payload_1_ptr == NULL) )
    {
      /* 1st payload CRC passed but payload pointer is NULL.
      */
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_142, "ERLD U Rcved Invalid 1st payload with CRC=1 & dsm_ptr= NULL [%d]",
                  e_pl1_dl_blk[gas_id].frame_number);

    }
    else if( (e_pl1_dl_blk[gas_id].crc_1 == 0) && (e_pl1_dl_blk[gas_id].payload_1_ptr == NULL) )
    {
      MSG_GRLC_EXT_MED_1_G(GRLC_F3_MSG_ENUM_143, "ERLD U Rcved 1st payload with CRC=0 & dsm_ptr= NULL [%d]",
                e_pl1_dl_blk[gas_id].frame_number);
    }

    if( (e_pl1_dl_blk[gas_id].crc_2 == 1) && (e_pl1_dl_blk[gas_id].payload_2_ptr == NULL) )
    {
      /* 2nd payload payload CRC passed but payload pointer is NULL.
      */
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_144, "ERLD U Rcved Invalid 2nd payload with CRC=1 & dsm_ptr=NULL [%d]",
                  e_pl1_dl_blk[gas_id].frame_number);
    }
    else if( (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6) &&
             (e_pl1_dl_blk[gas_id].crc_2 == 0) &&
             (e_pl1_dl_blk[gas_id].payload_2_ptr == NULL)
           )
    {
      MSG_GRLC_EXT_MED_1_G(GRLC_F3_MSG_ENUM_145, "ERLD U Rcved 2nd payload with CRC=0 & dsm_ptr= NULL [%d]",
                e_pl1_dl_blk[gas_id].frame_number);
    }

    if( (e_pl1_dl_blk[gas_id].mcs <= RM_MCS_6) && (e_pl1_dl_blk[gas_id].payload_2_ptr!= NULL) )
    {
      /* Single payload MCS with valid 2nd payload pointer! Release 2nd payload */
      GPRS_PDU_FREE( &e_pl1_dl_blk[gas_id].payload_2_ptr );
      e_pl1_dl_blk[gas_id].payload_2_ptr = NULL;

      MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_146, "ERLD U Rcved MCS=%d with valid 2nd payload [%d]",
                  e_pl1_dl_blk[gas_id].mcs, e_pl1_dl_blk[gas_id].frame_number);
    }
  }
} /* e_validate_dl_pl1_block() */

/*===========================================================================
===
===  FUNCTION     PROCESS_DL_ACK_BLOCK()
===
===  DESCRIPTION
===
===    If BSN is within the valid rx window then attach the block to the
===    rx array and update V(R), V(N) and SSN. If the block is outside the
===    rx window then mark it invalid and free the DSM memory.
===
===    In the second stage the BSN is checked and if it co-incides with
===    V(Q) then V(Q) is updated and all continuous blocks are assembled
===    and appended to the current LLC PDU.
===
===    In the final stage, if FBI had been received and all blocks have
===    been received then the LLC PDU is sent off to LLC.
===
===    input: global dl (dl_ass, vr, vq, bsn), dl[gas_id].bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void process_dl_ack_block ( gas_id_t gas_id )
{
  uint8     b, ind;     /* loop count */
  uint8     temp_vq;     /* temporary value of V(Q) */

  /*----------------------------------------------------------*/

  /* Data block in cur_dl_blk[gas_id] has just taken out of the FIFO. Check the BSN
  ** of the data block and if it is within the rx window then attach the
  ** block to the rx array and update VN.
  **
  ** Also update the V(R) in the process. BSN is valid if:
  ** [V(R) <= BSN' < V(Q)+WS] mod SNS is TRUE. However, due to the way we
  ** implement our modulo comparison function, the expression is modifed to:
  ** [V(R) <= BSN' <= (V(Q)+WS-1)] mod SNS.
  */
  if (IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl[gas_id].bsn, WS))
  {
    /* put block on dl queue
    */
    RX_ARR(gas_id, dl[gas_id].bsn) = cur_dl_blk[gas_id];

    /* pointer to payload in DSM item is considered transferred
    */
    cur_dl_blk[gas_id].dsm_payload_ptr = NULL;

    /* Update the VN array
    */
    RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED;

    /* Update V(R)
    */
    if (IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vr, dl[gas_id].bsn, dl[gas_id].vq, RM_RLC_WINDOW_SIZE))
    {
      /* Clear the blocks between the old VR (inc VR) and BSN-1 but only if VR != BSN
      */
      if (dl[gas_id].vr != dl[gas_id].bsn)
      {
        clear_receive_status(gas_id, dl[gas_id].vr, dl[gas_id].bsn);
      }

      dl[gas_id].vr = (uint8)MOD_SNS(dl[gas_id].bsn + 1);
      UPD_DL_VR(gas_id);

      /* Update SSN as well
      */
      dl[gas_id].ssn = dl[gas_id].vr;
      UPD_DL_SSN(gas_id);
    }
  }
  else
  /* Data block is not in the receive window. Now check to see the BSN of
  ** is outside the VN window range which is relative to V(R). If, so
  ** mark that VN element RXS_INVALID
  **
  ** This operation is somewhat redundant but is implemented in case the
  ** meaning is overlooked.
  */
  {
    if (IS_VR_LE_BSN_LE_VR_MINUS_K(dl[gas_id].vr, dl[gas_id].bsn, RM_RLC_WINDOW_SIZE))
    {
      RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;
    }

    /* if the block is ignored, we must release the DSM item created by PL1
    ** here
    */
    GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
    cur_dl_blk[gas_id].dsm_payload_ptr = NULL;

  }

  /*----------------------------------------------------------------------*/

  /* Update V(Q). Only if BSN hits the start of the queue (vq).
  ** If V(Q) moves then assemble the block into the current PDU
  */
  if (dl[gas_id].vq == dl[gas_id].bsn)
  {
    /* Set V(Q) to the value of the BSN within the window that
    ** has not yet been properly received and which minimises
    ** the expression [BSN - V(R)] mod SNS.
    **
    ** The expression above means the NEW value of V(Q) will be
    ** nearest to the old value of V(Q). When searching from
    ** V(Q)+1 towards V(R) the first BSN with a status not being
    ** RXS_RECEIVED item shall be assigned to V(Q).
    ** If there is no item whose V(N) value is RXS_RECEIVED then
    ** assign V(R) to V(Q).
    **
    ** When V(Q) hops along the contiguous blocks are assembled into PDU(s)
    **
    ** The case where V(Q) = V(R) before the search starts will
    ** not happen because when V(R) = V(Q) = 0 and BSN = 0,
    ** V(R) gets incremented and is 1 higher than V(Q).
    */

    /* When V(Q) moves the data block must be assembled into the current PDU
    */
    grlc_dl_assemble(gas_id, &RX_ARR(gas_id, dl[gas_id].vq));
    RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED_ASSEMBLED;

    /* Advance V(Q). Now loop from the new value of V(Q) to the new value
    ** of V(R) to seek a contiguous block with a RXS_RECEIVED state.
    ** If found the
    */
    dl[gas_id].vq = (uint8)MOD_SNS(dl[gas_id].vq+1);
    UPD_DL_VQ(gas_id);

    /* Use a copy of V(Q) since V(Q) gets altered in the loop
    */
    temp_vq = (uint8)dl[gas_id].vq;

    for (b=0; b<MOD_DIF(dl[gas_id].vr, temp_vq, SNS); b++)
    {
      ind = (uint8)MOD_SNS(b + temp_vq);
      if (RX_ARR(gas_id, ind).vn == RXS_RECEIVED)
      {
        /* found another contiguous block with RXS_RECEIVED status.
        ** So, assemble it and advance V(Q)
        */
        grlc_dl_assemble(gas_id, &RX_ARR(gas_id, ind));
        dl[gas_id].vq = (uint8)MOD_SNS(ind + 1);
        UPD_DL_VQ(gas_id);

        RX_ARR(gas_id, ind).vn = RXS_RECEIVED_ASSEMBLED;
      }
      else
      {
        /* break out of loop and do not alter V(Q) when the first RXS_INVALID
        ** block is met
        */
        break;
      }
    } /* end search V(Q) loop */
  }
} /* process_dl_ack_block */
/*===========================================================================
===
===  FUNCTION     E_PROCESS_DL_ACK_BLOCK()
===
===  DESCRIPTION
===
===    If BSN is within the valid rx window then attach the block to the
===    rx array and update V(R), V(N) and SSN. If the block is outside the
===    rx window then mark it invalid and free the DSM memory.
===
===    In the second stage the BSN is checked and if it co-incides with
===    V(Q) then V(Q) is updated and all continuous blocks are assembled
===    and appended to the current LLC PDU.
===
===    In the final stage, if FBI had been received and all blocks have
===    been received then the LLC PDU is sent off to LLC.
===
===    input: global dl (dl_ass, vr, vq, bsn), dl[gas_id].bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void e_process_dl_ack_block ( gas_id_t gas_id )
{

  uint16     b, ind;     /* loop count */
  uint16     temp_vq;     /* temporary value of V(Q) */

  /*----------------------------------------------------------*/

  /* Data block in cur_dl_blk[gas_id] has just taken out of the FIFO. Check the BSN
  ** of the data block and if it is within the rx window then attach the
  ** block to the rx array and update VN.
  **
  ** Also update the V(R) in the process. BSN is valid if:
  ** [V(R) <= BSN' < V(Q)+WS] mod SNS is TRUE. However, due to the way we
  ** implement our modulo comparison function, the expression is modifed to:
  ** [V(R) <= BSN' <= (V(Q)+WS-1)] mod SNS.
  */
  if (EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl[gas_id].bsn, dl[gas_id].ws) )
  {
    /* Is it a split data block */
    if( IS_BLOCK_SPLIT(gas_id) )
    {
      /* Check if both splits have been rcved */
      if( ( (cur_dl_blk[gas_id].spb        == SPB_RETX_PART_1) &&
            (E_RX_ARR(gas_id, dl[gas_id].bsn).spb  == SPB_RETX_PART_2)
          ) ||
          ( (cur_dl_blk[gas_id].spb        == SPB_RETX_PART_2) &&
            (E_RX_ARR(gas_id, dl[gas_id].bsn).spb  == SPB_RETX_PART_1)
          )
        )
      {
        /* Both parts of the split block have been rcved.
        */

        /* Form the original payload by combining the 2 parts
        ** and proceed with processing the block
        */
        if( cur_dl_blk[gas_id].spb == SPB_RETX_PART_2 )
        {
          gprs_pdu_augment_tail( &(E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr),
                                 &(cur_dl_blk[gas_id].dsm_payload_ptr) );

          /* Delink pointer. The LI/E of the first split will
          ** be used for re-assembly
          */
          cur_dl_blk[gas_id].dsm_payload_ptr = NULL;
        }
        else
        {
          gprs_pdu_augment_tail( &(cur_dl_blk[gas_id].dsm_payload_ptr),
                                 &(E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr) );
        }

        MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_147, "ERLD Rcved both splits for bsn=%d",dl[gas_id].bsn);
      }
      else if( E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr == NULL )
      {
        /* One part of the split block is rcved. Put it on dl queue
        */
        E_RX_ARR(gas_id, dl[gas_id].bsn)    = cur_dl_blk[gas_id];

        E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;

        /* pointer to payload in DSM item is considered transferred
        */
        cur_dl_blk[gas_id].dsm_payload_ptr = NULL;

        /* Do not process the dl block. Only one part has been recved
        */
        return;
      }
      else
      {
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_148, "ERLD Rcved duplicate split bsn=%d, rcved_spb=%d, spb=%d",
          dl[gas_id].bsn, cur_dl_blk[gas_id].spb, E_RX_ARR(gas_id, dl[gas_id].bsn).spb );

        /* Free DSM item */
        GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
        cur_dl_blk[gas_id].dsm_payload_ptr = NULL;

        /* Do not process the dl block. Only one part has been recved
        */
        return;
      }
    }

    /* Some NW send block as split PART 1 and do not send PART 2 but rather
    ** send the complete block. so the previous stored split block require
    ** freeing.
    */
    if ((E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr != NULL) &&
        (
          ((E_RX_ARR(gas_id, dl[gas_id].bsn).spb == SPB_RETX_PART_1) &&
           (cur_dl_blk[gas_id].spb       != SPB_RETX_PART_2)
          )
           ||
          ((E_RX_ARR(gas_id, dl[gas_id].bsn).spb == SPB_RETX_PART_2) &&
           (cur_dl_blk[gas_id].spb       != SPB_RETX_PART_1)
          )
        )
       )
    {
      /* Free DSM item */
      GPRS_PDU_FREE(&(E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr));
      E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr = NULL;
      MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_149, "ERLD partial split block dsm freed bsn=%u",dl[gas_id].bsn);
    }

    /* If splits received in order (2nd followed by 1st), current payload pointer
    ** has been set to NULL, above, to prevent overwriting the block
    ** formed by combining the 2 splits.
    */
    if( cur_dl_blk[gas_id].dsm_payload_ptr != NULL )
    {
      /* put block on dl queue
      */
      E_RX_ARR(gas_id, dl[gas_id].bsn) = cur_dl_blk[gas_id];

      /* pointer to payload in DSM item is considered transferred
      */
      cur_dl_blk[gas_id].dsm_payload_ptr = NULL;
    }

    /* Ensure length accounts for both split blocks
    */
    E_RX_ARR(gas_id, dl[gas_id].bsn).len = (uint8)(GPRS_PDU_LENGTH(E_RX_ARR(gas_id, dl[gas_id].bsn).dsm_payload_ptr));

    /* Update the VN array
    */
    E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED;

    /* Update V(R)
    */
    if (EGPRS_IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vr, dl[gas_id].bsn, dl[gas_id].vq, dl[gas_id].ws))
    {
      /* Clear the blocks between the old VR (inc VR) and BSN-1 but only if VR != BSN
      */
      if (dl[gas_id].vr != dl[gas_id].bsn)
      {
        clear_receive_status(gas_id, dl[gas_id].vr, dl[gas_id].bsn);
      }

      dl[gas_id].vr = MOD_ESNS(dl[gas_id].bsn + 1);
      UPD_DL_VR(gas_id);

      /* Update SSN as well
      */
      dl[gas_id].ssn = dl[gas_id].vr;
      UPD_DL_SSN(gas_id);
    }
  }
  else
  /* Data block is not in the receive window, mark VN element RXS_INVALID
  */
  {
    E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;

    /* if the block is ignored, we must release the DSM item created by PL1
    ** here
    */
    if( cur_dl_blk[gas_id].dsm_payload_ptr )
    {
      GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
      cur_dl_blk[gas_id].dsm_payload_ptr = NULL;
    }
  }

  /*----------------------------------------------------------------------*/

  /* Update V(Q). Only if BSN hits the start of the queue (vq).
  ** If V(Q) moves then assemble the block into the current PDU
  */
  if (dl[gas_id].vq == dl[gas_id].bsn)
  {
    /* Set V(Q) to the value of the BSN within the window that
    ** has not yet been properly received and which minimises
    ** the expression [BSN - V(R)] mod SNS.
    **
    ** The expression above means the NEW value of V(Q) will be
    ** nearest to the old value of V(Q). When searching from
    ** V(Q)+1 towards V(R) the first BSN with a status not being
    ** RXS_RECEIVED item shall be assigned to V(Q).
    ** If there is no item whose V(N) value is RXS_RECEIVED then
    ** assign V(R) to V(Q).
    **
    ** When V(Q) hops along the contiguous blocks are assembled into PDU(s)
    **
    ** The case where V(Q) = V(R) before the search starts will
    ** not happen because when V(R) = V(Q) = 0 and BSN = 0,
    ** V(R) gets incremented and is 1 higher than V(Q).
    */

    /* When V(Q) moves the data block must be assembled into the current PDU
    */
    grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, dl[gas_id].bsn) );
    E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED_ASSEMBLED;

    /* Advance V(Q). Now loop from the new value of V(Q) to the new value
    ** of V(R) to seek a contiguous block with a RXS_RECEIVED state.
    ** If found the
    */
    dl[gas_id].vq = MOD_ESNS(dl[gas_id].vq+1);
    UPD_DL_VQ(gas_id);

    /* Use a copy of V(Q) since V(Q) gets altered in the loop
    */
    temp_vq = dl[gas_id].vq;

    for (b=0; b < MOD_DIF_ESNS(dl[gas_id].vr, temp_vq); b++)
    {
      ind = MOD_ESNS(b + temp_vq);
      if (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED)
      {
        /* found another contiguous block with RXS_RECEIVED status.
        ** So, assemble it and advance V(Q)
        */
        grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, ind) );
        dl[gas_id].vq = MOD_ESNS(ind + 1);
        UPD_DL_VQ(gas_id);
        E_RX_ARR(gas_id, ind).vn = RXS_RECEIVED_ASSEMBLED;
      }
      else
      {
        /* break out of loop and do not alter V(Q) when the first RXS_INVALID
        ** block is met
        */
        break;
      }
    } /* end search V(Q) loop */

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
    /* Update V(R)_MAX only when Vr_max falls out of window
    ** We want to retain vr_max only for crc failure blocks
    ** so as to report ms_out_of_mem only when for fail crc
    ** blocks and good crc blocks.
    */
    if (! EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl[gas_id].vr_max, dl[gas_id].ws) )
    {
      dl[gas_id].vr_max = dl[gas_id].vq;
      MSG_GRLC_LOW_2_G(GRLC_F3_MSG_ENUM_150, "ERLD vr_max has gone out of window,update vr_max %d vq %d",
               dl[gas_id].vr_max,dl[gas_id].vq);
    }
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */
  }
} /* e_process_dl_ack_block */

/*===========================================================================
===
===  FUNCTION     E_PROCESS_DL_UNACK_BLOCK()
===
===  DESCRIPTION
===
===    In UNACK mode all BSN are within the rx window. Attach
===    the block to the rx array. If the block belongs to a deferent
===    frame number from the previous block received, then assemble all blocks
===    between V(Q) to V(R). Update V(R), V(N). Set V(Q) in accordance with
===    the new value of V(R) after re-assembly completes.
===
===    Note: Sending a LLC PDU after Re-assembly to LLC, should be held pending
===    until the last BSN in the current radio block period has been received.
===    This is essential when interacting with a BSS that does not
===    order it's BSNs in line with the time slot number (eg Motorola BSS).
===    Failure to deal with this results in filler blocks been created for
===    those BSNs that are received out of order. This results in FCS errors
===    in LLC and eventually reduced throughput.
===
===    input: global dl (dl_ass, vr, vq, bsn), dl[gas_id].bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void e_process_dl_unack_block ( gas_id_t gas_id )
{
  uint16                  b, ind;       /* loop count */

  /* temporary downlink data block for creating filler blocks */
  grlc_dl_data_block_t  temp_dl_blk;

  /*----------------------------------------------------------*/

  /* Data block in cur_dl_blk[gas_id] has just taken out of the FIFO.
  ** In UNACK mode all BSNs are valid and should not be checked
  ** for rx window compliance. Also, there is no need to check for
  ** split blocks since no blocks are re-txed.
  */

  /* Some infras (Ericsson-Vodafone Southampton) re-tx the last block in RC4.
  ** Therefore if the received block is 2 (for dual payload) or 1 BSNs behind V(R)
  ** it should be ignored.
  */
  if( (dl[gas_id].bsn != dl[gas_id].vr) && MOD_DIF_ESNS(dl[gas_id].vr, dl[gas_id].bsn ) <= 2 )
  {
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_151, "GRLD UNACK discard bsn=%d out of seq, vr=%d, vq=%d",dl[gas_id].bsn, dl[gas_id].vr,dl[gas_id].vq);

    if( cur_dl_blk[gas_id].dsm_payload_ptr )
    {
      GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
      cur_dl_blk[gas_id].dsm_payload_ptr = NULL;
    }
    return;
  }

  /* put block on dl queue
  */
  E_RX_ARR(gas_id, dl[gas_id].bsn) = cur_dl_blk[gas_id];

  /* pointer to payload in DSM item is considered transferred
  */
  cur_dl_blk[gas_id].dsm_payload_ptr = NULL;

  /* Update the VN array
  */
  E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED;

  if( (dl[gas_id].bsn == dl[gas_id].vr) && (dl[gas_id].ass_start == E_GRLC_INVALID_BSN) )
  {
    /* new block is next in seq and there are no outstanding BSN gaps
    ** in this frame. Update the state variables before calling
    ** grlc_dl_e_assemble() to append block to pdu. Send it off
    ** if pdu is complete.
    */
    e_update_dl_unack_state_vars( gas_id, dl[gas_id].bsn );
    grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, dl[gas_id].bsn) );
    E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED_ASSEMBLED;
  }
  else
  {
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_152, "GRLD UNACK re-ass bsn=%d out of seq, vr=%d, vq=%d",dl[gas_id].bsn, dl[gas_id].vr,dl[gas_id].vq);

    /* BSN is out of seq, we have to delay re-assembly until all BSNs of the same
    ** frame are received. After the frame changes re-ordering can occur.
    ** There are two possible scenarios after reordering:
    ** a. There are some BSNs missing trailing V(R). Create fillers and re-assemble PDU.
    ** b. No missing BSNs. Re-assemble PDU.
    */

    /* If BSN > V(R) + MAX_DL_BLCKS_PER_FR, there is no need to delay re-assembly.
    ** The BSN is genuinely out of order and not due to bsn ordering in the frame.
    ** Note, MAX_DL_BLCKS_PER_FR is a function of num of dl nts.
    **/
    if(  IS_GE_MOD( dl[gas_id].bsn,dl[gas_id].vr,dl[gas_id].ws )    &&
         IS_GT_MOD( MOD_ESNS(dl[gas_id].vr + GRLC_MAX_DL_BLCKS_PER_FR(gas_id)), dl[gas_id].bsn,dl[gas_id].ws)
      )
    {
      /* BSN is out of seq. Mark starting point of re-assembly if it hasn't already
      ** been marked due to a previous out of seq BSN in the same frame.
      */
      if( dl[gas_id].ass_start == E_GRLC_INVALID_BSN )
      {
        dl[gas_id].ass_start = dl[gas_id].vr;
        dl[gas_id].ass_end   = dl[gas_id].bsn;

        /* Latch frame number to use for detecting frame number change */
        dl[gas_id].frame_number_unack = dl[gas_id].frame_number;

        /* Clear the blocks between V(R) [inc V(R)] and V(R) + MAX_DL_BLCKS_PER_FR blocks.
        ** Eight is the max number of blocks in a frame ( 4 TS, dual pld block )
        ** Note, the 2nd parameter minus 1, of the following function, determines
        ** the upper limit. That is why it is set to V(R) + 9 blocks.
        */
        clear_receive_status(gas_id, dl[gas_id].vr,
                             (uint16)MOD_ESNS(dl[gas_id].vr + (GRLC_MAX_DL_BLCKS_PER_FR(gas_id) + 1))
                            );

        /* Update the VN array
        */
        E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED;

        if( dl[gas_id].fbi == RM_FBI_FINAL )
        {
          /* If FBI is set, there is no point delaying re-assembly.*/
          grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, dl[gas_id].bsn) );
          E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_RECEIVED_ASSEMBLED;
        }
      }
      else
      {
        /* Update end point and V(R) if lower than current BSN. */
        if( IS_GE_MOD( dl[gas_id].bsn, dl[gas_id].ass_end, dl[gas_id].ws )  )
        {
          dl[gas_id].ass_end = dl[gas_id].bsn;
        }

        /* An out of seq BSN has been already received in this frame.
        ** If frame changed,or FBI set, re-assemble outstanding blocks.
        */
        if( ( dl[gas_id].frame_number_unack != dl[gas_id].frame_number ) ||
            ( dl[gas_id].fbi == RM_FBI_FINAL )                                            )
        {
          /* Re-assemble all outstanding blocks from ass_start to ass_end.
          ** Note that any missing BSNs at this point are genuine and no due to out
          ** of sequence BSNs in the same frame. Create fillers for the missing blocks.
          */
          for (b=0; b <= MOD_DIF_ESNS(dl[gas_id].ass_end, dl[gas_id].ass_start); b++)
          {
            ind = MOD_ESNS(dl[gas_id].ass_start + b);

            if (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED)
            {
              grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, ind) );
              E_RX_ARR(gas_id, ind).vn = RXS_RECEIVED_ASSEMBLED;
            }
            else
            {
              /* create filler block with given bsn and some existing
              ** info belonging to the current dl block.
              */
              e_create_dl_filler_block(gas_id, &temp_dl_blk, ind);

              /* update diag filled octet count
              */
              dl_diag_stats_st[gas_id].unack_oct_filled_cnt += (uint32)temp_dl_blk.len;

              dl[gas_id].filler_block_cnt++;

              /* assemble the filler block
              */
              grlc_dl_e_assemble( gas_id, &temp_dl_blk);
            }
          } /* end of for loop */

          /* Set V(R) & V(Q) */
          e_update_dl_unack_state_vars( gas_id, dl[gas_id].ass_end );

          /* Reset re-assembly indexes */
          dl[gas_id].ass_start = E_GRLC_INVALID_BSN;
          dl[gas_id].ass_end   = E_GRLC_INVALID_BSN;
          dl[gas_id].frame_number = dl[gas_id].frame_number_unack;
        }
      }
    } /* if ( V(R) < BSN < V(R) + 8 ) */
    else
    {
      /* BSN is out of seq. We need to re-sync V(R) & V(Q). V(R) shall be set to
      ** [ BSN' + 1 ] modulo SNS, where BSN' is the BSN of most recently received
      ** RLC data block.
      ** if [V(R) - V(Q)] modulo SNS > WS after updating V(R), then V(Q) is set
      ** to [V(R) - WS] modulo SNS.
      */

      /* Update end point and V(R) if lower than current BSN. */
      if( IS_GE_MOD( dl[gas_id].bsn, dl[gas_id].ass_end, dl[gas_id].ws )  )
      {
        dl[gas_id].ass_end = dl[gas_id].bsn;
      }

      if( dl[gas_id].ass_start == E_GRLC_INVALID_BSN )
      {
        dl[gas_id].ass_start = dl[gas_id].vr;
      }

      /* Before re-sync, finish re-assembling outstanding blocks,
      ** send re-assembled PDU to LLC.
      */

      if( dl[gas_id].ass_start != E_GRLC_INVALID_BSN )
      {
        /* Outstanding blocks to be re-assembled. ass_start points to the 1st one
        */
        for (b=0; b <= MOD_DIF_ESNS(dl[gas_id].ass_end, dl[gas_id].ass_start); b++)
        {
          ind = MOD_ESNS(dl[gas_id].ass_start + b);

          if (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED)
          {
            grlc_dl_e_assemble( gas_id, &E_RX_ARR(gas_id, ind) );
            E_RX_ARR(gas_id, ind).vn = RXS_RECEIVED_ASSEMBLED;
          }
          else
          {
            /* create filler block with given bsn and some existing
            ** info belonging to the current dl block.
            */
            e_create_dl_filler_block(gas_id, &temp_dl_blk, ind);

            /* update diag filled octet count
            */
            dl_diag_stats_st[gas_id].unack_oct_filled_cnt += (uint32)temp_dl_blk.len;

            dl[gas_id].filler_block_cnt++;

            /* assemble the filler block
            */
            grlc_dl_e_assemble( gas_id, &temp_dl_blk);
          }
        } /* end of for loop */
      } /* End re-ass out of order BSNs */

      /* Set V(R) & V(Q) */
      e_update_dl_unack_state_vars( gas_id, dl[gas_id].bsn );

      /* Reset re-assembly indexes */
      dl[gas_id].ass_start = E_GRLC_INVALID_BSN;
      dl[gas_id].ass_end   = E_GRLC_INVALID_BSN;

    } /* BSN > V(R) + 8 */
  }  /* Out of Seq BSN */
} /* e_process_dl_unack_block */

/*===========================================================================
===
===  FUNCTION    FORMAT_DL_ACKNACK_DESC()
===
===  DESCRIPTION
===
===    Format the packet downlink acknack message by converting the receive
===    status array V(N) into the Starting Sequence Number (SSN) and the
===    Receive Block Bitmap (RBB) array.
===
===     input: global vn, vr
===     output: acknack_desc (rbb, ssn)
===
===  DEPENDENCIES
===
===    dl
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/

#if ( RM_RLC_WINDOW_SIZE != 64 )
  #error Window Size not Supported!
#endif

static void format_dl_acknack_desc
(
  gas_id_t                   gas_id,
  rm_dl_pkt_acknack_req_t    *dl_acknack_ptr
)
{
  /* Must use register variables to avoid repeated memory access. */

  register uint32 bitmap_lo = 0;
  register uint32 bitmap_hi = 0;
  register uint32 bitmap_bit;
  register uint32 vn_index;

  /* Initialise SSN to V(R) and V(N) index to [V(R)-1] MOD SNS */

  dl_acknack_ptr->acknack_desc.ssn = (uint8)dl[gas_id].vr;

  if (dl[gas_id].vr > 0)
    vn_index = dl[gas_id].vr - 1;
  else
    vn_index = SNS - 1;

  /* Construct bitmap for [V(R)-1] MOD SNS to [V(R)-32] MOD SNS */

  for ( bitmap_bit = 1; bitmap_bit != 0; bitmap_bit <<= 1 )
  {
    if ( RX_ARR(gas_id, vn_index).vn != RXS_INVALID )
      bitmap_lo |= bitmap_bit;

    if (vn_index > 0)
      vn_index--;
    else
      vn_index = SNS - 1;
  }

  /* Construct bitmap for [V(R)-33] MOD SNS to [V(R)-64] MOD SNS */

  for ( bitmap_bit = 1; bitmap_bit != 0; bitmap_bit <<= 1 )
  {
    if ( RX_ARR(gas_id, vn_index).vn != RXS_INVALID )
      bitmap_hi |= bitmap_bit;

    if (vn_index > 0)
      vn_index--;
    else
      vn_index = SNS - 1;
  }

  /* Load the bitmap to the ACK/NACK description with the required format:
  ** bit 63..56, index = 0
  ** bit 55..48, index = 1
  ** ..
  ** bit  7.. 0, index = 7
  */

  dl_acknack_ptr->acknack_desc.rbb[0] = (uint8)(bitmap_hi >> 24);
  dl_acknack_ptr->acknack_desc.rbb[1] = (uint8)(bitmap_hi >> 16);
  dl_acknack_ptr->acknack_desc.rbb[2] = (uint8)(bitmap_hi >>  8);
  dl_acknack_ptr->acknack_desc.rbb[3] = (uint8)(bitmap_hi >>  0);
  dl_acknack_ptr->acknack_desc.rbb[4] = (uint8)(bitmap_lo >> 24);
  dl_acknack_ptr->acknack_desc.rbb[5] = (uint8)(bitmap_lo >> 16);
  dl_acknack_ptr->acknack_desc.rbb[6] = (uint8)(bitmap_lo >>  8);
  dl_acknack_ptr->acknack_desc.rbb[7] = (uint8)(bitmap_lo >>  0);

} /* end of format_dl_acknack_desc() */

/*===========================================================================
===
===  FUNCTION    GEN_URBB()
===
===  DESCRIPTION
===
===    This function takes a pointer to the first word of the un-compressed
===    bitmap and generates the un-compressed (URBB) bitmap starting from
===    the given bsn number for the specified length
===
===  PARAMS
===
===    urbb_ptr: points to the first word of the URBB
===    start_bsn: starting BSN for the bitmap
===    len: number of blocks to generate the bitmap for
===
===  DEPENDENCIES
===
===     'len' bits of the URBB is overwritten with the bitmap.
===     maximum size of the URBB is 5 32-bit words (160 bits)
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gen_urbb
(
  gas_id_t                gas_id,
  rm_dl_pkt_acknack_req_t *dl_pkt_acknack_req,
  uint16  start_bsn,
  uint16  len
)
{
  uint32*         urbb_ptr = &(dl_pkt_acknack_req->e_acknack_desc.urbb[0]);
  register uint16 i, ind;
  register uint32 mask;

  /* Clear the entire URBB. Later on, we just set the postive ack ones
  */
  urbb_ptr[0] = 0UL;
  urbb_ptr[1] = 0UL;
  urbb_ptr[2] = 0UL;
  urbb_ptr[3] = 0UL;
  urbb_ptr[4] = 0UL;
  urbb_ptr[5] = 0UL;
  urbb_ptr[6] = 0UL;
  urbb_ptr[7] = 0UL;

  mask = 1UL;
  ind = start_bsn;

  for (i=0; i<len; i++)
  {
    ind = MOD_ESNS(start_bsn+i);

    if (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED ||
        E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED_ASSEMBLED
       )
    {
      /* i >> 5 means div by 32 which gives the index to the 32-bit URBB array
      */
      urbb_ptr[i >> 5] |= mask;
    }

    /* Update mask, when the 1 drops out on the left (MSB) reset it to 1 again
    */
    mask <<= 1;
    if (mask == 0UL)
      mask = 1UL;
  }

  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_153, "ELRD gUrbb Start_bsn=%u Urbb[4]=%08x Urbb[3]=%08x",
          start_bsn,
          urbb_ptr[4],
          urbb_ptr[3]);

  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_154, "ELRD gUrbb Urbb[2]=%08x Urbb[1]=%08x Urbb[0]=%08x",
          urbb_ptr[2],
          urbb_ptr[1],
          urbb_ptr[0]);

  dl_pkt_acknack_req->e_acknack_desc.urbb_len = (uint8)len;

} /* gen_urbb */

/*===========================================================================
===
===  FUNCTION    GEN_CRBB()
===
===  DESCRIPTION
===
===    This function generates the compressed bitmap starting from the the specified
===    SSN value for bm_len (blocks). The compressed bitmap CRBB[], its length
===    CRBB_LEN and the Colour Code SCC are stored back in the passed structure.
===    The function also returns the Ending block Sequence Number of the compressed CRBB
===
===    The CRBB comprises an array of 4 32-bit words in the following format:
===
===    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
===
===    --code word 5-cont-LSB-\MSB---------code word 6-------LSB\MSB--code word 7--(highest BSN)---LSB  [0]
===    -------LSB\MSB-----code word 3-------LSB\MSB------code word 4--------LSB\MSB----code word 5----  [1]
===    -----------\MSB-make-up code 1-lowest BSN)--LSB\MSB-terminating code 1 LSB\MSB---code word 2---  [2]
===    -----------------------------------------------------------------------------------------------  [3]
===
===  DEPENDENCIES
===
===    E_RX_ARR(gas_id, ),
===
===  PARAMS
===
===    INPUT
===    *desc_ptr: points to acknack description structure where the input and results
===               will go.
===               desc_ptr->ssn: starting BSN to generate the bitmap for
===    bm_len: number of blocks to generate the bitmap from SSN inclusive (to VR-1)
===    avail_space_for_bm: Available space for CRBB and URBB in the EPDAN description.
===    rbm_with_qrep_len : Available space for CRBB and URBB in the EPDAN description
===                        after including the Quality report.
===    esp               : ES/P value
===
===    OUTPUT
===    desc_ptr->crbb[], desc_ptr->crbb_len, desc_ptr->scc
===
===  RETURN VALUE
===
===    esn_crbb: absolute BSN value representing the ending sequence number of the CRBB
===
===  SIDE EFFECTS
===    If Quality Report can not be included in the available space, then
===    dl_pkt_acknack_req->pl1_qrep_ptr is set to NULL.
===
===  COMMENTS
===
===    To do:
===
===========================================================================*/
/* Maximum size of CRBB assuming no quality report and no packet channel request
** that an EPDAN can accomodate
*/
#define MAX_CRBB_LEN                 127         /* See TS 04.60 table 12.3.1.2 */

/* Minimum averaged length of a code word. This is taken as the average sum of the
** shortest zero run length code word and the shortest one run length code word
*/
#define MIN_CODE_WORD_LEN           ((4+2)/2)   /* average of sum of shortest
                                                ** zero and one code words
                                                */

/* Maximum number of code words (assuming terminating code words only) in the
** largest CRBB possible
*/
#define MAX_CODE_WORD_ARRAY_SIZE    (MAX_CRBB_LEN/MIN_CODE_WORD_LEN+1)

static uint16 gen_crbb
(
  gas_id_t                gas_id,
  rm_dl_pkt_acknack_req_t *dl_pkt_acknack_req,
  uint16                  bm_len,
  uint16                  avail_space_for_bm,
  uint16                  rbm_with_qrep_len,
  uint8                   esp
)
{
  /* Array to hold the encoded results. This array does not really need to be init'ed
  */
  t4_encode_res_t t4_res_arr[MAX_CODE_WORD_ARRAY_SIZE];

  register uint16 u=0;
  register uint16 ind=0, ind_end_crbb =0;
  register uint16 run_len=0;  /* run length of the 'run' thus far */
  register uint16 sum=0;      /* running sum of code words in bits */
  int16    res_cnt=0;         /* SIGNED - # of code word struct returned and accepted */
  uint16   msg_pos;           /* running bit position of code word being packed */
  uint8    run = 0;           /* carries the value of the run: 0 or 1 */
  uint8    state = 0;         /* state of block: 0=invalid, 1 or 2: received */
  uint16   esn_crbb;          /* absolute BSN value representing the ending sequence
                              ** number of the CRBB
                              */

  /* EGPRS Ack/Nack Description pointer */
  egprs_ack_nack_des_t  *desc_ptr = &(dl_pkt_acknack_req->e_acknack_desc);

  /* variables to hold the compression gain during CRBB formation*/
  uint16          temp_comp_gain, highest_comp_gain;

  /* Absolute BSN value representing the ending sequence number of the
  ** CRBB at the highest gain.
  */
  uint16          esn_crbb_highest_gain;

  /* Index to the codeword where the compression gain was the highest.
  */
  uint16          idx_codeword_highest_gain;

  /* Length of the CRBB at the highest compression gain.
  */
  uint16          crbb_len_at_highest_gain;

  boolean  b_end_encode = FALSE;  /* indicate when encoding should stop:
                                   * a) adding last code word exceeds avail_space_for_bm
                                   * a) adding last code word exceeds MAX_CRBB_LEN
                                   * b) remaining available space is greater than
                                   *    the bits to compress
                                   *    HV: temp'ly ignored
                                   */

  /* Set to true when input bitmap fits in available space with the quality report */
  boolean  crbb_urbb_qrep_fit = FALSE;

  /*----------------------------------------------------------------------*/

  /* Initialize t4_res_arr */
  memset((void*)t4_res_arr,0,(sizeof(t4_encode_res_t)* MAX_CODE_WORD_ARRAY_SIZE));

  NOTUSED(rbm_with_qrep_len);

  /* Init crbb[]
  */
  desc_ptr->crbb_len = 0;
  desc_ptr->crbb[0] = 0UL;
  desc_ptr->crbb[1] = 0UL;
  desc_ptr->crbb[2] = 0UL;
  desc_ptr->crbb[3] = 0UL;

  MSG_GRLC_LOW_2_G(GRLC_F3_MSG_ENUM_155, "ERLD gcrbb bm_len=%u avail_space_for_bm=%u",bm_len, avail_space_for_bm);

  /* Index the very first block
  */
  ind = MOD_ESNS(desc_ptr->ssn);

  /* Initialise ending sequence number of the CRBB */
  esn_crbb = ind;

  /* State of the first block. 0:invalid (not yet received), 1 or 2: received.
  ** This is also the colour code
  */
  state = E_RX_ARR(gas_id, ind).vn == RXS_INVALID ? 0 : 1;
  desc_ptr->scc = state;

  /* The run starts in this state
  */
  run = state;
  run_len = 1;

  /* Initialise compression gain tagging parameters */
  highest_comp_gain             = MOD_DIF_ESNS( dl[gas_id].vr, MOD_ESNS(desc_ptr->ssn));
  esn_crbb_highest_gain         = 0;
  idx_codeword_highest_gain     = (uint16)res_cnt;
  crbb_len_at_highest_gain      = sum;

  for (u = 1;                             /* from 2nd block */
       u <= bm_len && !b_end_encode;      /* use '=' to ensure we run to V(R) to detect
                                          **  the last state change
                                          */
       u++
      )
  {
    ind = MOD_ESNS(u + desc_ptr->ssn);
    state = E_RX_ARR(gas_id, ind).vn == RXS_INVALID ? 0 : 1;

    /* MSG_GERAN_MED_3_G("ELRD gcrbb ind=%u run=%u state=%u", ind, run, state); */

    /* run continues
    */
    if (run == state && ind != dl[gas_id].vr)
    {
      run_len++;
    }
    else
    {
      /*-----------------------------
      ** Current run terminates here
      **-----------------------------
      */
      MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_156, "ERLD gcrbb run=%u len=%u ind=%u", run, run_len, ind);

      /* Store the latest result in the last element of the array
      */
      if (grlc_t4_encode(&(t4_res_arr[res_cnt]), run, run_len))
      {
        /* If the results so far plus the latest code word does not exceed the
        ** maximum allowed length then keep the result by: updating the sum and
        ** increment the result count.
        ** Note that we add 8 bits for the CRBB len (7 bits) and the color code (1 bit)
        ** before checking if the CRBB fits in the available space in the EPDAN
        ** description.
        */
        if( ((sum + t4_res_arr[res_cnt].mcode_len + t4_res_arr[res_cnt].tcode_len + 8) <=
             avail_space_for_bm
            ) &&
            ((sum + t4_res_arr[res_cnt].mcode_len + t4_res_arr[res_cnt].tcode_len) <=
              MAX_CRBB_LEN
            )
          )
        {
          /* Update the number of bits in code words returned so far
          */
          sum += t4_res_arr[res_cnt].mcode_len + t4_res_arr[res_cnt].tcode_len;

          /* Calculate the compression gain. This is obtained by adding the CRBB length
          ** and the remaining uncompressed bitmap length.
          ** We need to tag the bsn and the position of the codeword with the best
          ** compression gain so that we can extract the optimal compressed/uncompressed
          ** bit map combination at the end of the compression process.
          */
          temp_comp_gain = sum + (MOD_DIF_ESNS( dl[gas_id].vr,ind ));

          if( temp_comp_gain < highest_comp_gain )
          {
            highest_comp_gain             = temp_comp_gain;
            /* ind points to current bsn + 1 in order to detect the state change.
            ** We need to step one back before storing its value as end BSN for CRBB
            */
            esn_crbb_highest_gain         = MOD_ESNS(ind + ESNS - 1);
            idx_codeword_highest_gain     = (uint16)res_cnt;
            crbb_len_at_highest_gain      = sum;
          }

          if( (temp_comp_gain + 8) <= rbm_with_qrep_len )
          {
            /* Reported bitmap and Quality Report fit in the available space.
            ** Note that since the compressed bitmap is included we have to
            ** take into account 1 bit for the starting colour code
            ** and 7 bits for the compressed bitmap length.
            */
            crbb_urbb_qrep_fit = TRUE;
          }
          else
          {
            crbb_urbb_qrep_fit = FALSE;
          }

          /* Update the code word count
          */
          res_cnt++;
        }
        else
        {
          /* Stop encoding
          */
          b_end_encode = TRUE;
          ind_end_crbb = MOD_ESNS(ind + ESNS - run_len);

        } /* packing code words */

      } /* encode sucesful */
      else
      {
        MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_157, "ERLD gcrbb bad encode run=%u len=%u ind=%u",run, run_len, ind);
      }

      /* Switch state and start and new run
      */
      run = state;
      run_len = 1;

    } /* a run switches state */

  } /* parsing */

  /* The latest result exceeds the number of allowed length so we dont
  ** add it to the array by: not incrementing the result count -- even the
  ** result is already stored in the array anyway
  */

  /* Now we take the results from the array and pack them into the CRBB array
  ** IN REVERSE ORDER
  */
  msg_pos = 0;

  /* Ignore retrieving the optimal CRBB/URBB combination according to the
  ** gain calculated above if:
  ** a. Ending sequence number of the CRBB is equal to V(R).
  **    This means that the whole bitmap fits in the CRBB.
  ** b. The last run produced the highest gain.
  ** c. When CRBB+URBB fits in rbm_with_qrep_len.
  */
  if( IS_GT_MOD( dl[gas_id].vr, ind_end_crbb, dl[gas_id].ws)  &&
      (esn_crbb_highest_gain != MOD_ESNS(ind+ESNS - 1)) &&
      (crbb_urbb_qrep_fit == FALSE)
    )
  {
    /* Initialise the index to the codeword where we had the highest compression gain
    */
    res_cnt = (int16)idx_codeword_highest_gain;
    desc_ptr->crbb_len = (uint8)crbb_len_at_highest_gain;
    esn_crbb = esn_crbb_highest_gain;
  }
  else
  {
    /* Step back to the previously valid index of the result array
    */
    res_cnt--;
    desc_ptr->crbb_len = (uint8)sum;

    /* ind points to current bsn + 1 (or bsn + run_len) in order to detect the state
    ** change. We need to step back before storing its value as the end BSN for CRBB
    */
    if( crbb_urbb_qrep_fit == TRUE )
    {
      esn_crbb = MOD_ESNS(ind + ESNS - 1);
    }
    else
    {
      esn_crbb = MOD_ESNS(ind + ESNS - run_len);
    }
  }

  /* If there is no need for including the Quality Report (ES/P = 1 or 2)
  ** Set Quality Report pointer to NULL if there is not enough space to include it.
  */
  if( esp != ESP_NPB_QREP && !crbb_urbb_qrep_fit )
  {
#ifdef PERLUTF
    #error code not present
#endif
    dl_pkt_acknack_req->pl1_qrep_ptr = NULL;
    MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_158, "ELRD gcrbb dropped QR ES/P=%d", esp);
  }

  if ( res_cnt < MAX_CODE_WORD_ARRAY_SIZE )
  {
    while (res_cnt >= 0)
    {
      /* Pack terminating code which is always present
      */
#ifdef  ERLD_SHOW_DL_CODEWORDS
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_159, "ELRD gcrbb pos=%u tcode=%x, tcode_len=%u",
              msg_pos,
              t4_res_arr[res_cnt].tcode,
              t4_res_arr[res_cnt].tcode_len);
#endif /* ERLD_SHOW_DL_CODEWORDS */

      epackw( t4_res_arr[res_cnt].tcode,
              (uint8*)&(desc_ptr->crbb[0]),
              &msg_pos,
              t4_res_arr[res_cnt].tcode_len
            );

      /* Then make-up code if it exists. Note this takes up the higher order
      ** words than the terminating code in the CRBB array
      */
      if (t4_res_arr[res_cnt].mcode_len != 0)
      {
#ifdef  ERLD_SHOW_DL_CODEWORDS
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_160, "ELRD gcrbb pos=%u mcode=%x, mcode_len=%u",
                msg_pos,
                t4_res_arr[res_cnt].mcode,
                t4_res_arr[res_cnt].mcode_len);
#endif /* ERLD_SHOW_DL_CODEWORDS */

        epackw( t4_res_arr[res_cnt].mcode,
                (uint8*)&(desc_ptr->crbb[0]),
                &msg_pos,
                t4_res_arr[res_cnt].mcode_len
              );

      }

#ifdef  ERLD_SHOW_DL_CODEWORDS
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_161, "ELRD gcrbb pos=%u crbb[1]=%08x crbb[0]=%08x",
                msg_pos,
                desc_ptr->crbb[1],
                desc_ptr->crbb[0]);

      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_162, "ELRD       pos=%u crbb[3]=%08x crbb[2]=%08x",
              msg_pos,
              desc_ptr->crbb[3],
              desc_ptr->crbb[2]);
#endif /* ERLD_SHOW_DL_CODEWORDS */

      /* One less result to pack
      */
      --res_cnt;

    } /* loop packing code words in reverse order */
  }
  else
  {
    MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_163, "res_cnt greater than or equal to MAX_CODE_WORD_ARRAY_SIZE oor %u",res_cnt);
  }

  /* If no valid CRBB, then the end bsn for the crbb is set to V(Q) */
  if( !desc_ptr->crbb_len )
    esn_crbb = dl[gas_id].vq;

  MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_164, "ERLD gcrbb crbb_len=%u, esn_crbb=%u",
    desc_ptr->crbb_len, esn_crbb);

  return (esn_crbb);

} /* gen_crbb */

/*===========================================================================
===
===  FUNCTION     UPDATE_DL_UNACK_STATE_VARS()
===
===  DESCRIPTION
===
===    Update the V(R) and V(Q) state variables after a new block of
===    given BSN arrives. In ack mode, the status array V(N) is then
===    updated. V(N) is not updated in un-ack mode.
===    In un-ack mode, V(R) is updated first before V(Q) because the
===    conditions for updating V(Q) involves V(R).
===
===    input: global dl (dl_ass, vr, vq, bsn), dl[gas_id].bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void update_dl_unack_state_vars ( gas_id_t gas_id )
{
  /*-------------------------------------------------
  ** Update V(R) & V(Q) in un-ack mode.
  ** Must update V(R) first since V(Q) depends on it
  **-------------------------------------------------
  */
  dl[gas_id].vr = (uint8)MOD_SNS(dl[gas_id].bsn + 1);
  UPD_DL_VR(gas_id);

  /* When [V(R) - V(Q)] mod SNS > WS, the window exceeds its maximum size.
  ** To correct it, V(Q) is shifted along to V(R)-WS which is the element
  ** just inside the receive window.
  */
  if (MOD_DIF(dl[gas_id].vr, dl[gas_id].vq, SNS) > RM_RLC_WINDOW_SIZE)
  {
    dl[gas_id].vq = (uint8)MOD_SNS(dl[gas_id].vr - RM_RLC_WINDOW_SIZE);
    UPD_DL_VQ(gas_id);
  }
} /* update_dl_unack_state_vars */

/*===========================================================================
===
===  FUNCTION      CREATE_DL_FILLER_BLOCK
===
===  DESCRIPTION
===
===    Create a blank filler downlink data block. The payload contains
===    filler octets with the full data block length as per the current
===    coding scheme less the 2 octets RLC header. The block has no
===    extension octet and its FBI field is set to NOT_FINAL.
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
static void create_dl_filler_block
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t  *blk_ptr,
  uint8                 bsn
)
{
  /* temp buffer to create filled block. No longer used after DSM item is
  ** created
  */
  uint8   buf[GRLC_RADIO_BLOCK_MAX_LEN];

  /* the grlc_dl_assemble() function needs the BSN and FBI fields. The FBI should
  ** have a NOT_FINAL value because it is a filled block, hence it is
  ** behind the current block which means it can not be final.
  ** The EXT field should have a NO_OCTET_FOLLOWS value since there is no
  ** extension octet. The data block length is the full length according to
  ** the coding scheme less the RLC header.
  */
  blk_ptr->bsn = bsn;
  blk_ptr->fbi = RM_FBI_NOT_FINAL;
  blk_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;

  blk_ptr->num_ext_octets = 0;
  blk_ptr->len = (uint8)(GRLC_DATA_BLOCK_LEN[dl[gas_id].cs] - RM_RLC_HDR_LEN);
  blk_ptr->cs = dl[gas_id].cs;

  /* fills temp buffer with filler octet
  */
  (void)memset(buf, RM_DL_FILLER_OCTET, blk_ptr->len);

  /* convert buffer to DSM item
  */
  blk_ptr->dsm_payload_ptr = grlc_pdu_construct(buf, (uint16)blk_ptr->len);

} /* create_dl_filler_block */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_ack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t         *rlc_msg_ptr;

  uint16 loop_ctr  = 0;
#ifdef FEATURE_GPRS_PS_HANDOVER
  uint16 idx = 0;
#endif /* FEATURE_GPRS_PS_HANDOVER */
  /* if there is data block available
  */
#ifdef FEATURE_GPRS_PS_HANDOVER
  if (dl[gas_id].b_psho_active)
  {
    loop_ctr = dl[gas_id].blind_data_blks_ctr;
  }
  else
#endif /* FEATURE_GPRS_PS_HANDOVER */
  {
    loop_ctr = grlc_dl_fifo_get_cnt(gas_id);
  }

  while (loop_ctr > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (dl[gas_id].b_psho_active)
    {
      pl1_blk_hdr_ptr[gas_id] = &dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_hdr;
      pl1_blk_dsm_ptr[gas_id] = dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_dsm_ptr;
      dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_dsm_ptr = NULL;
      idx++;
    }
    else
#endif /* FEATURE_GPRS_PS_HANDOVER */
    if (grlc_dl_fifo_get_block(gas_id, &pl1_blk_hdr_ptr[gas_id], &pl1_blk_dsm_ptr[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_165, "GRLD un-exp DL empty");
      continue;
    }

    {
      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /* translate the over-the-air raw data into cur_dl_blk[gas_id]
      */
      xlate_ext_dl_data(gas_id, pl1_blk_hdr_ptr[gas_id], pl1_blk_dsm_ptr[gas_id], &cur_dl_blk[gas_id]);

      /* Save the coding scheme, frame number, time_slot, S/P, RRBP and
      ** final block indicator from the data block just translated to the
      ** dl struct
      */
      dl[gas_id].bsn = cur_dl_blk[gas_id].bsn;

      /* Update coding scheme for engg_mode
      */
      if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
      {
        geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
      }

      /* copy PL1 header info
      */
      dl[gas_id].cs = cur_dl_blk[gas_id].cs;

#ifdef FEATURE_GPRS_PS_HANDOVER
      /* RLC to inhibit sending PDAN for blind data blocks having
      ** poll set
      */
      if(dl[gas_id].b_psho_active)
      {
        dl[gas_id].sp = 0;
      }
      else
      {
        dl[gas_id].sp = cur_mac_hdr[gas_id].type.dl_data.sp;
      }
#else
      dl[gas_id].sp = cur_mac_hdr[gas_id].type.dl_data.sp;
#endif /* FEATURE_GPRS_PS_HANDOVER */

      dl[gas_id].rrbp = cur_mac_hdr[gas_id].type.dl_data.rrbp;
      dl[gas_id].fbi = cur_dl_blk[gas_id].fbi;

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
      show_bsn_and_fifo_vars(gas_id);
#endif
      /* latch the FBI value if the current FBI (belonging to a block)
      ** is set and the latch value if it is NOT set
      ** if Incoming BSN is out of window , dont latch FBI.
      */

       /* NW if transmit the same BSN with FBI =0 and then FBI = 1,
          we should act on the FBI=1.
       */
      if ( IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, cur_dl_blk[gas_id].bsn, WS) ||
           (cur_dl_blk[gas_id].bsn == MOD_SNS(dl[gas_id].vq-1))
         )
      {
        if (dl[gas_id].fbi == RM_FBI_FINAL)
        {
          dl[gas_id].fbi_latch = RM_FBI_FINAL;
        }
      }
      else
      {
        dl[gas_id].fbi = 0;
      }

      /* Put data block on dl queue and update state vars if the block
      ** has not been received before otherwise ignore the data carried
      ** by the block
      */
      if ( (RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_INVALID                ) ||
           (RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_RECEIVED_ASSEMBLED)    )
      {
        /* If the data block in cur_dl_blk[gas_id] is within the receive window,
        ** then attach it to the receive array. If not then ignore it but
        ** free the DSM memory. Update VR, VN and SSN. If the BSN of data
        ** block co-incides with V(Q) the block is assembled into the
        ** current PDU, and V(Q) is moved along until a contiguous data
        ** block is not found. When assembling into the current PDU if a
        ** PDU is complete then it's sent off to LLC and a new PDU is created
        */
        dl[gas_id].eng_mode_rcvd_dl_block_cnt ++;

        dl[gas_id].rx_blk_cnt++;
        process_dl_ack_block(gas_id);
      }
      else if ( RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_RECEIVED )
      {
        dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt++;

        /* ignored block already received. Must release its DSM resource
        ** for the payload. The header area was freed when it was
        ** 'stripped'
        */
        GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
      }
      else
      {
        /* This code has been added as a defensive measure against an
           unexplained situation where RLC continually NACKed a block
           when polled but then appeared to repeatedly ignore the
           retransmitted block from the network. */

        MSG_GRLC_ERROR_2_G( GRLC_F3_MSG_ENUM_166, "GRLD VN Receive State Err: BSN %d VN %d",
          dl[gas_id].bsn, RX_ARR(gas_id, dl[gas_id].bsn).vn);

        RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;
        process_dl_ack_block(gas_id);
      }

      /*-----------------------------------------------------------------
      ** If this is the 1st block received then send indication to MAC
      ** and set the flag. DO this before consider sending paknak to MAC
      **-----------------------------------------------------------------
      */
      if (dl[gas_id].b_first_block_ind_sent == FALSE)
      {
        dl[gas_id].b_first_block_ind_sent = TRUE;

        /* Indicate to MAC so it can enter DL transfer mode
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
          rlc_msg_ptr->msg.dl_first_block_ind.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      }

      /*-------------------------------------------------------------
      ** now check for FBI, polling, VR=VQ and do:
      ** Now check for these conditions:
      ** 1.polled (RRBP valid): send Pkt Dl Acknack msg
      ** 2.polled and FBI set and VQ = VR (all blocks received):
      **   assemble PDU(s) and send to LLC, send Pkt Dl Acknack with
      **   FAI set. Enter RX_ACK_PENDING_RELEASE.
      ** 3.polled and FBI set and VQ != VR: send Pkt Dl Acknack with
      **   FAI clear.
      **-------------------------------------------------------------
      */

      if ( grlc_dl_schedule_pkt_ack_nack_transmission(gas_id) )
      {
        grlc_dl_request_pkt_ack_nack_transmission(gas_id);
      }

    } /* end get valid DL block */

    loop_ctr--;
  } /* end while */
} /* ds_rx_ack_state_pl1_ph_data_ind_event_handler */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state in EGPRS mode.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_ack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{

  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;
  grlc_dl_data_block_t  *tmp_data_block_ptr;

  /* Flag to indicate that the EPDAN has been send to MAC */
  boolean                 epdan_req_not_serviced = FALSE;

  /* loop counter for dual payload */
  uint8                   max_block_no;

  uint16 loop_ctr = 0;
#ifdef FEATURE_GPRS_PS_HANDOVER
  uint16 idx = 0;
#endif /*FEATURE_GPRS_PS_HANDOVER */

  /* if there is data block available
  */
#ifdef FEATURE_GPRS_PS_HANDOVER
  if (dl[gas_id].b_psho_active)
  {
    loop_ctr = dl[gas_id].blind_data_blks_ctr;
  }
  else
#endif /* FEATURE_GPRS_PS_HANDOVER */
  {
    loop_ctr = grlc_dl_fifo_get_cnt(gas_id);
  }

  while (loop_ctr > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (dl[gas_id].b_psho_active)
    {
      e_pl1_dl_blk[gas_id] = dl_psho_blind_arr[gas_id][idx].e_dl_blk;
      dl_psho_blind_arr[gas_id][idx].e_dl_blk.payload_1_ptr = NULL;
      dl_psho_blind_arr[gas_id][idx].e_dl_blk.payload_2_ptr = NULL;
      idx++;
    }
    else
#endif /* FEATURE_GPRS_PS_HANDOVER */
    if (e_grlc_dl_fifo_get_block(gas_id, &e_pl1_dl_blk[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_167, "ERLD un-exp DL Q empty ");
      continue;
    }

    {
      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /* Validate L1 DL block(s)
      */
      e_validate_dl_pl1_block( gas_id, TRUE );

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_xlate_ext_dl_data(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);

      /* Validate BSN number and FBI field  */
      if( data_block2.dsm_payload_ptr != NULL )
      {
        /* If dual payload with same BSN number for both payloads,
        ** or 1st payload has FBI set, then ignore 2nd payload.
        */
        if( (data_block1.bsn == data_block2.bsn) ||
            (data_block1.fbi == RM_FBI_FINAL)       )
        {
          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr );
          data_block2.dsm_payload_ptr = NULL;
        }
      }

      grlc_dl_diag_rx_stat_add_entry(&data_block1, gas_id);
      if (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6)
      {
        grlc_dl_diag_rx_stat_add_entry(&data_block2, gas_id);
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);
#endif

      /* Determine the number of or RLC blocks (payloads) */
      if( e_pl1_dl_blk[gas_id].mcs <= RM_MCS_6 )
        max_block_no = 1;
      else
        max_block_no = 2;

      /* Start with the 1st DL data block  */
      tmp_data_block_ptr = &data_block1;

      while( max_block_no-- )
      {
        /* Store DL block to current RLC DL block */
        cur_dl_blk[gas_id] = *tmp_data_block_ptr;

        /* Save the coding scheme, frame number, time_slot, ES/P, RRBP and
        ** final block indicator from the data block just translated to the
        ** dl struct
        */
        dl[gas_id].bsn = cur_dl_blk[gas_id].bsn;

        /* Save the TFI for generating EPDAN later on
        */
        dl[gas_id].tfi = e_cur_hdr[gas_id].tfi;

        /* Update coding scheme for engg_mode
        */
        if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
        {
          geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
        }

        /* copy PL1 header info
        */
        dl[gas_id].cs = cur_dl_blk[gas_id].cs;

        dl[gas_id].sp   = e_cur_hdr[gas_id].esp;
        dl[gas_id].rrbp = e_cur_hdr[gas_id].rrbp;

        /* latch the FBI value if the current FBI (belonging to a block)
         ** is set and the latch value if it is NOT set
         ** if Incoming BSN is out of window , dont latch FBI.
        */
         /* NW if transmit the same BSN with FBI =0 and then FBI = 1,
            we should act on the FBI=1.
         */
         if( EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, cur_dl_blk[gas_id].bsn, dl[gas_id].ws) ||
            (cur_dl_blk[gas_id].bsn == MOD_ESNS(dl[gas_id].vq-1))
          )
         {
          if (dl[gas_id].cs > RM_MCS_6 && dl[gas_id].cs <= RM_MCS_9)
          {
            dl[gas_id].fbi = data_block1.fbi | data_block2.fbi;
          }
          else
          {
            dl[gas_id].fbi = data_block1.fbi;
          }
         }
         else
         {
            dl[gas_id].fbi = 0;
         }

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
        show_bsn_and_fifo_vars(gas_id);
#endif

        /* Processing payload only if it has passed CRC */
        if( cur_dl_blk[gas_id].crc && cur_dl_blk[gas_id].dsm_payload_ptr )
        {
          /* latch the FBI value if the current FBI (belonging to a block)
          ** is set and the latch value if it is NOT set
          */
          if (dl[gas_id].fbi == RM_FBI_FINAL)
          {
            dl[gas_id].fbi_latch = RM_FBI_FINAL;
          }

          /* Put data block on dl queue and update state vars if the block
          ** has not been received before otherwise ignore the data carried
          ** by the block
          */
          if ( (E_RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_INVALID           ) ||
               (E_RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_RECEIVED_ASSEMBLED)    )
          {
            /* If the data block in cur_dl_blk[gas_id] is within the receive window,
            ** then attach it to the receive array. If not then ignore it but
            ** free the DSM memory. Update VR, VN and SSN. If the BSN of data
            ** block co-incides with V(Q) the block is assembled into the
            ** current PDU, and V(Q) is moved along until a contiguous data
            ** block is not found. When assmebling into the current PDU if a
            ** PDU is complete then it's sent off to LLC and a new PDU is created
            */
            dl[gas_id].eng_mode_rcvd_dl_block_cnt ++;

            dl[gas_id].rx_blk_cnt++;

            e_process_dl_ack_block(gas_id);
          }
          else if ( E_RX_ARR(gas_id, dl[gas_id].bsn).vn == RXS_RECEIVED )
          {
            dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt ++;
            /* ignored block already received. Must release its DSM resource
            ** for the payload. The header area was freed when it was
            ** 'stripped'
            */
            GPRS_PDU_FREE(&(cur_dl_blk[gas_id].dsm_payload_ptr));
          }
          else
          {
            /* This code has been added as a defensive measure against an
               unexplained situation where RLC continually NACKed a block
               when polled but then appeared to repeatedly ignore the
               retransmitted block from the network. */

            MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_168, "ERLD VN Receive State Err: BSN %d EVN %d",
              dl[gas_id].bsn, E_RX_ARR(gas_id, dl[gas_id].bsn).vn);

            E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;
            e_process_dl_ack_block(gas_id);
          }

          /*-----------------------------------------------------------------
          ** If this is the 1st block received then send indication to MAC
          ** and set the flag. DO this before consider sending paknak to MAC
          **-----------------------------------------------------------------
          */
          if (dl[gas_id].b_first_block_ind_sent == FALSE)
          {
            dl[gas_id].b_first_block_ind_sent = TRUE;

            /* Indicate to MAC so it can enter DL transfer mode
            */
            rlc_msg_ptr = mac_get_cmd_buf_rlc();
            if (rlc_msg_ptr != NULL)
            {
              rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
              rlc_msg_ptr->msg.dl_first_block_ind.gas_id = gas_id;
              mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
            }
          }
        } /* end processing block with valid CRC */
        else
        {
          /* Block failed CRC. Do not process/reassemble
          ** do not update V(R). if bsn within window mark it as invalid.
          ** V(R) is updated only in test mode to meet the tester requirements.
          */
          if (EGPRS_IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vr, dl[gas_id].bsn, dl[gas_id].vq, dl[gas_id].ws))
          {
            E_RX_ARR(gas_id, dl[gas_id].bsn).vn = RXS_INVALID;
          }

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
          /* Update V(R)_MAX , it should be bsn and not vr+1 ,as we want to test if thi sbsn
          ** is in physical operational window range
          */
          if (EGPRS_IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vr_max, dl[gas_id].bsn, dl[gas_id].vq, dl[gas_id].ws))
          {
            dl[gas_id].vr_max = dl[gas_id].bsn;
          }
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
          E_RX_ARR(gas_id, dl[gas_id].bsn).b_extra_depadding = cur_dl_blk[gas_id].b_extra_depadding;
#endif /*  FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */
        }

        /*-------------------------------------------------------------
        ** now check for FBI, polling, VR=VQ and do:
        ** Now check for these conditions:
        ** 1.polled (RRBP valid): send Pkt Dl Acknack msg
        ** 2.polled and FBI set and VQ = VR (all blocks received):
        **   assemble PDU(s) and send to LLC, send Pkt Dl Acknack with
        **   FAI set. Enter RX_ACK_PENDING_RELEASE.
        ** 3.polled and FBI set and VQ != VR: send Pkt Dl Acknack with
        **   FAI clear.
        **-------------------------------------------------------------
        */

        /* Check if the EPDAN_REQ signal mask is set
        */
        if( grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_EPDAN_REQ_SIG )
        {
          /* Without removing the EPDAN_REQ signal from the queue, check if the
          ** current block is the the block that EPDAN was requested by PL1.
          */
          epdan_req_ptr = (pl1_rlc_msg_t *)q_check(&DPQ(gas_id));

          if( epdan_req_ptr == NULL )
          {
            MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_169, "ERLD EPDAN_REQ mask without msg");
          }
          else
          {
            /* Flag to check if an EPDAN was sent to MAC after the request was detected
            */
            epdan_req_not_serviced = TRUE;

            /* Reply to the EPDAN only if
            ** a. Single payload in the DL block
            ** b. Dual payload and all payloads have been processed
            ** For both cases make sure that the BSN, which the EPDAN has
            ** been requested for, has been procesed and BSN < V(R),
            ** or the bsn is outside the rx window.
            */
            if( max_block_no == 0)  /* all payloads have been processed */
            {
              /* The current block is the block that EPDAN was requested by PL1. Read
              ** EPDAN signal from the queue and clear signal mask if queue empty.
              */
              epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ(gas_id));

              GRLC_DL_MSG_LOCK(gas_id);

              if (q_cnt(&DPQ(gas_id)) == 0)
              {
                RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
              }

              GRLC_DL_MSG_UNLOCK(gas_id);

              if( epdan_req_ptr != NULL )
              {
                /* Check the Final Block and if all DL blocks sent by the peer
                ** have been received and format the EPDAN.
                */

                /* get buffer for msg to send to MAC
                */
                rlc_msg_ptr = mac_get_cmd_buf_rlc();
                if (rlc_msg_ptr != NULL)
                {
                  /* vq=vr & fbi means all data blocks have been received
                  */
                  if (dl[gas_id].fbi_latch == RM_FBI_FINAL && dl[gas_id].vr == dl[gas_id].vq)
                  {
                    /* Stop T3190 when sending paknak with FAI set
                    */
                    TIMER_STOP_T3190(gas_id);

                    /* PL1 control the starting and re-starting of T3192 when it
                    ** next sends a DL acknack msg with FAI bit set
                    */

                    /* Now wait in release pending
                    */
                    dl[gas_id].state = DS_RX_ACK_PENDING_RELEASE;
                    UPD_DL_STATE(gas_id);

                    /* MSG_GERAN_LOW_3_G("GRLD RX_A sp FAI P_REL vq=%d vr=%d bsn=%d",
                    **          dl[gas_id].vq,dl[gas_id].vr,dl[gas_id].bsn);
                    */

                    /* set FAI of packet dl acknack
                    ** ! formulate acknack msg
                    */
                    rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                              RM_ACKNACK_FAI_TBF_COMPLETE;

                  } /* end tbf complete */
                  else
                  {
                    /* all data blocks have not been received.
                    ** Set FAI = 0 in packet dl acknack
                    */
                    rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                              RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

                    /*MSG_LOW("GRLD RX_A sp vq=%d vr=%d ",dl[gas_id].vq,dl[gas_id].vr,0);*/
                  } /* end tbf NOT complete */

                  /*---------------------------------------------------------------
                  ** Formulate ssn & rbb array vars. Set signal and send packet dl
                  ** acknack RRBP and frame number and TFI
                  **---------------------------------------------------------------
                  */

                  /* Copy the Quality Report from PL1. If the Quality Report is dropped
                  ** while the EPDAN description is formatted (ESP 1/2), the following
                  ** pointer is set to NULL, in e_format_dl_acknack_desc(), to indicate
                  ** to MAC that the QReport should not be included in the EPDAN.
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr =
                      epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
                  #error code not present
#endif

                  e_format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req,
                                            epdan_req_ptr);
                  UPD_DL_ACKNACK(gas_id);
                  /* Fill the rest of the message */
                  rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->
                                                                          frame_no;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                                          include_chan_req;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl[gas_id].tfi;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->
                                                                          timeslot;

#ifdef  ERLD_SHOW_EPDAN
                  show_epdan (gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif
                  /* Log DL EPDAN
                  ** Log Code: 0x5211
                  */
                  rlc_log_epdan(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);

                  /* Populate the out-of-memory bit with value from L1 message (from FW)
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.ms_out_of_mem =
                    epdan_req_ptr->ms_out_of_mem;

                  /* Send EPDAN to MAC */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
                  mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

                  epdan_req_not_serviced = FALSE;

                  /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
                  #error code not present
#else
                  gprs_rlc_l1isr_free_buffer(gas_id, epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF*/
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
                }
              } /* end if( epdan_req_ptr != NULL ) */
              else
              {
                MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_170, "ERLD EPDAN_REQ without sig in DPQueue");
              }
            } /*  if( !(IS_GT_MOD( epdan_req_ptr->bsn, dl[gas_id].vr, dl[gas_id].ws)) ) */
          } /*  end if( epdan_req_ptr == NULL ) */
        }  /*  end if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG ) */

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
        /*
        ** process 2nd block only after V(q) is updated and received for the
        ** first time. max_block_no = 1 after decrement for dual payload.
        */
        if (max_block_no)
        {
          if( EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, data_block2.bsn, dl[gas_id].ws))
          {
            if((data_block2.failed_crc) && (data_block2.cs == RM_MCS_8) &&
               (E_RX_ARR(gas_id, data_block2.bsn).vn == RXS_INVALID)
              )
            {
              data_block2.b_extra_depadding = TRUE;
              MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_171, "ERLD incoming BSN2 mcs 8 marked TRUE=%u mcs=%u",
                          data_block2.bsn,
                          data_block2.cs);
            }
            else
            {
              data_block2.b_extra_depadding = FALSE;
            }
          }
          else
          {
            data_block2.b_extra_depadding = FALSE;
          }
        }
#endif /* FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */

        /* Now process the 2nd payload if present */
        tmp_data_block_ptr = &data_block2;

      } /* while( tmp_data_block_ptr && max_block_no-- ) */
    } /* end get valid DL block */
    loop_ctr--;
  } /* end while */

  if( epdan_req_not_serviced == TRUE )
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_172, "ERLD Did not respond to EPDAN_REQ ");
  }
} /* e_ds_rx_ack_state_pl1_ph_data_ind_event_handler */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_STATE_PL1_EPDAN_REQ_EVENT_HANDLER
===
===  DESCRIPTION
===
===   Handler of PL1_EPDAN_REQ event in DS_RX_ACK state in EGPRS mode
===   when DL fifo is empty.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_ack_state_pl1_epdan_req_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;


  /* Check if the EPDAN_REQ signal mask is set
  */
  /* Get message from the queue*/
  epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ(gas_id));

  if( epdan_req_ptr == NULL )
  {
    MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_173, "ERLD EPDAN_REQ mask without msg");
  }
  else
  {
    GRLC_DL_MSG_LOCK(gas_id);

    if (q_cnt(&DPQ(gas_id)) == 0)
    {
      RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
    }

    GRLC_DL_MSG_UNLOCK(gas_id);

    /* Process EPDAN whether the block is in (BSN) range or not
    */

    /* get buffer for msg to send to MAC
    */
    rlc_msg_ptr = mac_get_cmd_buf_rlc();
    if (rlc_msg_ptr != NULL)
    {
      /* vq=vr & fbi means all data blocks have been received
      */
      if (dl[gas_id].fbi_latch == RM_FBI_FINAL && dl[gas_id].vr == dl[gas_id].vq)
      {
        /* Stop T3190 when sending paknak with FAI set
        */
        TIMER_STOP_T3190(gas_id);

        /* PL1 control the starting and re-starting of T3192 when it
        ** next sends a DL acknack msg with FAI bit set
        */

        /* Now wait in release pending
        */
        dl[gas_id].state = DS_RX_ACK_PENDING_RELEASE;
        UPD_DL_STATE(gas_id);

        /*MSG_LOW("GRLD RX_A sp FAI P_REL vq=%d vr=%d bsn=%d",dl[gas_id].vq,dl[gas_id].vr,dl[gas_id].bsn);*/

        /* set FAI of packet dl acknack
        ** ! formulate acknack msg
        */
        rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                  RM_ACKNACK_FAI_TBF_COMPLETE;

      } /* end tbf complete */
      else
      {
        /* all data blocks have not been received.
        ** Set FAI = 0 in packet dl acknack
        */
        rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                  RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

        /*MSG_LOW("GRLD RX_A sp vq=%d vr=%d ",dl[gas_id].vq,dl[gas_id].vr,0);*/
      } /* end tbf NOT complete */

      /*---------------------------------------------------------------
      ** Formulate ssn & rbb array vars. Set signal and send packet dl
      ** acknack RRBP and frame number and TFI
      **---------------------------------------------------------------
      */

      /* Copy the Quality Report from PL1. If the Quality Report is dropped
      ** while the EPDAN description is formatted (ESP 1/2), the following pointer
      ** is set to NULL, in e_format_dl_acknack_desc(), to indicate
      ** to MAC that the QReport should not be included in the EPDAN.
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr = epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
      #error code not present
#endif

      e_format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req, epdan_req_ptr);
      UPD_DL_ACKNACK(gas_id);

      /* Fill the rest of the message */
      rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->frame_no;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req =
        epdan_req_ptr->include_chan_req;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl[gas_id].tfi;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->timeslot;

#ifdef  ERLD_SHOW_EPDAN
      show_epdan (gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif

      /* Log DL EPDAN
      ** Log Code: 0x5211
      */
      rlc_log_epdan(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);

      /* Populate the out-of-memory bit with value from L1 message (from FW)
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.ms_out_of_mem =
        epdan_req_ptr->ms_out_of_mem;

      /* Send EPDAN to MAC */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
      mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

      /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
      #error code not present
#else
      gprs_rlc_l1isr_free_buffer(gas_id, epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF*/
    }
    else
    {
      ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
    }
  }/*  end if( epdan_req_ptr == NULL ) */

} /* e_ds_rx_ack_state_pl1_epdan_req_event_handler */

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK_PEND_REL state.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /*--------------------------------------------------------------------
  ** We have received all data blocks and sent DL acknack with FAI.
  ** However, if the network might have missed it in which case it will
  ** re-send data block(s) with the Polling bit set (SP). We must check
  ** for this Polling bit and re-send our DL acknack with FAI until such
  ** time as T3192 expires
  **--------------------------------------------------------------------
  */
  while (grlc_dl_fifo_get_cnt(gas_id) > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
    if (grlc_dl_fifo_get_block(gas_id, &pl1_blk_hdr_ptr[gas_id], &pl1_blk_dsm_ptr[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_174, "GRLD un-exp DL empty");
    }
    else
    {
      /* translate the over-the-air raw data into cur_dl_blk[gas_id]
      */
      xlate_ext_dl_data(gas_id, pl1_blk_hdr_ptr[gas_id], pl1_blk_dsm_ptr[gas_id], &cur_dl_blk[gas_id]);
#ifdef  GRLD_SHOW_BSN
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_175, "GRLD A BSN=%d SP=%d FBI=%d",dl[gas_id].bsn,dl[gas_id].sp,dl[gas_id].fbi);
#endif
      /* Note that in ACK_PENDING_REL we only check for RRBP being valid
      ** and no SP checking is required
      */
      if (cur_mac_hdr[gas_id].type.dl_data.sp == RM_SP_RRBP_VALID)
      {
        dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt++;

        /*-----------------
        ** Re-send paknack
        **-----------------
        */
        dl[gas_id].rrbp = cur_mac_hdr[gas_id].type.dl_data.rrbp;

        /* Update coding scheme for engg_mode
        */
        if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
        {
          geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
        }

        /* update dl cs from PL1
        */
        dl[gas_id].cs = cur_dl_blk[gas_id].cs;

        /* get buffer for msg to send to MAC
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          /* Stop T3190 when sending paknak with FAI set
          */
          TIMER_STOP_T3190(gas_id);

          /* PL1 control the starting and re-starting of T3192 when it
          ** next sends a DL acknack msg with FAI bit set
          */

          /* Set FAI of packet dl acknack.
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai =
                    RM_ACKNACK_FAI_TBF_COMPLETE;

          /*---------------------------------------------------------------
          ** Formulate ssn & rbb array vars. Set signal and send packet dl
          ** acknack RRBP and frame number and TFI
          **---------------------------------------------------------------
          */

          format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
          UPD_DL_ACKNACK(gas_id);

          show_dl_rbb(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc,
            rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai);

          rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
          /* rrbp from the current dl data block where the polling bit
          ** is detected
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp = dl[gas_id].rrbp;
          /* frame number carried beside data block from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = dl[gas_id].frame_number;

          /* copy time slot to MAC as well
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot = dl[gas_id].time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi = dl[gas_id].tfi;

          rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end polling bit (SP) set */

      GPRS_PDU_FREE(&cur_dl_blk[gas_id].dsm_payload_ptr);

    } /* end get valid block */
  } /* end while */
} /* ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler() */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK_PEND_REL state.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;
  grlc_dl_data_block_t  *tmp_data_block_ptr;

  /* Flag to indicate that the EPDAN has been send to MAC */
  boolean                 epdan_req_not_serviced = FALSE;

  /* loop counter for dual payload */
  uint8                   max_block_no;

  /* if there is data block available
  */
  while (grlc_dl_fifo_get_cnt(gas_id) > 0)
  {
    if (e_grlc_dl_fifo_get_block(gas_id, &e_pl1_dl_blk[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_176, "ERLD EPDAN_REQ with DL Q empty ");
    }
    else
    {
      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      /* Validate L1 DL block(s)
      */
      e_validate_dl_pl1_block( gas_id, TRUE );

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_xlate_ext_dl_data(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);

      /* Validate BSN number and FBI field  */
      if( data_block2.dsm_payload_ptr != NULL )
      {
        /* If dual payload with same BSN number for both payloads,
        ** or 1st payload has FBI set, then ignore 2nd payload.
        */
        if( (data_block1.bsn == data_block2.bsn) ||
            (data_block1.fbi == RM_FBI_FINAL)       )
        {

          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr );
          data_block2.dsm_payload_ptr = NULL;
        }
      }

      grlc_dl_diag_rx_stat_add_entry(&data_block1, gas_id);
      if (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6)
      {
        grlc_dl_diag_rx_stat_add_entry(&data_block2, gas_id);
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);
#endif

      /* Determine the number of or RLC blocks (payloads) */
      if( e_pl1_dl_blk[gas_id].mcs <= RM_MCS_6 )
        max_block_no = 1;
      else
        max_block_no = 2;

      /* Start with the 1st DL data block  */
      tmp_data_block_ptr = &data_block1;

      while( max_block_no-- )
      {
        dl[gas_id].eng_mode_rcvd_dup_dl_block_cnt++;

        /* Store DL block to current RLC DL block */
        cur_dl_blk[gas_id] = *tmp_data_block_ptr;

        /* Update coding scheme for engg_mode
        */
        if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
        {
          geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
        }

        /* update the coding scheme.
        */
        dl[gas_id].cs = cur_dl_blk[gas_id].cs;

        /* Check if the EPDAN_REQ signal mask is set
        */
        if( grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_EPDAN_REQ_SIG )
        {
          /* Without removing the EPDAN_REQ signal from the queue, check if the
          ** current block is the block that EPDAN was requested by PL1.
          */
          epdan_req_ptr = (pl1_rlc_msg_t *)q_check(&DPQ(gas_id));

          if( epdan_req_ptr == NULL )
          {
            MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_177, "ERLD EPDAN_REQ mask without msg");
          }
          else
          {
            /* Flag to check if an EPDAN was sent to MAC after the request was detected
            */
            epdan_req_not_serviced = TRUE;

            /* Reply to the EPDAN only if
            ** a. Single payload in the DL block
            ** b. Dual payload and all payloads have been processed
            ** For both cases make sure that the BSN, which the EPDAN has
            ** been requested for, has been procesed and BSN < V(R), or
            ** has failed crc, or the bsn is outside the rx window.
            */
            if(  max_block_no == 0 )  /* all payloads have been processed */
            {
              /* The current block is the block that EPDAN was requested by PL1. Read
              ** EPDAN signal from the queue and clear signal mask if queue empty.
              */
              epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ(gas_id));

              GRLC_DL_MSG_LOCK(gas_id);

              if (q_cnt(&DPQ(gas_id)) == 0)
              {
                RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
              }

              GRLC_DL_MSG_UNLOCK(gas_id);

              if( epdan_req_ptr != NULL )
              {
                /* Stop T3190 when sending paknak with FAI set
                */
                TIMER_STOP_T3190(gas_id);

                /* PL1 control the starting and re-starting of T3192 when it
                ** next sends a DL acknack msg with FAI bit set
                */

                /*-----------------
                ** Re-send paknack
                **-----------------
                */
                dl[gas_id].sp   = e_cur_hdr[gas_id].esp;
                dl[gas_id].rrbp = e_cur_hdr[gas_id].rrbp;

                /* get buffer for msg to send to MAC
                */
                rlc_msg_ptr = mac_get_cmd_buf_rlc();
                if (rlc_msg_ptr != NULL)
                {
                  /* set FAI of packet dl acknack
                  ** ! formulate acknack msg
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                            RM_ACKNACK_FAI_TBF_COMPLETE;

                  /*---------------------------------------------------------------
                  ** Formulate ssn & rbb array vars. Set signal and send packet dl
                  ** acknack RRBP and frame number and TFI
                  **---------------------------------------------------------------
                  */
                  /* Copy the Quality Report from PL1. If the Quality Report is dropped
                  ** while the EPDAN description is formatted (ESP 1/2), the following
                  ** pointer is set to NULL, in e_format_dl_acknack_desc(), to indicate
                  ** to MAC that the QReport should not be included in the EPDAN.
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr =
                    epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
                  #error code not present
#endif

                  e_format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req,
                                            epdan_req_ptr);
                  UPD_DL_ACKNACK(gas_id);

                  /* Fill the rest of the message */
                  rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->
                                                                      frame_no;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                                        include_chan_req;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl[gas_id].tfi;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->
                                                                      timeslot;

#ifdef  ERLD_SHOW_EPDAN
                  show_epdan (gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif
                  /* Log DL EPDAN
                  ** Log Code: 0x5211
                  */
                  rlc_log_epdan(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);

                  /* Populate the out-of-memory bit with value from L1 message (from FW)
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.ms_out_of_mem =
                    epdan_req_ptr->ms_out_of_mem;

                  /* Send EPDAN to MAC */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
                  mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

                  epdan_req_not_serviced = FALSE;

                  /* Free epdan_req_ptr  */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
                  #error code not present
#else
                  gprs_rlc_l1isr_free_buffer(gas_id, epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
                }
              } /* end if( epdan_req_ptr != NULL ) */
              else
              {
                MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_178, "ERLD EPDAN_REQ without sig in DPQueue");
              }
            } /* if( (IS_GT_MOD( dl[gas_id].vr, epdan_req_ptr->bsn, dl[gas_id].ws)) ||.. */
          } /*  end if( epdan_req_ptr == NULL ) */
        }  /*  end if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG ) */

        if( cur_dl_blk[gas_id].dsm_payload_ptr )
        {
          /* Free DL RLC data block */
          GPRS_PDU_FREE(&cur_dl_blk[gas_id].dsm_payload_ptr);
        }

        /* Now process the 2nd payload if present */
        tmp_data_block_ptr = &data_block2;

      } /* while( tmp_data_block_ptr && max_block_no-- ) */
    } /* end get valid DL block */
  } /* end while */

  if( epdan_req_not_serviced == TRUE )
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_179, "ERLD Did not respond to EPDAN_REQ ");
  }
} /* e_ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler() */

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK state in GPRS mode.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_unack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* temporary downlink data block for creating filler block */
  grlc_dl_data_block_t  temp_dl_blk;

  /* pointer to buffer for holding messages to send to MAC */
  rlc_mac_msg_t         *rlc_msg_ptr;

  /* loop count */
  uint8                 b;
  uint16                u;

  uint16 loop_ctr  = 0;
#ifdef FEATURE_GPRS_PS_HANDOVER
  uint16 idx = 0;
#endif /* FEATURE_GPRS_PS_HANDOVER */
  /* if there is data block available
  */
#ifdef FEATURE_GPRS_PS_HANDOVER
  if (dl[gas_id].b_psho_active)
  {
    loop_ctr = dl[gas_id].blind_data_blks_ctr;
  }
  else
#endif /* FEATURE_GPRS_PS_HANDOVER */
  {
    loop_ctr = grlc_dl_fifo_get_cnt(gas_id);
  }

  /* if there is data block available
  */
  while (loop_ctr > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (dl[gas_id].b_psho_active)
    {
      pl1_blk_hdr_ptr[gas_id] = &dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_hdr;
      pl1_blk_dsm_ptr[gas_id] = dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_dsm_ptr;
      dl_psho_blind_arr[gas_id][idx].dl_blk.pl1_blk_dsm_ptr = NULL;
      idx++;
    }
    else
#endif /* FEATURE_GPRS_PS_HANDOVER */
    if (grlc_dl_fifo_get_block(gas_id, &pl1_blk_hdr_ptr[gas_id], &pl1_blk_dsm_ptr[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_180, "GRLD un-exp DL empty");
      continue;
    }

    {
      /* translate the over-the-air raw data into cur_dl_blk[gas_id]
      */
      xlate_ext_dl_data(gas_id, pl1_blk_hdr_ptr[gas_id], pl1_blk_dsm_ptr[gas_id], &cur_dl_blk[gas_id]);

      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /*-------------------------------------------------------
      ** deal with downlink data block from L1 in un-ack mode
      **-------------------------------------------------------
      */
      dl[gas_id].bsn = cur_dl_blk[gas_id].bsn;

      /* Update coding scheme for engg_mode
      */
      if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
      {
        geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
      }

      /* copy PL1 header info
      */
      dl[gas_id].cs = cur_dl_blk[gas_id].cs;

#ifdef FEATURE_GPRS_PS_HANDOVER
      /* RLC to inhibit sending PDAN for blind data blocks having
      ** poll set
      */
      if(dl[gas_id].b_psho_active)
      {
        dl[gas_id].sp = 0;
      }
      else
      {
        dl[gas_id].sp = cur_mac_hdr[gas_id].type.dl_data.sp;
      }
#else
      dl[gas_id].sp = cur_mac_hdr[gas_id].type.dl_data.sp;
#endif /* FEATURE_GPRS_PS_HANDOVER */

      dl[gas_id].rrbp = cur_mac_hdr[gas_id].type.dl_data.rrbp;
      dl[gas_id].fbi  = cur_dl_blk[gas_id].fbi;

      /* latch the FBI value if the current FBI (belonging to a block)
      ** is set and the latch value if it is NOT set
      */
      if (dl[gas_id].fbi == RM_FBI_FINAL)
      {
        dl[gas_id].fbi_latch = RM_FBI_FINAL;
      }

      /* Only deal with block whose bsn has not been received
      ** In unack mode, this means the bsn has to be greater than
      ** or equal to V(R)
      */
      if (IS_GE_MODULO_COMP(cur_dl_blk[gas_id].bsn,dl[gas_id].vr,SNS))
      {
    #ifdef  GRLD_SHOW_BSN
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_181, "GRLD U BSN=%d SP=%d FBI=%d",dl[gas_id].bsn,dl[gas_id].sp,dl[gas_id].fbi);
    #endif
        dl[gas_id].eng_mode_rcvd_dl_block_cnt ++;

        dl[gas_id].rx_blk_cnt++;

        /* if it is the new bsn next in sequence
        ** ! to consider wrap-round here
        */
        if (dl[gas_id].bsn == dl[gas_id].vr)
        {
          /* new block is next in seq.
          ** Update the state variables before calling
          ** grlc_dl_assemble() to append block to pdu.
          ** Send it off if pdu is complete.
          */
          update_dl_unack_state_vars(gas_id);
          grlc_dl_assemble(gas_id, &cur_dl_blk[gas_id]);
        }
        else
        {
          /* New block reveals several missing blocks which need to be
          ** filled. Loop to create blank blocks starting from V(R) to
          ** the block just before the current block. Assemble each of
          ** the block to the current pdu(s) as they are created.
          ** If a pdu is complete then create a new PDU.
          */

          /* loop to send filled blocks from V(R) to the block before
          ** the current block (BSN-1).
          */
          for (b=0; b<MOD_DIF(dl[gas_id].bsn, dl[gas_id].vr, SNS); b++)
          {
            /* create filler block with given bsn and some existing
            ** info belonging to the current dl block.
            */
            create_dl_filler_block(gas_id, &temp_dl_blk, (uint8)MOD_SNS(dl[gas_id].vr+b));

            /* update diag filled octet count
            */
            dl_diag_stats_st[gas_id].unack_oct_filled_cnt += (uint32)temp_dl_blk.len;

            dl[gas_id].filler_block_cnt++;

            /* assemble the filler block
            */
            grlc_dl_assemble(gas_id, &temp_dl_blk);
          }

          /* now send the current block
          */
          grlc_dl_assemble(gas_id, &cur_dl_blk[gas_id]);

          /* update other dl vars
          */
          update_dl_unack_state_vars(gas_id);
        }
      }  /* end got new block */
      else
      {
        /* Free payload of already received block
        */
        GPRS_PDU_FREE(&cur_dl_blk[gas_id].dsm_payload_ptr);
        MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_182, "GRLD U free %d vr=%d vq=%d",cur_dl_blk[gas_id].bsn,dl[gas_id].vr,dl[gas_id].vq);
      }

      /*-----------------------------------------------------------------
      ** If this is the 1st block received then send indication to MAC
      ** and set the flag. Do this before consider sending paknak to MAC
      **-----------------------------------------------------------------
      */
      if (dl[gas_id].b_first_block_ind_sent == FALSE)
      {
        dl[gas_id].b_first_block_ind_sent = TRUE;

        /* Indicate to MAC so it can enter DL transfer mode
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
          rlc_msg_ptr->msg.dl_first_block_ind.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      }

      /*------------------------------------------------------------------
      ** Send PCA and enter PENDING_RELEASE if FBI is received. Otherwise
      ** send PDAN if polled
      **------------------------------------------------------------------
      */
      if (dl[gas_id].fbi == RM_FBI_FINAL)
      {
        /* Stop T3190 when sending Pkt Ctrl Acknack
        */
        TIMER_STOP_T3190(gas_id);

        /*----------------------------------------------------
        ** Request MAC to send Packet Control Acknowledgement
        **----------------------------------------------------
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

          /* rrbp from the current dl data block where polling bit is detected
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl[gas_id].rrbp;

          /* frame number carried over from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = dl[gas_id].frame_number;

          /* ..and time slot
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl[gas_id].time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = dl[gas_id].tfi;

          MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_183, "GRLD RX_U FBI Sent PCAck");
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

          /* Next state is unack pending release
          */
          dl[gas_id].state = DS_RX_UNACK_PENDING_RELEASE;
          UPD_DL_STATE(gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end FB bit set */
      else
      /*------------------------------------------------------------
      ** Send PDAN if Polling bit is set. Yes even in un-ack mode !
      **------------------------------------------------------------
      */
      if (dl[gas_id].sp == RM_SP_RRBP_VALID)
      {
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;

          /*---------------------------------------------------------------
          ** Set SSN to VR. Set RBB to all 0xff since all blocks from VR
          ** must have been received or filled in
          **---------------------------------------------------------------
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.ssn = (uint8)dl[gas_id].vr;

          /* Fill the bitmap with all 1's
          */
          for (u=0; u<RM_RBB_ARRAY_LEN; u++)
            rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.rbb[u] = 0xff;
          UPD_DL_ACKNACK(gas_id);

          /* set FA to 0 in un-ack mode
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai =
                    RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

          show_dl_rbb(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc,
            rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai);

          /* rrbp from the current dl data block where the polling bit
          ** is detected
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp = dl[gas_id].rrbp;

          /* frame number carried beside data block from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = dl[gas_id].frame_number;

          /* copy time slot to MAC as well
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot = dl[gas_id].time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi = dl[gas_id].tfi;

          rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end SP bit set */
    } /* end get valid block */

    loop_ctr--;
  } /* end while loop */
} /* ds_rx_unack_state_pl1_ph_data_ind_event_handler() */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK state in EGPRS mode.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_unack_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;
  grlc_dl_data_block_t  *tmp_data_block_ptr;

  /* Flag to indicate that the EPDAN has been send to MAC */
  boolean                 epdan_req_not_serviced = FALSE;

  /* Flag to indicate that the Packet Control Acknowledgement has been send to MAC */
  boolean                 is_pca_sent = FALSE;

  /* loop counter for dual payload */
  uint8                   max_block_no;

  uint16 loop_ctr = 0;
#ifdef FEATURE_GPRS_PS_HANDOVER
  uint16 idx = 0;
#endif /*FEATURE_GPRS_PS_HANDOVER */

  /* if there is data block available
  */
#ifdef FEATURE_GPRS_PS_HANDOVER
  if (dl[gas_id].b_psho_active)
  {
    loop_ctr = dl[gas_id].blind_data_blks_ctr;
  }
  else
#endif /* FEATURE_GPRS_PS_HANDOVER */
  {
    loop_ctr = grlc_dl_fifo_get_cnt(gas_id);
  }

  while (loop_ctr > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (dl[gas_id].b_psho_active)
    {
      e_pl1_dl_blk[gas_id] = dl_psho_blind_arr[gas_id][idx].e_dl_blk;
      dl_psho_blind_arr[gas_id][idx].e_dl_blk.payload_1_ptr = NULL;
      dl_psho_blind_arr[gas_id][idx].e_dl_blk.payload_2_ptr = NULL;
      idx++;
    }
    else
#endif /* FEATURE_GPRS_PS_HANDOVER */
    if (e_grlc_dl_fifo_get_block(gas_id, &e_pl1_dl_blk[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_184, "ERLD un-exp with DL Q empty ");
      continue;
    }

    {
      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /* Validate L1 DL block(s)
      */
      e_validate_dl_pl1_block( gas_id, FALSE );

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_xlate_ext_dl_data(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);

      /* Validate BSN number and FBI field  */
      if( data_block2.dsm_payload_ptr != NULL )
      {
        /* If dual payload with same BSN number for both payloads,
        ** or 1st payload has FBI set, then ignore 2nd payload.
        */
        if( (data_block1.bsn == data_block2.bsn) ||
            (data_block1.fbi == RM_FBI_FINAL)       )
        {
          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr );
          data_block2.dsm_payload_ptr = NULL;
        }
      }

      grlc_dl_diag_rx_stat_add_entry(&data_block1, gas_id);
      if (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6)
      {
        grlc_dl_diag_rx_stat_add_entry(&data_block2, gas_id);
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);
#endif

      /* Determine the number of or RLC blocks (payloads) */
      if( e_pl1_dl_blk[gas_id].mcs <= RM_MCS_6 )
        max_block_no = 1;
      else
        max_block_no = 2;

      /* Start with the 1st DL data block  */
      tmp_data_block_ptr = &data_block1;

      while( max_block_no-- )
      {
        /* Store DL block to current RLC DL block */
        cur_dl_blk[gas_id] = *tmp_data_block_ptr;

        /* Save the coding scheme, frame number, time_slot, ES/P, RRBP and
        ** final block indicator from the data block just translated to the
        ** dl struct
        */
        dl[gas_id].bsn = cur_dl_blk[gas_id].bsn;

        /* Save the TFI for generating EPDAN later on
        */
        dl[gas_id].tfi = e_cur_hdr[gas_id].tfi;

        /* Update coding scheme for engg_mode
        */
        if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
        {
          geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
        }

        /* copy PL1 header info
        */
        dl[gas_id].cs = cur_dl_blk[gas_id].cs;

        dl[gas_id].sp   = e_cur_hdr[gas_id].esp;
        dl[gas_id].rrbp = e_cur_hdr[gas_id].rrbp;

        if (dl[gas_id].cs > RM_MCS_6 && dl[gas_id].cs <= RM_MCS_9)
        {
          dl[gas_id].fbi = data_block1.fbi | data_block2.fbi;
        }
        else
        {
          dl[gas_id].fbi = data_block1.fbi;
        }

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
        show_bsn_and_fifo_vars(gas_id);
#endif
        /* latch the FBI value if the current FBI (belonging to a block)
        ** is set and the latch value if it is NOT set
        */
        if (dl[gas_id].fbi == RM_FBI_FINAL)
        {
          dl[gas_id].fbi_latch = RM_FBI_FINAL;
        }

        /* Processing payload */
        if( cur_dl_blk[gas_id].dsm_payload_ptr )
        {
          dl[gas_id].eng_mode_rcvd_dl_block_cnt++;

          dl[gas_id].rx_blk_cnt++;

          e_process_dl_unack_block(gas_id);
        }

        /*-----------------------------------------------------------------
        ** If this is the 1st block received then send indication to MAC
        ** and set the flag. DO this before consider sending paknak to MAC
        **-----------------------------------------------------------------
        */
        if (dl[gas_id].b_first_block_ind_sent == FALSE)
        {
          dl[gas_id].b_first_block_ind_sent = TRUE;

          /* Indicate to MAC so it can enter DL transfer mode
          */
          rlc_msg_ptr = mac_get_cmd_buf_rlc();
          if (rlc_msg_ptr != NULL)
          {
            rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
            rlc_msg_ptr->msg.dl_first_block_ind.gas_id = gas_id;
            mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("GRLC heap exhaustion");
          }
        }

        /*------------------------------------------------------------------
        ** Send PCA and enter PENDING_RELEASE if FBI is received. Otherwise
        ** send PDAN if polled. Only send one PCA if processing a dual payload
        **------------------------------------------------------------------
        */
        if( (dl[gas_id].fbi == RM_FBI_FINAL) && (is_pca_sent == FALSE))
        {
          /* Stop T3190 when sending Pkt Ctrl Acknack
          */
          TIMER_STOP_T3190(gas_id);

          /*----------------------------------------------------
          ** Request MAC to send Packet Control Acknowledgement
          **----------------------------------------------------
          */
          rlc_msg_ptr = mac_get_cmd_buf_rlc();
          if (rlc_msg_ptr != NULL)
          {
            rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

            /* rrbp from the current dl data block where polling bit is detected
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl[gas_id].rrbp;

            /* frame number carried over from PL1 for each data block
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = e_pl1_dl_blk[gas_id].frame_number;

            /* ..and time slot
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl[gas_id].time_slot;

            /* ..and TFI
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = dl[gas_id].tfi;

            is_pca_sent = TRUE;

            MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_185, "GRLD RX_U FBI in RX_UNACK Sent PCAck");
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.gas_id = gas_id;
            mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

            /* Next state is unack pending release
            */
            dl[gas_id].state = DS_RX_UNACK_PENDING_RELEASE;
            UPD_DL_STATE(gas_id);

            /* if dual payload and the PCA has been send after processing the 1st payload
            ** make sure we do not send a PCA again when processing the 2nd payload
            */
          }
          else
          {
            ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
          }
        } /* end FBI bit set */
        else /* Check if the EPDAN_REQ signal mask is set */
        if( grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_EPDAN_REQ_SIG )
        {
          /*-------------------------------------------------------------
          ** now check for FBI, polling, VR=VQ and do:
          ** Now check for these conditions:
          ** 1.polled (RRBP valid): send Pkt Dl Acknack msg
          ** 2.polled and FBI set and VQ = VR (all blocks received):
          **   assemble PDU(s) and send to LLC, send Pkt Dl Acknack with
          **   FAI set. Enter RX_ACK_PENDING_RELEASE.
          ** 3.polled and FBI set and VQ != VR: send Pkt Dl Acknack with
          **   FAI clear.
          **-------------------------------------------------------------
          */

          /* Without removing the EPDAN_REQ signal from the queue, check if the
          ** current block is the the block that EPDAN was requested by PL1.
          */
          epdan_req_ptr = (pl1_rlc_msg_t *)q_check(&DPQ(gas_id));

          if( epdan_req_ptr == NULL )
          {
            MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_186, "ERLD EPDAN_REQ mask without msg");
          }
          else
          {
            /* Flag to check if an EPDAN was sent to MAC after the request was detected
            */
            epdan_req_not_serviced = TRUE;

            /* Reply to the EPDAN only if
            ** a. Single payload in the DL block
            ** b. Dual payload and all payload have been processed */
            if( max_block_no == 0 )  /* all payloads have been processed */
            {
              /* The current block is the block that EPDAN was requested by PL1. Read
              ** EPDAN signal from the queue and clear signal mask if queue empty.
              */
              epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ(gas_id));

              GRLC_DL_MSG_LOCK(gas_id);

              if (q_cnt(&DPQ(gas_id)) == 0)
              {
                RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
              }

              GRLC_DL_MSG_UNLOCK(gas_id);

              if( epdan_req_ptr != NULL )
              {
                /* Check the Final Block and if all DL blocks sent by the peer
                ** have been received and format the EPDAN.
                */

                /* get buffer for msg to send to MAC
                */
                rlc_msg_ptr = mac_get_cmd_buf_rlc();
                if (rlc_msg_ptr != NULL)
                {
                  /* set FA to 0 in un-ack mode
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                                      RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

                  /*---------------------------------------------------------------
                  ** Formulate ssn & rbb array vars. Set signal and send packet dl
                  ** acknack RRBP and frame number and TFI
                  **---------------------------------------------------------------
                  */

                  /* Copy the Quality Report from PL1. If the Quality Report is dropped
                  ** while the EPDAN description is formatted (ESP 1/2), the following
                  ** pointer is set to NULL, in e_format_dl_acknack_desc(), to indicate
                  ** to MAC that the QReport should not be included in the EPDAN.
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr =
                    epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
                  #error code not present
#endif

                  e_format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req,
                                            epdan_req_ptr);
                  UPD_DL_ACKNACK(gas_id);
                  /* Fill the rest of the message */
                  rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->
                                                                        frame_no;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                                        include_chan_req;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl[gas_id].tfi;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->
                                                                        timeslot;

#ifdef  ERLD_SHOW_EPDAN
                  show_epdan (gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif
                  /* Log DL EPDAN
                  ** Log Code: 0x5211
                  */
                  rlc_log_epdan(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);

                  /* Populate the out-of-memory bit with value from L1 message (from FW)
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.ms_out_of_mem =
                    epdan_req_ptr->ms_out_of_mem;

                  /* Send EPDAN to MAC */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
                  mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

                  epdan_req_not_serviced = FALSE;

                  /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
                  #error code not present
#else
                  gprs_rlc_l1isr_free_buffer(gas_id, epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
                }
              } /* end if( epdan_req_ptr != NULL ) */
              else
              {
                MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_187, "ERLD EPDAN_REQ without sig in DPQueue");
              }
            } /*  if( !(IS_GT_MOD( epdan_req_ptr->bsn, dl[gas_id].vr, dl[gas_id].ws)) ) */
          } /*  end if( epdan_req_ptr == NULL ) */
        }  /*  end if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG ) */

        /* Now process the 2nd payload if present */
        tmp_data_block_ptr = &data_block2;

      } /* while( tmp_data_block_ptr && max_block_no-- ) */
    } /* end get valid DL block */

    loop_ctr--;
  } /* end while */

  if( epdan_req_not_serviced == TRUE )
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_188, "ERLD Did not respond to EPDAN_REQ ");
  }
}

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_STATE_PL1_EPDAN_REQ_EVENT_HANDLER
===
===  DESCRIPTION
===
===   Handler of PL1_EPDAN_REQ event in DS_RX_UNACK state in EGPRS mode
===   when DL fifo is empty.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_unack_state_pl1_epdan_req_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* Check if the EPDAN_REQ signal mask is set
  */
  /* Get message from the queue*/
  epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ(gas_id));

  if( epdan_req_ptr == NULL )
  {
    MSG_GRLC_HIGH_0_G( GRLC_F3_MSG_ENUM_191, "ERLD U EPDAN_REQ mask without msg");
  }
  else
  {
    GRLC_DL_MSG_LOCK(gas_id);

    if (q_cnt(&DPQ(gas_id)) == 0)
    {
      RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
    }

    GRLC_DL_MSG_UNLOCK(gas_id);

    /* Process EPDAN whether the block is in (BSN) range or not
    */

    /* get buffer for msg to send to MAC
    */
    rlc_msg_ptr = mac_get_cmd_buf_rlc();
    if (rlc_msg_ptr != NULL)
    {
      /* set FA to 0 in un-ack mode
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                        RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

      /*---------------------------------------------------------------
      ** Formulate ssn & rbb array vars. Set signal and send packet dl
      ** acknack RRBP and frame number and TFI
      **---------------------------------------------------------------
      */

      /* Copy the Quality Report from PL1. If the Quality Report is dropped
      ** while the EPDAN description is formatted (ESP 1/2), the following pointer
      ** is set to NULL, in e_format_dl_acknack_desc(), to indicate
      ** to MAC that the QReport should not be included in the EPDAN.
      */
#ifdef PERLUTF
      #error code not present
#endif
      rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr = epdan_req_ptr->qrep_ptr;

      e_format_dl_acknack_desc(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req, epdan_req_ptr);
      UPD_DL_ACKNACK(gas_id);

      /* Fill the rest of the message */
      rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->frame_no;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                            include_chan_req;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl[gas_id].tfi;
      rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->timeslot;

#ifdef  ERLD_SHOW_EPDAN
      show_epdan (gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif

      /* Log DL EPDAN
      ** Log Code: 0x5211
      */
      rlc_log_epdan(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req);

      /* Populate the out-of-memory bit with value from L1 message (from FW)
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.ms_out_of_mem =
        epdan_req_ptr->ms_out_of_mem;

      /* Send EPDAN to MAC */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
      mac_put_cmd_rlc(gas_id, rlc_msg_ptr);

      /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
      #error code not present
#else
      gprs_rlc_l1isr_free_buffer(gas_id, epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
    }
    else
    {
      ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
    }
  }/*  end if( epdan_req_ptr == NULL ) */

} /* e_ds_rx_unack_state_pl1_epdan_req_event_handler() */

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK_PEND_REL state in GPRS mode.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Already sent Packet Control Acknack but we want to check the data
  ** for the SP (Polling) bit set and valid RRBP. If they are set then
  ** send Packet Control Acknack request to MAC again. Ignore the payload
  ** by freeing it
  */
  while (grlc_dl_fifo_get_cnt(gas_id) > 0)
  {
    /* count is not 0. There's at least 1 block hence ignore return value
    */
    if (grlc_dl_fifo_get_block(gas_id, &pl1_blk_hdr_ptr[gas_id], &pl1_blk_dsm_ptr[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_192, "GRLD un-exp DL empty");
    }
    else
    {
      /* translate the over-the-air raw data into cur_dl_blk[gas_id]
      */
      xlate_ext_dl_data(gas_id, pl1_blk_hdr_ptr[gas_id], pl1_blk_dsm_ptr[gas_id], &cur_dl_blk[gas_id]);

      /* Update coding scheme for engg_mode
      */
      if (dl[gas_id].cs != cur_dl_blk[gas_id].cs)
      {
        geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk[gas_id].cs, TRUE, gas_id);
      }

      /* copy PL1 header info
      */
      dl[gas_id].cs = cur_dl_blk[gas_id].cs;

      dl[gas_id].sp   = cur_mac_hdr[gas_id].type.dl_data.sp;
      dl[gas_id].rrbp = cur_mac_hdr[gas_id].type.dl_data.rrbp;
      dl[gas_id].fbi  = cur_dl_blk[gas_id].fbi;

    #ifdef  GRLD_SHOW_BSN
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_193, "GRLD U BSN=%d SP=%d FBI=%d",dl[gas_id].bsn,dl[gas_id].sp,dl[gas_id].fbi);
    #endif
      if (dl[gas_id].fbi == RM_FBI_FINAL && dl[gas_id].sp == RM_SP_RRBP_VALID)
      {
        /* Stop T3190 when sending Pkt Ctrl Acknack
        */
        TIMER_STOP_T3190(gas_id);

        /* PL1 control the starting and re-starting of T3192 when it
        ** next sends a DL acknack msg with FAI bit set
        */

        /*---------------------------------------
        ** send DL packet control ack req to MAC
        **---------------------------------------
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

          /* rrbp from the current dl data block where polling bit is detected
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl[gas_id].rrbp;

          /* frame number carried over from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = dl[gas_id].frame_number;

          /* ..and time slot
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl[gas_id].time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = dl[gas_id].tfi;

          MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_194, "GRLD RX_U_P_R FBI Sent PCAck");
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end FBI Final & valid RRBP */

      GPRS_PDU_FREE(&cur_dl_blk[gas_id].dsm_payload_ptr);

    } /* end get valid block */
  } /* end while */

} /* ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_PEND_REL_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_UNACK_PEND_REL state in EGPRS mode.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id )
{

  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;
  grlc_dl_data_block_t  *tmp_data_block_ptr;

  /* if there is data block available
  */
  while (grlc_dl_fifo_get_cnt(gas_id) > 0)
  {
    if (e_grlc_dl_fifo_get_block(gas_id, &e_pl1_dl_blk[gas_id]) == DL_FIFO_EMPTY)
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_195, "ERLD U PH_DATA_IND with DL Q empty ");
    }
    else
    {
      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      /* re-start when receiving a new data block
      */
      TIMER_START_T3190(gas_id);

      /* Validate L1 DL block(s)
      */
      e_validate_dl_pl1_block( gas_id, FALSE );

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_xlate_ext_dl_data(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);

      /* Validate BSN number and FBI field  */
      if( data_block2.dsm_payload_ptr != NULL )
      {
        /* If dual payload with same BSN number for both payloads,
        ** or 1st payload has FBI set, then ignore 2nd payload.
        */
        if( (data_block1.bsn == data_block2.bsn) ||
            (data_block1.fbi == RM_FBI_FINAL)       )
        {

          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr );
          data_block2.dsm_payload_ptr = NULL;
        }
      }

      /* Start with the 1st DL data block */
      tmp_data_block_ptr = &data_block1;

      grlc_dl_diag_rx_stat_add_entry(&data_block1, gas_id);
      if (e_pl1_dl_blk[gas_id].mcs > RM_MCS_6)
      {
        grlc_dl_diag_rx_stat_add_entry(&data_block2, gas_id);
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block(gas_id, &e_pl1_dl_blk[gas_id], &data_block1, &data_block2);
#endif

      /* Already sent Packet Control Acknack but we want to check the data
      ** for the ESP (Polling) bit set and valid RRBP. If they are set then
      ** send Packet Control Acknack request to MAC again. Ignore the payload
      ** by freeing it
      */

      /* Save the coding scheme, frame number, time_slot, ES/P, RRBP and
      ** final block indicator from the data block just translated to the
      ** dl struct
      */
      dl[gas_id].bsn = tmp_data_block_ptr->bsn;

      /* Save the TFI for generating EPDAN later on
      */
      dl[gas_id].tfi = e_cur_hdr[gas_id].tfi;

      /* Update coding scheme for engg_mode
      */
      if (dl[gas_id].cs != tmp_data_block_ptr->cs)
      {
        geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &tmp_data_block_ptr->cs, TRUE, gas_id);
      }

      /* copy PL1 header info
      */
      dl[gas_id].cs = tmp_data_block_ptr->cs;

      dl[gas_id].sp   = e_cur_hdr[gas_id].esp;
      dl[gas_id].rrbp = e_cur_hdr[gas_id].rrbp;

      if (dl[gas_id].cs > RM_MCS_6 && dl[gas_id].cs <= RM_MCS_9)
      {
        dl[gas_id].fbi = data_block1.fbi | data_block2.fbi;
      }
      else
      {
        dl[gas_id].fbi = data_block1.fbi;
      }

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
      show_bsn_and_fifo_vars(gas_id);
#endif
      /* latch the FBI value if the current FBI (belonging to a block)
      ** is set and the latch value if it is NOT set
      */
      if (dl[gas_id].fbi == RM_FBI_FINAL)
      {
        dl[gas_id].fbi_latch = RM_FBI_FINAL;
      }

      /* Send PCA again.
      */
      if (dl[gas_id].fbi == RM_FBI_FINAL)
      {
        /* Stop T3190 when sending Pkt Ctrl Acknack
        */
        TIMER_STOP_T3190(gas_id);

        /*----------------------------------------------------
        ** Request MAC to send Packet Control Acknowledgement
        **----------------------------------------------------
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

          /* rrbp from the current dl data block where polling bit is detected
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl[gas_id].rrbp;

          /* frame number carried over from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = dl[gas_id].frame_number;

          /* ..and time slot
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl[gas_id].time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = dl[gas_id].tfi;

          MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_196, "GRLD RX_U FBI in PEND_REL Sent PCAck");
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.gas_id = gas_id;
          mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end FBI bit set */

      /* Now free DSM items convaying both payloads if present */
      if( data_block1.dsm_payload_ptr )
        GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);

      if( data_block2.dsm_payload_ptr )
        GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);

    } /* end get valid DL block */
  } /* end while */

} /* e_ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler() */

/*===========================================================================
===
===  FUNCTION     E_UPDATE_DL_UNACK_STATE_VARS()
===
===  DESCRIPTION
===
===    Update the V(R) and V(Q) state variables after a new block of
===    given BSN arrives. V(R) is updated first before V(Q) because the
===    conditions for updating V(Q) involves V(R).
===
===    input: Latest DL BSN, global dl (dl_ass, vr, vq, bsn).
===    output: dl (vr, vq) and Vn array
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
void e_update_dl_unack_state_vars ( gas_id_t gas_id, uint16 bsn )
{
  uint16 ind, temp_vq, temp_vr, b;

  /* When BSNs are out of order in the same frame, V(R) is not updated.
  ** In this case, use ass_end index to get the latest received BSN
  */
  if( dl[gas_id].ass_start != E_GRLC_INVALID_BSN )
  {
    temp_vr = MOD_ESNS(dl[gas_id].ass_end + 1);
  }
  else
  {
    temp_vr = MOD_ESNS( dl[gas_id].vr + 1 );
  }

  /* We need to re-sync V(R) & V(Q). V(R) shall be set to [ BSN' + 1 ]
  ** modulo SNS, where BSN' is the BSN of most recently received RLC
  ** data block. If [V(R) - V(Q)] modulo SNS > WS after updating V(R),
  ** then V(Q) is set to [V(R) - WS] modulo SNS.
  */

  /* Reset block receive status while shifting V(R).
  ** Reset up to (BSN + 1), the new updated value of V(R).
  */
  for( ind = 0; ind <= MOD_DIF_ESNS( MOD_ESNS(bsn+1), temp_vr); ind++ )
  {
    E_RX_ARR(gas_id, MOD_ESNS(temp_vr + ind)).vn = RXS_INVALID;
  }

  /* Update V(R)
  */
  dl[gas_id].vr = (uint16)MOD_ESNS(bsn + 1);
  UPD_DL_VR(gas_id);

  if (MOD_DIF_ESNS(bsn, dl[gas_id].vq) > dl[gas_id].ws )
  {
    /* Reset block receive status while shifting V(Q) */
    for( ind = 0; ind <= MOD_DIF_ESNS(dl[gas_id].vr, dl[gas_id].ws); ind++ )
    {
      E_RX_ARR(gas_id, MOD_ESNS(dl[gas_id].vq + ind)).vn = RXS_INVALID;
    }

    dl[gas_id].vq = (uint16)MOD_DIF_ESNS(dl[gas_id].vr, dl[gas_id].ws);
    UPD_DL_VQ(gas_id);
  }

  /* Shift V(Q) to point to the first block, in the receive,
  ** window with status RXS_INVALID in order to
  ** report a correct bitmap in the next EPDAN.
  */

  /* Use a copy of V(Q) since V(Q) gets altered in the loop */
  temp_vq = dl[gas_id].vq;

  for (b=0; b <= MOD_DIF_ESNS(dl[gas_id].vr, temp_vq); b++)
  {
    ind = MOD_ESNS(b + temp_vq);
    if( (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED) ||
        (E_RX_ARR(gas_id, ind).vn == RXS_RECEIVED_ASSEMBLED ) )
    {
      /* found another contiguous block with RXS_RECEIVED status.
      ** So, clear block state and advance V(Q)
      */
      E_RX_ARR(gas_id, dl[gas_id].vq).vn = RXS_INVALID;
      dl[gas_id].vq = MOD_ESNS(ind + 1);
      UPD_DL_VQ(gas_id);
    }
    else
    {
      /* break out of loop and do not alter V(Q) when the first RXS_INVALID
      ** block is met
      */
      break;
    }
  } /* end search V(Q) loop */
} /* e_update_dl_unack_state_vars */


/*===========================================================================
===
===  FUNCTION    E_FORMAT_DL_ACKNACK_DESC()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===    dl
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_format_dl_acknack_desc
(
  gas_id_t                   gas_id,
  rm_dl_pkt_acknack_req_t    *dl_acknack_ptr,
  const pl1_rlc_msg_t        *epdan_req_ptr
)
{
/* 7 bits for compressed Bitmap length field plus 1 bit for starting color code.
*/
#define  COMP_LEN_AND_SCC_IE      8

/* When the EGPRS Ack/Nack description IE doesn't fill the remaining part of
** the EPDAN message, then 8 bits are needed to indicate the length of the IE.
*/
#define  ACK_NACK_DESCR_LEN_IE    8

/* The minumim length of a EGPRS ACK/NACK Description IE is 16 bits:
** IE length indicator 1 bit, FAI 1 bit, BOW 1 bit, EOW 1 bit, SSN 11
** bits, Compressed bitmap length indicator 1 bit.
*/
#define  ACKNACK_DESCR_IE_MIN_LEN    16

/* Maximum default field length in acknack description IE with compression present
** MAX_LEN_COMP_PRESENT is 23:
** (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)+1(CRBB_PRESENT))
*/
#define MAX_LEN_COMP_PRESENT    23

/* Maximum default field length in acknack description IE without compression present
** MAX_LEN_COMP_NOT_PRESENT is 15:
** (11(SSN)+3(FBI,BOW,EOW)+1(CRBB_PRESENT))
*/
#define MAX_LEN_COMP_NOT_PRESENT    15

  /* Holds the number of bits available for reported bit map
  ** without including the Quality report.
  */
  uint16     rbm_no_qrep_len;

  /* Holds the number of bits available for reported bit map
  ** including the Quality report.
  */
  uint16     rbm_with_qrep_len;

  /* Available space in EGPRS ACK/NACK Description for reported bitmap */
  uint16     rbm_avl_space;

  /* Holds the full bitmap length according to rx_array V(N) state */
  uint16     bm_len;

  /* The ending BSN of the compressed bitmap
  */
  uint16     esn_crbb = 0;

  /* Remaining space between esn_crbb and V(R). It is used for URBB generation */
  uint16     esn_crbb_to_vr;

  /* Length of Uncompressed bitmap */
  uint16     urbb_len;

  /* Starting BSN for Uncompressed bitmap */
  uint16     urbb_sbsn;

  /* For a valid Compressed bitmap, is set to 8.
  ** 7 bits for compressed Bitmap length field plus 1 bit for starting color code.
  */
  uint8      crbb_optinal_ie_len = 0;

  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_197, "ERLD epdan len_no_qrep=%u len_with_qrep=%u",
          epdan_req_ptr->avail_len_no_qrep,
          epdan_req_ptr->avail_len_qrep,
          0);

  (void)memset( &(dl_acknack_ptr->e_acknack_desc.urbb[0]),0,sizeof(uint32)*8);
  (void)memset( &(dl_acknack_ptr->e_acknack_desc.crbb[0]),0,sizeof(uint32)*4);

  /* Initialize URBB len */
  dl_acknack_ptr->e_acknack_desc.urbb_len = 0;

  /* Initialize CRBB len */
  dl_acknack_ptr->e_acknack_desc.crbb_len = 0;

  /* Initialize optional fields indicators  */
  dl_acknack_ptr->e_acknack_desc.len_present  = not_valid;
  dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
  /* check if vr_max is withing the smaller allocated window space
  ** if the vr_max is withing the operating window space , ms_out_of memory
  ** is FALSE , otherwise its TRUE
  ** e.g physical WS = 256 , NW allocated WS = 512
  ** vq = 0 , Vr= 255, vr_max = 256 , vr_max is 260 , it is within operating ws of
  ** 512 but outside physical memory space. we need to set ms_out_mem = 1.
  */
  if(dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK)
  {
    if (EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl[gas_id].vr_max, dl[gas_id].ir_oper_win_size) )
    {
      if(dl[gas_id].ms_out_of_mem)
      {
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_198, "ERLD ms_out_mem RESET vq %d vr_max %d oper_ws %d",
                            dl[gas_id].vq,dl[gas_id].vr_max,dl[gas_id].ir_oper_win_size);
      }
      /* within window - ms_out_of_mem should be false */
      dl[gas_id].ms_out_of_mem  = FALSE;
    }
    else
    {
      /* if ms_out_of_mem already set , then do not print the message again */
      if(dl[gas_id].ms_out_of_mem)
      {
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_199, "ERLD ms_out_mem SET vq %d vr_max %d oper_ws %d",
                         dl[gas_id].vq,dl[gas_id].vr_max,dl[gas_id].ir_oper_win_size);
      }
      dl[gas_id].ms_out_of_mem  = TRUE;
    }
    dl_acknack_ptr->ms_out_of_mem = dl[gas_id].ms_out_of_mem;
  }
  else
  {
    /*
    ** ms out of mem is always FALSE for RLC UNACK MODE as no IR needed
    */
    dl_acknack_ptr->ms_out_of_mem = FALSE;
  }
#else  /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */
  /* ms out of mem is always FALSE */
  dl_acknack_ptr->ms_out_of_mem = FALSE;
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

  /* Derive the exact number of bits available for the reported bitmap (rbm).
  ** 16 bits are the mandatory fields in the EGPRS ACK/NACK Description.
  ** IE length indicator 1 bit, FAI 1 bit, BOW 1 bit, EOW 1 bit, SSN 11
  ** bits, Compressed bitmap length indicator 1 bit.
  */
  if( ( ( epdan_req_ptr->esp == ESP_FPB_NO_QREP ||
          epdan_req_ptr->esp == ESP_NPB_NO_QREP
        ) &&
        (epdan_req_ptr->avail_len_no_qrep <= ACKNACK_DESCR_IE_MIN_LEN
        )
      ) ||
      ( (epdan_req_ptr->esp == ESP_NPB_QREP &&
         epdan_req_ptr->avail_len_qrep <= ACKNACK_DESCR_IE_MIN_LEN
        )
      )
    )
  {
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_200, "GRLD No avail space for EPDAN desc len_no_qrep %d, len_qrep %d ",
              epdan_req_ptr->avail_len_no_qrep,
              epdan_req_ptr->avail_len_qrep,
              0);
    return;
  }

  rbm_no_qrep_len   = (epdan_req_ptr->avail_len_no_qrep > ACKNACK_DESCR_IE_MIN_LEN) ?
                      (epdan_req_ptr->avail_len_no_qrep - ACKNACK_DESCR_IE_MIN_LEN) :
                      0;
  rbm_with_qrep_len = (epdan_req_ptr->avail_len_qrep > ACKNACK_DESCR_IE_MIN_LEN ) ?
                      (epdan_req_ptr->avail_len_qrep - ACKNACK_DESCR_IE_MIN_LEN) :
                      0;

  if( dl[gas_id].vq == dl[gas_id].vr )
  {
#if 0
    /* All blocks have been received. No bitmap to report */
    if( dl[gas_id].state == DS_RX_UNACK )
      dl_acknack_ptr->e_acknack_desc.ssn = (dl[gas_id].vr + 1) % ESNS;
    else
#endif
      dl_acknack_ptr->e_acknack_desc.ssn = (dl[gas_id].vq + 1) % ESNS;

    dl_acknack_ptr->e_acknack_desc.bow = 1;
    dl_acknack_ptr->e_acknack_desc.eow = 1;
    dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

    /* set partial bitmap Sequence number. See TS 04.60 table 9.1.8.2.2.1 */
#if 0
    if( dl[gas_id].state == DS_RX_UNACK )
      dl[gas_id].pbsn = dl[gas_id].vr;
    else
#endif
      dl[gas_id].pbsn = dl[gas_id].vq;

    if( rbm_with_qrep_len == 0 )
    {
      /* EPDAN description fits precisely in the remaining space of the EPDAN message.
      ** No need to include the length of the EPDAN description.
      */
      dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
    }
    else if ( rbm_with_qrep_len > ACK_NACK_DESCR_LEN_IE )
    {
      /* EPDAN desc doesn't fill the remaining space of the EPDAN message precisely.
      ** The length of the EPDAN description needs to be included ( 8 bits field).
      ** 3 bits for fai/bow/eow, 11 bits for ssn
      ** and 1 bit for comp_present indicator. 15 bits total.
      */
      dl_acknack_ptr->e_acknack_desc.len_present = valid;
      dl_acknack_ptr->e_acknack_desc.length = MAX_LEN_COMP_NOT_PRESENT;
    }
    return;
  }

  if( ( ( (epdan_req_ptr->esp == ESP_FPB_NO_QREP) ||
          (epdan_req_ptr->esp == ESP_NPB_NO_QREP)
        ) && (!epdan_req_ptr->avail_len_no_qrep )
      ) ||
      ( (epdan_req_ptr->esp == ESP_NPB_QREP) &&
        (!epdan_req_ptr->avail_len_qrep)
      )
    )
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_201, "GRLD No avail space for EPDAN desc when vq != vr");
    return;
  }

  /* Calculate the bitmap length that we need to report.
  ** Note that if ES/P is 1, we need to report a full bitmap starting from V(Q).
  ** Otherwise, a partial bitmap is reported starting from PBSN.
  ** Note that V(Q) is not included in the reported bitmap.
  */
  if(epdan_req_ptr->esp == ESP_FPB_NO_QREP)
  {
    dl[gas_id].pbsn = dl[gas_id].vq;
    bm_len = MOD_DIF_ESNS( dl[gas_id].vr, MOD_ESNS(dl[gas_id].vq + 1) );
    dl_acknack_ptr->e_acknack_desc.bow = 1;
  }
  else
  {
    if( (!EGPRS_IS_VQ_LE_BSN_LT_VR(dl[gas_id].vq,MOD_ESNS(dl[gas_id].pbsn + 1),dl[gas_id].ws,dl[gas_id].vr)) ||
        ( MOD_ESNS(dl[gas_id].pbsn + 1) == dl[gas_id].vq )
      )
    {
      dl[gas_id].pbsn = dl[gas_id].vq;
      bm_len = MOD_DIF_ESNS( dl[gas_id].vr, MOD_ESNS(dl[gas_id].vq + 1) );
      dl_acknack_ptr->e_acknack_desc.bow = 1;
    }
    else
    {
      bm_len = MOD_DIF_ESNS( dl[gas_id].vr, MOD_ESNS(dl[gas_id].pbsn + 1) );

      /* If Partial Bitmap smaller than the available space, backtrack from PBSN
      ** and represent as much of the V(Q) to PBSN range as possible
      */
#ifdef GRLC_PBSN_BACKTRACKING_ON
      if( bm_len < rbm_with_qrep_len )
      {
        bm_len  = rbm_with_qrep_len;
        dl[gas_id].pbsn = MOD_ESNS( dl[gas_id].vr + ESNS - (rbm_with_qrep_len +1) );
      }
      else if( (bm_len < rbm_no_qrep_len) && (epdan_req_ptr->esp != ESP_NPB_QREP) )
      {
        bm_len  = rbm_no_qrep_len;
        dl[gas_id].pbsn = MOD_ESNS( dl[gas_id].vr + ESNS - (rbm_no_qrep_len +1) );
      }
#endif
      if( dl[gas_id].pbsn == dl[gas_id].vq )
        dl_acknack_ptr->e_acknack_desc.bow = 1; /* Backtracking reached V(Q) */
      else
        dl_acknack_ptr->e_acknack_desc.bow = 0; /* BOW is not part of the Partial bm */
    }
  }

  if( (bm_len + ACK_NACK_DESCR_LEN_IE) <= rbm_with_qrep_len )
  {
    /* Enough space to include full uncompressed bitmap and quality
    ** report even after including EGPRS ACK/NACK Description length
    ** indicator L ( 8 bits field )
    */
    dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
    dl_acknack_ptr->e_acknack_desc.eow = 1;
    dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

    /* EPDAN description doesn't fill precisely the remaining space of the EPDAN message.
    ** The length of the EPDAN description needs to be included ( 8 bits field).
    */
    dl_acknack_ptr->e_acknack_desc.len_present = valid;

    /* EPDAN desc len = 15 bits for the mandatory fields (starting from FAI) +
    ** uncompressed bitmap length
    */
    dl_acknack_ptr->e_acknack_desc.length = (uint8)(bm_len + MAX_LEN_COMP_NOT_PRESENT);

    /* Generate uncompressed bitmap */
    gen_urbb( gas_id,
              dl_acknack_ptr,
              dl_acknack_ptr->e_acknack_desc.ssn,
              bm_len
            );

    /* set partial bitmap Sequence number. See TS 04.60 table 9.1.8.2.2.1 */
    dl[gas_id].pbsn = dl[gas_id].vq;

    return;
  }
  else if( bm_len <= rbm_with_qrep_len )
  {
    /* Uncompressed bitmap and quality report fit in the remaining space.
    ** If not an exact fit then the remaining space will be filled up with "0"s
    ** as part of the URBB. The filling "0"s will be ignored by the peer since the
    ** fall outside the receiving window.
    */

    /* Fill and return the EGPRS ACK/NACK Description.
    */
    dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
    dl_acknack_ptr->e_acknack_desc.eow = 1;
    dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

    /* The EPDAN description fills the remaining space of the EPDAN
    ** massage precisely ( by adding "0"s in the URBB).
    ** The length of the EPDAN description is omitted.
    */
    dl_acknack_ptr->e_acknack_desc.len_present = not_valid;

    /* Generate uncompressed bitmap */
    gen_urbb( gas_id,
              dl_acknack_ptr,
              dl_acknack_ptr->e_acknack_desc.ssn,
              bm_len
            );

    /* set partial bitmap Sequence number. See TS 04.60 table 9.1.8.2.2.1 */
    dl[gas_id].pbsn = dl[gas_id].vq;

    return;
  }

  switch( epdan_req_ptr->esp )  /* see TS 04.60 table 9.1.8.2.1.1 */
  {
    case ESP_FPB_NO_QREP:
    case ESP_NPB_NO_QREP:  /*  May include quality report */
      {

        if( ( bm_len + ACK_NACK_DESCR_LEN_IE ) <= rbm_no_qrep_len )
        {
          /* Enough space to include full uncompressed bitmap even after including
          ** EGPRS ACK/NACK Description length indicator L ( 8 bits field ).
          ** Drop quality report.
          ** Note, for future improvement we can compress the bitmap so that the
          ** quality report can fit in the EPDAN, but this is not a "shall" when
          ** ES/P = 1 or 2.
          ** Fill EGPRS ACK/NACK Description IE as per (2) above.
          */
          dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
          dl_acknack_ptr->e_acknack_desc.bow = 1;
          dl_acknack_ptr->e_acknack_desc.eow = 1;
          dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

          /* The EPDAN description doesn't fill the remaining space of the EPDAN
          ** message precisely.
          ** The length of the EPDAN description needs to be included ( 8 bits field).
          */
          dl_acknack_ptr->e_acknack_desc.len_present = valid;

          /* EPDAN desc len = 15 bits for the mandatory fields (starting from FAI) +
          ** uncompressed bitmap length
          */
          dl_acknack_ptr->e_acknack_desc.length =
            (uint8)(MAX_LEN_COMP_NOT_PRESENT + bm_len);

          /* Generate uncompressed bitmap */
          gen_urbb(gas_id,
              dl_acknack_ptr,
              dl_acknack_ptr->e_acknack_desc.ssn,
              bm_len
            );

          /* set partial bitmap Sequence number. See TS 04.60 table 9.1.8.2.2.1 */
          dl[gas_id].pbsn = dl[gas_id].vq;

          /* NULL pointer to indicate to MAC that the Quality Report has been dropped */
#ifdef PERLUTF
          #error code not present
#endif
          dl_acknack_ptr->pl1_qrep_ptr = NULL;

          MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_202, "ELRD urbb dropped QR ES/P=%d", epdan_req_ptr->esp);
        }
        else if( bm_len <= rbm_no_qrep_len )
        {
          /* Full uncompressed bitmap fits in EPDAN desc. Drop quality report.
          ** Note, for future improvement we can compress the bitmap so that the
          ** quality report can fit in the EPDAN, but this is not a "shall" when
          ** ES/P = 1 or 2.
          ** Fill EGPRS ACK/NACK Description IE as per (1) above.
          */

          /* Uncompressed bitmap fits in the remaining space.
          ** If not an exact fit then the remaining space will be filled up with "0"s
          ** as part of the URBB. The filling "0"s will be ignored by the peer since
          ** they fall outside the receiving window.
          */

          /* Fill and return the EGPRS ACK/NACK Description.
          */
          dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
          dl_acknack_ptr->e_acknack_desc.bow = 1;
          dl_acknack_ptr->e_acknack_desc.eow = 1;
          dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

          /* The EPDAN description fills the remaining space of the EPDAN
          ** massage precisely ( by adding "0"s in the URBB).
          ** The length of the EPDAN description is omitted.
          */
          dl_acknack_ptr->e_acknack_desc.len_present = not_valid;

          /* Generate uncompressed bitmap */
          gen_urbb( gas_id,
              dl_acknack_ptr,
              dl_acknack_ptr->e_acknack_desc.ssn,
              bm_len
            );

          /* set partial bitmap Sequence number. See TS 04.60 table 9.1.8.2.2.1 */
          dl[gas_id].pbsn = dl[gas_id].vq;

          /* NULL pointer to indicate to MAC that the Quality Report has been dropped */
          dl_acknack_ptr->pl1_qrep_ptr = NULL;
#ifdef PERLUTF
          #error code not present
#endif
          MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_203, "ELRD urbb dropped QR ES/P=%d", epdan_req_ptr->esp);
        }
        else
        {
          MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_204, "ERLD CRBB active");

          /* Not enough space in the EPDAN desc for uncompressed bitmap.
          ** Attempt compressing the bit map.
          */

          /* Full bitmap does not fit in available space.
          ** Set SSN acording to TS 04.60 table 9.1.8.2.2.1
          */
          if( (epdan_req_ptr->esp == ESP_FPB_NO_QREP) ||
              ( (epdan_req_ptr->esp == ESP_NPB_NO_QREP) &&
                ( !EGPRS_IS_VQ_LE_BSN_LT_VR(dl[gas_id].vq,MOD_ESNS(dl[gas_id].pbsn + 1),dl[gas_id].ws,dl[gas_id].vr) ||
                  MOD_ESNS(dl[gas_id].pbsn + 1) == dl[gas_id].vq
                )
              )
            )
          {
            dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].vq + 1);
          }
          else
          {
            dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
          }

          if( dl_acknack_ptr->e_acknack_desc.ssn == MOD_ESNS(dl[gas_id].vq + 1) )
          {
            dl_acknack_ptr->e_acknack_desc.bow = 1;
          }
          else
          {
            dl_acknack_ptr->e_acknack_desc.bow = 0;
          }

          dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

          /* esn_crbb: the BSN of the last block covered by the compressed bitmap.
          ** Note that the following function might set dl_acknack_ptr->pl1_qrep_ptr to
          ** NULL to indicate to MAC that the Quality Report has been dropped
          */
          esn_crbb = gen_crbb(gas_id, dl_acknack_ptr, bm_len, rbm_no_qrep_len,
                                rbm_with_qrep_len, epdan_req_ptr->esp );

          /* If CRBB is valid, make sure that the optional CRBB
          ** related IEs are included in the message
          */
          if( dl_acknack_ptr->e_acknack_desc.crbb_len )
          {
            dl_acknack_ptr->e_acknack_desc.comp_present = valid;

            /* If CRBB is included, we need to take into account 7 bits for the
            ** Compressed Bitmap length field plus 1 bit for starting colour code
            */
            crbb_optinal_ie_len = COMP_LEN_AND_SCC_IE;
          }

          /* if esn_crbb does not coindcides with VR-1 then compressed bitmap does
          ** not cover the bm_len bits.
          */
          if (IS_GT_MOD(MOD_ESNS(dl[gas_id].vr + ESNS -1), esn_crbb,dl[gas_id].ws))
          {
            /* Calculate the remaining of the input bitmap after compression.
            ** Note that if no valid CRBB, then the reported bitmap starts from SSN
            */
            if( dl_acknack_ptr->e_acknack_desc.crbb_len )
            {
              esn_crbb_to_vr = MOD_DIF_ESNS(MOD_ESNS(dl[gas_id].vr + ESNS - 1), esn_crbb);
              urbb_sbsn = esn_crbb + 1;
            }
            else
            {
              esn_crbb_to_vr = MOD_DIF_ESNS(dl[gas_id].vr, dl_acknack_ptr->e_acknack_desc.ssn);
              urbb_sbsn = dl_acknack_ptr->e_acknack_desc.ssn;
            }

            /* Determine if QRep should be included or not and set the
            ** available space for reported bitmap.
            */
            if( (dl_acknack_ptr->e_acknack_desc.crbb_len +
                 esn_crbb_to_vr + crbb_optinal_ie_len
                ) <= rbm_with_qrep_len
              )
            {
              /* Keep Qrep. Reported bitmap fits in available space */
              rbm_avl_space = rbm_with_qrep_len;
            }
            else
            {
              /* Make sure that Qrep is dropped and try to fit bitmap in rbm_no_qrep_len
              */
              rbm_avl_space = rbm_no_qrep_len;

              /* NULL pointer to indicate to MAC that the Quality Report has been dropped */
              dl_acknack_ptr->pl1_qrep_ptr = NULL;

              MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_205, "ELRD urbb dropped QR ES/P=%d", epdan_req_ptr->esp);
            }

            /* Check if CRBB + Rest of the Input Bitmap fit in the available space.
            ** Note that if CRBB is valid, 8 bits are added for the CRBB len (7bits) and
            ** the colour code (1bit).
            */
            if( (dl_acknack_ptr->e_acknack_desc.crbb_len +
                 esn_crbb_to_vr + crbb_optinal_ie_len
                ) < rbm_avl_space
              )
            {
              if( (dl_acknack_ptr->e_acknack_desc.crbb_len + esn_crbb_to_vr +
                   crbb_optinal_ie_len + ACK_NACK_DESCR_LEN_IE
                  ) > rbm_avl_space
                )
              {
                /* CRBB + Rest of the Input Bitmap do not fit exactly in the available
                ** space and by adding 8 more bits for EGPRS ACK/NACK Description length
                ** indicator we go over the available space. Fill the rest of the
                ** available space with "0" as part of the URBB, to avoid using 8 more
                ** bits for EGPRS ACK/NACK Description length indicator.
                ** Note that any bsn in the reported bitmap that falls outside the
                ** receive window will be ignored. Therefore, the peer should ignore
                ** the "filling" "0" on the urbb.
                */
                gen_urbb( gas_id, dl_acknack_ptr,
                          urbb_sbsn,
                          esn_crbb_to_vr
                        );

                /* Set Partial BSN to V(Q). */
                dl[gas_id].pbsn = dl[gas_id].vq;

                /* V(R)-1 is included */
                dl_acknack_ptr->e_acknack_desc.eow = 1;

                /* The EPDAN description fills the remaining space of the EPDAN
                ** message precisely. The length of the EPDAN description is omitted.
                */
                dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
              }
              else
              {
                /* CRBB + URBB fits even after adding
                ** 8 more bits for EGPRS ACK/NACK Description length indicator.
                */
                gen_urbb( gas_id, dl_acknack_ptr,
                          urbb_sbsn,
                          esn_crbb_to_vr
                        );

                dl[gas_id].pbsn = dl[gas_id].vq;

                /* V(R)-1 is now included in the bitmap */
                dl_acknack_ptr->e_acknack_desc.eow = 1;

                /* The EPDAN description does not fill the remaining space of the EPDAN
                ** message precisely. The length of the EPDAN description needs to be
                ** included ( 8 bits field).
                */
                dl_acknack_ptr->e_acknack_desc.len_present = valid;

                if( dl_acknack_ptr->e_acknack_desc.comp_present )
                {
                  /* EPDAN desc len = {MAX_LEN_COMP_PRESENT which is ..}
                  ** 23 (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)+1(CRBB_PRESENT)) +
                  ** CRBB + URBB
                  */
                  dl_acknack_ptr->e_acknack_desc.length =
                    (uint8)(MAX_LEN_COMP_PRESENT +
                            dl_acknack_ptr->e_acknack_desc.crbb_len +
                            esn_crbb_to_vr
                           );
                }
                else
                {
                  /* EPDAN desc len = 15 (11(SSN)+3(FBI,BOW,EOW)+1(CRBB_PRESENT)) +
                  ** URBB
                  */
                  dl_acknack_ptr->e_acknack_desc.length =
                    (uint8)(MAX_LEN_COMP_NOT_PRESENT + esn_crbb_to_vr);
                }

              }/* if( (dl_acknack_ptr->e_acknack_desc.crbb_len + ...
               **       ACK_NACK_DESCR_LEN_IE) > rbm_no_qrep_len
               **   )
               */

            } /* (dl_acknack_ptr->e_acknack_desc.crbb_len + ... ) < rbm_no_qrep_len */

            else if( (dl_acknack_ptr->e_acknack_desc.crbb_len +
                      esn_crbb_to_vr + crbb_optinal_ie_len) == rbm_avl_space )
            {
              /* CRBB + Rest of the Input Bitmap fit exactly in the available space */

              gen_urbb(gas_id, dl_acknack_ptr,
                       urbb_sbsn,
                       esn_crbb_to_vr
                      );

              dl[gas_id].pbsn = dl[gas_id].vq;

              /* V(R)-1 is now included in the bitmap */
              dl_acknack_ptr->e_acknack_desc.eow = 1;

              /* The EPDAN description fill the remaining space of the EPDAN message
              ** precisely. The length of the EPDAN description is omitted.
              */
              dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
            }
            else
            {
              /* CRBB + Rest of the Input Bitmap exceed the available space.
              ** A partial bitmap will be formed. Calculate the length of the URBB
              ** so that CRBB + URBB will fit exactly the available space.
              ** Note that we deduct 8bits for the CRBB len (7bits) and the colour
              ** code (1bit).
              */
              urbb_len =  (rbm_no_qrep_len -
                           dl_acknack_ptr->e_acknack_desc.crbb_len
                          ) -
                          crbb_optinal_ie_len;

              gen_urbb( gas_id, dl_acknack_ptr,
                        urbb_sbsn,
                        urbb_len
                      );

              /* Set Partial BSN to the last BSN of the URBB. */
              dl[gas_id].pbsn = MOD_ESNS( esn_crbb + urbb_len );

              /* Partial bitmap. V(R)-1 is not included */
              dl_acknack_ptr->e_acknack_desc.eow = 0;

              /* The EPDAN description fill the remaining space of the EPDAN
              ** message precisely. The length of the EPDAN description is omitted.
              */
              dl_acknack_ptr->e_acknack_desc.len_present = not_valid;

            } /* if( (dl_acknack_ptr->e_acknack_desc.crbb_len ....) < rbm_no_qrep_len )
              */
          }
          else
          {
            /* 2) esn_crbb is beyond VR-1: generate warning
            */
            if (IS_GT_MOD(esn_crbb, MOD_ESNS(dl[gas_id].vr + ESNS - 1),dl[gas_id].ws))
            {
              MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_206, "ERLD epdan ESN_CRBB(%u) beyond VR-1 (%u)",
                        esn_crbb,
                        MOD_ESNS(dl[gas_id].vr + ESNS - 1),
                        0);

              return;
            }
            else
            {
              /* vr-1 == esn_crbb: crbb covers bm_len bits. No need to generate URBB.
              */
              dl[gas_id].pbsn                                     = dl[gas_id].vq;
              dl_acknack_ptr->e_acknack_desc.eow          = 1;

              /* Determine if QRep should be included or not and set the
              ** available space for reported bitmap.
              */
              if( (dl_acknack_ptr->e_acknack_desc.crbb_len + crbb_optinal_ie_len
                  ) <= rbm_with_qrep_len
                )
              {
                /* Keep Qrep. Reported bitmap fits in available space */
                rbm_avl_space = rbm_with_qrep_len;
              }
              else
              {
                /* Make sure that Qrep is dropped and try to fit bitmap in rbm_no_qrep_len
                */
                rbm_avl_space = rbm_no_qrep_len;

                /* NULL pointer to indicate to MAC that the Quality Report has been dropped */
                dl_acknack_ptr->pl1_qrep_ptr = NULL;

                MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_207, "ERLD urbb dropped QR ES/P=%d", epdan_req_ptr->esp);
              }

              if( (dl_acknack_ptr->e_acknack_desc.crbb_len + crbb_optinal_ie_len +
                    ACK_NACK_DESCR_LEN_IE) <= rbm_avl_space )
              {
                /* The EPDAN description doesn't fill the remaining space of the EPDAN
                ** message precisely.
                ** The length of the EPDAN description can be included ( 8 bits field)
                ** without exceeding the available space.
                */
                dl_acknack_ptr->e_acknack_desc.len_present = valid;

                if( dl_acknack_ptr->e_acknack_desc.comp_present )
                {
                  /* EPDAN desc len = 23 (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)
                  ** +1(CRBB_PRESENT)) +
                  ** CRBB_LEN
                  */
                  dl_acknack_ptr->e_acknack_desc.length =
                    MAX_LEN_COMP_PRESENT + dl_acknack_ptr->e_acknack_desc.crbb_len;
                }
                else
                {
                  /* EPDAN desc len = 15 (11(SSN)+3(FBI,BOW,EOW)+1(CRBB_PRESENT)) */
                  dl_acknack_ptr->e_acknack_desc.length = MAX_LEN_COMP_NOT_PRESENT;

                  MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_208, "ERLD No URBB/CRBB. Implies V(R)=V(Q)!!");
                }
              }
              else
              {
                /* The EPDAN description fill the remaining space of the EPDAN
                ** massage precisely ( by adding "0"s in the URBB).
                ** The length of the EPDAN description is omitted.
                */
                dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
              }
            }
          } /* if (IS_GT_MOD(MOD_ESNS(dl[gas_id].vr + ESNS -1), esn_crbb,dl[gas_id].ws)) */
        }
        break;
      } /* case esp = ESP_FPB_NO_QREP (1) or ESP_NPB_NO_QREP (2) */

    case ESP_NPB_QREP:  /* Shall include Channel quality report */
      {
        /* We have to fit the reported bitmap to rbm_with_qrep_len by using compression.
        ** Note that we have dealt with bm_len <= rbm_with_qrep_len
        ** before the switch statement above.
        */

        /* Full bitmap does not fit in available space.
        ** Set SSN acording to TS 04.60 table 9.1.8.2.2.1
        */
        if( !EGPRS_IS_VQ_LE_BSN_LT_VR(dl[gas_id].vq,MOD_ESNS(dl[gas_id].pbsn + 1),dl[gas_id].ws,dl[gas_id].vr) ||
             MOD_ESNS(dl[gas_id].pbsn + 1) == dl[gas_id].vq
          )
        {
          dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].vq + 1);
        }
        else
        {
          dl_acknack_ptr->e_acknack_desc.ssn = MOD_ESNS(dl[gas_id].pbsn + 1);
        }

        if( dl_acknack_ptr->e_acknack_desc.ssn == MOD_ESNS(dl[gas_id].vq + 1) )
        {
          dl_acknack_ptr->e_acknack_desc.bow = 1;
        }
        else
        {
          dl_acknack_ptr->e_acknack_desc.bow = 0;
        }

        dl_acknack_ptr->e_acknack_desc.comp_present = not_valid;

        /* esn_crbb: the BSN of the last block covered by the compressed bitmap.
        ** Note that the following function might set dl_acknack_ptr->pl1_qrep_ptr to
        ** NULL to indicate to MAC that the Quality Report has been dropped
        */
        esn_crbb = gen_crbb(gas_id, dl_acknack_ptr, bm_len, rbm_with_qrep_len,
                              rbm_with_qrep_len, epdan_req_ptr->esp);

        /* Set the optional bit in the PDAN descritption IE */
        if( dl_acknack_ptr->e_acknack_desc.crbb_len )
        {
          dl_acknack_ptr->e_acknack_desc.comp_present = valid;

          /* If CRBB is included, we need to take into account 7 bits for the
          ** Compressed Bitmap length field plus 1 bit for starting colour code
          */
          crbb_optinal_ie_len = COMP_LEN_AND_SCC_IE;
        }

        /* if esn_crbb does not coindcides with VR-1 then compressed bitmap does
        ** not cover the bm_len bits.
        */
        if (IS_GT_MOD(MOD_ESNS(dl[gas_id].vr + ESNS -1), esn_crbb,dl[gas_id].ws))
        {
          /* Calculate the remaining of the input bitmap after compression.
          ** Note that if no valid CRBB, then the reported bitmap starts from SSN
          */
          if( dl_acknack_ptr->e_acknack_desc.crbb_len )
          {
            esn_crbb_to_vr = MOD_DIF_ESNS(dl[gas_id].vr, esn_crbb);
            urbb_sbsn = esn_crbb + 1;
          }
          else
          {
            esn_crbb_to_vr = MOD_DIF_ESNS(MOD_ESNS(dl[gas_id].vr + ESNS - 1),
                                          dl_acknack_ptr->e_acknack_desc.ssn);
            urbb_sbsn = dl_acknack_ptr->e_acknack_desc.ssn;
          }

          /* Check if CRBB + Rest of the Input Bitmap fit in the available space.
          ** Note that we add 8 for the CRBB len (7bits) and the colour code (1bit).
          */
          if( (dl_acknack_ptr->e_acknack_desc.crbb_len + esn_crbb_to_vr +
                crbb_optinal_ie_len) < rbm_with_qrep_len )
          {
            if( (dl_acknack_ptr->e_acknack_desc.crbb_len + esn_crbb_to_vr +
                  crbb_optinal_ie_len + ACK_NACK_DESCR_LEN_IE) > rbm_with_qrep_len )
            {
              /* CRBB + Rest of the Input Bitmap do not fit exactly in the available
              ** space and by adding 8 more bits for EGPRS ACK/NACK Description length
              ** indicator we go over the available space. Fill the rest of the available
              ** space with "0" as part of the URBB, to avoid using 8 more bits for
              ** EGPRS ACK/NACK Description length indicator. Note that any bsn in the
              ** reported bitmap that falls outside the receive window will be ignored.
              ** Therefore, the peer should ignore the "filling" "0" on the urbb.
              */
              gen_urbb( gas_id, dl_acknack_ptr,
                        urbb_sbsn,
                        esn_crbb_to_vr
                      );

              /* Set Partial BSN to V(Q). */
              dl[gas_id].pbsn = dl[gas_id].vq;

              /* V(R)-1 is included */
              dl_acknack_ptr->e_acknack_desc.eow = 1;

              /* The EPDAN description fills the remaining space of the EPDAN
              ** message precisely. The length of the EPDAN description is omitted.
              */
              dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
            }
            else
            {
              /* CRBB + URBB fits even after adding
              ** 8 more bits for EGPRS ACK/NACK Description length indicator.
              */
              gen_urbb( gas_id, dl_acknack_ptr,
                        urbb_sbsn,
                        esn_crbb_to_vr
                      );

              dl[gas_id].pbsn = dl[gas_id].vq;

              /* V(R)-1 is now included in the bitmap */
              dl_acknack_ptr->e_acknack_desc.eow = 1;

              /* The EPDAN description does not fill the remaining space of the EPDAN
              ** message precisely. The length of the EPDAN description needs to be
              ** included ( 8 bits field).
              */
              dl_acknack_ptr->e_acknack_desc.len_present = valid;

              if( dl_acknack_ptr->e_acknack_desc.comp_present )
              {
                /* EPDAN desc len = 23 (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)
                **  +1(CRBB_PRESENT)) + CRBB + URBB
                */
                dl_acknack_ptr->e_acknack_desc.length =
                  (uint8)(MAX_LEN_COMP_PRESENT +
                  dl_acknack_ptr->e_acknack_desc.crbb_len +
                  esn_crbb_to_vr);
              }
              else
              {
                /* EPDAN desc len = 15 (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)+
                ** 1(CRBB_PRESENT)) + URBB
                */
                dl_acknack_ptr->e_acknack_desc.length =
                  (uint8)(MAX_LEN_COMP_NOT_PRESENT + esn_crbb_to_vr);
              }

            }/* if( (dl_acknack_ptr->e_acknack_desc.crbb_len + ...
             **      ACK_NACK_DESCR_LEN_IE) > rbm_with_qrep_len
             **   )
             */

          } /* (dl_acknack_ptr->e_acknack_desc.crbb_len + .... ) < rbm_with_qrep_len */

          else if( (dl_acknack_ptr->e_acknack_desc.crbb_len + esn_crbb_to_vr +
                      crbb_optinal_ie_len) == rbm_with_qrep_len )
          {
            /* CRBB + Rest of the Input Bitmap fit exactly in the available space */

            gen_urbb( gas_id, dl_acknack_ptr,
                      urbb_sbsn,
                      esn_crbb_to_vr
                    );

            dl[gas_id].pbsn = dl[gas_id].vq;

            /* V(R)-1 is now included in the bitmap */
            dl_acknack_ptr->e_acknack_desc.eow = 1;

            /* EPDAN description fill the remaining space of the EPDAN message precisely.
            ** The length of the EPDAN description is omitted.
            */
            dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
          }
          else
          {
            /* CRBB + Rest of the Input Bitmap exceed the available space.
            ** A partial bitmap will be formed. Calculate the length of the URBB
            ** so that CRBB + URBB will fit exactly the available space.
            ** Note that we deduct 8bits for the CRBB len (7bits) and the colour code
            ** (1bit).
            */
            urbb_len = (rbm_with_qrep_len -
                        dl_acknack_ptr->e_acknack_desc.crbb_len
                       ) -
                       crbb_optinal_ie_len;

            gen_urbb( gas_id, dl_acknack_ptr,
                      urbb_sbsn,
                      urbb_len
                    );

            /* Set Partial BSN to the last BSN of the URBB. */
            dl[gas_id].pbsn = MOD_ESNS( esn_crbb + urbb_len );

            /* Partial bitmap. V(R)-1 is not included */
            dl_acknack_ptr->e_acknack_desc.eow = 0;

            /* EPDAN description fill the remaining space of the EPDAN message precisely.
            ** The length of the EPDAN description is omitted.
            */
            dl_acknack_ptr->e_acknack_desc.len_present = not_valid;

          } /* if((dl_acknack_ptr->e_acknack_desc.crbb_len + ...) < rbm_with_qrep_len )*/
        }
        else
        {
          /*
          ** 2) esn_crbb is beyond VR-1: generate warning
          */
          if (IS_GT_MOD(esn_crbb, MOD_ESNS(dl[gas_id].vr + ESNS -1),dl[gas_id].ws))
          {
            MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_209, "ERLD epdan ESN_CRBB(%u) beyond VR-1 (%u)",
              esn_crbb, MOD_ESNS(dl[gas_id].vr + ESNS -1));

            return;
          }
          else
          {
            /* vr-1 == esn_crbb: crbb covers bm_len bits. No need to generate URBB.
            */
            dl[gas_id].pbsn                                     = dl[gas_id].vq;
            dl_acknack_ptr->e_acknack_desc.eow          = 1;

            if( (dl_acknack_ptr->e_acknack_desc.crbb_len +
                  crbb_optinal_ie_len + ACK_NACK_DESCR_LEN_IE) <= rbm_with_qrep_len )
            {
              /* The EPDAN description doesn't fill the remaining space of the EPDAN
              ** message precisely. The length of the EPDAN description can be
              ** included ( 8 bits field) without exceeding the available space.
              */
              dl_acknack_ptr->e_acknack_desc.len_present = valid;

              if( dl_acknack_ptr->e_acknack_desc.comp_present )
              {
                /* EPDAN desc len = 23 (11(SSN)+3(FBI,BOW,EOW)+1(COLOUR)+7(CRBB_LEN)+
                ** 1(CRBB_PRESENT)) + CRBB
                */
                dl_acknack_ptr->e_acknack_desc.length =
                    MAX_LEN_COMP_PRESENT + dl_acknack_ptr->e_acknack_desc.crbb_len;
              }
              else
              {
                /* EPDAN desc len = 15 (11(SSN)+3(FBI,BOW,EOW)+1(CRBB_PRESENT)) */
                dl_acknack_ptr->e_acknack_desc.length = MAX_LEN_COMP_NOT_PRESENT;

                MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_210, "ERLD No URBB/CRBB. Implies V(R)=V(Q)!!");
              }
            }
            else
            {
              /* The EPDAN description fill the remaining space of the EPDAN
              ** massage precisely ( by adding "0"s in the URBB).
              ** The length of the EPDAN description is omitted.
              */
              dl_acknack_ptr->e_acknack_desc.len_present = not_valid;
            }
          }
        } /* if (IS_GT_MOD(MOD_ESNS(dl[gas_id].vr + ESNS -1), esn_crbb,dl[gas_id].ws)) */
      }
      break;   /* case esp = ESP_NPB_QREP (3) */

    default:
      break;
  }   /* end  switch ( esp ) */

} /* e_format_dl_acknack_desc */

/*===========================================================================
===
===  FUNCTION    GRLC_DL_SCHEDULE_PKT_ACK_NACK_TRANSMISSION()
===
===  DESCRIPTION
===
===    Processes the SP indication of the current received radio block to
===    determine whether the Packet ACK/NACK Description Request should be
===    requested immediately, or held pending until the last BSN in the
===    current radio block period has been received.
===
===    This decision is essential when interacting with a BSS that does not
===    order it's BSNs in line with the time slot number (eg Motorola BSS).
===    Failure to deal with this results in false NAKed block indications,
===    unnecessary retransmission, and reduced throughput.
===
===  DEPENDENCIES
===
===    dl
===
===  RETURN VALUE
===
===    TRUE if PKT ACK/NACK transmission is required, FALSE otherwise.
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean grlc_dl_schedule_pkt_ack_nack_transmission( gas_id_t gas_id )
{
  boolean request_pkt_acknack = FALSE;

  /* --------------------------------------------------------------------- */

  if ( dl[gas_id].sp == RM_SP_RRBP_VALID )
  {
    /* The current block being processed is polled, so add to the poll
    ** queue so that all polls can be serviced at once when the receive
    ** bitmap would be truly representative of the receive state for this
    ** polling period.
    */
    if ( dl[gas_id].poll_queue_index < GRLC_DL_MAX_QUEUED_POLLS )
    {
      dl[gas_id].poll_queue[dl[gas_id].poll_queue_index].rrbp         = dl[gas_id].rrbp;
      dl[gas_id].poll_queue[dl[gas_id].poll_queue_index].frame_number = dl[gas_id].frame_number;
      dl[gas_id].poll_queue[dl[gas_id].poll_queue_index].time_slot    = dl[gas_id].time_slot;

      dl[gas_id].poll_queue_index++;
    }
    else
    {
      MSG_GRLC_ERROR_1_G( GRLC_F3_MSG_ENUM_211, "GRLD Poll Queue Overflow %d", dl[gas_id].poll_queue_index);
      dl[gas_id].poll_queue_index = 0;

      return (request_pkt_acknack);                              /* RETURN */
    }
  }

  /* --------------------------------------------------------------------- */

  if ( grlc_dl_fifo_ref_blk_hdr(gas_id, &pl1_blk_hdr_ptr[gas_id]) == DL_FIFO_REF_OK )
  {
    if ( pl1_blk_hdr_ptr[gas_id] -> frame_number != dl[gas_id].frame_number )
    {
      /* Prepare packet ack/nak description now, as this BSN
      ** is the last block received in the current radio block period.
      */
      if ( dl[gas_id].poll_queue_index > 0 )
        request_pkt_acknack = TRUE;
    }
  }
  else
  {
    /* Prepare packet ack/nak description now, as this BSN
    ** is the last block received in the FIFO.
    */
    if ( dl[gas_id].poll_queue_index > 0 )
      request_pkt_acknack = TRUE;
  }

  /* --------------------------------------------------------------------- */

  return (request_pkt_acknack);

} /* end of grlc_dl_schedule_pkt_ack_nack_transmission() */

/*===========================================================================
===
===  FUNCTION    GRLC_DL_REQUEST_PKT_ACK_NACK_TRANSMISSION()
===
===  DESCRIPTION
===
===    Prepare a RM_DL_PKT_ACKNACK_REQ message for each queued SP (Poll)
===    and send this to MAC.
===
===    Also monitor FBI indication and enter into pending release state if
===    all blocks have been received.
===
===  DEPENDENCIES
===
===    dl
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_request_pkt_ack_nack_transmission( gas_id_t gas_id )
{
  rlc_mac_msg_t  *rlc_msg_ptr;
  uint32         poll_index;

  /* --------------------------------------------------------------------- */

  /* Format the DL ACK/NACK description from the receive state variables.
  ** This will be copied to each of the request messages to MAC.
  */
  format_dl_acknack_desc( gas_id, &dl[gas_id].acknack );

  /* --------------------------------------------------------------------- */

  /* For each of the queued polling instances for this polling period,
  ** format a DL PKT ACK/NACK transmission request message.
  */
  for
  (
    poll_index = 0;
    (
      (poll_index < dl[gas_id].poll_queue_index     ) &&
      (poll_index < GRLC_DL_MAX_QUEUED_POLLS)
    );
    poll_index++
  )
  {
    /* get buffer for msg to send to MAC and set message identity
    */
    rlc_msg_ptr = mac_get_cmd_buf_rlc();
    if (rlc_msg_ptr != NULL)
    {
      rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;

      /* Copy pre-formatted DL ACK/NACK Description to message buffer. */

      (void)memscpy(
        &rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc,
        sizeof(rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc),
        &dl[gas_id].acknack.acknack_desc, sizeof(rm_acknack_desc_t) );

      /* If Final Block and RRBP is valid. Must send dl
      ** acknack msg here but first check if all blocks
      ** have been received.
      ** 9.3.2.5
      ** vq=vr & fbi means all data blocks have been received
      */
      if (dl[gas_id].fbi_latch == RM_FBI_FINAL && dl[gas_id].vr == dl[gas_id].vq)
      {
        /* Stop T3190 when sending paknak with FAI set
        */
        TIMER_STOP_T3190(gas_id);

        /* PL1 control the starting and re-starting of T3192 when it
        ** next sends a DL acknack msg with FAI bit set
        */

        /* Now wait in release pending
        */
        if ( dl[gas_id].state != DS_RX_ACK_PENDING_RELEASE )
        {
          dl[gas_id].state = DS_RX_ACK_PENDING_RELEASE;
          UPD_DL_STATE(gas_id);
        }

        dl[gas_id].test_mode = TEST_MODE_OFF;

        /*MSG_LOW("GRLD RX_A sp FAI P_REL vq=%d vr=%d bsn=%d",dl[gas_id].vq,dl[gas_id].vr,dl[gas_id].bsn);*/

        /* set FAI of packet dl acknack
        ** ! formulate acknack msg
        */
        rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai =
                  RM_ACKNACK_FAI_TBF_COMPLETE;

      } /* end tbf complete */
      else
      {
        /* all data blocks have not been received.
        ** Set FAI = 0 in packet dl acknack
        */
        rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai =
                  RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

        /*MSG_LOW("GRLD RX_A sp vq=%d vr=%d ",dl[gas_id].vq,dl[gas_id].vr,0);*/
      } /* end tbf NOT complete */

      /* Compile the following parameters PKT Ack/Nak Request:
      **
      ** - RRBP from the polled radio block
      ** - FRAME NUMBER on which the polled radio block was received
      ** - TIME SLOT on which the polled radio block was received
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp =
        dl[gas_id].poll_queue[poll_index].rrbp;

      rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number =
        dl[gas_id].poll_queue[poll_index].frame_number;

      rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot =
        dl[gas_id].poll_queue[poll_index].time_slot;

      /* ..and TFI
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi = dl[gas_id].tfi;

      /* Perform any DIAG activities.
      */

      UPD_DL_ACKNACK(gas_id);

      show_dl_rbb(gas_id, &rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc,
        rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai);

      /* Send message to MAC
      */
      rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id = gas_id;
      mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("GRLC heap exhaustion");
    }
  } /* for loop */

  /* --------------------------------------------------------------------- */

  /* Now reset the poll queue index as all queued polls have now
  ** been serviced.
  */
  dl[gas_id].poll_queue_index = 0;

  /* --------------------------------------------------------------------- */

} /* end of grlc_dl_request_pkt_ack_nack_transmission() */

/*===========================================================================
===
===  FUNCTION      CLEAR_RECEIVE_STATUS()
===
===  DESCRIPTION
===
===    Clear receive status of data blocks in the receive array from 'start' to
===    'end' - 1 for both GPRS and EGPRS modes
===
===  DEPENDENCIES
===
===    'start' and 'end' represent the start and end points within the RX array
===    MODULO SNS
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void clear_receive_status
(
  gas_id_t  gas_id,
  uint16    start,
  uint16    end
)
{
  uint16  ind, b;

  MSG_GRLC_LOW_2_G(GRLC_F3_MSG_ENUM_212, "GRLD clr status %d to %d-1",start,end);

  if (dl[gas_id].tbf_mode == TBF_MODE_EGPRS)
  {
    for (b=0; b < MOD_DIF_ESNS(end, start); b++)
    {
      ind = MOD_ESNS(b + start);
      E_RX_ARR(gas_id, ind).vn = RXS_INVALID;

      /* Reset failed_crc flag */
      E_RX_ARR(gas_id, ind).failed_crc = FALSE;
    }
  }
  else if (dl[gas_id].tbf_mode == TBF_MODE_GPRS)
  {
    for (b=0; b<MOD_DIF(end, start, SNS); b++)
    {
      ind = MOD_SNS(b + start);
      RX_ARR(gas_id, ind).vn = RXS_INVALID;
    }
  }
  else
  {
    MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_213, "GRLD clr status bad mode");
  }
} /* clear_receive_status */


#ifdef FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      grlc_dl_process_blind_data_blocks()
===
===  DESCRIPTION
===
===  It is used to process Blind Data Block stored as per the TBF MODE /
===  RLCMODE when PSHO is Successfull.
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
static void grlc_dl_process_blind_data_blocks( gas_id_t gas_id )
{
  if (dl[gas_id].psho_tbf_mode == TBF_MODE_GPRS)
  {
    if(dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK)
    {
      ds_rx_ack_state_pl1_ph_data_ind_event_handler(gas_id);
    }
    else
    {
      ds_rx_unack_state_pl1_ph_data_ind_event_handler(gas_id);
    }
  }
  else
  {
    if(dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK)
    {
      e_ds_rx_ack_state_pl1_ph_data_ind_event_handler(gas_id);
    }
    else
    {
      e_ds_rx_unack_state_pl1_ph_data_ind_event_handler(gas_id);
    }
  }

#ifdef  ERLD_SHOW_DL_VARS
  e_ds_rx_ack_state_pl1_epdan_req_event_handler(gas_id);
#endif /* ERLD_SHOW_DL_VARS */

} /* grlc_dl_process_blind_data_blocks */

/*===========================================================================
===
===  FUNCTION      grlc_handle_dl_psho_complete_ind_null_and_release_state()
===
===  DESCRIPTION
===
===  On Reception of DL_PSHO_COMPLETE_IND in DS_NULL/DS_PENDING_REL
===  if the PSHO is FAILURE -  Blind Data blocks are deleted.
===  if the PSHO is SUCCESS ,  TBF is inialized and blind data blocks processed.
===
===
===
===  DEPENDENCIES
===
===    dl_ass_ind_present when new TBF and when T3192 running and complete_ind
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_handle_dl_psho_complete_ind_null_and_release_state
(
  gas_id_t                        gas_id,
  const mr_dl_psho_complete_ind_t *dl_psho_complete_ind
)
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t  *rlc_msg_ptr;

  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_214, "GRLD PSHO MR_DL_PSHO_COMPLETE_IND in psho_status=%d psho_type=%d reset_flag=%d",
                dl_psho_complete_ind->psho_status,
                dl_psho_complete_ind->psho_type,
                dl_psho_complete_ind->reset_flag);

  /* Stop T3192
  */
  grlc_dl_stop_t3192(gas_id);

  /* if psho is successful , then rlc need to process the downlink data blocks
  ** else delete them,
  */
  if (dl_psho_complete_ind->psho_status == PSHO_SUCCESS)
  {
    /* as we are in DS_NULL or DS_REL, we can process the dl assignment
    ** in the MR_DL_PSHO_COMPLETE_IND like receiving a new packet downlink
    ** assignment.
    */
    if ( dl[gas_id].state == DS_RX_ACK_PENDING_RELEASE  ||
         dl[gas_id].state == DS_RX_UNACK_PENDING_RELEASE )
    {
      grlc_dl_fifo_clear(gas_id);
      grlc_dl_init(gas_id);
    }

    if ( dl_psho_complete_ind->dl_ass_ind_present &&
         (dl_psho_complete_ind->dl_ass_ind.tbf_mode !=
          dl[gas_id].psho_tbf_mode
         )
       )
    {
      MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_215, "GRLD NULL/REL ##tbf mode mismatch for ps handover");
    }

    /* In XFER , MAC does not give dl_ass_ind for GPRS TBF */
    if (dl_psho_complete_ind->dl_ass_ind_present)
    {
      /* store assignment info , dl_ass_ind_present = TRUE from MAC in DS_NULL and
      ** DS_REL
      */
      dl[gas_id].dl_ass = dl_psho_complete_ind->dl_ass_ind;

      if (dl_psho_complete_ind->dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)
      {
        dl[gas_id].tbf_mode = TBF_MODE_EGPRS;

        /* Store the DL Window Size */
        dl[gas_id].ws       = dl_psho_complete_ind->dl_ass_ind.ws;
      }
      else /* TBF_MODE_GPRS */
      {
        dl[gas_id].tbf_mode = TBF_MODE_GPRS;
      }
    }
    else
    {
      /* dl[gas_id].ws and other parameters remain same as before.
      */
      dl[gas_id].tbf_mode = dl[gas_id].psho_tbf_mode;
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_216, "GRLD NULL/REL dl_ass_ind_present FALSE for ps handover");
    }

    /* ready to receive.
    ** Next state is rx ack or rx un-ack
    */
    dl[gas_id].state = (dl_state_t)((dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK) ?
                            DS_RX_ACK :
                            DS_RX_UNACK
                           );
    UPD_DL_STATE(gas_id);

    /* process any blocks in DL FIFO
    */
    grlc_dl_store_blind_data_blocks(gas_id);

    /* process the blind data blocks */
    grlc_dl_process_blind_data_blocks(gas_id);

#ifdef  GRLD_SHOW_DL_ASS
    MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_217, "GRLD NULL/REL Rcvd DL_ASS mode=%d tlli=0x%x ws=%u",
            dl[gas_id].dl_ass.rlc_mode,
            dl[gas_id].dl_ass.tlli,
            dl[gas_id].ws);
#endif

    /* init dl pdu with appropriate rlc mode if needed
    */
    if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_UNINIT)
    {
      grlc_dl_init_llc_pdu(gas_id, &cur_dl_pdu[gas_id], dl[gas_id].dl_ass.rlc_mode);
    }

    /* Start at dl assignment to trap when the 1st data block is late
    */
    TIMER_START_T3190(gas_id);

    /* Clear PSHO variables
    */
    grlc_dl_clr_psho_vars(gas_id);
  }
  else if (dl_psho_complete_ind->psho_status == PSHO_FAILURE)
  {
    /* Delete the stored downlink block during blind transmission.
    */
    grlc_dl_delete_blind_data_blocks(gas_id);

    /* if DS_STATE is PENDING_RELEASE , as T3192 is running we need to inform MAC
    ** to release layer 1
    */
    if ( dl[gas_id].state == DS_RX_ACK_PENDING_RELEASE    ||
         dl[gas_id].state == DS_RX_UNACK_PENDING_RELEASE
       )
    {
      /* inform MAC of normal release of DL TBF
      */
      rlc_msg_ptr = mac_get_cmd_buf_rlc();
      if (rlc_msg_ptr != NULL)
      {
        rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
        rlc_msg_ptr->msg.dl_release_ind.gas_id = gas_id;
        rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_NORMAL;
        mac_put_cmd_rlc(gas_id, rlc_msg_ptr);
      }
      else
      {
        ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
      }
    }

    /*-----------------------------------------------------------------
    ** Read out all un-read blocks and free DSM item for each to drain
    ** out un-read items in the FIFO. This is required before starting
    ** a new TBF
    **-----------------------------------------------------------------
    */
    grlc_dl_fifo_clear(gas_id);
    grlc_dl_init(gas_id);

    /* Clear PSHO variables
    */
    grlc_dl_clr_psho_vars(gas_id);
  }
  else /* Invalid PSHO_STATUS */
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_218, "GRLD NULL/REL PSHO Invalid psho_status");
  }

} /* grlc_handle_dl_psho_complete_ind_null_and_release_state */


/*===========================================================================
===
===  FUNCTION      grlc_handle_dl_psho_complete_ind_xfer()
===
===  DESCRIPTION
===
===  On Reception of DL_PSHO_COMPLETE_IND in ACK/ UNACK XFER.
===  if the PSHO is FAILURE -  Blind Data blocks are deleted.
===  if the PSHO is success from GtoW - DL is released Locally.
===  if the PSHO is GtoG and Reset - 1 , DL TBF is initialised.
===  if the PSHO is GtoG and Reset - 0 , DL TBF is continued.
===
===  DEPENDENCIES
===
===    dl_ass_ind_present when new TBF and when T3192 running and complete_ind
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_handle_dl_psho_complete_ind_xfer
(
  gas_id_t                        gas_id,
  const mr_dl_psho_complete_ind_t *dl_psho_complete_ind
)
{
  MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_219, "GRLD X_FER PSHO MR_DL_PSHO_COMPLETE_IND in psho_status=%d psho_type=%d reset_flag=%d",
                dl_psho_complete_ind->psho_status,
                dl_psho_complete_ind->psho_type,
                dl_psho_complete_ind->reset_flag);

  if(dl_psho_complete_ind->psho_status == PSHO_SUCCESS)
  {
    /* store assignment info , dl_ass_ind_present = TRUE from MAC
    ** when reset= TRUE, or when EGPRS TBF or new GPRS TBF.
    */
    if (dl_psho_complete_ind->psho_type == PSHO_GTOW)
    {
      dl[gas_id].dl_ass = dl_psho_complete_ind->dl_ass_ind;

      MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_220, "GRLD X_FER PSHO WTOG successfull");

      /* RLC should release the dl resources.
      */
      /* Delete the stored downlink block during blind transmission.
      */
      grlc_dl_delete_blind_data_blocks(gas_id);

      rlc_log_dl_release_ind( gas_id, dl[gas_id].tfi,RLC_LOG_DL_MAC_RELEASE_IND);

      /*-----------------------------------------------------------------
      ** Read out all un-read blocks and free DSM item for each to drain
      ** out un-read items in the FIFO. This is required before starting
      ** a new TBF
      **-----------------------------------------------------------------
      */
      grlc_dl_release(gas_id);
    }
    else if (dl_psho_complete_ind->psho_type == PSHO_GTOG)
    {
      if ( dl_psho_complete_ind->dl_ass_ind_present  &&
           (dl_psho_complete_ind->dl_ass_ind.tbf_mode !=
            dl[gas_id].psho_tbf_mode
           )
         )
      {
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_221, "GRLD ##tbf mode mismatch for ps handover");
      }

      if(dl_psho_complete_ind->reset_flag)
      {
        /* process any blocks in DL FIFO
        */
        grlc_dl_store_blind_data_blocks(gas_id);

        /*-----------------------------------------------------------------
        ** Read out all un-read blocks and free DSM item for each to drain
        ** out un-read items in the FIFO. This is required before starting
        ** a new TBF
        **-----------------------------------------------------------------
        */
        grlc_dl_fifo_clear(gas_id);
        grlc_dl_init(gas_id);

        /* In XFER , MAC does not give dl_ass_ind for GPRS TBF */
        if (dl_psho_complete_ind->dl_ass_ind_present)
        {
          /* store assignment info , dl_ass_ind_present = TRUE from MAC
          */
          dl[gas_id].dl_ass = dl_psho_complete_ind->dl_ass_ind;

          if (dl_psho_complete_ind->dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)
          {
            dl[gas_id].tbf_mode = TBF_MODE_EGPRS;

            /* Store the DL Window Size */
            dl[gas_id].ws       = dl_psho_complete_ind->dl_ass_ind.ws;
          }
          else /* TBF_MODE_GPRS */
          {
            dl[gas_id].tbf_mode = TBF_MODE_GPRS;
          }
        }
        else
        {
          /* dl[gas_id].ws and other parameters remain same as before.
          */
          dl[gas_id].tbf_mode = dl[gas_id].psho_tbf_mode;
          MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_222, "GRLD XFER dl_ass_ind_present FALSE for ps handover");
        }

        /* ready to receive.
        ** Next state is rx ack or rx un-ack
        */
        dl[gas_id].state = (dl_state_t)((dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK) ?
                                DS_RX_ACK :
                                DS_RX_UNACK
                               );
        UPD_DL_STATE(gas_id);

        /* process the blind data blocks */
        grlc_dl_process_blind_data_blocks(gas_id);

        /* init dl pdu with appropriate rlc mode
        */
        if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_UNINIT)
        {
          grlc_dl_init_llc_pdu(gas_id, &cur_dl_pdu[gas_id], dl[gas_id].dl_ass.rlc_mode);
        }

        /* Start at dl assignment to trap when the 1st data block is late
        */
        TIMER_START_T3190(gas_id);
      }
      else /* reset = 0 */
      {
        /* process any blocks in DL FIFO
        */
        grlc_dl_store_blind_data_blocks(gas_id);

        if(dl_psho_complete_ind->dl_ass_ind_present)
        {
          /* Update the TLLI as it may change if the RA changes*/
          dl[gas_id].dl_ass.tlli = dl_psho_complete_ind->dl_ass_ind.tlli;

          /* Update dl nts assigned by NW */
          dl[gas_id].dl_ass.num_dl_ts = dl_psho_complete_ind->dl_ass_ind.num_dl_ts;

          if(dl[gas_id].tbf_mode == TBF_MODE_EGPRS)
          {
            if ( (dl_psho_complete_ind->dl_ass_ind.ws  >= 64    ) &&
                 (dl_psho_complete_ind->dl_ass_ind.ws  <= 1024  ) &&
                 (dl_psho_complete_ind->dl_ass_ind.ws  >= dl[gas_id].ws )
                    )
            {
              dl[gas_id].ws = dl_psho_complete_ind->dl_ass_ind.ws;
            }
            else
            {
              MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_223, "ERLD X_FER PSHO RX_A  WS=%u in DL_ASS_IND curr ws=%u not updated nts %d",
                         dl_psho_complete_ind->dl_ass_ind.ws,dl[gas_id].ws,dl[gas_id].dl_ass.num_dl_ts);
            }
          }
          else
          {
            MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_224, "GRLD X_FER PSHO GPRS DL_ASS");
          }
        }
        else /* GPRS re-assignment in the new cell */
        {
          MSG_GRLC_MED_0_G(GRLC_F3_MSG_ENUM_225, "GRLD X_FER PSHO GPRS DL_ASS");
          MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_226, "GRLD XFER dl_ass_ind_present FALSE for ps handover");
        }

        /* process the blind data blocks */
        grlc_dl_process_blind_data_blocks(gas_id);
      }

#ifdef GRLD_SHOW_DL_ASS
      MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_227, "GRLD X_FER PSHO Rcvd DL_ASS mode=%d tlli=0x%x ws=%u",
                  dl[gas_id].dl_ass.rlc_mode,
                  dl[gas_id].dl_ass.tlli,
                  dl[gas_id].ws);
#endif
      /* Clear PSHO variables
      */
      grlc_dl_clr_psho_vars(gas_id);
    }
    else
    {
      MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_228, "GRLD X_FER PSHO ##invalid psho_type=%d",dl_psho_complete_ind->psho_type);
    }
  }
  else if (dl_psho_complete_ind->psho_status == PSHO_FAILURE)
  {
    /* Delete the stored downlink block during blind transmission.
    */
    grlc_dl_delete_blind_data_blocks(gas_id);

    /* clear the fifo if there are any unprocessed blocks of new cell
    ** which has failed PSHO , should not be any
    */
    grlc_dl_fifo_clear(gas_id);

    /* Clear PSHO variables
    */
    grlc_dl_clr_psho_vars(gas_id);
  }
  else /* Invalid PSHO_STATUS */
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_229, "GRLD X_FER PSHO ##invalid psho_status");
  }

} /* grlc_handle_dl_psho_complete_ind_xfer */



/*===========================================================================
===
===  FUNCTION      grlc_dl_delete_blind_data_blocks()
===
===  DESCRIPTION
===
===  When there is PSHO FAILURE or PANIC RESET , it is called  to free
===  DSM items.
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
void grlc_dl_delete_blind_data_blocks( gas_id_t gas_id )
{
  uint16 u;

  /* Free the dsm items of the stored blocks if not freed.
  */
  for (u=0; u < dl[gas_id].blind_data_blks_ctr; u++)
  {
    if (dl[gas_id].psho_tbf_mode == TBF_MODE_EGPRS)
    {
      if ( dl_psho_blind_arr[gas_id][u].e_dl_blk.payload_1_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[gas_id][u].e_dl_blk.payload_1_ptr);
      }
      if ( dl_psho_blind_arr[gas_id][u].e_dl_blk.payload_2_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[gas_id][u].e_dl_blk.payload_2_ptr);
      }
    }
    else if (dl[gas_id].dl_ass.tbf_mode == TBF_MODE_GPRS)
    {
      if (dl_psho_blind_arr[gas_id][u].dl_blk.pl1_blk_dsm_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[gas_id][u].dl_blk.pl1_blk_dsm_ptr);
      }
    }
    else
    {
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_230, "GRLD ##Invalid tbf mode");
      break;
    }
  }
  dl[gas_id].blind_data_blks_ctr  = 0;
} /* grlc_dl_delete_blind_data_blocks */

/*===========================================================================
===
===  FUNCTION      grlc_dl_store_blind_data_blocks()
===
===  DESCRIPTION
===
===   The function read the DL FIFO and store the block as per TBF_MODE
===   for blind data transmission
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
void grlc_dl_store_blind_data_blocks( gas_id_t gas_id )
{
  if ( (dl[gas_id].blind_data_blks_ctr + grlc_dl_fifo_get_cnt(gas_id)) >= DL_BLIND_DATA_BLKS)
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_231, "GRLD Blind DL FIFO storage full,ignoring new incoming blocks");
    grlc_dl_free_excess_dl_blocks(gas_id);
  }
  else if(dl[gas_id].psho_tbf_mode == TBF_MODE_EGPRS)
  {
    while (grlc_dl_fifo_get_cnt(gas_id) > 0)
    {
      if ( e_grlc_dl_fifo_get_block( gas_id,
                                     &dl_psho_blind_arr[gas_id][dl[gas_id].blind_data_blks_ctr].e_dl_blk
                                    )   !=
                                    DL_FIFO_EMPTY
         )
      {
        dl[gas_id].blind_data_blks_ctr++;
      }
      else
      {
        MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_232, "GRLD DL FIFO EMPTY");
        return;
      }
    }
  }
  else if (dl[gas_id].psho_tbf_mode == TBF_MODE_GPRS)
  {
    while (grlc_dl_fifo_get_cnt(gas_id) > 0)
    {
      if ( grlc_dl_fifo_get_block( gas_id,
                                   &pl1_blk_hdr_ptr[gas_id],
                                   &dl_psho_blind_arr[gas_id][dl[gas_id].blind_data_blks_ctr].dl_blk.pl1_blk_dsm_ptr
                                 )   !=
                                 DL_FIFO_EMPTY
         )
      {
        dl_psho_blind_arr[gas_id][dl[gas_id].blind_data_blks_ctr].dl_blk.pl1_blk_hdr = *pl1_blk_hdr_ptr[gas_id];
        dl[gas_id].blind_data_blks_ctr++;
      }
      else
      {
        MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_233, "GRLD DL FIFO EMPTY");
        return;
      }
    }
  }
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_234, "GRLD Invalid TBF MODE");
  }

  MSG_GRLC_MED_1_G(GRLC_F3_MSG_ENUM_235, "GRLD PSHO Blind Data Block counter = %u",dl[gas_id].blind_data_blks_ctr);

} /* grlc_dl_store_blind_data_blocks */

#endif /* FEATURE_GPRS_PS_HANDOVER */


/*** EOF: don't remove! ***/
