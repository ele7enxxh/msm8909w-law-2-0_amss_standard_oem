    EXPORT  VMMM_SetupMMU
    EXPORT  VMMM_Invalidate_BP_ALL
    EXPORT  VMMM_Invalidate_TLB_BY_IPA
    AREA    vm_mm_asm, CODE, READONLY
    CODE32

/* R0, R1 has VTTBR*/
/* R2 has VTCR*/
/* R3 is used as scratch register */
VMMM_SetupMMU
    ; setup stage 2 translation registers
    MCRR    p15, 6, r0, r1, c2           ; Write VTTBR
    MCR     p15, 4, r2, c2, c1, 2        ; Write VTCR
    bx r14

VMMM_Invalidate_BP_ALL
   ; BPIALLIS operation, ignoring the value in Rt
   MCR   p15,0,r0,c7,c1,6 
   DSB
   ISB
   bx       r14

; R0-R1: 64-bit IPA to be invalidated
VMMM_Invalidate_TLB_BY_IPA

   LSL R1, R1, #20
   ORR R0, R1, R0, LSR #12

   ; TLBIIPAS2IS operation
   MCR   p15,4,r0,c8,c0,1 

   DSB
   ISB
   bx       r14

   END
