/*=============================================================================
                        sysdbg Memory Dump Utility File 

GENERAL DESCRIPTION     
  This module defines structures for the memory dump driver.
  
    Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/sysdbg/inc/mdm9x07/sysdbg_mem_dump.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
11/26/2013 ntonupun     Added RTC Logging, updated sysdbgStatsDump structure.
10/02/2013  AJCheriyan  Bumped up CPU Dump version
07/15/2013  AJCheriyan  Some API updates
06/26/2013  AJCheriyan  Fixed order of stats dump fields
03/12/2012	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#ifndef _sysdbg_MEMORY_DUMP_H
#define _sysdbg_MEMORY_DUMP_H

#include "comdef.h"

// Common Macros
#define SYSDBG_ENUM_32BITS(x) x##_INVALID=0x7FFFFFFF


#define SIZE_CPU_CTXT_STRUCT    sizeof(dbi_cpu_ctxt_regs_type)
#define INVALID_CPU             0xFFFFFFFF
#define INVALID_CLUSTER         0xFFFFFFFF

// Magic and version numbers used by TZ code. Needs to match TZ code
#define DUMP_SUCCESS_MAGIC      0x59414B4F
#define CACHE_FORMAT_VERSION    0x100

#define DUMP_MAGIC_NUMBER       0x42445953

#define CPU_MAGIC_NUMBER        0x44434151
#define CPU_FORMAT_VERSION      0x11
#define CPU_TZ_STATUS_BIT       0x5

// ETM related macros
// ETM Register block size = 4k bytes
#define ETM_BLOCK_SIZE          0x1000

// Maximum number of Debug UI Data registers
#define MAX_DEBUG_UI_REGS       64

extern void __weak tzbsp_cache_flush(void);

typedef enum
{
    CPU_SEC_CTXT,
    CPU_NSEC_CTXT,
    CPU_CTXT_COUNT,
    SYSDBG_ENUM_32BITS(CPU_CTXT),
}cpu_ctxt_type;

typedef enum
{
    /* ETR - Embedded Trace Router */
    TMC_ETR_TYPE=0,
    /* Embedded Trace Buffer / Funnel */    
    TMC_ETB_TYPE,
    MAX_TMC_TYPES,
    SYSDBG_ENUM_32BITS(TMC_TYPES),
}tmc_type;

typedef enum
{
    ETM_CORE0_TYPE=0,
    ETM_CORE1_TYPE,
    ETM_CORE2_TYPE,
    ETM_CORE3_TYPE,
    MAX_ETM_TYPES=TARGET_APPS_CPUS,
}etm_type;
/* Dump data type */
typedef struct
{
    uint32 version;
    uint32 magic;
    char name[32];
    uint64 start_addr;
    uint64 len;
}dump_data_type;

typedef struct 
{
    uint32 sec:16;
    uint32 cpu:16;
}cpuid_type;

/* Generic header for every kind of dump supported */
typedef struct
{
    /* Magic number */
    uint32 magic;
    /* Format version of the saved structure */
    uint32 version; 
    /* Status bits - Indicates 
     * status of the entity when info 
     * is being dumped etc. */
    uint32 status[2];
}dump_header_type;

/* External CPU Dump Structure - 64 bit EL */
typedef struct
{
    uint64 x0;
    uint64 x1;
    uint64 x2;
    uint64 x3;
    uint64 x4;
    uint64 x5;
    uint64 x6;
    uint64 x7;
    uint64 x8;
    uint64 x9;
    uint64 x10;
    uint64 x11;
    uint64 x12;
    uint64 x13;
    uint64 x14;
    uint64 x15;
    uint64 x16;
    uint64 x17;
    uint64 x18;
    uint64 x19;
    uint64 x20;
    uint64 x21;
    uint64 x22;
    uint64 x23;
    uint64 x24;
    uint64 x25;
    uint64 x26;
    uint64 x27;
    uint64 x28;
    uint64 x29;
    uint64 x30;
    uint64 pc;
    uint64 currentEL;
    uint64 sp_el3;
    uint64 elr_el3;
    uint64 spsr_el3;
    uint64 sp_el2;
    uint64 elr_el2;
    uint64 spsr_el2;
    uint64 sp_el1;
    uint64 elr_el1;
    uint64 spsr_el1;
    uint64 sp_el0;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu64_ctxt_regs_type;

/* External CPU Dump format - 32 bit EL */
typedef struct
{
    uint64 r0;
    uint64 r1;
    uint64 r2;
    uint64 r3;
    uint64 r4;
    uint64 r5;
    uint64 r6;
    uint64 r7;
    uint64 r8;
    uint64 r9;
    uint64 r10;
    uint64 r11;
    uint64 r12;
    uint64 r13_usr;
    uint64 r14_usr;
    uint64 r13_hyp;
    uint64 r14_irq;
    uint64 r13_irq;
    uint64 r14_svc;
    uint64 r13_svc;
    uint64 r14_abt;
    uint64 r13_abt;
    uint64 r14_und;
    uint64 r13_und;
    uint64 r8_fiq;
    uint64 r9_fiq;
    uint64 r10_fiq;
    uint64 r11_fiq;
    uint64 r12_fiq;
    uint64 r13_fiq;
    uint64 r14_fiq;
    uint64 pc;
    uint64 cpsr;
    uint64 r13_mon;
    uint64 r14_mon;
    uint64 r14_hyp;
    uint64 _reserved;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu32_ctxt_regs_type;

typedef union
{
    sysdbg_cpu32_ctxt_regs_type cpu32_ctxt;
    sysdbg_cpu64_ctxt_regs_type cpu64_ctxt;
}sysdbg_cpu_ctxt_regs_type;

// External format for CPU context
typedef struct
{
    /* Status fields */
    uint32 status[4];
    /* Context for all CPUs */
    sysdbg_cpu_ctxt_regs_type cpu_regs;
    /* Secure Context - Not used  */
    sysdbg_cpu_ctxt_regs_type __reserved3;
}sysdbgCPUCtxtType;

typedef struct
{
    dump_data_type *CPUDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
}sysdbgCPUDumpType;


/* Stats dump structure */
typedef struct
{
    /* First 3 entries populated by TZ */
    /* Magic number */
    uint32 magic;
    /* Bit mask for L1 validity */
    uint32 l1_valid;
    /* Bit Mask for L2 validity */
    uint32 l2_valid;
    /* Populated by sysdbg */
    // MSM Reset Reason
    uint32 msm_reset_reason;
    // PMIC Power On Reason
    uint32 pon_reason;
    // PMIC Warm Reset Reason
    uint32 wrst_reason;
    // PMIC Power Off Reason
    uint32 poff_reason;
	// PMIC misc including PMIC PON PBL status, PS_HOLD reset control and 
	// PS_HOLD RESET CTL2
	uint32 misc;
	
}sysdbgStatsDumpType;


/* Enum for cache type */
typedef enum
{
    L0DATA = 0x0,
    L0INST = 0x1,
    L0UNIF = 0x2,
    L1DATA = 0x3,
    L1INST = 0x4,
    L1UNIF = 0x5,
    L2DATA = 0x6,
    L2INST = 0x7,
    L2UNIF = 0x8,
    L3INST = 0x8,
    L3DATA = 0x9,
    L3UNIF = 0xA,
    LxMAX  = 0x7FFFFFFF,
}sysdbg_cache_type;

/* Enum for various cache rams. A one-to-one mapping with the value */
typedef enum
{
    L1ITAGRAM  = 0x0,
    L1IDATARAM = 0x1,
    L1IBTBRAM  = 0x2,
    L1IGHBRAM  = 0x3,
    L1ITLBRAM  = 0x4,
    L1IPRAMRAM = 0x5,
    L1DTAGRAM  = 0x8,
    L1DDATARAM = 0x9,
    L1DTLBRAM  = 0xA,
    RESERV1 = 0xB,
    RESERV2 = 0xC,
    RESERV3 = 0xD,
    RESERV4 = 0xE,
    RESERV5 = 0xF,
    L2TAGRAM  = 0x10,
    L2DATARAM = 0x11,
    L2STAGRAM = 0x12,
    L2PARITYRAM = 0x13,
    L2DIRTYRAM = 0x14,
    L2TLBRAM   = 0x15,
    CACHE_RAM_MAX = 0x7FFFFFFF,
}sysdbgOCRamType;

/* Cache dump structure */
typedef struct 
{
    /* Status fields */
    uint32 status[4];
    /* Tag Size in u32 words */
    uint32 TagSize;
    /* Line Size in u32 words */
    uint32 LineSize;
    /* Number of sets */
    uint32 NumSets;
    /* Number of ways */
    uint32 NumWays;
    uint64 *next;
    uint32 __reserved[4];
    uint32 lines[];
}sysdbgCacheLvlDumpType;

/* Onchip RAM Dump structure */
typedef struct
{
    /* Number of entries in the dump*/
    uint32 NumEntries;
    /* Size of each entry */
    uint32 EntrySize;
    uint64 *next;
    uint32 __reserved[4];
    uint32 lines[];
}sysdbgOCRamDumpType;

/* Collective Cache Dump Structure */
typedef struct
{
    /* All L0 cache dump buffers */
    dump_data_type *L0CacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L1 I-cache dump buffers */
    dump_data_type *L1ICacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L1 D-cache dump buffers */
    dump_data_type *L1DCacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L2 cache dump buffers */
    dump_data_type *L2CacheDump[TARGET_CLUSTERS];
    /* At the time of writing, a reserved and unused field */
    dump_data_type *L3CacheDump[TARGET_CLUSTERS];
}sysdbgCacheDumpType;

/* L2 dump structure */
typedef struct
{
    uint32 magic_number;
    uint32 version;
    uint32 TagSize;
    uint32 LineSize;
    uint32 NumLines; /* Ways * Sets */
    uint32 lines[];
}sysdbgL2DumpType;

/* ETB / ETF Dump Structure */
typedef struct
{
    uint32 entries[1024];
}sysdbgETBDumpType;

/* ETM Dump Structure per CPU */
typedef struct
{
    uint32 entries[1024];
}sysdbgETMCPUDumpType;

/* Global struct for ETM Regs */
typedef struct
{
    dump_data_type *ETMCPU[TARGET_APPS_CPUS];
}sysdbgETMDumpType;

/* TMC Dump Structure per Tracing Cell */
typedef struct
{
    uint32 entries[1024];
}sysdbgTMCPerType;

typedef struct
{
    dump_data_type *TMCs[MAX_TMC_TYPES];
//    sysdbgTMCPerType *TMCs[MAX_TMC_TYPES];
}sysdbgTMCDumpType;

typedef struct 
{
    uint32  debugUIData [MAX_DEBUG_UI_REGS];
} sysdbgDebugUIRegDumpType;

/* HLOS Dump Structure */
/* Dump Type */
typedef enum
{
    MSM_CPU_REGS_DUMP = 0x00,
    CPU_L0_ICACHE_DUMP = 0x20,
    CPU_L0_DCACHE_DUMP = 0x40,
    CPU_L1_ICACHE_DUMP = 0x60,
    CPU_L1_DCACHE_DUMP = 0x80,
    CPU_L2_DCACHE_DUMP = 0xC0,
    CPU_L2_ICACHE_DUMP = CPU_L2_DCACHE_DUMP,
    CPU_L3_CACHE_DUMP = 0xD0,
    CPU_L3_ICACHE_DUMP = CPU_L3_CACHE_DUMP,
    CPU_L3_DCACHE_DUMP = CPU_L3_ICACHE_DUMP,
    _RESERVED1 = 0xE0, // Reserved for OCMEM
    MSM_ETB_DUMP = 0xF0, 
    MSM_ETM_DUMP = 0xA0,
    MSM_TMC_DUMP = 0x100,
    MSM_QDSS_DEBUG_UI_DUMP = 0x110,
    MSM_MAX_DUMP = 0x200,
    MAX_DUMP_TYPES = 0x7FFFFFFF,
}dump_type;

/* Dump entry in the table */
typedef struct
{
    dump_type id;
    char name[32];
    uint32 type;
    uint64 start_addr;
}dump_entry_type;

/* Dump Table */
typedef struct 
{
    uint32 version;
    uint32 num_entries;
    dump_entry_type entries[MSM_MAX_DUMP>>4];
}dump_table_type;


/* Enums for the various clients in the dump table */
typedef enum
{
    MSM_DUMP_CLIENT_APPS = 0,
    MSM_DUMP_CLIENT_RPM  = 1,
    MSM_DUMP_CLIENT_MPSS = 2, 
    MSM_DUMP_CLIENT_ADSP = 3, 
    MSM_DUMP_CLIENT_WCSS = 4,
    MSM_DUMP_CLIENT_MAX  = 5,
}msm_dump_clients;

/* Enum for the type of dump entry provided by HLOS */
typedef enum
{
    MSM_DUMP_TYPE_DATA = 0x0,
    MSM_DUMP_TYPE_TABLE = 0x1,
}msm_dump_entry_type;

/**
 * CPU general purpose register context as used by the monitor for context
 * saving and loading. Copied from tzbsp moniter util
 */
typedef struct sysdbg_cpu_ctx_s
{
  uint32 usr_r0;
  uint32 usr_r1;
  uint32 usr_r2;
  uint32 usr_r3;
  uint32 usr_r4;
  uint32 usr_r5;
  uint32 usr_r6;
  uint32 usr_r7;
  uint32 usr_r8;
  uint32 usr_r9;
  uint32 usr_r10;
  uint32 usr_r11;
  uint32 usr_r12;
  uint32 usr_r13;
  uint32 usr_r14;
  uint32 irq_r13;
  uint32 irq_r14;
  uint32 svc_r13;
  uint32 svc_r14;
  uint32 abt_r13;
  uint32 abt_r14;
  uint32 und_r13;
  uint32 und_r14;
  uint32 fiq_r8;
  uint32 fiq_r9;
  uint32 fiq_r10;
  uint32 fiq_r11;
  uint32 fiq_r12;
  uint32 fiq_r13;
  uint32 fiq_r14;
  uint32 mon_r13;
  uint32 mon_r14;
} sysdbg_cpu_ctx_t;

/*===========================================================================
** 	Function: sysdbgFlushCache
** ==========================================================================
*/
/*!
* 
* @brief
*    Wrapper function that checks whether cache needs to be flushed or not 
*
* @param[in] 
*    None
*  
* @par Dependencies
*    None
* 
* @retval
*   None
* 
* @par Side Effects
*	Must be called only after global struct is initialized
*
*/
boolean sysdbgFlushCache(void);



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
*    Pointer to data that contains stats populated by sysdbg
*  
* @par Dependencies
*    None
* 
* @retval
*   Success / Fail
* 
* @par Side Effects
*    Must be called only after global struct is initialized
*
*/
boolean sysdbgDumpStats(uint32 *data);

/*===========================================================================
** 	Function: sysdbgMemoryReset
** ==========================================================================
*/
/*!
* 
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
boolean sysdbgMemoryReset(void);


/*===========================================================================
** 	Function: SYSDBGDumpETM
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
*	Must be called only after global pointer SYSDBGETMDump is initialized
*
*/
boolean sysdbgDumpETM(void);

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

boolean sysdbgDumpTMC(tmc_type tmc);

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
boolean sysdbgDumpTrace(void);

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
boolean sysdbgDumpETB(void);

/*===========================================================================
** 	Function: check_trace_enabled
** ==========================================================================
*/
/*!
* 
* @brief
*	Helper function that checks if a particular sink is enabled
*/
static boolean check_trace_enabled(tmc_type tmc);


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
boolean sysdbgMemoryDumpInit(void);


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
boolean sysdbgCPUDumpCopy(void);


/*===========================================================================
** 	Function: sysdbg_get_cpudump()
** ==========================================================================
*/
/*!
* 
* @brief
*
* 	initializes and returns sysdbgCPUDump pointer with CPU dump pointer
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
sysdbgCPUDumpType* sysdbg_get_cpudump(void);


/*===========================================================================
**     Function: sysdbg_dumpDebugUIRegs()
** ==========================================================================
*/
/*!
* 
* @brief
*     dumps out the 32 Debug UI registers
*
* @param[in] 
*    None
*  
* @par Dependencies
*    None
* 
* @retval
*   None
* 
* @par Side Effects
*    Must be called only after global pointer is initialized
*
*/
boolean sysdbg_dumpDebugUIRegs(void);


#endif /* _sysdbg_MEMORY_DUMP_H */
