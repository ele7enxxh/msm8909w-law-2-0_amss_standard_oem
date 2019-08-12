;============================================================================
;
;             Scorpion architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   This file contains the Scorpion hierarchichal cache and mmu control functions.
;   File originaly taken from: 
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
; 01/25/11  pre     Initial Version
;============================================================================
;============================================================================
;                             MODULE DEFINES
;============================================================================
;============================================================================
;                             MODULE INCLUDES
;============================================================================
    GET tzbsp_libarm_asm.inc
;============================================================================
;                             MODULE IMPORTS
;============================================================================
;============================================================================
;                             MODULE EXPORTS
;============================================================================
    EXPORT  tzbsp_cache_flush
    EXPORT  tzbsp_dcache_enable
    EXPORT  tzbsp_dcache_disable
    EXPORT  tzbsp_dcache_is_enabled
    EXPORT  tzbsp_dcache_clean_all
    EXPORT  tzbsp_dcache_inval_all
    EXPORT  tzbsp_dcache_flush_all
    EXPORT  tzbsp_dcache_clean_region
    EXPORT  tzbsp_dcache_inval_region
    EXPORT  tzbsp_dcache_flush_region
    EXPORT  tzbsp_dcache_inval_l1
    EXPORT  tzbsp_dcache_inval_l2
    EXPORT  tzbsp_dcache_clean_lx
    EXPORT  tzbsp_dcache_clean_inv_lx
    EXPORT  tzbsp_dcache_iterate_set_way
    EXPORT  tzbsp_icache_enable
    EXPORT  tzbsp_icache_disable
    EXPORT  tzbsp_icache_is_enabled
    EXPORT  tzbsp_icache_inv
    EXPORT  tzbsp_icache_inv_area
;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
   AREA    ARM_CACHE, CODE, READONLY
   PRESERVE8
   CODE32
;============================================================================
; MACRO dcache_granule_size
;
; ARGS
;   $reg - cache granule size in bytes
;   $tmp - cache granule size in words used for calculation
;          of the size in bytes
;
; DESCRIPTION
;   cache_granule_size - get the size of the smallest dcache/unified
;   cache (available on ARMv7+). Note that the value of $reg and $tmp
;   will be modified.
;============================================================================
  MACRO
    dcache_granule_size $reg, $tmp
    mrc     p15, 0, $tmp, c0, c0, 1 ; read CTR
    lsr     $tmp, #16
    and     $tmp, #0xF              ; $tmp = CTR[DminLine]
    mov     $reg, #4                ; CTR[DminLine] = log2 num words
    lsl     $reg, $reg, $tmp        ; cache writeback granule size in bytes
  MEND
; void tzbsp_dcache_enable(void)
tzbsp_dcache_enable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    orr     r0, r0, #(1 << 2)       ; SCTLR[C]=1.
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR.
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_dcache_disable(void)
tzbsp_dcache_disable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #~(1 << 2)      ; SCTLR[C]=0.
    mcr     p15, 0, r0, c1, c0, 0   ; Write SCTLR.
    mdsb
    misb
    bx      lr
    ENDFUNC
; boolean tzbsp_dcache_is_enabled(void)
tzbsp_dcache_is_enabled FUNCTION
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #(1 << 2)       ; Extract SCTLR[C].
    lsr     r0, r0, #2              ; Move bit to position 0.
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_iterate_set_way
;
; DESCRIPTION -
;   Iterates through the set/way and calls function passed in
;============================================================================
tzbsp_dcache_iterate_set_way FUNCTION
    stmfd   sp!, {r0-r12, lr}
    mov     r6, r0
    DSB                             ; ensure ordering with previous
                                    ; memory  accesses
    mrc     p15, 1, r11, c0, c0, 1  ; read clidr
    ands    r3, r11, #0x7000000     ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     iterate_finished        ; if loc is 0, then no need to clean
    mov     r10, #0                 ; start clean at cache level 0
iterate_loop1
    add     r2, r10, r10, lsr #1    ; work out 3x current cache level
    mov     r1, r11, lsr r2         ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache only
    cmp     r1, #2                  ; see what cache we have at this level
    blt     iterate_skip            ; skip if no cache, or just i-cache
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache lines
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way size
    clz     r5, r4                  ; find bit position of way size increment
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index size
iterate_loop2
    mov     r9, r4                  ; create working copy of max way size
iterate_loop3
    orr     r0, r10, r9, lsl r5     ; factor way and cache number into r0
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    blx     r6                      ; Jump to function for given cache line
    subs    r9, r9, #1              ; decrement the way
    bge     iterate_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     iterate_loop2
iterate_skip
    DSB                             ; A7 errata 814220
    add     r10, r10, #2            ; increment cache number
    cmp     r3, r10
    bgt     iterate_loop1
iterate_finished
    mov     r10, #0                 ; swith back to cache level 0
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r12, pc}
    ENDFUNC
;============================================================================
; tzbsp_cache_flush
;
; DESCRIPTION -
;  Flush the entire cache system.  The data cache flush is achieved
;  using atomic clean / invalidates working outwards from L1
;  cache. This is done using Set/Way based cache maintainance
;  instructions.  The instruction cache can still be invalidated back
;  to the point of unification in a single instruction.  This also
;  flushes the branch target cache.
;============================================================================
tzbsp_cache_flush FUNCTION
    push    {lr}
    bl      tzbsp_dcache_flush_all
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU: instruction & branch target
                                    ; invalidate
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_line
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
tzbsp_dcache_clean_line FUNCTION
    mcr     p15, 0, r0, c7, c10, 2 ; clean by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_all
;
; DESCRIPTION -
;   Clean the whole D-cache.
;============================================================================
tzbsp_dcache_clean_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_clean_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_line
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
tzbsp_dcache_flush_line FUNCTION
    mcr     p15, 0, r0, c7, c14, 2 ; clean & invalidate by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_all
;
; DESCRIPTION -
;   Flush the whole D-cache.
;============================================================================
tzbsp_dcache_flush_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_flush_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_line
;
; DESCRIPTION -
;   Invalidate a single cache line by set/way
;============================================================================
tzbsp_dcache_inval_line FUNCTION
    mcr     p15, 0, r0, c7, c6, 2  ; invalidate by set/way
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_all
;
; DESCRIPTION -
;   Invalidate the whole D-cache.
;============================================================================
tzbsp_dcache_inval_all FUNCTION
    push    {lr}
    DSB
    ldr     r0, =tzbsp_dcache_inval_line
    bl      tzbsp_dcache_iterate_set_way
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_l1
;
; DESCRIPTION -
;   Invalidate the l1 section of the D-cache.
;============================================================================
tzbsp_dcache_inval_l1 FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     il1_finished            ; if loc is 0, then no need to clean
    mov     r8, #0                  ; start clean at cache level 0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache only
    cmp     r1, #2                  ; see what cache we have at this level
    blt     il1_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache lines
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way size
    clz     r5, r4                  ; find bit position of way size increment
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index size
il1_loop2
    mov     r9, r4                  ; create working copy of max way size
il1_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r0
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c6, 2   ; invalidate by set/way
    subs    r9, r9, #1              ; decrement the way
    bge     il1_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     il1_loop2
il1_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_inval_l2
;
; DESCRIPTION -
;   Invalidate the l2 section of the D-cache.
;=======================================================================
tzbsp_dcache_inval_l2 FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     il2_finished            ; if loc is 0, then no need to clean
    mov     r8, #1                  ; start clean at cache level 1
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     il2_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
il2_loop2
    mov     r9, r4                  ; create working copy of max way siz
il2_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c6, 2   ; invalidate by set/way
    subs    r9, r9, #1              ; decrement the way
    bge     il2_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     il2_loop2
il2_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_clean_lx
;
; DESCRIPTION -
;   Clean L1/L2 D-cache.
;   r0: 0 for L1 & 1 for L2
;=======================================================================
tzbsp_dcache_clean_lx FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     ilx_finished            ; if loc is 0, then no need to clean
    mov     r8, r0                  ; start clean at cache level r0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     ilx_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
ilx_loop2
    mov     r9, r4                  ; create working copy of max way siz
ilx_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c10, 2  ; clean by set/way
    subs    r9, r9, #1 
    bge     ilx_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     ilx_loop2
ilx_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;=======================================================================
; tzbsp_dcache_clean_inv_lx
;
; DESCRIPTION -
;   Clean and invalidate L1/L2 D-cache.
;   r0: 0 for L1 & 1 for L2
;=======================================================================
tzbsp_dcache_clean_inv_lx FUNCTION
    DSB
    stmfd   sp!, {r0-r9}
    mrc     p15, 1, r6, c0, c0, 1   ; read clidr
    ands    r3, r6, #0x7000000      ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     ilx_finished            ; if loc is 0, then no need to clean
    mov     r8, r0                  ; start clean at cache level r0
    add     r2, r8, r8, lsl #1      ; work out 3x current cache level
    mov     r1, r6, lsr r2          ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache
    cmp     r1, #2                  ; see what cache we have at this lev
    blt     inv_ilx_finished            ; skip if no cache, or just i-cache
    mcr     p15, 2, r8, c0, c0, 0   ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache li
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way siz
    clz     r5, r4                  ; find bit position of way size incr
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index si
inv_ilx_loop2
    mov     r9, r4                  ; create working copy of max way siz
inv_ilx_loop3
    orr     r0, r8, r9, lsl r5      ; factor way and cache number into r
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    mcr     p15, 0, r0, c7, c14, 2  ; clean & invalidate by set/way
    subs    r9, r9, #1 
    bge     inv_ilx_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     inv_ilx_loop2
inv_ilx_finished
    mov     r8, #0                 ; swith back to cache level 0
    mcr     p15, 2, r8, c0, c0, 0  ; select current cache level in cssr
    DSB
    ISB
    ldmfd   sp!, {r0-r9}
    bx      lr
    ENDFUNC
;============================================================================
; tzbsp_dcache_flush_region
;
; PROTOTYPE
;   void tzbsp_dcache_flush_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans and invalidates a memory region in the cache. Please note that the
;   data in the cache would be written back to main memory if dirty and the
;   region invalidated. So any further access to the data would result in a
;   cache-miss.
;============================================================================
tzbsp_dcache_flush_region FUNCTION
    stmfd   sp!, {r0, lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; get the end address
    dcache_granule_size r2, r3      ; r2 = cache granule in bytes
    sub     r3, r2, #1              ; r3 = (cache granule size - 1)
    bic     r0, r0, r3              ; r0 = start address with cache granule
                                    ;      size bits removed
flush_loop
    mcr     p15, 0, r0, c7, c14, 1  ; clean & invalidate line
    add     r0, r0, r2              ; r0 = (r0 + cache granule size)
    cmp     r0, r1                  ; r0 > "end of region" ?
    blt     flush_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {r0, pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_inval_region
;
; PROTOTYPE
;   void tzbsp_dcache_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Invalidate a memory region in the cache. Please note that the data in
;   the cache will not be written back to the main memory. However, any
;   further accesses to data in this region would result in a cache-miss.
;============================================================================
tzbsp_dcache_inval_region FUNCTION
    stmfd   sp!, {lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; r1 = end address
    dcache_granule_size r2, r3      ; r2 = cache granule size in bytes
    sub     r3, r2, #1              ; r3 = (cache granule size - 1)
    bic     r0, r0, r3              ; r0 = start address with cache granule
                                    ;      size bits removed
inval_loop
    mcr     p15, 0, r0, c7, c6, 1   ; invalidate dcache granule
    add     r0, r0, r2              ; r0 += dcache granule size
    cmp     r0, r1
    blt     inval_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {pc}
    ENDFUNC
;============================================================================
; tzbsp_dcache_clean_region
;
; PROTOTYPE
;   void tzbsp_dcache_clean_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache. Please note that this will write
;   back any data that is dirty but it will not invalidate the cache region.
;   So any further accesses to data in this region would result in a cache-hit.
;============================================================================
tzbsp_dcache_clean_region FUNCTION
    stmfd   sp!, {lr}
    DSB                             ; data barrier before flushing
    add     r1, r0, r1              ; get the end address
    dcache_granule_size r2, r3
    sub     r3, r2, #1
    bic     r0, r0, r3
clean_loop
    mcr     p15, 0, r0, c7, c10, 1  ; clean D / U line
    add     r0, r0, r2
    cmp     r0, r1
    blt     clean_loop
    DSB                             ; ensure all memory operations are complete
    ldmfd   sp!, {pc}
    ENDFUNC
; void tzbsp_icache_enable(void)
tzbsp_icache_enable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read from CP15 Control Register
    orr     r0, r0, #(1 << 12)      ; Enable L1 Instruction Cache
    mcr     p15, 0, r0, c1, c0, 0   ; Write back to CP15 Control Register
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_icache_disable(void)
tzbsp_icache_disable FUNCTION
    mdsb
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read from CP15 Control Register
    and     r0, r0, #~(1 << 12)     ; Disable L1 Instruction Cache
    mcr     p15, 0, r0, c1, c0, 0   ; Write back to CP15 Control Register
    mdsb
    misb
    bx      lr
    ENDFUNC
; boolean tzbsp_icache_is_enabled(void)
tzbsp_icache_is_enabled FUNCTION
    misb
    mrc     p15, 0, r0, c1, c0, 0   ; Read SCTLR.
    and     r0, r0, #(1 << 12)      ; Extract SCTLR[I].
    lsr     r0, r0, #12             ; Move bit to position 0.
    bx      lr
    ENDFUNC
; void tzbsp_icache_inv(void)
tzbsp_icache_inv FUNCTION
    mdsb
    misb
    ; ICIALLUIS (PoU, Inner Shareable)
    mcr     p15, 0, r0, c7, c1, 0   ; r0 is ignored by ICIALLUIS
    mdsb
    misb
    bx      lr
    ENDFUNC
; void tzbsp_icache_inv_area(void* start, void* end)
tzbsp_icache_inv_area FUNCTION
    mdsb
    misb
    cmp     r0, r1
    bls     %f0                     ; start <= end
    mov     r2, r0                  ; start > end, need to swap
    mov     r0, r1                  ; Not swapping by XORing, plenty of regs
    mov     r1, r2
    ; ICIMVAU addresses in range [start, end] (PoU)
0   mcr     p15, 0, r0, c7, c5, 1
    add     r0, r0, #4
    cmp     r0, r1
    bls     %b0                     ; if(r0 <= r1) then invalidate(r0)
    mdsb
    misb
    bx      lr
    ENDFUNC
    END
