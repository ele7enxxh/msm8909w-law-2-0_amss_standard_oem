;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      Q D S S
;
;
; GENERAL DESCRIPTION
;   This file implements the save and restore of the Qualcomm Debug
;   Sub System registers.
;
; Copyright (c) 2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/src/tzbsp_qdss.s#1 $
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 01/12/12   pre     Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "tzbsp_target.h"
   GET qctps_common_macros.inc

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    EXPORT  tzbsp_restore_debug_regs
    EXPORT  tzbsp_save_debug_regs
    EXPORT  tzbsp_clear_pc_debug_locks

    EXPORT  CPU0_DBGDTRRXext
    EXPORT  CPU0_DBGDTRTXext
    EXPORT  CPU0_DBGDSCRext
    EXPORT  CPU0_DBGWFAR
    EXPORT  CPU0_DBGVCR
    EXPORT  CPU0_DBGCLAIMCLR
    EXPORT  CPU0_DBGBVR00
    EXPORT  CPU0_DBGBCR00
    EXPORT  CPU0_DBGBVR01
    EXPORT  CPU0_DBGBCR01
    EXPORT  CPU0_DBGBVR02
    EXPORT  CPU0_DBGBCR02
    EXPORT  CPU0_DBGBVR03
    EXPORT  CPU0_DBGBCR03
    EXPORT  CPU0_DBGBVR04
    EXPORT  CPU0_DBGBCR04
    EXPORT  CPU0_DBGBVR05
    EXPORT  CPU0_DBGBCR05
    EXPORT  CPU0_DBGBVR06
    EXPORT  CPU0_DBGBCR06
    EXPORT  CPU0_DBGBVR07
    EXPORT  CPU0_DBGBCR07
    EXPORT  CPU0_DBGBVR08
    EXPORT  CPU0_DBGBCR08
    EXPORT  CPU0_DBGBVR09
    EXPORT  CPU0_DBGBCR09
    EXPORT  CPU0_DBGBVR10
    EXPORT  CPU0_DBGBCR10
    EXPORT  CPU0_DBGBVR11
    EXPORT  CPU0_DBGBCR11
    EXPORT  CPU0_DBGBVR12
    EXPORT  CPU0_DBGBCR12
    EXPORT  CPU0_DBGBVR13
    EXPORT  CPU0_DBGBCR13
    EXPORT  CPU0_DBGBVR14
    EXPORT  CPU0_DBGBCR14
    EXPORT  CPU0_DBGBVR15
    EXPORT  CPU0_DBGBCR15
    EXPORT  CPU0_DBGWVR00
    EXPORT  CPU0_DBGWCR00
    EXPORT  CPU0_DBGWVR01
    EXPORT  CPU0_DBGWCR01
    EXPORT  CPU0_DBGWVR02
    EXPORT  CPU0_DBGWCR02
    EXPORT  CPU0_DBGWVR03
    EXPORT  CPU0_DBGWCR03
    EXPORT  CPU0_DBGWVR04
    EXPORT  CPU0_DBGWCR04
    EXPORT  CPU0_DBGWVR05
    EXPORT  CPU0_DBGWCR05
    EXPORT  CPU0_DBGWVR06
    EXPORT  CPU0_DBGWCR06
    EXPORT  CPU0_DBGWVR07
    EXPORT  CPU0_DBGWCR07
    EXPORT  CPU0_DBGWVR08
    EXPORT  CPU0_DBGWCR08
    EXPORT  CPU0_DBGWVR09
    EXPORT  CPU0_DBGWCR09
    EXPORT  CPU0_DBGWVR10
    EXPORT  CPU0_DBGWCR10
    EXPORT  CPU0_DBGWVR11
    EXPORT  CPU0_DBGWCR11
    EXPORT  CPU0_DBGWVR12
    EXPORT  CPU0_DBGWCR12
    EXPORT  CPU0_DBGWVR13
    EXPORT  CPU0_DBGWCR13
    EXPORT  CPU0_DBGWVR14
    EXPORT  CPU0_DBGWCR14
    EXPORT  CPU0_DBGWVR15
    EXPORT  CPU0_DBGWCR15

#if (TZBSP_CPU_COUNT_MAX >= 2)
    EXPORT  CPU1_DBGDTRRXext
    EXPORT  CPU1_DBGDTRTXext
    EXPORT  CPU1_DBGDSCRext
    EXPORT  CPU1_DBGWFAR
    EXPORT  CPU1_DBGVCR
    EXPORT  CPU1_DBGCLAIMCLR
    EXPORT  CPU1_DBGBVR00
    EXPORT  CPU1_DBGBCR00
    EXPORT  CPU1_DBGBVR01
    EXPORT  CPU1_DBGBCR01
    EXPORT  CPU1_DBGBVR02
    EXPORT  CPU1_DBGBCR02
    EXPORT  CPU1_DBGBVR03
    EXPORT  CPU1_DBGBCR03
    EXPORT  CPU1_DBGBVR04
    EXPORT  CPU1_DBGBCR04
    EXPORT  CPU1_DBGBVR05
    EXPORT  CPU1_DBGBCR05
    EXPORT  CPU1_DBGBVR06
    EXPORT  CPU1_DBGBCR06
    EXPORT  CPU1_DBGBVR07
    EXPORT  CPU1_DBGBCR07
    EXPORT  CPU1_DBGBVR08
    EXPORT  CPU1_DBGBCR08
    EXPORT  CPU1_DBGBVR09
    EXPORT  CPU1_DBGBCR09
    EXPORT  CPU1_DBGBVR10
    EXPORT  CPU1_DBGBCR10
    EXPORT  CPU1_DBGBVR11
    EXPORT  CPU1_DBGBCR11
    EXPORT  CPU1_DBGBVR12
    EXPORT  CPU1_DBGBCR12
    EXPORT  CPU1_DBGBVR13
    EXPORT  CPU1_DBGBCR13
    EXPORT  CPU1_DBGBVR14
    EXPORT  CPU1_DBGBCR14
    EXPORT  CPU1_DBGBVR15
    EXPORT  CPU1_DBGBCR15
    EXPORT  CPU1_DBGWVR00
    EXPORT  CPU1_DBGWCR00
    EXPORT  CPU1_DBGWVR01
    EXPORT  CPU1_DBGWCR01
    EXPORT  CPU1_DBGWVR02
    EXPORT  CPU1_DBGWCR02
    EXPORT  CPU1_DBGWVR03
    EXPORT  CPU1_DBGWCR03
    EXPORT  CPU1_DBGWVR04
    EXPORT  CPU1_DBGWCR04
    EXPORT  CPU1_DBGWVR05
    EXPORT  CPU1_DBGWCR05
    EXPORT  CPU1_DBGWVR06
    EXPORT  CPU1_DBGWCR06
    EXPORT  CPU1_DBGWVR07
    EXPORT  CPU1_DBGWCR07
    EXPORT  CPU1_DBGWVR08
    EXPORT  CPU1_DBGWCR08
    EXPORT  CPU1_DBGWVR09
    EXPORT  CPU1_DBGWCR09
    EXPORT  CPU1_DBGWVR10
    EXPORT  CPU1_DBGWCR10
    EXPORT  CPU1_DBGWVR11
    EXPORT  CPU1_DBGWCR11
    EXPORT  CPU1_DBGWVR12
    EXPORT  CPU1_DBGWCR12
    EXPORT  CPU1_DBGWVR13
    EXPORT  CPU1_DBGWCR13
    EXPORT  CPU1_DBGWVR14
    EXPORT  CPU1_DBGWCR14
    EXPORT  CPU1_DBGWVR15
    EXPORT  CPU1_DBGWCR15
#endif
#if (TZBSP_CPU_COUNT_MAX >= 3)
    EXPORT  CPU2_DBGDTRRXext
    EXPORT  CPU2_DBGDTRTXext
    EXPORT  CPU2_DBGDSCRext
    EXPORT  CPU2_DBGWFAR
    EXPORT  CPU2_DBGVCR
    EXPORT  CPU2_DBGCLAIMCLR
    EXPORT  CPU2_DBGBVR00
    EXPORT  CPU2_DBGBCR00
    EXPORT  CPU2_DBGBVR01
    EXPORT  CPU2_DBGBCR01
    EXPORT  CPU2_DBGBVR02
    EXPORT  CPU2_DBGBCR02
    EXPORT  CPU2_DBGBVR03
    EXPORT  CPU2_DBGBCR03
    EXPORT  CPU2_DBGBVR04
    EXPORT  CPU2_DBGBCR04
    EXPORT  CPU2_DBGBVR05
    EXPORT  CPU2_DBGBCR05
    EXPORT  CPU2_DBGBVR06
    EXPORT  CPU2_DBGBCR06
    EXPORT  CPU2_DBGBVR07
    EXPORT  CPU2_DBGBCR07
    EXPORT  CPU2_DBGBVR08
    EXPORT  CPU2_DBGBCR08
    EXPORT  CPU2_DBGBVR09
    EXPORT  CPU2_DBGBCR09
    EXPORT  CPU2_DBGBVR10
    EXPORT  CPU2_DBGBCR10
    EXPORT  CPU2_DBGBVR11
    EXPORT  CPU2_DBGBCR11
    EXPORT  CPU2_DBGBVR12
    EXPORT  CPU2_DBGBCR12
    EXPORT  CPU2_DBGBVR13
    EXPORT  CPU2_DBGBCR13
    EXPORT  CPU2_DBGBVR14
    EXPORT  CPU2_DBGBCR14
    EXPORT  CPU2_DBGBVR15
    EXPORT  CPU2_DBGBCR15
    EXPORT  CPU2_DBGWVR00
    EXPORT  CPU2_DBGWCR00
    EXPORT  CPU2_DBGWVR01
    EXPORT  CPU2_DBGWCR01
    EXPORT  CPU2_DBGWVR02
    EXPORT  CPU2_DBGWCR02
    EXPORT  CPU2_DBGWVR03
    EXPORT  CPU2_DBGWCR03
    EXPORT  CPU2_DBGWVR04
    EXPORT  CPU2_DBGWCR04
    EXPORT  CPU2_DBGWVR05
    EXPORT  CPU2_DBGWCR05
    EXPORT  CPU2_DBGWVR06
    EXPORT  CPU2_DBGWCR06
    EXPORT  CPU2_DBGWVR07
    EXPORT  CPU2_DBGWCR07
    EXPORT  CPU2_DBGWVR08
    EXPORT  CPU2_DBGWCR08
    EXPORT  CPU2_DBGWVR09
    EXPORT  CPU2_DBGWCR09
    EXPORT  CPU2_DBGWVR10
    EXPORT  CPU2_DBGWCR10
    EXPORT  CPU2_DBGWVR11
    EXPORT  CPU2_DBGWCR11
    EXPORT  CPU2_DBGWVR12
    EXPORT  CPU2_DBGWCR12
    EXPORT  CPU2_DBGWVR13
    EXPORT  CPU2_DBGWCR13
    EXPORT  CPU2_DBGWVR14
    EXPORT  CPU2_DBGWCR14
    EXPORT  CPU2_DBGWVR15
    EXPORT  CPU2_DBGWCR15
#endif
#if (TZBSP_CPU_COUNT_MAX >= 4)
    EXPORT  CPU3_DBGDTRRXext
    EXPORT  CPU3_DBGDTRTXext
    EXPORT  CPU3_DBGDSCRext
    EXPORT  CPU3_DBGWFAR
    EXPORT  CPU3_DBGVCR
    EXPORT  CPU3_DBGCLAIMCLR
    EXPORT  CPU3_DBGBVR00
    EXPORT  CPU3_DBGBCR00
    EXPORT  CPU3_DBGBVR01
    EXPORT  CPU3_DBGBCR01
    EXPORT  CPU3_DBGBVR02
    EXPORT  CPU3_DBGBCR02
    EXPORT  CPU3_DBGBVR03
    EXPORT  CPU3_DBGBCR03
    EXPORT  CPU3_DBGBVR04
    EXPORT  CPU3_DBGBCR04
    EXPORT  CPU3_DBGBVR05
    EXPORT  CPU3_DBGBCR05
    EXPORT  CPU3_DBGBVR06
    EXPORT  CPU3_DBGBCR06
    EXPORT  CPU3_DBGBVR07
    EXPORT  CPU3_DBGBCR07
    EXPORT  CPU3_DBGBVR08
    EXPORT  CPU3_DBGBCR08
    EXPORT  CPU3_DBGBVR09
    EXPORT  CPU3_DBGBCR09
    EXPORT  CPU3_DBGBVR10
    EXPORT  CPU3_DBGBCR10
    EXPORT  CPU3_DBGBVR11
    EXPORT  CPU3_DBGBCR11
    EXPORT  CPU3_DBGBVR12
    EXPORT  CPU3_DBGBCR12
    EXPORT  CPU3_DBGBVR13
    EXPORT  CPU3_DBGBCR13
    EXPORT  CPU3_DBGBVR14
    EXPORT  CPU3_DBGBCR14
    EXPORT  CPU3_DBGBVR15
    EXPORT  CPU3_DBGBCR15
    EXPORT  CPU3_DBGWVR00
    EXPORT  CPU3_DBGWCR00
    EXPORT  CPU3_DBGWVR01
    EXPORT  CPU3_DBGWCR01
    EXPORT  CPU3_DBGWVR02
    EXPORT  CPU3_DBGWCR02
    EXPORT  CPU3_DBGWVR03
    EXPORT  CPU3_DBGWCR03
    EXPORT  CPU3_DBGWVR04
    EXPORT  CPU3_DBGWCR04
    EXPORT  CPU3_DBGWVR05
    EXPORT  CPU3_DBGWCR05
    EXPORT  CPU3_DBGWVR06
    EXPORT  CPU3_DBGWCR06
    EXPORT  CPU3_DBGWVR07
    EXPORT  CPU3_DBGWCR07
    EXPORT  CPU3_DBGWVR08
    EXPORT  CPU3_DBGWCR08
    EXPORT  CPU3_DBGWVR09
    EXPORT  CPU3_DBGWCR09
    EXPORT  CPU3_DBGWVR10
    EXPORT  CPU3_DBGWCR10
    EXPORT  CPU3_DBGWVR11
    EXPORT  CPU3_DBGWCR11
    EXPORT  CPU3_DBGWVR12
    EXPORT  CPU3_DBGWCR12
    EXPORT  CPU3_DBGWVR13
    EXPORT  CPU3_DBGWCR13
    EXPORT  CPU3_DBGWVR14
    EXPORT  CPU3_DBGWCR14
    EXPORT  CPU3_DBGWVR15
    EXPORT  CPU3_DBGWCR15
#endif
#if (TZBSP_CPU_COUNT_MAX >= 5)
    EXPORT  CPU4_DBGDTRRXext   
    EXPORT  CPU4_DBGDTRTXext   
    EXPORT  CPU4_DBGDSCRext    
    EXPORT  CPU4_DBGWFAR       
    EXPORT  CPU4_DBGVCR        
    EXPORT  CPU4_DBGCLAIMCLR   
    EXPORT  CPU4_DBGBVR00      
    EXPORT  CPU4_DBGBCR00      
    EXPORT  CPU4_DBGBVR01      
    EXPORT  CPU4_DBGBCR01      
    EXPORT  CPU4_DBGBVR02      
    EXPORT  CPU4_DBGBCR02      
    EXPORT  CPU4_DBGBVR03      
    EXPORT  CPU4_DBGBCR03      
    EXPORT  CPU4_DBGBVR04      
    EXPORT  CPU4_DBGBCR04      
    EXPORT  CPU4_DBGBVR05      
    EXPORT  CPU4_DBGBCR05      
    EXPORT  CPU4_DBGBVR06      
    EXPORT  CPU4_DBGBCR06      
    EXPORT  CPU4_DBGBVR07      
    EXPORT  CPU4_DBGBCR07      
    EXPORT  CPU4_DBGBVR08      
    EXPORT  CPU4_DBGBCR08      
    EXPORT  CPU4_DBGBVR09      
    EXPORT  CPU4_DBGBCR09      
    EXPORT  CPU4_DBGBVR10      
    EXPORT  CPU4_DBGBCR10      
    EXPORT  CPU4_DBGBVR11      
    EXPORT  CPU4_DBGBCR11      
    EXPORT  CPU4_DBGBVR12      
    EXPORT  CPU4_DBGBCR12      
    EXPORT  CPU4_DBGBVR13      
    EXPORT  CPU4_DBGBCR13      
    EXPORT  CPU4_DBGBVR14      
    EXPORT  CPU4_DBGBCR14      
    EXPORT  CPU4_DBGBVR15      
    EXPORT  CPU4_DBGBCR15      
    EXPORT  CPU4_DBGWVR00      
    EXPORT  CPU4_DBGWCR00      
    EXPORT  CPU4_DBGWVR01      
    EXPORT  CPU4_DBGWCR01      
    EXPORT  CPU4_DBGWVR02      
    EXPORT  CPU4_DBGWCR02      
    EXPORT  CPU4_DBGWVR03      
    EXPORT  CPU4_DBGWCR03      
    EXPORT  CPU4_DBGWVR04      
    EXPORT  CPU4_DBGWCR04      
    EXPORT  CPU4_DBGWVR05      
    EXPORT  CPU4_DBGWCR05      
    EXPORT  CPU4_DBGWVR06      
    EXPORT  CPU4_DBGWCR06      
    EXPORT  CPU4_DBGWVR07      
    EXPORT  CPU4_DBGWCR07      
    EXPORT  CPU4_DBGWVR08      
    EXPORT  CPU4_DBGWCR08      
    EXPORT  CPU4_DBGWVR09      
    EXPORT  CPU4_DBGWCR09      
    EXPORT  CPU4_DBGWVR10      
    EXPORT  CPU4_DBGWCR10      
    EXPORT  CPU4_DBGWVR11      
    EXPORT  CPU4_DBGWCR11      
    EXPORT  CPU4_DBGWVR12      
    EXPORT  CPU4_DBGWCR12      
    EXPORT  CPU4_DBGWVR13      
    EXPORT  CPU4_DBGWCR13      
    EXPORT  CPU4_DBGWVR14      
    EXPORT  CPU4_DBGWCR14      
    EXPORT  CPU4_DBGWVR15      
    EXPORT  CPU4_DBGWCR15      
#endif
#if (TZBSP_CPU_COUNT_MAX >= 6)
    EXPORT  CPU5_DBGDTRRXext   
    EXPORT  CPU5_DBGDTRTXext   
    EXPORT  CPU5_DBGDSCRext    
    EXPORT  CPU5_DBGWFAR       
    EXPORT  CPU5_DBGVCR        
    EXPORT  CPU5_DBGCLAIMCLR   
    EXPORT  CPU5_DBGBVR00      
    EXPORT  CPU5_DBGBCR00      
    EXPORT  CPU5_DBGBVR01      
    EXPORT  CPU5_DBGBCR01      
    EXPORT  CPU5_DBGBVR02      
    EXPORT  CPU5_DBGBCR02      
    EXPORT  CPU5_DBGBVR03      
    EXPORT  CPU5_DBGBCR03      
    EXPORT  CPU5_DBGBVR04      
    EXPORT  CPU5_DBGBCR04      
    EXPORT  CPU5_DBGBVR05      
    EXPORT  CPU5_DBGBCR05      
    EXPORT  CPU5_DBGBVR06      
    EXPORT  CPU5_DBGBCR06      
    EXPORT  CPU5_DBGBVR07      
    EXPORT  CPU5_DBGBCR07      
    EXPORT  CPU5_DBGBVR08      
    EXPORT  CPU5_DBGBCR08      
    EXPORT  CPU5_DBGBVR09      
    EXPORT  CPU5_DBGBCR09      
    EXPORT  CPU5_DBGBVR10      
    EXPORT  CPU5_DBGBCR10      
    EXPORT  CPU5_DBGBVR11      
    EXPORT  CPU5_DBGBCR11      
    EXPORT  CPU5_DBGBVR12      
    EXPORT  CPU5_DBGBCR12      
    EXPORT  CPU5_DBGBVR13      
    EXPORT  CPU5_DBGBCR13      
    EXPORT  CPU5_DBGBVR14      
    EXPORT  CPU5_DBGBCR14      
    EXPORT  CPU5_DBGBVR15      
    EXPORT  CPU5_DBGBCR15      
    EXPORT  CPU5_DBGWVR00      
    EXPORT  CPU5_DBGWCR00      
    EXPORT  CPU5_DBGWVR01      
    EXPORT  CPU5_DBGWCR01      
    EXPORT  CPU5_DBGWVR02      
    EXPORT  CPU5_DBGWCR02      
    EXPORT  CPU5_DBGWVR03      
    EXPORT  CPU5_DBGWCR03      
    EXPORT  CPU5_DBGWVR04      
    EXPORT  CPU5_DBGWCR04      
    EXPORT  CPU5_DBGWVR05      
    EXPORT  CPU5_DBGWCR05      
    EXPORT  CPU5_DBGWVR06      
    EXPORT  CPU5_DBGWCR06      
    EXPORT  CPU5_DBGWVR07      
    EXPORT  CPU5_DBGWCR07      
    EXPORT  CPU5_DBGWVR08      
    EXPORT  CPU5_DBGWCR08      
    EXPORT  CPU5_DBGWVR09      
    EXPORT  CPU5_DBGWCR09      
    EXPORT  CPU5_DBGWVR10      
    EXPORT  CPU5_DBGWCR10      
    EXPORT  CPU5_DBGWVR11      
    EXPORT  CPU5_DBGWCR11      
    EXPORT  CPU5_DBGWVR12      
    EXPORT  CPU5_DBGWCR12      
    EXPORT  CPU5_DBGWVR13      
    EXPORT  CPU5_DBGWCR13      
    EXPORT  CPU5_DBGWVR14      
    EXPORT  CPU5_DBGWCR14      
    EXPORT  CPU5_DBGWVR15      
    EXPORT  CPU5_DBGWCR15      
#endif
#if (TZBSP_CPU_COUNT_MAX >= 7)
    EXPORT  CPU6_DBGDTRRXext   
    EXPORT  CPU6_DBGDTRTXext   
    EXPORT  CPU6_DBGDSCRext    
    EXPORT  CPU6_DBGWFAR       
    EXPORT  CPU6_DBGVCR        
    EXPORT  CPU6_DBGCLAIMCLR   
    EXPORT  CPU6_DBGBVR00      
    EXPORT  CPU6_DBGBCR00      
    EXPORT  CPU6_DBGBVR01      
    EXPORT  CPU6_DBGBCR01      
    EXPORT  CPU6_DBGBVR02      
    EXPORT  CPU6_DBGBCR02      
    EXPORT  CPU6_DBGBVR03      
    EXPORT  CPU6_DBGBCR03      
    EXPORT  CPU6_DBGBVR04      
    EXPORT  CPU6_DBGBCR04      
    EXPORT  CPU6_DBGBVR05      
    EXPORT  CPU6_DBGBCR05      
    EXPORT  CPU6_DBGBVR06      
    EXPORT  CPU6_DBGBCR06      
    EXPORT  CPU6_DBGBVR07      
    EXPORT  CPU6_DBGBCR07      
    EXPORT  CPU6_DBGBVR08      
    EXPORT  CPU6_DBGBCR08      
    EXPORT  CPU6_DBGBVR09      
    EXPORT  CPU6_DBGBCR09      
    EXPORT  CPU6_DBGBVR10      
    EXPORT  CPU6_DBGBCR10      
    EXPORT  CPU6_DBGBVR11      
    EXPORT  CPU6_DBGBCR11      
    EXPORT  CPU6_DBGBVR12      
    EXPORT  CPU6_DBGBCR12      
    EXPORT  CPU6_DBGBVR13      
    EXPORT  CPU6_DBGBCR13      
    EXPORT  CPU6_DBGBVR14      
    EXPORT  CPU6_DBGBCR14      
    EXPORT  CPU6_DBGBVR15      
    EXPORT  CPU6_DBGBCR15      
    EXPORT  CPU6_DBGWVR00      
    EXPORT  CPU6_DBGWCR00      
    EXPORT  CPU6_DBGWVR01      
    EXPORT  CPU6_DBGWCR01      
    EXPORT  CPU6_DBGWVR02      
    EXPORT  CPU6_DBGWCR02      
    EXPORT  CPU6_DBGWVR03      
    EXPORT  CPU6_DBGWCR03      
    EXPORT  CPU6_DBGWVR04      
    EXPORT  CPU6_DBGWCR04      
    EXPORT  CPU6_DBGWVR05      
    EXPORT  CPU6_DBGWCR05      
    EXPORT  CPU6_DBGWVR06      
    EXPORT  CPU6_DBGWCR06      
    EXPORT  CPU6_DBGWVR07      
    EXPORT  CPU6_DBGWCR07      
    EXPORT  CPU6_DBGWVR08      
    EXPORT  CPU6_DBGWCR08      
    EXPORT  CPU6_DBGWVR09      
    EXPORT  CPU6_DBGWCR09      
    EXPORT  CPU6_DBGWVR10      
    EXPORT  CPU6_DBGWCR10      
    EXPORT  CPU6_DBGWVR11      
    EXPORT  CPU6_DBGWCR11      
    EXPORT  CPU6_DBGWVR12      
    EXPORT  CPU6_DBGWCR12      
    EXPORT  CPU6_DBGWVR13      
    EXPORT  CPU6_DBGWCR13      
    EXPORT  CPU6_DBGWVR14      
    EXPORT  CPU6_DBGWCR14      
    EXPORT  CPU6_DBGWVR15      
    EXPORT  CPU6_DBGWCR15      
#endif
#if (TZBSP_CPU_COUNT_MAX >= 8)
    EXPORT CPU7_DBGDTRRXext    
    EXPORT CPU7_DBGDTRTXext    
    EXPORT CPU7_DBGDSCRext     
    EXPORT CPU7_DBGWFAR        
    EXPORT CPU7_DBGVCR         
    EXPORT CPU7_DBGCLAIMCLR    
    EXPORT CPU7_DBGBVR00       
    EXPORT CPU7_DBGBCR00       
    EXPORT CPU7_DBGBVR01       
    EXPORT CPU7_DBGBCR01       
    EXPORT CPU7_DBGBVR02       
    EXPORT CPU7_DBGBCR02       
    EXPORT CPU7_DBGBVR03       
    EXPORT CPU7_DBGBCR03       
    EXPORT CPU7_DBGBVR04       
    EXPORT CPU7_DBGBCR04       
    EXPORT CPU7_DBGBVR05       
    EXPORT CPU7_DBGBCR05       
    EXPORT CPU7_DBGBVR06       
    EXPORT CPU7_DBGBCR06       
    EXPORT CPU7_DBGBVR07       
    EXPORT CPU7_DBGBCR07       
    EXPORT CPU7_DBGBVR08       
    EXPORT CPU7_DBGBCR08       
    EXPORT CPU7_DBGBVR09       
    EXPORT CPU7_DBGBCR09       
    EXPORT CPU7_DBGBVR10       
    EXPORT CPU7_DBGBCR10       
    EXPORT CPU7_DBGBVR11       
    EXPORT CPU7_DBGBCR11       
    EXPORT CPU7_DBGBVR12       
    EXPORT CPU7_DBGBCR12       
    EXPORT CPU7_DBGBVR13       
    EXPORT CPU7_DBGBCR13       
    EXPORT CPU7_DBGBVR14       
    EXPORT CPU7_DBGBCR14       
    EXPORT CPU7_DBGBVR15       
    EXPORT CPU7_DBGBCR15       
    EXPORT CPU7_DBGWVR00       
    EXPORT CPU7_DBGWCR00       
    EXPORT CPU7_DBGWVR01       
    EXPORT CPU7_DBGWCR01       
    EXPORT CPU7_DBGWVR02       
    EXPORT CPU7_DBGWCR02       
    EXPORT CPU7_DBGWVR03       
    EXPORT CPU7_DBGWCR03       
    EXPORT CPU7_DBGWVR04       
    EXPORT CPU7_DBGWCR04       
    EXPORT CPU7_DBGWVR05       
    EXPORT CPU7_DBGWCR05       
    EXPORT CPU7_DBGWVR06       
    EXPORT CPU7_DBGWCR06       
    EXPORT CPU7_DBGWVR07       
    EXPORT CPU7_DBGWCR07       
    EXPORT CPU7_DBGWVR08       
    EXPORT CPU7_DBGWCR08       
    EXPORT CPU7_DBGWVR09       
    EXPORT CPU7_DBGWCR09       
    EXPORT CPU7_DBGWVR10       
    EXPORT CPU7_DBGWCR10       
    EXPORT CPU7_DBGWVR11       
    EXPORT CPU7_DBGWCR11       
    EXPORT CPU7_DBGWVR12       
    EXPORT CPU7_DBGWCR12       
    EXPORT CPU7_DBGWVR13       
    EXPORT CPU7_DBGWCR13       
    EXPORT CPU7_DBGWVR14       
    EXPORT CPU7_DBGWCR14       
    EXPORT CPU7_DBGWVR15       
    EXPORT CPU7_DBGWCR15       
#endif    

    IMPORT  tzbsp_has_cpu_cold_booted

; ---------------------
; MACRO: ReadRegisterR
; ---------------------
; Reads a given register where the address is in a register
  MACRO
  ReadRegisterR $addr_r, $val_r, $work_r
#ifdef TZBSP_WITH_TBASE
    movw    $work_r, #(TZBSP_SANDBOX_END & 0xFFFF)
    movt    $work_r, #((TZBSP_SANDBOX_END >> 16) & 0xFFFF)
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    movw    $work_r, #(TZBSP_SANDBOX_RELOCATE_OFFSET & 0xFFFF)
    movt    $work_r, #((TZBSP_SANDBOX_RELOCATE_OFFSET >> 16) & 0xFFFF)

10  ldr     $val_r, [$addr_r, $work_r]
#else
   ldr     $val_r, [$addr_r, $work_r]
#endif
  MEND


;============================================================================
;
;                             MODULE CODE
;
;============================================================================
    PRESERVE8
    CODE32
    AREA    TZBSP_QDSS_CODE, CODE, READONLY, align=5


;===================================================================
; FUNCTION - void tzbsp_restore_debug_regs(void)
;===================================================================
; On entry, R0 points to a block of saved debug registers in.
; Returns the pointer beyond the block and corrupts R1-R3,R12
;
tzbsp_restore_debug_regs FUNCTION
    push    {r0, lr}

    mrc     p15, 0, r1, c0, c0, 5   ; Current MPIDR
    and     r1, r1, #0xFF           ; r1 = cpu_num

    ; Restore debug registers needed for JTAG connection
    ; Check to see if debugger is attached
    ldr     r0, =DebugLogicEnabled  ; Load the current debugger status
    ldr     r0, [r0]                ; from bitfield according to cpu
    mov     r2, #1
    lsl     r2, r2, r1
    ands    r0, r2
    beq     restore_debug_end       ; 0 means no debugger

    ; Restore debug registers for warm boot only
    cmp     r1, #0                  ; r1 = CPU number
    beq     restore_debug           ; Always restore for CPU0
    ldr     r0, =tzbsp_has_cpu_cold_booted
    ldr     r0, [r0, r1, LSL #2]    ; has CPU N cold booted?
    cmp     r0, #0                  ; Only restore for CPU n if not
                                    ;  coldboot
    beq     restore_debug_end

restore_debug

    ; Restore Debug regs first because they it will disable OS double
    ; lock
    mov     r2, #KRAIT_DEBUG_REG_SZ ; r2 = sizeof debug regs for 1 krait
    mul     r2, r1                  ; r2 = offset of debug regs for cpu
    ldr     r0, =DebugRegisters
    add     r0, r2                  ; Shift pointer by CPU offset
    bl      RestoreDebugRegisters

    ; Turn on ETM clocks for restoring ETM registers
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    orr     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to enable ETM clock

    mrc     p15, 0, r1, c0, c0, 5   ; Current MPIDR
    and     r1, r1, #0xFF           ; r1 = cpu_num
    mov     r2, #KRAIT_TRACE_REG_SZ ; r2 = sizeof trace regs for 1 krait
    mul     r2, r1                  ; r2 = offset of trace regs for cpu
    ldr     r0, =TraceRegisters
    add     r0, r2                  ; Shift pointer by CPU offset
    bl      RestoreTraceRegisters

    ; Turn off ETM clocks after restoring ETM registers
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    bic     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to disable ETM clock


restore_debug_end

    pop     {r0, pc}
    ENDFUNC


;===================================================================
; FUNCTION - void tzbsp_save_debug_regs(void)
;===================================================================
; Determines if debug registers need to be saved, then calls save
; routines
;
tzbsp_save_debug_regs FUNCTION
    push    {r0, lr}

    ; Save the debug registers for CPU before PC if debugger is attached
    movw    r0, #0x0FB8             ; low 16
    movt    r0, #0x01A1             ; high 16
                                    ; r0 = base debugger logic register

    mrc     p15, 0, r1, c0, c0, 5   ; Current MPIDR
    and     r1, r1, #0xFF           ; r1 = cpu_num
    mov     r2, #0x2000             ; r2 = 0x2000
    mul     r2, r1                  ; 0x2000 is per-CPU offset for reg
    add     r0, r2                  ; r0 = debugger logic reg for cur cpu

    ReadRegisterR r0, r0, r2        ; Read register into r0
    ands    r0, #0x1                ; Is debug logic enabled for CPU?
    mov     r0, #0x1                ; r0 = 1
    ldr     r2, =DebugLogicEnabled
    ldr     r2, [r2]                ; r2 = DebugLogicEnabled
    lsl     r0, r1                  ; r0 = 1 << cpu_num

    ; Clear CPU status in DebugLogicEnabled bitmask
    biceq   r2, r0                  ; (eq) r2= DebugLogicEnabled & ~(1<<cpu_num)
    ldreq   r0, =DebugLogicEnabled  ; (eq) r0= &DebugLogicEnabled
    streq   r2, [r0]
    beq     save_debug_end

    ; Set CPU status in DebugLogicEnabled bitmask
    orr     r2, r0                  ; r2 = DebugLogicEnabled | (1 << cpu_num)
    ldr     r0, =DebugLogicEnabled  ; r0 = &DebugLogicEnabled
    str     r2, [r0]

    ; Turn on ETM clocks for saving ETM registers
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    orr     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to enable ETM clock


    ; Call SaveTraceRegisters with pointer to data for CPU
    ldr     r0, =TraceRegisters     ; r0 = &TraceRegisters
    mov     r2, #KRAIT_TRACE_REG_SZ ; r2 = sizeof trace regs for 1 krait
    mul     r2, r1                  ; r2 = offset of trace regs for cpu
    add     r0, r2                  ; Shift pointer by CPU offset
    bl      SaveTraceRegisters

    ; Turn off ETM clocks after saving ETM registers
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    bic     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to disable ETM clock

    ; Call SaveDebugRegisters with pointer to data for CPU
    ; Do this last since it sets the "os double lock"
    mrc     p15, 0, r1, c0, c0, 5   ; Current MPIDR
    and     r1, r1, #0xFF           ; r1 = cpu_num
    ldr     r0, =DebugRegisters     ; r0 = &DebugRegisters
    mov     r2, #KRAIT_DEBUG_REG_SZ ; r2 = sizeof debug regs for 1 krait
    mul     r2, r1                  ; r2 = offset of debug regs for cpu
    add     r0, r2                  ; Shift pointer by CPU offset
    bl      SaveDebugRegisters

save_debug_end
    pop     {r0, pc}
    ENDFUNC

;===================================================================
; FUNCTION - void tzbsp_clear_pc_debug_locks
;===================================================================
; Clear any debug register locks that are grabbed prior to power
; collapse that need to be freed if PC does not terminate due to
; pending interrupts.
;
tzbsp_clear_pc_debug_locks FUNCTION
    ; Clear the debug locks since PC did not happen
    mov     r0, #0x0000
    ISB

    mcr     p14,0,r0,c1,c3,4        ; Write DBGOSDLR which clears
    ISB                             ;  the OS double lock

    mcr     p14,0,r0,c1,c0,4        ; Write DBGOSLAR
    ISB

    ; Turn on ETM clocks for clearing lock
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    orr     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to enable ETM clock

    WCP14_ETMOSLAR r0               ; Write ETMOSLAR which clears the OS lock
    ISB

    ; Turn off ETM clocks after clearing lock
    mrc     p15, 7, r0, c15, c0, 5  ; Read CPMR
    bic     r0, r0, #8              ; ETMCLKEN
    mcr     p15, 7, r0, c15, c0, 5  ; Write CPMR to disable ETM clock

    bx      lr
    ENDFUNC


; ===================================================================
; FUNCTION - void SaveDebugRegisters(uint32 debug_mem)
;===================================================================
;  On entry, R0 points to a block to save the debug registers in.
;  Returns the pointer beyond the block and corrupts R1-R3
;
SaveDebugRegisters FUNCTION
; (1) Set OS Lock via DBGOSLAR.
; This informs the debugger that the OS is in the process of saving
; debug registers.  It also prevents accidental modification of the
; debug regs by the debugger.  Execute an ISB to make sure the setting
; of the OS lock is visible architecturally.
    ldr     r1, =0xC5ACCE55
    mcr     p14,0,r1,c1,c0,4        ; Write DBGOSLAR which sets the OS lock
    ISB                             ; Synchronization barrier

; (2) Walk through the registers, saving them
    mrc     p14,0,r1,c0,c0,2        ; Read DBGDTRRXext
    mrc     p14,0,r2,c0,c3,2        ; Read DBGDTRTXext
    mrc     p14,0,r3,c0,c2,2        ; Read DBGDSCRext
    stmia   r0!,{r1-r3}             ; Save { DTRRXext, DTRTXext, DSCRext }
    mrc     p14,0,r1,c0,c6,0        ; Read DBGWFAR
    mrc     p14,0,r2,c0,c7,0        ; Read DBGVCR
    mrc     p14,0,r3,c7,c9,6        ; Read CLAIM ? note, have to read via
                                    ;  DBGCLAIMCLR
    stmia   r0!,{r1-r3}             ; Save { WFAR, VCR, CLAIM }

; (3) Breakpoints
    mrc     p14,0,r1,c0,c0,0        ; Read DBGDIDR
    lsr     r1,r1,#24
    and     r1,r1,#0xf              ; Extract BRPs field

    mrc     p14,0,r2,c0,c0,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c0,5        ; Read DBGxCRn
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCRn }

    mrc     p14,0,r2,c0,c1,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c1,5        ; Read DBGxCRn
    cmp     r1,#1
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(14*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c2,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c2,5        ; Read DBGxCRn
    cmp     r1,#2
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(13*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c3,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c3,5        ; Read DBGxCRn
    cmp     r1,#3
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(12*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c4,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c4,5        ; Read DBGxCRn
    cmp     r1,#4
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(11*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c5,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c5,5        ; Read DBGxCRn
    cmp     r1,#5
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(10*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c6,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c6,5        ; Read DBGxCRn
    cmp     r1,#6
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(9*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c7,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c7,5        ; Read DBGxCRn
    cmp     r1,#7
    stmia   r0!,{r2-r3}             ; Save { ;xVRn,xCrm }
    addeq   r0, #(8*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c8,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c8,5        ; Read DBGxCRn
    cmp     r1,#8
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(7*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c9,4        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c9,5        ; Read DBGxCRn
    cmp     r1,#9
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(6*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c10,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c10,5       ; Read DBGxCRn
    cmp     r1,#10
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(5*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c11,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c11,5       ; Read DBGxCRn
    cmp     r1,#11
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(4*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c12,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c12,5       ; Read DBGxCRn
    cmp     r1,#12
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(3*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c13,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c13,5       ; Read DBGxCRn
    cmp     r1,#13
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(2*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c14,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c14,5       ; Read DBGxCRn
    cmp     r1,#14
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    addeq   r0, #(1*8)
    beq     SaveDebugRegisters_Watchpoints

    mrc     p14,0,r2,c0,c15,4       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c15,5       ; Read DBGxCRn
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCRn }

SaveDebugRegisters_Watchpoints
; (4) Watchpoints
    mrc     p14,0,r1,c0,c0,0        ; Read DBGDIDR
    lsr     r1,r1,#28               ; Extract WRPs field

                                    ; SaveRP 0,0,15,6,7,SaveDebugRegisters_Exit
    mrc     p14,0,r2,c0,c0,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c0,7        ; Read DBGxCRn
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCRn }

                                    ; SaveRP 1,0,15,6,7,SaveDebugRegisters_Exit
    mrc     p14,0,r2,c0,c1,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c1,7        ; Read DBGxCRn
    cmp     r1,#1
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c2,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c2,7        ; Read DBGxCRn
    cmp     r1,#2
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c3,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c3,7        ; Read DBGxCRn
    cmp     r1,#3
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c4,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c4,7        ; Read DBGxCRn
    cmp     r1,#4
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c5,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c5,7        ; Read DBGxCRn
    cmp     r1,#5
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c6,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c6,7        ; Read DBGxCRn
    cmp     r1,#6
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c7,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c7,7        ; Read DBGxCRn
    cmp     r1,#7
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c8,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c8,7        ; Read DBGxCRn
    cmp     r1,#8
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c9,6        ; Read DBGxVRn
    mrc     p14,0,r3,c0,c9,7        ; Read DBGxCRn
    cmp     r1,#9
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c10,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c10,7       ; Read DBGxCRn
    cmp     r1,#10
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c11,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c11,7       ; Read DBGxCRn
    cmp     r1,#11
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c12,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c12,7       ; Read DBGxCRn
    cmp     r1,#12
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c13,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c13,7       ; Read DBGxCRn
    cmp     r1,#13
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c14,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c14,7       ; Read DBGxCRn
    cmp     r1,#14
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCrm }
    beq     SaveDebugRegisters_Exit

    mrc     p14,0,r2,c0,c15,6       ; Read DBGxVRn
    mrc     p14,0,r3,c0,c15,7       ; Read DBGxCRn
    stmia   r0!,{r2-r3}             ; Save { xVRn,xCRn }

SaveDebugRegisters_Exit
; (5) Execute ISB before and after executing the double lock
    ISB
    mov     r1, #0x0001
    mcr     p14,0,r1,c1,c3,4        ; Write DBGOSDLR which sets the
                                    ;  OS double lock
    ISB                             ; Synchronization barrier

; (6) Return the pointer to first word not read. This pointer is
; already in R0, so all that is needed is to return from this
; function.
    bx      lr
    ENDFUNC

;===================================================================
; FUNCTION - void RestoreDebugRegisters(uint32 debugmem)
;===================================================================
; On entry, R0 points to a block of saved debug registers in.
; Returns the pointer beyond the block and corrupts R1-R3,R12
;
RestoreDebugRegisters FUNCTION
; (0) Execute ISB before and after clearing the double lock
    ISB
    mov     r1, #0x0000
    mcr     p14,0,r1,c1,c3,4        ; Write DBGOSDLR which clears
                                    ;  the OS double lock
    ISB                             ; Synchronization barrier

; (1) Set OS Lock via DBGOSLAR. The lock will already be set, but this
; write included to ensure it is set.
    ldr     r1,=0xC5ACCE55
    mcr     p14,0,r1,c1,c0,4        ; Write DBGOSLAR
    ISB                             ; Synchronization barrier

; (2) Walk through the registers, restoring them
    ldmfd   r0!,{r1-r3}             ; Read { DTRRXext,DTRTXext DSCRext }
    mcr     p14,0,r1,c0,c0,2        ; Restore DBGDTRRXext
    mcr     p14,0,r2,c0,c3,2        ; Restore DBGDTRTXext
    mcr     p14,0,r3,c0,c2,2        ; Restore DBGDSCRext
    ldmfd   r0!,{r1-r3}             ; Read { WFAR,VCR,CLAIM }
    mcr     p14,0,r1,c0,c6,0        ; Restore DBGWFAR
    mcr     p14,0,r2,c0,c7,0        ; Restore DBGVCR
    mcr     p14,0,r3,c7,c8,6        ; Restore CLAIM tags; note:
                                    ;   writes  DBGCLAIMSET

; (3) Breakpoints
    mrc     p14,0,r1,c0,c0,0        ; Read DBGDIDR
    lsr     r1,r1,#24
    and     r1,r1,#0xf              ; Extract BRPs field

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c0,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c0,5        ; Restore DBGxCRn

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c1,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c1,5        ; Restore DBGxCRn
    cmp     r1,#1
    addeq   r0, #(14*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c2,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c2,5        ; Restore DBGxCRn
    cmp     r1,#2
    addeq   r0, #(13*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c3,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c3,5        ; Restore DBGxCRn
    cmp     r1,#3
    addeq   r0, #(12*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c4,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c4,5        ; Restore DBGxCRn
    cmp     r1,#4
    addeq   r0, #(11*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c5,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c5,5        ; Restore DBGxCRn
    cmp     r1,#5
    addeq   r0, #(10*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c6,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c6,5        ; Restore DBGxCRn
    cmp     r1,#6
    addeq   r0, #(9*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c7,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c7,5        ; Restore DBGxCRn
    cmp     r1,#7
    addeq   r0, #(8*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c8,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c8,5        ; Restore DBGxCRn
    cmp     r1,#8
    addeq   r0, #(7*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c9,4        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c9,5        ; Restore DBGxCRn
    cmp     r1,#9
    addeq   r0, #(6*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c10,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c10,5       ; Restore DBGxCRn
    cmp     r1,#10
    addeq   r0, #(5*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c11,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c11,5       ; Restore DBGxCRn
    cmp     r1,#11
    addeq   r0, #(4*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c12,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c12,5       ; Restore DBGxCRn
    cmp     r1,#12
    addeq   r0, #(3*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c13,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c13,5       ; Restore DBGxCRn
    cmp     r1,#13
    addeq   r0, #(2*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c14,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c14,5       ; Restore DBGxCRn
    cmp     r1,#14
    addeq   r0, #(1*8)
    beq     RestoreDebugRegisters_Watchpoints

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c15,4       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c15,5       ; Restore DBGxCRn

RestoreDebugRegisters_Watchpoints
; (4) Watchpoints
    mrc     p14,0,r1,c0,c0,0        ; Read DBGDIDR
    lsr     r1,r1,#28               ; Extract WRPs field

; RestoreRP 0,0,15,6,7,RestoreDebugRegisters_Exit
    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c0,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c0,7        ; Restore DBGxCRn

; RestoreRP 1,0,15,6,7,RestoreDebugRegisters_Exit
    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c1,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c1,7        ; Restore DBGxCRn
    cmp     r1,#1
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c2,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c2,7        ; Restore DBGxCRn
    cmp     r1,#2
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c3,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c3,7        ; Restore DBGxCRn
    cmp     r1,#3
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c4,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c4,7        ; Restore DBGxCRn
    cmp     r1,#4
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c5,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c5,7        ; Restore DBGxCRn
    cmp     r1,#5
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c6,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c6,7        ; Restore DBGxCRn
    cmp     r1,#6
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c7,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c7,7        ; Restore DBGxCRn
    cmp     r1,#7
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c8,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c8,7        ; Restore DBGxCRn
    cmp     r1,#8
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c9,6        ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c9,7        ; Restore DBGxCRn
    cmp     r1,#9
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c10,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c10,7       ; Restore DBGxCRn
    cmp     r1,#10
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c11,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c11,7       ; Restore DBGxCRn
    cmp     r1,#11
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c12,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c12,7       ; Restore DBGxCRn
    cmp     r1,#12
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c13,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c13,7       ; Restore DBGxCRn
    cmp     r1,#13
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c14,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c14,7       ; Restore DBGxCRn
    cmp     r1,#14
    beq     RestoreDebugRegisters_Exit

    ldmfd   r0!,{r2-r3}             ; Read { xVRn,xCRn }
    mcr     p14,0,r2,c0,c15,6       ; Restore DBGxVRn
    mcr     p14,0,r3,c0,c15,7       ; Restore DBGxCRn

; (4.5) Clear any status error bits
    ISB
    mrc     p14,0,r4,c1,c5,4        ; clear DBGPRSR[STICKYPD]
    mrc     p14,1,r4,c1,c5,4        ; clear ETMPDSR[STICKYPD]

RestoreDebugRegisters_Exit
; (5) Clear the OS Lock. Writing any non-key value clears the lock, so
; use the value in R1.
    ISB
    mcr     p14,0,r1,c1,c0,4        ; Write DBGOSLAR

; (6) A final ISB guarantees the restored register values are visible
; to subsequent instructions.
    ISB

; (7) Return the pointer to first word not read. This pointer is
; already in R0, so all that is needed is to return from this
; function.
    bx      lr
    ENDFUNC


;-------------------------------------------------------------------------------
;
;  Function: SaveTraceRegisters
;
; PTM register list for save/restore derived from "CoreSight Program
; Flow Trace", "PFTv1.0 and PFTv1.1", "Architecture Specification",
; ARM IHI 0035B (ID041411).  The list of PTM registers is located on
; page 3-137, section 3.17.4 "Guidelines for the PTM trace registers
; to be saved and restored".
;
; On entry, R0 points to a block to save the trace registers in.
; Returns the pointer beyond the block and corrupts R1-R3

SaveTraceRegisters FUNCTION
; (1) Set OS Lock via ETMOSLAR.
; This informs the debugger that the OS is in the process of saving
; trace registers.  It also prevents accidental modification of the
; trace regs by the debugger.  Execute an ISB to make sure the setting
; of the OS lock is visible architecturally.
    ldr     r1, =0xC5ACCE55
    WCP14_ETMOSLAR r1               ; Write ETMOSLAR which sets the OS lock
    ISB                             ; Synchronization barrier

; (2) Walk through the registers, saving them
    RCP14_ETMCR r1
    RCP14_ETMTRIGGER r2
    RCP14_ETMSR r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMTSSCR r1
    RCP14_ETMTEEVR r2
    RCP14_ETMTECR1 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMFFLR r1
    RCP14_ETMACVR1 r2
    RCP14_ETMACVR2 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMACVR3 r1
    RCP14_ETMACVR4 r2
    RCP14_ETMACVR5 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMACVR6 r1
    RCP14_ETMACVR7 r2
    RCP14_ETMACVR8 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMACTR1 r1
    RCP14_ETMACTR2 r2
    RCP14_ETMACTR3 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMACTR4 r1
    RCP14_ETMACTR5 r2
    RCP14_ETMACTR6 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMACTR7 r1
    RCP14_ETMACTR8 r2
    RCP14_ETMCNTRLDVR1 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMCNTRLDVR2 r1
    RCP14_ETMCNTENR1 r2
    RCP14_ETMCNTENR2 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMCNTRLDEVR1 r1
    RCP14_ETMCNTRLDEVR2 r2
    RCP14_ETMCNTVR1 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMCNTVR2 r1
    RCP14_ETMSQ12EVR r2
    RCP14_ETMSQ21EVR r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMSQ23EVR r1
    RCP14_ETMSQ31EVR r2
    RCP14_ETMSQ32EVR r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMSQ13EVR r1
    RCP14_ETMSQR r2
    RCP14_ETMEXTOUTEVR1 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMEXTOUTEVR2 r1
    RCP14_ETMEXTOUTEVR3 r2
    RCP14_ETMEXTOUTEVR4 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMCIDCVR1 r1
    RCP14_ETMCIDCVR2 r2
    RCP14_ETMCIDCVR3 r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMCIDCMR r1
    RCP14_ETMISR0 r2
    RCP14_ETMSYNCFR r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMEXTINSELR r1
    RCP14_ETMTSEVR r2
    RCP14_ETMAUXCR r3
    stmia   r0!,{r1-r3}             ; Save { , , }

    RCP14_ETMTRACEIDR r1
    RCP14_ETMVMIDCVR r2
    RCP14_ETMCLAIMCLR r3            ; Tag value can be read from CLR, TAG
                                    ;  cannot provide current TAG value
    stmia   r0!,{r1-r3}             ; Save { , , }

; (3) Return the pointer to first word not read. This pointer is
; already in R0, so all that is needed is to return from this
; function.
    bx      lr
    ENDFUNC

;-------------------------------------------------------------------------------
;
;  Function:  RestoreTraceRegisters
;
; PTM register list for save/restore derived from "CoreSight Program
; Flow Trace", "PFTv1.0 and PFTv1.1", "Architecture Specification",
; ARM IHI 0035B (ID041411).  The list of PTM registers is located on
; page 3-137, section 3.17.4 "Guidelines for the PTM trace registers
; to be saved and restored".
;
; On entry, R0 points to a block of saved trace registers in.
; Returns the pointer beyond the block and corrupts R1-R3,R12

RestoreTraceRegisters
; (1) Set OS Lock via ETMOSLAR.
; This informs the debugger that the OS is in the process of restoring
; trace registers.  It also prevents accidental modification of the
; trace regs by the debugger.  Execute an ISB to make sure the setting
; of the OS lock is visible architecturally.
    ldr     r1, =0xC5ACCE55
    WCP14_ETMOSLAR r1               ; Write ETMOSLAR which sets the OS lock
    ISB                             ; Synchronization barrier

; (2) Walk through the registers, restoring them
    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCR r1
    WCP14_ETMTRIGGER r2
    WCP14_ETMSR r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMTSSCR r1
    WCP14_ETMTEEVR r2
    WCP14_ETMTECR1 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMFFLR r1
    WCP14_ETMACVR1 r2
    WCP14_ETMACVR2 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMACVR3 r1
    WCP14_ETMACVR4 r2
    WCP14_ETMACVR5 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMACVR6 r1
    WCP14_ETMACVR7 r2
    WCP14_ETMACVR8 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMACTR1 r1
    WCP14_ETMACTR2 r2
    WCP14_ETMACTR3 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMACTR4 r1
    WCP14_ETMACTR5 r2
    WCP14_ETMACTR6 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMACTR7 r1
    WCP14_ETMACTR8 r2
    WCP14_ETMCNTRLDVR1 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCNTRLDVR2 r1
    WCP14_ETMCNTENR1 r2
    WCP14_ETMCNTENR2 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCNTRLDEVR1 r1
    WCP14_ETMCNTRLDEVR2 r2
    WCP14_ETMCNTVR1 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCNTVR2 r1
    WCP14_ETMSQ12EVR r2
    WCP14_ETMSQ21EVR r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMSQ23EVR r1
    WCP14_ETMSQ31EVR r2
    WCP14_ETMSQ32EVR r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMSQ13EVR r1
    WCP14_ETMSQR r2
    WCP14_ETMEXTOUTEVR1 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMEXTOUTEVR2 r1
    WCP14_ETMEXTOUTEVR3 r2
    WCP14_ETMEXTOUTEVR4 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCIDCVR1 r1
    WCP14_ETMCIDCVR2 r2
    WCP14_ETMCIDCVR3 r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMCIDCMR r1
    WCP14_ETMISR0 r2
    WCP14_ETMSYNCFR r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMEXTINSELR r1
    WCP14_ETMTSEVR r2
    WCP14_ETMAUXCR r3

    ldmia   r0!,{r1-r3}             ; Read { , , }
    WCP14_ETMTRACEIDR r1
    WCP14_ETMVMIDCVR r2
    WCP14_ETMCLAIMSET r3            ; Restore SET using data read from TAG
                                    ; Position in pushed memory
                                    ; matches TAG location

; (3) Execute ISB before and after executing the lock
    ISB
    mov     r1, #0x0001
    WCP14_ETMOSLAR r1               ; Write ETMOSLAR which sets the OS lock
    ISB                             ; Synchronization barrier

; (4) Return the pointer to first word not read. This pointer is
; already in R0, so all that is needed is to return from this
; function.
    bx      lr

    AREA TZBSP_UNCACHED_ZI_QDSS_AREA, DATA, READWRITE, ALIGN=2
DebugLogicEnabled    SPACE 0x4
; PTM registers
TraceRegisters       SPACE KRAIT_TRACE_REG_SZ  * TZBSP_CPU_COUNT_MAX
; Debug registers saved before power collapse
DebugRegisters
CPU0_DBGDTRRXext     DCD 0
CPU0_DBGDTRTXext     DCD 0
CPU0_DBGDSCRext      DCD 0
CPU0_DBGWFAR         DCD 0
CPU0_DBGVCR          DCD 0
CPU0_DBGCLAIMCLR     DCD 0
CPU0_DBGBVR00        DCD 0
CPU0_DBGBCR00        DCD 0
CPU0_DBGBVR01        DCD 0
CPU0_DBGBCR01        DCD 0
CPU0_DBGBVR02        DCD 0
CPU0_DBGBCR02        DCD 0
CPU0_DBGBVR03        DCD 0
CPU0_DBGBCR03        DCD 0
CPU0_DBGBVR04        DCD 0
CPU0_DBGBCR04        DCD 0
CPU0_DBGBVR05        DCD 0
CPU0_DBGBCR05        DCD 0
CPU0_DBGBVR06        DCD 0
CPU0_DBGBCR06        DCD 0
CPU0_DBGBVR07        DCD 0
CPU0_DBGBCR07        DCD 0
CPU0_DBGBVR08        DCD 0
CPU0_DBGBCR08        DCD 0
CPU0_DBGBVR09        DCD 0
CPU0_DBGBCR09        DCD 0
CPU0_DBGBVR10        DCD 0
CPU0_DBGBCR10        DCD 0
CPU0_DBGBVR11        DCD 0
CPU0_DBGBCR11        DCD 0
CPU0_DBGBVR12        DCD 0
CPU0_DBGBCR12        DCD 0
CPU0_DBGBVR13        DCD 0
CPU0_DBGBCR13        DCD 0
CPU0_DBGBVR14        DCD 0
CPU0_DBGBCR14        DCD 0
CPU0_DBGBVR15        DCD 0
CPU0_DBGBCR15        DCD 0
CPU0_DBGWVR00        DCD 0
CPU0_DBGWCR00        DCD 0
CPU0_DBGWVR01        DCD 0
CPU0_DBGWCR01        DCD 0
CPU0_DBGWVR02        DCD 0
CPU0_DBGWCR02        DCD 0
CPU0_DBGWVR03        DCD 0
CPU0_DBGWCR03        DCD 0
CPU0_DBGWVR04        DCD 0
CPU0_DBGWCR04        DCD 0
CPU0_DBGWVR05        DCD 0
CPU0_DBGWCR05        DCD 0
CPU0_DBGWVR06        DCD 0
CPU0_DBGWCR06        DCD 0
CPU0_DBGWVR07        DCD 0
CPU0_DBGWCR07        DCD 0
CPU0_DBGWVR08        DCD 0
CPU0_DBGWCR08        DCD 0
CPU0_DBGWVR09        DCD 0
CPU0_DBGWCR09        DCD 0
CPU0_DBGWVR10        DCD 0
CPU0_DBGWCR10        DCD 0
CPU0_DBGWVR11        DCD 0
CPU0_DBGWCR11        DCD 0
CPU0_DBGWVR12        DCD 0
CPU0_DBGWCR12        DCD 0
CPU0_DBGWVR13        DCD 0
CPU0_DBGWCR13        DCD 0
CPU0_DBGWVR14        DCD 0
CPU0_DBGWCR14        DCD 0
CPU0_DBGWVR15        DCD 0
CPU0_DBGWCR15        DCD 0

#if (TZBSP_CPU_COUNT_MAX >= 2)
CPU1_DBGDTRRXext     DCD 0
CPU1_DBGDTRTXext     DCD 0
CPU1_DBGDSCRext      DCD 0
CPU1_DBGWFAR         DCD 0
CPU1_DBGVCR          DCD 0
CPU1_DBGCLAIMCLR     DCD 0
CPU1_DBGBVR00        DCD 0
CPU1_DBGBCR00        DCD 0
CPU1_DBGBVR01        DCD 0
CPU1_DBGBCR01        DCD 0
CPU1_DBGBVR02        DCD 0
CPU1_DBGBCR02        DCD 0
CPU1_DBGBVR03        DCD 0
CPU1_DBGBCR03        DCD 0
CPU1_DBGBVR04        DCD 0
CPU1_DBGBCR04        DCD 0
CPU1_DBGBVR05        DCD 0
CPU1_DBGBCR05        DCD 0
CPU1_DBGBVR06        DCD 0
CPU1_DBGBCR06        DCD 0
CPU1_DBGBVR07        DCD 0
CPU1_DBGBCR07        DCD 0
CPU1_DBGBVR08        DCD 0
CPU1_DBGBCR08        DCD 0
CPU1_DBGBVR09        DCD 0
CPU1_DBGBCR09        DCD 0
CPU1_DBGBVR10        DCD 0
CPU1_DBGBCR10        DCD 0
CPU1_DBGBVR11        DCD 0
CPU1_DBGBCR11        DCD 0
CPU1_DBGBVR12        DCD 0
CPU1_DBGBCR12        DCD 0
CPU1_DBGBVR13        DCD 0
CPU1_DBGBCR13        DCD 0
CPU1_DBGBVR14        DCD 0
CPU1_DBGBCR14        DCD 0
CPU1_DBGBVR15        DCD 0
CPU1_DBGBCR15        DCD 0
CPU1_DBGWVR00        DCD 0
CPU1_DBGWCR00        DCD 0
CPU1_DBGWVR01        DCD 0
CPU1_DBGWCR01        DCD 0
CPU1_DBGWVR02        DCD 0
CPU1_DBGWCR02        DCD 0
CPU1_DBGWVR03        DCD 0
CPU1_DBGWCR03        DCD 0
CPU1_DBGWVR04        DCD 0
CPU1_DBGWCR04        DCD 0
CPU1_DBGWVR05        DCD 0
CPU1_DBGWCR05        DCD 0
CPU1_DBGWVR06        DCD 0
CPU1_DBGWCR06        DCD 0
CPU1_DBGWVR07        DCD 0
CPU1_DBGWCR07        DCD 0
CPU1_DBGWVR08        DCD 0
CPU1_DBGWCR08        DCD 0
CPU1_DBGWVR09        DCD 0
CPU1_DBGWCR09        DCD 0
CPU1_DBGWVR10        DCD 0
CPU1_DBGWCR10        DCD 0
CPU1_DBGWVR11        DCD 0
CPU1_DBGWCR11        DCD 0
CPU1_DBGWVR12        DCD 0
CPU1_DBGWCR12        DCD 0
CPU1_DBGWVR13        DCD 0
CPU1_DBGWCR13        DCD 0
CPU1_DBGWVR14        DCD 0
CPU1_DBGWCR14        DCD 0
CPU1_DBGWVR15        DCD 0
CPU1_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 3)
CPU2_DBGDTRRXext     DCD 0
CPU2_DBGDTRTXext     DCD 0
CPU2_DBGDSCRext      DCD 0
CPU2_DBGWFAR         DCD 0
CPU2_DBGVCR          DCD 0
CPU2_DBGCLAIMCLR     DCD 0
CPU2_DBGBVR00        DCD 0
CPU2_DBGBCR00        DCD 0
CPU2_DBGBVR01        DCD 0
CPU2_DBGBCR01        DCD 0
CPU2_DBGBVR02        DCD 0
CPU2_DBGBCR02        DCD 0
CPU2_DBGBVR03        DCD 0
CPU2_DBGBCR03        DCD 0
CPU2_DBGBVR04        DCD 0
CPU2_DBGBCR04        DCD 0
CPU2_DBGBVR05        DCD 0
CPU2_DBGBCR05        DCD 0
CPU2_DBGBVR06        DCD 0
CPU2_DBGBCR06        DCD 0
CPU2_DBGBVR07        DCD 0
CPU2_DBGBCR07        DCD 0
CPU2_DBGBVR08        DCD 0
CPU2_DBGBCR08        DCD 0
CPU2_DBGBVR09        DCD 0
CPU2_DBGBCR09        DCD 0
CPU2_DBGBVR10        DCD 0
CPU2_DBGBCR10        DCD 0
CPU2_DBGBVR11        DCD 0
CPU2_DBGBCR11        DCD 0
CPU2_DBGBVR12        DCD 0
CPU2_DBGBCR12        DCD 0
CPU2_DBGBVR13        DCD 0
CPU2_DBGBCR13        DCD 0
CPU2_DBGBVR14        DCD 0
CPU2_DBGBCR14        DCD 0
CPU2_DBGBVR15        DCD 0
CPU2_DBGBCR15        DCD 0
CPU2_DBGWVR00        DCD 0
CPU2_DBGWCR00        DCD 0
CPU2_DBGWVR01        DCD 0
CPU2_DBGWCR01        DCD 0
CPU2_DBGWVR02        DCD 0
CPU2_DBGWCR02        DCD 0
CPU2_DBGWVR03        DCD 0
CPU2_DBGWCR03        DCD 0
CPU2_DBGWVR04        DCD 0
CPU2_DBGWCR04        DCD 0
CPU2_DBGWVR05        DCD 0
CPU2_DBGWCR05        DCD 0
CPU2_DBGWVR06        DCD 0
CPU2_DBGWCR06        DCD 0
CPU2_DBGWVR07        DCD 0
CPU2_DBGWCR07        DCD 0
CPU2_DBGWVR08        DCD 0
CPU2_DBGWCR08        DCD 0
CPU2_DBGWVR09        DCD 0
CPU2_DBGWCR09        DCD 0
CPU2_DBGWVR10        DCD 0
CPU2_DBGWCR10        DCD 0
CPU2_DBGWVR11        DCD 0
CPU2_DBGWCR11        DCD 0
CPU2_DBGWVR12        DCD 0
CPU2_DBGWCR12        DCD 0
CPU2_DBGWVR13        DCD 0
CPU2_DBGWCR13        DCD 0
CPU2_DBGWVR14        DCD 0
CPU2_DBGWCR14        DCD 0
CPU2_DBGWVR15        DCD 0
CPU2_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 4)
CPU3_DBGDTRRXext     DCD 0
CPU3_DBGDTRTXext     DCD 0
CPU3_DBGDSCRext      DCD 0
CPU3_DBGWFAR         DCD 0
CPU3_DBGVCR          DCD 0
CPU3_DBGCLAIMCLR     DCD 0
CPU3_DBGBVR00        DCD 0
CPU3_DBGBCR00        DCD 0
CPU3_DBGBVR01        DCD 0
CPU3_DBGBCR01        DCD 0
CPU3_DBGBVR02        DCD 0
CPU3_DBGBCR02        DCD 0
CPU3_DBGBVR03        DCD 0
CPU3_DBGBCR03        DCD 0
CPU3_DBGBVR04        DCD 0
CPU3_DBGBCR04        DCD 0
CPU3_DBGBVR05        DCD 0
CPU3_DBGBCR05        DCD 0
CPU3_DBGBVR06        DCD 0
CPU3_DBGBCR06        DCD 0
CPU3_DBGBVR07        DCD 0
CPU3_DBGBCR07        DCD 0
CPU3_DBGBVR08        DCD 0
CPU3_DBGBCR08        DCD 0
CPU3_DBGBVR09        DCD 0
CPU3_DBGBCR09        DCD 0
CPU3_DBGBVR10        DCD 0
CPU3_DBGBCR10        DCD 0
CPU3_DBGBVR11        DCD 0
CPU3_DBGBCR11        DCD 0
CPU3_DBGBVR12        DCD 0
CPU3_DBGBCR12        DCD 0
CPU3_DBGBVR13        DCD 0
CPU3_DBGBCR13        DCD 0
CPU3_DBGBVR14        DCD 0
CPU3_DBGBCR14        DCD 0
CPU3_DBGBVR15        DCD 0
CPU3_DBGBCR15        DCD 0
CPU3_DBGWVR00        DCD 0
CPU3_DBGWCR00        DCD 0
CPU3_DBGWVR01        DCD 0
CPU3_DBGWCR01        DCD 0
CPU3_DBGWVR02        DCD 0
CPU3_DBGWCR02        DCD 0
CPU3_DBGWVR03        DCD 0
CPU3_DBGWCR03        DCD 0
CPU3_DBGWVR04        DCD 0
CPU3_DBGWCR04        DCD 0
CPU3_DBGWVR05        DCD 0
CPU3_DBGWCR05        DCD 0
CPU3_DBGWVR06        DCD 0
CPU3_DBGWCR06        DCD 0
CPU3_DBGWVR07        DCD 0
CPU3_DBGWCR07        DCD 0
CPU3_DBGWVR08        DCD 0
CPU3_DBGWCR08        DCD 0
CPU3_DBGWVR09        DCD 0
CPU3_DBGWCR09        DCD 0
CPU3_DBGWVR10        DCD 0
CPU3_DBGWCR10        DCD 0
CPU3_DBGWVR11        DCD 0
CPU3_DBGWCR11        DCD 0
CPU3_DBGWVR12        DCD 0
CPU3_DBGWCR12        DCD 0
CPU3_DBGWVR13        DCD 0
CPU3_DBGWCR13        DCD 0
CPU3_DBGWVR14        DCD 0
CPU3_DBGWCR14        DCD 0
CPU3_DBGWVR15        DCD 0
CPU3_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 5)
CPU4_DBGDTRRXext     DCD 0
CPU4_DBGDTRTXext     DCD 0
CPU4_DBGDSCRext      DCD 0
CPU4_DBGWFAR         DCD 0
CPU4_DBGVCR          DCD 0
CPU4_DBGCLAIMCLR     DCD 0
CPU4_DBGBVR00        DCD 0
CPU4_DBGBCR00        DCD 0
CPU4_DBGBVR01        DCD 0
CPU4_DBGBCR01        DCD 0
CPU4_DBGBVR02        DCD 0
CPU4_DBGBCR02        DCD 0
CPU4_DBGBVR03        DCD 0
CPU4_DBGBCR03        DCD 0
CPU4_DBGBVR04        DCD 0
CPU4_DBGBCR04        DCD 0
CPU4_DBGBVR05        DCD 0
CPU4_DBGBCR05        DCD 0
CPU4_DBGBVR06        DCD 0
CPU4_DBGBCR06        DCD 0
CPU4_DBGBVR07        DCD 0
CPU4_DBGBCR07        DCD 0
CPU4_DBGBVR08        DCD 0
CPU4_DBGBCR08        DCD 0
CPU4_DBGBVR09        DCD 0
CPU4_DBGBCR09        DCD 0
CPU4_DBGBVR10        DCD 0
CPU4_DBGBCR10        DCD 0
CPU4_DBGBVR11        DCD 0
CPU4_DBGBCR11        DCD 0
CPU4_DBGBVR12        DCD 0
CPU4_DBGBCR12        DCD 0
CPU4_DBGBVR13        DCD 0
CPU4_DBGBCR13        DCD 0
CPU4_DBGBVR14        DCD 0
CPU4_DBGBCR14        DCD 0
CPU4_DBGBVR15        DCD 0
CPU4_DBGBCR15        DCD 0
CPU4_DBGWVR00        DCD 0
CPU4_DBGWCR00        DCD 0
CPU4_DBGWVR01        DCD 0
CPU4_DBGWCR01        DCD 0
CPU4_DBGWVR02        DCD 0
CPU4_DBGWCR02        DCD 0
CPU4_DBGWVR03        DCD 0
CPU4_DBGWCR03        DCD 0
CPU4_DBGWVR04        DCD 0
CPU4_DBGWCR04        DCD 0
CPU4_DBGWVR05        DCD 0
CPU4_DBGWCR05        DCD 0
CPU4_DBGWVR06        DCD 0
CPU4_DBGWCR06        DCD 0
CPU4_DBGWVR07        DCD 0
CPU4_DBGWCR07        DCD 0
CPU4_DBGWVR08        DCD 0
CPU4_DBGWCR08        DCD 0
CPU4_DBGWVR09        DCD 0
CPU4_DBGWCR09        DCD 0
CPU4_DBGWVR10        DCD 0
CPU4_DBGWCR10        DCD 0
CPU4_DBGWVR11        DCD 0
CPU4_DBGWCR11        DCD 0
CPU4_DBGWVR12        DCD 0
CPU4_DBGWCR12        DCD 0
CPU4_DBGWVR13        DCD 0
CPU4_DBGWCR13        DCD 0
CPU4_DBGWVR14        DCD 0
CPU4_DBGWCR14        DCD 0
CPU4_DBGWVR15        DCD 0
CPU4_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 6)
CPU5_DBGDTRRXext     DCD 0
CPU5_DBGDTRTXext     DCD 0
CPU5_DBGDSCRext      DCD 0
CPU5_DBGWFAR         DCD 0
CPU5_DBGVCR          DCD 0
CPU5_DBGCLAIMCLR     DCD 0
CPU5_DBGBVR00        DCD 0
CPU5_DBGBCR00        DCD 0
CPU5_DBGBVR01        DCD 0
CPU5_DBGBCR01        DCD 0
CPU5_DBGBVR02        DCD 0
CPU5_DBGBCR02        DCD 0
CPU5_DBGBVR03        DCD 0
CPU5_DBGBCR03        DCD 0
CPU5_DBGBVR04        DCD 0
CPU5_DBGBCR04        DCD 0
CPU5_DBGBVR05        DCD 0
CPU5_DBGBCR05        DCD 0
CPU5_DBGBVR06        DCD 0
CPU5_DBGBCR06        DCD 0
CPU5_DBGBVR07        DCD 0
CPU5_DBGBCR07        DCD 0
CPU5_DBGBVR08        DCD 0
CPU5_DBGBCR08        DCD 0
CPU5_DBGBVR09        DCD 0
CPU5_DBGBCR09        DCD 0
CPU5_DBGBVR10        DCD 0
CPU5_DBGBCR10        DCD 0
CPU5_DBGBVR11        DCD 0
CPU5_DBGBCR11        DCD 0
CPU5_DBGBVR12        DCD 0
CPU5_DBGBCR12        DCD 0
CPU5_DBGBVR13        DCD 0
CPU5_DBGBCR13        DCD 0
CPU5_DBGBVR14        DCD 0
CPU5_DBGBCR14        DCD 0
CPU5_DBGBVR15        DCD 0
CPU5_DBGBCR15        DCD 0
CPU5_DBGWVR00        DCD 0
CPU5_DBGWCR00        DCD 0
CPU5_DBGWVR01        DCD 0
CPU5_DBGWCR01        DCD 0
CPU5_DBGWVR02        DCD 0
CPU5_DBGWCR02        DCD 0
CPU5_DBGWVR03        DCD 0
CPU5_DBGWCR03        DCD 0
CPU5_DBGWVR04        DCD 0
CPU5_DBGWCR04        DCD 0
CPU5_DBGWVR05        DCD 0
CPU5_DBGWCR05        DCD 0
CPU5_DBGWVR06        DCD 0
CPU5_DBGWCR06        DCD 0
CPU5_DBGWVR07        DCD 0
CPU5_DBGWCR07        DCD 0
CPU5_DBGWVR08        DCD 0
CPU5_DBGWCR08        DCD 0
CPU5_DBGWVR09        DCD 0
CPU5_DBGWCR09        DCD 0
CPU5_DBGWVR10        DCD 0
CPU5_DBGWCR10        DCD 0
CPU5_DBGWVR11        DCD 0
CPU5_DBGWCR11        DCD 0
CPU5_DBGWVR12        DCD 0
CPU5_DBGWCR12        DCD 0
CPU5_DBGWVR13        DCD 0
CPU5_DBGWCR13        DCD 0
CPU5_DBGWVR14        DCD 0
CPU5_DBGWCR14        DCD 0
CPU5_DBGWVR15        DCD 0
CPU5_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 7)
CPU6_DBGDTRRXext     DCD 0
CPU6_DBGDTRTXext     DCD 0
CPU6_DBGDSCRext      DCD 0
CPU6_DBGWFAR         DCD 0
CPU6_DBGVCR          DCD 0
CPU6_DBGCLAIMCLR     DCD 0
CPU6_DBGBVR00        DCD 0
CPU6_DBGBCR00        DCD 0
CPU6_DBGBVR01        DCD 0
CPU6_DBGBCR01        DCD 0
CPU6_DBGBVR02        DCD 0
CPU6_DBGBCR02        DCD 0
CPU6_DBGBVR03        DCD 0
CPU6_DBGBCR03        DCD 0
CPU6_DBGBVR04        DCD 0
CPU6_DBGBCR04        DCD 0
CPU6_DBGBVR05        DCD 0
CPU6_DBGBCR05        DCD 0
CPU6_DBGBVR06        DCD 0
CPU6_DBGBCR06        DCD 0
CPU6_DBGBVR07        DCD 0
CPU6_DBGBCR07        DCD 0
CPU6_DBGBVR08        DCD 0
CPU6_DBGBCR08        DCD 0
CPU6_DBGBVR09        DCD 0
CPU6_DBGBCR09        DCD 0
CPU6_DBGBVR10        DCD 0
CPU6_DBGBCR10        DCD 0
CPU6_DBGBVR11        DCD 0
CPU6_DBGBCR11        DCD 0
CPU6_DBGBVR12        DCD 0
CPU6_DBGBCR12        DCD 0
CPU6_DBGBVR13        DCD 0
CPU6_DBGBCR13        DCD 0
CPU6_DBGBVR14        DCD 0
CPU6_DBGBCR14        DCD 0
CPU6_DBGBVR15        DCD 0
CPU6_DBGBCR15        DCD 0
CPU6_DBGWVR00        DCD 0
CPU6_DBGWCR00        DCD 0
CPU6_DBGWVR01        DCD 0
CPU6_DBGWCR01        DCD 0
CPU6_DBGWVR02        DCD 0
CPU6_DBGWCR02        DCD 0
CPU6_DBGWVR03        DCD 0
CPU6_DBGWCR03        DCD 0
CPU6_DBGWVR04        DCD 0
CPU6_DBGWCR04        DCD 0
CPU6_DBGWVR05        DCD 0
CPU6_DBGWCR05        DCD 0
CPU6_DBGWVR06        DCD 0
CPU6_DBGWCR06        DCD 0
CPU6_DBGWVR07        DCD 0
CPU6_DBGWCR07        DCD 0
CPU6_DBGWVR08        DCD 0
CPU6_DBGWCR08        DCD 0
CPU6_DBGWVR09        DCD 0
CPU6_DBGWCR09        DCD 0
CPU6_DBGWVR10        DCD 0
CPU6_DBGWCR10        DCD 0
CPU6_DBGWVR11        DCD 0
CPU6_DBGWCR11        DCD 0
CPU6_DBGWVR12        DCD 0
CPU6_DBGWCR12        DCD 0
CPU6_DBGWVR13        DCD 0
CPU6_DBGWCR13        DCD 0
CPU6_DBGWVR14        DCD 0
CPU6_DBGWCR14        DCD 0
CPU6_DBGWVR15        DCD 0
CPU6_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 8)
CPU7_DBGDTRRXext     DCD 0
CPU7_DBGDTRTXext     DCD 0
CPU7_DBGDSCRext      DCD 0
CPU7_DBGWFAR         DCD 0
CPU7_DBGVCR          DCD 0
CPU7_DBGCLAIMCLR     DCD 0
CPU7_DBGBVR00        DCD 0
CPU7_DBGBCR00        DCD 0
CPU7_DBGBVR01        DCD 0
CPU7_DBGBCR01        DCD 0
CPU7_DBGBVR02        DCD 0
CPU7_DBGBCR02        DCD 0
CPU7_DBGBVR03        DCD 0
CPU7_DBGBCR03        DCD 0
CPU7_DBGBVR04        DCD 0
CPU7_DBGBCR04        DCD 0
CPU7_DBGBVR05        DCD 0
CPU7_DBGBCR05        DCD 0
CPU7_DBGBVR06        DCD 0
CPU7_DBGBCR06        DCD 0
CPU7_DBGBVR07        DCD 0
CPU7_DBGBCR07        DCD 0
CPU7_DBGBVR08        DCD 0
CPU7_DBGBCR08        DCD 0
CPU7_DBGBVR09        DCD 0
CPU7_DBGBCR09        DCD 0
CPU7_DBGBVR10        DCD 0
CPU7_DBGBCR10        DCD 0
CPU7_DBGBVR11        DCD 0
CPU7_DBGBCR11        DCD 0
CPU7_DBGBVR12        DCD 0
CPU7_DBGBCR12        DCD 0
CPU7_DBGBVR13        DCD 0
CPU7_DBGBCR13        DCD 0
CPU7_DBGBVR14        DCD 0
CPU7_DBGBCR14        DCD 0
CPU7_DBGBVR15        DCD 0
CPU7_DBGBCR15        DCD 0
CPU7_DBGWVR00        DCD 0
CPU7_DBGWCR00        DCD 0
CPU7_DBGWVR01        DCD 0
CPU7_DBGWCR01        DCD 0
CPU7_DBGWVR02        DCD 0
CPU7_DBGWCR02        DCD 0
CPU7_DBGWVR03        DCD 0
CPU7_DBGWCR03        DCD 0
CPU7_DBGWVR04        DCD 0
CPU7_DBGWCR04        DCD 0
CPU7_DBGWVR05        DCD 0
CPU7_DBGWCR05        DCD 0
CPU7_DBGWVR06        DCD 0
CPU7_DBGWCR06        DCD 0
CPU7_DBGWVR07        DCD 0
CPU7_DBGWCR07        DCD 0
CPU7_DBGWVR08        DCD 0
CPU7_DBGWCR08        DCD 0
CPU7_DBGWVR09        DCD 0
CPU7_DBGWCR09        DCD 0
CPU7_DBGWVR10        DCD 0
CPU7_DBGWCR10        DCD 0
CPU7_DBGWVR11        DCD 0
CPU7_DBGWCR11        DCD 0
CPU7_DBGWVR12        DCD 0
CPU7_DBGWCR12        DCD 0
CPU7_DBGWVR13        DCD 0
CPU7_DBGWCR13        DCD 0
CPU7_DBGWVR14        DCD 0
CPU7_DBGWCR14        DCD 0
CPU7_DBGWVR15        DCD 0
CPU7_DBGWCR15        DCD 0
#endif
#if (TZBSP_CPU_COUNT_MAX >= 9)
#error "Add more space for debug regs"
#endif
    END
