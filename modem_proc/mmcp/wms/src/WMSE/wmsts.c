/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsts.c -- Translation Services

  The WMS module which implements the User API for SMS. This source file
  implements the translation functions for the client applications to call
  and for internal use.
  -------------

    Copyright (c) 2001-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsts.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wms.h"
#include "wmsts.h"
#include "qcmt.h"
#include "wmspriv.h"
#include "msg.h"
#include "bit.h"
#include "stringl.h"
#include "err.h"
#include "wmsutils.h"

#define CB_PAGE_LEN 82

#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#endif /* FEATURE_CDSMS */

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

static const uint8 ascii_to_def_alpha_table[] =
{
  0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x0A, 0x2E, 0x2E, 0x0D, 0x2E, 0x2E,
  0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
  0x20, 0x21, 0x22, 0x23, 0x02, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x00, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x28, 0x2F, 0x29, 0x2E, 0x11,
  0x2F, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x21, 0x2F, 0x29, 0x2D, 0x2E
};

static const uint8 def_alpha_to_ascii_table[] =
{
  '@',  '#',  '$',  'Y',  'e',  'e',  'u',  'i',  'o',  'C', 0x0A,  'O',  'o', 0x0D,  'A',  'a',
  'D',  '_',  '?',  '?',  'L',  'O',  'P',  '?',  'S',  'T',  '?',  ' ',  'A',  'a',  's',  'E',
  ' ',  '!', 0x22,  '#',  '?',  '%',  '&', 0x27,  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
  '@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  'A',  'O',  'N',  'U',  'S',
  '?',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  'a',  'o',  'n',  'u',  'a'
};

typedef struct
{
  uint16   unicode;
  uint8    gsm_def;
} unicode_to_gsm_def;

enum { WMS_ENHANCED_VP_OCTETS   = 7};

#define WMS_GW_USER_DATA_LEN_MAX 140

enum { WMS_UDH_OCTETS_PORT8                =2};
enum { WMS_UDH_OCTETS_PORT16               =4};
enum { WMS_UDH_OCTETS_CONCAT8              =3};
enum { WMS_UDH_OCTETS_SPECIAL_SM           =2};
enum { WMS_UDH_OCTETS_CONCAT16             =4};
enum { WMS_UDH_OCTETS_PRE_DEF              =2};
enum { WMS_UDH_OCTETS_TEXT_FORMATTING      =3};
enum { WMS_UDH_OCTETS_RFC822               =1};
enum { WMS_UDH_OCTETS_NAT_LANG_SS          =1};
enum { WMS_UDH_OCTETS_NAT_LANG_LS          =1};
enum { WMS_UDH_OCTETS_USER_PROMPT          =1};
enum { WMS_UDH_OCTETS_EO_HEADER            =7};

qc_crit_sect_type encode_decode_data_crit_sect;
boolean            encode_decode_data_crit_sect_init = FALSE;

static boolean wms_ts_udh_decode_first_seg_check
(
  const uint8               len,                /* user_data_length*/
  const uint8               *data,              /* first byte of user data */
  boolean                   *is_first_segment_ptr /* OUT */
);


void wms_check_udh_null()
{
   MSG_ERROR_0("udh is NULL");  
}


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/


/*=========================================================================
FUNCTION
  wms_ts_bcd_to_int

DESCRIPTION
  Convert a BCD to Integer.
  Valid BCD values altogether are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  Failure if either of the BCD digit is invaild (greater than 9, e.g. 11111010 = 15 and 10)
  Success if both BCD digits are valid

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_bcd_to_int
(
  const uint8 bcd, /*IN*/
  uint8 *result    /*OUT*/
)
{
  if ( (bcd & 0x0F) > 9 || ((bcd & 0xF0) >> 4) > 9)
  {
    *result = 0;
    return FALSE;
  }
  else
  {
    *result = ( (bcd & 0x0F) + (((bcd & 0xF0) >> 4) * 10) );
    return TRUE;
  }
}


/*=========================================================================
FUNCTION
  wms_ts_int_to_bcd

DESCRIPTION
  Convert an Integer to BCD.
  Valid Integer values are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_int_to_bcd
(
  const uint8 i
)
{
  return (uint8)(((i % 10) + ((i / 10) << 4)));
}

/*=========================================================================
FUNCTION
  wms_ts_ascii_to_default

DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_default
(
  const uint8   * asc,
  uint32        len,
  uint8         * def
)
{
  uint32  i, j;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; i < len; i ++ )
  {
    j = asc[i];
    def[i] = ascii_to_def_alpha_table[j];
  }

} /* wms_ts_ascii_to_default() */



/*=========================================================================
FUNCTION
  wms_ts_default_to_ascii

DESCRIPTION
  Convert 7-bit GSM Default alphabet string to an ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_default_to_ascii
(
  const uint8   * def,
  uint32        len,
  uint8         * asc
)
{
  uint32  i, j;

  for( i = 0; i < len; i ++ )
  {
    j = def[i];
    asc[i] = def_alpha_to_ascii_table[j];
  }

} /* wms_ts_default_to_ascii() */


/*=========================================================================
FUNCTION
  wms_ts_bcd_to_ascii

DESCRIPTION
  Convert BCD digits to an ASCII number string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_bcd_to_ascii
(
  const uint8         * addr,
  uint8               len,
  uint8               * out
)
{
  uint8   i;

  for( i=0; i<len; i++ )
  {
    uint8   digit; /* to store stripped digit */

    digit = addr[i] & 0x0F;

    switch( digit )
    {
      case 0xA:
        out[i] = '*';
        break;
      case 0xB:
        out[i] = '#';
        break;
      case 0xC:
      case 0xD:
      case 0xE:
        out[i] = (digit - 0xC) + 'a';
        break;
      case 0xF:
        /* Ignore it */
        out[i] = ' ';
        break;
      default:
        out[i] = digit + '0';
        break;
    }
  }

  /* done */
  return;

} /* wms_ts_bcd_to_ascii() */



/*=========================================================================
FUNCTION
  wms_ts_ascii_to_bcd

DESCRIPTION
  Convert an ASCII number string to BCD digits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_bcd
(
  const uint8     * ascii,
  uint8           len,
  uint8           * out
)
{
  uint8   i;

  for( i=0; i<len; i++ )
  {
    switch( ascii[i] )
    {
      case '*':
        out[i] = 0xA;
        break;
      case '#':
        out[i] = 0xB;
        break;
      default:
        out[i] = ascii[i] - '0';
        break;
    }
  }

  /* done */
  return;

} /* wms_ts_ascii_to_bcd() */


/*=========================================================================
FUNCTION
  wms_ts_encode_address

DESCRIPTION
  Encode address structure into format for SIM storage and for interfacing
  with lower layers.

DEPENDENCIES
  None

RETURN VALUE
  0: Failure
  Otherwise: Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_address
(
  const wms_address_s_type    * addr,
  uint8                       * data
)
{
  uint8   i, pos = 0;

  if( NULL == addr || NULL == data )
  {
    MSG_ERROR_0("NULL pointer");
    return 0;
  }

  /* Spec TS 23.040 Section 9.1.2.5 and 9.2.3.8 specify that the maximum length of the
     full address field (Address Length, Type of Address and Address Value) is 12 octets.
     The length of the destination address should be between 3 to 12 octets (20 semi-octets).
  */
  if( addr->number_of_digits > WMS_GW_ADDRESS_MAX )
  {
    MSG_ERROR_1("Addr len too long: %d", addr->number_of_digits);
    return 0;
  }

  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    data[pos] = (uint8)((addr->number_of_digits*7 + 3)/4);
  }
  else
  {
    /* Len field: */
    data[pos] = addr->number_of_digits;
  }
  pos ++;

  /* TON & NPI: */
  data[pos] = 0x80;
  data[pos] |= (uint8) ((uint8) addr->number_type << 4);
  data[pos] |= (uint8) addr->number_plan;
  pos ++;

  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    /* Alphanumberic Number Type */
    pos += (uint8)wms_ts_pack_gw_7_bit_chars
              (
                addr->digits,
                addr->number_of_digits,
                0,
                WMS_GW_ADDRESS_MAX,
                &data[pos]
              );
  }
  else
  {
    /* the digits: */
    for( i=0; i<addr->number_of_digits; i++ )
    {
      /* pack two digits each time */
      data[pos]  = (uint8) (addr->digits[i++] & 0x0F);

      if( i == addr->number_of_digits )
      {
        data[pos] |= 0xF0;
      }
      else
      {
        data[pos] |= (uint8)(addr->digits[i] << 4);
      }
      pos ++;
    }
  }
  /* done */
  return pos;
} /* wms_ts_encode_address() */

/*=========================================================================
FUNCTION
  wms_ts_encode_sc_address

DESCRIPTION
  Encode address structure into format for SIM storage and for interfacing
  with lower layers.

DEPENDENCIES
  wms_ts_encode_address

RETURN VALUE
  None.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_sc_address
(
  const wms_address_s_type    * addr,
  uint8                       * data
)
{
  uint8  pos =0;
  pos += wms_ts_encode_address( addr, data);

  data[0] = (uint8)(pos-1);

  /* Spec TS 23.040 Section 9.1.2.5 and 9.2.3.8 specify that the maximum length of the
     full address field (Address Length, Type of Address and Address Value) is 12 octets.
     The length of the destination address should be between 3 to 12 octets (20 semi-octets).
  */
  if( data[0] < 2 || data[0] > 11 )
  {
    MSG_ERROR_1("Bad SC Addr len: %d", data[0]);
    return 0;
  }

  return pos;
}
/*=========================================================================
FUNCTION
  wms_ts_decode_address

DESCRIPTION
  Decode address data into a structure.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_address
(
  const uint8               * data,
  wms_address_s_type        * addr
)
{
  uint8   i, pos = 0;

  if( NULL == data || NULL == addr )
  {
    MSG_ERROR_0("NULL pointer");
    return 0;
  }

  /* Len field: number of digits */
  i = data[pos];

  if( i > WMS_GW_ADDRESS_MAX )
  {
    /* Address is too long */
    MSG_ERROR_1("Addr len too long: %d", i);
    return 0;
  }

  addr->number_of_digits = i;

  pos ++;
  //Type-of-Address field
  /* Spec TS 23.040 Section 9.1.2.5 is 1 octet long and has the following format:
        Bit 7    : 1
		Bits 4-6 : Depict TON
		Bits 0-3 : Numbering plan
  */
  if (0 == (data[pos] & 0x80))
  {
     MSG_ERROR_0("The most significant bit in Type-of-Address field is not 1");
     return 0;
  }

  /* TON & NPI: */
  addr->digit_mode  = WMS_DIGIT_MODE_4_BIT;
  addr->number_type = (wms_number_type_e_type) (( data[pos] & 0x70 ) >> 4);
  addr->number_plan = (wms_number_plan_e_type) (data[pos] & 0x0F);
  pos ++;

  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    uint8 bytes_increment=0;
    /* Alphanumberic Number Type */
    addr->digit_mode = WMS_DIGIT_MODE_8_BIT;

    /* length = number of BCD digits */
    bytes_increment = (addr->number_of_digits+1)/2;

    addr->number_of_digits = (uint8)(addr->number_of_digits*4/7);

    (void)wms_ts_unpack_gw_7_bit_chars
           (
             &data[pos],
             addr->number_of_digits,
             WMS_GW_ADDRESS_MAX,
             0,
             addr->digits
            );

    pos += bytes_increment;
  }
  else
  {
    /* the digits: */
    for( i=0; i<addr->number_of_digits; i++ )
    {
      /* unpack two digits each time */
      addr->digits[i++] = data[pos] & 0x0F;
      addr->digits[i]   = ( data[pos] & 0xF0 ) >> 4;
      pos ++;
    }
  }
  return pos;
} /* wms_ts_decode_address() */

/*=========================================================================
FUNCTION
  wms_ts_decode_sc_address

DESCRIPTION
  Decode SC address data into a structure.

DEPENDENCIES
  wms_ts_decode_address

RETURN VALUE
0: Failure
Otherwise: bytes decoded

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_sc_address
(
  const uint8 *data,
  wms_address_s_type *addr
)
{
  uint8  buf[(WMS_GW_ADDRESS_MAX/2) + 2];
  int    length;

  if (data == NULL || addr == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_sc_address");
    return 0;
  }

  /*Check valid range for sc address, number type/plan (1 byte) and dest addr (between 1 to 10 bytes)*/
  if( data[0] < 2 || data[0] > 11 )
  {
    MSG_ERROR_1("Bad SC Addr len: %d", data[0]);
    return 0;
  }

  /*Get total number of bytes, includes 1 byte of addr_length */
  length = data[0]+1;

  (void)memscpy( buf,
                 (uint32)length,
                 (void *)(data),
                 (uint32)length );

  /*Calculate number of digits */
  buf[0] =(uint8)( 2 * (buf[0]-1));

  if( (data[data[0]] & 0xF0) == 0xF0)
  {
    buf[0] -= 1;
  }

  return wms_ts_decode_address( buf, addr );
}

/*=========================================================================
FUNCTION
  wms_ts_pack_gw_7_bit_chars

DESCRIPTION
  Pack GSM 7-bit characters into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint16 wms_ts_pack_gw_7_bit_chars
(
  const uint8     * in,
  uint16          in_len,      /* Number of 7-bit characters */
  uint16          shift,
  uint16          out_len_max, /* Number of 7-bit characters */
  uint8           * out
)
{
  uint16    i=0;
  uint16    pos = 0;

  if (in == NULL || out == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_pack_gw_7_bit_chars");
    return 0;
  }
  /* pack the ASCII characters */
  shift %= 7;

  if (shift != 0)
  {
    /* Pack the upper bytes of the first character */
    out[pos] |= (uint8) ((uint32)in[i] << shift);
    shift = (7 - shift) + 1;
    if (shift == 7)
    {
      shift = 0;
      i++;
    }
    pos++;
  }

  for( ; pos < out_len_max && i < in_len;
       pos++, i++ )
  {
    /* pack the low bits */
    out[pos] = in[i] >> shift;

    if( i+1 < in_len )
    {
      /* pack the high bits using the low bits of the next character */
      out[pos] |= (uint8) ((uint32)in[i+1] << (7-shift));

      shift ++;

      if( shift == 7 )
      {
        shift = 0;
        i ++;
      }
    }
  }

  /* done */
  return pos;

} /* wms_ts_pack_gw_7_bit_chars() */


/*=========================================================================
FUNCTION
  wms_ts_unpack_gw_7_bit_chars

DESCRIPTION
  Unpack raw data to GSM 7-bit characters.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_unpack_gw_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,        /* Number of 7-bit characters */
  uint8             out_len_max,   /* Number of maximum 7-bit characters after unpacking */
  uint16            shift,
  uint8             * out
)
{
  int      i=0;
  uint16   pos=0;

  if (in == NULL || out == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_unpack_gw_7_bit_chars");
    return 0;
  }

  /*If the number of fill bits != 0, then it would cause an additional shift*/
  if (shift != 0)
   pos = pos+1;

  if (shift ==7)
  {
    out[0] = in[0] >> 1; /*7 fillbits*/
    shift =0;            /*First Byte is a special case*/
    i=1;
  }

  for( ;
       i < out_len_max && i< in_len;
       i++, pos++ )
  {
    out[i] = (uint8)( (uint32)in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      if( i >= out_len_max )
      {
        MSG_ERROR_0("Not enough output buffer for unpacking");
        break;
      }
      out[i] = in[pos] >> 1;
    }
  }

  return(uint8)(pos);

} /* wms_ts_unpack_gw_7_bit_chars() */

/*=========================================================================
FUNCTION
  wms_ts_encode_dcs

DESCRIPTION
  Encode Data Coding Scheme into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_dcs
(
  const wms_gw_dcs_s_type   *dcs,  /* IN */
  uint8                     *data  /* OUT */
)
{
  uint8 pos = 0;
#ifdef FEATURE_GWSMS
  wms_ts_init();
  qc_enter_crit_sect(&encode_decode_data_crit_sect);

  /* TP-DCS is defined in spec 3GPP TS 23.038, section 4. */
  if( dcs->msg_waiting == WMS_GW_MSG_WAITING_NONE )
  {
    /* Using the pattern 00xx xxxx */

    /* bit 5 */
    data[pos] = dcs->is_compressed ? 0x20 : 0;

    /* bit 4 */
    data[pos] |= (dcs->msg_class != WMS_MESSAGE_CLASS_NONE ) ? 0x10 : 0;

    /* bits 3-2 */
    data[pos] |= (uint8)((int)dcs->alphabet << 2);

    /* bits 1-0 */
    data[pos] |= (uint8)((int)dcs->msg_class & 0x03);
  }
  else if (dcs->msg_waiting == WMS_GW_MSG_WAITING_NONE_1111)
  {
    /* Using the pattern 1111 xxxx */

    data[pos] = 0xf0;

    /*bit 2*/
    if (dcs->alphabet == WMS_GW_ALPHABET_8_BIT)
      data[pos] |= 0x04;


    /* bits 1-0 */
    data[pos] |= (int)dcs->msg_class & 0x03;
  }
  else
  {
    /* bits 7-4 */
    if( dcs->msg_waiting == WMS_GW_MSG_WAITING_DISCARD )
    {
      data[pos] = 0xc0;
    }
    else if( dcs->msg_waiting == WMS_GW_MSG_WAITING_STORE &&
             dcs->alphabet    == WMS_GW_ALPHABET_7_BIT_DEFAULT )
    {
      data[pos] = 0xd0;
    }
    else
    {
      data[pos] = 0xe0;
    }

    /* bit 3 */
    data[pos] |= ( dcs->msg_waiting_active == TRUE ) ? 0x08 : 0;

    /* bit 2 is reserved, set to 0 */

    /* bits 1-0 */
    data[pos] |= (int)dcs->msg_waiting_kind & 0x03;
  }

  pos ++;

  qc_leave_crit_sect(&encode_decode_data_crit_sect);
#endif /* FEATURE_GWSMS */
  return pos;
} /* wms_ts_encode_dcs() */

/*=========================================================================
FUNCTION
  wms_ts_encode_evp_rel_time

DESCRIPTION
  Encode a timestamp(hour/min/hour only) into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/

uint8 wms_ts_encode_evp_rel_time
(
  const wms_timestamp_s_type *vp,
  uint8                      *data
  )
{
  uint8 pos = 0, j;

  if (data == NULL || vp == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_encode_evp_rel_time");
    return 0;
  }

  /* hour check */
  if ( wms_ts_bcd_to_int(vp->hour, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR_1("Hour is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos++] = ((vp->hour & 0x0F) << 4) + ((vp->hour & 0xF0) >> 4);

  /* minute check */
  if ( wms_ts_bcd_to_int(vp->minute, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Minute is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos++] = ((vp->minute & 0x0F) << 4) + ((vp->minute & 0xF0) >> 4);

  /* second check */
  if ( wms_ts_bcd_to_int(vp->second, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Second is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos++] = ((vp->second & 0x0F) << 4) + ((vp->second & 0xF0) >> 4);

  return pos;
}/* wms_ts_encode_evp_rel_time */

/*=========================================================================
FUNCTION
  wms_ts_encode_timestamp

DESCRIPTION
  Encode a timestamp into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_timestamp
(
  const wms_timestamp_s_type      *timestamp, /* IN */
  uint8                           *data     /* OUT */
)
{
  sint7     i;
  uint8     pos = 0, j;

  if (data == NULL || timestamp == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_encode_timestamp");
    return 0;
  }

  /* year check */
  if ( !wms_ts_bcd_to_int(timestamp->year, &j) )
  {
    return 0;
  }
  data[pos] = ((timestamp->year & 0x0F) << 4) + ((timestamp->year& 0xF0) >> 4);
  pos ++;

  /* month check */
  if ( wms_ts_bcd_to_int(timestamp->month, &j) )
  {
    if (j > 12 || j < 1)
    {
      MSG_ERROR_1("Month is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->month & 0x0F) << 4) + ((timestamp->month & 0xF0) >> 4);
  pos ++;

  /* day check */
  if ( wms_ts_bcd_to_int(timestamp->day, &j) )
  {
    if (j > 31 || j < 1)
    {
      MSG_ERROR_1("Day is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->day & 0x0F) << 4) + ((timestamp->day & 0xF0) >> 4);
  pos ++;

  /* hour check */
  if ( wms_ts_bcd_to_int(timestamp->hour, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR_1("Hour is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->hour & 0x0F) << 4) + ((timestamp->hour & 0xF0) >> 4);
  pos ++;

  /* minute check */
  if ( wms_ts_bcd_to_int(timestamp->minute, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Minute is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->minute & 0x0F) << 4) + ((timestamp->minute & 0xF0) >> 4);
  pos ++;

  /* second check */
  if ( wms_ts_bcd_to_int(timestamp->second, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Second is invalid: %d", j);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->second & 0x0F) << 4) + ((timestamp->second & 0xF0) >> 4);
  pos ++;

  /* timezone check */
  i = (sint7)timestamp->timezone;
  // Valid range of timezone is from -12 to +14 hours
  if (i > 56 || i < -48)
  {
    MSG_ERROR_1("Timezone is out of bound: %d", (uint32) i);
    return 0;
  }

  if (i >= 0)
  {
    data[pos] = (uint8) (((uint8)( i % 10 )) << 4);
    data[pos] |= ( i / 10 );
  }
  else
  {
    i *= (-1);
    data[pos] = (uint8) (((uint8)( i % 10 )) << 4);
    data[pos] |= ( i / 10 );
    data[pos] |= 0x08;
  }
  pos ++;

  return pos;

} /* wms_ts_encode_timestamp() */

/*=========================================================================
FUNCTION
  wms_ts_encode_enhanced_vp

DESCRIPTION
  Encode a TP-VP(Enhanced format) into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_enhanced_vp
(
  const wms_enhanced_vp_s_type    *enhanced_vp, /* IN */
  uint8                           *data     /* OUT */
)
{
  uint8 pos = 0;
  uint8 i = 0;
  uint8 j = 0;

  if( NULL == enhanced_vp || NULL == data )
  {
    MSG_ERROR_0("wms_ts_encode_enhanced_vp NULL pointer");
    return 0;
  }
  memset(data, 0, WMS_ENHANCED_VP_OCTETS);

  data[pos]  = enhanced_vp->with_extension ? 0x80 : 0;
  data[pos] |= enhanced_vp->single_shot ? 0x40 : 0;
  data[pos] |= (uint8)((int)enhanced_vp->vp_format & 0x07);

  pos ++;

  if( TRUE == enhanced_vp->with_extension )
  {
    data[pos++] = enhanced_vp->extension;
  }
  switch( enhanced_vp->vp_format )
  {
    case WMS_ENHANCED_VP_NONE:
      break;

    case WMS_ENHANCED_VP_RELATIVE:
      data[pos] = wms_ts_encode_relative_time(&(enhanced_vp->time));
      pos++;
      break;

    case WMS_ENHANCED_VP_RELATIVE_INT:
      if( !wms_ts_bcd_to_int(enhanced_vp->time.minute, &j ))
      {
        MSG_ERROR_0("Minute is invalid");
      }
      data[pos] = (uint8)(j * 60);
      if( !wms_ts_bcd_to_int(enhanced_vp->time.second, &j ))
      {
        MSG_ERROR_0("Second is invalid");
      }
      data[pos] += j;

      /* Validity period is relative in integer representation and the
        following octet contains the TP-VP value in the range 0 to 255
        representing 0 to 255 seconds. A TP-VP value of zero is
        undefined and reserved for future use.
        */
      pos++;
      break;

    case WMS_ENHANCED_VP_SEMI_OCTET:
      i = wms_ts_encode_evp_rel_time(&enhanced_vp->time, data+pos);
      if( i == 0 )
      {
        MSG_ERROR_0("Error while encoding Enhanced VP relative time");
      }
      pos += i;
      break;

    default:
      break;
  }

  return WMS_ENHANCED_VP_OCTETS;
}/* wms_ts_encode_enhanced_vp */

static int wms_ts_encode_udh_concat_8
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos =0;

  if ( const_header->u.concat_8.total_sm == 0 ||
       const_header->u.concat_8.seq_num  == 0 ||
       const_header->u.concat_8.seq_num > const_header->u.concat_8.total_sm)

  {
    MSG_ERROR_1("SMS UDH Header id %d Present with no Data", const_header->header_id);
    return 0;
  }


  udh[pos++] = (uint8) WMS_UDH_CONCAT_8;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_CONCAT8;
  udh[pos++] = const_header->u.concat_8.msg_ref;
  udh[pos++] = const_header->u.concat_8.total_sm;
  udh[pos++] = const_header->u.concat_8.seq_num;

  return pos;
}/*wms_ts_encode_udh_concat_8*/

static int wms_ts_encode_udh_concat16
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  if (const_header->u.concat_16.total_sm == 0 ||
      const_header->u.concat_16.seq_num == 0 ||
      const_header->u.concat_16.seq_num > const_header->u.concat_16.total_sm)
  {
    MSG_ERROR_1("SMS UDH Header id %d Present with no Data", const_header->header_id);
    return 0;
  }

  udh[pos++] = (uint8) WMS_UDH_CONCAT_16;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_CONCAT16;
  udh[pos++] = (uint8)((const_header->u.concat_16.msg_ref & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.concat_16.msg_ref & 0x00FF);
  udh[pos++] = const_header->u.concat_16.total_sm;
  udh[pos++] = const_header->u.concat_16.seq_num;

  return pos;
}/*wms_ts_encode_udh_concat16*/


int wms_ts_encode_udh_special_sm
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;
  udh[pos++] = (uint8) WMS_UDH_SPECIAL_SM;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_SPECIAL_SM;

  /* store or discard the message */
  udh[pos] = (const_header->u.special_sm.msg_waiting == WMS_GW_MSG_WAITING_STORE) ? 0x80 : 0;
  if( const_header->u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VIDEOMAIL )
  {
    udh[pos] |= WMS_SP_EXTD_VIDEOMAIL_MSG_WAITING_TYPE;
  }
  else
  {
    udh[pos] |= (uint8) const_header->u.special_sm.msg_waiting_kind;
  }
  pos++;
  udh[pos++] = const_header->u.special_sm.message_count;

  return pos;
}/*wms_ts_encode_udh_special_sm*/

static int wms_ts_encode_udh_port_8
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos =0;

  udh[pos++] = (uint8) WMS_UDH_PORT_8;
  udh[pos++] = WMS_UDH_OCTETS_PORT8;
  udh[pos++] = const_header->u.wap_8.dest_port;
  udh[pos++] = const_header->u.wap_8.orig_port;

  return pos;
}/*wms_ts_encode_udh_port_8*/

static int wms_ts_encode_udh_port16
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos =0;

  udh[pos++] = (uint8) WMS_UDH_PORT_16;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PORT16;
  udh[pos++] = (uint8)((const_header->u.wap_16.dest_port & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.wap_16.dest_port & 0x00FF);
  udh[pos++] = (uint8)((const_header->u.wap_16.orig_port & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.wap_16.orig_port & 0x00FF);

  return pos;

}/*wms_ts_encode_udh_port16*/

int wms_ts_encode_udh_text_formatting
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_TEXT_FORMATING;

  if(const_header->u.text_formating.is_color_present)
  {
    udh[pos++] = (uint8) WMS_UDH_OCTETS_TEXT_FORMATTING + 1;
  }
  else
  {
    udh[pos++] = (uint8) WMS_UDH_OCTETS_TEXT_FORMATTING;
  }

  udh[pos++] = const_header->u.text_formating.start_position;
  udh[pos++] = const_header->u.text_formating.text_formatting_length;
  udh[pos]   = (uint8) const_header->u.text_formating.alignment_type;
  /*bit 1 and  bit 0*/
  udh[pos]   = (uint8) (((uint8)const_header->u.text_formating.font_size <<2) | udh[pos]);
  /*bit 3 and  bit 2*/
  udh[pos]   = (uint8)(const_header->u.text_formating.style_bold <<4)         | udh[pos];
  /*bit 4 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_italic <<5)       | udh[pos];
  /*bit 5 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_underlined<<6)    | udh[pos];
  /*bit 6 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_strikethrough<<7) | udh[pos];
  pos++;
  /*bit 7 */

  if(const_header->u.text_formating.is_color_present)
  {
    udh[pos] = 0;
    udh[pos] = (uint8) (const_header->u.text_formating.text_color_foreground)| udh[pos];
    /* bit 0-3 */
    udh[pos] = (uint8) ((uint8) const_header->u.text_formating.text_color_background<<4 | udh[pos]);
    /* bit 4-7 */
    pos++;
  }



  return pos;
}/*wms_ts_encode_udh_text_formatting*/

int wms_ts_encode_udh_pre_def_sound
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_PRE_DEF_SOUND;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PRE_DEF;
  udh[pos++] = const_header->u.pre_def_sound.position;
  udh[pos++] = const_header->u.pre_def_sound.snd_number;

  return pos;
}/*wms_ts_encode_udh_pre_def_sound*/

int wms_ts_encode_udh_user_def_sound
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j;

  if (const_header->u.user_def_sound.data_length == 0)
  {
     MSG_ERROR_1("SMS UDH Header id %d Present with no Data", const_header->header_id);
  }


  udh[pos++] = (uint8) WMS_UDH_USER_DEF_SOUND;
  udh[pos++] = const_header->u.user_def_sound.data_length+1;
  udh[pos++] = const_header->u.user_def_sound.position;

  for (j=0;j<const_header->u.user_def_sound.data_length && j < WMS_UDH_MAX_SND_SIZE;j++)
      udh[pos++] = const_header->u.user_def_sound.user_def_sound[j];

  return pos;
}/*wms_ts_encode_udh_user_def_sound*/


int wms_ts_encode_udh_pre_def_anim
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_PRE_DEF_ANIM;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PRE_DEF;
  udh[pos++] = const_header->u.pre_def_anim.position;
  udh[pos++] = const_header->u.pre_def_anim.animation_number;

  return pos;
}/*wms_ts_encode_udh_pre_def_anim*/

int wms_ts_encode_udh_large_anim
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j, k;

  udh[pos++] = (uint8) WMS_UDH_LARGE_ANIM;
  udh[pos++] = (uint8) WMS_UDH_LARGE_BITMAP_SIZE * (uint8) WMS_UDH_ANIM_NUM_BITMAPS  + 1;
  udh[pos++] = const_header->u.large_anim.position;

  for (j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_LARGE_BITMAP_SIZE;k++)
      udh[pos++] = const_header->u.large_anim.data[j][k];

  return pos;
}/*wms_ts_encode_udh_large_anim*/


int wms_ts_encode_udh_small_anim
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j, k;

  udh[pos++] = (uint8) WMS_UDH_SMALL_ANIM;
  udh[pos++] = (uint8) (WMS_UDH_SMALL_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS) + 1;
  udh[pos++] = const_header->u.small_anim.position;


  for (j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_SMALL_BITMAP_SIZE;k++)
      udh[pos++] = const_header->u.small_anim.data[j][k];

  return pos;
}/*wms_ts_encode_udh_small_anim*/

int wms_ts_encode_udh_large_picture
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j;

  udh[pos++] = (uint8) WMS_UDH_LARGE_PICTURE;
  udh[pos++] = (uint8) WMS_UDH_LARGE_PIC_SIZE +1;
  udh[pos++] = const_header->u.large_picture.position;

  for (j=0;j<WMS_UDH_LARGE_PIC_SIZE;j++)
    udh[pos++] = const_header->u.large_picture.data[j];

  return pos;
}/*wms_ts_encode_udh_large_picture*/


int wms_ts_encode_udh_small_picture
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j;

  udh[pos++] = (uint8) WMS_UDH_SMALL_PICTURE;
  udh[pos++] = (uint8) WMS_UDH_SMALL_PIC_SIZE +1;
  udh[pos++] = const_header->u.small_picture.position;

  for (j=0;j<WMS_UDH_SMALL_PIC_SIZE;j++)
    udh[pos++] = const_header->u.small_picture.data[j];

  return pos;
}/*wms_ts_encode_udh_small_picture*/



int wms_ts_encode_udh_var_picture
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0, j;

  udh[pos++] = (uint8) WMS_UDH_VAR_PICTURE;
  udh[pos++] = (uint8)((const_header->u.var_picture.height * const_header->u.var_picture.width/8) + 3);
  udh[pos++] = const_header->u.var_picture.position;
  udh[pos++] = const_header->u.var_picture.width/8;
  udh[pos++] = const_header->u.var_picture.height;

  for (j=0;j<(const_header->u.var_picture.height * const_header->u.var_picture.width/8);j++)
    udh[pos++] = const_header->u.var_picture.data[j];


  return pos;
}/*wms_ts_encode_udh_var_picture*/


int wms_ts_encode_udh_user_prompt
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_USER_PROMPT;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_USER_PROMPT; /* only 1 byte for the udh data */
  udh[pos++] = const_header->u.user_prompt.number_of_objects;

  return pos;
} /* wms_ts_encode_udh_user_prompt() */


int wms_ts_encode_udh_eo
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int    pos = 0;
  uint8  udh_length;


  udh[pos++] = (uint8) WMS_UDH_EXTENDED_OBJECT;

  /* Pack UDH length */
  if( const_header->u.eo.first_segment == TRUE )
  {
    udh_length = WMS_UDH_OCTETS_EO_HEADER + const_header->u.eo.content.length;
  }
  else
  {
    udh_length = const_header->u.eo.content.length;
  }
  if( udh_length > WMS_UDH_EO_DATA_SEGMENT_MAX )
  {
    MSG_ERROR_1("UDH EO data too long: %d", udh_length);
    return 0;
  }

  udh[pos++] = udh_length;

  /* Pack EO header for the first segment */
  if( const_header->u.eo.first_segment == TRUE )
  {
    udh[pos++] = const_header->u.eo.reference;
    udh[pos++] = const_header->u.eo.length >> 8;
    udh[pos++] = const_header->u.eo.length & 0xFF;
    udh[pos++] = const_header->u.eo.control;
    udh[pos++] = (uint8) const_header->u.eo.type;
    udh[pos++] = const_header->u.eo.position >> 8;
    udh[pos++] = const_header->u.eo.position & 0xFF;
  }

  /* Pack content data */
  (void)memscpy( udh + pos,
                 const_header->u.eo.content.length,
                 const_header->u.eo.content.data,
                 const_header->u.eo.content.length );
  pos += const_header->u.eo.content.length;

  return pos;

} /* wms_ts_encode_udh_eo() */

int wms_ts_encode_udh_rfc822
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_RFC822;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_RFC822;
  udh[pos++] = const_header->u.rfc822.header_length;

  return pos;
}/*wms_ts_encode_udh_rfc822*/

int wms_ts_encode_udh_nat_lang_ss
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_NAT_LANG_SS;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_NAT_LANG_SS;
  udh[pos++] = (uint8) const_header->u.nat_lang_ss.nat_lang_id;

  return pos;
}/*wms_ts_encode_udh_nat_lang_ss*/

int wms_ts_encode_udh_nat_lang_ls
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_NAT_LANG_LS;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_NAT_LANG_LS;
  udh[pos++] = (uint8) const_header->u.nat_lang_ls.nat_lang_id;

  return pos;
}/*wms_ts_encode_udh_nat_lang_ls*/


int wms_ts_encode_udh_other
(
  const wms_udh_s_type *const_header,
  uint8 *udh
)
{
  int i=0;
  int pos =0;

  udh[pos++] = (uint8) const_header->u.other.header_id;
  udh[pos++] = const_header->u.other.header_length;

  for(i = 0; i< const_header->u.other.header_length;i++)
  {
    udh[pos++] = const_header->u.other.data[i];
  }

  return pos;
}

/*=========================================================================
FUNCTION
  wms_ts_encode_user_data_header

DESCRIPTION
  Encode User Data structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_user_data_header
(
  uint8                           num_headers, /* IN */
  const wms_udh_s_type            * headers,   /* IN */
  uint8                           *data        /* OUT */
)
{
   int i;
   int pos = 0;

   if (num_headers == 0)
   {
     return 0;
   }

   pos++; /*Fill the user data header length later*/

   for (i=0;i<WMS_MAX_UD_HEADERS && i< num_headers;i++)
   {
     const wms_udh_s_type *udh_hdr_ptr = &headers[i];

     switch (udh_hdr_ptr->header_id)
     {
       case WMS_UDH_CONCAT_8:
         pos += wms_ts_encode_udh_concat_8(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_CONCAT_16:
         pos += wms_ts_encode_udh_concat16(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_SPECIAL_SM:
         pos += wms_ts_encode_udh_special_sm(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_PORT_8:
         pos += wms_ts_encode_udh_port_8(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_PORT_16:
         pos += wms_ts_encode_udh_port16(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_TEXT_FORMATING:
         pos += wms_ts_encode_udh_text_formatting(udh_hdr_ptr, data+pos);
         break;


       case WMS_UDH_PRE_DEF_SOUND:
         pos += wms_ts_encode_udh_pre_def_sound(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_USER_DEF_SOUND:
         pos += wms_ts_encode_udh_user_def_sound(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_PRE_DEF_ANIM:
         pos += wms_ts_encode_udh_pre_def_anim(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_LARGE_ANIM:
         pos += wms_ts_encode_udh_large_anim(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_SMALL_ANIM:
         pos += wms_ts_encode_udh_small_anim(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_LARGE_PICTURE:
         pos += wms_ts_encode_udh_large_picture(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_SMALL_PICTURE:
         pos += wms_ts_encode_udh_small_picture(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_VAR_PICTURE:
         pos += wms_ts_encode_udh_var_picture(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_USER_PROMPT:
         pos += wms_ts_encode_udh_user_prompt(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_EXTENDED_OBJECT:
         pos += wms_ts_encode_udh_eo(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_RFC822:
         pos += wms_ts_encode_udh_rfc822(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_NAT_LANG_SS:
         pos += wms_ts_encode_udh_nat_lang_ss(udh_hdr_ptr, data+pos);
         break;

       case WMS_UDH_NAT_LANG_LS:
         pos += wms_ts_encode_udh_nat_lang_ls(udh_hdr_ptr, data+pos);
         break;

       default:
         pos += wms_ts_encode_udh_other(udh_hdr_ptr, data+pos);

     }

   }
   data[0] = (uint8)(pos-1); /*User Data Header Length*/
   return ((uint8)(pos-1));

} /*wms_ts_encode_user_data_header*/

#ifdef FEATURE_GWSMS
/*=========================================================================
FUNCTION
  wms_ts_encode_gw_user_data

DESCRIPTION
  Encode User Data structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_gw_user_data
(
  const wms_gw_dcs_s_type         *dcs,
  const wms_gw_user_data_s_type   *user_data,
  uint8                           *data
)
{
  uint16 i, pos=0;
  uint8 fill_bits = 0;
  uint16 total_bits_occupied=0;

  uint8 user_data_header_length;
  uint16 user_data_length; /*User Data Length with the header*/

  if (WMS_MAX_LEN < user_data->sm_len)
  {
    MSG_ERROR_0("Invalid user data");
    return (uint8)pos;
  }

  data[pos] = (uint8)user_data->sm_len; /*User Data length*/
  pos++;
  if( dcs->alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT )
  {
    /* pack GSM default characters */
    if (user_data->num_headers > 0)
    {
      /* Check for Memory Corruption - data[WMS_MAX_LEN] */
      if (wms_ts_compute_user_data_header_length(user_data->num_headers, user_data->headers) <= WMS_SMS_UDL_MAX_8_BIT)
      {

        user_data_header_length = wms_ts_encode_user_data_header( user_data->num_headers,
                                              user_data->headers,
                                              data+pos);

        pos += user_data_header_length + 1;

        total_bits_occupied = (user_data_header_length + 1) * 8;
        fill_bits = (total_bits_occupied % 7);    /* fill_bits vary from 0 to 6 */

        if (fill_bits != 0)
        {
          fill_bits = 7 - fill_bits;
        }

        user_data_length = (uint16)((total_bits_occupied + fill_bits + (user_data->sm_len * 7)) / 7);

        data[0] = (uint8)user_data_length;   /* UDL */
        data[1] = user_data_header_length;   /* UDHL */
      }
      else
      {
        MSG_ERROR_0("Encode User Data Header Exceeds Capacity - Skipping UDH");
      }
    }

    i = wms_ts_pack_gw_7_bit_chars( user_data->sm_data,
                                    user_data->sm_len,
                                    fill_bits,
                                   (uint16) (WMS_MAX_LEN - pos),
                                    & data[pos] );
    pos += i;
  }
  else
  {
    if (user_data->num_headers > 0)
    {
      /* Check for Memory Corruption - data[WMS_MAX_LEN] */
      if (wms_ts_compute_user_data_header_length(user_data->num_headers, user_data->headers) <= WMS_SMS_UDL_MAX_8_BIT)
      {
        user_data_header_length =
              wms_ts_encode_user_data_header(user_data->num_headers,
                                             user_data->headers,
                                             data+pos);

        /*TP-UDL is modified to actual user data + udhl */
        data[0] = (uint8)(user_data->sm_len + user_data_header_length +1);
        pos += user_data_header_length+1;
      }
      else
      {
        MSG_ERROR_0("Encode User Data Header Exceeds Capacity - Skipping UDH");
      }
    }

    (void)memscpy( &data[pos],
                   user_data->sm_len,
                   user_data->sm_data,
                   user_data->sm_len );
    pos += user_data->sm_len;
  }

  return (uint8)pos;

} /* wms_ts_encode_gw_user_data() */


/*=========================================================================
FUNCTION
  wms_ts_encode_gw_validity

DESCRIPTION
  Encode Validity structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_gw_validity
(
  const wms_gw_validity_s_type      *validity,
  uint8                               *data
)
{
  uint8 i, pos = 0;

  if( NULL == validity || NULL == data )
  {
    MSG_ERROR_0("wms_gs_encode_gw_validity NULL pointer");
    return 0;
  }
  switch( validity->format )
  {
    case WMS_GW_VALIDITY_NONE:
      break;

    case WMS_GW_VALIDITY_RELATIVE:
      data[pos] = wms_ts_encode_relative_time( & validity->u.time );
      pos ++;
      break;

    case WMS_GW_VALIDITY_ABSOLUTE:
      i = wms_ts_encode_timestamp( & validity->u.time, data+pos );
      if( i == 0 )
      {
        MSG_ERROR_0("Error while Decoding Absolute Validity Timestamp");
      }
      pos += i;
      break;

    case WMS_GW_VALIDITY_ENHANCED:
      i = wms_ts_encode_enhanced_vp(& validity->u.enhanced_vp, data+pos);
      if(  i == 0 )
      {
        MSG_ERROR_0("Error while Encoding Enhanced Validity Period");
      }
      pos += i;
      break;

    default:
      break;
  } /* switch */

  return pos;

} /* wms_ts_encode_gw_validity() */


/*=========================================================================
FUNCTION
  wms_ts_encode_deliver

DESCRIPTION
  Encode Deliver TPDU into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Encoding status.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_deliver
(
  wms_gw_tpdu_type_e_type       tpdu_type,
  const wms_gw_deliver_s_type   *deliver,
  wms_raw_ts_data_s_type        *raw_ts_data_ptr
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint8                *data;
  uint8                 pos = 0, i;

  if( deliver == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP-MMS, TP-SRI, TP_UDHI, TP-RP:
  */
  if(WMS_TPDU_DELIVER == tpdu_type)
  {
     data[pos] = 0x00;
  }
  else if (WMS_TPDU_RESERVED == tpdu_type)
  {
     data[pos] = 0x03;
  }
  else
  {
     MSG_ERROR_1("Invalid TPDU Type = %d", tpdu_type);
     return WMS_INVALID_TPDU_TYPE_S;
  }

  data[pos] |= deliver->more ? 0 : 0x04; /* bit 2 */
  /* bits 3, 4 are not used */
  data[pos] |= deliver->status_report_enabled ? 0x20 : 0; /* bit 5 */
  data[pos] |= deliver->user_data_header_present ? 0x40 : 0; /* bit 6 */
  data[pos] |= deliver->reply_path_present ? 0x80 : 0;    /* bit 7 */
  pos ++;

  /* TP-OA
  */
  i = wms_ts_encode_address( & deliver->address, & data[pos] );
  if( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;

  /* TP-PID
  */
  data[pos] = (uint8)deliver->pid;
  pos ++;

  /* TP-DCS
  */
  pos += wms_ts_encode_dcs( & deliver->dcs, data+pos );

  /* TP-SCTS
  */
  i = wms_ts_encode_timestamp( & deliver->timestamp, data+pos );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-UDL
  */
 /* data[pos] = (uint8)deliver->user_data.sm_len;
    pos ++;*/

 /*TP_UDL is filled in encode_gw_user_data function*/
  /* TP-UD
  */
  if (wms_ts_compute_gw_user_data_length( &deliver->dcs, &deliver->user_data) > WMS_SMS_UDL_MAX_8_BIT)
  {
    MSG_ERROR_0("User Data Length has exceeded capacity");
    st = WMS_INVALID_USER_DATA_SIZE_S;
  }
  else
  {
    i = wms_ts_encode_gw_user_data( & deliver->dcs,
                                    & deliver->user_data,
                                      data+pos );
    pos += i;
  }

  raw_ts_data_ptr->tpdu_type  = tpdu_type;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_deliver() */


/*=========================================================================
FUNCTION
  wms_ts_encode_deliver_report_ack

DESCRIPTION
  Encode Deliver-Report-Ack structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_deliver_report_ack
(
  const wms_gw_deliver_report_ack_s_type    *deliver_report_ack,
  wms_raw_ts_data_s_type                    *raw_ts_data_ptr
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint8                 *data;
  uint8                 i, pos = 0;

  if( deliver_report_ack == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x00; /* DELIVER-REPORT: bits 0, 1 */
  data[pos] |= deliver_report_ack->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;


  /* TP-PI
  */
  data[pos] = (uint8) deliver_report_ack->mask;
  pos ++;

  /* TP-PID
  */
  if( deliver_report_ack->mask & WMS_TPDU_MASK_PID )
  {
    data[pos] = (uint8)deliver_report_ack->pid;
    pos ++;
  }

  /* TP-DCS
  */
  if( deliver_report_ack->mask & WMS_TPDU_MASK_DCS )
  {
    pos += wms_ts_encode_dcs( & deliver_report_ack->dcs, data+pos );
  }

  if( deliver_report_ack->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /* TP-UDL
    */
    /*data[pos] =(uint8) deliver_report_ack->user_data.sm_len;
      pos ++;*/

    /*TP_UDL is filled in encode_gw_user_data function*/

    /* TP-UD
    */
    if (wms_ts_compute_gw_user_data_length( &deliver_report_ack->dcs, &deliver_report_ack->user_data) > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_0("User Data Length has exceeded capacity");
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }
    else
    {
      i = wms_ts_encode_gw_user_data( & deliver_report_ack->dcs,
                                      & deliver_report_ack->user_data,
                                        data+pos );
      pos += i;
    }
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_DELIVER_REPORT_ACK;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_deliver_report_ack() */


/*=========================================================================
FUNCTION
  wms_ts_encode_deliver_report_error

DESCRIPTION
  Encode Deliver-Report-Error structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_deliver_report_error
(
  const wms_gw_deliver_report_error_s_type    *deliver_report_error,
  wms_raw_ts_data_s_type                      *raw_ts_data_ptr
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint8                 *data;
  uint8                 i, pos = 0;

  if( deliver_report_error == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x00; /* DELIVER-REPORT: bits 0, 1 */
  data[pos] |= deliver_report_error->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;

  /* TP-FCS
  */
  data[pos] = (uint8)deliver_report_error->tp_cause;
  pos ++;

  /* TP-PI
  */
  data[pos] = (uint8) deliver_report_error->mask;
  pos ++;

  /* TP-PID
  */
  if( deliver_report_error->mask & WMS_TPDU_MASK_PID )
  {
    data[pos] = (uint8)deliver_report_error->pid;
    pos ++;
  }

  /* TP-DCS
  */
  if( deliver_report_error->mask & WMS_TPDU_MASK_DCS )
  {
    pos += wms_ts_encode_dcs( & deliver_report_error->dcs, data+pos );
  }

  if( deliver_report_error->mask & WMS_TPDU_MASK_USER_DATA)
  {
    /* TP-UDL
    */
    /*data[pos] =(uint8) deliver_report_error->user_data.sm_len;
      pos ++;*/

    /*TP_UDL is filled in encode_gw_user_data function*/

    /* TP-UD
    */
    if (wms_ts_compute_gw_user_data_length( &deliver_report_error->dcs, &deliver_report_error->user_data) > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_0("User Data Length has exceeded capacity");
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }
    else
    {
      i = wms_ts_encode_gw_user_data( & deliver_report_error->dcs,
                                      & deliver_report_error->user_data,
                                        data+pos );
      pos += i;
    }
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_DELIVER_REPORT_ERROR;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_deliver_report_error() */

/*=========================================================================
FUNCTION
  wms_ts_encode_status_report

DESCRIPTION
  Encode Status Report TPDU structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_status_report
(
  const wms_gw_status_report_s_type    *status_report,
  wms_raw_ts_data_s_type               *raw_ts_data_ptr
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint8                 *data;
  uint8                 pos = 0, i;

  if( status_report == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x02; /* Status Report bits 0, 1 */
  data[pos] |= status_report->more ? 0 : 0x04;/*bit 2*/
  data[pos] |= status_report->status_report_qualifier ? 0x20 : 0; /*bit 5*/
  data[pos] |= status_report->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;

  /* TP-MR
  */
  data[pos] = (uint8) status_report->message_reference;
  pos ++;

  /* TP-RA
  */
  i = wms_ts_encode_address( & status_report->address, & data[pos] );
  if( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;


  /* TP-SCTS
  */
  i = wms_ts_encode_timestamp( & status_report->timestamp, data+pos );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-DT*/
  i = wms_ts_encode_timestamp( & status_report->discharge_time, data+pos );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /*TP-ST*/
  data[pos] = (uint8)status_report->tp_status;
  pos++;

  if( status_report->mask > 0)
  {
     /*TP-PI*/
     data[pos] = (uint8) status_report->mask;
     pos ++;

     /*TP-PID*/
     if( status_report->mask & WMS_TPDU_MASK_PID )
     {
       data[pos] = (uint8)status_report->pid;
       pos ++;
     }

     /* TP-DCS
     */
     if( status_report->mask & WMS_TPDU_MASK_DCS )
     {
       pos += wms_ts_encode_dcs( & status_report->dcs, data+pos );
     }

     if( status_report->mask & WMS_TPDU_MASK_USER_DATA)
     {
       /* TP-UDL
       */
       /*data[pos] =(uint8) deliver_report_error->user_data.sm_len;
         pos ++;*/

       /*TP_UDL is filled in encode_gw_user_data function*/

       /* TP-UD
       */
       if (wms_ts_compute_gw_user_data_length( &status_report->dcs, &status_report->user_data) > WMS_SMS_UDL_MAX_8_BIT)
       {
         MSG_ERROR_0("User Data Length has exceeded capacity");
         st = WMS_INVALID_USER_DATA_SIZE_S;
       }
       else
       {
         i = wms_ts_encode_gw_user_data( & status_report->dcs,
                                         & status_report->user_data,
                                           data+pos );
         pos += i;
       }
     }
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_STATUS_REPORT;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_status_report() */

/*=========================================================================
FUNCTION
  wms_ts_encode_command

DESCRIPTION
  Encode Command TPDU structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_command
(
  const wms_gw_command_s_type          *command,
  wms_raw_ts_data_s_type               *raw_ts_data_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  uint8                 *data;
  uint8                 pos = 0, i;

  if( command == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x2; /* SMS Command bits 0, 1 */
  data[pos] |= command->status_report_enabled ? 0x20 :0; /*bit 5*/
  data[pos] |= command->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;

  /* TP-MR
  */
  data[pos] = (uint8) command->message_reference;
  pos ++;

  /*TP-PID
  */
  data[pos] = (uint8)command->pid;
  pos ++;

  /*TP-CT
  */
  data[pos] = (uint8)command->command;
  pos ++;

  /*TP-MN
  */
  data[pos] = (uint8) command->message_number;
  pos ++;

  /* TP-DA
  */
  i = wms_ts_encode_address( & command->address, & data[pos] );
  if( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;


  /* TP-CDL
  */
  /* Guard against Buffer Overflow */
  if (WMS_GW_COMMAND_DATA_MAX < command->command_data_len)
  {
     MSG_ERROR_1("Command Data Length %d Exceeds Command Data Buffer", command->command_data_len);
     data[pos] = WMS_GW_COMMAND_DATA_MAX;
  }
  else
  {
     data[pos] = command->command_data_len;
  }

  pos ++;

  /*TP_CD
  */
  (void)memscpy( data + pos,
                 MIN(data[pos - 1], WMS_MAX_LEN - pos),
                 command->command_data,
                 MIN(data[pos - 1], WMS_MAX_LEN - pos));

  pos += command->command_data_len;

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_COMMAND;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_command() */

/*=========================================================================
FUNCTION
  wms_ts_encode_submit

DESCRIPTION
  Encode Submit TPDU structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_submit
(
  const wms_gw_submit_s_type    *submit,
  wms_raw_ts_data_s_type        *raw_ts_data_ptr
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint8                *data;
  uint8                 pos = 0, i;

  if( submit == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP-RD, TP-VPF, TP-SRR, TP_UDHI, TP-RP:
  */
  data[pos] = 0x01; /* SUBMIT: bits 0, 1 */
  data[pos] |= submit->reject_duplicates ? 0x04 : 0; /* bit 2 */

  if ((int)submit->validity.format > 3 )
  {
    return st = WMS_INVALID_VALIDITY_FORMAT_S;
  }
  data[pos] |= (uint8)((uint32)submit->validity.format << 3);           /* bits 3, 4 */

  data[pos] |= submit->status_report_enabled ? 0x20 : 0;    /* bit 5 */
  data[pos] |= submit->user_data_header_present ? 0x40 : 0; /* bit 6 */
  data[pos] |= submit->reply_path_present ? 0x80 : 0;       /* bit 7 */
  pos ++;

  /* TP-MR
  */
  data[pos] = (uint8) submit->message_reference;
  pos ++;

  /* TP-DA
  */
  i = wms_ts_encode_address( & submit->address, & data[pos] );
  if( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;


  /* TP-PID
  */
  data[pos] = (uint8)submit->pid;
  pos ++;

  /* TP-DCS
  */
  pos += wms_ts_encode_dcs( & submit->dcs, data+pos );

  /* TP-VP
  */
  pos += wms_ts_encode_gw_validity( & submit->validity, data+pos );

  /* TP-UDL
  */
  /*data[pos] =(uint8) submit->user_data.sm_len;
    pos ++;*/

  /*TP_UDL is filled in encode_gw_user_data function*/

  /* TP-UD
  */

  if (wms_ts_compute_gw_user_data_length( &submit->dcs, &submit->user_data) > WMS_SMS_UDL_MAX_8_BIT)
  {
    MSG_ERROR_0("User Data Length has exceeded capacity");
    st = WMS_INVALID_USER_DATA_SIZE_S;
  }
  else
  {
    i = wms_ts_encode_gw_user_data( & submit->dcs,
                                    & submit->user_data,
                                      data+pos );
    pos += i;
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_SUBMIT;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_submit() */


/*=========================================================================
FUNCTION
  wms_ts_encode_submit_report_ack

DESCRIPTION
  Encode Submit-Report-Ack structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_submit_report_ack
(
  const wms_gw_submit_report_ack_s_type     *submit_report_ack,
  wms_raw_ts_data_s_type                    *raw_ts_data_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  uint8                 *data;
  uint8                 pos = 0, i;

  if( submit_report_ack == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x01; /* SUBMIT-REPORT: bits 0, 1 */
  data[pos] |= submit_report_ack->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;

  /* TP-PI
  */
  data[pos] = (uint8) submit_report_ack->mask;
  pos ++;

  /* TP-SCTS
  */
  i = wms_ts_encode_timestamp( & submit_report_ack->timestamp, data+pos );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-PID
  */
  if( submit_report_ack->mask & WMS_TPDU_MASK_PID )
  {
    data[pos] = (uint8)submit_report_ack->pid;
    pos ++;
  }

  /* TP-DCS
  */
  if( submit_report_ack->mask & WMS_TPDU_MASK_DCS )
  {
    pos += wms_ts_encode_dcs( & submit_report_ack->dcs, data+pos );
  }

  /* TP-UDL
  */
  if( submit_report_ack->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /*data[pos] =(uint8) submit_report_ack->user_data.sm_len;
      pos ++;*/

    /*TP_UDL is filled in encode_gw_user_data function*/
    /* TP-UD
    */
    if (wms_ts_compute_gw_user_data_length( &submit_report_ack->dcs, &submit_report_ack->user_data) > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_0("User Data Length has exceeded capacity");
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }
    else
    {
      i = wms_ts_encode_gw_user_data( & submit_report_ack->dcs,
                                      & submit_report_ack->user_data,
                                        data+pos );
      pos += i;
    }
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_SUBMIT_REPORT_ACK;
  raw_ts_data_ptr->len        = pos;


  return st;

} /* wms_ts_encode_submit_report_ack() */


/*=========================================================================
FUNCTION
  wms_ts_encode_submit_report_error

DESCRIPTION
  Encode Submit-Report-Error structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode_submit_report_error
(
  const wms_gw_submit_report_error_s_type     *submit_report_error,
  wms_raw_ts_data_s_type                      *raw_ts_data_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  uint8                 *data;
  uint8                 pos = 0, i;

  if( submit_report_error == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* Setting Data to 0 */
  (void)memset(data, 0, WMS_MAX_LEN);

  /* TP-MTI, TP_UDHI:
  */
  data[pos] = 0x01; /* SUBMIT-REPORT */
  data[pos] |= submit_report_error->user_data_header_present ? 0x40 : 0;
               /* bit 6 */
  pos ++;

  /* TP-FCS
  */
  data[pos] = (uint8)submit_report_error->tp_cause;
  pos ++;

  /* TP-PI
  */
  data[pos] = (uint8) submit_report_error->mask;
  pos ++;

  /* TP-SCTS
  */
  i = wms_ts_encode_timestamp( & submit_report_error->timestamp, data+pos );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-PID
  */
  if( submit_report_error->mask & WMS_TPDU_MASK_PID )
  {
    data[pos] = (uint8)submit_report_error->pid;
    pos ++;
  }

  /* TP-DCS
  */
  if( submit_report_error->mask & WMS_TPDU_MASK_DCS )
  {
    pos += wms_ts_encode_dcs( & submit_report_error->dcs, data+pos );
  }

  /* TP-UDL
  */
  if( submit_report_error->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /*data[pos] =(uint8) submit_report_error->user_data.sm_len;
      pos ++;*/

    /*TP_UDL is filled in encode_gw_user_data function*/
    /* TP-UD
    */
    if (wms_ts_compute_gw_user_data_length( &submit_report_error->dcs, &submit_report_error->user_data) > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_0("User Data Length has exceeded capacity");
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }
    else
    {
      i = wms_ts_encode_gw_user_data( & submit_report_error->dcs,
                                      & submit_report_error->user_data,
                                        data+pos );
      pos += i;
    }
  }

  raw_ts_data_ptr->tpdu_type  = WMS_TPDU_SUBMIT_REPORT_ERROR;
  raw_ts_data_ptr->len        = pos;

  return st;

} /* wms_ts_encode_submit_report_error() */

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST

uint8 wms_ts_encode_gw_cb_page_header
(
  const wms_gw_cb_page_header_s_type  * page_header_ptr,
  uint8                               * data
)
{
  uint8             pos = 0;

  if ( page_header_ptr == NULL || data == NULL)
  {
    return 0;
  }

  /* Serial Number */
  data[pos] = (uint8)((page_header_ptr->raw_serial_number & 0xFF00)>>8);
  pos++;
  data[pos] = (uint8)(page_header_ptr->raw_serial_number & 0x00FF);
  pos++;

  /* Message Identifier */
  data[pos] = (uint8)((page_header_ptr->cb_srv_id & 0xFF00)>>8);
  pos++;
  data[pos] = (uint8)(page_header_ptr->cb_srv_id & 0x00FF);
  pos++;

  data[pos] = page_header_ptr->cb_dcs.raw_dcs_data;
  pos++;

  data[pos] = (uint8)(((page_header_ptr->page_number & 0x0F)<<4)|(page_header_ptr->total_pages & 0x0F));
  pos++;

  return pos;

} /* wms_ts_encode_gw_cb_page_header() */

wms_status_e_type wms_ts_encode_gw_cb
(
  const wms_gw_cb_ts_data_s_type    * client_ts_ptr,   /* IN */
  wms_raw_ts_data_s_type            * raw_ts_data_ptr  /* OUT */
)
{
  wms_status_e_type   st = WMS_OK_S;

  wms_gw_user_data_s_type   *gw_user_data = NULL;

  if( client_ts_ptr == NULL || raw_ts_data_ptr == NULL )
  {
    st = WMS_NULL_PTR_S;
  }
  else if( wms_ts_encode_gw_cb_page_header(&client_ts_ptr->cb_page_hdr, raw_ts_data_ptr->data) == 0 )
  {
    st = WMS_INVALID_CB_DATA_S;
  }
  else
  {
    /* Process user data part */
    switch( client_ts_ptr->cb_page_hdr.cb_dcs.alphabet )
    {
      case WMS_GW_ALPHABET_7_BIT_DEFAULT:
        gw_user_data = (wms_gw_user_data_s_type*) wms_mem_malloc(sizeof(wms_gw_user_data_s_type));

        if(NULL == gw_user_data)
        {
           st = WMS_OUT_OF_RESOURCES_S;
           break;
        }

        raw_ts_data_ptr->len = WMS_GW_CB_PAGE_HEADER_LEN;
        gw_user_data->num_headers = 0;
        gw_user_data->sm_len = 0;
        if (client_ts_ptr->cb_page_hdr.cb_dcs.language == WMS_GW_CB_LANGUAGE_ISO_639)
        {
          /* First 3 characters of user data are Language */
          (void)memscpy( gw_user_data->sm_data,
                         3,
                         client_ts_ptr->cb_page_hdr.cb_dcs.iso_639_lang,
                         3);
          gw_user_data->sm_len += 3;
        }
        (void)memscpy( gw_user_data->sm_data + gw_user_data->sm_len,
                       client_ts_ptr->user_data.sm_len,
                       client_ts_ptr->user_data.sm_data,
                       client_ts_ptr->user_data.sm_len);

        gw_user_data->sm_len += client_ts_ptr->user_data.sm_len;

        raw_ts_data_ptr->len += (uint32)wms_ts_pack_gw_7_bit_chars(gw_user_data->sm_data,
                                           MIN(gw_user_data->sm_len, WMS_MAX_LEN),
                                           0, /* shift */
                                           WMS_MAX_LEN,
                                           raw_ts_data_ptr->data + WMS_GW_CB_PAGE_HEADER_LEN
                                           );

        wms_mem_free(gw_user_data);
        break;

      default:
        raw_ts_data_ptr->len = WMS_GW_CB_PAGE_HEADER_LEN;
        if (client_ts_ptr->cb_page_hdr.cb_dcs.language == WMS_GW_CB_LANGUAGE_ISO_639)
        {
          /* First 2 characters of user data are Language */
          (void)memscpy( raw_ts_data_ptr->data,
                         2,
                         client_ts_ptr->cb_page_hdr.cb_dcs.iso_639_lang,
                         2);
          raw_ts_data_ptr->len += 2;
        }
        (void)memscpy( raw_ts_data_ptr->data + raw_ts_data_ptr->len,
                       client_ts_ptr->user_data.sm_len,
                       client_ts_ptr->user_data.sm_data,
                       client_ts_ptr->user_data.sm_len);

        raw_ts_data_ptr->len += client_ts_ptr->user_data.sm_len;
        break;

    }
    raw_ts_data_ptr->format = WMS_FORMAT_GW_CB;
  }
  return st;
} /* wms_ts_encode_gw_cb */

#endif /*FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
  wms_ts_encode

DESCRIPTION
  Encode Transport Service structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode
(
  const wms_client_ts_data_s_type         * client_ts_data_ptr,
  wms_raw_ts_data_s_type                  * raw_ts_data_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
#ifdef FEATURE_GWSMS
  const wms_gw_pp_ts_data_s_type *msg;
#endif /* FEATURE_GWSMS */

  if( client_ts_data_ptr == NULL || raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

#ifdef FEATURE_GWSMS
  msg = & client_ts_data_ptr->u.gw_pp;
#endif /* FEATURE_GWSMS */

  wms_ts_init();
  qc_enter_crit_sect(&encode_decode_data_crit_sect);

  switch( client_ts_data_ptr->format )
  {

#ifdef FEATURE_CDSMS

    case WMS_FORMAT_CDMA:
    case WMS_FORMAT_ANALOG_AWISMS:
    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
    case WMS_FORMAT_MWI:
      st = wms_ts_encode_bearer_data( & client_ts_data_ptr->u.cdma,
                                      raw_ts_data_ptr );
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

    case WMS_FORMAT_GW_PP:
      /* ---------- Start encoding --------- */

      raw_ts_data_ptr->tpdu_type    = msg->tpdu_type;

      switch( msg->tpdu_type )
      {
        case WMS_TPDU_DELIVER:
        case WMS_TPDU_RESERVED:
          st = wms_ts_encode_deliver( msg->tpdu_type, & msg->u.deliver, raw_ts_data_ptr );
          break;


        case WMS_TPDU_DELIVER_REPORT_ACK:
          st = wms_ts_encode_deliver_report_ack( & msg->u.deliver_report_ack,
                                                   raw_ts_data_ptr );
          break;

        case WMS_TPDU_DELIVER_REPORT_ERROR:
          st = wms_ts_encode_deliver_report_error( & msg->u.deliver_report_error,
                                                     raw_ts_data_ptr );
          break;

        case WMS_TPDU_SUBMIT:
          st = wms_ts_encode_submit( & msg->u.submit, raw_ts_data_ptr );
          break;


        case WMS_TPDU_SUBMIT_REPORT_ACK:
          st = wms_ts_encode_submit_report_ack( & msg->u.submit_report_ack,
                                                  raw_ts_data_ptr );
          break;


        case WMS_TPDU_SUBMIT_REPORT_ERROR:
          st = wms_ts_encode_submit_report_error( & msg->u.submit_report_error,
                                                    raw_ts_data_ptr );
          break;


        case WMS_TPDU_STATUS_REPORT:
          st = wms_ts_encode_status_report( & msg->u.status_report,
                                              raw_ts_data_ptr );
          break;

        case WMS_TPDU_COMMAND:
          st = wms_ts_encode_command( & msg->u.command,
                                        raw_ts_data_ptr );
          break;

        default:
          MSG_ERROR_1("Invalid TPDU type %d", msg->tpdu_type);
          st = WMS_INVALID_TPDU_TYPE_S;
          break;
      }

      /* ---------- End of encoding --------- */
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
      case WMS_FORMAT_GW_CB:
        st = wms_ts_encode_gw_cb(&client_ts_data_ptr->u.gw_cb,
                                  raw_ts_data_ptr);
        break;
#endif /* FEATURE_GWSMS_BROADCAST */

      default:
        st = WMS_INVALID_FORMAT_S;
        MSG_ERROR_1("Invalid format: %d", client_ts_data_ptr->format);
        break;
  }

  raw_ts_data_ptr->format = client_ts_data_ptr->format;

  qc_leave_crit_sect(&encode_decode_data_crit_sect);

  return st;

} /* wms_ts_encode() */

/*=========================================================================
FUNCTION
  wms_ts_decode_dcs

DESCRIPTION
  Decode Data Coding Scheme from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_dcs
(
  const uint8           *data,
  wms_gw_dcs_s_type     *dcs
)
{
  uint8 pos = 0;
#ifdef FEATURE_GWSMS
  uint8 i;
  /* initialize the values */

  if (data == NULL || dcs == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_dcs");
    return 0;
  }

  wms_ts_init();
  qc_enter_crit_sect(&encode_decode_data_crit_sect);

  /* TP-DCS is defined in spec 3GPP TS 23.038, section 4. */
  dcs->msg_class      = WMS_MESSAGE_CLASS_NONE;
  dcs->msg_waiting    = WMS_GW_MSG_WAITING_NONE;
  dcs->alphabet       = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  dcs->is_compressed  = FALSE;
  dcs->msg_waiting_kind = WMS_GW_MSG_WAITING_VOICEMAIL;

  /* bits 7-6 */
  i = ( data[pos] & 0xC0 ) >> 6;
  switch( i )
  {
    case 0:
      /* pattern 00xx xxxx */
      dcs->is_compressed = data[pos] & 0x20;
      if( data[pos] & 0x10 )
      {
        dcs->msg_class = (wms_message_class_e_type) (data[pos] & 0x03);
      }
      else
      {
        /* no class information */
        dcs->msg_class = WMS_MESSAGE_CLASS_NONE;
      }
      dcs->alphabet = (wms_gw_alphabet_e_type) (( data[pos] & 0x0C ) >> 2);
      break;

    case 3:
      /* bits 5-4 */
      if( (data[pos] & 0x30) == 0x30 )
      {
        /* pattern 1111 xxxx */

        /* bit 3 is reserved */

        /* bit 2 */
        dcs->alphabet = (data[pos] & 0x04 ) ? WMS_GW_ALPHABET_8_BIT:
                            WMS_GW_ALPHABET_7_BIT_DEFAULT;

        /* bits 1-0 */
        dcs->msg_class = (wms_message_class_e_type) (data[pos] & 0x03);

        /* set remaining fields */
        dcs->is_compressed  = FALSE;
        dcs->msg_waiting    = WMS_GW_MSG_WAITING_NONE_1111;
      }
      else
      {
        /* Message waiting groups
        */
        dcs->is_compressed  = FALSE;
        dcs->msg_class          = WMS_MESSAGE_CLASS_NONE;

        /* bits 5-4 */
        if( (data[pos] & 0x30) == 0x00 )
        {
          dcs->msg_waiting  = WMS_GW_MSG_WAITING_DISCARD;
          dcs->alphabet     = WMS_GW_ALPHABET_7_BIT_DEFAULT;
        }
        else if( (data[pos] & 0x30) == 0x10 )
        {
          dcs->msg_waiting  = WMS_GW_MSG_WAITING_STORE;
          dcs->alphabet     = WMS_GW_ALPHABET_7_BIT_DEFAULT;
        }
        else
        {
          dcs->msg_waiting  = WMS_GW_MSG_WAITING_STORE;
          dcs->alphabet     = WMS_GW_ALPHABET_UCS2;
        }

        /* bit 3 */
        dcs->msg_waiting_active = ( data[pos] & 0x08 ) ? TRUE : FALSE;

        /* bit 2 is reserved */

        /* bits 1-0 */
        dcs->msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type) (data[pos] & 0x03);
      }
      break;

    default:
      // reserved values
      MSG_ERROR_1("Invalid DCS: %x", data[pos]);
      dcs->alphabet       = WMS_GW_ALPHABET_7_BIT_DEFAULT;
      dcs->is_compressed  = FALSE;
      dcs->msg_waiting    = WMS_GW_MSG_WAITING_NONE;
      dcs->msg_class          = WMS_MESSAGE_CLASS_NONE;
      break;
  }

  if ( dcs->alphabet > WMS_GW_ALPHABET_UCS2 )
  {
    dcs->alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  }

  dcs->raw_dcs_data = data[pos];

  pos ++;

  qc_leave_crit_sect(&encode_decode_data_crit_sect);
#endif /* FEATURE_GWSMS */
  return pos;

} /* wms_ts_decode_dcs() */

/*=========================================================================
FUNCTION
  wms_ts_decode_evp_rel_time

DESCRIPTION
  Decode a timestamp(hour/min/second) from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/

uint8 wms_ts_decode_evp_rel_time
(
  const uint8 *data,
  wms_timestamp_s_type *vp
)
{
  uint8 pos = 0, i, j;
  if( data == NULL || vp == NULL )
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_evp_rel_time");
    return 0;
  }
  /*hour check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR_1("Hour is too large: %d", j);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Hour : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  vp->hour = i;
  pos ++;

  /*minute check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Minute is too large: %d", j);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Minute : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  vp->minute = i;
  pos ++;

  /*seconds check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Second is too large: %d", i);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Second : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  vp->second = i;
  pos ++;

  return pos;

}/* wms_ts_decode_evp_rel_time */

/*=========================================================================
FUNCTION
  wms_ts_decode_timestamp

DESCRIPTION
  Decode a timestamp from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_timestamp
(
  const uint8             *data,
  wms_timestamp_s_type  *timestamp
)
{
  uint8 pos = 0, i, j;

  if (data == NULL || timestamp == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_timestamp");
    return 0;
  }

  /*year check*/

  /* in GW, swap the order of LSB, MSB */
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( !wms_ts_bcd_to_int(i, &j) )
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Year : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->year = i;
  pos ++;

  /*month check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 12 || j < 1)
    {
      MSG_ERROR_1("Month is invalid: %d", j);
      i = 1;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Month : %d", data[pos]);
    i = 1;  /* Modifying it to a Good Value */
  }
  timestamp->month = i;
  pos ++;

  /*day check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 31 || j < 1)
    {
      MSG_ERROR_1("Day is invalid: %d", j);
      i = 1;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Day : %d", data[pos]);
    i = 1;  /* Modifying it to a Good Value */
  }
  timestamp->day = i;
  pos ++;

  /*hour check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR_1("Hour is too large: %d", j);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Hour : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->hour = i;
  pos ++;

  /*minute check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Minute is too large: %d", j);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Minute : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->minute = i;
  pos ++;

  /*seconds check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR_1("Second is too large: %d", i);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR_1("Invalid BCD Digits in Encoded Timestamp Second : %d", data[pos]);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->second = i;
  pos ++;

  /*timezone, special case where timestamp->timezone is an integer value*/
  if (data[pos] & 0x08)
  {
    timestamp->timezone = (data[pos] & 0x07) * 10 + ((data[pos] & 0xF0)>>4);
    timestamp->timezone *= (-1);
  }
  else
  {
    timestamp->timezone = (sint7)((data[pos] & 0x0F) * 10 + ((data[pos] & 0xF0) >> 4 ));
  }
  // Valid range of timezone is from -12 to +14 hours
  if (timestamp->timezone > 56 || timestamp->timezone < -48)
  {
    MSG_ERROR_1("Timezone is out of bound: %d", timestamp->timezone);
    timestamp->timezone = 0;  /* Modifying it to a Good Value */
  }
  pos ++;

  return pos;

} /* wms_ts_decode_timestamp() */

/*=========================================================================
FUNCTION
  wms_ts_decode_enhanced_vp

DESCRIPTION
  Decode a TP-VP(Enhanced format) from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_enhanced_vp
(
  const uint8             *data,
  wms_enhanced_vp_s_type  *enhanced_vp
)
{
  uint8 pos = 0, i;

  if (data == NULL || enhanced_vp == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_timestamp");
    return 0;
  }

  /*
  bit 7 Extension bit
  Set to 1 if the TP-VP functionality indicator is to be extended to another
  octet. A setting of 0 indicates that there are no more TP-VP functionality
  indicator extension octets to follow.
  Any such extension octet shall immediately follow the previous TP-VP
  functionality indicator.
  */
  enhanced_vp->with_extension = (data[pos] & 0x80)>> 7;

  /*
  bit 6 Single shot SM.
  Set to 1 if the SC is required to make up to one delivery attempt.
  The TP-Validity Period, where present, shall be applicable to the Single
  shot SM.
  */
  enhanced_vp->single_shot    = (data[pos] & 0x40)>> 6;

  /*
  bits 2, 1, 0  Validity Period Format
  */
  enhanced_vp->vp_format      = (wms_enhanced_vp_format_e_type)(data[pos] & 0x07);

  pos++;

  if( TRUE == enhanced_vp->with_extension )
  {
    /* Octet2 is Extended TP-VP functionality indicator */
    enhanced_vp->extension = data[pos++];
  }

  switch( enhanced_vp->vp_format )
  {
    case WMS_ENHANCED_VP_NONE:
      break;

    case WMS_ENHANCED_VP_RELATIVE:
      /* Validity Period is as specified for the relative case. */
      wms_ts_decode_relative_time( data[pos], & enhanced_vp->time );
      break;

    case WMS_ENHANCED_VP_RELATIVE_INT:
      /* Validity period is relative in integer representation and the
         following octet contains the TP-VP value in the range 0 to 255
         representing 0 to 255 seconds. A TP-VP value of zero is undefined
         and reserved for future use.
         */
      memset( (void*) &(enhanced_vp->time), 0, sizeof(wms_timestamp_s_type) );
      enhanced_vp->time.minute = (uint8) wms_ts_int_to_bcd((uint8)(data[pos]/60));
      enhanced_vp->time.second = (uint8) wms_ts_int_to_bcd((uint8)(data[pos]%60));
      break;

    case WMS_ENHANCED_VP_SEMI_OCTET:
      /* Validity period is relative in semi-octet representation.
         The following 3 octets contain the relative time in Hours,
         Minutes and Seconds giving the length of the validity period
         counted from when the SMS-SUBMIT is received by the SC. The
         representation of time uses the same representation as the Hours,
         Minutes and Seconds in the TP Service Centre Time Stamp.
         */
      i = wms_ts_decode_evp_rel_time(data+pos, & enhanced_vp->time);
      if( i == 0 )
      {
        MSG_ERROR_0("Error while Decoding Enhanced VP relative time");
      }
      break;

    default:
      break;
  }
  return WMS_ENHANCED_VP_OCTETS;
}/* wms_ts_decode_enhanced_vp */

static uint8 wms_ts_decode_udh_concat_8
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr /* OUT */
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if (udh[pos] != 3)  /*Length of information element*/
  {
    MSG_ERROR_1("UDH Header Concat 8 Present with invalid data length = %d", udh[pos]);
    return 0; /*Return 0*/
  }


  /* if the maximum number of messages is 0     Or*/
  /* if the sequence number of the message is 0 Or*/
  /* if the sequence number of the current message is greater than the max messages*/
  if (udh[pos +2] == 0 ||
      udh[pos +3] == 0 ||
      udh[pos +3] > udh[pos +2])
  {
    MSG_ERROR_0("UDH Header Contact 8 with out of bound max messages");
    return 0;
  }

  pos++;
  header_ptr->header_id          = WMS_UDH_CONCAT_8;
  header_ptr->u.concat_8.msg_ref = udh[pos++];
  header_ptr->u.concat_8.total_sm= udh[pos++];
  header_ptr->u.concat_8.seq_num = udh[pos++];

  return (udh[0] + 1);

}/*wms_ts_decode_udh_concat_8*/

static uint8 wms_ts_decode_udh_concat16
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 4) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Concat16 Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  /* if the maximum number of messages is 0     Or*/
  /* if the sequence number of the message is 0 Or*/
  /* if the sequence number of the current message is greater than the max messages*/
  if (udh[pos +3] == 0 ||
      udh[pos +4] == 0 ||
      udh[pos +4] > udh[pos +3])
    return 0;

  header_ptr->header_id           = WMS_UDH_CONCAT_16;
  pos++;
  header_ptr->u.concat_16.msg_ref = udh[pos++];
  header_ptr->u.concat_16.msg_ref = (uint16)(header_ptr->u.concat_16.msg_ref << 8) | udh[pos++];
  header_ptr->u.concat_16.total_sm= udh[pos++];
  header_ptr->u.concat_16.seq_num = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_concat16*/


/*=========================================================================
FUNCTION
  wms_ts_udh_decode_first_seg_check

DESCRIPTION

  Local helper function used during decoding to check if message segment
  is first segment or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE depending on if function is successful or not.

SIDE EFFECTS
  Modifies is_first_segment_ptr with result
=========================================================================*/
static boolean wms_ts_udh_decode_first_seg_check
(
  const uint8  len,                /* user_data_length*/
  const uint8  *data,              /* first byte of user data */
  boolean      *is_first_segment_ptr      /* OUT */
)
{
  uint8 pos = 0;
  uint8 num_headers = 0;
  uint8 udhl = 0;
  uint8 iedl = 0;
  uint8 iei = 0;

  // Default to true - it might not have concat header present
  *is_first_segment_ptr = TRUE;

  if (data == NULL || data[pos] == 0 || len == 0 )
  {
    MSG_ERROR_0("null in wms_ts_udh_decode_first_seg_check");
    return FALSE;
  }

  // First byte is the UDH Length
  udhl = data[pos];

  if( (udhl+1) > MIN(len, WMS_GW_USER_DATA_LEN_MAX) )
  {
    MSG_ERROR_2( "Invalid UDHL %d (exceeds UD length %d)",
                 udhl + 1,
                 MIN(len, WMS_GW_USER_DATA_LEN_MAX) );
    return FALSE;
  }
  // Move pos to first user data header
  pos++;

  while ((pos < udhl) && (num_headers < WMS_MAX_UD_HEADERS))
  {
      // First byte is UDH, next byte is length
      iei  = data[pos];
      iedl = data[pos+1];

      if( (pos + iedl + 2) > (udhl + 1) )
      {
        /* 1 (IEI) + 1 (IEDL) + IED */
        MSG_ERROR_3( "Invalid IEDL: pos(%d) + iedl(%d) + 2 > UDHL(%d) + 1",
                     pos,
                     iedl,
                     udhl );
        return FALSE;
      }

      if ((uint8)WMS_UDH_CONCAT_16 == iei)
      {
          // For concat16 element, peek and see seq#
          // -------------------------------------------
          // || IEI | IEDL | Ref# | Ref# |Max# | Seq# ||
          // -------------------------------------------
          if ( (pos + 5 <= udhl) && (data[pos+5] != 1) )
          {
              MSG_HIGH_0("WMS_UDH_CONCAT_16 not first segment");
              *is_first_segment_ptr = FALSE;
              return TRUE;
          }
          else
          {
              return TRUE;
          }
      }
      // Not a concat header, so we dont care, skip over it's length
      else
      {
          num_headers++;
          pos += (2 + iedl); // IEI + IEDL + Actual Data Length
      }
  } // while

  return TRUE;
}


static uint8 wms_ts_decode_udh_special_sm
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 2) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Special SM Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                             = WMS_UDH_SPECIAL_SM;

  /* Bit 7 of octet 1 indicates if the message shall be stored or not.
     Bit 6..0 of octet 1 indicate which indicator is to be updated.
          000 0000 = Voice Message Waiting
          000 0001 = Fax Message Waiting
          000 0010 = Email Message Waiting
          000 0011 = Other Message Waiting
          000 0111 = Videomail Message Waiting (3GPP TS 23.040 Release 8 76 V8.5.0 (2009-06))
    Octet 2 represents the number of waiting messages
          The number can range from 0 to 255 with the value of 255 meaning
          that there are 255 or more messages waiting
  */

  header_ptr->u.special_sm.msg_waiting =  (wms_gw_msg_waiting_e_type) ((udh[pos] >> 7 == 0) ? WMS_GW_MSG_WAITING_DISCARD : WMS_GW_MSG_WAITING_STORE);

  /* Extended message - currently supports only videomail*/
  if( ( udh[pos] & WMS_SP_EXTD_VIDEOMAIL_MSG_WAITING_TYPE ) > 0 )
  {
    header_ptr->u.special_sm.msg_waiting_kind = WMS_GW_MSG_WAITING_VIDEOMAIL;
  }
  /* Not extended message*/
  else if( ( udh[pos] & 0x04 ) == 0 )
  {
    header_ptr->u.special_sm.msg_waiting_kind
                                   = (wms_gw_msg_waiting_kind_e_type) (udh[pos] & 0x03);
  }
  pos++;
  header_ptr->u.special_sm.message_count            = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_special_sm*/

static uint8 wms_ts_decode_udh_port_8
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }


  if (udh[pos] != 2)  /*Length of information element*/
  {
    MSG_ERROR_1("UDH Header Port 8 Present with invalid data length = %d", udh[pos]);
    return 0; /*Return 0*/
  }


  pos++;
  header_ptr->header_id          = WMS_UDH_PORT_8;
  header_ptr->u.wap_8.dest_port  = udh[pos++];
  header_ptr->u.wap_8.orig_port  = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_port_8*/

static uint8 wms_ts_decode_udh_port16
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 4) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Port16 Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  header_ptr->header_id           = WMS_UDH_PORT_16;
  pos++;
  header_ptr->u.wap_16.dest_port = udh[pos++];
  header_ptr->u.wap_16.dest_port = (uint16)(header_ptr->u.wap_16.dest_port << 8) | udh[pos++];
  header_ptr->u.wap_16.orig_port = udh[pos++];
  header_ptr->u.wap_16.orig_port = (uint16)(header_ptr->u.wap_16.orig_port << 8) | udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_port16*/


static uint8 wms_ts_decode_udh_text_formatting
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 3 && udh[pos] != 4) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Text Formatting Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  if(udh[pos] == 4)
  {
    header_ptr->u.text_formating.is_color_present = TRUE;
  }
  else if (udh[pos] == 3)
  {
    header_ptr->u.text_formating.is_color_present = FALSE;
  }
  pos++;


  header_ptr->header_id                                 = WMS_UDH_TEXT_FORMATING;
  header_ptr->u.text_formating.start_position           = udh[pos++];
  header_ptr->u.text_formating.text_formatting_length   = udh[pos++];
  header_ptr->u.text_formating.alignment_type           = (wms_udh_alignment_e_type) (udh[pos] & 0x03 ); /*bit 0 and  bit 1*/
  header_ptr->u.text_formating.font_size                = (wms_udh_font_size_e_type) ((udh[pos] & 0x0c) >> 2); /*bit 3 and  bit 2*/

  header_ptr->u.text_formating.style_bold               = (udh[pos] & 0x10) >> 4; /*bit 4 */
  header_ptr->u.text_formating.style_italic             = (udh[pos] & 0x20) >> 5; /*bit 5 */
  header_ptr->u.text_formating.style_underlined         = (udh[pos] & 0x40) >> 6; /*bit 6 */
  header_ptr->u.text_formating.style_strikethrough      = (udh[pos] & 0x80) >> 7; /*bit 7 */
  pos++;

  if(header_ptr->u.text_formating.is_color_present)
  {
    header_ptr->u.text_formating.text_color_foreground   = (wms_udh_text_color_e_type) (udh[pos] & 0x0F);  /* bit 0-3 */
    header_ptr->u.text_formating.text_color_background   = (wms_udh_text_color_e_type) ((udh[pos] & 0xF0) >> 4);  /* bit 4-7 */
    pos++;
  }
  return (udh[0] + 1);
}/*wms_ts_decode_udh_text_formatting*/

static uint8 wms_ts_decode_udh_pre_def_sound
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 2) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Pre Defined Sound Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_PRE_DEF_SOUND;
  header_ptr->u.pre_def_sound.position                  = udh[pos++];
  header_ptr->u.pre_def_sound.snd_number                = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_pre_def_sound*/

static uint8 wms_ts_decode_udh_user_def_sound
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] == 0) /*Length of information element*/
  {
    MSG_ERROR_0("SMS UDH Header User Defined Sound Present with no Data");
    return 0;
  }

  header_ptr->header_id                                 = WMS_UDH_USER_DEF_SOUND;
  header_ptr->u.user_def_sound.data_length              = udh[pos++]-1;
  header_ptr->u.user_def_sound.position                 = udh[pos++];


  if (header_ptr->u.user_def_sound.data_length > WMS_UDH_MAX_SND_SIZE)
  {
    MSG_ERROR_1("Max Size Exceed Header id %d ", header_ptr->header_id);
    //data[pos] += data[pos]; /*Skip the bytes*/
    return 0;
  }

  //pos++;
  memset(header_ptr->u.user_def_sound.user_def_sound, 0xff, WMS_UDH_MAX_SND_SIZE);

  for (j = 0; j < header_ptr->u.user_def_sound.data_length; j++)
  {
    header_ptr->u.user_def_sound.user_def_sound[j]      = udh[pos++];
  }

  return pos;
}/*wms_ts_decode_udh_user_def_sound*/


static uint8 wms_ts_decode_udh_pre_def_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 2) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Pre Defined Animation Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_PRE_DEF_ANIM;
  header_ptr->u.pre_def_anim.position                   = udh[pos++];
  header_ptr->u.pre_def_anim.animation_number           = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_pre_def_anim*/

static uint8 wms_ts_decode_udh_large_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j, k;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != (WMS_UDH_ANIM_NUM_BITMAPS * WMS_UDH_LARGE_BITMAP_SIZE + 1) ) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Large Defined Animation Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  header_ptr->header_id                             = WMS_UDH_LARGE_ANIM;
  pos++; /*Skip the Size*/
  header_ptr->u.large_anim.position                 = udh[pos++];

  for(j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_LARGE_BITMAP_SIZE; k++)
      header_ptr->u.large_anim.data[j][k] = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_large_anim*/


static uint8 wms_ts_decode_udh_small_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j, k;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != (WMS_UDH_ANIM_NUM_BITMAPS * WMS_UDH_SMALL_BITMAP_SIZE + 1) ) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Large Defined Animation Present with invalid data length = ", udh[pos]);
    return 0;
  }

  header_ptr->header_id                             = WMS_UDH_SMALL_ANIM;

  pos++; /*Skip the Size*/
  header_ptr->u.small_anim.position                 = udh[pos++];

  for(j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_SMALL_BITMAP_SIZE; k++)
      header_ptr->u.small_anim.data[j][k] = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_small_anim*/

static uint8 wms_ts_decode_udh_large_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != WMS_UDH_LARGE_PIC_SIZE + 1) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Large Picture Present with invalid data length = %d", udh[pos]);
    return 0;
  }


  header_ptr->header_id                                 = WMS_UDH_LARGE_PICTURE;
  pos++; /*Skip the Size*/
  header_ptr->u.large_picture.position                  = udh[pos++];

  for(j=0;j<WMS_UDH_LARGE_PIC_SIZE;j++)
    header_ptr->u.large_picture.data[j]    = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_large_picture*/


static uint8 wms_ts_decode_udh_small_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != WMS_UDH_SMALL_PIC_SIZE + 1) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Small Picture Present with invalid data legnth = %d", udh[pos]);
    return 0;
  }

  header_ptr->header_id                                 = WMS_UDH_SMALL_PICTURE;
  pos++; /*Skip the size*/
  header_ptr->u.small_picture.position                  = udh[pos++];

  for(j=0;j<WMS_UDH_SMALL_PIC_SIZE;j++)
    header_ptr->u.small_picture.data[j]           = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_small_picture*/


static uint8 wms_ts_decode_udh_var_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, j, pic_size;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] > WMS_UDH_VAR_PIC_SIZE + 3) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Var Picture Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  if ( (udh[pos] - 3) != (udh[pos+2] * udh[pos+3]) )
  {
    MSG_ERROR_0("SMS UDH Header Var Picture, pic size value mismatch with heigt and weight");
    return 0;
  }

  pic_size                                           = udh[pos++] -3;
  header_ptr->header_id                                 = WMS_UDH_VAR_PICTURE;
  header_ptr->u.var_picture.position                    = udh[pos++];
  if( udh[pos] * 8 < 256 )
  {
    header_ptr->u.var_picture.width                       = (uint8) (udh[pos++] * 8);
  }
  else
  {
    MSG_ERROR_0("SMS UDH Header Var Picture, pic width is too large");
    return 0;
  }

  header_ptr->u.var_picture.height                      = udh[pos++];

  for(j=0;j<pic_size && j<WMS_UDH_VAR_PIC_SIZE; j++)
    header_ptr->u.var_picture.data[j]             = udh[pos++];


  return pos;
}/*wms_ts_decode_udh_var_picture*/


static uint8 wms_ts_decode_udh_user_prompt
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 1) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH User Prompt present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos ++; /* Skip udh length */

  header_ptr->header_id                                 = WMS_UDH_USER_PROMPT;
  header_ptr->u.user_prompt.number_of_objects           = udh[pos++];

  return (udh[0] + 1);

} /* wms_ts_decode_udh_user_prompt() */


/* Decoding UDH Extended Object
*/
static uint8 wms_ts_decode_udh_eo
(
  const uint8 *udh,
  boolean     first_segment,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, udh_length;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] == 0) /*Length of information element*/
  {
    MSG_ERROR_0("SMS UDH Extended Object present with no Data");
    return 0;
  }

  /* Get the length of this UDH */
  udh_length = udh[pos++];

  header_ptr->header_id                  = WMS_UDH_EXTENDED_OBJECT;
  header_ptr->u.eo.first_segment         = first_segment;

  if( first_segment == TRUE )
  {
    /* The following fields in the first segment occupy 7 bytes */
    if( udh_length < WMS_UDH_OCTETS_EO_HEADER || udh_length > WMS_UDH_EO_DATA_SEGMENT_MAX )
    {
      MSG_ERROR_1(" SMS UDH Extended Object presents invalid UDHL:%d", udh_length);
      return 0;
    }

    header_ptr->u.eo.reference           = udh[pos++];
    header_ptr->u.eo.length              = udh[pos++] << 8;
    header_ptr->u.eo.length              |= udh[pos++];
    header_ptr->u.eo.control             = udh[pos++];
    header_ptr->u.eo.type                = (wms_udh_eo_id_e_type) udh[pos++];
    header_ptr->u.eo.position            = udh[pos++] << 8;
    header_ptr->u.eo.position            |= udh[pos++];
  }
  else /* first_segment  == FALSE */
  {
    if(udh_length > WMS_UDH_EO_DATA_SEGMENT_MAX)
    {
      MSG_ERROR_1(" SMS UDH Extended Object presents invalid UDHL:%d", udh_length);
      return 0;
    }
  }
  /* Decode EO content */
  header_ptr->u.eo.content.length = (udh_length - pos) + 1;

  if( header_ptr->u.eo.content.length > 0 )
  {
    (void)memscpy( header_ptr->u.eo.content.data,
                   header_ptr->u.eo.content.length,
                   udh + pos,
                   header_ptr->u.eo.content.length );

    pos += header_ptr->u.eo.content.length;
  }

  return pos;

} /* wms_ts_decode_udh_eo() */

static uint8 wms_ts_decode_udh_rfc822
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != 1) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH Header Rfc822 Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_RFC822;
  header_ptr->u.rfc822.header_length                    = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_rfc822*/

static uint8 wms_ts_decode_udh_nat_lang_ss
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != WMS_UDH_OCTETS_NAT_LANG_SS) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH National Lang Single Shift Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                 = WMS_UDH_NAT_LANG_SS;
  /* Validate national language id */
  if (((uint8)WMS_UDH_NAT_LANG_TURKISH > udh[pos]) ||
      ((uint8)WMS_UDH_NAT_LANG_PORTUGUESE < udh[pos]))
  {
    MSG_ERROR_1("SMS UDH National Lang Single Shift Present with invalid id = %d", udh[pos]);
    return 0;
  }
  header_ptr->u.nat_lang_ss.nat_lang_id = (wms_udh_nat_lang_id_e_type)udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_nat_lang_ss*/

static uint8 wms_ts_decode_udh_nat_lang_ls
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if(udh[pos] != WMS_UDH_OCTETS_NAT_LANG_LS) /*Length of information element*/
  {
    MSG_ERROR_1("SMS UDH National Lang Locking Shift Present with invalid data length = %d", udh[pos]);
    return 0;
  }

  pos++;
  header_ptr->header_id                 = WMS_UDH_NAT_LANG_LS;
  /* Validate national language id */
  if (((uint8)WMS_UDH_NAT_LANG_TURKISH > udh[pos]) ||
      ((uint8)WMS_UDH_NAT_LANG_PORTUGUESE < udh[pos]))
  {
    MSG_ERROR_1("SMS UDH National Lang Locking Shift Present with invalid id = %d", udh[pos]);
    return 0;
  }
  header_ptr->u.nat_lang_ls.nat_lang_id = (wms_udh_nat_lang_id_e_type)udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_nat_lang_ls*/


static uint8 wms_ts_decode_udh_other
(
  const uint8* udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, i=0;

  if (udh == NULL || header_ptr == NULL )
  {
    wms_check_udh_null();
    return 0;
  }

  if (udh[pos+1] > WMS_UDH_OTHER_SIZE)
  {
    MSG_ERROR_0("SMS UDH Header Other data length exceeding 226");
    return 0;
  }

  header_ptr->header_id                                 = (wms_udh_id_e_type) udh[pos];
  header_ptr->u.other.header_id                         = (wms_udh_id_e_type) udh[pos++];
  header_ptr->u.other.header_length                     = udh[pos++];

  for(i=0;i<header_ptr->u.other.header_length && i<WMS_UDH_OTHER_SIZE; i++)
  {
    header_ptr->u.other.data[i] = udh[pos++];
  }

  return (udh[1] + 1);
}

/*=========================================================================
FUNCTION
  wms_ts_decode_user_data_header

DESCRIPTION
  Decode User Data from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_user_data_header
(
  const uint8               len, /* user_data_length*/
  const uint8               *data, /* first byte of user data */
  uint8                     * num_headers_ptr, /* OUT */
  wms_udh_s_type            * udh_ptr          /* OUT */
)
{
  uint8 pos =0;
  uint8 header_length =0, num_headers=0;
  uint8 udhl;
  boolean    first_segment = TRUE; /* Used for Extended Object decoding */

  if (data == NULL || len == 0 || data[pos] == 0 || num_headers_ptr == NULL || udh_ptr == NULL )
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_user_data_header");
    return 0;
  }

  udhl = data[pos];

  if( (udhl+1) > MIN(len, WMS_GW_USER_DATA_LEN_MAX) )
  {
    MSG_ERROR_2( "Invalid UDHL %d (exceeds UD length %d)",
                 udhl + 1,
                 MIN(len, WMS_GW_USER_DATA_LEN_MAX) );
    return FALSE;
  }

  pos++;

  while ((pos < udhl)&&(num_headers < WMS_MAX_UD_HEADERS))
  {

    header_length = 0;

    /*first byte - IEI*/
    /*second byte - IEDL */
    if( pos + 2 + data[pos + 1] > udhl + 1 )
    {
      return 0;
    }

    switch(data[pos])
    {
      case WMS_UDH_CONCAT_8:
        header_length = wms_ts_decode_udh_concat_8(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_CONCAT_16:
        header_length = wms_ts_decode_udh_concat16(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_SPECIAL_SM:
        header_length = wms_ts_decode_udh_special_sm(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_PORT_8:
        header_length = wms_ts_decode_udh_port_8(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_PORT_16:
        header_length = wms_ts_decode_udh_port16(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_TEXT_FORMATING:
        header_length = wms_ts_decode_udh_text_formatting(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_PRE_DEF_SOUND:
        header_length = wms_ts_decode_udh_pre_def_sound(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_USER_DEF_SOUND:
        header_length = wms_ts_decode_udh_user_def_sound(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_PRE_DEF_ANIM:
        header_length = wms_ts_decode_udh_pre_def_anim(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_LARGE_ANIM:
        header_length = wms_ts_decode_udh_large_anim(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_SMALL_ANIM:
        header_length = wms_ts_decode_udh_small_anim(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_LARGE_PICTURE:
        header_length = wms_ts_decode_udh_large_picture(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_SMALL_PICTURE:
        header_length = wms_ts_decode_udh_small_picture(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_VAR_PICTURE:
        header_length = wms_ts_decode_udh_var_picture(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_USER_PROMPT:
        header_length = wms_ts_decode_udh_user_prompt( data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_EXTENDED_OBJECT:
        if ( wms_ts_udh_decode_first_seg_check ( len, data, &first_segment ) == FALSE )
        {
          MSG_ERROR_0("wms_ts_udh_decode_first_seg_check failed");
          return 0;
        }
        header_length = wms_ts_decode_udh_eo( data + pos + 1, first_segment, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_RFC822:
        header_length = wms_ts_decode_udh_rfc822(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_NAT_LANG_SS:
        header_length = wms_ts_decode_udh_nat_lang_ss(data + pos + 1, &udh_ptr[num_headers]);
        break;

      case WMS_UDH_NAT_LANG_LS:
        header_length = wms_ts_decode_udh_nat_lang_ls(data + pos + 1, &udh_ptr[num_headers]);
        break;

      default:
        header_length = wms_ts_decode_udh_other(data + pos, &udh_ptr[num_headers]);
        break;
    } /* End of Switch */

    if(header_length != 0)
    {
      pos += header_length + 1;
      num_headers++;
    }
    else
    {
      MSG_ERROR_2( "Bad UDH: pos=%d, data[pos]=%d",
                   pos + 1,
                   data[pos + 1] );
      * num_headers_ptr = 0;
      return 0;  /* SHORT-RETURN */
    }
  } /* End of While loop */


  if (num_headers >= WMS_MAX_UD_HEADERS)
  {
    /* Num Headers has Exceeded Max */
    MSG_ERROR_0("decode_udh: Num Headers has exceeded WMS_MAX_UD_HEADERS");

    /* Placing the correct value */
    pos = udhl+1;
  }

  if (pos!= (udhl+1))
  {
    MSG_ERROR_0("SMS UDH could not be decoded");
    num_headers =0;
    udhl =0;
  }

  if (num_headers >0)
  {
    * num_headers_ptr = num_headers;
  }

  return udhl;
}
/*wms_ts_decode_user_data_header*/

#ifdef FEATURE_GWSMS
/*=========================================================================
FUNCTION
  wms_ts_decode_gw_user_data

DESCRIPTION
  Decode User Data from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_gw_user_data
(
  const wms_gw_dcs_s_type   *dcs,
  const uint8               len, // user data length
  const uint8               *data,
  const boolean             user_data_header_present,
  wms_gw_user_data_s_type   *user_data
)
{
  uint8 i, pos=0;
  uint8   fill_bits =0;
  uint8   user_data_length;
  uint8   user_data_header_length = 0; /* only the user data header length*/

  if (dcs == NULL || data == NULL || user_data == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_gw_user_data");
    return 0;
  }

  /* Defaulting to all zeroes */
  (void)memset(user_data, 0, sizeof(wms_gw_user_data_s_type));

  if (len == 0)
  {
    return 0;
  }

  if( dcs->alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT )
  {
    if (len > WMS_SMS_UDL_MAX_7_BIT)
    {
      MSG_ERROR_0("user data length > max value for gw 7-bit alphabet");
      return 0;
    }
    user_data_length = len;

    if(user_data_header_present)
    {
      user_data_header_length = wms_ts_decode_user_data_header( data[pos]+1, data+pos,
                                                                & user_data->num_headers, user_data->headers );

      if ( (1 + user_data_header_length) > (len * 7 / 8) )
      {
         MSG_ERROR_0("user data header length >= total length");
         return 0;
      }
    }
    /*len would be the number of septets*/
    if(user_data_header_length > 0)
    {
      /*The number of fill bits required to make a septet boundary*/
      fill_bits =( (len * 7) - ((user_data_header_length+1)*8) ) % 7;
      user_data_length = (uint8)(( (len * 7) - ((user_data_header_length+1)*8) ) / 7);
      pos = user_data_header_length + 1;

      if (fill_bits != 0)
      {
        fill_bits = 8 - fill_bits;
      }
    }

    i = wms_ts_unpack_gw_7_bit_chars
        (
          & data[pos],
          user_data_length,
          WMS_MAX_LEN,
          fill_bits,
          user_data->sm_data
        );
    user_data->sm_len = user_data_length;
  }
  else
  {
    if (len > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_0("user data length > max value for 8-bit chararacters");
      return 0;
    }
    user_data_length = len;

    if(user_data_header_present)
    {
       user_data_header_length = wms_ts_decode_user_data_header( data[pos]+1,
                                                                 data+pos,
                                                       & user_data->num_headers,
                                                       user_data->headers );
       if (user_data_header_length >= len)
       {
          MSG_ERROR_0("user data header length >= total length");
          return 0;
       }
       pos += user_data_header_length +1;
       user_data_length = len - (user_data_header_length + 1);
    }

    (void)memscpy( user_data->sm_data,
                   user_data_length,
                   data + pos,
                   user_data_length );
    user_data->sm_len = user_data_length;
    i = (uint8)user_data->sm_len;
  }

  pos += i;

  return pos;

} /* wms_ts_decode_gw_user_data() */


/*=========================================================================
FUNCTION
  wms_ts_decode_gw_validity

DESCRIPTION
  Decode Validity from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_gw_validity
(
  const uint8                 * data,
  wms_gw_validity_s_type    * validity
)
{
  uint8 i, pos = 0;

  if (data == NULL || validity == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_gw_validity");
    return 0;
  }
  else
  {
    switch( validity->format )
    {
      case WMS_GW_VALIDITY_NONE:
      memset( validity, 0, sizeof(wms_gw_validity_s_type) );
        break;

      case WMS_GW_VALIDITY_RELATIVE:
        wms_ts_decode_relative_time( data[pos], & validity->u.time );
        pos ++;
        break;

      case WMS_GW_VALIDITY_ABSOLUTE:
        i = wms_ts_decode_timestamp( data+pos, & validity->u.time );
        if( i == 0 )
        {
          MSG_ERROR_0("Error while Decoding Absolute Validity Timestamp");
        }
        pos += i;
        break;

      case WMS_GW_VALIDITY_ENHANCED:
        i = wms_ts_decode_enhanced_vp (data + pos, & validity->u.enhanced_vp);
        if( i == 0 )
        {
          MSG_ERROR_0("Error while Decoding Enhanced Validity Period");
        }
        pos += i;
        break;

      default:
        MSG_ERROR_0("Invalid TP-VPF");
        break;
    } /* switch */
    return pos;
  }

} /* wms_ts_decode_gw_validity() */



/*=========================================================================
FUNCTION
  wms_ts_decode_deliver

DESCRIPTION
  Decode Deliver TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_deliver
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_deliver_s_type                   * deliver,
  uint32                                  * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data = raw_ts_data_ptr->data;

  /* TP-MTI, TP-MMS, TP-SRI, TP-UDHI, TP-RP
  */
  if (raw_ts_data_ptr == NULL || deliver == NULL)
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_deliver");
    return WMS_NULL_PTR_S;
  }
  /* check bit 0 and bit 1 for TP-MTI for deliver or Reserved type */
  else if ( ((data[pos] & 0x03) != 0x00) && ((data[pos] & 0x03) != 0x03) )
  {
    MSG_ERROR_1("invalid tpdu type = %d in wms_ts_decode_deliver ", raw_ts_data_ptr->tpdu_type);
    return WMS_INVALID_TPDU_TYPE_S;
  }
  else
  {
    deliver->more                     = (data[pos] & 0x04) ? FALSE : TRUE;
                                        /* bit 2 */
    /* bits 3, 4 are not used */
    deliver->status_report_enabled    = (data[pos] & 0x20) ? TRUE : FALSE;
                                        /* bit 5 */
    deliver->user_data_header_present = (data[pos] & 0x40) ? TRUE : FALSE;
                                        /* bit 6 */
    deliver->reply_path_present       = (data[pos] & 0x80) ? TRUE : FALSE;
                                        /* bit 7 */
    pos ++;

    /* TP-OA
    */
    i = wms_ts_decode_address( & data[pos], & deliver->address);
    if( i==0 )
    {
      MSG_ERROR_0("invalid param size in wms_ts_decode_deliver");
      return WMS_INVALID_PARM_SIZE_S;
    }
    pos += i;

    /* TP-PID
    */
    deliver->pid = (wms_pid_e_type) data[pos];
    pos ++;

    /* TP-DCS
    */
    pos += wms_ts_decode_dcs( data+pos, & deliver->dcs );

    if (deliver->dcs.msg_waiting_kind != WMS_GW_MSG_WAITING_VOICEMAIL)
    {
      if (deliver->pid == WMS_PID_RETURN_CALL)
      {
        deliver->dcs.msg_waiting        = WMS_GW_MSG_WAITING_STORE;
        deliver->dcs.msg_waiting_active = TRUE;
        deliver->dcs.msg_waiting_kind   = WMS_GW_MSG_WAITING_VOICEMAIL;
      }
    }

    /* TP-SCTS
    */
    i = wms_ts_decode_timestamp( data+pos, & deliver->timestamp );
    if ( i==0 )
    {
      MSG_ERROR_0("invalid param value in wms_ts_decode_deliver");
      return WMS_INVALID_PARM_VALUE_S;
    }
    pos += i;


    /* TP-UDL
    */
  //  deliver->user_data_len = data[pos];
    pos ++;

    /* TP-UD
    */
    i = wms_ts_decode_gw_user_data( & deliver->dcs,
                                         data[pos-1],
                                         data+pos,
                                         deliver->user_data_header_present,
                                       & deliver->user_data );

    if (i > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }

    pos += i;

    if (NULL != nRawTsLen)
    {
      *nRawTsLen = pos;
    }

    return st;
  }
} /* wms_ts_decode_deliver() */



/*=========================================================================
FUNCTION
  wms_ts_decode_deliver_report_ack

DESCRIPTION
  Decode Deliver-Report-Ack TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_deliver_report_ack
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_deliver_report_ack_s_type        * deliver_report_ack,
  uint32                                  * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data;

  if( raw_ts_data_ptr == NULL || deliver_report_ack == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP-UDHI
  */
  deliver_report_ack->user_data_header_present =
          (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-PI
  */
  deliver_report_ack->mask = data[pos];
  pos ++;

  if( deliver_report_ack->mask & WMS_TPDU_MASK_PID )
  {
    /* TP-PID
    */
    deliver_report_ack->pid = (wms_pid_e_type) data[pos];
    pos ++;
  }

  if( deliver_report_ack->mask & WMS_TPDU_MASK_DCS )
  {
    /* TP-DCS
    */
    pos += wms_ts_decode_dcs( data+pos, & deliver_report_ack->dcs );
  }

  if( deliver_report_ack->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /* TP-UDL
    */
//    deliver_report_ack->user_data_len = data[pos];
    pos ++;

    /* TP-UD
    */
    i = wms_ts_decode_gw_user_data( & deliver_report_ack->dcs,
                                         data[pos-1],
                                         data+pos,
                                         deliver_report_ack->user_data_header_present,
                                       & deliver_report_ack->user_data );

    if (i > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }

    pos += i;
  }

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_deliver_report_ack() */


/*=========================================================================
FUNCTION
  wms_ts_decode_deliver_report_error

DESCRIPTION
  Decode Deliver-Report-Error TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_deliver_report_error
(
  const wms_raw_ts_data_s_type              * raw_ts_data_ptr,
  wms_gw_deliver_report_error_s_type        * deliver_report_error,
  uint32                                    * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data;

  if( raw_ts_data_ptr == NULL || deliver_report_error == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP-UDHI
  */
  deliver_report_error->user_data_header_present =
         (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-FCS
  */
  deliver_report_error->tp_cause = (wms_tp_cause_e_type) data[pos];
  pos ++;

  /* TP-PI
  */
  deliver_report_error->mask = data[pos];
  pos ++;

  if( deliver_report_error->mask & WMS_TPDU_MASK_PID )
  {
    /* TP-PID
    */
    deliver_report_error->pid = (wms_pid_e_type) data[pos];
    pos ++;
  }

  if( deliver_report_error->mask & WMS_TPDU_MASK_DCS )
  {
    /* TP-DCS
    */
    pos += wms_ts_decode_dcs( data+pos, & deliver_report_error->dcs );
  }

  if( deliver_report_error->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /* TP-UDL
    */
//    deliver_report_error->user_data_len = data[pos];
    pos ++;

    /* TP-UD
    */
    i = wms_ts_decode_gw_user_data( & deliver_report_error->dcs,
                                         data[pos-1],
                                         data+pos,
                                         deliver_report_error->user_data_header_present,
                                       & deliver_report_error->user_data );

    if (i > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }

    pos += i;
  }

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_deliver_report_error() */

/*=========================================================================
FUNCTION
  wms_ts_decode_status_report

DESCRIPTION
  Decode Status Report TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_status_report
(
  const wms_raw_ts_data_s_type              * raw_ts_data_ptr,
  wms_gw_status_report_s_type               * status_report,
  uint32                                    * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data;

  if( raw_ts_data_ptr == NULL || status_report == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP_UDHI:
  */
  status_report->more = data[pos] & 0x04? FALSE:TRUE;/*bit 2*/
  status_report->status_report_qualifier = data[pos]  & 0x20 ? TRUE:FALSE;
      /*bit 5*/
  status_report->user_data_header_present =
         (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-MR
  */
  status_report->message_reference = data[pos];
  pos ++;

  /* TP-RA
  */
  i = wms_ts_decode_address( & data[pos], & status_report->address);
  if( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;

  /* TP-SCTS
  */
  i = wms_ts_decode_timestamp( data+pos, & status_report->timestamp);
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-DT*/
  i = wms_ts_decode_timestamp( data+pos, & status_report->discharge_time);
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /*TP-ST*/
  status_report->tp_status = (wms_tp_status_e_type) data[pos];
  pos++;

  status_report->mask = 0;
  /*TP-PID*/
  status_report->pid = (wms_pid_e_type) 0;
  memset(&status_report->dcs, 0, sizeof(wms_gw_dcs_s_type));
  status_report->user_data.sm_len = 0;

  // Check if optional TP-PI is present
  if( pos <  raw_ts_data_ptr->len )
  {
     /*TP-PI*/
     status_report->mask = data[pos];
     pos ++;
     /* to extract last byte */
     status_report->mask &= 0xFF;

     /* to check for valid mask */
     if ((status_report->mask != 0xFF)
         && (status_report->mask != 0))
     {
       if( status_report->mask & WMS_TPDU_MASK_PID )
       {
          status_report->pid = (wms_pid_e_type) data[pos];
          pos ++;
       }

       /* TP-DCS
       */
       if( status_report->mask & WMS_TPDU_MASK_DCS )
       {
         pos += wms_ts_decode_dcs( data+pos, & status_report->dcs );
       }

       if( status_report->mask & WMS_TPDU_MASK_USER_DATA )
       {
         /* TP-UDL
         */
         pos ++;
         /* TP-UD
         */
         i = wms_ts_decode_gw_user_data( & status_report->dcs,
                                            data[pos-1],
                                            data+pos,
                                            status_report->user_data_header_present,
                                            & status_report->user_data );

         if (i > WMS_SMS_UDL_MAX_8_BIT)
         {
           MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
           st = WMS_INVALID_USER_DATA_SIZE_S;
         }

         pos += i;
       }
     }
     else
     {
       status_report->mask = 0;
     }
  }

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_status_report() */

/*=========================================================================
FUNCTION
  wms_ts_decode_command

DESCRIPTION
  Decode Command TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_command
(
  const wms_raw_ts_data_s_type              * raw_ts_data_ptr,
  wms_gw_command_s_type                     * command,
  uint32                                    * nRawTsLen
)
{
  wms_status_e_type   st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data;

  if( raw_ts_data_ptr == NULL || command == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP_UDHI:
  */
  command ->status_report_enabled  =
         (data[pos] & 0x20) ? TRUE : FALSE; /* bit 5 */

  command->user_data_header_present =
         (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-MR
  */
  command->message_reference = data[pos];
  pos ++;

  /* TP-PID
  */
  command->pid = (wms_pid_e_type) data[pos];
  pos++;

  /*TP-CT
  */
  command->command = (wms_gw_command_e_type) data[pos];
  pos ++;

  /*TP-MN
  */
  command->message_number = data[pos];
  pos ++;

  /* TP-DA
  */
  i = wms_ts_decode_address( & data[pos], & command->address);
  if( i==0)
  {
    return WMS_INVALID_PARM_SIZE_S;
  }

  pos += i;

  /* TP-CDL
  */
  command->command_data_len = MIN (data[pos], WMS_GW_COMMAND_DATA_MAX);
  pos ++;

  /*TP-CD
  */
  (void)memscpy( command->command_data,
                 command->command_data_len,
                 data + pos,
                 command->command_data_len);

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_command() */

/*=========================================================================
FUNCTION
  wms_ts_decode_submit

DESCRIPTION
  Decode Submit TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_submit
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_submit_s_type                    * submit,
  uint32                                  * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8           *data;

  if( raw_ts_data_ptr == NULL || submit == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP-RD, TP-VPF, TP-SRR, TP_UDHI, TP-RP:
  */
  submit->reject_duplicates         = (data[pos] & 0x04) ? TRUE : FALSE;
                                      /* bit 2 */
  submit->validity.format           = (wms_gw_validity_format_e_type) (( data[pos] & 0x18 ) >> 3); /* bits 3, 4 */
  submit->status_report_enabled     = (data[pos] & 0x20) ? TRUE : FALSE;
                                      /* bit 5 */
  submit->user_data_header_present  = (data[pos] & 0x40) ? TRUE : FALSE;
                                      /* bit 6 */
  submit->reply_path_present        = (data[pos] & 0x80) ? TRUE : FALSE;
                                      /* bit 7 */
  pos ++;


  /* TP-MR
  */
  submit->message_reference = data[pos];
  pos ++;

  /* TP-DA
  */
  i = wms_ts_decode_address( & data[pos], & submit->address );
  if( i == 0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }

  pos += i;

  /* TP-PID
  */
  submit->pid = (wms_pid_e_type) data[pos];
  pos ++;

  /* TP-DCS
  */
  pos += wms_ts_decode_dcs( data+pos, & submit->dcs );

  /* TP-VP
  */
  i = wms_ts_decode_gw_validity( data+pos, & submit->validity );
  if ((submit->validity.format != WMS_GW_VALIDITY_NONE) && ( i == 0 ))
  {
    return WMS_INVALID_PARM_VALUE_S;
  }

  pos += i;

  /* TP-UDL
  */
//  submit->user_data_len = data[pos];
  pos ++;

  /* TP-UD
  */
  i = wms_ts_decode_gw_user_data( & submit->dcs,
                                       data[pos-1],
                                       data+pos,
                                       submit->user_data_header_present,
                                     & submit->user_data );

  if (i > WMS_SMS_UDL_MAX_8_BIT)
  {
    MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
    st = WMS_INVALID_USER_DATA_SIZE_S;
  }

  pos += i;

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_submit() */


/*=========================================================================
FUNCTION
  wms_ts_decode_submit_report_ack

DESCRIPTION
  Decode Submit-Report-Ack TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_submit_report_ack
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_submit_report_ack_s_type         * submit_report_ack,
  uint32                                  * nRawTsLen
)
{
  wms_status_e_type     st = WMS_OK_S;
  uint32                pos = 0, i;
  const uint8          *data;

  if( raw_ts_data_ptr == NULL || submit_report_ack == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP_UDHI:
  */
  submit_report_ack->user_data_header_present  =
            (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-PI
  */
  submit_report_ack->mask = data[pos];
  pos ++;

  /* TP-SCTS
  */

  i = wms_ts_decode_timestamp( data+pos, & submit_report_ack->timestamp );
  if ( i==0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;

  if( submit_report_ack->mask & WMS_TPDU_MASK_PID )
  {
    /* TP-PID
    */
    submit_report_ack->pid = (wms_pid_e_type) data[pos];
    pos ++;
  }

  if( submit_report_ack->mask & WMS_TPDU_MASK_DCS )
  {
    /* TP-DCS
    */
    pos += wms_ts_decode_dcs( data+pos, & submit_report_ack->dcs );
  }

  if( submit_report_ack->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /* TP-UDL
    */
//    submit_report_ack->user_data_len = data[pos];
    pos ++;

    /* TP-UD
    */
    i = wms_ts_decode_gw_user_data( & submit_report_ack->dcs,
                                         data[pos-1],
                                         data+pos,
                                         submit_report_ack->user_data_header_present,
                                       & submit_report_ack->user_data );
    if (i > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }

    pos += i;
  }

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_submit_report_ack() */


/*=========================================================================
FUNCTION
  wms_ts_decode_submit_report_error

DESCRIPTION
  Decode Submit-Report-Error TPDU from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_submit_report_error
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_gw_submit_report_error_s_type       * submit_report_error,
  uint32                                  * nRawTsLen
)
{
  wms_status_e_type   st = WMS_OK_S;
  uint32              pos = 0, i;
  const uint8        *data;

  if( raw_ts_data_ptr == NULL || submit_report_error == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  data = raw_ts_data_ptr->data;


  /* TP-MTI, TP_UDHI:
  */
  submit_report_error->user_data_header_present  =
         (data[pos] & 0x40) ? TRUE : FALSE; /* bit 6 */
  pos ++;

  /* TP-FCS
  */
  submit_report_error->tp_cause = (wms_tp_cause_e_type) data[pos];
  pos ++;

  /* TP-PI
  */
  submit_report_error->mask = data[pos];
  pos ++;

  /* TP-SCTS
  */
  i = wms_ts_decode_timestamp( data+pos, & submit_report_error->timestamp );
  if( i==0 )
  {
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  if( submit_report_error->mask & WMS_TPDU_MASK_PID )
  {
    /* TP-PID
    */
    submit_report_error->pid = (wms_pid_e_type) data[pos];
    pos ++;
  }

  if( submit_report_error->mask & WMS_TPDU_MASK_DCS )
  {
    /* TP-DCS
    */
    pos += wms_ts_decode_dcs( data+pos, & submit_report_error->dcs );
  }

  if( submit_report_error->mask & WMS_TPDU_MASK_USER_DATA )
  {
    /* TP-UDL
    */
//    submit_report_error->user_data_len = data[pos];
    pos ++;

    /* TP-UD
    */
    i = wms_ts_decode_gw_user_data( & submit_report_error->dcs,
                                         data[pos-1],
                                         data+pos,
                                         submit_report_error->user_data_header_present,
                                       & submit_report_error->user_data );

    if (i > WMS_SMS_UDL_MAX_8_BIT)
    {
      MSG_ERROR_1("User Data Length has exceeded capacity: UDL = %d", i);
      st = WMS_INVALID_USER_DATA_SIZE_S;
    }

    pos += i;
  }

  if (NULL != nRawTsLen)
  {
    *nRawTsLen = pos;
  }

  return st;

} /* wms_ts_decode_submit_report_error() */

#endif /* FEATURE_GWSMS */


#ifdef FEATURE_GWSMS_BROADCAST
wms_status_e_type wms_ts_decode_gw_cb
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr, /* IN */
  wms_gw_cb_ts_data_s_type                * client_ts_ptr    /* OUT */
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_gw_user_data_s_type   *gw_user_data = NULL;

  if( raw_ts_data_ptr == NULL || client_ts_ptr == NULL )
  {
    st = WMS_NULL_PTR_S;
  }
  else if( raw_ts_data_ptr->format != WMS_FORMAT_GW_CB ||
           raw_ts_data_ptr->len > WMS_GW_CB_PAGE_SIZE ||
           raw_ts_data_ptr->len < WMS_GW_CB_PAGE_HEADER_LEN)
  {
    st = WMS_INVALID_CB_DATA_S;
  }
  else if( wms_ts_decode_gw_cb_page_header( (const uint8*)raw_ts_data_ptr->data,
                                          & client_ts_ptr->cb_page_hdr ) == 0 )
  {
    st = WMS_INVALID_CB_DATA_S;
  }
  else
  {
    /* Process user data part */
    client_ts_ptr->user_data.num_headers = 0;

    switch( client_ts_ptr->cb_page_hdr.cb_dcs.alphabet )
    {
      case WMS_GW_ALPHABET_7_BIT_DEFAULT:
        gw_user_data = (wms_gw_user_data_s_type*) wms_mem_malloc(sizeof(wms_gw_user_data_s_type));

        if(NULL == gw_user_data)
        {
           st = WMS_OUT_OF_RESOURCES_S;
           break;
        }

        gw_user_data->sm_len = wms_ts_unpack_gw_7_bit_chars
                        (
                          raw_ts_data_ptr->data + WMS_GW_CB_PAGE_HEADER_LEN,
                          (uint8)(((raw_ts_data_ptr->len - WMS_GW_CB_PAGE_HEADER_LEN) * 8) / 7),
                          WMS_MAX_LEN,
                          0, /* fill bits */
                          gw_user_data->sm_data
                        );

        if (gw_user_data->sm_len > WMS_GW_CB_MAX_PAGE_USER_DATA_LEN)
        {
          st = WMS_INVALID_CB_DATA_S;
        }
        else
        {
          /* Convert it from Number of Bytes to Number of Characters */
          gw_user_data->sm_len = (uint16)((gw_user_data->sm_len * 8) / 7);

          client_ts_ptr->user_data.sm_len = gw_user_data->sm_len;

          if( client_ts_ptr->cb_page_hdr.cb_dcs.language ==
              WMS_GW_CB_LANGUAGE_ISO_639 )
          {
            /* The first 3 chars of user data is Language */
            if( client_ts_ptr->user_data.sm_len > 3 )
            {
              client_ts_ptr->user_data.sm_len -= 3;
              (void)memscpy( client_ts_ptr->user_data.sm_data,
                             client_ts_ptr->user_data.sm_len,
                             gw_user_data->sm_data + 3,
                             client_ts_ptr->user_data.sm_len );
            }
          }
          else
          {
            if( client_ts_ptr->user_data.sm_len > 0 )
            {
              (void)memscpy( client_ts_ptr->user_data.sm_data,
                             client_ts_ptr->user_data.sm_len,
                             gw_user_data->sm_data,
                             client_ts_ptr->user_data.sm_len );
            }
          }
        }

        wms_mem_free(gw_user_data);
        break;

      default:
        /* For 8-bit and 16-bit alphabets, no further decoding done */
        client_ts_ptr->user_data.sm_len
                         = (uint16)(raw_ts_data_ptr->len - WMS_GW_CB_PAGE_HEADER_LEN);

        if( client_ts_ptr->cb_page_hdr.cb_dcs.language ==
            WMS_GW_CB_LANGUAGE_ISO_639 )
        {
          if( client_ts_ptr->user_data.sm_len > 2 )
          {
            client_ts_ptr->user_data.sm_len -= 2;
            (void)memscpy( client_ts_ptr->user_data.sm_data,
                           client_ts_ptr->user_data.sm_len,
                           raw_ts_data_ptr->data + WMS_GW_CB_PAGE_HEADER_LEN + 2,
                           client_ts_ptr->user_data.sm_len );
          }
        }
        else
        {
          if( client_ts_ptr->user_data.sm_len > 0 )
          {
            (void)memscpy( client_ts_ptr->user_data.sm_data,
                           client_ts_ptr->user_data.sm_len,
                           raw_ts_data_ptr->data + WMS_GW_CB_PAGE_HEADER_LEN,
                           client_ts_ptr->user_data.sm_len );
          }
        }
        break;
    }
  }

  return st;

} /* wms_ts_decode_gw_cb() */

#endif /* FEATURE_GWSMS_BROADCAST */



/*=========================================================================
FUNCTION wms_ts_decode_ex

DESCRIPTION
  Decode Transport Service structure from raw bytes and optionally return
  the length of the original raw data for a GWSMS message.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode_ex
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr,
  uint32                                  * pnRawTsLenRet
)
{
  // TBD: GW format only
  wms_status_e_type   st = WMS_OK_S;
#ifdef FEATURE_GWSMS
  wms_gw_pp_ts_data_s_type *msg;
  uint32                   nRawTsLenRet = 0;
#endif /* FEATURE_GWSMS */

  if( raw_ts_data_ptr == NULL || client_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

#ifdef FEATURE_GWSMS
  msg = & client_ts_data_ptr->u.gw_pp;
  if ( NULL == msg )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }
#endif /* FEATURE_GWSMS */


  wms_ts_init();
  qc_enter_crit_sect(&encode_decode_data_crit_sect);

  switch( raw_ts_data_ptr->format )
  {

#ifdef FEATURE_CDSMS

    case WMS_FORMAT_CDMA:
    case WMS_FORMAT_ANALOG_AWISMS:
    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
    case WMS_FORMAT_MWI:
      st = wms_ts_decode_bearer_data( raw_ts_data_ptr,
                                      & client_ts_data_ptr->u.cdma );
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
    case WMS_FORMAT_GW_PP:

      /* ---------- Start decoding --------- */

      msg->tpdu_type = raw_ts_data_ptr->tpdu_type;

      switch( msg->tpdu_type )
      {
        case WMS_TPDU_DELIVER:
        case WMS_TPDU_RESERVED:
          st = wms_ts_decode_deliver( raw_ts_data_ptr,
                                      &msg->u.deliver,
                                      &nRawTsLenRet );
          break;


        case WMS_TPDU_DELIVER_REPORT_ACK:
          st = wms_ts_decode_deliver_report_ack( raw_ts_data_ptr,
                                                 &msg->u.deliver_report_ack,
                                                 &nRawTsLenRet );
          break;

        case WMS_TPDU_DELIVER_REPORT_ERROR:
          st = wms_ts_decode_deliver_report_error( raw_ts_data_ptr,
                                                   &msg->u.deliver_report_error,
                                                   &nRawTsLenRet );
          break;

        case WMS_TPDU_SUBMIT:
          st = wms_ts_decode_submit( raw_ts_data_ptr,
                                     &msg->u.submit,
                                     &nRawTsLenRet );
          break;


        case WMS_TPDU_SUBMIT_REPORT_ACK:
          st = wms_ts_decode_submit_report_ack( raw_ts_data_ptr,
                                                &msg->u.submit_report_ack,
                                                &nRawTsLenRet );
          break;


        case WMS_TPDU_SUBMIT_REPORT_ERROR:
          st = wms_ts_decode_submit_report_error( raw_ts_data_ptr,
                                                  &msg->u.submit_report_error,
                                                  &nRawTsLenRet );

          break;


        case WMS_TPDU_STATUS_REPORT:
          st = wms_ts_decode_status_report( raw_ts_data_ptr,
                                            &msg->u.status_report,
                                            &nRawTsLenRet );
          break;

        case WMS_TPDU_COMMAND:
          st = wms_ts_decode_command( raw_ts_data_ptr,
                                      &msg->u.command,
                                      &nRawTsLenRet );
          break;

        default:
          MSG_ERROR_1("Invalid TPDU type %d", msg->tpdu_type);
          st = WMS_INVALID_TPDU_TYPE_S;
          break;
      }

      /* ---------- End of decoding --------- */
      break;

#ifdef FEATURE_GWSMS_BROADCAST
      case WMS_FORMAT_GW_CB:
        st = wms_ts_decode_gw_cb( raw_ts_data_ptr,
                                  & client_ts_data_ptr->u.gw_cb );
        break;
#endif /* FEATURE_GWSMS_BROADCAST */

#endif /* FEATURE_GWSMS */

      default:
        st = WMS_INVALID_FORMAT_S;
        MSG_ERROR_1("Invalid format: %d", raw_ts_data_ptr->format);
        break;
  }

  client_ts_data_ptr->format = raw_ts_data_ptr->format;

#ifdef FEATURE_GWSMS
  if (NULL != pnRawTsLenRet)
  {
    *pnRawTsLenRet = nRawTsLenRet;
  }
#endif /* FEATURE_GWSMS */

  qc_leave_crit_sect(&encode_decode_data_crit_sect);

  return st;

} /* wms_ts_decode_ex() */

/*=========================================================================
FUNCTION wms_ts_decode

DESCRIPTION
  Decode Transport Service structure from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr
)
{
  return wms_ts_decode_ex(raw_ts_data_ptr, client_ts_data_ptr, NULL);
} /* wms_ts_decode() */


#ifdef FEATURE_GWSMS

/*
*/
boolean wms_ts_decode_smsp
(
  const uint8               * data,
  uint8                     rec_len,
  wms_gw_template_s_type  * tptr
)
{
  uint8   pos = 0;

  if (data == NULL || tptr == NULL)
  {
    MSG_ERROR_0("null pointers in wms_ts_decode_smsp()");
    return FALSE;
  }

  /* WMS_GW_TEMPLATE_MIN is the minimal record length for the file EF-SMSP */
  if( rec_len < WMS_GW_TEMPLATE_MIN)
  {
    return FALSE;
  }

  /* Alpha ID
  */
  /* find out the max len of alpha id */
  tptr->alpha_id.len = MIN( rec_len - WMS_GW_TEMPLATE_MIN, WMS_ALPHA_ID_MAX );
  /* clear out the output */
  memset( tptr->alpha_id.data, ' ', tptr->alpha_id.len );
  /* find out the actual len of alpha id */
  for( ; tptr->alpha_id.len > 0; tptr->alpha_id.len -- )
  {
    if( data[tptr->alpha_id.len -1] != 0xFF )
    {
      break;
    }
  }
  /* copy alpha id according to the actual len */
  (void)memscpy( tptr->alpha_id.data,
                 tptr->alpha_id.len,
                 data,
                 tptr->alpha_id.len );

  pos = rec_len - WMS_GW_TEMPLATE_MIN;


  /* Parameter Indicators
  */
  if (pos > (WMS_ALPHA_ID_MAX - WMS_GW_TEMPLATE_MIN) || data[pos] == 0) /*Invalid Template*/
  {
    return FALSE;
  }

  tptr->mask = ~ data[pos];
  pos ++;

  /* TP-DA
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR )
  {
    /*First Byte is the Number of Digits*/
    /*Max number of digits possible in 10 bytes is 20*/
    if( (data[pos] > 20) || (data[pos] == 0) )
    {
      /* As per spec 23.040 (section 9.2.2.4), the address length may be set to zero
         indicating that the Address-Value fields are not present.                        
                                                                                         
         Skip the Dest Addr field if its length is invalid, move on to decode the SC Addr.
      */
      MSG_HIGH_1("Skipping smsp Dest Address with len not in range 1-20 (len=%d)", data[pos]);
    }
    else if( !wms_ts_decode_address( data+pos, & tptr->dest_addr ))
    {
      MSG_ERROR_0("Failed to decode Dest Addr");
      return FALSE;
    }
  }
  pos += 12;

  /* TS-SCAddr
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_SC_ADDR )
  {
    if(data[pos] > 12)/*First Byte is the number of bytes*/
    {
      return FALSE;
    }

    if( !wms_ts_decode_sc_address( data+pos, & tptr->sc_addr ))
    {
      return FALSE;
    }
  }
  pos += 12;

  /* TP-PID
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_PID )
  {
    tptr->pid = (wms_pid_e_type) data[pos];
  }
  pos += 1;

  /* TP-DCS
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_DCS )
  {
    (void)wms_ts_decode_dcs( data+pos, & tptr->dcs );
  }
  pos += 1;

  /* TP-VP-Relative
  */
  if( pos < WMS_ALPHA_ID_MAX && tptr->mask & WMS_GW_TEMPLATE_MASK_VALIDITY )
  {
    wms_ts_decode_relative_time( data[pos], & tptr->relative_validity );
  }
  pos += 1;

  /* done */
  return TRUE;

} /* wms_ts_decode_smsp() */




/*
*/
boolean wms_ts_encode_smsp
(
  const wms_gw_template_s_type  * tptr,
  uint8                           rec_len,
  uint8                           * data
)
{
  uint8   pos = 0, i;

  if (data == NULL || tptr == NULL)
  {
    MSG_ERROR_0("null pointers in wms_ts_decode_smsp()");
    return FALSE;
  }


  if( rec_len < 28 )
  {
    return FALSE;
  }

  memset( data, 0xFF, rec_len);

  /* Alpha ID
  */
  if (tptr->alpha_id.len > 0)
  {
    (void)memscpy( data,
                   MIN( tptr->alpha_id.len, rec_len ),
                   tptr->alpha_id.data,
                   MIN( tptr->alpha_id.len, rec_len ) );
  }

  pos = rec_len - 28;

  /* Parameter Indicators
  */
  if (tptr->mask == 0) /* Invalid Template */
  {
    return FALSE;
  }
  data[pos] = ~ tptr->mask;
  pos ++;

  /* TP-DA
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR )
  {
    i = wms_ts_encode_address( & tptr->dest_addr, data+pos );
    if( i==0 )
    {
      return FALSE;
    }
  }
  pos += 12;

  /* TS-SCAddr
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_SC_ADDR )
  {
    i = wms_ts_encode_sc_address( & tptr->sc_addr, data+pos );
    if( i==0 )
    {
      return FALSE;
    }
  }
  pos += 12;

  /* TP-PID
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_PID )
  {
    data[pos] = (uint8)tptr->pid;
  }
  pos += 1;

  /* TP-DCS
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_DCS )
  {
    (void)wms_ts_encode_dcs( & tptr->dcs, data+pos );
  }
  pos += 1;

  /* TP-VP-Relative
  */
  if( tptr->mask & WMS_GW_TEMPLATE_MASK_VALIDITY )
  {
    data[pos] = wms_ts_encode_relative_time( & tptr->relative_validity );
  }
  pos += 1;


  /* done
  */
  return TRUE;

} /* wms_ts_encode_smsp() */

#endif /* FEATURE_GWSMS */



void wms_ts_decode_relative_time
(
  uint8                     v,
  wms_timestamp_s_type    * timestamp
)
{
  uint32                  i;

  if (timestamp != NULL)
  {
    wms_ts_init();
    qc_enter_crit_sect(&encode_decode_data_crit_sect);

    memset( (void*) timestamp, 0, sizeof(wms_timestamp_s_type) );

    if( v < 144 )
    {
      i = ( v + 1 ) * 5; /* minutes */
      timestamp->hour    = (uint8)wms_ts_int_to_bcd((uint8)(i/60));
      timestamp->minute  = (uint8)wms_ts_int_to_bcd((uint8)(i % 60));
    }
    else if (v < 167)
    {
      i = ( v - 143 ) * 30; /* minutes */
      timestamp->hour    = (uint8)wms_ts_int_to_bcd((uint8)(12 + i/60));
      timestamp->minute  = (uint8)wms_ts_int_to_bcd((uint8)(i % 60));
    }
    else if (v < 197)
    {
      i = v - 166; /* days */
      timestamp->month    = (uint8)wms_ts_int_to_bcd((uint8)(i/30));
      timestamp->day      = (uint8)wms_ts_int_to_bcd((uint8)(i % 30));
    }
    else
    {
      i = ( v - 192 ) * 7; /* days */
      timestamp->year     = (uint8)wms_ts_int_to_bcd((uint8)(i/365));
      timestamp->month    = (uint8)wms_ts_int_to_bcd((uint8)((i%365)/30));
      timestamp->day      = (uint8)wms_ts_int_to_bcd((uint8)(( i % 365 ) % 30));
    }

    qc_leave_crit_sect(&encode_decode_data_crit_sect);
  }
  else
  {
    MSG_ERROR_0("null pointer in wms_ts_decode_relative_time");
  }
  /* done */
  return;

} /* wms_ts_decode_relative_time() */



uint8 wms_ts_encode_relative_time
(
  const wms_timestamp_s_type  *timestamp
)
{
  uint32    i;
  uint8     v = 0, j;
  /* round up to the next time unit boundary*/

  if (timestamp != NULL)
  {
    wms_ts_init();
    qc_enter_crit_sect(&encode_decode_data_crit_sect);

    if ( !wms_ts_bcd_to_int(timestamp->year, &j) )
    {
      MSG_ERROR_1("Year is invalid: %d", j);
    }
    i = j * 365;

    if ( !wms_ts_bcd_to_int(timestamp->month, &j) )
    {
      MSG_ERROR_1("Month is invalid: %d", j);
    }
    i = i + j * 30;

    if ( !wms_ts_bcd_to_int(timestamp->day, &j) )
    {
      MSG_ERROR_1("Day is invalid: %d", j);
    }
    i += j;

    if( i > 30 )
    {
      /* 197 - 255: (TP-VP - 192) weeks */
      v = (uint8) ( (i+6) / 7 + 192 );
    }
    else if( i >= 1 )
    {
      /* 168 to 196: (TP-VP - 166 ) days */
      v = (uint8) ( i + 166 );
    }
    else
    {
      if ( !wms_ts_bcd_to_int(timestamp->day, &j) )
      {
        MSG_ERROR_1("Day is invalid: %d", j);
      }
      i = j * 24 * 60;

      if ( !wms_ts_bcd_to_int(timestamp->hour, &j) )
      {
        MSG_ERROR_1("Hour is invalid: %d", j);
      }
      i = i + j * 60;

      if ( !wms_ts_bcd_to_int(timestamp->minute, &j) )
      {
        MSG_ERROR_1("Minute is invalid: %d", j);
      }
      i += j;

      if( i > 12 * 60 ) /* greater than 12 hours */
      {
        /* 144 - 167: 12 hours + ( (TP-VP - 143) * 30 minutes ) */
        v = (uint8) (((i - 12 * 60) + 29) / 30 + 143);
      }
      else
      {
        /* 0 - 143: (TP-VP + 1) * 5 minutes */
        v = (uint8) ( ( i + 4 ) / 5 - 1 );
      }
    }
    qc_leave_crit_sect(&encode_decode_data_crit_sect);
  }
  else
  {
    MSG_ERROR_0("null pointer in wms_ts_encode_relative_time");
  }
  /* done */
  return v;

} /* wms_ts_encode_relative_time() */

/*=========================================================================
FUNCTION
wms_ts_pack_gw_user_data

DESCRIPTION
  Packs only the user data for a GW Message

DEPENDENCIES
  None

RETURN VALUE
  status of the conversion

SIDE EFFECTS
  Modifies raw_ts_data_ptr directly

=========================================================================*/
wms_status_e_type wms_ts_pack_gw_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
)
{
#ifdef FEATURE_GWSMS
  wms_status_e_type   st = WMS_OK_S;

  uint32                i, pos = 0;
  const uint8           *data;
  wms_gw_submit_s_type  *submit = NULL;
  wms_client_ts_data_s_type   *client_ts_ptr = NULL;

  if( raw_ts_data_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  client_ts_ptr = (wms_client_ts_data_s_type*) wms_mem_malloc(sizeof(wms_client_ts_data_s_type));

  if(NULL == client_ts_ptr)
  {
     return WMS_OUT_OF_RESOURCES_S;
  }

  submit = &client_ts_ptr->u.gw_pp.u.submit;

  data = raw_ts_data_ptr->data;

  /* TP-MTI, TP-RD, TP-VPF, TP-SRR, TP_UDHI, TP-RP:
  */
  submit->reject_duplicates         = (data[pos] & 0x04) ? TRUE : FALSE;
                                      /* bit 2 */
  submit->validity.format           = (wms_gw_validity_format_e_type) (( data[pos] & 0x18 ) >> 3); /* bits 3, 4 */
  submit->status_report_enabled     = (data[pos] & 0x20) ? TRUE : FALSE;
                                      /* bit 5 */
  submit->user_data_header_present  = (data[pos] & 0x40) ? TRUE : FALSE;
                                      /* bit 6 */
  submit->reply_path_present        = (data[pos] & 0x80) ? TRUE : FALSE;
                                      /* bit 7 */
  pos ++;


  /* TP-MR
  */
  submit->message_reference = data[pos];
  pos ++;

  /* TP-DA
  */
  i = wms_ts_decode_address( & data[pos], & submit->address );
  if (i==0)
  {
    wms_mem_free(client_ts_ptr);
    return WMS_INVALID_PARM_SIZE_S;
  }
  pos += i;


  /* TP-PID
  */
  submit->pid = (wms_pid_e_type) data[pos];
  pos ++;

  /* TP-DCS
  */
  pos += wms_ts_decode_dcs( data+pos, & submit->dcs );

  /* TP-VP
  */
  i = wms_ts_decode_gw_validity( data+pos, & submit->validity );
  if ((submit->validity.format != WMS_GW_VALIDITY_NONE) && ( i == 0 ))
  {
    wms_mem_free(client_ts_ptr);
    return WMS_INVALID_PARM_VALUE_S;
  }
  pos += i;

  /* TP-UDL
  */

  /* TP-UD
  */
  submit->user_data.sm_len = MIN (data[pos], WMS_MAX_LEN);
  pos ++;

  submit->user_data.num_headers = 0; /*Reset the Number of Headers*/

  (void)memscpy( submit->user_data.sm_data,
                 submit->user_data.sm_len,
                 &data[pos],
                 submit->user_data.sm_len );



  pos+= submit->user_data.sm_len;
  /* Set the global raw ts data len
  */

  st = wms_ts_encode_submit( submit, raw_ts_data_ptr );

  wms_mem_free(client_ts_ptr);

  return st;
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */
}

#ifdef FEATURE_GWSMS
#ifdef FEATURE_GWSMS_BROADCAST


uint8 wms_ts_decode_gw_cb_page_header
(
  const uint8                   * data,
  wms_gw_cb_page_header_s_type  * page_header_ptr
)
{
  uint8             pos = 0;

  if( data == NULL || page_header_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return 0;
  }


  /* Copy the original Serial Number data */
  page_header_ptr->raw_serial_number = (data[pos] << 8) | data[pos+1];

  page_header_ptr->geo_scope      =
                    (wms_gw_cb_geo_scope_e_type) ( (data[pos] & 0xC0) >> 6 );

  page_header_ptr->message_code   = ( data[pos] & 0x30 ) << 4;
  page_header_ptr->message_code  |= ( data[pos] & 0x0F ) << 4;
  pos ++;
  page_header_ptr->message_code  |= ( data[pos] & 0xF0 ) >> 4;

  page_header_ptr->update_number  = data[pos] & 0x0F;
  pos ++;

  page_header_ptr->cb_srv_id      = data[pos] << 8;
  pos ++;
  page_header_ptr->cb_srv_id     |= data[pos];
  pos ++;

  if( wms_ts_decode_gw_cb_dcs( data[pos],
                               data+WMS_GW_CB_PAGE_HEADER_LEN,
                               & page_header_ptr->cb_dcs ) != 0 )
  {
    pos ++;
    page_header_ptr->page_number  = (data[pos] & 0xF0) >> 4;
    page_header_ptr->total_pages  = data[pos] & 0x0F;
    pos ++;

    /* treat as single message if either field is 0000 */
    if (page_header_ptr->page_number == 0 || page_header_ptr->total_pages == 0)
    {
      page_header_ptr->page_number = 0x0001;
      page_header_ptr->total_pages = 0x0001;
    }
    if (page_header_ptr->page_number > page_header_ptr->total_pages)
    {
      MSG_ERROR_2( "Invalid GW CB DCS, page number > total pages, %d > %d",
                   page_header_ptr->page_number,
                   page_header_ptr->total_pages );
      pos = 0;
    }
  }
  else
  {
    pos = 0;
  }

  return pos;

} /* wms_ts_decode_gw_cb_page_header() */


uint8 wms_ts_encode_gw_cb_serial_number
(
  wms_gw_cb_geo_scope_e_type  geo_scope,
  uint16                      message_code,
  uint8                       update_number,
  uint16                      * result
)
{
  uint8  pos = 0;

  if( result == NULL )
  {
    wms_check_for_null_ptr();
    return 0;
  }


  result[pos]    = ((uint8)geo_scope & 0x03) << 6;
  result[pos]   |= (message_code & 0x03F0) >> 4;
  pos ++;

  result[pos]    = (message_code & 0x0F ) << 4;
  result[pos]   |= update_number & 0x0F;
  pos ++;

  return pos;
}

#endif /* FEATURE_GWSMS_BROADCAST */
#endif/*FEATURE_GWSMS*/

uint8 wms_ts_decode_gw_cb_dcs
(
  const uint8                  data,            /* INPUT */
  const uint8                  * user_data_ptr, /* INPUT */
  wms_gw_cb_dcs_s_type         * dcs_ptr        /* OUTPUT */
)
{
  uint32  i;
  uint8 pos = 0;

  if( dcs_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return 0;
  }


  dcs_ptr->group         = WMS_GW_CB_DCS_GROUP_RESERVED;
  dcs_ptr->msg_class         = WMS_MESSAGE_CLASS_NONE;
  dcs_ptr->is_compressed = FALSE;
  dcs_ptr->alphabet      = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  dcs_ptr->language      = WMS_GW_CB_LANGUAGE_UNSPECIFIED;
  memset( dcs_ptr->iso_639_lang, 0, sizeof(dcs_ptr->iso_639_lang) );
  dcs_ptr->is_udh_present = FALSE;

  i = (data & 0xF0) >> 4;

  switch( i )
  {
    case 0:
    case 2:
    case 3:
      dcs_ptr->group         = WMS_GW_CB_DCS_GROUP_DEFINED;
      dcs_ptr->language      = (wms_gw_cb_language_e_type) data;
      break;

    case 1:
      if( data == 0x10 || data == 0x11 )
      {
        dcs_ptr->group         = WMS_GW_CB_DCS_GROUP_DEFINED;
        dcs_ptr->alphabet      = (data & 0x01) ?
                        WMS_GW_ALPHABET_UCS2 : WMS_GW_ALPHABET_7_BIT_DEFAULT;
        dcs_ptr->language      = WMS_GW_CB_LANGUAGE_ISO_639;

        if (user_data_ptr != NULL)
        {
          dcs_ptr->iso_639_lang[0]  = user_data_ptr[0] & 0x7F;
          dcs_ptr->iso_639_lang[1]  = ( user_data_ptr[0] & 0x80 ) >> 7;
          dcs_ptr->iso_639_lang[1] |= ( user_data_ptr[1] & 0x3F ) << 1;
          dcs_ptr->iso_639_lang[2]  = 13; /* CR char in GSM 7-bit Alphabet */
        }
        else
        {
          MSG_ERROR_0("No User Data Ptr for decoding ISO 639 language");

          /* Default it to English if user_data_ptr is not passed */
          dcs_ptr->iso_639_lang[0] = 0x45;  /* E */
          dcs_ptr->iso_639_lang[1] = 0x4E;  /* N */
          dcs_ptr->iso_639_lang[2] = 13;    /* CR */
        }
      }
      break;

    case 4:
    case 5:
    case 6:
    case 7:
      dcs_ptr->group         = WMS_GW_CB_DCS_GROUP_DEFINED;
      if( data & 0x10 )
      {
        dcs_ptr->msg_class         = (wms_message_class_e_type) (data & 0x03);
      }
      else
      {
        dcs_ptr->msg_class         = WMS_MESSAGE_CLASS_NONE;
      }
      dcs_ptr->is_compressed = (data & 0x20) >> 5;
      dcs_ptr->alphabet      = (wms_gw_alphabet_e_type) ((data & 0x0C) >> 2);
      break;

    case 14:
      dcs_ptr->group = WMS_GW_CB_DCS_GROUP_WAP;
      break;

    case 15:
      dcs_ptr->group         = WMS_GW_CB_DCS_GROUP_DEFINED;
      dcs_ptr->msg_class         = (wms_message_class_e_type) (data & 0x03);
      if (WMS_MESSAGE_CLASS_0 == dcs_ptr->msg_class)
      {
        dcs_ptr->msg_class = WMS_MESSAGE_CLASS_NONE;
      }
      dcs_ptr->alphabet      = (wms_gw_alphabet_e_type) ((data & 0x04) >> 2);
      break;

    default:
      /* case 8, 9, 10, 11, 12, 13, reserved coding groups */
      MSG_HIGH_0("gw cb dcs, reserved coding groups");
      break;
  }

  if( dcs_ptr->alphabet > WMS_GW_ALPHABET_UCS2 )
  {
    dcs_ptr->alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  }

  dcs_ptr->raw_dcs_data = data;
  pos ++;

  return pos;

} /* wms_ts_decode_gw_cb_dcs() */


/*=========================================================================
FUNCTION
  wms_ts_decode_etws_primary_notification

DESCRIPTION
  Decodes a Earthquake and Tsunami Warninng System (ETWS) primary notification
  from raw bytes.

  @param *data         [IN]  Pointer to raw ETWS message.
  @param data_len      [IN]  Length of raw ETWS message.
  @param *etws_msg_ptr [OUT] Pointer to the output ETWS primary notification buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_INVALID_PARM_SIZE_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
===========================================================================*/

wms_status_e_type wms_ts_decode_etws_primary_notification
(
  uint8                                *data,             /* IN */
  uint32                                data_len,         /* IN */
  wms_etws_primary_notification_s_type *etws_msg_ptr      /* OUT */
)
{
  uint8 pos = 0;
  if (NULL == data || NULL == etws_msg_ptr)
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }
  /* 1-2 Serial Number
   * ___________________________________________________
   * Octet 1                |     Octet 2
   * _______________________|___________________________
   * b7 b6|b5 b4 b3 b2 b1 b0|b7 b6 b5 b4 b3 b2 b1 b0
   * GS   |    Message Code             |Update Number
   * _____|_____________________________|_______________
   */

  /* length has to be 6(without signature) or 56(with signature)*/
  if( 6 == data_len || 56 == data_len )
  {
    etws_msg_ptr->cb_header_info.geo_scope = (wms_gw_cb_geo_scope_e_type)( (data[pos] & 0xC0) >> 6 );
    etws_msg_ptr->cb_header_info.message_code = (data[pos] & 0x3F) << 4;/*high bytes of message_code */
    etws_msg_ptr->cb_header_info.raw_serial_number = data[pos]<<8;/* hight bytes of serial number */
    pos++;
    etws_msg_ptr->cb_header_info.message_code |= (data[pos] & 0xF0)>>4;/* low bytes of message_code */
    etws_msg_ptr->cb_header_info.update_number = data[pos] & 0x0F;
    etws_msg_ptr->cb_header_info.raw_serial_number |= data[pos];/*low bytes of serial number */
    pos++;

    // 3-4 Message Identifier
    etws_msg_ptr->cb_header_info.cb_srv_id = data[pos++] << 8;
    etws_msg_ptr->cb_header_info.cb_srv_id |= (wms_gw_cb_srv_id_type)data[pos++];

    /* 5-6 warning type
     * ___________________________________________________________
     * Octet 1                       | Octet 2
     * ______________________________|____________________________
     * b7 b6 b5 b4 b3 b2 b1|b0       |b7   |b6 b5 b4 b3 b2 b1 b0
     * Warning Type        |Emergency|Popup|
     * ____________________|_________|_____|______________________
     */
    etws_msg_ptr->warning_type = (wms_etws_warning_e_type)((data[pos] & 0xFE)>>1);
    etws_msg_ptr->activate_emergency_user_alert = (boolean)(data[pos++] &0x01);
    etws_msg_ptr->activate_popup = (boolean)((data[pos++] & 0x80) >> 7);

    etws_msg_ptr->security_info_present = FALSE;
    if( data_len ==56 )
    {

      // 7-56 Security Information
      /*-------------------------------------
        Year                        Octet1
        -------------------------------------
        Month                       Octet2
        -------------------------------------
        Day                         Octet3
        -------------------------------------
        Hour                        Octet4
        -------------------------------------
        Minute                      Octet5
        -------------------------------------
        Second                      Octet6
        -------------------------------------
        Time Zone                   Octet7
        -------------------------------------
        Digital Signature           Octet8-50
        -------------------------------------
        */
      etws_msg_ptr->security_info_present = TRUE;
      if (0 == wms_ts_decode_timestamp(&data[pos], &etws_msg_ptr->security_info.timestamp))
      {
        MSG_ERROR_0("TimeStamp decode failed");
        return WMS_GENERAL_ERROR_S;
      }
      pos +=7;
      (void)memscpy( etws_msg_ptr->security_info.digital_signature,
                     WMS_ETWS_DIGITAL_SIGNATURE_LEN,
                     &data[pos],
                     WMS_ETWS_DIGITAL_SIGNATURE_LEN);
    }
    return WMS_OK_S;
  }
  else
  {
    MSG_ERROR_1("DATA LENGH: %d has to be 6 or 56", data_len);
    return WMS_INVALID_PARM_SIZE_S;
  }
}
/*=========================================================================
FUNCTION
  wms_ts_encode_etws_primary_notification
DESCRIPTION
  Encodes a a Earthquake and Tsunami Warninng System (ETWS) primary notification
  structure into raw bytes.

  @param *etws_msg_ptr [IN]  Pointer to the ETWS primary notification buffer.
  @param *data         [IN,OUT] Pointer to the data buffer.
  @param  data_len      [IN]  Length of the ETWS message.
  @param *data_len_req [OUT] Pointer to the data length request.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
==========================================================================*/
wms_status_e_type wms_ts_encode_etws_primary_notification
(
  wms_etws_primary_notification_s_type   *etws_msg_ptr,    /* IN */
  uint8                                  *data,            /* IN-OUT */
  uint32                                  data_len,        /* IN */
  uint32                                 *data_len_req     /* OUT */
)
{
  uint32 pos = 0;
  uint8 data_buf[6 + WMS_ETWS_DIGITAL_SIGNATURE_LEN + 7]; /* 6: Serial Number +
                                                                Message Identifier
                                                                Warning Type
                                                             7: Signature-Timestamp
                                                           */

  if ( NULL == etws_msg_ptr )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  /* 1-2 Octet Serial Number */

  data_buf[pos++] = (etws_msg_ptr->cb_header_info.raw_serial_number & 0xFF00)>>8;
  data_buf[pos++] = etws_msg_ptr->cb_header_info.raw_serial_number & 0x00FF;

  /* 3-4 Message Identifier */
  data_buf[pos++] = (etws_msg_ptr->cb_header_info.cb_srv_id & 0xFF00) >> 8;
  data_buf[pos++] = etws_msg_ptr->cb_header_info.cb_srv_id & 0x00FF;

  /* 5-6 Warning Type */
  data_buf[pos] = (uint8)(((uint16)etws_msg_ptr->warning_type & 0x00FF) << 1);
  data_buf[pos] |= etws_msg_ptr->activate_emergency_user_alert;
  pos++;
  data_buf[pos] = (uint8)( (TRUE == etws_msg_ptr->activate_popup) ? 1 << 7 : 0);
  pos++;

  if( TRUE == etws_msg_ptr->security_info_present )
  {
    if (0 == wms_ts_encode_timestamp(&etws_msg_ptr->security_info.timestamp, &data_buf[pos]))
    {
      MSG_ERROR_0("TimeStamp encode failed");
      return WMS_GENERAL_ERROR_S;
    }
    pos += 7;
    (void)memscpy( &data_buf[pos],
                   WMS_ETWS_DIGITAL_SIGNATURE_LEN,
                   etws_msg_ptr->security_info.digital_signature,
                   WMS_ETWS_DIGITAL_SIGNATURE_LEN);
    pos = pos + WMS_ETWS_DIGITAL_SIGNATURE_LEN;
  }

  if (data_len_req != NULL)
  {
    *data_len_req = pos;
  }
  if( data != NULL && data_len > 0)
  {
    (void)memscpy( data,
                   MIN(data_len, pos),
                   data_buf,
                   MIN(data_len, pos));
  }

  return WMS_OK_S;
}
/*=========================================================================
FUNCTION  wms_ts_decode_etws_secondary_notification

DESCRIPTION
  Decodes a Earthquake and Tsunami Warninng System (ETWS) secondary notification
  from raw bytes.

  @param *data          [IN]  Pointer to raw ETWS secondary notification.
  @param  data_len      [IN]  Length of raw ETWS secondary notification.
  @param  format        [IN]  Format used for ETWS secondary notification.
  @param *etws_msg_ptr  [OUT] Pointer to the output ETWS secondary notification buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_INVALID_PARM_SIZE_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
==========================================================================*/
wms_status_e_type wms_ts_decode_etws_secondary_notification
(
  uint8                                  *data,             /* IN */
  uint32                                  data_len,         /* IN */
  wms_etws_notification_format_e_type     format,
  wms_etws_secondary_notification_s_type *etws_msg_ptr      /* OUT */
)

{
  uint32 pos = 0;

  uint8  page_index = 0;
  uint16 cb_data_index = 0;
  uint8  curr_page_len = 0;
  uint8  packed_bytes = 0;

  if (NULL == data || NULL == etws_msg_ptr)
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  // should at least contain one page, minimum size
  // header(6) + number_of_pages(1) + 1 page(CB_PAGE_LEN + 1) = 90
  if( data_len < 90)
  {
    MSG_ERROR_0("Invalid input data_len");
    return WMS_INVALID_PARM_VALUE_S;
  }

  if( format == WMS_ETWS_NOTIFICATION_FORMAT_UMTS )
  {
    etws_msg_ptr->format = WMS_ETWS_NOTIFICATION_FORMAT_UMTS;
    /* 1 message Type */
    pos++;
    /* 2-3 Message ID */
    etws_msg_ptr->cb_header_info.cb_srv_id = data[pos++] << 8;
    etws_msg_ptr->cb_header_info.cb_srv_id |= (wms_gw_cb_srv_id_type)data[pos++];

    /* 4-5 Serial Number */
    etws_msg_ptr->cb_header_info.geo_scope = (wms_gw_cb_geo_scope_e_type)( (data[pos] & 0xC0) >> 6 );
    etws_msg_ptr->activate_emergency_user_alert = (boolean)((data[pos] & 0x20) >> 5);
    etws_msg_ptr->activate_popup = (boolean)((data[pos] & 0x10) >> 4);
    etws_msg_ptr->cb_header_info.message_code = (data[pos] & 0x3F) << 4;
    etws_msg_ptr->cb_header_info.raw_serial_number = data[pos] << 8;
    pos++;
    etws_msg_ptr->cb_header_info.message_code |= (data[pos] & 0xF0)>>4;
    etws_msg_ptr->cb_header_info.update_number = data[pos] & 0x0F;
    etws_msg_ptr->cb_header_info.raw_serial_number |= data[pos];
    pos++;

    /* 6 Data Coding Scheme */
    if(0 == wms_ts_decode_gw_cb_dcs(data[pos++],
                               data+WMS_GW_CB_PAGE_HEADER_LEN,
                               &(etws_msg_ptr->cb_header_info.cb_dcs)) )
    {
      MSG_ERROR_0("DCS decode failed");
      return WMS_GENERAL_ERROR_S;
    }

    /* 7-N CB Data
     * 1 Number-of-Pages
     * 2-83 CBS-Message-Information-Page 1
     * 84   CBS-Message-Information-Length 1
     * .....
     *      CBS-Message-Information-Page n
     *      CBS-Message-Information-Length n
     *
     *      n<= 15
     */

    etws_msg_ptr->cb_data_len = 0;
    etws_msg_ptr->cb_header_info.total_pages = data[pos++] & 0x0F;
    etws_msg_ptr->cb_header_info.page_number = 0x01; /* INVALID SHOULD NOT USED IN UMTS */

    // pos point to first byte of each CBS page.
    for( page_index = 0; page_index < etws_msg_ptr->cb_header_info.total_pages; page_index++ )
    {
      etws_msg_ptr->page_lengths[page_index] = data[pos + CB_PAGE_LEN ];
      curr_page_len = data[pos + CB_PAGE_LEN ];

      /* Message Information Length should not exceed maximum value 82 */
      if( curr_page_len > CB_PAGE_LEN )
      {
        MSG_ERROR_1("Invalid Message Information Length: %d", curr_page_len);
        return WMS_INVALID_PARM_VALUE_S;
      }

      if( etws_msg_ptr->cb_header_info.cb_dcs.alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT )
      {
        int in_chars = (curr_page_len * 8) / 7;
        packed_bytes = wms_ts_unpack_gw_7_bit_chars(&data[pos],
                                                    (uint8)in_chars, /* number of 7-bit chars*/
                                                    93, /* maximum size of 7-bit chars after unpacking
                                                           one page cb_data is CB_PAGE_LEN(82) bytes,
                                                           can contain 82*8/7=93 7-bit chars*/
                                                    0,
                                                    &(etws_msg_ptr->cb_data[cb_data_index]));
        cb_data_index += (packed_bytes * 8 )/7;

        pos += 83;
        etws_msg_ptr->cb_data_len += (packed_bytes * 8)/7;
      }
      else
      {
        (void)memscpy( &(etws_msg_ptr->cb_data[etws_msg_ptr->cb_data_len]),
                       curr_page_len,
                       &data[pos],
                       curr_page_len);
        pos += (CB_PAGE_LEN + 1);
        etws_msg_ptr->cb_data_len += curr_page_len;
      }
    }
    return WMS_OK_S;
  }
  else
  {
    MSG_ERROR_0("Unsupported ETWS format");
    return WMS_UNSUPPORTED_S;
  }
}

/*=========================================================================
FUNCTION  wms_ts_encode_etws_secondary_notification

DESCRIPTION
  Encodes a Earthquake and Tsunami Warninng System (ETWS) secondary notification
  into raw bytes.

  @param *etws_msg_ptr [IN]  Pointer to the ETWS secondary notification buffer.
  @param *data         [IN,OUT] Pointer to the data buffer.
  @param data_len      [IN]  Length of the data buffer.
  @param *data_len_req [OUT] Pointer to the required mininum length for the data buffer.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_NULL_PTR_S
   - WMS_UNSUPPORTED_S

  @dependencies
  None.
===========================================================================*/
wms_status_e_type wms_ts_encode_etws_secondary_notification
(
  wms_etws_secondary_notification_s_type  *etws_msg_ptr,    /* IN */
  uint8                                   *data,            /* IN-OUT */
  uint32                                   data_len,        /* IN */
  uint32                                  *data_len_req     /* OUT */
)
{
  uint32 pos = 0;
  uint8 data_buf[WMS_ETWS_MAX_MSG_LEN];  /* 1 Message Type
                               * 2-3 Message ID        : 2
                               * 4-5 Serial number     : 2
                               * 6 Data Coding Scheme  : 1
                               * 7-N CB Data           : 1+ CB_PAGE_LEN(82)*15
                               * n<= 15
                               */
  uint8 page_index = 0;
  uint16 prev_pages_len = 0;
  uint16 cb_data_index = 0;
  uint16 packed_bytes = 0;
  uint8 curr_page_len = 0;

  if ( NULL == etws_msg_ptr )
  {
    wms_check_for_null_ptr();
    return WMS_NULL_PTR_S;
  }

  if( etws_msg_ptr->format == WMS_ETWS_NOTIFICATION_FORMAT_UMTS)
  {
    memset(data_buf, 0,  WMS_ETWS_MAX_MSG_LEN);
    data_buf[pos++] = 1; /* Message Type */
    data_buf[pos++] = (etws_msg_ptr->cb_header_info.cb_srv_id & 0xFF00) >> 8;
    data_buf[pos++] = etws_msg_ptr->cb_header_info.cb_srv_id & 0x00FF;

    data_buf[pos++] = (etws_msg_ptr->cb_header_info.raw_serial_number & 0xFF00) >> 8;
    data_buf[pos++] = etws_msg_ptr->cb_header_info.raw_serial_number & 0x00FF;

    data_buf[pos++] = etws_msg_ptr->cb_header_info.cb_dcs.raw_dcs_data;
    data_buf[pos++] = etws_msg_ptr->cb_header_info.total_pages;

    if(WMS_ETWS_MAX_INFO_PAGE_NUMBER < etws_msg_ptr->cb_header_info.total_pages )
    {
      MSG_ERROR_0("Total Pages is out of boundary(15)");
      return WMS_UNSUPPORTED_S;
    }
    for( page_index = 0; page_index < etws_msg_ptr->cb_header_info.total_pages; page_index ++)
    {
      curr_page_len = etws_msg_ptr->page_lengths[page_index];
      if (etws_msg_ptr->cb_header_info.cb_dcs.alphabet != WMS_GW_ALPHABET_7_BIT_DEFAULT)
      {
        for( cb_data_index = 0; cb_data_index < curr_page_len; cb_data_index ++ )
        {
          data_buf[pos++] = etws_msg_ptr->cb_data[cb_data_index + prev_pages_len];
        }
        if( CB_PAGE_LEN >= curr_page_len )
        {
          pos += (CB_PAGE_LEN - curr_page_len);
          prev_pages_len += curr_page_len;
          data_buf[pos++] = curr_page_len;
        }
      }
      else if ( cb_data_index < WMS_ETWS_MAX_USER_DATA_LEN )
      {
        packed_bytes = wms_ts_pack_gw_7_bit_chars(&etws_msg_ptr->cb_data[cb_data_index],
                                                  (etws_msg_ptr->page_lengths[page_index] * 8 / 7),
                                                   0,
                                                   93,
                                                   &data_buf[pos] );
        pos += CB_PAGE_LEN;
        data_buf[pos++] = curr_page_len;
        cb_data_index += (uint16)(packed_bytes * 8)/7;
      }
    }
    if (data_len_req != NULL)
    {
      *data_len_req = pos;
    }
    if( data != NULL && data_len > 0)
    {
      (void)memscpy( data,
                     MIN(data_len, pos),
                     data_buf,
                     MIN(data_len, pos));
    }

    return WMS_OK_S;
  }
  else
  {
    MSG_ERROR_0("Unsupported ETWS format");
    return WMS_UNSUPPORTED_S;
  }
}

/*=========================================================================
FUNCTION
  wms_ts_get_udh_length

DESCRIPTION
  Allow the client to get the header length for the given user data header.

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_get_udh_length
(
  const wms_udh_s_type                    *udh
)
{
  uint8 length = 0;
  if (udh != NULL)
  {
    switch(udh->header_id)
    {
      /* 0x00 */
      case WMS_UDH_CONCAT_8:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_CONCAT8;
        break;

      /* 0x08 */
      case WMS_UDH_CONCAT_16:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_CONCAT16;
        break;

      /* 0x01 */
      case WMS_UDH_SPECIAL_SM:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_SPECIAL_SM;
        break;

      /* 0x02 - 0x03 Reserved */

      /* 0x04 */
      case WMS_UDH_PORT_8:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PORT8;
        break;

      /* 0x05 */
      case WMS_UDH_PORT_16:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PORT16;
        break;

      /* 0x06 */
      case WMS_UDH_SMSC_CONTROL:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x07 */
      case WMS_UDH_SOURCE:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x09 */
      case WMS_UDH_WCMP:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x0A */
      case WMS_UDH_TEXT_FORMATING:
        /* Header ID Octet + Header Length Octet + Header Length */
        if(!udh->u.text_formating.is_color_present) {
          length = 1 + 1 + WMS_UDH_OCTETS_TEXT_FORMATTING;
        }
        else {
          /* Header ID Octet + Header Length Octet + Header Length + text color octet */
          length = 1 + 1 + WMS_UDH_OCTETS_TEXT_FORMATTING + 1;
        }
        break;

      /* 0x0B */
      case WMS_UDH_PRE_DEF_SOUND:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PRE_DEF;
        break;

      /* 0x0C */
      case WMS_UDH_USER_DEF_SOUND:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + udh->u.user_def_sound.data_length + 1;
        break;

      /* 0x0D */
      case WMS_UDH_PRE_DEF_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PRE_DEF;
        break;

      /* 0x0E */
      case WMS_UDH_LARGE_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_LARGE_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS + 1;
        break;

      /* 0x0F */
      case WMS_UDH_SMALL_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_SMALL_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS + 1;
        break;

      /* 0x10 */
      case WMS_UDH_LARGE_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_LARGE_PIC_SIZE + 1;
        break;

      /* 0x11 */
      case WMS_UDH_SMALL_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_SMALL_PIC_SIZE + 1;
        break;

      /* 0x12 */
      case WMS_UDH_VAR_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 3 (for height, width and position octets)*/
        length = 1 + 1 + (uint8)(udh->u.var_picture.height * udh->u.var_picture.width/8) + 3;
        break;

      /* 0x13 - 0x1F Reserved for future EMS*/

      /* 0x20 */
      case WMS_UDH_RFC822:
        /* Header ID Octet + Header Length Octet + Header Length*/
        length = 1 + 1 + WMS_UDH_OCTETS_RFC822;
        break;

      /* 0x24 */
      case WMS_UDH_NAT_LANG_SS:
        /* Header ID Octet + Header Length Octet + Header Length*/
        length = 1 + 1 + WMS_UDH_OCTETS_NAT_LANG_SS;
        break;

      /* 0x25 */
      case WMS_UDH_NAT_LANG_LS:
        /* Header ID Octet + Header Length Octet + Header Length*/
        length = 1 + 1 + WMS_UDH_OCTETS_NAT_LANG_LS;
        break;

     /*  0x21 - 0x23 and 0x26 - 0x6F Reserved for future use*/
     /*  0x70 - 0x7f Reserved for (U)SIM Toolkit Security Headers */
     /*  0x80 - 0x9F SME to SME specific use*/
     /*  0xA0 - 0xBF Reserved for future use*/
     /*  0xC0 - 0xDF SC specific use*/
     /*  0xE0 - 0xFF Reserved for future use*/

      case WMS_UDH_USER_PROMPT:
        length = 1 + 1 + WMS_UDH_OCTETS_USER_PROMPT;
        break;

      case WMS_UDH_EXTENDED_OBJECT:
        length = 1 + 1 + udh->u.eo.content.length;
        if( udh->u.eo.first_segment == TRUE )
        {
          length += WMS_UDH_OCTETS_EO_HEADER;
        }
        break;

      default:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

    }
  }
  return length;
}

/*=========================================================================
FUNCTION
  wms_ts_compute_user_data_header_length

DESCRIPTION
  Computes the User Data Header Length

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
uint32 wms_ts_compute_user_data_header_length
(
  const uint8           num_headers,
  const wms_udh_s_type *headers
)
{
  uint32 length = 0;
  uint32 i;

  if( headers == NULL )
  {
    wms_check_for_null_ptr();
    return 0;
  }

  if (num_headers > 0)
  {
    length += 1; /* 1 byte for UDHL byte */

    /* User Data Headers Length */
    for ( i=0 ; i<num_headers && i<WMS_MAX_UD_HEADERS ; i++)
    {
      length += (uint32)wms_ts_get_udh_length(&headers[i]);
    }
  }

  return length;
}

#ifdef FEATURE_GWSMS

/*=========================================================================
FUNCTION
  wms_ts_pack_sim_sms

DESCRIPTION
  Pack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_pack_sim_sms
(
  const wms_msg_sim_sms_unpacked_s_type   * sim_sms_unpacked,  // INPUT
  wms_msg_sim_sms_s_type                  * sim_sms            // OUTPUT
)
{
  uint8   pos = 0, i;

  if( sim_sms_unpacked == NULL || sim_sms == NULL )
  {
    wms_check_for_null_ptr();
    return FALSE;
  }

  memset( (void *)sim_sms->data, 0xFF, WMS_SIM_SMS_MAX);

  i = wms_ts_encode_address( & sim_sms_unpacked->sc_address,
                             (void*) & sim_sms->data[pos] );
  if( i==0 )
  {
    return FALSE;
  }

  pos += i;

  if( (pos >= WMS_SIM_SMS_MAX) ||
      (pos + sim_sms_unpacked->raw_ts_data.len > WMS_SIM_SMS_MAX) )
  {
    MSG_ERROR_0("Exceed Max Data Length, Pack SIM SMS Failed");
    return FALSE;
  }

  sim_sms->data[0] = (uint8)(pos-1);

  if ( sim_sms->data[0] < 2 || sim_sms->data[0] > 11 )
  {
    MSG_ERROR_0("Invalid SC address Length");
    return FALSE;
  }

  (void)memscpy( (void *)&sim_sms->data[pos],
                 sim_sms_unpacked->raw_ts_data.len,
                 sim_sms_unpacked->raw_ts_data.data,
                 sim_sms_unpacked->raw_ts_data.len );
  pos += (uint8) sim_sms_unpacked->raw_ts_data.len;

  sim_sms->len = (uint8) pos;
  sim_sms->status = sim_sms_unpacked->status;

  return TRUE;

} /* wms_ts_pack_sim_sms() */

/*=========================================================================
FUNCTION
  wms_ts_unpack_sim_sms

DESCRIPTION
  Unpack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_unpack_sim_sms
(
  const wms_msg_sim_sms_s_type            * sim_sms,           // INPUT
  wms_msg_sim_sms_unpacked_s_type         * sim_sms_unpacked   // OUTPUT
)
{
  uint8   pos = 0;
  uint8   i =0;
  uint8   buf[(WMS_GW_ADDRESS_MAX/2) + 2];
  int    length;
  wms_client_ts_data_s_type   *client_ts_ptr = NULL;
  wms_message_tag_e_type sim_sms_unpacked_status;

  boolean res = FALSE;

  do
  {
    if ( (NULL == sim_sms) || (NULL == sim_sms_unpacked) )
    {
      MSG_ERROR_0("null pointer in wms_ts_unpack_sim_sms");
      break; /* do-while */
    }

    if ( sim_sms->data[0] > 11 )
    {
      MSG_ERROR_1("Invalid SC address Length = %d", sim_sms->data[0]);
      break; /* do-while */
    }

    length = MIN (sim_sms->data[0]+1, sizeof(buf));

    (void)memscpy( buf,
                   (uint32)length,
                   (void *)(sim_sms->data),
                   (uint32)length );

    if (sim_sms->data[0] > 0)
    {
      /*Calculate number of digits */
      buf[0] =(uint8)( 2 * (sim_sms->data[0]-1));

      if ((sim_sms->data[sim_sms->data[0]] & 0xF0) == 0xF0)
      {
        buf[0] -= 1;
      }

      i =  wms_ts_decode_address( buf, & sim_sms_unpacked->sc_address );

      if (i != (sim_sms->data[0]+1)  )
      {
        MSG_ERROR_0("Invalid BCD address Length");
        break; /* do-while */
      }
    }
    else
    {
      MSG_ERROR_0("Zero Length SC address");
      (void)memset( &sim_sms_unpacked->sc_address, 0, sizeof(wms_address_s_type) );
      i = 1; // Skip byte containing 0 SC Address Length
    }

    pos += i;

    if (sim_sms->len < i)
    {
      MSG_ERROR_0("Invalid sim_sms.len - Check Failed");
      break; /* do-while */
    }

    sim_sms_unpacked->raw_ts_data.len = sim_sms->len - i;

    if ((pos >= WMS_SIM_SMS_MAX) ||
        (pos + sim_sms_unpacked->raw_ts_data.len > WMS_SIM_SMS_MAX) )
    {
      MSG_ERROR_0("Exceed Max Data Length, Unpack SIM SMS Failed");
      break; /* do-while */
    }

    (void)memscpy( sim_sms_unpacked->raw_ts_data.data,
                   sim_sms_unpacked->raw_ts_data.len,
                   (void *)&sim_sms->data[pos],
                   sim_sms_unpacked->raw_ts_data.len );

    sim_sms_unpacked->status = sim_sms->status;
    sim_sms_unpacked_status = (wms_message_tag_e_type)sim_sms_unpacked->status;

    if (WMS_IS_MO(sim_sms_unpacked_status))
    {
       sim_sms_unpacked->raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
    }
    else if (WMS_IS_MT(sim_sms_unpacked_status))
    {
       if (0x00 == (sim_sms_unpacked->raw_ts_data.data[0] & 0x03))
       {
          sim_sms_unpacked->raw_ts_data.tpdu_type = WMS_TPDU_DELIVER;
       }
       else if (0x03 == (sim_sms_unpacked->raw_ts_data.data[0] & 0x03 ))
       {
          sim_sms_unpacked->raw_ts_data.tpdu_type = WMS_TPDU_RESERVED;
       }
    }

    sim_sms_unpacked->raw_ts_data.format = WMS_FORMAT_GW_PP;

    client_ts_ptr = (wms_client_ts_data_s_type*) wms_mem_malloc(sizeof(wms_client_ts_data_s_type));

    if (NULL == client_ts_ptr)
    {
       break; /* do-while */
    }

    /* Decode the message to see if it is good */
    if (WMS_OK_S == wms_ts_decode_ex( &sim_sms_unpacked->raw_ts_data,
                                      client_ts_ptr,
                                      &sim_sms_unpacked->raw_ts_data.len) )
    {
      /* Decode successful; length of raw_ts_data returned from wms_ts_decode_ex */
      res = TRUE;
    }

  } while (0);

  WMS_MEM_FREEIF(client_ts_ptr);
  return res;

} /* wms_ts_unpack_sim_sms() */

/*=========================================================================
FUNCTION
  wms_ts_pack_sim_smsr

DESCRIPTION
  Pack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_pack_sim_smsr
(
  const wms_raw_ts_data_s_type   * raw_ts_data,  // INPUT
  wms_msg_sim_smsr_s_type        * sim_smsr     // OUTPUT
)
{
  if( raw_ts_data == NULL || sim_smsr == NULL )
  {
    wms_check_for_null_ptr();
    return FALSE;
  }

  if ( raw_ts_data->len > WMS_SIM_SMS_MAX)
  {
    MSG_ERROR_0("Exceed Max Data Len, Pack SIM SMS Status Report Failed");
    return FALSE;
  }

  (void)memscpy( (void *)sim_smsr->data,
                 raw_ts_data->len,
                 raw_ts_data->data,
                 raw_ts_data->len );

  sim_smsr->len = (uint8)raw_ts_data->len;

  return TRUE;

} /* wms_ts_pack_sim_smsr() */

/*=========================================================================
FUNCTION
  wms_ts_unpack_sim_smsr

DESCRIPTION
  Unpack SIM data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_unpack_sim_smsr
(
  const wms_msg_sim_smsr_s_type           * sim_smsr,           // INPUT
  wms_raw_ts_data_s_type                  * raw_ts_data        // OUTPUT
)
{
  if( sim_smsr == NULL || raw_ts_data == NULL )
  {
    wms_check_for_null_ptr();
    return FALSE;
  }

  raw_ts_data->format = WMS_FORMAT_GW_PP;
  raw_ts_data->len    = MIN (sim_smsr->len, WMS_MAX_LEN);
  raw_ts_data->tpdu_type = WMS_TPDU_STATUS_REPORT;

  if ( raw_ts_data->len > WMS_SIM_SMS_MAX)
  {
    MSG_ERROR_0("Exceed Max Data Len, Unpack SIM SMS Status Report Failed");
    return FALSE;
  }


  (void)memscpy( raw_ts_data->data,
                 raw_ts_data->len,
                 (void *)sim_smsr->data,
                 raw_ts_data->len );

  return TRUE;

} /* wms_ts_unpack_sim_smsr() */

/*=========================================================================
FUNCTION
  wms_ts_compute_gw_user_data_length

DESCRIPTION
  Computes the GW User Data Length

DEPENDENCIES
  None

RETURN VALUE
  GW User Data Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
uint32 wms_ts_compute_gw_user_data_length
(
  const wms_gw_dcs_s_type         *dcs,
  const wms_gw_user_data_s_type   *user_data
)
{
  uint32 length = 0;

  if( dcs == NULL || user_data == NULL )
  {
    wms_check_for_null_ptr();
    return 0;
  }

  length += wms_ts_compute_user_data_header_length(user_data->num_headers, user_data->headers);

  if( dcs->alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT )
  {
    length += ((user_data->sm_len * 7) + 7)/8;
  }
  else
  {
    length += user_data->sm_len;
  }

  return length;

} /* wms_ts_compute_gw_user_data_length */
#endif /* FEATURE_GWSMS */

/*=========================================================================
FUNCTION
  wms_ts_init

DESCRIPTION
  Initialize the TS Module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_init
(
  void
)
{
  if (FALSE == encode_decode_data_crit_sect_init)
  {
    qc_init_crit_sect(&encode_decode_data_crit_sect);
    encode_decode_data_crit_sect_init = TRUE;
  }
}

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
