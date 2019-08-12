;
;
; CPSR Control Masks 
PSR_Fiq_Mask    EQU     0x40
PSR_Irq_Mask    EQU     0x80
; Processor mode definitions 
PSR_Supervisor  EQU     0x13
;
;
        IMPORT  jsdcc_init
        EXPORT  __main
        PRESERVE8
        AREA    StartHere, CODE, READONLY
        CODE32
__main
        ENTRY
;=======================================================================
; MACRO mmu_set_default_cfg
; ARGS
;   NONE
; DESCRIPTION
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor
;=======================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, = 0x00050078
   MCR    p15, 0, r0, c1, c0, 0
   MEND
;=======================================================================
;=======================================================================
; MACRO mmu_enable_i_cache
; ARGS
;  NONE
; DESCRIPTION
;  Enables the I Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
   MACRO
   mmu_enable_i_cache
   mrc     p15, 0 , r0, c1, c0, 0
   orr     r0, r0, # (1 << 12 )
   mcr     p15, 0 , r0, c1, c0, 0
   MEND
;=======================================================================
;
; Supervisor Mode
; Set up the Supervisor stack pointer.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =0x00200000+0x20000
; ======================================================================
; Enable the instruction cache
; ======================================================================
   mmu_set_default_cfg
   mmu_enable_i_cache
; Enter C code execution
        b       jsdcc_init
        END
