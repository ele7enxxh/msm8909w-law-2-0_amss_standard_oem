#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/src/dal_asm_rvct.s"
;============================================================================
;===  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved.
;============================================================================
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/dalconfig.h"

 





#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"






 









#line 24 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"
























 
 
 























 

 









#line 97 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"



#line 110 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"

#line 9 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/dalconfig.h"





 

 


 






#line 5 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/src/dal_asm_rvct.s"
   AREA    DAL_ASM, CODE, READONLY
   
;  CODE32  Same code compiles for nor flash tool image which run on cortex-M3.
;          (RPM). Cortex-M3 is Thumb2 based instruction set. Setting it 
;          explicitly to ARM(CODE32) leads to compilation when build for nor 
;          flash image. Let build enviorment choose the right cpu when compiling
;          the code(Cortex-M3 or Sparrorw A5)

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
