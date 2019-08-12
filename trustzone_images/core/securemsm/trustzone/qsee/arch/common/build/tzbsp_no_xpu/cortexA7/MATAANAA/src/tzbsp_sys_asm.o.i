;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      S Y S
;
;
; GENERAL DESCRIPTION
;   Cache coherency, and managment for armv7.
;
;   Assumptions:
;   - The scope of barriers/operations is Inner Shareable (ISH).
;   - D-cache operations are done to the Point of Coherency (PoC).
;   - Memory is flat mapped -> MVA == VA == PA.
;
; Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
; 05/19/10   tk      Initial version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
    EXPORT tzbsp_mutex_init
    EXPORT tzbsp_mutex_trylock
    EXPORT tzbsp_mutex_lock
    EXPORT tzbsp_mutex_unlock
    EXPORT tzbsp_enable_branch_prediction
    EXPORT tzbsp_bp_invalidate_all
    EXPORT tzbsp_int_enable
    EXPORT tzbsp_int_disable
    EXPORT tzbsp_int_read
    EXPORT tzbsp_int_disable_all
    EXPORT tzbsp_int_restore
    EXPORT tzbsp_dmb
    EXPORT memory_barrier ; Alias for dsb, needed by DMOV.
    EXPORT tzbsp_dsb
    EXPORT tzbsp_isb
    EXPORT wcp15_csselr
    EXPORT rcp15_ccsidr
    EXPORT rcp15_ttbr0
    EXPORT wcp15_ttbr0_safe
    EXPORT wcp15_vbar_safe
    EXPORT wcp15_scr
    IMPORT tzbsp_milestone_complete
    IMPORT tzbsp_cpu_count_per_cluster
    CODE32
    PRESERVE8
    AREA TZBSP_SYS_ASM, align=4, CODE, READONLY
; void tzbsp_enable_branch_prediction
tzbsp_enable_branch_prediction
   ;SCTLR
   mrc    p15, 0, r0, c1, c0, 0     ; Read from CP15 Control Register
   orr    r0, r0, # (1 << 11 )      ; Enable branch prediction
   ;SCTLR
   mcr    p15, 0, r0, c1, c0, 0     ; Write back to CP15 Control Register
   mov    r0, #0                    ; Return success
   bx     lr
; void tzbsp_bp_invalidate_all(void)
tzbsp_bp_invalidate_all FUNCTION
    mcr     p15, 0, r0, c7, c5, 6
    dsb
    isb
    bx      lr
    ENDFUNC
; void tzbsp_int_enable(uint32 flags)
tzbsp_int_enable FUNCTION
    and     r0, r0, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    mrs     r1, cpsr                            ; Read the status register.
    bic     r1, r1, r0                          ; Clear requested A/I/F bits
    msr     cpsr_cx, r1                         ; Write control & extension field
    bx      lr
    ENDFUNC
; void tzbsp_int_disable(uint32 flags)
tzbsp_int_disable FUNCTION
    and     r0, r0, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    mrs     r1, cpsr                            ; Read the status register.
    orr     r1, r1, r0                          ; Set requested A/I/F bits
    msr     cpsr_cx, r1                         ; Write control & extension field
    bx      lr
    ENDFUNC
; int tzbsp_int_read(void)
tzbsp_int_read FUNCTION
  mrs       r0, cpsr
  and       r0, r0,  #(1<<6):OR:(1<<7):OR:(1<<8)
  bx        lr
  ENDFUNC
; uint32 tzbsp_int_disable_all(void)
tzbsp_int_disable_all FUNCTION
    mrs     r1, cpsr                            ; Read the status register.
    and     r0, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    orr     r1, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Set A/I/F bits.
    msr     cpsr_cx, r1                         ; Apply the new int mask.
    ; R0 contains the A/I/F bits before masking
    bx      lr
    ENDFUNC
; void tzbsp_int_restore(uint32 flags)
tzbsp_int_restore FUNCTION
    and     r0, r0, #(1<<6):OR:(1<<7):OR:(1<<8) ; Only care about A/I/F bits.
    mrs     r1, cpsr                            ; Read the status register.
    bic     r1, r1, #(1<<6):OR:(1<<7):OR:(1<<8) ; Clear A/I/F bits.
    orr     r1, r1, r0                          ; Set requested A/I/F bits.
    msr     cpsr_cx, r1                         ; Write control & extension field
    bx      lr
    ENDFUNC
; void tzbsp_dmb(void)
tzbsp_dmb FUNCTION
    mdmb
    bx      lr
    ENDFUNC
memory_barrier ; Alias for tzbsp_dsb
; void tzbsp_dsb(void)
tzbsp_dsb FUNCTION
    mdsb
    bx      lr
    ENDFUNC
; void tzbsp_isb(void)
tzbsp_isb FUNCTION
    misb
    bx      lr
    ENDFUNC
; int _mutex_initialize(mutex *m)
; uint32 tzbsp_mutex_init(tzbsp_mutex_t* mutex)
;
; r0 - Address of the mutex structure
_mutex_initialize   ; Alias for ARM C library
tzbsp_mutex_init FUNCTION
    mdmb
    mov     r1, #0
    str     r1, [r0]
    ; Return non-zero to tell the ARM C libraries that we are running
    ; multithreaded code.
    mov     r0, #1
    bx      lr
    ENDFUNC
; void _mutex_acquire(mutex *m)
; void tzbsp_mutex_lock(tzbsp_mutex_t* mutex)
;
; Waits until the provided mutex lock has been reclaimed.
;
; r0 - Address of the mutex structure
_mutex_acquire  ; Alias for ARM C library
tzbsp_mutex_lock FUNCTION
    ; Check milestone complete flag
    ldr     r1, =tzbsp_milestone_complete
    ldrb    r2, [r1]            
    cmp     r2, #0              
    beq     %f43                ; ... no need to grab the lock
    mov     r1, #1
42  ldrex   r2, [r0]            ; Load the current lock status.
    cmp     r2, #0              ; Is locked?
    strexeq r2, r1, [r0]        ; Not locked: try to claim the lock.
    cmpeq   r2, #0              ; Not locked: Did we get the lock?
    bne     %b42                ; Locked or claiming lock failed, retry.
    mdmb                        ; Ensure other observers see lock claim.
43  bx      lr
    ENDFUNC
; void _mutex_acquire(mutex *m)
; void tzbsp_mutex_trylock(tzbsp_mutex_t* mutex)
;
; Waits until the provided mutex lock has been reclaimed.
;
; r0 - Address of the mutex structure
tzbsp_mutex_trylock FUNCTION
    ; Check milestone complete flag
    ldr     r1, =tzbsp_milestone_complete
    ldrb    r2, [r1]            
    cmp     r2, #0              
    beq     %f43                ; ... no need to grab the lock
    mov     r1, #1
    ldrex   r2, [r0]            ; Load the current lock status.
    cmp     r2, #0              ; Is locked?
    strexeq r2, r1, [r0]        ; Not locked: try to claim the lock.
    cmpeq   r2, #0              ; Not locked: Did we get the lock?
    bne     %f42                ; Locked or claiming lock failed, retry.
    mdmb                        ; Ensure other observers see lock claim.
43  mov     r0, #1
    bx      lr
42  mov     r0, #0
    bx      lr
    ENDFUNC
; void _mutex_release(mutex *m)
; void tzbsp_mutex_unlock(tzbsp_mutex_t* mutex)
;
; r0 - Address of the mutex structure
_mutex_release  ; Alias for ARM C library
tzbsp_mutex_unlock FUNCTION
    ; Make sure other CPU's observe any writes before releasing the lock.
    mdmb
    mov     r1, #0              ; Zero equals unlocked.
    str     r1, [r0]            ; Clear the lock
    bx      lr
    ; Make sure other CPU's observe any writes after releasing the lock.
    mdmb
    ENDFUNC
; void wcp15_csselr(uint32 val)
wcp15_csselr FUNCTION
    mcr p15, 2, r0, c0, c0, 0 ; Write CSSELR
    bx      lr
    ENDFUNC
; uint32 rcp15_ccsidr(void)
rcp15_ccsidr FUNCTION
    mrc p15, 1, r0, c0, c0, 0 ; Read CCSIDR
    bx      lr
    ENDFUNC
;uint32 rcp15_ttbr0(void);
rcp15_ttbr0 FUNCTION
    mrc     p15, 0, r0, c2, c0, 0
    bx      lr
    ENDFUNC
; void wcp15_ttbr0_safe(uint32)
wcp15_ttbr0_safe FUNCTION
    dsb
    mcr     p15, 0, r0, c2, c0, 0
    isb
    mcr     p15, 0, r0, c8, c7, 0
    dsb
    isb
    misb
    bx      lr
    ENDFUNC
; void wcp15_vbar_safe(uint32 val)
wcp15_vbar_safe FUNCTION
    mcr     p15, 0, r0, c12, c0, 0
    dsb
    isb
    bx      lr
    ENDFUNC
; void wcp15_scr(uint32)
wcp15_scr FUNCTION
    mcr     p15, 0, r0, c1, c1, 0
    bx      lr
    ENDFUNC
    END
