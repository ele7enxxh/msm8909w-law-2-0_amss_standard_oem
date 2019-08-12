#ifndef TZBSP_MEMORY_DUMP_H
#define TZBSP_MEMORY_DUMP_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_memory_dump.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/12   tk      First version.
============================================================================*/

#include "tzbsp_target.h"

/* Dump data type */
typedef struct
{
    uint32 version;
    uint32 magic;
    char name[32];
    uint64 start_addr;
    uint64 len;
}dump_data_type;

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
    dump_data_type *CPUDump[TZBSP_CPU_COUNT_MAX];
}sysdbgCPUDumpType;

/* Dump format for power status, spm and GICD registers */
typedef struct tzbsp_dump_gicd_spm_reg_s
{
  uint32 MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL;
  uint32 APCS_ALIASn_SAW2_SPM_STS[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_ALIASn_SAW2_SPM_CTL[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_ALIASn_APC_PWR_STATUS[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_ALIASn_APC_PWR_GATE_STATUS[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_L2_PWR_STATUS;
  uint32 APCS_L2_SAW2_SPM_STS;
  uint32 APCS_L2_SAW2_SPM_CTL;
  uint32 APCS_GICD_ISENABLER0_PRIV[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_GICD_ISPENDR0_PRIV[TZBSP_CPU_COUNT_MAX];
  uint32 APCS_GICD_ISENABLERn_PUB[7];
  uint32 APCS_GICD_ISPENDRn_PUB[7];
} tzbsp_dump_gicd_spm_reg_t;

#endif /* TZBSP_MEMORY_DUMP_H */
