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
    bx lr
atomic_and
    bx lr
atomic_or
    bx lr
arch_idle
    bx lr
    END
