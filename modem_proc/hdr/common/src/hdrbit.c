/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R  S P E C I F I C  B I T  M A N I P U L A T I O N 
                             S E R V I C E S

GENERAL DESCRIPTION
  This file contains bit manipulation routines specific to HDR. It has
  unpacking routines for extracting fields of a received message in contained
  in dsm items.

EXTERNALIZED FUNCTIONS
  hdrbit_pack8    - Calls the DSM routine to pack up to 8 bits

  hdrbit_pack16   - Calls the DSM routine to pack up to 16 bits

  hdrbit_pack32   - Calls the DSM routine to pack up to 32 bits

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdrbit.c_v   1.10   02 Jan 2001 21:09:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrbit.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/08   smd     Replaced assert.h with amssassert.h
11/27/00   nmn     Rewrote entire file.  It now calls dsmbit pack functions
                   and ERR_FATALs if the pack is not successful.
07/27/00   nmn     Added pack functions
05/15/00   vas     Created module.

===========================================================================*/

/* <EJECT> */


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "amssassert.h"
#include "hdrbit.h"
#include "dsmbit.h"
#include "bit.h"
#include "err.h"

/*============================================================================

                        DATA DECLARATIONS

=============================================================================*/

/*===========================================================================

                        FUNCTION DEFINITIONS

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
)
{
  if (dsmbit_pack8_tail(item_ptr, pack_data, offset, len) != len)
  {
    ERR_FATAL("Message Pack Failure",0,0,0);
  }
} /* hdrbit_pack8() */

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
)
{
  if (dsmbit_pack16_tail(item_ptr, pack_data, offset, len) != len)
  {
    ERR_FATAL("Message Pack Failure",0,0,0);
  }
} /* hdrbit_pack16() */

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
)
{
  if (dsmbit_pack32_tail(item_ptr, pack_data, offset, len) != len)
  {
    ERR_FATAL("Message Pack Failure",0,0,0);
  }
} /* hdrbit_pack32() */
