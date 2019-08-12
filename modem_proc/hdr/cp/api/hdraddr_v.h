#ifndef HDRADDR_V_H
#define HDRADDR_V_H

/*===========================================================================

                        H D R   A D D R E S S 

DESCRIPTION
  This contains the definitions & declaration related to 128 bit HDR 
  addressing scheme.

Copyright (c) 2000 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdraddr.h_v   1.6   19 Mar 2001 21:12:58   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdraddr_v.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: split header into public and private
03/19/01   dna     New function to get specific bytes of an address
12/04/00   nmn     Added set_octet to set individual octest of the address
10/27/00   nmn     Added hdraddr_set.  Changed parameter name in pack_addr.
10/24/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "dsm.h"

#ifdef FEATURE_CMI
#include "hdraddr.h"
#endif

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/

/*=============================================================================

                        FUNCTION DECLARATIONS 

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
);

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
);

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
);

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
  None.

SIDE EFFECTS
  None

=============================================================================*/
void hdraddr_set
(
  hdraddr_type addr1, 
  hdraddr_type addr2
);

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
);

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
);

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
);

#endif /* HDRADDR_V_H */
