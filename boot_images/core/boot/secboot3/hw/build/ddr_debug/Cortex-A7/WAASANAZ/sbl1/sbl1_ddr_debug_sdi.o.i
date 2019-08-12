;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                              SBL1 DDR Debug
;
; GENERAL DESCRIPTION
;   This file bootstraps the processor. The Start-up
;   (SBL1_ddr_debug) performs the following functions:
;
;      - Set up the hardware to continue boot process.
;      - Initialize DDR memory
;      - Copies SBL1 to CODERAM
;      - Sets up stack in WDOG reset path .
;      - Jumps to OCIMEM to execute WDOG reset path
;
;   The SBL1_ddr_debug is written to perform the above functions with optimal speed.
;   It also attempts to minimize the execution time and hence reduce boot time.
;
; Copyright 2013 by Qualcomm Technologies, Incorporated.All Rights Reserved.
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
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 07/14/14   yps     Added sbl1_external_abort_enable funtion for DDI build
; 05/14/14   yps     Porting code to 8916 platform.
; 09/13/13   sl      Cleaned up exception handlers.
; 07/17/13   sr      Initial Version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
;#define ((0x004a0000 + 0x0000b000) + 0x00000000) 0x004AB000
;#define 0x1 0x01
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
Mode_MON		EQU    0x16
I_Bit                   EQU    0x80
F_Bit                   EQU    0x40
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    ; Import the external symbols that are referenced in this module.
    IMPORT sbl1_wdogpath_main_ctl
    PRESERVE8
    AREA    SYSTEM_DEBUG_VECTOR_TABLE, CODE, READWRITE
    CODE32
unused_reset_vector
    B     0x00000000
undefined_instruction_vector
    B     loophere
swi_vector
    B     loophere
prefetch_abort_vector
    B     loophere
data_abort_vector
    B     loophere
reserved_vector
    B     loophere
irq_vector
    B     loophere
fiq_vector
    B     loophere
;============================================================================
; Qualcomm IMEM INITIAL INSTRUCTIONS AFTER WDOG RESET ENTRY POINT
;============================================================================
    AREA  WDOG_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
wdog_entry
    ; We have two registers to play with 	
    ; r0, r1 have been saved by the PBL
    ; Always start in monitor mode.
    ;msr	CPSR_c,	#Mode_MON:OR:I_Bit:OR:F_Bit
    ; PS_HOLD needs to be pulled high to fix hardware issue
    ldr r0, =((0x004a0000 + 0x0000b000) + 0x00000000)
    ldr r1, =0x1
    str r1, [r0]
debug_loop 
     b debug_loop
    ldr r13, =(0x08005800 + 0x0040000)
    ldr r5, =(sbl1_wdogpath_main_ctl-0x8005800+0x5000+0x200000-1)
    blx r5
loophere
    b loophere
    END
