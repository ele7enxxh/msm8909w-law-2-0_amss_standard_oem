#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/nor/nor_in.scl"



















































































 

RAM 0x00200000
{
    CODE 0x00200000 
    {
      nor_start.o (StartHere, +FIRST)
      nor_core.o (+RO)
      * (+RO)
    }

    
    
    APP_RAM +0x0
    {
      * (+RW, +ZI)
    }
    
    PARAM 0x0021E000
    {
      nor_param.o (+ZI)
    }
}

