#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/tools/jsdcc/jsdcc_in.scl"


















































































 

RAM 0x00200000 0x20000
{
    CODE 0x00200000 
    {
      jsdcc_start.o (StartHere, +FIRST)
      jsdcc_main.o (+RO)
      * (+RO)
    }

    
    
    APP_RAM +0x0
    {
      * (+RW)
    }
    
    
    
    APP_RAM_ZI +0x0
    {
      * (+ZI)
    }
}

