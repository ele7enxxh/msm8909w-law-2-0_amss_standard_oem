;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;             System Debug Image Initialization Routine
;
; GENERAL DESCRIPTION
;	This file contains initialization code for the system_debug image.
;       This performs a dump of the CPU core information as well.
;
; Copyright 2014 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when         who   what, where, why
; ----------   ---   --------------------------------------------------------
; 03/13/2014   ck    Save r0 and r1 values that were stored by PBL 
; 03/03/2014   ck    Pull PS_HOLD high at start because PBL is not doing it
; 02/18/2014   ck    Removed duplicate 32 to 64 bit switch logic
;                    Single copy is stored in SDI code space and shared
; 02/06/2014   ck    Added A53 32 to 64 bit logic switch logic 
; 01/02/2014   ck    Initial release for Bear family
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "msmhwioreg.h"
	

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;
Mode_USR                EQU    0x10
Mode_FIQ                EQU    0x11
Mode_IRQ                EQU    0x12
Mode_SVC                EQU    0x13
Mode_MON		EQU    0x16
Mode_ABT                EQU    0x17
Mode_HYP                EQU    0x1A
Mode_UND                EQU    0x1B
Mode_SYS                EQU    0x1F

I_Bit                   EQU    0x80
F_Bit                   EQU    0x40

; Secure Configuration Register Bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW)
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW)



;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT sys_debug_undefined_c_handler
    IMPORT sys_debug_swi_c_handler
    IMPORT sys_debug_prefetch_abort_c_handler
    IMPORT sys_debug_data_abort_c_handler
    IMPORT sys_debug_reserved_c_handler
    IMPORT sys_debug_irq_c_handler
    IMPORT sys_debug_fiq_c_handler

    ; Main Entry point of C code
    IMPORT sys_debug_main_ctl

	
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================


    PRESERVE8
    AREA    SYSTEM_DEBUG_VECTOR_TABLE, CODE, READWRITE
    CODE32
unused_reset_vector
    DCD     0x00000000 
undefined_instruction_vector
    DCD     sys_debug_undefined_c_handler
swi_vector
    DCD     sys_debug_swi_c_handler
prefetch_abort_vector
    DCD     sys_debug_prefetch_abort_c_handler
data_abort_vector
    DCD     sys_debug_data_abort_c_handler
reserved_vector
    DCD     sys_debug_reserved_c_handler
irq_vector
    DCD     sys_debug_irq_c_handler
fiq_vector
    DCD     sys_debug_fiq_c_handler


;============================================================================
; Qualcomm System Debug Image ENTRY POINT
;============================================================================

    AREA  SYSTEM_DEBUG_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
    
; This function is written specifically to match the sys_debug_cpu_ctxt_regs
; structure.  If the structure changes this function must change.
; The structure has 64 bit values so each value must be stored individually.
; 
sys_debug_entry
    ; We have two registers to play with 	
    ; r0, r1 have been saved by the PBL

    ; Always start in monitor mode.
    msr	CPSR_c,	#Mode_MON:OR:I_Bit:OR:F_Bit

    ; PS_HOLD needs to be pulled high to fix hardware issue
    ldr r0, =HWIO_MPM2_MPM_PS_HOLD_ADDR
    ldr r1, =HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK
    str r1, [r0]
	
    ; Keep r0 as the base address of the structure and use r1 as current address
    ldr r0, =sys_debug_cpu_ctxt_base
    mov r1, r0
	
    ; Store r2 - r12 taking into account the 64 bit structure
    ; r0 and r1 will be saved later.  These were stored by PBL. 
    add r1, r1, #8
    add r1, r1, #8
    str r2, [r1]
    add r1, r1, #8
    str r3, [r1]
    add r1, r1, #8
    str r4, [r1]
    add r1, r1, #8
    str r5, [r1]
    add r1, r1, #8
    str r6, [r1]
    add r1, r1, #8
    str r7, [r1]
    add r1, r1, #8
    str r8, [r1]
    add r1, r1, #8
    str r9, [r1]
    add r1, r1, #8
    str r10, [r1]
    add r1, r1, #8
    str r11, [r1]
    add r1, r1, #8
    str r12, [r1]
    add r1, r1, #8

    ; Switch to SYS mode so we can get r13 and r14 from USR mode.
    ; SYS mode uses the same registers as USR but maintains privliges.
    cps #Mode_SYS
    str r13, [r1]	
    add r1, r1, #8
    str r14, [r1]
    add r1, r1, #8

    ; HYP mode only exists in NS world.
    ; Skipping collection of r13 HYP for now.
    add r1, r1, #8

    ; Store r14 and r13 for IRQ Mode
    cps #Mode_IRQ
    str r14, [r1]
    add r1, r1, #8
    str r13, [r1]
    add r1, r1, #8

    ; Store r14 and r13 for SVC Mode
    cps #Mode_SVC
    str r14, [r1]
    add r1, r1, #8
    str r13, [r1]
    add r1, r1, #8

    ; Store r14 and r13 for ABT Mode
    cps #Mode_ABT
    str r14, [r1]
    add r1, r1, #8
    str r13, [r1]
    add r1, r1, #8
	
    ; Store r14 and r13 for UND Mode
    cps #Mode_UND
    str r14, [r1]
    add r1, r1, #8
    str r13, [r1]
    add r1, r1, #8

    ; Store r8 - r14 for FIQ Mode
    cps #Mode_FIQ
    str r8, [r1]
    add r1, r1, #8
    str r9, [r1]
    add r1, r1, #8
    str r10, [r1]
    add r1, r1, #8
    str r11, [r1]
    add r1, r1, #8
    str r12, [r1]
    add r1, r1, #8
    str r13, [r1]
    add r1, r1, #8
    str r14, [r1]
    add r1, r1, #8

    ; Skip the next entry in the struct.  This is PC and will be filled in later
    add r1, r1, #8

    ; Store r13 and r14 for MON Mode
    cps #Mode_MON
    str r13, [r1]
    add r1, r1, #8
    str r14, [r1]
    add r1, r1, #8

    ; HYP mode only exists in NS world.
    ; Skipping collection of r14 HYP for now.
    add r1, r1, #8
		
    ; Switch back to SVC Mode before continuing
    cps #Mode_SVC

    ; Save r0 and r1 that was stored by PBL
    ; r0 was stored to the Process id register
    MRC p15, 0, r2, c13, c0, 2
    ; r1 was stored to TPIDRPRW
    MRC p15, 0, r3, c13, c0, 4
    ; Reset structure pointer back to beginning 
    mov r1, r0
    str r2, [r1]
    add r1, r1, #8
    str r3, [r1]
	
    ; Set the stack pointer
    ldr r13, =sys_debug_cpu_stack_top

    ; We have saved all the registers for this CPU.
    ; Now we can branch to the main loop passing the ctxt structure address
    ; We will not return
    ldr r5,  =sys_debug_main_ctl
    blx r5

    ; For safety	
    bl loophere  ; never returns, keep lr in r14 for debug	

	
loophere
    bx r14

	
;=======================================================================
;
;		Data for the module 
;
;=======================================================================
    AREA SYSTEM_DEBUG_DATA , DATA, READWRITE
; Allocate memory for sys_debug_cpu_ctxt_type
; Round up big so if structure changes it does not overflow
sys_debug_cpu_ctxt_base      SPACE 0x200

; Alocate memory in RW data area for the stack 
sys_debug_cpu_stack_bottom   SPACE 0x110
sys_debug_cpu_stack_top      SPACE 0x4

END
