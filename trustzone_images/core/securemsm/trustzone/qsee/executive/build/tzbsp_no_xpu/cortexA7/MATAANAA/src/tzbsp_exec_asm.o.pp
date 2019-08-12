#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/executive/src/tzbsp_exec_asm.s"
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E
;
; GENERAL DESCRIPTION
;   This file contains the TZBSP exec func
;
; EXTERNALIZED SYMBOLS
;
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;
; Copyright (c) 2010 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 05/03/10   na      Added exec implementation
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================

   ;GET tzbsp_libarm_asm.inc
   ;GET tzbsp_asm.inc

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
; Export the symbols that defined in this module

    EXPORT tzbsp_reset_exec
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
; Import the external symbols that are referenced in this module.

    IMPORT tzbsp_int_disable_all
    IMPORT tzbsp_exec_get_fgroups_ptr
    IMPORT tzbsp_exec_get_ddr_ptr
    IMPORT tzbsp_exec_get_imem_ptr
;===================================================================
; Workspace
;===================================================================
;

    AREA |TZBSPExecWorkspace|, DATA, READWRITE, ALIGN=2

TZBSP_EXEC_PC       SPACE 0x4
TZBSP_EXEC_CPSR     SPACE 0x4
TZBSP_EXEC_R0       SPACE 0x4
TZBSP_EXEC_R1       SPACE 0x4
TZBSP_EXEC_R2       SPACE 0x4
TZBSP_EXEC_R3       SPACE 0x4
TZBSP_EXEC_R4       SPACE 0x4
TZBSP_EXEC_R5       SPACE 0x4
TZBSP_EXEC_R6       SPACE 0x4
TZBSP_EXEC_R7       SPACE 0x4
TZBSP_EXEC_R8       SPACE 0x4
TZBSP_EXEC_R9       SPACE 0x4
TZBSP_EXEC_R10      SPACE 0x4
TZBSP_EXEC_R11      SPACE 0x4
TZBSP_EXEC_R12      SPACE 0x4
TZBSP_EXEC_R13      SPACE 0x4
TZBSP_EXEC_R14      SPACE 0x4

;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================
    PRESERVE8
    AREA    TZBSP_EXEC, CODE, READONLY, ALIGN=2

;======================================================================
; Reset exec function implementation
;======================================================================

;void tzbsp_reset_exec(uint32 addr)
;r0 will hold addr passed

tzbsp_reset_exec FUNCTION

    cmp     r0, #0                 ; if r0 is 0 that means none of the
    bxeq    lr                     ; TZOS (LK or MobiCore)is present

    ldr     r1, =TZBSP_EXEC_CPSR        ;Save CPSR
    mrs     r2, CPSR
    str     r2, [r1]
    ldr     r1, =TZBSP_EXEC_R4          ;Save r4-r14
    stmia   r1, {r4-r14}

    ldr     r1, =TZBSP_EXEC_R0          ;Save addr passed
    str     r0, [r1]                    ;Set r0, this is the addr passed to it

    ldr     r2, =tzbsp_int_disable_all  ;Function call to disable interrupts
    blx     r2

    ; Initialize Image
    ldr     r0, =tzbsp_exec_get_fgroups_ptr
    blx     r0
    mov     r2, r0                      ;Move fgroups to 3rd arg

    ldr     r0, =tzbsp_exec_get_ddr_ptr
    blx     r0
    mov     r1, r0                      ;Move ddr ptr to 2nd arg

    ldr     r0, =tzbsp_exec_get_imem_ptr
    blx     r0                          ;Move MC address struct to first arg

    ldr     r3, =TZBSP_EXEC_R0
    ldr     r3, [r3]
    blx     r3                          ;Branch to addr passed

    cmp     r0, #0                      ;Check return status, 0 if SUCCESS
    beq     %f100
    mov     r0,#-1

100 ldr     r1, =TZBSP_EXEC_CPSR        ;Restore CPSR
    ldr     r2, [r1]
    msr     CPSR_cxsf, r2
    ldr     r1, =TZBSP_EXEC_R4
    ldmia   r1, {r4-r14}

    bx      lr

    ENDFUNC
 END

