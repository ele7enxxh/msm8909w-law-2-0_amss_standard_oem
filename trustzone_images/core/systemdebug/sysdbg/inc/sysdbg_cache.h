/*=============================================================================
           			 D B I  	C A C H E 	H A N D L E R 

GENERAL DESCRIPTION     
		  The file defines the functions to control enabling and flushing of the 
		  caches in the debug image. There is nothing target specific about this 
		  file and should be maintained across all targets. 
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/inc/sysdbg_cache.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
02/19/12	AJCheriyan	First cut for Aragorn. First ever sysdbg

===========================================================================*/


/* ===========================================================================
 *							H E A D E R		F I L E S 	
 * =========================================================================*/


#ifndef _sysdbg_CACHE_H
#define _sysdbg_CACHE_H

/*===========================================================================
**  Function : SYSDBG_DCACHE_FLUSH
** ==========================================================================
*/
/*!
* 
* @brief
*	It flushes  all levels of data cache to the main memory and and invalidates them.
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*
*/

void sysdbg_dcache_flush(uint32 l1_valid, uint32 l2_valid);





#endif /* _sysdbg_CACHE_H */



