#ifndef PS_BYTE_H
#define PS_BYTE_H

/*===========================================================================
               Miscellaneous machine independent utilities
 ===========================================================================*/
/**
  @file ps_byte.h
  @brief miscellaneous machine independent utilities
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the ipUtilities group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 1995-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsbyte.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_byte.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
12/31/08    pp     Common Modem Interface: Public/Private API split.
10/24/08    pp     Fixed compile warnings observed in OFFTARGET build.
07/17/06    aw     Fixed a bug in get64 function.
08/15/05    msr    Added hput64()
01/27/05    ssh    This is the first incarnation of ps_byte.h, previously
                   known as dsbyte.h. Moved dsbyte.h from
                   //depot/asic/msmshared/mmdata/ds/ to
                   //depot/asic/msmshared/mmdata/ps/, renamed it as ps_byte.h
                   and edited it to include inlined versions of all get*()
                   and put*() functions to potentially improve performance.
                   The function definitions previously resided in
                   //depot/asic/msmshared/mmdata/ds/dsbyte.c, which was
                   deleted.
09/29/04    msr/ks Added hget16,hget32(),hput32(),hput16()
09/09/04    msr    Added put8, get8.
09/06/03    ifk    Added put64, get64().
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
05/24/00    hcg    Added TARGET_OS_SOLARIS to compile with Solaris DSPE.
04/21/00    mvl    Fixed a #define so compiles properly under COMET
01/09/99    jjw    Changed to generic Browser interface
10/27/98    ldg    For T_ARM included C version of TCP checksum routine.
06/16/98    na     Converted the routine that calculates the TCP checksum
                   into 186 assembly.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
07/22/95    jjw    Created Initial Version
===========================================================================*/
/* group: ipUtilities */
#include "comdef.h"
#include "amssassert.h"

/*lint -save -e1534 */


/*===========================================================================
FUNCTION get8
===========================================================================*/
/** @ingroup get8
  Gets the byte value from the 1-byte value.

  @param[in] cp   Pointer to the byte string from which the eight bits are
                  returned.

  @return
  A byte representing the 1-byte value pointed to by the passed parameter.

  @dependencies
  None.
  @newpage
*/
INLINE byte get8
(
  byte *cp
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );
  return (*cp);

} /* get8() */


/*===========================================================================
FUNCTION get16
===========================================================================*/
/** @ingroup get16
  Gets the word value from the 2-byte value.

  @param[in] cp  Pointer to the byte string from which the 16 bits are
                 returned.

  @return
  A word representing the 2-byte value pointed to by the passed parameter.

  @dependencies
  None.
  @newpage
*/
INLINE word get16
(
  byte *cp
)
{
  register word x;       /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  x = *cp++;
  x <<= 8;
  x |= *cp;

  return( x);

} /* get16 */

/*===========================================================================
FUNCTION get32
===========================================================================*/
/** @ingroup get32
  Performs machine-independent, alignment-insensitive, network-to-host long 
  conversions.

  @param[in] cp Pointer to the byte string from which the 32 bits are returned.

  @return
  The 32-bit value representing the four network bytes in the passed character 
  string.

  @dependencies
  None.
  @newpage
*/
INLINE dword get32
(
  byte *cp
)
{
  dword rval;            /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  rval = *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp;

  return( rval);

} /* get32 */


/*===========================================================================
FUNCTION hget16
===========================================================================*/
/** @ingroup hget16
  Extracts 16 bits in host order.

  @param[in] cp  Pointer to the byte string from which the 16 bits are returned 
                 in host order.

  @return
  The 16-bit value in host order that represents the two network bytes in the 
  passed character string.

  @dependencies
  None.
  @newpage
*/
INLINE word hget16
(
  byte *cp
)
{
  word rval;            /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );
  cp += 2;

  rval = *(--cp);
  rval <<= 8;
  rval |= *(--cp);

  return( rval);

} /* hget16() */


/*===========================================================================
FUNCTION hget32
===========================================================================*/
/** @ingroup hget32
  Extracts 32 bits in host order.

  @param[in] cp  Pointer to the byte string from which the 32 bits are returned 
                 in host order.

  @return
  The 32-bit value in host order representing the four network bytes in the passed
  character string.

  @dependencies
  None.
  @newpage
*/
INLINE dword hget32
(
  byte *cp
)
{
  dword rval;            /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );
  cp += 4;

  rval = *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);

  return( rval);

} /* hget32() */

/*===========================================================================
FUNCTION hget64
===========================================================================*/
/** @ingroup hget64
  Extracts 64 bits in host order.

  @param[in] cp  Pointer to the byte string from which the 64 bits are returned
                 in host order.

  @return
  The 64-bit value in host order that represents the eight network bytes in the 
  passed character string.

  @dependencies
  None.
  @newpage
*/
INLINE uint64 hget64
(
  byte *cp
)
{
  uint64 rval;            /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );
  cp += 8;

  rval = *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);

  return( rval);

} /* hget64() */


/*===========================================================================
FUNCTION put8
===========================================================================*/
/** @ingroup put8
  Writes 8 bits to the pointer, and returns the updated pointer.

  @param[out] cp   Buffer to which the 8-bit value is copied.
  @param[in] x     8-bit value to be copied.

  @return
  The updated pointer.

  @dependencies
  None.
  @newpage
*/
INLINE byte *put8
(
  byte *cp,
  byte  x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  *cp++ = x;
  return (cp);

} /* put8() */

/*===========================================================================
FUNCTION put16
===========================================================================*/
/** @ingroup put16
  Writes 16 bits to the pointer, and returns the updated pointer.

  @param[out] cp   Buffer to which the 16-bit value is copied.
  @param[in] x     16-bit value to be copied.

  @return
  The updated pointer.

  @dependencies
  None.
  @newpage
*/
INLINE byte *put16
(
  byte *cp,
  word x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  *cp++ = x >> 8;
  *cp++ = (x & 0xFF);

  return( cp);

} /* put16 */

/*===========================================================================
FUNCTION put32
===========================================================================*/
/** @ingroup put32
  Writes 32 bits to the pointer, and returns the updated pointer.

  @param[out] cp  Buffer to which the 32-bit value is copied.
  @param[in] x    32-bit value to be copied.

  @return
  The updated pointer.

  @dependencies
  None.
  @newpage
*/
INLINE byte *put32
(
  byte *cp,
  dword x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  *cp++ = (byte)(x >> 24);
  *cp++ = (byte)(x >> 16);
  *cp++ = (byte)(x >> 8);
  *cp++ = (byte)x;

  return( cp);

} /* put32 */

/*===========================================================================
FUNCTION hput16
===========================================================================*/
/** @ingroup hput16
  Writes 16 bits to the pointer in host order, and returns the updated pointer.

  @param[out] cp  Buffer to which the 16-bit value is copied in host order.
  @param[in] x    16-bit value to be copied.

  @return
  The updated pointer.

  @dependencies
  None.
  @newpage
*/
INLINE byte *hput16
(
  byte *cp,
  word x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  cp += 2;
  *(--cp) = x >> 8;
  *(--cp) = (byte)x;

  return (cp + 2);

} /* hput16 */

/*===========================================================================
FUNCTION hput32
===========================================================================*/
/** @ingroup hput32
  Writes 32 bits to the pointer in host order, and returns the updated pointer.

  @param[out] cp  Buffer to which the 32-bit value is copied in host order.
  @param[in] x    32-bit value to be copied.

  @return
  The updated pointer.

  @dependencies
  None.
  @newpage
*/
INLINE byte *hput32
(
  byte *cp,
  dword x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  cp += 4;
  *(--cp) = (byte)(x >> 24);
  *(--cp) = (byte)(x >> 16);
  *(--cp) = (byte)(x >> 8);
  *(--cp) = (byte)x;

  return (cp + 4);

} /* hput32 */

/*===========================================================================
FUNCTION PUT64
===========================================================================*/
/** @ingroup put64
  Copies a 64-bit value into a buffer.

  @param[out] cp  Buffer to which the 64-bit value is copied.
  @param[in] x    64-bit value to be copied.

  @return
  The pointer in the buffer after the 64-bit quantity is returned.

  @dependencies
  Sufficient memory must be allocated to the buffer to correctly copy the
  64-bit value.

  @sideeffects
  This function copies the 64-bit quantity into the passed buffer.
  @newpage
*/
INLINE byte *put64
(
  byte   *cp,
  uint64  x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  *cp++ = (byte)(x >> 56);
  *cp++ = (byte)(x >> 48);
  *cp++ = (byte)(x >> 40);
  *cp++ = (byte)(x >> 32);
  *cp++ = (byte)(x >> 24);
  *cp++ = (byte)(x >> 16);
  *cp++ = (byte)(x >> 8);
  *cp++ = (byte)x;

  return( cp);

} /* put64 */

/*===========================================================================
FUNCTION GET64
===========================================================================*/
/** @ingroup get64
  Acquires and returns a 64-bit value from the passed buffer.

  @param[in] cp  Buffer from which the 64-bit value is copied.

  @return
  The 64-bit value acquried.

  @dependencies
  The passed buffer must be at least 64-bit.
  @newpage
*/
INLINE uint64 get64
(
  byte   *cp
)
{
  uint64 rval;            /* The 64-bit quantity to be returned            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  rval = *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp;

  return( rval);

} /* get64() */


/*===========================================================================
FUNCTION HPUT64
===========================================================================*/
/** @ingroup hput64
  Copies a 64-bit value into a buffer in host order.

  @param[out] cp  Buffer to which the 64-bit value is copied.
  @param[in] x    64-bit value to be copied.

  @return
  The pointer in the buffer after the 64-bit quantity is returned.

  @dependencies
  Sufficient memory must be allocated to the buffer to correctly copy the
  64-bit value.

  @sideeffects
  This function copies the 64-bit quantity into the passed buffer.
  @newpage
*/
INLINE byte *hput64
(
  byte   *cp,
  uint64  x
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != cp );

  cp += 8;
  *(--cp) = (byte)(x >> 56);
  *(--cp) = (byte)(x >> 48);
  *(--cp) = (byte)(x >> 40);
  *(--cp) = (byte)(x >> 32);
  *(--cp) = (byte)(x >> 24);
  *(--cp) = (byte)(x >> 16);
  *(--cp) = (byte)(x >> 8);
  *(--cp) = (byte)x;

  return (cp + 8);

} /* hput64 */

/*lint -restore */

#endif /* PS_BYTE_H */
