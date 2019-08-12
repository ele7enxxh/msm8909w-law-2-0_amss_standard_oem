    EXPORT  __cmnlib_main
    EXPORT  _cmnlib_main
    EXPORT  CMNLIBENTRY
    EXPORT  set_cmnlib_rw_base
    IMPORT  cmnlib_main
    PRESERVE8
    AREA    CMNLIBENTRYCODE, CODE, READONLY
    CODE32
__cmnlib_main
_cmnlib_main
    ENTRY
CMNLIBENTRY
    push  {r4,lr}
    blx   cmnlib_main
    pop   {r4,pc}           ; Return.
set_cmnlib_rw_base
    push  {r4,lr}
    mov   r9, r0            ; set RW base
    pop   {r4,pc}           ; Return.
    END
