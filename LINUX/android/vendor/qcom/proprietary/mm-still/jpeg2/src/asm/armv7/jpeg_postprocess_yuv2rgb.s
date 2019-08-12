;==========================================================================
;    jpeg_postprocess_yuv2rgb.s
;
;
;
; Copyright (c) 1999-2002, 2009 Qualcomm Technologies, Inc.
; All Rights Reserved.
; Qualcomm Technologies Confidential and Proprietary
;==========================================================================

;==========================================================================
;                          Edit History
;
;
; when       who         what where why
; --------   -------     --------------------------------------------------
; 10/05/09   mingy       Changed function names to fit in Linux.
; 02/23/09   staceyw     Reverse order of blue and red for RGB888 and aRGB
; 02/10/09   staceyw     Merge all functions to jpeg_postprocess_yuv2rgb.s
; 01/29/09   staceyw     Implementation of Y1UV2toRGB, function added
; 01/22/09   staceyw     Initial implementation of Y2UV2toRGB, file added
;
;==========================================================================

;==========================================================================
;                         FUNCTION LIST
;--------------------------------------------------------------------------
; - y2vu2rgb888line
; - y2vu2rgb565line
; - y2vu2rgbaline
;
; - y1vu2rgb888line
; - y1vu2rgb565line
; - y1vu2rgbaline
;
;==========================================================================

    AREA  |.text|, CODE, READONLY
    CODE32

;==========================================================================
;   ARM Registers
;==========================================================================

pSrcLuma    RN 0
pSrcChroma  RN 1
pDstRGB     RN 2
width       RN 3

address_ptr RN 4

;==========================================================================
;   Main Routine
;==========================================================================

    EXPORT y2vu2rgb888line
    EXPORT y2vu2rgb565line
    EXPORT y2vu2rgbaline

    EXPORT y1vu2rgb888line
    EXPORT y1vu2rgb565line
    EXPORT y1vu2rgbaline

;==========================================================================
;   Constants
;==========================================================================

; coefficients in color conversion matrix multiplication
COEF_Y_SCALE  EQU  298                 ; contribution of Y
COEF_CB_RED   EQU  409                 ; contribution of Cb for red
COEF_CR_GREEN EQU  -100                ; contribution of Cr for green
COEF_CB_GREEN EQU  -208                ; contribution of Cb for green
COEF_CR_BLUE  EQU  516                 ; contribution of Cr for blue

; bias coefficients for red, green and blue
COEFF_BIAS_R EQU  -56992               ; bias of red=-298*16 -409*128 +128
COEFF_BIAS_G EQU   34784               ; bias of grn=-298*16 +308*128 +128
COEFF_BIAS_B EQU  -70688               ; bias of blu=-298*16 -516*128 +128

; constants of 0x0, 0xFF for clamping and 0xF8 for bits packing to RGB565
COEFF_0      EQU    0
COEFF_255    EQU    255
COEFF_F8     EQU    248



;==========================================================================
; FUNCTION     : y2vu2rgb888line
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to RGB888
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y2vu2rgb888line(uint8 * pLumaLine,
;                                     uint8 * pChromaLine,
;                                     uint8 * pRGB888Line,
;                                     uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output RGB Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R8
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y2vu2rgb888line  FUNCTION


    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r8,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB888+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB888 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y2RGB888

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y3 y5 y7 y9 y11 y13 y15
    ; d1: y2 y4 y6 y8 y10 y12 y14 y16
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD2.U8  {d0,d1}, [pSrcLuma]       ; Load 8 8-bit Luma elements to d0,
                                       ; d1 from Luma
    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 8-bit Chroma elements to d2,
                                       ; d3 from Luma

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y3 y5 y7 y9 y11 y13 y15
    ; q1: y2 y4 y6 y8 y10 y12 y14 y16
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q1, d1                ; (q1) = Y
    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d6, q12              ; output Red to d6

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d5, q12              ; output Green to d5

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Blue to d4

    VST3.u8  {d4,d5,d6}, [pDstRGB]     ; vector store red, green, blue to destination
                                       ; blue on low bytes
    ADD  pDstRGB, pDstRGB, #24

    ;==========================================================================
    ; First 8 elements done, Then Working on next 8 elements
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To d6

    VMULL.S16  q4, d29, d6[0]          ; Red
                                       ; (d29) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d29) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d31, d6[1]          ; Green
    VMLAL.S16  q5, d29, d6[2]          ; (d31) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d29) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d31, d6[3]          ; Blue
                                       ; (q6)=(d31) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Duplicating bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d6, q12              ; output Red to d6

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d5, q12              ; output Green to d5

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Blue to d4

    VST3.u8  {d4,d5,d6}, [pDstRGB]     ; vector store red, green, blue to destination
                                       ; blue on low bytes
    ADD  pDstRGB, pDstRGB, #24

    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #16       ; update uint8 *pSrcLuma with 16 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma with 16 bytes

    SUBS width, width, #16             ; update width counter with 16 bytes

    CMP width, #16
    BGE loop_neon_Y2RGB888             ; if greater than 16 bytes, continue
                                       ; to neon optimization

    CMP width, #0
    BLE loop_end_Y2RGB888              ; if no more pixels, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using ARM instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================
loop_arm_Y2RGB888

    ;==========================================================================
    ; Load input from y, then calculate y-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y-16) + 128
    ;==========================================================================
    MOV  r5, #298                      ; reading coefficient to r5
    SMULBB r5, r4, r5                  ; (luma-16) multiply coefficient to r5
    ADD  r5, #128                      ; add bias to r5

    LDRB r4, [pSrcLuma, #1]            ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16
    MOV  r6, #298                      ; reading coefficient to r6
    SMULBB r6, r4, r6                  ; (luma-16) multiply coefficient to r6
    ADD  r6, #128                      ; add bias to r6

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load Cb to r7
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load Cr to r8
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128
    ;==========================================================================
    MOV  r4, #409                      ; read coefficients
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF                     ; if r4 >255, then r4 =255
    MOVGT   r4, #0xFF

    STRB  r4, [pDstRGB, #2]            ; store red
                                       ; red on high bytes

    MOV  r4, #409                      ; Red
    SMLABB  r4, r8, r4, r6             ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128

    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF
    MOVGT   r4, #0xFF                  ; if r4 >255, then r4 =255

    STRB  r4, [pDstRGB, #5]            ; store red
                                       ; red on high bytes

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5             ; Calculate 516*(Cb-128),  plus 298*(y-16)+128

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    STRB  r4, [pDstRGB]                ; store blue
                                       ; blue on low bytes

    MOV r4, #516
    SMLABB  r4, r7, r4, r6             ; Calculate 516*(Cb-128),  plus 298*(y-16)+128

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    STRB  r4, [pDstRGB, #3]            ; store blue
                                       ; blue on low bytes

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV  r4, #-100
    SMLABB  r4, r7, r4, r5             ; Calculate -100*(Cr-128) plus 298*(y-16) + 128

    MOV  r5, #-208                     ; Calculate -208*(Cb-128)
    SMLABB  r4, r8, r5, r4             ; add together

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    STRB  r4, [pDstRGB, #1]            ; store green

    MOV  r4, #-100                     ; Calculate -100*(Cr-128) plus 298*(y-16) + 128
    SMLABB  r4, r7, r4, r6

    MOV  r5, #-208                     ; Calculate -208*(Cb-128)
    SMLABB  r4, r8, r5, r4             ; add together

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    STRB  r4, [pDstRGB, #4]            ; store green

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #6          ; update uint8 *pDstRGB with 6 bytes
    ADD  pSrcLuma,pSrcLuma, #2         ; update uint8 *pSrcLuma with 2 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #2              ; update width counter with 2 bytes

    BGT loop_arm_Y2RGB888              ; if more pixels left, continue
    BLE loop_end_Y2RGB888              ; otherwise go to the end


loop_end_Y2RGB888
    LDMFD sp!, {r4-r8, pc}

constantsY2RGB888
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    ENDFUNC                             ;end of y2vu2rgb888line





;==========================================================================
; FUNCTION     : y2vu2rgb565line
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to RGB565
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y2vu2rgb565line(uint8 * pLumaLine,
;                                     uint8 * pChromaLine,
;                                     uint8 * pRGB888Line,
;                                     uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output RGB Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R9
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y2vu2rgb565line  FUNCTION

    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r9,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB565+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB565 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y2RGB565

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y3 y5 y7 y9 y11 y13 y15
    ; d1: y2 y4 y6 y8 y10 y12 y14 y16
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD2.U8  {d0,d1}, [pSrcLuma]       ; Load 8 8-bit Luma elements to d0, d1 from Luma
    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 8-bit Chroma elements to d2, d3 from Luma

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y3 y5 y7 y9 y11 y13 y15
    ; q1: y2 y4 y6 y8 y10 y12 y14 y16
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q1, d1                ; (q1) = Y
    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB565    ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Red to d4

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d5, q12              ; output Green to d5

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d6, q12              ; output Blue to d6

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 565 values
    ; d4:  8 bits of Red
    ; d5:  8 bits of Green
    ; d6:  8 bits of Blue
    ;==========================================================================
    VSHL.U8 d9, d5, #3                 ; left shift green 3 bits
    VMOV.I8 d8, #0xE0                  ; (d8) = 1110,0000
    VAND.8  d9, d8                     ; bitwise and to get the 3 bits of green
    VSHR.U8 d8, d6, #3                 ; right shift blue 3 bits
    VORR.8  d8, d9                     ; bitwise or to pack 3 bits green and 5 bits blue

    VMOV.I8 d9, #0xF8                  ; (d9) = 1111,1000
    VAND.8  d9, d4                     ; bitwise and to get most significant 5 bits red
    VSHR.U8 d5, d5, #5                 ; right shift green 5 bits
    VORR.8  d9, d5                     ; bitwise or to pack 5 bits red and 3 bits green

    ;==========================================================================
    ; d9:  5 bits of Red   + 3 bits of Green
    ; d8:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    VST2.u8  {d8,d9}, [pDstRGB]        ; store vectors
    ADD  pDstRGB, pDstRGB, #16


    ;==========================================================================
    ; First 8 elements done, Then Working on next 8 elements
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    ADR      address_ptr, constantsY2RGB565
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To d6

    VMULL.S16  q4, d29, d6[0]          ; Red
                                       ; (d29) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d29) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d31, d6[1]          ; Green
    VMLAL.S16  q5, d29, d6[2]          ; (d31) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d29) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d31, d6[3]          ; Blue
                                       ; (q6)=(d31) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Duplicating bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Red to d4

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d5, q12              ; output Green to d5

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d6, q12              ; output Blue to d6


    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 565 values
    ; d4:  8 bits of Red
    ; d5:  8 bits of Green
    ; d6:  8 bits of Blue
    ;==========================================================================
    VSHL.U8 d9, d5, #3                 ; left shift green 3 bits
    VMOV.I8 d8, #0xE0                  ; (d8) = 1110,0000
    VAND.8  d9, d8                     ; bitwise and to get the 3 bits of green
    VSHR.U8 d8, d6, #3                 ; right shift blue 3 bits
    VORR.8  d8, d9                     ; bitwise or to pack 3 bits green and 5 bits blue

    VMOV.I8 d9, #0xF8                  ; (d9) = 1111,1000
    VAND.8  d9, d4                     ; bitwise and to get most significant 5 bits red
    VSHR.U8 d5, d5, #5                 ; right shift green 5 bits
    VORR.8  d9, d5                     ; bitwise or to pack 5 bits red and 3 bits green

    ;==========================================================================
    ; d9:  5 bits of Red   + 3 bits of Green
    ; d8:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    VST2.u8  {d8,d9}, [pDstRGB]        ; store vectors
    ADD  pDstRGB, pDstRGB, #16


    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #16       ; update uint8 *pSrcLuma for 16 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma for 16 bytes

    SUBS width, width, #16             ; update width counter with 16 bytes

    CMP width, #16                     ; if greater than 16 pixels
    BGE loop_neon_Y2RGB565             ; go to the neon optimization

    CMP width, #0
    BLE loop_end_Y2RGB565              ; if no more pixels left, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using ARM instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================

loop_arm_Y2RGB565

    ;==========================================================================
    ; Load input from y1, then calculate y1-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y1-16) + 128
    ;==========================================================================
    MOV  r5, #298
    SMULBB r5, r4, r5                  ; calculate 298*(luma-16)
    ADD  r5, #128                      ; plus 128

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load chroma Cr
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load chroma Cb
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y1-16) + 128
    ;==========================================================================
    MOV  r4, #409
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y1-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV    r6, #-100
    SMLABB r6, r7, r6, r5

    MOV    r9, #-208
    SMLABB r6, r8, r9, r6

    ASRS  r6, #8                       ; Shifting
    MOVMI r6, #0                       ; if r6 <0, then r6 =0
    CMP   r6, #0xFF                    ; if r6 >255, then r6 =255
    MOVGT r6, #0xFF

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 888 values
    ; r4:  8 bits of Red
    ; r6:  8 bits of Green
    ; r4:  output of 5 bits of Red   + 3 bits of Green
    ;==========================================================================
    AND   r4, r4, #0xF8                ; bitwise and red with 1111,1000
    ORR   r4, r4, r6, LSR #5           ; bitwise or to pack 5 bits red with 3 bits green

    STRB  r4, [pDstRGB, #1]            ; store

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y1-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 888 values
    ; d4:  8 bits of Blue
    ; d6:  8 bits of Green
    ; d9:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    MOV  r9, #0xE0
    AND  r9, r9, r6, LSL #3            ; bitwise and shifted green with 1110,0000
    ORR  r9, r9, r4, LSR #3            ; bitwise or to get 3 bits green with 5 bits blue

    STRB  r9, [pDstRGB]                ; store


    ;==========================================================================
    ; Load input from y2, then calculate y2-16
    ;==========================================================================
    LDRB r4, [pSrcLuma, #1]            ; load luma
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y2-16) + 128
    ;==========================================================================
    MOV  r5, #298
    SMULBB r5, r4, r5
    ADD  r5, #128

    ;==========================================================================
    ; Sharing input from Cb, and calculated Cb-128
    ; Sharing input from Cr, and calculated Cr-128
    ;==========================================================================
    ;LDRB r7, [pSrcChroma, #1]         ; load Cr
    ;SUB  r7, r7, #128                 ; Cr-128

    ;LDRB r8, [pSrcChroma]             ; load Cb
    ;SUB  r8, r8, #128                 ; Cb-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y2-16) + 128
    ;==========================================================================
    MOV  r4, #409
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y2-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV    r6, #-100
    SMLABB r6, r7, r6, r5

    MOV    r9, #-208
    SMLABB r6, r8, r9, r6

    ASRS  r6, #8                       ; Shifting
    MOVMI r6, #0                       ; if r6 <0, then r6 =0
    CMP   r6, #255                     ; if r6 >255, then r6 =255
    MOVGT r6, #255

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 888 values
    ; d4:  8 bits of Red
    ; d6:  8 bits of Green
    ; d4:  5 bits of Red   + 3 bits of Green
    ;==========================================================================
    AND   r4, r4, #0xF8                ; bitwise and red with 1111,1000
    ORR   r4, r4, r6, LSR #5           ; bitwise or to pack 5 bits red with 3 bits green

    STRB  r4, [pDstRGB, #3]

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y2-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 565 values
    ; d4:  8 bits of Blue
    ; d6:  8 bits of Green
    ; d9:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    MOV  r9, #0xE0
    AND  r9, r9, r6, LSL #3            ; bitwise and shifted green with 1110,0000
    ORR  r9, r9, r4, LSR #3            ; bitwise or to get 3 bits green with 5 bits blue

    STRB  r9, [pDstRGB, #2]

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #4          ; update uint8 *pDstRGB with 4 bytes
    ADD  pSrcLuma,pSrcLuma, #2         ; update uint8 *pSrcLuma with 2 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #2              ; update width counter with 2 bytes

    BGT loop_arm_Y2RGB565              ; if no more pixels left, go to the end
    BLE loop_end_Y2RGB565              ; otherwise, continue


loop_end_Y2RGB565
    LDMFD sp!, {r4-r9, pc}

constantsY2RGB565
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    ENDFUNC                            ;end of y2vu2rgb565line




;==========================================================================
; FUNCTION     : y2vu2rgbaline
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to aRGB
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y2vu2rgbaline(uint8 * pLumaLine,
;                                   uint8 * pChromaLine,
;                                   uint8 * pRGB888Line,
;                                   uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output RGB Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R8
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y2vu2rgbaline  FUNCTION


    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r8,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    ADR      address_ptr, constantsY2aRGB888+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constantsY2aRGB888 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y2aRGB888

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y3 y5 y7 y9 y11 y13 y15
    ; d1: y2 y4 y6 y8 y10 y12 y14 y16
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD2.U8  {d0,d1}, [pSrcLuma]       ; Load 8 8-bit Luma elements to d0, d1 from Luma
    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 8-bit Chroma elements to d2, d3 from Luma

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y3 y5 y7 y9 y11 y13 y15
    ; q1: y2 y4 y6 y8 y10 y12 y14 y16
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q1, d1                ; (q1) = Y
    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constantsY2aRGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d2, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d15, q12              ; output Red to d15(q7)

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d14, q12              ; output Green to d14(q7)

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d13, q12             ; output Blue to d13(q6)

    ;==========================================================================
    ; aRGB format with starting 0xFF byte
    ;==========================================================================
    VMOVN.s16     d16, q10              ; (q10) =0xFF, duplicate it to vector d16(q8)

    VST4.u8  {d13,d14,d15,d16}, [pDstRGB]
                                       ; vector store 0xFF,red, green, blue
                                       ; blue on low bytes
    ADD  pDstRGB, pDstRGB, #32


    ;==========================================================================
    ; First 8 elements done, Then Working on next 8 elements
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    ADR      address_ptr, constantsY2aRGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To d6

    VMULL.S16  q4, d29, d6[0]          ; Red
                                       ; (d29) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d29) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d31, d6[1]          ; Green
    VMLAL.S16  q5, d29, d6[2]          ; (d31) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d29) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d31, d6[3]          ; Blue
                                       ; (q6)=(d31) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Duplicating bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMOV.S32  q7,q4                    ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q4, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q7, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMOV.S32 q8,q5                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q5, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q8, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMOV.S32 q9,q6                     ; y1 and y2 share the same Cb1 Cr1

    VMLAL.S16  q6, d1, d7[0]           ; for y1 y3 y5 y7
    VMLAL.S16  q9, d3, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d24, q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d25, q7, #8            ; into d24 and d25

    ;==========================================================================
    ;  Interleave from output of [1 3 5 7 2 4 6 8 ] to [1 2 3 4 5 6 7 8]
    ;==========================================================================
    VZIP.16   d24, d25                 ; interleaving d24 and d25 elements

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d5, q12              ; output Red to d5(q2)

    VSHRN.S32   d24, q5, #8            ; q5 and q8 is 32 bits of Green, right shifting
    VSHRN.S32   d25, q8, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Green to d4(q2)

    VSHRN.S32   d24, q6, #8            ; q6 and q9 is 32 bits of Blue, right shifting
    VSHRN.S32   d25, q9, #8            ; into d24 and d25
    VZIP.16   d24, d25                 ; interleaving d24 and d25 vector elements

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d3, q12              ; output Blue to d3(q1)


    ;==========================================================================
    ; aRGB format with starting 0xFF byte
    ;==========================================================================
    VMOVN.s16     d6, q10              ; (q10) =0xFF, duplicate it to vector d6(q3)

    VST4.u8  {d3, d4,d5,d6}, [pDstRGB] ; vector store 0xFF,red, green, blue
                                       ; blue on low bytes

    ADD  pDstRGB, pDstRGB, #32

    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #16       ; update uint8 *pSrcLuma with 16 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma with 16 bytes

    SUBS width, width, #16             ; update width counter with 16 bytes

    CMP width, #16                     ; if more than 16 pixels left,
    BGE loop_neon_Y2aRGB888            ; continue neon optimization

    CMP width, #0
    BLE loop_end_Y2aRGB888             ; if no more pixels left, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using ARM instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================
loop_arm_Y2aRGB888

    ;==========================================================================
    ; Load input from y, then calculate y-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; aRGB format with starting 0xFF byte
    ;==========================================================================
    MOV  r6, #0xFF
    STRB r6, [pDstRGB, #3]             ; store 0xFF
    STRB r6, [pDstRGB, #7]             ; store 0xFF

    ;==========================================================================
    ; Calculate 298*(y-16) + 128
    ;==========================================================================
    MOV  r5, #298                      ; reading coefficient to r5
    SMULBB r5, r4, r5                  ; (luma-16) multiply coefficient to r5
    ADD  r5, #128                      ; add bias to r5

    LDRB r4, [pSrcLuma, #1]            ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16
    MOV  r6, #298                      ; reading coefficient to r6
    SMULBB r6, r4, r6                  ; (luma-16) multiply coefficient to r6
    ADD  r6, #128                      ; add bias to r6

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load Cb to r7
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load Cr to r8
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128
    ;==========================================================================
    MOV  r4, #409                      ; read coefficients
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF                     ; if r4 >255, then r4 =255
    MOVGT   r4, #0xFF

    STRB  r4, [pDstRGB, #2]            ; store red
                                       ; red on high bytes

    MOV  r4, #409                      ; Red
    SMLABB  r4, r8, r4, r6             ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128

    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF
    MOVGT   r4, #0xFF                  ; if r4 >255, then r4 =255

    STRB  r4, [pDstRGB, #6]            ; store red
                                       ; red on high bytes

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5             ; Calculate 516*(Cb-128),  plus 298*(y-16)+128

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    STRB  r4, [pDstRGB]                ; store blue
                                       ; blue on low bytes

    MOV r4, #516
    SMLABB  r4, r7, r4, r6             ; Calculate 516*(Cb-128),  plus 298*(y-16)+128

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    STRB  r4, [pDstRGB, #4]            ; store blue
                                       ; blue on low bytes

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV  r4, #-100
    SMLABB  r4, r7, r4, r5             ; Calculate -100*(Cr-128) plus 298*(y-16) + 128

    MOV  r5, #-208                     ; Calculate -208*(Cb-128)
    SMLABB  r4, r8, r5, r4             ; add together

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    STRB  r4, [pDstRGB, #1]            ; store green

    MOV  r4, #-100                     ; Calculate -100*(Cr-128) plus 298*(y-16) + 128
    SMLABB  r4, r7, r4, r6

    MOV  r5, #-208                     ; Calculate -208*(Cb-128)
    SMLABB  r4, r8, r5, r4             ; add together

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    STRB  r4, [pDstRGB, #5]            ; store green

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #8          ; update uint8 *pDstRGB with 8 bytes
    ADD  pSrcLuma,pSrcLuma, #2         ; update uint8 *pSrcLuma with 2 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #2              ; update width counter with 2 bytes

    BGT loop_arm_Y2aRGB888             ; if no more pixels left, go to the end
    BLE loop_end_Y2aRGB888             ; otherwise, continue


loop_end_Y2aRGB888
    LDMFD sp!, {r4-r8, pc}

constantsY2aRGB888
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    ENDFUNC                             ;end of y2vu2rgbaline



;==========================================================================
; FUNCTION     : y1vu2rgb888line
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to RGB888
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y1vu2rgb888line(uint8 * pLumaLine,
;                                     uint8 * pChromaLine,
;                                     uint8 * pRGB888Line,
;                                     uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output RGB888 Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R8
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y1vu2rgb888line  FUNCTION

    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r8,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB888+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB888 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y1RGB888

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y2 y3 y4 y5 y6 y7 y8
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD1.U8  {d0}, [pSrcLuma]          ; Load 8 bytes Luma elements to d0

    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 bytes Chroma elements to d2, d3

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y2 y3 y4 y5 y6 y7 y8
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================

    VMLAL.S16  q4, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================

    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================

    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d2 , q4, #8            ; q4 is 32 bits of Red, right shifting
    VSHRN.S32   d4 , q5, #8            ; q5 is 32 bits of green, right shifting
    VSHRN.S32   d24, q6, #8            ; q6 is 32 bits of blue, right shifting


    ;==========================================================================
    ; Working on first 4 elements above, Then Working on next 4 elements
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d29, d6[0]          ; Red
                                       ; (d29) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d29) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d31, d6[1]          ; Green
    VMLAL.S16  q5, d29, d6[2]          ; (d31) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d29) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d31, d6[3]          ; Blue
                                       ; (q6)=(d31) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Duplicating bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================

    VMLAL.S16  q4, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================

    VMLAL.S16  q5, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================

    VMLAL.S16  q6, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d3, q4, #8             ; q4 is 32 bits of Red, right shifting
    VSHRN.S32   d5, q5, #8             ; q5 is 32 bits of green, right shifting
    VSHRN.S32   d25, q6, #8            ; q6 is 32 bits of blue, right shifting

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q1, q1, q11              ; if q1 <0, q1 =0
    VMIN.s16  q1, q1, q10              ; if q1 >255, q1 =255

    VQMOVUN.s16   d6, q1               ; store red to d6(q3)

    VMAX.s16  q2, q2, q11              ; if q2 <0, q2 =0
    VMIN.s16  q2, q2, q10              ; if q2 >255, q2 =255

    VQMOVUN.s16   d5, q2               ; store green to d5(q2)

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; store blue to d4(q2)

    VST3.u8  {d4,d5,d6}, [pDstRGB]     ; vector store red, green, blue to destination
                                       ; blue on low bytes

    ADD  pDstRGB, pDstRGB, #24

    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #8        ; update uint8 *pSrcLuma with 8 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma with 16 bytes

    SUBS width, width, #8              ; update width counter with 8 bytes

    CMP width, #8
    BGE loop_neon_Y1RGB888             ; if greater than 8 bytes, continue
                                       ; to neon optimization

    CMP width, #0
    BLE loop_end_Y1RGB888              ; if no more pixels, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using ARM instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================

loop_arm_Y1RGB888

    ;==========================================================================
    ; Load input from y, then calculate y-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y-16) + 128
    ;==========================================================================
    MOV  r5, #298                      ; reading coefficient to r5
    SMULBB r5, r4, r5                  ; (luma-16) multiply coefficient to r5
    ADD  r5, #128                      ; add bias to r5

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load Cb to r7
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load Cr to r8
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128
    ;==========================================================================
    MOV  r4, #409                      ; read coefficients
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF                     ; if r4 >255, then r4 =255
    MOVGT   r4, #0xFF

    STRB  r4, [pDstRGB, #2]            ; store red
                                       ; red on high bytes

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516                       ; Calculate 516*(Cb-128),  plus 298*(y-16)+128
    SMLABB  r4, r7, r4, r5

    ASRS  r4, #8
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    STRB  r4, [pDstRGB]                ; store blue
                                       ; blue on low bytes

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV  r4, #-100
    SMLABB  r4, r7, r4, r5             ; Calculate -100*(Cr-128) plus 298*(y-16) + 128

    MOV  r5, #-208                     ; Calculate -208*(Cb-128)
    SMLABB  r4, r8, r5, r4             ; add together

    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP   r4, #0xFF                    ; if r4 >255, then r4 =255
    MOVGT r4, #0xFF

    STRB  r4, [pDstRGB, #1]            ; store green

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #3          ; update uint8 *pDstRGB with 3 bytes
    ADD  pSrcLuma,pSrcLuma, #1         ; update uint8 *pSrcLuma with 1 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #1              ; update width counter with 2 bytes

    BGT loop_arm_Y1RGB888              ; if more pixels left, continue
    BLE loop_end_Y1RGB888              ; otherwise go to the end


loop_end_Y1RGB888
    LDMFD sp!, {r4-r8, pc}


constants_Y1RGB888
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)


    ENDFUNC                            ; end of y1vu2rgb888line




;==========================================================================
; FUNCTION     : y1vu2rgb565line
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to RGB565
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y1vu2rgb565line(uint8 * pLumaLine,
;                                     uint8 * pChromaLine,
;                                     uint8 * pRGB888Line,
;                                     uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output RGB565 Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R9
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y1vu2rgb565line  FUNCTION

    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r9,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB565+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB565 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y1RGB565

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y3 y5 y7
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD1.U8  {d0}, [pSrcLuma]          ; Load 8 8-bit Luma elements to d0 from Luma

    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 8-bit Chroma elements to d2, d3
                                       ; from Luma

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y3 y5 y7
    ; q1: y2 y4 y6 y8
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constants_Y1RGB565
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of blue duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================

    VMLAL.S16  q4, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================

    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================

    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]              ; (q11) =0
    vdup.s16  q10,  d7[2]              ; (q10) =255

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d2 , q4, #8            ; q4 and q7 is 32 bits of Red, right shifting
    VSHRN.S32   d4 , q5, #8
    VSHRN.S32   d24, q6, #8

    ;==========================================================================
    ; First 8 elements done, Then Working on next 8 elements
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d29, d6[0]          ; Red
                                       ; (d29) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d29) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d31, d6[1]          ; Green
    VMLAL.S16  q5, d29, d6[2]          ; (d31) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d29) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d31, d6[3]          ; Blue
                                       ; (q6)=(d31) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Duplicating bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]
    vdup.s32  q8,  q13[1]
    vdup.s32  q9,  q13[2]

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; Red

    VADD.S32  q5, q8                   ; Green

    VADD.S32  q6, q9                   ; Blue

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMLAL.S16  q4, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMLAL.S16  q5, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMLAL.S16  q6, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]
    vdup.s16  q10,  d7[2]

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d3, q4, #8
    VSHRN.S32   d5, q5, #8
    VSHRN.S32   d25, q6, #8

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q1, q1, q11              ; if q1 <0, q1 =0
    VMIN.s16  q1, q1, q10              ; if q1 >255, q1 =255

    VQMOVUN.s16   d2, q1               ; output Red to d2

    VMAX.s16  q2, q2, q11              ; if q2 <0, q2 =0
    VMIN.s16  q2, q2, q10              ; if q2 >255, q2 =255

    VQMOVUN.s16   d3, q2               ; output Green to d3

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d4, q12              ; output Blue to d4

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 565 values
    ; d4:  8 bits of Red
    ; d5:  8 bits of Green
    ; d6:  8 bits of Blue
    ;==========================================================================
    VSHL.U8 d9, d3, #3                 ; left shift green 3 bits
    VMOV.I8 d8, #0xE0                  ; (d8) = 1110,0000
    VAND.8  d9, d8                     ; bitwise and to get the 3 bits of green
    VSHR.U8 d8, d4, #3                 ; right shift blue 3 bits
    VORR.8  d8, d9                     ; bitwise or to pack 3 bits green and 5 bits blue

    VMOV.I8 d9, #0xF8                  ; (d9) = 1111,1000
    VAND.8  d9, d2                     ; bitwise and to get most significant 5 bits red
    VSHR.U8 d3, d3, #5                 ; right shift green 5 bits
    VORR.8  d9, d3                     ; bitwise or to pack 5 bits red and 3 bits green

    ;==========================================================================
    ; d9:  5 bits of Red   + 3 bits of Green
    ; d8:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    VST2.u8  {d8,d9}, [pDstRGB]        ; store vectors
    ADD  pDstRGB, pDstRGB, #16


    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #8        ; update uint8 *pSrcLuma for 8 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma for 8 bytes

    SUBS width, width, #8              ; update width counter with 8 bytes

    CMP width, #8                      ; if greater than 8 pixels
    BGE loop_neon_Y1RGB565             ; go to the neon optimization

    CMP width, #0
    BLE loop_end_Y1RGB565              ; if no more pixels left, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using ARM instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================
loop_arm_Y1RGB565

    ;==========================================================================
    ; Load input from y1, then calculate y1-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y1-16) + 128
    ;==========================================================================
    MOV  r5, #298                      ; calculate 298*(luma-16)
    SMULBB r5, r4, r5
    ADD  r5, #128                      ; plus 128

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load chroma Cr
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load chroma Cb
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y1-16) + 128
    ;==========================================================================
    MOV    r4, #409
    SMLABB r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP  r4, #255                      ; if r4 >255, then r4 =255
    MOVGT   r4, #255

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y1-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV    r6, #-100
    SMLABB r6, r7, r6, r5

    MOV    r9, #-208
    SMLABB r6, r8, r9, r6

    ASRS  r6, #8                       ; Shifting
    MOVMI r6, #0                       ; if r6 <0, then r6 =0
    CMP   r6, #0xFF                    ; if r6 >255, then r6 =255
    MOVGT r6, #0xFF

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 888 values
    ; r4:  8 bits of Red
    ; r6:  8 bits of Green
    ; r4:  output of 5 bits of Red   + 3 bits of Green
    ;==========================================================================
    AND   r4, r4, #0xF8                ; bitwise and red with 1111,1000
    ORR   r4, r4, r6, LSR #5           ; bitwise or to pack 5 bits red with 3 bits green

    STRB  r4, [pDstRGB, #1]            ; store

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y1-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    ;==========================================================================
    ; Packing the RGB 888 pixel values to RGB 888 values
    ; d4:  8 bits of Blue
    ; d6:  8 bits of Green
    ; d9:  3 bits of Green + 5 bits of Blue
    ;==========================================================================
    MOV  r9, #0xE0
    AND  r9, r9, r6, LSL #3            ; bitwise and shifted green with 1110,0000
    ORR  r9, r9, r4, LSR #3            ; bitwise or to get 3 bits green with 5 bits blue

    STRB  r9, [pDstRGB]                ; store

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #2          ; update uint8 *pDstRGB with 2 bytes
    ADD  pSrcLuma,pSrcLuma, #1         ; update uint8 *pSrcLuma with 1 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #1              ; update width counter with 1 bytes

    BGT loop_arm_Y1RGB565              ; if no more pixels left, go to the end
    BLE loop_end_Y1RGB565              ; otherwise, continue


loop_end_Y1RGB565
    LDMFD sp!, {r4-r9, pc}

constants_Y1RGB565
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    ENDFUNC                           ; end of y1vu2rgb565line




;==========================================================================
; FUNCTION     : y1vu2rgbaline
;--------------------------------------------------------------------------
; DESCRIPTION  : ASM optimization of YCbCr to aRGB888
;--------------------------------------------------------------------------
; C PROTOTYPE  : void y1vu2rgbaline(uint8 * pLumaLine,
;                                   uint8 * pChromaLine,
;                                   uint8 * pRGB888Line,
;                                   uint32 nLineWidth);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint8  pointer to the input Luma Line
;                R1 uint8  pointer to the input Chroma Line
;                R2 uint8  pointer to the output RGB Line
;                R3 uint32 width of Line
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R2 output aRGB Line
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : ARM:  R0-R8
;                NEON: Q0-Q15
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : none
;
;--------------------------------------------------------------------------
; NOTES        :
;==========================================================================
y1vu2rgbaline  FUNCTION

    ;==========================================================================
    ; Store stack registers
    ;==========================================================================
    STMFD sp!, {r4-r8,  lr}

    PLD [r0, r3]                       ; preload luma line

    ;==========================================================================
    ; Reading bias
    ;=========================================================================
    ADR      address_ptr, constants_Y1aRGB888+ 8*2
                                       ; Load the address of biases
    VLD1.S32 q13,  [address_ptr]!      ; To the q13

    ;==========================================================================
    ; Reading coefficients
    ;==========================================================================
    ADR      address_ptr, constants_Y1aRGB888 + 4*2
                                       ; Load the address of coefficients
    VLD1.S16  d7,  [address_ptr]!      ; To the d7


    ;==========================================================================
    ; Outer loop
    ;==========================================================================
loop_neon_Y1aRGB888

    ;==========================================================================
    ; Load input from y, u and v
    ; d0: y1 y2 y3 y4 y5 y6 y7 y8
    ; d2: Cb
    ; d3: Cr
    ;==========================================================================
    VLD1.U8  {d0}, [pSrcLuma]          ; Load 8 8-bit Luma elements to d0

    VLD2.U8  {d2,d3}, [pSrcChroma]     ; Load 8 8-bit Chroma elements to d2, d3

    ;==========================================================================
    ; Expand unsigned 8 bits value to 16 bits
    ; q0: y1 y3 y5 y7
    ; q1: y2 y4 y6 y8
    ; q14: Cb
    ; q15: Cr
    ;==========================================================================
    VMOVL.U16.U8 q15, d3               ; (q15) = Cr
    VMOVL.U16.U8 q14, d2               ; (q14) = Cb

    VMOVL.U16.U8 q0, d0                ; (q0) = Y

    ;==========================================================================
    ; Reading Coefficients of chrominance for multiplication
    ;==========================================================================
    ADR      address_ptr, constants_Y1aRGB888
                                       ; Load the address of coefficients
    VLD1.S16  d6,  [address_ptr]       ; To the d6

    ;==========================================================================
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ;==========================================================================

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d28, d6[0]          ; Red
                                       ; (d28) = Cb
                                       ; (d6[0]) = COEF_CB_RED
                                       ; (q4)= (d28) * (d6[0]) = Cb * COEF_CB_RED

    VMULL.S16  q5, d30, d6[1]          ; Green
    VMLAL.S16  q5, d28, d6[2]          ; (d30) * (d6[1]) = Cr * COEF_CR_GREEN
                                       ; (d28) * (d6[2]) = Cb * COEF_CB_GREEN
                                       ; (q5)= Cb * COEF_CB_GREEN + Cr * COEF_CR_GREEN

    VMULL.S16  q6, d30, d6[3]          ; Blue
                                       ; (q6)=(d30) * (d6[3]) = Cr * COEF_CR_BLUE

    ;==========================================================================
    ; Reading bias
    ;=========================================================================
    vdup.s32  q7,  q13[0]              ; bias of Red duplicate to all elements of vectors
    vdup.s32  q8,  q13[1]              ; bias of Green duplicate to all elements of vectors
    vdup.s32  q9,  q13[2]              ; bias of Green duplicate to all elements of vectors

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMLAL.S16  q4, d0, d7[0]            ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMLAL.S16  q5, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMLAL.S16  q6, d0, d7[0]           ; for y1 y3 y5 y7

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]
    vdup.s16  q10,  d7[2]

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d2 , q4, #8            ; store red to d2(q1)
    VSHRN.S32   d4 , q5, #8            ; store green to d4(q2)
    VSHRN.S32   d24, q6, #8            ; store blue to d24(q12)

    ; -------------------------------------------------------------------------------------
    ; The new algorithm is:
    ; R_vector = clamp( ((298*(Luma_vector -16) |   0   409|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; G_vector = clamp( ((298*(Luma_vector -16)+|-100  -208|(u_vector-128 v_vector-128)’+128) >> 8) )
    ; B_vector = clamp( ((298*(Luma_vector -16) | 516   0  |(u_vector-128 v_vector-128)’+128) >> 8) )
    ;
    ; R_vector = clamp( ((298*Luma_vector + |   0   409|(u_vector v_vector)’ - 56992) >> 8) )
    ; G_vector = clamp( ((298*Luma_vector + |-100  -208|(u_vector v_vector)’ + 34784) >> 8) )
    ; B_vector = clamp( ((298*Luma_vector + | 516   0  |(u_vector v_vector)’ - 70688) >> 8) )
    ; -------------------------------------------------------------------------------------

    ;==========================================================================
    ; Multiply contribution from chrominance
    ;==========================================================================
    VMULL.S16  q4, d29, d6[0]          ; Red for y2 y4 y6 y8

    VMULL.S16  q5, d31, d6[1]          ; Green for y2 y4 y6 y8
    VMLAL.S16  q5, d29, d6[2]

    VMULL.S16  q6, d31, d6[3]          ; Blue for y2 y4 y6 y8

    ;==========================================================================
    ; Reading bias
    ;==========================================================================
    vdup.s32  q7,  q13[0]
    vdup.s32  q8,  q13[1]
    vdup.s32  q9,  q13[2]

    ;==========================================================================
    ; Add or subtract bias
    ;==========================================================================
    VADD.S32  q4, q7                   ; q4 add bias of Red from q7

    VADD.S32  q5, q8                   ; q5 add bias of Green from q8

    VADD.S32  q6, q9                   ; q6 add bias of Blue from q9

    ;==========================================================================
    ; Calculate Red
    ;==========================================================================
    VMLAL.S16  q4, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Green
    ;==========================================================================
    VMLAL.S16  q5, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Calculate Blue
    ;==========================================================================
    VMLAL.S16  q6, d1, d7[0]           ; for y2 y4 y6 y8

    ;==========================================================================
    ; Load clamping parameters to duplicate vector elements
    ;==========================================================================
    vdup.s16  q11,  d7[1]
    vdup.s16  q10,  d7[2]

    ;==========================================================================
    ; Right shift to eight bits and round
    ;==========================================================================
    VSHRN.S32   d3, q4, #8             ; store red to d3(q1)
    VSHRN.S32   d5, q5, #8             ; store green to d5(q2)
    VSHRN.S32   d25, q6, #8            ; store blue to d12(q12)

    ;==========================================================================
    ; Clamping the value to be within [0~255]
    ; if >255, =255
    ; if <0,   =0
    ;==========================================================================
    VMAX.s16  q1, q1, q11              ; if q1 <0, q1 =0
    VMIN.s16  q1, q1, q10              ; if q1 >255, q1 =255

    VQMOVUN.s16   d3, q1               ; store red to d3(q1)

    VMAX.s16  q2, q2, q11              ; if q2 <0, q2 =0
    VMIN.s16  q2, q2, q10              ; if q2 >255, q2 =255

    VQMOVUN.s16   d2, q2               ; store green to d2(q1)

    VMAX.s16  q12, q12, q11            ; if q12 <0, q12 =0
    VMIN.s16  q12, q12, q10            ; if q12 >255, q12 =255

    VQMOVUN.s16   d1, q12              ; store blue to d1(q0)

    ;==========================================================================
    ; aRGB format with starting 0xFF byte
    ;==========================================================================
    VMOVN.s16 d4, q10                  ; (q10) =0xFF, duplicate it to vector d4(q2)
    VST4.u8  {d1, d2,d3,d4}, [pDstRGB] ; vector store 0xFF,red, green, blue
                                       ; blue on low bytes

    ADD  pDstRGB, pDstRGB, #32


    ;==========================================================================
    ; End of Outerloop for nWidth overhead
    ;==========================================================================
    ADD  pSrcLuma, pSrcLuma, #8        ; update uint8 *pSrcLuma for 8 bytes
    ADD  pSrcChroma, pSrcChroma, #16   ; update uint8 *pSrcChroma for 8 bytes

    SUBS width, width, #8              ; update width counter with 8 bytes

    CMP width, #8                      ; if more than 8 pixels left,
    BGE loop_neon_Y1aRGB888            ; continue neon optimization

    CMP width, #0
    BLE loop_end_Y1aRGB888             ; if no more pixels left, go to the end


    ;==========================================================================
    ; If number of pixels left is less than 16, using arm instructions:
    ; For H2V2 and H2V1, y1 y2 share same Cb Cr
    ; Thus Calculating output using same Cb Cr with neighbor y1 and y2
    ;==========================================================================

loop_arm_Y1aRGB888

    ;==========================================================================
    ; Load input from y, then calculate y-16
    ;==========================================================================
    LDRB r4, [pSrcLuma]                ; load luma to r4
    SUB  r4, r4, #16                   ; luma-16

    ;==========================================================================
    ; Calculate 298*(y-16) + 128
    ;==========================================================================
    MOV  r5, #298                      ; reading coefficient to r5
    SMULBB r5, r4, r5
    ADD  r5, #128

    ;==========================================================================
    ; aRGB format with starting 0xFF byte
    ;==========================================================================
    MOV  r6, #0xFF                     ; store 0xFF
    STRB r6, [pDstRGB, #3]

    ;==========================================================================
    ; Load input from Cb, then calculate Cb-128
    ; Load input from Cr, then calculate Cr-128
    ;==========================================================================
    LDRB r7, [pSrcChroma, #1]          ; load Cb to r7
    SUB  r7, r7, #128                  ; Cb-128

    LDRB r8, [pSrcChroma]              ; load Cr to r8
    SUB  r8, r8, #128                  ; Cr-128

    ;==========================================================================
    ; Red:
    ; Calculate 409*(Cr-128), then plus 298*(y-16) + 128
    ;==========================================================================
    MOV  r4, #409
    SMLABB  r4, r8, r4, r5

    ;==========================================================================
    ; Right shift 8 bits, Then Clamping to [0, 255]
    ;==========================================================================
    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF                     ; if r4 >255, then r4 =255
    MOVGT   r4, #0xFF

    STRB  r4, [pDstRGB, #2]            ; store red
                                       ; red on high bytes

    ;==========================================================================
    ; Blue:
    ; Calculate 516*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    MOV r4, #516
    SMLABB  r4, r7, r4, r5             ; Calculate 516*(Cb-128),  plus 298*(y-16)+128

    ASRS  r4, #8
    MOVMI   r4, #0                     ; if r4 <0, then r4 =0
    CMP  r4, #0xFF                     ; if r4 >255, then r4 =255
    MOVGT   r4, #0xFF

    STRB  r4, [pDstRGB]                ; store blue
                                       ; blue on low bytes

    ;==========================================================================
    ; Green:
    ; Calculate -100*(Cr-128)-208*(Cb-128), then plus 298*(y-16) + 128
    ; Next to shift 8 bits, then Clamping to [0, 255]
    ;==========================================================================
    ; Green
    MOV  r4, #-100
    SMLABB  r4, r7, r4, r5             ; Calculate -100*(Cr-128) plus 298*(y-16) + 128

    MOV  r5, #-208
    SMLABB  r4, r8, r5, r4

    ASRS  r4, #8                       ; Shifting
    MOVMI r4, #0                       ; if r4 <0, then r4 =0
    CMP   r4, #255                     ; if r4 >255, then r4 =255
    MOVGT r4, #255

    STRB  r4, [pDstRGB, #1]            ; store green

    ;==========================================================================
    ; Update Luma, Chroma and output RGB pointer
    ;==========================================================================
    ADD  pDstRGB, pDstRGB, #4          ; update uint8 *pDstRGB with 4 bytes
    ADD  pSrcLuma,pSrcLuma, #1         ; update uint8 *pSrcLuma with 1 bytes
    ADD  pSrcChroma, pSrcChroma, #2    ; update uint8 *pSrcChroma with 2 bytes

    SUBS width, width, #1              ; update width counter with 1 bytes

    BGT loop_arm_Y1aRGB888             ; if no more pixels left, go to the end
    BLE loop_end_Y1aRGB888             ; otherwise, continue


loop_end_Y1aRGB888
    LDMFD sp!, {r4-r8, pc}

constants_Y1aRGB888
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    ENDFUNC                            ; end of y1vu2rgbaline


    AREA  RODataArea, DATA, READONLY, ALIGN=5

constants
    DCW (COEF_CB_RED), (COEF_CR_GREEN), (COEF_CB_GREEN),(COEF_CR_BLUE)
    DCW (COEF_Y_SCALE), (COEFF_0), (COEFF_255) , (COEFF_F8)
    DCD (COEFF_BIAS_R), (COEFF_BIAS_G), (COEFF_BIAS_B)

    END
