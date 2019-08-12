/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GS Bit Stream Module

GENERAL DESCRIPTION
   This module contains the functions to support the encoding decoding of a bit stream

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsbitstream.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/02   rmc     Moved to NAS VU and renamed gs_
06/18/02   rmc     Initial revision
                   
===========================================================================*/

#include "gsbitstream.h"

#include "amssassert.h"

#define NAS_ASSERT_UT ASSERT

/**
 * Initializes the bit stream with the given values
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param buffer_ptr     pointer to buffer used for the bit stream
 * 
 * @param length         length of the buffer (set to 0 if not known)
 */
void gs_bit_stream_initialize(gs_bit_stream_T *bit_stream_ptr, byte *buffer_ptr, uint32 length)
{
  bit_stream_ptr->buffer_ptr = buffer_ptr;
  bit_stream_ptr->length     = length;
  bit_stream_ptr->current_bit = 8;
  bit_stream_ptr->current_byte = 0;

  /* default GSM pad pattern */
  bit_stream_ptr->pad_pattern = 0x2b;

  bit_stream_ptr->overflow = FALSE;
} /* gs_bit_stream_initialize */

/**
 * fills the given number of bytes in the buffer to the given value
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param number         number of bytes to fill
 * 
 * @param value          value to fill
 */
void gs_bit_stream_fill(gs_bit_stream_T *bit_stream_ptr,uint32 number, byte value)
{
  uint32 index;
  NAS_ASSERT_UT(bit_stream_ptr->length == 0 || number <= bit_stream_ptr->length);

  for (index=0;index < number;++index)
  {
    bit_stream_ptr->buffer_ptr[index] = value;
  }
} /* gs_bit_stream_fill */

/**
 * advances the bit stream to the next byte, checking for overflow.
 * 
 * @param bit_stream_ptr pointer to the bit stream
 */
void gs_bit_stream_next_byte(gs_bit_stream_T *bit_stream_ptr)
{
  NAS_ASSERT_UT(bit_stream_ptr != NULL);

  /* move to the next byte */
  ++(bit_stream_ptr->current_byte);
  bit_stream_ptr->current_bit = 8;

  if (!(bit_stream_ptr->length == 0 || bit_stream_ptr->current_byte < bit_stream_ptr->length))
  {
    bit_stream_ptr->overflow = TRUE;
  }
} /* gs_bit_stream_next_byte */

uint32 gs_bit_stream_insert_byte_value(gs_bit_stream_T *bit_stream_ptr,uint32 bits, uint32 value)
{
  uint32 bits_left;
  byte shift_amount;
  NAS_ASSERT_UT(bit_stream_ptr != NULL);
  if (bit_stream_ptr->current_bit == 0)
  {
    /* need a new byte */
    gs_bit_stream_next_byte(bit_stream_ptr);
  }

  if (!(bit_stream_ptr->length == 0 || bit_stream_ptr->current_byte < bit_stream_ptr->length))
  {
    bit_stream_ptr->overflow = TRUE;
    return bits;
  }

  bits_left = bit_stream_ptr->current_bit;

  /* transfer bit by bit */
  while (bits_left > 0 && bits > 0)
  {
    byte valueBit = (byte) (((value & (1 << (bits - 1)))) == 0) ? 0 : 1;
    shift_amount = (byte)bits_left - 1;

    /* clear any existing value first */
    bit_stream_ptr->buffer_ptr[bit_stream_ptr->current_byte] &= 
      (byte)((~(byte)(1 << shift_amount)) & ((byte) 0xFF));

    /* set the new value */
    bit_stream_ptr->buffer_ptr[bit_stream_ptr->current_byte] |= 
      (byte)((valueBit << (shift_amount)) & 0xFF);

    /* decrement both */
    --bits_left;
    --bits;
  }

  /* store the new value */
  bit_stream_ptr->current_bit = bits_left;

  return bits;
} /* gs_bit_stream_insert_byte_value */

/**
 * inserts a field (numeric value) into the next position in the bit stream.
 * Values are inserted in left-to-right order starting with the MSB
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param bits           number of bits in the field
 * 
 * @param value          value of the field
 */
void gs_bit_stream_insert_field(gs_bit_stream_T *bit_stream_ptr, uint32 bits, uint32 value)
{
  NAS_ASSERT_UT(bit_stream_ptr != NULL);
  
  while ((bits > 0) && (!bit_stream_ptr->overflow))
  {
    bits = gs_bit_stream_insert_byte_value(bit_stream_ptr,bits,value);

    if (!(bit_stream_ptr->length == 0 || bit_stream_ptr->current_byte < bit_stream_ptr->length))
    {
      bit_stream_ptr->overflow = TRUE;
      break;
    }
  }
} /* gs_bit_stream_insert_field */

/**
 * Inserts an H or L into the current bit of the bit stream
 * 
 * @param bit_stream_ptr pointer to the bit stream
 * 
 * @param value          value (TRUE for H, FALSE for L)
 */
void gs_bit_stream_insert_HL(gs_bit_stream_T *bit_stream_ptr,boolean value)
{
  uint32 pad_bit;
  uint32 encoded_value;
  uint32 bit_index;

  NAS_ASSERT_UT(bit_stream_ptr != NULL);
  bit_index = bit_stream_ptr->current_bit;
  /* if out of bits */
  if (bit_index == 0)
  {
    gs_bit_stream_next_byte(bit_stream_ptr);
    bit_index = 8;
  }

  if (!(bit_stream_ptr->length == 0 || bit_stream_ptr->current_byte < bit_stream_ptr->length))
  {
    bit_stream_ptr->overflow = TRUE;
    return;
  }
  
  /* transfer a single bit (appropriately mapped for H|L */
  pad_bit = (bit_stream_ptr->pad_pattern & (1 << (bit_index - 1))) ? 1 : 0;

  /* encoded value is just xor of two */
  encoded_value = (pad_bit ^ value);

  gs_bit_stream_insert_field(bit_stream_ptr,1,encoded_value);
} /* gs_bit_stream_insert_field */

/**
 * inserts a spare field of the given size into the stream. By definition, spare
 * bits are set to 0 when inserted.
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param bits           number of bits for spare field
 */
void gs_bit_stream_insert_spare(gs_bit_stream_T *bit_stream_ptr,uint32 bits)
{
  gs_bit_stream_insert_field(bit_stream_ptr,bits,0);
} /* gs_bit_stream_insert_spare */

/**
 * extract a single byte out of the stream (given number of bits) as long as we do not drop off
 * the end of the stream.
 * 
 * @param bit_stream_ptr pointer to the bit stream
 * 
 * @param bits_ptr       [in/out] pointer to the number of bits that need to be extracted,
 *                       set to the actual number of bits that were extracted
 * 
 * @param value          value extracted so far from the bit stream
 *  
 * @return value extracted from the bitstream
 */
uint32 gs_bit_stream_extract_byte_value(
  gs_bit_stream_T *bit_stream_ptr, 
  uint32 *bits_ptr,
  uint32 value
)
{
  uint32 bits_left;
  uint32 bits = *bits_ptr;
  NAS_ASSERT_UT(bit_stream_ptr != NULL);
  if (bit_stream_ptr->current_bit == 0)
  {
    /* need a new byte */
    gs_bit_stream_next_byte(bit_stream_ptr);
  }
  
  if (bit_stream_ptr->overflow)
  {
    return 0;
  }
  
  if (!(bit_stream_ptr->length == 0 || bit_stream_ptr->current_byte < bit_stream_ptr->length))
  {
    bit_stream_ptr->overflow = TRUE;
    return 0;
  }

  bits_left = bit_stream_ptr->current_bit;

  /* transfer bit by bit */
  while (bits_left > 0 && bits > 0)
  {
    byte valueBit = ((bit_stream_ptr->buffer_ptr[bit_stream_ptr->current_byte]) & (1 << (bits_left - 1)));
    valueBit = (valueBit == 0) ? 0 : 1;

    /* shift the value up by one to make room for next bit */
    value <<= 1;

    /* set the new value */
    if (valueBit)
    {
      value |= valueBit;
    }

    /* decrement both */
    --bits_left;
    --bits;
  }

  /* store the new value */
  bit_stream_ptr->current_bit = bits_left;

  /* set the out parameter */
  *bits_ptr = bits;

  return value;
} /* gs_bit_stream_extract_byte_value */

/**
 * extract a field (of given bit length) from the bit stream but only if this will not cause
 * us to overflow the source array.
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param bits           number of bits to extract
 *
 * @param overflow pointer to a boolean that will be set to TRUE if an overflow is encoutered (i.e. we drop off the end of the source array).
 * 
 * @return value of the extracted field
 */
uint32 gs_bit_stream_extract_field(gs_bit_stream_T *bit_stream_ptr,uint32 bits)
{
  uint32 value = 0;
  while (bits > 0 && !bit_stream_ptr->overflow)
  {
    value = gs_bit_stream_extract_byte_value(bit_stream_ptr,&bits,value);
  }

  return value;
} /* gs_bit_stream_extract_field */


/**
 * extracts a single H|L bit from the bit stream, checking for overflow of the bitstream.
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @return 1 if the decoded bit value is H, 0 if it is L
 */
boolean gs_bit_stream_extract_HL(gs_bit_stream_T *bit_stream_ptr)
{
  uint32 pad_bit;
  uint32 encoded_value;
  uint32 bit_index = bit_stream_ptr->current_bit;

  if (bit_stream_ptr->overflow)
  {
    return FALSE;
  }
  
  /* if out of bits */
  if (bit_index == 0)
  {
    /* do not automatically move to the next byte... will be done when extracting */
    bit_index = 8;
  }

  encoded_value = gs_bit_stream_extract_field(bit_stream_ptr, 1);

  if (!bit_stream_ptr->overflow)
  {
    /* decode a single bit (appropriately mapped for H|L */
    pad_bit = (bit_stream_ptr->pad_pattern & (1 << (bit_index - 1))) ? 1 : 0;

    /* decoded value is just xor of two */
    return ((pad_bit ^ encoded_value) == 0) ? FALSE : TRUE;
  }
  
  return FALSE;
} /* gs_bit_stream_extract_HL */


/**
 * extracts a spare field of the given size from the bit stream
 * (bit values are ignored)
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @param bits           number of bits to extract
 */
void gs_bit_stream_extract_spare(gs_bit_stream_T *bit_stream_ptr, uint32 bits)
{
  (void) gs_bit_stream_extract_field(bit_stream_ptr,bits);
} /* gs_bit_stream_extract_spare */

/**
 * 
 * @param bit_stream_ptr pointer to bit stream
 * 
 * @return number of bytes that have been inserted into the bit stream (including partial bytes)
 */
uint32 gs_bit_stream_byte_count(gs_bit_stream_T *bit_stream_ptr)
{
  /* if the current bit is 8 */
  if (bit_stream_ptr->current_bit == 8)
  {
    /* no partial byte */
    return bit_stream_ptr->current_byte;
  }
  else /* partial byte */
  {
    return bit_stream_ptr->current_byte + 1;
  }
} /* gs_bit_stream_byte_count */

