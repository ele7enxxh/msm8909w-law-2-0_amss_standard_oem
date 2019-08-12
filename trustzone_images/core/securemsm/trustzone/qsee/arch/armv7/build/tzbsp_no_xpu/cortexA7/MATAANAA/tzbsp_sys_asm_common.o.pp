#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/tzbsp_sys_asm_common.s"
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      S Y S
;
;
; GENERAL DESCRIPTION
;   Convenience functions for armv7.
;
; Copyright (c) 2014 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
; 02/18/14   sg      Initial version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    GET tzbsp_asm_common.inc

    EXPORT tzbsp_cur_cpu_num
    EXPORT tzbsp_cur_cluster_num
    EXPORT tzbsp_cur_cpu_num_in_cluster
    IMPORT tzbsp_cpu_count_per_cluster

    CODE32
    PRESERVE8

    AREA TZBSP_SYS_ASM_COMMON, align=4, CODE, READONLY

; uint32 tzbsp_cur_cluster_num(void)
tzbsp_cur_cluster_num FUNCTION
    GetCurrentClusterNum
    bx      lr
    ENDFUNC

; uint32 tzbsp_cur_cpu_num_in_cluster(void)
tzbsp_cur_cpu_num_in_cluster FUNCTION
    GetCurrentCpuNumInCluster
    bx      lr
    ENDFUNC

; uint32 tzbsp_cur_cpu_num(void)
tzbsp_cur_cpu_num FUNCTION
    GetCurrentCpuNum
    bx      lr
    ENDFUNC

    END
