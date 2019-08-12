;/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                      S T A R T U P   C O D E
;
; GENERAL DESCRIPTION
;   This module contains the entry point for our Flash Programming Plug-In 
;   for Lauterbach JTAG/ICD TRACE32. All it does is setups stack and calls
;   our main 'C' function "main_c".
;
;   The C function does return and expects a breakpoint there. This is the
;   case because the FLASH programming plug-in specification requires it.
;   The break-point is used to give control back to TRACE32 Debugger so 
;   that it may fill up the buffer and call our plug-in again. This cycle
;   continues until FLASH programming is complete.
;
; EXTERNALIZED FUNCTIONS
;   None.  
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;   This module must be linked first, specifically the AREA "StartHere", 
;   since this is our entry point. This is done in our scatter load file.
;
;  Copyright (c) 2002-2004, 2010 Qualcomm Technologies Incorporated. 
;  All Rights Reserved.
;  Qualcomm Confidential and Proprietary
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

;/*============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_start_rpm.s#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
;
; when      who    what, where, why
; --------  -----  ----------------------------------------------------------
; 01/14/19  mcg    ARM7TDMI version created from ARM9/ARM11 version.         
;                                                                     
;===================================================================*/

;/*============================================================================
;
;                  REGISTER DEFINITIONS
;
;============================================================================*/

; CPSR Control Masks 
PSR_Fiq_Mask    EQU     0x40
PSR_Irq_Mask    EQU     0x80

; Processor mode definitions 
PSR_Supervisor  EQU     0x13


;/*============================================================================
;
;                  STACK DEFINITIONS
;
;============================================================================*/


; Stack sizes
SVC_Stack_Size  EQU     0x1000
    

;/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/


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


;/*============================================================================
;
; FUNCTION STARTUP
;
; DESCRIPTION
;   This function just setup the stack and calls "c" routine main_c.
;
; FORMAL PARAMETERS
;   None.
;
; DEPENDENCIES
;   Any initialization required for normal CPU operation must have
;   been performed prior to calling this module. This can be achieved
;   in the startup script files using data.set command to write to
;   appropriate MSM/ASB registers.
;
; RETURN VALUE
;   This function never returns.
;
; SIDE EFFECTS
;
;============================================================================*/

; Supervisor Mode
; Set up the Supervisor stack pointer.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =svc_stack+SVC_Stack_Size


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
         
         bl      memory_init

; indicate zero init completed
         ldr   r2, =zero_init_needed
         mov   r0, #0x0
         str   r0, [r2]

; Enter C code execution
        
skipmem        
         bl      main_c

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

        
#ifdef USE_BUFFERED_TERMIO        

        CODE16
        EXPORT  term_blip
term_blip
        bx      lr

        EXPORT  output_port
        EXPORT  input_port

        ; This is in RAM, so just put it here.
output_port
        % 256
input_port
        % 256

   
#else /* Normal terminal IO */
        

        CODE16
        EXPORT  term_put
        ALIGN
term_put
        bx      pc
        
        DCW    0x0                  ; Just to suppress a warning
        CODE32
        ALIGN

term_put_arm7
        mov     r2, #2

put_again_arm7
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        bne     put_again_arm7
        mcr     p14, 0, r0, c1, c0
        bx      lr

#endif /* USE_BUFFERED_TERMIO */



        END

