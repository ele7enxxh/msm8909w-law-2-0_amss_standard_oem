#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/lk/arch/src/ops.s"





















 
    EXPORT  arch_enable_ints
    EXPORT  arch_disable_ints
    EXPORT  atomic_swap
    EXPORT  atomic_add
    EXPORT  atomic_and
    EXPORT  atomic_or
    EXPORT  arch_idle
    AREA    ops_asm, CODE, ALIGN=2
    CODE32


 
arch_enable_ints





    bx lr

 
arch_disable_ints





    bx lr

 
atomic_swap
    

 
    bx lr

 
atomic_add
#line 94 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/lk/arch/src/ops.s"
    bx lr


 
atomic_and
#line 132 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/lk/arch/src/ops.s"
    bx lr


 
atomic_or
#line 170 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/lk/arch/src/ops.s"
    bx lr


 
arch_idle
#line 189 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/lk/arch/src/ops.s"
    bx lr

    END
