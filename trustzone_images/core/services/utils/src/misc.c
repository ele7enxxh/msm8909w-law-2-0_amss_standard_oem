/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               M I S C E L L A N E O U S   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains generic utilites used by many different modules.

EXTERNALIZED FUNCTIONS
  insb_rep
    This inputs a specified number of bytes from a specified port using
    a string I/O operation.
  outsb_rep
    This function ouputs a specified string of bytes to a specified byte
    port.
  div4x2
    This function divides a specified 32 bit unsigned dividend by a
    specified 16 bit unsigned divisor.
  
  insh_rep
    Input specified number of half words from a half word port
  insbh_rep
    Input specified number of bytes from a half word port
  outsh_rep
    Outputs specified number of half words to a half word port
  outsbh_rep
    Outputs specified number of bytes to a half word port

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1990,1991,1992 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1996,1998,1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/misc.c_v   1.1   29 Jan 2001 10:52:30   dyang  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/misc.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/06   tmz     Brought insw_rep and outsw_rep from 6800 branch
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Added support for MSM5000_IRAM_FWD
04/09/99    ms     Lint cleanup.
03/17/99    ms     Changes for Virtual Boot Block.
                     Removed bb_ prefix
                     Not including bbjmpout.h
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporate cdecl and MSM3000 changes
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Prepended bb_ to function names and put module in Boot
                   Block.
05/14/92   jah     Ported from the brassboard

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.    */
#include "processor.h"
#include "misc.h"               /* Miscellaneous utilities prototypes  */
#include "msmhwio.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION INSB_REP

DESCRIPTION
  This procedure reads a specified number of bytes from a specified
  byte port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void insb_rep
(
  void * port,
    /* Specifies the port of number of the byte-wide I/O port from which
       to read bytes. */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive bytes
       will be read. */

  word  cnt
    /* Specifies the number of bytes to read from the port. */
)
{
  int x=0;

  while(x < cnt) ((byte *) buf_ptr)[x++] = inp( (byte *) port);

  return;
} /* END insb_rep */



#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION INSH_REP

DESCRIPTION
  This procedure reads a specified number of half words (2 bytes) from a
  specified half word port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void insh_rep
(
  void * port,
    /* Specifies the port of number of the half word-wide (2 bytes) I/O port
       from which to read half-word (2 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive half-words
       will be read. */

  word  cnt
    /* Specifies the number of half-words to read from the port. */
)
{

  int x=0;

  while(x < cnt) ((word *) buf_ptr)[x++] = inpw ( (word *) port);

  return;
} /* END insh_rep */
#endif /*MSM5000_IRAM_FWD*/


/*===========================================================================

FUNCTION INSBH_REP

DESCRIPTION
  This procedure reads a specified number of (8 bit) bytes from a specified 16
  bit word port and returns them aligned in the form of byte sequence.


  This function reads word port containing 2 byte word b1b0 as
  b1- First byte
  b0- Second byte

  If last word read is assumed to be bn00.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void insbh_rep
(
  void * port,
    /* Specifies the port of number of the 16 bit word-wide (2 bytes) I/O port
       from which to read 16 bit word (2 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive bytes
       will be stored. */

  word  cnt
    /* Specifies the number of bytes to be read from the port. */
)
{

   int x = 0, y = 0;
   word temp;

   while( x < cnt/2 ) {
     temp = inpw( (word *)port );
     ((byte *) buf_ptr)[y++] = (byte) (temp >> 8);
     ((byte *) buf_ptr)[y++] = (byte) (temp & 0xff);
     x++;
   }

   /* If the count is odd number then the remaining byte will be read here. 
   ** Note that this byte will be MSbyte of 16 bit word and hence should be
   **  shifted down
   ** AJ/HK This implies your byte counting is MSbyte=x, LSbyte=x+1 is this 
   ** always true?
   */

   if( cnt%2 != 0 ) {
     ((byte *) buf_ptr)[y] = (byte) (inpw( (word *) port) >> 8);
   }

   return;
} /* END insbh_rep */


/*===========================================================================

FUNCTION OUTSB_REP

DESCRIPTION
  This procedure writes a specified number of bytes to a specified byte
  port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void outsb_rep
(
  void * port,
    /* Specifies the port of number of the byte-wide I/O port to which
       the bytes should be written. */

  void *buf_ptr,
    /* Specifies the address of the buffer containing the byte values to
       write to the port. */

  word  cnt
    /* Specifies the number of bytes to write to the port. */
)
{
  int x=0;

  while(x < cnt) outp( (byte *) port, ((byte *) buf_ptr)[x++]);

  return;
} /* END outsb_rep */



/*===========================================================================

FUNCTION OUTSH_REP

DESCRIPTION
  This procedure writes a specified number of half words to a specified
  half word (2 bytes) port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void outsh_rep
(
  void * port,
    /* Specifies the port of number of the half word-wide (2 bytes) I/O port
       to which the half words should be written. */

  void *buf_ptr,
    /* Specifies the address of the buffer containing the half word values
       to write to the port. */

  word  cnt
    /* Specifies the number of half words (2 bytes) to write to the port. */
)
{
   int x=0;

   while (x < cnt) outpw ( (word *) port, ((word *) buf_ptr)[x++]);

   return;

} /* END outsh_rep */

#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION OUTSBH_REP

DESCRIPTION
  This procedure writes a specified number of (8 bit) bytes to a specified 16
  bit word port.

  This function writes MSword = x and LSword = x+1 th byte.

  e.g. if the bytes to be written are
   b0
   b1
   b2
   b3
   b4
   the words written on the port are

   b1b0
   b3b2
   b400

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void outsbh_rep
(
  void * port,
    /* Specifies the port of number of the 16 bit word-wide (2 bytes) I/O port
       to which the specified bits are to be written */

  void *buf_ptr,
    /* Specifies the address of the buffer from which successive bytes
       will be read. */

  word  cnt
    /* Specifies the number of bytes to be written to the port. */
  )
{
  int x=0;
  word temp;

  while( x < (cnt / 2) * 2 )
        {
         temp = (word) ((byte *) buf_ptr)[x++];
         temp = (word) ( temp | ( ((byte *)  buf_ptr)[x++] << 8 ) );
         outpw( (word *) port, temp );
        }
  if( cnt % 2 != 0 )
        {
         outpw( (word *) port, (word) ((byte *) buf_ptr)[x] );
        }
  return;
} /* END outsbh_rep */
#endif /*MSM5000_IRAM_FWD*/


/*===========================================================================

FUNCTION DIV4X2

DESCRIPTION
  This procedure divides a specified 32 bit unsigned dividend by a
  specified 16 bit unsigned divisor. Both the quotient and remainder
  are returned.

DEPENDENCIES
  None

RETURN VALUE
  The quotient of dividend / divisor.

SIDE EFFECTS
  None

===========================================================================*/
dword div4x2
(
  dword dividend,       /* Dividend, note dword     */
  word  divisor,         /* Divisor                  */
  word  *rem_ptr    /* Pointer to the remainder */
)
{
  *rem_ptr = (word) (dividend % divisor);

  return (dividend / divisor);

} /* END div4x2 */


/*===========================================================================

FUNCTION INSW_REP

DESCRIPTION
  This procedure reads a specified number of words (4 bytes) from a
  specified word port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void insw_rep
(
  void * port,
    /* Specifies the port of number of the word-wide (4 bytes) I/O port
       from which to read word (4 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive words
       will be read. */

  word  cnt
    /* Specifies the number of words to read from the port. */
)
{

  int x=0;

    /* dword = 4 bytes */
  while(x < cnt) ((dword *) buf_ptr)[x++] = in_dword ( (dword *) port);

  return;
} /* END insw_rep */

/*===========================================================================

FUNCTION outsw_rep

DESCRIPTION
  This procedure write a specified number of words (4 bytes) to a
  specified word port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void outsw_rep
(
  void * port,
    /* Specifies the port of number of the word-wide (4 bytes) I/O port
       from which to write word (4 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive words
       will be written. */

  word  cnt
    /* Specifies the number of words to write to the port. */
)
{

  int x=0;

  /* dword = 4 bytes */
  while(x < cnt) outpdw( (dword *) port, ((dword *) buf_ptr)[x++]);

  return;
} /* END outsw_rep */


