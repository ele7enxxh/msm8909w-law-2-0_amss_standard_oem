;=========================================================================
;
;*//** @file jpege_engine_sw_quant_zigzag.s
;  This file contains the implementations for JPEG Encode Quant and
;  Zigzag in ARM assembly.
;
;@par EXTERNALIZED FUNCTIONS
;  jpege_engine_sw_quant_zigzag
;
;@par INITIALIZATION AND SEQUENCING REQUIREMENTS
;  (none)
;
;Copyright (C) 2009 Qualcomm Technologies, Inc.
;All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
;
;*//*=====================================================================

;=========================================================================
;                           Edit History
;
;$Header$
;
;when       who     what, where, why
;--------   ---     -------------------------------------------------------
;04/08/09   zhiminl Replaced zigzag table with zigzag offset table.
;
;=========================================================================

;-------------------------------------------------------------------------
;                           Private Constants
;-------------------------------------------------------------------------

BLOCK_SIZE_MINUS_1      EQU 63

; Right shift Count = 15+3 to eliminate 15 fractional bits from Quant O/P.
; Additional 3 for compensating scaling by 8 in DCT.
QUANT_FIX_POINT_FACTOR  EQU 18

;-------------------------------------------------------------------------
;                           EXPORTS
;-------------------------------------------------------------------------

    EXPORT jpege_engine_sw_quant_zigzag

;-------------------------------------------------------------------------
;                           Function Definitions
;-------------------------------------------------------------------------

    AREA |.text|, CODE, READONLY
    CODE32

;=========================================================================
;
; FUNCTION            : jpege_engine_sw_quant_zigzag
;
; DESCRIPTION         : Quantizes a block of 64 coefficients. Also does
;                       zigzag. So, no in-place computation allowed.
;
; C PROTOTYPE         : void jpege_engine_sw_quant_zigzag (
;                         const int16_t *quantInput,
;                               int16_t *zigzagOutput,
;                         const int16_t *zigzagOffsetTable,
;                         const int16_t *quantTable)
;
; REGISTER INPUTS     : R0: int16_t *quantInput
;                             pointer of the input 64 coefficients
;                       R1: int16_t *zigzagOutput
;                             pointer of the quantized and zigzaged
;                             64 coefficients
;                       R2: int16_t *zigzagOffsetTable
;                             pointer of the rotated zigzag offset table
;                       R3: int16_t *quantTable
;                             pointer of the quantization table
;
; STACK ARGUMENTS     : None
;
; REGISTER OUTPUTS    : None
;
; MEMORY INPUTS       : quantInput - to be quantized and zigzagged
;
; MEMORY OUTPUTS      : zigzagOutput - quantized and zigzagged
;
; REGISTERS AFFECTED  : R0-R9
;
; STACK USAGE         : None
;
; CYCLES              :
;
; NOTES               : None
;
;=========================================================================
jpege_engine_sw_quant_zigzag FUNCTION
quantInput                RN 0            ; pointer of the input 64 coefficients
zigzagOutput              RN 1            ; pointer of the output 64 coefficients
                                          ; in zigzag order
zigzagOffsetTable         RN 2            ; pointer of the zigzag table
quantTable                RN 3            ; pointer of the quantization table

fromQuantInput1           RN 4            ; quantInput[i]
fromQuantTable1           RN 5            ; quantTable[i]
offset1                   RN 6            ; zigzagOffsetTable[i]
quantized1                RN 6            ; quantized value for i
fromQuantInput2           RN 7            ; quantInput[i - 1]
fromQuantTable2           RN 8            ; quantTable[i - 1]
offset2                   RN 9            ; zigzagOffsetTable[i - 1]
quantized2                RN 9            ; quantized value for i - 1

index                     RN 12           ; scratch register

    STMFD   sp!, {r4-r9}                  ; start of jpege_engine_sw_quant_zigzag()

    MOV     index, #BLOCK_SIZE_MINUS_1
                                          ; index (r12) = BLOCK_SIZE_MINUS_1
    ADD     quantInput, quantInput, index, LSL #1
                                          ; quantInput (r0) += BLOCK_SIZE_MINUS_1
    ADD     quantTable, quantTable, index, LSL #1
                                          ; quantTable (r3) += BLOCK_SIZE_MINUS_1
    ADD     zigzagOffsetTable, zigzagOffsetTable, index, LSL #1
                                          ; zigzagOffsetTable (r2) += BLOCK_SIZE_MINUS_1
    ADD     zigzagOutput, zigzagOutput, index, LSL #1
                                          ; zigzagOutput (r1) += BLOCK_SIZE_MINUS_1

quant_zigzag_loop
    LDRSH   offset1, [zigzagOffsetTable], #-2
                                          ; offset1 (r6) = zigzagOffsetTable[i]
                                          ; zigzagOffsetTable-- (r2)
    LDRSH   offset2, [zigzagOffsetTable], #-2
                                          ; offset2 (r9) = zigzagOffsetTable[i - 1]
                                          ; zigzagOffsetTable-- (r2)

                                          ; the offset is in BYTE !!!
    LDRSH   fromQuantInput1, [quantInput], offset1
                                          ; fromQuantInput1 (r4) = *quantInput
                                          ; quantInput (r0) += offset
    LDRSH   fromQuantTable1, [quantTable], offset1
                                          ; fromQuantTable1 (r5) = *quantTable
                                          ; quantTable (r3) += offset

    LDRSH   fromQuantInput2, [quantInput], offset2
                                          ; fromQuantInput2 (r7) = *quantInput
                                          ; quantInput (r0) += offset
    LDRSH   fromQuantTable2, [quantTable], offset2
                                          ; fromQuantTable2 (r8) = *quantTable
                                          ; quantTable (r3) += offset2

    SMULBB  quantized1, fromQuantInput1, fromQuantTable1
                                          ; quantized1 (r6) = (*quantInput) (r4) * (*quantTable) (r5)

    SMULBB  quantized2, fromQuantInput2, fromQuantTable2
                                          ; quantized2 (r9) = (*quantInput) (r7) * (*quantTable) (r8)

    MOVS    quantized1, quantized1, ASR #QUANT_FIX_POINT_FACTOR
                                          ; C flag = quantized1[QUANT_FIX_POINT_FACTOR - 1]
                                          ; quantized1 (r6) >>= QUANT_FIX_POINT_FACTOR
    ADC     quantized1, quantized1, #0x0  ; if (C flag) quantized1++ (r6) to round

    MOVS    quantized2, quantized2, ASR #QUANT_FIX_POINT_FACTOR
                                          ; C flag = quantized2[QUANT_FIX_POINT_FACTOR - 1]
                                          ; quantized2 (r9) >>= QUANT_FIX_POINT_FACTOR
    ADC     quantized2, quantized2, #0x0  ; if (C flag) quantized2++ (r9) to round

    STRH    quantized1, [zigzagOutput], #-2
                                          ; zigzagOutput[i] = quantized1 (r6)
                                          ; zigzagOutput-- (r1)

    STRH    quantized2, [zigzagOutput], #-2
                                          ; zigzagOutput[i - 1] = quantized2 (r9)
                                          ; zigzagOutput-- (r1)

    SUBS    index, index, #2              ; while ((i-=2) !=0) (r12)
    BGE     quant_zigzag_loop

    LDMFD   sp!, {r4-r9}                  ; POP     {r4-r9}
    BX      lr
    ENDFUNC                               ; end of jpege_engine_sw_quant_zigzag()

    END                                   ; end of jpege_engine_sw_quant_zigzag.S
