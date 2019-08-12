;=========================================================================
;
;*//** @file jpege_engine_sw_dct.s
;  This file contains the implementations for JPEG Encode DCT
;  in ARM assembly.
;
;@par EXTERNALIZED FUNCTIONS
;  jpege_engine_sw_fdct_block
;
;@par INITIALIZATION AND SEQUENCING REQUIREMENTS
;  (none)
;
;Copyright (C) 2009 Qualcomm Technologies, Inc.
;All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
;
;*//*=====================================================================

;=========================================================================
;                             Edit History
;
;$Header$
;
;when       who     what, where, why
;--------   ---     ------------------------------------------------------
;04/09/09   zhiminl Fast DCT algorithm based on ARMv6 SIMD.
;
;=========================================================================

;-------------------------------------------------------------------------
;                           Private Constants
;-------------------------------------------------------------------------

DCTSIZE         EQU 8             ; The basic DCT block is 8x8
DCTSIZE2        EQU 64            ; DCTSIZE squared,
                                  ; # of elements in a block

; CCk = sqrt(1 / 8)cos((k*pi) / 16) in Q15, where k = 0,
; CCk = sqrt(2 / 8)cos((k*pi) / 16) in Q15, where k = 1, 2, ..., 7.
CC0             EQU   11585       ; 0x2D41, sqrt(1 / 8) * FIX(1.0000000000)
CC1             EQU   16069       ; 0x3EC5, sqrt(2 / 8) * FIX(0.9807852804)
CC2             EQU   15137       ; 0x3B21, sqrt(2 / 8) * FIX(0.9238795325)
CC3             EQU   13623       ; 0x3537, sqrt(2 / 8) * FIX(0.8314696123)
CC4             EQU   11585       ; 0x2D41, sqrt(2 / 8) * FIX(0.7071067812)
CC5             EQU   9102        ; 0x238E, sqrt(2 / 8) * FIX(0.5555702330)
CC6             EQU   6270        ; 0x187E, sqrt(2 / 8) * FIX(0.3826834324)
CC7             EQU   3196        ; 0x0C7C, sqrt(2 / 8) * FIX(0.1950903220)

; The maximum sum of 8 pixel is 255 * 8 * sqrt(1 / 8) in PASS1, which
; is 10 bits, the DCT kernel is in Q15, so the PASS1 8-point DCT
; results could be 25 bits at most, right shift 12 bits to save as
; signed 16-bit integer in Q3.
;
; In PASS2, the final results are also saved as signed 16-bit integer
; in Q3 by right shift 15 bits.
DCT_PASS1_BITS  EQU   12
DCT_PASS2_BITS  EQU   15

;-------------------------------------------------------------------------
;                           EXPORTS
;-------------------------------------------------------------------------

    EXPORT jpege_engine_sw_fdct_block

;-------------------------------------------------------------------------
;                           Function Definitions
;-------------------------------------------------------------------------

    AREA |.text|, CODE, READONLY
    CODE32

;=========================================================================
;
; FUNCTION            : jpege_engine_sw_fdct_block
;
; DESCRIPTION         : Perform the forward DCT on a block of 64 pixels.
;
; C PROTOTYPE         : void jpege_engine_sw_fdct_block (
;                         const uint8_t *pixelDomainBlock,
;                               int16_t *frequencyDomainBlock)
;
; REGISTER INPUTS     : R0: uint8_t *pixelDomainBlock
;                             pointer of the input 64 pixels
;                       R1: int16_t *frequencyDomainBlock
;                             pointer of the output 64 DCT coefficients
;
; STACK ARGUMENTS     : None
;
; REGISTER OUTPUTS    : None
;
; MEMORY INPUTS       : pixelDomainBlock - 64 pixels
;
; MEMORY OUTPUTS      : frequencyDomainBlock - 64 DCT coefficients in Q3
;
; REGISTERS AFFECTED  : R0-R11
;
; STACK USAGE         : (DCTSIZE2 * 2) bytes for intermediate results
;
; CYCLES              :
;
; NOTES               :
;
;=========================================================================
jpege_engine_sw_fdct_block FUNCTION
;-------------------------------------------------------------------------
; register allocation:
; ------------------------------------------------------------------------
pixelDomainBlock          RN 0
frequencyDomainBlock      RN 1

x0                        RN 2            ; process 8 pixels at a time
x1                        RN 3
x2                        RN 4
x3                        RN 5
x4                        RN 6
x5                        RN 7
x6                        RN 8
x7                        RN 9

x1x0                      RN 2            ; x1 in [31:16], x0 in [15:0]
x3x2                      RN 4            ; x3 in [31:16], x2 in [15:0]
x4x5                      RN 3            ; x4 in [31:16], x5 in [15:0]
x5x4                      RN 3            ; x5 in [31:16], x4 in [15:0],
                                          ; happened in PASS2
x6x7                      RN 5            ; x6 in [31:16], x7 in [15:0]
x7x6                      RN 5            ; x7 in [31:16], x6 in [15:0],
                                          ; happened in PASS2

;-------------------------------------------------------------------------
; step 1: calculate t4, t5, t6, t7
;-------------------------------------------------------------------------
t5t4                      RN 6            ; t5 in [31:16], t4 in [15:0]
t7t6                      RN 7            ; t7 in [31:16], t6 in [15:0]
                                          ; need to exchange as t6t7
t6t7                      RN 7            ; t6 in [31:16], t7 in [15:0]

;-------------------------------------------------------------------------
; step 2: calculate t8, t9, t10, t11
;-------------------------------------------------------------------------
t9t8                      RN 8            ;  t9 in [31:16],  t8 in [15:0]
t11t10                    RN 9            ; t11 in [31:16], t10 in [15:0]

;-------------------------------------------------------------------------
; step 3: calculate t0, t1, t2, t3
; t5t4 (r6), t6t7 (r7) are not needed any more
;-------------------------------------------------------------------------
t1t0                      RN 6            ; t1 in [31:16], t0 in [15:0]
t3t2                      RN 7            ; t3 in [31:16], t2 in [15:0]

;-------------------------------------------------------------------------
; step 4: calculate s0, s4
; x1x0 (r2), x3x2 (r4), x4x5 (r3), x6x7 (r5) are not needed any more
;-------------------------------------------------------------------------
c00c00                    RN 2            ; c00 in [31:16], c00 in [15:0]
y0                        RN 3            ; s0
c41c40                    RN 4            ; c41 in [31:16], c40 in [15:0]
y4                        RN 5            ; s4

;-------------------------------------------------------------------------
; step 5: calculate s2, s6
; t11t10 (r9) is not needed any more
;-------------------------------------------------------------------------
c21c20                    RN 2            ; c21 in [31:16], c20 in [15:0]
y2                        RN 3            ; s2
c61c60                    RN 4            ; c61 in [31:16], c60 in [15:0]
y6                        RN 5            ; s6

;-------------------------------------------------------------------------
; step 6: calculate s1, s3, s5, s7
; t9t8 (r8) is not needed any more
;-------------------------------------------------------------------------
c11c10                    RN 2            ; c11 in [31:16], c10 in [15:0]
c13c12                    RN 3            ; c13 in [31:16], c12 in [15:0]
y1                        RN 4            ; s1
c31c30                    RN 5            ; c31 in [31:16], c30 in [15:0]
c33c32                    RN 8            ; c33 in [31:16], c32 in [15:0]
y3                        RN 9            ; s3

c51c50                    RN 2            ; c51 in [31:16], c50 in [15:0]
c53c52                    RN 3            ; c53 in [31:16], c52 in [15:0]
y5                        RN 4            ; s5
c71c70                    RN 5            ; c71 in [31:16], c70 in [15:0]
c73c72                    RN 8            ; c73 in [31:16], c72 in [15:0]
y7                        RN 9            ; s7

;-------------------------------------------------------------------------
; scratch registers
;-------------------------------------------------------------------------
kernel                    RN 10           ; DCT kernel coefficients
dctDataPtr                RN 11           ; DCT output data location
index                     RN 12           ; DCT loop count
pass1DataBlock            RN  lr          ; DCT PASS1 results are saved
                                          ; in stack

dc                        RN 4            ; DC coefficient

;-------------------------------------------------------------------------
; end register allocation
;-------------------------------------------------------------------------

    STMFD   sp!, {r4-r11, lr}             ; start of jpege_engine_sw_fdct_block()
                                          ; PUSH    {r4-r11, lr}

    ADR     kernel, dct_kernel_table      ; kernel = dct_kernel_table

    ;---------------------------------------------------------------------
    ; Allocate (DCTSIZE2 * 2) bytes in stack as placeholder of PASS1 results
    ;---------------------------------------------------------------------
    SUB     sp, sp, #(DCTSIZE2 * 2)

    ;---------------------------------------------------------------------
    ; PASS1: processing rows, the results are saved in corresponding
    ;        columns, such as row 1 is saved in column 1, ..., and so on.
    ;---------------------------------------------------------------------
    MOV     index, #0                     ; index (r12) = 0
    MOV     dctDataPtr, sp                ; dctDataPtr (r11) = &pass1DataBlock[0][0]
                                          ; stack allocated (DCTSIZE2 * 2) bytes

load_pixels
    ;---------------------------------------------------------------------
    ; PASS1: load the pixel values from input pointer
    ;---------------------------------------------------------------------
    LDRB    x0, [pixelDomainBlock], #1    ; x0 (r2) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x1, [pixelDomainBlock], #1    ; x1 (r3) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x2, [pixelDomainBlock], #1    ; x2 (r4) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x3, [pixelDomainBlock], #1    ; x3 (r5) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x4, [pixelDomainBlock], #1    ; x4 (r6) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x5, [pixelDomainBlock], #1    ; x5 (r7) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x6, [pixelDomainBlock], #1    ; x6 (r8) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)
    LDRB    x7, [pixelDomainBlock], #1    ; x7 (r9) = *pixelDomainBlock
                                          ; pixelDomainBlock++ (r0)

    ORR     x1x0, x0, x1, LSL #16         ; x1x0 (r2) = (x1 (r3) << 16) | x0 (r2)
                                          ; x1 in r2[31:16], x0 in r2[15:0]
    ORR     x3x2, x2, x3, LSL #16         ; x3x2 (r4) = (x3 (r5) << 16) | x2 (r4)
                                          ; x3 in r4[31:16], x2 in r4[15:0]
    ORR     x4x5, x5, x4, LSL #16         ; x4x5 (r3) = x4 (r6) << 16) | x5 (r7)
                                          ; x4 in r3[31:16], x5 in r3[15:0]
    ORR     x6x7, x7, x6, LSL #16         ; x6x7 (r5) = x6 (r8) << 16) | x7 (r9)
                                          ; x6 in r5[31:16], x7 in r5[15:0]

dct_pass1_loop
    ;---------------------------------------------------------------------
    ; PASS1: 1D 8-point DCT processing begins
    ;---------------------------------------------------------------------
    ;---------------------------------------------------------------------
    ; PASS1 step 1: calculate t4, t5, t6, t7
    ;---------------------------------------------------------------------
    SADD16  t7t6, x3x2, x4x5              ; t7t6 (r7) = x3x2 (r4) + x4x5 (r3)
                                          ; t7 in r7[31:16], t6 in r7[15:0]
    SADD16  t5t4, x1x0, x6x7              ; t5t4 (r6) = x1x0 (r2) + x6x7 (r5)
                                          ; t5 in r6[31:16], t4 in r6[15:0]
    MOV     t6t7, t7t6, ROR #16           ; t6t7 (r7)
                                          ; t6 in r7[31:16], t7 in r7[15:0]

    ;---------------------------------------------------------------------
    ; PASS1 step 2: calculate t8, t9, t10, t11
    ;---------------------------------------------------------------------
    SSUB16  t9t8, t5t4, t6t7              ; t9t8 (r8) = t5t4 (r6) - t6t7 (r7)
                                          ; t9 in r8[31:16],  t8 in r8[15:0]
    SADD16  t11t10, t5t4, t6t7            ; t11t10 (r9) = t5t4 (r6) + t6t7 (r7)
                                          ; t11 in r9[31:16], t10 in r9[15:0]

    ;---------------------------------------------------------------------
    ; PASS1 step 3: calculate t0, t1, t2, t3
    ; t5t4 (r6), t6t7 (r7) are not needed any more
    ;---------------------------------------------------------------------
    SSUB16  t1t0, x1x0, x6x7              ; t1t0 (r6) = x1x0 (r2) - x6x7 (r5)
                                          ; t1 in r6[31:16], t0 in r6[15:0]
    SSUB16  t3t2, x3x2, x4x5              ; t3t2 (r7) = x3x2 (r4) - x4x5 (r3)
                                          ; t3 in r7[31:16], t2 in r7[15:0]

    ;---------------------------------------------------------------------
    ; PASS1 step 4: calculate s0, s4
    ; x1x0 (r2), x3x2 (r4), x4x5 (r3), x6x7 (r5) are not needed any more
    ;---------------------------------------------------------------------
    LDR     c00c00, [kernel, #0]          ; c00c00 (r2) = [C00, C00]
    LDR     c41c40, [kernel, #(8 * 4)]    ; c41c40 (r4) = [C41, C40]

    SMUAD   y0, c00c00, t11t10            ; s0 (r3) = c00 * t11 + c00 * t10
                                          ;           r2.T * r9.T + r2.B * r9.B
    SMUAD   y4, c41c40, t11t10            ; s4 (r5) = c41 * t11 + c40 * t10
                                          ;           r4.T * r9,T + r4.B *r9.B

    MOVS    y0, y0, ASR #DCT_PASS1_BITS   ; s0 (r3) >>= DCT_PASS1_BITS (in Q3)
    MOVS    y4, y4, ASR #DCT_PASS1_BITS   ; s4 (r5) >>= DCT_PASS1_BITS (in Q3)
    STRH    y0, [dctDataPtr, #0]          ; *dctDataPtr = s0
    STRH    y4, [dctDataPtr, #(DCTSIZE * 2 * 4)]
                                          ; *(dctDataPtr + DCTSIZE * 4) = s4

    ;---------------------------------------------------------------------
    ; PASS1 step 5: calculate s2, s6
    ; t11t10 (r9) is not needed any more
    ;---------------------------------------------------------------------
    LDR     c21c20, [kernel, #(8 * 2)]    ; c21c20 (r2) = [C21, C20]
    LDR     c61c60, [kernel, #(8 * 6)]    ; c61c60 (r4) = [C61, C60]

    SMUAD   y2, c21c20, t9t8              ; s2 (r3) = c21 * t9 + t20 * t8
                                          ;           r2.T * r8.T + r2.B * r8.B
    SMUAD   y6, c61c60, t9t8              ; s6 (r5) = c61 * t9 + c60 * t8
                                          ;           r4.T * r8.T + r4.B * r8.B

    MOVS    y2, y2, ASR #DCT_PASS1_BITS   ; s2 (r3) >>= DCT_PASS1_BITS (in Q3)
    MOVS    y6, y6, ASR #DCT_PASS1_BITS   ; s6 (r5) >>= DCT_PASS1_BITS (in Q3)

    STRH    y2, [dctDataPtr, #(DCTSIZE * 2 * 2)]
                                          ; *(dctDataPtr + DCTSIZE * 2) = s2
    STRH    y6, [dctDataPtr, #(DCTSIZE * 2 * 6)]
                                          ; *(dctDataPtr + DCTSIZE * 6) = s6

    ;---------------------------------------------------------------------
    ; PASS1 step 6: calculate s1, s3, s5, s7
    ; t9t8 (r8) is not needed any more
    ;---------------------------------------------------------------------
    LDR     c11c10, [kernel, #(8 * 1)]    ; c11c10 (r2) = [C11, C10]
    LDR     c13c12, [kernel, #(8 * 1 + 4)]; c13c12 (r3) = [C13, C12]

    LDR     c31c30, [kernel, #(8 * 3)]    ; c31c30 (r5) = [C31, C30]
    LDR     c33c32, [kernel, #(8 * 3 + 4)]; c33c32 (r8) = [C33, C32]

    SMUAD   y1, c11c10, t1t0              ; s1 (r4) = c11 * t1 + c10 * t0
                                          ;           r2.T * r6.T + r2.B * r6.B
    SMUAD   y3, c31c30, t1t0              ; s3 (r9) = c31 * t1 + c30 * t0
                                          ;           r5.T * r6.T + r5.B * r6.B

    SMLAD   y1, c13c12, t3t2, y1          ; s1 (r4) += c13 * t3 + c12 * t2
                                          ;            r3.T * r7.T + r3.B * r7.B
    SMLAD   y3, c33c32, t3t2, y3          ; s3 (r9) += c33 * t3 + c32 * t2
                                          ;            r8.T * r7.T + r8.B * r7.B

    MOVS    y1, y1, ASR #DCT_PASS1_BITS   ; s1 (r4) >>= DCT_PASS1_BITS (in Q3)
    MOVS    y3, y3, ASR #DCT_PASS1_BITS   ; s3 (r9) >>= DCT_PASS1_BITS (in Q3)

    STRH    y1, [dctDataPtr, #(DCTSIZE * 2 * 1)]
                                          ; *(dctDataPtr + DCTSIZE * 1) = s1
    STRH    y3, [dctDataPtr, #(DCTSIZE * 2 * 3)]
                                          ; *(dctDataPtr + DCTSIZE * 3) = s3

    LDR     c51c50, [kernel, #(8 * 5)]    ; c51c50 (r2) = [C51, C50]
    LDR     c53c52, [kernel, #(8 * 5 + 4)]; c53c52 (r3) = [C53, C52]

    LDR     c71c70, [kernel, #(8 * 7)]    ; c71c70 (r5) = [C71, C70]
    LDR     c73c72, [kernel, #(8 * 7 + 4)]; c73c72 (r8) = [C73, C72]

    SMUAD   y5, c51c50, t1t0              ; s5 (r4) = c51 * t1 + c50 * t0
                                          ;           r2.T * r6.T + r2.B * r6.B
    SMUAD   y7, c71c70, t1t0              ; s7 (r9) = c71 * t1 + c70 * t0
                                          ;           r5.T * r6.T + r5.B * r6.B

    SMLAD   y5, c53c52, t3t2, y5          ; s5 (r4) += c53 * t3 + c52 * t2
                                          ;            r3.T * r7.T + r3.B * r7.B
    SMLAD   y7, c73c72, t3t2, y7          ; s7 (r9) += c73 * t3 + c72 * t2
                                          ;            r8.T * r7.T + r8.B * r7.B

    MOVS    y5, y5, ASR #DCT_PASS1_BITS   ; s5 (r4) >>= DCT_PASS1_BITS (in Q3)
    MOVS    y7, y7, ASR #DCT_PASS1_BITS   ; s7 (r9) >>= DCT_PASS1_BITS (in Q3)

    STRH    y5, [dctDataPtr, #(DCTSIZE * 2 * 5)]
                                          ; *(dctDataPtr + DCTSIZE * 5) = s5
    STRH    y7, [dctDataPtr, #(DCTSIZE * 2 * 7)]
                                          ; *(dctDataPtr + DCTSIZE * 7) = s7
    ;---------------------------------------------------------------------
    ; PASS1: 1D 8-point DCT processing ends
    ;---------------------------------------------------------------------

    ADD     index, index, #1              ; index++
    ADD     dctDataPtr, dctDataPtr, #2    ; dctDataPtr (r11) = &pass1DataBlock[0][index]
    CMP     index, #DCTSIZE               ; index is from 0 to (DCTSIZE - 1)
    BLT     load_pixels

    ;---------------------------------------------------------------------
    ; PASS2: processing columns
    ;---------------------------------------------------------------------
    MOV     index, #0                     ; index (r12) = 0
    MOV     pass1DataBlock, sp            ; pass1DataBlock (lr) = stack allocated
                                          ; (DCTSIZE2 * 2) bytes

dct_pass2_loop
    ADD     dctDataPtr, frequencyDomainBlock, index, LSL #1
                                          ; dctDataPtr = &frequencyDomainBlock[0][index]
load_pass1_data
    ;---------------------------------------------------------------------
    ; PASS2: load the PASS1 results from the intermediate location
    ;---------------------------------------------------------------------
    LDR     x1x0, [pass1DataBlock], #4    ; x1x0 (r2)
                                          ; pass1DataBlock += 4
    LDR     x3x2, [pass1DataBlock], #4    ; x3x2 (r4)
                                          ; pass1DataBlock += 4
    LDR     x5x4, [pass1DataBlock], #4    ; x5x4 (r3)
                                          ; pass1DataBlock += 4
    LDR     x7x6, [pass1DataBlock], #4    ; x7x6 (r5)
                                          ; pass1DataBlock += 4
    MOV     x4x5, x5x4, ROR #16           ; x4x5 (r3)
    MOV     x6x7, x7x6, ROR #16           ; x6x7 (r5)

    ;---------------------------------------------------------------------
    ; PASS2: 1D 8-point DCT processing begins
    ;---------------------------------------------------------------------
    ;---------------------------------------------------------------------
    ; PASS2 step 1: calculate t4, t5, t6, t7
    ;---------------------------------------------------------------------
    SADD16  t7t6, x3x2, x4x5              ; t7t6 (r7) = x3x2 (r4) + x4x5 (r3)
                                          ; t7 in r7[31:16], t6 in r7[15:0]
    SADD16  t5t4, x1x0, x6x7              ; t5t4 (r6) = x1x0 (r2) + x6x7 (r5)
                                          ; t5 in r6[31:16], t4 in r6[15:0]
    MOV     t6t7, t7t6, ROR #16           ; t6t7 (r7)
                                          ; t6 in r7[31:16], t7 in r7[15:0]

    ;---------------------------------------------------------------------
    ; PASS2 step 2: calculate t8, t9, t10, t11
    ;---------------------------------------------------------------------
    SSUB16  t9t8, t5t4, t6t7              ; t9t8 (r8) = t5t4 (r6) - t6t7 (r7)
                                          ; t9 in r8[31:16],  t8 in r8[15:0]
    SADD16  t11t10, t5t4, t6t7            ; t11t10 (r9) = t5t4 (r6) + t6t7 (r7)
                                          ; t11 in r9[31:16], t10 in r9[15:0]

    ;---------------------------------------------------------------------
    ; PASS2 step 3: calculate t0, t1, t2, t3
    ; t5t4 (r6), t6t7 (r7) are not needed any more
    ;---------------------------------------------------------------------
    SSUB16  t1t0, x1x0, x6x7              ; t1t0 (r6) = x1x0 (r2) - x6x7 (r5)
                                          ; t1 in r6[31:16], t0 in r6[15:0]
    SSUB16  t3t2, x3x2, x4x5              ; t3t2 (r7) = x3x2 (r4) - x4x5 (r3)
                                          ; t11 in r9[31:16], t10 in r9[15:0]

    ;---------------------------------------------------------------------
    ; PASS2 step 4: calculate s0, s4
    ; x1x0 (r2), x3x2 (r4), x4x5 (r3), x6x7 (r5) are not needed any more
    ;---------------------------------------------------------------------
    LDR     c00c00, [kernel, #0]          ; c00c00 (r2) = [C00, C00]
    LDR     c41c40, [kernel, #(8 * 4)]    ; c41c40 (r4) = [C41, C40]

    SMUAD   y0, c00c00, t11t10            ; s0 (r3) = c00 * t11 + c00 * t10
                                          ;           r2.T * r9.T + r2.B * r9.B
    SMUAD   y4, c41c40, t11t10            ; s4 (r5) = c41 * t11 + c40 * t10
                                          ;           r4.T * r9,T + r4.B *r9.B

    MOVS    y0, y0, ASR #DCT_PASS2_BITS   ; s0 (r3) >>= DCT_PASS2_BITS (in Q3)
    MOVS    y4, y4, ASR #DCT_PASS2_BITS   ; s4 (r5) >>= DCT_PASS2_BITS (in Q3)
    STRH    y0, [dctDataPtr, #0]          ; *dctDataPtr = s0
    STRH    y4, [dctDataPtr, #(DCTSIZE * 2 * 4)]
                                          ; *(dctDataPtr + DCTSIZE * 4) = s4

    ;---------------------------------------------------------------------
    ; PASS2 step 5: calculate s2, s6
    ; t11t10 (r9) is not needed any more
    ;---------------------------------------------------------------------
    LDR     c21c20, [kernel, #(8 * 2)]    ; c21c20 (r2) = [C21, C20]
    LDR     c61c60, [kernel, #(8 * 6)]    ; c61c60 (r4) = [C61, C60]

    SMUAD   y2, c21c20, t9t8              ; s2 (r3) = c21 * t9 + t20 * t8
                                          ;           r2.T * r8.T + r2.B * r8.B
    SMUAD   y6, c61c60, t9t8              ; s6 (r5) = c61 * t9 + c60 * t8
                                          ;           r4.T * r8.T + r4.B * r8.B

    MOVS    y2, y2, ASR #DCT_PASS2_BITS   ; s2 (r3) >>= DCT_PASS2_BITS (in Q3)
    MOVS    y6, y6, ASR #DCT_PASS2_BITS   ; s6 (r5) >>= DCT_PASS2_BITS (in Q3)

    STRH    y2, [dctDataPtr, #(DCTSIZE * 2 * 2)]
                                          ; *(dctDataPtr + DCTSIZE * 2) = s2
    STRH    y6, [dctDataPtr, #(DCTSIZE * 2 * 6)]
                                          ; *(dctDataPtr + DCTSIZE * 6) = s6

    ;---------------------------------------------------------------------
    ; PASS2 step 6: calculate s1, s3, s5, s7
    ; t9t8 (r8) is not needed any more
    ;---------------------------------------------------------------------
    LDR     c11c10, [kernel, #(8 * 1)]    ; c11c10 (r2) = [C11, C10]
    LDR     c13c12, [kernel, #(8 * 1 + 4)]; c13c12 (r3) = [C13, C12]

    LDR     c31c30, [kernel, #(8 * 3)]    ; c31c30 (r5) = [C31, C30]
    LDR     c33c32, [kernel, #(8 * 3 + 4)]; c33c32 (r8) = [C33, C32]

    SMUAD   y1, c11c10, t1t0              ; s1 (r4) = c11 * t1 + c10 * t0
                                          ;           r2.T * r6.T + r2.B * r6.B
    SMUAD   y3, c31c30, t1t0              ; s3 (r9) = c31 * t1 + c30 * t0
                                          ;           r5.T * r6.T + r5.B * r6.B

    SMLAD   y1, c13c12, t3t2, y1          ; s1 (r4) += c13 * t3 + c12 * t2
                                          ;            r3.T * r7.T + r3.B * r7.B
    SMLAD   y3, c33c32, t3t2, y3          ; s3 (r4) += c33 * t3 + c32 * t2
                                          ;            r8.T * r7.T + r8.B * r7.B

    MOVS    y1, y1, ASR #DCT_PASS2_BITS   ; s1 (r4) >>= DCT_PASS2_BITS (in Q3)
    MOVS    y3, y3, ASR #DCT_PASS2_BITS   ; s3 (r9) >>= DCT_PASS2_BITS (in Q3)

    STRH    y1, [dctDataPtr, #(DCTSIZE * 2 * 1)]
                                          ; *(dctDataPtr + DCTSIZE * 1) = s1
    STRH    y3, [dctDataPtr, #(DCTSIZE * 2 * 3)]
                                          ; *(dctDataPtr + DCTSIZE * 3) = s3

    LDR     c51c50, [kernel, #(8 * 5)]    ; c51c50 (r2) = [C51, C50]
    LDR     c53c52, [kernel, #(8 * 5 + 4)]; c53c52 (r3) = [C53, C52]

    LDR     c71c70, [kernel, #(8 * 7)]    ; c71c70 (r5) = [C71, C70]
    LDR     c73c72, [kernel, #(8 * 7 + 4)]; c73c72 (r8) = [C73, C72]

    SMUAD   y5, c51c50, t1t0              ; s5 (r4) = c51 * t1 + c50 * t0
                                          ;           r2.T * r6.T + r2.B * r6.B
    SMUAD   y7, c71c70, t1t0              ; s7 (r9) = c71 * t1 + c70 * t0
                                          ;           r5.T * r6.T + r5.B * r6.B

    SMLAD   y5, c53c52, t3t2, y5          ; s5 (r4) += c53 * t3 + c52 * t2
                                          ;            r3.T * r7.T + r3.B * r7.B
    SMLAD   y7, c73c72, t3t2, y7          ; s7 (r9) += c73 * t3 + c72 * t2
                                          ;            r8.T * r7.T + r8.B * r7.B

    MOVS    y5, y5, ASR #DCT_PASS2_BITS   ; s5 (r4) >>= DCT_PASS2_BITS (in Q3)
    MOVS    y7, y7, ASR #DCT_PASS2_BITS   ; s7 (r9) >>= DCT_PASS2_BITS (in Q3)

    STRH    y5, [dctDataPtr, #(DCTSIZE * 2 * 5)]
                                          ; *(dctDataPtr + DCTSIZE * 5) = s5
    STRH    y7, [dctDataPtr, #(DCTSIZE * 2 * 7)]
                                          ; *(dctDataPtr + DCTSIZE * 7) = s7
    ;---------------------------------------------------------------------
    ; PASS2: 1D 8-point DCT processing ends
    ;---------------------------------------------------------------------

    ADD     index, index, #1              ; index++
    CMP     index, #DCTSIZE               ; index is from 0 to (DCTSIZE - 1)
    BLT     dct_pass2_loop

    ;---------------------------------------------------------------------
    ; Free the allocated (DCTSIZE2 * 2) bytes in stack
    ;---------------------------------------------------------------------
    ADD     sp, sp, #(DCTSIZE2 * 2)       ; restore stack pointer

    ;---------------------------------------------------------------------
    ; Adjust DC with level shift
    ;---------------------------------------------------------------------
    LDRSH   dc, [frequencyDomainBlock]    ; dc = *frequencyDomainBlock
    SUB     dc, dc, #(128 * DCTSIZE2)     ; dc = *frequencyDomainBlock - (128 * DCTSIZE2)
    STRH    dc, [frequencyDomainBlock]    ; *frequencyDomainBlock = dc

    LDMFD   sp!, {r4-r11, lr}             ; POP     {r4-r11, lr}
    BX      lr                            ; end of jpege_engine_sw_fdct_block()

dct_kernel_table
    DCW (CC0), (CC0),  (CC0),  (CC0)
    DCW (CC1), (CC3),  (CC5),  (CC7)
    DCW (CC2), (CC6),  (-CC6), (CC2)
    DCW (CC3), (-CC7), (-CC1), (-CC5)
    DCW (CC4), (-CC4), (-CC4), (CC4)
    DCW (CC5), (-CC1), (CC7),  (CC3)
    DCW (CC6), (-CC2), (CC2),  (-CC6)
    DCW (CC7), (-CC5), (CC3),  (-CC1)

    ENDFUNC                               ; end of jpege_engine_sw_fdct_block()

    END                                   ; end of jpege_engine_sw_dct.s
