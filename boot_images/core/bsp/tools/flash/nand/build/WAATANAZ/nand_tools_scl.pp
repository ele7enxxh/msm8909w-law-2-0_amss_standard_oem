#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/nand/nand_in.scl"
 














































 












 

RAM 0x08000000
{
    CODE 0x08000000 
    {
      nand_start.o (StartHere, +FIRST)
      nand_entry.o (+RO)
      nand_core.o (+RO)
      * (+RO)
    }

    
    
    
    APP_RAM +0x0
    {
      * (+RW, +ZI)
    }
     
    ZI_RAM  0x00210000
    {
      
      
      * (FLASH_TOOLS_HEAP_ZONE)
    }
      
    
    
    PARAM 0x0801E000
    {
      nand_param.o (+ZI)
    }
}
