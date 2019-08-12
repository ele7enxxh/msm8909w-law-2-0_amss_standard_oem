
/*=============================================================================
                       SYSDBG 	A S S E M B L Y 	M A C R O S

GENERAL DESCRIPTION     
  Defines common assembly macros to be used 
  
	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when       who          what, where, why
--------   ---          -------------------------------------------------- 
06/26/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/


#ifndef _SYSDBG_ASM_H
#define _SYSDBG_ASM_H


__asm __inline void __DSB(void)
{
	DSB
	BX lr
}

__asm __inline void __DMB(void) 
{
	DMB
	BX lr
}

__asm __inline void __ISB(void) 
{    
	ISB
	BX lr
}

#endif /* _SYSDBG_ASM_H */
