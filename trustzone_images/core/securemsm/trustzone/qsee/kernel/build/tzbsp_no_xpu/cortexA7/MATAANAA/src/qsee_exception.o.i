    PRESERVE8
    EXPORT  qsee_swi_handler
    EXPORT  qsee_undef_handler
    EXPORT  qsee_dabort_handler
    EXPORT  qsee_pabort_handler
    IMPORT  qsee_data_fault_status
    IMPORT  qsee_data_fault_addr
    IMPORT  qsee_instr_fault_status
    IMPORT  qsee_instr_fault_addr
    IMPORT  qsee_handle_page_fault
    EXPORT  qsee_get_request_info
    EXPORT  qsee_save_thread_regs
    IMPORT  qsee_handle_app_request
    IMPORT  entry_cpsr
    EXPORT  qsee_apps_svc_stack_end
    EXPORT  qsee_apps_svc_stack_guard
    AREA    qsee_exception_asm, CODE, ALIGN=8
    CODE32
qsee_swi_handler
    mrs     r2, spsr
    ldr     r1, =exception_cpsr
    str     r2, [r1]         
    sub     r1, r1, #4       
    str     r14, [r1]         
    stmdb   r1, {r4 - r14}^   
    ldr     r1, =qsee_apps_svc_stack_start
    sub     r1, r1, #8
    mov     r13, r1
    mrs     r2, cpsr         
    ldr     r1, =entry_cpsr   
    ldr     r1, [r1]
    and     r1, #0xC0        
    and     r2, #(~0xC0)     
    orr     r2, r1
    msr     cpsr_c, r2         
    ldr     r1, =qsee_handle_app_request
    blx     r1
qsee_get_request_info
    push    {r4-r8, r14}
    ldr     r2, =exception_regs
    add     r2, r2, #0x10   
copy_loop
    ldmia     r2!, {r3}
    stmia     r0!, {r3}
    sub     r1, r1, #1
    cmp     r1, #0
    bne     copy_loop
    pop     {r4-r8, pc}
qsee_save_thread_regs
    ldr     r1, =exception_regs
    mov     r2, #17    
save_loop
    ldmia   r1!, {r3}
    stmia   r0!, {r3}
    sub     r2, r2, #1
    cmp     r2, #0
    bne     save_loop
    bx      r14
qsee_undef_handler
    str     r14, [r13]     
    b       qsee_exception_handler
qsee_dabort_handler
    str     r14, [r13]    
    mov     r14, #0x0    
    str     r14, [r13, #0x4]
    b       qsee_exception_handler
qsee_pabort_handler
    str     r14, [r13]    
    mov     r14, #0x1    
    str     r14, [r13, #0x4]
    b       qsee_exception_handler
qsee_exception_handler
    mrs     r14, spsr
    ands    r14, r14, #0xF             
    subnes  r14, r14, #0xF             
    bne     handle_non_usr_mode_fault
    ldr     r14, =exception_cpsr
    sub     r14, r14, #4       
    stmdb   r14, {r0 - r14}^
    ldr     r0, [r13]
    str     r0, [r14]          
    mrs     r0, spsr
    str     r0, [r14, #4]      
    ldr r1, =qsee_data_fault_status
    mrc p15, 0, r0, c5, c0, 0    
    str r0, [r1]
    ldr r1, =qsee_data_fault_addr
    mrc p15, 0, r0, c6, c0, 0    
    str r0, [r1]
    ldr r1, =qsee_instr_fault_status
    mrc p15, 0, r0, c5, c0, 1    
    str r0, [r1]
    ldr r1, =qsee_instr_fault_addr
    mrc p15, 0, r0, c6, c0, 2    
    str r0, [r1]
    msr     CPSR_c, #0xD3
    ldr     r1, =qsee_apps_svc_stack_start
    sub     r1, r1, #8
    mov     r13, r1
    ldr     r1, =qsee_handle_app_request
    mov     r0, #0
    blx     r1
handle_non_usr_mode_fault
    mrs     r14, spsr
    and     r14, r14, #0x1F             
    cmp     r14, #0x13
    bne     exception_loop_here   
    mrs     r14, cpsr
    and     r14, r14, #0x1F             
    cmp     r14, #0x17               
    bne     exception_loop_here   
    ldr     r14, =exception_regs
    stm     r14, {r0 - r3,r12}
    ldr     r0, [r13]     
    ldr     r1, [r13, #0x4]     
    cmp     r1, #0x0      
    bne     pref_abt_mode
    sub     r0, r0, #0x4   
pref_abt_mode
    sub     r0, r0, #0x4   
    mrs     r1, spsr
    ldr     r14, =exception_cpsr
    str     r1, [r14]
    sub     r14, r14, #4       
    str     r0, [r14]          
    ldr     r1, [r13, #0x4]     
    cmp     r1, #0x0        
    bne     invoke_pager    
    mrc p15, 0, r0, c6, c0, 0    
invoke_pager
    ldr     r2, =qsee_handle_page_fault
    blx     r2
    cmp     r0, #0x0
    bne     pager_error    
    ldr     r14, =exception_regs
    ldm     r14, {r0 - r3,r12}
    ldr     r14, =exception_cpsr
    sub     r14, r14, #4       
    rfe     r14
pager_error
    b      pager_error
exception_loop_here
    b       exception_loop_here
    AREA |QSEE_SWI_Area|, DATA, READWRITE
exception_regs    SPACE 0x40
exception_cpsr    SPACE 0x4
qsee_apps_svc_stack_guard DCD   0xdeadbeef
qsee_apps_svc_stack_end   SPACE 0x1000
qsee_apps_svc_stack_start DCD   0xdeadbeef
    END
