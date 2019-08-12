;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      A S S E M B L Y
;
;
; GENERAL DESCRIPTION
;   Assembly macros and definitions
;
; EXTERNALIZED SYMBOLS
;   None.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2010 by QUALCOMM, Incorporated. All Rights Reserved.
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

; Options for DMB/DSB/ISB Barriers, define these manually for RVCT < 3.0
   IF {ARCHITECTURE} != "7-A" :LAND: {ARCHITECTURE} != "7-M"
SY                      EQU    0xF
UN                      EQU    0x7
ST                      EQU    0xE
UNST                    EQU    0x6
   ENDIF

;============================================================================
; MACRO mdmb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data memory barrier, either using the ARMv7 instruction or the
;   legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   mdmb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M"
      dmb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF050 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F50 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 5  ; Legacy Data Memory Barrier
   ENDIF
   MEND

;============================================================================
; MACRO mdsb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data synchronization barrier, either using the ARMv7 instruction
;   or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   mdsb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M"
      dsb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF040 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F40 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 4  ; Legacy Data Write Barrier
   ENDIF
   MEND

;============================================================================
; MACRO misb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs an instruction synchronization barrier, either using the ARMv7
;   instruction or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   misb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M"
      isb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF060 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F60 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c5, 4   ; Legacy Pre-Fetch Flush
   ENDIF
   MEND

   MACRO
   ISB
        misb
   MEND

   MACRO
   DSB
        mdsb
   MEND

   END
