/*****************************************************************************
***
*** TITLE
***
***  GPRS Radio Link Control Downlink Test Module
***
*** DESCRIPTION
***
***  This module contains the test (host/back-to-back-targets) and monitoring
***  functions for the GPRS RLC downlink module
***
***
*** EXTERNALIZED FUNCTIONS
***
***  grlc_diag_dl_statistics_t()
***  grlc_diag_clr_dl_statistics()
***  grlc_diag_show_dl()
***  rlc_get_pl1_radio_block_size()
***  rlc_dl_show_state_vars()
***  rlc_dl_show_rx_vars()
***  rlc_dl_check_rbb()
***  rlc_dl_force_reset()
***  grlc_dl_inc_missync_blk_cnt()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcdltst.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
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
#include "grlcdiag.h"
#include "grlcdli.h"
#include "grlclog.h"
#include "grlcdl.h"

/*-----------------------------------------------------------------------
**  Downlink statistical info
**-----------------------------------------------------------------------
*/
grlc_diag_dl_statistics_t  dl_diag_stats_st[NUM_GERAN_DATA_SPACES];

grlc_diag_dl_pdu_stat_t    dl_diag_pdu_stat[NUM_GERAN_DATA_SPACES];
grlc_diag_dl_rx_stat_t     dl_diag_rx_stat[NUM_GERAN_DATA_SPACES];

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

/*===========================================================================
===
===  FUNCTION      SHOW_DL_RBB()
===
===  DESCRIPTION
===
===    Show SSN and bitmap of PDAN
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
void show_dl_rbb (gas_id_t gas_id, const rm_acknack_desc_t *desc_ptr, uint8 fai)
{
  uint16  u;
  uint32  hu32=0UL, lu32=0UL;

  for (u=0; u<4; u++)
  {
    /* RBB[4..7] into the lower 32 bits
    */
    lu32 |= (desc_ptr->rbb[u+4] << (8*((4-u)-1)) );

    /* RBB[0..3] into the higher 32 bits
    */
    hu32 |= (desc_ptr->rbb[u] << (8*((4-u)-1)) );
  }

#ifdef  GRLD_SHOW_PDAN
  /* SnFaVrVq contains SSN, FAI and VR and VQ into 4 bytes MSB first
  */
  MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_24, "GRLD SnFaVrVq %08x RBB %08x %08x",
          (((uint32)(desc_ptr->ssn)) << 24 |
           ((uint32)fai)             << 16 |
           ((uint32)dl[gas_id].vr)   <<  8 |
           ((uint32)dl[gas_id].vq)
          ),
          hu32,
          lu32
         );
#endif

  rlc_log_dl_acknack_params(gas_id, dl[gas_id].tfi, fai, (uint8)dl[gas_id].vr,
                            (uint8)dl[gas_id].vq, hu32, lu32, dl[gas_id].cs);

} /* show_dl_rbb */

/*===========================================================================
===
===  FUNCTION      SHOW_DL_VARS()
===
===  DESCRIPTION
===
===    Show values of internal variables of Downlink State Machine
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
void show_dl_vars ( gas_id_t gas_id )
{
  uint32  u1=0UL, u2=0UL, u3=0UL;

  /*   Tbf_mode (28-31)
  **       rlc_mode (24-27)
  **           state (20-23)
  **               event (16-19)
  **                      mcs (12-15)
  **                          window size (0-11)
  **   f   f   f   f      f   f   f   f
  */

  u1 =     ((uint32)dl[gas_id].tbf_mode << 28 ) |
           ((uint32)dl[gas_id].dl_ass.rlc_mode << 24 ) |
           ((uint32)dl[gas_id].state           << 20 ) |
           ((uint32)dl[gas_id].event           << 16 ) |

           ((uint32)dl[gas_id].cs             << 12 ) |
           ((uint32)dl[gas_id].ws & 0xfff);

  u2 = (uint32)((uint32)dl[gas_id].vq << 16) | (uint32)dl[gas_id].vr;
  u3 = (uint32)((uint32)dl[gas_id].ssn << 16) | (uint32)dl[gas_id].pbsn;

  MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_25, "ERLD TRSEmWsz=%08x 00vq00vr=%08x ssnPBSN=%08x",u1,u2,u3);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u1; (void)u2; (void)u3;
}

/*===========================================================================
===
===  FUNCTION      SHOW_EPDAN()
===
===  DESCRIPTION
===
===    Show the values of the current EDGE Packet Downlink Acknowledgement Message
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
void show_epdan (gas_id_t gas_id, const rm_dl_pkt_acknack_req_t  *dl_acknack_ptr)
{
  uint32 u1=0UL, u2=0UL, u3=0UL;

  /*-----------------------------------------------------------
  **   FBI (28)
  **   |   BOW (24)
  **   |   |   EOW (20)
  **   |   |   |   ESP (16)
  **   |   |   |   |       comp_present (12)
  **   |   |   |   |       |   scc (8)
  **   |   |   |   |       |   |   crbb_len
  **   |   |   |   |       |   |   |___|
  **   f   f   f   f       f   f   f   f
  **-----------------------------------------------------------
  */
  u1 = (uint32)dl_acknack_ptr->e_acknack_desc.fai << 28 |
       (uint32)dl_acknack_ptr->e_acknack_desc.bow << 24 |
       (uint32)dl_acknack_ptr->e_acknack_desc.eow << 20 |
       (uint32)dl[gas_id].sp << 16 |

       (uint32)dl_acknack_ptr->e_acknack_desc.comp_present << 12 |
       (uint32)dl_acknack_ptr->e_acknack_desc.scc << 8 |
       (uint32)dl_acknack_ptr->e_acknack_desc.crbb_len;

  /*-----------------------------------------------------------
  **        SSN                PBSN
  **   |____________|      |____________|
  **   f   f   f   f       f   f   f   f
  **-----------------------------------------------------------
  */
  u2 = (uint32)dl_acknack_ptr->e_acknack_desc.ssn << 16 |
       (uint32)dl[gas_id].pbsn;

  /*-----------------------------------------------------------
  **   urbb_len    VQ          VR
  **    (24)      (12)
  **   |___|   |_______|   |_______|
  **   f   f   f   f   f   f   f   f
  **-----------------------------------------------------------
  */
  u3 = (uint32)dl_acknack_ptr->e_acknack_desc.urbb_len << 24 |
       (uint32)dl[gas_id].vq << 12 |
       (uint32)dl[gas_id].vr;

  if( (dl[gas_id].state != DS_RX_UNACK_PENDING_RELEASE) &&
      (dl[gas_id].state != DS_RX_UNACK )                   )
  {
    MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_26, "ERLD epdan FBEeCSLe=%08x ssnPBSN=%08x DL0vq0vr=%08x",u1,u2,u3);
  }
  else
  {
    MSG_GRLC_EXT_HIGH_3_G(GRLC_F3_MSG_ENUM_27, "ERLD U epdan FBEeCSLe=%08x ssnPBSN=%08x DL0vq0vr=%08x",u1,u2,u3);
  }

  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u1; (void)u2; (void)u3;

  if (dl_acknack_ptr->e_acknack_desc.comp_present)
  {
    MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_28, "           crbb[2]=%08x [1]=%08x [0]=%08x",
            dl_acknack_ptr->e_acknack_desc.crbb[2],
            dl_acknack_ptr->e_acknack_desc.crbb[1],
            dl_acknack_ptr->e_acknack_desc.crbb[0]);
    MSG_GRLC_EXT_MED_2_G(GRLC_F3_MSG_ENUM_29, "           crbb[3]=%08x len=%u dec",
            dl_acknack_ptr->e_acknack_desc.crbb[3],
            dl_acknack_ptr->e_acknack_desc.crbb_len
            );
  }

  if (dl_acknack_ptr->e_acknack_desc.urbb_len != 0)
  {
    MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_30, "           urbb[2]=%08x [1]=%08x [0]=%08x",
            dl_acknack_ptr->e_acknack_desc.urbb[2],
            dl_acknack_ptr->e_acknack_desc.urbb[1],
            dl_acknack_ptr->e_acknack_desc.urbb[0]);
    MSG_GRLC_EXT_MED_3_G(GRLC_F3_MSG_ENUM_31, "               [4]=%08x [3]=%08x len=%u dec ",
            dl_acknack_ptr->e_acknack_desc.urbb[4],
            dl_acknack_ptr->e_acknack_desc.urbb[3],
            dl_acknack_ptr->e_acknack_desc.urbb_len);
  }

#if 0
  /*-----------------------------------------------------------------------------
  ** Code to help debug PL1's DSM problem
  **-----------------------------------------------------------------------------
  */
  MSG_GRLC_LOW_3_G(GRLC_F3_MSG_ENUM_32, "ERLD cnt_a=%08x cnt_b=%08x cnt_c=%08x",
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_a,
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_b,
            dl_diag_stats_st[gas_id].dl_gen_purpose_count_c
         );
#endif

} /* show_epdan */

#ifdef ERLD_DUMP_12BYTES_PLD
/*===========================================================================
===
===  FUNCTION      E_DUMP_DL_PLD()
===
===  DESCRIPTION
===
===   Dumps the first 12 octets of the dl payloads in hex format in a F3 message.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void e_dump_dl_pld( gas_id_t gas_id )
{
  gas_id_t  gas_id,
  uint8     i, *ptr;
  uint16    *hdr_ptr;
  uint32    a, b, c;

  MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_33, "ERLD enter dump mode on bsn=%d", dl[gas_id].bsn);

  for( i = 0; i <= GRLC_DL_PL1_FIFO_SIZE; i++ )
  {
    /* Get the block's header */
    hdr_ptr = &dl[gas_id].dump_buff[i].hdr[0];

    a = 0L;
    a |=  (uint32)(*hdr_ptr++) << 16;
    a |=  (uint32)(*hdr_ptr++) << 0;

    b = 0L;
    b |=  (uint32)(*hdr_ptr++) << 16;
    b |=  (uint32)(*hdr_ptr++) << 0;

    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_34, "ERLD dump hdr bsn=%d %08x %08x",
      (E_GET_RAW_DL_BSN1(&dl[gas_id].dump_buff[i].hdr[0])), a, b );

    /* Get the block's 1st payload */
    ptr = &dl[gas_id].dump_buff[i].pld1[0];

    a = 0L;
    a |=  (uint32)(*ptr++) << 24;
    a |=  (uint32)(*ptr++) << 16;
    a |=  (uint32)(*ptr++) << 8;
    a |=  (uint32)(*ptr++) << 0;

    b = 0L;
    b |=  (uint32)(*ptr++) << 24;
    b |=  (uint32)(*ptr++) << 16;
    b |=  (uint32)(*ptr++) << 8;
    b |=  (uint32)(*ptr++) << 0;

    c = 0L;
    c |=  (uint32)(*ptr++) << 24;
    c |=  (uint32)(*ptr++) << 16;
    c |=  (uint32)(*ptr++) << 8;
    c |=  (uint32)(*ptr++) << 0;

    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_35, "ERLD dump 1st pld %08x %08x %08x", a, b, c );


    ptr = &dl[gas_id].dump_buff[i].pld2[0];

    a = 0L;
    a |=  (uint32)(*ptr++) << 24;
    a |=  (uint32)(*ptr++) << 16;
    a |=  (uint32)(*ptr++) << 8;
    a |=  (uint32)(*ptr++) << 0;

    b = 0L;
    b |=  (uint32)(*ptr++) << 24;
    b |=  (uint32)(*ptr++) << 16;
    b |=  (uint32)(*ptr++) << 8;
    b |=  (uint32)(*ptr++) << 0;

    c = 0L;
    c |=  (uint32)(*ptr++) << 24;
    c |=  (uint32)(*ptr++) << 16;
    c |=  (uint32)(*ptr++) << 8;
    c |=  (uint32)(*ptr++) << 0;

    /* Get the block's 2nd payload */
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_36, "ERLD dump 2nd pld %08x %08x %08x", a, b, c );

  } /* end of for loop */
}
#endif /* ERLD_DUMP_12BYTES_PLD  */

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_DL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Downlink Statistics info
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
grlc_diag_dl_statistics_t  grlc_diag_dl_statistics_s[NUM_GERAN_DATA_SPACES];
const grlc_diag_dl_statistics_t* grlc_diag_get_dl_statistics ( gas_id_t gas_id )
{
  grlc_diag_dl_statistics_s[gas_id] = dl_diag_stats_st[gas_id];

  return &grlc_diag_dl_statistics_s[gas_id];
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_DL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to clear Downlink Statistics info
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
void grlc_diag_clr_dl_statistics ( gas_id_t gas_id )
{
  (void)memset(&dl_diag_stats_st[gas_id], 0, sizeof(dl_diag_stats_st[gas_id]));
}

/*-------------------------------------------------------------------------*/

#ifdef  GRLD_SHOW_BLOCK
/*===========================================================================
===
===  FUNCTION      SHOW_BLOCK()
===
===  DESCRIPTION
===
===    Display contents of a DL data block by packing BSN, FBI, E, LI, M, E
===    fields into a three 32-bit hex numbers
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
#ifdef  GRLD_SHOW_BLOCK
void show_block 
(
  gas_id_t              gas_id,
  grlc_dl_data_block_t *blk_ptr
)
#else
static void show_block (grlc_dl_data_block_t *blk_ptr)
#endif
{
  uint32 lu1=0L, lu2=0L, lu3=0L;

  dsm_item_type *dsm_ptr;
  uint16 len, l;
  uint8 buf[64];
  uint8 *c_ptr;

  if (blk_ptr == NULL)
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_37, "GRLD blk_ptr is NULL");
  }
  else
  {
    /*        24       16        8    4   0
    ** |--------|--------|--------|--------|
    **      #Ext      Len      BSN  FBI   E
    */
    lu1 = ((uint32)blk_ptr->num_ext_octets) << 24 |
          ((uint32)blk_ptr->len)            << 16 |
          ((uint32)blk_ptr->bsn)            <<  8 |
          ((uint32)blk_ptr->fbi)            <<  4 |
          ((uint32)blk_ptr->ext);

    if (blk_ptr->num_ext_octets > 0)    /* 1 */
    {
      /*        24   20  16
      ** |--------|--------|--------|--------|
      **        LI    M   E
      */
      lu2 = ((uint32)blk_ptr->ext_octet[0].gprs.li)   << 24 |
            ((uint32)blk_ptr->ext_octet[0].gprs.more) << 20 |
            ((uint32)blk_ptr->ext_octet[0].gprs.ext)  << 16;

      if (blk_ptr->num_ext_octets > 1)  /* 2 */
      {
        /*                           8    4   0
        ** |--------|--------|--------|--------|
        **                          LI    M   E
        */
        lu2 |= ((uint32)blk_ptr->ext_octet[1].gprs.li)   << 8 |
               ((uint32)blk_ptr->ext_octet[1].gprs.more) << 4 |
               ((uint32)blk_ptr->ext_octet[1].gprs.ext);

        if (blk_ptr->num_ext_octets > 2)  /* 3 */
        {
          /*        24   20  16
          ** |--------|--------|--------|--------|
          **        LI    M   E
          */
          lu3 = ((uint32)blk_ptr->ext_octet[2].gprs.li)   << 24 |
                ((uint32)blk_ptr->ext_octet[2].gprs.more) << 20 |
                ((uint32)blk_ptr->ext_octet[2].gprs.ext)  << 16;

          if (blk_ptr->num_ext_octets > 3)  /* 4 */
          {
            /*                           8    4   0
            ** |--------|--------|--------|--------|
            **                          LI    M   E
            */
            lu3 |= ((uint32)blk_ptr->ext_octet[3].gprs.li)   << 8 |
                   ((uint32)blk_ptr->ext_octet[3].gprs.more) << 4 |
                   ((uint32)blk_ptr->ext_octet[3].gprs.ext);
          }
        }
      }
    }

    if (blk_ptr->num_ext_octets == 0)
    {
      MSG_GRLC_HIGH_1_G(GRLC_F3_MSG_ENUM_38, "GRLD NeLeBsFE %08x", lu1);
    }
    else if (blk_ptr->num_ext_octets == 1 || blk_ptr->num_ext_octets == 2)
    {
      MSG_GRLC_HIGH_2_G(GRLC_F3_MSG_ENUM_39, "GRLD NeLeBsFE %08x LiMELiME %08x", lu1, lu2);
    }
    else
    {
      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_40, "GRLD NeLeBsFE %08x LiMELiME %08x %08x", lu1, lu2, lu3);
    }

    /* This section shows the complete block's content. The payload in DSM format is
    ** copied and stripped into straight memory for showing
    */
    memset(buf,0,64);
    c_ptr=buf;
    len = 0;

    dsm_ptr=gprs_pdu_copy(blk_ptr->dsm_payload_ptr);
    len=(uint16)GPRS_PDU_LENGTH(dsm_ptr);
    (void)gprs_pdu_strip_head(&dsm_ptr, buf, len);
    MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_41, "dsm_a_len=%u len=%u cs=%d", len, blk_ptr->len, blk_ptr->cs);

    for (l=0; l<((len/12)+1); l++)
    {
      lu1  = 0L;
      lu1 |= (uint32)(*c_ptr++) << 24;
      lu1 |= (uint32)(*c_ptr++) << 16;
      lu1 |= (uint32)(*c_ptr++) <<  8;
      lu1 |= (uint32)(*c_ptr++);

      lu2  = 0L;
      lu2 |= (uint32)(*c_ptr++) << 24;
      lu2 |= (uint32)(*c_ptr++) << 16;
      lu2 |= (uint32)(*c_ptr++) <<  8;
      lu2 |= (uint32)(*c_ptr++);

      lu3  = 0L;
      lu3 |= (uint32)(*c_ptr++) << 24;
      lu3 |= (uint32)(*c_ptr++) << 16;
      lu3 |= (uint32)(*c_ptr++) <<  8;
      lu3 |= (uint32)(*c_ptr++);

      MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_42, "GRLD DATA BLK: %08x %08x %08x", lu1, lu2, lu3);
    }
  } /* blk_ptr valid */
}
#endif  /* GRLD_SHOW_BLOCK */

#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined(PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST / PERLUTF */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_PDU_STAT_ADD_ENTRY
===
===  DESCRIPTION
===
===     Adds an Downlink PDU to the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_diag_pdu_stat_add_entry(uint32 pdu_num, grlc_dl_pdu_t *ptr, const gas_id_t gas_id)
{
  grlc_diag_dl_pdu_t *diag_ptr;

  if (ptr == NULL)
  {
    return;
  }

  if (dl_diag_pdu_stat[gas_id].cnt == GRLC_DIAG_DL_PDU_BUF_MAX)
  {
    rlc_log_dl_statistics_ext(gas_id);
  }

  diag_ptr = &dl_diag_pdu_stat[gas_id].buf[dl_diag_pdu_stat[gas_id].index];
  diag_ptr->pdu_num   = pdu_num;
  diag_ptr->len       = ptr->len;
  diag_ptr->first_bsn = ptr->first_bsn;
  diag_ptr->last_bsn  = ptr->last_bsn;

  INC_MOD(dl_diag_pdu_stat[gas_id].index, GRLC_DIAG_DL_PDU_BUF_MAX);
  dl_diag_pdu_stat[gas_id].cnt++;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_PDU_STAT_REMOVE_ENTRY
===
===  DESCRIPTION
===
===     Removes the oldest Downlink PDU from the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Downlink PDU if present, otherwise NULL
===
===  SIDE EFFECTS
===
===========================================================================*/
grlc_diag_dl_pdu_t* grlc_dl_diag_pdu_stat_remove_entry(const gas_id_t gas_id)
{
  grlc_diag_dl_pdu_t *diag_pdu = NULL;

  if (dl_diag_pdu_stat[gas_id].cnt > 0)
  {
    /* Remove the oldest PDU */
    uint8 remove_index = MOD_DIF(dl_diag_pdu_stat[gas_id].index, dl_diag_pdu_stat[gas_id].cnt, GRLC_DIAG_DL_PDU_BUF_MAX);
    diag_pdu = &dl_diag_pdu_stat[gas_id].buf[remove_index];
    DEC_MOD(dl_diag_pdu_stat[gas_id].cnt, GRLC_DIAG_DL_PDU_BUF_MAX);
  }

  return diag_pdu;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_PDU_STAT_GET_COUNT
===
===  DESCRIPTION
===
===     Returns number of Downlink PDUs present in the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Number of Downlink PDUs present
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 grlc_dl_diag_pdu_stat_get_count(const gas_id_t gas_id)
{
  return dl_diag_pdu_stat[gas_id].cnt;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_RX_STAT_ADD_ENTRY
===
===  DESCRIPTION
===
===     Adds Downlink Rx Block info to the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_diag_rx_stat_add_entry(grlc_dl_data_block_t *ptr, const gas_id_t gas_id)
{
  grlc_diag_dl_rx_t *diag_ptr;

  if (ptr == NULL)
  {
    return;
  }

  if (dl_diag_rx_stat[gas_id].cnt == GRLC_DIAG_DL_RX_BUF_MAX)
  {
    rlc_log_dl_statistics_ext(gas_id);
  }

  diag_ptr = &dl_diag_rx_stat[gas_id].buf[dl_diag_rx_stat[gas_id].index];
  diag_ptr->time_slot = dl[gas_id].time_slot;
  diag_ptr->crc       = ptr->crc;
  diag_ptr->cs        = ptr->cs;
  diag_ptr->bsn       = ptr->bsn;
  diag_ptr->len       = ptr->len;
  diag_ptr->vn        = E_RX_ARR(gas_id, ptr->bsn).vn;
  diag_ptr->sp        = e_pl1_dl_blk[gas_id].esp;
  diag_ptr->fbi       = ptr->fbi;

  INC_MOD(dl_diag_rx_stat[gas_id].index, GRLC_DIAG_DL_RX_BUF_MAX);
  dl_diag_rx_stat[gas_id].cnt++;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_RX_STAT_REMOVE_ENTRY
===
===  DESCRIPTION
===
===     Removes the oldest Downlink Rx Block info from the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Downlink Rx block if present, otherwise NULL
===
===  SIDE EFFECTS
===
===========================================================================*/
grlc_diag_dl_rx_t* grlc_dl_diag_rx_stat_remove_entry(const gas_id_t gas_id)
{
  grlc_diag_dl_rx_t *diag_rx = NULL;

  if (dl_diag_rx_stat[gas_id].cnt > 0)
  {
    /* Remove the oldest Tx block */
    uint8 remove_index = MOD_DIF(dl_diag_rx_stat[gas_id].index, dl_diag_rx_stat[gas_id].cnt, GRLC_DIAG_DL_RX_BUF_MAX);
    diag_rx = &dl_diag_rx_stat[gas_id].buf[remove_index];
    DEC_MOD(dl_diag_rx_stat[gas_id].cnt, GRLC_DIAG_DL_RX_BUF_MAX);
  }

  return diag_rx;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_RX_STAT_GET_COUNT
===
===  DESCRIPTION
===
===     Returns number of Downlink Rx Blocks present in the diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     Number of Downlink Rx blocks present
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 grlc_dl_diag_rx_stat_get_count(const gas_id_t gas_id)
{
  return dl_diag_rx_stat[gas_id].cnt;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_DIAG_CLEAR_STAT
===
===  DESCRIPTION
===
===     Clears the Downlink diag statistics buffer
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_diag_clear_stat(const gas_id_t gas_id)
{
  if (dl_diag_pdu_stat[gas_id].cnt != 0)
  {
    MSG_GERAN_ERROR_1_G("GRLC DL Statistics:: PDU counter (%d) reset to zero", dl_diag_pdu_stat[gas_id].cnt);
    dl_diag_pdu_stat[gas_id].cnt = 0;
  }

  if (dl_diag_rx_stat[gas_id].cnt != 0)
  {
    MSG_GERAN_ERROR_1_G("GRLC DL Statistics:: RX Block counter (%d) reset to zero", dl_diag_rx_stat[gas_id].cnt);
    dl_diag_rx_stat[gas_id].cnt = 0;
  }
}

/*** EOF: don't remove! ***/