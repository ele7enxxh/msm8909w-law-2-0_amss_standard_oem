;
;
        EXPORT hal_sdcc_memcpy32
        EXPORT hal_sdcc_memcpy_read
        EXPORT hal_sdcc_memcpy_write
        PRESERVE8
        AREA    hal_sdcc_main, CODE, READONLY
        CODE32
; Going for R0 = DataDesc, R1 = SRC
hal_sdcc_memcpy32
        push    {r4-r11}
        ldmia   r1!, {r4-r11}
        stmia   r0!, {r4-r11}
        pop     {r4-r11}
        bx      r14
        ldr     r2, [r1], #4
        str     r2, [r0], #4
; Going for R0 = DataDesc, R1 = SRC, R2 = Status_addr, R3 = Mask
hal_sdcc_memcpy_read
        push    {r4-r11}
read_loop
        ; Actual data moving
        ldmia   r1, {r4-r11}
        stmia   r0!, {r4-r11}
        ldr     r5, [r2]
        ands    r5, r3, r5
        bne     read_loop
        pop     {r4-r11}
        bx      r14
; Going for R0 = DataDesc, R1 = SRC, R3 = Mask Var
; R3 array info: 1 - Status, 2 - MaskBit, 3 - Size, 4 - error
; Feature not working yet - moving 32 bytes at a time for now
hal_sdcc_memcpy_write
        push    {r4-r12, r14}
        ldr     r12, [r2, #8]    ; Use R12 as size keeper
        ldr     r3, [r2, #4]     ; Read mask register value
        ldr     r14, [r2]        ; Get the status register addr
write_loop
        ldmia   r1!, {r4-r11}
        stmia   r0, {r4-r11}
        ; Check the size info
        sub     r12, #0x20
        cmp     r12, #0x20
        blt     write_done
        ; Check error flags
write_wait
        ldr     r5, [r14]        ; Read status register
        ldr     r6, [r2, #0xC]   ; Read error flag
        ands    r6, r5, r6
        bne     write_done
        ; Check half-full
        ands    r5, r5, r3
        beq     write_wait
        b       write_loop
write_done
        str     r12, [r2, #8]    ; Write the size information back
        pop     {r4-r12, r14}
        mov     r0, r1
        bx      r14
        END
