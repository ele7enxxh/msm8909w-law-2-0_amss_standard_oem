;==========================================================================
;    jpegd_engine_sw_huff_extend.s
;
;
;
; Copyright (c) 1999-2002, 2008 Qualcomm Technologies, Inc.
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
; 12/01/08   staceyw     Merge to jpegd_engine_sw_huff_extend.s
; 11/12/08   staceyw     Create function jpegd_engine_sw_get_residue_huff_extend
; 10/22/08   staceyw     Initial implementation of jpegd_engine_sw_huff_extend,
;                        file added
;
;==========================================================================

;==========================================================================
;                         FUNCTION LIST
;--------------------------------------------------------------------------
; - jpegd_engine_sw_huff_extend
; - jpegd_engine_sw_get_residue_huff_extend
;
;==========================================================================


    AREA  ||.text||, CODE, READONLY
    CODE32

    EXPORT jpegd_engine_sw_huff_extend
    EXPORT jpegd_engine_sw_get_residue_huff_extend

;==========================================================================
; FUNCTION     : jpegd_engine_sw_huff_extend
;--------------------------------------------------------------------------
; DISCRIPTION  : ASM optimization of huffman sign extension
;--------------------------------------------------------------------------
; C PROTOTYPE  : int32 jpegd_engine_sw_huff_extend (uint32 nResidue, int32 nResidueSize);
;--------------------------------------------------------------------------
; REG INPUT    : R0 uint32 nResidue before extention
;                R1 int32  nResidueSize
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : None
;--------------------------------------------------------------------------
; REG OUTPUT   : R0 int32 category
;--------------------------------------------------------------------------
; MEM OUTPUT   : None
;--------------------------------------------------------------------------
; REG AFFECTED : R0-3
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : 2 for positive codeword
;                5 for negative codeword
;--------------------------------------------------------------------------
; NOTES        : called for case of (codeSize + nResidueSize) > 16 bits
;==========================================================================

jpegd_engine_sw_huff_extend  FUNCTION

    ;======================================================================
    ; First check if the input is a positive or negative.
    ; Do so by rotate nResidue right for (size) number of bits.
    ;
    ; The first bits of residue will set the carry register
    ; carry bit is set if R0 is positive
    ;======================================================================
    RORS     r3, r0, r1             ; Rotate R0 (nResidue) right for R1 (nResidueSize)
                                    ; number of bits and save in R3

    ;======================================================================
    ; The following 3 instructions are executed for negative nResidue only
    ; In case of negative nResidue, need to pad sign bits in front and add
    ; 1 to residue to revert the 2s complements operation done by the
    ; encode process
    ; 1. prepare sign extension
    ; 2. pad sign bits in front of nResidue
    ; 3. add 1 to nResidue
    ;======================================================================
    MOVCC    r2, #-1                ; Load R2 with (-1), get ready for sign
                                    ; extension

    ADDCC    r0, r2, LSL r1         ; Left shift R2 (-1) for R1 (nResidueSize)
                                    ; number of bits and add to R0 (nResidue),
                                    ; now R0 (nResidue) is padded with sign bits

    ADDCC    r0, #1                 ; Add 1 to R0 (nResidue);

    ;======================================================================
    ; Return category value in R0, and function return
    ; - if input R0 (nResidue) is positive, R0 is un-altered and returned
    ;   directly
    ; - if input R0 (nResidue) is negative, R0 is sign extended and
    ;   returned
    ;======================================================================
    BX       lr                     ; Return to link register

    ENDFUNC                         ; End of Function jpegd_engine_sw_huff_extend.s




;==========================================================================
; FUNCTION     : jpegd_engine_sw_get_residue_huff_extend
;--------------------------------------------------------------------------
; DISCRIPTION  : ASM optimization of getting residue,
;                32 bits buffer update and huffman sign extension
;--------------------------------------------------------------------------
; C PROTOTYPE  : int32 jpegd_engine_sw_get_residue_huff_extend (jpegdBitBufferStructT * pBufferPtr,
;                int32 nCodeSize, int32 nResidueSize);
;--------------------------------------------------------------------------
; REG INPUT    : R0 : jpegdBitBufferStructT * pBufferPtr
;                R1 : int32  nCodeSize
;                R2 : int32  nResidueSize
;                R3 : uint32 pBufferPtr->bitsLeft
;--------------------------------------------------------------------------
; STACK ARG    : None
;--------------------------------------------------------------------------
; MEM INPUT    : pBufferPtr->buffer
;--------------------------------------------------------------------------
; REG OUTPUT   : R0 int32 category after extension
;--------------------------------------------------------------------------
; MEM OUTPUT   : pBufferPtr->buffer
;--------------------------------------------------------------------------
; REG AFFECTED : R0-3
;--------------------------------------------------------------------------
; STACK USAGE  : none
;--------------------------------------------------------------------------
; CYCLES       : 8 for positive codeword
;                11 for negative codeword
;--------------------------------------------------------------------------
; NOTES        : called for case of (codeSize + nResidueSize) <= 16 bits
;==========================================================================

jpegd_engine_sw_get_residue_huff_extend  FUNCTION

    ;======================================================================
    ; R0 : address of input pBufferPtr structure which has
    ;      first is  uint32 buffer
    ;      second is int32  bitsLeft
    ; First load the address of buffer to R3
    ;
    ; R0 : pBufferPtr
    ; R3 : pBufferPtr->buffer
    ;======================================================================
    LDR r3, [r0, #0]                ; Load the [R0] of pBufferPtr->buffer to R3

    ;======================================================================
    ; R1 is the input of nCodeSize
    ; R3 is pBufferPtr->buffer
    ; Left shift buffer with length of nCodeSize to update the buffer
    ;
    ; R3 : pBufferPtr->buffer << nCodeSize
    ;======================================================================
    LSL r3, r1                      ; shift R3 left for R1 number of bits

    ;======================================================================
    ; R2 is the input of nResidueSize
    ; R3 is pBufferPtr->buffer
    ; Left shift buffer with length of nResidueSize to update the buffer
    ;
    ; R1 : pBufferPtr->buffer << nResidueSize
    ; Then store the content of R1 back to R0 which is pBufferPtr->buffer
    ;======================================================================
    LSL r1,  r3, r2                 ; shift R3 left for R2 number of bits
                                    ; and save in R1

    STR r1, [r0, #0]                ; store R1 in [R0]
                                    ; which is updating pBufferPtr->buffer

    ;======================================================================
    ; R2 is the input of nResidueSize
    ; R0 is euqal to 32 - nResidueSize
    ; Right shift buffer with length of 32- nResidueSize to get the residue
    ; Then R0 is the residue
    ;
    ; R0 : pBufferPtr->buffer >> nResidueSize
    ;=====================================================================
    RSB r0, r2, #32                 ; (32) minus r2 and save it in R0

    LSR r0, r3,  r0                 ; shift R3 right for R0 number of bits
                                    ; and save in R0

    ;======================================================================
    ; Now R0 is the input of residue
    ; Need to huffman extend it based on the value of residue
    ;=====================================================================

    ;======================================================================
    ; First check if the residue is a positive or negative.
    ; Do so by rotate residue right for (size) number of bits.
    ;
    ; The first bits of residue will set the carry register
    ; carry bit is set if R0 is positive
    ;======================================================================
    RORS     r3, r0, r2             ; Rotate R0 (r) right for R2 (s) number
                                    ; of bits and save in R3

    ;======================================================================
    ; The following 3 instructions are executed for negative residue only
    ; In case of negative residue, need to pad sign bits in front and add
    ; 1 to residue to revert the 2s complements operation done by the
    ; encode process
    ; 1. prepare sign extension
    ; 2. pad sign bits in front of residue
    ; 3. add 1 to residue
    ;======================================================================
    MOVCC    r3, #-1                ; Load R3 with (-1), get ready for sign
                                    ; extension

    ADDCC    r0, r3, LSL r2         ; Left shift R3 (-1) for R2 (s) number
                                    ; of bits and add to R0 (r), now R0 (r)
                                    ; is padded with sign bits

    ADDCC    r0, #1                 ; Add 1 to R0 (r);

    ;======================================================================
    ; Return category value in R0, and function return
    ; - if input R0 (residue) is positive, R0 is un-altered and returned
    ;   directly
    ; - if input R0 (residue) is negative, R0 is sign extended and
    ;   returned
    ;======================================================================
    BX       lr                     ; Return to link register

    ENDFUNC                         ; End of Function jpegd_engine_sw_get_residue_huff_extend.s




    ;======================================================================
    ;End of File CJpegDecodeEngineHuffExtend.s
    ;======================================================================
    END
