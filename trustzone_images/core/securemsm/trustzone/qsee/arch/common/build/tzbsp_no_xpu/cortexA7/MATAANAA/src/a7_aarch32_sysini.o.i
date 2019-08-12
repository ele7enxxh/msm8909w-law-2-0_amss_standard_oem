;-------------------------------------------------------------------------------
; SYSINI
; Version: 3.8 (experimental)
; CPU:     Cortex-A7
; Mode:    AArch32
;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------
   AREA RO, CODE, READONLY
   ARM
   ; Export functions and global variables
   EXPORT      a7_aarch32_sysini
   PRESERVE8
;-------------------------------------------------------------------------------
;-----------------------------------------------------------------------
; a7_aarch32_sysini
; Inputs- None
; Description - This function initializes several implementation defined
; CPU registers. Fixes for several ARM errata are also applied here.
;-------------------------------------------------------------------------------
a7_aarch32_sysini
   ; ---------------------------------------------------------------------------
   ; Set SMP Bit -  must be set before caches and MMU are enabled
   ; ---------------------------------------------------------------------------
   MRC p15, 0, r0, c1, c0, 1 ; Read Auxiliary Control Register
   ORR r0, r0, #(0x1 << 6)   ; Set SMP bit
   MCR p15, 0, r0, c1, c0, 1 ; Write Auxiliary Control Register
   ISB
   DSB
   ; ---------------------------------------------------------------------------
   ; Disable Adv. SIMD & FP through Floating-point Exception Control register
   ; FPEXC
   ; ---------------------------------------------------------------------------
   ;MOV r0, #0
   ;VMSR FPEXC, r0; Write Floating-point Exception Control Register
   ;ISB
   ; ---------------------------------------------------------------------------
   ; Disable VFP / NEON, remove full access to CP 10/11 space
   ; through CPACR register
   ; ---------------------------------------------------------------------------
    MOV r0, #0
    MCR  p15, 0, r0, c1, c0, 2
    ISB                            
   ; ---------------------------------------------------------------------------
   ; Enable non-secure access to coprocessors 10, 11 through NSACR
   ; Non-Secure Access Control Register
   ; ---------------------------------------------------------------------------
  ; MRC p15, 0, r0, c1, c1, 2 ; Read Non-Secure Access Control Register data
  ; MOV r1, #3<<10;
  ; BIC r0, r0, r1        ; Write 0b11 to bits 11:10
  ; MCR p15, 0, r0, c1, c1, 2 ; Write Non-Secure Access Control Register data
   ; ---------------------------------------------------------------------------
   ; Disable Secure Invasive and Non-Invasive Debug
   ; through SDER - Secure Debug Enable Register
   ; This is a Restricted access register, meaning it exists only in the Secure
   ; state.
   ; This should ideally be set by Secure Monitor code
   ; ---------------------------------------------------------------------------
   MRC p15, 0, R0, c1, c1, 1 ; Read SDER into R0
   BIC R0, R0, #3 ; Disable bits [0] and [1]
   MCR p15, 0, R0, c1, c1, 1 ; Write R0 to SDER
   ISB
   ; --------------------------------------------------------------------------
   ; Disable debugging through SDCR - Secure Debug Configuration Register
   ; --------------------------------------------------------------------------
   ;MRC p15, 0, R0, c1, c3, 1 ; Read SDCR into R0
   ;MOV R1, #3<<14
   ;BIC R0, R0, R1; Clear bits 14 and 15
   ;MOV R1, #1<<17
   ;BIC R0, R0, R1; Clear bit 17
   ;ORR R0, R0, #3<<20 ; Set bits 20 and 21
   ;MCR p15, 0, R0, c1, c3, 1 ; Write R0 to SDCR
   ;ISB
   DSB
   ISB ;Make sure all changes are propagated before returning from sysini
   BX lr
   END ; End of sysini 
