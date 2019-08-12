/*=============================================================================
                        SYSDBG Target Specific API Header

GENERAL DESCRIPTION     
		  This module defines objects for the main control loop of the debug image.
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/src/target/msm8909/sysdbg_target.h#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
02/01/13	AJCheriyan	Added api for HW Revision ID check
02/22/12	AJCheriyan	First cut for Aragorn. First ever SYSDBG

===========================================================================*/


#ifndef _SYSDBG_TARGET_H
#define _SYSDBG_TARGET_H

#include "sysdbg_def.h"
#include "sysdbg_mem_dump.h"

/*===========================================================================
**  Function : SYSDBG_EXECUTE_PASS
** ==========================================================================
*/
/*!
* 
* @brief
*	This function is the workhorse for the image. It iteratively scans through 
*	the pass structure and executes every function in there. The functions are
*	classified as init, work and exit functions respectively. Init and exit functions
*	don't return values while work functions return a success / fail value. Every
*	function must be capable of being run on all cores. If you need to distinguish
*	between a particular core, handle it within the function.
*
* @param[in] 
*	The pointer to the SYSDBG pass data structure, pass_count j
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   Should be capable of handling multi-core situations as needed.
*
*
*/

void sysdbg_execute_pass(sysdbg_pass_data_type *sysdbg_pass_ptr);

/*===========================================================================
**  Function :  SYSDBG_GET_SHARED_IMEM
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get the location of shared imem
* 
* @param[in] 
*  None
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

uint8 *sysdbg_get_shared_imem(void);

/*===========================================================================
*  Function :  SYSDBG_DUMP_STATS_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get chip reset reason
* 
* @param[in] 
*  None
*  
* @par Dependencies
*   None
* 
* @retval
*   Boolean
* 
* @par Side Effects
*   None
* 
*/

boolean sysdbg_dump_stats_init(void);

/*===========================================================================
**  Function :  SYSDBGGETCORESTATUS
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get the status of a particular apps core 
* 
* @param[in] 
*  Core whose status has to be detected
*  
* @par Dependencies
*   None
* 
* @retval
*   Boolean to indicate online status
* 
* @par Side Effects
*   None
* 
*/

boolean SYSDBGGetCoreStatus(uint32);

/*===========================================================================
**  Function :  SYSDBGGETL2VALIDSTATUS
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get the valid status of L2 i.e. cache data is
* 	valid or not
* 
* @param[in] 
*  None
*  
* @par Dependencies
*   None
* 
* @retval
*   Boolean to indicate online status
* 
* @par Side Effects
*   None
* 
*/

boolean SYSDBGGetL2SValidStatus(void);

/*===========================================================================
**  Function :  SYSDBG_RESET_NEXT_CORE
** ==========================================================================
*/
/*!
* 
* @brief
* The function that pulls the next CPU core out of reset. This reset sequence
* is for CORTEX-A53
* 
* @param[in] 
*  None. Current CPU number is obtained from TZ API
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
boolean sysdbg_reset_next_core(void);

/*===========================================================================
**  Function :  DDR_ENTER_SELF_REFRESH_AND_RESET
** ==========================================================================
*/
/*!
* 
* @brief
* The function gets copied to a code region either in IMEM or RPM CODERAM for
* execution and is not called directly by sysdbg
* 
* @param[in] 
*  None
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

void ddr_enter_self_refresh_and_reset(uint32 cur_cpu, uint32 last_act_cpu);

/*===========================================================================
**  Function :  SYSDBG_EXIT
** ==========================================================================
*/
/*!
* 
* @brief
* clean up function for sysdbg
* copies ddr_enter_self_refresh_reset into a non-ddr memory region,
* then finishes and reset
* 
* @param[in] 
*  None
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

boolean sysdbg_exit(void);

#endif /* _SYSDBG_TARGET_H */

