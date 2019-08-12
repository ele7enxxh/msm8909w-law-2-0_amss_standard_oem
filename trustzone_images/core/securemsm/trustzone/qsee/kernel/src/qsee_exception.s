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

#define QSEE_SVC_STACK_SIZE   0x1000

qsee_swi_handler
    /* r0 contains the syscall number. Do not modify.
     * rest of the parameters are in r4-r11
     */
    mrs     r2, spsr
    ldr     r1, =exception_cpsr
    str     r2, [r1]        /*Save exception cpsr*/
    sub     r1, r1, #4      /*Make r1 point to use pc*/
    str     r14, [r1]        /*Save exception pc*/
    stmdb   r1, {r4 - r14}^  /*Do not save r0-r3*/
    ldr     r1, =qsee_apps_svc_stack_start
    sub     r1, r1, #8
    mov     r13, r1
    mrs     r2, cpsr        /* Get current cpsr and adjust I & F */
    ldr     r1, =entry_cpsr  /* based on entry cpsr*/
    ldr     r1, [r1]
    and     r1, #0xC0       /* Mask all bits except I, F*/
    and     r2, #(~0xC0)    /* Mask I, F*/
    orr     r2, r1
    msr     cpsr_c, r2        /*Update CPSR*/
    ldr     r1, =qsee_handle_app_request
    blx     r1

qsee_get_request_info
    push    {r4-r8, r14}
    ldr     r2, =exception_regs
    add     r2, r2, #0x10  /*Make it point to r4 of saved exc regs */
copy_loop
    ldmia     r2!, {r3}
    stmia     r0!, {r3}
    sub     r1, r1, #1
    cmp     r1, #0
    bne     copy_loop
    /*stmia   lr, {r0 - r3}^*/
    pop     {r4-r8, pc}

/*r0 points to thread->arch-regs*/
qsee_save_thread_regs
    ldr     r1, =exception_regs
    mov     r2, #17   /*Need to save 17 regs*/
save_loop
    ldmia   r1!, {r3}
    stmia   r0!, {r3}
    sub     r2, r2, #1
    cmp     r2, #0
    bne     save_loop
    bx      r14

qsee_undef_handler
    str     r14, [r13]    /*Save L4 onto stack*/
    b       qsee_exception_handler
qsee_dabort_handler
    str     r14, [r13]   /*Save L4 onto stack*/
    mov     r14, #0x0   /*Indicate data abort*/
    str     r14, [r13, #0x4]
    b       qsee_exception_handler
qsee_pabort_handler
    str     r14, [r13]   /*Save L4 onto stack*/
    mov     r14, #0x1   /*Indicate prefetch abort*/
    str     r14, [r13, #0x4]
    b       qsee_exception_handler

qsee_exception_handler
    mrs     r14, spsr
    /* Check if fault is from user */
    ands    r14, r14, #0xF            /* check for bits 3:0 == 0x0 */
    subnes  r14, r14, #0xF            /* check for bits 3:0 == 0xf */
    bne     handle_non_usr_mode_fault
    ldr     r14, =exception_cpsr
    sub     r14, r14, #4      /*Make r14 point to use pc*/
    stmdb   r14, {r0 - r14}^
    ldr     r0, [r13]
    str     r0, [r14]         /*Save exception faulted pc*/
    mrs     r0, spsr
    str     r0, [r14, #4]     /*Save exception cpsr*/

    ldr r1, =qsee_data_fault_status
    mrc p15, 0, r0, c5, c0, 0   /*DFSR*/
    str r0, [r1]
    ldr r1, =qsee_data_fault_addr
    mrc p15, 0, r0, c6, c0, 0   /*DFAR*/
    str r0, [r1]
    ldr r1, =qsee_instr_fault_status
    mrc p15, 0, r0, c5, c0, 1   /*IFSR*/
    str r0, [r1]
    ldr r1, =qsee_instr_fault_addr
    mrc p15, 0, r0, c6, c0, 2   /*IFAR*/
    str r0, [r1]

    /*enter svc mode */
    msr     CPSR_c, #0xD3
    ldr     r1, =qsee_apps_svc_stack_start
    sub     r1, r1, #8
    mov     r13, r1
    ldr     r1, =qsee_handle_app_request
    mov     r0, #0
    blx     r1

/*Handle non-user mode fault. only treat svc mode faults.*/
handle_non_usr_mode_fault
    mrs     r14, spsr
    and     r14, r14, #0x1F            /* keep Mode bits 4:0 */
    cmp     r14, #0x13
    bne     exception_loop_here  /*Fault not from svc, do not handle*/

    /*Handle faults from SVC mode*/
    mrs     r14, cpsr
    and     r14, r14, #0x1F            /* keep Mode bits 4:0 */
    cmp     r14, #0x17              /*check for ABT mode*/
    bne     exception_loop_here  /*In UND mode, do not handle*/

    /*Proceed in abt mode*/
    ldr     r14, =exception_regs
    stm     r14, {r0 - r3,r12}
    ldr     r0, [r13]    /*Read preferred return address*/
    ldr     r1, [r13, #0x4]    /*read data or prefetch fault mode*/
    cmp     r1, #0x0     /*Check for data abort*/
    bne     pref_abt_mode
    sub     r0, r0, #0x4  /*data abt ret addr is LR - 8*/
pref_abt_mode
    sub     r0, r0, #0x4  /*pref abt ret addr is LR - 4*/

    /*Save R14 and SPSR*/
    mrs     r1, spsr
    ldr     r14, =exception_cpsr
    str     r1, [r14]
    sub     r14, r14, #4      /*Make r14 point to use pc*/
    str     r0, [r14]         /*Save exception faulted pc*/

    ldr     r1, [r13, #0x4]    /*read data or prefetch fault mode*/
    cmp     r1, #0x0       /*Check for data abort*/
    bne     invoke_pager   /*pref abt:r0 already has addr to be paged*/
    mrc p15, 0, r0, c6, c0, 0   /* Read DFAR*/
                           /* data abt: get addr to be paged in r0*/

invoke_pager
    /*r0=fault address, r1=data/prefetch abt info*/
    ldr     r2, =qsee_handle_page_fault
    blx     r2
    cmp     r0, #0x0
    bne     pager_error   /*Could not page in*/
    ldr     r14, =exception_regs
    ldm     r14, {r0 - r3,r12}
    ldr     r14, =exception_cpsr
    sub     r14, r14, #4      /*Make r14 point to use pc*/
    rfe     r14

pager_error
    b      pager_error
exception_loop_here
    b       exception_loop_here

    AREA |QSEE_SWI_Area|, DATA, READWRITE
exception_regs    SPACE 0x40
exception_cpsr    SPACE 0x4

qsee_apps_svc_stack_guard DCD   0xdeadbeef
qsee_apps_svc_stack_end   SPACE QSEE_SVC_STACK_SIZE
qsee_apps_svc_stack_start DCD   0xdeadbeef

    END

