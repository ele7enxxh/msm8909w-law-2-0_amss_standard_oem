;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     Flash Utility Assembly File
;
; GENERAL DESCRIPTION
;	This file contains the assembly instructions for miscelleaneous functions 
;   in flash
;
;  Copyright (c) 2012 Qualcomm Technologies Incorporated. 
;  All Rights Reserved.
;  Qualcomm Confidential and Proprietary
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

;/*===========================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who    what, where, why
; --------  -----  ----------------------------------------------------------
; 02/15/12   sb    Initial version
;=============================================================================*/

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the symbols defined in this module
    EXPORT  flash_hal_asm_memcpy


;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    PRESERVE8
    AREA    FLASH_UTIL_ASM, CODE, READONLY
    CODE32
        
;======================================================================
;
; void flash_hal_asm_memcpy(int32* dst, int32* src, int32 size)
;
; We perform our copy by using assembly.
; Parameters:
;   r0 contains the destination address
;   r1 is the source address
;   r2 is the size to be copied.
; Notes:
;   Size should be evenly divisible by 4 bytes; if not, it will overwrite
;   up to 3 unspecified bytes.
;======================================================================
flash_hal_asm_memcpy
   stmfd    sp!,  {r0-r10,lr}     ; First save all registers
   mov      r8,   r0              ; r0 is destination
   mov      r9,   r1              ; r1 is source
   mov      r10,  r2
   cmp      r10,  #0x20           ; check bigger than 32 bytes or not.
   bge      loop_for_32byte_copy
check_left_over
   cmp      r10,#0                ; check smaller than 32 bytes or not.
   bgt      loop_for_4byte_copy
flash_memcpy_end
   ldmfd    sp!,  {r0-r10,pc}     ; return
loop_for_32byte_copy
   ldmia r9!,  {r0-r7}            ; load 32 bytes each time
   stmia r8!,  {r0-r7}
   sub      r10,r10,#0x20
   cmp      r10,#0x20
   bge      loop_for_32byte_copy
   blt      check_left_over       ; check whether there is left over bytes.
loop_for_4byte_copy
   ldmia r9!,  {r0}               ; load 4 bytes each time
   stmia r8!,  {r0}
   sub      r10,r10,#0x4;
   cmp      r10,#0x0
   bgt      loop_for_4byte_copy
   ble      flash_memcpy_end        ; end the byte copy.

   END

