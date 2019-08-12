;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                              SBL1
;
; GENERAL DESCRIPTION
;   This file bootstraps the processor. The Start-up Primary Bootloader
;   (SBL1) performs the following functions:
;
;      - Set up the hardware to continue boot process.
;      - Initialize DDR memory
;      - Load Trust-Zone OS
;      - Load RPM firmware
;      - Load APPSBL and continue boot process
;
;   The SBL1 is written to perform the above functions with optimal speed.
;   It also attempts to minimize the execution time and hence reduce boot time.
;
; Copyright 2010-2013 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1.s#1 $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 12/12/14   jz      Fixed data abort issue when restoring r6 in sbl_save_regs
; 09/15/14   jz      Updated to use PBL stack for SVC mode
; 08/20/14   jz      Enable asynchronous abort (the A bit in CPSR)
; 07/28/14   jz      Ported fix for the abort modes so that the r14 is not corrupted
; 05/01/14   ck      Added logic to assign stacks based on processor mode
; 03/19/14   ck      Fixed stack base issue.  Now using proper address which is "Limit" of SBL1_STACK
; 03/07/14   ck      Removed -4 logic from check_for_nesting as bear SBL has its own vector table
; 03/03/14   ck      Updated vector table with branches as VBAR is being used in Bear family 
; 10/18/13   kedara  Fix registers that are saved when enter in exception modes
; 07/10/13   aus     Added support to save processor registers
; 06/19/13   kedara  Initial revision for 9x35
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "boot_msm.h"
#include "boot_target.h"

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;
Mode_SVC                EQU    0x13
Mode_ABT                EQU    0x17
Mode_UND                EQU    0x1b
Mode_USR                EQU    0x10
Mode_FIQ                EQU    0x11
Mode_IRQ                EQU    0x12
Mode_SYS                EQU    0x1F

I_Bit                   EQU    0x80
F_Bit                   EQU    0x40

SBL_ERR_UNDEF_INSTR     EQU    0x3027
SBL_ERR_PREFETCH_ABORT  EQU    0x3029
SBL_ERR_DATA_ABORT      EQU    0x302A

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT |Image$$SBL1_UND_STACK$$ZI$$Limit|
    IMPORT |Image$$SBL1_ABT_STACK$$ZI$$Limit|
    IMPORT boot_swi_c_handler
    IMPORT boot_reserved_c_handler
    IMPORT boot_irq_c_handler
    IMPORT boot_fiq_c_handler
    IMPORT boot_nested_exception_c_handler
    IMPORT boot_error_handler	
    IMPORT sbl1_main_ctl
    IMPORT boot_crash_dump_regs_ptr

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the external symbols that are referenced in this module.
    EXPORT sbl_loop_here
   
    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main
    EXPORT  sbl_save_regs
    
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

;
; Area that defines the register data structure
;
    AREA    SAVE_REGS, DATA

arm_core_t RN     r7 
           MAP    0,arm_core_t

svc_r0     FIELD  4
svc_r1     FIELD  4
svc_r2     FIELD  4
svc_r3     FIELD  4
svc_r4     FIELD  4
svc_r5     FIELD  4
svc_r6     FIELD  4
svc_r7     FIELD  4
svc_r8     FIELD  4
svc_r9     FIELD  4
svc_r10    FIELD  4
svc_r11    FIELD  4
svc_r12    FIELD  4
svc_sp     FIELD  4
svc_lr     FIELD  4
svc_spsr   FIELD  4
svc_pc     FIELD  4
cpsr       FIELD  4
sys_sp     FIELD  4
sys_lr     FIELD  4
irq_sp     FIELD  4
irq_lr     FIELD  4
irq_spsr   FIELD  4
abt_sp     FIELD  4
abt_lr     FIELD  4
abt_spsr   FIELD  4
udf_sp     FIELD  4
udf_lr     FIELD  4
udf_spsr   FIELD  4
fiq_r8     FIELD  4
fiq_r9     FIELD  4
fiq_r10    FIELD  4
fiq_r11    FIELD  4
fiq_r12    FIELD  4
fiq_sp     FIELD  4
fiq_lr     FIELD  4
fiq_spsr   FIELD  4

    PRESERVE8
    AREA    SBL1_VECTORS, CODE, READONLY, ALIGN=4
    CODE32
unused_reset_vector
    B     0x00000000
undefined_instruction_vector
    B     sbl1_undefined_instruction_nested_handler
swi_vector
    B     boot_swi_c_handler
prefetch_abort_vector
    B     sbl1_prefetch_abort_nested_handler
data_abort_vector
    B     sbl1_data_abort_nested_handler
reserved_vector
    B     boot_reserved_c_handler
irq_vector
    B     boot_irq_c_handler
fiq_vector
    B     boot_fiq_c_handler


;============================================================================
; Qualcomm SECONDARY BOOT LOADER 1 ENTRY POINT
;============================================================================

    AREA  SBL1_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
    ENTRY
    
__main
_main

;============================================================================
;   We contiue to disable interrupt and watch dog until we jump to apps
;============================================================================
sbl1_entry

    ;Change to Supervisor Mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Save the passing parameter from PBL
    mov     r7, r0

    ; Set VBAR (Vector Base Address Register) to SBL vector table
    ldr     r0, =SCL_SBL1_VECTOR_BASE
    MCR     p15, 0, r0, c12, c0, 0
	
    ; Setup the supervisor mode stack (uses PBL stack)
    ldr     r0, =BOOT_PBL_STACK_BASE
    mov     r13, r0

    ; Switch to undefined mode and setup the undefined mode stack
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    ldr     r0, =|Image$$SBL1_UND_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Switch to abort mode and setup the abort mode stack
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    ldr     r0, =|Image$$SBL1_ABT_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Return to supervisor mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Restore the passing parameter
    mov     r0, r7
    
    ; Disable Sparrow boot remapper. this is required to be able
    ; to access DDR at address 0x00000000 and not have this address
    ; translated
    ldr     r4, =HWIO_APCS_GLB_BOOT_REMAP_ADDR
    ldr     r5, [r4]
    ; Write 0 to lsb 2 bits to disable remapper
    bic     r5, r5, #0x3
    str     r5, [r4]
    
    ; Enable asynchronous abort
    CPSIE A
    
    ; ------------------------------------------------------------------
    ; Call functions external to perform SBL1 function.
    ; It should never return.
    ; ------------------------------------------------------------------
    ldr    r5, =sbl1_main_ctl
    blx    r5

    ; For safety
    bl loophere  ; never returns, keep lr in r14 for debug


;============================================================================
; sbl_save_regs
;
; PROTOTYPE
;   void sbl_save_regs();
;
; ARGS
;   None
;
; DESCRIPTION
;   Configure VBAR, vector table base register.
;   
;============================================================================    
sbl_save_regs

    ; Save CPSR
    stmfd   sp!, {r6}
    mrs     r6, CPSR

    stmfd   sp!, {r7}

    ; Switch to SVC mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Capture User Mode r0-r14 (no SPSR)
    ; Registers are stored in svc structure for backwards compatibility
    ldr     arm_core_t,=boot_crash_dump_regs_ptr
    ldr     arm_core_t, [r7]
    str     r0,  svc_r0
    str     r1,  svc_r1
    str     r2,  svc_r2
    str     r3,  svc_r3
    str     r4,  svc_r4
    str     r5,  svc_r5
    ; Store r6 later after restoring it from the stack
    ; Store r7 later after restoring it from the stack
    str     r8,  svc_r8
    str     r9,  svc_r9
    str     r10, svc_r10
    str     r11, svc_r11    
    str     r12, svc_r12
    str     r14, svc_lr

    ; Store SP value
    str     sp, svc_sp

    ; Store SPSR
    mrs     r0, SPSR
    str     r0, svc_spsr

    ; Store the PC for restoration later
    ldr     r0, =sbl_save_regs
    str     r0, svc_pc

    ; Save SYS mode registers
    msr     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
    str     r13, sys_sp
    str     r14, sys_lr

    ; Save IRQ mode registers
    msr     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
    str     r13, irq_sp
    str     r14, irq_lr
    mrs     r0, SPSR
    str     r0, irq_spsr    

    ; Save ABT mode registers
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    str     r13, abt_sp
    str     r14, abt_lr
    mrs     r0, SPSR
    str     r0, abt_spsr  

    ; Save UND mode registers
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    str     r13, udf_sp
    str     r14, udf_lr
    mrs     r0, SPSR
    str     r0, udf_spsr  

    ; Save FIQ mode registers
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    str     r8,  fiq_r8
    str     r9,  fiq_r9
    str     r10, fiq_r10
    str     r11, fiq_r11    
    str     r12, fiq_r12    
    str     r13, fiq_sp
    str     r14, fiq_lr
    mrs     r0, SPSR
    str     r0, fiq_spsr  

    ; Switch back to original mode using r6 which holds the cpsr
    msr     CPSR_c, r6

    ; Store CPSR
    str     r6, cpsr

    ; Restore r7 value 
    ldmfd   sp!, {r3}
    str     r3, svc_r7

    ; Restore r6 value
    ldmfd   sp!, {r2}
    str     r2, svc_r6
    mov     r6, r2
    mov     r7, r3


    ; Finished so return
    bx      lr

;======================================================================
; Called by sbl1_error_handler only. We clean up the registers and loop
; here until JTAG is connected.
;======================================================================
sbl_loop_here
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
loophere
    b loophere


;======================================================================
; SBL1 exception handlers that can have nested calls to them.  These
; handlers check for nesting and if it is the first exception they
; call a "C" exception handler that calls the SBL1 error handler.
; If it is a nested exception, the "C" exception handler is not
; re-entered and the JTAG interface is enabled immediately. Nesting
; is only a concern for undefined instruction and abort exceptions.
; Note, a separate exception handler is used for each exception to
; provide additional debug information (see sbl1_error_handler.c for
; more information).
;======================================================================
sbl1_undefined_instruction_handler
    mov r0,#0x0
    mov r1,#0x0
    mov r2,#SBL_ERR_UNDEF_INSTR
    ldr r4,=boot_error_handler
    bx r4
	
sbl1_prefetch_abort_handler
    mov r0,#0x0
    mov r1,#0x0
    mov r2,#SBL_ERR_PREFETCH_ABORT
    ldr r4,=boot_error_handler
    bx r4	

sbl1_data_abort_handler
    mov r0,#0x0
    mov r1,#0x0
    mov r2,#SBL_ERR_DATA_ABORT
    ldr r4,=boot_error_handler
    bx r4

sbl1_undefined_instruction_nested_handler
    ldr r4,=sbl1_undefined_instruction_handler
    b   check_for_nesting

sbl1_prefetch_abort_nested_handler
    ldr r4,=sbl1_prefetch_abort_handler
    b   check_for_nesting

sbl1_data_abort_nested_handler
    ldr r4,=sbl1_data_abort_handler
    b   check_for_nesting

;======================================================================
; Checks for nested exceptions and then calls the "C" exception
; handler pointed to by R5 if this is the first time this exception
; has occurred, otherwise it calls the "C" nested exception handler
; that just enables JTAG debug access.  The mode stack pointer is used
; to determine if a nested exception or a second abort exception has
; occurred.  This is accomplished by comparing the mode stack pointer
; to the top of the stack that was initially assigned to the stack.
; If they are equal, it is a first time exception.
;======================================================================
check_for_nesting

    ; Initial stack base depends on the current processor mode
    ; Mode will either be ABT or UND.  Load proper stack base.
    mrs r7, cpsr
    and r7, r7, #Mode_SYS ; Use Mode_SYS for mode bitmask as all bits are high
	
    ldreq r6,=|Image$$SBL1_ABT_STACK$$ZI$$Limit|
    cmp r7, #Mode_UND
    ldreq r6,=|Image$$SBL1_UND_STACK$$ZI$$Limit|

    mov r7, r13                            ; Save current stack ptr
    cmp r6, r7                             ; Compare initial and actual
    bxeq r4                                ; First time exception
    ldr r4,=boot_nested_exception_c_handler; This is a nested exception
    bx r4

    END
