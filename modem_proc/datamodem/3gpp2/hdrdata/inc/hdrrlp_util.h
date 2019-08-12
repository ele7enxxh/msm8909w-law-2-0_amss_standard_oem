#ifndef _HDRRLP_UTIL_H
#define _HDRRLP_UTIL_H

/*===========================================================================

                    H D R R L P _ U T I L H E A D E R   F I L E


DESCRIPTION
  This header file contains the interface definition for the hrdrlp utility functions

  Copyright (c) 2008-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlp_util.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   msh     TCB changes due to Core changes  
12/23/11   sb      Fixed compiler warnings 
11/18/11   vpk     HDR SU API cleanup
07/26/11   vpk     Added helpers for bit pack and unpack 
04/30/08   sjun    Initial file creation.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "rex.h"
#include "hdrrlp.h"
#include "dsmbit.h"
#include "hdrcom_api.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRRLP_UTIL_INIT

DESCRIPTION
  This function does the initialization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlp_util_init (void);

/*===========================================================================

FUNCTION HDRRLP_UTIL_IS_TASK_HDRRX

DESCRIPTION
  This function determines whether the current task is HDRRX

DEPENDENCIES
  None

RETURN VALUE
  A boolean value representing whether the current task pointer is HDRRX.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrrlp_util_is_task_hdrrx(void);

/*===========================================================================

FUNCTION HDRRLP_UTIL_IS_TASK_HDRTX

DESCRIPTION
  This function determines whether the current task is HDRTX

DEPENDENCIES
  None

RETURN VALUE
  A boolean value representing whether the current task pointer is HDRTX.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrrlp_util_is_task_hdrtx(void);

/*===========================================================================

FUNCTION HDRRLP_UTIL_GET_HDRRX_TCB

DESCRIPTION
  This function returns the hdrrx_tcb pointer

DEPENDENCIES
  None

RETURN VALUE
  hdrrlp_util_hdrrx_tcb

SIDE EFFECTS
  None
===========================================================================*/

rex_tcb_type* hdrrlp_util_get_hdrrx_tcb 
(
  void
);

/*===========================================================================

FUNCTION HDRRLP_UTIL_GET_HDRTX_TCB

DESCRIPTION
  This function returns the hdrtx_tcb pointer

DEPENDENCIES
  None

RETURN VALUE
  hdrrlp_util_hdrtx_tcb

SIDE EFFECTS
  None
===========================================================================*/

rex_tcb_type* hdrrlp_util_get_hdrtx_tcb 
(
  void
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
      (hdrcom_errno_enum_type) \
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
     (hdrcom_errno_enum_type) \
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
     (hdrcom_errno_enum_type) \
     (dsmbit_unpack32( item_ptr, ret_val_ptr, (uint16) (offset), len ) == 0 ? \
     E_NO_DATA : E_SUCCESS )

     
/*===========================================================================
FUNCTION HDRUTILBIT_PACK8

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
void hdrutilbit_pack8
(
  dsm_item_type * item_ptr,
  uint8 pack_data,
  uint16 offset,
  uint16 len
);

/*===========================================================================
FUNCTION HDRUTILBIT_PACK32

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
void hdrutilbit_pack32
(
  dsm_item_type * item_ptr,
  uint32 pack_data,
  uint16 offset,
  uint16 len
);

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
/* TODO: Remove this once HDR changes are mainlined */
#ifndef HDRBIT_PACK8
#define HDRBIT_PACK8( item_ptr, pack_data, offset, len ) \
    hdrutilbit_pack8(item_ptr, pack_data, (uint16) (offset), len)
#endif /*HDRBIT_PACK8 */


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
/* TODO: Remove this once HDR changes are mainlined */
#ifndef HDRBIT_PACK32
#define HDRBIT_PACK32( item_ptr, pack_data, offset, len ) \
        hdrutilbit_pack32(item_ptr, pack_data, (uint16)(offset), len)
#endif /* HDRBIT_PACK32 */    
#endif /* _HDRRLP_UTIL_H */
