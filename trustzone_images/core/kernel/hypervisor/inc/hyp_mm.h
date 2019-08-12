#ifndef HYP_MM_H
#define HYP_MM_H

/**
@file hyp_mm.h
@brief Hypervisor Memory Management

Contains Macros for logging HYP debug messages. By default all 
logs go to the internal ring buffer.

*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/inc/hyp_mm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
05/21/14   dc       Created

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*=========================================================================== 
    Preprocessor Definitions and Constants
============================================================================*/


/*===========================================================================

          FUNCTIONS

============================================================================*/
int32 hyp32_grant_access(uint64_t ipa, uint64_t size, uint64_t perm, uint64_t cache, uint64_t share);
int32 hyp32_remove_access(uint64_t ipa, uint64_t size);

#endif /* HYP_MM_H */
