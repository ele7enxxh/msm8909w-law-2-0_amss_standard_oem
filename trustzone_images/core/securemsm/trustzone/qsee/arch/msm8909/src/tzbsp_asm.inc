;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      A S S E M B L Y
;
;
; GENERAL DESCRIPTION
;   Assembly macros and definitions common to TZ monitor and TZBSP.
;
; EXTERNALIZED SYMBOLS
;   None.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2010-2012 by QUALCOMM, Incorporated. All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header:  $
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 05/19/10   tk      Added DMB, DSB and ISB macros.
; 05/14/10   tk      Added DMB to WaitForAccess to sync between cores.
; 04/09/10   tk      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

; -----------------------------------------------------------------------------
; MACRO: WaitForAccess
; -----------------------------------------------------------------------------
; Multicore safe spinlock implementation. Waits until the monitor lock
; is cleared and the lock is successfully claimed.
  MACRO
  WaitForAccess $work, $addr, $cpu, $lock
    ldr     $addr, =$lock        ; Load lock state address.
    mrc     p15, 0, $cpu, c0, c0, 5 ; Current MPIDR.
    and     $cpu, $cpu, #0xFF       ; AFFL0 is the CPU number.
    add     $cpu,  #0x1          ; (cpu num + 1) = TZ is locked
42  ldrex   $work, [$addr]       ; Load the current lock status.
    cmp     $work, #0            ; Is TZ locked?
    strexeq $work, $cpu, [$addr] ; Not locked: try to claim the lock.
    cmpeq   $work, #0            ; Not locked: Did we get the lock?
    bne     %b42                 ; Locked or claiming lock failed, retry.
    mdmb                         ; Ensure other observers see lock claim.
  MEND

; -----------------------------------------------------------------------------
; MACRO: ReleaseAccess
; -----------------------------------------------------------------------------
; Releases a lock acquired by WaitForAccess
  MACRO
  ReleaseAccess $zero, $addr, $lock
    ; Ensure all reads/writes are visible to other observers before
    ; releasing the lock.
    mdmb
    ldr     $addr, =$lock        ; Load lock state address.
    mov     $zero, #0            ; Zero equals unlocked.
    str     $zero, [$addr]       ; Clear the lock
    mdmb
  MEND

; -----------------------------------------------------------------------------
; MACRO: CheckIORange
; -----------------------------------------------------------------------------
; $reg_addr      - register containing address to check against range
; $reg_tmp       - temp register to use
; $range_low     - constant value of low address of range to check
; $range_high    - constant value of high address of range to check
; $success_label - Text name of label to execute if addr is within range
  MACRO
  CheckIORange $reg_addr, $reg_tmp, $range_low, $range_high, $success_label
    movw    $reg_tmp, #($range_low & 0xFFFF)         ; load range_low
    movt    $reg_tmp, #(($range_low >> 16) & 0xFFFF) ; into reg_tmp
    cmp     $reg_addr, $reg_tmp     ; Check address against range_low
    blo     %f20                    ; addr < range_low, illegal range

    movw    $reg_tmp, #($range_high & 0xFFFF)         ; load range_high
    movt    $reg_tmp, #(($range_high >> 16) & 0xFFFF) ; into reg_tmp
    cmp     $reg_addr, $reg_tmp     ; Check if address <= range_high
    bls     $success_label          ; addr <= range_high, success

20
  MEND

; -----------------------------------------------------------------------------
; MACRO: CheckIOAddress
; -----------------------------------------------------------------------------
; $reg_addr      - register containing address to check against range
; $reg_tmp       - temp register to use
; $address       - constant value of low address of range to check
; $success_label - Text name of label to execute if addr is within range
  MACRO
  CheckIOAddress $reg_addr, $reg_tmp, $address, $success_label
    movw    $reg_tmp, #($address & 0xFFFF)          ; load address into
    movt    $reg_tmp, #(($address >> 16) & 0xFFFF)  ; reg_addr
    cmp     $reg_addr, $reg_tmp     ; check address against constant addr
    beq     $success_label          ; if equal, success

  MEND

; -----------------------------------------------------------------------------
; MACRO: SetCpuStateMilestoneVar
; -----------------------------------------------------------------------------
; Setting Milestone variable for PC & Warm Boot.
; $work0      - Work register, will be corrupted
; $work1      - Work register, will be corrupted
; $value      - Milestone value per cpu basis
; $address    - Variable that will hold value.
  MACRO
  SetCpuStateMilestoneVar $work0, $work1, $value, $addr
  ; NOT IMPLEMENTED.

 MEND

; Mode values
TZBSP_Mode_SVC                EQU    0x13
TZBSP_Mode_ABT                EQU    0x17
TZBSP_Mode_UND                EQU    0x1b
TZBSP_Mode_USR                EQU    0x10
TZBSP_Mode_FIQ                EQU    0x11
TZBSP_Mode_IRQ                EQU    0x12

; CPSE I/F mask bits
TZBSP_I_Bit                   EQU    0x80
TZBSP_F_Bit                   EQU    0x40

  END
