#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"































 
























 

#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.h"
















 





















 





 




#line 58 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.h"

 


 



 



 



 

 



 



 



 



 



 



 



 



 



 



 




 


 








 
#line 140 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.h"

#line 149 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.h"

#line 60 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"
 




 








#line 82 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"






 




















    





 

 

   CODE32
      AREA    BootRom, CODE, READONLY

      IMPORT  main_c
      IMPORT  abort_stack
      IMPORT  undef_stack
      IMPORT  irq_stack
      IMPORT  svc_stack





      IMPORT  hw_init

      IMPORT  |Image$$APP_RAM$$ZI$$Base|
      IMPORT  |Image$$APP_RAM$$ZI$$Limit|
      IMPORT  |Image$$ZI_RAM$$ZI$$Base|
      IMPORT  |Image$$ZI_RAM$$ZI$$Limit|
      EXPORT  startup

   PRESERVE8

      ENTRY

















 

   
   MACRO   
      blxx     $destreg
      ROUT

      tst     $destreg, #0x01         

      ldrne   lr, =%1
      ldreq   lr, =%2
      bx      $destreg
1
      CODE16
      bx      pc

      ALIGN
      CODE32
2
      MEND



;=======================================================================
; MACRO mmu_set_default_cfg
; ARGS
;   NONE
; DESCRIPTION
;   Update the mmu control register with the read-modify-write sequence
;   bit [0]:   0 = MMU disabled
;   bit [1]:   0 = Strict alignment fault checking disabled
;   bit [3]:   1 = Write buffer enabled (for ARM9)
;   bit [6:4]: Should Be One
;   bit [13]:  1 = High exception vectors selected.
;   bit [16]:  Should Be One
;   bit [18]:  Should Be One
;   bit [22]:  1 = unaligned data access support enabled (for ARM11)
;=======================================================================
   MACRO
   mmu_set_default_cfg
   MRC    p15, 0, r0, c1, c0, 0     ; read control register
   LDR    r1, = 0x00452078
   ORR    r0, r0, r1
   MCR    p15, 0, r0, c1, c0, 0     ; write control register
   MEND
;=======================================================================


;=======================================================================
; MACRO mmu_enable_i_cache
; ARGS
;  NONE
; DESCRIPTION
;  Enables the I Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
  MACRO
    mmu_enable_i_cache
    mrc     p15, 0 , r0, c1, c0, 0
    orr     r0, r0, # (1 << 12 )
    mcr     p15, 0 , r0, c1, c0, 0
  MEND
;=======================================================================

;=======================================================================
; MACRO mmu_disable_d_cache
; ARGS
;  NONE
; DESCRIPTION
;  Disables the D Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
  MACRO
    mmu_disable_d_cache
    mrc     p15, 0 , r0, c1, c0, 0
    and     r0, r0, # ~(1 << 2 )
    mcr     p15, 0 , r0, c1, c0, 0
  MEND
;=======================================================================


;=======================================================================
; MACRO init_app_ram_zi
; ARGS
;  NONE
; DESCRIPTION
;  Initializes the zero-init region (.bss) to zero
;  r0  = start of APP RAM ZI
;  r1  = end of APP RAM ZI
;  r2  = 0 (for initializing memory) 
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;=======================================================================
  MACRO
    init_app_ram_zi
    ldr     r0, =|Image$$APP_RAM$$ZI$$Base|
    ldr     r1, =|Image$$APP_RAM$$ZI$$Limit|
    mov     r2,#0
init_app_ram_zi_label
    cmp     r0,r1          ; unsigned compare, clear c on borrow
    strcc   r2,[r0],#4     ; str 0 in [r0] if r0 < r1
    bcc     init_app_ram_zi_label  ; loop while ro < r1
  MEND
;=======================================================================

;=======================================================================
; MACRO init_zi
; ARGS
;  NONE
; DESCRIPTION
;  Initializes the zero-init region (.bss) to zero
;  r0  = start of ZI
;  r1  = end of ZI
;  r2  = 0 (for initializing memory) 
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;=======================================================================
  MACRO
    init_zi
    ldr     r0, =|Image$$ZI_RAM$$ZI$$Base|
    ldr     r1, =|Image$$ZI_RAM$$ZI$$Limit|
    mov     r2,#0
init_zi_label
    cmp     r0,r1          ; unsigned compare, clear c on borrow
    strcc   r2,[r0],#4     ; str 0 in [r0] if r0 < r1
    bcc     init_zi_label  ; loop while ro < r1
  MEND
;=======================================================================

;=======================================================================
; MACRO reloc_data
; ARGS
;  NONE
; DESCRIPTION
;  Relocates the RW data for eHOSTDL
;  r0  = start of dest mem
;  r1  = end of dest mem (length of data to be relocated)
;  r2  = start of src mem
;  r3  = watchdog register
;  r4  = 1 (for kicking the dog)
;  r5  = tmp data holder for data relocation
;=======================================================================
  MACRO
    reloc_data
    ldr     r0, =|Image$$APP_RAM$$Base|
    ldr     r1, =|Image$$APP_RAM$$ZI$$Base|
    ldr     r2, =|Load$$APP_RAM$$Base|
reloc_data_label
    cmp     r0,r1            ; unsigned compare, clear c on borrow
    ldrcc   r5,[r2],#4       ; Load [r2] to r5, increment [r2] if r0 < r1
    strcc   r5,[r0],#4       ; str r5 at [r0],  increment [r0] if r0 < r1
    bcc     reloc_data_label ; loop while ro < r1
  MEND
;=======================================================================



























 



startup
   
   ldr     r1,  =0xdeadbeef             
   ldr     r2,  =0xdeadbeef
   ldr     r3,  =0xdeadbeef
   ldr     r4,  =0xdeadbeef
   ldr     r5,  =0xdeadbeef
   ldr     r6,  =0xdeadbeef
   ldr     r7,  =0xdeadbeef
   ldr     r8,  =0xdeadbeef
   ldr     r9,  =0xdeadbeef
   ldr     r11, =0xdeadbeef
   ldr     r12, =0xdeadbeef
;   ldr     r13, =0xdeadbeef
   ldr     r14, =0xdeadbeef
                                        
   
   mrs     a1, CPSR                
   orr     a2, a1, #0x80
   msr     CPSR_c, a2

   
   
   msr     CPSR_c, #0x17:OR:0x40:OR:0x80
   ldr     r13, =abort_stack+0x100

   
   
   msr     CPSR_c, #0x1b:OR:0x40:OR:0x80
   ldr     r13,=undef_stack+0x100

   
   
   msr     CPSR_c, #0x12:OR:0x40:OR:0x80
   ldr     r13, =irq_stack+0x800

   
   
   
   msr     CPSR_c, #0x13:OR:0x80:OR:0x40

   
   
   
   
   
#line 403 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"
   ldr     r13, =svc_stack+0x1000


   ldr     a1,=hw_init
   blxx    a1

   
   init_zi
   init_app_ram_zi




  

; ======================================================================
; Set MMU control register and enable the instruction cache
; ======================================================================

   mmu_set_default_cfg
   mmu_enable_i_cache
   mmu_disable_d_cache


#line 435 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"
     
   
   ldr     a1, =main_c
   bx      a1


; Software breakpoint code
; main_c never returns, but if it did, a breakpoint here
; would catch it
swbp    DCD     0xEEDEEEDE


#line 487 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"


; Terminal emulation entry point for put/get a character.
; When called, it will put or get a charactr from the
; JTAG debugger terminal emulation window.  These function
; are incompatible with the JTAG debugger FLASH.* functions
; and will cause the debugger to hang if both are used.


        CODE16
        EXPORT  term_put
      
term_put
        bx      pc
        ALIGN
        CODE32

#line 517 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_startup.s"
; Check the ID code by reading the cp15 register c0 to
; determine if it's ARM9 or ARM11
; [15:4] Part number 0x926 for ARM926, 0xB36 for ARM1136
        mrc p15, 0, r1, c0, c0, 0   ; read ID
        and r1, r1, #0xf000
        cmp r1, #0xb000             ; if 0xb in part number it's ARM11
        bge term_put_arm11
  
term_put_arm9
        mov     r2, #2

put_again_arm9
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        bne     put_again_arm9
        mcr     p14, 0, r0, c1, c0
        bx      lr

term_put_arm11
        ldr     r2, =0x20000000      ; bit 29
     
put_again_arm11
        mrc     p14, 0, r1, c0, c1   ; read debug status and control register
        and     r1, r1, r2           ; check if wDTR is ready to receive data
        cmp     r1, #0
        bne     put_again_arm11
        mcr     p14, 0, r0, c0, c5   ; write the write data transfer register
        bx      lr





;  Jump to address passed in.
;  Used to fake a reset by passing zero
        CODE16
        EXPORT  jmp_abs
        
jmp_abs
        bx      pc

        ALIGN
        CODE32
    
jmp_abs_arm
        mov r7,r0
        bx r7
        

   
   END
   
