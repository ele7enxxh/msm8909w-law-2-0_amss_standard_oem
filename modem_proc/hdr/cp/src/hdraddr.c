/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                          H D R   A D R R E S S

GENERAL DESCRIPTION
  This file contains routines necessary for manipulation of the 128 bit
  HDR Addresses (SectorID, SubnetID & UATI)

EXTERNALIZED FUNCTIONS 
  hdraddr_unpack_addr()
    Unpacks the 128 bit address from a packed message

  hdraddr_pack_addr()
    Packs the 128 bit address into a dsm item

  hdraddr_is_equal()
    Compares two 128 bit addresses.

  hdraddr_set()
    Sets one 128 bit address equal to another.

  hdraddr_mask()
    Masks the specified number of bits in the address.

  hdraddr_set_octet()
    Sets a specific octet of the address

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdraddr.c_v   1.9   10 Jul 2002 18:07:38   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdraddr.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
07/13/10   cnx     Fixed Klocwork warnings.
08/28/09   wsh     CMI: Removing conditional defines from API headers
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
12/10/08   smd     Replaced assert.h with amssassert.h
04/04/05   pba     Removed Lint errors
07/10/02   dna     Allow the address mask to be 128 bits.
03/21/01   dna     hdraddr_mask() - memset HDRADDR_LENGTH_IN_BYTES, not BITS.
03/19/01   dna     New function to get specific bytes of an address
12/04/00   nmn     Added set_octet to set individual octest of the address
10/27/00   nmn     Added hdraddr_set.  Changed hdraddr_mask.  Improved 
                   readability of unpack_addr and pack_addr.
10/24/00   vas     Created module.

===============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

===============================================================================*/

#include "hdraddr.h"
#include "hdraddr_v.h"
#include "hdrbit.h"
#include "amssassert.h"
#include "memory.h"
#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

FUNCTION HDRADDR_UNPACK_ADDR

DESCRIPTION
  Unpacks the 128 bit address (ATI or SectorID) from a received message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - dsm item containing message
  ret_val - Pointer to where the unpacked value will be placed
  offset - Offset of the field in bits from the start of message

RETURN VALUE
  E_SUCCESS - If unpacking succeeds
  E_FAILURE - Otherwise

SIDE EFFECTS
  None

=============================================================================*/
hdrerrno_enum_type hdraddr_unpack_addr
(
  dsm_item_type *item_ptr,
  hdraddr_type  ret_val,
  uint16 offset
)
{
  uint16 index;    /* Index Counter */
  uint16 err = 0;  /* Records unpack return values */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < HDRADDR_LENGTH_IN_BYTES; index++) 
  {
    err |= (uint16) HDRBIT_UNPACK8( item_ptr, &ret_val[index], 
                    (uint16)(offset + (index * 8)), 8 );
  } 
  return ( hdrerrno_enum_type ) ((err == 0) ? E_SUCCESS : E_FAILURE); 
} /* hdraddr_unpack_addr */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRADDR_PACK_ADDR

DESCRIPTION
  Packs the 128 bit address (ATI or SectorID) to a dsm item.

DEPENDENCIES
  None

PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - 128 bit address to be packed
  offset    - Offset at which addr is packed

RETURN VALUE
  None

SIDE EFFECTS
  If the dsm item is full, a new dsm item may be acquired.  Failure to acquire 
  a new dsm item when necessary is a fatal condition.

=============================================================================*/
void hdraddr_pack_addr
(
  dsm_item_type * item_ptr,
  hdraddr_type  pack_data,
  uint16 offset
)
{
  uint8 index;    /* Index Counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < HDRADDR_LENGTH_IN_BYTES; index++) 
  {
    HDRBIT_PACK8( item_ptr, pack_data[index], 
                  (uint16)(offset + (index * 8)), 8);
  } 
} /* hdraddr_pack_addr */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRADDR_IS_EQUAL

DESCRIPTION
  Compares two 128 bit addresses.

DEPENDENCIES
  None

PARAMETERS
  addr1 - First 128 bit address
  addr2 - Second 128 bit address

RETURN VALUE
  TRUE  - If the 2 addresses match
  FALSE - otherwise

SIDE EFFECTS
  None.

=============================================================================*/
boolean hdraddr_is_equal
(
  hdraddr_type addr1 ,
  hdraddr_type addr2 
)
{
  int index;   /* Index Counter */
  boolean ret_val = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare each of the 16 bytes in the 128 bit address */
  for(index = 0; index < HDRADDR_LENGTH_IN_BYTES; index++)
  {
    if(addr1[index] != addr2[index])
    {
      ret_val = FALSE;
    }
  }

  return ret_val;

} /* hdraddr_is_equal */

/* <EJECT> */
/*=============================================================================

FUNCTION HDRADDR_SET

DESCRIPTION
  Sets one address equal to the value of another.

DEPENDENCIES
  None

PARAMETERS
  addr1 - address to be assigned a value
  addr2 - address with value to assign

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdraddr_set
(
  hdraddr_type addr1,
  hdraddr_type addr2
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( addr1, HDRADDR_LENGTH_IN_BYTES, addr2, HDRADDR_LENGTH_IN_BYTES );
#else
  memcpy( addr1, addr2, HDRADDR_LENGTH_IN_BYTES );
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdraddr_set */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRADDR_MASK

DESCRIPTION
  Given a 128 bit address (in_addr), this function generates a new version 
  (ret_addr) with only the upper (keep) bits set, and the rest set to zero.

DEPENDENCIES
  None

PARAMETERS
  ret_addr   - 128 bit result of masking the address.
  in_addr    - 128 bit address to be masked
  keep       - Number of upper bits of in_addr to copy to ret_addr

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void hdraddr_mask
(
  hdraddr_type ret_addr, 
  hdraddr_type in_addr, 
  uint8 keep 
)
{
  uint8 keep_bytes = keep/8; /* How many bytes of in_addr to keep */
  uint8 keep_bits = keep%8;  /* The remaining bits of in_addr to keep */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( keep < HDRADDR_LENGTH_IN_BITS );

  /*---------------------------------------------------------------------------
  Set value of subnet id to 0 
  ---------------------------------------------------------------------------*/
  memset( ret_addr, 0, HDRADDR_LENGTH_IN_BYTES );

  /*---------------------------------------------------------------------------
  Set those bytes which are part of the subnet 
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( ret_addr, keep_bytes, in_addr, keep_bytes);
#else
  memcpy( ret_addr, in_addr, keep_bytes);
#endif /* FEATURE_MEMCPY_REMOVAL */

  /*---------------------------------------------------------------------------
  If the mask was not an integer number of bytes, the last bits of in_addr
  still need to be copied into ret_addr.
  ---------------------------------------------------------------------------*/
  if ((keep_bits)!=0)
  {
    ret_addr[keep_bytes] = in_addr[keep_bytes] & (uint8)(0xff << (8-keep_bits));
  }
} /* hdraddr_mask */

/*=============================================================================

FUNCTION HDRADDR_SET_OCTET

DESCRIPTION
  Assigns a value to a specified octet of a 128-bit address.  The octet value
  is in the range [0..HDRADDR_LENGTH_IN_BYTES-1].

DEPENDENCIES
  None

PARAMETERS
  addr   -  128 bit address.
  set_val - value to set into the specified octet
  octet  -  which octet to set

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void hdraddr_set_octet
(
  hdraddr_type ret_addr, 
  uint8 set_val,
  uint8 octet
)
{
  ret_addr[octet] = set_val;
} /* hdraddr_set_octet */

/*=============================================================================

FUNCTION HDRADDR_COPY_SPECIFIC_OCTETS

DESCRIPTION
  Copies only the requested octets from one hdraddr into the MSB of a
    byte array

DEPENDENCIES
  None

PARAMETERS
  result - array of bytes into which the requested octets are to be copied
  from_addr - hdraddr from which the requested octets are to be obtained
  start_byte - bytes from the MSB of the hdraddr to start copying data
  num_bytes - number of bytes to copy from the hdraddr into the array of bytes

RETURN VALUE
  None.

SIDE EFFECTS
  result must point to an array of bytes at least as long as the value passed
    into num_bytes

=============================================================================*/
void hdraddr_copy_specific_octets
(
  uint8 * result,
  hdraddr_type from_addr,
  uint16 start_byte,
  uint16 num_bytes
)
{
  uint8 count;

  for (count = 0; count < num_bytes; count++)
  {
    result[count] = from_addr[count + start_byte];
  }
} /* hdraddr_copy_specific_octets */

