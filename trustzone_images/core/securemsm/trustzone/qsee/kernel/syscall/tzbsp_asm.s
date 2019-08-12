
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   A S M
;
; GENERAL DESCRIPTION
;   Assembly functions
;
; Copyright (c) 2011 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/syscall/tzbsp_asm.s#1 $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/28/11   tk      Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "tzbsp_target.h"

    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT tzbsp_syscall
    IMPORT Workspace_N_Begin
    IMPORT |Image$$TZBSP_STACK$$ZI$$Limit|
    IMPORT |Image$$TZBSP_STACK$$ZI$$Base|
    IMPORT tzbsp_dcache_flush_all

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    EXPORT tzbsp_loop_here
    EXPORT tzbsp_switch_execute_smc

;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    PRESERVE8
    CODE32
    AREA    TZBSP_ASM, CODE, READONLY, align=2

;======================================================================
; Called by tzbsp_error_hanlder only. We perform
; Clean up the registers and loop here until JTAG is connected.
;======================================================================
tzbsp_loop_here FUNCTION
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
1   b   %b1
    ENDFUNC

;======================================================================
; The main message loop. Each incoming syscall will be handled by this
; loop.
;======================================================================
tzbsp_switch_execute_smc FUNCTION
    SMI  0x0                    ; After init just go to non-secure side.
1   ldr  r0,=Workspace_N_Begin
    blx  tzbsp_syscall          ; Every subsequent call is to msg processing.
    SMI  0x0                    ; Msg handled, return to non-secure side.
    b    %b1                    ; Back to secure side, loop to msg processing.
    ENDFUNC

; This area is used currently by DAL for heap.  It is defined here
; rather than the DAL layer so we can use the base address in our SCL
; file and MMU pagetable configurations
    AREA    TZBSP_HEAP_AREA, DATA, READWRITE, align=12
tzbsp_heap    % TZBSP_HEAP_SIZE

    END
