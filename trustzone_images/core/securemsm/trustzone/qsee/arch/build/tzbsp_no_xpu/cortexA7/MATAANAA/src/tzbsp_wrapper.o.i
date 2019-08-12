;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   W R A P P E R
;
; GENERAL DESCRIPTION
;   This file contains the TZBSP main entry
;
; EXTERNALIZED SYMBOLS
;   __main
;   _main
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;
; Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when        who        what, where, why
; --------   ------     --------------------------------------------------------
; 12/21/12   ah         Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND
; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r
    Mov32   $work_r, 0x02000000
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, 0x04800000
10  ldr     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND
; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r
    Mov32   $work_r, 0x02000000
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, 0x04800000
10  str     $val_r, [$addr_r, $work_r]
  MEND
; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
    GET qctps_common_macros.inc
    GET tzbsp_asm_common.inc
    GET tzbsp_asm.inc
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    IMPORT a7_aarch32_sysini
    IMPORT L2_INIT_SEMAPHORE_LOC
    IMPORT tzbsp_disable_l2_as_tcm
    IMPORT tzbsp_hyp_regs_init
    IMPORT tzbsp_do_l2_init
    IMPORT tzbsp_was_gdhs_pc
    IMPORT tzbsp_dcache_clean_inv_lx
    IMPORT tzbsp_pc_perform_l2_collapse_preconditions
    IMPORT tzbsp_chip_is_frodo_v1
    IMPORT tzbsp_cpu_count_per_cluster
    IMPORT tzbsp_mon_stack_start
    IMPORT tzbsp_dcache_flush_region
    IMPORT tzbsp_cpu_boot_pc_state
	IMPORT tzbsp_cpus_per_cluster_in_lpm
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    EXPORT sysini_wrapper
    EXPORT sysini_wrapper_cold_init
    EXPORT tzbsp_reset_vtimer_cntvoff
    EXPORT tzbsp_cpu_power_down_sequence
    EXPORT l2_shmoo_hole_workaround
    EXPORT tzbsp_cpu_invalidate_done_flag
    EXPORT tzbsp_boot_disable_cpu_l2_spm_arch
; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit
; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f
CPSR_MODE_MASK              EQU     0x1f
;------------------------------------------------------------------------------
    PRESERVE8
    CODE32
    AREA wrapper, CODE
;------------------------------------------------------------------------------
  MACRO
    switch_to_ns_mode $cpyreg, $tmp
    mrc     p15, 0, $cpyreg, c1, c1, 0 ; Read the current SCR
    orr     $tmp, $cpyreg, #SCR_NS_BIT ; Set SCR.NS=1
    mcr     p15, 0, $tmp, c1, c1, 0    ; write SCR
  MEND
  MACRO
    restore_cpsr_mode $reg 
    mcr     p15, 0, $reg, c1, c1, 0    ; write SCR
  MEND
; void sysini_wrapper(uint32 inval)
; Arguments:
;  r0: 1 to invalidate L2 cache, 0 to not invalidate L2 cache.
sysini_wrapper FUNCTION
    push    {r12, lr}
	   push    {r0}
    blx     tzbsp_hyp_regs_init ; Init CP15 VTimer Regs if needed
    pop     {r0}
    cmp     r0, #1      ; Check L2 invalidation argument.
    bne     %f1         ; Skip L2 invalidation if argument not set.
    ; Set up the L2 semaphore to do the full L2 init.
    GetCurrentCpuNum
    mov     r1, #1
    lsl     r1, r1, r0                  ; Set the current CPU bit high.
    ldr     r0, =L2_INIT_SEMAPHORE_LOC  ; L2 semaphore address.
    str     r1, [r0]                    ; Allow this CPU to do full L2 i
1   blx     a7_aarch32_sysini
    pop     {r12, pc}
    ENDFUNC
wfe_loop
    wfe
    ldr     r1, [r2, #0x0]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ldr     r1, [r2, #0x4]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ldr     r1, [r2, #0x8]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    ldr     r1, [r2, #0xC]        ; Check all invalidate done flags
    cmp     r1, #0x1
    bne     wfe_loop
    DSB
    bx r14
errata_wfi_fall_through     
    b       errata_wfi_fall_through    ; never happen? or just return? 
    ENDFUNC
sysini_wrapper_cold_init FUNCTION
    ;Cold Init being called; Unlock Cache lines in milestone
    ldr     r0, =tzbsp_l2cc_unlock
    mov     r1, #0x1 ;store value in l2cc unlock flag.
    str     r1, [r0]
    bx      lr
    ENDFUNC
tzbsp_reset_vtimer_cntvoff FUNCTION
    push    {r0-r3, lr}
    cps     #CPSR_MODE_MON              ; Switch to Monitor Mode
    mrc     p15, 0, r2, c1, c1, 0       ; Read SCR
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT:OR:SCR_NS_BIT
    mcr     p15, 0, r0, c1, c1, 0       ; Switch to NS
    mov     r0, #0
    mov     r1, #0
    MCRR    p15, 4, r0, r1, c14         ; set CNTVOFF = 0  
    mcr     p15, 0, r2, c1, c1, 0       ; Restore secure mode
    cps     #CPSR_MODE_SVC              ; Restore SVC mode
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; l2_shmoo_hole_workaround
;
;  DESCRIPTION - The very first write into L2 cache line (upper or lower 
;                bank) leaves corrupted data in the cache line when L2 
;                is powered up (Wake from L2 collapse sleep).
;                Reading directly into L2 cache lines after L2 power 
;                collapse will fix this issue (AKA. Shmoo hole).
;=======================================================================
l2_shmoo_hole_workaround FUNCTION
    push    {r0-r3, lr}
    ; only applicable to 8x26 v1
    bl      tzbsp_chip_is_frodo_v1
    cmp     r0, #1
    bne     l2_ret
    ; Check for L2 power collapse flag or GDHS L2 Collapse flag
    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]
    ldr     r1, =tzbsp_was_gdhs_pc
    ldr     r2, [r1]
    orr     r0, r0, r2
    cmp     r0, #1
    bne     l2_ret
    ; Clear GDHS L2 Collapse Flag
    mov     r0, #0
    str     r0, [r1]
    ; Enable L2 as TCM
    MOV32   r0, #0x0B010010        ; APCS_TCM_START_ADDR
    MOV32   r1, #0x08000000        ; TCM_START_ADDR
    orr     r1, r1, #1             ; Set TCM_REDIRECT_EN_0
    str     r1, [r0]
    DMB                            ;ARM recommandation
    bic     r1, r1, #1             ; TCM_START_ADDR (Lower 512KB)
    mov     r2, #0x40              ; Cache Line Size (64B)
l2_loop_lower_bank
    ldr     r3, [r1]               ; Read into L2 Lower Bank
    add     r1, r1, #0x4           ; Advance lower bank pointer
    subs    r2, r2, #0x4           ; Cache line counter
    bne     l2_loop_lower_bank
    MOV32   r1, #0x08080000        ; TCM_START_ADDR (Upper 512KB)
    orr     r1, r1, #1             ; Set TCM_REDIRECT_EN_0
    str     r1, [r0]	
    DMB                            ;ARM recommandation
    bic     r1, r1, #1             ; TCM_START_ADDR (Upper 512KB)
    mov     r2, #0x40              ; Cache Line Size (64B)
l2_loop_upper_bank
    ldr     r3, [r1]               ; Read into L2 Upper Bank
    add     r1, r1, #0x4           ; Advance upper bank pointer
    subs    r2, r2, #0x4           ; Cache line counter
    bne     l2_loop_upper_bank
    ; Disable L2 as TCM
    MOV32   r1, #0                 ; Disable L2 TCM mode
    str     r1, [r0]
    DMB                            ;ARM recommandation
l2_ret
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; tzbsp_cpu_power_down_sequence
;
;  DESCRIPTION - Power Down Sequence
;  1. Clear SCTLR.C
;  2. Clean & invalidate sec. & non-sec. L1 D-Cache
;  3. Clean & invalidate sec. & non-sec. L2 Cache if L2 is collapsing
;  4. Execute CLREX instruction
;  5. Clear ACTLR.SMP (switch to AMP mode)
;  6. Execute ISB
;  7. Execute DSB
; 
;=======================================================================
tzbsp_cpu_power_down_sequence FUNCTION
    push    {r0-r3, lr}
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4000, tzbsp_cpu_boot_pc_state
    DSB
    GetCurrentCpuNum
    add     r0, r0, #1
    ldr     r1, =tzbsp_mon_stack_start ; load base address for stack
    mov     r2, #1024
    mul     r0, r2, r0                  ; obtain stack offset for CPU
    sub     r0, r1, r0
    mov     r1, r2
    bl      tzbsp_dcache_flush_region
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4100, tzbsp_cpu_boot_pc_state
    ; 1. Clear SCTLR.C & SCTLR.I 
    mrc     p15, 0, r1, c1, c0, 0   ; Read SCTLR, Control register
    bic     r1, r1, #(1 << 2)       ; C bit - Disable D-Cache
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR, Control register
	ISB
    switch_to_ns_mode r2, r3    
    DSB
    mrc     p15, 0, r1, c1, c0, 0   ; Read SCTLR, Control register
    bic     r1, r1, #(1 << 2)       ; C bit - Disable D-Cache
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR, Control register
    ISB
    restore_cpsr_mode r2
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4200, tzbsp_cpu_boot_pc_state
    ; Check for L2 power collapse flag
    ldr     r0, =tzbsp_do_l2_init
    ldr     r1, [r0]
    cmp     r1, #1
    beq     l2_collapse
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4300, tzbsp_cpu_boot_pc_state
    ; 2. Clean & Invalidate L1 D-Cache only if L2 is not collapsing
    mov     r0, #0                  ; L1 cache
    blx     tzbsp_dcache_clean_inv_lx
    ; Check if L2 will be in GDHS state. Disable Snooping into L2.
    ldr     r1, =tzbsp_was_gdhs_pc
    ldr     r1, [r1]
    cmp     r1, #1
    beq     l2_gdhs
    b       tzbsp_cpu_power_down_sequence_exit
l2_collapse	
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4400, tzbsp_cpu_boot_pc_state
    ; 3. Clean & Invalidate Secure L1 & L2 D-Cache if L2 is collapsing
    blx     tzbsp_pc_perform_l2_collapse_preconditions
    b       tzbsp_cpu_power_down_sequence_exit
l2_gdhs
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4500, tzbsp_cpu_boot_pc_state
tzbsp_cpu_power_down_sequence_exit  ; Cache Maintenance done.
    ; 5. Clear ACTLR.SMP (switch to AMP mode).
    ; This is done after gic check for pending interrupts
    ; 6. Execute ISB
    ISB
    ; 7. Execute DSB
    DSB
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x4600, tzbsp_cpu_boot_pc_state
    pop     {r0-r3, pc}
    ENDFUNC
;=======================================================================
; tzbsp_boot_disable_cpu_l2_spm_arch
;
;  DESCRIPTION - Disable Current CPU & L2 SPM
; 
;  NOTE: Can't use Stack. Can corrupt all General register.
;=======================================================================
tzbsp_boot_disable_cpu_l2_spm_arch FUNCTION
    ; Only 1st Core waking up in the cluster disable L2 SPM.
	GetCurrentClusterNum
    ldr   r1, =tzbsp_cpus_per_cluster_in_lpm
    ldr   r1, [r1, r0, lsl #2]
    cmp   r1, #(4 / 1)    ; Are all CPUs down?
    bne   disable_cpu_spm	
    ; Disabling L2 SPM
    ldr   r1, =((0x0b000000 + 0x00012000) + 0x00000030)
    ldr   r0, [r1]
    tst   r0, #0x1
    beq   disable_cpu_spm           ; SPM_EN bit is already set to 0x0.
    bic   r0, #0x1
    str   r0, [r1]                  ; Clear SPM_EN bit / Bit 1
1   ldr   r0, [r1]
    tst   r0, #0x1
    bne   %b1                       ; Wait for write success for spm disable.
disable_cpu_spm    
    ldr   r1, =((0x0b000000 + 0x00009000) + 0x00000030)
    ldr   r0, [r1]
    bic   r0, #1                    ; Clear SPM_EN bit / Bit 1
    str   r0, [r1]
1   ldr   r0, [r1]
    tst   r0, #0x1
    bne   %b1                       ; Wait for write success for spm disable.
    ; return
    bx r14
    ENDFUNC
;======================================================================
; TZBSP data area
;======================================================================
    AREA    TZBSP_9k_DATA, DATA, READWRITE
tzbsp_l2cc_unlock                DCD     0x0 ; should TZ unlock L2CC-310?
tzbsp_cpu_invalidate_done_flag   SPACE   (4 * 4)
    END
