/*=============================================================================

                              SBL1 DDR Debug Main Control

GENERAL DESCRIPTION
  This file contains the main control for  DDR Debug SBL1 execution. Since this implements
  the main control functionality, it may be dependent on all modules of the
  system.

Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8956/sbl1/sbl1_ddr_debug_mc.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $ 
$Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/14/14    yps    proting code to 8916 platform.
09/13/13     sl      Removed PBL dependencies.
08/29/13     sl      Updated scl section names.
07/17/13     sr      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_target.h"
#include "boot_util.h"
#include "boot_config_data.h"
#include "boot_cache_mmu.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_bus_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_bam_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* Internal heap memory, inside the SBL1 image*/
#define INTERNAL_HEAP_SIZE  4096
#pragma arm section zidata = "BOOT_INTERNAL_HEAP"
static byte internal_heap[INTERNAL_HEAP_SIZE];
#pragma arm section zidata

extern uint8 config_data_table[CONFIG_DATA_TABLE_MAX_SIZE];

extern uint32  Load$$DDR_DEBUG_WDOG$$Base;
extern uint32  Image$$DDR_DEBUG_WDOG$$Base;
extern uint32  Image$$DDR_DEBUG_WDOG$$Length;

extern uint32  Load$$DDR_DEBUG_CODE$$Base;
extern uint32  Image$$DDR_DEBUG_CODE$$Base;
extern uint32  Image$$DDR_DEBUG_CODE$$Length;

extern uint32  Load$$DDR_DEBUG_RO$$Base;
extern uint32  Image$$DDR_DEBUG_RO$$Base;
extern uint32  Image$$DDR_DEBUG_RO$$Length;

extern uint32  Load$$DDR_DEBUG_RW$$Base;
extern uint32  Image$$DDR_DEBUG_RW$$Base;
extern uint32  Image$$DDR_DEBUG_RW$$Length;


extern uint32  Load$$DDR_DEBUG_RW_COPY$$Base;
extern uint32  Image$$DDR_DEBUG_RW_COPY$$Base;
extern uint32  Image$$DDR_DEBUG_RW_COPY$$Length;

extern uint32  Load$$DDR_DEBUG_ZI$$Base;
extern uint32  Image$$DDR_DEBUG_ZI$$Base;
extern uint32  Image$$DDR_DEBUG_ZI$$ZI$$Length;


/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**	Function :	sbl1_wdogpath_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
*	This function does the basic/needed initializations for both ( coldboot / warmboot ) paths .
*		- it sets up the rw base , by coping the backup from RW_COPY base 
*		- it also setsup the ddr for ddr_debug_mode 
*		- Initializes ram
*		- And so on...
* 
* @param  None
*  
* @par Dependencies
*	None
* 
* @retval
*	None
* 
* @par Side Effects
*	This function never returns.
* 
*/
void sbl1_wdogpath_main_ctl(void)
{
  const void *cdt_ptr;
  uint32 cdt_size;
  ddr_info info;
  DALResult dal_result; 

  /* Zero out the zero-init data segment. */
  qmemset( (void *)&Image$$DDR_DEBUG_ZI$$Base,
		   0,
		   (uint32)&Image$$DDR_DEBUG_ZI$$ZI$$Length );
#ifdef SBL1_DISABLE_D_CACHE
	mmu_flush_cache_and_disable();
	mmu_enable_instruction_cache();
#endif

  /*Configure Domain access control register */
  mmu_set_dacr(DACR_ALL_DOMAIN_CLIENTS);

  /* Enable qdss workaround*/
  boot_clock_debug_init();

  /* Initialize clocks */
  BL_VERIFY((boot_clock_init() == TRUE), BL_ERR_SBL);

  boot_DALSYS_HeapInit(internal_heap, INTERNAL_HEAP_SIZE, FALSE);

  boot_DALSYS_InitMod(NULL);
  /*Initialize busywait module*/
  dal_result = boot_busywait_init();
  BL_VERIFY(dal_result == DAL_SUCCESS,BL_ERR_SBL);

  /* Initialize SPMI bus */
  BL_VERIFY((boot_SpmiCfg_Init(TRUE) == SPMI_CFG_SUCCESS), BL_ERR_SBL);
  /* Initialize busywait module */
  BL_VERIFY((boot_busywait_init() == DAL_SUCCESS), BL_ERR_SBL);

  /* Initialize PMIC */
  BL_VERIFY((boot_pm_device_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);

  /* Get DDR CDT parameters */
  cdt_ptr = boot_get_config_data_block(config_data_table,
									   CONFIG_DATA_BLOCK_INDEX_V1_DDR,
									   &cdt_size);

  /* Make sure CDT parameters are valid */
  BL_VERIFY( (cdt_size <= CONFIG_DATA_TABLE_MAX_SIZE) &&
			 (cdt_ptr != NULL),  BL_ERR_SBL );

  /* Copy CDT DDR parameters to shared IMEM */
  qmemcpy( (void*)SHARED_IMEM_BOOT_CDT_BASE, cdt_ptr, cdt_size );

  /* Pass CDT parameters to DDR drivers */
  BL_VERIFY((boot_ddr_set_params((void*)SHARED_IMEM_BOOT_CDT_BASE, cdt_size) == TRUE), BL_ERR_SBL);
  


  /* Initialize DDR clock according to DDR type */
  BL_VERIFY((boot_pre_ddr_clock_init(boot_ddr_get_info().ddr_type) == TRUE), BL_ERR_SBL);

  /* Initialize buses */
  BL_VERIFY((boot_ICB_Config_Init("/dev/icbcfg/boot") == ICBCFG_SUCCESS), BL_ERR_SBL);

  /* Initialize DDR */
  boot_ddr_initialize_device(boot_clock_get_ddr_speed());

  info = boot_ddr_get_info();

  /* Initialize bus segments according to DDR size info */
  boot_ICB_Segments_Init("/dev/icbcfg/boot", &info);

  /* Go to DDR Debug Mode */
  boot_ddr_debug();

} /* sbl1_wdogpath_main_ctl() */

/*===========================================================================
**  Function :  sbl1_ddr_debug_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
*   This function gets called from .s (startup ) , it disable the cache as the sbl1
*   code now runs from CODERAM instead of L2Cache .SBL1 code needs to be run from CODERAM
*   as L2 cache gets flushed after watch dog reset .
* 
* @param  None
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void sbl1_ddr_debug_main_ctl()
{
  cache_mmu_disable();
  sbl1_wdogpath_main_ctl();
}
