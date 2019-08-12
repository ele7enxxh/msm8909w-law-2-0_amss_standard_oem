/*=============================================================================
                        sysdbg_ Memory Dump objects 

GENERAL DESCRIPTION     
  This module defines objects for storing the CPU context saved by the debug image.
  
	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/src/target/msm8916/sysdbg_mem_dump.c#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
10/02/13    AJCheriyan  Updates to CPU Dump for new format
09/11/13    AJCheriyan  More Robust entry to flush cache routine
09/03/13    AJCheriyan  Restore PBL / sysdbg Boot backed registers
07/16/13    AJCheriyan  Some API updates
07/08/13    AJCheriyan  Retain lower 16 bits of reset reason
05/16/13    AJCheriyan  Unlock ETM registers using OSLAR
06/26/12	AJCheriyan	Added L1 and L2 Save Code
02/26/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#include "sysdbg_mem_dump.h"
#include "sysdbg_cache.h"
#include "sysdbg_asm.h"
#include "sysdbg_target.h"
#include "tzbsp_sys.h"
#include "stddef.h"
#include <stdint.h>
#include "msmhwio.h"
#include "comdef.h"
#include "tzbsp_secboot.h"
#include "stringl/stringl.h"
//#include "tzbsp_context_util.h"
#include "sysdbg_hwio.h"

//name used to populate dump_data_type
const char dump_sysdbg_name[] = "sysdbg";

// Main structure containing the register dump
sysdbg_cpu_ctxt_regs_type sysdbg_cpu_regs;

// Main struct containing all the cache dump pointer entries
sysdbgCacheDumpType sysdbgCacheDump;
//static dump_data_type sysdbgCacheDump;

// Main struct containing the information about the L0/L1 cache
//static sysdbgCacheLvlDumpType *sysdbgL1Dump = NULL;

// Main struct containing the information about the L2 cache
//static sysdbgL2DumpType ** const sysdbgL2Dump = (sysdbgL2DumpType **)sysdbg_CACHEDUMP_PTR;
//static sysdbgL2DumpType *sysdbgL2Dump = NULL;

// Main struct containing the information for the CPU register dump
//static dump_data_type sysdbgCPUDump;
 sysdbgCPUDumpType sysdbgCPUDump;

/* Main struct containing the information for the ETB dump */
//static sysdbgETBDumpType *sysdbgETBDump = NULL;
static dump_data_type *sysdbgETBDump = NULL;

/* Main struct containing the information for ETM dump */
static sysdbgETMDumpType sysdbgETMDump;

/* Main struct containing the information for TMC dump */
static sysdbgTMCDumpType sysdbgTMCDump;

/* Main struct that contains the sysdbg Stats dump */
static sysdbgStatsDumpType *sysdbgStatsDump;

// Main global for the CPU count
// Warning : This returns the total no of CPUs that have booted up. 
//static uint32 current_cpu = INVALID_CPU;

/* Main global for cluster count */
//static uint32 current_cluster = INVALID_CLUSTER;

// "Lock" used by all 4 CPU cores
//static uint64 global_lock;

// Pointer to the cpu regs dump structure
sysdbg_cpu_ctxt_regs_type *sysdbg_cpu_ctxt_regs_addr;

/* Overlay structure \c sysdbg_cpu_ctx_t starting from monitor's label
 * \c Workspace_N_Begin to access monitor's saved non-secure context. */
extern sysdbg_cpu_ctx_t Sysdbg_Workspace_N_Begin;

extern boolean tzbsp_allow_memory_dump(void);

/*===========================================================================
** 	Function: sysdbg_FlushCache
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper function that checks whether cache needs to be flushed or not 
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
*	Must be called only after global struct is initialized
*
*/
boolean sysdbgFlushCache(void)
{
	uint32 l1_valid = 0, l2_valid = 0;

	uint32 cpu=tzbsp_cur_cpu_num();
	if (sysdbgStatsDump) 
		if (sysdbgStatsDump->magic == STATS_MAGIC_NUMBER)
		{
			l1_valid = (sysdbgStatsDump->l1_valid & (1 << cpu)) >> cpu;
			l2_valid = sysdbgStatsDump->l2_valid;
			if ((l1_valid) || (l2_valid))
	 			sysdbg_dcache_flush(l1_valid, l2_valid);
		}

	return TRUE;
}

/*===========================================================================
** 	Function: sysdbgDumpStats
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that contains stats populated for and by sysdbg
* 	Includes 
* 		- State of caches populated by TZ for sysdbg
* 		- Reset reason as seen by sysdbg. Populated by sysdbg*
*
* @param[in] 
*	Pointer to data that contains stats populated by sysdbg
*  
* @par Dependencies
*	None
* 
* @retval
*   Success / Fail
* 
* @par Side Effects
*	Must be called only after global struct is initialized
*
*/
boolean sysdbgDumpStats(uint32 *data)
{
	sysdbgStatsDump = (sysdbgStatsDumpType *)((uint8 *)sysdbg_get_shared_imem() + 0x758); //set the table pointer again as it's not retained accross reset
	// Copy the data over
	const uint32 size = sizeof(sysdbgStatsDumpType) - offsetof(sysdbgStatsDumpType, msm_reset_reason);
	memscpy(&(sysdbgStatsDump->msm_reset_reason), size, data, size);
	
	return TRUE;
}	


/*===========================================================================
** 	Function: sysdbgMemoryReset
** ==========================================================================
*/
/*!
PU
* @brief
* 	Function that resets any memory locations that HLOS / TZ has to initialize.
*
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
*	Check for the "reset" value in the second pass of the debug image and then
*	proceed
*
*/
boolean sysdbgMemoryReset(void)
{
	// Locate the dump table pointer that HLOS will use to give us memory
	uint32 *dump_table_ptr = (uint32 *)((uint8 *)sysdbg_get_shared_imem() + 0x10);

	// Reset the value to our magic number
	*dump_table_ptr = SYSDBG_MAGIC_NUMBER;

	// Init our stats dump and set magic numbers
	sysdbgStatsDump = (sysdbgStatsDumpType *)((uint8 *)sysdbg_get_shared_imem() + 0x758);
	// Retain the lower 16 bits to contain the value from the previous reset
	// If the magic number is set
//	sysdbgStatsDump->msm_reset_reason = (sysdbg_MAGIC_NUMBER & 0xFFFF0000) | (sysdbgStatsDump->msm_reset_reason & 0xFFFF);



	return TRUE;
}	

/*===========================================================================
** 	Function: sysdbgDumpTrace
** ==========================================================================
*/
/*!
* 
* @brief
*	Function that wraps all the trace related dumps. ETM dump is treated separately
*	and needs to happen on each CPU*
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
*/
boolean sysdbgDumpTrace(void)
{
	/* Dump TMC for ETB */
	sysdbgDumpTMC(TMC_ETB_TYPE);
	/* Dump TMC for ETR */
	sysdbgDumpTMC(TMC_ETR_TYPE);
	/* Dump ETB */
	sysdbgDumpETB();
	return TRUE;

}


/*===========================================================================
** 	Function: check_trace_enabled
** ==========================================================================
*/
/*!
* 
* @brief
*	Helper function that checks if a particular sink is enabled
*/
static boolean check_trace_enabled(tmc_type tmc)
{
	switch (tmc)
	{
		case TMC_ETB_TYPE:
		/* Following are the values for mode
		 * 0x0 - Circular buffer
		 * 0x1 - SW Fifo
		 * 0x2 - HW Fifo
		 * 0x3 - Reserved
		 * If enabled, it should be configured as a buffer */
			if (HWIO_INF(QDSS_ETFETB_CTL, TRACECAPTEN) && (!HWIO_INF(QDSS_ETFETB_MODE, MODE)))
				return TRUE;
			else
				return FALSE;
			
		
		case TMC_ETR_TYPE:
			if (HWIO_INF(QDSS_ETR_CTL, TRACECAPTEN) && (!HWIO_INF(QDSS_ETR_MODE, MODE)))
				return TRUE;
			else
				return FALSE;
			
		default:
			return FALSE;
	}
}



/*===========================================================================
** 	Function: sysdbgMemoryDumpInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that initalizes the dump structure for the various dump types
*
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
*/
boolean sysdbgMemoryDumpInit(void)
{
		uint32 i, j, cpu = tzbsp_cur_cpu_num();

		/* Check the cookie in OCIMEM that HLOS has setup for us */
		// Add the additional checks here including the one for the magic cookie
		dump_table_type ** DumpTable = (dump_table_type **)((uint8 *)sysdbg_get_shared_imem() + 0x10);

		/* KW error fix */	
        if (cpu > (TARGET_APPS_CPUS-1))
		return FALSE;
		
		// Check if HLOS has populated the table after we "reset" the value
		if (*DumpTable != (dump_table_type *)SYSDBG_MAGIC_NUMBER)
		{
			for (i = 0; i < (*DumpTable)->num_entries; i++)
			{
				dump_entry_type top_entry = (**DumpTable).entries[i];

				/* Find the table populated for APPS*/
				if ((top_entry.type == MSM_DUMP_TYPE_TABLE) && (MSM_DUMP_CLIENT_APPS == top_entry.id))
				{
					/* We have the apps dump table now. */
					dump_table_type *apps_dump_table = (dump_table_type *)(uintptr_t)top_entry.start_addr;

					/* Parse all the entries within the apps dump table */
					for (j = 0; j < apps_dump_table->num_entries; j++)
					{
						dump_entry_type entry = apps_dump_table->entries[j];

						/* Find the corresponding entry for each feature for every CPU */
						if ((entry.start_addr) && (entry.type == MSM_DUMP_TYPE_DATA))
						{
							/* The LSN of the id field indicates which CPU / block when the feature involves
							 * multiple CPUs or blocks*/
							uint32 count = entry.id & 0xFFFFFFF0;
	                        //if (count > MSM_MAX_DUMP )
	                            //sysdbg_ERR_FATAL("Invalid Dump Table entry id", entry.id, count, 0);

							switch (count)
							{
							  case CPU_L1_ICACHE_DUMP:
								  if (!sysdbgCacheDump.L1ICacheDump[cpu])
									  sysdbgCacheDump.L1ICacheDump[cpu] =(dump_data_type*)(uintptr_t)entry.start_addr;
								  break;

							  case CPU_L1_DCACHE_DUMP:
								  if (!sysdbgCacheDump.L1DCacheDump[cpu])
									  sysdbgCacheDump.L1DCacheDump[cpu] = (dump_data_type*)(uintptr_t)entry.start_addr;
								  break;

							  case CPU_L2_DCACHE_DUMP:
							      if (cpu == 0  &&  !sysdbgCacheDump.L2CacheDump[cpu])
									  sysdbgCacheDump.L2CacheDump[cpu] = (dump_data_type*)(uintptr_t)entry.start_addr;
								  break;
							  
							  case MSM_CPU_REGS_DUMP:
								  if (!sysdbgCPUDump.CPUDump[cpu] && ((entry.id&0x0000000F)==cpu) )
									  sysdbgCPUDump.CPUDump[cpu] = (dump_data_type*)(uintptr_t)entry.start_addr;
								  break;

							  case MSM_ETB_DUMP:
								  if (!sysdbgETBDump)
									  sysdbgETBDump = (dump_data_type*)(uintptr_t)entry.start_addr;
								  break;

							  case MSM_ETM_DUMP:
								  if (!sysdbgETMDump.ETMCPU[cpu])
									  sysdbgETMDump.ETMCPU[cpu] = (dump_data_type*)(uintptr_t)entry.start_addr;
								  break;


							  case MSM_TMC_DUMP:
							      /* KW error fix */
								  {
                                      int id = entry.id - MSM_TMC_DUMP;

                                      if (id < 0  ||  id > 1 )
								          return FALSE;
								      if (!sysdbgTMCDump.TMCs[id])
									      sysdbgTMCDump.TMCs[id] = (dump_data_type*)(uintptr_t)entry.start_addr; 
                                  }
								  break;

							  //default:
								  //sysdbg_ERR_FATAL(sysdbg_FATAL_ERROR, entry.id, 0, 0); 
							}
						}
					}
				}
			}
		 }
	return TRUE;
}


/*===========================================================================
** 	Function: sysdbgDumpETB
** ==========================================================================
*/
/*!
* 
* @brief
*	Function that dumps the ETB information
*
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
*	Must be called only after global pointer sysdbgETBDump is initialized
*
*/
boolean sysdbgDumpETB(void)
{
	static uint32 timeout = 50;
	uint32 head, data, size, time = 0;

	if ((sysdbgETBDump) && (sysdbgETBDump->magic != DUMP_MAGIC_NUMBER))
	{
		sysdbgETBDump->version = 0x11;
		sysdbgETBDumpType *ETBDump = (sysdbgETBDumpType *)(uintptr_t)sysdbgETBDump->start_addr;
		
		// We have do something only if Trace was enabled
		if (check_trace_enabled(TMC_ETB_TYPE))
		{
			// Find the size of the ETB
			size = HWIO_IN(QDSS_ETFETB_RSZ);
			// Unclock ETB
			HWIO_OUT(QDSS_ETFETB_LAR, 0xC5ACCE55);
			__DSB();

			// Flush the system and stop formatting when flush completes
			// HWIO_OUTF(QDSS_ETFETB_FFCR, STOPONFL, 0x1);
			// HWIO_OUTF(QDSS_ETFETB_FFCR, FLUSHMAN_W, 0x1);
			// Disable ETB. Should be disabled on a reset
			// HWIO_OUT(QDSS_ETFETB_CTL, 0x0);
			// Poll for the flush bit to go low
			while ((time++ < timeout) && (HWIO_INF(QDSS_ETFETB_FFCR, FLUSHMAN_W)));
			time = 0;
			while ((time++ < timeout) && (!HWIO_INF(QDSS_ETFETB_STS, TMCREADY)));

			// As per the new spec, we should be ready the data directly without keeping track 
			// of the size or current location of the read / write pointers. So keep reading until we 
			// get a 0xFFFFFFFF
			head = 0;
			while (head < size)
			{
				data = HWIO_IN(QDSS_ETFETB_RRD);
				if (data == 0xFFFFFFFF)
					break;
				ETBDump->entries[head] = data;
				head++;
			}

			// Lock ETB again. 
			HWIO_OUT(QDSS_ETFETB_LAR, 0xF00DD00D);

			// Write the magic number
			sysdbgETBDump->magic = DUMP_MAGIC_NUMBER;
			memcpy(sysdbgETBDump->name, "SYSDBG", 6);
		}
	}

	return TRUE;
}

/*===========================================================================
** 	Function: sysdbgDumpETM
** ==========================================================================
*/
/*!
* 
* @brief
*	Function that dumps the ETM information
*
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
*	Must be called only after global pointer sysdbgETMDump is initialized
*
*/
boolean sysdbgDumpETM(void)
{
	uint32 etmcount, reg;
	uint32 *base;

	/* Make sure pointer is not null */
	etmcount = tzbsp_cur_cpu_num();

	base = (uint32 *)((uintptr_t)(CPU0_ETM_BASE + (etmcount * ETM_BLOCK_SIZE)));

	if ((sysdbgETMDump.ETMCPU[etmcount]) && (sysdbgETMDump.ETMCPU[etmcount]->magic == DUMP_MAGIC_NUMBER))
	{

		sysdbgETMDump.ETMCPU[etmcount]->version = 0x11;
		sysdbgETMCPUDumpType *ETMCPU = 	(sysdbgETMCPUDumpType *)(uintptr_t)sysdbgETMDump.ETMCPU[etmcount]->start_addr;

		for (reg = 0; reg < 1024; reg +=1)
		{
			// We need to unlock access to the block
			if (!reg)
			{
				// Disable OS Lock using OSLAR
				*(base + 0x00C0) = 0;
				// Disable global lock
				*(base + 0x03EC) = 0xC5ACCE55;	
			}

			ETMCPU->entries[reg] = *(base + reg);
		}
			
		// Write the magic number to say that we have dumped ETB registers
		sysdbgETMDump.ETMCPU[etmcount]->magic = DUMP_MAGIC_NUMBER;
		memcpy(sysdbgETMDump.ETMCPU[etmcount]->name, "SYSDBG", 6);

	}

    return TRUE;

}

/*===========================================================================
** 	Function: sysdbgDumpTMC
** ==========================================================================
*/
/*!
* 
* @brief
*	Function that dumps the TMC information.
*
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
*	Must be called only after global pointer sysdbgTMCDump is initialized
*
*/

boolean sysdbgDumpTMC(tmc_type tmc)
{
#define TMC_RRD	0x4
#define TMC_RWD	0x9

	uint32 reg;
   	uint32 *base;

	/* Set the base depending on the TMC */
	switch(tmc)
	{
		case TMC_ETB_TYPE:
		  base = (uint32 *)(QDSS_ETFETB_BASE);
		  break;
		
		case TMC_ETR_TYPE:
		  base = (uint32 *)(QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE);
		  break;

		default:
		  return FALSE;
	}
	
	/* Dump that TMC's registers */
	if ((sysdbgTMCDump.TMCs[tmc]) && (sysdbgTMCDump.TMCs[tmc]->magic!=DUMP_MAGIC_NUMBER))
	{
		sysdbgTMCDump.TMCs[tmc]->version = 0x11;
		sysdbgTMCPerType *TMCs = (sysdbgTMCPerType*)(uintptr_t)sysdbgTMCDump.TMCs[tmc]->start_addr ;
		for(reg = 0; reg < 1024; reg+= 1)
		{
			if (!reg)
				*(base + 0x03EC) = 0xC5ACCE55;

			// Do not touch the RRD and RWD registers as reading them modifies the pointers
			if ((reg == TMC_RRD) || (reg == TMC_RWD))
				continue;

			TMCs->entries[reg] = *(base + reg);
		}
		// Write the magic number to say that we have dumped that tmc's registers
		sysdbgTMCDump.TMCs[tmc]->magic = DUMP_MAGIC_NUMBER;
		memcpy(sysdbgTMCDump.TMCs[tmc]->name, "SYSDBG", 6);
	}
		
	return TRUE;
}


/*===========================================================================
** 	Function: sysdbgCPUDumpCopy
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that copies the data from internal IMEM structure to external
* 	DDR location
*
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
*	Must be called only after global pointer is initialized
*
*/
boolean sysdbgCPUDumpCopy(void)
{
	//volatile uint32 *reg;
	// Find the entry for this CPU
	uint32 cpu = tzbsp_cur_cpu_num();
	if(sysdbgCPUDump.CPUDump[cpu] && sysdbgCPUDump.CPUDump[cpu]->magic != DUMP_MAGIC_NUMBER)
	{
		sysdbgCPUDump.CPUDump[cpu]->version = 0x11; //update version before dump
		sysdbg_cpu_ctx_t *sysdbg_cpu_ctx = &Sysdbg_Workspace_N_Begin;
	
		sysdbgCPUCtxtType *sysdbgCPUCtxt = (sysdbgCPUCtxtType*)(uintptr_t)sysdbgCPUDump.CPUDump[cpu]->start_addr;	
		sysdbg_cpu32_ctxt_regs_type *sysdbg_ctxt_regs = (sysdbg_cpu32_ctxt_regs_type*)&(sysdbgCPUCtxt->cpu_regs);
		if(cpu==0)
		{

			//sysdbg_cpu32_ctxt_regs_type *sbl2_saved_ctxt_regs = (sysdbg_cpu32_ctxt_regs_type *)sysdbg_cpu_ctx->usr_r0;
			memcpy(sysdbg_ctxt_regs, (void *)sysdbg_cpu_ctx->usr_r0, sizeof(sysdbg_cpu32_ctxt_regs_type));
		}
		else
		{
		
			uint64 *sysdbg_cpu_reg_ptr = (uint64*)&sysdbg_ctxt_regs->r0;	
            uint32 *tzbsp_saved_reg_ptr = (uint32*)&sysdbg_cpu_ctx->usr_r0; //pointing to USR_R0
				for(int i=0; i<13; i++)
			{
				*sysdbg_cpu_reg_ptr++ = (uint64)*tzbsp_saved_reg_ptr++;//Saving all the USR registers
			}
     	sysdbg_ctxt_regs->r13_usr = (uint64) sysdbg_cpu_ctx->usr_r13;
        sysdbg_ctxt_regs->r14_usr = (uint64) sysdbg_cpu_ctx->usr_r14;
		
        sysdbg_ctxt_regs->r13_irq = (uint64) sysdbg_cpu_ctx->irq_r13;
        sysdbg_ctxt_regs->r14_irq = (uint64) sysdbg_cpu_ctx->irq_r14;

        sysdbg_ctxt_regs->r13_svc = (uint64) sysdbg_cpu_ctx->svc_r13;
        sysdbg_ctxt_regs->r14_svc = (uint64) sysdbg_cpu_ctx->svc_r14;

        sysdbg_ctxt_regs->r13_abt = (uint64) sysdbg_cpu_ctx->abt_r13;
        sysdbg_ctxt_regs->r14_abt = (uint64) sysdbg_cpu_ctx->abt_r14;

        sysdbg_ctxt_regs->r13_und = (uint64) sysdbg_cpu_ctx->und_r13;
        sysdbg_ctxt_regs->r14_und = (uint64) sysdbg_cpu_ctx->und_r14;
		
		sysdbg_ctxt_regs->r13_mon = (uint64) sysdbg_cpu_ctx->mon_r13;
        sysdbg_ctxt_regs->r14_mon = (uint64) sysdbg_cpu_ctx->mon_r14;


			sysdbg_cpu_reg_ptr = (uint64*)&sysdbg_ctxt_regs->r8_fiq;	
        tzbsp_saved_reg_ptr = (uint32*)&sysdbg_cpu_ctx->fiq_r8; //pointing to fiq_r8
			for(int i=0; i<7; i++)
			{
				*sysdbg_cpu_reg_ptr++ = (uint64)*tzbsp_saved_reg_ptr++;//Saving all the FIQ registers
			}

		}

		//reg = (uint32 *)(HWIO_QDSS_WRAPPER_DEBUG_UI_DATA_0_ADDR + (0x10 * cpu));
		sysdbg_ctxt_regs->pc = HWIO_INI(QDSS_WRAPPER_DEBUG_UI_DATA_n, (cpu*0x4));	//reading PC for current core from DEBUG_UI
		sysdbgCPUDump.CPUDump[cpu]->magic = DUMP_MAGIC_NUMBER; //update magic after dumping
	
		//TODO SPSR REGS, MON/HYP REGISTERS
	}
	return TRUE;
}

sysdbgCPUDumpType* sysdbg_get_cpudump()
{
	sysdbgMemoryDumpInit();
	return &sysdbgCPUDump;
}
