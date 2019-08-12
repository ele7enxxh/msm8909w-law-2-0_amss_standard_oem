;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      M O N I T O R
;
;
; GENERAL DESCRIPTION
;   This file contains the TZ monitor code to switch from
;   Scorpion's non-secure world to secure world and vice versa.
;   The TZ monitor executes in Scorpion's Monitor processing
;   mode and can be entered via a SMC exception or interrupts.
;
;   This monitor is based on ARM Sample Monitor. The following
;   copyright statement from ARM also applies:
;     (C) COPYRIGHT 2005 ARM Limited
;          ALL RIGHTS RESERVED
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   tzbsp_init must be called to initialize the TZ
;   monitor before it can perform any world switch.
;
; Copyright (c) 2009-2013 by QUALCOMM Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/msm8909/src/tzbsp_monitor.s#1 $
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 06/25/09   sl      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "tzbsp_target.h"
#include "tzbsp_asm.h"
#include "tzbsp_shared_imem.h"

   GET tzbsp_libarm_asm.inc
   GET tzbsp_asm_common.inc
   GET tzbsp_asm.inc
   GET qctps_common_macros.inc

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;

; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_HYP               EQU     0x1a
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f
CPSR_MODE_MASK              EQU     0x1f

; CPSR Thumb Instruction Set State Bit (CPSR[T])
CPSR_T_BIT                  EQU     0x20

; CPSR Interrupt Mask (CPSR[I], CPSR[I]) bits
CPSR_I_BIT                  EQU     0x80
CPSR_F_BIT                  EQU     0x40

; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit

; Distance between S and N contexts in the workspace.
Base_Diff                   EQU     Workspace_S_Base - Workspace_N_Base
Workspace_Diff              EQU     Workspace_N_Base - Workspace_N_Begin

; ----------------
; MACRO: SaveStateSDI
; ----------------
; Saves registers to the workspace r13 points into.
; Registers r0-r3 are not corrupted.
    MACRO
    SaveStateSDI
        stmdb   sp, {r0-r12}^
        mov     r4, sp
		cps     #CPSR_MODE_SYS
        stmia   r4!, {r13-r14}
        cps     #CPSR_MODE_IRQ
        stmia   r4!, {r13-r14}
        cps     #CPSR_MODE_SVC
        stmia   r4!, {r13-r14}
        cps     #CPSR_MODE_ABT
        stmia   r4!, {r13-r14}
        cps     #CPSR_MODE_UND
        stmia   r4!, {r13-r14}
        cps     #CPSR_MODE_FIQ
        stmia   r4!, {r8-r14}
		cps     #CPSR_MODE_MON
		stmia   r4!, {r13-r14}
    MEND


; ----------------
; MACRO: SaveState
; ----------------
; Saves registers to the workspace r13 points into.
; Registers r0-r3 are not corrupted.
    MACRO
    SaveState
        stmdb   sp, {r0-r14}^
        mov     r4, sp
        cps     #CPSR_MODE_IRQ
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_SVC
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_ABT
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_UND
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_FIQ
        mrs     r7, SPSR
        stmia   r4, {r7-r14}
        cps     #CPSR_MODE_MON
    MEND

; ---------------------
; MACRO: SaveStateNoGp
; ---------------------
; Saves registers to the workspace r13 points into.
; Registers r0-r3 are not corrupted. General purpose
; registers are not saved.
    MACRO
    SaveStateNoGp
        mov     r4, sp
        cps     #CPSR_MODE_IRQ
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_SVC
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_ABT
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_UND
        mrs     r12, SPSR
        stmia   r4!, {r12-r14}
        cps     #CPSR_MODE_FIQ
        mrs     r7, SPSR
        stmia   r4, {r7-r14}
        cps     #CPSR_MODE_MON
    MEND

; ----------------
; MACRO: LoadState
; ----------------
; Loads registers from the workspace r13 points into.
    MACRO
    LoadState
        mov     r1, sp
        cps     #CPSR_MODE_IRQ
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_SVC
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_ABT
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_UND
        ldmia   r1!, {r12-r14}
        msr     SPSR_fsxc, r12
        cps     #CPSR_MODE_FIQ
        ldmia   r1,  {r7-r14}
        msr     SPSR_fsxc, r7
        cps     #CPSR_MODE_MON
        ldmdb   sp, {r0-r14}^
    MEND

; ----------------
; MACRO: Save_NS_PC_CPSR
; ----------------
; Save r0 and r1 into N_PC and N_CPSR, respectively.
; Will set the T_BIT, if necessary
    MACRO
    Save_NS_PC_CPSR $pc, $cpsr, $scratch
        bic     $cpsr, $cpsr, #CPSR_T_BIT ; Clear the T bit if currently set
        ands    $scratch, $pc, #1         ; Test for THUMB mode
        beq     %f1
        orr     $cpsr, $cpsr, #CPSR_T_BIT ; New PC is THUMB code
1       ldr     $scratch, =N_PC
        stmia   $scratch, {r0-r1}
    MEND

; ----------------
; MACRO: SaveSCR
; ----------------
    MACRO
    SaveSCR $rx, $ry, $rz
        RCP15_SCR $rx
        mrc       p15, 0, $ry, c0, c0, 5  ; Current MPIDR.
        and       $ry, $ry, #0xFF         ; AFFL0 is the CPU number.
        ldr       $rz, =tzbsp_scr         ; cached SCR base address.
        str       $rx, [$rz, $ry, LSL #2] ; Write SCR for this CPU.
    MEND

; -----------------
; MACRO: RestoreSCR
; -----------------
    MACRO
    RestoreSCR $rx, $ry
        mrc       p15, 0, $ry, c0, c0, 5  ; Current MPIDR.
        and       $ry, $ry, #0xFF         ; AFFL0 is the CPU number.
        ldr       $rx, =tzbsp_scr         ; cached SCR base address.
        ldr       $rx, [$rx, $ry, LSL #2] ; Read SCR for this CPU.
        WCP15_SCR $rx
    MEND

; -------------------
; MACRO: SetSecure
; -------------------
    MACRO
    SetSecure $rx
        ; Set up SCR for Secure world.
        ; All FIQ, IRQ and EA are taken by monitor and not maskable by NS.
        mov       $rx, #SCR_EA_BIT:OR:SCR_IRQ_BIT:OR:SCR_FIQ_BIT
        WCP15_SCR $rx
        misb
    MEND

; -------------------
; MACRO: SetNonSecure
; -------------------
    MACRO
    SetNonSecure $rx
        ; Set up SCR for Normal world.
        ; FIQ - monitor, not maskable by NS.
        ; IRQ - NS, maskable by NS.
        ; EA  - NS, maskable by NS.
        mov       $rx, #SCR_AW_BIT:OR:SCR_NS_BIT:OR:SCR_FIQ_BIT
        WCP15_SCR $rx
        misb
    MEND

; -------------------------------
; MACRO: MonCallReturnToNonSecure
; -------------------------------
; All monitor calls return to non-secure mode.
;
; Arguments:
; rx: Used as a work register, set to zero.
    MACRO
    MonCallReturnToNonSecure $rx
        SetNonSecure    $rx
        mov             $rx, #0     ; Don't leak anything in this reg.
        movs            pc, lr      ; Return to the normal world.
    MEND

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    IMPORT  tzbsp_loop_here
    IMPORT  tzbsp_dcache_inval_all
    IMPORT  tzbsp_graphics_dcvs_reset
    IMPORT  tzbsp_graphics_dcvs_busystats_update
    IMPORT  tzbsp_dcache_flush_region
    IMPORT  tzbsp_dcache_clean_region
    IMPORT  tzbsp_exec_power_collapse_notifier
    IMPORT  tzbsp_cpus_per_cluster_in_lpm
    IMPORT  tzbsp_do_l2_init
    IMPORT  g_tzbsp_term_entry_counters
    IMPORT  g_tzbsp_term_exit_counters
    IMPORT  tzbsp_dcvs_system_event
    IMPORT  g_DcvsReturnVal0
    IMPORT  g_DcvsReturnVal1
    IMPORT  tzbsp_save_debug_regs
    IMPORT  tzbsp_clear_pc_debug_locks
    IMPORT  tzbsp_ns_wdt_isr
    IMPORT  tzbsp_l1_dump_request_state
    IMPORT  tzbsp_l1_dump
    IMPORT  tzbsp_l2_dump
    IMPORT  tzbsp_cpu_pc_state
    IMPORT  tzbsp_reset_vector
    IMPORT  g_tzbsp_ns_debug_vector
    IMPORT  g_tzbsp_ns_debug_stack
    IMPORT  g_tzbsp_ns_debug_ctx
    IMPORT  g_tzbsp_ns_debug_thumb
    IMPORT  tzbsp_wdt_mask_bark
    IMPORT  tzbsp_mmu_cache_disable
    IMPORT  tzbsp_mmu_disable
    IMPORT  tzbsp_mmu_cache_enable_no_cfg
    IMPORT  mmu_remove_tz_mapping
    IMPORT  sysini_wrapper
    IMPORT  tzbsp_was_gdhs_pc
    IMPORT  qsee_spmi_bus_disable_ns
    IMPORT  tzbsp_cpu_power_down_sequence
    IMPORT  tzbsp_check_pend_disable_gicc
    IMPORT  tzbsp_pc_check_votes_contention
    IMPORT  tzbsp_check_core_remove_int_target
    IMPORT  tzbsp_sec_interrupts_add_target
    IMPORT  tzbsp_cpu_invalidate_done_flag
    IMPORT  tzbsp_release_hw_lock
    IMPORT  lm_get_mitigation_state_from_look_up_table
    IMPORT  lm_apply_clk_div_vote
    IMPORT  g_s1_lm_return_1
    IMPORT  g_s1_lm_return_2
    IMPORT  g_s1_lm_return_3
    IMPORT Sysdbg_Workspace_N_Base
    IMPORT  tzbsp_cpu_boot_pc_state
    IMPORT LR_BACKUP
    IMPORT SP_BACKUP
    IMPORT  tzbsp_pc_disable_cpu_spm
    IMPORT  tzbsp_pc_enable_cpu_l2_spm
    IMPORT  tzbsp_secure_io_read
    IMPORT  tzbsp_secure_io_write

    IMPORT  |Image$$TZBSP_STACK$$Base|
    IMPORT  |Image$$TZBSP_STACK$$Length|
    IMPORT  |Image$$TZBSP_RAM_RW$$Base|
    IMPORT  |Image$$TZBSP_RAM_RW$$Length|
    IMPORT  |Image$$TZBSP_RAM_ZI$$ZI$$Base|
    IMPORT  |Image$$TZBSP_RAM_ZI$$ZI$$Length|

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    EXPORT  tzbsp_init_monitor
    EXPORT  tzbsp_smc_lock
    EXPORT  tzbsp_boot_lock
    EXPORT  tzbsp_install_irq_function
    EXPORT  tzbsp_install_fiq_function
    EXPORT  tzbsp_save_spbl_context
    EXPORT  tzbsp_hand_control_to_ns
    EXPORT  tzbsp_hand_control_to_ns_nfdbg
    EXPORT  tzbsp_is_non_secure
    EXPORT  Workspace_N_Begin
    EXPORT  Workspace_S_Begin
    EXPORT  Workspace_Mon_Begin
    EXPORT  tzbsp_irq
    EXPORT  N_CPSR
    EXPORT  S_MON_IP
    EXPORT  tzbsp_set_ns_return_addr
    EXPORT  tzbsp_setup_initial_ns_env
    EXPORT  tzbsp_setup_warmboot_ns_env
    EXPORT  tzbsp_set_cnthctl
    EXPORT  tzbsp_set_cntfrq
    EXPORT  tzbsp_smc_handler ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_smc_handler_nfdbg ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_mon_fiq_handler ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_mon_enter_pc ; Makes the symbol appear to debugger.
    EXPORT  tzbsp_scr
    EXPORT  tzbsp_mon_configure_smc_for_nfdbg
    EXPORT  tzbsp_mon_configure_smc_for_normal
    EXPORT  tzbsp_mon_save_banked_gp_regs
    EXPORT  tzbsp_mon_save_banked_sp_lr_regs
    EXPORT  g_tzbsp_clk_mutex
    EXPORT  tzbsp_mmu_disable_and_configure_ns
    EXPORT  tzbsp_disable_ns_mmu_after_sbl
    EXPORT  g_tzbsp_fiq_conf_mutex
    EXPORT  tzbsp_mon_stack_start 
    EXPORT  g_tzbsp_timer_list_mutex

    EXPORT  sysdbg_save_ctx
    PRESERVE8
    CODE32

;============================================================================
;
;                             MODULE CODE
;
;============================================================================

;===================================================================
; Monitor vector table
;===================================================================
;
; MVBAR always handles SMC and FIQ interrupts. IRQ interrupts are
; handled by monitor only when in secure mode. When in non-secure
; mode, IRQs are handled by non-secure VBAR.
;
; Because the device starts in secure SVC mode, the TZBSP vector
; table is defined as the first entry in .scl file. Monitor vector
; table can reside in any secure memory location with 2^5 byte
; alignment.
;
    AREA    TZBSP_MON_VECTOR_TABLE, CODE, READONLY, ALIGN=5

; Monitor vector table for normal operation.
tzbsp_mon_vector_table
    B       tzbsp_err_fatal         ; Not used (reset done in SVC mode)
    B       tzbsp_err_fatal         ; Not used (ARM constraint)
    B       tzbsp_smc_handler       ; SMC
    B       tzbsp_err_fatal         ; Prefetch abt, not handled.
    B       tzbsp_err_fatal         ; Data abt, not handled.
    B       tzbsp_err_fatal         ; Not used / reserved
    B       tzbsp_mon_irq_handler   ; IRQ
    B       tzbsp_mon_fiq_handler   ; FIQ

; Monitor vector table for non-secure debug FIQ use case. Vector tables
; are 32 bytes in size, the align directive is just a safeguard.
    ALIGN 32
tzbsp_mon_vector_table_nfdbg
    B       tzbsp_err_fatal         ; Not used (reset done in SVC mode)
    B       tzbsp_err_fatal         ; Not used (ARM constraint)
    B       tzbsp_smc_handler_nfdbg ; SMC
    B       tzbsp_err_fatal         ; Prefetch abt, not handled.
    B       tzbsp_err_fatal         ; Data abt, not handled.
    B       tzbsp_err_fatal         ; Not used / reserved
    B       tzbsp_err_fatal         ; IRQ can never happen during NFDBG.
    B       tzbsp_err_fatal         ; FIQ can never happen during NFDBG.

;===================================================================
; Handle fatal error
;===================================================================
tzbsp_err_fatal
    b       tzbsp_err_fatal

;===================================================================
; Secure IRQ handler
;===================================================================
;
; IRQs are trapped to monitor only when in secure side, hence we
; can assume that the current context is secure context.
;   - IRQ nesting is not supported during monitor IRQ handling.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1 (Interrupt disable),
;     CPSR.A=1 (Async abort disable) and CPSR.F=1 (FIQ Disable)
;   - Because only one core can be in secure side at any given time
;     only one core can get an IRQ in secure side. In case one CPU
;     is waiting for secure side access in its monitor, it won't get
;     IRQs because its CPSR.I=1.
;
; IRQs are handled by the non-secure side. The interrupted secure
; state will be saved and returning to the non-secure side will go
; through the SMC handlers secure-to-non-secure exit routine. After
; that the flow is as follows:
;
; 1. Non-secure side IRQ handler will be invoked. It will save the
;    context (which is what we loaded in SMC exit routine).
;
; 2. Non-secure side will handle the IRQ.
;
; 3. At some point the originally interrupted HLOS thread is
;    continued.
;
; 4. The execution continues in non-secure Secure Channel Manager
;    (SCM) which will see that the call to secure side has returned.
;    In reality, this was a side effect of IRQ handling and the secure
;    side is in interupted state and should be called again to
;    complete the service request.
;
; 5. Secure Channel Manager calls the secure side again which will
;    restore the interrupted secure context and continue.
;
; Access to secure side is serialized by the non-secure SCM. Once
; secure side has been exited to handle non-secure IRQ, access to TZBSP
; is unlocked. If a SMC instruction is issued, bypassing the
; non-secure SCM and indicating a compromized non-secure side, it is
; possible to enter TZBSP and continue in a context that didn't make the
; original service call. If this happens, then TZBSP just completes
; the service and returns to non-secure side but in the wrong context.
; The original call context will most probably fail in an
; unpredictable way in the non-secure side.
;
tzbsp_mon_irq_handler
    clrex
    push    {r0-r3, r12, lr}        ; Make work registers

    ; Mark that we have exited secure side via an IRQ.
    ldr     r0, =tzbsp_irq          ; Location of IRQ state
    mov     r1, #1
    str     r1, [r0]                ; Set IRQ state high

    ; Modify saved secure state to continue from the interrupted
    ; instruction whenever TZBSP is rescheduled again by non-secure
    ; side.
    ldr     r0, =S_PC               ; Currently saved secure PC.
    sub     r1, lr, #4              ; The next secure instruction.
    mrs     r2, spsr                ; Current SPSR.
    stmia   r0, {r1, r2}            ; Overwrite saved secure state.

    ; Check if we need to run TZBSP IRQ intercept function.
    ldr     r0, =tzbsp_irq_function ; Location of function addr.
    ldr     r0, [r0]                ; Value of function addr.
    cmp     r0, #0                  ; Check if function is set
    blxne   r0                      ; Call the IRQ interceptor

    pop     {r0-r3, r12, lr}        ; Restore original context.
    b       schedule_irq_exit       ; Run the TZ exit routine.

    LTORG                           ; Ensure the literal pool is
                                    ; close enough for the tzbsp_irq
                                    ; and S_PC LDR instructions

;===================================================================
; Secure FIQ handler
;===================================================================
;
; All FIQs are trapped to monitor mode, regardles of the processor
; security state when FIQ is triggered.
;   - FIQ nesting is not supported during monitor FIQ handling.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1, CPSR.A=1 and CPSR.F=1.
;
; Because FIQ handler is executed in monitor mode, the normal set of
; banked FIQ registers are not available.
;
tzbsp_mon_fiq_handler
    clrex

    sub     lr, lr, #4              ; Construct return address.
    stmfd   sp!, {lr}               ; Save mon_lr.
    mrs     lr, SPSR                ; Grab mon_spsr.
    stmfd   sp!, {lr}               ; Save mon_spsr.
    stmfd   sp!, {r0-r12}           ; Save r0-r12.

    SaveSCR   r0, r1, r2            ; Save the current secure state
    SetSecure r0                    ; Make sure we are in secure state

    ldr     r0, =tzbsp_fiq_function ; Location of FIQ handler ptr.
    ldr     r0, [r0]                ; Value of FIQ handler ptr.
    cmp     r0, #0                  ; Check that FIQ handler is set.
    ldreq   r0, =tzbsp_loop_here    ; Handler NULL, load err loop.
    bxeq    r0                      ; Branch to err loop.
    blx     r0                      ; Handler found, call it.

    RestoreSCR r0, r1               ; Restore the secure state

    ldmfd   sp!, {r0-r12}           ; Restore r0-r12
    ldmfd   sp!, {lr}               ; Grab saved mon_spsr.
    msr     SPSR_cxsf, lr           ; Restore mon_spsr.
    ldmfd   sp!, {pc}^              ; Return from FIQ to mon_lr.

;===================================================================
; Secure Monitor Call handler
;===================================================================
;
; SCM handler handles the entry from non-secure side to secure side and
; the exit in opposite direction.
;   - The handler runs in monitor mode, hence sp and lr are banked.
;   - On entry CPSR.I=1, CPSR.A=1 and CPSR.F=1.
;
; Only a single core can execute TZ code at any given time. Because IRQ
; is disabled by default when in SMC handler, single core pre-emptive
; scheduling is not possible. Simultaneous multicore entry is prevented
; by a spinlock that must be acquired before entering TZ.
;
; Any modification of TZ shared state without acquiring the lock is not
; allowed.
;
; The spinlock is a last line of defense, having to wait on the spinlock
; indicates abnormal execution flow. In normal flow HLOS Secure Channel
; Manager (SCM) contains a semaphore for serializing access to secure side.
; In other words, in the normal execution flow the entering core is
; expected to claim the lock without waiting.
;
tzbsp_smc_handler
    push    {r0-r3}             ; Make some work registers.

    ; Detect which world executed the SMC
    mrc     p15, 0, r0, c1, c1, 0
    tst     r0, #SCR_NS_BIT
    beq     schedule_secure_exit

; ---------------------
; N to S context switch
; ---------------------
schedule_secure_entry
    ; Set up SCR for Secure world.
    SetSecure r0

    ; Catch monitor calls that should be handled in monitor only. For
    ; all commands the bottom 12 bits can be ignored and should be
    ; ignored because for monitor calls we don't care about argument count
    ; or whether or not the register call bit is set. All monitor calls are
    ; register based.
    pop     {r0}        ; Get r0 argument from NS call.
    mov     r2, r0, LSR #12 ; Grab the command ID
    ; Monitor call --- Power Collapse.
    mov     r1, #0x0402
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have PC entry.
    beq     tzbsp_mon_enter_pc
    ; Monitor call --- L0/L1/L2 cache dump
    mov     r1, #0x0C05
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_cache_dump
    ; Monitor call --- I/O read cmd.
    mov     r1, #0x1401
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O read.
    beq     tzbsp_mon_io_read
    ; Monitor call --- I/O write cmd.
    mov     r1, #0x1402
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O write
    beq     tzbsp_mon_io_write
    ; Monitor call --- DCVS fast system event
    mov     r1, #0x3404
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack, we know we have I/O write
    beq     tzbsp_mon_dcvs_fast_system_event
    ; Monitor call --- DCVS reset
    mov     r1, #0x1403
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_dcvs_reset
    ; Monitor call --- DCVS update
    mov     r1, #0x1404
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_dcvs_update
    ; Monitor call --- SPMI bus disable
    mov     r1, #0x2401
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_spmi_bus_disable
    ; Monitor call --- S1-LM ID1 kernel code for power 
    mov     r1, #0x2402
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_s1_lm_kernel_atomic_call_5_2
    ; Monitor call -- S1-Lm ID2 syscall
    mov     r1, #0x2403
    cmp     r1, r2
    popeq   {r1-r3}     ; Unwind stack
    beq     tzbsp_mon_s1_lm_kernel_atomic_call_1_0
    push    {r0}        ; Restore context and continue normal SMC flow.

    ; Wait until TZ is not busy.
    WaitForAccess r0, r1, r2, tzbsp_smc_lock
    ; Got the lock, TZ is now exclusively ours.

    ; Store r14_mon and spsr_mon.
    ldr     r0, =N_PC
    mov     r1, lr
    mrs     r2, spsr
    stmia   r0, {r1, r2}

    ; Always clear the IRQ exit status when entering TZ.
    ldr     r1, =tzbsp_irq
    mov     r0, #0
    str     r0, [r1]            ; Clear IRQ state.
    pop     {r0-r3}             ; Restore non-secure r0-r3.

    ; Save the non-secure context.
    mov     lr, sp              ; Save the monitor stack address.
    ldr     sp, =Workspace_N_Base ; Start address for non-secure context save.
    SaveState                   ; Save NS, won't corrupt r0-r3
    dsb

    ; Restore the secure side context.
    ldr     sp, =Workspace_S_Base ; Start address for secure context save.
    LoadState                   ; Context = S-Context

    mov     sp, lr              ; Restore the monitor stack address.
    ldr     lr, =S_PC           ; Start location for secure PC and SPSR.
    rfeia   lr                  ; Return to Secure World code.

; ---------------------
; S to N context switch
; --------------------
schedule_secure_exit
    ; Store the secure world return address and SCPR.
    ldr     r0, =S_PC           ; Currently saved secure PC.
    mov     r1, lr
    mrs     r2, spsr            ; Current secure SPSR.
    stmia   r0, {r1, r2}        ; Overwrite saved secure state.

    pop     {r0-r3}             ; Restore secure state
    
schedule_irq_exit
    ; Save the secure context and load non-secure context.
    mov     lr, sp              ; Save monitor stack.
    ldr     sp, =Workspace_S_Base   ; The start of secure context
    SaveState                   ; S-Context = Context
    sub     sp, sp, #Base_Diff  ; r13_mon = Ptr to N-Workspace
    LoadState                   ; Context = N-Context
    mov     sp, lr              ; Restore monitor stack.

    ; Secure side calls are supposed to made only by non-secure
    ; side Secure Channel Manager. The call to secure side returns
    ; r0-r1 from non-secure perspective. This means we can corrupt
    ; non-secure registers r2-r3.

    ; If we exited secure state via IRQ, then set r0 to a special return
    ; value one.
    ldr     r3, =tzbsp_irq
    ldr     r2, [r3]
    cmp     r2, #0              ; If non-zero, should do IRQ return.
    movne   r0, #1              ; Indicate IRQ exit to NS side.

    ldr     lr, =N_PC           ; Currently saved non-secure PC.
    ldr     lr, [lr]
    ldr     r3, =N_CPSR         ; Currently saved non-secure CPSR.
    ldr     r3, [r3]
    bic     r3, r3, #(1<<6)     ; Ensure the SPSR.F bit is zero
    msr     SPSR_fsxc, r3

    ; Clear the lock.
    ReleaseAccess r2, r3, tzbsp_smc_lock
    ; At this point any other core can enter TZBSP.

    mov     r2, #0
    mov     r3, #0

    MonCallReturnToNonSecure r3 ; Return to the normal world

    ; Assembling a literal pool close to SMC handler. Monitor is no large
    ; enough that without a literal pool SMC handler cannot generate all
    ; the variable addresses it needs.
    LTORG

;===================================================================
; Monitor Power Collapse Termination Handler
;===================================================================
; Handler for entering power collapse state. Monitor stack is unwound when this
; handler is invoked. Value zero is returned if entering power collapse failed.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2[3:0]: 0 - No L2 GDHS nor L2 power collapse.
;     1 - L2 is about to be power collapsed.
;     2 - Reserved (was used for CPU retention flag on a-family).
;     3 - L2 is about to be put to GDHS retention state.
; r2[7:4]: 0 - core is interruptible
;          1 - core is un-interruptible  
tzbsp_mon_enter_pc
    ; Note: Should do any secure, per-cpu state saving here.

    push    {r2, lr}

    ; Disable CPU SPM here & enable before Power Collapse
    blx     tzbsp_pc_disable_cpu_spm

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0100, tzbsp_cpu_boot_pc_state

    WaitForAccess r0, r1, r2, tzbsp_boot_lock

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0200, tzbsp_cpu_boot_pc_state

    ; With Multicore PC support, To avoid confusion between 
    ; HLOS and TZ in L2 LPM mode (L2 PC vs. L2 GDHS), HLOS 
    ; will take a HW lock before make monitor power collapse 
    ; SCM call and TZ will release the lock after acquiring 
    ; tzbsp_boot_lock.
    blx     tzbsp_release_hw_lock

    ; Update CPU power collapse state
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_cpu_pc_state     ; Get current CPU state
    ldrb    r2, [r1, r0]                ;  for given CPU
    sub     r2, r2, #1                  ; Move state back to pwrdwn
    strb    r2, [r1, r0]                ; Save current CPU state

    ; Mark this CPU's L1 as invalid for SDI
    mov     r1, #1
    lsl     r1, r1, r0  ; Bit mask based on current CPU
    Mov32   r0, TZBSP_SHARED_IMEM_L1_VALIDITY
    ldr     r2, [r0]
    bic     r2, r2, r1
    str     r2, [r0]

    ; Increment the number of CPUs in low power mode.
    ldr     r0, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r1, [r0]
    add     r1, r1, #1
    str     r1, [r0]

    pop     {r2}                    ; Restore the input arg
    mov     r0, r2                  ; Make copy of flags
    and     r2, #0xF                ; Lowest nibble has L2 flags
    push    {r2}

    lsr     r0, #0x4                ; Nibble 1 has interruptible flag
    and     r0, #0xF 
    mrc     p15, 0, r1, c0, c0, 5   ; Current MPIDR.
    and     r1, r1, #0xFF           ; AFFL0 is the CPU number.
    blx     tzbsp_check_core_remove_int_target
    
    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0300, tzbsp_cpu_boot_pc_state

    ; Disable the secure WDOG bark before sleeping to prevent
    ; unnecessary wake-ups. External C function, so must
    ; ensure 8-byte stack alignment (i.e. before popping r2)
    mov     r0, #1 ; mask the bark interrupt
    blx     tzbsp_wdt_mask_bark

    pop     {r2}                    ; Restore the L2 input arg

    ; Determine the PC type indicated by HLOS.
    cmp     r2, #1                  ; PC with L2 PC.
    beq     with_l2_pc
    cmp     r2, #3                  ; PC with L2 GDHS.
    beq     with_l2_gdhs
    b       pc_common               ; Default to #0 style handling.

with_l2_pc
    ldr     r0, =tzbsp_do_l2_init
    ldr     r1, [r0]
    orr     r2, r1, r2              ; don't erase other cpu l2 init vote
    str     r2, [r0]
    ; Check if other cores are online. If this is not the last core going down,
    ; then cancel the L2 init vote.
    mov     r2, #0
    ldr     r1, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r1, [r1]
    cmp     r1, #TZBSP_CPU_COUNT_PER_CLUSTER    ; Are all CPUs down?
    strne   r2, [r0]                ; r0 still has tzbsp_do_l2_init address.
    b       pc_common

with_l2_gdhs
    mov     r2, #1
    ldr     r0, =tzbsp_was_gdhs_pc
    ldr     r1, [r0]
    orr     r2, r1, r2              ; Don't erase other CPU GDHS vote.
    str     r2, [r0]

    ; Check if other cores are online. If this is not the last core going down,
    ; then cancel the L2 GDHS vote.
    mov     r2, #0
    ldr     r1, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r1, [r1]
    cmp     r1, #TZBSP_CPU_COUNT_PER_CLUSTER    ; Are all CPUs down?
    strne   r2, [r0]                ; r0 still has tzbsp_was_gdhs_pc address.

    ; Monitor PC calls are serialized,
    ; Only honor the last L2 LPM request
    mov     r2, #0
    ldr     r0, =tzbsp_do_l2_init
    str     r2, [r0]

pc_common

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0400, tzbsp_cpu_boot_pc_state

    ; Check pending SPM votes and if none, then disable TZ SPM votes
    ; returns 0 if no pending and continue with wfi
    ; returns non-zero if pending votes and needs to skip wfi 
    blx tzbsp_pc_check_votes_contention

    cmp r0, #0x0
    beq votes_contention_ok
    b votes_contention_nok

votes_contention_ok

    ldr     r0, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r0, [r0]
    cmp     r0, #TZBSP_CPU_COUNT_PER_CLUSTER    ; Are all CPUs down?
    bne     skip_cm                 ; Not all CPUs down, skip CM

    ldr     r0, =tzbsp_do_l2_init
    ldr     r0, [r0]
    cmp     r0, #1                  ; was the L2 flag set?
    bne     skip_cm                 ; L2 flag was not set, skip CM

do_cm
    mov     r1, #0                  ; Mark L2 invalid for SDI
    Mov32   r0, TZBSP_SHARED_IMEM_L2_VALIDITY
    str     r1, [r0]

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0500, tzbsp_cpu_boot_pc_state

    push    {r12} ; For 8-byte stack alignment
    blx     tzbsp_exec_power_collapse_notifier ; Notify executive
    pop     {r12}                              ; client(s) of power
                                               ; collapse so they can
                                               ; do cache maintenance

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0600, tzbsp_cpu_boot_pc_state

    bl      tzbsp_flush_all_tz_regions ; Flush secure cache lines before
                                       ; updating lock.  Lock must be last.

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0700, tzbsp_cpu_boot_pc_state

    ; intentional fallthrough
skip_cm
    ; intentional fallthrough

    ; Update the PC termination entry counter. Diagnostics area is non-cached.
    mrc     p15, 0, r0, c0, c0, 5            ; Current MPIDR.
    and     r0, r0, #0xFF                    ; AFFL0 is the CPU number.
    ldr     r1, =g_tzbsp_term_entry_counters ; Base of entry counter addresses.
    ldr     r1, [r1, r0, LSL #2]             ; Read the counter address.
    ldr     r2, [r1]                         ; Read the stored counter value.
    add     r2, r2, #1                       ; Increment the counter.
    str     r2, [r1]                         ; Write the new counter value.

    ; Save the debug registers for CPU before PC
    ; Note: Cannot debug with external debugger after this point
;    bl      tzbsp_save_debug_regs

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0800, tzbsp_cpu_boot_pc_state

    ; Check pending interrupt and if none, then disable gicc
    ; returns 0 if no pending and continue with wfi
    ; returns non-zero if pending interrupt and needs to skip wfi 
    blx tzbsp_check_pend_disable_gicc

    cmp r0, #0x0
    bne skip_wfi

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0900, tzbsp_cpu_boot_pc_state

    ; A7 specific power collapse sequence
    bl      tzbsp_cpu_power_down_sequence

    ldr     r0, =tzbsp_cpu_invalidate_done_flag
    mov     r2, #0x0    
    str     r2, [r0, #0x0]                     ; Clear invalidate done
    str     r2, [r0, #0x4]                     ; Clear invalidate done
    str     r2, [r0, #0x8]                     ; Clear invalidate done
    str     r2, [r0, #0xC]                     ; Clear invalidate done

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0A00, tzbsp_cpu_boot_pc_state

    ; enable SPM to allow CPU & L2 Power Collapse
    blx     tzbsp_pc_enable_cpu_l2_spm

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0x0B00, tzbsp_cpu_boot_pc_state

    ReleaseAccess r0, r1, tzbsp_boot_lock

    ; Set Milestone for PC Entry.0x0000XX00
    SetCpuStateMilestoneVar r0, r1, 0xFF00, tzbsp_cpu_boot_pc_state
    
    ; 5. Clear ACTLR.SMP (switch to AMP mode)
    MRC     p15, 0, r0, c1, c0, 1     ; Read ACTLR
    bic     r0, r0, #(0x1 << 6)       ; Clear SMP Bit
    MCR     p15, 0, r0, c1, c0, 1     ; Write ACTLR

    mdsb

    wfi                                      ; Try entering low power state.

    ; Execution should never reach here since QGIC is disabled.
    SetCpuStateMilestoneVar r0, r1, 0xDEAD, tzbsp_cpu_boot_pc_state   
    
wfi_fatal
    b       wfi_fatal

skip_wfi

    ; Set Milestone for PC Exit.0x00XX0000
    SetCpuStateMilestoneVar r0, r1, 0x010000, tzbsp_cpu_boot_pc_state

    ; Release debug locks that were supposed to be held across PC
    ; Note: This restores access to external debugger
;    bl      tzbsp_clear_pc_debug_locks

    ; Update the PC termination exit counter. Diagnostics area is non-cached.
    mrc     p15, 0, r0, c0, c0, 5           ; Current MPIDR.
    and     r0, r0, #0xFF                   ; AFFL0 is the CPU number.
    ldr     r1, =g_tzbsp_term_exit_counters ; Base of exit counter addresses.
    ldr     r1, [r1, r0, LSL #2]            ; Read the counter address.
    ldr     r2, [r1]                        ; Read the stored counter value.
    add     r2, r2, #1                      ; Increment the counter.
    str     r2, [r1]                        ; Write the new counter value.

    ; Power collapse didn't happen - L2 remains valid from SDI perspective.
    mov     r1, #1
    Mov32   r0, TZBSP_SHARED_IMEM_L2_VALIDITY
    str     r1, [r0]

votes_contention_nok

    ; Set Milestone for PC Exit.0x00XX0000
    SetCpuStateMilestoneVar r0, r1, 0x020000, tzbsp_cpu_boot_pc_state

    ;Add CPU to all the secure interrupts target list
    mrc     p15, 0, r0, c0, c0, 5           ; Current MPIDR.
    and     r0, r0, #0xFF                   ; AFFL0 is the CPU number.
    blx     tzbsp_sec_interrupts_add_target

    ; Set Milestone for PC Exit.0x00XX0000
    SetCpuStateMilestoneVar r0, r1, 0x030000, tzbsp_cpu_boot_pc_state


    ; Decrement the number of CPUs in low power mode.
    ldr     r0, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r1, [r0]
    sub     r1, r1, #1
    str     r1, [r0]

    ; Clear the L2 counter in any case.
    ldr     r0, =tzbsp_do_l2_init
    mov     r1, #0
    str     r1, [r0]
	
    ; Clear the GDHS state as well.
    ldr     r0, =tzbsp_was_gdhs_pc
    mov     r1, #0
    str     r1, [r0]

    ; Variable tzbsp_was_l2_pc is only updated in warm boot -> no need to
    ; clear that here.

    ; Update CPU power collapse state
    mrc     p15, 0, r0, c0, c0, 5       ; Current MPIDR.
    and     r0, r0, #0xFF               ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_cpu_pc_state     ; Get current CPU state
    ldrb    r2, [r1, r0]                ;  for given CPU
    add     r2, r2, #1                  ; Move state back to pwrup
    strb    r2, [r1, r0]                ; Save current CPU state

    ; Power collapse didn't happen - this CPU's L1 remains valid from SDI perspective.
    mov     r1, #1
    lsl     r1, r1, r0  ; Bit mask based on current CPU
    Mov32   r0, TZBSP_SHARED_IMEM_L1_VALIDITY
    ldr     r2, [r0]
    orr     r2, r2, r1
    str     r2, [r0]

    ; Re-enable the secure WDOG bark; external C function, so must
    ; ensure 8-byte stack alignment
    push    {r0}
    mov     r0, #0 ; un-mask the bark interrupt
    blx     tzbsp_wdt_mask_bark
    pop     {r0}

    ; Set Milestone for PC Exit.0x00XX0000
    SetCpuStateMilestoneVar r0, r1, 0x040000, tzbsp_cpu_boot_pc_state

    ReleaseAccess r0, r1, tzbsp_boot_lock

    pop     {lr}

    ; Entry to low power state failed, e.g. an interrupt was pending.
    ; Will have to return back to non-secure world.

    ; Set Milestone for PC Exit.0x00XX0000
    SetCpuStateMilestoneVar r0, r1, 0xFF0000, tzbsp_cpu_boot_pc_state

    ; Don't Leak any register through r0-r3.
    mov     r0, #0
    mov     r1, #0
    mov     r2, #0

    MonCallReturnToNonSecure r3 ; Return to the non-secure world.

    ; Assembling a literal pool close to Mon Enter PC handler. Monitor
    ; is now large enough that without a literal pool tzbsp_mon_enter_pc
    ; cannot generate all the variable addresses it needs.
    LTORG

;===================================================================
; Cache coherency handlers
;===================================================================
; Flush areas of memory that belong to TZ that are cacheable.  This
; must be called before exiting the secure side after any stores to
; cacheable memory.  Stack and monitor stack do not need to be flushed
; because stacks should be empty upon normal exit/entry from/to TZ.
; This function is not appropriate for IRQ exits.
;
tzbsp_flush_all_tz_regions
    push    {lr}

    ldr     r0, =|Image$$TZBSP_STACK$$Base|
    ldr     r1, =|Image$$TZBSP_STACK$$Length|
    bl      tzbsp_dcache_flush_region

    ldr     r0, =|Image$$TZBSP_RAM_RW$$Base|
    ldr     r1, =|Image$$TZBSP_RAM_RW$$Length|
    bl      tzbsp_dcache_flush_region

    ldr     r0, =|Image$$TZBSP_RAM_ZI$$ZI$$Base|
    ldr     r1, =|Image$$TZBSP_RAM_ZI$$ZI$$Length|
    bl      tzbsp_dcache_flush_region

    ldmfd   sp!, {pc}

;===================================================================
; Monitor I/O read handler
;===================================================================
; Handler for monitor mode I/O read. Monitor stack is unwound when this handler
; is invoked. Value zero is returned in r0 if the access is illegal.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Address to be read.
; r3: Not used, can be corrupted.
tzbsp_mon_io_read
    push    {r12, lr}
    mov     r0, r2
    blx     tzbsp_secure_io_read
    mov     r1, #0 
    mov     r2, #0 
    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

;===================================================================
; Monitor I/O write handler
;===================================================================
; Handler for monitor mode I/O write. Monitor stack is unwound when this
; handler is invoked.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Address to be written.
; r3: Value to be written.
tzbsp_mon_io_write
    push    {r12, lr}
    mov     r0, r2
    mov     r1, r3
    blx     tzbsp_secure_io_write
    mov     r1, #0 
    mov     r2, #0 
    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

;===================================================================
; Monitor DCVS reset handler
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
tzbsp_mon_dcvs_reset
    push    {r12, lr}
    blx     tzbsp_graphics_dcvs_reset

    mov     r0, #0
    mov     r1, #0
    mov     r2, #0

    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

;===================================================================
; Monitor DCVS update handler
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Argument for update.
tzbsp_mon_dcvs_update
    push    {r12, lr}
    mov     r0, r2
    mov     r1, r3
    mov     r2, r4
    blx     tzbsp_graphics_dcvs_busystats_update
    ; R0 contains the update return value

    mov     r1, #0
    mov     r2, #0

    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

tzbsp_mon_dcvs_fast_system_event
    push    {r12, lr}

    ; Shuffle the incoming arguments to conform to the standard C call
    ; style.
    mov     r0, r2      ; core_id
    mov     r1, r3      ; event_id
    mov     r2, r4      ; param0
    mov     r3, r5      ; param1

    blx tzbsp_dcvs_system_event
    ; R0 contains the update return value

    ; R1 contains the first additional return value.
    ldr     r1, =g_DcvsReturnVal0
    ldr     r1, [r1]

    ; R2 contains the second additional return value.
    ldr     r2, =g_DcvsReturnVal1
    ldr     r2, [r2]

    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

;===================================================================
; Monitor cache dump handler
;===================================================================
; Handler for monitor mode cache dumping. Monitor stack is unwound when this
; handler is invoked.
;
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: 0, handle L1 dump request similar to WDT bark, resulting in L0/L1 cache
;        dump, CPU context dumps and device reset.
;     1, only dump L1 cache and return to HLOS after the dump is done.
;     2, only dump L2 cache and return to HLOS after the dump is done.
;     Any other value for r2 will return -4 (-E_NOT_SUPPORTED).
tzbsp_mon_cache_dump
    cmp     r2, #0          ; If r2=0,
    beq     %f4             ;   then handle similar to WDT bark.
    cmp     r2, #1          ; If r2=1,
    beq     %f1             ;   then just do L0/L1 dump and return.
    cmp     r2, #2          ; If r2=2,
    beq     %f2             ;   then just do L2 dump and return.
    mov     r0, #(-4)       ; Else set -E_NOT_SUPPORTED...
    b       %f3             ;   ...and return.

    ; L1 cache dumping.
1   push    {r12, lr}
    blx     tzbsp_l1_dump   ; Jump to L1 dump handler.
    pop     {r12, lr}
    mov     r0, #0          ; Set E_SUCCESS return value.
    b       %f3             ; Return to non-secure side.

    ; L2 cache dumping.
2   push    {r12, lr}
    blx     tzbsp_l2_dump   ; Jump to L2 dump handler.
    pop     {r12, lr}
    mov     r0, #0          ; Set E_SUCCESS return value.
    ; Continue to local label 3; return to non-secure side.

    ; Exit sequence.
3   ; This local label is on a separate line due to the macro below.
    mov     r1, #0
    mov     r2, #0

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

    ; L1 dumping similar to WDT bark never returns and causes device reset.
    ; Pushing of stack registers emulates FIQ handler. The stack layout must be
    ; identical to FIQ handler for the context dumping.
4   stmfd   sp!, {lr}                   ; Save mon_lr.
    mrs     lr, SPSR                    ; Grab mon_spsr.
    stmfd   sp!, {lr}                   ; Save mon_spsr.
    stmfd   sp!, {r0-r12}               ; Save r0-r12.

    ; Indicate that the L1 dump is caused by L1 request from kernel.
    mov     r0, #1
    ldr     r1, =tzbsp_l1_dump_request_state
    str     r0, [r1]

    ; Modify the saved SCR to indicate the "interrupted" context is non-secure.
    ; As we are entering WDT bark handling, FIQs are disabled and the call
    ; never returns, hence stomping the saved SCR is ok.
    mrc     p15, 0, r0, c0, c0, 5   ; Current MPIDR.
    and     r0, r0, #0xFF           ; AFFL0 is the CPU number.
    ldr     r1, =tzbsp_scr          ; Saved SCR base address.
    ldr     r2, [r1, r0, LSL #2]    ; Read the saved SCR.
    orr     r2, r2, #1              ; Ensure NS bit is set.
    str     r2, [r1, r0, LSL #2]    ; Write the saved SCR.

    ; Jump directly to WDT bark handler. Note that WDT bark related SGI is
    ; always registered by TZBSP. This means other CPUs will be able to receive
    ; and handle the WDT bark SGI even if HLOS has not registered WDT0 as FIQ.
    mov     r0, #0                  ; Pass NULL to WDT bark ISR.
    ldr     r1, =tzbsp_ns_wdt_isr   ; Location of WDT bark ISR.
    blx     r1                      ; Jump to WDT bark ISR.
    ; WDT bark ISR will never return.

;===================================================================
; Monitor SPMI bus disable
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Not used, spare, should be zero.
tzbsp_mon_spmi_bus_disable
    push    {r12, lr}

    mov     r0, r2
    blx     qsee_spmi_bus_disable_ns

    ; R0 contains the update return value
    mov     r1, #0
    mov     r2, #0
    pop     {r12, lr}

    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

	
	
;===================================================================
; Monitor S1-LM Kernel code logic for the PC 
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; r2: Argument 1.
; r3: Argument 2.
; r4: Argument 3.
; r5: Argument 4.
; r6: Argument 5.

tzbsp_mon_s1_lm_kernel_atomic_call_5_2
    push    {r12, lr}

    ; Shuffle the incoming arguments to conform to the standard C call
    ; style.
    mov     r0, r2      ; param1
    mov     r1, r3      ; param2
    mov     r2, r4      ; param3
    mov     r3, r5      ; param4
    mov     r4, r6      ; param5
    push   {r4}
	
    blx lm_get_mitigation_state_from_look_up_table
    ; R0 contains the update return value

    pop     {r4}
    ; R1 contains the first additional return value.
    ldr     r1, =g_s1_lm_return_1
    ldr     r1, [r1]

    ; R2 contains the second additional return value.
    ldr     r2, =g_s1_lm_return_2
    ldr     r2, [r2]

    ; R3 contains the second additional return value.
    ldr     r3, =g_s1_lm_return_3
    ldr     r3, [r3]

    pop     {r12, lr}
    MonCallReturnToNonSecure r4     ; Switch back to non-secure side.
	
	
;===================================================================
; Monitor S1-LM Kernel code logic for the PC 
;===================================================================
; Arguments:
; r0: Command ID, can be corrupted.
; r1: Token, can be corrupted.
; 

tzbsp_mon_s1_lm_kernel_atomic_call_1_0
    push    {r12, lr}

    ; Shuffle the incoming arguments to conform to the standard C call
    ; style.
    mov     r0, r2      ; param1
	
    blx  lm_apply_clk_div_vote
    ; R0 contains the update return value

    mov     r1, #0
    mov     r2, #0

    pop     {r12, lr}
    MonCallReturnToNonSecure r3     ; Switch back to non-secure side.

;===================================================================
; FUNCTION: void tzbsp_save_spbl_context( uin32 lr_nwd, uint32 sp_nwd )
;===================================================================
;
; Saves the SPBL state to monitor's NS context. SPBL is now known as
; SBL2.
;
; TZ initialization is called as a function from SPBL. TZ is entered
; from SPBL in secure mode and will return to SPBL in non-secure mode.
;
; This implies the following:
;   - Registers r0-r3 can be corrupted in TZ.
;   - SPBL context must be saved and NS banked registers must be
;     configured so that exiting from the secure side to SPBL emulates
;     a function return.
;
; We need to keep/configure the following SPBL state:
;   1. SPBL sp  : Written to NS saved context.
;   2. SPBL lr  : Written to NS saved context.
;
; Parameters
;   r0 - Entry ptr of the Normal World
;   r1 - Stack ptr of the Normal World
;
tzbsp_save_spbl_context FUNCTION
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    ; ------------------------------------------------------------------
    ; Clear the whole workspace. Other values to be written on top.
    ; ------------------------------------------------------------------
    ldr     r2, =MonitorWorkspaceBase
1   mov     r3, #0x0
    str     r3, [r2],#4
    ldr     r3, =MonitorWorkspaceEnd
    cmp     r2, r3
    bne     %b1

    ; ------------------------------------------------------------------
    ; Build up the Non secure context by using the secure world context.
    ; ------------------------------------------------------------------
    ; Save the non-secure stack
    ldr     r2, =N_USR_R13
    str     r1, [r2]
    ldr     r2, =S_SVC_R13
    str     r1, [r2]

    ldr     r13, =Workspace_N_Base
    SaveState

    ; Store the Normal World "reset" PC and CPSR, checking if the
    ; PC is ARM or THUMB code.
    mov     r1, #CPSR_MODE_SVC:OR:CPSR_I_BIT
    Save_NS_PC_CPSR r0, r1, r2

    ; Pass through the SBL2 VBAR address.
    ldr     r1, =tzbsp_ns_vbar
    mrc     p15, 0, r0, c12, c0, 0  ; Current VBAR
    str     r0, [r1]

    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_hand_control_to_ns( uin32 lr_nwd )
;===================================================================
;
; Hands control the the non-secure side.
;
; Parameters
;   r0 - Entry ptr of the Normal World
;
tzbsp_hand_control_to_ns FUNCTION
    ; Change to monitor mode
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    mov     r4, r0                  ; Save the normal world
                                    ; jump address in r4

    ; Unlock the boot flow for other CPUs.
    ReleaseAccess r1, r2, tzbsp_boot_lock

    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x80, tzbsp_cpu_boot_pc_state

    ; After lock gets to coherency point any other core can boot
    mov     lr, r4                  ; Restore LR, normal world jump addr

    ; Clear the entire register file. For each mode SPSR is left as it is.
    mov     r1,  #0
    mov     r2,  #0
    mov     r3,  #0
    mov     r4,  #0
    mov     r5,  #0
    mov     r6,  #0
    mov     r7,  #0
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    cps     #CPSR_MODE_SYS
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_IRQ
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_SVC
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_ABT
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_UND
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_FIQ
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    mov     r13, #0
    mov     r14, #0

    ; Switch back to monitor mode
    cps     #CPSR_MODE_MON

    ; Store the Normal World "reset" PC and CPSR.
    msr     SPSR_cfsx, #CPSR_MODE_SVC:OR:CPSR_I_BIT

    ; Switch to non-secure state. Set up SCR for Normal world.
    SetNonSecure r0

   ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0xFF, tzbsp_cpu_boot_pc_state

    mov     r0, #0
    mov     r1, #0

    ; Jump to the non-secure side, will never return.
    movs    pc, lr
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_mon_configure_smc_for_nfdbg(void)
;===================================================================
; Configures the monitor vector table for the non-secure debug FIQ
; use case. Must be called in monitor mode.
tzbsp_mon_configure_smc_for_nfdbg FUNCTION
    ldr     r0, =tzbsp_mon_vector_table_nfdbg
    mcr     p15, 0, r0, c12, c0, 1              ; MVBAR
    isb
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_mon_configure_smc_for_normal(void)
;===================================================================
; Configures the monitor vector table for normal TZ operation.
; Must be called in monitor mode.
tzbsp_mon_configure_smc_for_normal FUNCTION
    ldr     r0, =tzbsp_mon_vector_table
    mcr     p15, 0, r0, c12, c0, 1              ; MVBAR
    isb
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_mon_save_banked_gp_regs(uint32* buf)
;===================================================================
; Saves the banked general purpose registers to the provided buffer.
; Can only be called in monitor mode.
;
; Parameters
;   r0 - Start address of the save buffer.
tzbsp_mon_save_banked_gp_regs FUNCTION
    stmia   r0, {r13-r14}^
    add     r0, #8
    cps     #CPSR_MODE_IRQ
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_SVC
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_ABT
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_UND
    mrs     r1, SPSR
    stmia   r0!, {r1, r13-r14}
    cps     #CPSR_MODE_FIQ
    mrs     r1, SPSR
    stmia   r0!, {r1, r8-r14}

    ; Switch back to monitor mode
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_mon_save_banked_sp_lr_regs(uint32* buf)
;===================================================================
; Saves the banked sp and lr registers of the interrupted process to
; the provided buffer.
; Can only be called in monitor mode.
; sp/lr saving for hypervisor needs special handling since switching
; to/from hyp cannot be done by directly modifying CPSR. Attempting to
; do so will lead target into illegal state. Instead, a special case
; is done for hyp and the elr_hyp/sp_hyp are directly addressed to
; save them.
;
; Parameters
;   r0 - Start address of the save buffer.
tzbsp_mon_save_banked_sp_lr_regs FUNCTION
    push    {r1-r2}

    mrs     r1, SPSR                    ; Interrupted process CPSR
    and     r1, r1, #CPSR_MODE_MASK
    cmp     r1, #CPSR_MODE_HYP
    beq     %f1
    cmp     r1, #CPSR_MODE_USR          ; Check for usr mode, if found
    moveq   r1, #CPSR_MODE_SYS          ; then use system mode
    mrs     r2, CPSR
    bic     r2, r2, #CPSR_MODE_MASK
    orr     r1, r2, r1
    msr     CPSR_c, r1                  ; Switch to the interruped context
    stmia   r0, {r13-r14}               ; Store sp and lr
    cps     #CPSR_MODE_MON              ; Switch back to monitor mode
    b       %f2
1   mrs     r1, elr_hyp
    str     r1, [r0]
    add     r0, r0, #4
    mrs     r1, sp_hyp
    str     r1, [r0]

2   pop     {r1-r2}
    bx      lr
    ENDFUNC

;===================================================================
; Debug FIQ Secure Monitor Call handler
;===================================================================
; When debug FIQ exit from TZ happens, we only allow returning to
; TZ to continue FIQ handling. Non-secure context saving is not
; necessary when returning, as the non-secure context is created on
; the fly when switching from secure to non-secure debug FIQ handler.
tzbsp_smc_handler_nfdbg
    push    {r0-r3}             ; Make some work registers.

    ; Detect which world executed the SMC
    mrc     p15, 0, r0, c1, c1, 0
    tst     r0, #SCR_NS_BIT
    beq     nfdbg_secure_exit

    ; Return back from non-secure debug FIQ handler to TZ FIQ handler.
nfdbg_secure_entry

    ; Locks, monitor commands and non-secure context saving ignored. This
    ; SMC handler will only switch back to non-secure side, which is in
    ; interrupted FIQ handler state.

    pop     {r0-r3}                     ; Unwind stack (corrupting regs ok).

    SetSecure r0                        ; Set up SCR for Secure world.

    ; Restore the secure side context.
    mov     lr, sp              ; Save the monitor stack address.
    ldr     sp, =Workspace_S_NFDBG_Base ; Start address for secure context.
    LoadState                           ; Restore secure context.
    mov     sp, lr              ; Restore the monitor stack address.

    ldr     lr, =S_NFDBG_PC     ; Start location for secure PC and SPSR.
    rfeia   lr                  ; Return to Secure World code.

    ; Control is handed to non-secure side debug FIQ handler in MON mode.
    ; The control is never handed via SMC call, if exit from secure side
    ; is attempted via this SMC handler, then it is an error condition.
nfdbg_secure_exit
    b       nfdbg_secure_exit  ; Start waiting for wdog reset.

;===================================================================
; FUNCTION: void tzbsp_hand_control_to_ns_nfdbg(void)
;===================================================================
; Hand control to non-secure debug FIQ handler. Current context is saved and
; non-secure handler context is constructed from scratch. The return from
; the non-secure side will end up executing this handler from nfdbg_fiq_return.
; Because FIQ handling is done in MON mode it isn't possible to SMC from
; the debug FIQ ISR to non-secure side (monitor itself is the context
; switching entity).
tzbsp_hand_control_to_ns_nfdbg FUNCTION
    stmfd   sp!, {lr}
    ; Store the secure world return address and SCPR.
    ldr     r0, =S_NFDBG_PC      ; Address of saved PC
    adr     r1, nfdbg_fiq_return ; Address of the return label
    mrs     r2, cpsr             ; Current MON CPSR.
    stmia   r0, {r1, r2}         ; Overwrite saved secure state.

    ; Save the secure context and load non-secure context.
    mov     lr, sp                          ; Save monitor stack.
    ldr     sp, =Workspace_S_NFDBG_Base     ; The start of secure context.
    SaveState                               ; Save secure context.
    mov     sp, lr                          ; Restore monitor stack.

    ; Clear the entire register file. For each mode SPSR is left as it is.
    mov     r4,  #0
    mov     r5,  #0
    mov     r6,  #0
    mov     r7,  #0
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    cps     #CPSR_MODE_SYS
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_IRQ
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_SVC               ; Set up stack for non-secure handler.
    ldr     r0, =g_tzbsp_ns_debug_stack  ; Location of non-secure stack addr.
    ldr     r0, [r0]                     ; Address of non-secure stack.
    mov     r13, r0
    mov     r14, #0
    cps     #CPSR_MODE_ABT
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_UND
    mov     r13, #0
    mov     r14, #0
    cps     #CPSR_MODE_FIQ
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0
    mov     r13, #0
    mov     r14, #0

    cps     #CPSR_MODE_MON                  ; Switch back to monitor mode.

    ldr     lr, =g_tzbsp_ns_debug_vector    ; Location of non-secure jump addr.
    ldr     lr, [lr]                        ; Non-secure jump address.

    ; Construct non-secure CPSR. Non-secure debug FIQ handler will execute
    ; in SVC mode, IRQs and FIQs disabled.
    mov     r0, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ldr     r1, =g_tzbsp_ns_debug_thumb     ; Thumb mode status location.
    ldr     r1, [r1]                        ; Thumb mode status itself.
    cmp     r1, #1                          ; If 1, then non-secure...
    orreq   r0, #CPSR_T_BIT                 ; ...handler is Thumb code.
    msr     SPSR_cfsx, r0

    ldr     r0, =g_tzbsp_ns_debug_ctx   ; Location of debug context address.
    ldr     r0, [r0]    ; Address of debug context, passed to non-secure side.
    mov     r1, #0
    mov     r2, #0
    mov     r3, #0

    MonCallReturnToNonSecure r1 ; Jump to non-secure debug FIQ handler.

    ; When non-secure side returns control to TZ, it will continue from
    ; this label. Context is restored by tzbsp_smc_handler_nfdbg.
nfdbg_fiq_return

    ldmfd   sp!, {pc} ; Return to the debug FIQ handler.
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_init_monitor( void )
;===================================================================
;
; Initializes the Secure Monitor
;
tzbsp_init_monitor FUNCTION
    ; ------------------------------------------------------------------
    ; Change to monitor mode and program the Monitor vector table.
    ; ------------------------------------------------------------------
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    ldr     r0, =tzbsp_mon_vector_table
    mcr     p15, 0, r0, c12, c0, 1

    ; ------------------------------------------------------------------
    ; Initialize Non secure system control registers
    ; ------------------------------------------------------------------
    bl      tzbsp_non_secure_sysini

    ; Set up the CPU specific monitor stack.
    ldr     sp, =tzbsp_mon_stack_start ; load base address for stack
    mrc     p15, 0, r0, c0, c0, 5   ; Read MPIDR
    and     r0, r0, #0xFF           ; LSB is AFFL0 = CPU number.
    mov     r1, #TZBSP_MON_STACK_SIZE
    mul     r1, r0                  ; obtain stack offset for CPU
    sub     sp, r1                  ; shift stack to CPU specific stack

    ; Let's route FIQs to monitor to catch any spurious/unexpected FIQs during
    ; initialization.
    mov     r0, #SCR_FIQ_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb

    ; Change to Supervisor mode
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    ; Return to Secure World (SWD)
    bx      r14
    ENDFUNC

;===================================================================
; FUNCTION: void tzbsp_non_secure_sysini( void )
;===================================================================
;
; Initializes the non-secure banked registers. The full SYSINI is done in
; the secure world.
tzbsp_non_secure_sysini FUNCTION
    ; Copy the secure banked register for SCTLR. The banked copies for ACTLR
    ; and CCPR are handled by the sysini.
    mrc     p15, 0, r1, c1, c0, 0   ; Read secure SCTLR

    ; Switch to non-secure world.
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT:OR:SCR_NS_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb

    ; SCTLR is only partially banked between secure and
    ; non-secure worlds.
    mcr     p15, 0, r1, c1, c0, 0   ; Write SCTLR

    mov     r0, #0

    ; All fully banked CP15 registers have either zero or unpredictable
    ; HW reset value -> reset to zero.
    mcr     p15, 2, r0, c0, c0, 0   ; CSSELR
    mcr     p15, 0, r0, c2, c0, 0   ; TTBR0
    mcr     p15, 0, r0, c2, c0, 1   ; TTBR1
    mcr     p15, 0, r0, c2, c0, 2   ; TTBCR
    mcr     p15, 0, r0, c3, c0, 0   ; DACR
    mcr     p15, 0, r0, c5, c0, 0   ; DFSR
    mcr     p15, 0, r0, c5, c0, 1   ; IFSR
    mcr     p15, 0, r0, c5, c1, 0   ; ADFSR
    mcr     p15, 0, r0, c5, c1, 1   ; AIFSR
    mcr     p15, 0, r0, c6, c0, 0   ; DFAR
    mcr     p15, 0, r0, c6, c0, 2   ; IFAR
    mcr     p15, 0, r0, c7, c4, 0   ; PAR
    mcr     p15, 0, r0, c10, c2, 0  ; MAIR0 (was PRRR)
    mcr     p15, 0, r0, c10, c2, 1  ; MAIR1 (was NMRR)

    ldr     r1, =tzbsp_ns_vbar
    ldr     r0, [r1]
    mcr     p15, 0, r0, c12, c0, 0  ; VBAR

    ; Clear the VBAR to zero. The first value is passed through from
    ; SBL2.
    mov     r0, #0
    str     r0, [r1]

    mcr     p15, 0, r0, c13, c0, 0  ; FCSEIDR
    mcr     p15, 0, r0, c13, c0, 1  ; CONTEXTIDR
    mcr     p15, 0, r0, c13, c0, 2  ; TPIDRURW
    mcr     p15, 0, r0, c13, c0, 3  ; TPIDRURO
    mcr     p15, 0, r0, c13, c0, 4  ; TPIDRPRW

    ; Back to secure world.
    mov     r0, #SCR_FW_BIT:OR:SCR_AW_BIT
    mcr     p15, 0, r0, c1, c1, 0
    misb

    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION - void tzbsp_install_irq_function( void (*) (void) )
;===================================================================
;
; Function to set the IRQ intercept function. Setting this function
; allows TZBSP to inspect raised IRQs. The function is not supposed
; to acknowledge/end the IRQ interrupts as IRQ handling is done in
; non-secure side. If the function is set to NULL (default), then
; IRQ is handed to non-secure side without TZBSP intervention.
;
; Parameters:
; r0 - Contains the function pointer of type void (*) (void). The
;      function will be executed in monitor mode when invoked.
;
tzbsp_install_irq_function FUNCTION
    ldr     r1, =tzbsp_irq_function
    str     r0, [r1]
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION - void tzbsp_set_ns_return_addr( uint32 addr )
;===================================================================
;
; Parameters:
; r0 - Contains the new NS return address
;
tzbsp_set_ns_return_addr FUNCTION
    ldr     r2, =N_CPSR
    ldr     r1, [r2]
    Save_NS_PC_CPSR r0, r1, r2
    bx      lr
    ENDFUNC

;===================================================================
;FUNCTION-void tzbsp_setup_initial_ns_env(uint32 hypaddr, 
;                                         uint32 aboot_addr,
;                                         uint32 hyp_config_bmsk)
;===================================================================
;
; Parameters:
; r0 - Contains the new NS return address (hypervisor address)
; r1 - Contains the apps boot address
; r2 - Contains the hypervisor configuration bitmask
;
tzbsp_setup_initial_ns_env FUNCTION
    ldr     r3, =N_PC
    str     r0, [r3]
    ldr     r3, =N_USR_R0
    str     r1, [r3]
    ldr     r3, =N_USR_R1
    str     r2, [r3]
    ands    r0, r0, #0x1
    mov     r2, #CPSR_MODE_HYP:OR:CPSR_I_BIT
    beq     arm_code
    orr     r2, r2, #CPSR_T_BIT
arm_code
    ldr     r3, =N_CPSR
    str     r2, [r3]
    bx      lr
    ENDFUNC

;===================================================================
;FUNCTION-void tzbsp_setup_warmboot_ns_env(uint32 boot_addr)
;===================================================================
;
; Parameters:
; r0 - Contains the apps boot address
;
tzbsp_setup_warmboot_ns_env FUNCTION
    ldr     r3, =N_PC_WARMBOOT
    str     r0, [r3]
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION - void tzbsp_install_fiq_function( int (*) (void) )
;===================================================================
;
; Function to set the FIQ handler function. Setting this function
; allows TZBSP to handle raised FIQs. The function is supposed to
; acknowledge/end the FIQ interrupts. This function must always be
; set when receiving FIQs. That is, if FIQ is raised while the
; handler function is NULL, then the monitor will start waiting in
; an error loop.
;
; Parameters:
; r0 - Contains the function pointer of type void (*) (void). The
;      function will be executed in monitor mode when invoked.
;
tzbsp_install_fiq_function FUNCTION
    ldr     r1, =tzbsp_fiq_function
    str     r0, [r1]
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION - boolean tzbsp_is_non_secure(void)
;===================================================================
; Queries SCR[NS] state. Should only be used in monitor mode.
tzbsp_is_non_secure FUNCTION
    RCP15_SCR   r0
    and         r0, r0, #1              ; SCR[NS] is bit [0]
    bx          lr
    ENDFUNC

;======================================================================
; Allow PL1 access to the PL1 timer module
;======================================================================
tzbsp_set_cnthctl FUNCTION
    ; To modify the CNTHCTL register, we need to be in Monitor mode
    ; with the NS bit set
    msr   CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    mrc   p15, 0, r3, c1, c1, 0   ; Read the current SCR
    orr   r2, r3, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr   p15, 0, r2, c1, c1, 0   ; write SCR

    ; Bits to allow PL1 and PL2 to set the access permission for PL1 timer
    mov   r0, #0x3                ; Set the PL1PCEN and PL1PCTEN bits
    mcr   p15, 4, r0, c14, c1, 0  ; Write to CNTHCTL register

    mcr   p15, 0, r3, c1, c1, 0   ; write SCR back to original
    msr   CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    bx    lr
    ENDFUNC

;======================================================================
; Sets CNTFRQ, so as to be readable by PL0 and PL1
;
; r0  contains the frequency
;======================================================================
tzbsp_set_cntfrq FUNCTION
    mcr   p15, 0, r0, c14, c0, 0
    bx    lr
    ENDFUNC

;============================================================================
; TZBSP_MMU_DISABLE_AND_CONFIGURE_NS
;
; ARGS
;
;  DESCRIPTION -
;        Disables the secure MMU and, after copying the configuration to
;        non-secure world, enables the non-secure MMU.
;        Performs the following:
;                1- Disable I and D caches, and flush all caches
;                2- Disable secure MMU and invalidate TLB
;                3- Switch to Monitor mode, set NS bit, and copy TTBR0, TTBCR,
;                   and DACR to non-secure MMU
;                4- Enable I and D caches, enable MMU
;                5- Clear NS bit, switch to SVC, and remove TZ mappings from
;                   SBL pagetable (taken from TTBR0)
;============================================================================
tzbsp_mmu_disable_and_configure_ns FUNCTION
    stmfd   sp!, {r4, lr}

    bl      tzbsp_mmu_cache_disable

    ; Set the non-secure Translation Table registers to be the same
    ; as the current secure registers.  This requires being in Monitor
    ; mode and toggling the SCR.NS bit
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    ; Read the TT registers
    mrc     p15, 0, r0, c2, c0, 0   ; TTBR0
    mrc     p15, 0, r1, c2, c0, 2   ; TTBCR
    mrc     p15, 0, r2, c3, c0, 0   ; DACR

    ; Switch to NS mode
    mrc     p15, 0, r4, c1, c1, 0   ; Read the current SCR
    orr     r3, r4, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr     p15, 0, r3, c1, c1, 0   ; write SCR

    ; Write the NS TT registers, and enable the MMU and caches
    mcr     p15, 0, r0, c2, c0, 0   ; TTBR0
    mcr     p15, 0, r1, c2, c0, 2   ; TTBCR
    mcr     p15, 0, r2, c3, c0, 0   ; DACR
    mov     r3, r0                  ; Save TTBR0 for later use, the follow 
                                    ; macros use r0

    bl      tzbsp_mmu_cache_enable_no_cfg

    mcr     p15, 0, r4, c1, c1, 0   ; write SCR back to original
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    ; Remove the TZ entries in the SBL pagetable
    mov     r0, r3
    blx     mmu_remove_tz_mapping   ; r0 holds Level-1 pagetable address

    ldmfd   sp!, {r4, pc}
    ENDFUNC
    
;============================================================================
; TZBSP_DISABLE_NS_MMU_AFTER_SBL
;
; ARGS
;
;  DESCRIPTION -
;       The following functions are performed:
;               1- Unlock the L2 cache lines
;               2- Clean and Invalidate L2
;               3- Invalidate TLB
;               4- Disable the non-secure MMU
;============================================================================
tzbsp_disable_ns_mmu_after_sbl FUNCTION
    stmfd   sp!, {r4-r11,lr}  ; Save the entire register map, since sysini doesn't

    ; Disable the non-secure MMU; this requires being in Monitor
    ; mode and toggling the SCR.NS bit
    msr     CPSR_c, #CPSR_MODE_MON:OR:CPSR_I_BIT:OR:CPSR_F_BIT
    mrc     p15, 0, r3, c1, c1, 0   ; Read the current SCR
    orr     r2, r3, #SCR_NS_BIT     ; Set SCR.NS=1
    mcr     p15, 0, r2, c1, c1, 0   ; write SCR

    bl      tzbsp_mmu_disable

    ; Zero out the TTBR0
    mov     r0, #0
    mcr     p15, 0, r0, c2, c0, 0

    mcr     p15, 0, r3, c1, c1, 0   ; write SCR back to original
    msr     CPSR_c, #CPSR_MODE_SVC:OR:CPSR_I_BIT:OR:CPSR_F_BIT

    bl      tzbsp_mmu_cache_disable

    ; Do sysini with L2 invalidation.
    mov     r0, #1
    blx     sysini_wrapper

    bl      tzbsp_mmu_cache_enable_no_cfg

    ldmfd   sp!, {r4-r11,pc}
    ENDFUNC

;============================================================================
;   FUNCTION: SYSDBG_SAVE_CTX
;============================================================================
;   Handles the saving the ctxt for sysdbg using existing TZ functions
;   Save necessary current context 

sysdbg_save_ctx FUNCTION

     ; Keep back up of LR & SP before they get clobbered.
    ldr   r0, =SP_BACKUP
    str   r13, [r0]
    ldr   r0, =LR_BACKUP
    str   r14, [r0]
    MRC   p15, 0, r0, c13, c0, 2
    MRC   p15, 0, r14, c13, c0, 4

    ldr   r13, =Sysdbg_Workspace_N_Base 

    SaveStateSDI

    cps     #CPSR_MODE_SVC
    ldr   r14, =SP_BACKUP
    ldr   r14, [r14]
    str   r14, [r13, #0x10]

    ldr   r14, =LR_BACKUP
    ldr   r14, [r14]
    bx    lr
    
ENDFUNC


;============================================================================
;
;                             MODULE DATA
;
;============================================================================

;===================================================================
; Monitor Workspace
;===================================================================
;
; Workspace for storing the non-secure and secure processor
; contexts. Only one call context is supported, hence only one CPU
; at any given time can execute secure side code. That is, the
; secure side is not re-entrant.
;
    AREA |MonitorWorkspaceDataArea|, DATA, READWRITE, ALIGN=2

; Using directive SPACE will allocate zeroed bytes.
MonitorWorkspaceBase
Workspace_N_Begin

N_PC           SPACE 0x4    ; PC to load to return to Normal World
N_CPSR         SPACE 0x4    ; CPSR to load when returning to Normal Worl
N_USR_R0       SPACE 0x4    ; Normal world registers
N_USR_R1       SPACE 0x4
N_USR_R2       SPACE 0x4
N_USR_R3       SPACE 0x4
N_USR_R4       SPACE 0x4
N_USR_R5       SPACE 0x4
N_USR_R6       SPACE 0x4
N_USR_R7       SPACE 0x4
N_USR_R8       SPACE 0x4
N_USR_R9       SPACE 0x4
N_USR_R10      SPACE 0x4
N_USR_R11      SPACE 0x4
N_USR_R12      SPACE 0x4
N_USR_R13      SPACE 0x4
N_USR_R14      SPACE 0x4

Workspace_N_Base

N_IRQ_SPSR     SPACE 0x4    ; Normal banked registers
N_IRQ_R13      SPACE 0x4
N_IRQ_R14      SPACE 0x4
N_SVC_SPSR     SPACE 0x4
N_SVC_R13      SPACE 0x4
N_SVC_R14      SPACE 0x4
N_ABT_SPSR     SPACE 0x4
N_ABT_R13      SPACE 0x4
N_ABT_R14      SPACE 0x4
N_UND_SPSR     SPACE 0x4
N_UND_R13      SPACE 0x4
N_UND_R14      SPACE 0x4
N_FIQ_SPSR     SPACE 0x4
N_FIQ_R8       SPACE 0x4
N_FIQ_R9       SPACE 0x4
N_FIQ_R10      SPACE 0x4
N_FIQ_R11      SPACE 0x4
N_FIQ_R12      SPACE 0x4
N_FIQ_R13      SPACE 0x4
N_FIQ_R14      SPACE 0x4

Workspace_N_End
Workspace_S_Begin

S_PC           SPACE 0x4    ; PC to load to return to Secure World
S_CPSR         SPACE 0x4    ; CPSR to load to return to Secure World
S_USR_R0       SPACE 0x4    ; Secure World registers
S_USR_R1       SPACE 0x4
S_USR_R2       SPACE 0x4
S_USR_R3       SPACE 0x4
S_USR_R4       SPACE 0x4
S_USR_R5       SPACE 0x4
S_USR_R6       SPACE 0x4
S_USR_R7       SPACE 0x4
S_USR_R8       SPACE 0x4
S_USR_R9       SPACE 0x4
S_USR_R10      SPACE 0x4
S_USR_R11      SPACE 0x4
S_USR_R12      SPACE 0x4
S_USR_R13      SPACE 0x4
S_USR_R14      SPACE 0x4

Workspace_S_Base

S_IRQ_SPSR     SPACE 0x4    ; Secure World banked registers
S_IRQ_R13      SPACE 0x4
S_IRQ_R14      SPACE 0x4
S_SVC_SPSR     SPACE 0x4
S_SVC_R13      SPACE 0x4
S_SVC_R14      SPACE 0x4
S_ABT_SPSR     SPACE 0x4
S_ABT_R13      SPACE 0x4
S_ABT_R14      SPACE 0x4
S_UND_SPSR     SPACE 0x4
S_UND_R13      SPACE 0x4
S_UND_R14      SPACE 0x4
S_FIQ_SPSR     SPACE 0x4
S_FIQ_R8       SPACE 0x4
S_FIQ_R9       SPACE 0x4
S_FIQ_R10      SPACE 0x4
S_FIQ_R11      SPACE 0x4
S_FIQ_R12      SPACE 0x4
S_FIQ_R13      SPACE 0x4
S_FIQ_R14      SPACE 0x4

Workspace_S_END

Workspace_S_NFDBG_Begin

S_NFDBG_PC           SPACE 0x4    ; PC to load to return to Secure World
S_NFDBG_CPSR         SPACE 0x4    ; CPSR to load to return to Secure World
S_NFDBG_USR_R0       SPACE 0x4    ; Secure World registers
S_NFDBG_USR_R1       SPACE 0x4
S_NFDBG_USR_R2       SPACE 0x4
S_NFDBG_USR_R3       SPACE 0x4
S_NFDBG_USR_R4       SPACE 0x4
S_NFDBG_USR_R5       SPACE 0x4
S_NFDBG_USR_R6       SPACE 0x4
S_NFDBG_USR_R7       SPACE 0x4
S_NFDBG_USR_R8       SPACE 0x4
S_NFDBG_USR_R9       SPACE 0x4
S_NFDBG_USR_R10      SPACE 0x4
S_NFDBG_USR_R11      SPACE 0x4
S_NFDBG_USR_R12      SPACE 0x4
S_NFDBG_USR_R13      SPACE 0x4
S_NFDBG_USR_R14      SPACE 0x4

Workspace_S_NFDBG_Base

S_NFDBG_IRQ_SPSR     SPACE 0x4    ; Secure World banked registers
S_NFDBG_IRQ_R13      SPACE 0x4
S_NFDBG_IRQ_R14      SPACE 0x4
S_NFDBG_SVC_SPSR     SPACE 0x4
S_NFDBG_SVC_R13      SPACE 0x4
S_NFDBG_SVC_R14      SPACE 0x4
S_NFDBG_ABT_SPSR     SPACE 0x4
S_NFDBG_ABT_R13      SPACE 0x4
S_NFDBG_ABT_R14      SPACE 0x4
S_NFDBG_UND_SPSR     SPACE 0x4
S_NFDBG_UND_R13      SPACE 0x4
S_NFDBG_UND_R14      SPACE 0x4
S_NFDBG_FIQ_SPSR     SPACE 0x4
S_NFDBG_FIQ_R8       SPACE 0x4
S_NFDBG_FIQ_R9       SPACE 0x4
S_NFDBG_FIQ_R10      SPACE 0x4
S_NFDBG_FIQ_R11      SPACE 0x4
S_NFDBG_FIQ_R12      SPACE 0x4
S_NFDBG_FIQ_R13      SPACE 0x4
S_NFDBG_FIQ_R14      SPACE 0x4

MonitorWorkspaceEnd

tzbsp_irq          SPACE 0x4 ; IRQ status, 0: no IRQ, !0: waiting IRQ return.
tzbsp_irq_function SPACE 0x4 ; IRQ intercept function address, can be NULL.
tzbsp_fiq_function SPACE 0x4 ; FIQ handler function address, cannot be NULL.
tzbsp_ns_vbar      SPACE 0x4 ; Address of the non-secure VBAR.
; Reserve space for SCR for each CPU for FIQ handling.
tzbsp_scr          SPACE 0x4 * TZBSP_CPU_COUNT_MAX

;===================================================================
; Monitor Stack
;===================================================================
;
; Monitor stack memory for each CPU. Stacks are full descending; the
; stack grows downwards and the current value of sp points to the
; last taken stack item. The stacks are 8 byte aligned.
;
; The same stack area is used by monitor for SMC, IRQ and FIQ
; handling.
;
; The stack area is aligned to Krait cache line size, which is 128 bytes (2^8).
    AREA |MonitorStack|, DATA, READWRITE, ALIGN=8
tzbsp_mon_stack_end     SPACE   TZBSP_MON_STACK_SIZE * TZBSP_CPU_COUNT_MAX
tzbsp_mon_stack_start   DCD     0xdeadbeef

;===================================================================
; Stores the context during an FIQ (taken in monitor mode)
;===================================================================
;
; Workspace for storing context of an interrupted process
;
    AREA |FIQWorkspaceDataArea|, DATA, READWRITE, ALIGN=2

Workspace_Mon_Begin

S_MON_IP       SPACE 0x4    ; Interrupted context IP
S_MON_SPSR     SPACE 0x4    ; CPSR of the interruped context

Workspace_Mon_Registers

S_MON_R0       SPACE 0x4    ; Interrupted context registers
S_MON_R1       SPACE 0x4
S_MON_R2       SPACE 0x4
S_MON_R3       SPACE 0x4
S_MON_R4       SPACE 0x4
S_MON_R5       SPACE 0x4
S_MON_R6       SPACE 0x4
S_MON_R7       SPACE 0x4
S_MON_R8       SPACE 0x4
S_MON_R9       SPACE 0x4
S_MON_R10      SPACE 0x4
S_MON_R11      SPACE 0x4
S_MON_R12      SPACE 0x4
S_MON_R13      SPACE 0x4
S_MON_R14      SPACE 0x4


N_PC_WARMBOOT  SPACE 0x4 ;Warmboot Apps address to jump to

;===================================================================
; TZ locks
;===================================================================
; Krait SWI, CTR[23:20], RGSIZE[3:0], Reservation Granule Size,
; Log2 of words in the reservation granule, reset value = 0x5 (32 words)
; Place the logs on separate L1 cache lines.  For Krait, line size is 64 bytes.
    AREA |MonitorLock|, DATA, READWRITE, ALIGN=7
tzbsp_smc_lock      SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler0             SPACE 0x4 * 31

    AREA TZBSP_UNCACHED_ZI_LOCKS, DATA, READWRITE, ALIGN=7
tzbsp_boot_lock     SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler1             SPACE 0x4 * 31
g_tzbsp_clk_mutex   SPACE 0x4 ; Spinlock mem, 0: not busy, 1: is busy.
filler3             SPACE 0x4 * 31
g_tzbsp_fiq_conf_mutex SPACE 0x4 ; Spinlock for FIQ configuration 
filler4             SPACE 0x4 * 31
g_tzbsp_timer_list_mutex SPACE 0x4 ; Spinlock for timer list management 
filler5             SPACE 0x4 * 31

    END
