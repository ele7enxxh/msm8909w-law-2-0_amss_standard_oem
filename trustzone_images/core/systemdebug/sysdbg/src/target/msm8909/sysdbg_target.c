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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/src/target/msm8909/sysdbg_target.c#1 $

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
#include "tzbsp_ext_os.h"

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

// "Lock" used by all 4 CPU cores
static uint32 global_lock = 0x0;
extern uint8 total_cpu_count;

#define APPS_ALIAS_SIZE		0x10000

#ifdef TZBSP_WITH_TBASE
extern uint32  sysdbg_reset_check_flat_mapped(void);
boolean sysdbg_dump_stats_init_flat_mapped(void);
boolean sysdbg_reset_next_core_flat_mapped(void);
void sysdbg_debug_ui_en_flat_mapped(void);
boolean sysdbg_exit_flat_mapped(void);
boolean sysdbg_enable_pmic_wdog_flat_mapped(void);

#endif
/*===========================================================================
** 	Function: GLOBAL LOCK SET/ GET Functions
** ==========================================================================
*/
/*!
* 
* @brief
*	The global lock is used to put current core in wait forever loop before 
*   bringing the next core out of reset. 
*/	
void set_global_lock(uint32 count)
{
	global_lock = count;
}

uint32 get_global_lock(void)
{
	return global_lock;
}


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
#ifdef TZBSP_WITH_TBASE
	uint32  pass_count;
#endif
	uint32 i = 0, current_cpu = tzbsp_cur_cpu_num(); 
	sysdbg_pass_sequence_type *sysdbg_seq_entry;

#ifdef TZBSP_WITH_TBASE
	// pass 0 is cold boot; pass 1 is reset boot
	if (tzbsp_sysdbg_flat_mapped())
		pass_count = sysdbg_reset_check_flat_mapped();
	else
		pass_count = sysdbg_reset_check();
#else
	uint32 pass_count = sysdbg_reset_check();//pass 0 is cold boot; pass 1 is reset boot
#endif
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
			for (i=0; (i < ARRAY_SIZE(sysdbg_seq_entry->fcn_info) && (pass)); i++){
				SYSDBG_FUNC_START_MARKER(i,current_cpu);
				CALL_IF_VALID_AND_CHECK(pass, sysdbg_seq_entry->fcn_info[i].cpu_affinity[current_cpu], sysdbg_seq_entry->fcn_info[i].sysdbg_fcn);
			}
			/* Check the return value before proceeding further */
		
		}while(FALSE);
		
		// We are done with this pass. We are good. So mark it
		//sysdbg_pass_ptr[pass_count].complete[current_cpu] = TRUE;

		// Log the exit
		//SYSDBG_LOG_EVENT(SYSDBG_PASS_EXIT, __current_pc(), __current_sp(), current_cpu);
	}
}


/*===========================================================================
**  Function :  SYSDBG_DUMP_STATS_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get PMIC debug register info
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

#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		sysdbg_dump_stats_init_flat_mapped();
		init = TRUE;
		return init;
	}
#endif
	if (!init)
	{
	
		HWIO_OUT(GCC_GPLL2_MODE, 0x0);
		HWIO_OUT(GCC_GPLL2_L_VAL, 0x29);
		//HWIO_OUT(GCC_GPLL2_M_VAL, 0x2);
		//HWIO_OUT(GCC_GPLL2_N_VAL, 0x3);
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
	
	    /* MSM Reset Reason	*/
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

#ifndef TZBSP_WITH_TBASE
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
#endif

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
#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		sysdbg_debug_ui_en_flat_mapped();
		return;
	}
#endif
	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55); 
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);   	   // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MASK
}

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

boolean SYSDBGGetCoreStatus(uint32 corenum)
{
	uint32 *reg = NULL;
	uint32 ret = 0;
    /* Check the input */
    if (corenum > total_cpu_count)
        return FALSE;
	
	/* Get the status */
	reg = (uint32 *)((HWIO_APCS_ALIAS0_APC_PWR_STATUS_ADDR + (APPS_ALIAS_SIZE * corenum )));
	ret = INP32(reg);
	ret = (ret & (HWIO_APCS_ALIAS0_APC_PWR_STATUS_GDHS_STS_BMSK))>> HWIO_APCS_ALIAS0_APC_PWR_STATUS_GDHS_STS_SHFT;
	return ((boolean)ret);
}

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
*  Cluster whose L2 status has to be detected
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
boolean SYSDBGGetL2SValidStatus()
{
    uint32 *reg = NULL;
	uint32 ret = 0;
    boolean l2_valid = FALSE;

   	/* Check that for valid L2 cache using clamp status and power supply on. It shouldn't be clamped and supply should be on. */
	reg  = (uint32 *)(HWIO_APCS_L2_PWR_STATUS_ADDR);
	ret = INP32(reg);
	l2_valid = (ret & (HWIO_APCS_L2_PWR_STATUS_L2_HS_STS_BMSK)) >> HWIO_APCS_L2_PWR_STATUS_L2_HS_STS_SHFT;
	return l2_valid;
}

/*===========================================================================
**  Function :  SYSDBGGETNXTACTCORE
** ==========================================================================
*/
/*!
* 
* @brief
* 	API to get the next active core.
* 
* @param[in] 
*  Core from which the search will start i.e. search all cores to core n to 
*  the max number of cores in cluster and return the first active one
*  
* @par Dependencies
*   Core whose number is provided as input is known to be alive. That will be 
*   the return value if no other active cores are found.
* 
* @retval
*   Boolean to indicate online status
* 
* @par Side Effects
*   None
* 
*/

uint32 SYSDBGGetNxtActCore(uint32 corenum)
{
    uint32 coreloop = corenum + 1 ;
	boolean core_alive = 0 ;

    if (corenum >= total_cpu_count)
        return FALSE;
	
	/* Find the first core which was alive*/
	for(; coreloop < total_cpu_count; coreloop++)
	{
		core_alive = SYSDBGGetCoreStatus(coreloop);

		/* If alive, done*/
		if (core_alive)
            break;
	}

	if (coreloop == total_cpu_count)
		return corenum ;
	else
		return coreloop;
		
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
static void (*func_ptr)(uint32,uint32);
boolean sysdbg_reset_next_core(void)
{
	volatile uint32 *reg;

#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		return sysdbg_reset_next_core_flat_mapped();
	}
#endif
	uint32 next_act_core = 0;
	
	//Using ENTRY point of SYSDBG in RPM CODERAM 
	func_ptr = (void (*)(uint32,uint32))(HWIO_IN(TCSR_RESET_DEBUG_SW_ENTRY));
	if(tzbsp_cur_cpu_num()==0)
	{
	    memcpy(func_ptr, ((void *)ddr_enter_self_refresh_and_reset), 0x100);
  	}
	// Program the start address to the reset vector of TZ
	HWIO_OUT(APCS_START_ADDR_SEC, (uint32)&Image$$TZBSP_CODE$$Base | 0x1);
	HWIO_OUT(APCS_CFG_SECURE, 0x1);

	// Find the next core to be reset based on the status of the headswitch for the start
    next_act_core = SYSDBGGetNxtActCore(tzbsp_cur_cpu_num());
	
	 /* Means this is the last active core */
    if (next_act_core == tzbsp_cur_cpu_num())
        return TRUE;
		
	if (next_act_core < total_cpu_count)
	{
		
		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core));
		*reg = 0x0000001D;

		reg = (uint32 *)(HWIO_APCS_ALIAS0_APC_PWR_GATE_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core));
		*reg = 0x10000001;

		busywait(2);
	
		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002001C;
	
		busywait(2);

		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002000C;

		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002008C;
			
		/* Increment our lock to indicate that one more core is going to be up */
		set_global_lock(next_act_core);
			
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

void ddr_enter_self_refresh_and_reset(uint32 cur_cpu, uint32 last_act_cpu)
{
    if(cur_cpu == last_act_cpu) //only reset if it is the last CPU
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
	uint32 cur_cpu = tzbsp_cur_cpu_num();
    uint32 last_act_cpu = get_global_lock();
#ifdef TZBSP_WITH_TBASE
		if (tzbsp_sysdbg_flat_mapped())
		{
			sysdbg_exit_flat_mapped();
			return TRUE;
		}
#endif

    if (cur_cpu == last_act_cpu)
	{
		PM_DisableWatchdog();
		HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x0);
		HWIO_OUT(TCSR_RESET_DEBUG_SW_ENTRY, 0x0);
		HWIO_OUT(GCC_RESET_STATUS, 0x2);
		/* Update system debug end marker */
	    (*(uint32*)SYSTEM_DEBUG_MARKER) = 0xDEADD00D;
	}
	
	func_ptr(cur_cpu, last_act_cpu); //never returns
	while(TRUE);
}

boolean sysdbg_enable_pmic_wdog(void)
{

#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		return sysdbg_enable_pmic_wdog_flat_mapped();
	}
#endif

	/*Before we enable the PMIC WDOG, clear DEBUG_EN bits in the GCC_RESET_DEBUG in order  
	    not  to block the next resin_n by the reset controller, so that device will go to download mode*/
	MSM_CLEAR_BIT(TCSR_RESET_DEBUG_SW_ENTRY, 0x0);
	HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x0);
	MSM_CLEAR_BIT(GCC_RESET_STATUS,0x0);
	MSM_SET_BIT(GCC_RESET_STATUS, 0x1);
	/*Enabled for 8909*/
	PM_EnableWatchdog();
	return TRUE;
}

#ifdef TZBSP_WITH_TBASE
/* Use the C pre-processor to re-define the below macros based on
 * physical addresses. Note that the new defines are valid until
 * the end of the current translation unit */
#undef  GCC_CLK_CTL_REG_REG_BASE
#define GCC_CLK_CTL_REG_REG_BASE	   GCC_CLK_CTL_REG_REG_BASE_PHYS
#undef  QDSS_WRAPPER_TOP_BASE
#define QDSS_WRAPPER_TOP_BASE   	   QDSS_WRAPPER_TOP_BASE_PHYS
#undef  MPM2_MPM_BASE
#define MPM2_MPM_BASE   			   MPM2_MPM_BASE_PHYS
#undef  CORE_TOP_CSR_BASE
#define CORE_TOP_CSR_BASE   		   CORE_TOP_CSR_BASE_PHYS
#undef  BIMC_BASE
#define BIMC_BASE   				   BIMC_BASE_PHYS
#undef  CORE_TOP_CSR_BASE
#define CORE_TOP_CSR_BASE   		   CORE_TOP_CSR_BASE_PHYS

boolean sysdbg_dump_stats_init_flat_mapped(void)
{
	static boolean init = FALSE;
	uint32 reset_reason[5] = {0};

	uint32 temp = 0;

	if (!init)
	{
	
		HWIO_OUT(GCC_GPLL2_MODE, 0x0);
		HWIO_OUT(GCC_GPLL2_L_VAL, 0x29);
		//HWIO_OUT(GCC_GPLL2_M_VAL, 0x2);
		//HWIO_OUT(GCC_GPLL2_N_VAL, 0x3);
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
	
	    /* MSM Reset Reason	*/
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
**  Function :  SYSDBG_DEBUG_UI_EN_FLAT_MAPPED
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
void sysdbg_debug_ui_en_flat_mapped(void)
{
	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);      // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MASK
}

/*===========================================================================
**  Function :  DDR_ENTER_SELF_REFRESH_AND_RESET_FLAT_MAPPED
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
void ddr_enter_self_refresh_and_reset_flat_mapped(uint32 cur_cpu, uint32 last_act_cpu)
{
    if(cur_cpu == last_act_cpu) //only reset if it is the last CPU
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
static void (*func_ptr)(uint32,uint32);
boolean sysdbg_reset_next_core_flat_mapped(void)
{
	volatile uint32 *reg;

	uint32 next_act_core = 0;
	
	//Using ENTRY point of SYSDBG in RPM CODERAM 
	func_ptr = (void (*)(uint32,uint32))(HWIO_IN(TCSR_RESET_DEBUG_SW_ENTRY));
	if(tzbsp_cur_cpu_num()==0)
	{
	    memcpy(func_ptr, ((void *)ddr_enter_self_refresh_and_reset_flat_mapped), 0x100);
  	}
	// Program the start address to the reset vector of TZ
	HWIO_OUT(APCS_START_ADDR_SEC, (uint32)&Image$$TZBSP_CODE$$Base | 0x1);
	HWIO_OUT(APCS_CFG_SECURE, 0x1);

	// Find the next core to be reset based on the status of the headswitch for the start
    next_act_core = SYSDBGGetNxtActCore(tzbsp_cur_cpu_num());
	
	 /* Means this is the last active core */
    if (next_act_core == tzbsp_cur_cpu_num())
        return TRUE;
		
	if (next_act_core < total_cpu_count)
	{
		
		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core));
		*reg = 0x0000001D;

		reg = (uint32 *)(HWIO_APCS_ALIAS0_APC_PWR_GATE_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core));
		*reg = 0x10000001;

		busywait(2);
	
		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002001C;
	
		busywait(2);

		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002000C;

		reg = (uint32 *)(HWIO_APCS_ALIAS0_CPU_PWR_CTL_ADDR + (APPS_ALIAS_SIZE * next_act_core)); 
		*reg = 0x0002008C;
			
		/* Increment our lock to indicate that one more core is going to be up */
		set_global_lock(next_act_core);
			
	}
    return TRUE;
}

/*===========================================================================
**  Function :  SYSDBG_EXIT_FLAT_MAPPED
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
*   Nones
* 
* @par Side Effects
*   None
* 
*/
boolean sysdbg_exit_flat_mapped(void)
{
	uint32 cur_cpu = tzbsp_cur_cpu_num();
    uint32 last_act_cpu = get_global_lock();
    if (cur_cpu == last_act_cpu)
	{
	    PM_DisableWatchdog();
		HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x0);
		HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x0);
		HWIO_OUT(TCSR_RESET_DEBUG_SW_ENTRY, 0x0);
		HWIO_OUT(GCC_RESET_STATUS, 0x2);
		/* Update system debug end marker */
	    (*(uint32*)SYSTEM_DEBUG_MARKER) = 0xDEADD00D;
	}
	
	func_ptr(cur_cpu, last_act_cpu); //never returns
	while(TRUE);
}

boolean sysdbg_enable_pmic_wdog_flat_mapped(void)
{

	/*Before we enable the PMIC WDOG, clear DEBUG_EN bits in the GCC_RESET_DEBUG in order  
	    not  to block the next resin_n by the reset controller, so that device will go to download mode*/
	MSM_CLEAR_BIT(TCSR_RESET_DEBUG_SW_ENTRY, 0x0);
	HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x0);
	MSM_CLEAR_BIT(GCC_RESET_STATUS,0x0);
	MSM_SET_BIT(GCC_RESET_STATUS, 0x1);
	/*Enabled for 8909*/
	PM_EnableWatchdog();
	return TRUE;
}

#endif
