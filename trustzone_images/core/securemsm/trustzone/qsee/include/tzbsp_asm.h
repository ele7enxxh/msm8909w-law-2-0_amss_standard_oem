#ifndef TZBSP_ASM_H
#define TZBSP_ASM_H
/*=============================================================================

                          TZBSP Assembly Macros

GENERAL DESCRIPTION
    This header file defines assembly macros that require the C
    preprocessor

Copyright 2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_asm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/12   pre     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tzbsp_target.h"

#ifdef _ARM_ASM_
; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND

; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r
#ifdef TZBSP_WITH_TBASE
    Mov32   $work_r, TZBSP_SANDBOX_END
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, TZBSP_SANDBOX_RELOCATE_OFFSET

10  ldr     $val_r, [$addr_r, $work_r]
#else
    ldr     $val_r, [$addr_r, $work_r]
#endif
  MEND

; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND

; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r
#ifdef TZBSP_WITH_TBASE
    Mov32   $work_r, TZBSP_SANDBOX_END
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, TZBSP_SANDBOX_RELOCATE_OFFSET

10  str     $val_r, [$addr_r, $work_r]
#else
    str     $val_r, [$addr_r, $work_r]
#endif
  MEND

; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
#endif /* _ARM_ASM_ */
#endif  /* TZBSP_ASM_H */
