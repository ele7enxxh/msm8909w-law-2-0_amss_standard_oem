#ifndef MISCASM_H
#define MISCASM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             M I S C    A S S E M B L Y    L E V E L    R O U T I N E S
                  
                          H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for misceallaneous assembly-
  level routines used for quickly transfering data between memory and MSM
  registers.
  
  Copyright (c) 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/miscasm.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/04   rp     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"



/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION MISCASM_WRITE_BURST_DATA

DESCRIPTION
  
  Write a specified number of words to a burst interface data register. It
  is assumed that the register is ready to receive data when called. 
  Parameters are the burst interface data register address, the address 
  of the memory to load, and a word count.

DEPENDENCIES
  The burst interface register is ready to receive data.
  
PARAMETERS
  data_ptr  - pointer to the data to load.
  dest_reg  - the register address for writing burst data.
  num_words - the number of words to write.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void miscasm_write_burst_data
(
  uint32 *data_ptr,
  uint32 *dest_reg, 
  uint16 num_words
);


/*===========================================================================
FUNCTION MISCASM_READ_BURST_DATA

DESCRIPTION
  
  Read a specified number of words from a register. Parameters are the 
  data register address, the destination address in memory, and a word 
  count.

DEPENDENCIES
  The burst interface register must be ready to be read.
  
PARAMETERS
  source_reg - pointer to the data to load.
  dest_ptr   - the register address.
  num_words - the number of words to write.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void miscasm_read_burst_data
(
  uint32 *source_reg,
  uint32 *dest_ptr, 
  uint16 num_words
);

#endif /* MISCASM_H */
