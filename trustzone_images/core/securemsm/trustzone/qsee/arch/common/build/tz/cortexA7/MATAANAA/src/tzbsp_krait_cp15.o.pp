#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/common/src/tzbsp_krait_cp15.s"
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;               T R U S T Z O N E   K R A I T    C P 1 5
;
;
; GENERAL DESCRIPTION
;   CP15 read/write access functions.
;
;   Assumptions:
;
; Copyright (c) 2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
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
; 02/29/12   tk      Initial version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    GET qctps_common_macros.inc

    EXPORT wcp15_dcrsw
    EXPORT wcp15_icrsw

    EXPORT rcp15_dcrtr0
    EXPORT rcp15_dcrdr0

    EXPORT rcp15_icrtr0
    EXPORT rcp15_icrdr0
    EXPORT rcp15_icrdr1

    EXPORT rcp15_l0dcrtr0
    EXPORT rcp15_l0dcrdr0

    EXPORT rcp15_l0icrtr0
    EXPORT rcp15_l0icrdr0
    EXPORT rcp15_l0icrdr1

    EXPORT read_l2_data

    EXPORT rcp15_pvr2f0
    EXPORT wcp15_pvr2f0

    EXPORT rcp15_vbar
    EXPORT wcp15_vbar
	
	EXPORT rcp15_sec_sctlr
	EXPORT rcp15_nsec_sctlr
	EXPORT rcp15_actlr
	EXPORT rcp15_scr
 EXPORT save_context

    CODE32
    PRESERVE8

    ; Just lump these to the libarm area
    AREA TZBSP_KRAIT_CP15, align=4, CODE, READONLY

; void wcp15_dcrsw(uint32 val)
wcp15_dcrsw FUNCTION
    DCRSW   r0
    bx      lr
    ENDFUNC

; void wcp15_icrsw(uint32 val)
wcp15_icrsw FUNCTION
    ICRSW   r0
    bx      lr
    ENDFUNC

; uint32 rcp15_dcrtr0(void)
rcp15_dcrtr0 FUNCTION
    RCP15_DCRTR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_dcrdr0(void)
rcp15_dcrdr0 FUNCTION
    RCP15_DCRDR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_icrtr0(void)
rcp15_icrtr0 FUNCTION
    RCP15_ICRTR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_icrdr0(void)
rcp15_icrdr0 FUNCTION
    RCP15_ICRDR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_icrdr1(void)
rcp15_icrdr1 FUNCTION
    RCP15_ICRDR1 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_l0dcrtr0(void)
rcp15_l0dcrtr0 FUNCTION
    RCP15_L0DCRTR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_l0dcrdr0(void)
rcp15_l0dcrdr0 FUNCTION
    RCP15_L0DCRDR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_l0icrtr0(void)
rcp15_l0icrtr0 FUNCTION
    RCP15_L0ICRTR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_l0icrdr0(void)
rcp15_l0icrdr0 FUNCTION
    RCP15_L0ICRDR0 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_l0icrdr1(void)
rcp15_l0icrdr1 FUNCTION
    RCP15_L0ICRDR1 r0
    bx      lr
    ENDFUNC

; uint32 rcp15_vbar(void)
rcp15_vbar FUNCTION
    RCP15_VBAR     r0
    bx      lr
    ENDFUNC

; void wcp15_vbar(uint32 val)
wcp15_vbar FUNCTION
    WCP15_VBAR     r0
    dsb
    isb
    bx      lr
    ENDFUNC

; uint32 rcp15_sec_sctlr(void)
rcp15_sec_sctlr FUNCTION
    mrc     p15, 0, r0, c1, c0, 0
    bx      lr
    ENDFUNC

; uint32 rcp15_nsec_sctlr(void)
rcp15_nsec_sctlr FUNCTION
    mrc     p15, 0, r1, c1, c1, 0 ; Read the current SCR
    orr     r2, r1, #0x1          ; Set SCR.NS=1
    mcr     p15, 0, r2, c1, c1, 0 ; write SCR
    isb
    mrc     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r1, c1, c1, 0 ; write SCR
    isb
    bx      lr
    ENDFUNC

; uint32 rcp15_actlr(void)
rcp15_actlr FUNCTION
    mrc     p15, 0, r0, c1, c0, 1
    bx      lr
    ENDFUNC

; uint32 rcp15_scr(void)
rcp15_scr FUNCTION
    mrc     p15, 0, r0, c1, c1, 0
    bx      lr
    ENDFUNC

; void save_context(uint32 *buf)
save_context FUNCTION
    add     r0, #4
    stmia   r0!, {r1-r14}
    bx      lr
    ENDFUNC

;============================================================================
; uint32 read_l2_data(uint32 offset)
;  INPUT       - register r0:  L2 reg Offset to be written to L2CPUCPSELR
;  OUTPUT      - register r0:  data from L2 register whose offset is given
;                              as input
;  DESCRIPTION -
;     This api gets the data from L2 register whose offset is written
;     into L2CPUCPSELR.
;
;============================================================================
read_l2_data FUNCTION
    ; Reads from L2 cache registers are indirect by writing the L2 CP
    ; register pointer in L2CPUCPSELR and accessing the value in
    ; L2CPUCPDR.
    WCP15_L2CPUCPSELR r0
    isb
    RCP15_L2CPUCPDR   r0
    isb
    dsb
    bx                lr
    ENDFUNC

; uint32 rcp15_pvr2f0(void)
rcp15_pvr2f0 FUNCTION
    RCP15_PVR2F0    r0
    bx      lr
    ENDFUNC

; void wcp15_pvr2f0(uint32 val)
wcp15_pvr2f0 FUNCTION
    WCP15_PVR2F0    r0
    bx      lr
    ENDFUNC

    END
