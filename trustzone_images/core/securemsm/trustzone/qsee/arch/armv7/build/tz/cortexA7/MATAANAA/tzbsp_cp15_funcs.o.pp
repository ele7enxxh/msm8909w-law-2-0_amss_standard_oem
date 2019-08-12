#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/tzbsp_cp15_funcs.s"
;============================================================================
;
;             ARMv7 CP15 register read/write functions
;
; GENERAL DESCRIPTION
;   Functions for reading and writing select CP15 registers.
;
; Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved
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
; 08/12/12  tk      Initial Version
;============================================================================

;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================

    ; Write to CP15 register.
    ; Creates a function with $fname. R0 is the value written to CP15.
    MACRO
    MCR15 $fname, $op1, $crn, $crm, $op2
    EXPORT  $fname
$fname  FUNCTION
    mcr     p15, $op1, r0, $crn, $crm, $op2
    bx      lr
    ENDFUNC
    MEND

    ; Read from CP15 register.
    ; Creates a function with $fname. R0 is the value read from to CP15.
    MACRO
    MRC15 $fname, $op1, $crn, $crm, $op2
    EXPORT  $fname
$fname  FUNCTION
    mrc     p15, $op1, r0, $crn, $crm, $op2
    bx      lr
    ENDFUNC
    MEND

    ; Write to 64-bit CP15 register.
    ; Creates a function with $fname. R0, R1 are written to CP15.
    MACRO
    MCRR15 $fname, $op1, $crn
    EXPORT  $fname
$fname  FUNCTION
    mcrr    p15, $op1, r0, r1, $crn
    bx      lr
    ENDFUNC
    MEND

    ; Read from 64-bit CP15 register.
    ; Creates a function with $fname. R0, R1 read from CP15.
    MACRO
    MRRC15 $fname, $op1, $crn
    EXPORT  $fname
$fname  FUNCTION
    mrrc    p15, $op1, r0, r1, $crn
    bx      lr
    ENDFUNC
    MEND

    PRESERVE8
    CODE32

    AREA    CP15_FUNCS, CODE, READONLY

; CP15 timer encoding, from beta version of ARMv7 manual.
;
; Name,VMSAa    Name, PMSAa CRn  opc1 CRm opc2 Type Description
; CNTFRQ        CNTFRQ      c14  0    c0  0    RW   Counter Frequency register
; CNTPCT        CNTPCT      c14  0    c14 -    RO   Physical Count register
; CNTKCTL       CNTKCTL     c14  0    c1  0    RW   Timer PL1 Control register
; CNTP_TVAL     CNTP_TVAL   c14  0    c2  0    RW   PL1 Physical TimerValue register
; CNTP_CTL      CNTP_CTL    c14  0    c2  1    RW   PL1 Physical Timer Control register
; CNTV_TVAL     CNTV_TVAL   c14  0    c3  0    RW   Virtual TimerValue register
; CNTV_CTL      CNTV_CTL    c14  0    c3  1    RW   Virtual Timer Control register
; CNTVCT        CNTVCT      c14  1    c14 -    RO   Virtual Count register
; CNTP_CVAL     CNTP_CVAL   c14  2    c14 -    RW   PL1 Physical Timer CompareValue register1
; CNTV_CVAL     CNTV_CVAL   c14  3    c14 -    RW   Virtual Timer CompareValue register
; CNTVOFFb      -           c14  4    c14 -    RW   Virtual Offset register
; CNTHCTLb      -           c14  4    c1  0    RW   Timer PL2 Control register
; CNTHP_TVALb   -           c14  4    c2  0    RW   PL2 Physical TimerValue register
; CNTHP_CTLb    -           c14  4    c2  1    RW   PL2 Physical Timer Control register
; CNTHP_CVALb   -           c14  6    c14 -    RW   PL2 Physical Timer CompareValue register

    MRC15  rcp15_cntfrq,     0, c14, c0, 0
    MRRC15 rcp15_cntpct,     0, c14         ; Read-only
    MRC15  rcp15_cntkctl,    0, c14, c1, 0
    MRC15  rcp15_cntp_tval,  0, c14, c2, 0
    MRC15  rcp15_cntp_ctl,   0, c14, c2, 1
    MRC15  rcp15_cntv_tval,  0, c14, c3, 0
    MRC15  rcp15_cntv_ctl,   0, c14, c3, 1
    MRRC15 rcp15_cntvct,     1, c14         ; Read-only
    MRRC15 rcp15_cntp_cval,  2, c14
    MRRC15 rcp15_cntv_cval,  3, c14
    MRRC15 rcp15_cntvoff,    4, c14
; Virtualized QTimer is not implemented.
;    MRC15  rcp15_cnthctl,    4, c14, c1, 0
;    MRC15  rcp15_cnthp_tval, 4, c14, c2, 0
;    MRC15  rcp15_cnthp_ctl,  4, c14, c2, 1
;    MRRC15 rcp15_cnthp_cval, 6, c14
    MRC15 rcp15_isr,         0, c12, c1, 0

    MCR15  wcp15_cntfrq,     0, c14, c0, 0
    MCR15  wcp15_cntkctl,    0, c14, c1, 0
    MCR15  wcp15_cntp_tval,  0, c14, c2, 0
    MCR15  wcp15_cntp_ctl,   0, c14, c2, 1
    MCR15  wcp15_cntv_tval,  0, c14, c3, 0
    MCR15  wcp15_cntv_ctl,   0, c14, c3, 1
    MCRR15 wcp15_cntp_cval,  2, c14
    MCRR15 wcp15_cntv_cval,  3, c14
    MCRR15 wcp15_cntvoff,    4, c14
; Virtualized QTimer is not implemented.
;    MCR15  wcp15_cnthctl,    4, c14, c1, 0
;    MCR15  wcp15_cnthp_tval, 4, c14, c2, 0
;    MCR15  wcp15_cnthp_ctl,  4, c14, c2, 1
;    MCRR15 wcp15_cnthp_cval, 6, c14

    END
