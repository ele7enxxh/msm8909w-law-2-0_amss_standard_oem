/*=============================================================================
                        sysdbg Main Control Function

GENERAL DESCRIPTION     
		  This module defines objects for the main control loop of the debug image.
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/src/target/msm8916/sysdbg_target.c#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
07/02/13    AJCheriyan  Updated for Gandalf
01/28/13	AJCheriyan	Added BOOT_PARTITION_SELECT write for v2
01/15/13	AJCheriyan	Moved reset apps core and debug feature enablement here
02/22/12	AJCheriyan	First cut for Aragorn. First ever sysdbg

===========================================================================*/
#include "comdef.h"
#include "sysdbg_mem_dump.h"
#include "sysdbg_def.h"
#include "sysdbg_pmic.h"
#include "sysdbg_target.h"
#include "tzbsp_sys.h"
#include "sysdbg_hwio.h"
#include "busywait.h"
#include <string.h>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define CALL_IF_VALID(execute, fcnptr)	if (execute) \
											if (fcnptr) \
												{ /*SYSDBG_LOG_EVENT(SYSDBG_ENTER_FCN, fcnptr, get_current_cpu(), 0);*/ fcnptr();  /*SYSDBG_LOG_EVENT(SYSDBG_EXIT_FCN, fcnptr, get_current_cpu(), 0);*/} \
											else { break; }	\
										else				\
											{ continue; }	\

#define CALL_IF_VALID_AND_CHECK(rval, execute, fcnptr)	if (execute) \
															if (fcnptr) \
																{ /*SYSDBG_LOG_EVENT(SYSDBG_ENTER_FCN, fcnptr, get_current_cpu(), 0);*/ rval = fcnptr(); /*SYSDBG_LOG_EVENT(SYSDBG_EXIT_FCN, fcnptr, get_current_cpu(), 0);*/ } \
															else					    \
																{ break; }				\
														else							\
															{ continue; }


extern uint32 Image$$TZBSP_CODE$$Base;

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

void sysdbg_execute_pass(sysdbg_pass_data_type *sysdbg_pass_ptr)
{
	boolean pass = TRUE;
	uint32 i = 0, current_cpu = tzbsp_cur_cpu_num(); 
	sysdbg_pass_sequence_type *sysdbg_seq_entry;
	uint32 pass_count = sysdbg_reset_check();//pass 0 is cold boot; pass 1 is reset boot
	if (sysdbg_pass_ptr)
	{
		// Log the entry
		//SYSDBG_LOG_EVENT(SYSDBG_PASS_ENTER, __current_pc(), __current_sp(), current_cpu);

		// We have our entry from the table. Now assign the pointer
		sysdbg_seq_entry = &sysdbg_pass_ptr[pass_count].pass_sequence;
		
		do 
		{
			/* Now call all the functions that actually get something
	 		 done if 1) it is intended for current cpu 2) valid */
			for (i=0; (i < ARRAY_SIZE(sysdbg_seq_entry->fcn_info) && (pass)); i++)
	
				CALL_IF_VALID_AND_CHECK(pass, sysdbg_seq_entry->fcn_info[i].cpu_affinity[current_cpu], sysdbg_seq_entry->fcn_info[i].sysdbg_fcn);
			
			/* Check the return value before proceeding further */
		
		}while(FALSE);
		
		// We are done with this pass. We are good. So mark it
		//sysdbg_pass_ptr[pass_count].complete[current_cpu] = TRUE;

		// Log the exit
		//SYSDBG_LOG_EVENT(SYSDBG_PASS_EXIT, __current_pc(), __current_sp(), current_cpu);
	}
}


/*===========================================================================
**  Function :  SYSDBG_VALID_RESET
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get chip reset reason
* 	Watchdog reset - 0x3 - supported
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

boolean sysdbg_dump_stats_init(void)
{
	static boolean init = FALSE;
	uint32 reset_reason[5] = {0};

	uint32 temp = 0;

	if (!init)
	{
	
	HWIO_OUT(GCC_GPLL2_MODE, 0x0);
	HWIO_OUT(GCC_GPLL2_L_VAL, 0x29);
	HWIO_OUT(GCC_GPLL2_M_VAL, 0x2);
	HWIO_OUT(GCC_GPLL2_N_VAL, 0x3);
	HWIO_OUT(GCC_GPLL2_USER_CTL, 0x01100000);
	HWIO_OUT(GCC_GPLL2_MODE, 0x2);
	HWIO_OUT(GCC_GPLL2_MODE, 0x6);
	HWIO_OUT(GCC_GPLL2_MODE, 0x200006);
	HWIO_OUT(GCC_GPLL2_MODE, 0x6);
	HWIO_OUT(GCC_GPLL2_MODE, 0x4006);
	HWIO_OUT(GCC_GPLL2_MODE, 0x4106);
	HWIO_OUT(GCC_GPLL2_MODE, 0x104106);
	HWIO_OUT(GCC_RPM_GPLL_ENA_VOTE, 0x4);

	//PLL should be Locked and Active
	HWIO_OUT(GCC_GPLL2_USER_CTL, 0x1); //ENABLE MAIN_OUT
	
	HWIO_OUTF(GCC_QDSS_AT_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_TRACECLKIN_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_SYS_NOC_QDSS_STM_AXI_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_ETR_USB_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_STM_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_CFG_AHB_CBCR,CLK_ENABLE, 0x1);

	HWIO_OUTM(GCC_APSS_AHB_CBCR, 0x1, 0x1);
	//HWIO_OUT(GCC_APSS_AXI_CBCR, 0x1);
	HWIO_OUTF(GCC_APCS_TZ_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 0x1);
	// HWIO_OUTM(GCC_CFG_NOC_AHB_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_PCNOC_AHB_CBCR, 0x1, 0x1);
	// HWIO_OUTM(GCC_CFG_NOC_DDR_CFG_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_PCNOC_DDR_CFG_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_DDR_DIM_CFG_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_DDR_DIM_SLEEP_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_GCC_XO_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_GCC_XO_DIV4_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_MPM_AHB_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_DAP_AHB_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_CFG_AHB_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_AT_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_STM_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_DAP_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TSCTR_DIV2_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TSCTR_DIV3_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TSCTR_DIV4_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TSCTR_DIV8_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TSCTR_DIV16_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_TRACECLKIN_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_QDSS_ETR_USB_CBCR, 0x1, 0x1);
	// HWIO_OUTM(GCC_PERIPH_NOC_AT_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_PCNOC_AT_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_SYS_NOC_AT_CBCR, 0x1, 0x1);
	HWIO_OUTM(GCC_SYS_NOC_QDSS_STM_AXI_CBCR, 0x1, 0x1);


	HWIO_OUTF(GCC_QDSS_DAP_CBCR, CLK_ENABLE, 0x1);
	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);      // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MAS
	
		// Enable MPM timer to start the parent timer
		HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, EN, 0x1);

		// We are going to count at XO speed. Configure
		// for XO = 19.2 MHz 
		HWIO_OUTF(APCS_QTMR_AC_CNTFRQ, CNTFRQ, 19200000);
		busywait_init();
		// We need to know the PMIC reset reason as well
		// To be able to talk to the PMIC, SPMI init needs to 
		// happen and data needs to be saved. Hence, doing it here
		//SPMIFW_Init();
	
		/* MSM Reset Reasoninit	*/
		reset_reason[0] = HWIO_IN(GCC_RESET_STATUS);
		// Other reset reasons
		// PON Reason 1 - 0x08
		PM_ReadPerphData(PONA, 0x08, &temp);
		reset_reason[1] |= temp << 8;
		//Warm Reset Reason 1 - 0x0A
		PM_ReadPerphData(PONA, 0x0A, &temp);
		//Warm Reset Reason 2 - 0x0B
		PM_ReadPerphData(PONA, 0x0B, &reset_reason[2]);
		reset_reason[2] |= temp << 8;
		// POFF Reason 1 - 0x0C
		PM_ReadPerphData(PONA, 0xC, &temp);
		// POFF Reason 2 - 0x0D
		PM_ReadPerphData(PONA, 0xD, &reset_reason[3]);
		reset_reason[3] |= temp << 8;
		// PON PBL STATUS
		PM_ReadPerphData(PONA, 0x07, &temp);
		reset_reason[4] |= temp;
		// PS_HOLD_RESET_CTL
		PM_ReadPerphData(PONA, 0x5A, &temp);
		reset_reason[4] |= temp << 8;
		// PS_HOLD_RESET_CTL2
		PM_ReadPerphData(PONA, 0x5B, &temp);
		reset_reason[4] |= temp << 16;
		
		// Now populate our dump structure
		sysdbgDumpStats(reset_reason);	
	
		init = TRUE;
	}

	return init;
}



/*===========================================================================
**  Function :  sysdbg_GET_SHARED_IMEM
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

uint8 *sysdbg_get_shared_imem(void)
{
	return ((uint8 *)(SHARED_IMEM_BASE));
}

/*===========================================================================
**  Function :  SYSDBG_DEBUG_UI_EN
** ==========================================================================
*/
/*!
* 
* @brief
* Initializes DEBUG UI hardware block
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
void sysdbg_debug_ui_en(void)
{
	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);      // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MASK
}


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
static void (*func_ptr)(uint32);
boolean sysdbg_reset_next_core(void)
{
	#define APPS_ALIAS_SIZE		0x10000
	volatile uint32 *reg;

	// The next core to be reset is the current_cpu + 1
	uint32 next_core = tzbsp_cur_cpu_num() + 1;
	
	//Using ENTRY point of SYSDBG in RPM CODERAM 
	func_ptr = (void (*)(uint32))(HWIO_IN(TCSR_RESET_DEBUG_SW_ENTRY));
	if(tzbsp_cur_cpu_num()==0)
	{
	    memcpy(func_ptr, ((void *)ddr_enter_self_refresh_and_reset), 0x100);
  	}
	// Program the start address to the reset vector of TZ
	HWIO_OUT(APCS_BOOT_START_ADDR_SEC, (uint32)&Image$$TZBSP_CODE$$Base | 0x1);
	HWIO_OUT(APCS_CFG_SECURE, 0x1);

	if (next_core < TARGET_APPS_CPUS)
	{
		
			reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_core));
			*reg = 0x0000001D;

			reg = (uint32 *)(HWIO_APCS_ALIAS0_APC_PWR_GATE_CTL_ADDR + (APPS_ALIAS_SIZE * next_core));
			*reg = 0x10000001;

			busywait(2);
	
			reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_core)); 
			*reg = 0x0002001C;
	
			busywait(2);

			reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_core)); 
			*reg = 0x0002000C;

			reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_core)); 
			*reg = 0x0002008C;
			
	}
     return TRUE;
}


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
//#pragma arm section code="SYSDBG_CODE"
#pragma push 
#pragma arm 

void ddr_enter_self_refresh_and_reset(uint32 cpu_num)
{
	if(cpu_num == TARGET_APPS_CPUS-1) //only reset if it is the last CPU
	{
		HWIO_OUTF(BIMC_S_DDR0_SHKE_DRAM_MANUAL_0, ENTER_SELF_REFRESH_ASAP, 0x1); //Put DDR into self-refresh
		while(HWIO_INF(BIMC_S_DDR0_SHKE_DRAM_MANUAL_0, ENTER_SELF_REFRESH_ASAP)); //loop until bit is cleared to indicate cmd is executed
	
		/* Trigger for pshold reset */
		HWIO_OUTF(MPM2_MPM_PS_HOLD, PSHOLD, 0);
  	}
	while(TRUE);
}

#pragma pop
//#pragma arm section
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

boolean sysdbg_exit(void)

{
	uint32 cur_cpu= tzbsp_cur_cpu_num();
	
	if (cur_cpu==(TARGET_APPS_CPUS-1))
	{
		HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x0);
		HWIO_OUT(TCSR_RESET_DEBUG_SW_ENTRY, 0x0);
		HWIO_OUT(GCC_RESET_STATUS, 0x2);
		/* Update system debug end marker */
	    (*(uint32*)SYSTEM_DEBUG_MARKER) = 0xDEADD00D;
	}
	
	func_ptr(cur_cpu); //never returns
	while(TRUE);
}




