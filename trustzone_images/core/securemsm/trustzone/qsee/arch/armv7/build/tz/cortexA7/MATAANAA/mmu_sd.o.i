;============================================================================
;
;                  Short Descriptor MMU Functions
;
; Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved
;============================================================================
;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; $Header: 
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  ---     ---------------------------------------------------------
; 08/13/13   pre     Initial revision.
;============================================================================
        GBLS    current_node_name
        GBLS    current_node_type
        GBLA    current_node_aregcount
        GBLA    current_node_vregcount
        MACRO
        pusha   $stack, $reg
        str     $reg, [$stack, #-4]!
        MEND
        MACRO
        popa    $stack, $reg
        ldr     $reg, [$stack], #4
        MEND
        MACRO
        ENTRY_NODE $node_name
current_node_name       SETS    "$node_name"
        EXPORT  $node_name
        ALIGN
        ROUT
$node_name
        MEND
                                        ;  The end of an entry node.
        MACRO
        ENTRY_NODE_END
$current_node_name._exit
current_node_name       SETS    ""
        MEND
        MACRO
        LEAF_NODE $node_name
current_node_type       SETS    "Leaf"
        CODE16
        ENTRY_NODE $node_name
        bx      pc
        ALIGN
        CODE32
        EXPORT  $node_name._32
$node_name._32
        MEND
        MACRO
        LEAF_NODE_END $node_name
        ASSERT  "$current_node_type" = "Leaf"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND
        MACRO
        LEAF_NODE_16 $node_name
current_node_type       SETS    "Leaf16"
        CODE16
        ENTRY_NODE $node_name
        MEND
        MACRO
        LEAF_NODE_END_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND
        MACRO
        ALTERNATE_ENTRY_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        EXPORT $node_name
$node_name
        MEND
        MACRO
        blatox     $destreg
        ROUT
        tst     $destreg, #0x01          
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
    GET     qctps_common_macros.inc
;============================================================================
;                             MODULE IMPORTS
;============================================================================
    IMPORT  tzbsp_nmrr
    IMPORT  tzbsp_prrr
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
    ;TTBCR - TTB Control Register
    ; 2:0 - N - Size of TTBR0 translation table
    ;      0b000 - 16KB (default)
    ;      0b001 -  8KB
    ;      0b010 -  4KB
    ;      0b011 -  2KB
    ;      0b100 -  1KB
    ;      0b101 - 512B
    ;      0b110 - 256B
    ;      0b111 - 128B
    mov     r2, #(((1 << 5)))
    mcr     p15, 0, r2, c2, c0, 2
    ;TTBR0 - Translation Table Base Register 0
    ; 6 IRGN[0] Inner Region Cache Policy
    ;   (see IRGN[1] for definition of full field)
    ; 5 NOS Not Outer Shareable
    ;   1 = inner shareable
    ;   0 = outer shareable
    ; 4:3 ORGN[1:0] Outer Region Cache
    ;   Policy
    ;   0b00 = non-cacheable
    ;   0b01 = copy-back, write-allocate
    ;   0b10 = write-through
    ;   0b11 = copy-back, write-no-allocate
    ; 1 SH Shareable
    ;   0 = Private memory region
    ;   1 = Shareable memory region
    ; 0 IRGN[1] Inner Region Cache Policy
    ;   0b00 = non-cacheable
    ;   0b01 = copy-back, write-allocate
    ;   0b10 = write-through
    ;   0b11 = copy-back, write-no-allocate
    orr     r0, #0x6B
    mcr     p15, 0, r0, c2, c0, 0
    bx      lr
    ENDFUNC
;============================================================================
; Enable TeX Remap
;   tex remap scheme described in tzbsp_mmu_config.h
;============================================================================
mmu_enable_tex_remap FUNCTION
    ldr     r0, =tzbsp_nmrr
    ldr     r0, [r0]
    WCP15_NMRR r0
    ldr     r0, =tzbsp_prrr
    ldr     r0, [r0]
    WCP15_PRRR r0
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
