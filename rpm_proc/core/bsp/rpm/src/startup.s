;*****************************************************************************
; * @file:    startup.s
; * @purpose: Startup File for Bear RPM
;
;            Copyright (c) 2014 Qualcomm Technologies Incorporated.
;                   All Rights Reserved.
;             QUALCOMM Proprietary and Confidential
; *****************************************************************************

                IMPORT  ||Image$$INIT_DATA_RAM_STACK_END$$Base||

; Vector Table Mapped to Address 0 at Reset

;                AREA    RESET, DATA, READONLY
                AREA    RPM_ENTRY, DATA, READONLY
__Vectors       DCD     (||Image$$INIT_DATA_RAM_STACK_END$$Base|| - 4)
                                                  ; Initial Main SP
                DCD     Reset_Handler             ; Reset Handler


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler
                EXPORT  Reset_Handler
                IMPORT  __main
                LDR     R0, =__main
                BX      R0

                END

