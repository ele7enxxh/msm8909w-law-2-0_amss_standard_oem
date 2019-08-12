#ifndef GS_BIT_STREAM_H
#define GS_BIT_STREAM_H

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gsbitstream.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/02   rmc     Moved to NAS VU and renamed gs_
06/18/02   rmc     Initial revision 

===========================================================================*/

#include "comdef.h"


typedef struct
{
  /**
   * pointer to a buffer where data is decoded/encoded
   */
  byte *buffer_ptr;

  /**
   * lenght of the buffer (set to 0 if buffer length is not known)
   */
  uint32 length;

  /**
   * current byte index into the buffer (starting at 0 going up)
   */
  uint32 current_byte;

  /**
   * current bit index into the current byte (starting at 8 going down to 1)
   */
  uint32 current_bit;

  /**
   * pad pattern used to encode/decode H|L in CSN.1
   */
  byte pad_pattern;

  boolean   overflow;
} gs_bit_stream_T;

extern void gs_bit_stream_initialize(gs_bit_stream_T *bit_stream_ptr, byte *buffer, uint32 length);

extern void gs_bit_stream_fill(gs_bit_stream_T *bit_stream_ptr,uint32 number, byte value);

extern void gs_bit_stream_insert_field(gs_bit_stream_T *bit_stream_ptr,uint32 bits, uint32 value);
extern void gs_bit_stream_insert_HL(gs_bit_stream_T *bit_stream_ptr,boolean value);
extern void gs_bit_stream_insert_spare(gs_bit_stream_T *bit_stream_ptr,uint32 bits);

extern uint32 gs_bit_stream_extract_field(gs_bit_stream_T *bit_stream_ptr,uint32 bits);
extern boolean gs_bit_stream_extract_HL(gs_bit_stream_T *bit_stream_ptr);

extern void gs_bit_stream_extract_spare(gs_bit_stream_T *bit_stream_ptr, uint32 bits);

extern uint32 gs_bit_stream_byte_count(gs_bit_stream_T *bit_stream_ptr);

#endif
