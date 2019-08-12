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
; Copyright (c) 2014 by QUALCOMM, Incorporated. All Rights Reserved.
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
; 02/18/14   sg      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

; -----------------------------------------------------------------------------
; MACRO: GetCurrentCpuNum
; -----------------------------------------------------------------------------
  MACRO
  GetCurrentCpuNum
    ldr     r1, =tzbsp_cpu_count_per_cluster
    ldr     r1, [r1]
    GetCurrentClusterNum
    mul     r0, r0, r1
    mrc     p15, 0, r1, c0, c0, 5 ; Determine the current CPU,
    and     r1, r1, #0xFF         ; LSB is the AFFL0, which is the CPU number.
    add     r0, r1, r0
  MEND

; -----------------------------------------------------------------------------
; MACRO: GetCurrentClusterNum
; -----------------------------------------------------------------------------
  MACRO
  GetCurrentClusterNum
    mrc     p15, 0, r0, c0, c0, 5
    and     r0, r0, #0xFF00         ; AFFL1 is the cluster number.
    asr     r0, r0, #8
  MEND

; -----------------------------------------------------------------------------
; MACRO: GetCurrentCpuNumInCluster
; -----------------------------------------------------------------------------
  MACRO
  GetCurrentCpuNumInCluster
    mrc     p15, 0, r0, c0, c0, 5 ; Determine the current CPU,
    and     r0, r0, #0xFF         ; LSB is the AFFL0, which is the CPU number.
  MEND

  END
