    EXPORT  arm_switch_to
    EXPORT  arm_context_switch
    EXPORT  arm_context_switch_to_usr
    EXPORT  arm_get_cpsr
    AREA    contex_switch_asm, CODE, ALIGN=2
    CODE32
arm_switch_to
    mov     sp, r0
    mov     r0, #0x0
    ldmdb   sp, {r0 - r14}^
    rfeia   sp
arm_get_cpsr
    mrs r0, cpsr
    bx r14
arm_context_switch_to_usr
    sub  r3, sp, #(11*4)   
    mov  r12, r14
    stmia r3, { r4-r11, r12, r13, r14 }^
    str  r3, [r0] 
    mov     sp, r1
    mov     r0, #0x0
    ldmdb   sp, {r0 - r14}^
    rfeia   sp
arm_context_switch
    sub  r3, sp, #(11*4)   
    mov  r12, lr
    stmia r3, { r4-r11, r12, r13, r14 }^
    str  r3, [r0] 
    ldmia r1, { r4-r11, r12, r13, r14 }^
    mov  lr, r12     
    add  sp, r1, #(11*4)      
    bx  lr
    END