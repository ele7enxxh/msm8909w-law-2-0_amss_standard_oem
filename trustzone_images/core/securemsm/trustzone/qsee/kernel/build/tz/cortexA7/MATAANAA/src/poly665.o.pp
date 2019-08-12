#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/kernel/src/poly665.s"
















 

    EXPORT poly665_hash
    EXPORT poly665_hash_copy

    PRESERVE8
    CODE32
    AREA    TZBSP_ASM, CODE, READONLY, align=2

 

poly665_hash FUNCTION

    push {r4-r11}

 
    bic r3, r3, #0x7

 
    ldmia r1, {r4-r5}
 
    eor r6, r6, r6
    eor r7, r7, r7
    eor r8, r8, r8


 
1   ldmia r2!, {r10-r11}
 
    adds r6, r6, r10
    adcs r7, r7, r11
    adcs r8, r8, #1

 
    eor r1, r1, r1
    umull r9, r10, r4, r6
    umull r11, r12, r4, r7
    umaal r10, r11, r5, r6
    umaal r11, r12, r4, r8
    umaal r1, r11, r5, r7
    umaal r11, r12, r5, r8

 
    and r8, r1, #0x3
    bic r1, r1, #0x3

 
    adds r6, r9, r1
    adcs r7, r10, r11
    adcs r8, r8, r12

 
    adds r6, r6, r1, lsr #2
    adcs r7, r7, r11, lsr #2
    adcs r8, r8, r12, lsr #2
    adds r6, r6, r11, lsl #30
    adcs r7, r7, r12, lsl #30
    adcs r8, r8, #0
    bic  r1, r8, #0x3
    and  r8, r8, #0x3
    add  r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0

    subs r3, r3, #8
    bne %b1

 
 
    bic r1, r8, #0x3
    and r8, r8, #0x3
    add r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0
 
    bic r1, r8, #0x3
    and r8, r8, #0x3
    add r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0

 
    mvn r2, #0x4
    mvn r3, #0x0

 
    subs r2, r6, r2
    sbcs r3, r7, r3
    sbcs r4, r8, #0x3
    asr r4, r4, #31

 
    and r6, r6, r4
    and r7, r7, r4
    bic r2, r2, r4
    bic r3, r3, r4
    orr r6, r6, r2
    orr r7, r7, r3

    stmia r0, {r6-r7}
    pop {r4-r11}
    bx lr
        
    ENDFUNC


        
 

poly665_hash_copy FUNCTION

    push {r4-r11}

    push {r0}
    ldr r0, [sp, #36]
 

 
    bic r3, r3, #0x7

 
    ldmia r1, {r4-r5}
 
    eor r6, r6, r6
    eor r7, r7, r7
    eor r8, r8, r8


 
1   ldmia r2!, {r10-r11}
    stmia r0!, {r10-r11}

 
    adds r6, r6, r10
    adcs r7, r7, r11
    adcs r8, r8, #1

 
    eor r1, r1, r1
    umull r9, r10, r4, r6
    umull r11, r12, r4, r7
    umaal r10, r11, r5, r6
    umaal r11, r12, r4, r8
    umaal r1, r11, r5, r7
    umaal r11, r12, r5, r8

 
    and r8, r1, #0x3
    bic r1, r1, #0x3

 
    adds r6, r9, r1
    adcs r7, r10, r11
    adcs r8, r8, r12

 
    adds r6, r6, r1, lsr #2
    adcs r7, r7, r11, lsr #2
    adcs r8, r8, r12, lsr #2
    adds r6, r6, r11, lsl #30
    adcs r7, r7, r12, lsl #30
    adcs r8, r8, #0
    bic  r1, r8, #0x3
    and  r8, r8, #0x3
    add  r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0

    subs r3, r3, #8
    bne %b1

 
 
    bic r1, r8, #0x3
    and r8, r8, #0x3
    add r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0
 
    bic r1, r8, #0x3
    and r8, r8, #0x3
    add r1, r1, r1, lsr #2
    adds r6, r6, r1
    adcs r7, r7, #0
    adcs r8, r8, #0

 
    mvn r2, #0x4
    mvn r3, #0x0

 
    subs r2, r6, r2
    sbcs r3, r7, r3
    sbcs r4, r8, #0x3
    asr r4, r4, #31

 
    and r6, r6, r4
    and r7, r7, r4
    bic r2, r2, r4
    bic r3, r3, r4
    orr r6, r6, r2
    orr r7, r7, r3

    pop {r0}
 
    stmia r0, {r6-r7}
    pop {r4-r11}
    bx lr
        
    ENDFUNC

    END
