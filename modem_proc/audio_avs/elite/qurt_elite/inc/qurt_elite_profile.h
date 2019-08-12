/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_profile.h

@brief This file contains profiling utilities. This file provides functions
to read the processor cycles count (PCycle).
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_profile.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */


#ifndef QURT_ELITE_PROFILE_H
#define QURT_ELITE_PROFILE_H

#include "qurt.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

//***************************************************************************
// Profiling
//***************************************************************************

/** @ingroup qurt_elite_profile
  Gets the number of PCycles since bootup.

  @return
  Total number of PCycles elapsed since bootup.

  @dependencies
  None.
*/
static inline uint64_t qurt_elite_profile_get_pcycles(void)
{
   return qurt_get_core_pcycles();
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_PROFILE_H

