#ifndef MISC_H
#define MISC_H
/*===========================================================================

            M I S C E L L A N E O U S   U T I L I T I E S
                      H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the utilities contained
  in misc.c.

Copyright (c) 1991-1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/misc.h_v   1.0   Aug 21 2000 11:51:06   lpetrucc  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/misc.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/28/05   vlc     Added prototype for insw_rep() and outsw_rep. 
03/17/99   ms      Removed comments related bb_ for Virtual Boot Block.
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporate cdecl and MSM3000 changes
05/14/92   jah     Ported from the brassboard

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

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
extern void insb_rep
(
  void * port,
    /* Specifies the port of number of the byte-wide I/O port from which
       to read bytes. */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive bytes
       will be read. */

  word cnt
    /* Specifies the number of bytes to read from the port. */
);


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
extern void outsb_rep
(
  void * port,
    /* Specifies the port of number of the byte-wide I/O port to which
       the bytes should be written. */

  void *buf_ptr,
    /* Specifies the address of the buffer containing the byte values to
       write to the port. */

  word  cnt
    /* Specifies the number of bytes to write to the port. */
);



/*===========================================================================

FUNCTION INSH_REP

DESCRIPTION
  This procedure reads a specified number of half-words (2 bytes) from a
  specified half-word port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void insh_rep
(
  void * port,
    /* Specifies the port of number of the half word-wide (2 bytes) I/O port from which
       to read half-word (2 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive half-words
       will be read. */

  word  cnt
    /* Specifies the number of half-words to read from the port. */
);



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
extern void outsh_rep
(
  void * port,
    /* Specifies the port of number of the half word-wide (2 bytes) I/O port
       to which the half words should be written. */

  void *buf_ptr,
    /* Specifies the address of the buffer containing the half word values
       to write to the port. */

  word  cnt
    /* Specifies the number of half words (2 bytes) to write to the port. */
);


/*===========================================================================

FUNCTION INSBH_REP

DESCRIPTION
  This procedure reads a specified number of (8 bit) bytes from a specified 16
  bit word port and  returns them aligned in the form of byte sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void insbh_rep
(
  void *   port,
    /* Specifies the port of number of the 16 bit word-wide (2 bytes) I/O port
       from which to read 16 bit word (2 bytes). */

  void  *buf_ptr,
    /* Specifies the address of the buffer into which successive bytes
       will be stored. */

  word  cnt
    /* Specifies the number of bytes to be read from the port. */
);


/*===========================================================================
FUNCTION OUTSBH_REP

DESCRIPTION
  This procedure writes a specified number of (8 bit) bytes to a specified 16
  bit word port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void outsbh_rep
(
  void * port,
    /* Specifies the port of number of the 16 bit word-wide (2 bytes) I/O port
       to which the specified bits are to be written */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive bytes
       will be stored. */

  word  cnt
    /* Specifies the number of bytes to be written to the port. */
);



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
extern dword div4x2
(
  dword dividend,   /* Dividend, note dword     */
  word  divisor,    /* Divisor                  */
  word  *rem_ptr    /* Pointer to the remainder */
);

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
extern void insw_rep
(
  void * port,
    /* Specifies the port of number of the word-wide (4 bytes) I/O port from 
       which to read word (2 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive words
       will be read. */

  word  cnt
    /* Specifies the number of words to read from the port. */
);

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
extern void outsw_rep
(
  void * port,
    /* Specifies the port of number of the word-wide (4 bytes) I/O port
       from which to write word (4 bytes). */

  void *buf_ptr,
    /* Specifies the address of the buffer into which successive words
       will be written. */

  word  cnt
    /* Specifies the number of words to write to the port. */
);

#endif  /* MISC_H */
