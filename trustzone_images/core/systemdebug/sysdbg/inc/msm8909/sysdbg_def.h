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
#include "sysdbg_hwio.h"
#include "HALhwio.h"
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---          -------------------------------------------------- 
01/14/2014 	davidai	Ported SDI to Bear

===========================================================================*/

#define TARGET_APPS_CPUS 4
#define SYSTEM_DEBUG_MARKER (SHARED_IMEM_BASE + 0xB14)

#define SYSDBG_DEBUG_ENABLE_FLAG 1

/*SDI debug marker structure.

31               15                 7                  0
+++++++++++++++++++++++++++
+         FUNC ID               + CURR CPU  +
+++++++++++++++++++++++++++
*/


#ifdef SYSDBG_DEBUG_ENABLE_FLAG									
#define SYSDBG_FUNC_START_MARKER(fnc, cpu) 					\
	do{														\
		*((uint32*)(SYSTEM_DEBUG_MARKER)) = (fnc << 8)| cpu;\
	}while(0)
#else	
#define SYSDBG_FUNC_START_MARKER(fnc, cpu) do{}while(0)													
#endif			

/* Sysdbg Functions */
typedef boolean (*sysdbg_fcn_type)(void);
typedef struct
{
	boolean cpu_affinity[TARGET_APPS_CPUS];
	sysdbg_fcn_type sysdbg_fcn;
}sysdbg_info_type;
	
typedef struct
{
	sysdbg_info_type fcn_info[16];
}sysdbg_pass_sequence_type;

typedef struct
{
	sysdbg_pass_sequence_type pass_sequence;
}sysdbg_pass_data_type;

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
**  Function :  SYSDBG_ENABLE
** ==========================================================================
*/
/*!
* 
* @brief
* The function that enables the debug through watchdog feature
* 
* @param[in] 
*   None
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
*/

boolean sysdbg_enable(void);

/*===========================================================================
**  Function :  DEBUG_RESET_BOOT
** ==========================================================================
*/
/*!
* 
* @brief
* The function checks for reset status
* 
* @param[in] 
*   None
*
* @par Dependencies
*   None
* 
* @retval
*   TRUE indicates reset due to crash; FALSE indicates cold boot
* 
* @par Side Effects
*   None
* 
*/

boolean debug_reset_boot(void);

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

/*===========================================================================
**  Function : SYSDBG_ENABLE_PMIC_WDOG
** ==========================================================================
*/
/*!
* @brief
*   API used to enable PMIC wdog inside SDI.
*
*/


boolean sysdbg_enable_pmic_wdog(void);
#endif /* _SYSDBG_MAIN_H */
