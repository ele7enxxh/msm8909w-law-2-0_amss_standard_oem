/*==============================================================================

                       C S N . 1   E N C O D E

GENERAL DESCRIPTION
  Translation services for RLC/MAC Control Messages, over-the-air external
  data format and internal data structure representation.

EXTERNALIZED FUNCTIONS
  gcsn1_encoder
  gcsn1_rr_encoder

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None required

Copyright (c) 2007-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/csn_encode.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
07-06-15  sjw     Mainlined the refactored CSN files. Feature def is not required.
06-05-09  tjw     Initial version carved from
                  //depot/asic/msmshared/geran/core/gcommon/csn_decode_encode.c#26
                  as a baseline for refactoring.

==============================================================================*/
/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"


#include "bit.h"

  #include "sys_type.h"

#include "csn_aptx.h"
#include "csn_api.h"

#include "string.h"

/* added for GMACUTF */
#include "msg.h"
#include "err.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

#define CSN_ENCODE_ARFCN(msg_ptr, msg_pos_ptr) \
  {gpackw((msg_ptr)->arfcn.num, src, (msg_pos_ptr), CSN_ARFCN_LEN);}



#define MAX_POS_PTR ((uint16) (23 * 8))
#define MAX_ENCODE_BITS ((uint16)(UPLINK_MESSAGE_SIZE << 3))

#define MAX_SIZE_IN_BITS_OF_DST 176

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


//  CSN.1 message processing Functions


static void csn_enc_ack_nack_ie (
  uint8 src[],
  uint16 *pos_ptr,
  const ack_nack_desc_t *msg_ptr);
static void csn_enc_chan_req_desc_ie(
  uint8 src[],
  uint16 *pos_ptr,
  channel_req_desc_t *msg_ptr);
static void csn_enc_global_tfi_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const global_tfi_t *msg_ptr);
static uint16 csn_enc_nc_meas_rprt_strct(
  uint8 src[],
  uint16 *pos_ptr,
  const enc_nc_meas_rprt_strct_t *msg_ptr);
static void csn_enc_padding_bits(
  uint8 src[],
  uint16 *pos_ptr);
static void csn_enc_rx_si_msg_struct(
  uint8 src[],
  uint16 *pos_ptr,
  rx_si_msg_struct_t *msg_ptr);
static void csn_enc_rx_unknw_si_msg_struct(
  uint8 src[],
  uint16 *pos_ptr,
  const rx_unknw_si_msg_struct_t *msg_ptr);
static void csn_enc_egprs_chn_qual_rpt_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const egprs_chn_qual_rpt_t *msg_ptr);
static uint8 csn_enc_egprs_ack_nack_des_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const egprs_ack_nack_des_t *msg_ptr);


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/



/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/
#define l_gpackb(src, dst, pos_ptr, len) csn_safe_ul_msg_gpackb(src, dst, pos_ptr, len)

#define gpackb(src, dst, pos_ptr, len) (void)csn_safe_ul_msg_gpackb(src, dst, pos_ptr, len)

#define gpackw(src, dst, pos_ptr, len) csn_safe_ul_msg_gpackw(src, dst, pos_ptr, len)

#define gpackd(src, dst, pos_ptr, len) csn_safe_ul_msg_gpackd(src, dst, pos_ptr, len)

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Encoder CSN building functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_safe_ul_msg_gpackb()
===
===  DESCRIPTION
===
===    Packs the given byte into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
===
===    Checks that the request to pack will not overflow the MAX size of the
===    UL control msg.
===
===    If insufficient space, to pack all the requested bits. It will pack those
===    it can without causing an overflow of UL control msg buffer and discard
===    the rest.
===
===    If UL control msg buffer is full, request to pack is ignored.
===
===  PARAMETERS
===
===    src:     byte value representing value of a data field.
===
===    dst:     byte array representing part of an external bitstream.
===
===    pos_ptr: pointer to the position var. Ranges from 0 (MSBit) to 7 (LSBit)
===
===              MSB           LSB
===             +-+-+-+-+-+-+-+-+
===             |8|7|6|5|4|3|2|1|
===             +-+-+-+-+-+-+-+-+
===             |0 0 0 1 1 1 1 0|
===             +-+-+-+-+-+-+-+-+
===              ------> *pos_ptr = 3, len = 4
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_packb()
===
===  RETURN VALUE
===
===    Value of input length, regardless of truncation by encoder
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/

uint16 csn_safe_ul_msg_gpackb
(
   uint8  src,
   uint8  dst[],
   uint16 *pos_ptr,
   uint16 len
)
{
   uint16 fill_remaining_space = 0;


   if((*pos_ptr + len) < MAX_SIZE_IN_BITS_OF_DST )
   {
     b_packb(src, dst, *pos_ptr, len);
     *pos_ptr += len;
   }
   else
   {
     fill_remaining_space = MAX_SIZE_IN_BITS_OF_DST - *pos_ptr ;

     if(fill_remaining_space > 0)
     {
        MSG_GERAN_ERROR_3("Ul ctrl msg full: Current bit pos=%d remaining space=%d len=%d",
                *pos_ptr,fill_remaining_space,len);

        /*Shift scr so that only the msb bits are added*/
        src = ( src >> (len - fill_remaining_space));

        b_packb(src, dst, *pos_ptr, fill_remaining_space);
        *pos_ptr += fill_remaining_space;
     }
   }

   return(len);
}

/*===========================================================================
===
===  FUNCTION      csn_safe_ul_msg_gpackw()
===
===  DESCRIPTION
===
===    Packs the given word into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
===
===    Checks that the request to pack will not overflow the MAX size of the
===    UL control msg.
===
===    If insufficient space, to pack all the requested bits. It will pack those
===    it can without causing an overflow of UL control msg buffer and discard
===    the rest.
===
===    If UL control msg buffer is full, request to pack is ignored.
===
===  PARAMETERS
===
===    src:     word value representing value of a data field.
===
===    dst:     byte array representing part of an external bitstream.
===
===    pos_ptr: pointer to the position var.
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_packw()
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

void csn_safe_ul_msg_gpackw
(
   uint16 src,
   uint8  dst[],
   uint16 *pos_ptr,
   uint16 len
)
{
   uint16 fill_remaining_space = 0;

   if((*pos_ptr + len) < MAX_SIZE_IN_BITS_OF_DST )
   {
     b_packw(src, dst, *pos_ptr, len);
     *pos_ptr += len;
   }
   else
   {
     fill_remaining_space = MAX_SIZE_IN_BITS_OF_DST - *pos_ptr ;

     if(fill_remaining_space > 0)
     {
        MSG_GERAN_ERROR_3("Ul ctrl msg full: Current bit pos=%d remaining space=%d len=%d",
                *pos_ptr,fill_remaining_space,len);

        /*Shift scr so that only the msb bits are added*/
        src = ( src >> (len - fill_remaining_space));

        b_packw(src, dst, *pos_ptr, fill_remaining_space);
        *pos_ptr += fill_remaining_space;
     }
   }
}


/*===========================================================================
===
===  FUNCTION      csn_safe_ul_msg_gpackd()
===
===  DESCRIPTION
===
===    Packs the given dword into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
===
===    Checks that the request to pack will not overflow the MAX size of the
===    UL control msg.
===
===    If insufficient space, to pack all the requested bits. It will pack those
===    it can without causing an overflow of UL control msg buffer and discard
===    the rest.
===
===    If UL control msg buffer is full, request to pack is ignored.
===
===  PARAMETERS
===
===    src:     dword value representing value of a data field.
===
===    dst:     byte array representing part of an external bitstream.
===
===    pos_ptr: pointer to the position var.
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_packd()
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

void csn_safe_ul_msg_gpackd
(
  uint32 src,
  uint8  dst[],
  uint16 *pos_ptr,
  uint16 len
)
{
   uint16 fill_remaining_space = 0;

   if((*pos_ptr + len) < MAX_SIZE_IN_BITS_OF_DST )
   {
      b_packd(src, dst, *pos_ptr, len);
     *pos_ptr += len;
   }
   else
   {
     fill_remaining_space = MAX_SIZE_IN_BITS_OF_DST - *pos_ptr ;

     if(fill_remaining_space > 0)
     {
        MSG_GERAN_ERROR_3("Ul ctrl msg full: Current bit pos=%d remaining space=%d len=%d",
                *pos_ptr,fill_remaining_space,len);

        /*Shift scr so that only the msb bits are added*/
        src = ( src >> (len - fill_remaining_space));

        b_packd(src, dst, *pos_ptr, fill_remaining_space);
        *pos_ptr += fill_remaining_space;
     }
   }
}


/*===========================================================================
===
===  FUNCTION    csn_pkt_cell_chg_failure
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_cell_chg_failure(
  uint8 src[], uint16 *pos_ptr,
  const packet_cell_change_failure_t *msg_ptr)
{
  //  Insert the tlli field.
  gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);

  //  Insert the arfcn field.
  CSN_ENCODE_ARFCN(msg_ptr, pos_ptr);

  //  Insert the bsic field.
  gpackb(msg_ptr->bsic, src, pos_ptr, CSN_BSIC_LEN);

  //  Insert the cause field.
  gpackb(msg_ptr->cause, src, pos_ptr, CSN_CAUSE_LEN);

  if (msg_ptr->rel99 == TRUE)
  {
    //  include R99 additions
    gpackb(1, src, pos_ptr, (int16)1);

    // 3G UTRAN FDD
    if (msg_ptr->fdd_flag == TRUE)
    {
      gpackb(1, src, pos_ptr, (int16)1);

      gpackw(msg_ptr->fdd_arfcn, src, pos_ptr, CSN_FDD_ARFCN_LEN);
      gpackb(msg_ptr->fdd_diversity, src, pos_ptr, 1);

      if (msg_ptr->fdd_bandwidth_flag == TRUE)
      {
        gpackb(1, src, pos_ptr, (int16)1);
        gpackb(msg_ptr->fdd_bandwidth, src, pos_ptr, CSN_FDD_BANDWIDTH_LEN);
      }
      else
      {
        gpackb(0, src, pos_ptr, (int16)1);
      }
      gpackw(msg_ptr->fdd_scrambling_code, src, pos_ptr, CSN_FDD_SCRAMBLING_CODE_LEN);
    }
    else
    {
      gpackb(0, src, pos_ptr, (int16)1);
    }

    // 3G UTRAN TDD
    if (msg_ptr->tdd_flag == TRUE)
    {
      gpackb(1, src, pos_ptr, (int16)1);

      gpackw(msg_ptr->tdd_arfcn, src, pos_ptr, CSN_FDD_ARFCN_LEN);
      gpackb(msg_ptr->tdd_diversity, src, pos_ptr, 1);

      if (msg_ptr->tdd_bandwidth_flag == TRUE)
      {
        gpackb(1, src, pos_ptr, (int16)1);
        gpackb(msg_ptr->tdd_bandwidth, src, pos_ptr, CSN_FDD_BANDWIDTH_LEN);
      }
      else
      {
        gpackb(0, src, pos_ptr, (int16)1);
      }
      gpackb(msg_ptr->tdd_cell_param, src, pos_ptr, CSN_TDD_CELL_PARAM_LEN);
      gpackb(msg_ptr->tdd_sync_case, src, pos_ptr, 1);
    }
    else
    {
      gpackb(0, src, pos_ptr, (int16)1);
    }
  }
  else
  {
    //  no R99 additions
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);
}



/*===========================================================================
===
===  FUNCTION    csn_pkt_control_ack
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_control_ack(
  uint8 src[], uint16 *pos_ptr,
  const packet_control_acknowledge_t *msg_ptr)
{
  //  Insert the tlli field.
  gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);

  //  Insert the CTRL_ACK field.
  gpackb(msg_ptr->up_ctrl_ack, src, pos_ptr, CSN_UP_CTRL_ACK_LEN);

#ifdef FEATURE_GSM_EDTM
  if (msg_ptr->rel5_additions_flag == valid)
  {
    /* Release 5 extensions are present */
    gpackb(1, src, pos_ptr, 1);

    if (msg_ptr->tn_rrbp_flag == valid)
    {
      /* TN_RRBP field is present */
      gpackb(1, src, pos_ptr, 1);
      gpackb(msg_ptr->tn_rrbp, src, pos_ptr, CSN_TN_RRBP_LEN);
    }
    else
    {
      /* TN_RRBP field is not present */
      gpackb(0, src, pos_ptr, 1);
    }

    if (msg_ptr->grnti_ext_flag == valid)
    {
      /* G-RNTI extension field is present */
      gpackb(1, src, pos_ptr, 1);
      gpackb(msg_ptr->grnti_ext, src, pos_ptr, CSN_GRNTI_EXT_LEN);
    }
    else
    {
      /* G-RNTI extension field is not present */
      gpackb(0, src, pos_ptr, 1);
    }

    if (msg_ptr->rel6_additions_flag == valid)
    {
      /* Release 6 extensions are present */
      gpackb(1, src, pos_ptr, 1);

      if (msg_ptr->ctrl_ack_ext_flag == valid)
      {
        /* CTRL_ACK_EXTENSION is present */
        gpackb(1, src, pos_ptr, 1);
        gpackw(msg_ptr->ctrl_ack_extension, src, pos_ptr, CSN_CTRL_ACK_EXTENSION_LEN);
      }
      else
      {
        /* CTRL_ACK_EXTENSION is not present */
        gpackb(0, src, pos_ptr, 1);
      }
    }
    else
    {
      /* Release 6 extensions are not present */
      gpackb(0, src, pos_ptr, 1);
    }
  }
  else
  {
    /* Release 5 extensions are not present */
    gpackb(0, src, pos_ptr, 1);
  }
#endif /* FEATURE_GSM_EDTM */

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

}



/*===========================================================================
===
===  FUNCTION    csn_pkt_dwnlk_ack_nack
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_dwnlk_ack_nack(
  uint8 src[], uint16 *pos_ptr,
  packet_downlink_ack_nack_t *msg_ptr)
{
  uint8   tsn=0;
  uint8   mask_value=1;

  //  Insert the downlink_tfi field.
  gpackd(msg_ptr->downlink_tfi, src, pos_ptr, CSN_DOWNLINK_TFI_LEN);

  //  Insert ACK / NACK description ie
  csn_enc_ack_nack_ie (src, pos_ptr, &(msg_ptr->ack_nack_desc));

  //  Next field is optional, check if it is present.
  if (msg_ptr->chan_req_desc_flag)
  {
    //  Channel Request Description field is present
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert Channel Request Description
    csn_enc_chan_req_desc_ie(src, pos_ptr, &(msg_ptr->chan_req_desc));

  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }


  //  Insert the Channel Quality Report
  //  Insert the c_value field.
  gpackb(msg_ptr->c_value, src, pos_ptr, CSN_C_VALUE_LEN);

  //  Insert the rxqual field.
  gpackb(msg_ptr->rxqual, src, pos_ptr, CSN_RXQUAL_LEN);

  //  Insert the sign_var field.
  gpackb(msg_ptr->sign_var, src, pos_ptr, CSN_SIGN_VAR_LEN);

  //  The presence of the next fields determined by the i_level_bitmap
  while (tsn < 8)
  {
    if (msg_ptr->i_level_bitmap & mask_value)
    {
      //  Insert flag
      gpackb(1, src, pos_ptr, (int16)1);

      //  insert appropriate timeslot i level
      gpackb(msg_ptr->i_level[tsn], src, pos_ptr, CSN_I_LEVEL_LEN);
    }
    else
    {
      //  Insert flag
      gpackb(0, src, pos_ptr, (int16)1);
    }

    //  Update count and mask
    tsn++;
    mask_value <<=1;
  };

  //  Insert the R99 options (if present)
  if (msg_ptr->pfi_present)
  {
    //  PFI is only R99 option
    //  Set R99 additions bit
    gpackb(1, src, pos_ptr, (int16)1);

    //  Set PFI present bit
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert pfi field
    gpackb(msg_ptr->pfi, src, pos_ptr, CSN_PFI_LEN);
  }

  if ( *pos_ptr > MAX_ENCODE_BITS )
  {
    MSG_GERAN_ERROR_2("GMAC PDAN is greater than %d bits, size:%d", MAX_ENCODE_BITS, *pos_ptr);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

  /* Check that the encoded PDAN length is within the 23 octet message bounds. */

  if ( *pos_ptr > MAX_POS_PTR )
  {
    MSG_GERAN_FATAL_2( "CSN: PDAN Enc Len Err %d > %d", (*pos_ptr/8), (MAX_POS_PTR/8));
  }
}




/*===========================================================================
===
===  FUNCTION    csn_pkt_uplk_dummy_ctrl
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_uplk_dummy_ctrl(
  uint8 src[], uint16 *pos_ptr,
  const packet_uplk_dummy_cntrl_blk_t *msg_ptr)
{
  //  Insert the tlli field.
  gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

}


/*===========================================================================
===
===  FUNCTION    csn_pkt_meas_rprt
===
===  DESCRIPTION
===
===    This function compresses the data received from GRR / MAC into a format
===    suitable for transmission over the air interface.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    FAIL if the encoder determines that the encoding would overrun
===    PASS otherwise
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static uint8 csn_pkt_meas_rprt(
  uint8 src[],
  uint16 *pos_ptr,
  const packet_measurement_report_t *msg_ptr)
{
  uint16 max_pos;
  uint8  overrun_check;

  //  Insert the tlli field and initialise the local position counter
  gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);
  max_pos = *pos_ptr;

  if (msg_ptr->psi5_chng_mrk_flag)
  {
    //  Insert flag
    max_pos += l_gpackb(1, src, pos_ptr, (int16)1);

    //  insert PSI5 CHANGE MARK
    max_pos += l_gpackb(msg_ptr->psi5_chng_mrk, src, pos_ptr, CSN_PSI5_CHNG_MRK_LEN);
  }
  else
  {
    //  Insert flag
    max_pos += l_gpackb(0, src, pos_ptr, (int16)1);
  }

  //  NC Measurement Report Present
  //  Insert flag
  max_pos += l_gpackb(0, src, pos_ptr, (int16)1);

  //  Insert the nc meas parameters
  max_pos += csn_enc_nc_meas_rprt_strct(src, pos_ptr, &(msg_ptr->enc_nc_meas_rprt_strct));

  /* additions in Release 99 */
  max_pos += l_gpackb(1, src, pos_ptr, (uint16) 1);

  if (msg_ptr->ba_or_psi3_and_pmo_included == TRUE)
  {
    max_pos += l_gpackb(1, src, pos_ptr, (uint16) 1);
    if (msg_ptr->ba_or_psi3_change_mark == ba_present)
    {
      max_pos += l_gpackb(0, src, pos_ptr, (uint16) 1);
      max_pos += l_gpackb(msg_ptr->ba_used, src, pos_ptr, CSN_BA_USED_LEN);
      max_pos += l_gpackb(msg_ptr->three_g_ba_used, src, pos_ptr, CSN_THREE_G_BA_USED_LEN);
    }
    else
    {
      max_pos += l_gpackb(1, src, pos_ptr, (uint16) 1);
      max_pos += l_gpackb(msg_ptr->psi3_change_mark, src, pos_ptr, CSN_PSI3_CHANGE_MARK_LEN);
    }
    max_pos += l_gpackb(msg_ptr->pmo_used, src, pos_ptr, CSN_PMO_USED_LEN);
  }
  else
  {
    max_pos += l_gpackb(0, src, pos_ptr, (uint16) 1);
  }

#ifdef FEATURE_WCDMA

  /* 3G Measurement Report */
  if (msg_ptr->three_g_measurement_report_flag == TRUE)
  {
    uint8 i;

    max_pos += l_gpackb(1, src, pos_ptr, (uint16) 1);

    max_pos += l_gpackb(msg_ptr->three_g_measurement_report.n3g, src, pos_ptr, CSN_N3G_LEN);

    /* n3g is equal to 1 less than the number of entries. */
    for (i = 0; i <= msg_ptr->three_g_measurement_report.n3g; i++)
    {
      max_pos += l_gpackb(msg_ptr->three_g_measurement_report.three_g_cell_list[i].
                          three_g_cell_list_index, src, pos_ptr,
                          CSN_THREE_G_CELL_LIST_INDEX_LEN);
      max_pos += l_gpackb(msg_ptr->three_g_measurement_report.three_g_cell_list[i].
                          rep_quality, src, pos_ptr,
                          CSN_REP_QUALITY_LEN);
    }
  }
  else
  {
    max_pos += l_gpackb(0, src, pos_ptr, (uint16) 1);
  }

#else /* not FEATURE_WCDMA */

  /* default set to 0 (not present) */
  max_pos += l_gpackb(0, src, pos_ptr, (uint16) 1);

#endif /* FEATURE_WCDMA */

  /* check the overrun status **before** appending padding */
  if (max_pos <= MAX_ENCODE_BITS)
  {
    overrun_check = PASS;
  }
  else
  {
    overrun_check = FAIL;
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

  return(overrun_check);
}

static void csn_encode_packet_enhanced_measurement_report(
  uint8 dst[],
  uint16 *pos_ptr,
  const packet_enhanced_measurement_report_t *pemr)
{
  uint32 i;

  gpackd(pemr->tlli, dst, pos_ptr, CSN_TLLI_LEN);
  gpackb(pemr->nc_mode, dst, pos_ptr, 1);
  if (pemr->ba_used_and_3g_ba_used_valid)
  {
    gpackb(0, dst, pos_ptr, 1);
    gpackb(pemr->ba_used, dst, pos_ptr, 1);
    gpackb(pemr->ba_3g_used, dst, pos_ptr, 1);
  }
  else
  {
    gpackb(1, dst, pos_ptr, 1);
    gpackb(pemr->psi3_change_mark, dst, pos_ptr, CSN_PSI3_CHANGE_MARK_LEN);
  }
  gpackb(pemr->pmo_used, dst, pos_ptr, 1);
  gpackb(pemr->bsic_seen, dst, pos_ptr, 1);
  gpackb(pemr->scale, dst, pos_ptr, 1);

  if (pemr->serving_cell_data_valid)
  {
    gpackb(1, dst, pos_ptr, 1);
    gpackb(pemr->serving_cell_data.rxlev, dst, pos_ptr, CSN_SERVING_CELL_DATA_RXLEV);
    if (pemr->serving_cell_data.interference_valid)
    {
      gpackb(1, dst, pos_ptr, 1);
      gpackb(pemr->serving_cell_data.interference, dst, pos_ptr,
             CSN_SERVING_CELL_DATA_INTERFERENCE);
    }
    else
    {
      gpackb(0, dst, pos_ptr, 1);
    }
  }
  else
  {
    gpackb(0, dst, pos_ptr, 1);
  }

  for (i = 0; i < pemr->invalid_bsic_information_count; i++)
  {
    gpackb(1, dst, pos_ptr, 1);
    gpackb(pemr->invalid_bsic_information[i].bcch_freq_ncell, dst, pos_ptr,
           CSN_BCCH_FREQ_NCELL);
    gpackb(pemr->invalid_bsic_information[i].bsic, dst, pos_ptr, CSN_BSIC_LEN);
    gpackb(pemr->invalid_bsic_information[i].rxlev_ncell, dst, pos_ptr, CSN_RXLEV_NCELL);
  }
  gpackb(0, dst, pos_ptr, 1); /* end the invalid_bsic_information repeated structs */

  if (pemr->reporting_quantity_valid)
  {
    gpackb(1, dst, pos_ptr, 1);

    for (i = 0; i < MAX_REPORTING_QUANTITY; i++)
    {
      if (pemr->reporting_quantity[i].valid)
      {
        gpackb(1, dst, pos_ptr, 1);
        gpackb(pemr->reporting_quantity[i].value, dst, pos_ptr, CSN_REP_QUANT_LEN);
      }
      else
      {
        gpackb(0, dst, pos_ptr, 1);
      }
    }
  }
  else
  {
    gpackb(0, dst, pos_ptr, 1);
  }
}


/*===========================================================================
===
===  FUNCTION    csn_pkt_resource_req
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_resource_req(
  uint8 src[], uint16 *pos_ptr,
  packet_resource_request_t *msg_ptr)
{
  #define MAX_PRR_SIZE (22 * 8)
  #define REL_99_EXT_WITH_PFI_ONLY 13

  uint16  local_len;
  uint8   loop_count;
  uint8   tsn=0;
  uint8   mask_value=1;
  boolean prr_size_ok = FALSE;

  //  Next field is optional
  if (msg_ptr->access_type_flag)
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  insert ACCESS TYPE
    gpackb(msg_ptr->access_type, src, pos_ptr, CSN_ACCESS_TYPE_LEN);
  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  Next field is conditional
  if (msg_ptr->tfi_tlli_flag == CSN_USE_GLOBAL_TFI)
  {
    //  Global TFI present
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);

    //  Insert global tfi ie
    csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));
  }
  else
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert the tlli
    gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);

  }

  //  Next field is optional
  if (msg_ptr->ms_radio_access_cap_flag)
  {
    uint16 remaining_bits;
    uint8  remaining_data;

    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    local_len = msg_ptr->access_capability.ms_cap_len_bits;
#ifdef PERLUTF
    #error code not present
#endif
    //  Insert MS Radio Access Capability IE Value Part.
    for (loop_count=0;loop_count<(local_len/CSN_ACCESS_CAPABILITY_LEN);loop_count++)
    {
      //  Insert octets
      gpackb(msg_ptr->access_capability.ms_cap[loop_count], src,
                                        pos_ptr, CSN_ACCESS_CAPABILITY_LEN);
    }

    //  Determine if there is an incomplete octet at the end
    remaining_bits = local_len % CSN_ACCESS_CAPABILITY_LEN;
    if (remaining_bits)
    {
      //  There are some bits in the last octet
      //  Insert bits
      remaining_data =
        msg_ptr->access_capability.ms_cap[loop_count] >> (8 - remaining_bits);
      gpackb(remaining_data, src, pos_ptr, remaining_bits);
    }
  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  Insert Channel Request Description
  csn_enc_chan_req_desc_ie(src, pos_ptr, &(msg_ptr->chan_req_desc));

  //  Next field is optional
  if (msg_ptr->change_mark_flag)
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  insert change mark
    gpackb(msg_ptr->change_mark, src, pos_ptr, CSN_CHANGE_MARK_LEN);
  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  insert c_value
  gpackb(msg_ptr->c_value, src, pos_ptr, CSN_C_VALUE_LEN);

  //  Next field is optional
  if (msg_ptr->sign_var_flag)
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  insert sign_var
    gpackb(msg_ptr->sign_var, src, pos_ptr, CSN_SIGN_VAR_LEN);
  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  The presence of the next fields determined by the i_level_bitmap
  while (tsn < 8)
  {
    if (msg_ptr->i_level_tn_bitmap & mask_value)
    {
      //  Insert flag
      gpackb(1, src, pos_ptr, (int16)1);

      //  insert appropriate timeslot i level
      gpackb(msg_ptr->i_level_tn[tsn], src, pos_ptr, CSN_I_LEVEL_TN_PR_LEN);
    }
    else
    {
      //  Insert flag
      gpackb(0, src, pos_ptr, (int16)1);
    }

    //  Update count and mask
    tsn++;
    mask_value <<=1;
  };


  if(msg_ptr->include_egprs == TRUE)
  {
    if ( msg_ptr->pfi_present )
    {
      if ( ( *pos_ptr > (MAX_PRR_SIZE - REL_99_EXT_WITH_PFI_ONLY) ) )
      {
        MSG_GERAN_ERROR_0("GMAC info, PFI will not fit into PRR, NOT including rel "\
                   "99/PFI information in PRR msg");
        prr_size_ok = FALSE;
      }
      else
      {
        prr_size_ok = TRUE;
      }
    }
    else
    {
      prr_size_ok = TRUE;
    }

    if (prr_size_ok)
    {
      /*set EGPRS present flag*/
      gpackb(1, src, pos_ptr, (int16)1);

      /*                EGPRS BEP Link Quality Measurements                 */
      if(msg_ptr->egprs_bep_link_qual_present == TRUE)
      {

        /*EGPRS BEP Link Quality Measurements present flag*/
        gpackb(1, src, pos_ptr, (int16)1);

        /*< EGPRS BEP Link Quality Measurements       */
        /*comprising of GMSK and 8PSK BEp measurements*/
        if (msg_ptr->egprs_qual_rpt.gmsk_mean_bep_present == TRUE)
        {
          /*  GMSK MEAN BEP and GMSK CV BEP present        */
          gpackb(1, src, pos_ptr, (int16)1);

          /*insert gmsk_m_bep          5 bits*/
          gpackb(msg_ptr->egprs_qual_rpt.gmsk_m_bep, src, pos_ptr, CSN_GMSK_M_BEP_LEN);

          /*insert gmsk_cv_bep          3 bits*/
          gpackb(msg_ptr->egprs_qual_rpt.gmsk_cv_bep, src, pos_ptr, CSN_GMSK_CV_BEP_LEN);
        }
        else
        {
          /*  GMSK MEAN BEP and GMSK CV BEP not present        */
          gpackb(0, src, pos_ptr, (int16)1);
        }

        if (msg_ptr->egprs_qual_rpt.psk_mean_bep_present == TRUE)
        {
          /* 8PSK_MEAN_BEP and 8PSK CV BEP present */
          gpackb(1, src, pos_ptr, (int16)1);

          /* insert psk_m_bep 5 bits*/
          gpackb(msg_ptr->egprs_qual_rpt.psk_m_bep, src, pos_ptr, CSN_8PSK_M_BEP_LEN);

          /* insert psk_cv_bep  3 bits*/
          gpackb(msg_ptr->egprs_qual_rpt.psk_cv_bep, src, pos_ptr, CSN_8PSK_CV_BEP_LEN);
        }
        else
        {
          /* 8PSK_MEAN_BEP and 8PSK CV BEP NOT present */
          gpackb(0, src, pos_ptr, (int16)1);
        }
      }
      else
      {
        /*EGPRS BEP Link Quality Measurements NOT present flag*/
        gpackb(0, src, pos_ptr, (int16)1);
      }

      /*            EGPRS Timeslot Link Quality Measurements          */
      if(msg_ptr->egprs_ts_link_qual_present == TRUE)
      {
        /*EGPRS Timeslot Link Quality Measurements present flag*/
        gpackb(1, src, pos_ptr, (int16)1);

        /*{ 0 | 1< BEP_MEASUREMENTS : BEP Measurement Report Struct >} */
        if (msg_ptr->egprs_qual_rpt.bep_meas_rpt_flg == TRUE)
        {
          /* BEP_MEASUREMENTS PRESENT flag*/
          gpackb(1, src, pos_ptr, (int16)1);

          /*initialise */
          tsn = 0;
          mask_value = 1;

          //  The presence of the next fields determined by the tn_bitmap
          while (tsn < 8)
          {
            if (msg_ptr->egprs_qual_rpt.tn_bitmap & mask_value)
            {
              //Insert TN flag present
              gpackb(1, src, pos_ptr, (int16)1);

              if (msg_ptr->egprs_qual_rpt.psk_mean_bep_present & mask_value)
              {
                /*0 = GMSK MEAN BEP for TN present */
                gpackb(0, src, pos_ptr, (int16)1);
              }
              else
              {
                /*1 = 8PSK MEAN BEP for TN present */
                gpackb(1, src, pos_ptr, (int16)1);
              }

              /* insert MEAN BEP  4 bits*/
              gpackb(msg_ptr->egprs_qual_rpt.mean_bep[tsn], src, pos_ptr,
                     CSN_BEP_MEAN_LEN);

            }
            else
            {
              //Insert TN flag NOT present
              gpackb(0, src, pos_ptr, (int16)1);
            }

            //  Update count and mask
            tsn++;
            mask_value <<=1;
          };
        }
        else
        {
          /* BEP_MEASUREMENTS not PRESENT flag*/
          gpackb(0, src, pos_ptr, (int16)1);
        }
      }
      else
      {
        /*EGPRS Timeslot Link Quality Measurements NOT present flag*/
         gpackb(0, src, pos_ptr, (int16)1);
      }

      if(msg_ptr->pfi_present == TRUE)
      {
        /*PFI present flag*/
        gpackb(1, src, pos_ptr, (int16)1);
        /* insert PFI 7 bits*/
          gpackb(msg_ptr->pfi, src, pos_ptr, CSN_PFI_LEN);
      }
      else
      {
        /*PFI NOT present flag*/
        gpackb(0, src, pos_ptr, (int16)1);
      }

      /* insert additional_rac_info 1 bit*/
      gpackb(msg_ptr->additional_rac_info, src, pos_ptr, 1);

      /* insert rac_retran  1 bit*/
      gpackb(msg_ptr->rac_retran, src, pos_ptr, 1);
    }
  }
  else
  {
    /*set EGPRS NOT present flag*/
    gpackb(0, src, pos_ptr, (int16)1);
  }

  MSG_GERAN_HIGH_1("MAC CSN total length PRR: %d", *pos_ptr);

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

  /* Check that the encoded PRR length is within the 22 octet message bounds. */
  if ( *pos_ptr > MAX_PRR_SIZE )
  {
    MSG_GERAN_FATAL_2( "CSN: PRR Enc Len Err %d > %d", (*pos_ptr/8), (MAX_PRR_SIZE/8));
  }
}



/*===========================================================================
===
===  FUNCTION    csn_pkt_mobile_tbf_status
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_mobile_tbf_status(
  uint8 src[],
  uint16 *pos_ptr,
  const packet_mobile_tbf_status_t *msg_ptr)
{

  //  Insert global ie fields
  csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  //  insert TBF Cause
  gpackb(msg_ptr->tbf_cause, src, pos_ptr, CSN_TBF_CAUSE_LEN);

  //  Next field is optional
  if (msg_ptr->status_mess_type_flag)
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  insert Status Message Type
    gpackb(msg_ptr->status_mess_type, src, pos_ptr, CSN_STATUS_MESS_TYPE_LEN);
  }
  else
  {
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

}


/*===========================================================================
===
===  FUNCTION    csn_pkt_si_status
===
===  DESCRIPTION
===
===    This function compresses the SI stats received from GRR into a format
===    suitable for transmission over the air interface.
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

static void csn_pkt_si_status(
  uint8 src[], uint16 *pos_ptr,
  packet_si_status_t *msg_ptr)
{

  //  Insert global ie fields
  csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  //  insert BCCH change mark
  gpackb(msg_ptr->bcch_chng_mrk, src, pos_ptr, CSN_BCCH_CHNG_MRK_LEN);

  //  Insert Received SI Message List
  csn_enc_rx_si_msg_struct(src, pos_ptr, &(msg_ptr->rx_si_msg_struct));

  //  Insert Received Unknown SI Message List
  csn_enc_rx_unknw_si_msg_struct(src, pos_ptr, &(msg_ptr->rx_unknw_si_msg_struct));

  if (msg_ptr->rel6_additions_valid == TRUE)
  {
    // Insert bit to indicate Rel6 additions are present
    gpackb(1, src, pos_ptr, 1);

    // Insert PSCSI_SUPPORT - which must be 1 if Rel 6 is supported
    gpackb(1, src, pos_ptr, 1);

    // Insert PS_REL_REQ
    if (msg_ptr->ps_rel_req)
    {
      gpackb(1, src, pos_ptr, 1);
    }
    else
    {
      gpackb(0, src, pos_ptr, 1);
    }
  }
  else
  {
    // No Rel6 additions present
    gpackb(0, src, pos_ptr, 1);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

}

/*===========================================================================
===
===  FUNCTION    csn_encode_pkt_cell_change_notification
===
===  DESCRIPTION
===
===    This function compresses the PCCN received from GRR into a format
===    suitable for transmission over the air interface.
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

static void csn_encode_pkt_cell_change_notification(
  uint8 src[], uint16 *pos_ptr,
  packet_cell_change_notification_t *msg_ptr)
{
  uint8 meas;

  //  Insert global ie fields
  csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  if (msg_ptr->target_cell_3g_valid)
  {
    // message escape bits 10 - indicate 3G target cell follows
    gpackb(1, src, pos_ptr, 1);
    gpackb(0, src, pos_ptr, 1);

    // 3G TARGET CELL STRUCTURE FOLLOWS

    // FDD ARFCN
    if (msg_ptr->target_cell_3g.fdd_arfcn_valid)
    {
      gpackb(1, src, pos_ptr, 1);
      gpackw(msg_ptr->target_cell_3g.fdd_arfcn, src, pos_ptr, CSN_FDD_ARFCN_LEN);
      if (msg_ptr->target_cell_3g.fdd_bandwidth_valid)
      {
        gpackb(1, src, pos_ptr, 1);
        gpackb(msg_ptr->target_cell_3g.fdd_bandwidth, src, pos_ptr,
               CSN_FDD_BANDWIDTH_LEN);
      }
      else
      {
        // no FDD bandwidth present
        gpackb(0, src, pos_ptr, 1);
      }
      gpackw(msg_ptr->target_cell_3g.fdd_scrambling_code, src, pos_ptr,
             CSN_FDD_SCRAMBLING_CODE_LEN);
    }
    else
    {
      // no FDD ARFCN present
      gpackb(0, src, pos_ptr, 1);
    }

    // TDD ARFCN
    if (msg_ptr->target_cell_3g.tdd_arfcn_valid)
    {
      gpackb(1, src, pos_ptr, 1);
      gpackw(msg_ptr->target_cell_3g.tdd_arfcn, src, pos_ptr, CSN_TDD_ARFCN_LEN);
      if (msg_ptr->target_cell_3g.tdd_bandwidth_valid)
      {
        gpackb(1, src, pos_ptr, 1);
        gpackb(msg_ptr->target_cell_3g.tdd_bandwidth, src, pos_ptr,
               CSN_TDD_BANDWIDTH_LEN);
      }
      else
      {
        // no TDD bandwidth present
        gpackb(0, src, pos_ptr, 1);
      }
      gpackb(msg_ptr->target_cell_3g.tdd_cell_param, src, pos_ptr,
             CSN_TDD_CELL_PARAM_LEN);
      gpackb(msg_ptr->target_cell_3g.tdd_sync_case, src, pos_ptr, 1);
    }
    else
    {
      // no TDD ARFCN present
      gpackb(0, src, pos_ptr, 1);
    }

    // REPORTING QUANTITY
    gpackb(msg_ptr->target_cell_3g.rep_quant, src, pos_ptr, CSN_REP_QUANT_LEN);

  }
  else
  {
    // message escape bit 0 - indicates 2G ARFCN and BSIC follow
    gpackb(0, src, pos_ptr, 1);

    //  Insert the ARFCN and BSIC fields
    CSN_ENCODE_ARFCN(msg_ptr, pos_ptr);
    gpackb(msg_ptr->bsic, src, pos_ptr, CSN_BSIC_N_LEN);
  }

  if (msg_ptr->ba_or_psi3_change_mark == ba_present)
  {
    gpackb(0, src, pos_ptr, 1);
    gpackb(msg_ptr->ba_used, src, pos_ptr, 1);
  }
  else
  {
    gpackb(1, src, pos_ptr, 1);
    gpackb(msg_ptr->psi3_change_mark, src, pos_ptr, CSN_PSI3_CHANGE_MARK_LEN);
  }

  gpackb(msg_ptr->pmo_used, src, pos_ptr, 1);
  gpackb(msg_ptr->pccn_sending, src, pos_ptr, 1);

  // CCN Measurement Report
  gpackb(msg_ptr->ccn_meas_rpt.rxlev_srvg_cell, src, pos_ptr, CSN_RXLEV_SRVG_CELL_LEN);
  gpackb(0,src,pos_ptr,1); // interference measurements not included for Rel6 onwards

  gpackb(msg_ptr->ccn_meas_rpt.num_nc_meas, src, pos_ptr, CSN_NUM_NC_MEAS_LEN);
#ifdef PERLUTF
  #error code not present
#endif
  for (meas = 0; meas < msg_ptr->ccn_meas_rpt.num_nc_meas; meas++)
  {
    gpackb(msg_ptr->ccn_meas_rpt.nc_meas[meas].freq_n, src, pos_ptr, CSN_FREQ_N_LEN);
    if (msg_ptr->ccn_meas_rpt.nc_meas[meas].bsic_n_flag)
    {
      gpackb(1, src, pos_ptr, 1);
      gpackb(msg_ptr->ccn_meas_rpt.nc_meas[meas].bsic_n, src, pos_ptr, CSN_BSIC_N_LEN);
    }
    else
    {
      gpackb(0, src, pos_ptr, 1);
    }
    gpackb(msg_ptr->ccn_meas_rpt.nc_meas[meas].rxlev_n, src, pos_ptr, CSN_RXLEV_N_LEN);
  }

  if (msg_ptr->ccn_meas_3g_valid)
  {
    // single bit indicates that Release 6 additions are present
    gpackb(1, src, pos_ptr, 1);

    if (msg_ptr->ba_used_3g_valid)
    {
      gpackb(1, src, pos_ptr, 1);
      gpackb(msg_ptr->ba_used_3g, src, pos_ptr, 1);
    }
    else
    {
      gpackb(0, src, pos_ptr, 1);
    }

    // now encode the 3G measurement report
    if (msg_ptr->ccn_meas_3g.n_3g > (MAX_3G_MEAS-1))
    {
      msg_ptr->ccn_meas_3g.n_3g = MAX_3G_MEAS-1;
    }

    gpackb(msg_ptr->ccn_meas_3g.n_3g, src, pos_ptr, CSN_N_3G_LEN);

    // note deliberate <= condition here to encode (N_3G + 1) measurements
    for (meas = 0; meas <= msg_ptr->ccn_meas_3g.n_3g; meas++)
    {
      gpackb(msg_ptr->ccn_meas_3g.meas_3g[meas].lst_index, src, pos_ptr,
             CSN_LST_INDEX_LEN);
      gpackb(msg_ptr->ccn_meas_3g.meas_3g[meas].rep_quant, src, pos_ptr,
             CSN_REP_QUANT_LEN);
    }
  }
  else
  {
    // insert a single 0 to indicate no Release 6 additions
    gpackb(0, src, pos_ptr, 1);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);
}

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
static void csn_encode_pkt_cs_request(
  uint8 src[], uint16 *pos_ptr,
  const packet_cs_request_t *msg_ptr)
{
  /* Insert global TFI IE */
  csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  /* Insert the establishment cause */
  gpackb(msg_ptr->establishment_cause, src, pos_ptr, CSN_ESTABLISHMENT_CAUSE_LEN);

  /* Insert padding bits */
  csn_enc_padding_bits(src, pos_ptr);
}
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */


/*===========================================================================
===
===  FUNCTION    csn_egprs_pkt_dwnlk_ack_nack
===
===  DESCRIPTION
===
===    This function compresses the data recieved from RLC / MAC into a format
===    suitable for transmission over the air interface.
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

static uint8 csn_egprs_pkt_dwnlk_ack_nack(
  uint8 src[], uint16 *pos_ptr,
  egprs_packet_downlink_ack_nack_t *msg_ptr)
{
  uint8 encode_status = PASS;

  //  Insert the downlink_tfi field. 5 bits
  gpackd(msg_ptr->downlink_tfi, src, pos_ptr, CSN_DOWNLINK_TFI_LEN);

  //  Insert the out of memorey. 1 bit
  gpackd(msg_ptr->ms_out_of_mem, src, pos_ptr, 1);

  /*{ 0 | 1 < EGPRS Channel Quality Report : < EGPRS Channel Quality Report IE > >}*/
  if (msg_ptr->egprs_chn_qual_rpt_flag)
  {
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);
    csn_enc_egprs_chn_qual_rpt_ie(src, pos_ptr, &(msg_ptr->egprs_chn_qual_rpt));
  }
  else
  {
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }


  /*{ 0 | 1 < Channel Request Description : >Channel Request Description IE > >}*/
  if (msg_ptr->chan_req_desc_flag)
  {
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);
    csn_enc_chan_req_desc_ie(src, pos_ptr, &(msg_ptr->chan_req_desc));
  }
  else
  {
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  /*{ 0 | 1 < PFI : bit(7) > }*/
  if (msg_ptr->pfi_flag)
  {
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);
    gpackd(msg_ptr->pfi, src, pos_ptr, CSN_PFI_LEN);
  }
  else
  {
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  /*{ 0 | 1 < Extension Bits : Extension Bits IE > }*/
  if (msg_ptr->extension_bits_flag)
  {
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);
  }
  else
  {
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  /*< EGPRS Ack/Nack Description : < EGPRS Ack/Nack Description IE >>*/
  //new function here
  encode_status = csn_enc_egprs_ack_nack_des_ie(
                                                src,
                                                pos_ptr,
                                                &(msg_ptr->egprs_ack_nack_des));

  return encode_status;

}

/*===========================================================================
===
===  FUNCTION    csn_enc_egprs_chn_qual_rpt_ie
===
===  DESCRIPTION
===
===  Encodes egprs_chn_qual_rpt_ie
===========================================================================*/

static void csn_enc_egprs_chn_qual_rpt_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const egprs_chn_qual_rpt_t *msg_ptr )
{

  uint8   tsn;
  uint8   mask_value;

  /*< EGPRS BEP Link Quality Measurements       */
  /*comprising of GMSK and 8PSK BEp measurements*/
  if (msg_ptr->gmsk_mean_bep_present == TRUE)
  {
    /*  GMSK MEAN BEP and GMSK CV BEP present        */
    gpackb(1, src, pos_ptr, (int16)1);

    /*insert gmsk_m_bep          5 bits*/
    gpackb(msg_ptr->gmsk_m_bep, src, pos_ptr, CSN_GMSK_M_BEP_LEN);

    /*insert gmsk_cv_bep          3 bits*/
    gpackb(msg_ptr->gmsk_cv_bep, src, pos_ptr, CSN_GMSK_CV_BEP_LEN);

  }
  else
  {
    /*  GMSK MEAN BEP and GMSK CV BEP not present        */
    gpackb(0, src, pos_ptr, (int16)1);

  }

  if (msg_ptr->psk_mean_bep_present == TRUE)
  {
    /* 8PSK_MEAN_BEP and 8PSK CV BEP present */
    gpackb(1, src, pos_ptr, (int16)1);

    /* insert psk_m_bep 5 bits*/
    gpackb(msg_ptr->psk_m_bep, src, pos_ptr, CSN_8PSK_M_BEP_LEN);

    /* insert psk_cv_bep  3 bits*/
    gpackb(msg_ptr->psk_cv_bep, src, pos_ptr, CSN_8PSK_CV_BEP_LEN);
  }
  else
  {
    /* 8PSK_MEAN_BEP and 8PSK CV BEP NOT present */
    gpackb(0, src, pos_ptr, (int16)1);

  }

  /*< C_VALUE : bit (6) > */
  gpackb(msg_ptr->c_value, src, pos_ptr, CSN_C_VALUE_LEN);

  /* EGPRS Timeslot Link Quality Measurements                      */
  /* comprising of  BEP_MEASUREMENTS and INTERFERENCE_MEASUREMENTS */

  /*{ 0 | 1< BEP_MEASUREMENTS : BEP Measurement Report Struct >} */
  if (msg_ptr->bep_meas_rpt_flg == TRUE)
  {
    /* BEP_MEASUREMENTS PRESENT flag*/
    gpackb(1, src, pos_ptr, (int16)1);

    /*initialise */
    tsn = 0;
    mask_value = 1;

    //  The presence of the next fields determined by the tn_bitmap
    while (tsn < 8)
    {
      if (msg_ptr->tn_bitmap & mask_value)
      {
        //Insert TN flag present
        gpackb(1, src, pos_ptr, (int16)1);

        if( FALSE == (msg_ptr->gmsk_or_8psk_bitmap & mask_value) )
        {
          /*0 = GMSK MEAN BEP for TN present */
          gpackb(0, src, pos_ptr, (int16)1);
        }
        else
        {
          /*1 = 8PSK MEAN BEP for TN present */
          gpackb(1, src, pos_ptr, (int16)1);
        }

        /* insert MEAN BEP  4 bits*/
        gpackb(msg_ptr->mean_bep[tsn], src, pos_ptr, CSN_BEP_MEAN_LEN);

      }
      else
      {
        //Insert TN flag NOT present
        gpackb(0, src, pos_ptr, (int16)1);
      }

      //  Update count and mask
      tsn++;
      mask_value <<=1;
    };

  }
  else
  {
    /* BEP_MEASUREMENTS not PRESENT flag*/
    gpackb(0, src, pos_ptr, (int16)1);
  }

  /*{ 0 | 1 < INTERFERENCE_MEASUREMENTS : Interference Measurement Report Struct >}; */
  if(msg_ptr->int_meas_rpt_flg == TRUE)
  {
    /* INTERFERENCE_MEASUREMENTS Present flag */
    gpackb(1, src, pos_ptr, (int16)1);

    /*initialise */
    tsn = 0;
    mask_value = 1;

    //  The presence of the next fields determined by the i_level_bitmap
    while (tsn < 8)
    {
      if (msg_ptr->i_level_bitmap & mask_value)
      {
        //  Insert flag
        gpackb(1, src, pos_ptr, (int16)1);

        //  insert appropriate timeslot i level
        gpackb(msg_ptr->i_level[tsn], src, pos_ptr, CSN_I_LEVEL_LEN);
      }
      else
      {
        //  Insert flag
        gpackb(0, src, pos_ptr, (int16)1);
      }

      //  Update count and mask
      tsn++;
      mask_value <<=1;
    };

  }
  else
  {
    /* INTERFERENCE_MEASUREMENTS not Present flag */
    gpackb(0, src, pos_ptr, (int16)1);
  }

}


/*===========================================================================
===
===  FUNCTION    csn_enc_egprs_ack_nack_des_ie
===
===  DESCRIPTION
===
===  Encodes csn_enc_egprs_ack_nack_des_ie
===========================================================================*/
static uint8 csn_enc_egprs_ack_nack_des_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const egprs_ack_nack_des_t *msg_ptr)
{
  uint16 bitmap_length =0;
  uint8 array_cnt = 0;
  uint8  leftover_bits = 0;

  /*int_data->type.egprs_pkt_dwnlk_ack_nack.*/
  if (msg_ptr->len_present)
  {
    /*EGPRS Ack/Nack Description struct plus length ind*/
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);

    /*< Length L : bit (8) >*/
    gpackb(msg_ptr->length, src, pos_ptr,(int16)8);


  }
  else
  {
    /*0 < EGPRS Ack/Nack Description struct ONLY >*/
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }


  /*< FINAL_ACK_INDICATION : bit (1) >*/
  gpackb(msg_ptr->fai, src, pos_ptr, 1);

  /*< BEGINNING_OF_WINDOW : bit (1) >*/
  gpackb(msg_ptr->bow, src, pos_ptr, 1);

  /*< END_OF_WINDOW : bit (1) > */
  gpackb(msg_ptr->eow, src, pos_ptr, 1);

  /*< STARTING_SEQUENCE_NUMBER : bit (11) >*/
  gpackw(msg_ptr->ssn, src, pos_ptr, CSN_SSN_LEN);

  /**********     COMPRESSED_BITMAP_LENGTH   ******************/
  if (msg_ptr->comp_present)
  {
    // Insert present flag
    gpackb(1, src, pos_ptr, (int16)1);

    /*< COMPRESSED_BITMAP_LENGTH: bit (7) >*/
    gpackb(msg_ptr->crbb_len, src, pos_ptr, CSN_CRBB_LEN_LEN);

    /*< COMPRESSED_BITMAP_STARTING_COLOR_CODE: bit (1) >*/
    gpackb(msg_ptr->scc, src, pos_ptr, 1);

    /* for compressed bit map actual bit map length = crbb_len -  9*/
    bitmap_length = msg_ptr->crbb_len ;

    if( (*pos_ptr + msg_ptr->crbb_len) > MAX_ENCODE_BITS)
    {
      MSG_GERAN_ERROR_3("CSN E-PDAN insufficient left = %d current pos %d crbb_len %d",
                  (*pos_ptr + msg_ptr->crbb_len),
                  *pos_ptr,
                  msg_ptr->crbb_len
               );
      return FAIL;
    }

    /************         COMPRESSED_RECEIVED_BLOCK_BITMAP    ***************/

    if ( bitmap_length == 0 )
    {
      MSG_GERAN_FATAL_0("CSN EGPRS PDAN CRBB encode DIV by 0");
      return FAIL;
    }

    /* The CRBB array is supplied by RLC with the bit order INCREASING as the
       index increases.
       This is not how it should be sent OTA and requires re-ordering so that
       the highest number bit of the lowest number octet contains the highest
       order bit of the field.
       See 3GPP 44.060 sect 11, fig 11.1
     */

    /* calculate the number of elements in the array*/
    array_cnt = (uint8)(bitmap_length / 32);

    /* calculate the number of remaining bits not part of a complete 32-bit word*/
    leftover_bits = bitmap_length % 32;

    /*pack the leftover bits first*/
    if (leftover_bits)
    {
      gpackd(msg_ptr->crbb[array_cnt], src, pos_ptr, leftover_bits);
    }

    bitmap_length = bitmap_length - leftover_bits;
    array_cnt = array_cnt - 1;

    /* pack the remaining whole words*/
    while (bitmap_length >= 32)
    {
      gpackd(msg_ptr->crbb[array_cnt], src, pos_ptr, 32);
      bitmap_length -= 32;
      array_cnt -= 1;
    };
  }
  else
  {
    // Insert not present flag
    gpackb(0, src, pos_ptr, (int16)1);
  }

  /***********    UNCOMPRESSED_RECEIVED_BLOCK_BITMAP   *****************/

  /* Determine URBB bit len*/
  if (msg_ptr->len_present)
  {
    /* ACK/NACK DES does NOT fill rest of egprs pdan msg */
    if(msg_ptr->comp_present == TRUE)
    {
        /* when the compressed received block bitmap is included    */
        /* Lu         = L                - Lc                 - 23  */
        bitmap_length = (msg_ptr->length - msg_ptr->crbb_len) - 23;
    }
    else
    {
        /* when the compressed received block bitmap is not included  */
        /* Lu         = L               - 15                       */
        bitmap_length = msg_ptr->length - 15;
    }
  }
  else
  {
     /* ACK/NACK DES does fill rest of egprs pdan msg */

     /* MS can only use 1 block of 23 octets when sending control msgs.  */
     /* of which 1 octet is the header and 22 octets used to carrty date */
     /* thus max paylod in bits is (22 * 8)                              */

     bitmap_length = (22 * 8) - *pos_ptr ;
  }

  if( (*pos_ptr + bitmap_length) > MAX_ENCODE_BITS)
  {
    MSG_GERAN_ERROR_3("CSN E-PDAN insufficient space left=%d current pos=%d urbb_len=%d",
                (*pos_ptr - bitmap_length ),
                *pos_ptr,
                bitmap_length
             );
    return FAIL;
  }

  if ( bitmap_length )
  {

    /* The URBB array is supplied by RLC with the bit order INCREASING as the
       array index increases.
       This is not how it should be sent OTA and requires re-ordering so that
       the highest number bit of the lowest number octet contains the highest
       order bit of the field. (44.060 sect 11, fig 11.1). */

    /* calculate the number of elements in the array*/
    array_cnt = (uint8)(bitmap_length / 32);

    /* calculate the number of remaining bits not part of a complete 32-bit word*/
    leftover_bits = bitmap_length % 32;

    /*pack the leftover bits first*/
    if (leftover_bits)
    {
      gpackd(msg_ptr->urbb[array_cnt], src, pos_ptr, leftover_bits);
    }

    bitmap_length = bitmap_length - leftover_bits;
    array_cnt = array_cnt - 1;

    /* pack the remaining whole words*/
    while (bitmap_length >= 32)
    {
      gpackd(msg_ptr->urbb[array_cnt], src, pos_ptr, 32);
      bitmap_length -= 32;
      array_cnt -= 1;
    };
  }


  if(msg_ptr->len_present)
  {
    /* ACK/NACk des does not fill the 22 octets of E-PDAN*/
    /* Insert padding bits                               */
    csn_enc_padding_bits(src, pos_ptr);
  }

  return PASS;
}

/*===========================================================================
===
===  FUNCTION    csn_egprs_pkt_add_rac_info
===
===  DESCRIPTION
===
===  Encodes csn_enc_egprs_ack_nack_des_ie
===========================================================================*/
static void csn_egprs_pkt_add_rac_info
(
  uint8 src[],
  uint16 *pos_ptr,
  const egprs_pkt_add_rac_info_t *msg_ptr
)
{
  uint16  local_len;
  uint8   loop_count;
  uint16  remaining_bits;
  uint8   remaining_data;

  //  Next field is conditional
  if (msg_ptr->tlli_valid_flg == TRUE)
  {
    //tlli present

    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert the tlli field.
    gpackd(msg_ptr->tlli, src, pos_ptr, CSN_TLLI_LEN);
  }
  else
  {
    //  Global TFI present

    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);

    //  Insert global ie fields
    csn_enc_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));
  }

  // Insert MS Radio Access Capability IE Value Part. this field is mandatory
  local_len = msg_ptr->access_capability.ms_cap_len_bits;
#ifdef PERLUTF
  #error code not present
#endif
  //  Insert MS Radio Access Capability IE Value Part.
  for (loop_count=0;loop_count<(local_len/CSN_ACCESS_CAPABILITY_LEN);loop_count++)
  {
    //  Insert octets
    gpackb(msg_ptr->access_capability.ms_cap[loop_count], src,
                                      pos_ptr, CSN_ACCESS_CAPABILITY_LEN);
  }

  //  Determine if there is an incomplete octet at the end
  remaining_bits = local_len % CSN_ACCESS_CAPABILITY_LEN;
  if (remaining_bits)
  {
    //  There are some bits in the last octet
    //  Insert bits
    remaining_data =
      msg_ptr->access_capability.ms_cap[loop_count] >> (8 - remaining_bits);
    gpackb(remaining_data, src, pos_ptr, remaining_bits);
  }

  if ( *pos_ptr > MAX_ENCODE_BITS )
  {
    MSG_GERAN_ERROR_2("GMAC ARAC is greater than %d bits, size:%d", MAX_ENCODE_BITS, *pos_ptr);
  }

  //  Insert padding bits
  csn_enc_padding_bits(src, pos_ptr);

  /* Check that the encoded ARAC length is within the 23 octet message bounds. */
  if ( *pos_ptr > MAX_POS_PTR )
  {
    MSG_GERAN_FATAL_3("CSN: add RAC info Enc Len Err %d > %d",
              (*pos_ptr/8),
              (MAX_POS_PTR/8),
              0 );
  }

}

/*****************************************************************************
***
***     12.xx - Information Element Insert functions to follow
***
*****************************************************************************/


/*****************************************************************************
***
***     12.03 - Information Element - Ack/Nack description
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    csn_enc_ack_nack_ie
===
===  DESCRIPTION
===
===    This function inserts the information into the ACK/NACK description
===    IE.
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

static void csn_enc_ack_nack_ie (
  uint8 src[],
  uint16 *pos_ptr,
  const ack_nack_desc_t *msg_ptr)
{
  int8 i;

  for (i=2;i<16;i++)
    src[i]=0;
  //  Insert the final_ack_ind field.
  gpackb(msg_ptr->final_ack_ind, src, pos_ptr, CSN_FINAL_ACK_IND_LEN);

  //  Insert the strt_seq_num field.
  gpackb(msg_ptr->strt_seq_num, src, pos_ptr, CSN_STRT_SEQ_NUM_LEN);

  //  Insert the received_blk_btmp field.
  for (i=0; i<8; i++)
    gpackb(msg_ptr->received_blk_btmp[i], src,
                                        pos_ptr, CSN_RECEIVED_BLK_BTMP_LEN);

}


/*===========================================================================
===
===  FUNCTION    csn_enc_global_tfi_ie
===
===  DESCRIPTION
===
===    This function inserts the information into the Packet Resource Request
===    IE.
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

static void csn_enc_global_tfi_ie(
  uint8 src[],
  uint16 *pos_ptr,
  const global_tfi_t *msg_ptr)
{

  //  Next field is conditional
  if (!msg_ptr->direction)
  {
    //  Uplink TFI present
    //  Insert flag
    gpackb(0, src, pos_ptr, (int16)1);

    //  Insert Uplink TFI
    gpackb(msg_ptr->uplink_tfi, src, pos_ptr, CSN_UPLINK_TFI_LEN);
  }
  else
  {
    //  Insert flag
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert Downlink TFI
    gpackb(msg_ptr->dwnlink_tfi, src, pos_ptr, CSN_DWNLINK_TFI_LEN);
  }
}


/*****************************************************************************
***
***     12.07 - Information Element - Ack/Nack description
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    csn_enc_chan_req_desc_ie
===
===  DESCRIPTION
===
===    This function inserts the information into the Packet Resource Request
===    IE.
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

static void csn_enc_chan_req_desc_ie(
  uint8 src[],
  uint16 *pos_ptr,
  channel_req_desc_t *msg_ptr)
{
  //  Insert Peak throughput class
  gpackb(msg_ptr->peak_thr_class, src, pos_ptr, CSN_PEAK_THR_CLASS_LEN);

  //  Insert Radio priority
  //  Radio priority is sent over the air as two bits, but is sent down to
  //  csn encoder as 1 - 4, three bits, convert here.
  if (msg_ptr->radio_priority)
  {
    msg_ptr->radio_priority -= 1;
  }
  else
  {
    MSG_GERAN_ERROR_0("CSN1: RADIO PRIORITY INVALID - 0");

  }

  gpackb(msg_ptr->radio_priority, src, pos_ptr, CSN_RADIO_PRIORITY_LEN);

  //  Insert RLC_mode
  gpackb(msg_ptr->rlc_mode, src, pos_ptr, CSN_RLC_MODE_LEN);

  //  Insert LLC_PDU_TYPE
  gpackb(msg_ptr->llc_pdu_type, src, pos_ptr, CSN_LLC_PDU_TYPE_LEN);

  //  Insert RLC OCTET COUNT
  gpackw(msg_ptr->rlc_octet_cnt, src, pos_ptr, CSN_RLC_OCTET_CNT_LEN);

}




/*===========================================================================
===
===  FUNCTION    csn_enc_nc_meas_rprt_strct
===
===  DESCRIPTION
===
===    This function inserts the information into the Packet Measurement
===    Report.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    Max number of bits encoded by this function
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static uint16 csn_enc_nc_meas_rprt_strct(
  uint8 src[],
  uint16 *pos_ptr,
  const enc_nc_meas_rprt_strct_t *msg_ptr)
{
  uint16 max_pos = 0;
  uint8  loop_count;

  //  Insert the NC_MODE field.
  max_pos += l_gpackb(msg_ptr->nc_mode, src, pos_ptr, CSN_NC_MODE_LEN);

  //  Insert the RXLEV_SERVING_CELL field.
  max_pos += l_gpackb(msg_ptr->rxlev_srvg_cell, src, pos_ptr, CSN_RXLEV_SRVG_CELL_LEN);

  //  NOTE: intrfrnce_srvg_cell_flag was removed in Rel6, encode a zero bit here
  max_pos += l_gpackb(0, src, pos_ptr, (int16)1);

  //  Insert Number of NC measurements
  max_pos += l_gpackb(msg_ptr->num_nc_meas, src, pos_ptr, CSN_NUM_NC_MEAS_LEN);

  //  Number of nc measurements determines the number of fields following
  for(loop_count=0; loop_count<msg_ptr->num_nc_meas; loop_count++)
  {
    //  Insert frequency_n
    max_pos += l_gpackb(msg_ptr->num_meas[loop_count].freq_n, src,
                        pos_ptr, CSN_FREQ_N_LEN);

    //  Next field is optional
    if (msg_ptr->num_meas[loop_count].bsic_n_flag)
    {
      //  Insert flag
      max_pos += l_gpackb(1, src, pos_ptr, (int16)1);

      //  insert BSIC N
      max_pos += l_gpackb(msg_ptr->num_meas[loop_count].bsic_n, src,
                          pos_ptr, CSN_BSIC_N_LEN);
    }
    else
    {
      //  Insert flag
      max_pos += l_gpackb(0, src, pos_ptr, (int16)1);
    }

    //  Insert RXLEV N
    max_pos += l_gpackb(msg_ptr->num_meas[loop_count].rxlev_n, src,
                        pos_ptr, CSN_RXLEV_N_LEN);
  }

  return(max_pos);
}


/*===========================================================================
===
===  FUNCTION    csn_enc_rx_si_msg_struct
===
===  DESCRIPTION
===
===    This function inserts the information into the Packet SI Status
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

static void csn_enc_rx_si_msg_struct(
  uint8 src[], uint16 *pos_ptr,
  rx_si_msg_struct_t *msg_ptr)
{
  si_msg_list_t *si_struct_ptr;
  uint8          loop_count;

  //  Set pointer to first psi structure
  si_struct_ptr = &(msg_ptr->si_msg_list[0]);

  //  num_msg contains the number of messages in the list
  for(loop_count = 0; loop_count < msg_ptr->num_msg; loop_count++)
  {
    //  Start each message with a 1 bit to indicate message present
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert Message type
    gpackb(si_struct_ptr->msg_type, src, pos_ptr, CSN_SIX_MSG_TYPE_LEN);

    // Insert Message Received
    gpackb(si_struct_ptr->mess_rec, src, pos_ptr, CSN_MESS_REC_LEN);

    if (si_struct_ptr->mess_rec == 0x02)      // multiple instance, partially rcvd
    {
      //  Insert SI_x change mark
      gpackb(si_struct_ptr->six_chng_mrk, src, pos_ptr, CSN_SIX_CHNG_MRK_LEN);

      //  insert SI_x count
      gpackb(si_struct_ptr->six_count, src, pos_ptr, CSN_SIX_COUNT_LEN);

      //  insert instance bitmap
      gpackw(si_struct_ptr->instance_bitmap,
             src,
             pos_ptr,
             (int16)((si_struct_ptr->six_count) +1));
    }
    else if (si_struct_ptr->mess_rec == 0x03) // multiple instance, completely rcvd
    {
      //  Insert SI_x change mark
      gpackb(si_struct_ptr->six_chng_mrk, src, pos_ptr, CSN_SIX_CHNG_MRK_LEN);
    }

    //  Increment structure pointer
    si_struct_ptr++;
  }

  //  Insert flag, indicating si message list complete
  gpackb(0, src, pos_ptr, (int16)1);

  //  Insert Additional message type
  gpackb(msg_ptr->additional_msg_type, src, pos_ptr, CSN_ADDITIONAL_MSG_TYPE_LEN);

}



/*===========================================================================
===
===  FUNCTION    csn_enc_rx_unknw_si_msg_struct
===
===  DESCRIPTION
===
===    This function inserts the information into the Packet SI Status
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

static void csn_enc_rx_unknw_si_msg_struct(
  uint8 src[],
  uint16 *pos_ptr,
  const rx_unknw_si_msg_struct_t *msg_ptr)
{
  uint8 loop_count;

  //  num_msg contains the number of messages in the list

  for (loop_count = 0;loop_count < msg_ptr->num_msg; loop_count++)
  {
    //  Start each message with a 1 bit to indicate message present
    gpackb(1, src, pos_ptr, (int16)1);

    //  Insert Message type
    gpackb(msg_ptr->msg_type[loop_count], src, pos_ptr, CSN_SIX_MSG_TYPE_LEN);
  }

  //  Insert flag, indicating unknown si message list complete
  gpackb(0, src, pos_ptr, (int16)1);

  //  Insert Additional message type
  gpackb(msg_ptr->additional_msg_type, src, pos_ptr, CSN_ADDITIONAL_MSG_TYPE_LEN);

}

/*****************************************************************************
***
***     CSN Encoder Sub functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    csn_enc_padding_bits
===
===  DESCRIPTION
===
===    This function will fill to the end of a message with padding bits.
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

static void csn_enc_padding_bits(uint8 src[], uint16 *pos_ptr)
{
  if (*pos_ptr < MAX_ENCODE_BITS)
  {
    // some bits remaining, set next bit to zero
    gpackb(0, src, pos_ptr, (uint16)1);
  }

  return;
}

/*
RR CSN1 Encoder Functions
*/

/*
Function: encode_a5_bits
Purpose:  Encodes the A5 bits element of Mobile Station Classmark 3
*/
static void encode_a5_bits(uint16 *msg_pos, const a5_bits_t *src, uint8 *dst)
{
 gpackb(src->a5_7, dst, msg_pos, CSN_A5_7_LEN);
 gpackb(src->a5_6, dst, msg_pos, CSN_A5_6_LEN);
 gpackb(src->a5_5, dst, msg_pos, CSN_A5_5_LEN);
 gpackb(src->a5_4, dst, msg_pos, CSN_A5_4_LEN);
}

/*
Function: encode_ms_measurement_cap
Purpose:  Encodes the MS Measurement Capability element of Mobile Station Classmark 3
*/
static void encode_ms_measurement_cap
(
  uint16 *msg_pos,
  const ms_measurement_cap_t *src,
  uint8 *dst
)
{
 gpackb(src->sms_value, dst, msg_pos, CSN_SMS_VALUE_LEN);
 gpackb(src->sm_value, dst, msg_pos, CSN_SM_VALUE_LEN);
}

/*
Function: encode_edge_struct
Purpose:  Encodes the EDGE Struct element of Mobile Station Classmark 3
*/
static void encode_edge_struct
(
  uint16 *msg_pos,
  const edge_struct_t *src,
  uint8 *dst
)
{
 gpackb(src->modulation_cap, dst, msg_pos, CSN_MODULATION_CAP_LEN);
 if (src->edge_rf_power_cap_1_flag == TRUE)
 {
  gpackb((uint8) 1, dst, msg_pos, (uint16) 1);
  gpackb(src->edge_rf_power_cap_1, dst, msg_pos, CSN_EDGE_RF_POWER_CAP_1_LEN);
 }
 else
 {
  gpackb((uint8) 0, dst, msg_pos, (uint16) 1);
 }
 if (src->edge_rf_power_cap_2_flag == TRUE)
 {
  gpackb((uint8) 1, dst, msg_pos, (uint16) 1);
  gpackb(src->edge_rf_power_cap_2, dst, msg_pos, CSN_EDGE_RF_POWER_CAP_2_LEN);
 }
 else
 {
  gpackb((uint8) 0, dst, msg_pos, (uint16) 1);
 }
}

/*
Function: csn_encode_mobile_station_classmark_3
Purpose:  Encodes a mobile_station_classmark_3 struct (src) as a CSN.1 bitstream (dst)
*/
static uint8 csn_encode_mobile_station_classmark_3(
  const mobile_station_classmark_3_t *src,
  uint8 *dst)
{
  uint8 num_of_octets;
  uint16 msg_pos = 0;

  gpackb((uint8) 0, dst, &msg_pos, (uint16) 1);
  gpackb(src->multiband_supported, dst, &msg_pos, CSN_MULTIBAND_SUPPORTED_LEN);
  switch (src->multiband_supported)
  {
    case 0:
      encode_a5_bits(&msg_pos, &(src->a5_bits), dst);
      break;
    case (CLASS_DCS_1800 | CLASS_P_GSM):
    case (CLASS_DCS_1800 | CLASS_E_GSM_OR_R_GSM):
      encode_a5_bits(&msg_pos, &(src->a5_bits), dst);
      gpackb(src->associated_radio_cap_2, dst, &msg_pos, CSN_ASSOCIATED_RADIO_CAP_2_LEN);
      gpackb(src->associated_radio_cap_1, dst, &msg_pos, CSN_ASSOCIATED_RADIO_CAP_1_LEN);
      break;
    case CLASS_P_GSM:
    case CLASS_E_GSM_OR_R_GSM:
    case CLASS_DCS_1800:
      encode_a5_bits(&msg_pos, &(src->a5_bits), dst);
      gpackb((uint8) 0, dst, &msg_pos, (uint16) 4);
      gpackb(src->associated_radio_cap_1, dst, &msg_pos, CSN_ASSOCIATED_RADIO_CAP_1_LEN);
      break;
    default:
      return 0;
  }
  gpackb(src->r_support_flag, dst, &msg_pos, (uint16) 1);
  if (src->r_support_flag)
  {
    gpackb(src->r_support, dst, &msg_pos, CSN_R_SUPPORT_LEN);
  }
  gpackb(src->multi_slot_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->multi_slot_cap_flag)
  {
    gpackb(src->multi_slot_cap, dst, &msg_pos, CSN_MULTI_SLOT_CAP_LEN);
  }
  gpackb(src->ucs2_treatment, dst, &msg_pos, CSN_UCS2_TREATMENT_LEN);
  gpackb(src->extended_measurement_cap, dst, &msg_pos, CSN_EXTENDED_MEASUREMENT_CAP_LEN);
  gpackb(src->ms_measurement_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->ms_measurement_cap_flag)
  {
    encode_ms_measurement_cap(&msg_pos, &(src->ms_measurement_cap), dst);
  }
  gpackb(src->ms_positioning_method_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->ms_positioning_method_cap_flag)
  {
    gpackb(src->ms_positioning_method, dst, &msg_pos, CSN_MS_POSITIONING_METHOD_LEN);
  }
  gpackb(src->edge_multi_slot_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->edge_multi_slot_cap_flag)
  {
    gpackb(src->edge_multi_slot_class, dst, &msg_pos, CSN_EDGE_MULTI_SLOT_CLASS_LEN);
  }
  gpackb(src->edge_struct_flag, dst, &msg_pos, (uint16) 1);
  if (src->edge_struct_flag)
  {
    encode_edge_struct(&msg_pos, &(src->edge_struct), dst);
  }
  gpackb(src->gsm400_bands_supported_flag, dst, &msg_pos, (uint16) 1);
  if (src->gsm400_bands_supported_flag)
  {
    gpackb(src->gsm400_bands_supported,
           dst,
           &msg_pos,
           CSN_GSM400_BANDS_SUPPORTED_LEN);
    gpackb(src->gsm400_associated_radio_cap,
           dst,
           &msg_pos,
           CSN_GSM400_ASSOCIATED_RADIO_CAP_LEN);
  }
  gpackb(src->gsm850_associated_radio_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->gsm850_associated_radio_cap_flag)
  {
    gpackb(src->gsm850_associated_radio_cap,
           dst,
           &msg_pos,
           CSN_GSM850_ASSOCIATED_RADIO_CAP_LEN);
  }
  gpackb(src->pcs1900_associated_radio_cap_flag, dst, &msg_pos, (uint16) 1);
  if (src->pcs1900_associated_radio_cap_flag)
  {
    gpackb(src->pcs1900_associated_radio_cap,
            dst,
            &msg_pos,
            CSN_PCS1900_ASSOCIATED_RADIO_CAP_LEN);
  }
  gpackb(src->umts_fdd_radio_access_tech_cap,
          dst,
          &msg_pos,
          CSN_UMTS_FDD_RADIO_ACCESS_TECH_CAP_LEN);
  gpackb(src->umts_tdd_radio_access_tech_cap,
          dst,
          &msg_pos,
          CSN_UMTS_TDD_RADIO_ACCESS_TECH_CAP_LEN);
  gpackb(src->cdma2000_radio_access_tech_cap,
          dst,
          &msg_pos,
          CSN_CDMA2000_RADIO_ACCESS_TECH_CAP_LEN);
  gpackb(src->dtm_gprs_multi_slot_sub_class_flag, dst, &msg_pos, (uint16) 1);
  if (src->dtm_gprs_multi_slot_sub_class_flag)
  {
    gpackb(src->dtm_gprs_multi_slot_sub_class,
            dst,
            &msg_pos,
            CSN_DTM_GPRS_MULTI_SLOT_SUB_CLASS_LEN);
    gpackb(src->mac_mode_support, dst, &msg_pos, CSN_MAC_MODE_SUPPORT_LEN);
    gpackb(src->dtm_egprs_multi_slot_sub_class_flag, dst, &msg_pos, (uint16) 1);
    if (src->dtm_egprs_multi_slot_sub_class_flag == 1)
    {
      gpackb(src->dtm_egprs_multi_slot_sub_class,
              dst,
              &msg_pos,
              CSN_DTM_GPRS_MULTI_SLOT_SUB_CLASS_LEN);
    }
  }
  gpackb(src->single_band_support_flag, dst, &msg_pos, (uint16) 1);
  if (src->single_band_support_flag)
  {
    gpackb(src->gsm_band, dst, &msg_pos, CSN_GSM_BAND_LEN);
  }
  while (msg_pos < (MAX_MOBILE_STATION_CLASSMARK_3_LEN * 8))
  {
    gpackb((uint8) 0, dst, &msg_pos, (uint16) 1);
  }

  /* Convert bit-count into octet-count */
  num_of_octets = (uint8)(msg_pos / 8);

  /* If the num of bits used does not divide into whole octets,
     add one to the octet count */
  if ( (msg_pos & 7) != 0 )
  {
    num_of_octets++;
  }

  return num_of_octets;
}

#ifdef FEATURE_GSM_DTM
/*
Function: csn_encode_channel_req_desc_2
Purpose:  Encodes a Channel Request Description 2 struct (src) as a CSN.1 bitstream (dst)
Return:  length, in octets (rounded-up)
*/
static uint8 csn_encode_channel_req_desc_2(csn_channel_req_desc_2_t *src, uint8 *dst)
{
  uint8 num_of_octets;
  uint16 msg_pos = 0;

  gpackb(src->pkt_est_cause, dst, &msg_pos, CSN_PKT_EST_CAUSE_LEN);

  csn_enc_chan_req_desc_ie( dst, &msg_pos, &src->channel_req_desc );

  if (src->pfi_present)
  {
    gpackb((uint8) 1, dst, &msg_pos, (uint16) 1);
    gpackb(src->pfi, dst, &msg_pos, CSN_PFI_LEN);
  }

  gpackb(src->mult_tbf_cap, dst, &msg_pos, CSN_MULT_TBF_CAP_LEN);

  /* Convert bit-count into octet-count */
  num_of_octets = (uint8)(msg_pos / 8);

  /* If the num of bits used does not divide into whole octets,
     add one to the octet count */
  if ( (msg_pos & 7) != 0 )
  {
    num_of_octets++;
  }

  return num_of_octets;
}

#endif /* FEATURE_GSM_DTM */

/*===========================================================================
===
===  FUNCTION      gcsn1_rr_encoder
===
===  DESCRIPTION
===
===    Translate internal decompressed format into external compressed format.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    number of octets
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gcsn1_rr_encoder(
  csn_rr_encoder_msg_id_t message_type,
  void *src, uint8 *dst
)
{
  switch (message_type)
  {
    case MOBILE_STATION_CLASSMARK_3:
      return csn_encode_mobile_station_classmark_3(
                 (mobile_station_classmark_3_t *) src,
                 dst);
    #ifdef FEATURE_GSM_DTM
    case CHANNEL_REQ_DESC_2:
      return csn_encode_channel_req_desc_2((csn_channel_req_desc_2_t *) src, dst);
    #endif /* FEATURE_GSM_DTM */
    default:
      return 0;
  }
}


/*===========================================================================
===
===  FUNCTION      gcsn1_encoder
===
===  DESCRIPTION
===
===    Translate internal decompressed format into external compressed format.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    PASS/FAIL
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/

uint8 gcsn1_encoder(
  uint8   message_type,
  uint8   *src,   /* destination array (over-the-air) format */
  int_data_t *int_data      /* source data, internal format */
  )
{
  uint8   return_status = PASS;
  uint16  msg_pos = 0;

  /* initialise the buffer to "spare padding" */
  if (message_type != PACKET_CHANNEL_REQUEST)
  {
    memset(src, 0x2B, UPLINK_MESSAGE_SIZE);
  }

  //  Insert the message type into the bit stream prior to checking
  //  If the message type is invalid then the error code will be set to
  //  invalid later.
  gpackb(message_type, src, &msg_pos, CSN_MESSAGE_TYPE_LEN);

  // Using message type switch to correct extract function.
  switch(message_type)
  {
    //  "000000"  Packet Cell Change Failure
    case PACKET_CELL_CHANGE_FAILURE:

      csn_pkt_cell_chg_failure(src, &msg_pos,
                                        &(int_data->type.pkt_cell_chg_failure));

    break;

    case PACKET_CONTROL_ACKNOWLEDGE:

      csn_pkt_control_ack(src, &msg_pos, &(int_data->type.pkt_ctrl_ack));

    break;

    case PACKET_DOWNLINK_ACK_NACK:

      csn_pkt_dwnlk_ack_nack(src, &msg_pos,
                                         &(int_data->type.pkt_dwnlk_ack_nack));

    break;

    case PACKET_UPLK_DUMMY_CNTRL_BLK:

      csn_pkt_uplk_dummy_ctrl(src, &msg_pos,
                                     &(int_data->type.pkt_uplk_dummy_cntrl_blk));

    break;

    case PACKET_MEASUREMENT_RPT_PREPROCESS:

      /* used by GRR to determine whether a PMR can fit in an uplink control message */
      return_status = csn_pkt_meas_rprt(src, &msg_pos, &(int_data->type.pkt_meas_report));

    break;

    case PACKET_MEASUREMENT_REPORT:

      (void)csn_pkt_meas_rprt(src, &msg_pos, &(int_data->type.pkt_meas_report));

    break;

    case PACKET_RESOURCE_REQUEST:

      csn_pkt_resource_req(src, &msg_pos,
                                             &(int_data->type.pkt_resource_req));

    break;

    case PACKET_MOBILE_TBF_STATUS:

      csn_pkt_mobile_tbf_status(src, &msg_pos,
                                        &(int_data->type.pkt_mobile_tbf_status));

    break;

    case PACKET_SI_STATUS:

      csn_pkt_si_status(src, &msg_pos, &(int_data->type.pkt_si_status));

    break;

    case EGPRS_PACKET_DOWNLINK_ACK_NACK:
         return_status = csn_egprs_pkt_dwnlk_ack_nack(
                             src,
                             &msg_pos,
                             &(int_data->type.egprs_pkt_dwnlk_ack_nack));
    break;

    case ADDITIONAL_MS_RAC:
      csn_egprs_pkt_add_rac_info(
          src,
          &msg_pos,
          &(int_data->type.egprs_pkt_add_rac_info));
    break;

    case PACKET_PAUSE:
      MSG_GERAN_ERROR_1("UL cntrl msg not supported =x %x",message_type);
    break;

    case PACKET_ENHANCED_MEASUREMENT_REPORT:
      csn_encode_packet_enhanced_measurement_report(
          src,
          &msg_pos,
          &(int_data->type.pkt_enhanced_measurement_report));
    break;

    case PACKET_CELL_CHANGE_NOTIFICATION:
      csn_encode_pkt_cell_change_notification(
          src,
          &msg_pos,
          &(int_data->type.pkt_cell_change_notification));
    break;

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    case PACKET_CS_REQUEST:
      csn_encode_pkt_cs_request(src, &msg_pos, &(int_data->type.pkt_cs_request));
      break;
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

    //  Unknown message type.
    default:
      //  Return error code
      return_status = FAIL;
    break;
  }

  return(return_status);
}

