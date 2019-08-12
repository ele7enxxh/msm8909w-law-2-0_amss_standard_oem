;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;         T R U S T Z O N E   N O N S E C U R E   F I Q   D E B U G
;
;
; GENERAL DESCRIPTION
;   This file contains the Hypervisor init codes
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/src/hyp_entry.s#1 $
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "hyp_asm.h"
#include "hyp_target.h"

;=======================================================================
;
;                             MODULE IMPORTS
;
;=======================================================================
BOOT_STATE_COLD   EQU   0x0
BOOT_STATE_INIT   EQU   0x1
BOOT_STATE_WARM   EQU   0x2

        IMPORT |Image$$HYP_STACK$$ZI$$Limit|
        IMPORT |Image$$HYP_STACK$$ZI$$Base|
        IMPORT |Image$$HYP_IMG$$Base|
        IMPORT  hyp_log_init
        IMPORT  hyp_milestone
        IMPORT  hyp_log_warmboot_entry
        IMPORT  hyp_log_warmboot_exit
        IMPORT  hyp_log_exception
        IMPORT  hyp32_hvc_handler
        IMPORT  hyp_check_if_forward_misaligned_pc
        IMPORT  hyp_config_download_mode_backup
        IMPORT  hyp_log_registers
        IMPORT  hyp_mm_warm_init
        IMPORT  hyp_mm_cold_init
        EXPORT  HYP_SetupMMU 
        EXPORT  __main
        EXPORT  _main
        EXPORT HypervisorVectors

    PRESERVE8
    AREA    HYPENTRYCODE, CODE, READONLY, ALIGN=5
    CODE32

; -----------------------------------------------------------------------------
; MACRO: GetCPUUniqueID
; -----------------------------------------------------------------------------
; derrive an unique ID from cluster+core combo
  MACRO
  GetCPUUniqueID $rx, $ry
    ; Ensure all reads/writes are visible to other observers before
    ; releasing the lock.
    MRC     p15, 0, $rx, c0, c0, 5     ; Read MPIDR 
    AND     $ry, $rx, #0xFF00           ; AFFL1 is the cluster number.
    AND     $rx, $rx, #0xFF             ; AFFL0 is the CPU number.
    ORR     $rx, $ry, LSR #CLUSTER_RSHIFT
  MEND

__main
_main
        ENTRY

HypervisorVectors
  B       Hyp_Reset_Handler      ; For warm/cold boot
  B       Hyp_Undef_Handler      ; Not used now, log it and loop forever
  B       Hyp_SVC_Handler
  B       Hyp_Prefetch_Handler
  B       Hyp_Abort_Handler
  B       Hyp_Trap_Handler  
  B       Hyp_IRQ_Handler
  B       Hyp_FIQ_Handler

; ------------------------------------------------------------
; Handlers for unused exceptions
; ------------------------------------------------------------

Hyp_Undef_Handler
  MOV     r0, #HYP_LOG_EC_UND_HANDLER
  BL      hyp_log_exception
  B       Hyp_handler_loop_forever

Hyp_SVC_Handler
  MOV     r0, #HYP_LOG_EC_SVC_HANDLER
  BL      hyp_log_exception
  B       Hyp_handler_loop_forever

Hyp_Prefetch_Handler
  PUSH    {r0-r3}
  MOV     r0, #HYP_LOG_EC_PREFETCH_HANDLER
  MRS     r1, ELR_hyp
  BL      hyp_log_exception

  ; Implementation of ARM Cortex-A53 erratum 814270 -- begin
  MRS     r0, HSR
  MRS     r1, HIFAR
  MRS     r2, TTBCR
  MRS     r3, SPSR_hyp
  BL      hyp_check_if_forward_misaligned_pc
  CMP     r0, #0
  BEQ     Hyp_handler_loop_forever  ; Leave because not forward 
                                    ; this exception 
  MRS     r1, ELR_hyp
  MSR     lr_abt, r1
  
  MRS     r1, IFSR                  ; If TTBCR.EAE == 0, IFSR[10, 3:0] takes the value ‘00001’
  ORR     r1, r1, r0                ; If TTBCR.EAE == 1, IFSR[5:0] takes the value ‘100001’.
  MSR     IFSR, r1

  MRS     r0, HIFAR                 
  MSR     IFAR, r0

  MRC     p15, 0, r0, c12, c0, 0    ; Read VBAR into r0
  ADD     r0, r0, #0xC              ; EL1 Prefetch Abort handler offset
  MSR     ELR_hyp, r0               
    
  MRS     r1, SPSR_hyp
  MSR     SPSR_abt, r1
                                    ; r2 := r2 & ~(CPSR_MODE_MASK)
  MOV     r2, #CPSR_MODE_MASK       ; r2 := 0x1F
  MVN     r2, r2                    ; r2 := ~0x1F (similar to r2 := 0xFFE0)
  AND     r1, r1, r2                ; r1 := r1 & r2 (i.e. r1 &= 0xFFE0)
  ORR     r1, r1, #HYP_Mode_ABT     ; r1 |= #HYP_Mode_ABT
  MSR     SPSR_hyp, r1              ; set the mode as abt, assigns it to SPSR_hyp for ERET

  POP     {r0-r3}                  ; recover everyt general purpose registers
    
  ERET                              ; Switch mode to SPSR_hyp by copying SPSR_hyp to CPSR
                                    ; and copying ELR_hyp to PC
  ; Implementation of ARM Cortex-A53 erratum 814270 -- end

Hyp_Abort_Handler
  MOV     r0, #HYP_LOG_EC_ABORT_HANDLER
  MRS     r1, HIFAR
  MRS     r2, HDFAR
  BL      hyp_log_exception
  B       Hyp_handler_loop_forever

Hyp_FIQ_Handler
  MOV     r0, #HYP_LOG_EC_FIQ_HANDLER
  BL      hyp_log_exception
  B       Hyp_handler_loop_forever

Hyp_IRQ_Handler
  MOV     r0, #HYP_LOG_EC_IRQ_HANDLER
  BL      hyp_log_exception
  B       Hyp_handler_loop_forever


; ------------------------------------------------
; Trap Handler at vector 0x14
; ------------------------------------------------
Hyp_Trap_Handler
  PUSH    {r0-r12}
  MOV     r0, #HYP_LOG_EC_TRAP_HANDLER
  MRS     r1, HSR
  BL      hyp_log_exception
  POP     {r0-r12}

  PUSH    {r4-r5}
  MRS      r4, HSR
  LSR      r5, r4, #HSR_EC_SHIFT
  CMP      r5, #HSR_EC_HVC
  POP     {r4-r5}
  BNE      Hyp_trap_logging

  BL       hyp32_hvc_handler
  ERET 

Hyp_trap_logging
  PUSH    {r0-r12}
  MRS     r0, HIFAR
  MRS     r1, HDFAR
  MRS     r2, DFAR
  MRS     r3, IFAR
  MRS     r4, HPFAR
  MRS     r5, IFSR
  MRS     r6, TTBCR
  MRS     r7, ELR_Hyp
  PUSH    {r0-r7}

  MOV     r0, r13
  BL      hyp_log_registers

Hyp_handler_loop_forever
  B       Hyp_handler_loop_forever

; ------------------------------------------------
;r0 contains the entry point of virtual machine 0, passed by TZ
;r1 contains the hypervisor configuration bitmask
;Can only modify R0-R3, rest need to be preserved
; ------------------------------------------------
Hyp_Reset_Handler
  msr     CPSR_c,#HYP_Mode_HYP:OR:HYP_I_Bit

  ; setup ELR and SPSR
  MSR     ELR_Hyp, r0

  ands    r2, r0, #0x1
  mov     r2, #HYP_Mode_SVC:OR:HYP_I_Bit
  beq     arm_code
  orr     r2, r2, #HYP_T_Bit
arm_code        
  MSR     spsr_cxsf, r2

  GetCPUUniqueID r2, r3

  ;setup stack pointer for this core
  ldr     sp, =|Image$$HYP_STACK$$ZI$$Limit|
  mov     r3, #HYP_STACK_SIZE
  mul     r3, r2                  ; obtain stack offset for CPU
  sub     sp, r3                  ; shift stack to CPU specific stack

  PUSH    {r4, r12, lr}

  ; Set the Hyp Vector Base Address Register (HVBAR)
  ; ------------------------------------------------
  LDR     r3, =HypervisorVectors
  MCR     p15, 4, r3, c12, c0, 0      ; Write HVBAR

  ; update CPU boot status for debugging
  ldr     r3, =hyp_cpu_boot_state
  mov     r0, #BOOT_STATE_INIT
  lsl     r2, r2, #2
  str     r0, [r3, r2]                 ; Indicate CPU booted

  ldr     r3, =hyp_is_init
  ldr     r2, [r3]

  cmp     r2, #0
  bne     warm_boot                 ; branch if not CPU0

  ; cold-boot of first core
  mov     r4, r1                    ; r4 is hypervisor config bitmask

  ; update the init flag
  mov     r2, #1
  str     r2, [r3]

  ldr     r3, =hyp_config_bmsk
  str     r4, [r3]

  ldr     r0, =|Image$$HYP_IMG$$Base|
  ldr     r1, =|Image$$HYP_STACK$$ZI$$Limit|
  BL      hyp_config_download_mode_backup

  ; ------------------------------------------------------------
  ; Configure logging during CPU0 init phase
  ; ------------------------------------------------------------
  BL      hyp_log_init              ; Configure logging area

  cmp     r4, #0
  bne     %f1
  bl      hyp_mm_cold_init
1

  ; bring RPM out of reset
  ; this must be done at the very end of cpu0 cold boot procedure to avoid 
  ; any concurrency issues with other cores
  ldr     r0, =GCC_APSS_MISC_ADDR
  ldr     r1, [r0]
  and     r1, r1, #RPM_RESET_REMOVAL_BMSK
  str     r1, [r0]
  
  b      cold_boot_done

warm_boot
  ldr     r3, =hyp_config_bmsk
  ldr     r4, [r3]

cold_boot_done

  GetCPUUniqueID r0, r1
  ; r0 is expected to be the core+cluster ID
  BL      hyp_log_warmboot_entry

  cmp     r4, #0
  bne     %f1
  bl      hyp_mm_warm_init
1

  ; configure HCR 
#ifndef HYP_TARGET_8909
  ; Setup HCR2
  MOV     r3, #0
  MCR     p15, 4, r3, c1, c1, 4
#endif

  ; TLBIALLNSNH - TLB invalidate all entries, non-secure non-hyp
  MCR     p15, 4, r3, c8, c7, 4 

  dsb
  isb

  ; Setup HCR - reset to 0 
  MOV     r3, #0 

  cmp     r4, #0
  bne     %f1
  ORR     r3, r3, #1                   ; Set HCR.VM
  ORR     r3, r3, #(1 << 2)            ; Set HCR.PTW
1
  MCR     p15, 4, r3, c1, c1, 0

  dsb
  isb

  GetCPUUniqueID r0, r1

  ; Jump to VM 0
  ldr  r1, =hyp_cpu_boot_state
  lsl  r2, r0, #2
  mov  r3, #BOOT_STATE_WARM
  str  r3, [r1, r2]                 ; Indicate CPU booted

  ; r0 is expected to be the core+cluster ID
  BL      hyp_log_warmboot_exit

  ; Retrieve saved registers
  POP    {r4, r12, lr}

 ;clear work registers
  mov     r0, #0x0
  mov     r1, #0x0
  mov     r2, #0x0
  mov     r3, #0x0
     
  ERET

;Unreachable loop
Unreachable
  b Unreachable

/* R0, R1 has HTTBR*/
/* R2, R3 are scratch registers */
HYP_SetupMMU
  ;
  ; Set up page table pointer (64-bit register)
  ; -------------------------------------------
  ; Configure via the Hyp Translation Table Base Register (HTTBR)
  ; 63:40 - UNK/SBZP
  ; 39:x  - Physical address
  ; x-1:0 - UNK/SBZP
  MCRR    p15, 4, r0, r1, c2           ; Write HTTBR

  ;
  ; Set up the Hyp Memory Attribute Indirection Register 0 (HMAIR0)
  ; ---------------------------------------------------------------
  ; These are the settings to be programmed into HMAIR0:
  ; 31:24 - Attr3 - 0x00 - Strongly Ordered
  ; 23:16 - Attr2 - 0x04 - Device
  ; 15:8  - Attr1 - 0xFF - Normal inner/outer outer WB/RA/WA
  ; 15:8  - Attr1 - 0xBB - Normal inner/outer WT/WA/RA
  ; 7:0   - Attr0 - 0x44 - Normal inner/outer non-cachable.
  LDR     r0, =0x0004FF44
  MCR     p15, 4, r0, c10, c2, 0       ; Write HMAIR0

  ;
  ; Set up the Hyp Memory Attribute Indirection Register 0 (HMAIR1)
  ; ---------------------------------------------------------------
  ; These are the settings to be programmed into HMAIR1:
  ; 31:24 - Attr3 - 0x00 - Strongly Ordered
  ; 23:16 - Attr2 - 0x04 - Device
  ; 15:8  - Attr1 - 0x00 - Strongly Ordered
  ; 7:0   - Attr0 - 0x04 - Device
  LDR     r0, =0x00040004
  MCR     p15, 4, r0, c10, c2, 1       ; Write HMAIR1

  ;
  ; Configure Hyp Translation Control Register (HTCR)
  ; ------------------------------------------------
  ; HTCR format:
  ; 31    - (1), UNK/SBOP
  ; 30    - SBZ (Implementation defined as UNK/SBZP on Cortex-A15)
  ; 29:14 - UNK/SBZP
  ; 13:12 - SH0   - 0x3 - inner shareable
  ; 11:10 - ORGN0 - 0x1 - WB
  ; 9:8   - IRGN0 - 0x1 - WB
  ; 7:3   - UNK/SBZP
  ; 2:0   - T0SZ  - The size offset of memory region addressed by HTTBR
  ldr     r0, =0x80003500
  MCR     p15, 4, r0, c2, c0, 2       ; Write HTCR

  ; TLBIALLH operation, ignoring the value in Rt
  MCR p15,4,r0,c8,c7,0 
  dsb
  isb

  ; Turn MMU on for the Hypervisor
  ; this is done after all memory writes to keep cache clean
  ; ------------------------------
  ; Configure via the Hyp System Control Register (HSCTLR)
  MOV     r0, #0  ; Read HSCTLR
  cmp     r4, #0
  bne     %f1
  ORR     r0, r0, #1                ; Set HSCTLR.M (bit 0)
1 ORR     r0, r0, #(1 << 12)        ; Set HSCTLR.I (bit 12)
  ORR     r0, r0, #(1 << 2)         ; Set HSCTLR.C (bit 2)
  MCR     p15, 4, r0, c1, c0, 0     ; Write HSCTLR

  ISB

  ; TLBIALLH operation, ignoring the value in Rt
  MCR p15,4,r0,c8,c7,0 
  dsb
  isb

  bx r14


    AREA    HYPENTRYDATA, DATA, READWRITE
hyp_cpu_boot_state   SPACE    (NUM_CPUS * 4)
hyp_is_init          SPACE    4
hyp_config_bmsk      SPACE    4

    END


