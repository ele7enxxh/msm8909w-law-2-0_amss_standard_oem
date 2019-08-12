;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;            T R U S T Z O N E      K R A I T      M A C R O S
;
;
; GENERAL DESCRIPTION
;   Assembly macros and definitions for Krait processor.
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
; $Header:  $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/13/12   tk      Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    MACRO
    Check_E_L2ESR $rx
      mov     $rx, #0x204
      mcr     p15, 3, $rx, c15, c0, 6
      misb
      mrc     p15, 3, $rx, c15, c0, 7
      ands    $rx, #0xFF
      beq     %f1
      mcr     p15, 3, $rx, c15, c0, 7
1
    MEND

    MACRO
    Check_L2ESR $rx
      mov     $rx, #0x204
      mcr     p15, 3, $rx, c15, c0, 6
      misb
      mrc     p15, 3, $rx, c15, c0, 7
      ands    $rx, #0xFF
    MEND

    END
