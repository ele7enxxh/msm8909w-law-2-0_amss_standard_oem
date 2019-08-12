/*==================================================================
 *
 * FILE:        hostdl_sparrow_startup.s
 *
 * SERVICES:    None
 *
 * GENERAL DESCRIPTION:
 *     This module is a minimal wrapper for C code intended to run
 *     as a standalone downloaded module in RAM.  It assumes that all
 *     necessary initialization for normal standby operation has
 *     already been performed by the ROM-resident code (or other code
 *     run prior to this module).  It simply sets up the segment
 *     registers for small model and calls a C function called main_c.
 *     It starts to run from RPM and puts the SPARROW out of reset 
 *     and executes from it.
 *
 *     The C function should not return; if it does, this code sits
 *     in a tight loop until the watchdog expires.
 *  
 * EXTERNALIZED FUNCTIONS
 *     None.  However, the first code byte of this file is the entry
 *     point for the entire program.
 *  
 * INITIALIZATION AND SEQUENCING REQUIREMENTS
 *     All necessary initialization for normal CPU operation must be
 *     completed before jumping into this module.
 *  
 *     This module must be linked first, since the first code byte
 *     is the entry point, expected to be at the first address.
 *
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_sparrow_startup.s#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/22/11     sv      Initial version
 *==================================================================*/

;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "hostdl_config.h"


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


;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT main_c
    IMPORT clk_regime_rpm_init_boot_rpmsbl

    IMPORT  |Image$$APP_RAM$$ZI$$Base|
    IMPORT  |Image$$APP_RAM$$ZI$$Limit|
    IMPORT  hw_init
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the external symbols that are referenced in this module.
    EXPORT hostdl_loop_here
   
    ; Export the symbol startup to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT startup

;=======================================================================
; MACRO mmu_set_default_cfg
; ARGS
;   NONE
; DESCRIPTION
;   Update the mmu control register with the read-modify-write sequence
;   bit [0]:   0 = MMU disabled
;   bit [1]:   0 = Strict alignment fault checking disabled
;   bit [3]:   1 = Write buffer enabled (for ARM9)
;   bit [6:4]: Should Be One
;   bit [13]:  1 = High exception vectors selected.
;   bit [16]:  Should Be One
;   bit [18]:  Should Be One
;   bit [22]:  1 = unaligned data access support enabled (for ARM11)
;=======================================================================
   MACRO
   mmu_set_default_cfg
   MRC    p15, 0, r0, c1, c0, 0     ; read control register
   LDR    r1, = 0x00452078
   ORR    r0, r0, r1
   MCR    p15, 0, r0, c1, c0, 0     ; write control register
   MEND

;=======================================================================
; MACRO mmu_enable_i_cache
; ARGS
;  NONE
; DESCRIPTION
;  Enables the I Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
   MACRO
   mmu_enable_i_cache
   mrc     p15, 0 , r0, c1, c0, 0
   orr     r0, r0, # (1 << 12 )
   mcr     p15, 0 , r0, c1, c0, 0
   MEND

;=======================================================================
; MACRO init_zi
; ARGS
;  NONE
; DESCRIPTION
;  Initializes the zero-init region (.bss) to zero
;  r0  = start of ZI
;  r1  = end of ZI
;  r2  = 0 (for initializing memory) 
;=======================================================================
  MACRO
    init_zi
    ldr     r0, =|Image$$APP_RAM$$ZI$$Base|
    ldr     r1, =|Image$$APP_RAM$$ZI$$Limit|
    mov     r2,#0
init_zi_label
    cmp     r0,r1          ; unsigned compare, clear c on borrow
    strcc   r2,[r0],#4     ; str 0 in [r0] if r0 < r1
    bcc     init_zi_label  ; loop while ro < r1
  MEND

;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    ;--------------------------------------------------------------------------
    ; Data area is in IRAM and location must be defined in scatter load
    ; file.  Data area starts at the top of IRAM.
    ;--------------------------------------------------------------------------
    PRESERVE8
    AREA    BootRom, CODE, READWRITE
    CODE32
    ENTRY

startup
    ;--------------------------------------------------------------------------
    ; Exception vectors table located at the top of IMEM.  This is initialized
    ; to the Ehostdl exception handlers.  
    ;--------------------------------------------------------------------------

;============================================================================
; The aARM exception vector table is located where the aARM starts running
; when released from reset.  The boot ROM vector table will contain long
; branches that allow branching anywhere within the 32 bit address space.
; Each long branch consists of a 32 bit LDR instruction using the PC
; relative 12 bit immediate offset address mode (ARM address mode 2).
;============================================================================
    b       rpm_reset_handler                           ; rpm reset vector
    b       hostdl_reset_handler
    b       hostdl_undefined_inst_handler
    b       hostdl_swi_vector_handler
    b       hostdl_prefetch_abort_handler
    b       hostdl_data_abort_handler
    b       hostdl_reserved_handler
    b       hostdl_irq_handler
    b       hostdl_fiq_handler

;============================================================================
; Qualcomm SECONDARY BOOT LOADER 1 ENTRY POINT
;============================================================================

    AREA  HOSTDL_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
   
;============================================================================
;   We contiue to disable interrupt and watch dog 
;============================================================================
hostdl_reset_handler
	
    ; -----------------------------------------------------------------------
    ; To help debug hardware reset, we need to preserve the register values,
    ; particularly r13 and r14. This needs to be done before their
    ; contents have been changed. Since FIQ mode is not currently being
    ; used by boot, we will use the FIQ register bank to store
    ; any register values we feel are important.  Once a stack has been set
    ; up, we can use the stack to save register values as well.
    ; If the FIQ mode is ever used in boot, or if the pre-reset
    ; FIQ register values ever need to be saved themselves, then another
    ; approach will be needed.
    ; -----------------------------------------------------------------------

    ; ------------------------------------------------------------------
    ;  Registers are saved in the following order in ascending address:
    ;  SYS header         (12 bytes)
    ;  SYS mode R13, R14  (8  bytes)
    ;  IRQ header         (12 bytes)
    ;  IRQ mode R13, R14  (8  bytes)
    ;  FIQ header         (12 bytes)
    ;  FIQ mode R13, R14  (8  bytes)
    ;  ABT header         (12 bytes)
    ;  ABT mode R13, R14  (8  bytes)
    ;  UND header         (12 bytes)
    ;  UND mode R13, R14  (8  bytes)
    ;  SVC header         (12 bytes)
    ;  SVC mode R0 - R14  (60 bytes)
    ;  A total of 172 bytes will be saved at the bottom of hostdl stack
    ; ------------------------------------------------------------------

    ; FIQ mode gives us 7 banked registers (r8-r14)  We will use r8-r11 to 
    ; back up SVC mode register values until we have a stack.

    ; First, we will save the scratch registers (r0-r1)
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    mov     r8, r0
    mov     r9, r1
    
    ; r0,r1 is free to use now
    ; we will save the SVC r13 and r14 values to r10 and r11 in FIQ mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    mov     r0, r13
    mov     r1, r14

    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    mov     r10, r0 ; (svc r13)
    mov     r11, r1 ; (svc r14)
    
    ; Now setup the SVC mode stack
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    ldr     r0, = EHOSTDL_IMEM_STACK_BASE
    mov     r13, r0

    ; Store orginal SVC r13 and r14 value to r0 and r1
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    mov     r0, r10 ; (this is the r13 value)
    mov     r1, r11 ; (this is the r14 value)
    
    ; Store orginal SVC r13 and r14 values to stack
    ; while we pop up, r1 goes to r14, r0 goes to r13
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    stmfd   r13!, {r0-r1}

    ; We have a stack in SVC mode now, so we can save the rest of the SVC registers
    ; r12 goes to the bottom and so on...
    stmfd   r13!, {r2-r12}
    
    
    ; Now get original SVC r0 and r1 values from FIQ mode, and save them to stack
    ; so we have complete r0 to r15 registers.
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    mov     r0, r8
    mov     r1, r9
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    stmfd   r13!,{r0-r1}
    
    ; Now in SVC mode all r0-r12 are free to use
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =svc_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r13!, {r1-r3}
    
    
    ; Now save r13 and r14 from other modes to stack
    ; We will use r5 as our temporary SP during this process
    mov     r5, r13

    ; Save r13 and r14 for UND mode
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    stmfd   r5!, {r13-r14}
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =und_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r5!,  {r1-r3}
   
    ; Save r13 and r14 for ABT mode
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    stmfd   r5!, {r13-r14}
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =abt_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r5!,  {r1-r3}

    ; Save r13 and r14 for FIQ mode
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    stmfd   r5!, {r13-r14}
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =fiq_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r5!,  {r1-r3}
    
    ; Save r13 and r14 for IRQ mode
    msr     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
    stmfd   r5!, {r13-r14}
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =irq_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r5!,  {r1-r3}
    
    ; Save r13 and r14 for SYS mode   
    msr     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
    stmfd   r5!, {r13-r14}
    
    ; push the corresponding string header(12 bytes) on top of stack 
    ldr     r0, =sys_reg_header
    ldmfd   r0!,  {r1-r3}
    stmfd   r5!,  {r1-r3}
    
    ; switch back to SVC mode and update the sp
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
    mov     r13, r5

    ldr     a1,=hw_init

    blx     a1

    init_zi
 
   ; Set the default mmu configuration and enable instruction cache
   mmu_set_default_cfg   
   mmu_enable_i_cache
 
    ; Jump to SYSINI code of Sparrow (to the actual location of 0x2A000000)    
    ldr r1, =sparrow_init
	bx r1

sparrow_init	
    ; Reprogram the Boot remap register to map 0xFFFF0000 to 0x2A000000.
	; For this, write the following value to the register - 0x2A000003. 
	ldr r0, =APCS_GLB_BOOT_REMAP
	ldr r1, [r0]
	ldr r2, =SPARROW_START_ADDR
	ldr r3, [r2]
	orr r3, r3, #0x3
	str r3, [r1]
	
	; Modify the SCTLR bit-13 to change the reset address of Sparrow to 0xFFFF0000.
	; Thus with these two changes, the indirect exception vector table for Sparrow is 
	; located at the address 0xFFFF0000 which points to 0x2A000000. This will also
	; ensure that the RPM COde RAM range (0x0 - 0x43E50) is accessible to Sparrow.
	mrc p15, 0, r0, c1, c0, 0 ; Read SCTLR
	orr r0, r0, #0x2000       ; Enable bit 13
    mcr p15, 0, r0, c1, c0, 0 ; Write SCTLR

    
    ; ------------------------------------------------------------------
    ; Disable IRQ and FIQ interrupts.
    ; ------------------------------------------------------------------
    msr CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit
    mov     r0, r13
    
	; -----------------------------------------------------------------------
    ; Set up a boot stack pointer in Undefined,Abort mode.
    ; -----------------------------------------------------------------------    
    ; switch to Undefined mode
    msr     CPSR_c,#Mode_UND:OR:I_Bit:OR:F_Bit
    mov     r13, r0                 ; set Undefined SP

    ; switch to Abort mode
    msr     CPSR_c,#Mode_ABT:OR:I_Bit:OR:F_Bit
    mov     r13, r0                 ; set Abort SP

    ; switch back to Supervisor mode
    msr     CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit

    ; ------------------------------------------------------------------
    ; Call functions external to perform Ehostdl function.
    ; It should never return.
    ; ------------------------------------------------------------------
    ldr    r5, =main_c
    blx    r5

    ; For safety
    bl loophere  ; never returns, keep lr in r14 for debug

	
;============================================================================
; RPM reset handler is compiled for sparrow environment but a part of it is 
; rpm compatible and is executed by RPM to get sparrow out of reset. The 
; steps performed are as follows:
;    1. It configures clock for sparrow.
;    2. Replaces the reset vector (rpm_reset_handler) with sparrow reset handler 
;       (hostdl_reset_handler) .
;    3. RPM then copies code that releases sparrow, to coderam and jumps
;       to it from imem. 
;    4. RPM then releases sparrow and loops until RPM FW is loaded to Coderam and
;       then executes RPM FW. Sparrow starts executing code from IMEM.
;
;   Image Layout
;   +-----------+ 
;   |           | rpm_reset_handler/hostdl_reset_handler 
;   +-----------+ 0x2a000000
;   |			|    -
;   |			|    -
;   |			|    -
;   +-----------+ 0x43E50
;   |           | EHOSTDL code - Sparrow out of reset and wait for RPM FW to be loaded 
;   +-----------+ 0x43D00
;   |           | Shared data between RPM EHOSTDL and Sparrow EHOSTDL
;   +-----------+ 0x43C00
;   |           | Code RAM location for RPM FW
;   +-----------+ 0x20000
;   |           | Boot ROM
;   +-----------+ 0x0
;
;   
;============================================================================
rpm_reset_handler
    ; Initialize clock - Add alter - Suresh
    bl clk_regime_rpm_init_boot_rpmsbl

code_for_rpm
    ; rpm moves code between "src_in_code_ram_start" and
    ; "src_in_code_ram_end" to CODE RAM  
    ldr r1, =src_in_code_ram_start
    ldr r2, =EHOSTDL_RPM_JUMP_BASE
    ldr r3, =src_in_code_ram_end
	
copy_code
    cmp r1, r3 ; compare end address
    beq rpm_do_remaining_task
    ldr r0, [r1]
    str r0, [r2]
    add r1, r1, #4
    add r2, r2, #4
    b copy_code
	
rpm_do_remaining_task
   ; Copy the exception handlers to the reset address of Sparrow
   ldr r0, =SPARROW_START_ADDR
   ldr r1, [r0]
   
   ; For loop to move the opcodes for 8 exception handlers
   ldr r0, =SPARROW_EXP_INST
   ldr r2, [r0]  ; load the opcode
   mov r3, #08   ; move count
   
move_opcode
   str r2, [r1]    ; store it at reset vect table offset
   add r1, r1, #4  ; point to next location
   subs r3, r3, #1
   bne move_opcode
   
   ; now make rpm jump to code ram. When in Code_ram rpm brings scorpion
   ; out of reset and itself loops forever as its no longer needed.
   ldr r0, =EHOSTDL_RPM_JUMP_BASE
   bx r0 ; jump to coderam   

src_in_code_ram_start
   
   ; This code executes in Code RAM
   ; Make jump_to_rpm_trigger 0
   ;d.s APCS_GLB_QSB_LP %L   0x400
   ;d.s APCS_GLB_BOOT_REMAP %L   0x2A000001 
   ;d.s APCS_GLB_AHB_CLKON %L 1
   ;d.s APCS_GLB_GHDS_CNTR %L 0xA
   ;d.s APCS_GLB_EN_PWR_ST %L 0X3
   ;d.s APCS_CPU_PWR_CTL+0x80000 %L 0x001F0000
   ; d.s  APCS_GLB_CFG            %LONG   0x400

   ; check for the jump_to_rpm_trigger and jump to the start address of rpm
   
   ; Cannot rely on assembler to allocate value (DCD) for the address.
   ; It does that in imem address space and we dont want that.   
   
   ; Load address of shared data 0x00043C04
   mov r0, #0x4
   lsl r0, r0, #0x8
   add r0, r0, #0x3C
   lsl r0, r0, #0x8
   add r0, r0, #0x4

   ; Zero the magic number value
   mov r1, #0x0
   str r1, [r0]
   
   ; Load address 0x02010020 of the Sparrow registers
   mov r0, #0x2
   lsl r0, r0, #0x8
   add r0, r0, #0x1
   lsl r0, r0, #0x10
   add r0, r0, #0x20   
   
   ; Load 0x400 to address 0x02010030
   mov r1, #0x400
   str r1, [r0, #0x10]
   ; Load value 0x2A000001 as the reset address of Sparrow 
   ; The bit-1 and bit-0 indicates that 0x0 for Sparrow will be remapped to
   ; 0x2A000000
   mov r1, #0x2A
   lsl r1, r1, #0x18
   add r1, r1, #0x1
   
   ; Store the reset address to the Boot remap register 0x02010024
   str r1, [r0, #0x4]
   
   ; Store 0x1 to 0x02010034
   mov r1, #0x1
   str r1, [r0, #0x14]
   ; Store 0xA to 0x0201003C
   mov r1, #0xA
   str r1, [r0, #0x1C]
   ; Store 0x3 to 0x0201002C
   mov r1, #0x3
   str r1, [r0, #0xC]

   ; Load 0x02088004 address for the power control register of Sparrow
   mov r2, #0x2
   lsl r2, r2, #0x8
   add r2, r2, #0x8
   lsl r2, r2, #0x8
   add r2, r2, #0x80
   lsl r2, r2, #0x8
   add r2, r2, #0x4
   
   ; Store 0x001F0000 in 0x02088004
   mov r1, #0x1F
   lsl r1, r1, #0x10
   str r1, [r2]


   ; We need to wait for a few ms before resetting the sparrow, so lets 
   ; Load the BOOT_JUMP_TO_RPM_FW_MAGIC_NUM magic number value 0x4D505242
   ; which will be used later
   mov r2, #0x4D
   lsl r2, r2, #0x8
   add r2, r2, #0x50
   lsl r2, r2, #0x8
   add r2, r2, #0x52
   lsl r2, r2, #0x8
   add r2, r2, #0x42
   
   ; Store 0x400 to the Global Config register
   mov r1, #0x400
   str r1, [r0]

   ; Suresh to loop after starting sparrow
rpm_end_loop
   b rpm_end_loop

   ; Load address of shared data 0x00043C04
   mov r0, #0x4
   lsl r0, r0, #0x8
   add r0, r0, #0x3C
   lsl r0, r0, #0x8
   add r0, r0, #0x4

check_magic_number   
   ; Check if the magic number is written
   ldr r1, [r0]
   cmp r1, r2
   bne check_magic_number
   
   ; Get the address of RPM FW and jump to the start location
   ldr r1, [r0, #0x4]
   bx r1
   
src_in_code_ram_end

;============================================================================
; The exception handlers will do a endless loop 
;============================================================================
hostdl_undefined_inst_handler
    b       hostdl_undefined_inst_handler

hostdl_swi_vector_handler
    b       hostdl_swi_vector_handler

hostdl_prefetch_abort_handler
    b       hostdl_prefetch_abort_handler

hostdl_data_abort_handler
    b       hostdl_data_abort_handler

hostdl_reserved_handler
    b       hostdl_reserved_handler

hostdl_irq_handler
    b       hostdl_irq_handler

hostdl_fiq_handler
    b       hostdl_fiq_handler


;======================================================================
; Called by all the handlers currently. We clean up the registers and loop
; here until JTAG is connected.
;======================================================================
hostdl_loop_here
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
; When we are saving the cpu registers after reset,
; the following cpu mode string headers are pushed to stack on top of the cpu 
; registers of corresponding cpu modes for explanatory purpose.
; Each header is exactly 12 bytes
;======================================================================    
svc_reg_header
    DCB  "SVC: R1-R14" , 0
    
fiq_reg_header
    DCB  "FIQ:R13-R14" , 0
    
irq_reg_header
    DCB  "IRQ:R13-R14" , 0
    
und_reg_header
    DCB  "UND:R13-R14" , 0

abt_reg_header
    DCB  "ABT:R13-R14" , 0

sys_reg_header
    DCB  "SYS:R13-R14" , 0
	
SPARROW_START_ADDR
   DCD                 0x2A000000   
SPARROW_EXP_INST
   DCD                 0xEA000013 
APCS_GLB_BOOT_REMAP
   DCD                 0x02010024   

    END

