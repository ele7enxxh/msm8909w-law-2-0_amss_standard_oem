#ifndef MISCASM_H
#define MISCASM_H

/**
  @file miscasm.h
  @brief This module contains definitions and declarations for misceallaneous 
  assembly-level routines used for quickly transfering data between memory and 
  MSM registers.

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

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             M I S C    A S S E M B L Y    L E V E L    R O U T I N E S
                  
                          H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for misceallaneous assembly-
  level routines used for quickly transfering data between memory and MSM
  registers.
  
Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/miscasm.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated File.
08/17/04   rp      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"



/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/** @addtogroup utils_services
@{ */

/**
  Writes a specified number of words to a burst interface data register. It
  is assumed that the register is ready to receive data when called. 

  @param[in] data_ptr  Pointer to the data to be loaded.
  @param[out] dest_reg Register address for writing the burst data.
  @param[in] num_words Number of words to be written.

  @return
  None.

  @dependencies
  The register must be ready to receive the data.
*/
void miscasm_write_burst_data
(
  uint32 *data_ptr,
  uint32 *dest_reg, 
  uint16 num_words
);


/**
  Reads a specified number of words from a burst interface data register. 

  @param[in] source_reg Register address for reading the burst data.
  @param[out] dest_ptr  Pointer to the data to be loaded.
  @param[in] num_words  Number of words to be read.

  @return
  None.

  @dependencies
  The register must be ready to be read.
*/
void miscasm_read_burst_data
(
  uint32 *source_reg,
  uint32 *dest_ptr, 
  uint16 num_words
);

/** @} */ /* end_addtogroup utils_services */
#endif /* MISCASM_H */
