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
    IMPORT ddr_debug_init
    IMPORT sbl1_wdogpath_main_ctl
    IMPORT sbl1_ddr_debug_main_ctl
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
    EXPORT  sbl1_external_abort_enable
    EXPORT  ddr_debug_invalidate_tlb
    EXPORT  sbl_memcpy
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================
    PRESERVE8
    AREA    SBL1_INDIRECT_VECTOR_TABLE, CODE, READONLY, ALIGN=4
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
; Qualcomm SECONDARY BOOT LOADER 1 ENTRY POINT
;============================================================================
    AREA  SBL1_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
    ENTRY
__main
_main
sbl1_entry
    ;Change to Supervisor Mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    ; Save the passing parameter from PBL
    mov     r7, r0
    ; Set VBAR (Vector Base Address Register) to SBL vector table
    ldr     r0, =0x08005800
    MCR     p15, 0, r0, c12, c0, 0
    ; Setup the supervisor mode stack
    ldr     r0, =(0x08005800 + 0x0040000)
    mov     r13, r0
    ; Switch to undefined mode and setup the undefined mode stack
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    mov     r13, r0
    ; Switch to abort mode and setup the abort mode stack
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    mov     r13, r0
    ; Return to supervisor mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    ; Restore the passing parameter
    mov     r0, r7
    ldr    r5, =ddr_debug_init
    blx    r5
    ldr    r5,=sbl1_ddr_debug_main_ctl
    blx    r5
;    b wdog_entry
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
    ; Finished so return    
    bx lr
; void sbl1_external_abort_enable(uint32 flags)
sbl1_external_abort_enable FUNCTION
    bx      lr
    ENDFUNC
;======================================================================
; Called by sbl1_error_handler only. We clean up the registers and loop
; here until JTAG is connected.
;======================================================================
sbl_loop_here
    b sbl_loop_here
loophere
    b loophere
;============================================================================
; MACRO mdsb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data synchronization barrier, either using the ARMv7 instruction
;   or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   mdsb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      dsb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF040 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F40 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 4  ; Legacy Data Write Barrier
   ENDIF
   MEND
;======================================================================
;
; void sbl_memcpy(uint32* dst, uint32* src, int32 size)
;
; We perform our copy by using assembly.
; Parameters:
;   r0 contains the destination address
;   r1 is the source address
;   r2 is the size to be copied.
; Notes:
;   - Does not handle source/destination overlapping 
;======================================================================
sbl_memcpy
   cmp      r2,   #0x80             ; check bigger than 128 bytes or not.
   bge      loop_for_128byte_copy
check_32byte_left_over
   cmp      r2,   #0x20             ; check bigger than 32 bytes or not.
   bge      loop_for_32byte_copy
check_4byte_left_over
   cmp      r2,   #0x4
   bge      loop_for_4byte_copy
check_byte_left_over
   cmp      r2,   #0x0
   bgt      loop_for_byte_copy
memcpy_end
   bx       lr
loop_for_128byte_copy
   vldm     r1!, {Q0-Q7}            ; copy 128 bytes at a time
   vstm     r0!, {Q0-Q7}
   sub      r2, r2, #0x80           ; decrement size by 128
   cmp      r2, #0x80
   bge      loop_for_128byte_copy
   blt      check_32byte_left_over
loop_for_32byte_copy
   vldm     r1!, {Q0-Q1}            ; copy 32 bytes at a time
   vstm     r0!, {Q0-Q1}
   sub      r2, r2, #0x20           ; decrement size by 32
   cmp      r2, #0x20
   bge      loop_for_32byte_copy
   blt      check_4byte_left_over
loop_for_4byte_copy
   ldmia    r1!,  {r3}              ; load 4 bytes each time
   stmia    r0!,  {r3}
   sub      r2,r2,#0x4;
   cmp      r2,#0x4
   bge      loop_for_4byte_copy
   blt      check_byte_left_over
loop_for_byte_copy
   ldrb     r3,   [r1], #0x1
   strb     r3,   [r0], #0x1
   sub      r2,r2,#0x1
   cmp      r2, #0x0
   bgt      loop_for_byte_copy
   ble      memcpy_end              ; end the byte copy.
;============================================================================
; MACRO misb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs an instruction synchronization barrier, either using the ARMv7
;   instruction or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   misb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      isb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF060 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F60 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c5, 4   ; Legacy Pre-Fetch Flush
   ENDIF
   MEND
;============================================================================
; ddr_debug_invalidate_tlb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
;   operation (Data and Instruction). Invalidates all unlocked entries in the
;   TLB. Causes the prefetch buffer to be flushed. All following instructions
;   are fetched after the TLB invalidation.
;   We should do this before we enable to MMU.
;============================================================================
ddr_debug_invalidate_tlb
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   mov     r0 , #0
   mcr     p15, 0, r0, c8, c7, 0
   mdsb
   misb
   bx lr
    END
