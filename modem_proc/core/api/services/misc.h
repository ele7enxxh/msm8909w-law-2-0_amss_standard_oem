#ifndef MISC_H
#define MISC_H

/**
  @file misc.h
  @brief This file contains delarations necessary to use the utilities contained
  in misc.c.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_services" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

            M I S C E L L A N E O U S   U T I L I T I E S
                      H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the utilities contained
  in misc.c.

Copyright (c) 1992-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/misc.h_v   1.0   Aug 21 2000 11:51:06   lpetrucc  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/misc.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/21/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated File.
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


/** @addtogroup utils_services
@{ */

/**
  Reads a specific number of bytes from the specified byte-wide I/O port.
 
  @param[in] port    Port number from which to read the bytes.
  @param[in] buf_ptr Address of the buffer into which the successive bytes 
                     are read.
  @param[in] cnt     Number of bytes to be read from the port.

  @return
  None.

  @dependencies
  None.
*/
extern void insb_rep
(
  void * port,

  void *buf_ptr,

  word cnt
);


/**
  Writes a specific number of bytes to the specified byte-wide I/O port.
 
  @param[in] port    Port number to which the bytes are written.
  @param[in] buf_ptr Address of the buffer containing the byte values 
                     to write to the port.
  @param[in] cnt     Number of bytes to be written to the port.

  @return
  None.

  @dependencies
  None.
*/
extern void outsb_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);



/**
  Reads a specific number of half-words (2 bytes) from the specified
  half-word-wide (2 bytes) I/O port.
 
  @param[in] port    Port number from which to read the half-word.
  @param[in] buf_ptr Address of the buffer into which the successive 
                     half-words are read.
  @param[in] cnt     Number of half-words to be read from the port.

  @return
  None.

  @dependencies
  None.
*/
extern void insh_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);




/**
  Writes a specific number of half-words (2 bytes) to the specified
  half-word-wide (2 bytes) I/O port.
 
  @param[in] port    Port number to which the half-words are written.
  @param[in] buf_ptr Address of the buffer containing the half-word values 
                     to write to the port.
  @param[in] cnt     Number of half-words (2 bytes) to be written to the port.

  @return
  None.

  @dependencies
  None.
*/
extern void outsh_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);


/**
  Reads a specific number of (8-bit) bytes from the specified 
  16-bit word-wide (2 bytes) I/O port and returns them in an aligned byte 
  sequence.

  @param[in] port     Port number from which to read the 16-bit word (2 bytes).
  @param[out] buf_ptr Address of the buffer into which the successive bytes 
                      are read.
  @param[in] cnt      Number of bytes to be read from the port.

  @return
  None.

  @dependencies
  None.
*/
extern void insbh_rep
(
  void *   port,
  void  *buf_ptr,
  word  cnt
);


/**
  Writes a specific number of (8-bit) bytes to the specified 
  16-bit word-wide (2 bytes) I/O port.

  @param[in] port    Port number to which the specified bits are written.
  @param[in] buf_ptr Address of the buffer containing the byte values 
                     to write to the port.
  @param[in] cnt     Number of bytes to be written to the port.

  @return
  None.

  @dependencies
  None.
*/
extern void outsbh_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);



/**
  Divides a specific 32-bit unsigned dividend by the specified 
  16-bit unsigned divisor. Both the quotient and remainder are returned.

  @param[in] dividend Dividend; note that this is a dword.
  @param[in] divisor  Divisor.
  @param[in] rem_ptr  Pointer to the remainder.

  @return
  Quotient of the dividend/divisor.

  @dependencies
  None.
*/
extern dword div4x2
(
  dword dividend,
  word  divisor,
  word  *rem_ptr
);


/**
  Reads a specific number of words (4 bytes) from the specified word port.
 
  @param[in] port    Port number of the word-wide (4 bytes) I/O port from 
                     which to read the word.
  @param[in] buf_ptr Address of the buffer into which the successive 
                     words are read.
  @param[in] cnt     Number of words to be read from the port.

  @return
  None.

  @dependencies
  None.
*/
extern void insw_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);


/**
  Writes a specific number of words (4 bytes) to the specified word port.

  @param[in] port    Port number of the word-wide (4 bytes) I/O port to  
                     which the words are written.
  @param[in] buf_ptr Address of the buffer containing the word values 
                     to write to the port.
  @param[in] cnt     Number of words (4 bytes) to be written to the port.

  @return
  None.

  @dependencies
  None.
*/
extern void outsw_rep
(
  void * port,
  void *buf_ptr,
  word  cnt
);

/** @} */ /* end_addtogroup utils_services */
#endif  /* MISC_H */
