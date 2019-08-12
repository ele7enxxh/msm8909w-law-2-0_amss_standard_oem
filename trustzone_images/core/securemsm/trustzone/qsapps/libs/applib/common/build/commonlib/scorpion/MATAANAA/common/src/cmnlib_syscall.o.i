    EXPORT app_export_init_info_to_qsee
    EXPORT qsee_printf_syscall
    EXPORT qsee_generic_syscall
    AREA   tzapp_syscall_asm, CODE, align=8
    CODE32
app_export_init_info_to_qsee
    mov     r8, r4
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2
    mov     r7, r3
    mov     r0, #(0xffffff00 + 0x0)
    swi     (0x1400 + 0x0)
    bx      r14
qsee_printf_syscall
    push {r4-r12, r14}
    mov     r4, r0
    mov     r5, r1
    mov     r0, #(0xffffff00 + 0xF9)
    swi     (0x1400 + 0xF9)
    pop {r4-r12, pc}
qsee_generic_syscall
    push {r4-r12, r14}
    mov     r4, r1
    mov     r5, r2
    mov     r6, r3
    ldr     r7, [sp,#0x28]
    ldr     r8, [sp,#0x2C]
    ldr     r9, [sp,#0x30]
    swi     0x1400
    pop {r4-r12, pc}
    END
