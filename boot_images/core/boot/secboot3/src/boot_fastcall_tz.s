;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 BOOT FASTCALL TZ ASM FUNCTIONS
;
; Copyright (c) 2012 by Qualcomm Technologies, Incorporated. All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_fastcall_tz.s#1 $
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 11/28/12   kpa      Added cache management apis to smc call.
; 03/14/12   dh       Remove mmu disable/enable call
; 12/01/11   dh       Add boot_check_sec_mode
; 09/09/11   kpa      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;=======================================================================
;                             MODULE IMPORTS
;=======================================================================
  IMPORT mmu_flush_cache
  IMPORT mmu_invalidate_data_cache


;=======================================================================
;                             MODULE EXPORTS
;=======================================================================

  EXPORT boot_fastcall_tz
  EXPORT boot_check_sec_mode

;=======================================================================
;
;                             MODULE DATA AREA
;
;=======================================================================

  PRESERVE8
  AREA    BOOT_UTIL_ASM, CODE, READONLY
  CODE32

  MACRO
  SMCCALL
  ; It is important to flush and disable cache while
  ; making a smc call into tz to avoid cache coherency issues
  ; of data being passed over (especially via pointers).
  bl      mmu_flush_cache
100 
  smc     0x0
  adds    r0, #0         ; Check return value sign
  bgt     %b100          ; If return value was positive redo smc
  ; At this point r0 contains TZ smc call return status
  bl      mmu_invalidate_data_cache
  MEND

;============================================================================
; FUNCTION boot_check_sec_mode
;
; ARGS
;   None
;
; DESCRIPTION
;   This function checks the processor is in secure mode or not
;   return TRUE if processor is in secure mode
;   return FALSE if processor is in non secure mode
;============================================================================ 
boot_check_sec_mode FUNCTION
  ;read DBGDSCR 
  ;Bit 18 of DBGDSCR register indicates the processor is
  ;in secure mode(set to 0) or non secure mode (set to 1)
  
  stmfd   sp!, {lr}
  mrc     p14 , 0, r0 , c0, c1, 0x0
  tst     r0, #0x40000
  movne   r0, #0x0
  moveq   r0, #0x1
  ldmfd   sp!, {lr}
  bx lr
  
  ENDFUNC
  
;============================================================================
; FUNCTION boot_fastcall_tz
; int boot_fastcall_tz(uint32 id, uint32 token, uint32 r0, uint32 r1, ...)
;
; ARGS
; r0  - id, TZ SYSCALL id, lowest nibble contains the number of arguments.
; r1  - token, SMP token.
; r2  - First argument to TZ syscall (R0, optional).
; r3  - Second argument to TZ syscall (R1, optional).
; ... - Stacked arguments, TZ arguments from R2-R9 (optional)
;
; RETURN
;  reg R0: value 0xFFFFFFF0 (-E_INVALID_ARG) indicates error if arguments          
;  are more than whats supported.
;  Else error code is what returned by Tz from smc call.
;
; DESCRIPTION
;   Make a secure monitor call to TZ to perform read write in secure mode
;============================================================================   
boot_fastcall_tz FUNCTION
  stmfd sp!, {r1-r12, lr}
  orr     r0, r0, #(2 << 8)   ; val 2 in bits 8-11 means fastcall
  and     lr, r0, #0xff       ; last nibble is the arg count

  cmp     lr, #2      ; if (args <= 2)
  bgt     fstcll_4arg
  SMCCALL
  b      fstcll_end

fstcll_4arg
  cmp     lr, #4      ; if (args == 4)
  bne     fstcll_err
  push    {r4-r5}
  add     lr, sp, #60
  ldmia   lr, {r4-r5}
  SMCCALL
  pop     {r4-r5}
  b fstcll_end

fstcll_err
  ; return a -ve value value for error
  mvn     r0, #0xFFFFFFF0   ; Too many args

fstcll_end
  ldmfd sp!, {r1-r12, lr}   ; r0 has return value.
  bx lr  
  ENDFUNC

  END

