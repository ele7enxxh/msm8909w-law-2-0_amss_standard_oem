#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/systemdebug/sysdbg/src/target/msm8909/sysdbg_cache.s"
;========================================================================
;
;	Macros and Functions used to do cache related work 
;
;========================================================================

;========================================================================
;							MODULE CONSTANTS
;========================================================================
DMINLINE_SHFT			EQU		0x10
DMINLINE_MASK			EQU		0xF
CLIDR_LOC_MASK			EQU		0x7000000
CLIDR_LOC_SHFT			EQU		0x17
CCSIDR_LINESZ_MASK		EQU		0x7
CCSIDR_ASSOC_MASK		EQU		0x3FF
CCSIDR_ASSOC_SHFT		EQU		0x3
CCSIDR_NUMSETS_MASK		EQU		0x7FFF
CCSIDR_NUMSETS_SHFT		EQU		0xD
DCCISW_LEVEL_SHFT		EQU		0x1
DCCISW_SBZ_SHFT			EQU		0x4	
;========================================================================
;							MODULE EXPORTS
;========================================================================

	IMPORT	tzbsp_dcache_iterate_set_way
	EXPORT	sysdbg_dcache_flush

;========================================================================
;							MODULE DATA / CODE
;========================================================================

	PRESERVE8
	AREA ARM_CACHE, CODE, READONLY 
	CODE32


;============================================================================
; sysdbg_dcache_flush
;
; DESCRIPTION -
;   Flushes caches based on its validity, only called if at least one level of cache is valid
;   Uses existing TZ functions that iterates through all levels of cache
;   TZ API goes through all levels of cache, but we replace the standard flush cache line func with a check for validity, inefficient but leverages existing code
;   
;============================================================================
sysdbg_dcache_flush FUNCTION
    stmfd sp!, {r0-r12, lr}
    DSB
    lsl     r1, r1, #1
    orr      r0, r0, r1
    ldr     r1,=sysdbg_cache_validity; saving valid flags into buffer area
    str     r0, [r1]
    ldr     r0, =sysdbg_dcache_flush_line_if_valid ;flush l1 cache
    bl      tzbsp_dcache_iterate_set_way 
    
    ldmfd sp!, {r0-r12, pc}
    ENDFUNC	

;============================================================================
; sysdbg_dcache_flush_line_if_valid
;
; DESCRIPTION -
;   Flush a single cache line by set/way only if current cache level is valid
;============================================================================
sysdbg_dcache_flush_line_if_valid FUNCTION
    stmfd   sp!, {r0-r4, lr}
    ldr     r1, =sysdbg_cache_validity
    ldr     r2, [r1]
    mrc     p15, 2, r3, c0, c0, 0 	; 0'b00 represents lvl1, 0'b01 represents lvl2
    add     r3, r3, #1 			; 0'b01 represents lvl1, 0'b10 represents lvl2
    and     r3, r3, r2			; AND current cache level with cache valid bits
    cmp     r3, #0			; Any value other than 0 indicates current cache is valid
    beq     flush_line_end
flush_line 
    mcr     p15, 0, r0, c7, c14, 2 ; clean & invalidate by set/way only if current cache is valid
flush_line_end
    ldmfd   sp!, {r0-r4, pc}
    ENDFUNC	

;=======================================================================
;
;		Data for the module 
;
;======================================================================= 
    AREA SYSDBG_CACHE_DATA , DATA, READWRITE

; IMPORT dbi_cpu_regs

sysdbg_cache_validity 	 SPACE 	0x4 ; bit 0 represents l1 cache validity and bit0 represents l2


    END


