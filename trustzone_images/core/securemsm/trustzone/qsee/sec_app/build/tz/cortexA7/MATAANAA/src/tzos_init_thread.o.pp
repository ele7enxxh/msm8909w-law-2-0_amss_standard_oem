#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/sec_app/src/tzos_init_thread.s"
    EXPORT  qsee_app_init
    EXPORT  qsee_app_init_return
    AREA    qsee_init_thread_asm, CODE, align=8
    CODE32

qsee_app_init
    push    {r4-r12, r14}
    mov     r9, r3     
    ldr     r3, [r13, #0x28]   
    str     r13, [r3]          
    ldr     r3, =qsee_app_user_sp
    str     r2, [r3, #0x8]
    mrs     r2, cpsr
    and     r2, #0xC0
    orr     r2, #0x10     
    str     r2, [r3, #0xc]
    add     r3, r3, #0x8
    ldmdb   r3, {r13 - r14}^
    rfeia   r3

qsee_app_init_return
    mov     r13, r0
    pop     {r4-r12, pc}

    AREA |QSEE_APP_Init_Stack_Area|, DATA, READWRITE, ALIGN=8

qsee_app_user_sp      SPACE 0x4
qsee_app_user_lr      SPACE 0x4
qsee_app_user_pc      SPACE 0x4
qsee_app_user_cpsr    SPACE 0x4

    END


