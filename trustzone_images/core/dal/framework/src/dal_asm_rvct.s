;============================================================================
;===  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.
;============================================================================
   #include "dalconfig.h"
   AREA    DAL_ASM, CODE, READONLY

;============================================================================

 #if (DAL_CONFIG_ARCH >= DAL_CONFIG_ARCH_ARM_V6)
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
#endif

#if defined(FEATURE_AMSSHEAP_DEBUG) && !defined(BUILD_BOOT_CHAIN) && !defined(BUILD_TOOL_CHAIN)
  EXPORT    DALSYS_Malloc
  IMPORT    amssheap_malloc
;=== DALResult DALSYS_Malloc(uint32 dwSize, void **ppMem)
DALSYS_Malloc
   stmfd sp!, {r1,lr} ; save **ppMem and lr 
   mov r1, lr
   bl amssheap_malloc  ; void *amssheap_malloc ( size_t , void *call_ptr);
   ldmfd sp!, {r1,lr} ; restore **ppMem and lr
   str r0,[r1]
   mov r0, #0
   bx     r14

  EXPORT    DALSYS_Free
  IMPORT    amssheap_free
;=== DALResult DALSYS_Free(void *pmem)
DALSYS_Free
   stmfd sp!, {r1,lr} ; save r1 and lr
   mov r1, lr
   bl amssheap_free ; void amssheap_free ( void *ptr, void *caller_ptr)
   ldmfd sp!, {r1,lr} ; restore r1 and lr 
   mov r0, #0
   bx     r14
#endif

        END
