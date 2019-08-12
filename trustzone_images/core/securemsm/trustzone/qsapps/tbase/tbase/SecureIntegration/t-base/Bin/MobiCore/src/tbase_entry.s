;======================================================================
;
;======================================================================
    EXPORT  __main
    EXPORT  _main
    EXPORT  MOBICOREENTRY

;======================================================================
    AREA  MOBICORE_IMAGE, CODE, READONLY, ALIGN=12

mobicoreImage
    INCBIN tbase.img

;======================================================================
    PRESERVE8
    
;======================================================================
    ; this is needed for the split image, otherwise no .b03 file is
    ; generated
    AREA    STRINGS, DATA, READWRITE
appName
    DCB "tbase       ",0
;======================================================================
    
    AREA    MOBICORE_WRAPPER, CODE, READONLY, ALIGN=12
    CODE32

__main
_main
    ENTRY

MOBICOREENTRY

    b    mobicoreImage
    ALIGN 0x1000


;======================================================================
    END
