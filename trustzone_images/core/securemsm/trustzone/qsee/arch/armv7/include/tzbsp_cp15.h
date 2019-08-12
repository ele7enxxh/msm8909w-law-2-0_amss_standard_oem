
/***************************************************************************

 MODULE:  cp15_registers.h

 DESCRIPTION: define macros for reading and writing to the cp registers
     for the ARMv7

 AUTHOR: KAS

 REV/DATE:  Fri Mar 18 15:54:32 EST 2005

 Copyright (c) 2005-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*****************************************************************************/


#ifndef TZBSP_CP15_H
#define TZBSP_CP15_H

#include <comdef.h>
#include "tzbsp_mcrmrc.h"

#define WCP15_MIDR(reg)   MCR15(reg,0,  c0  ,c0 ,0)
#define WCP15_CTR(reg)    MCR15(reg,0,  c0  ,c0 ,1)
#define WCP15_TCMTR(reg)  MCR15(reg,0,  c0  ,c0 ,2)
#define WCP15_TLBTR(reg)  MCR15(reg,0,  c0  ,c0 ,3)
#define WCP15_MIDR_ALIAS2(reg)  MCR15(reg,0,  c0  ,c0 ,4)
#define WCP15_MPIDR(reg)  MCR15(reg,0,  c0  ,c0 ,5)
#define WCP15_MIDR_ALIAS3(reg)  MCR15(reg,0,  c0  ,c0 ,6)
#define WCP15_MIDR_ALIAS4(reg)  MCR15(reg,0,  c0  ,c0 ,7)
#define WCP15_CCSIDR(reg) MCR15(reg,1,  c0  ,c0 ,0)
#define WCP15_CLIDR(reg)  MCR15(reg,1,  c0  ,c0 ,1)
#define WCP15_CSSELR(reg) MCR15(reg,2,  c0  ,c0 ,0)
#define WCP15_ID_PFR0(reg)  MCR15(reg,0,  c0  ,c1 ,0)
#define WCP15_ID_PFR1(reg)  MCR15(reg,0,  c0  ,c1 ,1)
#define WCP15_ID_DFR0(reg)  MCR15(reg,0,  c0  ,c1 ,2)
#define WCP15_ID_AFR0(reg)  MCR15(reg,0,  c0  ,c1 ,3)
#define WCP15_ID_MMFR0(reg) MCR15(reg,0,  c0  ,c1 ,4)
#define WCP15_ID_MMFR1(reg) MCR15(reg,0,  c0  ,c1 ,5)
#define WCP15_ID_MMFR2(reg) MCR15(reg,0,  c0  ,c1 ,6)
#define WCP15_ID_MMFR3(reg) MCR15(reg,0,  c0  ,c1 ,7)
#define WCP15_ID_ISAR0(reg) MCR15(reg,0,  c0  ,c2 ,0)
#define WCP15_ID_ISAR1(reg) MCR15(reg,0,  c0  ,c2 ,1)
#define WCP15_ID_ISAR2(reg) MCR15(reg,0,  c0  ,c2 ,2)
#define WCP15_ID_ISAR3(reg) MCR15(reg,0,  c0  ,c2 ,3)
#define WCP15_ID_ISAR4(reg) MCR15(reg,0,  c0  ,c2 ,4)
#define WCP15_ID_ISAR5(reg) MCR15(reg,0,  c0  ,c2 ,5)
#define WCP15_SCTLR(reg)  MCR15(reg,0,  c1  ,c0 ,0)
#define WCP15_ACTLR(reg)  MCR15(reg,0,  c1  ,c0 ,1)
#define WCP15_CPACR(reg)  MCR15(reg,0,  c1  ,c0 ,2)
#define WCP15_SCR(reg)    MCR15(reg,0,  c1  ,c1 ,0)
#define WCP15_SDER(reg)   MCR15(reg,0,  c1  ,c1 ,1)
#define WCP15_NSACR(reg)  MCR15(reg,0,  c1  ,c1 ,2)
#define WCP15_TTBR0(reg)  MCR15(reg,0,  c2  ,c0 ,0)
#define WCP15_TTBR1(reg)  MCR15(reg,0,  c2  ,c0 ,1)
#define WCP15_TTBCR(reg)  MCR15(reg,0,  c2  ,c0 ,2)
#define WCP15_DACR(reg)   MCR15(reg,0,  c3  ,c0 ,0)
#define WCP15_DFSR(reg)   MCR15(reg,0,  c5  ,c0 ,0)
#define WCP15_IFSR(reg)   MCR15(reg,0,  c5  ,c0 ,1)
#define WCP15_ADFSR(reg)  MCR15(reg,0,  c5  ,c1 ,0)
#define WCP15_AIFSR(reg)  MCR15(reg,0,  c5  ,c1 ,1)
#define WCP15_DFAR(reg)   MCR15(reg,0,  c6  ,c0 ,0)
#define WCP15_IFAR(reg)   MCR15(reg,0,  c6  ,c0 ,2)
#define WCP15_PAR(reg)    MCR15(reg,0,  c7  ,c4 ,0)
#define WCP15_ICIALLU(reg)  MCR15(reg,0,  c7  ,c5 ,0)
#define WCP15_ICIMVAU(reg)  MCR15(reg,0,  c7  ,c5 ,1)
#define WCP15_PFF(reg)    MCR15(reg,0,  c7  ,c5 ,4)
#define WCP15_BPIALL(reg) MCR15(reg,0,  c7  ,c5 ,6)
#define WCP15_BPIMVA(reg) MCR15(reg,0,  c7  ,c5 ,7)
#define WCP15_DCIMVAC(reg)  MCR15(reg,0,  c7  ,c6 ,1)
#define WCP15_DCISW(reg)  MCR15(reg,0,  c7  ,c6 ,2)
#define WCP15_V2PCWPR(reg)  MCR15(reg,0,  c7  ,c8 ,0)
#define WCP15_V2PCWPW(reg)  MCR15(reg,0,  c7  ,c8 ,1)
#define WCP15_V2PCWUR(reg)  MCR15(reg,0,  c7  ,c8 ,2)
#define WCP15_V2PCWUW(reg)  MCR15(reg,0,  c7  ,c8 ,3)
#define WCP15_V2POWPR(reg)  MCR15(reg,0,  c7  ,c8 ,4)
#define WCP15_V2POWPW(reg)  MCR15(reg,0,  c7  ,c8 ,5)
#define WCP15_V2POWUR(reg)  MCR15(reg,0,  c7  ,c8 ,6)
#define WCP15_V2POWUW(reg)  MCR15(reg,0,  c7  ,c8 ,7)
#define WCP15_DCCMVAC(reg)  MCR15(reg,0,  c7  ,c10  ,1)
#define WCP15_DCCSW(reg)  MCR15(reg,0,  c7  ,c10  ,2)
#define WCP15_DSB(reg)    MCR15(reg,0,  c7  ,c10  ,4)
#define WCP15_DMB(reg)    MCR15(reg,0,  c7  ,c10  ,5)
#define WCP15_DCCMVAU(reg)  MCR15(reg,0,  c7  ,c11  ,1)
#define WCP15_DCCIMVAC(reg) MCR15(reg,0,  c7  ,c14  ,1)
#define WCP15_DCCISW(reg) MCR15(reg,0,  c7  ,c14  ,2)
#define WCP15_ITLBIALL(reg) MCR15(reg,0,  c8  ,c5 ,0)
#define WCP15_ITLBIMVA(reg) MCR15(reg,0,  c8  ,c5 ,1)
#define WCP15_ITLBIASID(reg)  MCR15(reg,0,  c8  ,c5 ,2)
#define WCP15_DTLBIALL(reg) MCR15(reg,0,  c8  ,c6 ,0)
#define WCP15_DTLBIMVA(reg) MCR15(reg,0,  c8  ,c6 ,1)
#define WCP15_DTLBIASID(reg)  MCR15(reg,0,  c8  ,c6 ,2)
#define WCP15_UTLBIALL(reg) MCR15(reg,0,  c8  ,c7 ,0)
#define WCP15_UTLBIMVA(reg) MCR15(reg,0,  c8  ,c7 ,1)
#define WCP15_UTLBIASID(reg)  MCR15(reg,0,  c8  ,c7 ,2)
#define WCP15_ICPLILK(reg)  MCR15(reg,0,  c9  ,c0 ,0)
#define WCP15_ICRSW(reg)  MCR15(reg,0,  c9  ,c0 ,1)
#define WCP15_ICISW(reg)  MCR15(reg,0,  c9  ,c0 ,2)
#define WCP15_DCPLDLK(reg)  MCR15(reg,0,  c9  ,c0 ,4)
#define WCP15_DCRSW(reg)  MCR15(reg,0,  c9  ,c0 ,5)
#define WCP15_DCIALL(reg) MCR15(reg,0,  c9  ,c0 ,6)
#define WCP15_ICRTR0(reg) MCR15(reg,1,  c9  ,c0 ,0)
#define WCP15_ICRTR1(reg) MCR15(reg,1,  c9  ,c0 ,1)
#define WCP15_ICRDR(reg)  MCR15(reg,1,  c9  ,c0 ,2)
#define WCP15_ICLKCR(reg) MCR15(reg,1,  c9  ,c0 ,3)
#define WCP15_DCRTR0(reg) MCR15(reg,1,  c9  ,c0 ,4)
#define WCP15_DCRTR1(reg) MCR15(reg,1,  c9  ,c0 ,5)
#define WCP15_DCRDR(reg)  MCR15(reg,1,  c9  ,c0 ,6)
#define WCP15_DCLKCR(reg) MCR15(reg,1,  c9  ,c0 ,7)
#define WCP15_ICVIC0(reg)   MCR15(reg,0,    c9  ,c1 ,0)
#define WCP15_ICVIC1(reg)   MCR15(reg,0,    c9  ,c1 ,1)
#define WCP15_ICVIC2(reg)   MCR15(reg,0,    c9  ,c1 ,2)
#define WCP15_ICVIC3(reg)   MCR15(reg,0,    c9  ,c1 ,3)
#define WCP15_ICVIC4(reg)   MCR15(reg,0,    c9  ,c1 ,4)
#define WCP15_ICVIC5(reg)   MCR15(reg,0,    c9  ,c1 ,5)
#define WCP15_ICVIC6(reg)   MCR15(reg,0,    c9  ,c1 ,6)
#define WCP15_ICVIC7(reg)   MCR15(reg,0,    c9  ,c1 ,7)

#define WCP15_ICFLOOR0(reg)   MCR15(reg,1 ,c9 ,c1 ,0)
#define WCP15_ICFLOOR1(reg)   MCR15(reg,1 ,c9 ,c1 ,1)
#define WCP15_ICFLOOR2(reg)   MCR15(reg,1 ,c9 ,c1 ,2)
#define WCP15_ICFLOOR3(reg)   MCR15(reg,1 ,c9 ,c1 ,3)
#define WCP15_ICFLOOR4(reg)   MCR15(reg,1 ,c9 ,c1 ,4)
#define WCP15_ICFLOOR5(reg)   MCR15(reg,1 ,c9 ,c1 ,5)
#define WCP15_ICFLOOR6(reg)   MCR15(reg,1 ,c9 ,c1 ,6)
#define WCP15_ICFLOOR7(reg)   MCR15(reg,1 ,c9 ,c1 ,7)

#define WCP15_DCVIC0(reg)   MCR15(reg,2 ,c9 ,c1 ,0)
#define WCP15_DCVIC1(reg)   MCR15(reg,2 ,c9 ,c1 ,1)
#define WCP15_DCVIC2(reg)   MCR15(reg,2 ,c9 ,c1 ,2)
#define WCP15_DCVIC3(reg)   MCR15(reg,2 ,c9 ,c1 ,3)
#define WCP15_DCVIC4(reg)   MCR15(reg,2 ,c9 ,c1 ,4)
#define WCP15_DCVIC5(reg)   MCR15(reg,2 ,c9 ,c1 ,5)
#define WCP15_DCVIC6(reg)   MCR15(reg,2 ,c9 ,c1 ,6)
#define WCP15_DCVIC7(reg)   MCR15(reg,2 ,c9 ,c1 ,7)

#define WCP15_DCFLOOR0(reg) MCR15(reg,3,  c9  ,c1 ,0)
#define WCP15_DCFLOOR1(reg) MCR15(reg,3,  c9  ,c1 ,1)
#define WCP15_DCFLOOR2(reg) MCR15(reg,3,  c9  ,c1 ,2)
#define WCP15_DCFLOOR3(reg) MCR15(reg,3,  c9  ,c1 ,3)
#define WCP15_DCFLOOR4(reg) MCR15(reg,3,  c9  ,c1 ,4)
#define WCP15_DCFLOOR5(reg) MCR15(reg,3,  c9  ,c1 ,5)
#define WCP15_DCFLOOR6(reg) MCR15(reg,3,  c9  ,c1 ,6)
#define WCP15_DCFLOOR7(reg) MCR15(reg,3,  c9  ,c1 ,7)

#define WCP15_BTACRSW(reg)  MCR15(reg,0,  c9  ,c2 ,0)
#define WCP15_BTACWSW(reg)  MCR15(reg,0,  c9  ,c2 ,1)
#define WCP15_BHTRSW(reg) MCR15(reg,0,  c9  ,c2 ,2)
#define WCP15_BHTWSW(reg) MCR15(reg,0,  c9  ,c2 ,3)
#define WCP15_BPTR0(reg)  MCR15(reg,1,  c9  ,c2 ,0)
#define WCP15_BPTR1(reg)  MCR15(reg,1,  c9  ,c2 ,1)
#define WCP15_BPTR2(reg)  MCR15(reg,1,  c9  ,c2 ,2)
#define WCP15_L2DCRTR0(reg) MCR15(reg,0,  c9  ,c5 ,0)
#define WCP15_L2DCRTR1(reg) MCR15(reg,0,  c9  ,c5 ,1)
#define WCP15_L2DCRDR0(reg) MCR15(reg,0,  c9  ,c5 ,2)
#define WCP15_L2DCRDR1(reg) MCR15(reg,0,  c9  ,c5 ,3)
#define WCP15_TCMSELR(reg)  MCR15(reg,0,  c9  ,c6 ,0)
#define WCP15_TCMBNSACR(reg)  MCR15(reg,0,  c9  ,c6 ,1)
#define WCP15_TCMBBR(reg) MCR15(reg,0,  c9  ,c6 ,2)
#define WCP15_TCMBRAR(reg)  MCR15(reg,0,  c9  ,c6 ,3)
#define WCP15_SLVCR(reg)  MCR15(reg,0,  c9  ,c7 ,0)
#define WCP15_SLVESR(reg) MCR15(reg,1,  c9  ,c7 ,0)
#define WCP15_SLVEAR(reg) MCR15(reg,1,  c9  ,c7 ,1)
#define WCP15_SLVESYNR(reg) MCR15(reg,1,  c9  ,c7 ,2)
#define WCP15_SLVMPUL0IDR(reg)  MCR15(reg,2,  c9  ,c7 ,0)
#define WCP15_SLVMPUL1IDR(reg)  MCR15(reg,2,  c9  ,c7 ,1)
#define WCP15_SLVMPUL2IDR(reg)  MCR15(reg,2,  c9  ,c7 ,2)
#define WCP15_SLVMPUL3IDR(reg)  MCR15(reg,2,  c9  ,c7 ,3)
#define WCP15_SLVMPUSELR(reg) MCR15(reg,3,  c9  ,c7 ,0)
#define WCP15_SLVMPURPAPR(reg)  MCR15(reg,3,  c9  ,c7 ,1)
#define WCP15_ANSACR(reg)     MCR15(reg, 7, c15, c0 ,0)
#define WCP15_IEFSR(reg)      MCR15(reg, 0, c15, c0 ,1)
#define WCP15_VBAR(reg)       MCR15(reg, 0, c12, c0 ,0)
#define WCP15_PRRR(reg)       MCR15(reg, 0, c10, c2 ,0)
#define WCP15_NMRR(reg)       MCR15(reg, 0, c10, c2 ,1)
#define WCP15_CONTEXTIDR(reg) MCR15(reg, 0, c13, c0 ,1)

#define WCP15_L2NSACR(reg)    MCR15(reg, 3, c15, c0, 0)
#define WCP15_L2CR0(reg)      MCR15(reg, 3, c15 ,c0 ,1)
#define WCP15_L2CR1(reg)      MCR15(reg, 3, c15 ,c0 ,3)
#define WCP15_L2CPUCR(reg)    MCR15(reg, 3, c15 ,c0 ,2)
#define WCP15_L2CPUCPSELR(reg) MCR15(reg, 3, c15 ,c0 ,6)
#define WCP15_L2CPUCPDR(reg)  MCR15(reg, 3, c15 ,c0 ,7)

#define WCP15_SPCR(reg)       MCR15(reg, 0, c9  ,c7 ,0)
#define WCP15_DMASELR(reg)    MCR15(reg, 0, c11 ,c0 ,0)
#define WCP15_DMACHCR(reg)    MCR15(reg, 0, c11 ,c0 ,2)
#define WCP15_DMACHNSACR(reg) MCR15(reg, 0, c11, c0, 1)

#define WCP15_TLBLKCR(reg)    MCR15(reg,0,  c10 ,c1 ,3)

#define WCP15_TPIDRPRW(reg)   MCR15(reg,0,  c13 ,c0 ,4)
#define WCP15_TPIDRURW(reg)   MCR15(reg,0,  c13 ,c0 ,2)


/*
 * Scorpion 13.0 Changes
 */
#define WCP15_MMUDMTR(reg)  MCR15(reg,0,  c10 ,c4 ,0)
#define WCP15_MMUDCPR(reg)  MCR15(reg,0,  c10 ,c4 ,1)
#define WCP15_PVR0F0(reg)   MCR15(reg,0,  c15 ,c15  ,0)
#define WCP15_PVR0F1(reg)   MCR15(reg,0,  c15 ,c15  ,1)
#define WCP15_PVR0F2(reg)   MCR15(reg,0,  c15 ,c15  ,2)
#define WCP15_PVR1F0(reg)   MCR15(reg,1,  c15 ,c15  ,0)
#define WCP15_PVR2F0(reg)   MCR15(reg,2,  c15 ,c15  ,0)

#define WCP15_LPM1EVTYPER(reg)  MCR15(reg,1,  c15 ,c0 ,0)
#define WCP15_LPM2EVTYPER(reg)  MCR15(reg,2,  c15 ,c0 ,0)
#define WCP15_L2LPMEVTYPER(reg) MCR15(reg,3,  c15 ,c2 ,0)
#define WCP15_ANSARC(reg) MCR15(reg,7,  c15 ,c0 ,0)
#define WCP15_EFSR(reg)   MCR15(reg,7,  c15 ,c0 ,1)
#define WCP15_BPCR(reg)   MCR15(reg,7,  c15 ,c0 ,2)
#define WCP15_VIRQCPTR(reg) MCR15(reg,7,  c15 ,c0 ,3)
#define WCP15_VFRQCPTR(reg) MCR15(reg,7,  c15 ,c0 ,4)
#define WCP15_CPMR(reg)   MCR15(reg,7,  c15 ,c0 ,5)


/*
 * Performance Monitor Registers
 */
#define WCP15_PMACTLR(reg)    MCR15(reg,0,  c9  ,c15  ,5)
#define WCP15_PMCCNTCR(reg)   MCR15(reg,0,  c9  ,c15  ,2)
#define WCP15_PMCCNTR(reg)    MCR15(reg,0,  c9  ,c13  ,0)
#define WCP15_PMCCNTSR(reg)   MCR15(reg,0,  c9  ,c13  ,3)
#define WCP15_PMCNTENCLR(reg)   MCR15(reg,0,  c9  ,c12  ,2)
#define WCP15_PMCNTENSET(reg)   MCR15(reg,0,  c9  ,c12  ,1)
#define WCP15_PMCR(reg)     MCR15(reg,0,  c9  ,c12  ,0)
#define WCP15_PMINTENCLR(reg)   MCR15(reg,0,  c9  ,c14  ,2)
#define WCP15_PMINTENSET(reg)   MCR15(reg,0,  c9  ,c14  ,1)
#define WCP15_PMOVSR(reg)   MCR15(reg,0,  c9  ,c12  ,3)
#define WCP15_PMRLDR(reg)   MCR15(reg,0,  c9  ,c15  ,4)
#define WCP15_PMSELR(reg)   MCR15(reg,0,  c9  ,c12  ,5)
#define WCP15_PMSWINC(reg)    MCR15(reg,0,  c9  ,c12  ,4)
#define WCP15_PMUSERENR(reg)    MCR15(reg,0,  c9  ,c14  ,0)
#define WCP15_PMXEVCNTCR(reg)   MCR15(reg,0,  c9  ,c15  ,0)
#define WCP15_PMXEVCNTR(reg)    MCR15(reg,0,  c9  ,c13  ,2)
#define WCP15_PMXEVCNTSR(reg)   MCR15(reg,0,  c9  ,c15  ,1)
#define WCP15_PMXEVTYPER(reg)   MCR15(reg,0,  c9  ,c13  ,1)
#define WCP15_LPM0EVTYPER(reg)    MCR15(reg,0,  c15 ,c0 ,0)
#define WCP15_LPM1EVTYPER(reg)    MCR15(reg,1,  c15 ,c0 ,0)
#define WCP15_LPM2EVTYPER(reg)    MCR15(reg,2,  c15 ,c0 ,0)
#define WCP15_LPM3EVTYPER(reg)    MCR15(reg,3,  c15 ,c0 ,0)
#define WCP15_L2LPMEVTYPER(reg)   MCR15(reg,3,  c15 ,c2 ,0)
#define WCP15_VLPMEVTYPER(reg)    MCR15(reg,7,  c11 ,c0 ,0)


/*==============================================================================
 *
 * READ the registers
 *
 ==============================================================================*/
#define RCP15_MIDR(reg)   MRC15(reg,0,  c0, c0, 0)
#define RCP15_CTR(reg)    MRC15(reg,0,  c0  ,c0 ,1)
#define RCP15_TCMTR(reg)  MRC15(reg,0,  c0  ,c0 ,2)
#define RCP15_TLBTR(reg)  MRC15(reg,0,  c0  ,c0 ,3)
#define RCP15_MIDR_ALIAS2(reg)  MRC15(reg,0,  c0  ,c0 ,4)
#define RCP15_MPIDR(reg)  MRC15(reg,0,  c0  ,c0 ,5)
#define RCP15_MIDR_ALIAS3(reg)  MRC15(reg,0,  c0  ,c0 ,6)
#define RCP15_MIDR_ALIAS4(reg)  MRC15(reg,0,  c0  ,c0 ,7)
#define RCP15_CCSIDR(reg) MRC15(reg,1,  c0  ,c0 ,0)
#define RCP15_CLIDR(reg)  MRC15(reg,1,  c0  ,c0 ,1)
#define RCP15_CSSELR(reg) MRC15(reg,2,  c0  ,c0 ,0)
#define RCP15_ID_PFR0(reg)  MRC15(reg,0,  c0  ,c1 ,0)
#define RCP15_ID_PFR1(reg)  MRC15(reg,0,  c0  ,c1 ,1)
#define RCP15_ID_DFR0(reg)  MRC15(reg,0,  c0  ,c1 ,2)
#define RCP15_ID_AFR0(reg)  MRC15(reg,0,  c0  ,c1 ,3)
#define RCP15_ID_MMFR0(reg) MRC15(reg,0,  c0  ,c1 ,4)
#define RCP15_ID_MMFR1(reg) MRC15(reg,0,  c0  ,c1 ,5)
#define RCP15_ID_MMFR2(reg) MRC15(reg,0,  c0  ,c1 ,6)
#define RCP15_ID_MMFR3(reg) MRC15(reg,0,  c0  ,c1 ,7)
#define RCP15_ID_ISAR0(reg) MRC15(reg,0,  c0  ,c2 ,0)
#define RCP15_ID_ISAR1(reg) MRC15(reg,0,  c0  ,c2 ,1)
#define RCP15_ID_ISAR2(reg) MRC15(reg,0,  c0  ,c2 ,2)
#define RCP15_ID_ISAR3(reg) MRC15(reg,0,  c0  ,c2 ,3)
#define RCP15_ID_ISAR4(reg) MRC15(reg,0,  c0  ,c2 ,4)
#define RCP15_ID_ISAR5(reg) MRC15(reg,0,  c0  ,c2 ,5)
#define RCP15_SCTLR(reg)  MRC15(reg,0,  c1  ,c0 ,0)
#define RCP15_ACTLR(reg)  MRC15(reg,0,  c1  ,c0 ,1)
#define RCP15_CPACR(reg)  MRC15(reg,0,  c1  ,c0 ,2)
#define RCP15_SCR(reg)    MRC15(reg,0,  c1  ,c1 ,0)
#define RCP15_SDER(reg)   MRC15(reg,0,  c1  ,c1 ,1)
#define RCP15_NSACR(reg)  MRC15(reg,0,  c1  ,c1 ,2)
#define RCP15_TTBR0(reg)  MRC15(reg,0,  c2  ,c0 ,0)
#define RCP15_TTBR1(reg)  MRC15(reg,0,  c2  ,c0 ,1)
#define RCP15_TTBCR(reg)  MRC15(reg,0,  c2  ,c0 ,2)
#define RCP15_DACR(reg)   MRC15(reg,0,  c3  ,c0 ,0)
#define RCP15_DFSR(reg)   MRC15(reg,0,  c5  ,c0 ,0)
#define RCP15_IFSR(reg)   MRC15(reg,0,  c5  ,c0 ,1)
#define RCP15_ADFSR(reg)  MRC15(reg,0,  c5  ,c1 ,0)
#define RCP15_AIFSR(reg)  MRC15(reg,0,  c5  ,c1 ,1)
#define RCP15_DFAR(reg)   MRC15(reg,0,  c6  ,c0 ,0)
#define RCP15_IFAR(reg)   MRC15(reg,0,  c6  ,c0 ,2)
#define RCP15_PAR(reg)    MRC15(reg,0,  c7  ,c4 ,0)
#define RCP15_ICIALLU(reg)  MRC15(reg,0,  c7  ,c5 ,0)
#define RCP15_ICIMVAU(reg)  MRC15(reg,0,  c7  ,c5 ,1)
#define RCP15_PFF(reg)    MRC15(reg,0,  c7  ,c5 ,4)
#define RCP15_BPIALL(reg) MRC15(reg,0,  c7  ,c5 ,6)
#define RCP15_BPIMVA(reg) MRC15(reg,0,  c7  ,c5 ,7)
#define RCP15_DCIMVAC(reg)  MRC15(reg,0,  c7  ,c6 ,1)
#define RCP15_DCISW(reg)  MRC15(reg,0,  c7  ,c6 ,2)
#define RCP15_V2PCWPR(reg)  MRC15(reg,0,  c7  ,c8 ,0)
#define RCP15_V2PCWPW(reg)  MRC15(reg,0,  c7  ,c8 ,1)
#define RCP15_V2PCWUR(reg)  MRC15(reg,0,  c7  ,c8 ,2)
#define RCP15_V2PCWUW(reg)  MRC15(reg,0,  c7  ,c8 ,3)
#define RCP15_V2POWPR(reg)  MRC15(reg,0,  c7  ,c8 ,4)
#define RCP15_V2POWPW(reg)  MRC15(reg,0,  c7  ,c8 ,5)
#define RCP15_V2POWUR(reg)  MRC15(reg,0,  c7  ,c8 ,6)
#define RCP15_V2POWUW(reg)  MRC15(reg,0,  c7  ,c8 ,7)
#define RCP15_DCCMVAC(reg)  MRC15(reg,0,  c7  ,c10  ,1)
#define RCP15_DCCSW(reg)  MRC15(reg,0,  c7  ,c10  ,2)
#define RCP15_DSB(reg)    MRC15(reg,0,  c7  ,c10  ,4)
#define RCP15_DMB(reg)    MRC15(reg,0,  c7  ,c10  ,5)
#define RCP15_DCCMVAU(reg)  MRC15(reg,0,  c7  ,c11  ,1)
#define RCP15_DCCIMVAC(reg) MRC15(reg,0,  c7  ,c14  ,1)
#define RCP15_DCCISW(reg) MRC15(reg,0,  c7  ,c14  ,2)
#define RCP15_ITLBIALL(reg) MRC15(reg,0,  c8  ,c5 ,0)
#define RCP15_ITLBIMVA(reg) MRC15(reg,0,  c8  ,c5 ,1)
#define RCP15_ITLBIASID(reg)  MRC15(reg,0,  c8  ,c5 ,2)
#define RCP15_DTLBIALL(reg) MRC15(reg,0,  c8  ,c6 ,0)
#define RCP15_DTLBIMVA(reg) MRC15(reg,0,  c8  ,c6 ,1)
#define RCP15_DTLBIASID(reg)  MRC15(reg,0,  c8  ,c6 ,2)
#define RCP15_UTLBIALL(reg) MRC15(reg,0,  c8  ,c7 ,0)
#define RCP15_UTLBIMVA(reg) MRC15(reg,0,  c8  ,c7 ,1)
#define RCP15_UTLBIASID(reg)  MRC15(reg,0,  c8  ,c7 ,2)
#define RCP15_ICPLILK(reg)  MRC15(reg,0,  c9  ,c0 ,0)
#define RCP15_ICRSW(reg)  MRC15(reg,0,  c9  ,c0 ,1)
#define RCP15_ICISW(reg)  MRC15(reg,0,  c9  ,c0 ,2)
#define RCP15_DCPLDLK(reg)  MRC15(reg,0,  c9  ,c0 ,4)
#define RCP15_DCRSW(reg)  MRC15(reg,0,  c9  ,c0 ,5)
#define RCP15_DCIALL(reg) MRC15(reg,0,  c9  ,c0 ,6)
#define RCP15_ICRTR0(reg) MRC15(reg,1,  c9  ,c0 ,0)
#define RCP15_ICRTR1(reg) MRC15(reg,1,  c9  ,c0 ,1)
#define RCP15_ICRDR(reg)  MRC15(reg,1,  c9  ,c0 ,2)
#define RCP15_ICLKCR(reg) MRC15(reg,1,  c9  ,c0 ,3)
#define RCP15_DCRTR0(reg) MRC15(reg,1,  c9  ,c0 ,4)
#define RCP15_DCRTR1(reg) MRC15(reg,1,  c9  ,c0 ,5)
#define RCP15_DCRDR(reg)  MRC15(reg,1,  c9  ,c0 ,6)
#define RCP15_DCLKCR(reg) MRC15(reg,1,  c9  ,c0 ,7)

#define RCP15_ICVIC0(reg)   MRC15(reg,0 ,c9 ,c1 ,0)
#define RCP15_ICVIC1(reg)   MRC15(reg,0 ,c9 ,c1 ,1)
#define RCP15_ICVIC2(reg)   MRC15(reg,0 ,c9 ,c1 ,2)
#define RCP15_ICVIC3(reg)   MRC15(reg,0 ,c9 ,c1 ,3)
#define RCP15_ICVIC4(reg)   MRC15(reg,0 ,c9 ,c1 ,4)
#define RCP15_ICVIC5(reg)   MRC15(reg,0 ,c9 ,c1 ,5)
#define RCP15_ICVIC6(reg)   MRC15(reg,0 ,c9 ,c1 ,6)
#define RCP15_ICVIC7(reg)   MRC15(reg,0 ,c9 ,c1 ,7)

#define RCP15_ICFLOOR0(reg)   MRC15(reg,1 ,c9 ,c1 ,0)
#define RCP15_ICFLOOR1(reg)   MRC15(reg,1 ,c9 ,c1 ,1)
#define RCP15_ICFLOOR2(reg)   MRC15(reg,1 ,c9 ,c1 ,2)
#define RCP15_ICFLOOR3(reg)   MRC15(reg,1 ,c9 ,c1 ,3)
#define RCP15_ICFLOOR4(reg)   MRC15(reg,1 ,c9 ,c1 ,4)
#define RCP15_ICFLOOR5(reg)   MRC15(reg,1 ,c9 ,c1 ,5)
#define RCP15_ICFLOOR6(reg)   MRC15(reg,1 ,c9 ,c1 ,6)
#define RCP15_ICFLOOR7(reg)   MRC15(reg,1 ,c9 ,c1 ,7)

#define RCP15_DCVIC0(reg)   MRC15(reg,2 ,c9 ,c1 ,0)
#define RCP15_DCVIC1(reg)   MRC15(reg,2 ,c9 ,c1 ,1)
#define RCP15_DCVIC2(reg)   MRC15(reg,2 ,c9 ,c1 ,2)
#define RCP15_DCVIC3(reg)   MRC15(reg,2 ,c9 ,c1 ,3)
#define RCP15_DCVIC4(reg)   MRC15(reg,2 ,c9 ,c1 ,4)
#define RCP15_DCVIC5(reg)   MRC15(reg,2 ,c9 ,c1 ,5)
#define RCP15_DCVIC6(reg)   MRC15(reg,2 ,c9 ,c1 ,6)
#define RCP15_DCVIC7(reg)   MRC15(reg,2 ,c9 ,c1 ,7)

#define RCP15_DCFLOOR0(reg) MRC15(reg,3,  c9  ,c1 ,0)
#define RCP15_DCFLOOR1(reg) MRC15(reg,3,  c9  ,c1 ,1)
#define RCP15_DCFLOOR2(reg) MRC15(reg,3,  c9  ,c1 ,2)
#define RCP15_DCFLOOR3(reg) MRC15(reg,3,  c9  ,c1 ,3)
#define RCP15_DCFLOOR4(reg) MRC15(reg,3,  c9  ,c1 ,4)
#define RCP15_DCFLOOR5(reg) MRC15(reg,3,  c9  ,c1 ,5)
#define RCP15_DCFLOOR6(reg) MRC15(reg,3,  c9  ,c1 ,6)
#define RCP15_DCFLOOR7(reg) MRC15(reg,3,  c9  ,c1 ,7)

#define RCP15_BTACRSW(reg)  MRC15(reg,0,  c9  ,c2 ,0)
#define RCP15_BTACWSW(reg)  MRC15(reg,0,  c9  ,c2 ,1)
#define RCP15_BHTRSW(reg) MRC15(reg,0,  c9  ,c2 ,2)
#define RCP15_BHTWSW(reg) MRC15(reg,0,  c9  ,c2 ,3)
#define RCP15_BPTR0(reg)  MRC15(reg,1,  c9  ,c2 ,0)
#define RCP15_BPTR1(reg)  MRC15(reg,1,  c9  ,c2 ,1)
#define RCP15_BPTR2(reg)  MRC15(reg,1,  c9  ,c2 ,2)
#define RCP15_L2DCRTR0(reg) MRC15(reg,0,  c9  ,c5 ,0)
#define RCP15_L2DCRTR1(reg) MRC15(reg,0,  c9  ,c5 ,1)
#define RCP15_L2DCRDR0(reg) MRC15(reg,0,  c9  ,c5 ,2)
#define RCP15_L2DCRDR1(reg) MRC15(reg,0,  c9  ,c5 ,3)
#define RCP15_TCMSELR(reg)  MRC15(reg,0,  c9  ,c6 ,0)
#define RCP15_TCMBNSACR(reg)  MRC15(reg,0,  c9  ,c6 ,1)
#define RCP15_TCMBBR(reg) MRC15(reg,0,  c9  ,c6 ,2)
#define RCP15_TCMBRAR(reg)  MRC15(reg,0,  c9  ,c6 ,3)
#define RCP15_SLVCR(reg)  MRC15(reg,0,  c9  ,c7 ,0)
#define RCP15_SLVESR(reg) MRC15(reg,1,  c9  ,c7 ,0)
#define RCP15_SLVEAR(reg) MRC15(reg,1,  c9  ,c7 ,1)
#define RCP15_SLVESYNR(reg) MRC15(reg,1,  c9  ,c7 ,2)
#define RCP15_SLVMPUL0IDR(reg)  MRC15(reg,2,  c9  ,c7 ,0)
#define RCP15_SLVMPUL1IDR(reg)  MRC15(reg,2,  c9  ,c7 ,1)
#define RCP15_SLVMPUL2IDR(reg)  MRC15(reg,2,  c9  ,c7 ,2)
#define RCP15_SLVMPUL3IDR(reg)  MRC15(reg,2,  c9  ,c7 ,3)
#define RCP15_SLVMPUSELR(reg) MRC15(reg,3,  c9  ,c7 ,0)
#define RCP15_SLVMPURPAPR(reg)  MRC15(reg,3,  c9  ,c7 ,1)
#define RCP15_ANSACR(reg) MRC15(reg,7,  c15 ,c0 ,0)
#define RCP15_IEFSR(reg)  MRC15(reg,0,  c15 ,c0 ,1)
#define RCP15_VBAR(reg)   MRC15(reg,0,  c12 ,c0 ,0)
#define RCP15_PRRR(reg)   MRC15(reg,0,  c10 ,c2 ,0)
#define RCP15_NMRR(reg)   MRC15(reg,0,  c10 ,c2 ,1)

#define RCP15_L2CPUCPDR(reg)  MRC15(reg, 3, c15 ,c0 ,7)

#define RCP15_TPIDRPRW(reg)   MRC15(reg,0,  c13 ,c0 ,4)
#define RCP15_TPIDRURW(reg)   MRC15(reg,0,  c13 ,c0 ,2)


/*
 * Scorpion 13.0 Changes
 */
#define RCP15_MMUDMTR(reg)  MRC15(reg,0,  c10 ,c4 ,0)
#define RCP15_MMUDCPR(reg)  MRC15(reg,0,  c10 ,c4 ,1)
#define RCP15_PVR0F0(reg)   MRC15(reg,0,  c15 ,c15  ,0)
#define RCP15_PVR0F1(reg)   MRC15(reg,0,  c15 ,c15  ,1)
#define RCP15_PVR0F2(reg)   MRC15(reg,0,  c15 ,c15  ,2)
#define RCP15_PVR1F0(reg)   MRC15(reg,1,  c15 ,c15  ,0)

#define RCP15_LPM1EVTYPER(reg)  MRC15(reg,1,  c15 ,c0 ,0)
#define RCP15_LPM2EVTYPER(reg)  MRC15(reg,2,  c15 ,c0 ,0)
#define RCP15_L2LPMEVTYPER(reg) MRC15(reg,3,  c15 ,c2 ,0)
#define RCP15_ANSARC(reg) MRC15(reg,7,  c15 ,c0 ,0)
#define RCP15_EFSR(reg)   MRC15(reg,7,  c15 ,c0 ,1)
#define RCP15_BPCR(reg)   MRC15(reg,7,  c15 ,c0 ,2)
#define RCP15_VIRQCPTR(reg) MRC15(reg,7,  c15 ,c0 ,3)
#define RCP15_VFRQCPTR(reg) MRC15(reg,7,  c15 ,c0 ,4)
#define RCP15_CPMR(reg)   MRC15(reg,7,  c15 ,c0 ,5)
#define RCP15_DMACHNSACR(reg) MRC15(reg, 0, c11, c0, 1)


/*
 * Performance Monitor Registers
 */
#define RCP15_PMACTLR(reg)    MRC15(reg,0,  c9  ,c15  ,5)
#define RCP15_PMCCNTCR(reg)   MRC15(reg,0,  c9  ,c15  ,2)
#define RCP15_PMCCNTR(reg)    MRC15(reg,0,  c9  ,c13  ,0)
#define RCP15_PMCCNTSR(reg)   MRC15(reg,0,  c9  ,c13  ,3)
#define RCP15_PMCNTENCLR(reg)   MRC15(reg,0,  c9  ,c12  ,2)
#define RCP15_PMCNTENSET(reg)   MRC15(reg,0,  c9  ,c12  ,1)
#define RCP15_PMCR(reg)     MRC15(reg,0,  c9  ,c12  ,0)
#define RCP15_PMINTENCLR(reg)   MRC15(reg,0,  c9  ,c14  ,2)
#define RCP15_PMINTENSET(reg)   MRC15(reg,0,  c9  ,c14  ,1)
#define RCP15_PMOVSR(reg)   MRC15(reg,0,  c9  ,c12  ,3)
#define RCP15_PMRLDR(reg)   MRC15(reg,0,  c9  ,c15  ,4)
#define RCP15_PMSELR(reg)   MRC15(reg,0,  c9  ,c12  ,5)
#define RCP15_PMSWINC(reg)    MRC15(reg,0,  c9  ,c12  ,4)
#define RCP15_PMUSERENR(reg)    MRC15(reg,0,  c9  ,c14  ,0)
#define RCP15_PMXEVCNTCR(reg)   MRC15(reg,0,  c9  ,c15  ,0)
#define RCP15_PMXEVCNTR(reg)    MRC15(reg,0,  c9  ,c13  ,2)
#define RCP15_PMXEVCNTSR(reg)   MRC15(reg,0,  c9  ,c15  ,1)
#define RCP15_PMXEVTYPER(reg)   MRC15(reg,0,  c9  ,c13  ,1)
#define RCP15_LPM0EVTYPER(reg)    MRC15(reg,0,  c15 ,c0 ,0)
#define RCP15_LPM1EVTYPER(reg)    MRC15(reg,1,  c15 ,c0 ,0)
#define RCP15_LPM2EVTYPER(reg)    MRC15(reg,2,  c15 ,c0 ,0)
#define RCP15_LPM3EVTYPER(reg)    MRC15(reg,3,  c15 ,c0 ,0)
#define RCP15_L2LPMEVTYPER(reg)   MRC15(reg,3,  c15 ,c2 ,0)
#define RCP15_VLPMEVTYPER(reg)    MRC15(reg,7,  c11 ,c0 ,0)
#define RCP15_L2NSACR(reg)  MRC15(reg, 3, c15, c0, 0)
#define RCP15_L2CR0(reg)    MRC15(reg,3,  c15 ,c0 ,1)
#define RCP15_L2CR1(reg)    MRC15(reg,3,  c15 ,c0 ,3)

/* Inline assembler has been deprecated starting from RVCT4.x. Instead of
 * inline assembler macros, add C functions for each CP15 register read and
 * write. */

/** CSSELR argument for selecting L1 d-cache. CSSELR[LEVEL]=0 selects L1 cache,
 * CSSELR[IND]=0 selects d-cache. */
#define CSSELR_L1_DCACHE          0x00000000

/** CSSELR argument for selecting L1 i-cache. CSSELR[LEVEL]=0 selects L1 cache,
 * CSSELR[IND]=1 selects i-cache. */
#define CSSELR_L1_ICACHE          0x00000001

/** CSSELR argument for selecting L2 d-cache (L2 is a unified cache).
 * CSSELR[LEVEL]=1 selects L2 cache, CSSELR[IND]=0 selects d-cache. */
#define CSSELR_L2_DCACHE          0x00000002

void wcp15_csselr(uint32 val);

uint32 rcp15_ccsidr(void);

/**
 * Queries the number of words (32bit) per cache line.
 *
 * @param [in] select   Argument to CSSELR.
 *
 * @return Number of words in a cache line.
 *
 * @pre Interrupts must be disabled.
 */
static inline uint32 rcp15_cache_nwords(uint32 select)
{
  wcp15_csselr(select);
  /* Number of words is CSSIDR bits [2:0]. */
  return 1 << ((rcp15_ccsidr() & 0x00000007) + 2);
}

/**
 * Queries the number of sets in a cache.
 *
 * @param [in] select   Argument to CSSELR.
 *
 * @return Number of sets in a cache.
 *
 * @pre Interrupts must be disabled.
 */
static inline uint32 rcp15_cache_nsets(uint32 select)
{
  wcp15_csselr(select);
  /* Number of sets is CCSIDR bits [27:13]. */
  return ((rcp15_ccsidr() & 0x0FFFE000) >> 13) + 1;
}

/**
 * Queriest the number of ways in a cache.
 *
 * @param [in] select   Argument to CSSELR.
 *
 * @return Number of ways in a cache.
 *
 * @pre Interrupts must be disabled.
 */
static inline uint32 rcp15_cache_nways(uint32 select)
{
  wcp15_csselr(select);
  /* Number of ways is CSSIDR bits [12:3]. */
  return ((rcp15_ccsidr() & 0x00001FF8) >> 3) + 1;
}

#endif /* TZBSP_CP15_H */
