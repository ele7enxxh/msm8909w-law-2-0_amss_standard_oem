#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/tzbsp_mmu.s"
;============================================================================
;
;             Scorpion architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   This file contains the Scorpion hierarchichal cache and mmu control functions.
;   File originaly taken from: 
;
; Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved
;============================================================================

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; $Header: 
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  ---     ---------------------------------------------------------
; 01/25/11  pre     Initial Version
;============================================================================


;============================================================================
;                             MODULE DEFINES
;============================================================================

;============================================================================
;                             MODULE INCLUDES
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





 







 






 







 
 





 






 


 




 











 











 




 



 



#line 36 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/tzbsp_mmu.s"

    GET tzbsp_libarm_asm.inc
    GET tzbsp_asm.inc
    GET qctps_common_macros.inc

;============================================================================
;                             MODULE IMPORTS
;============================================================================
    IMPORT  tzbsp_init_tts
    IMPORT  tzbsp_cache_flush
    IMPORT  tzbsp_dcache_clean_all
    IMPORT  tzbsp_dcache_inval_all
    IMPORT  tzbsp_dcache_flush_all
    IMPORT  tzbsp_dcache_clean_region
    IMPORT  tzbsp_dcache_inval_region
    IMPORT  tzbsp_dcache_flush_region
    IMPORT  tzbsp_dcache_inval_l1
    IMPORT  mmu_set_ttbrs
    IMPORT  mmu_enable_tex_remap
    IMPORT  l1

;============================================================================
;                             MODULE EXPORTS
;============================================================================
    EXPORT  tzbsp_mmu_cache_init
    EXPORT  tzbsp_mmu_cache_disable
    EXPORT  tzbsp_mmu_cache_re_enable
    EXPORT  tzbsp_mmu_cache_enable_no_cfg
    EXPORT  tzbsp_mmu_disable       ; For L2 TT dump purposes only
    EXPORT  tzbsp_mmu_enable        ; For L2 TT dump purposes only
    EXPORT  tzbsp_mmu_is_enabled
    EXPORT  tzbsp_tlb_inval_all
    EXPORT  tzbsp_itlb_inval_all
    EXPORT  tzbsp_tlb_inval_all_is
    EXPORT  tzbsp_tlb_inval_by_asid_is
    EXPORT  tzbsp_tlb_inval_by_mva_is
    EXPORT  tzbsp_dtlb_inval_all


;============================================================================
;                             MODULE EQUATES
;============================================================================
C1_REG_DEFAULT        EQU    0x10C56870

; CPSR Processing Modes M[4:0] (CPSR[M]) bits
CPSR_MODE_USR               EQU     0x10
CPSR_MODE_FIQ               EQU     0x11
CPSR_MODE_IRQ               EQU     0x12
CPSR_MODE_SVC               EQU     0x13
CPSR_MODE_MON               EQU     0x16
CPSR_MODE_ABT               EQU     0x17
CPSR_MODE_UND               EQU     0x1b
CPSR_MODE_SYS               EQU     0x1f

; CPSR Interrupt Mask (CPSR[I], CPSR[I]) bits
CPSR_I_BIT                  EQU     0x80
CPSR_F_BIT                  EQU     0x40

; SCR (Secure Configuration Register) bits
SCR_NS_BIT                  EQU     0x01    ; Non-Secure (NS) bit
SCR_IRQ_BIT                 EQU     0x02    ; IRQ bit
SCR_FIQ_BIT                 EQU     0x04    ; FIQ bit
SCR_EA_BIT                  EQU     0x08    ; EA bit
SCR_FW_BIT                  EQU     0x10    ; F Bit writable (FW) bit
SCR_AW_BIT                  EQU     0x20    ; A Bit writable (AW) bit

;============================================================================
;                            MMU PAGETABLE
;
; Create a page table for the mmu. This pagetable is 16 kb and contains 4k
; word section descriptors that will be populated by create_pagetable.
;============================================================================

;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
   AREA    ARM_MMU, CODE, READONLY
   PRESERVE8
   CODE32

;============================================================================
; MACRO mmu_set_default_cfg
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor.
;
; TBD - We should most likely only use this macro at initialization.
;
; TBD - handle default value as or of all the below from equates
;
;  Value  bit  Name    Function
; ---------------------------------------------------
;  0     0 - M bit - MMU disabled
;  0     1 - A bit - Alignment fault disabled
;  0     2 - C bit - Data Cache disabled
;  0     3 - W bit - Write Buffer enabled
;  ----------
;  111  4-6        - SHOULD BE ONES
;  0     7 - B bit - Little Endian
;  ----------
;  0     8 - S bit - MMU Protection disabled
;  0     9 - R bit - ROM Protection disabled
;  0    10 - F bit - SHOULD BE ZERO
;  0    11 - Z bit - Program Flow Protection disabled
;  ----------
;  0    12 - I bit - Level one Instruction Cache disabled
;  0    13 - V bit - Exception Vector Location is Normal
;  0    14 - RR bit- Replacement strategy is Normal/Random (not Round Robin)
;  0    15 - L4 bit- Loads to PC set the T bit
;  ----------
;  1    16 - DT bit- Global Data TCM enabled - SHOULD BE ONE for ARM11JF-S
;  0    17 -       - SHOULD BE ZERO
;  1    18 - IT bit- Global Instruction TCM enabled
;  0    19 -       - SHOULD BE ZERO
;  ----------
;  0    20 -       - SHOULD BE ZERO
;  0    21 - FI bit- Low Latency interrupt features disabled
;  1    22 - U bit - Unaligned data access enabled
;  1    23 - XP bit- HW page translation mechanism subpage AP bits disabled
;  ----------
;  0    24 - VE bit- Interrupt vectors are fixed (no VIC)
;  0    25 - EE bit- CPSR E bit set to 0 on taking an exception
;  0    26 -       - Reserved
;  0    27 - NMFI  - Non maskable fiq
;  ----------
;  0    28 - TRE   - TEX remap enable
;  0    29 - AFE   - Access flag enable
;  0    30 - TE    - Thumb Exception
;  0    31 -       - Reserved
;============================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, =C1_REG_DEFAULT
   mcr    p15, 0, r0, c1, c0, 0
   MEND

;============================================================================
; MACRO mmu_invalidate_tlb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
;   operation (Data and Instruction). Invalidates all unlocked entries in the
;   TLB. Causes the prefetch buffer to be flushed. All following instructions
;   are fetched after the TLB invalidation.
;   We should do this before we enable to MMU.
;============================================================================
   MACRO
   mmu_invalidate_tlb
   dsb
   TLBIALLIS
   BPIALLIS
   dsb
   isb
   MEND

;============================================================================
; MACRO mmu_enable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable_i_and_d_cache
   mrc     p15, 0, r0, c1, c0, 0   ;  Read System Control register
   orr     r0, r0, # (1 << 2)      ;  C bit - Enable Data Caches
   orr     r0, r0, # (1 << 12)     ;  I bit - Enable Instruction Caches
   mcr     p15, 0, r0, c1, c0, 0   ;  Write System Control register
   MEND

;============================================================================
; MACRO mmu_disable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the Data Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable_i_and_d_cache
   mrc     p15, 0, r0, c1, c0, 0   ;  Read SCTLR, Control register
   bic     r0, r0, #0x4            ;  C bit - Disable Data Cache
   bic     r0, r0, #0x1000         ;  I bit - Disable Instruction Cache
   mcr     p15, 0, r0, c1, c0, 0   ;  Write SCTLR, Control register
   MEND

;============================================================================
; MACRO mmu_set_page_table_base
;
; ARGS
;   base is as follows:
;
;  bit  Name    Function
; ---------------------------------------------------
;  0 - C bit - Page table walk is Inner Cacheable?
;  1 - S bit - Page table walk is Shareable?
;  2 - IMP bit - Implementation defined (Parity enabled for tablewalk)
; 3-4 - RGN bits- Outer cachable attributes for page table walking:
;                 00=Normal memory, Outer Noncacheable
;                 01=Normal memory, Outer Write-Back, write allocate
;                 10=Normal memory, Outer Write-Through, no allocate on write
;                 11=Normal memory, Outer Write-Back, no allocate on write
;  5 - Not Outer Shareable bit. (ARMv7)
;
;  6 - Combines with bit 0 to be IRGN
;
; IRGN [6,0] when multiprocessing extensions are implemented
;      0b00 Normal memory, Inner Non-cacheable
;      0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable
;      0b10 Normal memory, Inner Write-Through Cacheable
;      0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable
;
;14-31 - Pointer to level one translation table
;
;
; DESCRIPTION
;  Sets the base address of the mmu page table.
;============================================================================
   MACRO
   mmu_set_page_table_base  $base
   ldr     r0, =$base                ; location of our page table description
   mcr     p15, 0, r0, c2, c0, 0
   MEND


;============================================================================
; MACRO mmu_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable
   dsb
   isb
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x1              ; Set MMU Enable bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   isb
   MEND

;============================================================================
; MACRO mmu_disable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   bic     r0, r0, #0x1               ; Clear MMU bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND

;============================================================================
; MACRO mmu_align_fault_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables address alignment fault checking
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_align_fault_enable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x2              ; Set A bit - Enable alignment fault check
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND

;============================================================================
; MACRO mmu_set_dom_access
;
; ARGS
;   permissions
;     Contains 16 2 bit Domain fields
;
;      00 = No Access - Any access to this domain causes a domain fault
;      01 = Client - Accesses checked against access permissions in TLB entry
;      10 = Reserved - Any access to this domain causes a domain fault
;      11 = Master - No checking, no permission fault can be generated
;
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;  |D15|D14|D13|D12|D11|D10|D09|D08|D07|D06|D05|D04|D03|D02|D01|D00|
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;   struct
;   {
;      D15_Domain int:2
;      D14_Domain int:2
;      D13_Domain int:2
;      D12_Domain int:2
;      D11_Domain int:2
;      D10_Domain int:2
;      D9_Domain int:2
;      D8_Domain int:2
;      D7_Domain int:2
;      D6_Domain int:2
;      D5_Domain int:2
;      D4_Domain int:2
;      D3_Domain int:2
;      D2_Domain int:2
;      D1_Domain int:2
;      D0_Domain int:2
;   }
;
; DESCRIPTION
;   Sets the Access permissions for the 16 domains.
;
;============================================================================
   MACRO
   mmu_set_dom_access $hi_permissions, $lo_permissions
   mov     r0,   $lo_permissions
   movt    r0,   $hi_permissions
   mcr     p15, 0, r0, c3, c0, 0
   MEND

;============================================================================
; Enable AFE
;============================================================================
   MACRO
   mmu_enable_afe  $rx
      mrc     p15, 0, $rx, c1, c0, 0   ; RCP15_SCTLR  $reg
      orr     $rx, $rx, #0x20000000
      mcr     p15, 0, $rx, c1, c0, 0   ; WCP15_SCTLR  $reg
   MEND


;============================================================================
; mmu_enable_branch_prediction
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables branch prediction
;============================================================================
    MACRO
       mmu_enable_branch_prediction $r
       mrc     p15, 0, $r, c1, c0, 0   ; Read from CP15 Control Register
       orr     $r, $r, #(1 << 11)      ; Enable branch prediction
       mcr     p15, 0, $r, c1, c0, 0   ; Write back to CP15 Control Register
    MEND

;============================================================================
; mmu_disable_branch_prediction
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables branch prediction
;============================================================================
    MACRO
       mmu_disable_branch_prediction $r
       mrc     p15, 0, $r, c1, c0, 0   ; Read from CP15 Control Register
       bic     $r, $r, #(1 << 11)      ; Disable branch prediction
       mcr     p15, 0, $r, c1, c0, 0   ; Write back to CP15 Control Register
    MEND

;============================================================================
; tzbsp_tlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire unified TLB.
;============================================================================
tzbsp_tlb_inval_all FUNCTION
    mmu_invalidate_tlb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_itlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire intruction TLB.
;============================================================================
tzbsp_itlb_inval_all FUNCTION
    dsb
    ldr     r0, =0x0
    mcr     p15, 0, r0, c8, c5, 0
    dsb
    isb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_tlb_inval_all_is
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the unified TLB inner shared
;============================================================================
tzbsp_tlb_inval_all_is FUNCTION
    mmu_invalidate_tlb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_tlb_inval_by_asid_is
;
; ARGS
;   $asid - passed in r0
;
; DESCRIPTION
;   Invalidate the unified TLB by ASID inner shared
;============================================================================
tzbsp_tlb_inval_by_asid_is FUNCTION
    dsb
    mcr     p15, 0, r0, c8, c3, 2
    BPIALLIS
    dsb
    isb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_tlb_inval_by_mva_is
;
; ARGS
;   $mva - passed in r0
;
; DESCRIPTION
;   Invalidate the unified TLB by MVA inner shared
;============================================================================
tzbsp_tlb_inval_by_mva_is FUNCTION
    dsb
    mcr     p15, 0, r0, c8, c3, 1
    BPIALLIS
    dsb
    isb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_dtlb_inval_all
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire data TLB.
;============================================================================
tzbsp_dtlb_inval_all FUNCTION
    dsb
    ldr     r0, =0x0
    mcr     p15, 0, r0, c8, c6, 0
    BPIALLIS
    dsb
    isb
    bx      lr
    ENDFUNC

;============================================================================
; tzbsp_mmu_cache_disable
;
; ARGS
;
;
;  DESCRIPTION -
;        Disables the MMU and cache.
;           1- flushes out cache and write buffer
;           2- invaludates TLB
;           3- disable data and instruction cache
;           4- disables MMU
;        The page table is unaffected
;============================================================================
tzbsp_mmu_cache_disable FUNCTION
    stmfd   sp!, {r0-r12, lr}
    bl      tzbsp_cache_flush
    mov     r0, #0
    mmu_invalidate_tlb
    mmu_disable_i_and_d_cache
    mmu_disable
    ldmfd   sp!, {r0-r12, pc}
    ENDFUNC

;============================================================================
; tzbsp_mmu_cache_enable_no_cfg
;
;  Enables the MMU and caches, without applying any configurations
;
; ARGS
;
;
;  DESCRIPTION -
;        Enables the MMU and cache.
;           1- ICache and branch target invalidate
;           2- TLB invalidate
;           3- Enable D and I caches
;           4- Enable MMU
;============================================================================
tzbsp_mmu_cache_enable_no_cfg FUNCTION
    push    {r0, lr}

    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU instruction & branch
                                    ; target invalidate
    mmu_invalidate_tlb

    ; Enable the caches and MMU
    mmu_enable_i_and_d_cache
    mmu_enable

    pop     {r0, pc}
    ENDFUNC

;============================================================================
; tzbsp_mmu_cache_re_enable
;
;  DESCRIPTION -
;        re-enables the MMU and cache.
;        The page table is unaffected
;============================================================================
tzbsp_mmu_cache_re_enable FUNCTION
    push    {r0, lr}
    ICIALLU                         ; instruction & branch target invalidate
    bl      mmu_enable_common       ; doesn't return
    ENDFUNC

;============================================================================
; TZBSP_MMU_CACHE_INIT
;
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the physical address of the page table
;                3- populates the page_table with section entries
;                4- sets the domain access permissions
;                5- flushes out cache and write buffer
;                6- then enables the mmu
;                7- then enable data and instruction cache
;============================================================================
tzbsp_mmu_cache_init FUNCTION
    push    {r1, lr}
    bl      tzbsp_init_tts
    cmp     r0, #0
    bne     tzbsp_mmu_cache_init_err_return
    bl      tzbsp_cache_flush

mmu_enable_common
    ldr     r0, =l1
    bl      mmu_set_ttbrs
    bl      mmu_enable_tex_remap    ; Enable TEX Remapping

    mmu_invalidate_tlb              ; Invalidate instruction and data TLB
    mmu_set_dom_access #0x5555, #0x5555 ; set all domains for client access
    mmu_enable_i_and_d_cache
    mmu_enable
    mov     r0, #0
    pop     {r1, pc}

tzbsp_mmu_cache_init_err_return
    mov     r0, #-1
    pop     {r1, pc}
    ENDFUNC

;============================================================================
; TZBSP_MMU_DISABLE
;
;  DESCRIPTION -
;        Disables the ARM MMU.
;
;  NOTE -
;        This function does not perform any cache maintenance
;============================================================================
tzbsp_mmu_disable FUNCTION
    mmu_disable
    isb
    bx      lr
    ENDFUNC

;============================================================================
; TZBSP_MMU_ENABLE
;
;  DESCRIPTION -
;        Enables the ARM MMU.
;
;  NOTE -
;        This function does not perform any cache maintenance
;============================================================================
tzbsp_mmu_enable FUNCTION
    mmu_enable
    isb
    bx      lr
    ENDFUNC

;=======================================================================
; TZBSP_MMU_IS_ENABLED
;
;  DESCRIPTION -
;        Returns TRUE if the MMU is enabled
;
;=======================================================================
tzbsp_mmu_is_enabled FUNCTION
    mrc     p15, 0, r0, c1, c0, 0
    and     r0, r0, #1
    bx      lr
    ENDFUNC

;============================================================================
   END
