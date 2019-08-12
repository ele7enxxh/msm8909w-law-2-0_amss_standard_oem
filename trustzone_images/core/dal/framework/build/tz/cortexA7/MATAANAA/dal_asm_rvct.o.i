;============================================================================
;===  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.
;============================================================================
   AREA    DAL_ASM, CODE, READONLY
;============================================================================
   EXPORT    DALFW_LockedGetW
;==  pTarget= r0
DALFW_LockedGetW
   ldrex  r1,[r0]              ;tag and fetch value
   mov    r0,r1
   bx     r14
   EXPORT    DALFW_LockedConditionalSetW
;== pTarget=r0, value = r1
DALFW_LockedConditionalSetW
   strex  r2, r1, [r0]
   mov    r0, r2           ; return outcome 0 = success
   bx     r14
        END
