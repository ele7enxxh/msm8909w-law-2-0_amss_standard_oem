#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     T R U S T Z O N E   E N T R Y
;
; GENERAL DESCRIPTION
;   This file contains the TZBSP main entry
;
; EXTERNALIZED SYMBOLS
;   __main
;   _main
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;
; Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 03/28/11   tk      Initial version
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/tzbsp_target.h"
















 


















 





 
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/customer.h"


















 















































 






 



 
#line 1 "./custmataanaaa.h"






 

#line 1 "./targmataanaaa.h"






 

#line 176 "./targmataanaaa.h"



#line 185 "./targmataanaaa.h"

#line 11 "./custmataanaaa.h"


#line 142 "./custmataanaaa.h"


#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"










 










































 





 











   




 
#line 85 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"



     



   
#line 101 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"
   



   
  
 
  
   




   
   


   


   


   
  

   










   

  

     




     










#line 210 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"

   




   









   


















#line 258 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"

#line 279 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custremoteapis.h"






  

   
  

   
 

    
    


     
   







 




























   






















#line 145 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custtarget.h"










 

















 

#line 46 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custtarget.h"




 

#line 146 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsdcc.h"













 















 

 


 
#line 48 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsdcc.h"

 


 


 
 
 
 
 








#line 147 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsurf.h"








 




















 



 
 



 



 




 


 




 


 


 

 


 


 



 





 


#line 92 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsurf.h"


 




 
















 


                            



 


 






#line 148 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custdiag.h"








 






























 



 

  

 

#line 57 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custdiag.h"
  







   
   
 


 


 



















  

 






  

 




    
























#line 149 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custefs.h"









 















 

#line 59 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custefs.h"

   

   
















#line 150 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custpmic.h"








 


















 



 

 


 






      
 

    
      
 

    

       
    

     

    
     

    
     


    

    

 
 
 

 







#line 151 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"








 
                                        


































 






 




 








#line 72 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"

 














   
#line 95 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"




   
#line 106 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"





#line 117 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"




















 





 





#line 156 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"


 




 







#line 214 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"





#line 251 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsio_8660.h"













































#line 152 "./custmataanaaa.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsec.h"










 


















 






 



 

 


 






 


 


 


 
   
 







 



   


   


   







   




   
   
   



   
#line 111 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsec.h"

   


   




#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsfs.h"










 




























 






 



 

 


 









#line 72 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsfs.h"

 









#line 89 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsfs.h"

#line 122 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/custsec.h"






#line 153 "./custmataanaaa.h"

#line 187 "./custmataanaaa.h"

#line 81 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/customer.h"







 




























































































 
 




 




 





 




 
#line 247 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/build/cust/customer.h"

#line 44 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/tzbsp_target.h"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/../common/tzbsp_target.h"
















 


















 





 
#line 44 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/../common/tzbsp_target.h"





 



 




 
#line 65 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/../common/tzbsp_target.h"






 


 











 


 


 


 


 


 


 



 



 



 




 




 



 







 



 



 



 



 



 



 




 




  




 






#line 198 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/../common/tzbsp_target.h"


 





  


 









#line 45 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/tzbsp_target.h"





 







 






 







 
 





 






 


 




 











 











 




 



 



#line 39 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_shared_imem.h"





 















 



 
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/target.h"











 





























 

#line 61 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/target.h"


 



 







 
#line 78 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/target.h"


#line 28 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_shared_imem.h"



 





 




 









 





 



 










 




 










 





 
#line 104 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_shared_imem.h"



 






 





 


#line 40 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"
#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_asm.h"











 













 





 
#line 34 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_asm.h"


; ---------------------
; MACRO: Mov32
; ---------------------
; Moves a 32 bit value into a register
  MACRO
  Mov32 $r, $val_l
    movw    $r, #($val_l & 0xFFFF)
    movt    $r, #(($val_l >> 16) & 0xFFFF)
  MEND

; ---------------------
; MACRO: Read32R
; ---------------------
; Reads a given word where the address is in a register
  MACRO
  Read32R $val_r, $addr_r, $work_r

    Mov32   $work_r, 0x02000000
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, 0x04800000

10  ldr     $val_r, [$addr_r, $work_r]



  MEND

; ---------------------
; MACRO: Read32L
; ---------------------
; Reads a given word where the address is a literal
  MACRO
  Read32L $val_r, $addr_l, $work_r
    Mov32   $val_r, $addr_l
    Read32R $val_r, $val_r, $work_r
  MEND

; ---------------------
; MACRO: Write32R
; ---------------------
; Writes a given word where the address is in a register
  MACRO
  Write32R $val_r, $addr_r, $work_r

    Mov32   $work_r, 0x02000000
    cmp     $addr_r, $work_r        ; compare address to remap area
    mov     $work_r, #0
    bhs     %f10                    ; addr >= remap, no need to offset
    Mov32   $work_r, 0x04800000

10  str     $val_r, [$addr_r, $work_r]



  MEND

; ---------------------
; MACRO: Write32L
; ---------------------
; Writes a given word where the address is a literal
  MACRO
  Write32L $val_r, $addr_l, $work_r, $addr_r
    Mov32    $addr_r, $addr_l
    Write32R $val_r, $addr_r, $work_r
  MEND
#line 41 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"

    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
    GET tzbsp_asm_common.inc
    GET qctps_common_macros.inc

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT tzbsp_hand_control_to_ns
    IMPORT tzbsp_smc_lock
    IMPORT tzbsp_boot_lock
    IMPORT tzbsp_save_spbl_context
    IMPORT tzbsp_cold_init
    IMPORT tzbsp_cpu_init
    IMPORT tzbsp_loop_here
    IMPORT tzbsp_switch_execute_smc
    IMPORT Workspace_N_Begin
    IMPORT |Image$$TZBSP_STACK$$ZI$$Limit|
    IMPORT |Image$$TZBSP_STACK$$ZI$$Base|
    IMPORT |TZBSP_UNCACHED_ZI_BOOT_STACK$$Limit|
    IMPORT g_tzbsp_entry_counters
    IMPORT g_tzbsp_exit_counters
    IMPORT g_tzbsp_jump_addrs
    IMPORT g_tzbsp_warm_boot
; Not compiled in by default.
;    IMPORT tzbsp_restore_debug_regs
    IMPORT sysini_wrapper
    IMPORT sysini_wrapper_cold_init
    IMPORT tzbsp_secboot_set_shared_info

    IMPORT qsee_undef_handler
    IMPORT qsee_swi_handler
    IMPORT qsee_pabort_handler
    IMPORT qsee_dabort_handler


    IMPORT sysdbg_reset_check_flat_mapped



    IMPORT sysdbg_save_ctx
    IMPORT system_debug_main
	IMPORT sysdbg_stack



    IMPORT tzbsp_boot_disable_cpu_l2_spm_arch
    IMPORT tzbsp_enable_cluster_coherency_arch

    IMPORT a7_aarch32_sysini



;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main

    EXPORT   tzbsp_has_cpu_cold_booted
    EXPORT   RST_VEC

    EXPORT   tzbsp_cpus_per_cluster_in_lpm
    EXPORT   tzbsp_cpu_count_per_cluster
    EXPORT   tzbsp_do_l2_init
    EXPORT   tzbsp_cpu_pc_state
    EXPORT   tzbsp_reset_vector
    EXPORT   tzbsp_shared_imem_slp_ticks_0
    EXPORT   tzbsp_was_l2_pc
    EXPORT   tzbsp_was_gdhs_pc



    EXPORT   tzbsp_boot_cpu_booted_once
    EXPORT   g_tzbsp_cpu_count
    EXPORT   g_tzbsp_cluster_count
    EXPORT   g_tzbsp_boot_cluster_idx
    EXPORT   g_tzbsp_boot_cpu_idx
    EXPORT   tzbsp_cpu_boot_pc_state
    EXPORT   tzbsp_sec_wdt_target_list_log
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    ;========================================================================
    ; Section TZBSP_VECTOR_TABLE must be defined first in the scatter-load
    ;========================================================================
    PRESERVE8
    CODE32
    AREA    TZBSP_VECTOR_TABLE, CODE, READONLY, ALIGN=16
    ENTRY
__main
_main

; Cold boot entry into TZ
RST_VEC
tzbsp_reset_vector
    ldr     pc, =tzbsp_entry_chooser ; Reset
    ldr     pc, =qsee_undef_handler  ; Undefined instruction
    ldr     pc, =qsee_swi_handler    ; SVC
    ldr     pc, =qsee_pabort_handler ; Prefetch abort
    ldr     pc, =qsee_dabort_handler ; Data abort
    ldr     pc, =tzbsp_loop_here     ; Not used / reserved
    ldr     pc, =tzbsp_loop_here     ; IRQ
    ldr     pc, =tzbsp_loop_here     ; FIQ

tzbsp_entry_chooser
  
    ; Back up R0 and LR before clobbering them for cores beside 0
    ; Back up to TPIDRURW and TPIDRPRW
    MCR p15, 0, r0, c13, c0, 2
    MCR p15, 0, r14, c13, c0, 4


    ldr     r0, =sysdbg_reset_check_flat_mapped ; check to see if reset due to reset debug reason



    blx	    r0
    cmp     r0, #0                  ; check return value
    bne     sysdbg_entry_handler 
    MRC p15, 0, r0, c13, c0, 2    ;restore R0 for TZ
 
    ldr     r3, =tzbsp_boot_cpu_booted_once ; Is it TZ Cold Boot.
    ldr     r3, [r3]
    cmp     r3, #0
    beq     tzbsp_entry_handler  ; Entry from SPBL
    b       tzbsp_reset_handler  ; CPU1 cold boot, or any warm boot entry

;======================================================================
; When the reset handler is invoked we are either doing a secondary CPU cold
; boot or warm booting any of the CPUs. CPU0 cold boot is a special case
; handled in tzbsp_entry_handler. Because this is a real reset handler, we can
; use all available registers.
;======================================================================
tzbsp_reset_handler

    ; ------------------------------------------------------------------
    ; Verify our load address.
    ; ------------------------------------------------------------------
    adr     r0, tzbsp_reset_handler
    ldr     r1, =tzbsp_reset_handler
    cmp     r0, r1
0   bne     %0

    ; Set the warm boot status for each CPU. As this is a per-CPU
    ; variable, this can be done outside the boot lock.
    GetCurrentCpuNum
    ldr     r1, =g_tzbsp_warm_boot
    mov     r2, #1
    str     r2, [r1, r0, LSL #2]

    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x01, tzbsp_cpu_boot_pc_state

    ; ------------------------------------------------------------------
    ; Wait until we get exclusive access to boot path. This is a
    ; last line of defense preventing multiple CPUs doing the init
    ; simultaneously.
    ; ------------------------------------------------------------------
    WaitForAccess r0, r1, r2, tzbsp_boot_lock
    ; We got the lock, TZ boot path is now exclusively ours.

    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x02, tzbsp_cpu_boot_pc_state

    ; Disable L2 & CPU SPM
    blx     tzbsp_boot_disable_cpu_l2_spm_arch

    ; Decrement the number of CPUs in low power mode.
    GetCurrentClusterNum
    ldr     r1, =tzbsp_cpus_per_cluster_in_lpm
    ldr     r2, [r1, r0, lsl #2]
    sub     r2, r2, #1
    str     r2, [r1, r0, lsl #2]

    ; Update the entry counter.
    GetCurrentCpuNum
    ldr     r1, =g_tzbsp_entry_counters ; Base of entry counter addresses.
    ldr     r1, [r1, r0, LSL #2]        ; Read the counter address.
    ldr     r2, [r1]                    ; Read the stored counter value.
    add     r2, r2, #1                  ; Increment the counter.
    str     r2, [r1]                    ; Write the new counter value.

    ; Mark CPU's L1 valid from SDI perspective
    mov     r1, #1
    lsl     r1, r1, r0  ; Bit mask based on current CPU
    Mov32   r3, ((0x08600000 + 0x734) + (0x28))
    ldr     r2, [r3]
    orr     r2, r2, r1
    str     r2, [r3]

    ; Dump the current MPM_SLEEP_TIMETICK_COUNT_VAL to TZ area in Shared
    ; IMEM. This is for power collapse debugging. Note that r0 must contain
    ; the current CPU number.
    ;TODO: Remove once 8974v1 support is dropped (delete tzbsp_shared_imem_slp_ticks_0)
    ;Mov32   r1, ((0x08600000 + 0x734) + (0x60))
    ldr     r1, =tzbsp_shared_imem_slp_ticks_0
    ldr     r1, [r1]
    add     r1, r0, LSL #2 ; Select this CPU's slot.
    Mov32   r2, 0x004a3000 ; MPM_SLEEP_TIMETICK_COUNT_VAL addr
    ldr     r2, [r2]       ; Read the counter value.
    str     r2, [r1]       ; Store it to CPU specific slot in TZ shared IMEM.

    ; -----------------------------------------------------------------------
    ; Set up a stack pointer in Supervisor,Undefined,Abort mode.
    ; -----------------------------------------------------------------------
    ; Set the Stack Pointer address for supervisor mode
    msr     CPSR_c,#TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    ldr     r0, =|TZBSP_UNCACHED_ZI_BOOT_STACK$$Limit|
    mov     r13, r0

    ; Set the Stack Pointer address for undefined mode
    ldr     r0, =tzbsp_abt_stack
    add     r0, #0x400              ; Size is defined below.
    sub     r0, r0, #0x8
    msr     CPSR_c, #TZBSP_Mode_UND:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Undefined SP

    ; Set the Stack Pointer address for abort mode
    msr     CPSR_c, #TZBSP_Mode_ABT:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Abort SP

    ; Switch back to Supervisor mode
    msr     CPSR_c, #TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit

    ; Restore debug registers as early as possible after stack is set
    ; up to help debugging PC entry/exit
;    bl      tzbsp_restore_debug_regs

    ; Figure out if the L2 cache needs to be invalidated and initialized.
    ; TZ will have to do L2 setup only when L2 cache has been power collapsed.
    ; Check if all cpus are going to lpm and the l2 init flag is set.
    GetCurrentClusterNum
    ldr     r1, =tzbsp_cpus_per_cluster_in_lpm
    add     r1, r1, r0, lsl #2
    ldr     r1, [r1]
    mov     r0, #0 ;setup l2 init flag for sysini wrapper
    cmp     r1, #((4 / 1) - 1)  ; Are other CPUs down?
    bne     %f2                         ; Not all CPUs were PC'd skip L2 init.
    GetCurrentClusterNum
    ldr     r1, =tzbsp_do_l2_init
    add     r1, r1, r0, lsl #2
    ldr     r0, [r1]                    ; r0 contains L2 init flag for wrapper

    ; SYSINI must be done after the spinlock, because L2 cache common init
    ; is done conditionally. If all cores are released simultaneously from
    ; reset, only the core that first hits the SYSINI will apply the L2
    ; cache common init.
2   blx     sysini_wrapper


    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x03, tzbsp_cpu_boot_pc_state

    ; ------------------------------------------------------------------
    ; Setup the Vector Base Address Register (VBAR) to our vectors
    ; ------------------------------------------------------------------
    ldr     r0, =tzbsp_reset_vector
    mcr     p15, 0, r0, c12, c0, 0

    ; At this point, L2 is valid; mark it as such, for SDI
    mov     r1, #0x1
    Mov32   r0, ((0x08600000 + 0x734) + (0x2C))
    str     r1, [r0]

    ; Save the L2 sysini initialization state for QDSS tracing workaround.
    GetCurrentClusterNum
    ldr     r1, =tzbsp_do_l2_init
    add     r1, r1, r0, lsl #2
    ldr     r1, [r1]
    ldr     r2, =tzbsp_was_l2_pc
    add     r2, r2, r0, lsl #2
    str     r1, [r2]

    ; Clear the L2 counter in any case.
    ; r0 still contains the cluster num
    ldr     r1, =tzbsp_do_l2_init
    add     r1, r1, r0, lsl #2
    mov     r2, #0
    str     r2, [r1]
    ldr     r1, =tzbsp_was_gdhs_pc
    add     r1, r1, r0, lsl #2
    mov     r2, #0
    str     r2, [r1]
#line 350 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"

    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x04, tzbsp_cpu_boot_pc_state

    ldr     r0, =tzbsp_cpu_init
    blx     r0

    ; Update the exit counter (don't corrupt r0).
    push    {r0}

    ; Set Milestone for Warm Boot.0x000000XX
    SetCpuStateMilestoneVar r0, r1, 0x05, tzbsp_cpu_boot_pc_state

    GetCurrentCpuNum
    ldr     r2, =g_tzbsp_exit_counters  ; Base of exit counter addresses.
    ldr     r2, [r2, r0, LSL #2]        ; Read the counter address.
    ldr     r3, [r2]                    ; Read the stored counter value.
    add     r3, r3, #1                  ; Increment the counter.
    str     r3, [r2]                    ; Write the new counter value.

    ; Clear the warm boot status for each CPU. Not that r0 from above needs
    ; to be preserved for this (contains the CPU id).
    ldr     r2, =g_tzbsp_warm_boot
    mov     r3, #0
    str     r3, [r2, r0, LSL #2]
    pop     {r0}

    ; Return value from tzbsp_cpu_init, in r0, contains the HLOS jump address.
    ldr     r1, =tzbsp_hand_control_to_ns
    bx      r1                                  ; Never returns

#line 408 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"

;============================================================================
; Handles the init routine and main entry point for TZBSP
;
; Scorpion cold boot flow is slightly tainted as TZ is called from SPBL. The
; result is that we have to handle two different reset flows. In cold boot
; case TZ is called as function by SPBL and function calling conventions
; apply (cannot corrupt SPBL registers other than r0-r3). In warm boot the
; reset is coming straight from metal and we can corrupt any register.
;
;   Tasks include:
;   (1) Save necessary context information from caller
;   (2) Initialize the Trustzone monitor
;   (3) Jump to the Trustzone Init handler
;============================================================================
tzbsp_entry_handler

    ; ------------------------------------------------------------------
    ; Start by disabling IRQ and FIQ interrupts, when the boot flows
    ; from SPBL we cannot be sure of IRQ and FIQ masks.
    ; ------------------------------------------------------------------
    msr     CPSR_c,#TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit

    ; ------------------------------------------------------------------
    ; Save location of secboot shared info, passed by SBL as a pointer,
    ; currently in r0.  If not provided, spin.
    ; ------------------------------------------------------------------
    push    {r14}
    mov     r2, #0
    cmp     r0, r2
null_secboot_ftbl
    beq     null_secboot_ftbl
    ldr     r2, =tzbsp_secboot_set_shared_info
    blx     r2
    pop     {r14}

    ; ------------------------------------------------------------------
    ; Verify our load address.
    ; ------------------------------------------------------------------
    adr     r0, tzbsp_entry_handler
    ldr     r1, =tzbsp_entry_handler
    cmp     r0, r1
invalid_addr_trap
    bne     invalid_addr_trap

#line 466 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/init/src/tzbsp_entry.s"
    ; ------------------------------------------------------------------
    ; Wait until we get exclusive access to monitor/TZBSP. This is a
    ; last line of defense preventing multiple CPUs doing the init
    ; simultaneously.
    ; ------------------------------------------------------------------
    WaitForAccess r0, r1, r2, tzbsp_smc_lock
    ; We got the lock, TZ is now exclusively ours.

    ; ------------------------------------------------------------------
    ; Build the non secure context using the secure context.
    ; Parameters: r0 - return address of non secure world
    ; Parameters: r1 - sp for supervisor mode of non-secure world
    ; ------------------------------------------------------------------
    mov     r0, r14
    mov     r1, r13
    ldr     r2, =tzbsp_save_spbl_context
    blx     r2
    ; SPBL context is saved, we can corrupt any register now.

    ; Update boot CPU boot status.
    ldr     r0, =tzbsp_boot_cpu_booted_once
    mov     r1, #1                            ; Cold boot done
    str     r1, [r0] ; Save cold boot status

    ; -----------------------------------------------------------------------
    ; Set up a stack pointer in Supervisor, Undefined, Abort mode.
    ; -----------------------------------------------------------------------

    ; Set the Stack Pointer address for supervisor mode
    ldr     r0, =|Image$$TZBSP_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Set the Stack Pointer address for undefined mode
    ldr     r0, =tzbsp_abt_stack
    add     r0, #0x400              ; Size is defined below.
    sub     r0, r0, #0x8
    msr     CPSR_c, #TZBSP_Mode_UND:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Undefined SP

    ; Set the Stack Pointer address for abort mode
    msr     CPSR_c, #TZBSP_Mode_ABT:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Abort SP

    ; Switch back to Supervisor mode
    msr     CPSR_c, #TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit

    ; ------------------------------------------------------------------
    ; Setup the Vector Base Address Register (VBAR) to our vectors
    ; ------------------------------------------------------------------
    ldr     r0, =tzbsp_reset_vector
    mcr     p15, 0, r0, c12, c0, 0

    ; Force flush/refetch of prefetch pipeline
    misb

    blx     sysini_wrapper_cold_init

    ; Jump to TZBSP C initializer
    ldr     r0, =tzbsp_cold_init
    blx     r0


    ; bring RPM out of reset
    ; this must be done at the very end of cpu0 cold boot procedure to avoid 
    ; any concurrency issues with other cores
    ldr     r0, =(0x1860000 + 0x04800000)
    ldr     r1, [r0]
    and     r1, r1, #0x6
    str     r1, [r0]


    ; Everything is done, jump to TZBSP message loop (it will
    ; SMC to non-secure side).
    ldr     r0, =tzbsp_switch_execute_smc ; Start the TZBSP message loop
    bx      r0                            ; Never returns

;============================================================================
; Handles the init routine and main entry point for SYSDBG
;
;
;   Tasks include:
;   (1) Setup Stack as required using tz stack space
;   (2) Jump to system_debug_main for the reset debug case 
;============================================================================
sysdbg_entry_handler

    ldr     r0, =sysdbg_save_ctx ; save ctx regardless
    blx	    r0
    ; -----------------------------------------------------------------------
    ; Set up a stack pointer in Supervisor, Undefined, Abort mode.
    ; -----------------------------------------------------------------------

    ; Set the Stack Pointer address for supervisor mode
    ldr     r0, =sysdbg_stack
    add     r0, r0, #1024           ; We need to point SP at the end of stack. 
                                    ; adding 1024 considering current stack is 1K.
    mov     r13, r0

    ; Set the Stack Pointer address for undefined mode
    ldr     r0, =tzbsp_abt_stack
    add     r0, #0x400              ; Size is defined below.
    sub     r0, r0, #0x8
    msr     CPSR_c, #TZBSP_Mode_UND:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Undefined SP

    ; Set the Stack Pointer address for abort mode
    msr     CPSR_c, #TZBSP_Mode_ABT:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit
    mov     r13, r0                 ; set Abort SP

    ; Switch back to Supervisor mode
    msr     CPSR_c, #TZBSP_Mode_SVC:OR:TZBSP_I_Bit:OR:TZBSP_F_Bit

    ; execute sysini

    blx     a7_aarch32_sysini




    ; enable coherency
    ldr     r3, =tzbsp_enable_cluster_coherency_arch
    blx     r3

    ldr     r3, =system_debug_main ; jump to system dbg main
    bx      r3 ;


;======================================================================
; TZBSP data area
;======================================================================
    AREA    TZBSP_DATA, DATA, READWRITE
tzbsp_abt_stack                  SPACE  0x400
tzbsp_shared_imem_slp_ticks_0    DCD    ((0x08600000 + 0x734) + (0x60))
tzbsp_cpu_count_per_cluster      DCD    (4 / 1)

; Uncached data is for items that are read/written both when the MMU
; is on and off.  This area is enforced ZI by memset in init.
    AREA    TZBSP_UNCACHED_ZI_PC_DATA, DATA, READWRITE, ALIGN=2
; Array of values, nonzero indicating CPU has not booted. This needs to be 
; 4-Byte Aligned
tzbsp_has_cpu_cold_booted        SPACE   (4 * 4)
tzbsp_boot_cpu_booted_once       SPACE    4
tzbsp_cpus_per_cluster_in_lpm    SPACE   (4 * 1); TZBSP tracks PCed CPU count
tzbsp_do_l2_init                 SPACE   (4 * 1); saved HLOS flag from PC
tzbsp_was_l2_pc                  SPACE   (4 * 1); L2 power collapse status copy.
tzbsp_was_gdhs_pc                SPACE   (4 * 1); L2 GDHS collapse status copy.





; Need 4-Byte Aligned address, cluster idx variable used when MMU not enable
g_tzbsp_boot_cluster_idx         SPACE   4
g_tzbsp_cpu_count                SPACE   1
g_tzbsp_cluster_count            SPACE   1
g_tzbsp_boot_cpu_idx             SPACE   1
filler10                         SPACE   1 ; To make next variables 4 byte aligned

tzbsp_cpu_boot_pc_state          SPACE   (4 * 4)

; Looging CPU No, Target List & Enable State for Sec WDog Bark Interrupt during PC
tzbsp_sec_wdt_target_list_log    SPACE   12
tzbsp_cpu_pc_state               SPACE   4

; Boot stack is place do uncached memory as it is being used before and after
; MMU enablement.
    AREA   TZBSP_UNCACHED_ZI_BOOT_STACK, DATA, READWRITE, ALIGN=3
tzbsp_boot_stack                 SPACE   0x200
    END
