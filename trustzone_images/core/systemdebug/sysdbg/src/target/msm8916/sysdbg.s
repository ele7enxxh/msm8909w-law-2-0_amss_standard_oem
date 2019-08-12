;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;             SYSDBG INITIALIZATION ROUTINE
;
; GENERAL DESCRIPTION
;	This file contains initialization code for system debug. This performs
;	check on reset debug status and calls into tzbsp_monitor for saving ctxt.
;
;
; Copyright 2010-2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 10/26/2013 AJC    Add some more code markers
; 09/11/2013 AJC    Avoid clobbering mon mode r14 on 8x26 / 8x10
; 09/04/2013 AJC    Added change to avoid clobbering r0 and r1 on cores other than core 0
; 05/21/2013 AJC    Added change to verify CPU id before booting up after reset	
; 01/29/2013 AJC	Added change to save information from bootloader
; 09/13/2012 AJC	Fixed missing sys mode registers	
; 02/29/2012 AJC	Initial revision for DBI - 8974
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;
Mode_USR                EQU    0x10
Mode_FIQ                EQU    0x11
Mode_IRQ                EQU    0x12
Mode_SVC                EQU    0x13
Mode_MON				EQU	   0x16
Mode_ABT                EQU    0x17
Mode_UND                EQU    0x1B
Mode_SYS                EQU    0x1F

I_Bit                   EQU    0x80
F_Bit                   EQU    0x40

; Secure Configuration Register Bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW)
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW)

; MASKs
DEBUG_RESET_CHECK_MASK	EQU    0x000E0000
DEBUG_STATUS_CHECK_MASK	EQU    0x00000038

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    ; Import the external symbols that are referenced in this module.

    IMPORT sysdbg_save_ctx
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the external symbols that are referenced in this module.
    EXPORT sysdbg_reset_check
	EXPORT Sysdbg_Workspace_N_Base
	EXPORT Sysdbg_Workspace_N_Begin
	EXPORT LR_BACKUP
	EXPORT SP_BACKUP
   
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    AREA SYSDBG_FUNC, CODE, READONLY
    PRESERVE8
    CODE32

;---------------------------------------------------------------------
;	Debug Reset Check
;======================================================================

sysdbg_reset_check FUNCTION
    mov32   r0, 0x01815008	     ;GCC_RESET_STATUS
    ldr     r0, [r0]		     ;check for debug reset
    and     r0, r0, #DEBUG_STATUS_CHECK_MASK
    cmp     r0, #0
    bxeq    lr 	     ;
    mov32   r0, 0x01814000	     ;GCC_RESET_DEBUG
    ldr     r0, [r0]		     ;
    and     r0, r0, #DEBUG_RESET_CHECK_MASK
    cmp     r0, #0
    bxeq    lr			     ;return if not due to reset
    mov     r0, #0x1
    bx      lr
    ENDFUNC

;=======================================================================
;
;		Data for the module 
;
;======================================================================= 
    AREA DBI_DATA , DATA, READWRITE

; Pointer to the CPU register save datastruct
; IMPORT dbi_cpu_regs

	
	AREA |SysdbgWorkspaceDataArea|, DATA, READWRITE, ALIGN=2

; Using directive SPACE will allocate zeroed bytes.
SysdbgWorkspaceBase
Sysdbg_Workspace_N_Begin

N_USR_R0       SPACE 0x4    
N_USR_R1       SPACE 0x4
N_USR_R2       SPACE 0x4
N_USR_R3       SPACE 0x4
N_USR_R4       SPACE 0x4
N_USR_R5       SPACE 0x4
N_USR_R6       SPACE 0x4
N_USR_R7       SPACE 0x4
N_USR_R8       SPACE 0x4
N_USR_R9       SPACE 0x4
N_USR_R10      SPACE 0x4
N_USR_R11      SPACE 0x4
N_USR_R12      SPACE 0x4

Sysdbg_Workspace_N_Base

N_USR_R13      SPACE 0x4
N_USR_R14      SPACE 0x4
N_IRQ_R13      SPACE 0x4
N_IRQ_R14      SPACE 0x4
N_SVC_R13      SPACE 0x4
N_SVC_R14      SPACE 0x4
N_ABT_R13      SPACE 0x4
N_ABT_R14      SPACE 0x4
N_UND_R13      SPACE 0x4
N_UND_R14      SPACE 0x4
N_FIQ_R8       SPACE 0x4
N_FIQ_R9       SPACE 0x4
N_FIQ_R10      SPACE 0x4
N_FIQ_R11      SPACE 0x4
N_FIQ_R12      SPACE 0x4
N_FIQ_R13      SPACE 0x4
N_FIQ_R14      SPACE 0x4
N_MON_R13      SPACE 0x4
N_MON_R14      SPACE 0x4

Sysdbg_Workspace_N_End

LR_BACKUP      SPACE 0x4
SP_BACKUP      SPACE 0x4

    END


