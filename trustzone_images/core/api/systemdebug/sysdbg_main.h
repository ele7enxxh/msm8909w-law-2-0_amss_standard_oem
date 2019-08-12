#ifndef _SYSDBG_MAIN_H
#define _SYSDBG_MAIN_H

/*=============================================================================
                        SYSDBG MAIN Header File 

GENERAL DESCRIPTION     
  This module defines functions for intializing SYSDBG functionalities.
  
	Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#include "msmhwio.h"
#include "comdef.h"
#include "HALhwio.h"
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---          -------------------------------------------------- 
01/14/2014 	davidai	Ported SDI to Bear

===========================================================================*/

/*===========================================================================
**  Function :  SYSTEM_DEBUG_MAIN
** ==========================================================================
*/
/*!
* 
* @brief
* 	Main SYSDBG
* 
* @param[in] 
*  	uint32 pass_count
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*
*/
void system_debug_main(void);

/*===========================================================================
**  Function : SYSDBG_RESET_CHECK
** ==========================================================================
*/
/*!
* 
* @brief
*   API used to check for whether we're in cold boot or WDT reset boot
*
* @param[in] 
*	
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   
*
*
*/


boolean sysdbg_reset_check(void);

#endif /* _SYSDBG_MAIN_H */
