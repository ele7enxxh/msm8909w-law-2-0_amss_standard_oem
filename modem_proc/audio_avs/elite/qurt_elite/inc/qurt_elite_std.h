#ifndef QURT_ELITE_STD_H
#define QURT_ELITE_STD_H

/*======================================================================
Copyright (c) 2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/**
@file qurt_elite_std.h

@brief This file contains standard C functions which are missing from HEXAGON tools
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_std.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
09/07/11   sivapava     Created file.
========================================================================== */

#include "mmdefs.h"

/* =======================================================================
**                          Function Declarations
** ======================================================================= */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**	@ingroup qurt_elite_std
  Copies the source string to a destination.
 
  @param[in] dest_ptr  Pointer to the destination string.
  @param[in] src_ptr   Pointer to the source string.
  @param[in] dest_len  Length of the destination buffer.

  @return 
  src_len -- Source string size.
 
  @dependencies
  None.
*/ 
uint32_t qurt_elite_strl_cpy(char_t *dest_ptr, const char_t *src_ptr, uint32_t dest_len);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* #ifndef QURT_ELITE_STD_H */
