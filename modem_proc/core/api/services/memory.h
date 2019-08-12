#ifndef MEMORY_H
#define MEMORY_H

/**
  @file memory.h
  @brief The ARM compiler does not include a memory.h header, so we provide one
  here and point to the standard ARM include file which does have
  memory prototypes.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_memory" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*
Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/memory.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated File.
09/13/05   ck      Copied over from 6100 branch
07/12/02   jct     <string.h> has ANSI defines for mem* functions
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Modified to pull in string.h for NT builds.
11/03/98   jct     Added the else clause to include <memory.h> for non-ARM builds.
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created

===========================================================================*/
/** @addtogroup utils_memory
@{ */
#include <string.h>
/** @} */ /* end_addtogroup utils_memory */
#endif /* MEMORY_H */
