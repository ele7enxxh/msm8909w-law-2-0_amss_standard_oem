;-----------------------------------------------------------------------
        AREA sysini, CODE
;-----------------------------------------------------------------------

;--3456789 123456789 123456789 123456789 123456789 123456789 123456789 1
;-----------------------------------------------------------------------
;-- QUALCOMM Proprietary
;-- Copyright (c) 2002, Qualcomm Technologies Incorporated.  All rights reserved.
;--
;-- All data and information contained in or disclosed by this document 
;-- confidential and proprietary information of Qualcomm Technologies Incorporated, a
;-- all rights therein are expressly reserved. By accepting this materia
;-- the recipient agrees that this material and the information containe
;-- therein are held in confidence and in trust and will not be used,
;-- copied, reproduced in whole or in part, nor its contents revealed in
;-- any manner to others without the express written permission of QUALC
;-- Incorporated.
;--
;-- This technology was exported from the United States in accordance wi
;-- the Export Administration Regulations. Diversion contrary to U.S. la
;-- prohibited.
;-----------------------------------------------------------------------
;-- RCS File        : $Source: /prj/qct/cores/sparrow_ss/sandiego/Vault/
;-- Revision        : $Revision: #1 $
;-- Last Check In   : $Date: 2016/06/17 $ by $Author: pwbldsvc $
;-- For detailed revision history, use 'rlog <filename>'
;-----------------------------------------------------------------------
;
; This module performs ROM/RAM remapping (if required), initializes stac

; Note: LDR PC instructions are used here because branch (B) instruction
; could not simply be copied (the branch offsets would be wrong).  Also,
; a branch instruction might not reach if the ROM is at an address >32MB
; ------------------------------------------------------------------
; Import some definitions

        EXPORT      a7_sysini

        IMPORT      RST_VEC
        IMPORT      L2_INIT_SEMAPHORE_LOC

        IMPORT      l2_shmoo_hole_workaround
		IMPORT      tzbsp_do_l2_init

        ; Include the common macros
        INCLUDE     qctps_common_macros.inc

        PRESERVE8

;----------------------------------------------------------------------
; init_exec_branch_resources
; A subset of what is required in simulation.
;----------------------------------------------------------------------

init_exec_branch_resources

        ; Set SCTLR[Z] - Branch prediction enable
        RCP15_SCTLR     r1
        ORR             r1, r1, #0x00000800
        WCP15_SCTLR     r1

        ISB

        ; Set VBAR
        LDR             r1, =RST_VEC
        WCP15_VBAR      r1

        ; Clear V-bit
        RCP15_SCTLR    r0                  ;  Read System Control regist
        BIC            r0, r0, #0x2000     ;  Clear V bit 
        WCP15_SCTLR    r0                  ;  Write System Control regis

        ISB

        MOV             r1, lr          ; save return address while ...
        ; Initialize link stack with 8 BLs to sequential addresses
        ; This must be done with BP enabled to have an effect.
        BL      SEQ1
SEQ1    BL      SEQ2
SEQ2    BL      SEQ3
SEQ3    BL      SEQ4
SEQ4    BL      SEQ5
SEQ5    BL      SEQ6
SEQ6    BL      SEQ7
SEQ7    BL      SEQ8
SEQ8    MOV		lr, r1

        BX              lr

;-----------------------------------------------------------------------
; apps_proc_init
;  INPUT       -    
;  DESCRIPTION -
;-----------------------------------------------------------------------
        AREA sysini, CODE
;-----------------------------------------------------------------------
a7_sysini
        ; Push link reg to allow return later
        STMFD sp!, {lr}

        ; Initialize branch prediction and vector base reg
        BL    init_exec_branch_resources
        
        ; ----------------------------------------------------------
        ; Set all domain to have Client type access (limited)
        ; ----------------------------------------------------------
        MOV32 R0, #0x55555555
        MCR   p15, 0, R0, c3, c0, 0

        ; ----------------------------------------------------------
        ; Disable MMU
        ; ----------------------------------------------------------
        MRC   p15, 0, r0, c1, c0, 0       ; Read Control Register config
        BIC   r0, r0, #0x1
        MCR   p15, 0, r0, c1, c0, 0       ; Write Control Register confi

        ; ----------------------------------------------------------
        ; Disable L1 Caches
        ; ----------------------------------------------------------
        MRC   p15, 0, r0, c1, c0, 0       ; Read Control Register config
        BIC   r0, r0, #(0x1 << 12)        ; Disable I Cache
        BIC   r0, r0, #(0x1 << 2)         ; Disable D Cache
        MCR   p15, 0, r0, c1, c0, 0       ; Write Control Register confi

        ; ----------------------------------------------------------
        ; Set SMP Bit
        ; ----------------------------------------------------------
        MRC   p15, 0, r1, c1, c0, 1
        ORR   r1, r1, #(1 << 6)           ; Set SMP Bit
        MCR   p15, 0, r1, c1, c0, 1
        ISB
        DSB

        ; ----------------------------------------------------------
        ; Shmoo hole workaround (A7SS specific)
        ; ----------------------------------------------------------
	       BL    l2_shmoo_hole_workaround

        ; ----------------------------------------------------------
        ; Invalidate Instruction cache
        ; ----------------------------------------------------------
        ; MOV   r0, #0
        ; MCR   p15, 0, r0, c7, c5, 0

        ; ----------------------------------------------------------
        ; Invalidate Branch Prediction
        ; ----------------------------------------------------------
        MOV   r0, #0
        MCR   p15, 0, r0, c7, c5, 6

        ; ----------------------------------------------------------
        ; Invalidate TLB
        ; ----------------------------------------------------------
        MOV   r0, #0
        MCR   p15, 0, r0, c8, c7, 0

        ; ----------------------------------------------------------
        ; BP Global Enable
        ; ----------------------------------------------------------
        MRC   p15, 0, r0, c1, c0, 0       ; Read Control Register config
        ORR   r0, r0, #(0x1 << 11)        ; Global BP Enable
        MCR   p15, 0, r0, c1, c0, 0       ; Write Control Register confi
        
        ; ----------------------------------------------------------
        ; Enable D-side Prefetch
        ; ----------------------------------------------------------
        MRC   p15, 0, r0, c1, c0, 1       ; Read Auxiliary Control Regis
        ORR   r0, r0, #(0x1 <<2)          ; Enable D-side prefetch
        MCR   p15, 0, r0, c1, c0, 1       ; Write Auxiliary Control Regi

        ; ----------------------------------------------------------
        ; Removing SWP instruction as it is not supported .
		      ; Enabling VFP 
		      ; Enable VFP / NEON, give full access to CP 10/11 space
        ; ----------------------------------------------------------
        MOVT r3, #0x00F0
        MCR p15, 0, r3, c1, c0, 2
        MOVT r3, #0x4000
        FMXR FPEXC, r3
        
        ISB
        
        LDMFD       sp!, {lr}
        BX          lr
        END

;;;---------------------------------------------------------------------
;;; End of sysini main routine. Subroutines follow.
;;;---------------------------------------------------------------------




