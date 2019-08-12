#ifndef MD5_H
#define MD5_H

/**
  @file md5.h
  @brief Header file for MD5 Hashing functions.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_md5" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

 MD5.H - header file for MD5C.C

 Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 rights reserved.

 License to copy and use this software is granted provided that it
 is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 Algorithm" in all material mentioning or referencing this software
 or this function.

 License is also granted to make and use derivative works provided
 that such works are identified as "derived from the RSA Data
 Security, Inc. MD5 Message-Digest Algorithm" in all material
 mentioning or referencing the derived work.

 RSA Data Security, Inc. makes no representations concerning either
 the merchantability of this software or the suitability of this
 software for any particular purpose. It is provided "as is"
 without express or implied warranty of any kind.

 These notices must be retained in any copies of any part of this
 documentation and/or software.

 ===========================================================================*/

/* <EJECT> */

/*
Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/md5.h_v   1.1.1.0   30 Nov 2001 17:45:28   fchan  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/md5.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
05/03/06   ck      Removed all the featurizations and mainlined it.
10/01/01   sb      Merged the following from the common archive:
08/15/01   ss      Added FEATURE_DS_MOBILE_IP to include the file for CHAP or 
                   MOBILE_IP.
03/14/00   cah     Added FEATURE_DS_CHAP to include this file for snn.
06/25/98   ck      Checked in the file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */
/*=========================================================================*/
/** @addtogroup utils_md5
@{ */

/** Defines a generic pointer type.
*/
typedef unsigned char *POINTER;

/** @brief MD5 context.
*/
typedef struct {
  dword state[4];            /**< State (ABCD). */
  dword count[2];            /**< Number of bits; modulo 2^64 (LSB first). */
  unsigned char buffer[64];  /**< Input buffer. */
} MD5_CTX;

/**
  Initalizes the MD5 context.

  @param[out] MD5_CTX Context for the MD5 operation.

  @return
  None.

  @dependencies
  None.
*/
extern void MD5Init (MD5_CTX *);

/**
  MD5 block update operation.  This function continues the MD5 
  operation, processing another message block and updating the context.

  @param[in,out] MD5_CTX Context to be updated.
  @param[in] char        Block of input.
  @param[in] int         Length of the input block.

  @return
  None.

  @dependencies
  None.
*/
extern void MD5Update (MD5_CTX *, unsigned char *, unsigned int);

/**
  Final step of the MD5 operation.

  @param[out] char[16]   Buffer where the digest is to be written.
  @param[in,out] MD5_CTX Context to generate the digest.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Context is zeroed out at the end of this function.
*/
extern void MD5Final (unsigned char [16], MD5_CTX *);

/** @} */ /* end_addtogroup utils_md5 */
#endif /* MD5_H */
