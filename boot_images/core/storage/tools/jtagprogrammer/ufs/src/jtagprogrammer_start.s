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
;  Copyright (c) 1998-2005, 2008, 2010
;  Qualcomm Technologies, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*======================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/ufs/src/jtagprogrammer_start.s#1 $

when          who     what, where, why
-----------   ---     -----------------------------------------------------
2011-05-20    ab      Export swbp symbol explicitly for 7x30 SS BADA
2010-05-21    vj      Fixed bug in tool for ARM9
2010-05-13    RH      Adding support for Scorpion TERM window
2008-06-15    VTW     Ported from JFLASH.
===========================================================================*/
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


;/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/


        IMPORT  jsdcc_init

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
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor
;=======================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, = 0x00050078
   MCR    p15, 0, r0, c1, c0, 0
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



;/*============================================================================
;
; FUNCTION STARTUP
;
; DESCRIPTION
;   This function just setup the stack and calls "c" routine jsdcc_init.
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
        ldr     r13, =CODE_START_ADDR+CODE_SIZE


; ======================================================================
; Enable the instruction cache
; ======================================================================
   mmu_set_default_cfg
   mmu_enable_i_cache

; Enter C code execution
        b       jsdcc_init
        END
