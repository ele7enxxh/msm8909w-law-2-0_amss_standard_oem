#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsapps/libs/applib/qsee/src/tzapp_entry.s"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_version.h"





 













 






#line 2 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsapps/libs/applib/qsee/src/tzapp_entry.s"

        EXPORT  __main
        EXPORT  _main
        EXPORT TZAPPENTRY
        EXPORT read_app_rw_base
        EXPORT set_rw_base

        IMPORT  app_export_init_info_to_qsee
        IMPORT  tzlib_app_entry
        IMPORT  get_stack_base
        IMPORT  get_stack_size
        IMPORT  get_app_entry
        IMPORT  get_app_name

    PRESERVE8
    AREA    TZAPPENTRYCODE, CODE, READONLY
    CODE32

__main
_main
        ENTRY

TZAPPENTRY

    cmp  r0, #02
    bne  version_check_failed
    cmp  r1, #01
    bne  version_check_failed
    b    version_check_passed
version_check_failed
    mov  r0, #0xFF
    blx app_export_init_info_to_qsee
version_check_passed
    blx  get_stack_size
    mov  r2, r0
    blx  get_stack_base
    mov  r3, r0
    blx  get_app_name
    mov  r4, r0
    blx  get_app_entry
    mov  r1, r0
    mov  r0, #0
    blx app_export_init_info_to_qsee

read_app_rw_base
    push  {r4,lr}
    mov   r0, r9            ; Return RW base
    pop   {r4,pc}           ; Return.

set_rw_base
    push  {r4,lr}
    mov   r9, r0            ; set RW base
    pop   {r4,pc}           ; Return.

    END


