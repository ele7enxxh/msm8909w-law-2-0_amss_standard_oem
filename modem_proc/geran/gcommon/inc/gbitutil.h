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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gbitutil.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/26/01    hv	   Created
*** 03/27/01    hv     Added gpackb() and gunpackb().
*** 12/11/01   pjr     Added gpackq() and gunpackq().
***
*****************************************************************************/

#ifndef INC_GBITUTIL_H
#define INC_GBITUTIL_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "bit.h"
#include "comdef.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         GSIZE
===
===  DESCRIPTION
===
===    Expands a <base struct name> and a <member variable name> to a 
===    #define item which carries length information in an message field.
===
===    eg. GSIZE (<base_struct>,<member_variable>) expands to
===               <base_struct>_<member_variable>_len
===
===        GSIZE (rlc_ext_octet_t, li) expands to
===               rlc_ext_octet_t_li_len
===
===  DEPENDENCIES
===
===    GSIZ() macro
===  
===  RETURN VALUE
===
===    
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/

#define	GSIZE(x,y)	x##_##y##_len


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
);


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
//lint -sem(gunpackb, 3n >= 0 && 3n <= 8)
uint8 gunpackb
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
);

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
);

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
);

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
//lint -sem(gunpackw, 3n >= 0 && 3n <= 16)
uint16 gunpackw
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
);


/*===========================================================================
===
===  FUNCTION      GPACKD
===
===  DESCRIPTION
===
===    Packs the given word into the destination at the given offset for the
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
===    Invokes b_packD()
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
);

/*===========================================================================
===
===  FUNCTION      GUNPACKD
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
===    Right-aligned dword 
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
//lint -sem(gunpackd, 3n >= 0 && 3n <= 32)
uint32 gunpackd
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
);


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
   uint8  dst[], 
   uint16 *pos_ptr,
   uint16 len 
);


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
//lint -sem(gunpackq, 3n >= 0 && 3n <= 64)
uint64 gunpackq (uint8 src[], uint16 *pos_ptr, uint16 len);

uint16 eunpackw
(
   uint8  src[], 
   uint16 *pos_ptr, 
   uint16 len
);

void epackw
(
   word  src,
   uint8  dst[],
   uint16 *pos_ptr,
   uint16 len
);


#endif /* INC_GBITUTIL_H */

/*** EOF: don't remove! ***/
