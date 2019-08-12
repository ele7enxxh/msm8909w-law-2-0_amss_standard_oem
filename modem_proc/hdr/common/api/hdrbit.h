#ifndef HDRBIT_H
#define HDRBIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R  S P E C I F I C  B I T  M A N I P U L A T I O N 
                             S E R V I C E S

GENERAL DESCRIPTION
  This file contains declarations for bit manipulation routines specific to 
  HDR. It has unpacking routines for extracting fields of a received message 
  in contained in dsm items.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdrbit.h_v   1.10   02 Jan 2001 21:09:54   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrbit.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/05   pba     LINT error cleanup
11/28/00   nmn     changed pack macros to use new hdrbit functions
07/27/00   nmn     added pack functions
05/15/00   vas     Created module.

===========================================================================*/
#include "dsmbit.h"
#include "hdrerrno.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRBIT_PACK8

DESCRIPTION
  This function calls the dsmbit_pack8 function, and cuases an ERR_FATAL
  if the total number of bits is not packed.

DEPENDENCIES
  None.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  ERR_FATAL if the packing is unsuccessful

===========================================================================*/
void hdrbit_pack8
(
  dsm_item_type * item_ptr,
  uint8 pack_data,
  uint16 offset,
  uint16 len
);

/*===========================================================================
FUNCTION HDRBIT_PACK16

DESCRIPTION
  This function calls the dsmbit_pack16 function, and causes an ERR_FATAL
  if the total number of bits is not packed.

DEPENDENCIES
  None.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  ERR_FATAL if the packing is unsuccessful

===========================================================================*/
void hdrbit_pack16
(
  dsm_item_type * item_ptr,
  uint16 pack_data,
  uint16 offset,
  uint16 len
);

/*===========================================================================
FUNCTION HDRBIT_PACK32

DESCRIPTION
  This function calls the dsmbit_pack32 function, and causes an ERR_FATAL
  if the total number of bits is not packed.

DEPENDENCIES
  None.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  ERR_FATAL if the packing is unsuccessful

===========================================================================*/
void hdrbit_pack32
(
  dsm_item_type * item_ptr,
  uint32 pack_data,
  uint16 offset,
  uint16 len
);

/*===========================================================================
MACRO HDRBIT_UNPACK8

DESCRIPTION
  Wrapper for dsmbit_unpack8. Given a dsm item and an offset, unpacks the 
  requested number of bits into a byte.
  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this macro.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  E_SUCCESS   - If bits can be successfully extracted & unpacked from dsm item.
  E_NO_DATA   - If operation fails, ie bits cannot be extracted from dsm item.

SIDE EFFECTS
  None.

===========================================================================*/
#define HDRBIT_UNPACK8( item_ptr, ret_val_ptr, offset, len ) \
      (hdrerrno_enum_type) \
      (dsmbit_unpack8( item_ptr, ret_val_ptr, (uint16) (offset), len ) == 0 ? \
      E_NO_DATA : E_SUCCESS )

/*===========================================================================
FUNCTION HDRBIT_UNPACK16

DESCRIPTION
  Wrapper for dsmbit_unpack16. Given a dsm item and an offset, unpacks the 
  requested number of bits into a word.
 
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this macro.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  E_SUCCESS   - If bits can be successfully extracted & unpacked from dsm item.
  E_NO_DATA   - If operation fails, ie bits cannot be extracted from dsm item.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRBIT_UNPACK16( item_ptr, ret_val_ptr, offset, len ) \
     (hdrerrno_enum_type) \
     (dsmbit_unpack16( item_ptr, ret_val_ptr, (uint16) (offset), len ) == 0 ? \
     E_NO_DATA : E_SUCCESS )

/*===========================================================================
FUNCTION HDRBIT_UNPACK32

DESCRIPTION
  Wrapper for dsmbit_unpack. Given a dsm item and an offset, unpacks the 
  requested number of bits into a dword.
  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this macro.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  E_SUCCESS   - If bits can be successfully extracted & unpacked from dsm item.
  E_NO_DATA   - If operation fails, ie bits cannot be extracted from dsm item.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRBIT_UNPACK32( item_ptr, ret_val_ptr, offset, len ) \
     (hdrerrno_enum_type) \
     (dsmbit_unpack32( item_ptr, ret_val_ptr, (uint16) (offset), len ) == 0 ? \
     E_NO_DATA : E_SUCCESS )

/*===========================================================================
FUNCTION HDRBIT_UNPACK64

DESCRIPTION
  Wrapper for dsmbit_unpack. Given a dsm item and an offset, unpacks the 
  requested number of bits into a qword.
  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this macro.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  E_SUCCESS   - If bits can be successfully extracted & unpacked from dsm item.
  E_NO_DATA   - If operation fails, ie bits cannot be extracted from dsm item.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRBIT_UNPACK64( item_ptr, ret_val_ptr, offset, len ) \
     (hdrerrno_enum_type) \
     (dsmbit_unpack64( item_ptr, ret_val_ptr, (uint16) (offset), len ) == 0 ? \
     E_NO_DATA : E_SUCCESS )

/*===========================================================================
MACRO HDRBIT_PACK8

DESCRIPTION
  Wrapper for dsmbit_pack8. Packs a given value (up to eight bits) into a
  given dsm item at a specified offset.  If the dsm item pointer points to
  a NULL pointer, a new dsm item will be acquired.

DEPENDENCIES
  Message must be packed in order of fields.  For instance, a call to pack
  data at the 16th bit followed by a call to pack data at the 0th bit is
  not supported.

  item_ptr can not be NULL.

PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack 

RETURN VALUE
  None.

SIDE_EFFECTS
  If the dsm item is full, a new dsm item may be acquired.  Failure to acquire 
  a new dsm item is a failure condition.

===========================================================================*/
#define HDRBIT_PACK8( item_ptr, pack_data, offset, len ) \
        hdrbit_pack8( item_ptr, pack_data, (uint16) (offset), len )

/*===========================================================================
MACRO HDRBIT_PACK16

DESCRIPTION
  Wrapper for dsmbit_pack16. Packs a given value (up to sixteen bits) into a 
  given dsm item at a specified offset.  If the dsm item pointer points to a 
  NULL pointer, a new dsm item will be acquired.

DEPENDENCIES
  Message must be packed in order of fields.  For instance, a call to pack
  data at the 16th bit followed by a call to pack data at the 0th bit is
  not supported.

  item_ptr can not be NULL.

PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  If the dsm item is full, a new dsm item may be acquired.  Failure to acquire 
  a new dsm item is a failure condition.
===========================================================================*/
#define HDRBIT_PACK16( item_ptr, pack_data, offset, len ) \
        hdrbit_pack16( item_ptr, pack_data, (uint16) (offset), len )

/*===========================================================================
MACRO HDRBIT_PACK32

DESCRIPTION
  Wrapper for dsmbit_pack32. Packs a given value (up to thirty-two bits) into
  a given dsm item at a specified offset.  If the dsm item pointer points to a
  NULL pointer, a new dsm item will be acquired.

DEPENDENCIES
  Message must be packed in order of fields.  For instance, a call to pack
  data at the 16th bit followed by a call to pack data at the 0th bit is
  not supported.

  item_ptr can not be NULL.

PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  If the dsm item is full, a new dsm item may be acquired.  Failure to acquire 
  a new dsm item is a failure condition.
===========================================================================*/
#define HDRBIT_PACK32( item_ptr, pack_data, offset, len ) \
        hdrbit_pack32( item_ptr, pack_data, (uint16) (offset), len )

#endif /* HDRBIT_H */

