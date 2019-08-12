/**
@file hyp_asm.h
@brief Hypervisor definitions

This file contains the definition of hypervisor register info 

*/
/*===========================================================================
   Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/inc/hyp_asm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/

#ifndef HYP_ASM_H
#define HYP_ASM_H
#define CPSR_MODE_MASK 0x1F

/* Mode values*/
#define HYP_Mode_ABT                   0x17
#define HYP_Mode_UND                   0x1b
#define HYP_Mode_USR                   0x10
#define HYP_Mode_FIQ                   0x11
#define HYP_Mode_IRQ                   0x12
#define HYP_Mode_SVC                   0x13
#define HYP_Mode_HYP                   0x1A
#define HYP_Mode_SYS                   0x1F

/* CPSE I/F mask bits*/
#define HYP_I_Bit                      0x80
#define HYP_F_Bit                      0x40

#define HYP_T_Bit                      0x20

#define HSR_EC_UNKNOWN       0x00       //Unknown reason – all other fields invalid.
#define HSR_EC_WFEI          0x01       //Trapped WFI/WFE
#define HSR_EC_CP15          0x03       //Trapped MCR/MRC to CP15
#define HSR_EC_CP15x2        0x04       //Trapped MCRR/MRRC to CP15
#define HSR_EC_CP14          0x05       //Trapped MCR/MRC to CP14
#define HSR_EC_CP14x2        0x06       //Trapped LDC/STC to CP14 (DBGDTRTXint /DBGDTRRXint only)
#define HSR_EC_CP0_CP13      0x07       //Trapped Coprocessor Usage to CP0-CP13 or Advanced SIMD
#define HSR_EC_SVC           0x11       //SVC which is taken in Hyp mode.
#define HSR_EC_HVC           0x12       //Trapper HVC
#define HSR_EC_SMC           0x13       //Trapped SMC
#define HSR_EC_PREFETCH      0x20       //Instruction Abort that caused an entry into Hyp mode
#define HSR_EC_PCALIGN       0x22       //PC Alignment Exception
#define HSR_EC_DABORT        0x24       //Data Abort that caused an entry into Hyp mode
#define HSR_EC_MASK          0x3F
#define HSR_EC_SHIFT         26
#define HSR_IL_BIT                     (0x01 << 25)

/* TTBCR */
#define TTBCR_EAE_BIT                  0x80000000

/* IFSR */
#define HYP_IFSR_WHEN_EAE0             0x01
#define HYP_IFSR_WHEN_EAE1             0x21

/*Defines from VM*/
#define PC_OFFSET     0x48
#define USR_LR_OFFSET 0x58

#define SVC_SP_OFFSET 0x60

#define VBAR_OFFSET    0x0
#define SCTLR_OFFSET   0x4
#define TTBCR_OFFSET   0xC
#define TTBR0_0_OFFSET   0x10
#define TTBR0_1_OFFSET   0x14
#define TTBR1_0_OFFSET   0x18
#define TTBR1_1_OFFSET   0x1C
#define MAIR0_OFFSET   0x20
#define MAIR1_OFFSET   0x24

#define LPAE_ENABLE_MASK 0x80000000


/* Error codes for logging only. */
#define HYP_LOG_CORE0_BRINGUP                 0
#define HYP_LOG_CORE1_BRINGUP                 1
#define HYP_LOG_CORE2_BRINGUP                 2
#define HYP_LOG_CORE3_BRINGUP                 3
#define HYP_LOG_CORE4_BRINGUP                 4
#define HYP_LOG_CORE5_BRINGUP                 5
#define HYP_LOG_CORE6_BRINGUP                 6
#define HYP_LOG_CORE7_BRINGUP                 7
#define HYP_LOG_EC_RESET_HANDLER              8
#define HYP_LOG_EC_UND_HANDLER                9
#define HYP_LOG_EC_SVC_HANDLER               10
#define HYP_LOG_EC_PREFETCH_HANDLER          11
#define HYP_LOG_EC_ABORT_HANDLER             12
#define HYP_LOG_EC_FIQ_HANDLER               13
#define HYP_LOG_EC_TRAP_HANDLER              14
#define HYP_LOG_EC_IRQ_HANDLER               15
#endif 
