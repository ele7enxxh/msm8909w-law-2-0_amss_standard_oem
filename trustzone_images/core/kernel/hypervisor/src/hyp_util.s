;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;         T R U S T Z O N E   N O N S E C U R E   F I Q   D E B U G
;
;
; GENERAL DESCRIPTION
;   This file contains the Hypervisor Mutex Assembly codes
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/src/hyp_util.s#1 $
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "hyp_asm.h"


;=======================================================================
;
;                             MODULE IMPORTS
;
;=======================================================================
    EXPORT hyp_mutex_init
    ;EXPORT hyp_mutex_trylock
    EXPORT hyp_mutex_lock
    EXPORT hyp_mutex_unlock
    EXPORT hyp_int_disable_all
    EXPORT hyp_int_restore

;=======================================================================
; MACRO mdmb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data memory barrier, either using the ARMv7 instruction o
;   legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;=======================================================================
   MACRO
   MDMB
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M"
      DMB                        ; RVDS >= 3.0 supports ARMv7 instructio
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF050 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F50 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 5  ; Legacy Data Memory Barrier
   ENDIF
   MEND

    AREA    HYPUTILCODE, CODE, READONLY, ALIGN=5

; int hyp_mutex_initialize(mutex *m)
; uint64 hyp_mutex_init(hyp_mutex_t* mutex)
;
; r0 - Address of the mutex structure
hyp_mutex_initialize   ; Alias for ARM C library
hyp_mutex_init FUNCTION
    MDMB

    MOV     r1, #0
    STR     r1, [r0]
    ; Return non-zero to tell the ARM C libraries that we are running
    ; multithreaded code.
    MOV     r0, #1
    BX      lr
    ENDFUNC

; void hyp_mutex_acquire(mutex *m)
; void hyp_mutex_lock(tzbsp_mutex_t* mutex)
;
; Waits until the provided mutex lock has been reclaimed.
;
; x0 - Address of the mutex structure
hyp_mutex_acquire  ; Alias for ARM C library
hyp_mutex_lock FUNCTION
    ; Check milestone complete flag
    MOV     r1, #1
42  LDREX   r2, [r0]            ; Load the current lock status.
    CMP     r2, #0              ; Is locked?
    STREXEQ r2, r1, [r0]        ; Not locked: try to claim the lock.
    CMPEQ   r2, #0              ; Not locked: Did we get the lock?
    BNE     %b42                ; Locked or claiming lock failed, retry.
    MDMB                        ; Ensure other observers see lock claim.
43  BX      lr
    ENDFUNC

; void hyp_mutex_release(mutex *m)
; void hyp_mutex_unlock(tzbsp_mutex_t* mutex)
;
; x0 - Address of the mutex structure
hyp_mutex_release  ; Alias for ARM C library
hyp_mutex_unlock FUNCTION
    ; Make sure other CPU's observe any writes before releasing the lock.
    MDMB
    MOV     r1, #0              ; Zero equals unlocked.
    STR     r1, [r0]            ; Clear the lock
    BX      lr
    ; Make sure other CPU's observe any writes after releasing the lock.
    MDMB
    ENDFUNC

; uint32 hyp_int_disable_all(void)
;
; x0 - return the A/I/F of SPSR before new int mask 
hyp_int_disable_all FUNCTION
    MRS     r1, cpsr                            ; Read the status register.
    AND     r0, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    ORR     r1, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Set A/I/F bits.
    MSR     cpsr_cx, r1                         ; Apply the new int mask.
    ; R0 contains the A/I/F bits before masking
    BX      lr
    ENDFUNC

; void hyp_int_restore(uint32 flags)
hyp_int_restore FUNCTION
    AND     r0, r0, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    MRS     r1, cpsr                            ; Read the status register.
    BIC     r1, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Clear A/I/F bits.
    ORR     r1, r1, r0                          ; Set requested A/I/F bits.
    MSR     cpsr_cx, r1                         ; Write control & extension field
    BX      LR
    ENDFUNC

    END

