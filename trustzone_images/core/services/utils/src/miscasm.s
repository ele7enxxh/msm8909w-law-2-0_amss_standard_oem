; ==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==
;         M I S C   A S S E M B L Y    L E V E L    R O U T I N E S      
;                     F O R    D A T A    T R A N S F E R
;
; GENERAL DESCRIPTION
;
;   This module contains block data transfer routines for data transfer
;   between 32-bit MSM register(s) and RAM.  Routines are written in
;   assembly and use the ARM load multiple (LDM) and store multiple (STM)
;   instructions.  A separate module is used for these functions because 
;   the tcc compiler does not support ARM mode in-line assembly.
;
;   Note that because of way the LDM and STM block move instructions work,
;   MSM registers used with these routines must actually correspond to
;   a range of register addresses.                                      ffer
;
;
; EXTERNALIZED FUNCTIONS
;
;   miscasm_write_burst_data 
;     Writes data from memory to a burst interface register.
;   miscasm_read_burst_data
;     Read data from a burst interface register and copy to memory.
;   
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;   None.
;
; Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
; ==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

; ==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
; 
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/miscasm.s#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
; 
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
; 08/17/04   rp     Created.      
;                    
;
; ==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==


; ===========================================================================
;                           MODULE INCLUDE FILES
; ===========================================================================

; ===========================================================================
;                             MODULE IMPORTS
; ===========================================================================
       

; ===========================================================================
;                             MODULE EXPORTS
; ===========================================================================

    EXPORT miscasm_write_burst_data
    EXPORT miscasm_read_burst_data

; ===========================================================================
;                            ASSEMBLY ROUTINES
; ===========================================================================

    AREA    MISCASM_CODE, CODE, READONLY

;============================================================================
;
; FUNCTION: miscasm_write_burst_data
;
; DESCRIPTION
;   Writes data to a burst interface register.  A specified number of   rds 
;   words are written to a 32-bit register.                             the
;   It is assumed that the register is ready to receive                 eive
;   data when called.
;
; FORMAL ARGUMENTS
;   R0 is the source address for the data to be loaded.
;   R1 is the destination register address
;   R2 is the number of words to load.   
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================

    CODE16
miscasm_write_burst_data

; --------------------------------------------------------------------
; Switch to ARM mode, set up to switch to THUMB on return, 
; and save registers.
; --------------------------------------------------------------------

    bx      pc                   ; Enter ARM mode 
                                 ;  (Bit 0 of the PC is always 0)
 
    ALIGN
    CODE32
    orr     lr, lr , #01         ; Set up the return to THUMB mode by 
                                 ;  setting bit 0 to 1.

    stmfd   r13!, {r4-r12}       ; Save regs on stack (push)

    cmp     r2, #10              ; Check for at least 10 words
    blt     do_jump_copy         ; If less, branch to use jump table

; --------------------------------------------------------------------
; Begin loop to copy 10 word blocks (40 bytes) at a time
; --------------------------------------------------------------------
copy_block

  ; ------------------------------------------------------------------
  ; Copy 10 words (40 bytes)
  ;   r0 - source (memory)
  ;   r1 - destination (register)
  ;   r2 - number of words to copy
  ;   r3 - r12 are used for copy operation.
  ; ------------------------------------------------------------------
 
    ldmia   r0!, {r3-r12}        ; Increment source ptr during copy
    stmia   r1, {r3-r12}         ; 

    subs    r2,r2, #10           ; Subtract 10 from counter
    cmp     r2, #10              ; Check for more 10 word blocks
    bge     copy_block           ; loop if 10 or more words left

; --------------------------------------------------------------------
; End block copy loop (exits when r2<10)
; --------------------------------------------------------------------

do_jump_copy

; --------------------------------------------------------------------
; Use jump table to copy remaining words
;   r0 - source
;   r1 - destination 
;   r2 - words remaining
; --------------------------------------------------------------------

    adr     r3, jmp_table        ; result to index the jump table and
    ldr     pc,[r3,r2,LSL#2]     ; jump to the corresponding location

one_word                         
    ldr     r3, [r0]             ; copy one register
    str     r3, [r1]
    b       jump_copy_done

two_words
    ldmia   r0, {r4-r5}          ; copy two registers
    stmia   r1, {r4-r5}
    b       jump_copy_done

three_words
    ldmia   r0, {r4-r6}          ; copy three registers
    stmia   r1, {r4-r6}
    b       jump_copy_done

four_words
    ldmia   r0, {r4-r7}          ; copy four registers
    stmia   r1, {r4-r7}
    b       jump_copy_done

five_words
    ldmia   r0, {r4-r8}          ; copy five registers
    stmia   r1, {r4-r8}
    b       jump_copy_done

six_words
    ldmia   r0, {r4-r9}          ; copy six words
    stmia   r1, {r4-r9}
    b       jump_copy_done

seven_words
    ldmia   r0, {r4-r10}         ; copy seven words
    stmia   r1, {r4-r10}
    b       jump_copy_done

eight_words
    ldmia   r0, {r3-r10}         ; copy eight words
    stmia   r1, {r3-r10}
    b       jump_copy_done

nine_words
    ldmia   r0, {r3-r11}         ; copy nine words
    stmia   r1, {r3-r11}

jump_copy_done

; --------------------------------------------------------------------
; Clean up and return.
; --------------------------------------------------------------------

    ldmfd   r13!, {r4-r12}       ; Restore registers        
    bx      lr                   ; Return.  Since lr was adjusted before, 
                                 ; it will return in THUMB mode.
                                 
jmp_table
       DCD  jump_copy_done
       DCD  one_word
       DCD  two_words
       DCD  three_words
       DCD  four_words
       DCD  five_words
       DCD  six_words
       DCD  seven_words
       DCD  eight_words
       DCD  nine_words


;============================================================================
;
; FUNCTION: miscasm_read_burst_data
;
; DESCRIPTION
;   Reads data from a 32-bit burst interface register and transfer it to 
;   memory.
;
; FORMAL ARGUMENTS
;   R0 is the source register address.
;   R1 is the destination memory address.
;   R2 is the number of words to load.   
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================

    CODE16
miscasm_read_burst_data

; --------------------------------------------------------------------
; Switch to ARM mode, set up to switch to THUMB on return, 
; and save registers.
; --------------------------------------------------------------------

    bx      pc                   ; Enter ARM mode 
                                 ;  (Bit 0 of the PC is always 0)
 
    ALIGN
    CODE32
    orr     lr, lr , #01         ; Set up the return to THUMB mode by 
                                 ;  setting bit 0 to 1.

    stmfd   r13!, {r4-r12}       ; Save regs on stack (push)

    cmp     r2, #10              ; Check for at least 10 words
    blt     do_jump_copy         ; If less, branch to use jump table

; --------------------------------------------------------------------
; Begin loop to copy 10 word blocks (40 bytes) at a time
; --------------------------------------------------------------------
read_copy_block

  ; ------------------------------------------------------------------
  ; Copy 10 words (40 bytes)
  ;   r0 - source (register)
  ;   r1 - destination (memory)
  ;   r2 - number of words to copy
  ;   r3 - r12 are used for copy operation.
  ; ------------------------------------------------------------------
 
    ldmia   r0, {r3-r12}         ; Increment source ptr during copy
    stmia   r1!, {r3-r12}        ; 

    subs    r2,r2, #10           ; Subtract 10 from counter
    cmp     r2, #10              ; Check for more 10 word blocks
    bge     read_copy_block      ; loop if 10 or more words left

; --------------------------------------------------------------------
; End block copy loop (exits when r2<10)
; --------------------------------------------------------------------

    b       do_jump_copy


; =========================================================================
;                              END OF MODULE
; =========================================================================
                  
    END
        
