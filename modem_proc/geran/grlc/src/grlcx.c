/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 RLC DATA TRANSLATION SERVICES
***
***
*** DESCRIPTION
***
***  Translation services for RLC/MAC over-the-air external data format and
***  internal data representation.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  xlate_ext_mac_hdr()
***  xlate_int_mac_hdr()
***  xlate_ext_dl_data()
***  xlate_int_ul_data()
***
***  xlate_ext_ul_data()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None required
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcx.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/16/01    hv     RLC Translation functionality.
*** 09/18/01    hv     Added test code.
*** 04/22/04    hv     Added EGPRS utils
*** 04/11/04    hv     Added TI-E Shifting function
*** 28/09/04   zyu     Added rlc ul/dl header logging APIs, include the file
***                    defines the APIs .
*** 05/04/05    hv     Lint'ed. Tidied up
*** 19/10/12    hv     CR407619 - fixed potential security issues
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
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
#include "grlcx.h"
#include "grlcultypes.h"
#include "grlculproc.h"
#include "grlcdli.h"
#include "grlclog.h"
#include "grlcl1.h"
#include "grlci.h"
#include "grlcutil.h"
#include "grlcdl.h"
#include "grlcrx.h"


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      shift_in_ti_e_fields()
===
===  DESCRIPTION
===
===    This function shifts in the TI/E fields into an array of uint8 containing
===    the EGPRS RLC data unit.
===
===    If the RLC data unit contains 37 octets the TI/E fields shall occupy bits
===    1/0 of the first octet, and bits 7/6 of the 37th octet shall occupy bits
===    1/0 of the 38th octet.
===
===               b7  b6  b5  b4  b3  b2  b1  b0
===                7   6   5   4   3   2   1   0  1st data octet
===                ......................
===                X   Y   5   4   3   2   1   0  37th data octet
===
===              becomes
===
===               b7  b6  b5  b4  b3  b2  b1  b0
===                5   4   3   2   1   0  TI   E   1st data octet
===                ......................  7   6   2nd data octet
===                .............................
===                <        all 0's     >  X   Y   38th data octet
===
===
===    Two versions are provided: The THIRTY_TWO_BIT_VERSION should be roughly 4 times
===    faster if a barrel shifter is used
===
===  PARAMETERS
===
===    Ptr: points to the first data octet of the EGPRS UL RLC data unit which is
===         octet-aligned
===    Len: is the length of the RLC data unit without taking inbto account the TI/E
===         fields, as yet, ie. For MCS-1 this value should be 22
===    ti_field:  value of the TI field. Expects 0 or non-zero value (set)
===    e_field:   value of the E field. Expects 0 or non-zero value (set)
===
===  DEPENDENCIES
===
===    There must be room for the extra octet in len+1 th octet.
===    The 32-bit version is used if THIRTY_TWO_BIT_VERSION is defined otherwise
===    the 8-bit version shall be used. The 32-bit version assumes little-endian
===     memory model
===
===  RETURN VALUE
===
===    Perform shifting function as above where all len octets as well as the len+1
===    octets are altered
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
static void shift_in_ti_e_fields
(
  uint8   *ptr,
  uint8   len,
  uint8   ti_field,
  uint8   e_field
)
{
/* Not using the 32 bit version due to the ptr passed to RLC from PL1 is not 32-aligned
*/
#undef  THIRTY_TWO_BIT_VERSION

  register uint16  i;                   /* uint16 are accessed faster than uint8 */

#ifdef  THIRTY_TWO_BIT_VERSION

  register uint32 *p = (uint32 *)ptr;   /* cast to uint32 pointer in REGISTER */

  /* Clear the len+1 octet first to receive the 2 bits dropping out.
  ** Use uint8 pointer here
  */
  ptr[len] = 0;

  /*------------------------------------------------------------------------
  ** Loop to shift the data octets down by 2 bits. This is to make room for
  ** the TI/E fields at bit 1/0 of the very first word
  **
  ** The loop starts from the last word. 32 bit version only requires
  ** one-quarter the number of shift operations
  **------------------------------------------------------------------------
  */
  for ( i = (uint16)len >> 2;         /* 32-bit requires 1/4 number of shifts */
        i >= 1;                       /* exit when current word is the 2nd */
        i--                           /* .. and the previous word is the 1st */
      )
  {
    /* Shift the current word left by 2 bits to make room.
    ** Move bits 31/30 of the previous word into bits 1/0 of the current word
    */
    p[i] = (uint32)((uint32)((uint32)(p[i]) << 2)) |
           (uint32)((((uint32)(p[i-1]) >> 30) & 0x03));
  }

#else /* EIGHT_BIT_VERSION */

  register uint8 *p = ptr;            /* copy to uint8 pointer in REGISTER */

  /* Clear the len+1 octet first to receive the 2 bits dropping out.
  ** Use uint8 pointer here
  */
  ptr[len] = 0;

  /*------------------------------------------------------------------------
  ** Loop to shift the data octets down by 2 bits. This is to make room for
  ** the TI/E fields at bit 1/0 of the very first data octet
  **
  ** The loop starts from the bottom at the len+1 th octet
  **------------------------------------------------------------------------
  */
  for ( i = (uint16)len;
        i >= 1;                       /* exit when current octet is the 2nd */
        i--                           /* .. and the previous octet is the 1st */
      )
  {
    /* Shift the current octet left by 2 bits to make room.
    ** Or bits 7/6 of the previous octet into bits 1/0 of the current octet
    */
    p[i] = (uint8)(p[i] << 2) | (uint8)((p[i-1] >> 6) & 0x03);
  }

#endif  /* THIRTY_TWO_BIT_VERSION */

  /* Now shift the very first word/octet
  */
  p[0] <<= 2;

  /* Copy TI/E fields into bits 1/0 of the very first octet.
  ** Use uint8 pointer here
  */
  ptr[0] |= ((ti_field ? 0x02 : 0) | (e_field ? 0x01 : 0));

} /* shift_in_ti_e_fields */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      XLATE_EXT_MAC_HDR
===
===  DESCRIPTION
===
===    Translate mac header from external format into internal format.
===    The b_uplink param allows an uplink datat block in external format
===    (delivered to L1 by RLC UL) to be tranlated back into internal format
===    to enable L1 emulation function to examine results generated by RLC UL.
===
===  DEPENDENCIES
===
===    none
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

void xlate_ext_mac_hdr
(
  uint8     *src,     /* source message in external (over-the-air) format */
  mac_hdr_t *mac_hdr  /* destination internal format */
#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
  #error code not present
#endif
)
{
  uint16 msg_pos; /* offset from the beginning of the message to the current item */

  /*---------------------------------------------------------------------*/

  msg_pos = 0;

  /* Translate Payload Type
  */
  mac_hdr->payload = gunpackb(src, &msg_pos, GSIZE(mac_hdr_t, payload));

  /* The payload determines which of the 4 unions the remaining fields belong to
  */
  switch(mac_hdr->payload)
  {
    case MAC_PAYLOAD_DATA:
#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
      #error code not present
#else
      /* Since the translation is from ext, the direction must be downlink
      */
      mac_hdr->type.dl_data.rrbp = gunpackb(src,
                                            &msg_pos,
                                            GSIZE(mac_hdr_dl_data_t, rrbp));
      mac_hdr->type.dl_data.sp = gunpackb(src,
                                          &msg_pos,
                                          GSIZE(mac_hdr_dl_data_t, sp));
      mac_hdr->type.dl_data.usf = gunpackb( src,
                                            &msg_pos,
                                            GSIZE(mac_hdr_dl_data_t, usf));
#endif
      break;

    case MAC_PAYLOAD_CTRL_NO_OPT_OCTETS:
    case MAC_PAYLOAD_CTRL_1_OPT_OCTET:
      mac_hdr->type.dl_ctrl.rrbp = gunpackb(src,
                                            &msg_pos,
                                            GSIZE(mac_hdr_dl_ctrl_t, rrbp));
      mac_hdr->type.dl_ctrl.sp = gunpackb(src,
                                          &msg_pos,
                                          GSIZE(mac_hdr_dl_ctrl_t, sp));
      mac_hdr->type.dl_ctrl.usf = gunpackb( src,
                                            &msg_pos,
                                            GSIZE(mac_hdr_dl_ctrl_t, usf));
      break;

    case MAC_PAYLOAD_RESERVED:
      MSG_GRLC_LOW_0(GRLC_F3_MSG_ENUM_811, "Payload is RLC_PAYLOAD_RESERVED");
      break;

    default:
      MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_812, "xlate_ext_mac_hdr: bad payload type");
      break;
  }
} /* xlate_ext_mac_hdr */

/*===========================================================================
===
===  FUNCTION      XLATE_INT_MAC_HDR
===
===  DESCRIPTION
===
===    Translate mac header from internal format into external format
===
===  DEPENDENCIES
===
===    none
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
static void xlate_int_mac_hdr
(
  const mac_hdr_t *mac_hdr,
  uint8           *dst
)
{
  uint16 msg_pos; /* offset from the beginning of the message to the current item */

  /* ---------------------------------------------------------------------*/

  msg_pos = 0;

  /* format payload
  */
  gpackb(mac_hdr->payload, dst, &msg_pos, GSIZE(mac_hdr_t, payload));

  /* The payload determines which of the 4 unions the remaining fields belong to
  */
  switch(mac_hdr->payload)
  {
    /* Since the translation is from internal to external, the direction
    ** must be uplink
    */
    case MAC_PAYLOAD_DATA:
      gpackb(mac_hdr->type.ul_data.cv,
              dst,
              &msg_pos,
              GSIZE(mac_hdr_ul_data_t, cv));
      gpackb((uint8)mac_hdr->type.ul_data.si,
              dst,
              &msg_pos,
              GSIZE(mac_hdr_ul_data_t, si));
      gpackb((uint8)mac_hdr->type.ul_data.retry,
              dst,
              &msg_pos,
              GSIZE(mac_hdr_ul_data_t, retry));
      break;

    case MAC_PAYLOAD_CTRL_NO_OPT_OCTETS:
    case MAC_PAYLOAD_CTRL_1_OPT_OCTET:
      gpackb(mac_hdr->type.ul_ctrl.spare,
              dst,
              &msg_pos,
              GSIZE(mac_hdr_ul_ctrl_t, spare));
      gpackb((uint8)mac_hdr->type.ul_ctrl.retry,
              dst,
              &msg_pos,
              GSIZE(mac_hdr_ul_ctrl_t, retry));
      break;

    case MAC_PAYLOAD_RESERVED:
      MSG_GRLC_LOW_0(GRLC_F3_MSG_ENUM_813, "xlate_int_mac_hdr: RLC_PAYLOAD_RESERVED");
      break;

    default:
      MSG_GRLC_LOW_0(GRLC_F3_MSG_ENUM_814, "xlate_int_mac_hdr: bad payload type");
      break;
  }
} /* xlate_int_mac_hdr */

/*===========================================================================
===
===  FUNCTION      XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink RLC data block from External format into internal
===    format
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to DSM item representing a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data block (MAC header, RLC header and payload)
===
===    dl_blk_ptr:
===       Internal structure representing rlc dl data block
===
===  DEPENDENCIES
===
===    none
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
void xlate_ext_dl_data
(
  gas_id_t                       gas_id,
  const pl1_radio_block_header_t *pl1_hdr_ptr,
  dsm_item_type                  *dsm_ptr,
  grlc_dl_data_block_t           *dl_blk_ptr
)
{
  uint8 by = 0;      /* loop count uint8 */

  uint16 msg_pos;    /* offset in bits from the beginning of the message to
                     ** the current item
                     */
  uint8 buf[5];      /* 5 bytes buffer for the RLC header (only need 2)
                     */
  uint8 temp_ext_octet[2];
                     /* Temporary store for holding
                     ** an extension octet.
                     */
  uint8 log_dl_pld[2*LOG_MAX_LI_E] = {0};
                     /* Log (2*LOG_MAX_LI_E) = 6
                     ** extension octets
                     */

  /*--------------------------------------------------------*/

  /* translation in block header required as CS, time-slot and frame number
  ** are not packed
  */

  dl_blk_ptr->cs          = pl1_hdr_ptr->cs;

  dl[gas_id].time_slot            = pl1_hdr_ptr->time_slot;

  dl[gas_id].frame_number         = pl1_hdr_ptr->frame_number;

  /* no error
  **
  ** Get radio block MAC header (1 octet) into buf[0] and RLC fixed header
  ** (2 octets) into buf[1] and buf[2]
  */
  if
  (
    gprs_pdu_strip_head
    (
      &dsm_ptr, (uint8 *)buf, (uint16)(RM_MAC_HDR_LEN + RM_RLC_HDR_LEN)
    )
  )
  {
    /* bit position is now relative to &buf[0]
    */
    msg_pos = 0;

    /* MSG_GERAN_LOW_3("xlate MacRlcHdrs[0]=0x%02x [1]=0x%02x [2]=0x%02x",buf[0],buf[1],buf[2]);
    */

    /* translate MAC header into various fields. Note that this fn does
    ** not update the msg position pointer
    */
    #if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
    #error code not present
#else
    xlate_ext_mac_hdr(buf, &cur_mac_hdr[gas_id]);
    #endif /* DEBUG_GSM_GPRS_RLC_TEST/PERLUTF */

    /* add the size of MAC header manually
    */
    msg_pos += GSIZE(grlc_dl_data_block_t, mac_hdr);

    /* translate RLC header octet 1
    */

    /* since pr in mac_hdr is not used , no need to unpack , we need to
    ** increment msg_pos
    ** gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, pr));
    */
    msg_pos += grlc_dl_data_block_t_pr_len;

    dl[gas_id].tfi = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, tfi));
    dl_blk_ptr->fbi = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, fbi));

    /* get dl rlc header octet 2
    */
    dl_blk_ptr->bsn = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, bsn));
    dl_blk_ptr->ext = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, ext));

    /* Copy LOG_MAX_LI_E * 2 Bytes of payload regardless of actual number of
    ** extension octets
    */
    memscpy((void *)log_dl_pld, sizeof(log_dl_pld), (void *) dsm_ptr->data_ptr, (2*LOG_MAX_LI_E));

    /* get extension octet(s) by examining the E bit in RLC hdr octet 2
    */

    /* 'ext' determines if the next octet is an Extension Octet
    */
    if (dl_blk_ptr->ext == RM_EXT_OCTET_FOLLOWS)
    {
      /* set index to the first extension octet.
      ** 'by' also holds the number of extension octets at the end
      */
      by = 0;

      /* loop to parse all Extension Octets in array
      */
      do
      {
        /* get the 'next' extension octet into temp_ext_octet
        */
        if (gprs_pdu_strip_head(&dsm_ptr, temp_ext_octet, (uint16)1))
        {
          /* must reset bit position pointer every time gunpackb() is called
          */
          msg_pos = 0;
          /* MSG_GERAN_LOW_2("xlate ext-oct[%d]=0x%02x", by,buf[0]); */

          /* extract the LI, MORE and EXT of the extension octet
          */
          dl_blk_ptr->ext_octet[by].gprs.li =
            gunpackb(temp_ext_octet, &msg_pos, GSIZE(grlc_ext_octet_t, li));
          dl_blk_ptr->ext_octet[by].gprs.more =
            gunpackb(temp_ext_octet, &msg_pos, GSIZE(grlc_ext_octet_t, more));
          dl_blk_ptr->ext_octet[by].gprs.ext =
            gunpackb(temp_ext_octet, &msg_pos, GSIZE(grlc_ext_octet_t, ext));
        }
        else
        {
          MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_815, "xlate_ext_dl_data() Error stripping extension octet");
          break;
        }

        /* Next LI field. 'by' carries the number of extension octet when the
        ** loop terminates
        */
        ++by;

      } while ( /* by-1 indexes the current extension bit
                */
                dl_blk_ptr->ext_octet[by-1].gprs.ext == RM_EXT_OCTET_FOLLOWS &&
                by < MAX_NUM_EXTENSION_OCTETS
              );
    } /* end extension octet(s) processing */

    /* set the number of Extension Octets in the array
    */
    dl_blk_ptr->num_ext_octets = by;

    /* get payload.
    ** Simply copy the dsm pointer to the payload pointer. dsm_ptr
    ** originally holds a full PL1 radio block but now has been stripped
    ** off all fields except for the RLC DL payload.
    */
    dl_blk_ptr->dsm_payload_ptr = dsm_ptr;

    /* data length of this block is the length of the data block given
    ** the coding scheme, less the RLC header length and the number of
    ** extension octets
    */
    dl_blk_ptr->len = (uint8)((GRLC_DATA_BLOCK_LEN[dl_blk_ptr->cs] -
                               RM_RLC_HDR_LEN
                              ) -
                              by
                             );

    /* Log GPRS DL Header */
    buf[3] = 0x03 & dl_blk_ptr->cs;

    rlc_log_dl_header(gas_id, (byte)GPRS_HEADER_TYPE,
                      buf,
                      1,                        /* CRC status is defaulted to 1 */
                      dl[gas_id].time_slot,
                      dl[gas_id].frame_number,
                      log_dl_pld
                     );


  } /* end MAC header strip */
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_816, "xlate_e_d Failed to strip MAC/RLC headers");
  }

} /* xlate_ext_dl_data */


/*===========================================================================
===
===  FUNCTION      TEST_MODE_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink RLC data block from External format into internal
===    format  without extracting the LI/M/E octets
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to DSM item representing a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data block (MAC header, RLC header and payload)
===
===    dl_blk_ptr:
===       Internal structure representing rlc dl data block
===
===  DEPENDENCIES
===
===    none
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
void test_mode_xlate_ext_dl_data
(
  gas_id_t                        gas_id,
  const pl1_radio_block_header_t  *pl1_hdr_ptr,
  dsm_item_type                   *dsm_ptr,
  grlc_dl_data_block_t            *dl_blk_ptr
)
{
  uint8 by = 0;      /* loop count uint8 */

  uint16 msg_pos;     /* offset from the beginning of the message to
                    ** the current item
                    */

  uint8 buf[4];      /* 4 bytes buffer for the RLC header (only need 2) */

  /*--------------------------------------------------------*/

  /* translation in block header required as CS, time-slot and frame number
  ** are not packed.
  */
  dl_blk_ptr->cs   = pl1_hdr_ptr->cs;
  dl[gas_id].time_slot     = pl1_hdr_ptr->time_slot;
  dl[gas_id].frame_number  = pl1_hdr_ptr->frame_number;

  /* no error
  **
  ** Get radio block MAC header (1 octet) into buf[0] and RLC fixed header
  ** (2 octets) into buf[1] and buf[2]
  */
  if
  (
    gprs_pdu_strip_head
    (
      &dsm_ptr, (uint8 *)buf, (uint16)(RM_MAC_HDR_LEN + RM_RLC_HDR_LEN)
    )
  )
  {
    /* bit position is now relative to &buf[0]
    */
    msg_pos = 0;

    /* MSG_GERAN_LOW_3("xlate MacRlcHdrs[0]=0x%02x [1]=0x%02x [2]=0x%02x",buf[0],buf[1],buf[2]);
    */

    /* translate MAC header into various fields. Note that this fn does
    ** not update the msg position pointer
    */
    #if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
    #error code not present
#else
    xlate_ext_mac_hdr(buf, &cur_mac_hdr[gas_id]);
    #endif /* DEBUG_GSM_GPRS_RLC_TEST/PERLUTF */

    /* add the size of MAC header manually
    */
    msg_pos += GSIZE(grlc_dl_data_block_t, mac_hdr);

    /* translate RLC header octet 1
    */

    /* since pr in mac_hdr is not used , no need to unpack , we need to
    ** increment msg_pos.
    ** gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, pr));
    */
    msg_pos += grlc_dl_data_block_t_pr_len;

    dl[gas_id].tfi          = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, tfi));
    dl_blk_ptr->fbi = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, fbi));

    /* get dl rlc header octet 2
    */
    dl_blk_ptr->bsn = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, bsn));
    dl_blk_ptr->ext = gunpackb(buf, &msg_pos, GSIZE(grlc_dl_data_block_t, ext));


    /* get payload.
    ** Simply copy the dsm pointer to the payload pointer. dsm_ptr
    ** originally holds a full PL1 radio block but now has been stripped
    ** off all fields except for the RLC DL payload.
    */
    dl_blk_ptr->dsm_payload_ptr = dsm_ptr;

    /* data length of this block is the length of the data block given
    ** the coding scheme, less the RLC header length and the number of
    ** extension octets
    */
    dl_blk_ptr->len = (uint8)((GRLC_DATA_BLOCK_LEN[dl_blk_ptr->cs] -
                               RM_RLC_HDR_LEN
                              ) - by
                             );

  } /* end MAC header strip */
  else
  {
    MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_817, "xlate_e_d Failed to strip MAC/RLC headers");
  }

} /* test_mode_xlate_ext_dl_data */


/*===========================================================================
===
===  FUNCTION      E_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink EGPRS RLC data block from External format into internal
===    format
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data blocks (MAC header, RLC header and payload)
===
===    dl_blk1_ptr/dl_blk2_ptr:
===       Internal structure representing rlc dl data blocks
===
===  DEPENDENCIES
===
===    none
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
void e_xlate_ext_dl_data
(
  gas_id_t                gas_id,
  e_pl1_radio_block_t     *pl1_blk_ptr,
  grlc_dl_data_block_t    *dl_blk1_ptr,
  grlc_dl_data_block_t    *dl_blk2_ptr
)
{
  uint8   by      = 0;   /* loop count byte */
  uint16  msg_pos = 0;   /* offset from the beginning of the message to the current item
                          */
  uint8   buf[ E_GRLC_HDR_TYPE_MAX_LEN ]; /* holds the extension octets
                                          ** during translation
                                          */
  uint8   log_crc_status = 0;
  uint8   blck_len;
  boolean is_padding_removed;

#ifdef FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI
  /* used for validation of a block */
  uint16 total_len = 0;     /* total length of L1 */
  uint8 b = 0;              /* loop ctr */
  boolean b_nack = FALSE;   /* forcing the block nack */
#endif /* FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI */

  uint8   log_dl_pld[4*LOG_MAX_LI_E] = {0}; /* Init. Buffer to account for 2
                                            ** Payloads
                                            */

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
  uint8   pad_buf[E_GRLC_PADDING_BYTES];
  /* Initialize this variable correct to avoid un-neccessary padding */
  dl_blk1_ptr->b_extra_depadding = FALSE;
  dl_blk2_ptr->b_extra_depadding = FALSE;
#endif  /* #ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */

  /* Check if padding octets have been removed by PL1 from the DL block.
  ** We need this info to determine the actual payload length below.
  */
  is_padding_removed =
    e_grlc_dl_is_padding_removal_required(pl1_blk_ptr->hdr_arr, pl1_blk_ptr->mcs);

  /* Store RLC data block info included in the PL1 radio block.
  */
  dl_blk1_ptr->cs         = pl1_blk_ptr->mcs;
  dl_blk1_ptr->crc        = pl1_blk_ptr->crc_1;
  dl_blk1_ptr->failed_crc = (pl1_blk_ptr->crc_1 == 0) ? TRUE : FALSE;
  dl[gas_id].time_slot            = pl1_blk_ptr->time_slot;
  dl[gas_id].frame_number         = pl1_blk_ptr->frame_number;

  /* Read FBI bit from FBI&E bits. 2nd LSbit */
  dl_blk1_ptr->fbi       = (uint8) (pl1_blk_ptr->fbi_e_1 & 0x02) >> 1;

  /* If block failed CRC , set FBI to 0 to
  ** avoid accidental DL TBF release. NW will resend the DL block
  ** when it sees EPDAN with clear FAI.
  */

  /* Read E bit from FBI&E bits. LSbit */
  dl_blk1_ptr->ext       = (uint8) pl1_blk_ptr->fbi_e_1 & 0x01;

  if( !pl1_blk_ptr->crc_1  )
  {
    dl_blk1_ptr->fbi = 0;
    dl_blk1_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;
  }


  /* Extract fields that are common to all header types
  */
  msg_pos = 0;

  e_cur_hdr[gas_id].usf   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, usf));
  e_cur_hdr[gas_id].esp   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, esp));
  e_cur_hdr[gas_id].rrbp  = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, rrbp));
  e_cur_hdr[gas_id].tfi   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, tfi));
  e_cur_hdr[gas_id].pr    = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, pr));
  e_cur_hdr[gas_id].bsn1  = (uint16)eunpackw(pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, bsn1));
  dl_blk1_ptr->bsn       = e_cur_hdr[gas_id].bsn1;

  /* Extract fields hat are specific to each of the header types
  */
  switch (pl1_blk_ptr->mcs)
  {
    case RM_MCS_1:
    case RM_MCS_2:
    case RM_MCS_3:
    case RM_MCS_4:
    {
      log_crc_status  |= (pl1_blk_ptr->crc_1 & 0x01);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E Bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 2*LOG_MAX_LI_E = 6 Bytes of Payload */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld),
                (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
                (2*LOG_MAX_LI_E));
      }

      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_3,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );
      /* Read DL Header Type 3 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t3, cps));
      e_cur_hdr[gas_id].spb   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t3, spb));
      dl_blk1_ptr->spb = e_cur_hdr[gas_id].spb;

      if (e_cur_hdr[gas_id].spb == SPB_RETX_PART_2)
      {
        /* FBI and LI/E are meaningless in the 2nd split */
        dl_blk1_ptr->fbi = 0;
        dl_blk1_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;
      }

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
      /* crc pass and MCS3 and b_extra_depadding set (means previous block was RM_MCS_8 )
      */
     if( EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl_blk1_ptr->bsn, dl[gas_id].ws))
     {
        if(E_RX_ARR(gas_id, dl_blk1_ptr->bsn).b_extra_depadding &&
           dl_blk1_ptr->cs == RM_MCS_3                  &&
           e_cur_hdr[gas_id].spb == SPB_RETX_PART_1
          )
        {
         /* CRC has passed for the MCS_3 block
         */
         if(dl_blk1_ptr->failed_crc == FALSE)
         {
           is_padding_removed = TRUE;
           if(e_cur_hdr[gas_id].cps == 3 || e_cur_hdr[gas_id].cps == 4 || e_cur_hdr[gas_id].cps == 5)
           {
             /* Force cps value incorrect no padding to padding( 3->6 , 4->7 , 5->8)
             */
             e_cur_hdr[gas_id].cps += 3;
             /* Strip 6 bytes of pl1_blk_ptr->payload_1_ptr*/
             if (!gprs_pdu_strip_head(&(pl1_blk_ptr->payload_1_ptr),pad_buf,E_GRLC_PADDING_BYTES))
             {
               MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_818, "e_xlate_ext_dl_data Failed to strip payload");
             }

             MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_819, "ERLD DL_CORRECT_DEPADDING bsn=%u mcs=%u",
                      dl_blk1_ptr->bsn,
                      dl_blk1_ptr->cs);
           }
           E_RX_ARR(gas_id, dl_blk1_ptr->bsn).b_extra_depadding = FALSE;
           dl_blk1_ptr->b_extra_depadding = FALSE;
         }
         else
         {
           /*  if the re-transmitted mcs_3 block is having bad crc again,
            ** b_extra_depadding should not be reset.
            */
           dl_blk1_ptr->b_extra_depadding = TRUE;
           MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_820, "ERLD DL_CORRECT_DEPADDING MCS_3 CRC bsn=%u mcs=%u",
                        dl_blk1_ptr->bsn,
                        dl_blk1_ptr->cs);
         }
        }
        else
        {
          dl_blk1_ptr->b_extra_depadding = FALSE;
        }
     }
     else
     {
       /* bsn out of receive window
       */
       dl_blk1_ptr->b_extra_depadding = FALSE;
     }
#endif /* FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */
     break;
    }

    case RM_MCS_5:
    case RM_MCS_6:
    {
      log_crc_status  |= (pl1_blk_ptr->crc_1 & 0x01);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E Bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 2*LOG_MAX_LI_E = 6 Bytes of Payload */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld),
                (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
                (2*LOG_MAX_LI_E));
      }


      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_2,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );
      /* Read DL Header Type 2 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t2, cps));

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
      /* crc pass and MCS6 and b_extra_depadding set (means previous block was RM_MCS_8 )
      */
      if( EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl_blk1_ptr->bsn, dl[gas_id].ws))
      {
        if(E_RX_ARR(gas_id, dl_blk1_ptr->bsn).b_extra_depadding &&
           dl_blk1_ptr->cs == RM_MCS_6
          )
        {
          /* CRC has passed for the MCS_6 block
          */
          if ( dl_blk1_ptr->failed_crc == FALSE)
          {
            is_padding_removed = TRUE;
            if(e_cur_hdr[gas_id].cps == 0 || e_cur_hdr[gas_id].cps == 1)
            {
              /* Force cps value incorrect no padding to padding( 0->2 , 1->3)
              */
              e_cur_hdr[gas_id].cps += 2;
              /* Strip 6 bytes of pl1_blk_ptr->payload_1_ptr*/
              if (!gprs_pdu_strip_head(&(pl1_blk_ptr->payload_1_ptr),pad_buf,E_GRLC_PADDING_BYTES))
              {
                MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_821, "e_xlate_ext_dl_data Failed to strip payload");
              }
              MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_822, "ERLD DL_CORRECT_DEPADDING bsn=%u mcs=%u",
                        dl_blk1_ptr->bsn,
                        dl_blk1_ptr->cs);
            }
            E_RX_ARR(gas_id, dl_blk1_ptr->bsn).b_extra_depadding = FALSE;
            dl_blk1_ptr->b_extra_depadding = FALSE;
          }
          else
          {
            /*  if the re-transmitted mcs_6 block is having bad crc again,
             ** b_extra_depadding should not be reset.
            */
            dl_blk1_ptr->b_extra_depadding = TRUE;
            MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_823, "ERLD DL_CORRECT_DEPADDING MCS_6 CRC bsn=%u mcs=%u",
                        dl_blk1_ptr->bsn,
                        dl_blk1_ptr->cs);
          }
        }
        else
        {
          dl_blk1_ptr->b_extra_depadding = FALSE;
        }
      }
      else
      {
        /* bsn out of receive window
         */
        dl_blk1_ptr->b_extra_depadding = FALSE;
      }
#endif /* FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */
      break;
    }

    case RM_MCS_7:
    case RM_MCS_8:
    case RM_MCS_9:
    {

      /* Read DL Header Type 1 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].bsn2  = (uint16)eunpackw(pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t1, bsn2));
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t1, cps));

      dl_blk2_ptr->cs         = pl1_blk_ptr->mcs;
      dl_blk2_ptr->crc        = pl1_blk_ptr->crc_2;
      dl_blk2_ptr->failed_crc = (pl1_blk_ptr->crc_2 == 0) ? TRUE : FALSE;
      dl[gas_id].time_slot            = pl1_blk_ptr->time_slot;

      /* Read FBI bit from FBI&E bits. 2nd LSbit */
      dl_blk2_ptr->fbi       = (uint8) (pl1_blk_ptr->fbi_e_2 & 0x02) >> 1;

      /* If block failed CRC and in ACK mode, set FBI to 0 to
      ** avoid accidental DL TBF release. NW will resend the DL block
      ** when it sees EPDAN with clear FAI.
      */

      /* Read E bit from FBI&E bits. LSbit */
      dl_blk2_ptr->ext       = (uint8) pl1_blk_ptr->fbi_e_2 & 0x01;

      /* If block failed CRC , set FBI to 0 to
       ** avoid accidental DL TBF release. NW will resend the DL block
       ** when it sees EPDAN with clear FAI.
      */
      if( !pl1_blk_ptr->crc_2 )
      {
        dl_blk2_ptr->fbi = 0;
        dl_blk2_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;
      }

      dl_blk2_ptr->bsn       = MOD_ESNS((e_cur_hdr[gas_id].bsn1 + e_cur_hdr[gas_id].bsn2));

      /* Copy CRC status of Payloads into log MASK */
      log_crc_status |= (pl1_blk_ptr->crc_2 & 0x01) << 1;
      log_crc_status |= (pl1_blk_ptr->crc_1 & 0x01);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E Bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 2*LOG_MAX_LI_E = 6 Bytes of Payload - 1 */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld),
                (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
                (2*LOG_MAX_LI_E));
      }

      if (pl1_blk_ptr->crc_2 && pl1_blk_ptr->payload_2_ptr)
      {
         /* Copy FBI & E Bits of Payload 2 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_2 & 0x03) << 4;
         /* Copy 2*LOG_MAX_LI_E = 6 Bytes of Payload - 2 */
         memscpy((void *)(&log_dl_pld[2*LOG_MAX_LI_E]), sizeof(log_dl_pld),
                (void *)pl1_blk_ptr->payload_2_ptr->data_ptr,
                (2*LOG_MAX_LI_E));
      }

#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
      /* Mark the block as b_extra_depadding required if CRC=FAIL and
      ** MCS=RM_MCS_8,so that if the block is re-transmitted as MCS6 or MCS3
      ** cps can be corrected if incorrect from Network.
      */
      if( EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl[gas_id].vq, dl_blk1_ptr->bsn, dl[gas_id].ws))
      {
        if((dl_blk1_ptr->failed_crc) && (dl_blk1_ptr->cs == RM_MCS_8) &&
           (E_RX_ARR(gas_id, dl_blk1_ptr->bsn).vn == RXS_INVALID)
          )
        {
          dl_blk1_ptr->b_extra_depadding = TRUE;
          MSG_GRLC_MED_2_G(GRLC_F3_MSG_ENUM_824, "ERLD incoming BSN1 mcs 8 marked TRUE bsn=%u mcs=%u",
                      dl_blk1_ptr->bsn,
                      dl_blk1_ptr->cs);
        }
        else
        {
          dl_blk1_ptr->b_extra_depadding = FALSE;
        }
      }
      else
      {
        dl_blk1_ptr->b_extra_depadding = FALSE;
      }

#endif /* FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING */

      /*
      ** process 2nd bsn after vq is updated.
      */

      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_1,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );

#ifdef  EGPRS_RLC_DEBUG
      /* Check that [(BSN_of_2nd_block - BSN_of_1st_block) mod SNS] < WS */
      if( MOD_ESNS( (dl_blk2_ptr->bsn + ESNS - dl_blk1_ptr->bsn) ) >= dl[gas_id].ws )
      {
        MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_825, "GRLD 2nd BSN %d - 1st BSN %d >= WS %d ",
          dl_blk2_ptr->bsn,dl_blk1_ptr->bsn, dl[gas_id].ws);
      }
#endif /* EGPRS_RLC_DEBUG */

      /* Read the extension octets if present form the second payload.
      ** Note that in UNACK mode we might be processing a payload that failed CRC.
      ** In this case ignore the LI/E extension octets, they might carry invalid values
      */
      if( (dl_blk2_ptr->ext == RM_EXT_OCTET_FOLLOWS) &&
          (pl1_blk_ptr->payload_2_ptr != NULL)       &&
          (dl_blk2_ptr->crc == 1)                       )
      {
        /* loop to parse all Extension Octet array
        */
        do
        {
          /* get the 'next' extension octet into buf[0]
          */
          if (gprs_pdu_strip_head(&pl1_blk_ptr->payload_2_ptr, buf, (uint16)1))
          {
            /* must reset bit position pointer every time gunpackb() is called
            */
            msg_pos = 0;

            /* Extract the LI, and E of the extension octet
            */
            dl_blk2_ptr->ext_octet[by].egprs.li  =
              (uint8) gunpackb(buf, &msg_pos, GSIZE(e_grlc_ext_octet_t, li));

            dl_blk2_ptr->ext_octet[by].egprs.ext =
              (uint8) gunpackb(buf, &msg_pos, GSIZE(e_grlc_ext_octet_t, ext));
          }
          else
          {
            MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_826, "GRLD xlate_ext_dl_data() Error stripping extension octet");
            break;
          }

          /* Next LI field. 'by' carries the number of extension octet when the
          ** loop terminates
          */
          ++by;

        } while ( /* by-1 indexes the current extension bit
                  */
                  dl_blk2_ptr->ext_octet[by-1].egprs.ext == RM_EXT_OCTET_FOLLOWS &&
                  by < E_MAX_NUM_EXTENSION_OCTETS
                );
      } /* end extension octet(s) processing */

      /* set the number of Extension Octets in the array
      */
      dl_blk2_ptr->num_ext_octets = by;

#ifdef FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI
      /*  verify if the sum of LI do not exceed payload length
      */
      if( (dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK) && (pl1_blk_ptr->crc_2) )
      {
        /* loop around to validate octet_cnt
        */
        if(dl_blk2_ptr->num_ext_octets)
        {
          /* account for L1 */
          total_len = dl_blk2_ptr->num_ext_octets;

          for (b=0; b < dl_blk2_ptr->num_ext_octets; b++)
          {
            /* Validate LI and E combinations according to TS 04.60 Table 10.4.14a.1.
            ** - LI=0 only valid in first extension octet.
            ** - Reserved values 74 < LI != 127.
            ** - New PDU can't follow filling octets
            */
            if( (b!=0 && dl_blk2_ptr->ext_octet[b].egprs.li == 0)                             ||
                (dl_blk2_ptr->ext_octet[b].egprs.li == 127 && dl_blk2_ptr->ext_octet[b].egprs.ext == RM_EXT_OCTET_FOLLOWS) ||
                (dl_blk2_ptr->ext_octet[b].egprs.li > 74   && dl_blk2_ptr->ext_octet[b].egprs.li != 127)
              )
            {
              b_nack = TRUE;

              MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_827, "ELRD Inconsistent LI and E-bit combination when CRC pass b=%u l1=%u ext=%u",
                               b,
                               dl_blk2_ptr->ext_octet[b].egprs.li,
                               dl_blk2_ptr->ext_octet[b].egprs.ext);
              break;
            }
            else
            {
              if ( dl_blk2_ptr->ext_octet[b].egprs.li != 127)
              {
                total_len += dl_blk2_ptr->ext_octet[b].egprs.li;
              }
            }

            /* checking of invalid length sum.
            */
            if (total_len > E_GRLC_DATA_BLOCK_LEN[dl_blk2_ptr->cs])
            {
              b_nack =  TRUE;
              MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_828, "ERLD Invalid LI sum for payload crc pass, nacking the block");
              break;
            }
          }

          /* Free the block and mark it as payload crc failure
          ** Note : Logging will still show crc pass.
          */
          if (b_nack)
          {
            GPRS_PDU_FREE( &pl1_blk_ptr->payload_2_ptr);

            pl1_blk_ptr->payload_2_ptr = NULL;
            pl1_blk_ptr->crc_2 = 0;

            /* mark the FBI and extension octet invalid
            */
            dl_blk2_ptr->fbi = 0;
            dl_blk2_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;

            dl_blk2_ptr->failed_crc = (pl1_blk_ptr->crc_2 == 0) ? TRUE : FALSE;

            MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_829, "ERLD incorrect crc pass block %u fn=%ld mcs=%u",
                                    dl_blk2_ptr->bsn,
                                    pl1_blk_ptr->frame_number,
                                    dl_blk2_ptr->cs);
          }
        }

        /* reset the validation params for blk1 below */
        b_nack    = FALSE;
        total_len = 0;
      } /* end of rlc_ack mode */
#endif /* FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI */

      /* Copy PL1 payload pointer to the RLC block payload pointer.
      */
      dl_blk2_ptr->dsm_payload_ptr = pl1_blk_ptr->payload_2_ptr;

      /* Delink pointer */
      pl1_blk_ptr->payload_2_ptr = NULL;

      if(  dl_blk2_ptr->dsm_payload_ptr )
      {
        /* Verify the data block length in accordance with the coding scheme
        */
        blck_len = (uint8)GPRS_PDU_LENGTH(dl_blk2_ptr->dsm_payload_ptr);

        if( (blck_len + by) != E_GRLC_DATA_BLOCK_LEN[dl_blk2_ptr->cs] )
        {
            MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_830, "ERLD invalid blck length mcs_len=%d pl1_len=%d",
              E_GRLC_DATA_BLOCK_LEN[dl_blk2_ptr->cs], blck_len+by);
        }

        /* Data length of this block is the length of the remaining data block,
        ** after the LI/E extensions octets have been stripped out.
        */
        dl_blk2_ptr->len = blck_len;
      }

      break;
    }

    default:
     MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_831, "Invalid Coding Scheme %d",pl1_blk_ptr->mcs);
    break;

  } /* switch MCS */

  /* Reset counter before parsing 1st payload's extension octets
  */
  by = 0;

  /* Reset Block Length Counter
  */
  blck_len  = 0;

  /* Read the extension octets if present form the first payload.
  ** Note that in UNACK mode we might be processing a payload that failed CRC.
  ** In this case ignore the LI/E extension octets, they might carry invalid values
  */
  if( (dl_blk1_ptr->ext == RM_EXT_OCTET_FOLLOWS) &&
      (pl1_blk_ptr->payload_1_ptr != NULL)       &&
      (dl_blk1_ptr->crc == 1)                       )
  {
    /* loop to parse all Extension Octet array
    */
    do
    {
      /* get the 'next' extension octet into buf[0]
      */
      if (gprs_pdu_strip_head(&pl1_blk_ptr->payload_1_ptr, buf, (uint16)1))
      {
        /* must reset bit position pointer every time gunpackb() is called
        */
        msg_pos = 0;

        /* Extract the LI, and E of the extension octet
        */
        dl_blk1_ptr->ext_octet[by].egprs.li  =
          (uint8) gunpackb(buf, &msg_pos, GSIZE(e_grlc_ext_octet_t, li));

        dl_blk1_ptr->ext_octet[by].egprs.ext =
          (uint8) gunpackb(buf, &msg_pos, GSIZE(e_grlc_ext_octet_t, ext));
      }
      else
      {
        MSG_GRLC_ERROR_0_G(GRLC_F3_MSG_ENUM_832, "GRLD xlate_ext_dl_data() Error stripping extension octet");
        break;
      }

      /* Next LI field. 'by' carries the number of extension octet when the
      ** loop terminates
      */
      ++by;

    } while ( /* by-1 indexes the current extension bit
              */
              dl_blk1_ptr->ext_octet[by-1].egprs.ext == RM_EXT_OCTET_FOLLOWS &&
              by < E_MAX_NUM_EXTENSION_OCTETS
            );
  } /* end extension octet(s) processing */

  /* set the number of Extension Octets in the array
  */
  dl_blk1_ptr->num_ext_octets = by;

#ifdef FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI
  /*  verify if the sum of LI do not exceed payload length
  */
  if( (dl[gas_id].dl_ass.rlc_mode == GRLC_MODE_ACK) && (pl1_blk_ptr->crc_1) )
  {
    /* loop around to validate octet_cnt
    */
    if(dl_blk1_ptr->num_ext_octets)
    {
      uint8 payload_len = E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs];

      /* actual payload length when block combined
      */
      if (dl_blk1_ptr->spb == SPB_RETX_PART_1 )
      {
        payload_len = (uint8)(2 * payload_len);
      }

      /* accounting for length when padded
      */
      if (is_padding_removed)
      {
        payload_len -= E_GRLC_PADDING_BYTES;
      }

      /* account for L1 */
      total_len = dl_blk1_ptr->num_ext_octets;

      for (b=0; b < dl_blk1_ptr->num_ext_octets; b++)
      {
        /* Validate LI and E combinations according to TS 04.60 Table 10.4.14a.1.
        ** - LI=0 only valid in first extension octet.
        ** - Reserved values 74 < LI != 127.
        ** - New PDU can't follow filling octets
        */
        if( (b!=0 && dl_blk1_ptr->ext_octet[b].egprs.li == 0)                             ||
            (dl_blk1_ptr->ext_octet[b].egprs.li == 127 && dl_blk1_ptr->ext_octet[b].egprs.ext == RM_EXT_OCTET_FOLLOWS) ||
            (dl_blk1_ptr->ext_octet[b].egprs.li > 74   && dl_blk1_ptr->ext_octet[b].egprs.li != 127)
          )
        {
          b_nack = TRUE;

          MSG_GRLC_HIGH_3_G(GRLC_F3_MSG_ENUM_833, "ELRD Inconsistent LI and E-bit combination when CRC pass b=%u l1=%u ext=%u",
                           b,
                           dl_blk1_ptr->ext_octet[b].egprs.li,
                           dl_blk1_ptr->ext_octet[b].egprs.ext);
          break;
        }
        else
        {
          if (dl_blk1_ptr->ext_octet[b].egprs.li != 127)
          {
            total_len += dl_blk1_ptr->ext_octet[b].egprs.li;
          }
        }

        /* checking of invalid length sum.
        */
        if (total_len > payload_len)
        {
          b_nack =  TRUE;
          MSG_GRLC_HIGH_0_G(GRLC_F3_MSG_ENUM_834, "ERLD Invalid LI sum for payload crc pass, nacking the block");
          break;
        }
      }

      /* Free the block and mark it as payload crc failure
      ** Note : Logging will still show crc pass.
      */
      if (b_nack)
      {
        GPRS_PDU_FREE( &pl1_blk_ptr->payload_1_ptr);

        pl1_blk_ptr->payload_1_ptr = NULL;
        pl1_blk_ptr->crc_1 = 0;

        /* mark the FBI and extension octet invalid
        */
        dl_blk1_ptr->fbi = 0;
        dl_blk1_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;

        dl_blk1_ptr->failed_crc = (pl1_blk_ptr->crc_1 == 0) ? TRUE : FALSE;

        MSG_GRLC_ERROR_3_G(GRLC_F3_MSG_ENUM_835, "ERLD incorrect crc pass block %u fn=%ld mcs=%u",
                                          dl_blk1_ptr->bsn,
                                          pl1_blk_ptr->frame_number,
                                          dl_blk1_ptr->cs);
      }
    }
  } /* end of rlc mode */
#endif /* FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI */

  /* Copy PL1 payload pointer to the RLC block payload pointer.
  */
  dl_blk1_ptr->dsm_payload_ptr = pl1_blk_ptr->payload_1_ptr;

  /* Delink pointer */
  pl1_blk_ptr->payload_1_ptr = NULL;


  if( dl_blk1_ptr->dsm_payload_ptr )
  {
    /* Verify the data block length in accordance with the coding scheme
    */
    blck_len = (uint8) GPRS_PDU_LENGTH(dl_blk1_ptr->dsm_payload_ptr);
    if( is_padding_removed )
    {
      if( (blck_len + by + E_GRLC_PADDING_BYTES) !=
            E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_836, "ERLD invalid unpadded blck length mcs_len=%d pl1_len=%d",
                  E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs],
                  blck_len + by + E_GRLC_PADDING_BYTES
                  );
        /* Assign safe value */
        blck_len = E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] - by - E_GRLC_PADDING_BYTES;
      }
    }
    else
    {
      if( (blck_len + by) != E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_837, "ERLD invalid blck length mcs_len=%d pl1_len=%d",
                  E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs],
                  blck_len + by
                  );

        /* Assign safe value */
        blck_len = E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] - by;
      }
    }

    /* Data length of this block is the length of the remaining data block,
    ** after the LI/E extensions octets have been stripped out.
    */
    dl_blk1_ptr->len = blck_len;
  }

  /*--------------------------------------------------------------------------
  ** Error protections. Not essential.
  ** Verify that both RLC and PL1 have decoded the same ES/P and RRBP values.
  **--------------------------------------------------------------------------
  */
  if( e_cur_hdr[gas_id].esp != pl1_blk_ptr->esp )
  {
    MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_838, "ERLD Conflicting ES/P values RLC %d vs PL1 %d",
              e_cur_hdr[gas_id].esp,
              pl1_blk_ptr->esp
              );
  }

  if( e_cur_hdr[gas_id].rrbp != pl1_blk_ptr->rrbp )
  {
    MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_839, "ERLD Conflicting RRBP values RLC %d vs PL1 %d",
              e_cur_hdr[gas_id].rrbp,
              pl1_blk_ptr->rrbp
              );
  }

} /* e_xlate_ext_dl_data */

/*===========================================================================
===
===  FUNCTION      E_TEST_MODE_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink EGPRS RLC data block from External format into internal
===    format for ETSI test mode B.
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data blocks (MAC header, RLC header and payload)
===
===    dl_blk1_ptr/dl_blk2_ptr:
===       Internal structure representing rlc dl data blocks
===
===  DEPENDENCIES
===
===    none
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
void e_test_mode_xlate_ext_dl_data
(
  gas_id_t              gas_id,
  e_pl1_radio_block_t   *pl1_blk_ptr,
  grlc_dl_data_block_t  *dl_blk1_ptr,
  grlc_dl_data_block_t  *dl_blk2_ptr
)
{
  uint8  by      = 0; /* loop count byte */
  uint16 msg_pos = 0; /* offset from the beginning of the message to the current item */
  uint8  log_crc_status = 0;
  uint8   blck_len;
  boolean is_padding_removed;
  uint8   log_dl_pld[4*LOG_MAX_LI_E] = {0}; /* Init. Buffer to account for 2 Payloads */

  /* Check if padding octets have been removed by PL1 from the DL block.
  ** We need this info to determine the actual payload length below.
  */
  is_padding_removed = e_grlc_dl_is_padding_removal_required( pl1_blk_ptr->hdr_arr,
                                                              pl1_blk_ptr->mcs);

  /* Store RLC data block info included in the PL1 radio block.
  */
  dl_blk1_ptr->cs        = pl1_blk_ptr->mcs;
  dl_blk1_ptr->crc       = pl1_blk_ptr->crc_1;
  dl[gas_id].time_slot           = pl1_blk_ptr->time_slot;
  dl[gas_id].frame_number        = pl1_blk_ptr->frame_number;

  /* Read FBI bit from FBI&E bits. 2nd LSbit */
  dl_blk1_ptr->fbi       = (uint8) (pl1_blk_ptr->fbi_e_1 & 0x02) >> 1;

  /* Read E bit from FBI&E bits. LSbit */
  dl_blk1_ptr->ext       = (uint8) pl1_blk_ptr->fbi_e_1 & 0x01;


  /* Extract fields that are common to all header types
  */
  msg_pos = 0;

  e_cur_hdr[gas_id].usf   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, usf));
  e_cur_hdr[gas_id].esp   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, esp));
  e_cur_hdr[gas_id].rrbp  = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, rrbp));
  e_cur_hdr[gas_id].tfi   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, tfi));
  e_cur_hdr[gas_id].pr    = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, pr));
  e_cur_hdr[gas_id].bsn1  = (uint16)eunpackw(pl1_blk_ptr->hdr_arr,
                                            &msg_pos,
                                            GSIZE(e_grlc_dl_hdr, bsn1));
  dl_blk1_ptr->bsn = e_cur_hdr[gas_id].bsn1;

  /* Extract fields hat are specific to each of the header types
  */
  switch (pl1_blk_ptr->mcs)
  {
    case RM_MCS_1:
    case RM_MCS_2:
    case RM_MCS_3:
    case RM_MCS_4:
    {
      log_crc_status  |= (pl1_blk_ptr->crc_1 & 0x01);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 6 Bytes of Payload */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld), (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
          (2*LOG_MAX_LI_E));
      }

      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_3,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );


      /* Read DL Header Type 3 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t3, cps));
      e_cur_hdr[gas_id].spb   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t3, spb));

      dl_blk1_ptr->spb = e_cur_hdr[gas_id].spb;

      if (e_cur_hdr[gas_id].spb == SPB_RETX_PART_2)
      {
        /* LI/E are meaningless in the 2nd split */
        dl_blk1_ptr->ext = RM_EXT_NO_OCTET_FOLLOWS;
      }
      break;
    }

    case RM_MCS_5:
    case RM_MCS_6:
    {

      log_crc_status  |= (pl1_blk_ptr->crc_1 & 0x01);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 6 Bytes of Payload */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld), (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
         (2*LOG_MAX_LI_E));
      }


      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_2,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );

      /* Read DL Header Type 2 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t2, cps));

      break;
    }

    case RM_MCS_7:
    case RM_MCS_8:
    case RM_MCS_9:
    {

      /* Read DL Header Type 1 from right to left, least significant octet first.
      */
      e_cur_hdr[gas_id].bsn2  = (uint16)eunpackw(pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t1, bsn2));
      e_cur_hdr[gas_id].cps   = (uint8)eunpackw( pl1_blk_ptr->hdr_arr,
                                                &msg_pos,
                                                GSIZE(e_grlc_dl_hdr_t1, cps));

      dl_blk2_ptr->cs        = pl1_blk_ptr->mcs;
      dl_blk2_ptr->crc       = pl1_blk_ptr->crc_2;
      dl[gas_id].time_slot           = pl1_blk_ptr->time_slot;

      /* Read FBI bit from FBI&E bits. 2nd LSbit */
      dl_blk2_ptr->fbi       = (uint8) (pl1_blk_ptr->fbi_e_2 & 0x02) >> 1;

      /* Read E bit from FBI&E bits. LSbit */
      dl_blk2_ptr->ext       = (uint8) pl1_blk_ptr->fbi_e_2 & 0x01;
      dl_blk2_ptr->bsn       = MOD_ESNS((e_cur_hdr[gas_id].bsn1 + e_cur_hdr[gas_id].bsn2));

      /* Copy CRC status of payloads into log MASK */
      log_crc_status |= (pl1_blk_ptr->crc_2 & 0x1) << 1;
      log_crc_status |= (pl1_blk_ptr->crc_1 & 0x1);

      if (pl1_blk_ptr->crc_1 && pl1_blk_ptr->payload_1_ptr)
      {
         /* Copy FBI & E bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_1 & 0x03) << 2;
         /* Copy 6 Bytes of Payload - 1 */
         memscpy((void *)log_dl_pld, sizeof(log_dl_pld), (void *)pl1_blk_ptr->payload_1_ptr->data_ptr,
         (2*LOG_MAX_LI_E));
      }

      if (pl1_blk_ptr->crc_2 && pl1_blk_ptr->payload_2_ptr)
      {
         /* Copy FBI & E bits of Payload 1 */
         log_crc_status |= (pl1_blk_ptr->fbi_e_2 & 0x03) << 4;
         /* Copy 6 Bytes of Payload - 2 */
         memscpy((void *)&log_dl_pld[(2*LOG_MAX_LI_E)], sizeof(log_dl_pld),
                (void *)pl1_blk_ptr->payload_2_ptr->data_ptr,
                (2*LOG_MAX_LI_E));
      }

      rlc_log_dl_header(gas_id, (byte)EGPRS_HEADER_TYPE_1,
                        pl1_blk_ptr->hdr_arr,
                        log_crc_status,
                        pl1_blk_ptr->time_slot,
                        pl1_blk_ptr->frame_number,
                        log_dl_pld
                       );


      /* set the number of Extension Octets in the array
      */
      dl_blk2_ptr->num_ext_octets = by;

      /* Copy PL1 payload pointer to the RLC block payload pointer.
      */
      dl_blk2_ptr->dsm_payload_ptr = pl1_blk_ptr->payload_2_ptr;

      if(  dl_blk2_ptr->dsm_payload_ptr )
      {
        /* Verify the data block length in accordance with the coding scheme
        */
        blck_len = (uint8)GPRS_PDU_LENGTH(dl_blk2_ptr->dsm_payload_ptr);

        if( (blck_len + by) != E_GRLC_DATA_BLOCK_LEN[dl_blk2_ptr->cs] )
        {
            MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_840, "ERLD invalid blck length mcs_len=%d pl1_len=%d",
              E_GRLC_DATA_BLOCK_LEN[dl_blk2_ptr->cs], blck_len+by);
        }

        /* Data length of this block is the length of the remaining data block,
        ** after the LI/E extensions octets have been stripped out.
        */
        dl_blk2_ptr->len = blck_len;
      }

      break;
    }

    default:
     MSG_GRLC_ERROR_1_G(GRLC_F3_MSG_ENUM_841, "Invalid Coding Scheme %d",pl1_blk_ptr->mcs);
    break;

  } /* switch MCS */

  /* Reset counter before parsing 1st payload's extension octets
  */
  by = 0;

  /* set the number of Extension Octets in the array
  */
  dl_blk1_ptr->num_ext_octets = by;

  /* Copy PL1 payload pointer to the RLC block payload pointer.
  */
  dl_blk1_ptr->dsm_payload_ptr = pl1_blk_ptr->payload_1_ptr;


  if( dl_blk1_ptr->dsm_payload_ptr )
  {
    /* Verify the data block length in accordance with the coding scheme
    */
    blck_len = (uint8) GPRS_PDU_LENGTH(dl_blk1_ptr->dsm_payload_ptr);
    if( is_padding_removed )
    {
      if( (blck_len + by + E_GRLC_PADDING_BYTES) !=
            E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_842, "ERLD invalid unpadded blck length mcs_len=%d pl1_len=%d",
                  E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs],
                  blck_len + by + E_GRLC_PADDING_BYTES);
      }
    }
    else
    {
      if( (blck_len + by) != E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs] )
      {
        MSG_GRLC_ERROR_2_G(GRLC_F3_MSG_ENUM_843, "ERLD invalid blck length mcs_len=%d pl1_len=%d",
          E_GRLC_DATA_BLOCK_LEN[dl_blk1_ptr->cs], blck_len+by);
      }
    }

    /* Data length of this block is the length of the remaining data block,
    ** after the LI/E extensions octets have been stripped out.
    */
    dl_blk1_ptr->len = blck_len;
  }
} /* e_test_mode_xlate_ext_dl_data */


/*===========================================================================
===
===  FUNCTION      XLATE_INT_UL_DATA.
===
===  DESCRIPTION
===
===    Translate uplink RLC data block from internal format into external format
===    This function also applies RLC filler octets when the remaining octets
===    is not zero.
===
===
===  PARAMS
===    *ul_data:  structure representing ul RLC data block
===    *dst:      array of bytes representing external RLC data block
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    the dup_dsm_ptr is NULL after calling gprs_pdu_strip_head()
===
===========================================================================*/
void xlate_int_ul_data
(
  grlc_ul_data_block_t  *ul_data,
  uint8                 *dst
)
{
  uint8 by;           /* loop count */
  uint16 u, msg_pos;  /* offset from the beginning of the message to the current item */

 /*---------------------------------------------------------------------*/

  msg_pos = 0;

  /* translate mac header using fn. However, the translation fn does not
  ** update the msg position pointer.
  */
  xlate_int_mac_hdr(&(ul_data->mac_hdr), dst);
  msg_pos += GSIZE(grlc_ul_data_block_t, mac_hdr);

  /* ul rlc data header octet 1 */

  /* spare, pi, tfi and ti fields */
  gpackb(RLC_SPARE, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, spare));
  gpackb(ul_data->pi, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, pi));
  gpackb(ul_data->tfi, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, tfi));
  gpackb((uint8)ul_data->ti, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, ti));

  /* ul rlc data header octet 2 */

  /* bsn & ext fields */
  gpackb((uint8)ul_data->bsn, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, bsn));
  gpackb(ul_data->ext, dst, &msg_pos, GSIZE(grlc_ul_data_block_t, ext));

  /* loop to translate the extension octet array */
  for (by = 0; by < ul_data->num_ext_octets; by++)
  {
    gpackb(ul_data->ext_octet[by].gprs.li, dst, &msg_pos, GSIZE(grlc_ext_octet_t, li));
    gpackb(ul_data->ext_octet[by].gprs.more, dst, &msg_pos, GSIZE(grlc_ext_octet_t, more));
    gpackb(ul_data->ext_octet[by].gprs.ext, dst, &msg_pos, GSIZE(grlc_ext_octet_t, ext));
  }

  /*-----------------------------------------------------------------
  ** If TLLI indicator is TRUE then put TLLI fields in MSByte first
  ** (low address). Advance msg_pos 8 bits at a time.
  **-----------------------------------------------------------------
  */
  if (ul_data->ti == RM_TLLI_PRESENT)
  {
    dst[msg_pos/8] = (uint8)((ul_data->tlli >> 24) & 0xff);
    msg_pos += 8;

    dst[msg_pos/8] = (uint8)((ul_data->tlli >> 16) & 0xff);
    msg_pos += 8;

    dst[msg_pos/8] = (uint8)((ul_data->tlli >>  8) & 0xff);
    msg_pos += 8;

    dst[msg_pos/8] = (uint8)(ul_data->tlli & 0xff);
    msg_pos += 8;
  }

  /*-----------------------------------------------------------------
  ** If PFI indicator is TRUE then pack PFI field and the E bit.
  **-----------------------------------------------------------------
  */
  if( ul_data->pi == RM_PFI_PRESENT )
  {
    gpackb(ul_data->pfi, dst, &msg_pos, grlc_ul_data_block_t_pfi_len );
    gpackb(ul_data->pi, dst, &msg_pos, grlc_ul_data_block_t_pfi_e_len );
  }

  /* 'Strip' dsm data into a straight buffer to pass to L1 and ultimately DSP
  */
  if (!gprs_pdu_strip_head(&(ul_data->dup_dsm_ptr),
                           &dst[msg_pos/8],     /* offset into n'th octet */
                           ul_data->data_len    /* msg_pos is length in bits */
                          )
     )
  {
    MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_844, "xlate_e_d Failed to strip payload");
  }

  /* If dup_dsm_ptr is not NULL after stripping then warn of memory leak
  */
  if (ul_data->dup_dsm_ptr != NULL)
  {
    u = (uint16)GPRS_PDU_LENGTH(ul_data->dup_dsm_ptr);
    MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_845, "GRLU xlate_int non-null payload post-strip l=%u", u);
    GPRS_PDU_FREE(&ul_data->dup_dsm_ptr);
  }

  /* pad the remainder of data block with RLC filler octets starting from
  ** the end of the block (at data_len) for 'rem_cnt' octets
  */
  if (ul_data->rem_cnt)
  {
    memset(&dst[msg_pos/8+ul_data->data_len],
            RM_UL_FILLER_OCTET,
            ul_data->rem_cnt
          );
  }
} /* xlate_int_ul_data */

/*===========================================================================
===
===  FUNCTION      E_XLATE_INT_UL_DATA.
===
===  DESCRIPTION
===
===    Translate uplink EGPRS RLC data block from internal format into external format
===    This function also applies RLC filler octets when the remaining octets
===    is not zero.
===
===
===  PARAMS
===    *ul_data1:  structure representing 1st ul RLC data block.
===    *ul_data2:  structure representing 2nd ul RLC data block.
===                NULL for single payload MCS.
===    *dst1:      pointer to egprs PL1 FIFO element
===    *dst2:      pointer to 2nd egprs PL1 FIFO element incase of a split block
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    the dup_dsm_ptr is NULL after calling gprs_pdu_strip_head()
===
===========================================================================*/
void e_xlate_int_ul_data
(
  grlc_ul_data_block_t      *ul_data1,
  grlc_ul_data_block_t      *ul_data2,
  e_grlc_ul_fifo_element_t  *dst1,
  e_grlc_ul_fifo_element_t  *dst2
)
{
  uint8 by;                 /* loop count */
  uint16 u, msg_pos;        /* offset from the beginning of the message to the
                            ** current item
                            */
  uint8  temp_fill_len = 0; /* Holds the number of the fill octets */
  uint8  temp_data_len = 0; /* Holds the number of the payload octets */
 /*---------------------------------------------------------------------*/

  /* Copy the data block's MCS to the PL1 block */
  dst1->mcs = ul_data1->cs;

  /* Copy the data block's PS value to the PL1 block */
  dst1->ps1 = ul_data1->ps;

  /* Must clear header octets prior to ORing fields in
  */
  dst1->hdr[0] = 0;
  dst1->hdr[1] = 0;
  dst1->hdr[2] = 0;
  dst1->hdr[3] = 0;
  dst1->hdr[4] = 0;
  dst1->hdr[5] = 0;

  if( dst2 )
  {
    /* Copy the data block's MCS to the PL1 block */
    dst2->mcs = ul_data1->cs;

    /* Copy the data block's PS value to the PL1 block */
    dst2->ps1 = ul_data1->ps;

    dst2->hdr[0] = 0;
    dst2->hdr[1] = 0;
    dst2->hdr[2] = 0;
    dst2->hdr[3] = 0;
    dst2->hdr[4] = 0;
    dst2->hdr[5] = 0;
  }

  msg_pos = 0;

  /* Fill in the first 5 common fields for all headers types
  */
  epackw( ul_data1->hdr.retry, dst1->hdr, &msg_pos, e_grlc_ul_hdr_retry_len );
  epackw( ul_data1->hdr.si, dst1->hdr, &msg_pos, e_grlc_ul_hdr_si_len );

  /* In case of a dual payload block the CV in the header belongs to the 2nd payload
  */
  if( ul_data2 != NULL &&
      ( ul_data2->cs == RM_MCS_7 ||
        ul_data2->cs == RM_MCS_8 ||
        ul_data2->cs == RM_MCS_9
      )
    )
  {
    epackw( ul_data2->hdr.cv, dst1->hdr, &msg_pos, e_grlc_ul_hdr_cv_len );
  }
  else
  {
    epackw( ul_data1->hdr.cv, dst1->hdr, &msg_pos, e_grlc_ul_hdr_cv_len );
  }

  epackw( ul_data1->hdr.tfi, dst1->hdr, &msg_pos, e_grlc_ul_hdr_tfi_len );
  epackw( ul_data1->hdr.bsn1, dst1->hdr, &msg_pos, e_grlc_ul_hdr_bsn1_len );

  switch (ul_data1->cs)
  {
    case RM_MCS_1:
    case RM_MCS_2:
    case RM_MCS_3:
    case RM_MCS_4:
    {
      /* Fill in the rest of Header Type 3
      */
      epackw( ul_data1->hdr.cps, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t3_cps_len );
      epackw( ul_data1->hdr.spb, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t3_spb_len );
      epackw( ul_data1->hdr.rsb, dst1->hdr, &msg_pos, e_grlc_ul_hdr_rsb_len );
      epackw( ul_data1->hdr.pi, dst1->hdr, &msg_pos, e_grlc_ul_hdr_pi_len );
      epackw( RLC_SPARE, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t3_spare_len );
      break;
    }
    case RM_MCS_5:
    case RM_MCS_6:
    {
      /* Fill in the rest of Header Type 2 */
      epackw( ul_data1->hdr.cps, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t2_cps_len );
      epackw( ul_data1->hdr.rsb, dst1->hdr, &msg_pos, e_grlc_ul_hdr_rsb_len );
      epackw( ul_data1->hdr.pi, dst1->hdr, &msg_pos, e_grlc_ul_hdr_pi_len );
      epackw( RLC_SPARE, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t2_spare_len );
      break;
    }
    case RM_MCS_7:
    case RM_MCS_8:
    case RM_MCS_9:
    {
      /* Fill in the rest of Header Type 1 */
      epackw( ul_data1->hdr.bsn2, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t1_bsn2_len );
      epackw( ul_data1->hdr.cps, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t1_cps_len );
      epackw( ul_data1->hdr.rsb, dst1->hdr, &msg_pos, e_grlc_ul_hdr_rsb_len );
      epackw( ul_data1->hdr.pi, dst1->hdr, &msg_pos, e_grlc_ul_hdr_pi_len );
      epackw( RLC_SPARE, dst1->hdr, &msg_pos, e_grlc_ul_hdr_t3_spare_len );

      break;
    }

    default:
      MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_846, "ERLU xlate_ext OOR MCS %u",ul_data1->cs);
      break;

  } /* end switch( ul_data->mcs ) */

  /* Finished with the header. Reset pointer position
  ** and start packing the 1st RLC payload.
  */
  msg_pos = 0;

  /* Deal with padding
  */
  if( IS_PADDING_REQUIRED(ul_data1->init_mcs, ul_data1->cs) )
  {
    /* Pad MSC-6 or MCS-3 block */
    memset( &dst1->msg1[msg_pos], 0, E_GRLC_PADDING_BYTES );

    /* Set pointer possition just after the padding i.e. 48th bit.
    */
    msg_pos += ( E_GRLC_PADDING_BYTES * sizeof(uint8) * 8);
  }

  /* loop to translate the extension octet array */
  for (by = 0; by < ul_data1->num_ext_octets; by++)
  {
    gpackb( ul_data1->ext_octet[by].egprs.li,
            dst1->msg1,
            &msg_pos,
            e_grlc_ext_octet_t_li_len);
    gpackb( ul_data1->ext_octet[by].egprs.ext,
            dst1->msg1,
            &msg_pos,
            e_grlc_ext_octet_t_ext_len);
  }

  /*-----------------------------------------------------------------------------------
  ** If TLLI indicator is TRUE then put TLLI field such that LSBytes of the TLLI field
  ** is at M+1, and MSBytes of the TLLI field is at M+4 (high-address)
  **-----------------------------------------------------------------------------------
  */
  if (ul_data1->ti == RM_TLLI_PRESENT)
  {
    dst1->msg1[msg_pos/8] = (uint8)((ul_data1->tlli) & 0xff);
    msg_pos += 8;

    dst1->msg1[msg_pos/8] = (uint8)((ul_data1->tlli >> 8) & 0xff);
    msg_pos += 8;

    dst1->msg1[msg_pos/8] = (uint8)((ul_data1->tlli >> 16) & 0xff);
    msg_pos += 8;

    dst1->msg1[msg_pos/8] = (uint8)((ul_data1->tlli >> 24) & 0xff);
    msg_pos += 8;
  }

  /*-----------------------------------------------------------------
  ** If PFI indicator is TRUE then pack PFI field and the E bit.
  **-----------------------------------------------------------------
  */
  if( ul_data1->hdr.pi == RM_PFI_PRESENT )
  {
    gpackb(ul_data1->pfi, dst1->msg1, &msg_pos, e_grlc_ul_data_block_t_pfi_len);
    gpackb(ul_data1->hdr.pi, dst1->msg1, &msg_pos, e_grlc_ul_data_block_t_pfi_e_len);
  }

  /* Check if the block needs to split. Note that dst2 is set to NULL if split
  ** is not required
  */
  if( dst2 != NULL )
  {
    /*--------------------------------------
    ** Construct the first split
    **-------------------------------------
    */

    /* Set the SPB field in the header (4th byte, 3rd and 4th bit) to indicate
    ** 1st split
    */
    dst1->hdr[3] = ( dst1->hdr[3] & 0xf3 ) | (uint8)( SPB_RETX_PART_1 << 2 );

    /* If length of payload does not fill the rest of the split block.
    ** Use filling octets to fill up the rest of the block.
    */
    if( (ul_data1->data_len + (msg_pos/8)) < E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] )
    {
      temp_fill_len = E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] -
                      (uint8)(ul_data1->data_len + (msg_pos/8));

      /* 'Strip' dsm data into a straight buffer to pass to L1 and ultimately DSP
      */
      if (!gprs_pdu_strip_head(&(ul_data1->dup_dsm_ptr),
                               &dst1->msg1[msg_pos/8], /* offset into n'th octet */
                               ul_data1->data_len      /* msg_pos is length in bits */
                              )
         )
      {
        MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_847, "xlate_e_d Failed to strip payload");
      }

      /* If dup_dsm_ptr is not NULL after stripping then warn of memory leak
      */
      if (ul_data1->dup_dsm_ptr != NULL)
      {
        u = (uint16)GPRS_PDU_LENGTH(ul_data1->dup_dsm_ptr);
        MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_848, "GRLU xlate_int non-null payload post-strip l=%u",u);
        GPRS_PDU_FREE(&ul_data1->dup_dsm_ptr);
      }

      msg_pos += ul_data1->data_len * sizeof(uint8);

      /* pad the remainder of data block with RLC filler octets starting from
      ** the end of the block (at data_len) for 'temp_fill_len' octets
      */

      memset(&dst1->msg1[msg_pos/8], RM_UL_FILLER_OCTET, temp_fill_len );

      if( IS_PADDING_REQUIRED(ul_data1->init_mcs, ul_data1->cs) )
      {

        /* Add the TI and E bits after the padding */
        shift_in_ti_e_fields( &dst1->msg1[E_GRLC_PADDING_BYTES],
                              (uint8) ( E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] -
                                        E_GRLC_PADDING_BYTES
                                      ),
                              ul_data1->ti, ul_data1->ext
                            );
      }
      else
      {
        /* Add the TI and E bits */
        shift_in_ti_e_fields( dst1->msg1, (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs],
                              ul_data1->ti, ul_data1->ext
                            );

      }

      /*--------------------------------------
      ** Construct the second split
      **-------------------------------------
      */
      msg_pos = 0;

      /* Copy header from the first split */
      memscpy(dst2->hdr, sizeof(dst2->hdr), dst1->hdr, E_GRLC_HDR_TYPE_MAX_LEN);

      /* Set the SPB field in the header (4th byte, 3rd and 4th bit) to indicate
      ** 2nd split
      */
      dst2->hdr[3] = ( dst2->hdr[3] & 0xf3 ) | (uint8)( SPB_RETX_PART_2 << 2 );


      /* TLLI and PFI are never included in the 2nd split. If they were included in
      ** the original MCS8 block, they are included on the 1st split block.
      ** Always set TI and PI to zero in the header of the 2nd split block.
      */

      if(ul_data1->hdr.pi)
        dst2->hdr[3] = ( dst2->hdr[3] & ~0x20 );

      /* pad data block with RLC filler octets. Payload was used up in the 1st split.
      */
      memset( dst2->msg1,
              RM_UL_FILLER_OCTET,
              (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] );

      /* Add the TI and E bits. As explained above, TI in the 2nd split is always '0' */
      shift_in_ti_e_fields( dst2->msg1, (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs],
                            0, ul_data1->ext
                          );

    } /* not enough payload for 1st split block */
    else
    {
      /* enough payload to fill the first split block
      */

      /*---------------------------------------
      ** Construct 1st split
      **--------------------------------------
      */

      /* 'Strip' dsm data into a straight buffer to pass to L1 and ultimately DSP
      */
      if (!gprs_pdu_strip_head(&(ul_data1->dup_dsm_ptr),
                               &dst1->msg1[msg_pos/8], /* offset into n'th octet */
                               (uint8)  ( E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] -
                                          (msg_pos/8)
                                        )
                               /* length in bits */
                              )
         )
      {
        MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_849, "xlate_e_d Failed to strip payload");
      }

      /* Remaining payload for the 2nd split */
      temp_data_len = (uint8)(ul_data1->data_len +
                      (msg_pos/8)) -
                      E_GRLC_DATA_BLOCK_LEN[ul_data1->cs];

      if( IS_PADDING_REQUIRED(ul_data1->init_mcs, ul_data1->cs) )
      {

        /* Add the TI and E bits after the padding */
        shift_in_ti_e_fields( &dst1->msg1[E_GRLC_PADDING_BYTES],
                              (uint8) ( E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] -
                                        E_GRLC_PADDING_BYTES
                                      ),
                              ul_data1->ti, ul_data1->ext
                            );
      }
      else
      {
        /* Add the TI and E bits */
        shift_in_ti_e_fields( dst1->msg1,
                              (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs],
                              ul_data1->ti, ul_data1->ext
                            );

      }

      /*---------------------------------------
      ** Construct 2nd split
      **--------------------------------------
      */
      msg_pos = 0;

      /* Copy header from the first split */
      memscpy(dst2->hdr, sizeof(dst2->hdr), dst1->hdr, E_GRLC_HDR_TYPE_MAX_LEN);

      /* Set the SPB field in the header (4th byte, 3rd and 4th bit) to indicate
      ** 2nd split
      */
      dst2->hdr[3] = ( dst2->hdr[3] & 0xf3 ) | (uint8)( SPB_RETX_PART_2 << 2 );


      /* TLLI and PFI are never included in the 2nd split. If they were included in
      ** the original MCS8 block, they are included on the 1st split block.
      ** Always set TI and PI to zero in the header of the 2nd split block.
      */

      if(ul_data1->hdr.pi)
        dst2->hdr[3] = ( dst2->hdr[3] & 0xdf );

      /* 'Strip' remaining dsm data into a straight buffer to pass to L1 and
      ** ultimately DSP. Note that the remaining data could be zero.
      */
      if( temp_data_len )
      {
        if (!gprs_pdu_strip_head(&(ul_data1->dup_dsm_ptr),
                                 &dst2->msg1[msg_pos/8], /* offset into n'th octet */
                                 temp_data_len/* length in bits */
                                )
           )
        {
          MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_850, "xlate_e_d Failed to strip payload");
        }
      }

      /* If dup_dsm_ptr is not NULL after stripping then warn of memory leak
      */
      if (ul_data1->dup_dsm_ptr != NULL)
      {
        u = (uint16)GPRS_PDU_LENGTH(ul_data1->dup_dsm_ptr);
        MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_851, "GRLU xlate_int non-null payload post-strip l=%u",u);
        GPRS_PDU_FREE(&ul_data1->dup_dsm_ptr);
      }

      msg_pos += temp_data_len * sizeof(uint8);

      /* Fill rest of the block with filling octets */
      if(temp_data_len < E_GRLC_DATA_BLOCK_LEN[ul_data1->cs])
      {
        temp_fill_len = (uint8) (E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] - temp_data_len);
        memset(&dst2->msg1[msg_pos], RM_UL_FILLER_OCTET,temp_fill_len);
      }

      /* Add the TI and E bits. As explained above, TI in the 2nd split is always '0' */
      shift_in_ti_e_fields( dst2->msg1, (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs],
                            0, ul_data1->ext
                          );

    } /* enough payload for 1st block */
  } /* IS_SPLIT_REQUIRED () */
  else
  {
    /* 'Strip' dsm data into a straight buffer to pass to L1 and ultimately DSP
    */
    if (!gprs_pdu_strip_head(&(ul_data1->dup_dsm_ptr),
                             &dst1->msg1[msg_pos/8], /* offset into n'th octet */
                             ul_data1->data_len      /* msg_pos is length in bits */
                            )
       )
    {
      MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_852, "xlate_e_d Failed to strip payload");
    }

    /* If dup_dsm_ptr is not NULL after stripping then warn of memory leak
    */
    if (ul_data1->dup_dsm_ptr != NULL)
    {
      u = (uint16)GPRS_PDU_LENGTH(ul_data1->dup_dsm_ptr);
      MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_853, "GRLU xlate_int non-null payload post-strip l=%u",u);
      GPRS_PDU_FREE(&ul_data1->dup_dsm_ptr);
    }

    /* pad the remainder of data block with RLC filler octets starting from
    ** the end of the block (at data_len) for 'rem_cnt' octets
    */
    if (ul_data1->rem_cnt)
    {
      memset(&dst1->msg1[msg_pos/8+ul_data1->data_len],
             RM_UL_FILLER_OCTET,
             ul_data1->rem_cnt
            );
    }

    if( IS_PADDING_REQUIRED(ul_data1->init_mcs, ul_data1->cs) )
    {
      /* Add the TI and E bits after the padding */
      shift_in_ti_e_fields( &dst1->msg1[E_GRLC_PADDING_BYTES],
                            (uint8) ( E_GRLC_DATA_BLOCK_LEN[ul_data1->cs] -
                                      E_GRLC_PADDING_BYTES
                                    ),
                            ul_data1->ti, ul_data1->ext
                          );
    }
    else
    {
      /* Add the TI and E bits */
      shift_in_ti_e_fields(dst1->msg1, (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data1->cs],
                           ul_data1->ti, ul_data1->ext
                          );
    }
  } /* IS_SPLIT_REQUIRED () */

  /*===============================================================================
  ** Deal with the 2nd payload for MCS-7/8/9
  **===============================================================================
  */
  if( ul_data2 != NULL &&
      ( ul_data2->cs == RM_MCS_7 ||
        ul_data2->cs == RM_MCS_8 ||
        ul_data2->cs == RM_MCS_9
      )
    )
  {
    /* Copy the data block's PS value to the PL1 block */
    dst1->ps2 = ul_data2->ps;


    /* Reset pointer position and start packing the 2nd RLC payload.
    */
    msg_pos = 0;

    /* loop to translate the extension octet array */
    for (by = 0; by < ul_data2->num_ext_octets; by++)
    {
      gpackb( ul_data2->ext_octet[by].egprs.li,
              dst1->msg2,
              &msg_pos,
              e_grlc_ext_octet_t_li_len);
      gpackb( ul_data2->ext_octet[by].egprs.ext,
              dst1->msg2,
              &msg_pos,
              e_grlc_ext_octet_t_ext_len);
    }

    /*----------------------------------------------------------------------------------
    ** If TLLI indicator is TRUE then put TLLI field such that LSBytes of the TLLI field
    ** is at M+1, and MSBytes of the TLLI field is at M+4 (high-address)
    **----------------------------------------------------------------------------------
    */
    if (ul_data2->ti == RM_TLLI_PRESENT)
    {
      dst1->msg2[msg_pos/8] = (uint8)((ul_data2->tlli) & 0xff);
      msg_pos += 8;

      dst1->msg2[msg_pos/8] = (uint8)((ul_data2->tlli >> 8) & 0xff);
      msg_pos += 8;

      dst1->msg2[msg_pos/8] = (uint8)((ul_data2->tlli >> 16) & 0xff);
      msg_pos += 8;

      dst1->msg2[msg_pos/8] = (uint8)((ul_data2->tlli >> 24) & 0xff);
      msg_pos += 8;
    }

    /*-----------------------------------------------------------------
    ** If PFI indicator is TRUE then pack PFI field and the E bit.
    **-----------------------------------------------------------------
    */
    if( ul_data2->hdr.pi == RM_PFI_PRESENT )
    {
      gpackb(ul_data2->pfi, dst1->msg2, &msg_pos, e_grlc_ul_data_block_t_pfi_len);
      gpackb(ul_data2->hdr.pi, dst1->msg2, &msg_pos, e_grlc_ul_data_block_t_pfi_e_len);
    }

    /* 'Strip' dsm data into a straight buffer to pass to L1 and ultimately DSP
    */
    if (!gprs_pdu_strip_head(&(ul_data2->dup_dsm_ptr),
                             &dst1->msg2[msg_pos/8], /* offset into n'th octet */
                             ul_data2->data_len      /* msg_pos is length in bits */
                            )
       )
    {
      MSG_GRLC_ERROR_0(GRLC_F3_MSG_ENUM_854, "xlate_e_d Failed to strip payload");
    }

    /* If dup_dsm_ptr is not NULL after stripping then warn of memory leak
    */
    if (ul_data2->dup_dsm_ptr != NULL)
    {
      u = (uint16)GPRS_PDU_LENGTH(ul_data2->dup_dsm_ptr);
      MSG_GRLC_ERROR_1(GRLC_F3_MSG_ENUM_855, "GRLU xlate_int non-null payload post-strip l=%u",u);
      GPRS_PDU_FREE(&ul_data2->dup_dsm_ptr);
    }

    /* pad the remainder of data block with RLC filler octets starting from
    ** the end of the block (at data_len) for 'rem_cnt' octets
    */
    if (ul_data2->rem_cnt)
    {
      memset(&dst1->msg2[msg_pos/8+ul_data2->data_len],
             RM_UL_FILLER_OCTET,
             ul_data2->rem_cnt
            );
    }

    /* Add the TI and E bits */
    shift_in_ti_e_fields(
                          dst1->msg2, (uint8) E_GRLC_DATA_BLOCK_LEN[ul_data2->cs],
                          ul_data2->ti, ul_data2->ext
                        );
  }
} /* xlate_int_ul_data */

#if defined (DEBUG_GSM_GPRS_RLC_TEST)|| defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST / PERLUTF */

#if defined(DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST || DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */


/*** EOF: don't remove! ***/
