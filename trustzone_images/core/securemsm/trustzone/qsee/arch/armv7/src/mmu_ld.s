;============================================================================
;
;                  Long Descriptor MMU Functions
;
; Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved
;============================================================================

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/mmu_ld.s#1 $
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  ---     ---------------------------------------------------------
; 08/13/13   pre     Initial revision.
;============================================================================

    GET     qctps_common_macros.inc

;============================================================================
;                             MODULE IMPORTS
;============================================================================
    IMPORT  mair0
    IMPORT  mair1
	IMPORT  tzbsp_int_disable_all
  	IMPORT  tzbsp_int_restore

;============================================================================
;                             MODULE EXPORTS
;============================================================================
    EXPORT  mmu_set_ttbrs
    EXPORT  mmu_enable_tex_remap
    EXPORT  tzbsp_vtop

   AREA    ARM_MMU, CODE, READONLY
   PRESERVE8
   CODE32

;============================================================================
; mmu_set_ttbrs
;
; ARGS
;   r0 low 32 bits of base
;   r1 high 32 bits of base
;
; DESCRIPTION
;   Sets a value that defines the Translation Table Base boundary that will
;   be used by TTB Registers, and loads Translation Table Base Register 0
;   with the base address of page table.
;   Default value of N after power up is 0 for backward compatible. Any
;   change to the Translation Table Base Register 0 needs to be paired with
;   write to Translation Table Base Control Register.
;============================================================================
mmu_set_ttbrs  FUNCTION
    ; TODO: make memory attributes cacheable
    ; TTBCR - TTB Control Register
    ;   EAE IMP SH1 ORGN1 IRGN1 EPD1 A1 U/S T1SZ
    ; 0b  1   0  11    01    10    0  0 000  000
    ;   U/S SH0 ORGN0 IRGN0 EPD0  U/S T0SZ
    ;    00  11    01    10    0 0000  000 = 0xB6003600
    mov     r2, #0x3600
    movt    r2, #0xB600
    mcr     p15, 0, r2, c2, c0, 2

    ; TTBR0 - Translation Table Base Register 0
    ;         U/S     ASID      U/S BADDR[39:x] U/S[x:0]
    ; 0b 00000000 00000000 00000000        addr
    mov     r1, #0x0
    mcrr    p15, 0, r0, r1, c2

    bx      lr
    ENDFUNC

;============================================================================
; Enable TeX Remap
;   tex remap scheme described in tzbsp_mmu_config.h
;============================================================================
mmu_enable_tex_remap FUNCTION
    ldr     r0, =mair0
    ldr     r0, [r0]
    WCP15_MAIR0 r0

    ldr     r0, =mair1
    ldr     r0, [r0]
    WCP15_MAIR1 r0

    ; Enable TEX remap
    RCP15_SCTLR r0
    orr     r0, r0, #0x10000000
    WCP15_SCTLR r0

    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_vtop
;
;  ARGUMENTS -
;        uint32 virtual_addr
;        uint32 physical_addr
;
;  DESCRIPTION -
;        Converts input, virtual_addr, to physical address
;
;  NOTE -
;        This function is not interrupt safe
;============================================================================
tzbsp_vtop FUNCTION
	; r0,r1 will be used in disable interrupts routine, so save these in r4,r5
    ; we are assuiming stack is setup correctly at this point !!
    push    {r4-r5,lr}
    mov     r4, r0
    mov     r5, r1
    blx     tzbsp_int_disable_all
    ; save the result of the above call in r5
    ; restore back r0 and r1 from r4,r5
    mov     r1, r5
    mov     r5, r0
    mov     r0, r4
    mcr     p15, 0, r0, c7, c8, 0  ; V2PCWPR (privledge current security read)
    isb
    mrc     p15, 0, r2, c7, c4, 0  ; Read PAR
    isb

    ; bit 0 indicates success with 0
    ands    r3, r2, #1
    bne     tzbsp_vtop_err_return

    mov     r3, #0xF000
    movt    r3, #0xFFFF
    and     r2, r3                 ; r2 = r2 & 0xFFFFF000

    mov     r3, #0xFFF
    and     r0, r3                 ; r0 = r0 & 0xFFF
    orr     r0, r2                 ; r0 = r2 | r0

    str     r0, [r1]
    ; we should be sending the return in r0..put it temporarily in r4        
    mov     r4, #0                 ; Return E_SUCCESS
	b       tzbsp_vtop_return_back

tzbsp_vtop_err_return
   ; we should be sending the return in r0..put it temporarily in r4
   mov     r4, #-1          ; Return -E_FAILURE       
   
tzbsp_vtop_return_back
   ; restore expects a value in r0 so copy it from r5
    mov     r0, r5
	blx     tzbsp_int_restore
	; copy the intended return value from r4 to r0
	mov     r0, r4
	; restore the r4, r5 and lr
	pop     {r4-r5,lr}
    bx      lr

    ENDFUNC

   END
