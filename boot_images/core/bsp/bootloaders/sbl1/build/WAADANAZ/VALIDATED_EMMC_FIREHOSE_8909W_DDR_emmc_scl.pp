#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1.scl"










 

























#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"















 



































 





 




#line 1 "./custwaadanaza.h"






 

#line 1 "./targwaadanaza.h"






 

#line 136 "./targwaadanaza.h"




#line 11 "./custwaadanaza.h"











#line 64 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"



 





 




#line 103 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"








  


 






 






 





 









 




 







 









 






 


  




 





 





 




 




 




 





 




 




 




 




 




 




                                 


                                 
                                 


                                 
                               


                                 
                               


                                 
 


                                 
 


                             
                                 



 






 





                             


                             
#line 296 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"




  
 






 



                              


                              


 



   


 


 


 




 


#line 38 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1.scl"




CODE_ROM 0x08006000 0x0040000
{ 
  
  SBL1_ROM 0x08006000 ALIGN 0x8
  {
    sbl1.o (SBL1_VECTORS, +FIRST)
    sbl1.o (SBL1_ENTRY)
    ddr_test.o (+RO-CODE, +RO-DATA)
    * (+RO-CODE)
    * (+RO-DATA, LAST)
  }
}

CODE_STACKS (0x08006000 + 0x0040000) (0x0002000 + 0x0002000 + 0x0002000)
{
  SBL1_SVC_STACK +0x0 EMPTY 0x0002000
  {
  }

  SBL1_UND_STACK +0x0 EMPTY 0x0002000
  {
  }

  SBL1_ABT_STACK +0x0 EMPTY 0x0002000
  {
  }

  ScatterAssert((ImageLimit(SBL1_ABT_STACK) - (0x08006000 + 0x0040000))
  	         <= (0x0002000 + 0x0002000 + 0x0002000))
}

CODE_RAM ((0x08006000 + 0x0040000) + (0x0002000 + 0x0002000 + 0x0002000)) (0x007B000 - 0x0040000 - (0x0002000 + 0x0002000 + 0x0002000))
{
  
  
  
  SBL1_DATA_RW +0x0 ALIGN 0x1000
  {
    ddr_test.o (+RW)
    * (+RW)
  }


  
  
  SBL1_DATA_ZI +0x0 ALIGN 0x8
  {
    
    
    fs_hotplug*.o (+ZI)
    ddr_test.o (+ZI)
    * (BOOT_INTERNAL_HEAP)
    * (+ZI)
  }


  
  SBL1_LOG_META_INFO_SECTION +0x0 EMPTY 64
  {
  }
 
 
  
  SBL1_LOG_BUF_SECTION +0x0 EMPTY 2048
  {
  }


  ScatterAssert((ImageLimit(SBL1_LOG_BUF_SECTION) - 0x08006000)
                 < (448 * 1024))
}

  
  
  
  
  
  
DEVICEPROGRAMMER_BUFFERS 0x80000000 0x300000
{  
  
  DEVPRO_BUFF 0x80000000 
  {
    *(.bss.CHANNEL_BUFFER_SECTION)
    *(.bss.SDCC_ADMA_DESC_ZI_ZONE)	
  }
}

BOOT_DDR 0x87C00000 0x00E0000
{  
  
  SBL1_DDR_ZI 0x87C00000 0x0068000
  { 
    
    fs_*.o (+ZI)
    hfat*.o(+ZI)
    Ce*.o (+ZI)
    
    
    * (BOOT_EXTERNAL_HEAP)
    * (BOOT_DDR_ZI_DATA_ZONE)
    * (BOOT_CRASH_DUMP_ZONE)
  }    

  
  
  SBL1_DDR_PAGE_TABLE (0x87C00000 + 0x0068000) 0x0008000
  {
    *(BOOT_PAGE_TABLE_ZONE)
  }
  
  
  
  SBL1_DLOAD_ZI ((0x87C00000 + 0x0068000) + 0x0008000)
  {
    qhsusb_boot_*.o (+ZI)
    *sahara*.o(+ZI)
    *dload*.o(+ZI)
    *(BOOT_UNCACHED_DDR_ZI_ZONE)
  }

  
  
  
  SBL1_DEVPRO_USB_ZI_DUMMY +0x0
  {
  }
  SBL1_DEVICEPROGRAMMER_USB_ZI +0x0 ALIGN 0x800
  {
    qhsusb_*.o (+ZI)
  }
  
  
  SBL1_DDR_LOG_META_INFO_SECTION ((0x87C00000 + 0x00E0000 - 2496) - 64) EMPTY 64
  {
  }
  
  
  SBL1_DDR_LOG_BUF_SECTION (0x87C00000 + 0x00E0000 - 2496) EMPTY 2496
  {
  }
}


BOOT_SYSTEM_DEBUG ((0x00200000 + 0x0020000)) 0x0003000
{
  
  
  SYSTEM_DEBUG_CO_RO ((0x00200000 + 0x0020000)) 0x0003000
  {
    sys_debug.o (SYSTEM_DEBUG_ENTRY, +FIRST)
    sys_debug*.o (+RO-CODE,+RO-DATA)
  }
  
  BOOT_A53 +0x0
  {
    boot_a53.o (+RO-CODE,+RO-DATA)
    boot_a53_asm.o (+RO-CODE,+RO-DATA)
  }

  BOOT_SEGMENT2_DDRFW +0x0
  {
    ddr*.o (+RO-CODE,+RO-DATA)
    *BIMC*.o (+RO-CODE,+RO-DATA)
    *PHY*.o (+RO-CODE,+RO-DATA)
    boot_extern_ddr_interface.o (+RO-CODE,+RO-DATA)
  }  

  ScatterAssert(( ImageLimit(BOOT_SEGMENT2_DDRFW) -
                  ((0x00200000 + 0x0020000)))
                  < 0x0003000)
}

BOOT_SYSTEM_DEBUG_DATA ((0x00200000 + 0x0020000) + 0x0003000) (0x0004000 - 0x0003000)
{
  
  
  SYSTEM_DEBUG_DATA ((0x00200000 + 0x0020000) + 0x0003000) ALIGN 0x8 (0x0004000 - 0x0003000)
  {
    sys_debug*.o (SYSTEM_DEBUG_DATA)
    sys_debug*.o (+RW,+ZI)
    ddr*.o (+RW,+ZI)
    *BIMC*.o (+RW,+ZI)
    *PHY*.o (+RW,+ZI)
    boot_extern_ddr_interface.o (+RW,+ZI)
    *(.bss.SDCC_ADMA_BUFF_ZI_ZONE)	
  }

  BOOT_SEGMENT2_DDRFW_TBL +0x0 ALIGN 0x8
  {
    *(ddr_func_ptr)
  }
  
  
  SYSTEM_DEBUG_QSEE_INFO +0x0 EMPTY ALIGN 0x8 0x20
  { 
  }
  
  ScatterAssert(( ImageLimit(SYSTEM_DEBUG_QSEE_INFO) -
                  ((0x00200000 + 0x0020000) + 0x0003000))
                  < (0x0004000 - 0x0003000))
}
