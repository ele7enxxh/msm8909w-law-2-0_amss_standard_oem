#ifndef SMMUHAL_H
#define SMMUHAL_H
/*=============================================================================

                       S M M U  H A L  L I B R A R Y

FILE:      HALSMMU.h
   System MMU Hardware Abstraction Layer (HAL)

GENERAL DESCRIPTION
  This file contains data structures needed internall by the HAL SMMU library

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2010 - 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/HALSMMU.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/10   kcr     Initial Version
=============================================================================*/

/*-----------------------------------------------------------------------------
 * Include Files
 *---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
#include "SMMUTypes.h"
#ifdef TZBSP_WITH_TBASE
#include "tzbsp_target.h"
#endif

/*-----------------------------------------------------------------------------
 * Definitions
 *---------------------------------------------------------------------------*/
#define SMMU_HAL_GLOBAL_REGION0_OFFSET 0x0
#define SMMU_HAL_GLOBAL_REGION1_OFFSET 0x1000
#define SMMU_HAL_GLOBAL_REGION2_OFFSET 0x2000
#define SMMU_HAL_GLOBAL_REGION3_OFFSET 0x3000
#define SMMU_HAL_GLOBAL_REGION4_OFFSET 0x4000
//#define SMMU_HAL_CTX_BANK0_OFFSET      0x8000

#define SMMU_HAL_CTX_BANK_SIZE         0x00001000

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 0 
 *---------------------------------------------------------------------------*/
#define SMMU_HAL_CR0_OFFSET           0x00000000
#define SMMU_HAL_SCR1_OFFSET          0x00000004
#define SMMU_HAL_CR2_OFFSET           0x00000008
#define SMMU_HAL_ACR_OFFSET           0x00000010
#define SMMU_HAL_IDR0_OFFSET          0x00000020
#define SMMU_HAL_IDR1_OFFSET          0x00000024
#define SMMU_HAL_GFAR0_OFFSET         0x00000040
#define SMMU_HAL_GFAR1_OFFSET         0x00000044
#define SMMU_HAL_GFSR_OFFSET          0x00000048
#define SMMU_HAL_GFSRRESTORE_OFFSET   0x0000004C
#define SMMU_HAL_GFSYNR0_OFFSET       0x00000050
#define SMMU_HAL_GFSYNR1_OFFSET       0x00000054
#define SMMU_HAL_GFSYNR2_OFFSET       0x00000058
#define SMMU_HAL_TLBIALL_OFFSET       0x00000060
#define SMMU_HAL_TLBIVMID_OFFSET      0x00000064
#define SMMU_HAL_TLBIALLNSNH_OFFSET   0x00000068
#define SMMU_HAL_TLBGSYNC_OFFSET      0x00000070
#define SMMU_HAL_TLBGSTATUS_OFFSET    0x00000074
#define SMMU_HAL_NSCR0_OFFSET         0x00000400
#define SMMU_HAL_NSCR2_OFFSET         0x00000408
#define SMMU_HAL_NSACR_OFFSET         0x00000410
#define SMMU_HAL_NSIDR0_OFFSET        0x00000420
#define SMMU_HAL_NSIDR1_OFFSET        0x00000424
#define SMMU_HAL_NSGFAR0_OFFSET       0x00000440
#define SMMU_HAL_NSGFAR1_OFFSET       0x00000444
#define SMMU_HAL_NSGFSR_OFFSET        0x00000448
#define SMMU_HAL_NSGFSRRESTORE_OFFSET 0x0000044C
#define SMMU_HAL_NSGFSYNR0_OFFSET     0x00000450
#define SMMU_HAL_NSGFSYNR1_OFFSET     0x00000454
#define SMMU_HAL_NSGFSYNR2_OFFSET     0x00000458
#define SMMU_HAL_NSTLBIALL_OFFSET     0x00000460
#define SMMU_HAL_NSTLBIALLNSNH_OFFSET 0x00000468
#define SMMU_HAL_NSTLBGSYNC_OFFSET    0x00000470
#define SMMU_HAL_NSTLBGSTATUS_OFFSET  0x00000474
#define SMMU_HAL_SMR0_OFFSET          0x00000800
#define SMMU_HAL_S2CR0_OFFSET         0x00000C00

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 1 
 *---------------------------------------------------------------------------*/
#define SMMU_HAL_CBAR0_OFFSET         0x00000000
#define SMMU_HAL_CBFRSYNRA0_OFFSET    0x00000400
#define SMMU_HAL_CBA2R0_OFFSET        0x00000800

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 2 
 *---------------------------------------------------------------------------*/
/* SSID and FLOW specific */      
#define SMMU_HAL_AS2CR0_OFFSET        0x00000C00

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 3
 *---------------------------------------------------------------------------*/
/*Performance Monitors. TBD*/   

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 4 
 *---------------------------------------------------------------------------*/
#define SMMU_HAL_SSDR0_OFFSET         0x00000000

/*-----------------------------------------------------------------------------
 * Register Offsets inside the context bank regions 
 *---------------------------------------------------------------------------*/
#define SMMU_HAL_CBN_SCTLR_OFFSET          0x00000000
#define SMMU_HAL_CBN_ACTLR_OFFSET          0x00000004
#define SMMU_HAL_CBN_FSR_OFFSET            0x00000058
#define SMMU_HAL_CBN_FSR_RESTORE_OFFSET    0x0000005C
#define SMMU_HAL_CBN_FAR0_OFFSET           0x00000060
#define SMMU_HAL_CBN_FAR1_OFFSET           0x00000064
#define SMMU_HAL_CBN_FSYN0_OFFSET          0x00000068
#define SMMU_HAL_CBN_FSYN1_OFFSET          0x0000006C
#define SMMU_HAL_CBN_TTBR0_0_OFFSET        0x00000020
#define SMMU_HAL_CBN_TTBR0_1_OFFSET        0x00000024
#define SMMU_HAL_CBN_TTBR1_0_OFFSET        0x00000028
#define SMMU_HAL_CBN_TTBR1_1_OFFSET        0x0000002C
#define SMMU_HAL_CBN_TTBCR_OFFSET          0x00000030
#define SMMU_HAL_CBN_TCR2_OFFSET           0x00000010
#define SMMU_HAL_CBN_CONTEXTIDR_OFFSET     0x00000034
#define SMMU_HAL_CBN_TLBIALL_OFFSET        0x00000618
#define SMMU_HAL_CBN_TLBSYNC_OFFSET        0x000007F0
#define SMMU_HAL_CBN_TLBSTATUS_OFFSET      0x000007F4
#define SMMU_HAL_CBN_PAR0_OFFSET           0x00000050
#define SMMU_HAL_CBN_PAR1_OFFSET           0x00000054
#define SMMU_HAL_CBN_PRRR_OFFSET           0x00000038
#define SMMU_HAL_CBN_NMRR_OFFSET           0x0000003C
#define SMMU_HAL_CBN_MAIR0_OFFSET          0x00000038
#define SMMU_HAL_CBN_MAIR1_OFFSET          0x0000003C
#define SMMU_HAL_CBN_IPAFAR0_OFFSET        0x00000070
#define SMMU_HAL_CBN_IPAFAR1_OFFSET        0x00000074


// 32 CB interrupts from APP TCU to secure interrupt
#define SMMU_HAL_APPTCU_INTR_SEL_SEC_OFFSET 0x00000000

// PTW VMID
#define SMMU_HAL_APPTCU_PTW_VMID_SEC_OFFSET 0x00000020
#define SMMU_HAL_GFXTCU_PTW_VMID_SEC_OFFSET 0x00000028

// access GFX TCU registers from private Oxili AHB port
#define SMMU_HAL_GFX_AHB_4K_APERTURE_NSHYP_OFFSET 0x00001008

// SID2VMID_FAULT_VMID offset
#define SMMU_HAL_SID2VMID_FAULT_VMID_OFFSET       0x00000000

// SID2VMID_FAULT_INTR_EN offset
#define SMMU_HAL_SID2VMID_FAULT_INTR_EN_OFFSET    0x00000008

// SID2VMID_FAULT_INTR_STATUS offset
#define SMMU_HAL_SID2VMID_FAULT_INTR_STTS_OFFSET  0x00000010

// SID2VMID_FAULT_INTR_CLR offset
#define SMMU_HAL_SID2VMID_FAULT_INTR_CLR_OFFSET   0x00000018

/*-----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/
typedef union SMMU_HAL_Secure_IDR0_t
{
  struct {
    unsigned NUMSMRG:8;
    unsigned reserved:1;
    unsigned NUMSIDB:4;
    unsigned BTM:1;
    unsigned CTTW:1;
    unsigned reserved2:1;
    unsigned NUMIRPT:8;
    unsigned PTFS:1;
    unsigned reserved3:2;
    unsigned SMS:1;
    unsigned NTS:1;
    unsigned S2TS:1;
    unsigned S1TS:1;
    unsigned SES:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_Secure_IDR0_t;

// SMMU v2 and SMMU 500 have different IDR1 fields
typedef union SMMU_HAL_Secure_IDR1_t
{
  struct {
    unsigned NUMCB:8;
    unsigned NUMSSDNDXB:4;
    unsigned SSDTP:2;
    unsigned reserved:1;
    unsigned SMCD:1;
    unsigned NUMS2CB:8;
    unsigned reserved2:4;
    unsigned NUMPAGENDXB:3;
    unsigned PAGESIZE:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_Secure_IDR1_t;

typedef union SMMU_HAL_CR0_t
{
  struct {
    unsigned CLIENTPD:1;
    unsigned GFRE:1;
    unsigned GFIE:1;
    unsigned reserved:1;
    unsigned GCFGFRE:1;
    unsigned GCFGFIE:1;
    unsigned TRANSIENTCFG:2;
    unsigned STALLD:1;
    unsigned GSE:1;
    unsigned USFCFG:1;
    unsigned VMIDPNE:1;
    unsigned PTM:1;
    unsigned FB:1;
    unsigned BSU:2;
    unsigned MEMATTR:4;
    unsigned MTCFG:1;
    unsigned SMCFCFG:1;
    unsigned SHCFG:2;
    unsigned RACFG:2;
    unsigned WACFG:2;
    unsigned NSCFG:2;
    unsigned reserved2:2;
  } formatted;
  unsigned raw;
} SMMU_HAL_CR0_t;

typedef union SMMU_HAL_CR2_t
{
  struct {
    unsigned BPVMID:8;
    unsigned reserved:24;
  } formatted;
  unsigned raw;
} SMMU_HAL_CR2_t;

// SMMU v2 and SMMU 500 have different ACR fields
typedef union SMMU_HAL_ACR_t
{
  struct {
    unsigned reserved:2;
    unsigned S1WC2EN:1;
    unsigned S2WC2EN:1; // non-secure
    unsigned IPA2PA_CACHEEN:1; // non-secure
    unsigned reserved1:3;
    unsigned SMTNMC_BPTLBEN:1;
    unsigned MMUDIS_BPTLBEN:1;
    unsigned S2CR_BPTLBEN:1;
    unsigned reserved2:5;
    unsigned PAGESIZE:1; // secure
    unsigned reserved3:7;
    unsigned KB_DEPENDTCU:1; // non-secure
    unsigned KB_DEPENDTBU:1; // non-secure
    unsigned LOCK_ACTLR:1;  //no-secure
    unsigned reserved4:5;
  } formatted;
  unsigned raw;
} SMMU_HAL_ACR_t;

typedef union SMMU_HAL_SCR1_t
{
  struct {
    unsigned NSNUMCBO:8;
    unsigned NSNUMSMRGO:8;
    unsigned NSNUMIRPTO:8;
    unsigned GASRAE:1;
    unsigned GEFRO:1;
    unsigned SIF:1;
    unsigned SPMEN:1;
    unsigned reserved:4;
  } formatted;
  unsigned raw;
} SMMU_HAL_SCR1_t;

typedef struct SMMU_HAL_GFSR_t
{
  unsigned raw;
} SMMU_HAL_GFSR_t;

typedef struct SMMU_HAL_NSGFSR_t
{
  unsigned raw;
} SMMU_HAL_NSGFSR_t;

typedef union SMMU_HAL_CBn_SCTLR_t
{
  struct {
    union {
      struct {
        unsigned M:1;
        unsigned TRE:1;
        unsigned AFE:1;
        unsigned AFFD:1;
        unsigned E:1;
        unsigned CFRE:1;
        unsigned CFIE:1;
        unsigned CFCFG:1;
        unsigned HUPCF:1;
        unsigned WXN:1;
        unsigned UWXN:1;
        unsigned reserved:1;
        unsigned ASIDPNE:1;
        unsigned reserved2:1;
        unsigned TRANSIENTCFG:2;
        unsigned MEMATTR:4;
        unsigned MTCFG:1;
        unsigned reserved3:1;
        unsigned SHCFG:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned NSCFG:2;
        unsigned reserved4:2;
      } s1;
      struct {
        unsigned M:1;
        unsigned TRE:1;
        unsigned AFE:1;
        unsigned AFFD:1;
        unsigned E:1;
        unsigned CFRE:1;
        unsigned CFIE:1;
        unsigned CFCFG:1;
        unsigned HUPCF:1;
        unsigned reserved1:4;
        unsigned PTW:1;
        unsigned BSU:2;
        unsigned MEMATTR:4;
        unsigned reserved2:1;
        unsigned FB:1;
        unsigned SHCFG:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned reserved3:4;
      } s2;
    } translation_stage;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBn_SCTLR_t;

typedef union SMMU_HAL_CBARn_t
{
  struct {
    union {
      // stage 1 context with stage 2 bypass
      struct {
        unsigned VMID:8;
        unsigned BPSHCFG:2;
        unsigned HYPC:1;
        unsigned FB:1;
        unsigned MEMATTR:4;
        unsigned TYPE:2;
        unsigned BSU:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned IRPTNDX:8;
      } type_s1;
      // stage 2 context
      struct {
        unsigned VMID:8;
        unsigned reserved1:8;
        unsigned TYPE:2;
        unsigned SBZ:2;
        unsigned reserved2:4;
        unsigned IRPTNDX:8;
      } type_s2;
      // stage 1 context with stage 2 context (nested)
      struct {
        unsigned VMID:8;
        unsigned CBNDX:8;
        unsigned TYPE:2;
        unsigned SBZ:2;
        unsigned reserved2:4;
        unsigned IRPTNDX:8;
      } type_s2_nested;
    } cbar_type;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBARn_t;

typedef union SMMU_HAL_CBA2Rn_t
{
  struct {
    unsigned VA64:1;
    unsigned MONC:1;
    unsigned reserved:30;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBA2Rn_t;

typedef union SMMU_HAL_S2CRn_t
{
  struct {
    unsigned FIELD:8;
    unsigned SHCFG:2;
    unsigned reserved:1;
    unsigned MTCFG:1;
    unsigned MEMATTR:4;
    unsigned TYPE:2;
    unsigned NSCFG:2;
    unsigned RACFG:2;
    unsigned WACFG:2;
    unsigned PRIVCFG:2;
    unsigned INSTCFG:2;
    unsigned TRANSIENTCFG:2;
    unsigned reserved2:2;
  } formatted;
  unsigned raw;
} SMMU_HAL_S2CRn_t;

typedef union SMMU_HAL_AS2CRn_t
{
  struct {
    unsigned REQPRIORITY:2;
    unsigned reserved:2;
    unsigned REQPRIORITYCFG:1;
    unsigned reserved1:23;
    unsigned BPRCOSH:1;
    unsigned BPRCISH:1;
    unsigned BPRCNSH:1;
    unsigned reserved2:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_AS2CRn_t;

typedef union SMMU_HAL_SMRn_t
{
  struct {
    unsigned ID:15;
    unsigned reserved:1;
    unsigned MASK:15;
    unsigned VALID:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_SMRn_t;

typedef union SMMU_HAL_CBn_FSR_t
{
  struct {
    unsigned reserved:1;
    unsigned TF:1;
    unsigned AFF:1;
    unsigned PF:1;
    unsigned EF:1;
    unsigned TLBMCF:1;
    unsigned TLBLKF:1;
    unsigned reserved2:23;
    unsigned SS:1;
    unsigned MULTI:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBn_FSR_t;

typedef union SMMU_HAL_CBn_TTBCR_t
{
  struct {
    union {
      // ARMv7 VMSA
      struct {
        unsigned T0SZ:3;
        unsigned SBZ:1;
        unsigned PD0:1;
        unsigned PD1:1;
        unsigned reserved:8;
        unsigned NSCFG0:1;
        unsigned reserved2:15;
        unsigned NSCFG1:1;
        unsigned EAE:1;
      } eae_0;
      // LPAE
      struct {
        union {
          struct {
            unsigned T0SZ:3;
            unsigned reserved1:4;
            unsigned EPD0:1;
            unsigned IRGN0:2;
            unsigned ORGN0:2;
            unsigned SH0:2;
            unsigned NSCFG0:1;
            unsigned reserved2:1;
            unsigned T1SZ:3;
            unsigned reserved3:3;
            unsigned A1:1;
            unsigned EPD1:1;
            unsigned IRGN1:2;
            unsigned ORGN1:2;
            unsigned SH1:2;
            unsigned NSCFG1:1;
            unsigned EAE:1;
          } s1;
          // S2 SMMU_CBn_TCR
          struct {
            unsigned T0SZ:4;
            unsigned reserved1:2;
            unsigned SL0:2;
            unsigned IRGN0:2;
            unsigned ORGN0:2;
            unsigned SH0:2;
            unsigned reserved2:17;
            unsigned EAE:1;
          } s2;
        } translation_stage;
      } eae_1;
    } eae_state;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBn_TTBCR_t;

typedef union SMMU_HAL_CBn_TCR2_t
{
  struct 
  {
    unsigned PASIZE:3;
    unsigned reserved1:1;
    unsigned AS:1;
    unsigned TBI0:1;
    unsigned TBI1:1;
    unsigned reserved2:7;
    unsigned NSCFG0:1;
    unsigned SEP:3;
    unsigned reserved3:12;
    unsigned NSCFG1:1;
    unsigned reserved4:1;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBn_TCR2_t;

typedef union SMMU_HAL_CBn_ACTLR_t
{
  struct {
    unsigned TLB_CACHE_EN:1;
    unsigned IPFT_CACHE_EN:1;
    unsigned reserved:30;
  } formatted;
  unsigned raw;
} SMMU_HAL_CBn_ACTLR_t;

/**
* SMMU API Handle definition
*/

/* Global context bank HAL APIs */

SMMUResult_t   SMMUHAL_GetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_SCTLR_t *sctlr);
SMMUResult_t   SMMUHAL_SetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_SCTLR_t sctlr);
SMMUResult_t   SMMUHAL_GetFSR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_FSR_t *fsr);
SMMUResult_t   SMMUHAL_SetFSRRestore(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_FSR_t fsr);
SMMUResult_t   SMMUHAL_GetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far0);
SMMUResult_t   SMMUHAL_GetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far1);
SMMUResult_t   SMMUHAL_SetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far0);
SMMUResult_t   SMMUHAL_SetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far1);
SMMUResult_t   SMMUHAL_GetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn0);
SMMUResult_t   SMMUHAL_GetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn1);
SMMUResult_t   SMMUHAL_SetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn0);
SMMUResult_t   SMMUHAL_SetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn1);
SMMUResult_t   SMMUHAL_GetIPAFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ipafar);
SMMUResult_t   SMMUHAL_SetIPAFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ipafar);
SMMUResult_t   SMMUHAL_GetPAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par0);
SMMUResult_t   SMMUHAL_GetPAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par1);
SMMUResult_t   SMMUHAL_GetPRRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *prrr);
SMMUResult_t   SMMUHAL_GetNMRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *nmrr);
SMMUResult_t   SMMUHAL_SetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair0);
SMMUResult_t   SMMUHAL_SetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair1);
SMMUResult_t   SMMUHAL_GetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair0);
SMMUResult_t   SMMUHAL_GetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair1);
SMMUResult_t   SMMUHAL_SetAsid(uint32 baseVirtAddr, uint32 ctxBankNumber, uint8 asid);
SMMUResult_t   SMMUHAL_SetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr);
SMMUResult_t   SMMUHAL_GetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr);
SMMUResult_t   SMMUHAL_SetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr);
SMMUResult_t   SMMUHAL_GetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr);
SMMUResult_t   SMMUHAL_GetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_TTBCR_t *ttbcr);
SMMUResult_t   SMMUHAL_SetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_TTBCR_t ttbcr);
SMMUResult_t   SMMUHAL_GetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_TCR2_t *tcr2);
SMMUResult_t   SMMUHAL_SetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_TCR2_t tcr2);
SMMUResult_t   SMMUHAL_GetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_ACTLR_t *actlr);
SMMUResult_t   SMMUHAL_SetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, SMMU_HAL_CBn_ACTLR_t actlr);
SMMUResult_t   SMMUHAL_GetCBARn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_CBARn_t *cbar);
SMMUResult_t   SMMUHAL_SetCBARn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_CBARn_t cbar);
SMMUResult_t   SMMUHAL_GetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 *cbfrsynra);
SMMUResult_t   SMMUHAL_SetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 cbfrsynra);
SMMUResult_t   SMMUHAL_GetCBA2Rn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_CBA2Rn_t *cba2r);
SMMUResult_t   SMMUHAL_SetCBA2Rn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_CBA2Rn_t cba2r);
SMMUResult_t   SMMUHAL_GetS2CRn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_S2CRn_t *s2cr);
SMMUResult_t   SMMUHAL_SetS2CRn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_S2CRn_t s2cr);
SMMUResult_t   SMMUHAL_GetAS2CRn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_AS2CRn_t *as2cr);
SMMUResult_t   SMMUHAL_SetAS2CRn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_AS2CRn_t as2cr);
SMMUResult_t   SMMUHAL_SetSMRn(uint32 baseVirtAddr, uint32 index, uint32 id);
SMMUResult_t   SMMUHAL_SetSMRn_raw(uint32 baseVirtAddr, uint32 index, SMMU_HAL_SMRn_t smr);
SMMUResult_t   SMMUHAL_GetSMRn(uint32 baseVirtAddr, uint32 index, SMMU_HAL_SMRn_t *smr);
SMMUResult_t   SMMUHAL_GetSecureIDR0(uint32 baseVirtAddr, SMMU_HAL_Secure_IDR0_t *SecureIdr0);
SMMUResult_t   SMMUHAL_GetSecureIDR1(uint32 baseVirtAddr, SMMU_HAL_Secure_IDR1_t *SecureIdr1);
SMMUResult_t   SMMUHAL_GetSecureCR0(uint32 baseVirtAddr, SMMU_HAL_CR0_t *secureCR0);
SMMUResult_t   SMMUHAL_SetSecureCR0(uint32 baseVirtAddr, SMMU_HAL_CR0_t secureCR0);
SMMUResult_t   SMMUHAL_GetNonSecureCR0(uint32 baseVirtAddr, SMMU_HAL_CR0_t *nonSecureCR0);
SMMUResult_t   SMMUHAL_SetNonSecureCR0(uint32 baseVirtAddr, SMMU_HAL_CR0_t nonSecureCR0);
SMMUResult_t   SMMUHAL_GetSecureACR(uint32 baseVirtAddr, SMMU_HAL_ACR_t *secureACR);
SMMUResult_t   SMMUHAL_SetSecureACR(uint32 baseVirtAddr, SMMU_HAL_ACR_t secureACR);
SMMUResult_t   SMMUHAL_GetNonSecureACR(uint32 baseVirtAddr, SMMU_HAL_ACR_t *nonSecureACR);
SMMUResult_t   SMMUHAL_SetNonSecureACR(uint32 baseVirtAddr, SMMU_HAL_ACR_t nonSecureACR);
SMMUResult_t   SMMUHAL_GetSecureCR2(uint32 baseVirtAddr, SMMU_HAL_CR2_t *secureCR2);
SMMUResult_t   SMMUHAL_SetSecureCR2(uint32 baseVirtAddr, SMMU_HAL_CR2_t secureCR2);
SMMUResult_t   SMMUHAL_GetNonSecureCR2(uint32 baseVirtAddr, SMMU_HAL_CR2_t *nonSecureCR2);
SMMUResult_t   SMMUHAL_SetNonSecureCR2(uint32 baseVirtAddr, SMMU_HAL_CR2_t nonSecureCR2);
SMMUResult_t   SMMUHAL_GetSecureCR1(uint32 baseVirtAddr, SMMU_HAL_SCR1_t *secureCR1);
SMMUResult_t   SMMUHAL_SetSecureCR1(uint32 baseVirtAddr, SMMU_HAL_SCR1_t secureCR1);
SMMUResult_t   SMMUHAL_GetGFAR0(uint32 baseVirtAddr, uint32 *gfAddr0);
SMMUResult_t   SMMUHAL_GetGFAR1(uint32 baseVirtAddr, uint32 *gfAddr1);
SMMUResult_t   SMMUHAL_SetGFAR0(uint32 baseVirtAddr, uint32 gfAddr0);
SMMUResult_t   SMMUHAL_SetGFAR1(uint32 baseVirtAddr, uint32 gfAddr1);
SMMUResult_t   SMMUHAL_GetNSGFAR0(uint32 baseVirtAddr, uint32 *nsgfAddr0);
SMMUResult_t   SMMUHAL_GetNSGFAR1(uint32 baseVirtAddr, uint32 *nsgfAddr1);
SMMUResult_t   SMMUHAL_SetNSGFAR0(uint32 baseVirtAddr, uint32 nsgfAddr0);
SMMUResult_t   SMMUHAL_SetNSGFAR1(uint32 baseVirtAddr, uint32 nsgfAddr1);
SMMUResult_t   SMMUHAL_GetGFSR(uint32 baseVirtAddr, SMMU_HAL_GFSR_t *gfsr);
SMMUResult_t   SMMUHAL_GetNSGFSR(uint32 baseVirtAddr, SMMU_HAL_NSGFSR_t *nsgfsr);
SMMUResult_t   SMMUHAL_SetGFSRRestore(uint32 baseVirtAddr, SMMU_HAL_GFSR_t gfsr);
SMMUResult_t   SMMUHAL_SetNSGFSRRestore(uint32 baseVirtAddr, SMMU_HAL_NSGFSR_t nsgfsr);
SMMUResult_t   SMMUHAL_GetGFSYNR0(uint32 baseVirtAddr, uint32 *gfsynr0);
SMMUResult_t   SMMUHAL_GetGFSYNR1(uint32 baseVirtAddr, uint32 *gfsynr1);
SMMUResult_t   SMMUHAL_GetGFSYNR2(uint32 baseVirtAddr, uint32 *gfsynr2);
SMMUResult_t   SMMUHAL_SetGFSYNR0(uint32 baseVirtAddr, uint32 gfsynr0);
SMMUResult_t   SMMUHAL_SetGFSYNR1(uint32 baseVirtAddr, uint32 gfsynr1);
SMMUResult_t   SMMUHAL_SetGFSYNR2(uint32 baseVirtAddr, uint32 gfsynr2);
SMMUResult_t   SMMUHAL_GetNSGFSYNR0(uint32 baseVirtAddr, uint32 *nsgfsynr0);
SMMUResult_t   SMMUHAL_GetNSGFSYNR1(uint32 baseVirtAddr, uint32 *nsgfsynr1);
SMMUResult_t   SMMUHAL_GetNSGFSYNR2(uint32 baseVirtAddr, uint32 *nsgfsynr2);
SMMUResult_t   SMMUHAL_SetNSGFSYNR0(uint32 baseVirtAddr, uint32 nsgfsynr0);
SMMUResult_t   SMMUHAL_SetNSGFSYNR1(uint32 baseVirtAddr, uint32 nsgfsynr1);
SMMUResult_t   SMMUHAL_SetNSGFSYNR2(uint32 baseVirtAddr, uint32 nsgfsynr2);
SMMUResult_t   SMMUHAL_ChangeSecureIndex(uint32 baseVirtAddr, uint32 index, boolean value);
SMMUResult_t   SMMUHAL_GlobalInvalidateSecureTLBAll(uint32 baseVirtAddr); 
SMMUResult_t   SMMUHAL_GlobalInvalidateNonSecureTLBAll(uint32 baseVirtAddr);
SMMUResult_t   SMMUHAL_GlobalInvalidateNonSecureTLBVMID(uint32 baseVirtAddr, uint32 uVMID);
SMMUResult_t   SMMUHAL_InvalidateCBTLBAll(uint32 baseVirtAddr, uint32 ctxBankNumber);

SMMUResult_t   SMMUHAL_GetIntrSelSec(uint32 baseVirtAddr, uint32 *intr);
SMMUResult_t   SMMUHAL_SetIntrSelSec(uint32 baseVirtAddr, uint32 intr);
SMMUResult_t   SMMUHAL_SetAppTcuPtwVmidSec(uint32 baseVirtAddr, uint32 uVmid);
SMMUResult_t   SMMUHAL_SetGfxTcuPtwVmidSec(uint32 baseVirtAddr, uint32 uVmid);
SMMUResult_t   SMMUHAL_GetGfxAHB4KApertureNSHyp(uint32 baseVirtAddr, uint32 *uAperture);
SMMUResult_t   SMMUHAL_SetGfxAHB4KApertureNSHyp(uint32 baseVirtAddr, uint32 uAperture);
SMMUResult_t   SMMUHAL_GetSID2VMID_SMRn(uint32 baseVirtAddr, uint32 index, uint32 *uSID2Vmid);
SMMUResult_t   SMMUHAL_SetSID2VMID_SMRn(uint32 baseVirtAddr, uint32 index, uint32 uSID2Vmid);
SMMUResult_t   SMMUHAL_SetSID2VMID_FAULT_VMID(uint32 baseVirtAddr, uint32 uMaxSS, uint32 uVmid);
SMMUResult_t   SMMUHAL_SetSID2VMID_FAULT_INTR_EN(uint32 baseVirtAddr, uint32 uMaxSS, boolean bEnable);
SMMUResult_t   SMMUHAL_GetSID2VMID_FAULT_INTR_STATUS(uint32 baseVirtAddr, uint32 uMaxSS, uint32 *uIntrStatus);
SMMUResult_t   SMMUHAL_SetSID2VMID_FAULT_INTR_CLR(uint32 baseVirtAddr, uint32 uMaxSS);

/*-----------------------------------------------------------------------------
 * Inline Helper Functions
 *---------------------------------------------------------------------------*/
static __inline uint32 SMMU_HAL_CTX_BANK0_OFFSET(uint32 baseAddr)
{
  SMMU_HAL_Secure_IDR1_t uIDR1Val = {0};
  uint32 uPageSize = 0;
  uint32 uNumPage = 0;

  (void)SMMUHAL_GetSecureIDR1(baseAddr, &uIDR1Val);
  uPageSize = (uIDR1Val.formatted.PAGESIZE == 0) ? 0x1000 : 0x10000;
  uNumPage = 1 << (uIDR1Val.formatted.NUMPAGENDXB + 1);
  return (uPageSize * uNumPage);
}

#ifdef __cplusplus
}
#endif

#endif /* SMMUHAL_H */
