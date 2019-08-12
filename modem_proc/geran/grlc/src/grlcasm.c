/*****************************************************************************
***
*** TITLE
***
***  GPRS Radio Link Control Assembly Module
***
*** DESCRIPTION
***
***  This module contains download data blocks re-assembly functions
*** 
*** EXTERNALIZED FUNCTIONS
***
*** void grlc_dl_init_llc_pdu()
*** void grlc_dl_assemble()
*** void grlc_dl_e_assemble()
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
*** Copyright (c) 2001-2016 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcasm.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/09/06    hv     Created
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
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_msgs.h"
#include "gl1rlccmn.h"

#ifdef DEBUG_GSM_EGPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST */

#include "grlcdl.h"
#include "rex.h"
#include "gllc.h"
#ifndef DEBUG_GSM_EGPRS_DS_UNIT_TEST
#include "gllc_v.h"
#include "gllc_v_g.h"
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST */
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
#include "grlcsgm.h" /* definition of ul cur_dlk needed for TEST_MODE */
#include "grlcdltst.h"

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

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*---------------------------------------------------------
** Current downlink pdu.
** Holds the pdu currently being processed.
** Note the dsm item must be created and held in 'dsm_ptr'
**---------------------------------------------------------
*/
grlc_dl_pdu_t cur_dl_pdu[NUM_GERAN_DATA_SPACES];

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
===  FUNCTION      APPEND_PDU
===
===  DESCRIPTION
===
===    Initialise pdu struct if it is un-initialised and append the data
===    octets of given length to the pdu.
===
===    If the number of octets to append specified by 'len' is the same as the
===    length of the block the whole block is appended. If not, the block
===    is segmented such that the first 'len' octets are stripped and appended
===    to the current PDU. The remaining octets are left in the block.
===
===    This function also checks and truncates current PDU if the resultant
===    length after appending exceeds that of the maximum LLC PDU length of 
===    circa 1560 octets. When truncated the outstanding octets are freed by 
===    this function.
===
===    Note that dsm_ptr_ptr is a pointer to pointer to dsm item. This is 
===    required since append_pdu() may call gprs_pdu_segment_head() which alters
===    the DSM item after segmentation.
===
===  DEPENDENCIES
===
===    expect dl pdu to have been initialised and ready to accept a data
===  
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    dl_bsn,
===    vr,
===    vq
===    vn[]
===  
===========================================================================*/
static append_pdu_t  append_pdu
(
  gas_id_t        gas_id,
  dsm_item_type   **dsm_ptr_ptr,
  const uint16    len,
  uint16          bsn
)
{
  uint16      rem_space;  /* number of octets to be tail-truncated */
  uint16      blk_len;    /* length of the block passed to be appended. This may be
                          ** longer than the number of octets to be appended.
                          */
  dsm_item_type *temp_dsm_ptr;

  /*---------------------------------------------------*/

  /* take the length of the block to append to the current PDU.
  ** This block may contain more octets than the the amount to append
  ** specified by 'len'
  */
  blk_len = (uint16)GPRS_PDU_LENGTH(*dsm_ptr_ptr);

  /*-------------------------------------------------------------
  ** check current pdu status.
  ** If required, initialise by creating a dsm item of 0 length 
  ** and point to it, and mark the pdu as initialised ready 
  ** to accept data.
  **-------------------------------------------------------------
  */
  if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_UNINIT)
  {
    grlc_dl_init_llc_pdu(gas_id, &cur_dl_pdu[gas_id], dl[gas_id].dl_ass.rlc_mode);
  }

  /* If current PDU length is zero (at the start) then the first BSN appended is 
  ** recorded. last_bsn is also assigned the same value
  */
  if (cur_dl_pdu[gas_id].len == 0)
  {
    cur_dl_pdu[gas_id].first_bsn = bsn;
    cur_dl_pdu[gas_id].last_bsn = bsn;
  }

  /*----------------------------------------------------
  ** add the complete buffer and return APPEND_PDU_OK 
  ** if there is enough room. If the limit is exceeded 
  ** then just add as many octets to bring the size to 
  ** max and return APPEND_PDU_TRUNCATED
  **----------------------------------------------------
  */
  if ((cur_dl_pdu[gas_id].len + len) <= GRLC_DL_PDU_MAX_LEN)
  {
    /* enough room. Append buffer to dsm item
    */

    /* before appending we need to find out if the number to append and the
    ** length of the block is the same. If the same, augment the complete 
    ** block. If not, segment 'len' octets and then append
    */
    if (blk_len == len)
    {
      gprs_pdu_augment_tail(&(cur_dl_pdu[gas_id].dsm_ptr), dsm_ptr_ptr);
      cur_dl_pdu[gas_id].len += len;
    }
    else if ( blk_len > len )
    /* The number of octets in dsm_ptr must be greater than or equal
    ** to the amount to append 'len' and never shorter than 'len'
    */
    {
      /* we are going to call gprs_pdu_segment_head() which alters the **dsm
      */
      temp_dsm_ptr = gprs_pdu_segment_head(dsm_ptr_ptr, len);
      gprs_pdu_augment_tail(&(cur_dl_pdu[gas_id].dsm_ptr), &temp_dsm_ptr);

      cur_dl_pdu[gas_id].len += len;

      /* note that dsm_ptr_ptr carries the remaining octets (blk_len - len)
      */
    }
    else
    {
      MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_1, "ERLD append_pdu blk_len=%d < len=%d",blk_len,len);

      /* Append the pdu conveyed by dsm_ptr_ptr */
      gprs_pdu_augment_tail(&(cur_dl_pdu[gas_id].dsm_ptr), dsm_ptr_ptr);
      cur_dl_pdu[gas_id].len += blk_len;
    }

    return APPEND_PDU_OK;
  }
  else
  {
    /* number of octets to take the current PDU to the maximum
    */
    rem_space = GRLC_DL_PDU_MAX_LEN - cur_dl_pdu[gas_id].len;

    /* take only rem_space octets out of block and append it to the current PDU
    */
    temp_dsm_ptr = gprs_pdu_segment_head(dsm_ptr_ptr, rem_space);

    gprs_pdu_augment_tail(&(cur_dl_pdu[gas_id].dsm_ptr), &temp_dsm_ptr);

    cur_dl_pdu[gas_id].len = GRLC_DL_PDU_MAX_LEN;

    /* However, we were told to append 'len' octets. Hence, we must get
    ** rid of (len - rem_space) octets by freeing them. First we must get them
    ** into a DSM item pointed to by temp_dsm_ptr.
    ** 
    */
    temp_dsm_ptr = gprs_pdu_segment_head(dsm_ptr_ptr, (uint16)(len - rem_space));

    /* now free the surplus octets
    ** We use temp_dsm_ptr because GPRS_PDU_FREE() requires a pointer to *dsm_ptr
    */
    GPRS_PDU_FREE(&temp_dsm_ptr);
    
    /* if blk_len is greater than len then there is still (blk_len - len)
    ** octets remained in dsm_ptr
    */

    return APPEND_PDU_TRUNCATED;
  }
} /* append_pdu */

/*===========================================================================
===
===  FUNCTION      SEND_LLC_PDU
===
===  DESCRIPTION
===
===    Send the completed pdu to LLC and mark its status as un-initialised.
===
===     input: global cur_dl_pdu of type grlc_dl_pdu_t
===     output: message holding dl pdu posted to LLC
===
===  DEPENDENCIES
===
===    cur_dl_pdu
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    cur_dl_pdu[gas_id].status is changed to CUR_PDU_UNINIT after the pdu is sent.
===  
===========================================================================*/
static void send_llc_pdu ( gas_id_t gas_id )
{
  /* dl message to send to LLC
  */
  gprs_grr_dl_mes_t  *dl_msg_ptr;

  /*--------------------------------------------------------*/

  if (cur_dl_pdu[gas_id].len == 0)
  {
    MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_2, "ERLD req sending zero-length PDU");
    return;
  }

  dl_msg_ptr = GPRS_MEM_MALLOC(sizeof(gprs_grr_dl_mes_t));

  if ( dl_msg_ptr == NULL )
  {
    ERR_GERAN_FATAL_0_G("ERLD send_llc_pdu malloc failure");
  }
  else
  {
    /* fill dl message with LLC pdu info.
    ** The main item being the pdu in dsm format
    */
    if (cur_dl_pdu[gas_id].rlc_mode == GRLC_MODE_ACK)
    {
      dl_msg_ptr -> prim = GRR_DATA_IND;

      /* Update diag stuff in Ack mode
      */
      dl_diag_stats_st[gas_id].ack_rx_pdu_cnt++;
      dl_diag_stats_st[gas_id].ack_rx_oct_cnt += (uint32)cur_dl_pdu[gas_id].len;

      grlc_dl_diag_pdu_stat_add_entry(dl_diag_stats_st[gas_id].ack_rx_pdu_cnt, &cur_dl_pdu[gas_id], gas_id);

#ifdef  GRLD_SHOW_PDU_SENT
      MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_859, "ERLD send_pdu pnum=%d plen=%d octs=%d",
        dl_diag_stats_st[gas_id].ack_rx_pdu_cnt,
        cur_dl_pdu[gas_id].len,
        dl_diag_stats_st[gas_id].ack_rx_oct_cnt
       );
#endif
    }
    else
    {
      dl_msg_ptr -> prim = GRR_UNITDATA_IND;

      /* Update diag stuff in Unack mode
      */
      dl_diag_stats_st[gas_id].unack_rx_pdu_cnt++;
      dl_diag_stats_st[gas_id].unack_rx_oct_cnt += (uint32)cur_dl_pdu[gas_id].len;
      if (dl_diag_stats_st[gas_id].unack_oct_filled_cnt != 0)
      {
        dl_diag_stats_st[gas_id].unack_pdu_filled_cnt++;
      }

      grlc_dl_diag_pdu_stat_add_entry(dl_diag_stats_st[gas_id].unack_rx_pdu_cnt, &cur_dl_pdu[gas_id], gas_id);

#ifdef  GRLD_SHOW_PDU_SENT
      MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_860, "ERLD send_pdu pnum=%d plen=%d octs=%d",
        dl_diag_stats_st[gas_id].unack_rx_pdu_cnt,
        cur_dl_pdu[gas_id].len,
        dl_diag_stats_st[gas_id].unack_rx_oct_cnt
       );
#endif
    }

    dl_msg_ptr -> ll_pdu = cur_dl_pdu[gas_id].dsm_ptr;

    /* clear the source pointer to mark PDU being passed on
    */
    cur_dl_pdu[gas_id].dsm_ptr   = NULL;

    /* must include tlli field
    */
#if defined  (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
    #error code not present
#else
    /* Get TLLI value passed by MAC at assignment
    */
    dl_msg_ptr->tlli = dl[gas_id].dl_ass.tlli;
#endif

    /* mark the pdu as used and requires init again
    */
    cur_dl_pdu[gas_id].status = DL_PDU_STATUS_UNINIT;

#ifdef  GRLD_SHOW_PDU_SENT
    MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_3, "dpdu=0x%x tlli=0x%x FrstLast=%08x",
              dl_msg_ptr->ll_pdu,
              dl_msg_ptr->tlli,
              (((uint32)(cur_dl_pdu[gas_id].first_bsn & 0x07ff) << 16) | 
               (uint32)(cur_dl_pdu[gas_id].last_bsn  & 0x07ff)
              )
            );
#endif
    dl_msg_ptr->gas_id = gas_id;
    gllc_grr_dl_put_mes_buf( dl_msg_ptr );
  }
} /* send_llc_pdu */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    GRLC_DL_INIT_LLC_PDU
===
===  DESCRIPTION
===     Fills LLC PDU with default values
===
===  PARAMS
===
===     input: cur_dl_pdu of type grlc_dl_pdu_t
===            rlc_mode
===     output: none.
===  
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_init_llc_pdu
(
  gas_id_t      gas_id,
  grlc_dl_pdu_t *pdu_ptr, 
  const uint8   rlc_mode
)
{
  pdu_ptr->gas_id = gas_id;
  pdu_ptr->dsm_ptr = NULL;

  /* set rlc mode
  */
  pdu_ptr->rlc_mode = rlc_mode;

  /* zero the length
  */
  pdu_ptr->len = 0;

  pdu_ptr->status = DL_PDU_STATUS_INIT;

} /* grlc_dl_init_llc_pdu */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_ASSEMBLE
===
===  DESCRIPTION
===
===    Assemble the downlink data block in ack mode or un-ack mode into
===    one or more pdus. When a complete pdu is formed it is sent off to
===    LLC and a new pdu is created for the next assembly.
===
===    When a pdu exceeds its 1536 octets limit, it is truncated and sent 
===    off. A flag is set to ensure subsequent data blocks are discarded
===    until one containing a pdu boundary (with ext octet). The data 
===    following the pdu boundary will then be assembled into pdus again.
===
===    This fn should be called when a dl data block is ready in cur_dl_blk[gas_id].
===    In ack mode, another shell fn should call this function.
===
===  PARAMETERS
===
===    blk_ptr: points to an internal DL data block which could be the
===             current dl block or a filled block in un-ack mode or an
===             element of the DL array in ack mode.
===
===  DEPENDENCIES
===
===    cur_dl_pdu: input dl data block
===    cur_dl_pdu: current working pdu which may contain a partial pdu
===    or empty.
===
===    When this function is called cur_dl_blk[gas_id] is expected to hold a data 
===    block whose BSN is contiguous with the one before.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    b_pdu_truncated
===    
===  
===========================================================================*/
void grlc_dl_assemble
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t  *blk_ptr
)
{
  uint8 b,          /* loop count */
        me,         /* group M bit and E bit together */
        num_octets, /* total number of octets thus far taken out 
                    ** of this block for successive pdu's. This var
                    ** contains the accumulative values of the pdus' 
                    ** length, ie. LI field of an ext octet.
                    */
        next_ext_octet_index;
                    /* this points to the next extension octet

                    ** to be processed after a pdu boundary has been
                    ** found.
                    */
  /*----------------------------------------------------------*/

  SHOW_BLOCK(gas_id, blk_ptr);

  /* check current pdu status.
  ** If required, initialise by creating a dsm item and point
  ** to it, and mark the pdu as ready for data
  */
  if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_UNINIT)
  {
    grlc_dl_init_llc_pdu(gas_id, &cur_dl_pdu[gas_id], dl[gas_id].dl_ass.rlc_mode);
  }

  /* no octets taken out of this block so far
  */
  num_octets = 0;

  /* when there is no pdu truncation the 1st ext octet should have
  ** 0 for index.
  */
  next_ext_octet_index = 0;

  /* check if pdu had been truncated in which case all data octets are discarded
  ** until the next identified pdu boundary, ie any block with the EXT field set
  ** to RM_EXT_OCTET_FOLLOWS.
  */
  if (dl[gas_id].b_pdu_truncated)
  {
    /* is there an extension octet
    **
    ** ! check if RM_EXT_OCTET_FOLLOWS is considered as the presence of
    ** a boundary.
    **
    ** ! check handling of blocks after a truncation occurs
    */
    if (blk_ptr->ext == RM_EXT_OCTET_FOLLOWS)
    {
      /* reset pdu truncation flag
      */
      dl[gas_id].b_pdu_truncated = FALSE;

      /* proceed to the next part to assemble block
      */
    }
    else
    {
      /* if there are any octets left in the block we must free them
      */
      if (blk_ptr->dsm_payload_ptr != NULL)
      {
        GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
      }

      /* no pdu boundary here. Exit and wait for the next data block
      */
      return;
    }
  }
  /*-------------------------------------------------------------------*/

  /* deal with the case where there is no extension octet
  */
  if (blk_ptr->ext == RM_EXT_NO_OCTET_FOLLOWS)
  {
    /*--------------------------------------------------------
    ** No extension octet, 2 cases:
    ** 1. FBI = 0 (B.4 example): all data octets are part of
    **    a continuing pdu.
    ** 2. FBI = 1 (B.5 example): data octets fill precisely
    **    the remaining space and current pdu terminates
    **    in this data block.
    **--------------------------------------------------------
    */
    if (blk_ptr->fbi == RM_FBI_FINAL)
    {
      (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                        (uint16)(GRLC_DATA_BLOCK_LEN[blk_ptr->cs] - 
                                 RM_RLC_HDR_LEN
                               ),
                        blk_ptr->bsn 
                      );
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
      MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_4, "GRLD Info Sent PDU FBI gas_id=%d");
#endif
      cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
      send_llc_pdu(gas_id);
    }
    else /* FBI is clear */
    {
      /* FBI is clear: case (1) pdu continues (B.4 example)
      **
      ** PDU is supposed to continue however, if PDU is full then it will
      ** be truncated by append_pdu() and we need to terminate the PDU,
      ** discarding the extra octets.
      */
      if (append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr), 
                      (uint16)(GRLC_DATA_BLOCK_LEN[blk_ptr->cs] - 
                               RM_RLC_HDR_LEN),
                      blk_ptr->bsn                
                    ) == APPEND_PDU_TRUNCATED
         )
      {
        /* disregard  the remaining octets until the next pdu boundary
        ** as per 9.1.11
        */
        dl[gas_id].b_pdu_truncated = TRUE;

        /* pdu is now full so send it off
        */
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
        MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_5, "GRLD Info Sent PDU FBI=0 no-ext trnctd gas_id=%d");
#endif
        cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
        send_llc_pdu(gas_id);
      }
    }
  }

  /*-------------------------------------------------------------------*/

  /* Extension octet(s) present */

  else
  {
    /*MSG_LOW("GRLD asm ext bsn=%d",dl[gas_id].bsn,0,0);*/
    /* loop to process the extension octets. Note the first extension 
    ** octet may be skipped due to handling of pdu truncation.
    */
    for (b=next_ext_octet_index; b<blk_ptr->num_ext_octets; b++)
    {
      me = (uint8)GROUP_ME(blk_ptr->ext_octet[b].gprs.more,
                           blk_ptr->ext_octet[b].gprs.ext);

      switch(me)
      {
        /*-----------------------------------------------------------*/
      
        case RM_ME_IGNORE:
          /* ME reserved. Ignore all fields in block except for MAC header 
          ** (10.4.13). ! Must free dsm item though
          */
          GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));

          MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_6, "GRLD Asm Error Unexp ME=00 gas_id=%d");
          break;

        /*-----------------------------------------------------------*/

        case RM_ME_NO_PDU_FOLLOWS_CURRENT_NO_EXT_FOLLOWS_01:
          /*MSG_LOW("GRLD asm 01",0,0,0);*/
          /* No pdu after the current pdu and no ext octet follows:
          ** one of two cases:
          **
          ** 1. LI = 0 (B.2 example) to indicate the current pdu does not 
          **    end within this data block. The number of octets is held 
          **    in index (complete block length less ext octets) and 
          **    not LI.
          **
          **    a. pdu truncated: take enough octets, send pdu and set flag
          **       to ignore all data octets in the incoming blocks until
          **       the next pdu boundary.
          **
          **    b. pdu not-truncated: take data octets
          **
          ** 2. LI != 0 (B.3 example). Last ext octet specifies the length 
          **    for a new pdu which fills precisely the remaining part of 
          **    this data block. In this case, the last extension octet 
          **    specifies a complete pdu whose length is indicated by the 
          **    LI field.
          **
          ** In both cases processing of the block completes here and
          ** no adjustment to num_octets is required.
          */
          if (blk_ptr->ext_octet[b].gprs.li == 0)
          {
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
            MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_7, "GRLD Info Sent PDU no-pdu no-ext (01) li=0 gas_id=%d");
#endif
            /* take data octets (case 1a and 1b).
            */
            if (append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr), 
                            (uint16)(blk_ptr->len - num_octets),
                            blk_ptr->bsn
                          ) == APPEND_PDU_TRUNCATED
               )
            {
              /* case 1a:
              ** pdu truncated due to length exceeding limit.
              ** send off pdu
              */
              MSG_GRLC_LOW_0_G(GRLC_F3_MSG_ENUM_8, "GRLD Info Sent PDU p0 e0 01 li=0 trnctd gas_id=%d");
              cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
              send_llc_pdu(gas_id);

              /* set flag to ignore all data octets until the 
              ** next pdu boundary
              */
              dl[gas_id].b_pdu_truncated = TRUE;
              
              /* if there are any octets left in the block we must free them
              */
              if (blk_ptr->dsm_payload_ptr != NULL)
              {
                GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
              }
            }
          }
          else
          {
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
            MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_9, "GRLD Info Sent PDU p0 e0 01 li=%d gas_id=%d",blk_ptr->ext_octet[b].gprs.li,gas_id);
#endif
            /* case 2: 
            ** a new pdu fills the remaining data block and 
            ** terminates in this data block.
            */

            /* pdu terminates here so no need to check for overflow
            */
            (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr), 
                              blk_ptr->ext_octet[b].gprs.li,
                              blk_ptr->bsn
                            );

            /* send off pdu
            */
            cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
            send_llc_pdu(gas_id);

            /* if there are any octets left in the block we must free them
            */
            if (blk_ptr->dsm_payload_ptr != NULL)
            {
              GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
            }
          }
          break;

        /*-----------------------------------------------------------*/

        case RM_ME_PDU_FOLLOWS_EXT_FOLLOWS_10:
          /*----------------------------------------------------------------
          ** pdu after the current pdu and ext octet follows:
          ** B.1 example 1st ext octet or B.3 example 1st ext octet
          ** Used in cases where there are at least 2 pdus in a data block
          **----------------------------------------------------------------
          */

          /* pdu terminates here. No checking for truncation required.
          ** If the block overflows the pdu, the surplus octets are ignored
          ** without the need for a flag being set since another pdu is 
          ** expected to follow within this block.
          **
          ** num_octets accumulates the number of octets taken out of the
          ** block so far is is therefore used to index the block.
          */
          (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                            blk_ptr->ext_octet[b].gprs.li,
                            blk_ptr->bsn
                          );

          /* adjust the number of octets taken out of this block thus far
          */
          num_octets += blk_ptr->ext_octet[b].gprs.li;

          /* pdu is now full so send it off
          */
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
          MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_10, "GRLD Info Sent p1 e1 10 li=%d gas_id=%d",blk_ptr->ext_octet[b].gprs.li,gas_id);
#endif
          cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
          send_llc_pdu(gas_id);

          /* Since E is '0', ie. RM_EXT_OCTET_FOLLOWS, another extension
          ** octet is expected.
          */
          break;

        /*-----------------------------------------------------------*/

        case RM_ME_PDU_FOLLOWS_NO_EXT_FOLLOWS_11:
          /*MSG_LOW("GRLD asm 11",0,0,0);*/
          /* pdu after the current one and no ext octet following on:
          ** B1 example and B.2b example: In this case, there are two
          ** pdus, the current pdu whose length is held in the LI field
          ** of the current extension octet and it will terminate here.
          ** The following and last pdu within this block, whose length 
          ** is the length of the data block less the number of octets
          ** taken out thus far. This following pdu does not terminate 
          ** here
          */
          
          /*--------------------------------
          ** process the current (1st) pdu
          **--------------------------------
          */

          /* the current(1st) pdu terminates here so no checking for
          ** overflow.
          */
          (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                            blk_ptr->ext_octet[b].gprs.li,
                            blk_ptr->bsn
                          );
                                 
          /* adjust the number of octets taken out of this block thus far
          */
          num_octets += blk_ptr->ext_octet[b].gprs.li;

          /* pdu is now full so send it off
          */
          MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_11, "GRLD Sent p1 e0 11 num_octs=%d li=%d plen=%d",
              num_octets,
              blk_ptr->ext_octet[b].gprs.li,
              GPRS_PDU_LENGTH(cur_dl_pdu[gas_id].dsm_ptr));

          cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
          send_llc_pdu(gas_id);

          /*------------------------------------------------------------
          ** process the following pdu (just the start of it) and last 
          ** pdu in this data block
          **------------------------------------------------------------
          */

          /* pdu starts so it can not overflow
          */
          (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                            (uint16)(blk_ptr->len - num_octets),
                            blk_ptr->bsn
                          );

          MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_12, "GRLD 11 b_l=%d n_octs=%d n_e=%d",
                  blk_ptr->len,
                  num_octets,
                  blk_ptr->num_ext_octets);

          /* If it's the last block then terminate TBF here
          */
          if (blk_ptr->fbi == RM_FBI_FINAL)
          {
            MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_13, "GRLD 11 FBI p_len=%d gas_id=%d",
                    GPRS_PDU_LENGTH(blk_ptr->dsm_payload_ptr),
                    gas_id,
                    0);
            cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
            send_llc_pdu(gas_id);
          }

          break;

        /*-----------------------------------------------------------*/

        default:

          MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_14, "GRLD Asm Error Unexp ME=%d gas_id=%d",me,gas_id);
          break;

      }   /* switch on MORE & EXT bits */
    }   /* for loop to process the extension octets */
  }   /* end extension octet follows */

  /* time to free any superfluous octets passed by PL1
  */
  if (blk_ptr->dsm_payload_ptr != NULL)
  {
    GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
  }

} /* grlc_dl_assemble */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_E_ASSEMBLE
===
===  DESCRIPTION
===
===    Assemble the downlink data block in ack mode or un-ack mode into
===    one or more pdus. When a complete pdu is formed it is sent off to
===    LLC and a new pdu is created for the next assembly.
===
===    When a pdu exceeds its 1536 octets limit, it is truncated and sent 
===    off. A flag is set to ensure subsequent data blocks are discarded
===    until one containing a pdu boundary (with ext octet). The data 
===    following the pdu boundary will then be assembled into pdus again.
===
===    This fn should be called when a dl data block is ready in cur_dl_blk[gas_id].
===    In ack mode, another shell fn should call this function.
===
===  PARAMETERS
===
===    blk_ptr: points to an internal DL data block which could be the
===             current dl block or a filled block in un-ack mode or an
===             element of the DL array in ack mode.
===
===  DEPENDENCIES
===
===    cur_dl_pdu: input dl data block
===    cur_dl_pdu: current working pdu which may contain a partial pdu
===    or empty.
===
===    When this function is called cur_dl_blk[gas_id] is expected to hold a data 
===    block whose BSN is contiguous with the one before.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    b_pdu_truncated
===    
===  
===========================================================================*/
void grlc_dl_e_assemble
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t  *blk_ptr
)
{
  uint8 b,            /* loop count */
        num_octets,   /* total number of octets thus far taken out 
                      ** of this block for successive pdu's. This var
                      ** contains the cumulative values of the pdus' 
                      ** length, ie. LI field of an ext octet.
                      */
        next_ext_octet_index; /* points to the next extension octet
                              ** to be processed after a pdu boundary has been
                              ** found.
                              */

  /*----------------------------------------------------------*/

  SHOW_BLOCK(gas_id, blk_ptr);

  /* check current pdu status.
  ** If required, initialise by creating a dsm item and point
  ** to it, and mark the pdu as ready for data
  */
  if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_UNINIT)
  {
    grlc_dl_init_llc_pdu(gas_id, &cur_dl_pdu[gas_id], dl[gas_id].dl_ass.rlc_mode);
  }

  /* no octets taken out of this block so far
  */
  num_octets = 0;

  /* when there is no pdu truncation the 1st ext octet should have
  ** 0 for index.
  */
  next_ext_octet_index = 0;

  /* check if pdu had been truncated in which case all data octets are discarded
  ** until the next identified pdu boundary, ie any block with the EXT field set
  ** to RM_EXT_OCTET_FOLLOWS.
  */
  if (dl[gas_id].b_pdu_truncated)
  {
    /* is there an extension octet
    **
    ** ! check if RM_EXT_OCTET_FOLLOWS is considered as the presence of
    ** a boundary.
    **
    ** ! check handling of blocks after a truncation occurs
    */
    if (blk_ptr->ext == RM_EXT_OCTET_FOLLOWS)
    {
      /* Got a boundary.*/

      /* reset pdu truncation flag
      */
      dl[gas_id].b_pdu_truncated = FALSE;
      
      /* proceed to the next part to assemble block
      */
    }
    else
    {
      /* if there are any octets left in the block we must free them
      */
      if (blk_ptr->dsm_payload_ptr != NULL)
      {
        GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
      }

      /* no pdu boundary here. Exit and wait for the next data block
      */
      return;
    }
  }

  /*-------------------------------------------------------------------*/

  /* deal with the case where there is no extension octet
  */
  if (blk_ptr->ext == RM_EXT_NO_OCTET_FOLLOWS)
  {
    /*--------------------------------------------------------
    ** No extension octet, 2 cases:
    ** 1. FBI = 0 all data octets are part of
    **    a continuing pdu.
    ** 2. FBI = 1 data octets fill precisely
    **    the remaining space and current pdu terminates
    **    in this data block.
    **--------------------------------------------------------
    */
    if (blk_ptr->fbi == RM_FBI_FINAL)
    {
      (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                        (uint16)GPRS_PDU_LENGTH(blk_ptr->dsm_payload_ptr),
                        blk_ptr->bsn
                      );
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_15, "ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d",
                blk_ptr->bsn,
                gas_id,
                0);
#endif
      cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
      send_llc_pdu(gas_id);
    }
    else /* FBI is clear */
    {
      /* PDU is supposed to continue however, if PDU is full then it will
      ** be truncated by append_pdu() and we need to terminate the PDU,
      ** discarding the extra octets.
      */
      if (append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr), 
                      (uint16)GPRS_PDU_LENGTH(blk_ptr->dsm_payload_ptr),
                      blk_ptr->bsn
                    ) == APPEND_PDU_TRUNCATED
         )
      {
        /* disregard  the remaining octets until the next pdu boundary
        ** as per 9.1.12
        */
        dl[gas_id].b_pdu_truncated = TRUE;

        /* pdu is now full so send it off
        */
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
        MSG_GRLC_MED_3_G(GRLC_F3_MSG_ENUM_16, "ERLD Info Sent PDU FBI=0 no-ext trnctd last-bsn=%u bsn2=%u gas_id=%d",
                  blk_ptr->bsn,
                  gas_id,
                  0);
#endif
        cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
        send_llc_pdu(gas_id);
      } /* truncated */
    } /* FBI = 0 */
  } /* no ext octet follows */

  /*-------------------------------------------------------------------*/

  /* Extension octet(s) present */

  else
  {
    /*MSG_LOW("GRLD asm ext bsn=%d",dl[gas_id].bsn,0,0);*/
    /* loop to process the extension octets. Note the first extension 
    ** octet may be skipped due to handling of pdu truncation.
    */
    for (b=next_ext_octet_index; b<blk_ptr->num_ext_octets; b++)
    {
      /* Validate LI and E combinations according to TS 04.60 Table 10.4.14a.1.
      ** - LI=0 only valid in first extension octet.
      ** - Reserved vlues 74 < LI < 127.
      ** - New PDU can't follow filling octets
      */
      if( (b!=0 && blk_ptr->ext_octet[b].egprs.li == 0)                             ||
          (blk_ptr->ext_octet[b].egprs.li > 74   && blk_ptr->ext_octet[b].egprs.li < 127)   ||
          (blk_ptr->ext_octet[b].egprs.li == 127 && blk_ptr->ext_octet[b].egprs.ext == 0)
        )
      { 
        /* Invalid LI/E combination, free dsm item */
        GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));

        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_17, "ERLD Asm Error Unexp LI=%u E=%u Eth=%u",
                    blk_ptr->ext_octet[b].egprs.li,
                    blk_ptr->ext_octet[b].egprs.ext,
                    b);
                    
#ifdef ERLD_DUMP_12BYTES_PLD
        dl[gas_id].dump_pld_once = TRUE;
#endif /* ERLD_DUMP_12BYTES_PLD  */
      }
      else if( blk_ptr->ext_octet[b].egprs.li == 0 )  
      /* LI=0 only valid at 1st ext octet */
      /* b must be 0 when li=0. Already validated */
      {
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_18, "ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d",
                  blk_ptr->bsn,
                  gas_id,
                  0);
#endif
        /* This LI=0 delimits the current PDU. So send PDU it off
        */
        cur_dl_pdu[gas_id].last_bsn = MOD_ESNS(blk_ptr->bsn-1);
        send_llc_pdu(gas_id);

        /* The remaining of this data block contains the beginning of a new PDU:
        ** 1. Ext=1: no ext octet follows: new PDU whose starting length is 
        **           indicated by the MCS of this block less one octet (and only)
        **           for this LI=0
        ** 2. Ext=0: ext octet follows. Ex B.2.2 handled by the next loop
        */

        /* 3GPP TS 04.60 V8.21.0 (2003-12) Table 10.4.14a.1
        ** The previous RLC data block contains a LLC PDU, or a part of
        ** it, that fills precisely the previous data block and for which
        ** there is no length indicator in that RLC data block. The current
        ** RLC data block contains a LLC PDU that either fills the current
        ** RLC data block precisely or continues in the next RLC data
        ** block.
        */
        if (blk_ptr->ext_octet[b].egprs.ext == 1)   
        /* b must be 0 when LI=0. Already validated */
        { 
           (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                             (uint16) GPRS_PDU_LENGTH(blk_ptr->dsm_payload_ptr),
                             blk_ptr->bsn
                           );

          /* Special case: LI=0 termimates previous PDU. Remaining octets (with FBI set)
          ** represents the final PDU that fits precisely the remaining of this block
          */
          if (blk_ptr->fbi == RM_FBI_FINAL)
          {
            cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
            send_llc_pdu(gas_id);
          }

        }
        /* else: case 2: handled by the next loop iteration 
        */
      }
      else if( blk_ptr->ext_octet[b].egprs.li == 127 &&
               blk_ptr->ext_octet[b].egprs.ext == 1)
      {
        /* The rest of this block contains fillers. The LI=127 octet usually follows
        ** a meaningful LI=x octet where the associated PDU had already been sent off.
        ** However, if the LI=127 is the only extension octet then the current PDU
        ** may not have been terminated and sent off. So we check if the current PDU
        ** has been initialised, if so we send it off
        */
        if (cur_dl_pdu[gas_id].status == DL_PDU_STATUS_INIT)
        {
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
          MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_19, "ERLD Info Sent PDU last-bsn=%u gas_id=%d",
                    blk_ptr->bsn,
                    gas_id,
                    0);
#endif
          /* terminate current PDU
          */
          cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
          send_llc_pdu(gas_id);
    
          /* Free the remaining payload containing fillers
          */
          GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
        }
      }
      else if( blk_ptr->ext_octet[b].egprs.ext == 0)
      {
        /* Non zero LI and new pdu follows. Appent LI octets to LLC pdu.
        ** Terminate current pdu and send it to LLC.
        */

        /* pdu terminates here. No checking for truncation required.
        ** If the block overflows the pdu, the surplus octets are ignored
        ** without the need for a flag being set since another pdu is 
        ** expected to follow within this block.
        **
        ** num_octets accumulates the number of octets taken out of the
        ** block so far is is therefore used to index the block.
        */
        (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                          blk_ptr->ext_octet[b].egprs.li,
                          blk_ptr->bsn
                        );

        /* adjust the number of octets taken out of this block thus far
        */
        num_octets += blk_ptr->ext_octet[b].egprs.li;

        /* pdu is now full so send it off
        */
#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_20, "ERLD Info Sent PDU last-bsn=%u gas_id=%d",
                  blk_ptr->bsn,
                  gas_id,
                  0);
#endif
        cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
        send_llc_pdu(gas_id);
      }
      else if( blk_ptr->ext_octet[b].egprs.ext == 1)
      {
        /* Non zero LI and no extension octet follows. So terminate current
        ** PDU whose final segment's length is indicated by LI. Send off PDU.
        ** A new PDU occupies the remaining of this data block
        */

        /* pdu terminates here so no need to check for overflow
        */
        (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr), 
                          blk_ptr->ext_octet[b].egprs.li,
                          blk_ptr->bsn
                        );

#ifdef  GRLD_SHOW_BLOCK_ASSEMBLED
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_21, "ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d",
                blk_ptr->bsn,
                gas_id,
                0);
#endif
        /* send off pdu
        */
        cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
        send_llc_pdu(gas_id);

        /* adjust the number of octets taken out of this block thus far
        */
        num_octets += blk_ptr->ext_octet[b].egprs.li;
       
        /* (blk_ptr->len - blk_ptr->ext_octet[b].li) > 0 the remaining octets 
        ** belong to the next PDU
        */
        if (blk_ptr->len > num_octets)
        {
            (void)append_pdu( gas_id, &(blk_ptr->dsm_payload_ptr),
                              (uint16)(blk_ptr->len - num_octets),
                              blk_ptr->bsn
                            );
           
          /* Special case: remaining octets fit precisely and represents the last
          ** PDU of the TBF, hence send off last PDU
          */
          if (blk_ptr->fbi == RM_FBI_FINAL)
          {
            cur_dl_pdu[gas_id].last_bsn = blk_ptr->bsn;
            send_llc_pdu(gas_id);
          }
        }

        /* if there are any octets left in the block we must free them
        */
        if (blk_ptr->dsm_payload_ptr != NULL)
        {
          MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_22, "ERLD Asm Free trailing %u octets gas_id=%d",
                  GPRS_PDU_LENGTH(blk_ptr->dsm_payload_ptr),
                  gas_id,
                  0);
          GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
        }
      } /* end ext == 1 */
      else
      {
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_23, "GRLD Asm Error Unexp LI/E %d %d",
                  blk_ptr->ext_octet[b].egprs.li,
                  blk_ptr->ext_octet[b].egprs.ext,
                  0);
      }
    }   /* for loop to process the extension octets */
    
#ifdef ERLD_DUMP_12BYTES_PLD
    /* Dump payload information */
    if( dl[gas_id].dump_pld_once == TRUE )
    {
      e_dump_dl_pld();
      dl[gas_id].dump_pld_once = FALSE;
    }
#endif /* ERLD_DUMP_12BYTES_PLD  */
    
  }   /* end extension octet follows */

  /* time to free any superfluous octets passed by PL1
  */
  if (blk_ptr->dsm_payload_ptr != NULL)
  {
    GPRS_PDU_FREE(&(blk_ptr->dsm_payload_ptr));
  }
} /* grlc_dl_e_assemble */


/*** EOF: don't remove! ***/
