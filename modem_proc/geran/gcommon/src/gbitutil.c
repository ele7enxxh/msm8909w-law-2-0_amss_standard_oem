/*****************************************************************************
***
*** TITLE
***
***  GPRS BIT MANIPULATION SERVICES
***
***
*** DESCRIPTION
***
***  This module provides utility functions to manipulate data at bit level.
***  The functions are based on those provided by DMSS module bit.c/h.
***
*** 
*** EXTERNALIZED FUNCTIONS
***
***  gpackb()
***  gunpackb()
***  gunpackb_lh()
***
***  gpackw()
***  gunpackw()
***
***  gpackd()
***  gunpackd()
***
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  Requires DMSS bit.c/h module
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/gbitutil.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/03/01    hv	   Created
*** 12/11/01   pjr	   Added functions gpackq and gunpackq
*** 08/02/04    hv     Added epackw() and eunpackw()
*** 08/03/04    hv     Moved b_packw_hl() and b_unpackw_hl() from bit.c
*** 08/06/04    hv     Corrected and optimised b_unpackw_hl()
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
#include "gbitutil.h"

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*============================================================================

FUNCTION B_PACKW_HL

DESCRIPTION
  Packs the given word into the destination at the given offset for the
  given number of length bits. MSB is on the higher order byte.

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_packw_hl(
   word src, 
   byte dst[], 
   word pos, 
   word len 
)
{
  /*---------------------------------------------------------------------------
  ** Params
  **   - pos: bit position in the destination where the LSBit of the source
  **          data will go
  **
  **     bit 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
  **                  <--------- len = 10 -------->
  **                                              +
  **                                              \
  **    pos = 3 means LSB of data is here --------   ie, from the RHS
  **          range of pos: 0 .. 65535 but we expect the max value is around 50
  **---------------------------------------------------------------------------
  */

#ifndef PERLUTF
  /* uint16-version - works on Target */
  
  register uint16   u=0U;
  uint8             *ptr;
  
  /* Move destination address to the first octet. Adjust 'pos' accordingly
  */
  ptr = (uint8*)&(dst[pos/8]);
  pos %= 8;
  
  /* Clear out the MSBits outside the length specified
  */
  src &= (0xffffU >> (16-len));

  /* Read word into temp var octet-by-octet
  */
  u = ((uint16)(ptr[1] & 0xff) << 8);
  u |= (uint16)(ptr[0] & 0xff);

  /* Or in the resultant mask
  */
  u |= ( (uint16)(0xffffU >> (16-len)) << pos ) & (((uint16)src) << pos);

  /* write back octet by octet
  */
  ptr[1] = (uint8)((u >>  8) & 0xff);
  ptr[0] = (uint8)(u & 0xff);

#else
  #error code not present
#endif

#if 0 /* optimised version which works on Host Test */
  (uint8*)dst = (uint8*)&(dst[pos/8]);
  pos %= 8;

  /* Clear out the MSBits outside the length specified. A must
  */
  src &= (0xffff >> (16-len));

  /* Since a word can straddle across 3 octets, promote word to long unsigned (32-bit)
  ** to better utilise the 32-bit ARM
  */

  /* Clear out the bits where the data will go
  */
  *(uint32*)dst &= ~( (uint32)(0xffffffffUL >> (32-len)) << pos );

  /* OR the data in
  */
  *(uint32*)dst |= (((uint32)src) << pos);
#endif
} /* b_packw_hl */
   
/*===========================================================================
===
===  FUNCTION      B_UNPACK_HL
===
===  DESCRIPTION
===
===    Unpack 'len' bits from an array of octets from bit position 'pos' into
===    a word.
===
===  PARAMETERS
===
===    uint8 src: array of octets representing rlc ul data block [hdr:msg1:msg2]
===    uint16 pos: bit offset from bit0 (LSBit of LSByte of array src[]).
===                eg. 7 means MSBit of first octet, 16 means LSBit of 3rd octet
===    uint16 len: number of bits to unpack
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
==========================================================================*/
uint16 b_unpackw_hl
(
  uint8  src[],
  uint16 pos,
  uint16 len
     )
{


#ifdef PERLUTF
  #error code not present
#else
  /* uint16-version - works on Target */
  uint16 result = 0;
  uint16 lshift = 0;

  while (pos > 8)
  {
    pos -= 8;
    src++;
  }

  if (len + pos <= 8)
  {
     result = (*src >> pos) & (0xff >> (8-len));
  }
  else
  {
      /* Read the first or part of byte */
     result = (*src >> pos) & (0xff >> pos);
     lshift = 8 - pos;

     len -= lshift;
     src++;

     for(;len >=8; len-=8)
     {
        result |=  (uint16)(*src++ << lshift);
        lshift +=8;
     }

     if (len > 0 )
     {
        result |= (*src & (0xff >> (8-len))) << lshift;
     }
  }
  return result;


#endif
}
/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GPACKB
===
===  DESCRIPTION
===
===    Packs the given byte into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
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
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void gpackb
(
   uint8  src, 
   uint8  dst[], 
   uint16 *pos_ptr,
   uint16 len 
)
{
	b_packb(src, dst, *pos_ptr, len);
	*pos_ptr += len;
}

/*===========================================================================
===
===  FUNCTION      GUNPACKB
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a byte and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
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
===    Invokes b_unpackb()
===  
===  RETURN VALUE
===
===    Right-aligned byte representing a field extracted from the external 
===    data byte given the bit position offset from the left-hand-side and
===    the length of the byte.
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
uint8 gunpackb
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
)
{
	uint8 by = b_unpackb(src, *pos_ptr, len);
	*pos_ptr += len;
	
	return by;
}

/*===========================================================================
===
===  FUNCTION      GUNPACKB_LH
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks a single bit, advancing the offset
===    (position pointer). Calculates the value of the bit position in CSN1 L|H
===    padding, (0x2b padding pattern), and returns the bit XOR'd with this value
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. Ranges from 0 (MSBit) to 7 (LSBit)
===
===              MSB           LSB
===             +-+-+-+-+-+-+-+-+
===             |0|0|1|0|1|0|1|1|  0x2b padding pattern
===             +-+-+-+-+-+-+-+-+
===             |8|7|6|5|4|3|2|1|  bit numbering
===             +-+-+-+-+-+-+-+-+
===             |0 0 0 1 1 1 1 0|  example bit stream
===             +-+-+-+-+-+-+-+-+
===                ^ ^   ^
===                | |   | position pointer 4, return value is 0
===                | |
===                | | position pointer 2, return value is 1
===                |
===                | position pointer 1, return value is 0
===
=== eg a bit is considered set if it's value is the inverse of the same bit position
=== in the csn1 padding pattern, 0010 1011 (0x2b)
===
===
===  DEPENDENCIES
===
===    Invokes b_unpackb()
===
===  RETURN VALUE
===
===    Right-aligned byte representing a field extracted from the external
===    data byte given the bit position offset from the left-hand-side and
===    the value of the same bit position in the padding pattern 0x2b.
===    This function returns the meaning of the bit, not the value.
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gunpackb_lh
(
   uint8  src[],
   uint16 *pos_ptr
)
{
	uint8 bit_offset = *pos_ptr % 8;
	uint8 csn1_padding_bit;
	uint8 by;

	csn1_padding_bit = (0x2b >> (7 - bit_offset)) & 0x01;
	by = b_unpackb(src, *pos_ptr, 1) ^ csn1_padding_bit;
	*pos_ptr += 1;
  
	return by;
}

/*===========================================================================
===
===  FUNCTION      GPACKW
===
===  DESCRIPTION
===
===    Packs the given word into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
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
void gpackw
(
   uint16 src, 
   uint8  dst[], 
   uint16 *pos_ptr,
   uint16 len 
)
{
	b_packw(src, dst, *pos_ptr, len);
	*pos_ptr += len;
}

/*===========================================================================
===
===  FUNCTION      GUNPACKW
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a word and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. 
===
===    len:     length of data item in bits
===   
===  DEPENDENCIES
===
===    Invokes b_unpackw()
===  
===  RETURN VALUE
===
===    Right-aligned word 
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
uint16 gunpackw
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
)
{
	uint16 by = b_unpackw(src, *pos_ptr, len);
	*pos_ptr += len;
	
	return by;
}

/*===========================================================================
===
===  FUNCTION      GPACKD
===
===  DESCRIPTION
===
===    Packs the given dword into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
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
void gpackd
(
   uint32 src, 
   uint8  dst[], 
   uint16 *pos_ptr,
   uint16 len 
)
{
	b_packd(src, dst, *pos_ptr, len);
	*pos_ptr += len;
}

/*===========================================================================
===
===  FUNCTION      GUNPACKD
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a dword and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. 
===
===    len:     length of data item in bits
===   
===  DEPENDENCIES
===
===    Invokes b_unpackd()
===  
===  RETURN VALUE
===
===    Right-aligned dword 
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
uint32 gunpackd
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
)
{
	uint32 by = b_unpackd(src, *pos_ptr, len);
	*pos_ptr += len;
	
	return by;
}


/*===========================================================================
===
===  FUNCTION      GPACKQ
===
===  DESCRIPTION
===
===    Packs the given word into the destination at the given offset for the
===    given number of length bits and advances the position pointer.
===
===  PARAMETERS
===
===    src:     word value representing value of a data field.
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
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void gpackq
(
   uint32 src[2], 
   uint8 dst[], 
   uint16 *pos_ptr,
   uint16 len 
)
{
	b_packq(src, dst, *pos_ptr, len);
	*pos_ptr += len;
}


/*===========================================================================
===
===  FUNCTION      GUNPACKQ
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a dword and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. 
===
===    len:     length of data item in bits
===   
===  DEPENDENCIES
===
===    Invokes b_unpackd()
===  
===  RETURN VALUE
===
===    Right-aligned dword 
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/

uint64 gunpackq (uint8 src[], uint16 *pos_ptr, uint16 len)
{
  uint32 by;
  uint64 result = 0;
 
  while (len > 16)
  {
    len      -= 16;
    by        = b_unpackd(src, *pos_ptr, 16);
    *pos_ptr += 16;
    result   |= (uint64)by;
    result  <<= 16;
  }

  /*  Account for non integer 16 values */
  result >>= 16;

  if (len)
  {
    result  <<= len;
    by        = b_unpackd(src, *pos_ptr, len);
    *pos_ptr += len;
    result   |= (uint64)by;
  }

  return (result);
}

/*===========================================================================
===
===  FUNCTION      EUNPACKW
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a word and advances the position pointer. MSB unpacked from
===    higher order byte. Within a byte higher order bit is the MSB. 
===
===         MSB           LSB
===         +-+-+-+-+-+-+-+-+
===         |8|7|6|5|4|3|2|1|
===         +-+-+-+-+-+-+-+-+
===         |0 0 0 1 X X X X|
===         +-+-+-+-+-+-+-+-+   
===         |X X X X 1 1 0 1|     
===         +-+-+-+-+-+-+-+-+  
===                 MSB
===           ------> *pos_ptr = 4, len = 8
===
===
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. 
===
===    len:     length of data item in bits
===   
===  DEPENDENCIES
===
===    Invokes b_unpackw_hl()
===  
===  RETURN VALUE
===
===    Right-aligned word 
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/

uint16 eunpackw
(
   uint8  src[],
   uint16 *pos_ptr,
   uint16 len
)
{  
   uint16 result = b_unpackw_hl(src, *pos_ptr, len);
   *pos_ptr += len;

    return result;
}


/*===========================================================================
===
===  FUNCTION      EPACKW
===
===  DESCRIPTION
===
===    Given a word and an offset, packs the requested number of bits
===    into a buffer and advances the position pointer. LSB is packed first.
===    Within a byte higher order bit is the MSB. 
===
===         MSB           LSB
===         +-+-+-+-+-+-+-+-+
===         |8|7|6|5|4|3|2|1|
===         +-+-+-+-+-+-+-+-+
===         |1 0 X X X X X X|   pos_ptr = 6 => 8-6=2, len = 3 
===         +-+-+-+-+-+-+-+-+   
===         |X X X X X X X 0|     
===         +-+-+-+-+-+-+-+-+  
===                        MSB
===           
===
===
===
===  PARAMETERS
===    src:     word value representing value of a data field.
===
===    dst:     byte array representing part of an external bitstream.
===
===    pos_ptr: pointer to the position var. ==> 8 - *pos_ptr 
===
===    len:     length of data item in bits
===   
===  DEPENDENCIES
===
===    Invokes b_packw_hl()
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
void epackw
(
   word  src,
   uint8  dst[],
   uint16 *pos_ptr,
   uint16 len
)
{
  b_packw_hl(src, dst, *pos_ptr, len);
  
  /* Only advance pos_ptr after packing is done so that the next item starts from where 
  ** the current operation ends.
  */
  *pos_ptr += len;
}


/*** EOF: don't remove! ***/

