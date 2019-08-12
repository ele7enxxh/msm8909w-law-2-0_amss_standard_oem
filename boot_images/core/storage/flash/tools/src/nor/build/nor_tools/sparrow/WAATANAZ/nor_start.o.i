;=======================================================================
;  FILE:        nor_start.s
;
;  SERVICES:    
;   This module contains the entry point for our Flash Programming Plug-In 
;   for Lauterbach JTAG/ICD TRACE32. All it does is setups stack and calls
;   our main 'C' function "main_c".
;
;   The C function does return and expects a breakpoint there. This is the
;   case because the FLASH programming plug-in specification requires it.
;   The break-point is used to give control back to TRACE32 Debugger so 
;   that it may fill up the buffer and call our plug-in again. This cycle
;   continues until FLASH programming is complete.
; DESCRIPTION
;   This file has the main functions used for programming the flash
;
; Copyright (c) 2010, 2014 Qualcomm Technologies, Inc.
; All Rights Reserved.
; QUALCOMM Confidential and Proprietary
;=======================================================================*/
;===================================================================
;
;                       EDIT HISTORY FOR FILE
;
;   This section contains comments describing changes made to the
;   module. Notice that changes are listed in reverse chronological
;   order.
;
;   $Header: 
;
; when         who     what, where, why
; ----------   ---     ----------------------------------------------
; 09/16/14     sb      Move cache init to cmm 
; 09/10/10     sv      Initial version
;==================================================================*/
;
; CPSR Control Masks 
PSR_Fiq_Mask    EQU     0x40
PSR_Irq_Mask    EQU     0x80
; Processor mode definitions 
PSR_Supervisor  EQU     0x13
;
; Stack sizes
SVC_Stack_Size  EQU     0x1000
;
        IMPORT  main_c
        IMPORT  memory_init
        IMPORT  svc_stack
        IMPORT  zero_init_needed
        EXPORT  __main
        PRESERVE8
        AREA    StartHere, CODE, READONLY
        CODE32
__main
        ENTRY
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
;
; Supervisor Mode
; Set up the Supervisor stack pointer.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =svc_stack+SVC_Stack_Size
; ======================================================================
; Enable the instruction cache
; ======================================================================
;   mmu_set_default_cfg
;   mmu_enable_i_cache
; ======================================================================
; Initialize memory for C only once
;   The test/set of the global variable must be done here in assembly
;   because if we access a global variable in the C function, the
;   compiler will construct a PUSH/POP of registers and since we will
;   have just zeroed the stack, we will pop zero into R14 and then
;   branch to zero.  With no use of globals in the C function,
;   the compiler will generate a bx r14 for the return and all will
;   work correctly.
; ======================================================================
         ldr   r2, =zero_init_needed
         ldr   r1, [r2]
         cmp   r1, #0x0
         beq   skipmem
         ldr     a1, =memory_init
         blx     a1
; indicate zero init completed
         ldr   r2, =zero_init_needed
         mov   r0, #0x0
         str   r0, [r2]
; Enter C code execution
skipmem        
        ldr     a1, =main_c
        blx     a1
; Software breakpoint code
; TRACE32 requires this so that we can hand back the control to TRACE32
; Debugger when main_c returns
swbp    DCD     0xEEDEEEDE
; In case breakpoint above gets clobbered
hanghere
         b     hanghere
; Terminal emulation entry point for putting a character.
; When called, it will put or get a charactr from the
; JTAG debugger terminal emulation window.  These function
; are incompatible with the JTAG debugger FLASH.* functions
; and will cause the debugger to hang if both are used.
        CODE16
        EXPORT  term_put
        ALIGN
term_put
        bx      pc
        DCW    0x0                  ; Just to suppress a warning
        CODE32
        ALIGN
term_put_arm
        ldr     r2, =0x20000000      ; bit 29
put_again_arm
        mrc     p14, 0, r1, c0, c1   ; read debug status and control register
        and     r1, r1, r2           ; check if wDTR is ready to receive data
        cmp     r1, #0
        bne     put_again_arm
        mcr     p14, 0, r0, c0, c5   ; write the write data transfer register
        bx      lr
        END
