/*==================================================================
 *
 * FILE:        hostdl_startup.s
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
 * Copyright (c) 2008-2009, 2011-2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/05/13     sv      Fix stack issue - Set the SVC stack size to 4K
 * 10/25/12     sv      Remove wdog kick in zi init and reloc data
 * 10/19/12     sv      Update codes for ZI init
 * 09/22/11     sv      Modify/cleanup hwio_init call
 * 08/18/11     jz      ehostdl support on 9x15 RPM
 * 08/22/09     op      Call dbl_ehostdl_init for DBL
 * 06/22/09     sv      Modified to use the BUILD_EHOSTDL feature flag for Ehostdl
 * 05/20/09     jz      Put back call to mmu_set_default_cfg with updated value
 * 05/08/09     jz      Removed call to mmu_set_default_cfg, added DCC support for ARM11
 * 04/23/09     sv      Ehostdl Support/add init_zi & reloc_data asm routines
 * 10/29/08     mm      Initial version
 *==================================================================*/

#include "hostdl_config.h"
 
/*======================================================================
;
;                  REGISTER DEFINITIONS
;
;=======================================================================*/

// CPSR Control Masks 
#define PSR_Thumb_Mask       0x20
#define PSR_Fiq_Mask         0x40
#define PSR_Irq_Mask         0x80
#define PSR_Mode_Mask        0x1f

// Processor mode definitions
#define PSR_User        0x10
#define PSR_Fiq         0x11
#define PSR_Irq         0x12
#define PSR_Supervisor  0x13
#define PSR_Abort       0x17
#define PSR_Undef       0x1b
#define PSR_System      0x1f
#define PSR_Thumb       0x20


/*============================================================================
;
;                  STACK DEFINITIONS
;
;============================================================================*/

//  Used as flashprg stack (FLSH_Stack_Size). Allocate 4K for the    
//  stack.  A largish stack is required because      
//  the packet buffer will be allocated on the stack.
//  These stacks are defined at dword in the C code and are defined
//  as being this size divided by 4.  Creating them in the C code
//  as dword prevents unaligned accesses.
//  The actual memory size allocated for SVC stack is determined by
//  the SVC_Stack_Size defined in hostdl_startup.h. This define in
//  hostdl_startup.h and the below define need to be consistent. Any 
//  mismatch will lead to stack issues.

#define SVC_Stack_Size   0x1000

//  Define other stacks, these can probably be combined to use
//  the same stack

#define IRQ_Stack_Size   0x800
#define Abort_Stack_Size 0x100
#define Undef_Stack_Size 0x100
    

/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/

 

   CODE32
      AREA    BootRom, CODE, READONLY

      IMPORT  main_c
      IMPORT  abort_stack
      IMPORT  undef_stack
      IMPORT  irq_stack
      IMPORT  svc_stack
#ifdef BUILD_EHOSTDL
      IMPORT  |Image$$APP_RAM$$Base|
      IMPORT  |Load$$APP_RAM$$Base|
#endif
#ifndef BUILD_EHOSTDL_RPM
      IMPORT  hw_init
#endif
      IMPORT  |Image$$APP_RAM$$ZI$$Base|
      IMPORT  |Image$$APP_RAM$$ZI$$Limit|
      IMPORT  |Image$$ZI_RAM$$ZI$$Base|
      IMPORT  |Image$$ZI_RAM$$ZI$$Limit|
      EXPORT  startup

   PRESERVE8

      ENTRY

/*============================================================================
;
; Name: blx
;
; Description: Calls a function from ARM state without having to know whether
;    that function is ARM or Thumb state code.
;
; Arguments: 
;   destreg - The register that contains the address of the function to be
;   called.
;
; Registers modified: lr
;       
; MODE: Any
; STATE: ARM. Can call either ARM or Thumb state functions.
;
;============================================================================*/

   // call a function without knowing if it is ARM or THUMB code
   MACRO   
      blxx     $destreg
      ROUT

      tst     $destreg, #0x01         // Test for thumb mode call.

      ldrne   lr, =%1
      ldreq   lr, =%2
      bx      $destreg
1
      CODE16
      bx      pc

      ALIGN
      CODE32
2
      MEND



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

;=======================================================================
; MACRO mmu_disable_d_cache
; ARGS
;  NONE
; DESCRIPTION
;  Disables the D Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
  MACRO
    mmu_disable_d_cache
    mrc     p15, 0 , r0, c1, c0, 0
    and     r0, r0, # ~(1 << 2 )
    mcr     p15, 0 , r0, c1, c0, 0
  MEND
;=======================================================================


;=======================================================================
; MACRO init_app_ram_zi
; ARGS
;  NONE
; DESCRIPTION
;  Initializes the zero-init region (.bss) to zero
;  r0  = start of APP RAM ZI
;  r1  = end of APP RAM ZI
;  r2  = 0 (for initializing memory) 
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;=======================================================================
  MACRO
    init_app_ram_zi
    ldr     r0, =|Image$$APP_RAM$$ZI$$Base|
    ldr     r1, =|Image$$APP_RAM$$ZI$$Limit|
    mov     r2,#0
init_app_ram_zi_label
    cmp     r0,r1          ; unsigned compare, clear c on borrow
    strcc   r2,[r0],#4     ; str 0 in [r0] if r0 < r1
    bcc     init_app_ram_zi_label  ; loop while ro < r1
  MEND
;=======================================================================

;=======================================================================
; MACRO init_zi
; ARGS
;  NONE
; DESCRIPTION
;  Initializes the zero-init region (.bss) to zero
;  r0  = start of ZI
;  r1  = end of ZI
;  r2  = 0 (for initializing memory) 
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;=======================================================================
  MACRO
    init_zi
    ldr     r0, =|Image$$ZI_RAM$$ZI$$Base|
    ldr     r1, =|Image$$ZI_RAM$$ZI$$Limit|
    mov     r2,#0
init_zi_label
    cmp     r0,r1          ; unsigned compare, clear c on borrow
    strcc   r2,[r0],#4     ; str 0 in [r0] if r0 < r1
    bcc     init_zi_label  ; loop while ro < r1
  MEND
;=======================================================================

;=======================================================================
; MACRO reloc_data
; ARGS
;  NONE
; DESCRIPTION
;  Relocates the RW data for eHOSTDL
;  r0  = start of dest mem
;  r1  = end of dest mem (length of data to be relocated)
;  r2  = start of src mem
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;  r5  = tmp data holder for data relocation
;=======================================================================
  MACRO
    reloc_data
    ldr     r0, =|Image$$APP_RAM$$Base|
    ldr     r1, =|Image$$APP_RAM$$ZI$$Base|
    ldr     r2, =|Load$$APP_RAM$$Base|
reloc_data_label
    cmp     r0,r1            ; unsigned compare, clear c on borrow
    ldrcc   r5,[r2],#4       ; Load [r2] to r5, increment [r2] if r0 < r1
    strcc   r5,[r0],#4       ; str r5 at [r0],  increment [r0] if r0 < r1
    bcc     reloc_data_label ; loop while ro < r1
  MEND
;=======================================================================




/*============================================================================
;
; FUNCTION STARTUP
;
; DESCRIPTION
;   This function just sets up the segment registers and calls
;   the C function main_c.  If that function returns, a tight loop
;   waits for a watchdog reset.
;
; FORMAL PARAMETERS
;   None.
;
; DEPENDENCIES
;   Any initialization required for normal CPU operation must have
;   been performed prior to calling this module.
;
;   The entry point is intended to be jumped into by a downloader.
;
; RETURN VALUE
;   This function never returns.
;
; SIDE EFFECTS
;
;============================================================================*/

// All GPIO init/config is moved to C code in asb_and_msm_init

startup
   
   ldr     r1,  =0xdeadbeef             
   ldr     r2,  =0xdeadbeef
   ldr     r3,  =0xdeadbeef
   ldr     r4,  =0xdeadbeef
   ldr     r5,  =0xdeadbeef
   ldr     r6,  =0xdeadbeef
   ldr     r7,  =0xdeadbeef
   ldr     r8,  =0xdeadbeef
   ldr     r9,  =0xdeadbeef
   ldr     r11, =0xdeadbeef
   ldr     r12, =0xdeadbeef
;   ldr     r13, =0xdeadbeef
   ldr     r14, =0xdeadbeef
                                        
   // Inhibit interrupts
   mrs     a1, CPSR                
   orr     a2, a1, #PSR_Irq_Mask
   msr     CPSR_c, a2

   // Abort Mode
   // Enter Abort mode and set up the stack pointer
   msr     CPSR_c, #PSR_Abort:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13, =abort_stack+Abort_Stack_Size

   // Undef Mode
   // Enter Undef mode and set up the stack pointer
   msr     CPSR_c, #PSR_Undef:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13,=undef_stack+Undef_Stack_Size

   // IRQ Mode
   // Enter IRQ mode and set up the IRQ stack pointer
   msr     CPSR_c, #PSR_Irq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13, =irq_stack+IRQ_Stack_Size

   // Supervisor Mode
   // Set up the Supervisor stack pointer.
   // = FLSH_Stack_size
   msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

   // BUILD_EHOSTDL: Defined in ap_config.min when building HOSTDL
   //    for enandprg. Flag to indicates compiling the code relating to 
   //    relocating the data areas to IMEM/RAM
   // EHOSTDL_IMEM_STACK_BASE: Defined in boot_target_param.min. Indicates the 
   //    stack base address when in IMEM
#if defined(BUILD_EHOSTDL) && !defined(BUILD_EHOSTDL_RPM)
   // Use stack from IMEM before RAM is initialized for EHostdl

   ldr     r13, =EHOSTDL_IMEM_STACK_BASE

   // Configure ASB/MSM registers for required operation.
   // Only required when running standalone, in normal operation we
   // enter from Boot Code which has already performed this setup.

#else
   ldr     r13, =svc_stack+SVC_Stack_Size
#endif

   ldr     a1,=hw_init
   blxx    a1

   // Initialize memory for C
   init_zi
   init_app_ram_zi

#ifdef BUILD_EHOSTDL
   reloc_data
#endif  
  

; ======================================================================
; Set MMU control register and enable the instruction cache
; ======================================================================
#ifndef BUILD_EHOSTDL_RPM   
   mmu_set_default_cfg
   mmu_enable_i_cache
   mmu_disable_d_cache
#endif

#ifdef BUILD_EHOSTDL
   // EHOSTDL_RAM_STACK_BASE: Defined in boot_target_param.min. Indicates the 
   //    stack base address when in RAM
   // Use stack from RAM for EHostdl as it is initialized now
#ifndef BUILD_EHOSTDL_RPM
   ldr     r13, =EHOSTDL_RAM_STACK_BASE   
#endif
#endif 
     
   // Enter C code execution
   ldr     a1, =main_c
   bx      a1


; Software breakpoint code
; main_c never returns, but if it did, a breakpoint here
; would catch it
swbp    DCD     0xEEDEEEDE


#ifdef USE_BUFFERED_TERMIO        

; Terminal emulation entry point for put/get a buffer of characters.

; Buffered I/O uses a spot breakpoint on the host side.
; This does not work with T32 flash.* commands used
; in JFLASH and JNAND.  XScale processor does not implement
; regular I/O like our MSM does, so this must be used
; but is really only useful in ARMPRG/NANDPRG style
; flash programmers which are getting the data to be
; programmed from a UART or USB interface.

        CODE16
        EXPORT  term_blip
term_blip
        bx      lr

        EXPORT  output_port
        EXPORT  input_port

; This is in RAM, so just put it here.  These are the buffers
; used.  The first 32-bit word is the count of characters.
; The rest of the buffer is data, one character in every
; 32-bit word.  Character counter in printf file will only fill
; with 252 words of data, ensuring no overflows.
;
; Host side is invoked with:
;
; term.protocol bs
; term.view output_port input_port
; term.write output_port term.out
; b.s term_blip /s /onchip

output_port
        % 256
input_port
        % 256

   
#else /* Normal terminal IO */


; Terminal emulation entry point for put/get a character.
; When called, it will put or get a charactr from the
; JTAG debugger terminal emulation window.  These function
; are incompatible with the JTAG debugger FLASH.* functions
; and will cause the debugger to hang if both are used.


        CODE16
        EXPORT  term_put
      
term_put
        bx      pc
        ALIGN
        CODE32

#ifdef BUILD_EHOSTDL_RPM
term_put_arm7
        mov     r2, #2

put_again_arm7
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        bne     put_again_arm7
        mcr     p14, 0, r0, c1, c0
        bx      lr

#else      
; Check the ID code by reading the cp15 register c0 to
; determine if it's ARM9 or ARM11
; [15:4] Part number 0x926 for ARM926, 0xB36 for ARM1136
        mrc p15, 0, r1, c0, c0, 0   ; read ID
        and r1, r1, #0xf000
        cmp r1, #0xb000             ; if 0xb in part number it's ARM11
        bge term_put_arm11
  
term_put_arm9
        mov     r2, #2

put_again_arm9
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        bne     put_again_arm9
        mcr     p14, 0, r0, c1, c0
        bx      lr

term_put_arm11
        ldr     r2, =0x20000000      ; bit 29
     
put_again_arm11
        mrc     p14, 0, r1, c0, c1   ; read debug status and control register
        and     r1, r1, r2           ; check if wDTR is ready to receive data
        cmp     r1, #0
        bne     put_again_arm11
        mcr     p14, 0, r0, c0, c5   ; write the write data transfer register
        bx      lr

#endif   
#endif /* USE_BUFFERED_TERMIO */


;  Jump to address passed in.
;  Used to fake a reset by passing zero
        CODE16
        EXPORT  jmp_abs
        
jmp_abs
        bx      pc

        ALIGN
        CODE32
    
jmp_abs_arm
        mov r7,r0
        bx r7
        

   
   END
   
